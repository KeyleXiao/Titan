/*******************************************************************
** 文件名:	ResNode.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	资源结点
** 应  用:  	资源结点类，所有的资源加载都是这个

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

public sealed partial class ResNode 
{
    public HashSet<GameObject> GetAllInstanceResList
    {
        get
        {
            return AllInstanceResList;
        }
    }

    public HashSet<GameObject> GetActiveInstanceResList
    {
        get
        {
            return ActiveInstanceResList;
        }
    }

    public Queue<GameObject> GetUnActiveInstanceResList
    {
        get
        {
            return UnActiveInstanceResList;
        }
    }

    private HashSet<GameObject> AllInstanceResList = new HashSet<GameObject>();

    private HashSet<GameObject> ActiveInstanceResList = new HashSet<GameObject>();

    private Queue<GameObject> UnActiveInstanceResList = new Queue<GameObject>();

    /// <summary>
    /// 所有的实例化物体和他所属的ResNode
    /// </summary>
    private static Dictionary<UnityEngine.Object, ResNode> IntanceResNodeTable = new Dictionary<UnityEngine.Object, ResNode>();

    public bool bSavedMemory = false;

    private static bool openlog = false;
    public static void ClearWhenGameEnd()
    {
        IntanceResNodeTable.Clear();
    }

    /// <summary>
    /// 实例化资源
    /// </summary>
    /// <param name="go">源物体</param>
    /// <param name="useSource">使用源物体，不会去缓存池中查找，等于原来的 GameObject.Instantiate</param>
    /// <returns></returns>
    public static T InstantiateRes<T>(UnityEngine.Object go, bool useSource = false) where T : MonoBehaviour
    {
        if (!go)
        {
            return null;
        }

        if(!useSource)
        {
            ResNode res = null;
            if (IntanceResNodeTable.TryGetValue(go, out res))
            {
                return res.InstanceMainRes<T>();
            }
        }

        GameObject temp = GameObject.Instantiate(go) as GameObject;
        if(temp)
        {
            return temp.GetComponent<T>();
        }
        return null;
    }

    /// <summary>
    /// 实例化资源
    /// </summary>
    /// <param name="go">源物体</param>
    /// <param name="useSource">使用源物体，不会去缓存池中查找，等于原来的 GameObject.Instantiate</param>
    /// <returns></returns>
    public static GameObject InstantiateRes(UnityEngine.Object go,bool useSource = false)
    {
        if(!go)
        {
            return null;
        }
        if(!useSource)
        {
            ResNode res = null;
            if (IntanceResNodeTable.TryGetValue(go, out res))
            {
                return res.InstanceMainRes();
            }
        }

        return GameObject.Instantiate(go) as GameObject;
    }


    public static void DestroyRes<T>(ref T obj) where T : UnityEngine.Object
    {
        if(!obj)
        {
            return;
        }
        ResNode res = null;
        if (IntanceResNodeTable.TryGetValue(obj, out res))
        {
            GameObject go = obj as GameObject;
            res.DestroyInstanceRes(ref go);
            obj = null;
            return ;
        }
        UnityEngine.Object.Destroy(obj);
        obj = null;
    }

    private void SaveAssetInfo()
    {
        if (!IsValid())
        {
            return;
        }

        GameObject go = assetObj as GameObject;
        if(!go)
        {
            return;
        }
        RectTransform rtf = go.transform as RectTransform;
        if(rtf)
        {
            RectTransformRect = rtf.rect;
            anchoredPosition = rtf.anchoredPosition;
            sizeDelta = rtf.sizeDelta;
        }

        localPosition = go.transform.localPosition;
        localRoation = go.transform.localRotation;
        localScale = go.transform.localScale;

    }

    public void DestroyInstanceRes(ref GameObject []list)
    {

        if (assetType != AssetType.Asset_Prefab)
        {
            return;
        }

        if (list == null || list.Length <= 0)
        {
            return;
        }

        foreach (GameObject go in list)
        {
            GameObject temp = go;
            DestroyInstanceRes(ref temp);
        }
        list = null;
    }

    public void DestroyInstanceRes(ref List< GameObject> list)
    {
        
        if (assetType != AssetType.Asset_Prefab)
        {
            return;
        }

        if (list == null || list.Count <= 0)
        {
            return;
        }

        foreach(GameObject go in list)
        {
            GameObject temp = go;
            DestroyInstanceRes(ref temp);
        }
        list.Clear();
    }

    public void DestroyInstanceRes(ref GameObject obj)
    {
        if (assetType != AssetType.Asset_Prefab || m_bCleared)
        {
            return;
        }
        if (!obj)
        {
            return;
        }

        if (!AllInstanceResList.Contains(obj))
        {
            Debug.LogWarning(StringHelper.BuildString("需要销毁的实例不是由本资源结点产生的:", obj.name, ",资源结点名:", AssetName));
            return;
        }

        //if(!ActiveInstanceResList.Contains(go))
        //{
        //    return;
        //}

        if (!AssetBundleManager.HostGameObject)
        {
            return;
        }

        obj.SetActive(false);
        obj.transform.SetParent(AssetBundleManager.HostGameObject.transform);
        if (!UnActiveInstanceResList.Contains(obj))
        {
            UnActiveInstanceResList.Enqueue(obj);
        }

        if (ActiveInstanceResList.Contains(obj))
        {
            ActiveInstanceResList.Remove(obj);
        }
    }

    public void DestroyInstanceRes<T>(ref T obj) where T:UnityEngine.Component
    {
        if(assetType !=  AssetType.Asset_Prefab || m_bCleared)
        {
            return;
        }
        if(!obj)
        {
            return;
        }

        GameObject go = obj.gameObject;
        obj = null;
        if(!go)
        {
            return;
        }

        if (!AllInstanceResList.Contains(go))
        {
            Debug.LogWarning(StringHelper.BuildString("需要销毁的实例不是由本资源结点产生的:",go.name,",资源结点名:",AssetName));
            return;
        }

        //if(!ActiveInstanceResList.Contains(go))
        //{
        //    return;
        //}

        if (!AssetBundleManager.HostGameObject)
        {
            return;
        }

        go.SetActive(false);
        go.transform.SetParent(AssetBundleManager.HostGameObject.transform);
        
        go.SetActive(false);
        if(!UnActiveInstanceResList.Contains(go))
        {
            UnActiveInstanceResList.Enqueue(go);
        }

        if (ActiveInstanceResList.Contains(go))
        {
            ActiveInstanceResList.Remove(go);
        }
    }

    public void DestroyAllInstanceRes()
    {
        if (assetType != AssetType.Asset_Prefab)
        {
            return;
        }

        if(ActiveInstanceResList.Count > 0)
        {
            List<GameObject> tempList = new List<GameObject>();
            tempList.AddRange(ActiveInstanceResList);

            int cou = tempList.Count;
            for (int i = 0; i < cou; i++)
            {
                GameObject temp = tempList[i];
                DestroyInstanceRes(ref temp);
            }
        }

    }

    /// <summary>
    /// 目前只给光效预制体使用
    /// </summary>
    public void SaveMemory()
    {
        if (assetType != AssetType.Asset_Prefab || bSavedMemory)
        {
            return;
        }

        if(assetObjList.Count <= 0)
        {
            return;
        }

        List<UnityEngine.Object> tempList = new List<UnityEngine.Object>();
        tempList.AddRange(assetObjList);
        int count = assetObjList.Count;
        for (int i = 0; i < count; i++)
        {
            GameObject go = GameObject.Instantiate(assetObjList[i]) as GameObject;
            assetObjList[i] = go;
            UnActiveInstanceResList.Enqueue(go);
            AllInstanceResList.Add(go);
            IntanceResNodeTable.Add(go, this);
            GameObject.DontDestroyOnLoad(go);
            go.transform.SetParent(AssetBundleManager.HostGameObject.transform);
            go.SetActive(false);

            serialzedDataList = new List<object>();
            ISaveSerializedData []seriallist = go.GetComponentsInChildren<ISaveSerializedData>(true);
            foreach(ISaveSerializedData ser in seriallist)
            {
                serialzedDataList.Add(ser.SaveSerializedData());
            }
           
        }
        assetObj = assetObjList[0];

        count = tempList.Count;
        for (int i = 0; i < count; i++)
        {
            GameObject.DestroyImmediate(tempList[i], true);
        }
        tempList.Clear();
        bSavedMemory = true;
    }

    private void ApplySerializedData(GameObject go)
    {
        if(null == serialzedDataList)
        {
            return;
        }
        ISaveSerializedData[] seriallist = go.GetComponentsInChildren<ISaveSerializedData>(true);
        if (seriallist.Length != serialzedDataList.Count)
        {
            Debug.LogWarning("ApplySerializedData faild - data not match:" + AssetName);
            return;
        }
        for (int i = 0; i < seriallist.Length;i++ )
        {
            seriallist[i].SetSerializedData(serialzedDataList[i]);
        }
    }

    private void ClearInstanceRes()
    {
        foreach(GameObject go in AllInstanceResList)
        {
            if (go)
            {
                IntanceResNodeTable.Remove(go);
                GameObject.Destroy(go);
            }
            else
            {
                if(openlog)
                {
                    Debug.LogError(StringHelper.BuildString("在资源管理器外删除了资源实例(比较大的原因是删除了父节点),这个资源释放可能会失败，由资源管理器实例化的资源需调用ResNode.DestroyRes(ref )来删除:", AssetName, ",资源类型:", assetType));
                }
               
            }
        }
        if(serialzedDataList != null)
        {
            serialzedDataList.Clear();
            serialzedDataList = null;
        }

        ActiveInstanceResList.Clear();
        UnActiveInstanceResList.Clear();
        AllInstanceResList.Clear();
    }

    public GameObject InstanceMainRes()
    {
        return InstanceRes(assetObj);
    }

    public T InstanceMainRes<T>() where T:Behaviour
    {
        GameObject res = InstanceRes(assetObj);
        if (res)
        {
            T t = res.GetComponent<T>();
            return t;
        }
        DestroyInstanceRes(ref res);
        return null;
    }

    public List<GameObject> InstanceAllRes()
    {
        List<GameObject> reslut = new List<GameObject>();
        if (!IsValid())
        {
            return reslut;
        }

        for (int i = 0; i < assetObjList.Count;i++)
        {
            GameObject res = InstanceRes(assetObjList[i]);
            if(res)
            {
                reslut.Add(res);
            }
            else
            {
                DestroyInstanceRes(ref res);

            }
        }
        return reslut;
    }

    public List<T> InstanceAllRes<T>() where T : MonoBehaviour
    {

        List<T> reslut = new List<T>();
        if (!IsValid())
        {
            return reslut;
        }
        List<GameObject> temp = InstanceAllRes();
        for (int i = 0; i < temp.Count; i++)
        {
            GameObject res = temp[i];
            if (res)
            {
                T t = res.GetComponent<T>();
                if(t)
                {
                    reslut.Add(t);
                }
                else
                {
                    DestroyInstanceRes(ref res);
                }
            }
        }
        temp.Clear();
        return reslut;
    }

    private GameObject InstanceNewRes(UnityEngine.Object source)
    {
        GameObject sour = (GameObject)source;
        GameObject res = GameObject.Instantiate(sour);
        if (!res)
        {
            Debug.LogWarning(AssetName + "资源实例化失败!");
            return null;
        }
        res.SetActive(true);
        ApplySerializedData(res);

        UnityEngine.Object.DontDestroyOnLoad(res);
        AllInstanceResList.Add(res);
        IntanceResNodeTable.Add(res, this);
        return res;
    }

    private GameObject InstanceRes(UnityEngine.Object source)
    {
        if (m_bCleared)
        {
            Debug.LogWarning("已被清理");
            return null;
        }
        if (assetType != AssetType.Asset_Prefab)
        {
            Debug.LogWarning(assetType + "不支持实例化!");
            return null;
        }
        if (!isLoadFinish && isAsync)
        {
            Debug.LogWarning(AssetName + "没有被加载，请加载后再调用");
            return null;
        }
        if (!source)
        {
            Debug.LogWarning(StringHelper.BuildString(AssetName ,"资源在资源管理器外被销毁，请不要在资源管理器外销毁资源，应当使用DestroyInstanceRes函数来销毁资源!"));
            return null;
        }
        GameObject res = null;
        if(UnActiveInstanceResList.Count > 0)
        {
            res = UnActiveInstanceResList.Dequeue();
            //如果在实例在外面被删除了，循环取，还是取不到就新建一个
            while (true && !res)
            {
                if(UnActiveInstanceResList.Count <= 0)
                {
                    break;
                }
                res = UnActiveInstanceResList.Dequeue();
            }

            if(!res)
            {
                res = InstanceNewRes(source);
            }
            else
            {
                res.SetActive(true);
            }
            
            res.transform.SetParent(null);
            ResetTransform(res);
        }
        else
        {
            res = InstanceNewRes(source);
        }
       
        ActiveInstanceResList.Add(res);
        return res;
    }
}