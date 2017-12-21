/*******************************************************************
** 文件名:	UpdateBehaviourOnScreen.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	梁成
** 日  期:	2016-2-2
** 版  本:	1.0
** 描  述:	控制MonoBehaviour的更新行为

 * 1、跟屏幕相关，如果在屏幕之外则不做Update操作
 * 2、需要引用BaseStateMachine，以得知是否在屏幕内
 * 3、只实现了Update()和LateUpdate()。FixUpdate()和OnGUI()未实现，避免空调用，如需要可参照LateUpdate()实现
********************************************************************/
using UnityEngine;
using System;


public class UpdateBehaviourOnScreen : UpdateBehaviour
{
    BaseStateMachine machine = null;

    public void Start()
    {
        machine = GetComponent<BaseStateMachine>();
        m_isInScreen = true;
    }

    private bool m_isInScreen = true;
    public void Update()
    {
        m_isInScreen=true;

        if (machine == null)
        {
            machine = GetComponent<BaseStateMachine>();
        }
        if (machine && !machine.isHero && !machine.isInScreen)
        {
            m_isInScreen = false;
        }

        if (m_isInScreen)
        {
            OnUpdate();
        }
    }

    public void LateUpdate()
    {
        if (m_isInScreen)
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

    /// <summary>
    /// 用于替换FixedUpdate函数执行逻辑
    /// </summary>
    public override void OnFixedUpdate()
    {
    }

    /// <summary>
    /// 用于替换OnGUI函数执行逻辑
    /// </summary>
    public override void OnFrameGUI()
    {
    }
}