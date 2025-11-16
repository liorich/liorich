using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Net;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Text;
using System.ComponentModel.Design.Serialization;
using System.Xml.Linq;

namespace Tak.Models
{
    public class Root
    {
        // This collects all tree roots/meshes
        public List<AnyNode> Roots { get; set; }
        public Root() { Roots = new(); }
    }
    public partial class Worker
    {
        // Setup Roots
        public Root root = new Root();
        public List<string> root_names = new List<string>();

        // Data collectors
        public SHF_Worker shf_Worker;
        public DTC_Worker dtc_Worker;

        // Status and Info queues
        public bool close_threads;
        
        public Worker()
        {
            shf_Worker = new SHF_Worker();
            // Temporary

            dtc_Worker = new DTC_Worker();
            //Start();
        }
        ~Worker()
        {
            shf_Worker.close();
            dtc_Worker.close();
        }
        public bool Start()
        {
            try
            {
                // Read setup
                foreach(var name in root_names)
                {
                    root.Roots.Add(new AnyNode(name));
                }

                return true;
            }
            catch { return false; }
        }

        public void StaticStart()
        {

            Dictionary<string, AnyNode> StaticDictionary = new();
            StaticDictionary.Add("GDT1", new AnyNode("GDT1"));
            StaticDictionary.Add("GDT2", new AnyNode("GDT2"));
            int a = 1;
            //StaticDictionary.Add("ADT117", new AnyNode("ADT117"));

        }        
    }
}
