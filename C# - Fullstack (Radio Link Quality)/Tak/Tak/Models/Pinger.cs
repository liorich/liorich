using System.Net.NetworkInformation;
using System.Text.Json;

namespace Tak.Models
{
    public partial class Worker
    {
        public void PingerThread()
        {
            return;

            // This is a pinger, it's job is to ping all known platform until the end of time as we know it.
            // So, let's keep a track of Plats and their known IPs.
            Dictionary<UInt16, string> knownConnections = new Dictionary<UInt16, string>();

            // Load saved configurations.
            // Who updates this? Either via the file itself, or via admin UI. No need to write to it otherwise.
            if (File.Exists("PingerConfig.txt"))
            {
                string jsonString = File.ReadAllText("PingerConfig.txt");
                var savedConnections = JsonSerializer.Deserialize<Dictionary<UInt16, string>>(jsonString);
                if (savedConnections is not null)
                {
                    foreach (var link in savedConnections) knownConnections.Add(link.Key, link.Value);
                }
            }

            Ping p = new Ping();
            PingReply pr;
            long ms;
            string address = new(""); UInt16 id;
            /*
            while (true)
            {
                Thread.Sleep(100);
                foreach(var link in knownConnections)
                {
                    address = link.Value;
                    id = link.Key;
                    pr = p.Send(address, 2000);
                    if (pr.Status == IPStatus.TimedOut) { ms = -1; }
                    else ms = pr.RoundtripTime;

                    foreach(var host in main.groups.Values)
                    {
                        host.SetMs(ms, id); // If it doesn't exist, just skip
                    }
                }
                if (update_pinger)
                {
                    if (File.Exists("PingerConfig.txt"))
                    {
                        string jsonString = File.ReadAllText("PingerConfig.txt");
                        var savedConnections = JsonSerializer.Deserialize<Dictionary<UInt16, string>>(jsonString);
                        knownConnections.Clear();
                        if (savedConnections is not null)
                        {
                            foreach (var link in savedConnections) knownConnections.Add(link.Key, link.Value);
                        }
                    }
                    update_pinger = false;
                }
                if (close_threads) return;
            
            }
            */
        }
        public void TimeoutChecker()
            // Checks for timeout due to LACK OF REPORTS, not ping-wise
            
        {
            return;/*
            while (true)
            {
                Thread.Sleep(1000);
                foreach(var host in main.groups.Values)
                {
                    host.lastResponse++;
                    if(host.lastResponse > 15)
                    {
                        main.groups.Remove(host.host_id);
                        Timeouts.Add(host.host_id, "host");
                        break;
                    }
                    foreach(var plat in host.host_assigned_plats.Values)
                    {
                        plat.lastResponse++;
                        if(plat.lastResponse > 8)
                        {
                            Timeouts.Add(plat.plat_id, "plat");
                            LoggerComments.Add(plat.plat_id, "Platform TO/OFF");
                            Thread.Sleep(2000);
                            host.Remove(plat.plat_id);
                            break;
                        }
                    }
                }
                if(close_threads) return;
            }
            */
        }
    }
}
