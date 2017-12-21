#pragma once
#include "ManagedCode.h"

namespace GameLogic
{
    ENUM EMinimapClickType
    {
        EMCT_Empty = -1,
        //信号-进攻
	    EMCT_AttackSignal = 0,
        //信号-撤退
	    EMCT_RetreatSignal = 1,
	    //上帝视角
	    EMCT_GodEyeMode = 2,
	    //上帝视角移动
	    EMCT_GodEyeMove = 3,
	    //寻路
        EMCT_Navigate = 4,
        //信号-危险
        EMCT_DangerSignal = 5,
        //信号-正在路上
        EMCT_OnWaySignal = 6,
        //信号-协助我
        EMCT_HelpMeSignal = 7,
        //信号-敌人不见了
        EMCT_LostEyeSignal = 8,
    };
}