using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Scaleform;
using GameLogic;
using U3D_Render;
using War;
/// <summary>
/// Blood优化部分
/// </summary>
public partial class GfxBloodManager
{
    /// <summary>
    /// 缓存数目
    /// </summary>
    const int MonsterCacheCout = 160;
    const int PlayerCacheCout = 10;
    const int TankCacheCout = 0;
    static bool bPreWarmBlood = false;
    public static void PreWarmBlood()
    {
        if(bPreWarmBlood)
        {
            return;
        }
        bPreWarmBlood = true;
        Queue<GfxBlood> MonsterList = m_CacheBlood[ENTITY_TYPE.TYPE_MONSTER];
        for (int i = 0; i < MonsterCacheCout; i++)
        {
            MovieNode moveNode = new MovieNode();
            if (!AllocBloodPos(ENTITY_TYPE.TYPE_MONSTER, ref moveNode))
            {
                //Debug.LogWarning("Movie空间不足，最多只能容纳:" + MaxColCount * MaxRowCount + "个血条!!");
                continue;
            }
            GfxBlood gb = CreateBloodInstance(MonsterBloodAS3RefrenceName);
            gb.Init(-1, CacheBloodRoot, -1, ENTITY_TYPE.TYPE_MONSTER);
            gb.CreateBillBorad();
            gb.ChangeParent(CacheBloodRoot.transform);
            gb.SetEnable(false);
            gb.ChangeMovieNode(moveNode);

            MonsterList.Enqueue(gb);
        }

        Queue<GfxBlood> PlayerList = m_CacheBlood[ENTITY_TYPE.TYPE_PLAYER_ROLE];
        for (int i = 0; i < PlayerCacheCout; i++)
        {
            MovieNode moveNode = new MovieNode();
            if (!AllocBloodPos(ENTITY_TYPE.TYPE_PLAYER_ROLE, ref moveNode))
            {
                //Debug.LogWarning("Movie空间不足，最多只能容纳:" + MaxColCount * MaxRowCount + "个血条!!");
                continue;
            }

            GfxBlood gb = CreateBloodInstance(PlayerBloodAS3RefrenceName);
            gb.Init(-1, CacheBloodRoot, -1, ENTITY_TYPE.TYPE_PLAYER_ROLE);

            gb.SetMaxValue(100);
            gb.SetCurValue(100);
            gb.CreateBillBorad();
            gb.ChangeParent(CacheBloodRoot.transform);
            gb.SetEnable(false);
            gb.ChangeMovieNode(moveNode);
            PlayerList.Enqueue(gb);
        }

        Queue<GfxBlood> TankList = m_CacheBlood[ENTITY_TYPE.TYPE_TANK];
        for (int i = 0; i < TankCacheCout; i++)
        {
            MovieNode moveNode = new MovieNode();
            if (!AllocBloodPos(ENTITY_TYPE.TYPE_TANK, ref moveNode))
            {
                //Debug.LogWarning("Movie空间不足，最多只能容纳:" + MaxColCount * MaxRowCount + "个血条!!");
                continue;
            }
            GfxBlood gb = CreateBloodInstance(BuildingsBloodAS3RefrenceName);
            gb.Init(-1, CacheBloodRoot, -1, ENTITY_TYPE.TYPE_TANK);
            gb.SetMaxValue(100);
            gb.SetCurValue(100);
            gb.CreateBillBorad();
            gb.ChangeParent(CacheBloodRoot.transform);
            gb.SetEnable(false);
            gb.ChangeMovieNode(moveNode);
            TankList.Enqueue(gb);
        }
    }


