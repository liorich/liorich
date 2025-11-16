using System.Net.Sockets;
using System.Net;
using System.Text;
using System.Text.Json;

namespace TaksherSOI.Models
{
    [Serializable]
    public class SHF_LQR
    {
        // Basic Parameters
        public uint LinkQuality { get; set; }
        public double a1snr { get; set; }
        public double a1rssi { get; set; }
        public double a2snr { get; set; }
        public double a2rssi { get; set; }

        public SHF_LQR()
        {
            LinkQuality = 0;
            a1snr = 0;
            a1rssi = 0;
            a2snr = 0;
            a2rssi = 0;
        }

        public void Update(double a1snr, double a1rssi, double a2snr, double a2rssi, uint qual)
        {
            LinkQuality = qual;
            this.a1snr = a1snr;
            this.a1rssi = a1rssi;
            this.a2snr = a2snr;
            this.a2rssi = a2rssi;
        }
    }

    // A Host or a Plat can have a report. In the host's case, there's up to 4 reports.
    [Serializable]
    public class Plat
    {
        public UInt16 plat_id { get; set; }
        public SHF_LQR link_receiving_host { get; set; } // How Plat [ID] sees the Host

        public short lastResponse { get; set; }
        public Plat(UInt16 id)
        {
            plat_id = id;
            lastResponse = 0;
            link_receiving_host = new SHF_LQR();
        }
        public void Update(double a1snr, double a1rssi, double a2snr, double a2rssi, uint qual)
        {
            link_receiving_host.Update(a1snr, a1rssi, a2snr, a2rssi, qual);
            lastResponse = 0;
        }
    }
    [Serializable]
    public class Host
    {
        public UInt16 host_id { get; set; }
        public short lastResponse { get; set; }
        public HostReport host_report { get; set; }
        public Dictionary<UInt16, Plat> host_assigned_plats { get; set; }
        public Dictionary<UInt16, SHF_LQR> link_receiving_plats { get; set; }

        public Host(UInt16 host_id)
        {
            this.host_id = host_id;
            host_report = new HostReport();
            lastResponse = 0;
            host_assigned_plats = new Dictionary<UInt16, Plat>();
            link_receiving_plats = new Dictionary<UInt16, SHF_LQR>();
        }
        public void updateStatus(uint netID, uint onePPS, uint onePPSvalid, UInt16 upFreq, UInt16 downFreq, UInt16 txAnt)
        {
            lastResponse = 0;
            this.host_report.netID = netID;
            if (onePPS != 1) this.host_report.master = false; else this.host_report.master = true;
            if (onePPSvalid != 0) this.host_report.onePPSValid = false; else this.host_report.onePPSValid = true;
            this.host_report.upFreq = upFreq;
            this.host_report.downFreq = downFreq;
            this.host_report.txAnt = txAnt;
        }
        public void Remove(UInt16 plat_id)
        {
            host_assigned_plats.Remove(plat_id);
            link_receiving_plats.Remove(plat_id);
        }
        public void Update(double a1snr, double a1rssi, double a2snr, double a2rssi, uint qual, UInt16 ReporterID, UInt16 RemoteID, UInt16 HostID)
        {
            // ReporterID : Host that is reporting this LQReport.
            // SourceID: Plat/Host that is on the receiving end.
            // RemoteID: Plat/Host that is being received.

            lastResponse = 0;

            // We discard any data that does not have a host as the remote or source.
            if (host_id != RemoteID && host_id != HostID) return;

            // Let's separate: If the host is receiving...
            // Host Receiving
            if (ReporterID == HostID)
            {
                if (!link_receiving_plats.ContainsKey(RemoteID)) link_receiving_plats.Add(RemoteID, new SHF_LQR());
                link_receiving_plats[RemoteID].Update(a1snr, a1rssi, a2snr, a2rssi, qual);
                return;
            }
            // Plat receiving Host
            if (RemoteID == HostID)
            {
                if (!host_assigned_plats.ContainsKey(ReporterID)) host_assigned_plats.Add(ReporterID, new Plat(ReporterID));
                host_assigned_plats[ReporterID].Update(a1snr, a1rssi, a2snr, a2rssi, qual);
                return;
            }
        }
    }
    [Serializable]
    public class HostReport
    {
        public uint netID { get; set; }
        public bool master { get; set; } // Configured by 1PPS Source
        public bool onePPSValid { get; set; }
        public UInt16 upFreq { get; set; }
        public UInt16 downFreq { get; set; }
        public UInt16 txAnt { get; set; }

