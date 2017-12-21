using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Threading;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.IO;
using System;
using AssetEx;

public class ResourceInfoEx
{
    public string bundlename;
    public string assetfilename;
    public string classname;
    public string objectname;
    public string typename;
    public string path;
    public long offset;
    public long length;
    public Dictionary<string, object> propertyfield;

    public int refcount = 0;
    public UnityEngine.Object obj;
}


public class ResourceEx
{
    public static bool bEnable = false;

    private static void LogImp(string s)
    {
        Debug.Log(s);
    }

    public static Dictionary<string, Dictionary<string, ResourceInfoEx>> resourcetable = new Dictionary<string, Dictionary<string, ResourceInfoEx>>();

    private static void LoadResourceInfoFromAssetFile(AssetBundleEx ab, AssetFileEx af, string classname)
    {
        List<ObjectData> odlist = af.extractlist[classname];
        Dictionary<string, ResourceInfoEx> resourcetableoftype = resourcetable[classname];
        foreach (ObjectData od in odlist)
        {
            ResourceInfoEx currentinfo = new ResourceInfoEx();
            currentinfo.bundlename = ab.name;
            currentinfo.assetfilename = af.sourceFile;
            currentinfo.objectname = od.objectname;
            currentinfo.classname = od.classname;
            currentinfo.typename = od.typename;
            currentinfo.propertyfield = od.propertyfield;
            currentinfo.path = od.path;
            object data = od.data;
            if (data is StreamBlock)
            {
                StreamBlock sdi = (StreamBlock)od.data;
                if (sdi != null)
                {
                    currentinfo.offset = sdi.offset;
                    currentinfo.length = sdi.length;
                }
            }

            if (currentinfo.objectname == null || currentinfo.objectname == "")
            {
                continue;
            }

            //if (currentinfo.path == null || currentinfo.path == "")
            //{
            //    continue;
            //}

            if (resourcetableoftype.ContainsKey(currentinfo.objectname))
            {
                ResourceInfoEx otherinfo = resourcetableoftype[od.objectname];
                //Logger.Log("current=" + currentinfo.objectname + "<" + currentinfo.typename + ">,other=" + otherinfo.objectname + "<" + otherinfo.typename + ">");
            }
            else
            {
                resourcetableoftype.Add(currentinfo.objectname, currentinfo);
            }
        }
    }

    private static string GetAssetFileFolder()
    {
        string assetfilefolder = "";
        if (Application.isEditor)
        {
            assetfilefolder = Application.dataPath + "/" + "../../Bin";
                //+"test_Data";
            string[] exefiles = Directory.GetFiles(assetfilefolder, "*.exe");
            foreach(string exefile in exefiles)
            {
                string exefilename=Path.GetFileNameWithoutExtension(exefile);
                string subfolder=assetfilefolder+"/"+exefilename+"_Data";
                if (Directory.Exists(subfolder))
                {
                    assetfilefolder = subfolder;
                    break;
                }
            }
        }
        else
        {
            assetfilefolder = Application.dataPath;
        }
        assetfilefolder = Path.GetFullPath(assetfilefolder);
        return assetfilefolder;
    }


