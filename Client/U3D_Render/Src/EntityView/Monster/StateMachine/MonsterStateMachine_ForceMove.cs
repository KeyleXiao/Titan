/*******************************************************************
** 文件名:	MonsterStateMachine_Skin.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	怪物状态机
** 应  用:  	怪物状态机中的强制位移部分

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


public sealed partial class MonsterStateMachine : BaseStateMachine
{
    #region 强制位移部分

    //强制位移部分
    [HideInInspector]
    public bool bIsForceMoving = false;
    private uint m_nForceMoveStartTick = 0;
    private uint m_nForceMoveEndTick = 0;
    private Vector3 m_ForceMovePos = Vector3.zero;
    private Vector3 m_fForceMoveSpeed = Vector3.zero;
    private Vector3 m_fForceStartPos = Vector3.zero;

    [HideInInspector]
    public bool m_bForceFlighting = false;
    private float m_fFlightV0;
    private float m_fFlightTime;
    private float m_fFlightDownTime;
    private float m_fFlightVel;
    private uint m_uResidenceStopTick;
    private Vector3 m_flightStartPosition; //开始上升时候的位置记录，如果是连续浮空的话，会记最后一次浮空的开始位置
    private Vector3 m_flightEndPosition; //浮空的结束位置，如果是连续浮空的话，会记第一次浮空的开始位置
    private uint m_uResidenceTick;
    private float m_fFlightHeight;
    private float upAcc;
    private float downAcc;


    public override void ForceMove(cmd_force_move param)
    {
        Vector3 targetPos = new Vector3(param.fPosition_x, param.fPosition_y, param.fPosition_z);
        int moveTick = (int)param.fTime;//(int)(param.fTime * 1000);
        float fHeight = param.fHeight;

        if (bNotMove || moveTick <= 0)
        {
            return;
        }

        bIsForceMoving = true;
        m_nForceMoveStartTick = GameLogicAPI.getTickCount();
        m_nForceMoveEndTick = m_nForceMoveStartTick + (uint)moveTick;
        m_ForceMovePos = targetPos;
        Vector3 moveDis = targetPos - transform.position;
        moveDis.y = 0;//--
        m_fForceMoveSpeed = moveDis / ((float)moveTick / 1000.0f);
        m_fForceStartPos = transform.position;

        if (fHeight > 0.01f && moveTick > 0)
        {
            float Ag = 0;//重力加速度
            float TotalTime = ((float)moveTick / 1000.0f);//抵达所需总时间
            float RiseTime = 0;//上升使用时间

            float RiseHeight = 0; //上升距离
            if (targetPos.y > transform.position.y + fHeight)
            {
                //目标落点高于升高高度
                RiseHeight = 0;
            }
            else
            {
                RiseHeight = fHeight;
            }

            float DropHeight = 0;//（下落距离）
            if (RiseHeight != 0)//向上抛并有下落过程
            {
                DropHeight = transform.position.y + fHeight - targetPos.y;
                RiseTime = TotalTime - TotalTime / (Mathf.Sqrt(RiseHeight / DropHeight) + 1);
                Ag = 2.0f * RiseHeight / (RiseTime * RiseTime);
                m_fFlightV0 = Ag * RiseTime;  // 初始速度向上(记录抛出时的垂直初速度)
            }
            else //无下落过程
            {
                m_fFlightV0 = (targetPos.y - transform.position.y) / TotalTime;
                Ag = 0;
            }
            upAcc = Ag - currentGravity;
            downAcc = Ag - currentGravity;
            m_fFlightVel = m_fFlightV0;//垂直当前速度
            m_uResidenceStopTick = 0;
            m_uResidenceTick = 0;
            m_fFlightHeight = DropHeight ;
            m_flightStartPosition = transform.position;
            //if (m_bForceFlighting == false)
            //{
            //    m_flightEndPosition = transform.position;
            //}
            m_flightEndPosition = targetPos;
            m_fFlightTime = 0.0f;
            m_bForceFlighting = true;
        }
        else
        {
            m_fFlightHeight = 0.0f;
        }
    }
 
    private void forceMoveToTarget()
    {
        ///垂直方向移动
        if (m_bForceFlighting)
        {
            forceFlight();
        }
        uint tick = GameLogicAPI.getTickCount();
        ///强制移动停止
        if (m_fForceMoveSpeed.sqrMagnitude > 0.001f && tick >= m_nForceMoveEndTick)
        {
            bIsForceMoving = false;
            m_fForceMoveSpeed = Vector3.zero;

            Vector3 dis = m_ForceMovePos - creature.position;
            dis.y = 0; //忽略Y轴距离

            creature.Translate(dis, Space.World);

            //做一下贴地面
            creature.SetPosition(CorrectPosFromPhysic(creature.position, creaturePropety.CreatureHeightInMeters + 2, MonsterState_Standing.GroundedCheckDistance));

            m_CurStateHandler.OnForceMove();
            syncVelocity = Vector3.zero;
            syncPosition = transform.position;
            //m_CurStateHandler.Send_Sync_Position();

            return;
        }
        ///水平方向移动停止
        if (bIsForceMoving)
        {
            Vector3 currentIdealPos = m_fForceStartPos + m_fForceMoveSpeed * (tick - m_nForceMoveStartTick) / 1000.0f;
            Vector3 currentMovementOffset = currentIdealPos - creature.position;
            currentMovementOffset.y = 0.0f;
            creature.Translate(currentMovementOffset, Space.World);

            ////做一下贴地面
            //creature.SetPosition( CorrectPosFromPhysic(creature.position, creaturePropety.CreatureHeightInMeters);
            m_CurStateHandler.OnForceMove();
        }
    }

    public override void ForceFlight(cmd_effect_Flight param)
    {
        if (bNotMove)
        {
            return;
        }

        if (bIsForceMoving && m_bForceFlighting == false)
        {
            Trace.Log("在进行其他强制移动，不浮空");
            return;
        }


        if (param.nFlight == 0)
        {
            //此参数为0时，停止浮空，开始下落
            uint tick = GameLogicAPI.getTickCount();
            if (m_bForceFlighting && m_uResidenceStopTick > tick)
            {
                m_uResidenceStopTick = tick;
            }
            return;
        }

        if (m_bForceFlighting == true)
        {
            //已经在浮空状态，第二次浮空
            if ((m_fFlightVel > 0 && param.fHeight <= m_fFlightHeight) //在上升状态，最终升空高度比fHeight要求的高
                || (transform.position.y - m_flightEndPosition.y > param.fHeight)) //在下降状态，但已经比fHeight要求的高
            {
                //不会比目前状态升的更高，不需要重新上升，所以忽略
                return;
            }
            else
            {
                upAcc = param.nGoUpAcceleration;
                downAcc = param.nDropAcceleration;
                float acc = currentGravity + upAcc;
                float newHeight = param.fHeight - (transform.position.y - m_flightEndPosition.y);
                m_fFlightV0 = Mathf.Sqrt(2.0f * newHeight * acc);
                m_fFlightVel = m_fFlightV0;
                m_uResidenceStopTick = 0;
                m_uResidenceTick = (uint)param.nResidenceTime;
                m_fFlightHeight = param.fHeight;
                m_flightStartPosition = transform.position;
                //m_flightEndPosition不变
                m_fFlightTime = 0.0f;
            }
        }
        else
        {
            upAcc = param.nGoUpAcceleration;
            downAcc = param.nDropAcceleration;
            float acc = currentGravity + upAcc;
            m_fFlightV0 = Mathf.Sqrt(2.0f * param.fHeight * acc);
            m_bForceFlighting = true;
            m_fFlightVel = m_fFlightV0;
            m_uResidenceStopTick = 0;
            m_uResidenceTick = (uint)param.nResidenceTime;
            m_fFlightHeight = param.fHeight;
            m_flightStartPosition = transform.position;
            m_flightEndPosition = transform.position;
            m_fFlightTime = 0.0f;
        }


        m_nForceMoveStartTick = GameLogicAPI.getTickCount();
        m_nForceMoveEndTick = 0;
        m_ForceMovePos = transform.position;
        m_fForceMoveSpeed = Vector3.zero;
        m_fForceStartPos = transform.position;
    }

    private void forceFlight()
    {
        uint tick = GameLogicAPI.getTickCount();
        if (m_uResidenceStopTick > 0 && m_uResidenceStopTick > tick)
        {
            return;
        }
        m_fFlightTime += Time.deltaTime;

        float newVel;
        float hight;
        if (m_fFlightVel > 0.0f)
        {
            newVel = m_fFlightV0 - (currentGravity + upAcc) * m_fFlightTime;
            hight = m_fFlightV0 * m_fFlightTime - (currentGravity + upAcc) * m_fFlightTime * m_fFlightTime / 2.0f;
            if (newVel <= 0.0f)
            {
                //上升结束，下降计时清0，开始浮空计时
                m_uResidenceStopTick = tick + m_uResidenceTick;
                m_fFlightDownTime = 0.0f;
            }
            //             if (m_fForceMoveSpeed.sqrMagnitude < 0.001f)
            //             {
            //                 Vector3 newOffset = new Vector3(m_flightStartPosition.x, m_flightStartPosition.y + hight, m_flightStartPosition.z);
            //                 newOffset = newOffset - creature.position;
            //                 creature.Translate(newOffset, Space.World);
            //                 m_CurStateHandler.OnForceFlight();
            //             }
            //             else
            {
                Vector3 newOffset = new Vector3(creature.position.x, m_flightStartPosition.y + hight, creature.position.z);
                newOffset = newOffset - creature.position;
                creature.Translate(newOffset, Space.World);
                m_CurStateHandler.OnForceFlight();
            }
        }
        else
        {
            m_fFlightDownTime += Time.deltaTime;
            newVel = 0 - (currentGravity + downAcc) * m_fFlightDownTime;
            hight = m_fFlightHeight - (currentGravity + downAcc) * m_fFlightDownTime * m_fFlightDownTime / 2.0f;
            //             if (m_fForceMoveSpeed.sqrMagnitude < 0.001f)
            //             {
            //                 Vector3 newOffset = new Vector3(m_flightStartPosition.x, m_flightEndPosition.y + hight, m_flightStartPosition.z);
            //                 newOffset = newOffset - creature.position;
            //                 creature.Translate(newOffset, Space.World);
            //                 m_CurStateHandler.OnForceFlight();
            //             }
            //             else
            {
                Vector3 newOffset = new Vector3(creature.position.x, m_flightEndPosition.y + hight, creature.position.z);
                newOffset = newOffset - creature.position;
                creature.Translate(newOffset, Space.World);
                m_CurStateHandler.OnForceFlight();
            }
        }

        m_fFlightVel = newVel;
        if (m_fFlightVel < 0.0f && creature.position.y <= m_flightEndPosition.y) //已经落到底了
        {
            m_bForceFlighting = false;
            if (!bIsForceMoving) //如果还有强制位移的话，还要继续强制位移
            {
                //Vector3 newOffset = m_flightEndPosition;
                //newOffset = newOffset - transform.position;
                //controller.Move(newOffset);
            }
            creature.SetPosition(CorrectPosFromPhysic(creature.position, creaturePropety.CreatureHeightInMeters + 2, MonsterState_Standing.GroundedCheckDistance));
            m_CurStateHandler.OnForceFlight();
        }
    }


    public override void AutoMoveTo(Vector3 targetPos, bool rotateCamera = true)
    {

    }
    public override void StopAutoMove(bool userStop = true)
    {

    }

    //GameObject cube = null;
    public override void FaceTo(Vector3 targetPos)
    {
        if (!bNotTurnWhenAttackMask)
        {
            LookAtPoint(CorrectLookAtPoint(targetPos));
            defaultAngle = creature.eulerAngles;
            //SetAttackTargetBall(targetPos);
        }
    }
    #endregion
}
