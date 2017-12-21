//using System.Collections;
//using System.Collections.Generic;
//using System.Threading;
//using System.Runtime.InteropServices;
//using System.Security.Cryptography;
//using System.Text;
//using System.IO;
//using System;
//using AssetEx;

//namespace AssetEx
//{

//    /**
//     * 
//     * @author Nico Bergemann <barracuda415 at yahoo.de>
//     * @unity SerializedFile::ObjectInfo
//     */
//    public class ObjectInfo : UnityStruct
//    {
//        public long pathID;

//        // Object data offset
//        public long offset;

//        // Object data size
//        public long length;

//        // Type ID, equal to classID if it's not a MonoBehaviour
//        public int typeID;

//        // Class ID, probably something else in asset format <=5
//        public int classID;

//        // set to 1 if destroyed object instances are stored?
//        public short isDestroyed;

//        public ObjectInfo(VersionInfo versionInfo)
//            : base(versionInfo)
//        {

//        }

//        //throws IOException 
//        public void read(DataReader inr)
//        {
//            pathID = -1;
//            if (versionInfo.assetVersion > 13)
//            {
//                pathID = inr.readLong();
//            }
//            else
//            {
//                pathID = inr.readUnsignedInt();
//            }
//            if (pathID < 0 || pathID > 100000)
//            {
//                throw new Exception("pathID < 0 || pathID > 100000");
//            }

//            offset = inr.readUnsignedInt();
//            length = inr.readUnsignedInt();
//            if (length < 0 || length > inr.BaseStream.Length)
//            {
//                throw new Exception("length <= 0 || length > inr.BaseStream.Length");
//            }
//            typeID = inr.readInt();
//            classID = inr.readShort();
//            isDestroyed = inr.readShort();
//            if (versionInfo.assetVersion >= 15)
//            {
//                inr.readInt();
//            }


//            //assert typeID == classID || (classID == 114 && typeID < 0);
//        }

//        //throws IOException 
//        public void write(DataWriter outw)
//        {
//            if (versionInfo.assetVersion > 13)
//            {
//                outw.writeLong(pathID);
//            }
//            else
//            {
//                outw.writeUnsignedInt((uint)pathID);
//            }

//            outw.writeUnsignedInt((uint)offset);
//            outw.writeUnsignedInt((uint)length);
//            outw.writeInt(typeID);
//            outw.writeShort((short)classID);
//            outw.writeShort(isDestroyed);
//        }

//        public bool isScript()
//        {
//            return typeID < 0;
//        }


//        public UnityClass unityClass()
//        {
//            return new UnityClass(classID);
//        }


//    }
//}