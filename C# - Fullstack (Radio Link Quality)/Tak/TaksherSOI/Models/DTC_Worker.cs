using System.Net;
using System.Net.Sockets;
using System.Text.Json.Nodes;
using System.Text;

namespace Tak.Models
{
    public class DTC_Login
    {
        // A triplet combination where we'll pull data from once per tree/mesh.
        public string Name;
        public string Username;
        public string Password;
        public string Ip;
        public DTC_Login(string name, string username, string password, string ip)
        {
            Name = name;
            Username = username;
            Password = password;
            Ip = ip;
        }
    }
    public class DTC_Worker
    {
        private List<DTC_Login> logins;
        bool close_thread = false;
        Thread? t;
        public DTC_Worker(List<DTC_Login> Logins)
        {
            logins = Logins;
            t = new Thread(collect_thread); t.Start();
        }
        public DTC_Worker()
        {
            logins = new List<DTC_Login>();
        }
        public void setLogins(List<DTC_Login> Logins)
        {
            logins = Logins;
            t = new Thread(collect_thread); t.Start();
        }
        public void close() { close_thread = true; }
        public void collect_thread()
        {
            foreach (var login in logins)
            {
                var json = MeshGetResponse(login.Ip, login.Username, login.Password);
                System.IO.File.WriteAllText($"DTC_{login.Name}.json", json); // Need name validation...
            }
            if (close_thread) return;
        }
        public string MeshGetResponse(string ip, string username, string pw)
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
                return pageSource;
            }
            catch (Exception ex) { return null; }
        }
    }
}
