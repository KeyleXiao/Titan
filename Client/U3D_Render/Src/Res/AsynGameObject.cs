using UnityEngine;
using System.Collections;
using System.Collections.Generic;


public class AsynGameObject: MonoBehaviour,IResourceLoad
{
	private static Hashtable CachePrefabs=new Hashtable();

	private class AsynGameObjectAnswer:IResourceLoad
	{
		public void ResourceLoadSucc(ResLoadResult result)
		{
            Trace.Log("AsynGameObject::PreLoadFromPath OK, path = " + result.sPath);
				
			if (!AsynGameObject.CachePrefabs.Contains (result.sPath)) 
			{
				AsynGameObject.CachePrefabs[result.sPath]=result.resObj;
			}
		}
		// 资源加载失败回调函数
		public void ResourceLoadFail(ResLoadResult result)
		{
            Trace.Error("AsynGameObject::PreLoadFromPath Fail, error = " + result.sErrorMsg + ",path = " + result.sPath);
		}		
	}
	private static AsynGameObjectAnswer answer=new AsynGameObjectAnswer();

	public string resPath="";
	public System.EventHandler callback=null;

	/// <summary>
	/// 从路径预载异步游戏对象
	/// </summary>
	/// <param name="resPath">路径</param>
	public static void CleanCaches()
	{
		CachePrefabs.Clear ();
	}

	/// <summary>
	/// 从路径预载异步游戏对象
	/// </summary>
	/// <param name="resPath">路径</param>
	public static void PreLoadFromPath(string resPath)
	{
		if (!CachePrefabs.Contains (resPath)) 
		{
			AssetSystem.LoadAssetAsyn (resPath, answer);
		}
	}

	/// <summary>
	/// 从路径创建异步游戏对象
	/// </summary>
	/// <param name="resPath">路径</param>
	/// <param name="callback">回调对象</param>
	public static GameObject CreateFromPath(string resPath,System.EventHandler callback=null)
	{
		//GameObject tgo = GameObject.CreatePrimitive(PrimitiveType.Cube);
		GameObject tgo = new GameObject ();
		AsynGameObject ago=tgo.AddComponent<AsynGameObject> ();
		ago.resPath = resPath;
		ago.callback = callback;
		return tgo;
	}

	public void Start()
	{
		if (CachePrefabs.Contains (resPath)) 
		{
			GameObject prefab=CachePrefabs[resPath] as GameObject;
			Perform(prefab);
			DestroyImmediate(this);
		}
		else
		{
			AssetSystem.LoadAssetAsyn (resPath, this);
		}
	}

	private void Perform(GameObject prefab)
	{
		if (this == null) {
			return;	
		}

		if (prefab == null) 
		{
            Trace.Error("AsynGameObject: prefab is null");
			return;
		}

		GameObject go = ResNode.InstantiateRes (prefab) as GameObject;
		go.transform.parent = gameObject.transform;
		go.transform.localScale = new Vector3 (1.0f, 1.0f, 1.0f);
		go.transform.localRotation = Quaternion.identity;
		go.transform.localPosition = new Vector3 (0.0f, 0.0f, 0.0f);
		//go.transform.rotation = gameObject.transform.rotation;
		//go.transform.SetPosition( gameObject.transform.position;

		if (callback != null) 
		{
			callback.Invoke(gameObject,null);
		}
	}

	public void ResourceLoadSucc(ResLoadResult result)
	{
        Trace.Log("LoadAsynGameObject OK, path = " + result.sPath);

		if (!CachePrefabs.Contains (result.sPath)) 
		{
			CachePrefabs[result.sPath]=result.resObj;
		}

		Perform (result.resObj as GameObject);

		DestroyImmediate(this);
	}
	// 资源加载失败回调函数
	public void ResourceLoadFail(ResLoadResult result)
	{
        Trace.Error("LoadAsynGameObject Fail, error = " + result.sErrorMsg + ",path = " + result.sPath);

		DestroyImmediate(this);
	}
}

