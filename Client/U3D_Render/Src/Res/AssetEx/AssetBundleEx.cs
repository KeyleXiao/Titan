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

    public class AssetBundleEntry : object
    { }



    public class AssetBundleEntryInfo
    {
        public String name;
        public uint offset;
        public uint size;
    
        public void read(DataReader inr) 
        {
            name = inr.readStringNull();
            offset = inr.readUnsignedInt();
            size = inr.readUnsignedInt();
        }

        public void write(DataWriter outw) 
        {
            outw.writeStringNull(name);
            outw.writeUnsignedInt(offset);
            outw.writeUnsignedInt(size);
        }

    }


    /**
     * Structure for Unity asset bundles.
     * 
     * @author Nico Bergemann <barracuda415 at yahoo.de>
     * @unity UnityWebStreamHeader
     */
    //implements Struct
    public class AssetBundleEx 
    {
        public String name;
        public static String SIGNATURE_WEB = "UnityWeb";
        public static String SIGNATURE_RAW = "UnityRaw";

        // UnityWeb or UnityRaw
        public String signature;

        // file version
        // 3 in Unity 3.5 and 4
        // 2 in Unity 2.6 to 3.4
        // 1 in Unity 1 to 2.5
        public int streamVersion;

        // player version string
        // 2.x.x for Unity 2
        // 3.x.x for Unity 3/4
        public UnityVersion unityVersion;

        // engine version string
        public UnityVersion unityRevision;

        // minimum number of bytes to read for streamed bundles,
        // equal to completeFileSize for normal bundles
        public uint minimumStreamedBytes;

        // offset to the bundle data or size of the bundle header
        public int headerSize;

        // equal to 1 if it's a streamed bundle, number of levelX + mainData assets
        // otherwise
        public int numberOfLevelsToDownload;

        // list of compressed and uncompressed offsets
        public Dictionary<uint, uint> levelByteEnd = new Dictionary<uint, uint>();

        // equal to file size, sometimes equal to uncompressed data size without the header
        public uint completeFileSize;

        // offset to the first asset file within the data area? equals compressed
        // file size if completeFileSize contains the uncompressed data size
        public uint dataHeaderSize;


        //throws IOException
        private void readheader(DataReader inr)
        {
            signature = inr.readStringNull();
            streamVersion = inr.readInt();
            string s1 = inr.readStringNull();
            unityVersion = new UnityVersion(s1);
            string s2 = inr.readStringNull();
            unityRevision = new UnityVersion(s2);
            minimumStreamedBytes = inr.readUnsignedInt();
            headerSize = inr.readInt();

            numberOfLevelsToDownload = inr.readInt();
            int numberOfLevels = inr.readInt();

            //Assert(numberOfLevelsToDownload == numberOfLevels || numberOfLevelsToDownload == 1);

            for (int i = 0; i < numberOfLevels; i++)
            {
                uint PackSize = inr.readUnsignedInt();
                uint UncompressedSize = inr.readUnsignedInt();
                levelByteEnd[PackSize] = UncompressedSize;
            }

            if (streamVersion >= 2)
            {
                completeFileSize = inr.readUnsignedInt();
                //Assert(minimumStreamedBytes <= completeFileSize);
            }

            if (streamVersion >= 3)
            {
                dataHeaderSize = inr.readUnsignedInt();
            }

            inr.readByte();
        }

        //throws IOException 
        private void writeheader(DataWriter outw)
        {
            outw.writeStringNull(signature);
            outw.writeInt(streamVersion);
            outw.writeStringNull(unityVersion.ToString());
            outw.writeStringNull(unityRevision.ToString());
            outw.writeUnsignedInt(minimumStreamedBytes);
            outw.writeInt(headerSize);

            outw.writeInt(numberOfLevelsToDownload);
            outw.writeInt(levelByteEnd.Count);

            foreach (KeyValuePair<uint, uint> offset in levelByteEnd)
            {
                outw.writeUnsignedInt(offset.Key);
                outw.writeUnsignedInt(offset.Value);
            }

            if (streamVersion >= 2)
            {
                outw.writeUnsignedInt(completeFileSize);
            }

            if (streamVersion >= 3)
            {
                outw.writeUnsignedInt(dataHeaderSize);
            }

            outw.writeUnsignedByte(0);
        }

        public bool hasValidSignature()
        {
            return signature == SIGNATURE_WEB || signature == SIGNATURE_RAW;
        }

        public void compressed(bool compressed)
        {
            signature = compressed ? SIGNATURE_WEB : SIGNATURE_RAW;
        }

        public bool compressed()
        {
            return signature == SIGNATURE_WEB;
        }



        //public int streamVersion() {
        //    return streamVersion;
        //}

        //public void streamVersion(int format) {
        //    this.streamVersion = format;
        //}


        public int numberOfLevels()
        {
            return levelByteEnd.Count;
        }

        private List<AssetBundleEntryInfo> entryInfos = new List<AssetBundleEntryInfo>();
        private DataReader inr;

        public Dictionary<string, AssetFileEx> afs = new Dictionary<string,AssetFileEx>();

        List<object> m_PreloadTable;
        Dictionary<object, object> m_Container;
        AssetInfo m_MainAsset;
        Dictionary<long, List<object>> m_DependentAssets;

        //throws IOException 
        private MemoryStream getDataInputStream(long offset, long size)
        {
            MemoryStream s = null;

            // use LZMA stream if the bundle is compressed
            if (compressed())
            {
                Stream inStream = inr.BaseStream;
                inStream.Position = headerSize + offset;
                MemoryStream outStream = new MemoryStream();

                byte[] properties = new byte[5];
                if (inStream.Read(properties, 0, 5) != 5)
                    throw (new Exception("input .lzma is too short"));
                SevenZip.Compression.LZMA.Decoder decoder = new SevenZip.Compression.LZMA.Decoder();
                decoder.SetDecoderProperties(properties);
                long outSize = 0;
                for (int i = 0; i < 8; i++)
                {
                    int v = inStream.ReadByte();
                    if (v < 0)
                        throw (new Exception("Can't Read 1"));
                    outSize |= ((long)(byte)v) << (8 * i);
                }
                long compressedSize = inStream.Length - inStream.Position;
                decoder.Code(inStream, outStream, compressedSize, outSize, null);
                outStream.Position = 0;

                s = outStream;
            }
            else
            {
                inr.BaseStream.Position = headerSize + offset;
                byte[] outbuf=new byte[inr.BaseStream.Length-inr.BaseStream.Position];
                inr.BaseStream.Read(outbuf, 0, outbuf.Length);
                MemoryStream outStream = new MemoryStream(outbuf);
                s = outStream;
            }

            return s;
        }


        public void ReadPackage(Stream inputs,string[] filter)
        {
            //DataReaders.forFile(file, READ
            inr = new DataReader(inputs, Encoding.ASCII);
            readheader(inr);

            // check signature
            if (!hasValidSignature())
            {
                throw new Exception("Invalid signature");
            }

            if (dataHeaderSize == 0)
            {
                // old stream versions don't store the data header size, so use a large
                // fixed number instead
                dataHeaderSize = 4096;
            }

            MemoryStream unzips = getDataInputStream(0, dataHeaderSize);
            //DataReader inData = DataReaders.forInputStream(is);
            //int files = inData.readInt();

            DataReader inData = new DataReader(unzips, Encoding.ASCII);
            int files = inData.readInt();

            for (int i = 0; i < files; i++)
            {
                AssetBundleEntryInfo info = new AssetBundleEntryInfo();
                info.read(inData);
                entryInfos.Add(info);
            }

            //// sort entries by offset so that they're in the order in which they
            //// appear in the file, which is convenient for compressed bundles
            //Collections.sort(entryInfos, new EntryComparator());

            //for (AssetBundleEntryInfo entryInfo : entryInfos) {
            //    entries.add(new AssetBundleInternalEntry(this, entryInfo));
            //}

            foreach (AssetBundleEntryInfo info in entryInfos)
            {
                MemoryStream newms = new MemoryStream(unzips.GetBuffer(), (int)info.offset, (int)info.size);
                extractFile(newms, info.name, filter);
                if (!AssetFileEx.maintainstream)
                {
                    newms.Close();
                    newms.Dispose();
                    newms = null;
                }
            }

            if (unzips != null)
            {
                unzips.Close();
                unzips.Dispose();
                unzips = null;
            }

            if (afs.Count > 0)
            {
                foreach(KeyValuePair<string,AssetFileEx> pair in afs)
                {
                    AssetFileEx af = pair.Value;
                    if (af != null)
                    {
                        ObjectData abod = null;
                        if (af.extractlist.ContainsKey("AssetBundle"))
                        {
                            abod = af.extractlist["AssetBundle"][0];
                            if (abod.instance != null)
                            {
                                m_MainAsset = (AssetInfo)abod.instance.getObject("m_MainAsset");
                                m_PreloadTable = (List<object>)abod.instance.getObject("m_PreloadTable");
                                m_Container = (Dictionary<object, object>)abod.instance.getObject("m_Container");
                            }
                            break;
                        }
                    }

                }
  
            }

            AttachContainData();

            Close();
        }

        private void extractFiles(string folder, string fileext, string[] filter)
        {
            string[] assetfilepath2s = Directory.GetFiles(folder, fileext);
            foreach (string assetfile in assetfilepath2s)
            {
                if (!afs.ContainsKey(assetfile))
                {
                    FileStream fs = new FileStream(assetfile, FileMode.Open, FileAccess.Read);
                    extractFile(fs, assetfile, filter);
                    if (!AssetFileEx.maintainstream)
                    {
                        fs.Close();
                        fs.Dispose();
                        fs = null;
                    }
                }
            }
        }

        public void ReadFolder(string folder, string[] filter)
        {
            extractFiles(folder + "\\Resources", "*.", filter);
            extractFiles(folder, "resources.assets", filter);
            extractFiles(folder, "maindata.", filter);
            extractFiles(folder, "*.assets", filter);
            extractFiles(folder, "*.", filter);

            if (afs.Count > 0)
            {
                foreach (KeyValuePair<string, AssetFileEx> pair in afs)
                {
                    AssetFileEx af = pair.Value;
                    if (af != null)
                    {
                        ObjectData abod = null;
                        if (af.extractlist.ContainsKey("ResourceManager"))
                        {
                            abod = af.extractlist["ResourceManager"][0];
                            if (abod.instance != null)
                            {
                                m_Container = (Dictionary<object, object>)abod.instance.getObject("m_Container");
                                m_DependentAssets = GenerateDependentMap((List<object>)abod.instance.getObject("m_DependentAssets")); 
                            }
                            break;
                        }
                    }
                }
            }

            AttachContainData();
        }

        private Dictionary<long, List<object>> GenerateDependentMap(List<object> list)
        {
            Dictionary<long, List<object>> ret = new Dictionary<long, List<object>>();
            if (list != null)
            {
                foreach (ResourceManager_Dependency dependency in list)
                {
                    ret[dependency.m_Object.GetKey()] = (List<object>)dependency.m_Dependencies;
                }
            }

            return ret;
        }

        string[] afnames = null;
  
        ObjectData GetObjectDataFromPtr(object ptr)
        {
            PPtrObject pobj=new PPtrObject();
            pobj.m_FileID = -1;
            pobj.m_PathID = -1;
            if (ptr.GetType() == typeof(PPtrObject))
            {
                pobj = (PPtrObject)ptr;
            }
            else if (ptr.GetType() == typeof(AssetInfo))
            {
                AssetInfo ai = (AssetInfo)ptr;
                pobj = ai.asset;
            }

            if (pobj.m_FileID >= 0 && pobj.m_FileID < afnames.Length)
            {
                string assetfilename = afnames[pobj.m_FileID];
                AssetFileEx af = afs[assetfilename];
                Dictionary<long, ObjectData> ods = af.objects;
                if (ods.ContainsKey(pobj.m_PathID))
                {
                    ObjectData od = ods[pobj.m_PathID];
                    return od;
                }
            }
            return null;
        }

        void AttachPreLoadData(int start, int size, ObjectData srcod)
        {
            if (m_PreloadTable == null) return;

            for (int i = start; i < start + size; i++)
            {
                if (i >= 0 && i < m_PreloadTable.Count)
                {
                    PPtrObject ptr = (PPtrObject)m_PreloadTable[i];
                    ObjectData dependentod = GetObjectDataFromPtr(ptr);
                    if (dependentod != null)
                    {
                        if (dependentod.path == null || dependentod.path.Length <= 0)
                        {
                            dependentod.path = srcod.path;
                        }
                    }
                }
            }
        }

        void AttachDependencyData(PPtrObject ptr, ObjectData srcod)
        {
            if (m_DependentAssets == null) return;

            long key = ptr.GetKey();
            if (m_DependentAssets.ContainsKey(key))
            {
                List<object> dependentlist = m_DependentAssets[key];
                foreach (PPtrObject dependentobj in dependentlist)
                {
                    ObjectData dependentod = GetObjectDataFromPtr(dependentobj);
                    if (dependentod != null)
                    {
                        if (dependentod.path == null || dependentod.path.Length <= 0)
                        {
                            dependentod.path = srcod.path;
                            AttachDependencyData(dependentobj, dependentod);
                        }
                    }
                }
            }
        }

        private void AttachContainData()
        {
            afnames = new string[afs.Count];
            afs.Keys.CopyTo(afnames, 0);

            if (m_Container == null) return;

            foreach (KeyValuePair<object, object> pair in m_Container)
            {
                string path = (string)pair.Key;
                object obj = pair.Value;

                ObjectData od = GetObjectDataFromPtr(obj);
                if (od != null)
                {
                    od.path = path;

                    if (obj.GetType() == typeof(PPtrObject))
                    {
                        AttachDependencyData((PPtrObject)obj, od);
                    }
                    if (obj.GetType() == typeof(AssetInfo))
                    {
                        AssetInfo ai = (AssetInfo)obj;
                        AttachPreLoadData(ai.preloadIndex, ai.preloadSize, od);
                    }

                }
            }

            afnames = null;
        }

        //private void TravellObjectData(ObjectData srcod)
        //{
        //    if (srcod.instance != null)
        //    {
        //        TravellObjectDataNode(srcod, srcod.instance);
        //    }
        //}

        //private void TravellObjectDataValue(ObjectData srcod, object val)
        //{
        //    if (val != null)
        //    {
        //        if (val is PPtrObject || val is AssetInfo)
        //        {
        //            ObjectData od = GetObjectDataFromPtr(val);
        //            if (od != null)
        //            {
        //                if (od.path == null || od.path.Length <= 0)
        //                {
        //                    od.path = srcod.path;
        //                }
        //            }
        //        }
        //    }
        //}

        //private void TravellObjectDataNode(ObjectData srcod, FieldNode node)
        //{
        //    object val = node.value;
        //    TravellObjectDataValue(srcod, val);

        //    Dictionary<String, FieldNode> childnodes = node.getChilds();
        //    if (childnodes == null) return;
        //    foreach (KeyValuePair<string, FieldNode> keypair in childnodes)
        //    {
        //        FieldNode childnode = keypair.Value;
        //        TravellObjectDataNode(srcod, childnode);
        //    }
        //}

        private void extractFile(Stream news, string path,string[] filter)
        {
            DataReader inr = new DataReader(news);

            AssetFileEx af = new AssetFileEx();
            af.sourceFile = path;
            if (filter != null)
            {
                foreach (string type in filter)
                {
                    af.extractTypes[type] = false;
                }
            }
            bool ret=af.load(inr);
            if (ret)
            {
                afs[af.sourceFile] = af;
                af.loadExternals(afs);
            }
        }

        public void Close()
        {
            inr = null;
        }

  
    }
}