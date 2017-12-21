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
     * Type tree node class.
     * 
     * @author Nico Bergemann <barracuda415 at yahoo.de>
     * @unity TypeNode
     */
    //extends Node<TypeNode> 
    public class TypeNode: UnityStruct
    {

        public static int FLAG_FORCE_ALIGN = 0x4000;

        // field type string
        public String type;

        // field name string
        public String name;

        // size of the field value in bytes or -1 if the field contains sub-fields only
        public int size;

        // field index for the associated parent field
        public int index;

        // set to 1 if "type" is "Array" or "TypelessData"
        public bool isArray;

        // type version, starts with 1 and is incremented when the type
        // information is updated in a new Unity release
        //
        // equal to serializedVersion in YAML format files
        public int version;

        // field flags
        // observed values:
        // 0x1
        // 0x10
        // 0x800
        // 0x4000
        // 0x8000
        // 0x200000
        // 0x400000
        public int metaFlag;

        // Unity 5+, the level of this type within the type tree
        public int treeLevel;

        // Unity 5+, offset to the type string
        public int typeOffset;

        // Unity 5+, offset to the name string
        public int nameOffset;

        public TypeNode(VersionInfo versionInfo)
            : base(versionInfo)
        {
        }

        public bool isForceAlign()
        {
            return (metaFlag & FLAG_FORCE_ALIGN) != 0;
        }

        public void setForceAlign(bool forceAlign)
        {
            if (forceAlign)
            {
                metaFlag |= FLAG_FORCE_ALIGN;
            }
            else
            {
                metaFlag &= ~FLAG_FORCE_ALIGN;
            }
        }

 


        //throws IOException
        public void read(DataReader inr)
        {
            if (versionInfo.assetVersion > 13)
            {
                version = inr.readShort();
                treeLevel = inr.readUnsignedByte();
                isArray = inr.readBoolean();
                typeOffset = inr.readInt() & 0x0000ffff;
                nameOffset = inr.readInt() & 0x0000ffff;
                size = inr.readInt();
                index = inr.readInt();
                metaFlag = inr.readInt();
            }
            else
            {
                type = inr.readStringNull(256);
                name = inr.readStringNull(256);
                size = inr.readInt();
                index = inr.readInt();
                isArray = inr.readInt() == 1;
                version = inr.readInt();
                metaFlag = inr.readInt();
            }
        }

        //throws IOException
        public void write(DataWriter outw)
        {
            outw.writeStringNull(type);
            outw.writeStringNull(name);
            outw.writeInt(size);
            outw.writeInt(index);
            outw.writeInt(isArray ? 1 : 0);
            outw.writeInt(version);
            outw.writeInt(metaFlag);
        }

        private TypeNode _parent;
        private List<TypeNode> _childs = new List<TypeNode>();
        public TypeNode parent() { return _parent; }
        public int count() { return _childs.Count; }
        public List<TypeNode> childs() { return _childs; }
        public bool isEmpty() { return _childs.Count <= 0; }
        public void add(TypeNode n)
        {
            //_childs.Add(n.type.name,n);
            _childs.Add(n);
            n._parent = this;
        }

        public override string ToString()
        {
            return name + "(" + type + ")";
        }

        //public string nodename
        //{
        //    get 
        //    {
        //        if (type == null) return "";
        //        return type.name;
        //    }
        //}

        //public string nodetype
        //{
        //    get
        //    {
        //        if (type == null) return "";
        //        return type.type;
        //    }
        //}

        //public int hashCode() {
        //    int hash = super.hashCode();
        //    hash = 31 * hash + Objects.hashCode(this.type);
        //    return hash;
        //}

        //public boolean equals(Object obj) {
        //    if (obj == null) {
        //        return false;
        //    }
        //    if (getClass() != obj.getClass()) {
        //        return false;
        //    }
        //    final TypeNode other = (TypeNode) obj;
        //    if (!Objects.equals(this.type, other.type)) {
        //        return false;
        //    }
        //    return super.equals(obj);
        //}
    }



 
    /**
     *
     * @author Nico Bergemann <barracuda415 at yahoo.de>
     */
    public class TypeNodeReader
    {
        private VersionInfo versionInfo;

        public TypeNodeReader(VersionInfo versionInfo)
        {
            this.versionInfo = versionInfo;
        }

        //throws IOException 
        public void read(DataReader inr, TypeNode node)
        {
            if (versionInfo.assetVersion > 13)
            {
                readNew(inr, node);
            }
            else
            {
                readOld(inr, node, 0);
            }
        }

        //throws IOException 
        private void readNew(DataReader inr, TypeNode node)
        {
            int numFields = inr.readInt();
            int stringTableLen = inr.readInt();

            // read types
            List<TypeNode> types = new List<TypeNode>(numFields);
            for (int j = 0; j < numFields; j++)
            {
                TypeNode type = new TypeNode(versionInfo);
                type.read(inr);
                types.Add(type);
            }

            // read string table
            byte[] stringTable = new byte[stringTableLen];
            inr.readBytes(ref stringTable);

            // assign strings
            StringTable stExt = new StringTable();
            stExt.loadStrings(stringTable);
            foreach (TypeNode field in types)
            {
                int nameOffset = field.nameOffset;
                String name = stExt.getString(nameOffset);
                if (name == null)
                {
                    name = StringTable.stInt.getString(nameOffset);
                }
                field.name=name;

                int typeOffset = field.typeOffset;
                String type = stExt.getString(typeOffset);
                if (type == null)
                {
                    type = StringTable.stInt.getString(typeOffset);
                }
                field.type=type;

                if (field.isArray)
                {
                    field.name = "Array";
                    field.type = "Array";
                }
            }

            // convert list to tree structure
            TypeNode nodePrev = null;
            foreach (TypeNode nodeCurr in types)
            {
                if (nodePrev == null)
                {
                    //node.type = type;
                    nodePrev = node;
                    continue;
                }

                int levels = nodePrev.treeLevel - nodeCurr.treeLevel;
                if (levels >= 0)
                {
                    // move down in tree hierarchy if required
                    for (int i = 0; i < levels; i++)
                    {
                        nodePrev = nodePrev.parent();
                    }

                    nodePrev.parent().add(nodeCurr);
                }
                else
                {
                    // can move only one level up at a time, so simply add the node
                    nodePrev.add(nodeCurr);
                }

                nodePrev = nodeCurr;
            }

        }

        // throws IOException 
        private void readOld(DataReader inr, TypeNode node, int level)
        {
            node.read(inr);
            node.treeLevel = level;

            int numChildren = inr.readInt();
            for (int i = 0; i < numChildren; i++)
            {
                TypeNode childNode = new TypeNode(versionInfo);
                readOld(inr, childNode, level + 1);
                node.add(childNode);
            }

            bool q = true;
        }
    }

    /**
     *
     * @author Nico Bergemann <barracuda415 at yahoo.de>
     */
    public class TypeNodeWriter
    {

        private VersionInfo versionInfo;

        public TypeNodeWriter(VersionInfo versionInfo)
        {
            this.versionInfo = versionInfo;
        }

        // throws IOException 
        public void write(DataWriter outw, TypeNode node)
        {
            if (versionInfo.assetVersion > 13)
            {
                // TODO
                throw new Exception();
            }
            else
            {
                writeOld(outw, node);
            }
        }

        // throws IOException 
        private void writeOld(DataWriter outw, TypeNode node)
        {
            node.write(outw);

            int numChildren = node.count();
            outw.writeInt(numChildren);
            //foreach (KeyValuePair<string,TypeNode> pair in node.childs())
            foreach (TypeNode childNode in node.childs())
            {
                //TypeNode childNode = pair.Value;
                writeOld(outw, childNode);
            }
        }
    }

}