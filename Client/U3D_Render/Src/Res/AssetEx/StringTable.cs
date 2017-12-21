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
    public class StringTable
    {

        private static int FLAG_INTERNAL = 1 << 31;

        public static StringTable stInt;

        private Dictionary<int, String> strings = new Dictionary<int, String>();

        public void loadStrings(byte[] data, bool binternal)
        {
            string wholestring = "";
            for (int i = 0; i < data.Length; i++)
            {
                wholestring += (char)data[i];
            }
            string[] ss = wholestring.Split('\0');
            for (int i = 0; i < ss.Length; i++)
            { }

            for (int i = 0, n = 0; i < data.Length; i++)
            {
                if (data[i] == 0)
                {
                    char[] dataex = new char[data.Length];
                    for (int j = 0; j < dataex.Length; j++)
                    {
                        dataex[j] = (char)data[j];
                    }
                    String str = new String(dataex, n, i - n);//, Encoding.ASCII

                    if (binternal)
                    {
                        n |= FLAG_INTERNAL;
                    }

                    strings[n]=str;

                    n = i + 1;
                }
            }
        }

        public void loadStrings(byte[] data)
        {
            loadStrings(data, false);
        }

        public String getString(int offset)
        {
            if (strings.ContainsKey(offset))
                return strings[offset];
            else
                return null;
        }
    }
}
