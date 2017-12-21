using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using GameLogic;

public class CreatureState_Swimming : ICreatureState {
    
    // 状态机指针
    CreatureStateMachine m_pFSM;

    public CreatureState_Swimming(CreatureStateMachine pFsm)
    {
        m_pFSM = pFsm;
    }

    // 取得状态ID
    public int GetID() { return (int)EntityState.Swimming; }

    public void ClearState()
    {
    }

    // 进入状态
    public bool OnEnter(IntPtr lParam)
    {
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

    }

    // 更新状态逻辑
    public void Update()
    {
    }

    //public void OnAnimatorIK(int layerIndex)
    //{
    //}

    //返回是否在地面
    public bool IsGrounded()
    {
        return false;
    }

    public bool isFloating()
    {
        return false;
    }

    public void PositionSync(ref cmd_creature_rigidbody_sync sync)
    {

    }
    public void Send_Sync_Position(MoveSyncFlag reportToSever)
    {
    }

    private Vector3 m_Velocity;

    // 获得基本速度
    public Vector3 getVelocity()
    {
        return m_Velocity;
    }
    public void setVelocity(Vector3 vel)
    {
        m_Velocity = vel;
    }

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
