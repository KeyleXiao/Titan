using System;
using UnityEngine;
using GameLogic;

// 生物状态接口
public interface ICreatureState {

    // 取得状态ID
    int GetID();

    // 进入状态
    bool OnEnter(IntPtr lParam);

    // 反复进入状态，例如跳跃时再次起跳
    bool ReEnter(IntPtr lParam);

    // 离开状态
    void OnLeave();

    // 更新状态逻辑
    void Update();

    //public void OnAnimatorIK(int layerIndex);

    //返回是否在地面
    bool IsGrounded();

    //返回是否在飞行
    bool isFloating();

    // 同步位置
    void PositionSync(ref cmd_creature_rigidbody_sync sync);
    void Send_Sync_Position(MoveSyncFlag reportToSever);

    // 获得基本速度
    Vector3 getVelocity();
    void setVelocity(Vector3 vel);

    /// <summary>
    /// 传送
    /// </summary>
    void OnTransport();

    /// <summary>
    /// 强制位移
    /// </summary>
    void OnForceMove();

    /// <summary>
    /// 强制浮空
    /// </summary>
    void OnForceFlight();


    /// <summary>
    /// 离开此状态
    /// </summary>
    void ChangeState(IntPtr ptrParam);

    /// <summary>
    /// 应用动画
    /// </summary>
    void ApplyAnimation();

    /// <summary>
    /// 清除State
    /// </summary>
    void ClearState();
} 
