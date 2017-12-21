/*******************************************************************
** 文件名:	SkinManager_Cache.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/3
** 版  本:	1.0
** 描  述:	皮肤管理器
** 应  用:   用于管理皮肤的，简单的说，就是模型了
**************************** 修改记录 ******************************
** 修改人:  昔文博
** 日  期:  2016/1/19
** 描  述:  增加缓存皮肤函数
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

public class SkinPreLoadInfo
{
    public int skinID = 0;
    public int nMask = 0;
}

/// <summary>
/// 用于管理皮肤的，简单的说，就是模型了
/// </summary>
public partial class SkinManager 
{

    class SkinPreWarmNode
    {
        private Dictionary<int, int> CacheSkinIDCountTable = new Dictionary<int, int>();
        static List<int> PreWarmFailedSkin = new List<int>();

        public void Clear()
        {
            CacheSkinIDCountTable.Clear();
        }

        public bool IsPreWarmFinish()
        {
            return CacheSkinIDCountTable.Count <= 0;
        }

        public void PushPreWarm(SkinPreLoadInfo info)
        {
            if (CacheSkinIDCountTable.ContainsKey(info.skinID))
            {
                CacheSkinIDCountTable[info.skinID]++;
            }
            else
            {
                CacheSkinIDCountTable.Add(info.skinID, 1);
            }
        }

        public void PreWarm(Action<Skin> callBack)
        {
            if (CacheSkinIDCountTable.Count > 0)
            {
                foreach (KeyValuePair<int, int> skinCacheData in CacheSkinIDCountTable)
                {
                    int SkinCount = skinCacheData.Value;
                    int skinID = skinCacheData.Key;
                    if (!GetSkinAsyncImpl(skinID, callBack))
                    {
                        PreWarmFailedSkin.Add(skinID);
                        continue;
                    }
                }

                foreach (int id in PreWarmFailedSkin)
                {
                    CacheSkinIDCountTable.Remove(id);
                }
            }
        }

        public void RemovePreWarmSkin(int nSkinID)
        {
            if(CacheSkinIDCountTable.ContainsKey(nSkinID))
            {
                CacheSkinIDCountTable.Remove(nSkinID);
            }
        }
    }

    /// <summary>
    /// 当前已经使用的皮肤ID
    /// </summary>
    private static HashSet<int> currentActiveSkinList = new HashSet<int>();

    static SkinPreWarmNode FirstPart = new SkinPreWarmNode();
    static SkinPreWarmNode SecondPart = new SkinPreWarmNode();
    static SkinPreWarmNode ThirdPart = new SkinPreWarmNode();
    static SkinPreWarmNode FourPart = new SkinPreWarmNode();

    /// <summary>
    /// 清楚皮肤缓存
    /// </summary>
    public static void UnLoadAllSkin()
    {
        currentActiveSkinList.Clear();
        ClearAsyncLoadData();
        foreach(Skin temp in m_SkinMap.Values)
        {
            Skin sk = temp;
            UnLoadSkinRes(ref sk);
        }

        foreach (Skin temp in m_CacheSkinMap.Values)
        {
            Skin sk = temp;
            UnLoadSkinRes(ref sk);
        }

        m_SkinMap.Clear();
        m_CacheSkinMap.Clear();

    }

    public static void PushCacheSkinID(SkinPreLoadInfo info)
    {
        if ((info.nMask & (int)CachePriority.PRIORITY_VERY_HIGH) != 0)
        {
            FirstPart.PushPreWarm(info);
        }
        else if ((info.nMask & (int)CachePriority.PRIORITY_HIGH) != 0)
        {
            SecondPart.PushPreWarm(info);
        }
        else if ((info.nMask & (int)CachePriority.PRIORITY_MEDIUM) != 0)
        {
            ThirdPart.PushPreWarm(info);
        }
        else 
        {
            FourPart.PushPreWarm(info);
        }
    }

    public static void ClearCacheSkinID()
    {
        FirstPart.Clear();
        SecondPart.Clear();
        ThirdPart.Clear();
        FourPart.Clear();
    }

    public static bool IsSkinPreWarmFinish_FirstPart()
    {
        return FirstPart.IsPreWarmFinish();
    }

    public static bool IsSkinPreWarmFinish_SecondPart()
    {
        return SecondPart.IsPreWarmFinish();
    }

    public static bool IsSkinPreWarmFinish_ThirdPart()
    {
        return ThirdPart.IsPreWarmFinish();
    }

    public static bool IsSkinPreWarmFinish_FourPart()
    {
        return FourPart.IsPreWarmFinish();
    }

    public static void StartPreWarmSkin_FirstPart()
    {
        FirstPart.PreWarm(OnSkinLoadFinish);
    }

    public static void StartPreWarmSkin_SecondPart()
    {
        SecondPart.PreWarm(OnSkinLoadFinish);
    }

    public static void StartPreWarmSkin_ThirdPart()
    {
        ThirdPart.PreWarm(OnSkinLoadFinish);
    }

    public static void StartPreWarmSkin_FourPart()
    {
        FourPart.PreWarm(OnSkinLoadFinish);
    }

    private static void OnSkinLoadFinish(Skin sk)
    {
        if(null == sk)
        {
            return;
        }
        PreWarmSkin(sk);
    }

    private static void PreWarmSkin(Skin sk)
    {
        RemovePreWarmSkin(sk.nSkinID);

        if(!sk.isValid)
        {
            return;
        }
        sk.Clear(false);
        //非玩家比较简单，直接走缓存就可以了
        int skinCacheCout = sk.skinConfig.CacheCout;
       // if (sk.SkinType == ENTITY_TYPE.TYPE_PLAYER_ROLE && !sk.isStaticSkin)
        {
            skinCacheCout = 1;
        }

        for (int i = 0; i < skinCacheCout; i++)
        {
            SkinInstance go = sk.Instantiate_Main();
            if (go != null)
            {
                go.SkinObj.transform.SetPosition(Initialize.mainCam.transform.forward * 20.0f + Initialize.mainCam.transform.position);
            }
        }
    }

    public static void EndPreWarmSkin()
    {
        if (m_SkinMap.Count > 0)
        {
            foreach (Skin sk in m_SkinMap.Values)
            {
                if(!sk.skinConfig)
                {
                    continue;
                }

                if(currentActiveSkinList.Contains(sk.nSkinID))
                {
                    foreach (SkinInstance go in sk.AllSkin)
                    {
                        if (go != null)
                        {
                            if(!go.ReferencedSkinCtrl)
                            {
                                CacheSkinInstance(go);
                            }
                        }
                    }
                    continue;
                }
                foreach (SkinInstance go in sk.AllSkin)
                {
                    if (go != null)
                    {
                        CacheSkinInstance(go);
                    }
                }
            }
        }

    }

    static void RemovePreWarmSkin(int nSkinID)
    {
        FirstPart.RemovePreWarmSkin(nSkinID);
        SecondPart.RemovePreWarmSkin(nSkinID);
        ThirdPart.RemovePreWarmSkin(nSkinID);
        FourPart.RemovePreWarmSkin(nSkinID);
    }

    private static void UpdateSkinCache()
    {
        if(m_CacheSkinMap.Count > 0)
        {
            int_TempList.Clear();
            foreach(Skin temp in m_CacheSkinMap.Values)
            {
                if(temp.NotReleaseFromCache)
                {
                    continue;
                }
                temp.currentCacheTime += Time.deltaTime;
                if(temp.currentCacheTime > MaxCacheTime)
                {
                    int_TempList.Add(temp.nSkinID);
                    Skin sk = temp;
                    UnLoadSkinRes(ref sk);
                }
            }

            foreach(int skinID in int_TempList)
            {
                m_CacheSkinMap.Remove(skinID);
            }
        }
    }

    private static Skin GetSkinFromCache(int nSkinID)
    {
        Skin Reslut = null;
        if(m_CacheSkinMap.TryGetValue(nSkinID, out Reslut))
        {
            Reslut.currentCacheTime = 0.0f;
            m_CacheSkinMap.Remove(nSkinID);
        }
        return Reslut;
    }

    private static void PushToSkinCache(Skin sk, bool NotReleaseFromCache = false)
    {
        if(null == sk)
        {
            return;
        }

        //只要有了一次为true,就不释放，不释放的优先级高
        if(!sk.NotReleaseFromCache)
        {
            sk.NotReleaseFromCache = NotReleaseFromCache;
        }
   
        sk.currentCacheTime = 0;
        sk.OnCache();
        m_CacheSkinMap.Add(sk.nSkinID, sk);
    }

    /// <summary>
    /// 缓存的皮肤列表
    /// </summary>
    private static Dictionary<int, Skin> m_CacheSkinMap = new Dictionary<int, Skin>();
    private static List<int> int_TempList = new List<int>(20);

    private static float MaxCacheTime = 10.0f;
}