    static AssetBundleManagerEx abm = new AssetBundleManagerEx();
    public static void GenerateResourceTable()
    {
        // get database path based on the path to the current .jar file
        string TYPE_FILE_PATH = "types";
        string CLASSID_PATH = "classes";
        string STRINGTABLE_PATH = "strings";

        TextAsset ta1 = Resources.Load(STRINGTABLE_PATH) as TextAsset;
        StringTable.stInt = new StringTable();
        StringTable.stInt.loadStrings(ta1.bytes);

        TextAsset ta2 = Resources.Load(CLASSID_PATH) as TextAsset;
        MemoryStream classms = new MemoryStream(ta2.bytes);
        UnityClassDatabase.Instance.load(classms);

        TextAsset ta3 = Resources.Load(TYPE_FILE_PATH) as TextAsset;
        MemoryStream typems = new MemoryStream(ta3.bytes);
        TypeTreeDatabase.Instance.load(typems);

        string[] filter = new string[] { "Texture2D" };
        //string path1 = "D:\\project\\speedex\\Bin\\Client\\Game\\Assets\\AssetBundles\\Web\\Scene_TaoYuan.unity3d";
        //string path2 = "D:\\project\\speedex\\Bin\\Client\\Game\\Assets\\AssetBundles\\Web\\Scene_Moba101.unity3d";
        //abm.Load(path1);
        //abm.Load(path2);
        string assetfilefolder = GetAssetFileFolder();
        abm.LoadBundleFromFolder(assetfilefolder, filter);

        resourcetable.Clear();
        foreach(KeyValuePair<string,AssetBundleEx> pair1 in abm.abhs)
        {
            AssetBundleEx ab = pair1.Value;
            foreach (AssetFileEx af in ab.afs.Values)
            {
                foreach (string classname in af.extractlist.Keys)
                {
                    if (resourcetable.ContainsKey(classname) == false)
                    {
                        resourcetable[classname] = new Dictionary<string, ResourceInfoEx>();
                    }
                    LoadResourceInfoFromAssetFile(ab, af, classname);
                }
            }
        }

    }

    public static void LoadResourceTable(string path)
    {
        resourcetable.Clear();
        ScpReader reader = new ScpReader(path,true);

        for (int i = 0; i < reader.GetRecordCount(); i++)
        {
            ResourceInfoEx currentinfo = new ResourceInfoEx();
            currentinfo.bundlename = reader.GetString(i, 2, "");
            currentinfo.assetfilename = reader.GetString(i, 3, "");
            currentinfo.objectname =reader.GetString(i, 1, "");
            currentinfo.classname = reader.GetString(i, 0, "");
            currentinfo.typename = reader.GetString(i, 4, "");
            currentinfo.path = reader.GetString(i, 5, "");
            currentinfo.offset = reader.GetLong(i, 6, -1);
            currentinfo.length = reader.GetLong(i, 7, -1);
            if (resourcetable.ContainsKey(currentinfo.classname) == false)
            {
                resourcetable[currentinfo.classname] = new Dictionary<string, ResourceInfoEx>();
            }
            Dictionary<string, ResourceInfoEx> resourcelist = resourcetable[currentinfo.classname];
            resourcelist[currentinfo.objectname] = currentinfo;
        }

        reader.Dispose();

        Trace.Log("ResourceEx::LoadResourceTable:" + path + " finish");
    }

    public static void SaveResourceTable(string path)
    {
        StreamWriter sw = new StreamWriter(path, false);

        sw.WriteLine("classname,objectname,bundlename,assetfilename,typename,path,offset,length");
        foreach(KeyValuePair<string,Dictionary<string,ResourceInfoEx>> pair1 in resourcetable)
        {
            string classname = pair1.Key;
            Dictionary<string, ResourceInfoEx> resourcelist = pair1.Value;
            foreach (KeyValuePair<string, ResourceInfoEx> pair2 in resourcelist)
            {
                string objectname = pair2.Key;
                ResourceInfoEx info = pair2.Value;
                sw.WriteLine(classname+","+
                    objectname+","+
                    info.bundlename+","+
                    info.assetfilename+","+
                    info.typename + "," +
                    info.path+","+
                    info.offset+","+
                    info.length);
            }
        }

        sw.Close();
        sw.Dispose();
        sw = null;
    }

    public delegate UnityEngine.Object LoadResourceHandler(string path, System.Type t);
    public static event LoadResourceHandler LoadResourceEvent;

    public delegate string GetResourcePathHandler(UnityEngine.Object obj);
    public static event GetResourcePathHandler GetResourcePathEvent;

    public static string GetPath(UnityEngine.Object obj)
    {
        if (!obj) return "";

        if (!bEnable)
        {
            string path = "";
            if (GetResourcePathEvent != null)
            {
                path=GetResourcePathEvent(obj);
            }
            return path;
        }
        else
        {
            return obj.name;
        }
    }

