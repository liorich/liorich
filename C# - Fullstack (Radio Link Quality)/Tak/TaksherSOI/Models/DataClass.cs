using System.Net.NetworkInformation;
using System.Runtime.CompilerServices;
using System.Text.Json;

namespace Tak.Models
{
    public static class Constants
    {
        public const short STATIONTYPE_BASE = 1;
        public const short STATIONTYPE_REMOTE = 2;
        public const short CONNECTIONTYPE_SHF = 1;
        public const short CONNECTIONTYPE_DTC = 2;
        public const short GRAPHTYPE_P2P = 1;
        public const short GRAPHTYPE_MESH = 2;
        public const short LINKTYPE_SHF = 1;
        public const short LINKTYPE_DTC = 2;
        public const short LINKTYPE_MIXED = 3;
    }

    [Serializable]
    public class LogReportDot
    {
        // Used to craft a JSON Report
        public string dotTime { get; set; }
        public uint LQ_GDT { get; set; }
        public uint LQ_ADT { get; set; }
        public double GDT_A1SNR { get; set; }
        public double GDT_A2SNR { get; set; }
        public double GDT_A1RSSI { get; set; }
        public double GDT_A2RSSI { get; set; }
        public double ADT_A1SNR { get; set; }
        public double ADT_A2SNR { get; set; }
        public double ADT_A1RSSI { get; set; }
        public double ADT_A2RSSI { get; set; }
        public long ping_rtt { get; set; }
        public string comment { get; set; }

        public LogReportDot(string[] stringArr)
        {
            dotTime = stringArr[0];
            LQ_GDT = Convert.ToByte(stringArr[1]);
            GDT_A1SNR = Convert.ToDouble(stringArr[2]);
            GDT_A2SNR = Convert.ToDouble(stringArr[3]);
            GDT_A1RSSI = Convert.ToDouble(stringArr[4]);
            GDT_A2RSSI = Convert.ToDouble(stringArr[5]);
            LQ_ADT = Convert.ToByte(stringArr[6]);
            ADT_A1SNR = Convert.ToDouble(stringArr[7]);
            ADT_A2SNR = Convert.ToDouble(stringArr[8]);
            ADT_A1RSSI = Convert.ToDouble(stringArr[9]);
            ADT_A2RSSI = Convert.ToDouble(stringArr[10]);
            ping_rtt = (long)Convert.ToDouble(stringArr[13]);
            comment = stringArr[14];
            //Time	LQ GDT	GDT A1SNR	GDT A2SNR	GDT A1RSSI	GDT A2RSSI	LQ ADT	ADT A1SNR	ADT A2SNR	ADT A1RSSI	ADT A2RSSI	Range	Height	RTT	Comment
            //0     1       2           3           4           5           6       7           8           9           10          11      12      13  14
        }
    }
    [Serializable]
    public class AnyReport
    {
        // This is a data driven task, so lets obtain some good data.
        // The reports arrive from a source - usually the Base station, reporting on all the platforms it sees.
        // So, a report may include more reports within...

        // Basic Parameters
        public string? Name { get; set; } // a global identifier
        public UInt16? ShfID { get; set; } // Shahaf platform ID
        public string? MeshIP { get; set; } // Domo platform IP
        public uint LinkQuality { get; set; }
        public double A1snr { get; set; } // or ant A
        public double A1rssi { get; set; }
        public double A2snr { get; set; } // or ant B
        public double A2rssi { get; set; }

        // Telemetry
        public long Ping_rtt { get; set; }
        public bool Link_busy { get; set; } // This is currently decided by ping responsiveness. If RTT > 400, Link is very likely busy
        public bool Timed_out; // Indicated if the platform has timed out since the last ping. If RTT = -1, timed_out.
        public bool Link_uncontinuous { get; set; }
        int[] PingArray = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

        // Additional Parameters - currently unused. maybe math solution
        public short Height { get; set; }
        public short Range  { get; set; }
        public List<AnyReport>? Reports { get; set; }

        public void setPing(short value)
        {
            Array.Copy(PingArray, 1, PingArray, 0, 9); PingArray[9] = value;
            if (PingArray.Average() <= 0.8) Link_uncontinuous = true;
            else Link_uncontinuous = false;
        }
    }

