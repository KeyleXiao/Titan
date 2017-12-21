/*******************************************************************
** 文件名:	PrefabManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/3
** 版  本:	1.0
** 描  述:	预制体管理器
** 应  用:   用于管理所有功能性的预制体，预制体中通常不会包括皮肤。就是说没有模型，只有代码
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

/// <summary>
/// 预制体管理器，负责所有预制体的加载,预制体中通常不会包括皮肤，就是说没有模型，只有代码
/// 预制体和皮肤分开，所以预制体是通用的，即所有怪物用一个怪物的预制体，所有的人物用一个人物的预制体
/// </summary>
public sealed class PrefabManager
{
    class PrefabNode
    {
        public ENTITY_TYPE m_Type = ENTITY_TYPE.TYPE_MONSTER;
        public Queue<GameObject> m_PrefabList = new Queue<UnityEngine.GameObject>();
        public ResNode OrginalObj = null;

        public void Clear()
        {
            AssetBundleManager.DeleteAssets(ref OrginalObj,false);
            m_PrefabList.Clear();
            OrginalObj = null;
        }

        public GameObject Instantiate(ENTITY_TYPE entityType)
        {
            if (OrginalObj != null)
            {
                GameObject o = OrginalObj.InstanceMainRes();
                if (o)
                {
                    o.name = entityType.ToString() + "--cache";
                    o.transform.SetParent(PrefabCacheRoot.transform);
                    o.SetActive(false);
                }
                return o;
            }

            return null;

        }
    }

    private static bool bInit = false;
    private const string EntityPrefabPatch = "EntityPrefab";

    /// <summary>
    /// 怪物初始缓存多少个
    /// </summary>
    private const int monsterInitCacheCount = 200;

    /// <summary>
    /// 人物初始缓存多少个
    /// </summary>
    private const int playerInitCacheCount = 10;

    private static GameObject PrefabCacheRoot = null;

    public static void Init()
    {
        if (bInit)
        {
            return;
        }

        if (!PrefabCacheRoot)
        {
            PrefabCacheRoot = new GameObject();
            PrefabCacheRoot.name = "PrefabCacheRoot";
            UnityEngine.Object.DontDestroyOnLoad(PrefabCacheRoot);
        }
        LoadBuilt_InEntityPrefab();

        bInit = true;
    }

    public static void Clear()
    {
        foreach (PrefabNode pn in m_PrefabMap.Values)
        {
            pn.Clear();
        }
        //清掉引用，再调用UnLoad就行了
        m_PrefabMap.Clear();
        ResNode.DestroyRes(ref PrefabCacheRoot);
        Resources.UnloadUnusedAssets();
    }


    /// <summary>
    /// 加载内置的实体预制体，也就是EntityPrefab文件夹下的预制体
    /// </summary>
    /// <returns>是否成功</returns>
    public static void LoadBuilt_InEntityPrefab()
    {
        LoadPrefab(ENTITY_TYPE.TYPE_MONSTER, "EntityPrefab/MonsterPrefab");
        LoadPrefab(ENTITY_TYPE.TYPE_PLAYER_ROLE, "EntityPrefab/PlayerPrefab");
    }

    /// <summary>
    /// 加载预制体
    /// </summary>
    /// <param name="entityType">实体类型(是怪物，还是人物..)</param>
    /// <param name="strPrefabPath">模型路径</param>
    /// <returns>是否成功</returns>
    public static bool LoadPrefab(ENTITY_TYPE entityType, string strPrefabPath)
    {
        ResNode obj = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,strPrefabPath);
        if (obj != null)
        {
            if (m_PrefabMap.ContainsKey(entityType))
            {
                return true;
            }

            InsertToMap(obj, entityType);

            return true;
        }
        else
        {
            Trace.LogError("加载预制体失败,类型:" + entityType + ",路径:" + strPrefabPath);
            return false;
        }

    }

    private static void InsertToMap(ResNode obj, ENTITY_TYPE entityType)
    {
        PrefabNode prefabNode = new PrefabNode();
        prefabNode.OrginalObj = obj;

        int cacheCount = 0;
        //如果是怪物，缓存
        if (entityType == ENTITY_TYPE.TYPE_MONSTER)
        {
            cacheCount = monsterInitCacheCount;
        }
        else
        {
            cacheCount = playerInitCacheCount;
        }

        for (int i = 0; i < cacheCount; i++)
        {
            prefabNode.m_PrefabList.Enqueue(prefabNode.Instantiate(entityType));

        }
        prefabNode.m_Type = entityType;
        m_PrefabMap.Add(entityType, prefabNode);
        obj = null;
    }

    /// <summary>
    /// 取得预制体，失败返回null
    /// </summary>
    /// <param name="entityType">实体类型(是怪物，还是人物..)</param>
    /// <returns></returns>
    public static GameObject GetPrefab(ENTITY_TYPE entityType)
    {
        if (!bInit)
        {
            return null;
        }

        if (m_PrefabMap.ContainsKey(entityType))
        {
            PrefabNode prefabNode = m_PrefabMap[entityType];
            if(prefabNode.m_PrefabList.Count > 0)
            {
                GameObject go = prefabNode.m_PrefabList.Dequeue();
                go.SetActive(true);
                go.transform.SetParent(null);
                go.name = entityType.ToString();
                return go;
            }
            else
            {
                GameObject go = prefabNode.Instantiate(entityType);
                go.SetActive(true);
                go.transform.SetParent(null);
                go.name = entityType.ToString();
                return go;
            }
        }
        return null;
    }

    public static void CachePrefab(ENTITY_TYPE entityType,GameObject prefab)
    {
        if (!bInit)
        {
            return;
        }

        //如果还有子节点，删一下，避免带给下一个实体
        if(prefab != null && prefab.transform.childCount > 0)
        {
            for (int i = prefab.transform.childCount - 1; i >= 0; i--)
            {
                UnityEngine.Object.Destroy(prefab.transform.GetChild(i).gameObject);
            }
        }
        
        if (m_PrefabMap.ContainsKey(entityType))
        {
            prefab.name = entityType.ToString() + "--cache";
            PrefabNode prefabNode = m_PrefabMap[entityType];
            prefab.SetActive(false);
            prefab.transform.SetParent(PrefabCacheRoot.transform);
           // prefab.transform.localPosition = Vector3.zero;
            prefabNode.m_PrefabList.Enqueue(prefab);
        }
    }

    private static Dictionary<ENTITY_TYPE, PrefabNode> m_PrefabMap = new Dictionary<ENTITY_TYPE, PrefabNode>();
}