    /// <summary>
    /// 获取血条
    /// </summary>
    /// <param name="entity_Type"></param>
    /// <param name="isHero"></param>
    /// <param name="moveNode"></param>
    /// <returns></returns>
    static GfxBlood GetBlood(ENTITY_TYPE entity_Type,bool isHero,ref MovieNode moveNode)
    {
        GfxBlood gb = null;
        string name = m_entiyTypeMap[entity_Type];
        if (isHero) //主角直接创建
        {
            if (!AllocBloodPos(entity_Type, ref moveNode))
            {
                //Debug.LogWarning("Movie空间不足，最多只能容纳:" + MaxColCount * MaxRowCount + "个血条!!");
                return null;
            }

            name = PlayerSelfBloodAS3RefrenceName;
            gb = CreateBloodInstance(name);

            return gb;
        }

        if(!OptimitzedControlPlane.Instance.EnityCreateOptimize)
        {
            if (!AllocBloodPos(entity_Type, ref moveNode))
            {
                //Debug.LogWarning("Movie空间不足，最多只能容纳:" + MaxColCount * MaxRowCount + "个血条!!");
                return null;
            }
            gb = CreateBloodInstance(name);
            return gb;
        }
        
        if(!m_CacheBlood.ContainsKey(entity_Type))
        {
            moveNode = new MovieNode();
            return gb;
        }
        else
        {
            Queue<GfxBlood> m_CacheList = m_CacheBlood[entity_Type];
            if(m_CacheList.Count > 0)
            {
                gb = m_CacheList.Dequeue();
                gb.SetEnable(true);
                moveNode = gb.movieNode;
            }
            else
            {
                if (!AllocBloodPos(entity_Type, ref moveNode))
                {
                    //Debug.LogWarning("Movie空间不足，最多只能容纳:" + MaxColCount * MaxRowCount + "个血条!!");
                    return null;
                }
                gb = CreateBloodInstance(name);


            }
        }
        return gb;
    }


    /// <summary>
    /// 初始化缓存
    /// </summary>
    static void InitCache()
    {
        m_CacheBlood.Add(ENTITY_TYPE.TYPE_MONSTER, new Queue<GfxBlood>());
        m_CacheBlood.Add(ENTITY_TYPE.TYPE_PLAYER_ROLE, new Queue<GfxBlood>());
        m_CacheBlood.Add(ENTITY_TYPE.TYPE_TANK, new Queue<GfxBlood>());
        if(!CacheBloodRoot)
        {
            CacheBloodRoot = new GameObject();
            CacheBloodRoot.name = "BloodCacheRoot";
            GameObject.DontDestroyOnLoad(CacheBloodRoot);
        }
    }


    /// <summary>
    /// 清除缓存
    /// </summary>
    static void ClearCache()
    {
        foreach(Queue<GfxBlood> lis in m_CacheBlood.Values)
        {
            foreach(GfxBlood gb in lis)
            {
                Destroy(gb);
            }
        }
        GameObject.DestroyImmediate(CacheBloodRoot);
        bPreWarmBlood = false;
    }


    /// <summary>
    /// 创建血条实例
    /// </summary>
    /// <param name="AS3ReferenceName"></param>
    /// <returns></returns>
    static GfxBlood CreateBloodInstance(string AS3ReferenceName)
    {
        Value ins = new Value();
        if (!GfxBloodMovie.Instance.AttachMovie(ref ins, AS3ReferenceName, "Blood-" + System.DateTime.Today.Millisecond, -1))
        {
            Trace.LogError("Flash -- Blood创建失败！");
            ins = null;
            return null;
        }
        GfxBlood gb = new GfxBlood(ins);
        return gb;
    }


    /// <summary>
    /// 缓存血条
    /// </summary>
    /// <param name="b"></param>
    static void CacheBlood(GfxBlood b)
    {
        if (!OptimitzedControlPlane.Instance.EnityCreateOptimize || null == b)
        {
            return ;
        }

       
        ENTITY_TYPE entity_Type = b.bloodType; 
        if (!m_CacheBlood.ContainsKey(entity_Type))
        {
            Debug.LogError("缓存" + entity_Type + "类型血条失败，请在InitCache中添加!");
            return ;
        }
        int id = b.ID;
        //不存主角
        if(id == EntityFactory.MainHeroID)
        {
            return;
        }
        if (m_BloodMap.ContainsKey(id))
        {
            b.SetEnable(false);
            b.SetBloodColor(false);
            b.ChangeParent(CacheBloodRoot.transform);
            Queue<GfxBlood> m_list = m_CacheBlood[entity_Type];
            m_list.Enqueue(b);
            m_BloodMap.Remove(id);
        }
    }


    private static GameObject CacheBloodRoot = null;
    private static Dictionary<ENTITY_TYPE, Queue<GfxBlood>> m_CacheBlood = new Dictionary<ENTITY_TYPE,Queue<GfxBlood>>();
}
