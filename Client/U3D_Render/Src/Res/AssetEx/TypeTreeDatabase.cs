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
    public class TypeTreeDatabaseEntry 
    {
    
        public UnityClass unityClass;
        public UnityVersion unityVersion;
        public TypeNode typeNode;

        public TypeTreeDatabaseEntry(UnityClass unityClass, UnityVersion unityVersion, TypeNode typeNode) 
        {
            this.unityClass = unityClass;
            this.unityVersion = unityVersion;
            this.typeNode = typeNode;
        }


        //@Override
        //public int hashCode() {
        //    int hash = 7;
        //    hash = 37 * hash + Objects.hashCode(this.unityClass);
        //    hash = 37 * hash + Objects.hashCode(this.unityVersion);
        //    hash = 37 * hash + Objects.hashCode(this.typeNode);
        //    return hash;
        //}

        //@Override
        //public boolean equals(Object obj) {
        //    if (obj == null) {
        //        return false;
        //    }
        //    if (getClass() != obj.getClass()) {
        //        return false;
        //    }
        //    final TypeTreeDatabaseEntry other = (TypeTreeDatabaseEntry) obj;
        //    if (!Objects.equals(this.unityClass, other.unityClass)) {
        //        return false;
        //    }
        //    if (!Objects.equals(this.unityVersion, other.unityVersion)) {
        //        return false;
        //    }
        //    if (!Objects.equals(this.typeNode, other.typeNode)) {
        //        return false;
        //    }
        //    return true;
        //}
    }


    /**
     *
     * @author Nico Bergemann <barracuda415 at yahoo.de>
     */
    public class TypeTreeDatabase 
    {
        public static TypeTreeDatabase Instance = new TypeTreeDatabase();

        public static int FILE_FORMAT = 1;
        public static int NODE_FORMAT = 9;
    
        private List<TypeTreeDatabaseEntry> entries = new List<TypeTreeDatabaseEntry>();
        private Dictionary<string, TypeTreeDatabaseEntry> entryMap = new Dictionary<string, TypeTreeDatabaseEntry>();
    
        public void load(Stream fs) 
        {
            try 
            {
                DataReader inr = new DataReader(fs,Encoding.ASCII);
                // read header
                int dbVersion = inr.readInt();

                if (dbVersion != FILE_FORMAT) {
                    throw new Exception("Wrong database version");
                }
            
                // create node reader
                VersionInfo versionInfo = new VersionInfo();
                versionInfo.assetVersion=NODE_FORMAT;
                TypeNodeReader nodeReader = new TypeNodeReader(versionInfo);

                // read field node table
                int fieldNodeSize = inr.readInt();
                List<TypeNode> typeNodes = new List<TypeNode>(fieldNodeSize);
            
                for (int i = 0; i < fieldNodeSize; i++)
                {
                    TypeNode node = new TypeNode(versionInfo);
                    nodeReader.read(inr, node);
                    typeNodes.Add(node);
                }

                // read version string table
                int versionSize = inr.readInt();
                List<UnityVersion> versionsTmp = new List<UnityVersion>(versionSize);

                for (int i = 0; i < versionSize; i++) 
                {
                    versionsTmp.Add(new UnityVersion(inr.readStringNull()));
                }

                // read mapping data
                int numEntries = inr.readInt();

                for (int i = 0; i < numEntries; i++)
                {
                    int nodeIndex = inr.readInt();
                    int classID = inr.readInt();
                    int versionIndex = inr.readInt();
                
                    UnityVersion unityVersion = versionsTmp[versionIndex];
                    UnityClass unityClass = new UnityClass(classID);
                    TypeNode typeNode = typeNodes[nodeIndex];
                
                    addEntry(unityClass, unityVersion, typeNode);
                }
            } 
            catch (IOException ex) {
                Logger.Log("Can't read type database"+ ex);
            }
        }
    
        public void save()
        {
            Logger.Log("Saving type database");
        
            try 
            {
                //OutputStream os = Files.newOutputStream(dbFile, WRITE, CREATE, TRUNCATE_EXISTING);
                //save(os);
            }
            catch (IOException ex) 
            {
                Logger.Log("Can't open type database" + ex);
            }
        }
    
        public void save(Stream os) 
        {
            // write database file
            try 
            {
                DataWriter outw = new DataWriter(os,Encoding.ASCII);
                // write header
                outw.writeInt(FILE_FORMAT);
            
                // create node writer
                VersionInfo versionInfo = new VersionInfo();
                versionInfo.assetVersion=NODE_FORMAT;
                TypeNodeWriter nodeWriter = new TypeNodeWriter(versionInfo);

                // write field node table
                List<TypeNode> nodesTmp = new List<TypeNode>(new HashSet<TypeNode>(getNodeSet()));
                outw.writeInt(nodesTmp.Count);

                foreach (TypeNode typeNode in nodesTmp) 
                {
                    nodeWriter.write(outw, typeNode);
                }

                // write version string table
                List<UnityVersion> versionsTmp = new List<UnityVersion>(new HashSet<UnityVersion>(getVersionSet()));
                outw.writeInt(versionsTmp.Count);

                foreach (UnityVersion version in versionsTmp) 
                {
                    outw.writeStringNull(version.ToString());
                }

                // write mapping data
                outw.writeInt(entries.Count);

                foreach (TypeTreeDatabaseEntry entry in entries) 
                {
                    UnityClass unityClass = entry.unityClass;
                    UnityVersion unityVersion = entry.unityVersion;
                    TypeNode node = entry.typeNode;
                
                    int nodeIndex = nodesTmp.IndexOf(node);
                    int classID = unityClass.ID();
                    int versionIndex = versionsTmp.IndexOf(unityVersion);

                    outw.writeInt(nodeIndex);
                    outw.writeInt(classID);
                    outw.writeInt(versionIndex);
                }
            }
            catch (IOException ex)
            {
                Logger.Log("Can't write type database" + ex);
            }
        }
    
        public List<TypeTreeDatabaseEntry> getEntries() 
        {
            return entries;
        }
    
        public HashSet<UnityVersion> getVersionSet()
        {
            HashSet<UnityVersion> versions = new HashSet<UnityVersion>();
            foreach (TypeTreeDatabaseEntry entry in entries) 
            {
                versions.Add(entry.unityVersion);
            }
            return versions;
        }
    
        public HashSet<UnityClass> getClassSet() 
        {
            HashSet<UnityClass> classes = new HashSet<UnityClass>();
            foreach (TypeTreeDatabaseEntry entry in entries) 
            {
                classes.Add(entry.unityClass);
            }
            return classes;
        }
    
        public HashSet<TypeNode> getNodeSet() 
        {
            HashSet<TypeNode> nodes = new HashSet<TypeNode>();
            foreach (TypeTreeDatabaseEntry entry in entries) 
            {
                nodes.Add(entry.typeNode);
            }
            return nodes;
        }
    
        public TypeTreeDatabaseEntry getEntry(UnityClass unityClass, UnityVersion unityVersion, bool exact) 
        {
            // search for exact matches
            string pair=unityClass.name() + "_"+unityVersion.build;
            TypeTreeDatabaseEntry entryA = null;
            if (entryMap.ContainsKey(pair))
            {
                entryA=entryMap[pair];
            }
            if (entryA != null) 
            {
                return entryA;
            }
        
            // cancel if exact matches are required
            if (exact) 
            {
                return null;
            }
        
            TypeTreeDatabaseEntry entryB = null;
            UnityVersion versionB = null;

            TypeTreeDatabaseEntry entryC = null;
            UnityVersion versionC = null;
        
            foreach (TypeTreeDatabaseEntry entry in entries) 
            {
                UnityClass uclass = entry.unityClass;
                UnityVersion version = entry.unityVersion;
                    
                if (uclass.equals(unityClass)) 
                {
                    if (version.major == unityVersion.major)
                    {
                        if (version.minor == unityVersion.minor)
                        {
                            // if major and minor versions match, it will probably work
                            return entry;
                        }
                        else
                        {
                            // suboptimal choice
                            entryB = entry;
                            versionB = version;
                        }
                    }

                    // worst choice
                    entryC = entry;
                    versionC = version;
                }
            }

            // return less perfect match
            if (entryB != null)
            {
                //Logger.Log( "Unprecise match for class {0} (required: {1}, available: {2})"+ new Object[]{unityClass, unityVersion, versionB});
                return entryB;
            }

            // return field node from any revision as the very last resort
            if (entryC != null) 
            {
                //Logger.Log("Bad match for class {0} (required: {1}, available: {2})"+ new Object[]{unityClass, unityVersion, versionC});
                return entryC;
            }

            // no matches at all
            return null;
        }
    
        public TypeNode getTypeNode(UnityClass unityClass, UnityVersion unityVersion, bool exact)
        {
            TypeTreeDatabaseEntry entry = getEntry(unityClass, unityVersion, exact);
            if (entry != null)
            {
                return entry.typeNode;
            }
            else 
            {
                return null;
            }
        }
    
        public void addEntry(TypeTreeDatabaseEntry entry) 
        {
            // don't add duplicates
            if (entryMap.ContainsValue(entry)) 
            {
                return;
            }
        
            entries.Add(entry);
            string pair = entry.unityClass.name() + "_" + entry.unityVersion.build;
            entryMap[pair]=entry;
        }
    
        public void addEntry(UnityClass unityClass, UnityVersion unityVersion, TypeNode node)
        {
            addEntry(new TypeTreeDatabaseEntry(unityClass, unityVersion, node));
        }
    }
}
