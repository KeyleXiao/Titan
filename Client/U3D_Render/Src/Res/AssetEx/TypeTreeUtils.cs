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
    public class TypeTreeUtils
    {
    
        public static void embedTypes(AssetFileEx asset) 
        {
            UnityVersion unityRevision = asset.versionInfo.unityRevision;
            if (unityRevision == null) 
            {
                Logger.Log("unityRevision = null");
                return;
            }
        
            Dictionary<long,ObjectData> objects = asset.objects;
            Dictionary<int, EmbeddedTypeInfo> typemap = asset.embeddedTypeMap;
        
            foreach(KeyValuePair<long,ObjectData> pair in objects)
            {
                ObjectData obj = pair.Value;
                int typeID = obj.typeID;
            
                // skip types that already exist
                if (typemap.ContainsKey(typeID)) 
                {
                    continue;
                }

                TypeNode typeNode = getTypeNode(obj, false);
            
                // remove objects with no type tree, which would crash the editor
                // when loading the file otherwise
                if (typeNode == null) 
                {
                    Logger.Log( "Removing object {0} with unresolvable type {1}"+
                            new Object[]{obj, typeID});
                    //objectIter.remove();
                    continue;
                }

                typemap[typeID].typeTree = typeNode;
            }
        }

        public static int learnTypes(AssetFileEx asset) 
        {
            if (asset.isStandalone()) 
            {
                Logger.Log("File doesn't contain type information");
                return 0;
            }

            Dictionary<int, EmbeddedTypeInfo> typemap = asset.embeddedTypeMap;
        
            UnityVersion unityRevision = asset.versionInfo.unityRevision;
            if (unityRevision == null) 
            {
                Logger.Log("unityRevision = null");
                return 0;
            }
        
            int learned = 0;
        
            // merge the TypeTree map with the database field map
            foreach (KeyValuePair<int, EmbeddedTypeInfo> typeTreeEntry in typemap) 
            {
                int typeID = typeTreeEntry.Key;
                TypeNode typeNode = typeTreeEntry.Value.typeTree;
            
                // skip MonoBehaviour types
                if (typeID < 1) 
                {
                    continue;
                }
            
                UnityClass unityClass = new UnityClass(typeID);
                TypeNode typeNodeDB = TypeTreeUtils.getTypeNode(unityClass, unityRevision, true);

                if (typeNodeDB == null)
                {
                    Logger.Log("New: {0}" + unityClass);
                    TypeTreeDatabase.Instance.addEntry(unityClass, unityRevision, typeNode);
                    typeNodeDB = typeNode;
                    learned++;
                }

                // check the hashes, they must be identical at this point
                //int hash1 = typeNode.hashCode();
                //int hash2 = typeNodeDB.hashCode();

                //if (hash1 != hash2) 
                //{
                //    Logger.Log("Database hash mismatch for {0}: {1} != {2}"+
                //            new Object[] {typeNodeDB.type.typeName(), hash1, hash2});
                //}

                // check if the class name is known and suggest the type base name if not
                if (unityClass.name() == null) 
                {
                    Logger.Log("Unknown ClassID {0}, suggested name: {1}" +
                            new Object[] {unityClass.ID(), typeNode.type});
                }
            }
        
            return learned;
        }
    
        public static TypeNode getTypeNode(UnityClass unityClass, UnityVersion unityVersion, bool exact) 
        {
            return TypeTreeDatabase.Instance.getTypeNode(unityClass, unityVersion, exact);
        }
    
        public static TypeNode getTypeNode(ObjectData obj, bool strict) 
        {
            return TypeTreeUtils.getTypeNode(obj.unityClass(), obj.versionInfo.unityRevision, strict);
        }


    }
}
