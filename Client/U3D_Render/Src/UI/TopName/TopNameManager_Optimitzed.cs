using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using U3D_Render;
using Scaleform;
public partial class TopNameManager 
{

    private static bool bPreWarmTopName = false;
    const int CacheCout = 160;

    /// <summary>
    /// 预热
    /// </summary>
    public static void PreWarmTopName()
    {
        if(bPreWarmTopName)
        {
            return;
        }
        bPreWarmTopName = true;
        for (int i = 0; i < CacheCout; i++)
        {
            TopName tp = CreateTopNameInstance();
            tp.Init(CacheTopNameRoot, -1, "temp", Color.white);
            tp.CreateBillBorad();
            tp.ChangeParent(CacheTopNameRoot.transform);
            tp.SetEnable(false);
            m_CacheTopName.Enqueue(tp);
        }
    }


    /// <summary>
    /// 取得头顶名字
    /// </summary>
    /// <param name="moveNode"></param>
    /// <returns></returns>
    static TopName GetTopName(ref MovieNode moveNode)
    {
        TopName tp = null;

        if (!OptimitzedControlPlane.Instance.EnityCreateOptimize)
        {
            tp = CreateTopNameInstance();
            moveNode = new MovieNode();
            return tp;
        }

        if (m_CacheTopName.Count > 0)
        {
            tp = m_CacheTopName.Dequeue();
            tp.SetEnable(true);
            moveNode = tp.movieNode;
        }
        else
        {
            if (!AllocTopNamePos(ref moveNode))
            {
                //Debug.LogWarning("Movie空间不足，最多只能容纳:" + MaxColCount * MaxRowCount + "个头顶名!!");
                return null;
            }
            tp = CreateTopNameInstance();


        }
        return tp;
    }


    /// <summary>
    /// 创建实例
    /// </summary>
    /// <returns></returns>
    static TopName CreateTopNameInstance()
    {
        Value ins = new Value();
        if (!TopNameMovie.Instance.AttachMovie(ref ins, LableAS3RefernceName, LableAS3RefernceName + System.DateTime.Today.Millisecond, -1))
        {
            Trace.LogError("Flash -- Lable创建失败！");
            ins = null;
            return null;
        }

        TopName tp = new TopName(ins);
        return tp;
    }

    /// <summary>
    /// 初始化缓存
    /// </summary>
    public static void InitCache()
    {
        if (!CacheTopNameRoot)
        {
            CacheTopNameRoot = new GameObject();
            CacheTopNameRoot.name = "TopNameCacheRoot";
            GameObject.DontDestroyOnLoad(CacheTopNameRoot);
        }
    }


    /// <summary>
    /// 清楚缓存
    /// </summary>
    static void ClearCache()
    {
        foreach (TopName tp in m_CacheTopName)
        {
            Destroy(tp);
        }
        GameObject.DestroyImmediate(CacheTopNameRoot);
        bPreWarmTopName = false;
    }


    /// <summary>
    /// 缓存
    /// </summary>
    /// <param name="tp"></param>
    static void CacheTopName(TopName tp)
    {
        if (!OptimitzedControlPlane.Instance.EnityCreateOptimize || null == tp)
        {
            return;
        }

        int id = tp.ID;
        if (m_TopNameMap.ContainsKey(id))
        {
            tp.SetEnable(false);
            tp.ChangeParent(CacheTopNameRoot.transform);
            m_CacheTopName.Enqueue(tp);
            m_TopNameMap.Remove(id);
        }

    }


    private static GameObject CacheTopNameRoot = null;
    private static Queue<TopName> m_CacheTopName = new Queue<TopName>();
}
