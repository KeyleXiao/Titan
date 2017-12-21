using System;
using GameLogic;

class GLOBAL
{
    public const int MAX_ACT = (int)ActID.ACT_COUNT + 1;
    public const int MAX_POSTURE = (int)EntityState.MaxCount + 1;
    public const int MAX_BODYPART = 2;
}

// 生物姿态逻辑接口
interface ICreaturePostureState {

    // 取得姿态ID
    EntityState GetID();

    // 进入状态
    bool OnEnter( Int32 lParam );

    // 离开状态
    void OnLeave();

    // 更新状态逻辑
    void Update();

    // 更新刚体运动状态
    void UpdateRigidbodyState();

    // 动画移动
    void OnAnimatorMove();

    // 能否做某项动作
    bool CanAct(ActID id);
} 
