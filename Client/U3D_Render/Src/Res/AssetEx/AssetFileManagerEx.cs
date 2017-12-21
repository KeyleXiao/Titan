//using System.Collections;
//using System.Collections.Generic;
//using System.Threading;
//using System.Runtime.InteropServices;
//using System.Security.Cryptography;
//using System.Text;
//using System.IO;
//using System;
//using AssetEx;

//namespace AssetEx
//{
//    public class AssetFileManagerEx
//    {
//        public Dictionary<string, AssetFileEx> afs = new Dictionary<string, AssetFileEx>();

//        private AssetFileEx BuildAssetFileEx(string filename)
//        {
//            AssetFileEx af = new AssetFileEx();
//            af.sourceFile = filename;
//            af.AssetFileName = Path.GetFileNameWithoutExtension(af.sourceFile);
//            afs[af.sourceFile] = af;
//            return af;
//        }

//        public void load(string path,string[] extracttypes)
//        {
//            AssetFileEx af = BuildAssetFileEx(path);
//            if (extracttypes != null)
//            {
//                foreach (string type in extracttypes)
//                {
//                    af.extractTypes[type] = false;
//                }
//            }
//            af.load(path);
//            af.loadExternals(afs);
//        }
//    }
//}
