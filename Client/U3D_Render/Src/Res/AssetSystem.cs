#define U462
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Threading;
using System.Security.Cryptography;
using System.Text;
using System.IO;


// 资源加载接口
public interface IResourceLoad
{
	// 资源加载成功
	void ResourceLoadSucc(ResLoadResult result);
	// 资源加载失败
	void ResourceLoadFail(ResLoadResult result);
}
	
/// <summary>
/// 资源加载结果结构
/// </summary>
public struct ResLoadResult
{
	// 请求ID
	public uint uReqId;
		
	// 资源ID
	// 值为-1，代表加载资源包
	// 大于0，代表加载资源物体
	public int nResId;
		
	public string sPath;
		
	// 资源加载物体
	public Object resObj;
	public string resText;
		
	// 资源加载进度
	public float fProgress;
		
	// 错误信息
	// 正在加载，为null
	// 加载成功，为null
	// 加载失败，非null
	public string sErrorMsg;
		
	public ResLoadResult(uint reqId = 0)
	{
		uReqId = reqId;
		nResId = -1;
			
		resObj = null;
		resText = "";
		fProgress = 0f;
		sErrorMsg = null;
		sPath = "";
	}
}
	
/// <summary>
/// 加载类型
/// </summary>
public enum LoadType
{
	None,               // 无用的
		
	CSVFile,            // 配置文件
	UIPrefab,           // UI预制
	UIAtlas,            // UI图集
		
	Max,                // 最大的
}
	
#if SupportPackageIO
public class PackageIO
{
	public static bool Exists(string path)
	{
		return true;
	}
		
	public static string ReadAllText(string path)
	{
		WWW www = new WWW(path);
		while (!www.isDone) 
		{
			//Thread.Sleep(100);
		}
			
		string alltext=www.text;
		www.Dispose();
		return alltext;
	}
		
	public static byte[] ReadAllBytes(string path)
	{
		//安卓系统同步读内包会卡死
		if (Application.platform == RuntimePlatform.Android)
			return null;
			
		string name=System.IO.Path.GetFileNameWithoutExtension(path);
		WWW www = new WWW(path);
		while (!www.isDone) 
		{
			//Thread.Sleep(100);
		}
			
		byte[] srcbytes = www.bytes;
		byte[] allbytes = new byte[srcbytes.Length];
		srcbytes.CopyTo(allbytes,0);
		www.Dispose();
		return allbytes;
	}
		
};
#endif
	
public class AssetSystem : MonoBehaviour
{
	public const int CachePackageNumber = 15;
	public static bool bEncrypt=false;
		
	public static AssetSystem Singleton = null;
		
	public struct RequestAssetInfo
	{
		public string resPath;
		public IResourceLoad iLoad;
		public Object obj;
	}
	public class RequestPackageInfo
	{
		public bool bLoading = false;
		public List<RequestAssetInfo> RequestAssets = new List<RequestAssetInfo> ();
	}
		
	//private AssetBundle PatchAssetBundle=null;
	private Hashtable ResourceList = new Hashtable ();
	private Hashtable PackageList = new Hashtable ();
	private Hashtable CachePackages = new Hashtable();
	private List<string> CacheOrders=new List<string>();
	private Hashtable RequestTable = new Hashtable();
		
	public void Awake()
	{
        if (System.IO.Directory.Exists(ResDef.DataPath) == false)
        {
            System.IO.Directory.CreateDirectory(ResDef.DataPath);
        }
        if (System.IO.Directory.Exists(ResDef.OuterPackageDirectory) == false)
        {
            System.IO.Directory.CreateDirectory(ResDef.OuterPackageDirectory);
        }
		Singleton = this;
		LoadPackageList ();
		LoadResourceList ();

        ResourceEx.Init();
	}

    public void OnDestroy()
	{
        ResourceEx.Uninit();

		foreach (DictionaryEntry keyvalue in CachePackages) {
			AssetBundle ab = keyvalue.Value as AssetBundle;
			ab.Unload(true);
		}
		CachePackages.Clear ();
		CacheOrders.Clear ();
	}

    public void Update()
    {
        ResourceEx.Update();
    }

