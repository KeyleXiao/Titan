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
     * @unity SerializedFileHeader
     */
    public class AssetHeader : UnityStruct
    {

        // size of the structure data
        public long metadataSize;

        // size of the whole asset file
        public long fileSize;

        // offset to the serialized data
        public long dataOffset;

        // byte order of the serialized data?
        public byte endianness;

        // unused
        public byte[] reserved = new byte[3];

        public AssetHeader(VersionInfo versionInfo)
            : base(versionInfo)
        {

        }

        public int getversion()
        {
            return versionInfo.assetVersion;
        }

        public void setversion(int version)
        {
            versionInfo.assetVersion = version;
        }

        public void read(DataReader inr)
        {
            metadataSize = inr.readInt();
            fileSize = inr.readUnsignedInt();
            versionInfo.assetVersion = inr.readInt();
            dataOffset = inr.readUnsignedInt();
            if (versionInfo.assetVersion >= 9)
            {
                byte[] data = new byte[4];
                inr.readBytes(ref data);
                endianness = data[0];
                //endianness = inr.readByte();
                //inr.readBytes(ref reserved);
            }
        }

        public void write(DataWriter outw)
        {
            outw.writeUnsignedInt((uint)metadataSize);
            outw.writeUnsignedInt((uint)fileSize);
            outw.writeInt(versionInfo.assetVersion);
            outw.writeUnsignedInt((uint)dataOffset);
            if (versionInfo.assetVersion >= 9)
            {
                outw.writeByte(endianness);
                outw.writeBytes(reserved);
            }
        }

    }

}
