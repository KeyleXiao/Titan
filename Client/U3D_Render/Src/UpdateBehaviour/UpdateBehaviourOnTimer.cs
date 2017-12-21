/*******************************************************************
** 文件名:	UpdateBehaviourOnTimmer.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	梁成
** 日  期:	2016-2-2
** 版  本:	1.0
** 描  述:	控制MonoBehaviour的更新行为

 * 1、跟时间相关
 * 2、
 * 3、只实现了Update()和LateUpdate()。FixUpdate()和OnGUI()未实现，避免空调用，如需要可参照LateUpdate()实现
********************************************************************/
using UnityEngine;
using System;


public class UpdateBehaviourOnTimer : UpdateBehaviour
{

    public const uint TIMER_THREAD_HIGH = 10;
    public const uint TIMER_THREAD_MID = 20;
    public const uint TIMER_THREAD_LOW = 50;

    UpdateBehaviourOnTimerManager manager = null;
    
    private uint m_indexOnLayer = 0;
    protected int layer = 0;
    private uint lastUpdateTick = 0;
    private uint m_frameTickThread = 50; //tick thread of each frame for all components in this layer

    protected void startTimmer(int nLayer,uint frameTickThread)
    {
        layer = nLayer;
        manager = UpdateBehaviourOnTimerManager.Instance;
        m_indexOnLayer = 0;
        if (manager)
        {
            m_indexOnLayer = manager.applyIndexOnLayer(layer);
        }
        lastUpdateTick = 0;
        m_frameTickThread = frameTickThread;
    }

    public void OnDestroy()
    {
        if (manager)
        {
            manager.componentDestory(layer,m_indexOnLayer);
        }
        OnFrameDestroy();
    }

    private bool needUpdate;
    public void Update()
    {
        needUpdate = true;
        uint tick = GameLogicAPI.getTickCount();
        if (manager)
        {
            UpdateBehaviourOnTimerManager.UpdateBehaviourOnTimerLayer info = manager.getLayerInfo(layer);
            if (lastUpdateTick>info.roundStartTick)
            {
                //I am already updated in this round
                needUpdate = false;
            }
            else if (tick > manager.getFrameStartTick() + m_frameTickThread)
            {
                //update time for this frame is run out, wait for the next frame
                needUpdate = false;
            }
            if (m_indexOnLayer == info.componentCount - 1)
            {
                //the component[info.componentCount] is always valid. When it is destroyed, info.componentCount--
                manager.setRoundStartTick(layer);
            }
        }
        if (needUpdate)
        {
            OnUpdate();
            lastUpdateTick = tick;
        }
    }

    public void LateUpdate()
    {
        if (needUpdate)
        {
            OnLateUpdate();
        }
    }

    /// <summary>
    /// 用于替换Update函数执行逻辑
    /// </summary>
    public override void OnUpdate()
    {
    }

    /// <summary>
    /// 用于替换LateUpdate函数执行逻辑
    /// </summary>
    public override void OnLateUpdate()
    {
    }

    ///// <summary>
    ///// 用于替换FixedUpdate函数执行逻辑
    ///// </summary>
    //public override void OnFixedUpdate()
    //{
    //}

    ///// <summary>
    ///// 用于替换OnGUI函数执行逻辑
    ///// </summary>
    //public override void OnFrameGUI()
    //{
    //}

    /// <summary>
    /// 用于替换OnDestroy函数执行逻辑
    /// </summary>
    public override void OnFrameDestroy()
    {

    }
}