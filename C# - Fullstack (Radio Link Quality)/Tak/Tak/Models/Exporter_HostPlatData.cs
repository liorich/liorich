using System.Text.Json;
using System.Text;
using System;
using System.Text.Json.Serialization;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace Tak.Models
{
    public partial class Worker
    {/*
        public byte[]? Exporter_HostPlatData()
        {
            var options = new JsonSerializerOptions { WriteIndented = true };
            var dict = new Dictionary<string, object>
            {
                { "type", "Data" },
                { "Data", main }
            };
            var buffer = Encoding.ASCII.GetBytes(JsonSerializer.Serialize(dict, options));
            return buffer;
        }*/
        public byte[]? Initialise(string data)
        {
            File.WriteAllText("SetupConfig.json",data);
            var item = JsonSerializer.Deserialize<Dictionary<string, AnyNode>>(data);
            foreach(var node in item)
            {
                if (node.Value.StationType == Constants.STATIONTYPE_BASE) this.root_names.Add(node.Key);
            }
            this.Start();
            var dict = new Dictionary<string, object>
            {
                { "type", "Message" },
                { "Message", "Success" }
            };
            var options = new JsonSerializerOptions { WriteIndented = true };
            var buffer = Encoding.ASCII.GetBytes(JsonSerializer.Serialize(dict, options));
            return buffer;
        }
        public byte[]? Compose_Msg(string msg)
        {
            var dict = new Dictionary<string, object>
            {
                { "type", "Message" },
                { "Message", msg }
            };
            var options = new JsonSerializerOptions { WriteIndented = true };
            var buffer = Encoding.ASCII.GetBytes(JsonSerializer.Serialize(dict, options));
            return buffer;
        }
        public byte[]? Report_Timeouts(Dictionary<UInt16, string> data)
        {
            var dict = new Dictionary<string, object>
            {
                { "type", "Timeouts" },
                { "Timeouts", data }
            };
            var options = new JsonSerializerOptions { WriteIndented = true };
            var buffer = Encoding.ASCII.GetBytes(JsonSerializer.Serialize(dict, options));
            return buffer;
        }
        public byte[]? Exporter_RequestDates()
        {
            var directories = Directory.GetDirectories("Logger");
            for(var i = 0; i<directories.Length; i++)
            {
                directories[i] = directories[i].Substring(7, directories[i].Length - 7);
            }
            var dict = new Dictionary<string, object>
            {
                { "type", "logDates" },
                { "logDates", directories }
            };
            var options = new JsonSerializerOptions { WriteIndented = true };
            var buffer = Encoding.ASCII.GetBytes(JsonSerializer.Serialize(dict, options));
            return buffer;
        }
        public byte[]? Exporter_RequestCoupling(string date)
        {
            var directories = Directory.GetFiles("Logger\\" + date);
            var len = date.Length + 7 + 1;
            for (var i = 0; i < directories.Length; i++)
            {
                directories[i] = directories[i].Substring(len, directories[i].Length - len);
            }
            var dict = new Dictionary<string, object>
            {
                { "type", "logCoupling" },
                { "logCoupling", directories }
            };
            var options = new JsonSerializerOptions { WriteIndented = true };
            var buffer = Encoding.ASCII.GetBytes(JsonSerializer.Serialize(dict, options));
            return buffer;
        }
        public byte[]? Exporter_RequestAvailableTimeFrame(string date, string file)
        {
            List<(string, string)> TimeFrames = new List<(string, string)>();
            var path = $"Logger\\{date}\\{file}";
            const Int32 BufferSize = 256;
            using (var fileStream = File.OpenRead(path))
            {
                using ( var streamReader = new StreamReader(fileStream))
                {
                    string line;
                    string prev = "";
                    string start = "";
                    string end;
                    string curr;
                    string[] lineSplit;
                    streamReader.ReadLine(); streamReader.ReadLine(); // Skip details twice
                    while ((line = streamReader.ReadLine()) != null)
                    {
                        lineSplit = line.Split('\t');
                        curr = lineSplit[0];
                        if (start == "") { start = curr; prev = curr; continue; }
                        if (curr == "23:59:59" || prev == "23:59:59") break;
                        // if delta bigger than 5 mins
                        var delta = DateTime.Parse(curr) - DateTime.Parse(prev);
                        if(delta.TotalMinutes > 3)
                        {
                            // Create time point and continue
                            end = prev;
                            TimeFrames.Add((start, end));
                            start = curr;
                            prev = curr;
                            continue;
                        }
                        prev = curr;

                    }
                    end = prev;
                    if(!TimeFrames.Contains((start,end))) TimeFrames.Add((start,end));
                }
            }
            string[] timeSlots = new string[TimeFrames.Count];
            foreach(var timeSlot in TimeFrames)
            {
                var idx = TimeFrames.IndexOf(timeSlot);
                timeSlots[idx] = $"{timeSlot.Item1} - {timeSlot.Item2}";
            }
            var dict = new Dictionary<string, object>
            {
                { "type", "logAvailableTimeFrame" },
                { "logAvailableTimeFrame", timeSlots }
            };
            var options = new JsonSerializerOptions { WriteIndented = true };
            var buffer = Encoding.ASCII.GetBytes(JsonSerializer.Serialize(dict, options));
            return buffer;
        }

        public byte[]? Exporter_GenerateLogs(string day, string f, string start, string end)
        {
            List<LogReportDot> logs = new List<LogReportDot>();
            var path = $"Logger\\{day}\\{f}";
            const Int32 BufferSize = 256;
            using (var fileStream = File.OpenRead(path))
            {
                using (var streamReader = new StreamReader(fileStream))
                {
                    string line;
                    string curr = "";
                    string[] lineSplit;
                    bool seekingStart = true;
                    bool seekingEnd = true;
                    TimeSpan min = DateTime.Parse("23:59:59") - DateTime.Parse("00:00:00");
                    TimeSpan min_end = min;
                    streamReader.ReadLine(); streamReader.ReadLine(); // Skip details twice
                    while ((line = streamReader.ReadLine()) != null)
                    {
                        lineSplit = line.Split('\t');
                        curr = lineSplit[0];
                        if (seekingStart)
                        {
                            curr = lineSplit[0];
                            var delta = DateTime.Parse(curr) - DateTime.Parse(start);
                            if (Math.Abs(delta.TotalSeconds) <= Math.Abs(min.TotalSeconds)) min = delta;
                            else
                            {
                                seekingStart = false;
                            }
                        }
                        if (!seekingStart) logs.Add(new LogReportDot(lineSplit));
                        if (seekingEnd)
                        {
                            var delta = DateTime.Parse(end) - DateTime.Parse(curr);
                            if (delta.TotalSeconds <= min_end.TotalSeconds && delta.TotalSeconds >= 0) min_end = delta;
                            else
                            {
                                logs.RemoveAt(logs.Count - 1);
                                break;
                            }
                        }
                    }
                }
            }
            var dict = new Dictionary<string, object>
            {
                { "type", "logReport" },
                { "logReport", logs }
            };
            var options = new JsonSerializerOptions { WriteIndented = true };
            var buffer = Encoding.ASCII.GetBytes(JsonSerializer.Serialize(dict, options));
            return buffer;
        }
        public byte[]? Exporter_PingerConfig()
        {
            string config = "";
            try { config = System.IO.File.ReadAllText("PingerConfig.txt"); }
            catch(Exception ex){ config = "{\n\t\"0\": \"0.0.0.0\"\n" + "}"; }
            var dict = new Dictionary<string, object>
            {
                { "type", "PingerConfig" },
                { "PingerConfig", config }
            };
            var options = new JsonSerializerOptions { WriteIndented = true };
            var buffer = Encoding.ASCII.GetBytes(JsonSerializer.Serialize(dict, options));
            return buffer;
        }
        public byte[]? Exporter_NameConfig()
        {
            string config = "";
            try { config = System.IO.File.ReadAllText("NameConfig.txt"); }
            catch (Exception ex) { config = "{\n\t\"0\": \"0.0.0.0\"\n" + "}"; }
            var dict = new Dictionary<string, object>
            {
                { "type", "NameConfig" },
                { "NameConfig", config }
            };
            var options = new JsonSerializerOptions { WriteIndented = true };
            var buffer = Encoding.ASCII.GetBytes(JsonSerializer.Serialize(dict, options));
            return buffer;
        }
        /*
        public void UpdateNameConfig(string config)
        {
            System.IO.File.WriteAllText("NameConfig.txt", config);
            main.groups.Clear();
        }*/
    }
}
