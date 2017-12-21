#pragma once
#include "ManagedCode.h"

namespace GameLogic
{
    ENUM EMinimapClickType
    {
        EMCT_Empty = -1,
        //�ź�-����
	    EMCT_AttackSignal = 0,
        //�ź�-����
	    EMCT_RetreatSignal = 1,
	    //�ϵ��ӽ�
	    EMCT_GodEyeMode = 2,
	    //�ϵ��ӽ��ƶ�
	    EMCT_GodEyeMove = 3,
	    //Ѱ·
        EMCT_Navigate = 4,
        //�ź�-Σ��
        EMCT_DangerSignal = 5,
        //�ź�-����·��
        EMCT_OnWaySignal = 6,
        //�ź�-Э����
        EMCT_HelpMeSignal = 7,
        //�ź�-���˲�����
        EMCT_LostEyeSignal = 8,
    };
}