    public static bool Contain<T>(string relpath) where T:UnityEngine.Object
    {
        return Contain(relpath, typeof(T));
    }

    public static bool Contain(string relpath,Type t)
    {
        if (!bEnable)
        {
            UnityEngine.Object retobj = null;
            try
            {
                if (LoadResourceEvent != null)
                {
                    retobj = LoadResourceEvent(relpath, t);
                }
            }
            catch
            { }
            return retobj!=null;
        }
        else
        {
            string classname = t.Name;
            if (resourcetable.ContainsKey(classname) == false) return false;

            Dictionary<string, ResourceInfoEx> infolist = resourcetable[classname];
            if (infolist == null) return false;

            string resname = Path.GetFileNameWithoutExtension(relpath);
            return infolist.ContainsKey(resname);
        }
    }


    public static Texture2D getNormalTexture(Texture2D source)
    {
        Texture2D normalTexture = new Texture2D(source.width, source.height, TextureFormat.ARGB32, true);
        normalTexture.name = source.name;
        Color theColour = new Color();
        for (int x = 0; x < source.width; x++)
        {
            for (int y = 0; y < source.height; y++)
            {
                float srcr = source.GetPixel(x, y).a;
                float srcg = source.GetPixel(x, y).g;
                theColour.r = srcr;
                theColour.g = srcg;
                srcr = srcr * 2.0f - 1.0f;
                srcg = srcg * 2.0f - 1.0f;
                float srcb = Mathf.Sqrt(1.0f - srcr * srcr - srcg * srcg);
                theColour.b = srcb*0.5f+0.5f;
                theColour.a = 1;
                normalTexture.SetPixel(x, y, theColour);
            }
        }
        normalTexture.Compress(false);
        normalTexture.Apply();
        return normalTexture;
    }

    static List<ResourceInfoEx> loadedlist = new List<ResourceInfoEx>();
    private static UnityEngine.Object LoadByType(ResourceInfoEx info,byte[]data,Type t)
    {
        UnityEngine.Object retobj = null;
        if (t == typeof(Texture2D))
        {
            int tfval = (int)info.propertyfield["m_TextureFormat"];
            TextureFormat tf = (TextureFormat)tfval;
            bool minmip = (bool)info.propertyfield["m_MipMap"];
            int width = (int)info.propertyfield["m_Width"];
            int height = (int)info.propertyfield["m_Height"];
            int lightmapformat = (int)info.propertyfield["m_LightmapFormat"];

            Texture2D tex = null;
            int d3dformat = -1;
            if (tf == TextureFormat.ARGB32)
                d3dformat = 21;
            else if (tf == TextureFormat.DXT5)
                d3dformat = 894720068;
            else if (tf == TextureFormat.DXT1)
                d3dformat = 827611204;
            else if (tf == TextureFormat.Alpha8)
                d3dformat = 28;
            if (d3dformat != -1)
            {
                int usage = 0;// 0x00000200;
                int lockflag = 0;// 0x00002000;
                int levels = minmip ? 0 : 1;
                int pool = 0;
                IntPtr pData = Marshal.AllocHGlobal(data.Length);
                Marshal.Copy(data, 0, pData, data.Length);
                IntPtr p = RenderingPlugin.CreateTexture(width, height,
                    levels, usage, d3dformat, pool, lightmapformat, pData, data.Length, lockflag);
                Marshal.FreeHGlobal(pData);
                tex = Texture2D.CreateExternalTexture(width, height, tf, true, false, p);
                tex.name = info.objectname;
            }
            retobj = tex;
        }
        info.refcount = 1;
        info.obj = retobj;
        return retobj;
    }

