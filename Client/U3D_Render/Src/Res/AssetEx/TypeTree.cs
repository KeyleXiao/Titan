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
      * Class for objects that hold the runtime type information of an asset file.
      * 
      * @author Nico Bergemann <barracuda415 at yahoo.de>
      * @unity RTTIClassHierarchyDescriptor, RTTIBaseClassDescriptor2, TypeTree
      */
    public class TypeTree : UnityStruct
    {
        public Dictionary<int, EmbeddedTypeInfo> embeddedTypeMap = new Dictionary<int, EmbeddedTypeInfo>();
        public int attributes;
        public bool embedded;

        public TypeTree(VersionInfo versionInfo): base(versionInfo)
        {
        }

        //throws IOException 
        public void read(DataReader inr)
        {
            // revision/version for newer formats
            if (versionInfo.assetVersion > 6)
            {
                string sver = inr.readStringNull(255);
                versionInfo.unityRevision = new UnityVersion(sver);
                attributes = inr.readInt();
            }

            TypeNodeReader nodeReader = new TypeNodeReader(versionInfo);

            // Unity 5+ uses a serialized tree structure and string buffers
            if (versionInfo.assetVersion > 13)
            {
                embedded = inr.readBoolean();
                int numBaseClasses = inr.readInt();
                Logger.Log("numBaseClasses=" + numBaseClasses);
                if (numBaseClasses > 1000) return;
                for (int i = 0; i < numBaseClasses; i++)
                {
                    int classID = inr.readInt();
                    EmbeddedTypeInfo embeddedtype = new EmbeddedTypeInfo();
                    embeddedtype.classID = classID;

                    if (classID < 0)
                    {
                        UnityHash128 scriptID = new UnityHash128(versionInfo);
                        scriptID.read(inr);
                        embeddedtype.scriptID = scriptID;
                    }

                    UnityHash128 oldTypeHash = new UnityHash128(versionInfo);
                    oldTypeHash.read(inr);
                    embeddedtype.oldTypeHash = oldTypeHash;

                    if (embedded)
                    {
                        TypeNode node = new TypeNode(versionInfo);
                        nodeReader.read(inr, node);
                        embeddedtype.typeTree = node;
                    }

                    embeddedTypeMap[classID] = embeddedtype;
                }
            }
            else
            {
                int numBaseClasses = inr.readInt();
                Logger.Log("numBaseClasses=" + numBaseClasses);
                for (int i = 0; i < numBaseClasses; i++)
                {
                    int classID = inr.readInt();

                    EmbeddedTypeInfo embeddedtype = new EmbeddedTypeInfo();
                    embeddedtype.classID = classID;

                    TypeNode node = new TypeNode(versionInfo);
                    nodeReader.read(inr, node);
                    embeddedtype.typeTree = node;

                    embeddedTypeMap[classID]=embeddedtype;
                }

                embedded = numBaseClasses > 0;

                // padding
                if (versionInfo.assetVersion > 6)
                {
                    inr.readInt();
                }
            }
        }

        //throws IOException 
        public void write(DataWriter outw)
        {
            // revision/version for newer formats
            if (versionInfo.assetVersion > 6)
            {
                outw.writeStringNull(versionInfo.unityRevision.ToString());
                outw.writeInt(attributes);
            }

            TypeNodeWriter nodeWriter = new TypeNodeWriter(versionInfo);

            if (versionInfo.assetVersion > 13)
            {
                // TODO
                throw new Exception();
            }
            else
            {
                int numBaseClasses = embeddedTypeMap.Count;
                outw.writeInt(numBaseClasses);

                foreach (KeyValuePair<int, EmbeddedTypeInfo> pair in embeddedTypeMap)
                {
                    EmbeddedTypeInfo etinfo = pair.Value;
                    int classID = etinfo.classID;
                    outw.writeInt(classID);

                    TypeNode node = etinfo.typeTree;
                    nodeWriter.write(outw, node);
                }

                // padding
                if (versionInfo.assetVersion > 6)
                {
                    outw.writeInt(0);
                }
            }
        }
    }


}
