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
    // implements Iterable<FileIdentifier> 
    public class FileIdentifierTable : UnityStruct
    {

        public List<FileIdentifier> fileIDs=new List<FileIdentifier>();

        public FileIdentifierTable(VersionInfo versionInfo): base(versionInfo)
        {
        }

        // throws IOException 
        public void read(DataReader inr)
        {
            int entries = inr.readInt();
            Logger.Log("externals entries=" + entries);
            for (int i = 0; i < entries; i++)
            {
                FileIdentifier fi = new FileIdentifier(versionInfo);
                fi.read(inr);
                fileIDs.Add(fi);
            }
        }

        //throws IOException 
        public void write(DataWriter outw)
        {
            int entries = fileIDs.Count;
            outw.writeInt(entries);

            foreach (FileIdentifier fi in fileIDs)
            {
                fi.write(outw);
            }
        }

        //public Iterator<FileIdentifier> iterator() {
        //    return fileIDs.iterator();
        //}
    }
}
