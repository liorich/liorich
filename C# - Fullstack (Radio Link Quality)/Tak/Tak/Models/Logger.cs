using Microsoft.Extensions.Hosting;
using System.Text.Json;
using System.Xml.Linq;

namespace Tak.Models
{
    public partial class Worker
    {
        public void LoggerThread()
        {
            return;
            
            // How to write the data in the most efficient and useful way?
            // We should have two options: Drawing in web and exporting data to spreadsheet.

            // Every second, log all of the data.
            // Init: Make folder using timestamp.
            Directory.CreateDirectory("Logger");
            string folderPath = new string("Logger\\"+DateTime.Now.ToShortDateString().Replace('/', '-').Replace(':', '-').Replace(' ', '-'));
            Directory.CreateDirectory(folderPath);
            List<UInt16> createdLoggers = new List<UInt16>();

            // Also start the netstate thread for Rollback:
            //var ns = new Thread(() => LoggerNetstate(folderPath));
            //ns.Start();

            while (true)
            {
                Thread.Sleep(1000);
                /*
                // For each p2p link without logger thread, start one
                foreach(var host in main.groups.Keys)
                {
                    foreach(var plat in main.groups[host].host_assigned_plats.Keys)
                    {
                        if (!createdLoggers.Contains(plat))
                        {
                            createdLoggers.Add(plat);
                            var t = new Thread(() => LoggerThreadS(host, plat, folderPath));
                            t.Start();
                        }
                    }
                }
                // But then check if any were removed
                bool contains = false;
                foreach (var log in createdLoggers)
                {
                    contains = false;
                    foreach(var host in main.groups.Values)
                        if ((host.host_assigned_plats.ContainsKey(log))) contains = true;
                    if (!contains) { createdLoggers.Remove(log); break; }
                }
                if (close_threads) break;
                */
                /*
                Thread.Sleep(1000);
                foreach (var host in main.groups.Values)
                {
                    timeStr = DateTime.Now.ToString("HH:mm:ss");
                    foreach (var plat in host.host_assigned_plats)
                    {
                        if (!host.link_receiving_plats.ContainsKey(plat.Key)) continue;
                        if (!File.Exists($"{folderPath}/{host.host_id}-{plat.Key}.txt"))
                        {
                            // Write first line
                            File.AppendAllText($"{folderPath}/{host.host_id}-{plat.Key}.txt",
                                $"Details for report {host.host_id}-{plat.Key}:\nTime\tLQ GDT\tGDT A1SNR\tGDT A2SNR\tGDT A1RSSI\tGDT A2RSSI\t" +
                                "LQ ADT\tADT A1SNR\tADT A2SNR\tADT A1RSSI\tADT A2RSSI\t" +
                                "Range\tHeight\tRTT\tComment\tUPFREQ\tDOWNFREQ\tGDT Tx ANt\n"

                                );
                        }
                        if(LoggerComments.ContainsKey(plat.Key)) { comment = LoggerComments[plat.Key]; LoggerComments.Remove(plat.Key); }
                        infoStr = timeStr + "\t" + host.link_receiving_plats[plat.Key].getLQReport(true) + plat.Value.getLQReport(false) + comment + "\t" + host.getReport() + "\n";
                        comment = "";
                        LoggerSync.WaitOne();
                        File.AppendAllText($"{folderPath}/{host.host_id}-{plat.Key}.txt",infoStr);
                        LoggerSync.ReleaseMutex();
                    }
                }

                if (close_threads) break;
                */
            }

        }
        public void LoggerThreadS(UInt16 host_id, UInt16 plat_id, string folderPath)
        {
            string infoStr = new("");
            string timeStr = new("");
            string comment = new("");

            var filePath = $"{folderPath}\\{host_id}-{plat_id}.txt";
            var filePathDomo = $"{folderPath}\\{plat_id}-DOMO.txt";
            /*
            // Waits for creation of data
            while (!main.groups.ContainsKey(host_id));
            var host_tmp = main.groups[host_id];
            while (!host_tmp.host_assigned_plats.ContainsKey(plat_id));
            while (!host_tmp.link_receiving_plats.ContainsKey(plat_id));
            var plat_tmp = host_tmp.host_assigned_plats[plat_id]; 
            // Domo Extension
            if (plat_tmp.plat_mesh is not null)
            {
                if (!File.Exists(filePathDomo))
                {
                    // Write first line
                    File.AppendAllText(filePathDomo,
                        $"Details for {plat_id} MESH reports:\nTime\tNumNodes\tNodeIDs\tSNR Matrix\tLevA Matrix\tLevB Matrix\tSignal Quality\n"
                        );
                }
            }

            if (!File.Exists(filePath))
            {
                // Write first line
                File.AppendAllText(filePath,
                    $"Details for report {host_id}-{plat_id}:\nTime\tLQ GDT\tGDT A1SNR\tGDT A2SNR\tGDT A1RSSI\tGDT A2RSSI\t" +
                    "LQ ADT\tADT A1SNR\tADT A2SNR\tADT A1RSSI\tADT A2RSSI\t" +
                    "Range\tHeight\tRTT\tComment\tUPFREQ\tDOWNFREQ\tGDT Tx ANt\t1PPS Type\t1PPS Valid\tHas Mesh Link\n"
                    );
            }
            using (StreamWriter sw = new StreamWriter(filePath, true))
            {
                sw.AutoFlush = true;
                while (true)
                {
                    if (!main.groups.ContainsKey(host_id)) break;
                    var host = main.groups[host_id];
                    if (!host.host_assigned_plats.ContainsKey(plat_id)) break;
                    var plat = host.host_assigned_plats[plat_id];
                    // ^ If the host or the platform went offline, this thread terminates
                    if (LoggerComments.ContainsKey(plat_id)) { comment = LoggerComments[plat_id]; LoggerComments.Remove(plat_id); }
                    timeStr = DateTime.Now.ToString("HH:mm:ss");
                    infoStr = timeStr + "\t" + host.link_receiving_plats[plat_id].getLQReport(true) + plat.getLQReport(false) + comment + "\t" + host.getReport();
                    comment = "";
                    sw.WriteLine(infoStr);
                    if (plat.plat_mesh is not null)
                    {
                        // log plat_mesh
                        infoStr = timeStr + "\t" + plat.plat_mesh.getLQMeshReport() + "\n";
                        File.AppendAllText(filePathDomo, infoStr);
                    }
                    Thread.Sleep(1000);
                    if (close_threads) break;
                }
            }
        }
        public void LoggerNetstate(string folderPath)
        {
            var filePath = $"{folderPath}\\NetState.txt";
            if (!File.Exists(filePath))
            {
                // Write first line
                File.AppendAllText(filePath,
                    $"Network State for current date:\nTime\tState\n"
                    );
            }
            using (StreamWriter sw = new StreamWriter(filePath, true))
            {
                sw.AutoFlush = true;
                while (true) {
                    var state = JsonSerializer.Serialize(main.getNetState());
                    var time = DateTime.Now.ToString("HH:mm:ss");
                    sw.WriteLine($"{time}\t{state}");
                    Thread.Sleep(1000);
                    if (close_threads) break;
                }
            }
        */
        }
    }
}
