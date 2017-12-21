/*******************************************************************
** 文件名:	LightingEffectManager_Optimitzed.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	光效管理
** 应  用:   光效管理，优化部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using System;
using UnityEngine;
using System.Collections;
using System.Collections.Generic;


#if USE_TIMMER_BEHAVIOUR
public partial class LightingEffectManager : UpdateBehaviourOnTimer
#else
public partial class LightingEffectManager : MonoBehaviour
#endif
{
    #region 延迟创建光效相关

    private class EffectCreateInfo
    {
        public int nID = -1;
        public LightingEffectManager hostInstance = null;
        public LightEffectParam param = null;
    }

    /// <summary>
    /// 需要创建的列表
    /// </summary>
    private static Queue<EffectCreateInfo> WillCreateList = new Queue<EffectCreateInfo>();

    /// <summary>
    /// 已经创建的列表，回收利用
    /// </summary>
    private static Queue<EffectCreateInfo> CreatedList = new Queue<EffectCreateInfo>();

    /// <summary>
    /// 每帧最多创建多少个
    /// </summary>
    private const int MaxCreateOnPreFrame = 2;
    private static int CurrentCreateCount = 0;
    static EffectCreateInfo GetInvaildInfo()
    {
        EffectCreateInfo info = null;
        if(CreatedList.Count > 0)
        {
            info = CreatedList.Dequeue();
        }
        else
        {
            info = new EffectCreateInfo();
        }
        
        return info;
    }

    public static void UpdateCreateList()
    {
        if (WillCreateList.Count <= 0)
        {
            return;
        }

        for (int i = 0; i < MaxCreateOnPreFrame;i++ )
        {
            if (WillCreateList.Count <= 0)
            {
                break;
            }

            EffectCreateInfo info = WillCreateList.Dequeue();
            if(null != info)
            {
                info.hostInstance.AddLightingImpl(info.nID, info.param);
                CreatedList.Enqueue(info);
            }
        }
        CurrentCreateCount = 0;
    }

    public LightingEffectBehavior AddLighting(int nID, LightEffectParam param)
    {
        //打开开关，并且有预制体的，并且不是主角的，我们才存起来
        if (!param.bIsHero)
        {
            //如果需要创建的数量比每帧需要创建的要多,一次性最多只能创建MaxCreateOnPreFrame
            if (WillCreateList.Count > MaxCreateOnPreFrame && CurrentCreateCount < MaxCreateOnPreFrame)
            {
                EffectCreateInfo info = GetInvaildInfo();
                info.nID = nID;
                info.param = param;
                info.hostInstance = this;
                WillCreateList.Enqueue(info);
                return null;
            }
            else
            {
                CurrentCreateCount++;
                return AddLightingImpl(nID, param);
            }
        }
        else
        {
            return AddLightingImpl(nID, param);
        }
    }

    private void clearEffect()
    {
        clearEffectImpl();
        CreatedList.Clear();
        WillCreateList.Clear();
        CurrentCreateCount = 0;
        
    }
    #endregion

}
