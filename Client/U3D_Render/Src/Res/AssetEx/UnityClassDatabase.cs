using System.Collections;
using System.Collections.Generic;
using System.Threading;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.IO;
using System;
using AssetEx;

namespace AssetEx
{
    /**
 * Unity class ID container.
 * 
 * @author Nico Bergemann <barracuda415 at yahoo.de>
 */
    public class UnityClass
    {


        private int id;

        public UnityClass(int id)
        {
            this.id = id;
        }

        public UnityClass(String name)
        {
            int lid = UnityClassDatabase.Instance.IDForName(name);
            this.id = lid;
        }

        public int ID()
        {
            return id;
        }

        public String name()
        {
            return UnityClassDatabase.Instance.nameForID(id);
        }

        public int hashCode()
        {
            return 78 * id;
        }

        public bool equals(object obj)
        {
            if (obj == null)
            {
                return false;
            }
            if (GetType() != obj.GetType())
            {
                return false;
            }
            UnityClass other = (UnityClass)obj;
            return this.id == other.id;
        }

        public String toString()
        {
            StringBuilder sb = new StringBuilder();
            sb.Append("Class ");
            sb.Append(id);

            String name = "";
            sb.Append(" (");
            sb.Append(name);
            sb.Append(")");

            return sb.ToString();
        }
    }


    /**
      * Database class to translate Unity class names and IDs.
      * 
      * @author Nico Bergemann <barracuda415 at yahoo.de>
      */
    public class UnityClassDatabase
    {
        public static UnityClassDatabase Instance = new UnityClassDatabase();

        private Dictionary<int, String> IDToName = new Dictionary<int, String>();
        private Dictionary<String, int> nameToID = new Dictionary<String, int>();

        public void load(Stream s)
        {
            StreamReader reader = new StreamReader(s, Encoding.ASCII);
            for (String line; (line = reader.ReadLine()) != null; )
            {
                // skip comments and blank lines
                if (line.StartsWith("#") || line.StartsWith("//") || line.Trim() == "")
                {
                    continue;
                }

                String[] parts = line.Split(","[0]);

                if (parts.Length == 2)
                {
                    int id = -1;
                    int.TryParse(parts[0], out id);
                    String name = parts[1];

                    IDToName[id] = name;
                    nameToID[name] = id;
                }
            }
            reader.Close();
        }

        public int IDForName(String name)
        {
            if (nameToID.ContainsKey(name) == false) return -9999;
            return nameToID[name];
        }

        public String nameForID(int id)
        {
            if (IDToName.ContainsKey(id) == false) return "";
            return IDToName[id];
        }
    }
}
