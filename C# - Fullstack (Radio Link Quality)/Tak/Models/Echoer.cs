using Microsoft.AspNetCore.Authorization.Infrastructure;
using Microsoft.AspNetCore.Mvc.ViewFeatures;
using System.Net.WebSockets;
using System.Text;
using System.Text.Json;
using System.Xml.Serialization;

namespace Tak.Models
{
    public class Echoer
    {
        public static async Task Echo(WebSocket ws, Worker WorkerMaster)
        {
            Task feed_data = new Task(() =>
            {
                Feeder(ws, WorkerMaster);
            });
            feed_data.Start();
            Task get_requests = new Task(() =>
            {
                Receiver(ws, WorkerMaster);
            });
            get_requests.Start();
            while (!WorkerMaster.close_threads)
            {
                Thread.Sleep(1000);
            }
        }
        public static async Task Feeder(WebSocket ws, Worker WorkerMaster)
        {
            return;
            /*
            while (!WorkerMaster.close_threads)
            {
                var buffer = WorkerMaster.Exporter_HostPlatData();
                if(buffer != null)
                {
                    await ws.SendAsync(new ArraySegment<byte>(buffer), WebSocketMessageType.Text, true, CancellationToken.None);
                }
                if (WorkerMaster.Timeouts.Count > 0)
                {
                    var dict = WorkerMaster.Timeouts;
                    var buf = WorkerMaster.Report_Timeouts(dict);
                    WorkerMaster.Timeouts.Clear();
                    await ws.SendAsync(new ArraySegment<byte>(buf), WebSocketMessageType.Text, true, CancellationToken.None);
                }
                Thread.Sleep(1000);
            }*/
        }
        public static async Task Receiver(WebSocket ws, Worker WorkerMaster)
        {
            
            var buffer = new byte[1024 * 4];
            byte[]? buffer_msg;
            byte[]? buffer_data;
            
            while (!WorkerMaster.close_threads)
            {
                var response = await ws.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);
                var result = Encoding.ASCII.GetString(buffer, 0, response.Count);
                Array.Clear(buffer, 0, buffer.Length);
                var options = new JsonSerializerOptions { WriteIndented = true };
                var item = JsonSerializer.Deserialize<Dictionary<string, string>>(result, options);
                if (item == null) continue;
                else
                {
                    try
                    {
                        if (item["type"] != "message") continue;
                        var req = item["request"];
                        switch(req)
                        {
                            case "changeTxAnt":
                                // Requesting a Transmission antenna swap.
                                /*
                                var success = WorkerMaster.changeAntMsg(
                                    (uint)(3 - WorkerMaster.main.groups[Convert.ToUInt16(item["id"])].host_report.txAnt),
                                    Convert.ToUInt16(item["id"])
                                    );
                                buffer_msg = WorkerMaster.Compose_Msg(success);
                                await ws.SendAsync(new ArraySegment<byte>(buffer_msg), WebSocketMessageType.Text, true, CancellationToken.None);
                                */
                                break;
                            case "setComment":
                                // Add a comment in the logger.
                                /*
                                var plat_id = Convert.ToUInt16(item["id"]);
                                var com = item["comment"].Replace("\n", "");
                                if(WorkerMaster.LoggerComments.ContainsKey(plat_id)) { break; }
                                WorkerMaster.LoggerComments.Add(plat_id, com);
                                buffer_msg = WorkerMaster.Compose_Msg("Added comment for platform " + plat_id);
                                await ws.SendAsync(new ArraySegment<byte>(buffer_msg), WebSocketMessageType.Text, true, CancellationToken.None);
                                */
                                break;

                                // Display Logs Functionality
                            case "requestLoggerDates":
                                buffer_data = WorkerMaster.Exporter_RequestDates();
                                await ws.SendAsync(new ArraySegment<byte>(buffer_data), WebSocketMessageType.Text, true, CancellationToken.None);
                                break;
                            case "requestLoggerCoupling":
                                buffer_data = WorkerMaster.Exporter_RequestCoupling(item["Date"]);
                                await ws.SendAsync(new ArraySegment<byte>(buffer_data), WebSocketMessageType.Text, true, CancellationToken.None);
                                break;
                            case "requestAvailableTimeFrame":
                                buffer_data = WorkerMaster.Exporter_RequestAvailableTimeFrame(item["Date"], item["File"]);
                                await ws.SendAsync(new ArraySegment<byte>(buffer_data), WebSocketMessageType.Text, true, CancellationToken.None);
                                break;
                            case "requestLogs":
                                buffer_data = WorkerMaster.Exporter_GenerateLogs(item["Date"], item["File"], item["Start"], item["End"]);
                                await ws.SendAsync(new ArraySegment<byte>(buffer_data), WebSocketMessageType.Text, true, CancellationToken.None);
                                break;
                            case "saveSetup":
                                buffer_data = WorkerMaster.Initialise(item["dict"]);
                                await ws.SendAsync(new ArraySegment<byte>(buffer_data), WebSocketMessageType.Text, true, CancellationToken.None);
                                break;
                            // Get Pinger Configuration
                            case "requestRollbackDates":
                                buffer_data = WorkerMaster.getRollbackDates();
                                await ws.SendAsync(new ArraySegment<byte>(buffer_data), WebSocketMessageType.Text, true, CancellationToken.None);
                                break;
                            case "requestRollbackNetState":
                                buffer_data = WorkerMaster.getRollbackNetState(item["date"]);
                                await ws.SendAsync(new ArraySegment<byte>(buffer_data), WebSocketMessageType.Text, true, CancellationToken.None);
                                break;
                        }
                    }
                    catch (Exception ex) { 
                        Console.Write(ex.ToString());
                        continue;
                    }
                }
            }
        }
    }
}