    // Tak 1.08
    [Serializable]
    public class AnyNode
    {
        public string Name { get; set; } // a global identifier
        public UInt16? ShfID { get; set; } // Shahaf platform ID to listen for data
        public string? MeshIP { get; set; } // Domo platform IP to grab data from
        public string? MeshUsername { get; set; }
        public string? MeshPW { get; set; }
        public string? LinkCheckIP { get; set; } // IP to ping for checking link status
        public bool HasExtendedMesh { get; set; } // If unit has platform extension
        public DomoMesh? ExtendedMesh { get; set; }
        public short StationType { get; set; } // Where 1 - Base, 2 - Remote
        public BaseReport? Basereport { get; set; }
        public short ConnectionType { get; set; } // Where 1 - GDT/ADT, 2 - Domo
        public short GraphType { get; set; } // Where 1 - P2P/P2MP, 2 - Mesh
        public List<string>? RemotesNames { get; set; } // Platforms below this node in the tree
        public Dictionary<string, AnyNode>? Remotes { get; set; } // Platforms below this node in the tree
        public Dictionary<string, AnyReport>? Link_receiving { get; set; } // How this platform sees other platforms (Rx)

        // AnyNode() - Load from configs
        public AnyNode()
        {
            this.Name = "temp";
        }
        public AnyNode(string name)
        {
            this.Name = name;
            try
            {
                // We assume that SetupConfig has the defined node, if not, throws exception.
                string jsonString = File.ReadAllText("SetupConfig.json");
                var nodes = JsonSerializer.Deserialize<Dictionary<string, AnyNode>>(jsonString);
                if (nodes is not null)
                {
                    if (nodes.ContainsKey(name))
                    {
                        var node_data = nodes[name];
                        this.Name = node_data.Name;
                        this.ShfID = node_data.ShfID;
                        this.MeshIP = node_data.MeshIP;
                        this.MeshUsername = node_data.MeshUsername;
                        this.MeshPW = node_data.MeshPW;
                        this.LinkCheckIP = node_data.LinkCheckIP;
                        this.StationType = node_data.StationType;
                        if (StationType == 1) Basereport = new();
                        if (node_data.LinkCheckIP is not "") this.LinkCheckIP = node_data.LinkCheckIP; // else null
                        this.ConnectionType = node_data.ConnectionType;
                        this.GraphType = node_data.GraphType;
                        this.RemotesNames = node_data.RemotesNames;
                        if(this.RemotesNames.Count > 0) {
                            this.Remotes = new();
                        foreach(var remote_name in this.RemotesNames)
                            {
                                this.Remotes.Add(remote_name, new AnyNode(remote_name));
                            }
                        }
                        this.Link_receiving = new();
                    }
                    else throw new Exception();
                }
                // Successful creation here
            }
            catch (Exception) { throw; } // Thrown exception means, SetupConfig.txt was wrongly defined or node not found in the list
        }
        // Update function: Must account for some scenarios.
        public void Update(List<AnyReport> Data)
        // Assume we get relevant AnyReports here.
        {
            foreach (var report in Data)
            {
                if (!Link_receiving.ContainsKey(report.Name)) { Link_receiving.Add(report.Name, new AnyReport()); }
                Link_receiving[report.Name] = report; // Flat assignment?
            }
            if (StationType == 2 && LinkCheckIP is not null) // Then run a ping check
            {
                _ = Task.Run(() => PingCheck(this.LinkCheckIP));
            }
        }
        public void Update()
        {
            // Update according to own definitions
            if(ConnectionType == Constants.CONNECTIONTYPE_SHF)
                // Currently using SHF network, decided by ms if mixed
            {

            }
        }

        public void PingCheck(string ip)
        {
            Ping p = new Ping();
            PingReply pr;
            long ms;
            pr = p.Send(ip, 2000);
            if (pr.Status == IPStatus.TimedOut) { ms = -1; }
            else ms = pr.RoundtripTime;
            this.Link_receiving[this.Name].Ping_rtt = ms;
            this.Link_receiving[this.Name].setPing((short)((ms == -1) ? 0 : 1));
        }
    }

    [Serializable]
    public class BaseReport
    {
        public uint networkID { get; set; } // can also be mesh ID
        public short stationLinksType { get; set; } // Where 1 - GDT/ADT, 2 - Domo, 3 - Mixed
        public bool? master { get; set; } // Configured by 1PPS Source
        public bool? onePPSValid { get; set; }
        public UInt16 Freq1 { get; set; } // UP
        public UInt16 Freq2 { get; set; } // DOWN
        public UInt16? txAnt { get; set; }
    }

}

