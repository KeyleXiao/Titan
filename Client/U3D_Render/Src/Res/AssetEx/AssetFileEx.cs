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
     * Reader for Unity asset files.
     * 
     * @author Nico Bergemann <barracuda415 at yahoo.de>
     */
    //extends FileHandler 
    public class AssetFileEx
    {
        private static int METADATA_PADDING = 4096;

        public String sourceFile;
        public Dictionary<string,bool> extractTypes=new Dictionary<string,bool>();

        // collection fields

        public Dictionary<string, List<ObjectData>> extractlist = new Dictionary<string, List<ObjectData>>();


        // struct fields
        public VersionInfo versionInfo = new VersionInfo();
        private AssetHeader header;
        private ObjectInfoTable objectInfoStruct;
        private TypeTree typeTreeStruct;
        private FileIdentifierTable externalsStruct;

        // data block fields
        private DataBlock headerBlock = new DataBlock();
        private DataBlock objectInfoBlock = new DataBlock();
        private DataBlock typeTreeBlock = new DataBlock();
        private DataBlock externalsBlock = new DataBlock();

        // misc fields
        private ByteBuffer audioBuffer;

        public AssetFileEx()
        {
            header = new AssetHeader(versionInfo);
            objectInfoStruct = new ObjectInfoTable(versionInfo);
            typeTreeStruct = new TypeTree(versionInfo);
            externalsStruct = new FileIdentifierTable(versionInfo);
        }

        //throws IOException 
        public bool load(string file)
        {
            String fileName = Path.GetFileName(file);
            String fileExt = Path.GetExtension(fileName);

            DataReader reader;

            // join split asset files before loading
            if (fileExt.StartsWith("split"))
            {
                Logger.Log("Found split asset file");

                fileName = Path.GetFileNameWithoutExtension(fileName);
                List<string> parts = new List<string>();
                int splitIndex = 0;

                // collect all files with .split0 to .splitN extension
                while (true)
                {
                    String splitName = String.Format("%s.split%d", fileName, splitIndex);

                    string part = Path.Combine(file, splitName);// file.resolveSibling(splitName);
                    if (!File.Exists(part))
                    {
                        break;
                    }

                    Logger.Log("Adding splinter {0}" + Path.GetFileName(part));

                    splitIndex++;
                    parts.Add(part);
                }

                // load all parts to one byte buffer
                //reader = DataReaders.forByteBuffer(ByteBufferUtils.load(parts));
                FileStream fs = new FileStream(parts[0], FileMode.Open, FileAccess.Read);
                reader = new DataReader(fs, Encoding.ASCII);

            }
            else
            {
                //DataReaders.forFile(file, READ);
                FileStream fs = new FileStream(file, FileMode.Open, FileAccess.Read);
                reader = new DataReader(fs);

            }

            // load audio buffer if existing        
            //loadResourceStream(file.resolveSibling(fileName + ".streamingResourceImage"));
            //loadResourceStream(file.resolveSibling(fileName + ".resS"));
            loadResourceStream(Path.Combine(file, fileName + ".streamingResourceImage"));
            loadResourceStream(Path.Combine(file, fileName + ".resS"));

            return load(reader);
        }

        //throws IOException 
        private void loadResourceStream(string streamFile)
        {
            if (File.Exists(streamFile))
            {
                Logger.Log("Found sound stream file {0}" + Path.GetFileName(streamFile));
                audioBuffer = new ByteBuffer(File.ReadAllBytes(streamFile));
                //audioBuffer = ByteBufferUtils.openReadOnly(streamFile);
            }
        }

        public static bool maintainstream = true;
        public Stream stream;

        // throws IOException 
        public bool load(DataReader inr)
        {
            loadHeader(inr);

            if (header.fileSize != inr.BaseStream.Length)
            {
                return false;
            }

            if (maintainstream)
            {
                stream = inr.BaseStream;
            }

            // read as little endian from now on
            inr.order = ByteOrder.LITTLE_ENDIAN;

            // older formats store the object data before the structure data
            if (header.getversion() < 9)
            {
                inr.BaseStream.Position = header.fileSize - header.metadataSize + 1;
            }

            loadMetadata(inr);
            loadObjects(inr);
            checkBlocks();
            extractObjects(inr);

            return true;
        }

  
        public delegate void AssetExProgressHandler(string s,int p);
        public static event AssetExProgressHandler ProgressEvent;

        private void extractObject(DataReader inr, ObjectData od, bool restore=false)
        {
            long ofs = header.dataOffset + od.offset;
            inr.position = ( ofs);

            ObjectSerializer serializer = new ObjectSerializer();
            serializer.setSoundData(audioBuffer);
            serializer.deserialize(inr,od);

            if (!restore)
            {
                od.buffer = null;
            }
        }

        int lastprogress = -1;
        private void extractObjects(DataReader inr)
        {
            bool bAll = extractTypes.ContainsKey("All");
            int total = objectInfoStruct.infoMap.Count;
            int i = 0;
            foreach(KeyValuePair<long,ObjectData> pair in objectInfoStruct.infoMap)
            {
                ObjectData od = pair.Value;
                string classname=od.classname;
                bool bBoundle=classname == "AssetBundle" || classname == "ResourceManager";
                if (bAll || bBoundle || extractTypes.ContainsKey(classname) )
                {
                    bool restore = bAll || bBoundle ? false : extractTypes[classname];
                    loadObjectType(od);                    
                    extractObject(inr, od,restore);
                }
                if (extractlist.ContainsKey(classname) == false)
                {
                    extractlist[classname] = new List<ObjectData>();
                }
                extractlist[classname].Add(od);

                int progress = i * 100 / total;
                if (lastprogress != progress)
                {
                    lastprogress = progress;
                    if (ProgressEvent != null)
                    {
                        ProgressEvent("extracting: " + sourceFile + ", " + i + "/" + total, progress);
                    }
                }
                i++;
            }
            GC.Collect();
        }

        //throws IOException 
        public void loadExternals(Dictionary<string, AssetFileEx> loadedAssets)
        {
            foreach (FileIdentifier external in externalsStruct.fileIDs)
            {
                String filePath = external.filePath;

                if (filePath == null || filePath.Length <= 0)
                {
                    continue;
                }

                filePath = filePath.Replace("library/", "resources/");

                string folder = Path.GetDirectoryName(sourceFile);
                string refFile = Path.Combine(folder, filePath);// sourceFile.resolveSibling(filePath);
                string refName = Path.GetFileNameWithoutExtension(refFile);
                if (File.Exists(refFile) && loadedAssets.ContainsKey(refName) == false)
                {
                    Logger.Log("Loading dependency {0} for {1}" +
                            new object[] { refFile, sourceFile });
                    AssetFileEx childAsset = new AssetFileEx();
                    bool ret=childAsset.load(refFile);
                    if (ret)
                    {
                        loadedAssets[refName] = childAsset;
                        childAsset.loadExternals(loadedAssets);
                        external.assetFile = childAsset;
                    }
                }
            }
        }

        //throws IOException 
        private void loadHeader(DataReader inr)
        {
            headerBlock.markBegin(inr.position);
            header.read(inr);
            headerBlock.markEnd(inr.position);
            //Logger.Log("headerBlock: {0}" + headerBlock);
        }

        // throws IOException 
        private void loadMetadata(DataReader inr)
        {
            inr.order = versionInfo.order();

            // read structure data
            typeTreeBlock.markBegin(inr.position);
            typeTreeStruct.read(inr);
            typeTreeBlock.markEnd(inr.position);
            //Logger.Log("typeTreeBlock: {0}" + typeTreeBlock);

            objectInfoBlock.markBegin(inr.position);
            objectInfoStruct.read(inr);
            objectInfoBlock.markEnd(inr.position);
            //Logger.Log("objectInfoBlock: {0}" + objectInfoBlock);

            // unknown block for Unity 5
            if (header.getversion() > 13)
            {
                inr.align(4);
                int num = inr.readInt();
                if (num != 0 && header.getversion() >= 15)
                {
                    // sorry, no info on that block => cant read externals. num -> num-9 worked for my experimental file, but I dont know about others
                    Logger.Log("num=" + num);
                    return;
                }
                for (int i = 0; i < num; i++)
                {
                    inr.readInt();
                    inr.readInt();
                    inr.readInt();
                }
            }

            externalsBlock.markBegin(inr.position);
            externalsStruct.read(inr);
            externalsBlock.markEnd(inr.position);
            //Logger.Log("externalsBlock: {0}" + externalsBlock);

            byte[] userInformation = new byte[header.dataOffset - inr.position];
            inr.readBytes(ref userInformation);

        }

        private void loadObjectType(ObjectData data)
        {

            TypeNode typeNode = null;
            EmbeddedTypeInfo embeddedtype = null;
            if (typeTreeStruct.embeddedTypeMap.ContainsKey(data.typeID))
            {
                embeddedtype = typeTreeStruct.embeddedTypeMap[data.typeID];
                if (embeddedtype != null)
                {
                    typeNode = embeddedtype.typeTree;
                }
            }

            // get type from database if the embedded one is missing
            if (typeNode == null)
            {
                typeNode = TypeTreeUtils.getTypeNode(data.unityClass(),
                        versionInfo.unityRevision, false);
            }

            data.typeTree = typeNode;

            //// Add typeless objects to an internal list. They can't be
            //// (de)serialized, but can still be written to the file.
            //if (typeNode == null)
            //{
            //    // log warning if it's not a MonoBehaviour
            //    if (info.classID != 114)
            //    {
            //        //Logger.Log("{0} has no type information!" + data.ToString());
            //    }
            //    objectListBroken.Add(data);
            //}
            //else
            //{
            //    objectList.Add(data);
            //}
        }

        //throws IOException 
        private void loadObjects(DataReader inr)
        {
            //long ofsMin = long.MaxValue;
            //long ofsMax = long.MinValue;

            //objectList.Capacity = objectInfoStruct.infoMap.Count;
            //foreach (KeyValuePair<long, ObjectInfo> infoEntry in objectInfoStruct.infoMap)
            //{
            //    ObjectInfo info = infoEntry.Value;
            //    long pathID = infoEntry.Key;

            //    long ofs = header.dataOffset + info.offset;
            //    ofsMin = Math.Min(ofsMin, ofs);
            //    ofsMax = Math.Max(ofsMax, ofs + info.length);

            //    ObjectData data = new ObjectData(pathID, versionInfo);
            //    data.info = info;
            //    objectList.Add(data);
            //}

            //objectDataBlock.markBegin(ofsMin);
            //objectDataBlock.markEnd(ofsMax);
            //Logger.Log("objectDataBlock: {0}" + objectDataBlock);
        }

        // throws IOException 
        public void save(DataWriter outw)
        {
            saveHeader(outw);

            // write as little endian from now on
            outw.order = ByteOrder.LITTLE_ENDIAN;

            // older formats store the object data before the structure data
            if (header.getversion() < 9)
            {
                header.dataOffset = 0;

                saveObjects(outw);
                outw.writeUnsignedByte(0);

                saveMetadata(outw);
                outw.writeUnsignedByte(0);
            }
            else
            {
                saveMetadata(outw);

                // original files have a minimum padding of 4096 bytes after the
                // metadata
                if (outw.position < METADATA_PADDING)
                {
                    outw.align(METADATA_PADDING);
                }

                outw.align(16);
                header.dataOffset = outw.position;

                saveObjects(outw);

                // write updated path table
                outw.position = ( objectInfoBlock.offset);
                objectInfoStruct.write(outw);
                //outw.writeStruct(objectInfoStruct);
            }

            // update header
            header.fileSize = outw.size();

            // FIXME: the metadata size is slightly off in comparison to original files
            int metadataOffset = header.getversion() < 9 ? 2 : 1;

            header.metadataSize = typeTreeBlock.length
                    + objectInfoBlock.length
                    + externalsBlock.length
                    + metadataOffset;

            // write updated header
            outw.order = ByteOrder.BIG_ENDIAN;
            outw.position = (headerBlock.offset);
            header.write(outw);

            checkBlocks();
        }

        // throws IOException
        private void saveHeader(DataWriter outw)
        {
            headerBlock.markBegin(outw.position);
            //outw.writeStruct(header);
            header.write(outw);
            headerBlock.markEnd(outw.position);
            Logger.Log("headerBlock: {0}" + headerBlock);
        }

        //throws IOException 
        private void saveMetadata(DataWriter outw)
        {
            outw.order = versionInfo.order();

            typeTreeBlock.markBegin(outw.position);
            typeTreeStruct.write(outw);
            typeTreeBlock.markEnd(outw.position);
            Logger.Log("typeTreeBlock: {0}" + typeTreeBlock);

            objectInfoBlock.markBegin(outw.position);
            objectInfoStruct.write(outw);
            objectInfoBlock.markEnd(outw.position);
            Logger.Log("objectInfoBlock: {0}" + objectInfoBlock);

            externalsBlock.markBegin(outw.position);
            externalsStruct.write(outw);
            externalsBlock.markEnd(outw.position);
            Logger.Log("externalsBlock: {0}" + externalsBlock);
        }

        // throws IOException 
        private void saveObjects(DataWriter outw)
        {
            long ofsMin = long.MaxValue;
            long ofsMax = long.MinValue;

            // merge object lists
            //objectList.AddRange(objectListBroken);

            foreach(KeyValuePair<long,ObjectData> pair in objectInfoStruct.infoMap)
            {
                ObjectData data = pair.Value;
                ByteBuffer bb = data.buffer;
                bb.rewind();

                outw.align(8);

                ofsMin = Math.Min(ofsMin, outw.position);
                ofsMax = Math.Max(ofsMax, outw.position + bb.remaining());

                data.offset = outw.position - header.dataOffset;
                data.length = bb.remaining();

                outw.writeBuffer(bb);
            }

            // separate object lists
            //objectList.removeAll(objectListBroken);
            //foreach (ObjectData od in objectListBroken)
            //{
            //    objectList.Remove(od);
            //}

            //objectDataBlock.markBegin(ofsMin);
            //objectDataBlock.markEnd(ofsMax);
            //Logger.Log("objectDataBlock: {0}" + objectDataBlock);
        }

        private void checkBlocks()
        {
            // sanity check for the data blocks
            //assert !headerBlock.isIntersecting(typeTreeBlock);
            //assert !headerBlock.isIntersecting(objectInfoBlock);
            //assert !headerBlock.isIntersecting(externalsBlock);
            //assert !headerBlock.isIntersecting(objectDataBlock);

            //assert !typeTreeBlock.isIntersecting(objectInfoBlock);
            //assert !typeTreeBlock.isIntersecting(externalsBlock);
            //assert !typeTreeBlock.isIntersecting(objectDataBlock);

            //assert !objectInfoBlock.isIntersecting(externalsBlock);
            //assert !objectInfoBlock.isIntersecting(objectDataBlock);

            //assert !objectDataBlock.isIntersecting(externalsBlock);
        }





        public int typeTreeAttributes()
        {
            return typeTreeStruct.attributes;
        }

        public Dictionary<int, EmbeddedTypeInfo> embeddedTypeMap
        {
            get
            {
                return typeTreeStruct.embeddedTypeMap;
            }
        }

        public Dictionary<long,ObjectData> objects
        {
            get
            {
                return objectInfoStruct.infoMap;
            }
        }

        public bool isStandalone()
        {
            return !typeTreeStruct.embedded;
        }

        public void setStandalone()
        {
            typeTreeStruct.embedded = true;
        }
    }
}