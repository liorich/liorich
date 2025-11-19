using System.Text.Json;
using System.Text;
using System.Net;
using System.Text.Json.Nodes;
using Microsoft.AspNetCore.Authorization.Infrastructure;

namespace Tak.Models
{
    [Serializable]
    public class DomoMesh
    {
        uint id;
        private string ip_addr;
        private string username;
        private string password;
        SortedList<uint, DomoNode> Nodes;
        public short lastResponse { get; set; }
        public Dictionary<string, object>? shortReport { get; set; }

        public DomoMesh(string ip_addr, string username, string password)
        {
            this.id = 0;
            this.Nodes = new();
            this.lastResponse = 0;
            this.ip_addr = ip_addr;
            this.username = username;
            this.password = password;
        }
        private void Feed(uint id, string name, float[] SNR, float[] SigLevA, float[] SigLevB, float sigLevA0, float sigLevB0, float[] SigQuality)
        {
            this.lastResponse = 0;
            if (!Nodes.ContainsKey(id)) Nodes.Add(id, new DomoNode(id, name));
            Nodes[id].Feed(id, name, SNR, SigLevA, SigLevB, sigLevA0, sigLevB0, SigQuality);
        }
        public async Task selfUpdate()
        {
            var obj = MeshGetResponse(this.ip_addr, this.username, this.password);
            if (obj is not null)
            {
                var obj_status = (JsonArray?)obj["Status"]["Mesh1"]["RemoteStatus"];
                foreach (JsonObject? data in obj_status)
                {
                    if (data.Count != 12) continue;
                    else
                    {
                        var nodeID = JsonSerializer.Deserialize<uint>(data["UnitStatus"]["nodeId"]);
                        var nodeName = JsonSerializer.Deserialize<string>(data["UnitStatus"]["unitName"]);
                        var snr = JsonSerializer.Deserialize<float[]>(data["DemodStatus"]["snr"]);
                        var sigLevA = JsonSerializer.Deserialize<float[]>(data["DemodStatus"]["sigLevA"]);
                        var sigLevB = JsonSerializer.Deserialize<float[]>(data["DemodStatus"]["sigLevB"]);
                        var sigLevA0 = JsonSerializer.Deserialize<float>(data["DemodStatus"]["sigLevA0"]);
                        var sigLevB0 = JsonSerializer.Deserialize<float>(data["DemodStatus"]["sigLevB0"]);
                        var sigQuality = JsonSerializer.Deserialize<float[]>(obj["Status"]["Mesh1"]["signalQualityTable"][(int)nodeID]);
                        this.Feed(nodeID, nodeName, snr, sigLevA, sigLevB, sigLevA0, sigLevB0, sigQuality);
                    }
                }
                shortReport = this.ExportMeshData();
            }
            else
            {
                this.lastResponse++;
            }
        }
        public JsonNode? MeshGetResponse(string ip, string username, string pw)
        {
            // Tries to get mesh data using login details, otherwise returns null
            string formUrl = $"http://{ip}/login.cgi";
            string formParams = string.Format($"username={username}&password={pw}");
            string cookieHeader;
            WebRequest req = WebRequest.Create(formUrl);
            req.ContentType = "application/x-www-form-urlencoded";
            req.Method = "POST";
            req.Timeout = 4000;
            byte[] bytes = Encoding.ASCII.GetBytes(formParams);
            using (Stream os = req.GetRequestStream())
            {
                os.Write(bytes, 0, bytes.Length);
            }

            WebResponse? resp = null;
            try
            {
                resp = req.GetResponse();
                cookieHeader = resp.Headers["Set-cookie"];

                string pageSource;
                string getUrl = $"http://{ip}/status.json";
                WebRequest getRequest = WebRequest.Create(getUrl);
                getRequest.Headers.Add("Cookie", cookieHeader);
                WebResponse getResponse = getRequest.GetResponse();
                using (StreamReader sr = new StreamReader(getResponse.GetResponseStream()))
                {
                    pageSource = sr.ReadToEnd();
                }
                var obj = JsonObject.Parse(pageSource);
                return obj;
            }
            catch (Exception ex) { return null; }
        }
        public Dictionary<string, object> ExportMeshData()
        {
            // Creates a simple matrix-like report of the current Mesh
            int idx;
            var keys = Nodes.Keys;
            List<float?[]> SNR_rows = new();
            foreach (var key in keys)
            {
                var row = new float?[keys.Count];
                foreach (var id in keys)
                {
                    idx = Nodes.IndexOfKey(id);
                    if (id == key) row[idx] = null;
                    else row[idx] = Nodes[key].SNRof(id);
                }
                SNR_rows.Add(row);
            }

            List<float?[]> LvA_rows = new();
            foreach (var key in keys)
            {
                var row = new float?[keys.Count];
                foreach (var id in keys)
                {
                    idx = Nodes.IndexOfKey(id);
                    if (id == key) row[idx] = Nodes[key].sigLevA0;
                    else row[idx] = Nodes[key].SigLevAOf(id);
                }
                LvA_rows.Add(row);
            }

            List<float?[]> LvB_rows = new();
            foreach (var key in keys)
            {
                var row = new float?[keys.Count];
                foreach (var id in keys)
                {
                    idx = Nodes.IndexOfKey(id);
                    if (id == key) row[idx] = Nodes[key].sigLevB0;
                    else row[idx] = Nodes[key].SigLevAOf(id);
                }
                LvB_rows.Add(row);
            }
            List<float?[]> Qual_rows = new();
            foreach (var key in keys)
            {
                var row = new float?[keys.Count];
                foreach (var id in keys)
                {
                    idx = Nodes.IndexOfKey(id);
                    if (id == key) row[idx] = null;
                    else row[idx] = Nodes[key].QualOf(id);
                }
                Qual_rows.Add(row);
            }

            var names = new Dictionary<uint, string>();
            foreach (var node in Nodes) names.Add(node.Key, node.Value.name);

            var report = new Dictionary<string, object>
            {
                { "MeshID", this.id },
                { "Nodes", names },
                { "NumNodes", names.Count },
                { "SNR", SNR_rows },
                { "LvA", LvA_rows },
                { "LvB", LvB_rows },
                { "Qual", Qual_rows }
            };
            return report;
        }
        public string getLQMeshReport()
        {
            var report = this.ExportMeshData();
            var keys = JsonSerializer.Serialize(Nodes.Keys);
            var snr = JsonSerializer.Serialize(report["SNR"]);
            var lvA = JsonSerializer.Serialize(report["LvA"]);
            var lvB = JsonSerializer.Serialize(report["LvB"]);
            var sig = JsonSerializer.Serialize(report["Qual"]);
            var str = $"{Nodes.Count}\t{keys}\t{snr}\t{lvA}\t{lvB}\t{sig}";
            return str;
        }
    }

    public class DomoNode
    {
        uint id;
        public string name;
        public float sigLevA0;
        public float sigLevB0;
        float[] SNR;
        float[] SigLevA;
        float[] SigLevB;
        float[] SigQuality;
        public DomoNode(uint id, string name)
        {
            this.id = id;
            this.name = name;
            this.SNR = new float[16];
            this.SigLevA = new float[16];
            this.SigLevB = new float[16];
        }
        public void Feed(uint id, string name, float[] SNR, float[] SigLevA, float[] SigLevB, float sigLevA0, float sigLevB0, float[] SigQuality)
        {
            this.id = id;
            this.name = name;
            Array.Copy(SNR, this.SNR, 16);
            Array.Copy(SigLevA, this.SigLevA, 16);
            Array.Copy(SigLevB, this.SigLevB, 16);
            this.sigLevA0 = sigLevA0;
            this.sigLevB0 = sigLevB0;
            this.SigQuality = SigQuality;
            return;
        }
        public float SNRof(uint idx) { return SNR[idx]; }
        public float SigLevAOf(uint idx) { return SigLevA[idx]; }
        public float SigLevlBOf(uint idx) { return SigLevB[idx]; }
        public float QualOf(uint idx) { return SigQuality[idx]; }

    }
}

