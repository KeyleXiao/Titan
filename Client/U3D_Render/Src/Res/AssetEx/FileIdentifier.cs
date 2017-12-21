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
     * @unity FileIdentifier 
     */
    public class FileIdentifier : UnityStruct
    {

        // Path to the asset file? Unused in asset format <= 5.
        public String assetPath;

        // Globally unique identifier of the referred asset. Unity displays these
        // as simple 16 byte hex strings with each byte swapped, but they can also
        // be represented according to the UUID standard.
        private UnityGUID guid = new UnityGUID();

        // Path to the asset file. Only used if "type" is 0.
        public String filePath;

        // Reference type. Possible values are probably 0 to 3.
        public int type;

        public AssetFileEx assetFile;

        public FileIdentifier(VersionInfo versionInfo)
            : base(versionInfo)
        {
        }

        //throws IOException 
        public void read(DataReader inr)
        {
            if (versionInfo.assetVersion > 5)
            {
                assetPath = inr.readStringNull();
            }

            guid.read(inr);
            type = inr.readInt();
            filePath = inr.readStringNull();
        }

        //throws IOException 
        public void write(DataWriter outw)
        {
            if (versionInfo.assetVersion > 5)
            {
                outw.writeStringNull(assetPath);
            }

            guid.write(outw);
            outw.writeInt(type);
            outw.writeStringNull(filePath);
        }

    }
}