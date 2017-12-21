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
     *
     * @author Nico Bergemann <barracuda415 at yahoo.de>
     */
    public class ObjectInfoTable : UnityStruct
    {

        public Dictionary<long, ObjectData> infoMap = new Dictionary<long, ObjectData>();

        public ObjectInfoTable(VersionInfo versionInfo): base(versionInfo)
        {
        }

        //throws IOException 
        public void read(DataReader inr)
        {
            int entries = inr.readInt();
            Logger.Log("objectinfo entries=" + entries);
            if (versionInfo.assetVersion > 13)
            {
                inr.align(4);
            }

            for (int i = 0; i < entries; i++)
            {
                ObjectData info = new ObjectData(versionInfo);
                info.read(inr);
                if (infoMap.ContainsKey(info.fileID))
                {
                    throw new Exception("infoMap.ContainsKey(pathID)" + info.fileID);
                }
                infoMap[info.fileID] = info;

            }
        }

        //throws IOException 
        public void write(DataWriter outw)
        {
            int entries = infoMap.Count;
            outw.writeInt(entries);

            if (versionInfo.assetVersion > 13)
            {
                outw.align(4);
            }

            foreach (KeyValuePair<long, ObjectData> infoEntry in infoMap)
            {
                long pathID = infoEntry.Key;
                ObjectData info = infoEntry.Value;
                info.write(outw);
            }
        }
    }
}