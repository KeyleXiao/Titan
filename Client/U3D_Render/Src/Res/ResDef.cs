using UnityEngine;
using System.IO;
using System.Text;
using System.Collections;


public class ResDef
{
    ///// <summary>
    ///// 用户下载目录
    ///// </summary>
    //public static string UserDownLoadDirectory { get { return Game.m_singleton.GetFilePath() + "DownLoad/"; } }

    ///// <summary>
    ///// 用户包目录
    ///// </summary>
    //public static string UserPackageDirectory { get { return Game.m_singleton.GetFilePath() + "Package/"; } }

    public static string DataPath
    {
        get
        {
            string directory = "";
            // 初始化用户根目录
            if (Application.platform == RuntimePlatform.Android)
            {
                //安卓平台
                directory = Application.persistentDataPath + "/";
            }
            else if (Application.platform == RuntimePlatform.IPhonePlayer)
            {
                //IPhone平台
                string path = Application.dataPath.Substring(0, Application.dataPath.Length - 5);
                directory = path.Substring(0, path.LastIndexOf('/')) + "";
            }
            else
            {
                //PC平台
                directory = Application.dataPath + "/";
            }
            return directory;
        }
    }

    public static string UserPath
    {
        get
        {
            string directory = "";
            // 初始化用户根目录
            if (Application.platform == RuntimePlatform.Android)
            {
                //安卓平台
                directory = Application.persistentDataPath + "/User/";
            }
            else if (Application.platform == RuntimePlatform.IPhonePlayer)
            {
                //IPhone平台
                string path = Application.dataPath.Substring(0, Application.dataPath.Length - 5);
                directory = path.Substring(0, path.LastIndexOf('/')) + "/Documents/";
            }
            else
            {
                //PC平台
                directory = Application.dataPath + "/User/";
            }
            return directory; 
        }
    }

	/// <summary>
	/// 用户包目录
    /// </summary>
    public static string OuterPackageDirectory 
	{
		get 
		{
            return UserPath + "Package/"; 
		}
	}

    /// <summary>
    /// 内部包目录
    /// </summary>
    public static string InterPackageDirectory
    {
        get
        {
            string directory = "";
            if (Application.platform == RuntimePlatform.Android)
            {
                directory = Application.streamingAssetsPath +"/Android/";
            }
            else if (Application.platform == RuntimePlatform.IPhonePlayer)
            {
                directory = "file:///" + Application.streamingAssetsPath + "/Iphone/";
            }
			else
            {
                directory = "file:///" + Application.streamingAssetsPath + "/Web/";
            }

            return directory;
        }
    }

	/// <summary>
	/// 内部包目录
	/// </summary>
	public static string InterPackageDirectoryEx
	{
		get
		{
			string directory = "";
			if (Application.platform == RuntimePlatform.Android)
			{
				directory = Application.streamingAssetsPath +"/Android/";
			}
			else if (Application.platform == RuntimePlatform.IPhonePlayer)
			{
				directory = Application.streamingAssetsPath + "/Iphone/";
			}
			else
			{
				directory = Application.streamingAssetsPath + "/Web/";
			}
				
			return directory;
		}
	}

	private static bool bLocal = false;//是否本地
    /// <summary>
    /// 资源包公用url地址(先暂时用这个，以后由入口文件配置）
    /// </summary>
    public static string ResPackageCommonURL 
	{
		get
		{
            bLocal = Application.isEditor;

            string url = "";
            if (bLocal)
            {
                url = "file:///" + Application.dataPath + "/AssetBundles/";
            }
            else
            {
                url = "http://172.16.0.198/";
                //url = "http://172.16.0.102/";
                //url = "file:///" + Application.dataPath + "/../../Game/Assets/AssetBundles/";
            }
            
            if (Application.platform == RuntimePlatform.Android)
			{
				return url+"Android/"; 
			}
			else if (Application.platform == RuntimePlatform.IPhonePlayer)
			{
				return url+"Iphone/"; 
			}
			else
			{
				return url+"Web/"; 
			}
		} 
	}

    /// <summary>
    /// 资源包配置文件名称
    /// </summary>
    public static string NetPackageFileName { get { return "Package.csv"; } }

    /// <summary>
    /// Local资源包配置文件名称
    /// </summary>
    public static string CurrentPackageFileName { get { return "Local_Package.csv"; } }

    /// <summary>
    /// New资源包配置文件名称
    /// </summary>
    public static string NewPackageFileName { get { return "New_Package.csv"; } }

    /// <summary>
    /// 资源配置文件名称
    /// </summary>
    public static string ResourceFileName { get { return "list.csv"; } }

}
