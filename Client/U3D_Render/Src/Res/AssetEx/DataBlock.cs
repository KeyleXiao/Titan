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
    public class DataBlock
    {

        public long offset;
        public long length;

        public long endOffset()
        {
            return offset + length;
        }

        public bool isIntersecting(DataBlock that)
        {
            return this.endOffset() > that.offset && that.endOffset() > this.offset;
        }

        public bool isInside(DataBlock that)
        {
            return this.offset >= that.offset && this.endOffset() <= that.endOffset();
        }

        public void markBegin(long pos)
        {
            offset = pos;
        }

        public void markEnd(long pos)
        {
            this.length = pos - offset;
        }

        //@Override
        //public String toString() {
        //    return DataBlock.this.offset() + " - " + endOffset() + " (" + DataBlock.this.length() + ")";
        //}
    }
}