    private static void UpdateByType(ResourceInfoEx info, byte[] data)
    {
        if (info.obj == null) return;
        if (info.obj is Texture2D)
        {
            int tfval = (int)info.propertyfield["m_TextureFormat"];
            TextureFormat tf = (TextureFormat)tfval;
            bool minmip = (bool)info.propertyfield["m_MipMap"];
            int width = (int)info.propertyfield["m_Width"];
            int height = (int)info.propertyfield["m_Height"];
            int lightmapformat = (int)info.propertyfield["m_LightmapFormat"];

            Texture2D tex = (Texture2D)info.obj;
            int d3dformat = -1;
            if (tf == TextureFormat.ARGB32)
                d3dformat = 21;
            else if (tf == TextureFormat.DXT5)
                d3dformat = 894720068;
            else if (tf == TextureFormat.DXT1)
                d3dformat = 827611204;
            else if (tf == TextureFormat.Alpha8)
                d3dformat = 28;
            if (d3dformat != -1)
            {
                int usage = 0;// 0x00000200;
                int lockflag = 0;// 0x00002000;
                int levels = minmip ? 0 : 1;
                int pool = 0;
                IntPtr oldp = tex.GetNativeTexturePtr();
                RenderingPlugin.ReleaseTexture(oldp);
                IntPtr pData = Marshal.AllocHGlobal(data.Length);
                Marshal.Copy(data, 0, pData, data.Length);
                IntPtr p = RenderingPlugin.CreateTexture(width, height,
                    levels, usage, d3dformat, pool, lightmapformat, pData, data.Length, lockflag);
                Marshal.FreeHGlobal(pData);
                tex.UpdateExternalTexture(p);
                tex.name = info.objectname;
            }
        }
    }

    private static void UnloadByType(ResourceInfoEx info)
    {
        if (info.obj == null) return;
        if (info.obj is Texture2D)
        {
            Texture2D tex = (Texture2D)info.obj;
            IntPtr oldp = tex.GetNativeTexturePtr();
            RenderingPlugin.ReleaseTexture(oldp);
            tex.UpdateExternalTexture(IntPtr.Zero);
            Texture2D.Destroy(tex);
        }
        info.obj = null;
        info.refcount = 0;
    }

     public static T Load<T>(string relpath) where T:UnityEngine.Object
    {
        T obj = Load(relpath, typeof(T)) as T;
        return obj;
    }

    public static UnityEngine.Object Load(string relpath,Type t)
    {
        if (relpath == null || relpath.Length <= 0) return null;

        if (!bEnable)
        {
            UnityEngine.Object retobj = null;
            try
            {
                if (LoadResourceEvent != null)
                {
                    retobj = LoadResourceEvent(relpath, t);
                }
            }
            catch
            { }
            return retobj;
        }
        else
        {
            string classname=t.Name;
            if (resourcetable.ContainsKey(classname) == false) return null;

            Dictionary<string, ResourceInfoEx> infolist = resourcetable[classname];
            if (infolist == null) return null;

            string resname = Path.GetFileNameWithoutExtension(relpath);
            if (infolist.ContainsKey(resname) == false) return null;

            ResourceInfoEx info = infolist[resname];

            //if (info.path == null || info.path.Length <= 0) return null;
            //UnityEngine.Object retobj = Resources.Load(info.path, t);
            //if (retobj == null)
            //{
            //    retobj = Resources.Load(info.path);
            //}

            //if (!t.IsInstanceOfType(retobj))
            //{
            //    UnityEngine.Object extractobj = GetObjectFromDependent(retobj, resname, t);
            //    if (extractobj != null) retobj = extractobj;
            //}

            //if (t == null || !t.IsInstanceOfType(retobj))
            //{

            //}
            //Debug.Log("resname=" + resname + ",respath=" + info.path);
            //Debug.Log(retobj);
            //return retobj;


            if (info.obj != null)
            {
                info.refcount++;
                return info.obj; //已经有直接返回
            }

            UnityEngine.Object retobj = null;

            if (info.length > 0)
            {
                if (abm.abhs.ContainsKey(info.bundlename))
                {
                    AssetBundleEx ab = abm.abhs[info.bundlename];
                    if (ab.afs.ContainsKey(info.assetfilename))
                    {
                        AssetFileEx af = ab.afs[info.assetfilename];
                        Stream fs = af.stream;
                        fs.Position = info.offset;
                        byte[] data = new byte[info.length];
                        int ret = fs.Read(data, (int)0, (int)info.length);
                        if (ret == info.length)
                        {
                            retobj = LoadByType(info, data, t);
                            loadedlist.Add(info);
                        }
                    }
                }
            }

            return retobj;
        }
    }

