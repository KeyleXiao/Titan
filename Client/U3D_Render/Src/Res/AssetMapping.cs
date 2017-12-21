//using UnityEngine;
//using System.Collections;
//using System.Collections.Generic;
//using System.Threading;


//public class AssetMapping: MonoBehaviour
//{
//    /// <summary>
//    /// 资源包配置（外部使用）
//    /// </summary>
//    private PackageCsv mPackageCsv;
		
//    /// <summary>
//    /// 资源配置（外部使用）
//    /// </summary>
//    private ResourceCsv mResourceCsv;

//    public static AssetMapping Singleton=null;
		
//    public void Awake()
//    {
//        Singleton = this;

//        mPackageCsv = new PackageCsv();
//        mResourceCsv = new ResourceCsv();

//        LoadPackageList ();
//        LoadResourceList ();
//    }

//    public void LoadPackageList()
//    {
//        CsvManager.Instance.LoadSchemeFromResources("Scp/Package", mPackageCsv); //load atom list
//    }

//    public void LoadResourceList()
//    {
//        CsvManager.Instance.LoadSchemeFromResources("Scp/Resource", mResourceCsv);//load atom list
//    }

//    public static string ResourceIDToPath(int resid)
//    {
//        if (Singleton == null) {
//            return "";
//        }

//        ResourceData data = Singleton.mResourceCsv.GetCsvValue (resid);
//        if (data == null) {
//            TRACE.ErrorLn("AssetMapping::ResourceIDToPath--资源不存在，资源id = " + resid);
//            return "";
//        }

//        return data.m_ResPath;
//    }

//    public static string PackIDToPath(int packid)
//    {
//        if (Singleton == null) {
//            return "";
//        }
			
//        PackageData data = Singleton.mPackageCsv.GetCsvValue (packid);
//        if (data == null) {
//            TRACE.ErrorLn("AssetMapping::PackIDToPath--资源包不存在，资源包id = " + packid);
//            return "";
//        }

//        string packname=System.IO.Path.GetFileNameWithoutExtension (data.m_name);
//        return packname;
//    }

//    public static int LookupPackIDByResourceID(int resID)
//    {
//        if (Singleton == null) {
//            return -1;
//        }
			
//        ResourceData data = Singleton.mResourceCsv.GetCsvValue (resID);
//        if (data == null) {
//            TRACE.ErrorLn("AssetMapping::LookupPackIDByResourceID--资源不存在，资源id = " + resID);
//            return -1;
//        }
			
//        return data.m_PackID;
//    }
//}


