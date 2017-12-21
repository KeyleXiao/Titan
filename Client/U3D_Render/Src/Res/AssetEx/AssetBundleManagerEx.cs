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

    public class AssetBundleManagerEx
    {
        public Dictionary<string, AssetBundleEx> abhs = new Dictionary<string,AssetBundleEx>();


        private AssetBundleEx BuildAssetBundleEx(string filename)
        {
            AssetBundleEx abh = new AssetBundleEx();
            abh.name = filename;
            abhs[filename]=abh;
            return abh;
        }

        public void LoadBundleFromFolder(string path,string[] filter)
        {
            if (Directory.Exists(path) == false) return;
            AssetBundleEx abh = BuildAssetBundleEx(Path.GetFileNameWithoutExtension(path));
            abh.ReadFolder(path,filter);
        }

        //throws AssetBundleException, IOException 
        public void LoadBundleFromFile(string path,string[] filter)
        {
            if (File.Exists(path) == false) return;

            FileStream fs = new FileStream(path, FileMode.Open);
            fs.Position = 0;

            BinaryReader br = new BinaryReader(fs);
            int packnumber = br.ReadInt32();
            fs.Position = 0;

            if (packnumber < 10)
            {
                int nscenepkg = br.ReadInt32();
                for (int iscenepkg = 0; iscenepkg < nscenepkg; iscenepkg++)
                {
                    string filename = br.ReadString();
                    int datalen = br.ReadInt32();
                    long curpos = fs.Position;
                    byte[] buf = new byte[datalen];
                    fs.Read(buf, 0, datalen);
                    MemoryStream ms = new MemoryStream(buf);
                    AssetBundleEx abh = BuildAssetBundleEx(filename);
                    abh.ReadPackage(ms,filter);
                    ms.Close();
                    fs.Position = curpos + datalen;
                }

                int nadorningpkg = br.ReadInt32();
                for (int iadorningpkg = 0; iadorningpkg < nadorningpkg; iadorningpkg++)
                {
                    string filename = br.ReadString();
                    int datalen = br.ReadInt32();
                    long curpos = fs.Position;
                    byte[] buf = new byte[datalen];
                    fs.Read(buf, 0, datalen);
                    MemoryStream ms = new MemoryStream(buf);
                    AssetBundleEx abh = BuildAssetBundleEx(filename);
                    abh.ReadPackage(ms, filter);
                    ms.Close();
                    fs.Position = curpos + datalen;
                }

                int nmatpkg = br.ReadInt32();
                for (int imatpkg = 0; imatpkg < nmatpkg; imatpkg++)
                {
                    string filename = br.ReadString();
                    int datalen = br.ReadInt32();
                    long curpos = fs.Position;
                    byte[] buf = new byte[datalen];
                    fs.Read(buf, 0, datalen);
                    MemoryStream ms = new MemoryStream(buf);
                    AssetBundleEx abh = BuildAssetBundleEx(filename);
                    abh.ReadPackage(ms, filter);
                    ms.Close();
                    fs.Position = curpos + datalen;
                }

                int nmeshpkg = br.ReadInt32();
                for (int imeshpkg = 0; imeshpkg < nmeshpkg; imeshpkg++)
                {
                    string filename = br.ReadString();
                    int datalen = br.ReadInt32();
                    long curpos = fs.Position;
                    byte[] buf = new byte[datalen];
                    fs.Read(buf, 0, datalen);
                    MemoryStream ms = new MemoryStream(buf);
                    AssetBundleEx abh = BuildAssetBundleEx(filename);
                    abh.ReadPackage(ms, filter);
                    ms.Close();
                    fs.Position = curpos + datalen;
                }
            }
            else
            {
                AssetBundleEx abh = BuildAssetBundleEx(Path.GetFileNameWithoutExtension(path));
                abh.ReadPackage(fs, filter);
            }

            br = null;


            fs.Close();
            fs.Dispose();
            fs = null;
        }

    }

}