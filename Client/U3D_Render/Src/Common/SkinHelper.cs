/*******************************************************************
** 文件名:	SkinHelper.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2016-8-15
** 版  本:	1.0
** 描  述:	预加载皮肤辅助类

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameLogic;



public class SkinHelper
{
    public static List<SkinPreLoadInfo> PreLoadPlayerSkin(SkinPreLoadInfo skinInfo)
    {
        List<SkinPreLoadInfo> pushedSkinID = new List<SkinPreLoadInfo>();

        // 加载主皮肤
        SkinManager.PushCacheSkinID(skinInfo);
        pushedSkinID.Add(skinInfo);

        // 加载关联皮肤
        int[] nRelateSkinID = ASpeedGame.Data.PreLoadSkinLayout.PreLoadSkinLayout.Instance.GetRelateSkinID(skinInfo.skinID);
        if (nRelateSkinID != null)
        {
            for (int j = 0; j < nRelateSkinID.Length; ++j)
            {
                if (nRelateSkinID[j] > 0)
                {
                    SkinPreLoadInfo instance = new SkinPreLoadInfo();
                    instance.skinID = nRelateSkinID[j];
                    instance.nMask = skinInfo.nMask;
                    SkinManager.PushCacheSkinID(instance);

                    pushedSkinID.Add(instance);
                }
            }
        }

        return pushedSkinID;
    }

    public static List<SkinPreLoadInfo> PreLoadMonsterSkin(int MapID)
    {
        List<SkinPreLoadInfo> pushedSkinID = new List<SkinPreLoadInfo>();


        // 加载关联皮肤
        int[] nRelateSkinID = ASpeedGame.Data.PreloadMonsterRes.PreloadMonsterRes.Instance.GetSkinIDs(MapID);
        if (nRelateSkinID != null)
        {
            for (int j = 0; j < nRelateSkinID.Length; ++j)
            {
                if (nRelateSkinID[j] > 0)
                {
                    SkinPreLoadInfo instance = new SkinPreLoadInfo();
                    instance.skinID = nRelateSkinID[j];
                    instance.nMask = (int)CachePriority.PRIORITY_LOW;
                    SkinManager.PushCacheSkinID(instance);
                    pushedSkinID.Add(instance);
                }
            }
        }

        return pushedSkinID;
    }
}