        public HostReport()
        {
            netID = 0;
            master = true;
            onePPSValid = true;
            upFreq = 0;
            downFreq = 0;
            txAnt = 0;
        }
    }

    [Serializable]
    public class SHF_Worker
    {
        public Dictionary<UInt16, Host> groups { get; set; }
        public UdpClient udpClient;
        bool close_thread = false;
        Thread t, hb;
        public SHF_Worker()
        {
            groups = new Dictionary<UInt16, Host>();
            udpClient = new UdpClient(34007);
            t = new Thread(collect_thread); t.Start();
            hb = new Thread(heartbeat); hb.Start();
        }
        public void close(){ close_thread = true; }
        public void collect_thread()
        {
            try
            {
                IPEndPoint remoteEP = new IPEndPoint(IPAddress.Any, 34007);
                while (true)
                {
                    // Receive Bytes
                    byte[] rBytes = udpClient.Receive(ref remoteEP);
                    string rData = Encoding.ASCII.GetString(rBytes);
                    if ((rBytes[4] | (rBytes[5] << 8)) == 0x5294) ParseLinkQuality(rBytes); // Link Quality Message
                    if ((rBytes[4] | (rBytes[5] << 8)) == 0x55FA) parseStatusReport(rBytes);
                    if (close_thread) break;
                    ExportToJson();
                    Thread.Sleep(400);

                }
            }
            catch (Exception ex) { Console.WriteLine(ex.ToString()); }
            finally { udpClient.Close(); }
        }

        public void heartbeat()
        {
            while (true)
            {
                foreach(var host in groups)
                {
                    host.Value.lastResponse++;
                    if(host.Value.lastResponse > 5)
                    {
                        groups.Remove(host.Key); break;
                    }
                    foreach(var plat in host.Value.host_assigned_plats)
                    {
                        plat.Value.lastResponse++;
                        if (plat.Value.lastResponse > 5)
                        {
                            host.Value.host_assigned_plats.Remove(plat.Key); break;
                        }
                    }
                }
                if(close_thread) break;
                Thread.Sleep(2000);
            }
        }

