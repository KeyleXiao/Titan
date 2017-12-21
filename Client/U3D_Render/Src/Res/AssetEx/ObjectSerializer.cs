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
    public class StreamBlock
    {
        public long offset;
        public long length;

        public override string ToString()
        {
            return "{offset=" + offset + ",length=" + length + "}<StreamBlock>";
        }
    }

    public class Vector3f
    {
        public float x,y,z;
        public override string ToString()
        {
            return "{" + x + "," + y + "," + z + "}<Vector3f>";
        }
    }

    public class Quaternionf
    {
        public float x, y, z,w;
        public override string ToString()
        {
            return "{" + x + "," + y + "," + z + "," + w + "}<Quaternionf>";
        }
    }

    public class PackedBitVector
    {
        public uint m_NumItems;
        public float m_Range;
        public float m_Start;
        public StreamBlock m_Data;
        public byte m_BitSize;
    }

    public class VertexData
    {
        public uint m_CurrentChannels;
        public uint m_VertexCount;
        public List<object> m_Channels;
        public StreamBlock Array;
    }

    public class UnityPropertySheet
    {
        public Dictionary<object, object> m_TexEnvs;
        public Dictionary<object, object> m_Floats;
        public Dictionary<object, object> m_Colors;
    }

    public class PPtrObject
    {
        public int m_FileID;
        public long m_PathID;
        public long GetKey()
        {
            return m_FileID * 1000000 + m_PathID;
        }

        public override string ToString()
        {
            return "{m_FileID=" + m_FileID + ",m_PathID=" + m_PathID + "}<PPtrObject>";
        }
    }

    public class AssetInfo
    {
        public int preloadIndex;
        public int preloadSize;
        public PPtrObject asset;

        public override string ToString()
        {
            return "{preloadIndex=" + preloadIndex + ",preloadSize=" + preloadSize +","+asset.ToString()+ "}<AssetInfo>";
        }
    }

    public class ResourceManager_Dependency
    {
        public PPtrObject m_Object;
        public List<object> m_Dependencies;
    }

    /**
     *
     * @author Nico Bergemann <barracuda415 at yahoo.de>
     */
    public class ObjectSerializer
    {

        private static bool DEBUG = false;
        private static int ALIGNMENT = 4;

        private ByteBuffer soundData;
        private VersionInfo versionInfo;

        public ByteBuffer getSoundData()
        {
            return soundData;
        }

        public void setSoundData(ByteBuffer soundData)
        {
            this.soundData = soundData;
        }

        //throws IOException 
        public void serialize(ObjectData data)
        {
            throw new Exception("Not supported yet.");
        }

        // throws IOException 
        public void deserialize(DataReader inr, ObjectData data)
        {
            versionInfo = data.versionInfo;

            //MemoryStream ms = new MemoryStream(data.buffer.data);
            //DataReader inr = new DataReader(ms);
            ////DataReader inr = DataReaders.forByteBuffer(data.buffer);
            //inr.order = versionInfo.order();
            //inr.SetPosition( 0;
            //FieldNode instance = readObject(inr, data.typeTree);

            ByteOrder oldorder = inr.order;
            inr.order = versionInfo.order();
            FieldNode instance = readObject(inr, data.typeTree);
            inr.order = oldorder;

            data.instance = instance;
        }

        private void fillobject(object obj, FieldNode fn)
        {
            foreach (System.Reflection.FieldInfo fi in obj.GetType().GetFields())
            {
                object val = fn.getObject(fi.Name);
                fi.SetValue(obj, val);
            }
            fn.value = obj;
        }

        //throws IOException 
        private FieldNode readObject(DataReader inr, TypeNode typeNode)
        {
            if (typeNode == null)
            {
                return null;
            }



            //if (DEBUG)
            //{
            //    System.out.printf("%s0x%x: %s v: %d, f: 0x%x, s: %d\n",
            //            StringUtils.repeat("  ", type.treeLevel()), in.position(), type.typeName(),
            //            type.version(), type.metaFlag(), type.size());
            //}

            FieldNode fieldNode = new FieldNode();
            fieldNode.typenode = typeNode;

            // if the type has no children, it has a primitve value
            if (typeNode.isEmpty() && typeNode.size > 0)
            {
                fieldNode.value = readPrimitiveValue(inr, typeNode, -1);
            }

            // read object fields
            foreach (TypeNode childTypeNode in typeNode.childs())
            {
                //TypeNode childTypeNode = pair.Value;

                // Check if the current node is an array and if the current field is
                // "data". In that case, "data" needs to be read "size" times.
                if (typeNode.isArray && childTypeNode.name == "size")
                {
                    FieldNode childFieldNode = readObject(inr, childTypeNode);
                    fieldNode.add(childFieldNode);
                    int size = fieldNode.getSInt32("size");
                    if (size > 4096 * 4096 * 4)
                    {
                        fieldNode.error = true;
                        break;
                    }
                }
                else if (typeNode.isArray && childTypeNode.name == "data")
                {
                    int size = fieldNode.getSInt32("size");
                    if (size > 4096*4096*4)
                    {
                        fieldNode.error = true;
                        break;
                    }

                    FieldNode childFieldNode = new FieldNode();
                    childFieldNode.typenode = childTypeNode;

                    // if the child type has no children, it has a primitve array
                    if (childTypeNode.isEmpty())
                    {
                        childFieldNode.value = readPrimitiveValue(inr, childTypeNode, size);
                    }
                    else
                    {
                        // read list of object nodes
                        bool error = false;
                        List<FieldNode> childFieldNodes = new List<FieldNode>(size);
                        for (int i = 0; i < size; i++)
                        {
                            FieldNode fn = readObject(inr, childTypeNode);
                            childFieldNodes.Add(fn);
                            if (fn.error)
                            {
                                error = true;
                                break;
                            }
                        }
                        childFieldNode.value = childFieldNodes;
                        if (error)
                        {
                            childFieldNode.error = true;
                        }
                    }

                    fieldNode.add(childFieldNode);
                    if (childFieldNode.error)
                    {
                        fieldNode.error = true;
                        break;
                    }
                }
                else
                {
                    FieldNode childFieldNode = readObject(inr, childTypeNode);
                    fieldNode.add(childFieldNode);
                    if (childFieldNode.error)
                    {
                        fieldNode.error = true;
                        break;
                    }
                }
            }

            postProcessObjectNode(fieldNode);
  
            return fieldNode;
        }

        private void postProcessObjectNode(FieldNode fieldNode)
        {
            // convert byte buffers to string instances in "string" fields for convenience
            if (fieldNode.typenode == null || fieldNode.typenode.type == null) return;
            if (fieldNode.typenode.type == "string")
            {
                // strings use "char" arrays, so it should be wrapped in a ByteBuffer
                //ByteBuffer buf = fieldNode.getArrayData<ByteBuffer>();
                object arrayobj = fieldNode.getArrayData();
                if (arrayobj!=null && arrayobj is ByteBuffer)
                {
                    ByteBuffer buf = (ByteBuffer) arrayobj;
                    char[] carray = new char[buf.data.Length];
                    for (int i = 0; i < carray.Length; i++)
                    {
                        carray[i] = (char)buf.data[i];
                    }
                    fieldNode.value = new String(carray);//, "UTF-8"
                    fieldNode.cleanChilds();
                }
            }
            else if (fieldNode.typenode.isArray)
            {
                int size = fieldNode.getSInt32("size");
                fieldNode.value = fieldNode.getObject("data");
            }
            else if (fieldNode.typenode.type.StartsWith("PPtr<") && fieldNode.typenode.type.EndsWith(">"))
            {
                object obj = new PPtrObject();
                fillobject(obj, fieldNode);
                fieldNode.cleanChilds();
            }
            else if (fieldNode.typenode.type == "Quaternionf" || fieldNode.typenode.type == "Vector3f"||
                fieldNode.typenode.type == "PackedBitVector" || fieldNode.typenode.type == "VertexData" ||
                fieldNode.typenode.type == "UnityPropertySheet" || fieldNode.typenode.type == "AssetInfo"
                || fieldNode.typenode.type == "ResourceManager_Dependency")
            {
                System.Type t = System.Type.GetType("AssetEx."+fieldNode.typenode.type, false, false);
                System.Reflection.ConstructorInfo ci = t.GetConstructor(new Type[] { });
                object v = ci.Invoke(new object[] { });
                fillobject(v, fieldNode);
                fieldNode.cleanChilds();
            }
            else if (fieldNode.typenode.type == "map")
            {
                List<FieldNode> childnodes = (List<FieldNode>)fieldNode.getArrayData();
                Dictionary<object, object> map = new Dictionary<object, object>();
                foreach (FieldNode fn in childnodes)
                {
                    object first = fn.getObject("first");
                    object second = fn.getObject("second");
                    if (first != null && second != null)
                    {
                        map[first] = second;
                    }
                }
                fieldNode.value = map;
                fieldNode.cleanChilds();
            }
            else if (fieldNode.typenode.type == "vector")
            {
                object obj = fieldNode.getArrayData();
                if (obj is List<FieldNode>)
                {
                    List<FieldNode> childnodes = (List<FieldNode>)obj;
                    List<object> vectorl = new List<object>();
                    foreach (FieldNode fn in childnodes)
                    {
                        if (fn.value != null)
                        {
                            vectorl.Add(fn.value);
                        }
                        else
                        {
                            vectorl.Add(fn);
                        }
                    }
                    fieldNode.value = vectorl;
                    fieldNode.cleanChilds();
                }
                else
                {
                    fieldNode.value = obj;
                    fieldNode.cleanChilds();
                }
            }
            else if (fieldNode.getChilds() != null && fieldNode.getChilds().Count == 1)
            {
                fieldNode.value = fieldNode.getArrayData();
                if (fieldNode.value != null)
                {
                    fieldNode.cleanChilds();
                }
            }

        }

        // throws IOException, RuntimeTypeException 
        private object readPrimitiveValue(DataReader inr, TypeNode type, int size)
        {


            long pos = 0;
            //if (DEBUG) 
            //{
            //    pos = in.position();
            //}

            object value;
            if (size == -1)
            {
                value = readPrimitive(inr, type);
                if (type.isForceAlign())
                {
                    inr.align(ALIGNMENT);
                }
            }
            else
            {
                value = readPrimitiveArray(inr, type, size);
                if (versionInfo.assetVersion > 5)
                {
                    // arrays always need to be aligned in newer versions
                    inr.align(ALIGNMENT);
                }
            }

            //if (DEBUG) {
            //    long bytes = in.position() - pos;
            //    System.out.printf("%s0x%x: %s %s = %s, b: %d, v: %d, f: 0x%x, s: %d\n",
            //            StringUtils.repeat("  ", type.treeLevel()), pos, type.typeName(), type.fieldName(), value, bytes,
            //            type.version(), type.metaFlag(), size);
            //}

            return value;
        }

        //throws IOException, RuntimeTypeException 
        private object readPrimitive(DataReader inr, TypeNode type)
        {
            switch (type.type)
            {
                // 1 byte
                case "bool":
                    return inr.readBoolean();

                case "SInt8":
                    return inr.readByte();

                case "UInt8":
                case "char":
                    return inr.readUnsignedByte();

                // 2 bytes
                case "SInt16":
                case "short":
                    return inr.readShort();

                case "UInt16":
                case "unsigned short":
                    return inr.readUnsignedShort();

                // 4 bytes
                case "SInt32":
                case "int":
                    return inr.readInt();

                case "UInt32":
                case "unsigned int":
                    return inr.readUnsignedInt();

                case "float":
                    return inr.readFloat();

                // 8 bytes
                case "SInt64":
                case "long":
                    return inr.readLong();

                case "UInt64":
                case "unsigned long":
                    return inr.readUnsignedLong();

                case "double":
                    return inr.readDouble();
                case "bytes[3]":
                    return inr.readByte3();
                case "z":
                    return inr.readFloat();
                case "OffsetPtr":
                    return inr.readByte();
                default:
                    throw new Exception("Unknown primitive type: " + type.type);
            }
        }

        //throws IOException, RuntimeTypeException 
        private object readPrimitiveArray(DataReader inr, TypeNode type, int size)
        {
            switch (type.type)
            {
                // read byte arrays natively and wrap them as ByteBuffers,
                // which is much faster and more efficient than a list of wrappped
                // Byte/Integer objects
                case "SInt8":
                case "UInt8":
                    StreamBlock sdi = new StreamBlock();
                    sdi.offset = inr.position;

                    //ByteBuffer buf;

                    //// NOTE: AudioClips "fake" the size of m_AudioData when the stream is
                    //// stored in a separate file. The array contains just an offset integer
                    //// in that case, so pay attention to the bytes remaining in the buffer
                    //// as well to avoid EOFExceptions.
                    //long remaining = inr.remaining();
                    //if (size > remaining && remaining == 4)
                    //{
                    //    int offset = inr.readInt();
                    //    // create empty sound buffer in case the .resS file doesn't
                    //    // exist
                    //    if (soundData != null)
                    //    {
                    //        buf = new ByteBuffer(soundData, offset, size);// ByteBufferUtils.getSlice(soundData, offset, size);
                    //    }
                    //    else
                    //    {
                    //        buf = new ByteBuffer(size);
                    //    }
                    //}
                    //else
                    //{
                    //    buf = new ByteBuffer(size);
                    //    inr.readBuffer(buf);
                    //}

                    //buf.clear();


                    // NOTE: AudioClips "fake" the size of m_AudioData when the stream is
                    // stored in a separate file. The array contains just an offset integer
                    // in that case, so pay attention to the bytes remaining in the buffer
                    // as well to avoid EOFExceptions.
                    long remaining = inr.remaining();
                    if (size > remaining && remaining == 4)
                    {
                        int offset = inr.readInt();
                    }
                    else
                    {
                        inr.position += size;
                    }

                    sdi.length = inr.position - sdi.offset;
                    return sdi;

                // always wrap char arrays so array() is available on the buffer, which
                // is required to convert them to Java strings in readObject()
                case "char":
                    byte[] raw = new byte[size];
                    inr.readBytes(ref raw, 0, size);
                    return new ByteBuffer(raw);// ByteBuffer.wrap(raw);

                // read a list of primitive objects
                default:
                    List<object> list = new List<object>(size);
                    for (int i = 0; i < size; i++)
                    {
                        list.Add(readPrimitive(inr, type));
                    }
                    return list;
            }
        }
    }

}
