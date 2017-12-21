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

    public class DataReader
    {
        BinaryReader br;
        public DataReader(Stream s)
        {
            br = new BinaryReader(s);
        }

        public DataReader(Stream s, Encoding e)
        {
            br = new BinaryReader(s, e);
        }

        public ByteOrder order = ByteOrder.BIG_ENDIAN;

        public long position
        {
            get { return br.BaseStream.Position; }
            set { br.BaseStream.Position = value; }
        }

        int _align = 4;
        public void align(int v) 
        {
            _align = v;
            long pos=br.BaseStream.Position;
            long b = pos % _align;
            if (b > 0) pos += _align - b;
            br.BaseStream.Position = pos;
        }

        public string readStringNull()
        {
            return readStringNull(1024);
        }

        public string readStringNull(int max)
        {
            string ret = "";
            for (int i = 0; i < max; i++)
            {
                byte b = br.ReadByte();
                if (b == 0)
                {
                    break;
                }
                char c = (char)b;
                ret += c;
            }
            return ret;
        }

        public object readByType(Type t, int readsize = -1)
        {
            int stride = Marshal.SizeOf(t);
            if (readsize < 0) readsize = stride;
            byte[] src = br.ReadBytes(readsize);
            byte[] data = null;
            if (readsize <= stride)
            {
                data = new byte[stride];
                for (int i = 0; i < readsize; i++)
                {
                    data[i] = src[i];
                }
            }
            else
            {
                data = src;
            }
            byte[] dataex = new byte[stride];
            if (order == ByteOrder.BIG_ENDIAN)
            {
                for (int i = 0; i < stride; i++)
                {
                    dataex[i] = data[stride - 1 - i];
                }
            }
            else
            {
                dataex = data;
            }
            IntPtr p = Marshal.AllocHGlobal(stride);
            Marshal.Copy(dataex, 0, p, stride);
            object ret = Marshal.PtrToStructure(p, t);
            Marshal.FreeHGlobal(p);


            return ret;
        }

        public short readShort()
        {
            return (short)readByType(typeof(short));
        }

        public ushort readUnsignedShort()
        {
            return (ushort)readByType(typeof(ushort));
        }

        public float readFloat()
        {
            return (float)readByType(typeof(float));
        }

        public double readDouble()
        {
            return (double)readByType(typeof(double));
        }

        public ulong readUnsignedLong()
        {
            return (ulong)readByType(typeof(ulong));
        }

        public void readBuffer(ByteBuffer buf)
        {
            buf.data = br.ReadBytes(buf.data.Length);
        }

        public long readLong()
        {
            return (long)readByType(typeof(long));
        }

        public int readInt()
        {
            return (int)readByType(typeof(int));
        }

        public uint readUnsignedInt()
        {
            return (uint)readByType(typeof(uint));
        }

        public byte readUnsignedByte()
        {
            return br.ReadByte();
        }

        public sbyte readByte()
        {
            return br.ReadSByte();
        }

        public int readByte3()
        {
            return (int)readByType(typeof(int),3);
        }

        public void readBytes(ref byte[] data)
        {
            data = br.ReadBytes(data.Length);
        }

        public void readBytes(ref byte[] data, int start, int len)
        {
            data = br.ReadBytes(len);
        }

        public bool readBoolean()
        {
            return br.ReadByte() != 0;
        }

        public void Close()
        {
            br.Close();
        }

        public long size()
        {
            return br.BaseStream.Length;
        }

        public Stream BaseStream
        {
            get
            {
                return br.BaseStream;
            }
        }

        public long remaining()
        {
            return br.BaseStream.Length - br.BaseStream.Position;
        }
    }

    public class DataWriter
    {
        BinaryWriter bw;
        public ByteOrder order = ByteOrder.BIG_ENDIAN;
        public DataWriter(Stream s)
        {
            bw = new BinaryWriter(s);
        }

        public DataWriter(Stream s, Encoding e)
        {
            bw = new BinaryWriter(s, e);
        }

        public long position
        {
            get { return bw.BaseStream.Position; }
            set { bw.BaseStream.Position = value; }
        }

        int _align = 4;
        public void align(int v)
        {
            _align = v;
            long pos = bw.BaseStream.Position;
            long b = pos % _align;
            if (b > 0) pos += 4 - _align;
            bw.BaseStream.Position = pos;
        }

        public void writeStringNull(string str)
        {
            byte[] dest = Encoding.Convert(Encoding.Unicode, Encoding.ASCII, Encoding.Unicode.GetBytes(str));
            bw.Write(dest);
        }

        public void writeByType(object o)
        {
            int stride = Marshal.SizeOf(o);
            byte[] data = new byte[stride];
            byte[] dataex = new byte[stride];
            IntPtr p = Marshal.AllocHGlobal(stride);
            Marshal.StructureToPtr(o, p, false);
            Marshal.Copy(p, data, 0, stride);
            Marshal.FreeHGlobal(p);
            if (order == ByteOrder.BIG_ENDIAN)
            {
                for (int i = 0; i < stride; i++)
                {
                    dataex[i] = data[stride - 1 - i];
                }
            }
            else
            {
                dataex = data;
            }
            bw.Write(dataex, 0, stride);
        }

        public void writeLong(long val)
        {
            writeByType(val);
        }

        public void writeInt(int val)
        {
            writeByType(val);
        }

        public void writeShort(short val)
        {
            writeByType(val);
        }

        public void writeUnsignedInt(uint val)
        {
            writeByType(val);
        }

        public void writeBytes(byte[] data)
        {
            bw.Write(data);
        }

        public void writeUnsignedByte(sbyte val)
        {
            bw.Write(val);
        }

        public void writeByte(byte val)
        {
            bw.Write(val);
        }

        public void writeBuffer(ByteBuffer bb)
        {
            bw.Write(bb.data);
        }

        public long size()
        {
            return bw.BaseStream.Length;
        }

        public long remaining()
        {
            return bw.BaseStream.Length - bw.BaseStream.Position;
        }

    }

}