    public static void Refresh()
    {
        for (int i = 0; i < loadedlist.Count; i++)
        {
            ResourceInfoEx info = loadedlist[i];
            if (info.length > 0)
            {
                if (abm.abhs.ContainsKey(info.bundlename))
                {
                    AssetBundleEx ab = abm.abhs[info.bundlename];
                    if (ab.afs.ContainsKey(info.assetfilename))
                    {
                        AssetFileEx af = ab.afs[info.assetfilename];
                        Stream fs = af.stream;
                        fs.Position = info.offset;
                        byte[] data = new byte[info.length];
                        int ret = fs.Read(data, (int)0, (int)info.length);
                        if (ret == info.length)
                        {
                            UpdateByType(info, data);
                        }
                    }
                }
            }
        }
    }

    public delegate void ResourceAsynHandlerEx(string relpath, UnityEngine.Object obj);

    //定义异步读取状态类
    public class AsyncResourceState
    {
        public String ResourcePath { get; set; }
        public ResourceAsynHandlerEx Handler { get; set; }
        public ResourceInfoEx Info { get; set; }
        public System.Type T { get; set; }
        public Stream FS { get; set; }
        public byte[] Buffer { get; set; }
    }

    //异步读取回调处理方法
    private static void AsyncReadCallback(IAsyncResult asyncResult)
    {
        AsyncResourceState asyncState = (AsyncResourceState)asyncResult.AsyncState;
        int readCn = asyncState.FS.EndRead(asyncResult);
        //判断是否读到内容
        if (readCn == asyncState.Buffer.Length)
        {
            UnityEngine.Object retobj = LoadByType(asyncState.Info, asyncState.Buffer, asyncState.T);
            loadedlist.Add(asyncState.Info);
            if (asyncState.Handler != null)
            {
                asyncState.Handler(asyncState.ResourcePath, retobj);
            }
        }
    }

    public static void LoadAsyn<T>(string relpath, ResourceAsynHandlerEx handler) where T : UnityEngine.Object
    {
        LoadAsyn(relpath, typeof(T), handler);
    }

    public static void LoadAsyn(string relpath, Type t, ResourceAsynHandlerEx handler)
    {
        if (relpath == null || relpath.Length <= 0) return;

        if (!bEnable) //editor case
        {
            //return Resources.Load(relpath, t);
            return;
        }
        else
        {
            string classname = t.Name;
            if (resourcetable.ContainsKey(classname) == false) return;

            Dictionary<string, ResourceInfoEx> infolist = resourcetable[classname];
            if (infolist == null) return;

            string resname = Path.GetFileNameWithoutExtension(relpath);
            if (infolist.ContainsKey(resname) == false) return;

            ResourceInfoEx info = infolist[resname];

            //if (info.path == null || info.path.Length <= 0) return;
            //UnityEngine.Object retobj = Resources.Load(info.path, t);
            //if (retobj == null)
            //{
            //    retobj = Resources.Load(info.path);
            //}

            //if (!t.IsInstanceOfType(retobj))
            //{
            //    UnityEngine.Object extractobj = GetObjectFromDependent(retobj, resname, t);
            //    if (extractobj != null) retobj = extractobj;
            //}

            //if (t == null || !t.IsInstanceOfType(retobj))
            //{
            //    Debug.Log("resname=" + resname + ",respath=" + info.path);
            //    Debug.Log(retobj);
            //}
            //if (handler != null)
            //{
            //    handler(relpath, retobj);
            //}

            if (info.obj != null)//已经加载过的直接回调
            {
                info.refcount++;
                if (handler != null)
                {
                    handler(relpath, info.obj);
                }
                return;
            }

            if (info.length > 0)
            {
                if (abm.abhs.ContainsKey(info.bundlename))
                {
                    AssetBundleEx ab = abm.abhs[info.bundlename];
                    if (ab.afs.ContainsKey(info.assetfilename))
                    {
                        AssetFileEx af = ab.afs[info.assetfilename];
                        Stream fs = af.stream;
                        fs.Position = info.offset;
                        byte[] data = new byte[info.length];
                        AsyncResourceState asyncState = new AsyncResourceState { FS = fs, Buffer = data, Info = info, T = t, Handler = handler, ResourcePath = relpath };
                        IAsyncResult asyncResult = fs.BeginRead(data, (int)0, (int)info.length, new AsyncCallback(AsyncReadCallback), asyncState);
                    }
                }
            }

            return;
        }
    }

