/*******************************************************************
** 文件名:	MonsterState_Standing_LogicMessage.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	怪物的stand状态
** 应  用:  	怪物的stand状态中的逻辑层消息部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameLogic;
using System;
using System.Runtime.InteropServices;

public sealed partial class MonsterState_Standing : ICreatureState
{
    public void PositionSync(ref cmd_creature_rigidbody_sync sync)
    {
    }

    public void Send_Sync_Position(MoveSyncFlag reportToSever)
    {
    }

    /// <summary>
    /// 传送，清楚掉移动消息
    /// </summary>
    public void OnTransport()
    {
    }

    public void OnForceMove()
    {
    }

    public void OnForceFlight()
    {
    }

    public void ChangeState(IntPtr ptrParam)
    {
    }
    public void ApplyAnimation()
    {

    }
}