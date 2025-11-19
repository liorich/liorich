using System.Text.Json;
using System.Text;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace Tak.Models
{
    public partial class Worker
    {
        public byte[]? getRollbackDates()
        {
            var directories = Directory.GetDirectories("Logger");
            List<string> files = new List<string>();
            for (var i = 0; i < directories.Length; i++)
            {
                if(File.Exists(directories[0] + "\\NetState.txt"))
                {
                    files.Add(directories[i].Substring(7, directories[i].Length - 7));
                }
            }
            var dict = new Dictionary<string, object>
            {
                { "type", "rollbackDates" },
                { "rollbackDates", files }
            };
            var options = new JsonSerializerOptions { WriteIndented = true };
            var buffer = Encoding.ASCII.GetBytes(JsonSerializer.Serialize(dict, options));
            return buffer;
        }
        public byte[]? getRollbackNetState(string date)
        {
            List<(string, object)> netState = new();
            var path = $"Logger\\{date}\\NetState.txt";
            const Int32 BufferSize = 256;
            string? line;
            var item = File.ReadAllText(path);
            using (var fileStream = File.OpenRead(path))
            {
                using (var streamReader = new StreamReader(fileStream))
                {
                    streamReader.ReadLine(); streamReader.ReadLine(); // Skip details twice
                    while ((line = streamReader.ReadLine()) != null)
                    {
                        var split = line.Split("\t");
                        netState.Add((split[0], split[1]));
                    }
                }
            }
            var dict = new Dictionary<string, object>
            {
                { "type", "rollbackNetState" },
                { "rollbackNetState", netState }
            };
            var options = new JsonSerializerOptions { WriteIndented = true };
            var buffer = Encoding.ASCII.GetBytes(JsonSerializer.Serialize(dict, options));
            return buffer;
        }
    }
}