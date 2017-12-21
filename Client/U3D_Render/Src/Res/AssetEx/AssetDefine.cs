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
    public static class Logger
    {
        public delegate void LogHanlder(string s);
        public static event LogHanlder LogEvent;
        public static void Log(string s)
        {
            if (LogEvent != null) LogEvent(s);
        }
    }

    //public class ByteBuffer
    //{
    //    private List<byte> data;
    //    public byte[] array() { return data.ToArray(); }
    //    public ByteBuffer(int cap)
    //    {
    //        data = new List<byte>(cap);
    //    }
    //}

    public class ByteBuffer
    {
        public byte[] data;
        public byte[] array() { return data; }
        int pos = 0;
        public ByteBuffer(int cap)
        {
            data = new byte[cap];
        }
        public ByteBuffer(byte[] indata)
        {
            data = indata;
        }
        public ByteBuffer(ByteBuffer indata, int offset, int len)
        {
            data = new byte[len];
            for (int i = 0; i < len; i++)
            {
                data[i] = indata.data[i + offset];
            }
        }
        public void clear() { data = null; }
        public int remaining() { return data.Length - pos; }
        public void rewind() { pos = 0; }
    }

    /**
     *
     * @author Nico Bergemann <barracuda415 at yahoo.de>
     */
    public class UnityGUID
    {

        long guidMost;
        long guidLeast;

        // throws IOException 
        public void read(DataReader inr)
        {
            // read GUID as big-endian
            ByteOrder order = inr.order;
            inr.order = ByteOrder.BIG_ENDIAN;
            guidMost = inr.readLong();
            guidLeast = inr.readLong();
            inr.order = order;

        }

        //throws IOException 
        public void write(DataWriter outw)
        {
            // write GUID as big-endian
            ByteOrder order = outw.order;
            outw.order = ByteOrder.BIG_ENDIAN;
            outw.writeLong(guidMost);
            outw.writeLong(guidLeast);
            outw.order = order;
        }

        //@Override
        //public String toString() {
        //    return UUID().toString();
        //}
    }

    public class UnityHash128
    {

        private byte[] hash = new byte[16];

        public UnityHash128(VersionInfo versionInfo)
        {

        }

        public byte[] hashvalue()
        {
            return hash;
        }

        //throws IOException 
        public void read(DataReader inr)
        {
            inr.readBytes(ref hash);
        }

        //throws IOException 
        public void write(DataWriter outw)
        {
            outw.writeBytes(hash);
        }
    }

    public abstract class UnityStruct
    {

        public VersionInfo versionInfo;

        public UnityStruct(VersionInfo versionInfo)
        {
            this.versionInfo = versionInfo;
        }
    }

    /**
     * Unity engine version string container.
     * 
     * @author Nico Bergemann <barracuda415 at yahoo.de>
     */
    //implements Comparable<UnityVersion>
    public class UnityVersion
    {
        public int major;
        public int minor;
        public int patch;
        public String build;
        public String raw;

        public UnityVersion(String version)
        {
            try
            {
                major = partFromString(version.Substring(0, 1));
                minor = partFromString(version.Substring(2, 1));
                patch = partFromString(version.Substring(4, 1));
                build = version.Substring(0,5);
            }
            catch (Exception e)
            {
                // invalid format, save raw string
                raw = version;
            }

        }

        private int partFromString(String part)
        {
            if (part == "x")
            {
                return -1;
            }
            else
            {
                int ret = -1;
                bool ok = int.TryParse(part, out ret);
                return ret;
            }
        }

        private String partToString(int part)
        {
            if (part == -1)
            {
                return "x";
            }
            else
            {
                return part.ToString();
            }
        }

        public bool isValid()
        {
            return raw == null;
        }




        //public override String toString() {
        //    if (raw != null) {
        //        return raw;
        //    } else {
        //        return String.format("%s.%s.%s%s", partToString(major),
        //                partToString(minor), partToString(patch), build);
        //    }
        //}

        //public override int hashCode() {
        //    if (raw != null) {
        //        return raw.hashCode();
        //    } else {
        //        int hash = 5;
        //        hash = 97 * hash + this.major;
        //        hash = 97 * hash + this.minor;
        //        hash = 97 * hash + this.patch;
        //        hash = 97 * hash + Objects.hashCode(this.build);
        //        return hash;
        //    }
        //}

        //public override bool Equals(object obj) {
        //    if (obj == null) {
        //        return false;
        //    }
        //    if (GetType() != obj.GetType()) {
        //        return false;
        //    }

        //    UnityVersion other = (UnityVersion) obj;
        //    if (raw != null) 
        //    {
        //        if (!Objects.equals(this.raw, other.raw)) {
        //            return false;
        //        }
        //    }
        //    else 
        //    {
        //        if (this.major != other.major) {
        //            return false;
        //        }
        //        if (this.minor != other.minor) {
        //            return false;
        //        }
        //        if (this.patch != other.patch) {
        //            return false;
        //        }
        //        if (!Objects.equals(this.build, other.build)) {
        //            return false;
        //        }
        //        if (!Objects.equals(this.raw, other.raw)) {
        //            return false;
        //        }
        //    }
        //    return true;
        //}

        //public override int compareTo(UnityVersion that) {
        //    if (!this.isValid() && !that.isValid()) {
        //        return this.raw.compareTo(that.raw);
        //    }

        //    if (this.major < that.major) {
        //        return 1;
        //    } else if (this.major > that.major) {
        //        return -1;
        //    } else {
        //        if (this.minor < that.minor) {
        //            return 1;
        //        } else if (this.minor > that.minor) {
        //            return -1;
        //        } else {
        //            if (this.patch < that.patch) {
        //                return 1;
        //            } else if (this.patch > that.patch) {
        //                return -1;
        //            } else {
        //                return this.build.compareTo(that.build);
        //            }
        //        }
        //    }
        //}

        //public bool lesserThan(UnityVersion that) 
        //{
        //    return this.compareTo(that) == 1;
        //}

        //public bool greaterThan(UnityVersion that)
        //{
        //    return this.compareTo(that) == -1;
        //}
    }


     /**
     *
     * @author Nico Bergemann <barracuda415 at yahoo.de>
     */
    public class UnityObject 
    {
    
        protected FieldNode obj;

        public UnityObject(FieldNode obj)
        {
            this.obj = obj;
        }
    
        public String getName()
        {
            return obj.getString("m_Name");
        }
    }



 
}