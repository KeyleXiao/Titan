using System.Collections;
using System.Collections.Generic;
using System.Threading;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.IO;
using System;


namespace AssetEx
{
    public enum ByteOrder
    {
        LITTLE_ENDIAN = 0,
        BIG_ENDIAN = 1,
    }

    /**
 *
 * @author Nico Bergemann <barracuda415 at yahoo.de>
 * @unity SerializedFile::Type
 */
    public class EmbeddedTypeInfo
    {
        public int classID;
        public UnityHash128 scriptID;
        public UnityHash128 oldTypeHash;
        public TypeNode typeTree;
    }


    /**
     *
     * @author Nico Bergemann <barracuda415 at yahoo.de>
     */
    public class VersionInfo
    {
        public UnityVersion unityVersion;
        public UnityVersion unityRevision;

        // 5 = 1.2 - 2.0
        // 6 = 2.1 - 2.6
        // 7 = 3.0 (?)
        // 8 = 3.1 - 3.4
        // 9 = 3.5 - 4.5
        // 11 = pre-5.0
        // 12 = pre-5.0
        // 13 = pre-5.0
        // 14 = 5.0
        // 15 = 5.0 (p3 and newer)
        public int assetVersion;

        public ByteOrder order()
        {
            // older formats use big endian
            return assetVersion > 5 ? ByteOrder.LITTLE_ENDIAN : ByteOrder.BIG_ENDIAN;
        }
    }

 }