        public void ParseLinkQuality(byte[] data)
        // Parses ONLY Link Quality messages from the host.
        {
            uint len = data[0];

            UInt16 sourceID = (UInt16)data[1]; uint destID = data[2];
            if (!groups.ContainsKey((sourceID)))
            {
                groups.Add(sourceID, new Host(sourceID));
            }

            // uint interfaceID = data[3];
            // UInt16 opcode = data[4] | (data[5] << 8);
            // UInt16 presenceVector = data[6] | (data[7] << 8)
            // uint timeStamp = data[8] ...  data[12]
            // MESSAGE BODY: data[13] -> ...

            UInt16 PlatformID = (UInt16)(data[13] | data[14] << 8);

            //if (groups.ContainsKey(sourceID) && groups.ContainsKey(PlatformID) && (sourceID != PlatformID)) { return; } This is some weird error where the host reports as ID 0...

            uint numPlats = (UInt16)data[15];
            byte[] chunk = new byte[19];

            // Report Vars
            UInt16 RemoteID;
            uint LinkQual;
            // UInt32 Range;
            // UInt16 crcErr
            // UInt16 crcOk
            // UInt16 spare
            double a1rssi; double a1snr; //uint a1sync;
            double a2rssi; double a2snr; //uint a2sync;
            for (int i = 0; i < numPlats; i++)
            {
                Array.Copy(data, 16 + i * 19, chunk, 0, 19);
                RemoteID = (UInt16)(chunk[0] | (chunk[1] << 8));
                LinkQual = (uint)(chunk[2]);
                a1rssi = -(double)(int)(chunk[13]);
                a1snr = (double)(int)(chunk[14]);
                a2rssi = -(double)(int)(chunk[16]);
                a2snr = (double)(int)(chunk[17]);
                groups[sourceID].Update(a1snr, a1rssi, a2snr, a2rssi, LinkQual, PlatformID, RemoteID, sourceID);
            }
        }
        public void parseStatusReport(byte[] data)
        {
            uint len = data[0];
            // uint interfaceID = data[3];
            // UInt16 opcode = data[4] | (data[5] << 8);
            // UInt16 presenceVector = data[6] | (data[7] << 8)
            // uint timeStamp = data[8] ...  data[12]
            UInt16 systemState = Convert.ToUInt16(data[13] | (data[14] << 8));
            UInt16 systemSubState = Convert.ToUInt16(data[15] | (data[16] << 8));
            uint linkMode = data[17];
            uint masterSlaveState = data[18];
            uint onePPSsource = data[19];
            uint TODsource = data[20];
            uint netID = data[21];
            uint onePPSvalid = data[22];
            UInt16 PlatformID = Convert.ToUInt16(data[23] | (data[24] << 8));
            UInt16 upFreq = Convert.ToUInt16(data[25] | (data[26] << 8));
            UInt16 downFreq = Convert.ToUInt16(data[27] | (data[28] << 8));
            UInt16 txAnt = Convert.ToUInt16(data[29] | (data[30] << 8));

            if (groups.ContainsKey(PlatformID))
            {
                groups[PlatformID].updateStatus(netID, onePPSsource, onePPSvalid, upFreq, downFreq, txAnt);
            }
        }
        public void ExportToJson()
        {
            var options = new JsonSerializerOptions { WriteIndented = true };
            var str = JsonSerializer.Serialize(groups, options);
            System.IO.File.WriteAllText("SHF_All.json", str);
        }
        public string changeAntMsg(uint new_ant, UInt16 host_id) // Where do we place this?
        {
            byte[] sBytes = new byte[25];

            // set up message
            sBytes[0] = 0x19; sBytes[1] = 0x64; sBytes[2] = 0xc8; sBytes[3] = 0x00;
            sBytes[4] = 0x30; sBytes[5] = 0x52; //opcode for set antenna
            sBytes[6] = 0x00; sBytes[7] = 0x00; //presence vector
            sBytes[8] = 0xF2; sBytes[9] = 0x02; sBytes[10] = 0x00; sBytes[11] = 0x00; sBytes[12] = 0x00; //timestamp?
            // message body
            sBytes[13] = 0x00; sBytes[14] = 0x00; // remote ID
            sBytes[15] = (byte)new_ant; sBytes[16] = 0x00; // set Tx ant - 2
            sBytes[17] = 0x01; sBytes[18] = 0x00; // set Rx ant - N/A
            sBytes[19] = 0x00; sBytes[20] = 0x00; // Beam number
            sBytes[21] = 0x00; sBytes[22] = 0x00; // Local platform ID
            sBytes[23] = 0x00; sBytes[24] = 0x00; // Spare
            uint crc = CalcCRC32(sBytes);
            byte[] msg = new byte[29];
            Array.Copy(sBytes, 0, msg, 0, 25);
            byte[] crcArr = new byte[4];
            crcArr = BitConverter.GetBytes(crc);
            crcArr.Reverse();
            Array.Copy(crcArr, 0, msg, 25, 4);

            // Try to get IP
            string remoteIP;
            if (File.Exists("PingerConfig.txt"))
            {
                string jsonString = File.ReadAllText("PingerConfig.txt");
                var savedConnections = JsonSerializer.Deserialize<Dictionary<UInt16, string>>(jsonString);
                if (savedConnections == null)
                {
                    // Error loading PingerConfig.txt
                    return "Error: PingerConfig.txt is not well defined.";
                }
                if (savedConnections.ContainsKey(host_id)) remoteIP = savedConnections[host_id];
                else
                {
                    // Couldn't find link
                    return $"Error: PingerConfig.txt does not contain a link for Host ID {host_id}.";
                }
            }
            else
            {
                // Couldn't load link file
                return "Error: PingerConfig.txt does not exist";
            }
            IPEndPoint remoteEndPoint = new IPEndPoint(IPAddress.Parse(remoteIP), 44007);
            udpClient.Send(msg, msg.Length, remoteEndPoint);
            return $"Requested TX antenna change for Host ID {host_id} ({3 - new_ant} -> {new_ant})";
        }
        uint CalcCRC32(byte[] sBytes)
        {
            var CRC32Table = new uint[256];
            const uint polynomial = 0xEDB88320;
            for (uint i = 0; i < CRC32Table.Length; i++)
            {
                uint crc = i;
                for (uint j = 8; j > 0; j--)
                {
                    if ((crc & 1) == 1)
                    {
                        crc = (crc >> 1) ^ polynomial;
                    }
                    else
                    {
                        crc >>= 1;
                    }
                }
                CRC32Table[i] = crc;
            }

            uint crcF = 0xFFFFFFFF;
            foreach (byte b in sBytes)
            {
                byte tableIndex = (byte)((crcF ^ b) & 0xFF);
                crcF = (crcF >> 8) ^ CRC32Table[tableIndex];
            }
            crcF = ~crcF;
            return crcF;
        }
    }
}