    public static void Unload<T>(string relpath) where T : UnityEngine.Object
    {
        Unload(relpath, typeof(T));
    }

    public static void Unload(string relpath, Type t)
    {
        if (!bEnable)
        {
            return;
        }
        else
        {
            string classname = t.Name;
            if (resourcetable.ContainsKey(classname) == false) return;

            Dictionary<string, ResourceInfoEx> infolist = resourcetable[classname];
            if (infolist == null) return;

            string resname = Path.GetFileNameWithoutExtension(relpath);
            if (infolist.ContainsKey(resname) == false) return;

            ResourceInfoEx info = infolist[resname];
            if (info.refcount <= 1)
            {
                UnloadByType(info);
                loadedlist.Remove(info);
            }
            else
            {
                info.refcount--;
            }
           
        }
    }

    public static void Collect(bool force=false)
    {
        foreach (KeyValuePair<string, Dictionary<string, ResourceInfoEx>> pair in resourcetable)
        {
            string typename = pair.Key;
            Dictionary<string, ResourceInfoEx> infolist = pair.Value;
            foreach (KeyValuePair<string, ResourceInfoEx> pair1 in infolist)
            {
                ResourceInfoEx info = pair1.Value;
                if (force|| info.refcount <= 0)
                {
                    UnloadByType(info);
                    loadedlist.Remove(info);
                }
            }
        }
    }

    //static UnityEngine.Object o;

    public static void Init()
    {
        //Logger.LogEvent -= LogImp;
        //Logger.LogEvent += LogImp;

        if (!bEnable) return;

        GenerateResourceTable();
        //string resourcelistpath = GetAssetFileFolder() + "\\resourcelist.csv";
        //if (!File.Exists(resourcelistpath))
        //{
        //    GenerateResourceTable();
        //    SaveResourceTable(resourcelistpath);
        //}
        //else
        //{
        //    LoadResourceTable(resourcelistpath);
        //}

        ////下面是测试代码
        //Type t = typeof(Texture2D);
        //if (resourcetable.ContainsKey(t.Name))
        //{
        //    Dictionary<string, ResourceInfoEx> infolist = resourcetable[t.Name];
        //    Debug.Log(infolist.Count);
        //    int count = 0;
        //    foreach (KeyValuePair<string, ResourceInfoEx> pair in infolist)
        //    {
        //        ResourceInfoEx info = pair.Value;

        //        UnityEngine.Object o = ResourceEx.Load(info.objectname, t);
        //        //ResourceEx.Unload(info.objectname, t);
        //        //ResourceEx.LoadAsyn(info.objectname, t, null);

        //        count++;
        //        if (count > 100) break;
        //    }
        //}



        //UnityEngine.Object o = Resources.Load("../Assets/Artist/Effect/textures/daizi/daizi_00515");
        //UnityEngine.Object o = Resources.Load("Effect/textures/daizi/daizi_00515");
        //UnityEngine.Object o = Resources.Load("daizi_00515");
        //Debug.LogError(o);
    }

    public static void Uninit()
    {
    }

    private static int lastquality = -1;
    public static void Update()
    {
        int curquality = QualitySettings.GetQualityLevel();
        if (curquality != lastquality)
        {
            if (lastquality != -1)
            {
                Refresh();
            }
            lastquality = curquality;
        }
    }
}


