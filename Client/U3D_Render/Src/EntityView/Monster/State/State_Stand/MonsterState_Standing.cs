/*******************************************************************
** 文件名:	MonsterState_Standing.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	怪物的stand状态
** 应  用:  	怪物的stand状态中的main部分

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

public struct MovePoint
{
    public Vector3 vPoint;
    public float fDuration; // 从上一个点到这个点的时间
}

public sealed partial class MonsterState_Standing : ICreatureState
{
    // 状态机指针
    MonsterStateMachine m_pFSM;

    [HideInInspector]
    public bool grounded = true;
    public bool bIsMoving = false;

    [HideInInspector]
    public LayerMask groundLayers = -1;

    //是否在本状态，因为状态切换会延时一帧
    public bool isOnThisState = false;

    /// <summary>
    /// 地面检测距离
    /// </summary>
    public const float GroundedCheckDistance = 5.0f;


    Vector3 curPos = Vector3.zero;

    
    // 构造
    public MonsterState_Standing(MonsterStateMachine pFsm)
    {
        m_pFSM = pFsm;
    }

    // 取得ID
    public int GetID() { return (int)EntityState.Standing; }

    public void ClearState()
    {
    }

    // 进入状态
    public bool OnEnter(IntPtr lParam)
    {
        isOnThisState = true;

        return true;
    }


    // 反复进入状态，例如跳跃时再次起跳
    public bool ReEnter(IntPtr lParam)
    {
        return false;
    }

    // 离开状态
    public void OnLeave()
    {
        isOnThisState = false;
    }

    //返回是否在地面
    public bool IsGrounded()
    {
        return grounded;
    }

    public bool isFloating()
    {
        return false;
    }



    int runframecount = 0;
    float currappfactor = 0.05f;

    // 更新状态逻辑
    public void Update()
    {
        if (!isOnThisState)
        {
            return;
        }
        if (m_pFSM.bIsForceMoving || m_pFSM.m_bForceFlighting || m_pFSM.bPauseMachineUpdate)
        {
            return;
        }
        if(!m_pFSM.creaturePropety)
        {
            return;
        }

        //建筑不会移动
        if(m_pFSM.creaturePropety.GetNumProp(ENTITY_PROPERTY.PROPERTY_SEX) == (int)MONSTER_TYPE.MONSTER_TYPE_TOWER)
        {
            return;
        }
        Vector3 deltapos = m_pFSM.syncPosition - m_pFSM.transform.position;
        float deltadis = deltapos.magnitude;
        if (deltadis < 0.01f)
        {
            return;
        }

        float deltatime = Time.deltaTime;
        int smoothlevel = EntityFactory.Instance.smoothlevel;
        float apprfactor1 = EntityFactory.Instance.apprfactor1;
        float apprfactor2 = EntityFactory.Instance.apprfactor2;
  




        Vector3 curPos;
        if (smoothlevel==2)
        {
            int vavyingframe = EntityFactory.Instance.vavyingFrame;

            Vector3 advance = m_pFSM.syncVelocity.magnitude * deltapos.normalized * deltatime;
            float advancedis = advance.magnitude;

            bool bRun = advancedis > 0.001f && m_pFSM.GetAnimatorBool("isRun");

            if (bRun)
            {
                if (runframecount < 0) runframecount = 0;
                runframecount++;
                if (runframecount > 9999) runframecount = 9999;
            }
            else
            {
                if (runframecount > 0) runframecount = 0;
                runframecount--;
                if (runframecount < -9999) runframecount = -9999;
            }
            int absrunframecount = Mathf.Abs(runframecount);

            if (deltadis > 10.0f || deltadis < 0.1f)  //太远或者太近直接拉
            {
                curPos = m_pFSM.syncPosition;
            }
            else if (runframecount < -vavyingframe)  //稳定静止状态，已经停下来有vavyingframe帧，没走路情况只能直接拉
            {
                curPos = m_pFSM.syncPosition;
                currappfactor = apprfactor2;
            }
            else if (runframecount >= -vavyingframe && runframecount < 0 ) //正在停下来的情况都用逼近公式2，apprfactor2趋于极限（同步点）时收敛速度更快
            {
                float deltafactor = Mathf.Abs(apprfactor2 - apprfactor1) / (float)vavyingframe;
                currappfactor += deltafactor;
                currappfactor = Mathf.Clamp(currappfactor, apprfactor1, apprfactor2);
                curPos = m_pFSM.syncPosition * currappfactor + m_pFSM.transform.position * (1 - currappfactor);
            }
            else if (runframecount > 0 && runframecount <= vavyingframe ) //正在起步情况都用逼近公式2，apprfactor2趋于极限（同步点）时收敛速度更快
            {
                float deltafactor = Mathf.Abs(apprfactor2 - apprfactor1) / (float)vavyingframe;
                currappfactor -= deltafactor;
                currappfactor = Mathf.Clamp(currappfactor, apprfactor1, apprfactor2);
                curPos = m_pFSM.syncPosition * currappfactor + m_pFSM.transform.position * (1 - currappfactor);
            }
            else //稳定走路情况或临时停下来情况
            {
                if (advancedis * 0.5f > deltadis) //走路的最后一步，不足半个步长
                {
                    curPos = m_pFSM.syncPosition;
                }
                else if (advancedis * 3.0f > deltadis && advancedis < deltadis) //处于走路动作且3个步长内能走远到同步位置的距离，用普通走路公式
                {
                    curPos = m_pFSM.transform.position + advance;
                }
                else //否则，存在较大误差，比如时间轴缩放了，余下情况都用逼近公式1
                {
                    curPos = m_pFSM.syncPosition * apprfactor1 + m_pFSM.transform.position * (1 - apprfactor1);
                }
                currappfactor = apprfactor1;
            }
        }
        else if (smoothlevel == 1)
        {
            if (deltadis > 10.0f || deltadis < 0.1f)  //太远或者太近直接拉
            {
                curPos = m_pFSM.syncPosition;
            }
            else
            {
                curPos = m_pFSM.syncPosition * apprfactor2 + m_pFSM.transform.position * (1 - apprfactor2);
            }
        }
        else
        {
            curPos = m_pFSM.syncPosition;
        }

        if (m_pFSM.GetVisible())
        {
            curPos = MonsterStateMachine.CorrectPosFromPhysic(curPos, m_pFSM.creaturePropety.CreatureHeightInMeters + 2, GroundedCheckDistance);
        }
        m_pFSM.SetPosition(curPos);

        Quaternion ate = Quaternion.Euler(m_pFSM.syncAngles);
        Vector3 movDir = ate * Vector3.forward;
        Vector3 ptTarget = curPos + movDir * 10;
        m_pFSM.LookAtPoint(m_pFSM.CorrectLookAtPoint(ptTarget));

        return;
    }


    #region 速度的接口函数

    // 获得基本速度
    public Vector3 getVelocity()
    {
        return Vector3.zero;
    }
    public void setVelocity(Vector3 vel)
    {

    }

    // 获得同步速度
    public Vector3 getSyncVelocity()
    {
        return Vector3.zero;
    }

    public void setSyncVelocity(Vector3 vel)
    {

    }
    #endregion

}