	private static byte[] Keys = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF };

	public static byte[] EncryptDES(byte[] inputdata, string encryptKey)
	{
		try
		{	
				byte[] rgbKey = Encoding.UTF8.GetBytes(encryptKey.Substring(0, 8));
				byte[] rgbIV = Keys;
				DESCryptoServiceProvider DCSP = new DESCryptoServiceProvider();
				MemoryStream mStream = new MemoryStream();
				CryptoStream cStream = new CryptoStream(mStream, DCSP.CreateEncryptor(rgbKey, rgbIV), CryptoStreamMode.Write);
				cStream.Write(inputdata, 0, inputdata.Length);
				cStream.FlushFinalBlock();
				cStream.Close();
				return mStream.ToArray();
		}
		catch
		{
			return null;
		}
	}

	public static byte[] DecryptDES(byte[] inputdata, string decryptKey)
	{
		try
		{
				byte[] rgbKey = Encoding.UTF8.GetBytes(decryptKey.Substring(0, 8));
				byte[] rgbIV = Keys;
				DESCryptoServiceProvider DCSP = new DESCryptoServiceProvider();
				MemoryStream mStream = new MemoryStream();
				CryptoStream cStream = new CryptoStream(mStream, DCSP.CreateDecryptor(rgbKey, rgbIV), CryptoStreamMode.Write);
				cStream.Write(inputdata, 0, inputdata.Length);
				cStream.FlushFinalBlock();
				cStream.Close();
				return mStream.ToArray();
		}
		catch
		{
				return null;
		}
	}


	public void LoadPackageList()
	{
		PackageList.Clear ();
		string packcfgtxt = LoadAllTextSync(ResDef.CurrentPackageFileName);
		if (packcfgtxt != null && packcfgtxt.Length > 0)
		{
			ScpReader packageconfig = new ScpReader (packcfgtxt, "packcfgtxt");
			int n = packageconfig.GetRecordCount ();
			for (int i=0; i<n; i++)
			{
				string packname = packageconfig.GetString (i, 1, "");
				if (packname.Length > 0) 
				{
					PackageList [packname.ToLower()] = packname;
				}
			}
			Trace.Log("packageconfig,record count="+n.ToString());
            Trace.Log("PackageList,record count=" + PackageList.Count.ToString());
			packageconfig.Dispose ();
		}
	}

	public void LoadResourceList()
	{
		ResourceList.Clear ();
		string rescfgtxt = LoadAllTextSync(ResDef.ResourceFileName);
		if (rescfgtxt != null && rescfgtxt.Length > 0)
		{
			ScpReader resourceconfig = new ScpReader (rescfgtxt, "rescfgtxt");
			int n = resourceconfig.GetRecordCount ();
			for (int i=0; i<n; i++)
			{
				string respath = resourceconfig.GetString (i, 1, "");
				string packname = resourceconfig.GetString (i, 3, "");
				if (respath.Length > 0 && packname.Length > 0) 
				{
					ResourceList [respath] = packname;
				}
			}
            Trace.Log("rescfgtxt,record count=" + n.ToString());
            Trace.Log("ResourceList,record count=" + ResourceList.Count.ToString());
			resourceconfig.Dispose ();
		}
	}
		
	/// <summary>
	/// 从Resources目录加载
	/// </summary>
	/// <param name="type">文件加载类型</param>
	/// <param name="filePath">
	/// 文件相对路径(不带文件格式）
	/// 注：Resources目录下的相对路径
	/// </param>
	public Object LoadFromResources(LoadType type, string filePath)
	{
		Object resObj = null;
			
		// 区分类型加载
		switch (type)
		{
		case LoadType.CSVFile:
		{
			resObj = Resources.Load(filePath, typeof(TextAsset));                      
		}
			break;
				
		case LoadType.UIPrefab:
		{
			resObj = Resources.Load(filePath, typeof(GameObject));
		}
			break;
				
		case LoadType.UIAtlas:
		{
			resObj = Resources.Load(filePath, typeof(GameObject));
		}
			break;
				
		default:
			break;
		}
			
		// 校验资源加载是否成功
		if (resObj == null)
		{
            Trace.LogError("LoadFromResources--Resources目录中找不到该资源，资源相对路径 = " + filePath);
			return null;
		}
			
		return resObj;
	}
		
	//同步读文本
	public static string LoadAllTextSync(string textfilename)
	{
		//先外包
		string outerpath= ResDef.OuterPackageDirectory + textfilename; 
		if (System.IO.File.Exists(outerpath))
		{
			long t1=Api.GetTickCount();
			string text = System.IO.File.ReadAllText(outerpath,System.Text.Encoding.UTF8);
			long t2=Api.GetTickCount();
			long dt=t2-t1;
            Trace.Log("LoadAllTextSync OK:" + outerpath + ",time=" + dt.ToString() + "ms");
			return text;
		}
			
		//接着以文件系统方式读内包
		string innerpath1= ResDef.InterPackageDirectoryEx + textfilename; 
		if (System.IO.File.Exists(innerpath1))
		{
			long t1=Api.GetTickCount();
			string text = System.IO.File.ReadAllText(innerpath1,System.Text.Encoding.UTF8);
			long t2=Api.GetTickCount();
			long dt=t2-t1;
            Trace.Log("LoadAllTextSync OK:" + innerpath1 + ",time=" + dt.ToString() + "ms");
			return text;
		}
			
		#if SupportPackageIO
		//再以包系统方式读内包
		string innerpath2 = ResDef.InterPackageDirectory + textfilename;
		if (PackageIO.Exists (innerpath2)) 
		{
			return PackageIO.ReadAllText(innerpath2,System.Text.Encoding.UTF8);
		}
		#endif

        //Trace.LogError("LoadAllTextSync fail:" + outerpath + "," + innerpath1);
			
		//都失败就返回失败
		return "";
	}
		
	//异步读文本
	public static void LoadAllTextAsyn(string textfilename,IResourceLoad iLoad)
	{
		if (textfilename.Length<=0)
		{
			return;
		}

		//先外包
		string outerpath= ResDef.OuterPackageDirectory + textfilename; 
		if (System.IO.File.Exists(outerpath))
		{
			long t1=Api.GetTickCount();
			string text=System.IO.File.ReadAllText(outerpath);
			long t2=Api.GetTickCount();
			long dt=t2-t1;

            Trace.Log("LoadAllTextAsyn OK:" + outerpath + ",time=" + dt.ToString() + "ms");
				
			ResLoadResult ret = new ResLoadResult ();
			ret.fProgress = 1.0f;
			ret.resObj = null;
			ret.resText = text;
			ret.sErrorMsg = "";
			ret.sPath = outerpath;		
			iLoad.ResourceLoadSucc(ret);
				
			return;
		}
			
		//接着以文件系统方式读内包
		string innerpath1= ResDef.InterPackageDirectoryEx + textfilename; 
		if (System.IO.File.Exists(innerpath1))
		{
			long t1=Api.GetTickCount();
			string text=System.IO.File.ReadAllText(innerpath1);
			long t2=Api.GetTickCount();
			long dt=t2-t1;

            Trace.Log("LoadAllTextAsyn OK:" + innerpath1 + ",time=" + dt.ToString() + "ms");
				
			ResLoadResult ret = new ResLoadResult ();
			ret.fProgress = 1.0f;
			ret.resObj = null;
			ret.resText = text;
			ret.sErrorMsg = "";
			ret.sPath = innerpath1;		
			iLoad.ResourceLoadSucc(ret);
				
			return;
		}
			
		#if SupportPackageIO
		//再以包系统方式读内包
		string innerpath2 = ResDef.InterPackageDirectory + textfilename;
		if (PackageIO.Exists (innerpath2)) 
		{
			string text=PackageIO.ReadAllText(innerpath2);
				
			ResLoadResult ret = new ResLoadResult ();
			ret.fProgress = 1.0f;
			ret.resObj = null;
			ret.resText = text;
			ret.sErrorMsg = "";
			ret.sPath = innerpath2;		
			iLoad.ResourceLoadSucc(ret);
		}
		#endif

        //Trace.LogError("LoadAllTextSync fail:" + outerpath + "," + innerpath1);
			
		//都失败就返回失败
		{
			ResLoadResult ret = new ResLoadResult ();
			ret.fProgress = 1.0f;
			ret.resObj = null;
			ret.resText="";
			ret.sErrorMsg = "";
			ret.sPath = outerpath;		
			iLoad.ResourceLoadFail(ret);
				
			return;
		}
			
	}
		
	/// <summary>
	/// 资源包是否加载并缓存了
	/// </summary>
	/// <param name="packID">资源包名</param>
	public static bool IsResPackageLoaded(string PackName)
	{
		return Singleton.CachePackages.Contains (PackName);
	}
		
	/// <summary>
	/// 同步加载资源包
	/// </summary>
	/// <param name="packID">资源包名</param>
	public static AssetBundle LoadResPackageSync(string PackName)
	{
		if (!Application.isPlaying) {
			return null;
		}

		if (PackName.Length<=0) {
			return null;
		}
			
		if (Singleton.CachePackages.Contains (PackName)) {
			return Singleton.CachePackages[PackName] as AssetBundle;
		}
			
		if (Singleton.RequestTable.Contains (PackName)) {
			RequestPackageInfo info=Singleton.RequestTable[PackName] as RequestPackageInfo;
			if (info.bLoading==true)
			{
				return null;
			}
		}
			
		string finalpath = "";
		string ext = bEncrypt ? ".zen" : ".unity3D";
		string outerpath=ResDef.OuterPackageDirectory+ PackName+ext;
		string innerpath1 = ResDef.InterPackageDirectoryEx + PackName+ext;
		string innerpath2 = ResDef.InterPackageDirectory + PackName+ext;
		byte[] data = null;
		long t1 = Api.GetTickCount();
		//先读外包
		if (System.IO.File.Exists (outerpath)) 
		{
			data=System.IO.File.ReadAllBytes(outerpath);
			finalpath=outerpath;
		}
		//没有的话先以文件系统方式再读内包
		else if (System.IO.File.Exists (innerpath1)) 
		{
			data=System.IO.File.ReadAllBytes(innerpath1);
			finalpath=innerpath1;
		}
		//再没有再以包系统方式读内包（安卓下内包可能在jar包里需要多次解压）
		#if SupportPackageIO
		else if (PackageIO.Exists (innerpath2)) 
		{
			data=PackageIO.ReadAllBytes(innerpath2);
		}
		#endif
		else
		{
            Trace.LogError("LoadResPackageSync fail, the file doesn't exists in paths below:" + outerpath + "," + innerpath1 + "," + innerpath2);
		}
		if (bEncrypt) 
		{
			string key=PackName+"*^$@&%#!";
			byte[] outdata=DecryptDES(data,key);
			if (outdata!=null)
			{
				data=outdata;
			}
		}
			
		if (data!=null)
		{
			AssetBundle pAssetBundle=null;
			try
			{
#if U462
                pAssetBundle = AssetBundle.LoadFromMemory(data);
#else
                string temppath=ResDef.UserPath+"temp.unity3d";
                System.IO.File.WriteAllBytes(temppath, data);
                pAssetBundle = AssetBundle.CreateFromFile(temppath);
#endif
			}
			catch(UnityException e)
			{
                Trace.LogError(e.ToString());
			}
			if (pAssetBundle!=null)
			{
				Singleton.CachePackages[PackName]=pAssetBundle;
				Singleton.CacheOrders.Add(PackName);
				long t2=Api.GetTickCount();
				long dt=t2-t1;
                Trace.Log("LoadResPackageSync OK:" + finalpath + ",time=" + dt.ToString() + "ms");
				return pAssetBundle;
			}
			else
			{
                Trace.LogError("LoadResPackageSync fail, CreateFromMemoryImmediate fail:" + finalpath);
			}
		}
		else
		{
            Trace.LogError("LoadResPackageSync fail, ReadAllBytes fail:" + finalpath);
		}
			
		return null;
	}


	private void InvokePackageCallback(ResLoadResult ret, string PackName,IResourceLoad iLoad)
	{
		bool bSucc = CachePackages.Contains (PackName);
			
		if (bSucc)
		{
			if (iLoad!=null)
			{
				iLoad.ResourceLoadSucc(ret);
			}
		}
		else {
			if (iLoad!=null)
			{
				iLoad.ResourceLoadFail(ret);
			}
		}
			
		if (bSucc)
		{
			AssetBundle assetbundle=CachePackages[PackName] as AssetBundle;
			if (RequestTable.Contains (PackName)) 
			{
				RequestPackageInfo packinfo=RequestTable[PackName] as RequestPackageInfo;
				List<RequestAssetInfo> RequestList=packinfo.RequestAssets;
				for(int i=0;i<RequestList.Count;i++)
				{
					RequestAssetInfo info = RequestList[i];
					info.obj=null;
					if (assetbundle.Contains(info.resPath))
					{
						info.obj=assetbundle.LoadAsset(info.resPath);
					}
					if (info.obj==null)
					{
						string resShort=System.IO.Path.GetFileNameWithoutExtension(info.resPath);
						if (assetbundle.Contains(resShort))
						{
							info.obj=assetbundle.LoadAsset(resShort);
						}
					}
					RequestList[i]=info;
				}
				foreach(RequestAssetInfo info in RequestList)
				{
					InvokeAssetCallback(info.obj,info.resPath,info.iLoad);
				}
				RequestTable.Remove(PackName);
			}
		}
	}
		
	IEnumerator LoadResPackageAsynEnumerator(string PackName,IResourceLoad iLoad)  
	{
		if (RequestTable.Contains (PackName)) {
			RequestPackageInfo info=RequestTable[PackName] as RequestPackageInfo;
			if (info.bLoading==true)
			{
				yield break;
			}
		}

        Resources.UnloadUnusedAssets();
        Caching.CleanCache();

        ResLoadResult ret = new ResLoadResult ();
		ret.fProgress = 1.0f;
		ret.resObj = null;
		ret.sErrorMsg = "";
		ret.sPath = PackName;
			
		if (RequestTable.Contains(PackName))
		{
			RequestPackageInfo info=RequestTable[PackName] as RequestPackageInfo;
			info.bLoading=true;
			RequestTable[PackName]=info;
		}

		long t1 = Api.GetTickCount ();
		string ext = bEncrypt ? ".zen" : ".unity3D";
		string outerpath=ResDef.OuterPackageDirectory+ PackName+ext;
		string innerpath1 = ResDef.InterPackageDirectoryEx + PackName+ext;
		string innerpath2 = ResDef.InterPackageDirectory + PackName+ext;
			
		string finalpath = "";
		WWW www = null;
        AssetBundleCreateRequest request = null;
		//外包优先
        if (System.IO.File.Exists (outerpath)) 
		{
			finalpath=outerpath;
            //if (!bEncrypt &&(Application.platform == RuntimePlatform.WindowsEditor || Application.platform == RuntimePlatform.WindowsPlayer)) //对于外包，如果没加密则直接采用读文件方式
            if(false)
            {
                byte[] data = System.IO.File.ReadAllBytes(outerpath);
                if (data != null)
                {
                    request = AssetBundle.LoadFromMemoryAsync(data);
                    while (!request.isDone)
                    {
                        yield return new WaitForEndOfFrame();
                    }
                }
                data = null;
                System.GC.Collect();
                yield return new WaitForEndOfFrame();
            }
            else
            {
                www = WWW.LoadFromCacheOrDownload("file:///" + outerpath, 5);
                //www = new WWW("file:///" + outerpath);
                yield return www;
            }
            
		}
        //内包由于有可能在jar/zip等压缩包内，只能通过www组件去提取
		else if (System.IO.File.Exists (innerpath1)) 
		{
			finalpath=innerpath1;
			www = new WWW ("file:///"+innerpath1);
			yield return www;
		}
		else
		{
			finalpath=innerpath2;
			yield return true;
		}

		if (RequestTable.Contains(PackName))
		{
			RequestPackageInfo info=RequestTable[PackName] as RequestPackageInfo;
			info.bLoading=false;
			RequestTable[PackName]=info;
		}

        AssetBundle pAssetBundle = null;
		if (www!=null)
		{
			if (www.error== null)
			{
				if (!CachePackages.Contains(PackName))
				{
					try
					{
						if (bEncrypt) 
						{
							string key=PackName+"*^$@&%#!";
							byte[] outdata=DecryptDES(www.bytes,key);
							if (outdata!=null)
							{
#if U462
                                pAssetBundle = AssetBundle.LoadFromMemory(outdata);
#else
                                string temppath = ResDef.UserPath + "temp.unity3d";
                                System.IO.File.WriteAllBytes(temppath, outdata);
                                pAssetBundle = AssetBundle.CreateFromFile(temppath);
#endif
							}
						}
						else
						{
							pAssetBundle=www.assetBundle;
						}
					}
					catch(UnityException e)
					{
                        Trace.LogError(e.ToString());
					}
					if (pAssetBundle!=null)
					{
						CachePackages[PackName]=pAssetBundle;
						CacheOrders.Add(PackName);
						long t2=Api.GetTickCount();
						long dt=t2-t1;
                        Trace.Log("LoadResPackageAsynEnumerator OK, www.assetBundle OK:" + finalpath + ",time=" + dt.ToString() + "ms");
					}
					else
					{
                        Trace.LogError("LoadResPackageAsynEnumerator fail, www.assetBundle fail:" + finalpath);
					}
				}
			}
			else
			{
                Trace.LogError("LoadResPackageAsynEnumerator fail, www.errpr fail:" + www.error);
			}

            www.Dispose();
            www = null;
		}
        else if (request!=null)
        {
            if (request.isDone)
            {
                if (!CachePackages.Contains(PackName))
                {
                    try
                    {
                        pAssetBundle = request.assetBundle;
                    }
                    catch (UnityException e)
                    {
                        Trace.LogError(e.ToString());
                    }
                    if (pAssetBundle != null)
                    {
                        CachePackages[PackName] = pAssetBundle;
                        CacheOrders.Add(PackName);
                        long t2 = Api.GetTickCount();
                        long dt = t2 - t1;
                        Trace.Log("LoadResPackageAsynEnumerator OK, www.assetBundle OK:" + finalpath + ",time=" + dt.ToString() + "ms");
                    }
                    else
                    {
                        Trace.LogError("LoadResPackageAsynEnumerator fail, www.assetBundle fail:" + finalpath);
                    }
                }
            }
            else
            {
                Trace.LogError("LoadResPackageAsynEnumerator fail, www.errpr fail:" + www.error);
            }
            request = null;
        }
		else
		{
            Trace.LogError("LoadResPackageAsynEnumerator fail, www null,package doesn't exists:" + finalpath);
		}

        System.GC.Collect();
        yield return new WaitForEndOfFrame();

		InvokePackageCallback (ret,PackName,iLoad);
	}
		
	public static void LoadResPackageAsyn(string PackName, IResourceLoad iLoad, bool PureAsyn=true)
	{
		if (!Application.isPlaying) {
			return;
		}

		if (PackName.Length<=0) {
			ResLoadResult ret = new ResLoadResult ();
			ret.fProgress = 1.0f;
			ret.resObj = null;
			ret.sErrorMsg = "";
			ret.sPath = PackName;
			Singleton.InvokePackageCallback(ret,PackName,iLoad);
			return;
		}

		if (Singleton.CachePackages.Contains (PackName))
		{
			ResLoadResult ret = new ResLoadResult ();
			ret.fProgress = 1.0f;
			ret.resObj = Singleton.CachePackages[PackName] as AssetBundle;
			ret.sErrorMsg = "";
			ret.sPath = PackName;
			Singleton.InvokePackageCallback(ret,PackName,iLoad);
			return;
		}
			
		if (Singleton.RequestTable.Contains (PackName)) {
			RequestPackageInfo info=Singleton.RequestTable[PackName] as RequestPackageInfo;
			if (info.bLoading==true)
			{
				return;
			}
		}
			
		if (PureAsyn)
		{
			Singleton.StartCoroutine (Singleton.LoadResPackageAsynEnumerator (PackName,iLoad));
		}
		else
		{
			ResLoadResult ret = new ResLoadResult ();
			ret.fProgress = 1.0f;
			ret.resObj = null;
			ret.sErrorMsg = "";
			ret.sPath = PackName;
				
			LoadResPackageSync (PackName);
				
			Singleton.InvokePackageCallback(ret,PackName,iLoad);
		}
	}
		
	/// <summary>
	/// 卸载资源包
	/// </summary>
	/// <param name="packID">资源包名</param>
	public static void UnLoadResPackage(string PackName)
	{
		if (PackName.Length<=0) {
			return;
		}

		if (!Application.isPlaying) {
			return;
		}
			
		Resources.UnloadUnusedAssets ();
		if (Singleton.CachePackages.Contains (PackName))
		{
			AssetBundle pAssetBundle=Singleton.CachePackages[PackName] as AssetBundle;
			pAssetBundle.Unload(false);
			pAssetBundle=null;
			Singleton.CachePackages.Remove(PackName);
			Singleton.CacheOrders.Remove(PackName);
            Trace.Log("UnLoadResPackage OK:" + PackName);
		}
		else
		{
            Trace.LogError("UnLoadResPackage Fail:" + PackName);
		}
		//System.GC.Collect ();
	}
		

	private void InvokeAssetCallback(UnityEngine.Object obj,string resPath,IResourceLoad iLoad)
	{
		ResLoadResult ret = new ResLoadResult ();
		ret.fProgress = 1.0f;
		ret.resObj = obj;
		ret.sErrorMsg = "";
		ret.sPath = resPath;	
			
		//finally try to load from default resource package
		if (ret.resObj==null)
		{
			ret.resObj=Resources.Load (resPath);
		}
			
		if (ret.resObj != null) {
			if (iLoad!=null)
			{
				iLoad.ResourceLoadSucc(ret);
			}
		}
		else {
            Trace.LogError("LoadFromAssetBundleAsyn fail, respath = " + resPath);
				
			if (iLoad!=null)
			{
				iLoad.ResourceLoadFail(ret);
			}
		}
	}
		
	private void LoadFromAssetBundleAsynImp(string resPath,IResourceLoad iLoad)
	{
		UnityEngine.Object ret = null;

		long t1 = Api.GetTickCount ();

		//----------------------------------------------------
		//step1: try to load from packages
		//----------------------------------------------------
		//step1.1: unload packages
		int trynumber=CachePackages.Count-CachePackageNumber;
		for(int i=0;i<trynumber;i++)
		{
			string selectkey="";
			if (CacheOrders.Count>1)
			{
				selectkey=CacheOrders[0];
				if (selectkey=="patch")
				{
					selectkey=CacheOrders[1];
				}
			}
//				foreach(DictionaryEntry keyvalue in CachePackages)
//				{
//					string key=keyvalue.Key as string;
//					if (key!="patch" && RequestTable.Contains(key)==false) //unload where it is not requesting
//					{
//						selectkey=key;
//						break;
//					}
//				}
			if (selectkey.Length>0)
			{
				UnLoadResPackage(selectkey);
			}
		}
		//System.GC.Collect ();
			
		//step1.2: load packages
		string PackName="";
		if (CachePackages.Contains ("patch")) {
			PackName="patch"; //set default to patch if it is exists
		}
		if (ResourceList.Contains (resPath.ToLower())) {
			PackName = ResourceList [resPath.ToLower()] as string;
		}
		AssetBundle PackageAssetBundle=null;
		if (PackName.Length>0)
		{
			if (!CachePackages.Contains(PackName))
			{
				if (!RequestTable.Contains(PackName))
				{
					RequestPackageInfo packnewinfo=new RequestPackageInfo();
					RequestTable[PackName]=packnewinfo;
				}
				RequestPackageInfo packinfo=RequestTable[PackName] as RequestPackageInfo;
				RequestAssetInfo info=new RequestAssetInfo();
				info.resPath=resPath;
				info.iLoad=iLoad;
				packinfo.RequestAssets.Add(info);
				RequestTable[PackName]=packinfo;
				LoadResPackageAsyn(PackName,null,true);
				return;
			}
			PackageAssetBundle=CachePackages[PackName] as AssetBundle;
		}
			
		//step1.3: then try to load from patch package
		if (PackageAssetBundle != null) {
			if (PackageAssetBundle.Contains(resPath))
			{
				ret=PackageAssetBundle.LoadAsset(resPath);
			}
			if (ret==null)
			{
				string resShort=System.IO.Path.GetFileNameWithoutExtension(resPath);
				if (PackageAssetBundle.Contains(resShort))
				{
					ret=PackageAssetBundle.LoadAsset(resShort);
				}
			}
		}

		long t2 = Api.GetTickCount ();
		long dt = t2 - t1;
		if (ret!=null)
		{
            Trace.Log("LoadFromAssetBundleAsynImp OK, respath = " + resPath + ", pack=" + PackName + ", time=" + dt.ToString() + "ms");
		}
		else
		{
            Trace.LogError("LoadFromAssetBundleAsynImp fail, respath = " + resPath + ", pack=" + PackName + ", time=" + dt.ToString() + "ms");
		}
			
		InvokeAssetCallback(ret,resPath,iLoad);
	}
		
		
	private UnityEngine.Object LoadFromAssetBundleSyncImp(string resPath)
	{
		UnityEngine.Object ret = null;

		//----------------------------------------------------
		//step1: try to load from packages
		//----------------------------------------------------
		{
			long t1 = Api.GetTickCount ();

			//step1.1: unload packages
			int trynumber=CachePackages.Count-CachePackageNumber;
			for(int i=0;i<trynumber;i++)
			{
				string selectkey="";
				if (CacheOrders.Count>1)
				{
					selectkey=CacheOrders[0];
					if (selectkey=="patch")
					{
						selectkey=CacheOrders[1];
					}
				}
//					foreach(DictionaryEntry keyvalue in CachePackages)
//					{
//						string key=keyvalue.Key as string;
//						if (key!="patch" && RequestTable.Contains(key)==false)
//						{
//							selectkey=keyvalue.Key as string;
//							break;
//						}
//					}
				if (selectkey.Length>0)
				{
					UnLoadResPackage(selectkey);
				}
			}
			//System.GC.Collect ();

			string PackName="";
			if (CachePackages.Contains ("patch")) {
				PackName="patch";  //set default to patch if it is exists
			}
			if (ResourceList.Contains (resPath.ToLower())) {
				PackName = ResourceList [resPath.ToLower()] as string;
			}
				
			//step1.2:load package
			AssetBundle pAssetBundle=null;
			if (PackName.Length>0)
			{
				if (!CachePackages.Contains(PackName))
				{
					pAssetBundle=LoadResPackageSync(PackName);
				}
				else
				{
					pAssetBundle=CachePackages[PackName] as AssetBundle;
				}
			}
				
			//step1.3: then try to load from patch package
			if (pAssetBundle != null) {
				if (pAssetBundle.Contains(resPath))
				{
					ret=pAssetBundle.LoadAsset(resPath);
				}
				if (ret==null)
				{
					string resShort=System.IO.Path.GetFileNameWithoutExtension(resPath);
					if (pAssetBundle.Contains(resShort))
					{
						ret=pAssetBundle.LoadAsset(resShort);
					}
				}
			}

			long t2 = Api.GetTickCount ();
			long dt=t2-t1;

			//step1.4: return asset;
			if (ret != null) {
                Trace.Log("LoadFromAssetBundleSyncImp from Packages OK, respath = " + resPath + ", pack=" + PackName + ", time=" + dt.ToString() + "ms");
				return ret;
			}
		}
			
		//step2: try to load from default resource package
		{
			long t1=Api.GetTickCount();
			ret=Resources.Load (resPath);
			long t2=Api.GetTickCount();
			long dt=t2-t1;
			if (ret != null) {
                Trace.Log("LoadFromAssetBundleSyncImp from Resources OK, respath = " + resPath + ", time=" + dt.ToString() + "ms");
				return ret;
			}
		}
			
        ////step3: try to load from files if it is in edit mode
        //#if UNITY_EDITOR
        //string[] exts=new string[]{"prefab","fbx","3ds","obj","png","jpg","dds","bmp","tga"};
        //foreach(string ext in exts)
        //{
        //    string fullpath="Assets/OriginalAssets/"+resPath+"."+ext;
        //    ret=UnityEditor.AssetDatabase.LoadAssetAtPath(fullpath,typeof(UnityEngine.Object));
        //    if (ret!=null)
        //        return ret;
        //}
        //#endif

        Trace.LogError("LoadFromAssetBundleSyncImp final fail, respath = " + resPath);
			
		//otherwise 
		return null;
	}
		

	/// <summary>
	/// 通用异步加载
	/// </summary>
	/// <param name="resPath">资源相对路径（不带文件格式）</param>
	/// <param name="iLoad">资源加载回调接口</param>
	public static void LoadAssetAsyn(string resPath, IResourceLoad iLoad, bool PureAsyn=true)
	{
		if (resPath.Length <= 0) {
			return;
		}

		if (!Application.isPlaying) 
		{
			Debug.LogError("LoadAssetAsyn(resPath) can not use in edit mode");
			return;
		}
			
		//PureAsyn = true;
		if (PureAsyn)
		{
			Singleton.LoadFromAssetBundleAsynImp(resPath,iLoad);
		}
		else
		{
			ResLoadResult ret = new ResLoadResult ();
			ret.fProgress = 1.0f;
			ret.resObj = Singleton.LoadFromAssetBundleSyncImp(resPath);
			ret.sErrorMsg = "";
			ret.sPath = resPath;
				
			if (ret.resObj != null) {
				if (iLoad!=null)
				{
					iLoad.ResourceLoadSucc(ret);
				}
			}
			else {
				if (iLoad!=null)
				{
					iLoad.ResourceLoadFail(ret);
				}
			}
		}
	}
		

		
		
	/// <summary>
	/// 通用加载
	/// </summary>
	/// <param name="resPath">资源相对路径（不带文件格式）</param>
	public static UnityEngine.Object LoadAssetSync(string resPath)
	{
		if (resPath.Length <= 0) {
			return null;
		}

		UnityEngine.Object ret = null;

		if (!Application.isPlaying)
		{
            //#if UNITY_EDITOR
            //string[] exts=new string[]{"prefab","fbx","3ds","obj","png","jpg","dds","bmp","tga"};
            //foreach(string ext in exts)
            //{
            //    string fullpath="Assets/OriginalAssets/"+resPath+"."+ext;
            //    ret=UnityEditor.AssetDatabase.LoadAssetAtPath(fullpath,typeof(UnityEngine.Object));
            //    if (ret!=null)
            //        return ret;
            //}
            //#endif
				
			ret=Resources.Load(resPath);
			if (ret!=null)
			{
				return ret;
			}

			return null;
		}
		else if (Singleton!=null)
		{
			//first try to load from inner package
			ret = Singleton.LoadFromAssetBundleSyncImp (resPath);
			if (ret != null) {
				return ret;
			}
		}
			
		return null;
	}
		
	/// <summary>
	/// 资源包里是否存在该资源
	/// </summary>
	/// <param name="resPath">资源相对路径（不带文件格式）</param>
	public static bool ContainAsset(string resPath)
	{
		return Singleton.ResourceList.Contains (resPath.ToLower());
	}

    /// <summary>
    /// 返回资源对应的资源包名称
    /// </summary>
    /// <param name="resPath">资源相对路径（不带文件格式）</param>
    public static string GetPackNameByAsset(string resPath)
    {
        if (Singleton.ResourceList.ContainsKey(resPath.ToLower()))
        {
            return (string)Singleton.ResourceList[resPath.ToLower()];
        }
        else
        {
            return "";
        }
    }

    /// <summary>
    /// 是否有某个资源包
    /// </summary>
    /// <param name="resPath">资源包名字</param>
    public static bool ContainPackage(string pkgname)
    {
        return Singleton.PackageList.ContainsKey(pkgname.ToLower());
    }

    public delegate void UnloadAssetHanlder(UnityEngine.Object goasset);
    public static event UnloadAssetHanlder UnloadAssetEvent ;

    private static void UnloadSingleAssetImp(UnityEngine.Object asset)
    {
        if (asset == null)
        {
            return;
        }

        System.Type assettype = asset.GetType();

        bool handled = false;
        for(int k=0;k<10;k++)
        {
            if (assettype == null)
            {
                break;
            }

            if (assettype == typeof(AnimationClip) ||
                assettype == typeof(Texture2D) ||
                assettype == typeof(Texture3D) ||
                assettype == typeof(AnimationState) ||
                assettype == typeof(Material) ||
                assettype == typeof(Shader) ||
                assettype == typeof(Avatar) ||
                assettype == typeof(ScriptableObject) ||
                assettype == typeof(Mesh))
            {
                Resources.UnloadAsset(asset);
                handled = true;
                break;
            }
            else if (assettype == typeof(GameObject) ||
                  assettype == typeof(Transform) ||
                  assettype == typeof(AssetBundle) ||
                  assettype == typeof(Component))
            {
                handled = true;
                break;
            }
            else
            {
            }

            assettype = assettype.BaseType;
        }

        if (handled == false)
        {
            Trace.Log("UnloadSingleAssetImp:" + asset);
            Resources.UnloadAsset(asset);
        }
    }

    /// <summary>
    /// 彻底卸载资源
    /// </summary>
    /// <param name="asset">资源对象</param>
    public static bool UnloadAsset(UnityEngine.Object asset)
    {
        //if (Application.isEditor)
        //{
        //    //游戏里删除不掉某些依赖关系asset,让编辑器订阅删除事件，用编辑器方式强制删除，残念，by zjh2015-9-8
        //    if (UnloadAssetEvent != null)
        //    {
        //        UnloadAssetEvent(asset);
        //    }
        //}
        //else
        {
            UnloadSingleAssetImp(asset);
        }
        return true;
    }
}

