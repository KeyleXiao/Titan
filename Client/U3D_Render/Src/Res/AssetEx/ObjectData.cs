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
    public class ObjectData : UnityStruct
    {
        public long fileID;  //LocalIdentifierInFileType

        // Object data offset
        public long offset;

        // Object data size
        public long length;

        // Type ID, equal to classID if it's not a MonoBehaviour
        public int typeID;

        // Class ID, probably something else in asset format <=5
        public int classID;

        // set to 1 if destroyed object instances are stored?
        public short isDestroyed;

        public string path;

        int RemapClassIDToNewClassID(int classID)
        {
            switch (classID)
            {
                case 1012: return 1011; // AvatarSkeletonMask -> AvatarMask
                default: return classID;
            }
        }

        //throws IOException 
        public void read(DataReader inr)
        {
            fileID = -1;
            if (versionInfo.assetVersion > 13)
            {
                fileID = inr.readLong();
            }
            else
            {
                fileID = inr.readUnsignedInt();
            }
            if (fileID < 0 || fileID > 100000)
            {
                throw new Exception("pathID < 0 || pathID > 100000");
            }

            offset = inr.readUnsignedInt();
            length = inr.readUnsignedInt();
            if (length < 0 || length > inr.BaseStream.Length)
            {
                throw new Exception("length <= 0 || length > inr.BaseStream.Length");
            }
            typeID = RemapClassIDToNewClassID(inr.readInt());
            classID = RemapClassIDToNewClassID(inr.readShort());
            isDestroyed = inr.readShort();
            if (versionInfo.assetVersion >= 15)
            {
                inr.readInt();
            }


            //assert typeID == classID || (classID == 114 && typeID < 0);
        }

        //throws IOException 
        public void write(DataWriter outw)
        {
            if (versionInfo.assetVersion > 13)
            {
                outw.writeLong(fileID);
            }
            else
            {
                outw.writeUnsignedInt((uint)fileID);
            }

            outw.writeUnsignedInt((uint)offset);
            outw.writeUnsignedInt((uint)length);
            outw.writeInt(typeID);
            outw.writeShort((short)classID);
            outw.writeShort(isDestroyed);
        }

        public bool isScript()
        {
            return typeID < 0;
        }


        public UnityClass unityClass()
        {
            return new UnityClass(classID);
        }

        //public ObjectSerializer serializer;
        public ByteBuffer buffer;
        public TypeNode typeTree;
        public FieldNode instance;

        public ObjectData(VersionInfo versionInfo):base(versionInfo)
        {
        }


        public String objectname
        {
            get
            {
                String name = "";
                if (instance != null)
                {
                    name = instance.getString("m_Name");
                }
                return name;
            }
        }

        public Dictionary<string,object> propertyfield
        {
            get
            {
                if (instance == null) return null;
                Dictionary<string, object> ret=new Dictionary<string, object>();
                Dictionary<string, FieldNode> childs=instance.getChilds();
                if (childs != null)
                {
                    foreach (KeyValuePair<string, FieldNode> pair in childs)
                    {
                        if (pair.Value != null)
                        {
                            FieldNode fn = pair.Value;
                            object obj = fn.value;
                            ret[pair.Key] = obj;
                        }
                    }
                }
                return ret;
            }
        }

        public String classname
        {
            get
            {
                return unityClass().name();
            }
        }

        public String typename
        {
            get
            {
                if (typeTree == null) return "";
                return typeTree.type;
            }
        }

        public object data
        {
            get
            {
                object ret = null;
                if (instance != null)
                {
                    ret = instance.getObject("image data");
                    if (ret == null)
                    {
                        ret = instance.getObject("Array");
                    }
                }
                return ret;
            }
        }

        public override string ToString()
        {
            return objectname + "<"+classname+">";
        }
    }
}
