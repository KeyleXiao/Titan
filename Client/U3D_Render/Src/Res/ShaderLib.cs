using UnityEngine;
using System.Collections;
using System.Collections.Generic;


public class ShaderLib: MonoBehaviour, IResourceLoad
{
	private static Hashtable CacheShaders=new Hashtable();

	public static ShaderLib Instance=null;

	public void Awake()
	{
		Instance = this;
        //PreLoad ("Shader/Normal");
        //PreLoad ("Shader/Flow-Illumin-Diffuse");
        //PreLoad ("Shader/Flow-Normal-DiffuseDetail");
        //PreLoad ("Shader/ModelFadeIn");
		//PreLoad ("Shader/DoubleDiffuse");
	}

	public void ResourceLoadSucc(ResLoadResult result)
	{
		if (result.resObj != null && result.sPath.Length>0) {
			if (CacheShaders.ContainsKey(result.sPath)==false){
				CacheShaders[result.sPath]=result.resObj;
			}
		}
	}
			
	// 资源加载失败回调函数
	public void ResourceLoadFail(ResLoadResult result)
	{

	}

	public static void PreLoad(string path)
	{
		AssetSystem.LoadAssetAsyn (path, Instance);
	}

	public static Shader Load(string path)
	{
		if (CacheShaders.ContainsKey (path) == false) {
			Shader s=null;
			if (s==null){
				s=AssetSystem.LoadAssetSync(path) as Shader;
			}
			if (s==null){
				s=AssetBundleManager.GetShader(path);
			}
			CacheShaders[path]=s;
			return s;
		}
		else
		{
			Shader s=CacheShaders[path] as Shader;
			return s;
		}
	}
}

