/*******************************************************************
** 文件名:	MonsterStateMachine_LogicMessage.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	怪物状态机
** 应  用:  	怪物状态机中的逻辑层消息部分

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
using U3D_Render;


public sealed  partial class MonsterStateMachine : BaseStateMachine
{
    private void PositionSync_Imp(Vector3 pos, Vector3 angles, Vector3 velocity)
    {
        syncPosition = pos;
        syncAngles = angles;
        syncVelocity = velocity;
    }

    #region 逻辑层消息

    private static Vector3 MonsterInvalidPos = new Vector3(999, 999, 999);
    public override void PositionSync(ref cmd_creature_rigidbody_sync sync)
    {
        Vector3 pos = new Vector3(sync.fPosition_x, sync.fPosition_y, sync.fPosition_z);
        Vector3 angles = new Vector3(sync.fRotation_x, sync.fRotation_y, sync.fRotation_z);
        Vector3 velocity = new Vector3(sync.fVelocity_x, sync.fVelocity_y, sync.fVelocity_z);
        PositionSync_Imp(pos, angles, velocity);
    }

    public override void PlayAnim(string name, bool react = false, float speed = 1.0f, float durationTime = 0.0f, float normalizedTime = 0.0f)
    {
        //对于怪物来说，进入死亡状态，就不可能在出来了。
        //speed都暂时只用1.0，因为没有找到地方还原
        if (null != m_CurStateHandler)
        {
            if (m_CurStateHandler.GetID() == (int)EntityState.Dead)
            {
                return;
            }
        }

        if (react && animator) //受击动作不打断Attack标签下的动作
        {
            bool animatorAttack = animator.GetCurrentAnimatorStateInfo(0).IsTag("Attack");
            if (animatorAttack)
            { 
                return;
            }
        }
        AnimatorPlay(name, durationTime);
    }

    public override void BreakAttackAnim(string animName = "",float crossFaceTime = 0.1f)
    {
        SetAnimatorTrigger("BreakAttack");
    }

    public override void Transport(cmd_creature_transport pos)
    {
        Vector3 posit = Vector3.zero;
        
        posit.x = pos.fPosition_x;
        posit.y = pos.fPosition_y;
        posit.z = pos.fPosition_z;

        if (pos.bUseAngle == 1)
        {
            defaultAngle.x = pos.fRotation_x;
            defaultAngle.y = pos.fRotation_y;
            defaultAngle.z = pos.fRotation_z;

            creature.eulerAngles = defaultAngle;
        }

        posit = CorrectPosFromPhysic(posit, creaturePropety.CreatureHeightInMeters + 2, MonsterState_Standing.GroundedCheckDistance);

        //传送的时候使用纠正后的位置，避免防御塔之类的不会移动的怪物浮空
        PositionSync_Imp(posit, creature.eulerAngles, new Vector3(0, 0, 0));
        creature.SetPosition(posit);

        m_CurStateHandler.OnTransport();
    }


    //public override void OnEntityDead(IntPtr param) 和父类的一样，屏蔽掉先
    //{
    //    ChangeState((int)EntityState.Dead, param);
    //    //EntityFactory.entry_destroyEntry(entityID, false);
    //}

    public override void OnLeaveView()
    {
        base.OnLeaveView();

        //观战时进出视野由客户端自己控制，所以进视野的时候并不会再有传送消息了，所以离开视野时就先不拉位置了
        if (StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB && StageManager.Instance.CurSubState == EStage_SubState.ESSS_WAR)
            return;

        //离开视野的时候，设置同一个位置，进视野的时候，走传送再重新设置回来，梁成说的
        syncPosition = MonsterInvalidPos;
        creature.SetPosition(MonsterInvalidPos);
    }
    #endregion
}
