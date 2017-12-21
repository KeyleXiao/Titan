/*******************************************************************
** 文件名:	EffectDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - All Rights Reserved
** 创建人:	penglu
** 日  期:	2014-12-24
** 版  本:	1.0
** 描  述:	

			效果模块公用定义
********************************************************************/

#pragma once

#include "ManagedCode.h"
#include "SpellDef.h"
#include "EffectCommon_ManagedDef.h"
#include "Slot_ManagedDef.h"
using namespace SPELL;

namespace EFFECT_CLIENT
{
	DESC("播放动作")
	STRUCT EffectClient_PlayAnimation
	{
		int     nID;

		DESC("动作名")
		sp_char_array(strName,ANIMATION_NAME_LEN);

		STRUCT_INIT(EffectClient_PlayAnimation)
	};

	DESC("播放光效")
	STRUCT EffectClient_AddLighting
	{
		int     nID;

		DESC("光效ID")
		sp_char_array(strName,LIGHTING_NAME_LEN);

		DESC("效果停止时是否保留光效")
		MA_BOOL	nReserveLighting;

        DESC("光效源对象")
        ADD_LIGHTING_SRC_TYPE   nSrcType;

        DESC("光效目标对象")
        ADD_LIGHTING_TARGET_TYPE   nTargetType;

        STRUCT_INIT(EffectClient_AddLighting)
	};

	DESC("停止光效")
	STRUCT EffectClient_StopLighting
	{
		int     nID;

		DESC("光效ID")
		sp_char_array(strName,LIGHTING_NAME_LEN);

		STRUCT_INIT(EffectClient_StopLighting)
	};

	DESC("定身")
	STRUCT EffectClient_Root
	{
		int     nID;

		DESC("暂时无用,通过buff时间控制定身时间")
		int		nType;

		STRUCT_INIT(EffectClient_Root)
	};

	DESC("冷却")
	STRUCT EffectClient_CoolDown
	{
		int     nID;

		DESC("冷却组ID")
		EFREEZE_CLASS nClassID;

		DESC("冷却ID")
		int		nCoolDown;

		STRUCT_INIT(EffectClient_CoolDown)
	};

	//=================================================================================

	DESC("打断动作")
	STRUCT EffectClient_BreakAnimation
	{
		int     nID;

		DESC("类型-暂时无用，随便填")
		int		nType;

        DESC("动作名")
        sp_char_array(strName,ANIMATION_NAME_LEN);

		STRUCT_INIT(EffectClient_BreakAnimation)
	};

	DESC("浮空效果")
	STRUCT EffectClient_Flight
	{
		int nID;

		DESC("浮空高度")
		float	fHeight;

		DESC("空中滞留时间")
		int		nResidenceTime;

		DESC("上升加速度")
		int		nGoUpAcceleration;

		DESC("下落加速度")
		int		nDropAcceleration;

        DESC("下落距离")
        int	    nDropDistance;

        DESC("下落动作")
        sp_char_array(strActName,32);

		STRUCT_INIT(EffectClient_Flight)
	};

	DESC("扫射效果")
	STRUCT EffectClient_Strafe
	{
		int nID;

		DESC("更新坐标时间")
		int		nTime;

		DESC("更新坐标次数")
		int		nCount;

		DESC("光效ID")
		int		nLightID;

        DESC("扫射类型")
        STRAFE_TYPE     nStrafeType;

        DESC("技能ID")
        int		nSpellID;

        DESC("技能使用前描画类型")
        int             nDrawType;

		STRUCT_INIT(EffectClient_Strafe)
	};

	DESC("截断动作")
	STRUCT EffectClient_CutOffAnimation
	{
		int     nID;

        DESC("截断动作列表")
        sp_char_array(strAnimation,32);

		STRUCT_INIT(EffectClient_CutOffAnimation)
	};

	DESC("替换技能槽")
	STRUCT EffectClient_ReplaceSlot
	{
		int     nID;

		DESC("技能ID")
		int		nSpellID;

		DESC("替换技能ID")
		int		nReplaceSpellID;

		DESC("槽位置")
		int		nSlotIndex;

		DESC("替换槽位类型")
		REPLACE_SLOT_TYPE		nReplaceSlotType;

		STRUCT_INIT(EffectClient_ReplaceSlot)
	};

	DESC("不可转向")
	STRUCT EffectClient_NoTurnTo
	{
		int     nID;

		DESC("类型-暂时无用，随便填")
		int		nType;

		STRUCT_INIT(EffectClient_NoTurnTo)
	};

	DESC("停止移动操作")
	STRUCT EffectClient_StopMoveHandle
	{
		int     nID;

		DESC("不响应移动操作时间")
		int		nTime;

		STRUCT_INIT(EffectClient_StopMoveHandle)
	};

	DESC("禁用技能")
	STRUCT EffectClient_DisableSpell
	{
		int     nID;

		DESC("禁用技能列表")
		sp_char_array(strSpellID,256);

		STRUCT_INIT(EffectClient_DisableSpell)
	};

	DESC("转向方向")
	STRUCT EffectClient_TurnToDirection
	{
		int     nID;

		DESC("使用对象")
		TURN_TO_POS_TYPE	nTagetType;

		STRUCT_INIT(EffectClient_TurnToDirection)
	};

	DESC("设置技能槽")
	STRUCT EffectClient_SetSpellSlot
	{
		int     nID;

		DESC("槽位置")
		int		nSlotIndex;

		DESC("槽位类型")
		SLOT_TYPE	nSlotType;

		DESC("槽等级")
		int		nSlotLevel;

		DESC("技能ID")
		int		nSpellID;

		DESC("技能查找类型")
		SPELL_FIND_TYPE nSpellFindType;

		STRUCT_INIT(EffectClient_SetSpellSlot)
	};

	DESC("禁止鼠标带动人物转向")
	STRUCT EffectClient_ForbidMouseTurnTo
	{
		int     nID;

		DESC("禁止时间")
		int		nTime;

		STRUCT_INIT(EffectClient_ForbidMouseTurnTo)
	};
	
	DESC("播放声音")
	STRUCT EffectClient_PlaySound
	{
		int     nID;

		DESC("声音ID列表")
        sp_char_array(strSoundID,SOUND_NAME_LEN);

		DESC("是否停止,为0播放,1停止")
		int		nStop;

		STRUCT_INIT(EffectClient_PlaySound)
	};

    DESC("改变上帝模式")
    STRUCT EffectClient_ChangeGodEyeMode
    {
        int     nID;

        DESC("进入上帝模式")
        MA_BOOL		nEnter;

        STRUCT_INIT(EffectClient_ChangeGodEyeMode)
    };

    DESC("设置技能单个数据(int)")
    STRUCT EffectClient_SetSingleSpellData
    {
        int nID;

        DESC("效果结束是否恢复数据")
        MA_BOOL nRecovery;

        DESC("技能ID")
        int nSpellID;

        DESC("技能数值定义枚举")
        SINGLE_SPELL_DATA nIndex;

        DESC("技能数据")
        int nIndexData;

        STRUCT_INIT(EffectClient_SetSingleSpellData)
    };

    // 字符串和int拆出来，不然一个效果占用字段太大
    DESC("设置技能单个字符串数据(str)")
    STRUCT EffectClient_SetSingleStrSpellData
    {
        int nID;

        DESC("效果结束是否恢复数据")
        MA_BOOL nRecovery;

        DESC("技能ID")
        int nSpellID;

        DESC("技能数值定义枚举")
        SINGLE_STR_SPELL_DATA nIndex;

        DESC("技能字符串")
        sp_char_array(strIndex,SINGLE_STR_SPELL_DATA_LEN);

        STRUCT_INIT(EffectClient_SetSingleStrSpellData)
    };

    DESC("改变翅膀状态")
    STRUCT EffectClient_ChangeWingState
    {
        int     nID;

        DESC("翅膀状态")
        WING_STATE_TYPE     nType;
        DESC("翅膀模型")
        sp_char_array(szPath, 256);
        DESC("飞行速度")
        float   fSpeed;
        DESC("加速度")
        float   fAcceleration;
        DESC("最高速度")
        float   fMaxSpeed;
        DESC("最小飞行高度")
        float   fMinFlyHeight;
        DESC("最高飞行高度")
        float   fMaxFlyHeight;
        DESC("上升速度")
        float   fUpSpeed;
        DESC("镜头最小仰角")
        float   fMinCameraAngle;
        DESC("镜头最大仰角")
        float   fMaxCameraAngle;
        DESC("摄像机距离")
        float   fCameraDistance;

        STRUCT_INIT(EffectClient_ChangeWingState)
    };

    DESC("随机增加Buff")
    STRUCT EffectClient_OrderPlayLighting
    {
        int nID;

        DESC("光效ID列表")
        sp_char_array(strLightingID,32);

        STRUCT_INIT(EffectClient_OrderPlayLighting)
    };

    DESC("不可选择")
    STRUCT EffectClient_NoSelect
    {
        int     nID;

        DESC("类型-暂时无用，随便填")
        int     nType;

        DESC("选取目标类型") // TARGET_FILTER 
        int     nSelectTargetFilter;

        STRUCT_INIT(EffectClient_NoSelect)
    };

    DESC("显示提示UI")
    STRUCT EffectClient_ShowPromptUI
    {
        int     nID;

        DESC("提示ID")
        int		nPromptID;

        STRUCT_INIT(EffectClient_ShowPromptUI)
    };

    DESC("跟随镜头")
    STRUCT EffectClient_StickyCamerra
    {
        int     nID;

        DESC("镜头跟随类型")
        STICKY_CAMERRA_TYPE		nType;

        STRUCT_INIT(EffectClient_StickyCamerra)
    };

    DESC("保持改变模式")
    STRUCT EffectClient_HoldChangeOperation
    {
        int     nID;

        DESC("模式")
        int		nType;

        STRUCT_INIT(EffectClient_HoldChangeOperation)
    };

    DESC("清除攻击命令")
    STRUCT EffectClient_ClearAttackCommand
    {
        int     nID;

        DESC("模式-暂时无用，随便填")
        int		nType;

        STRUCT_INIT(EffectClient_ClearAttackCommand)
    };

    DESC("设置小地图范围提示")
    STRUCT EffectClient_SetObjRangeTip
    {
        int nID;

        DESC("目标ID")
        int	nObjID;

        DESC("范围")
        int	nRange;

        STRUCT_INIT(EffectClient_SetObjRangeTip)
    };

    DESC("使用技能")
    STRUCT EffectClient_CastSpell
    {
        int     nID;

        DESC("使用技能ID")
        int     nSpellID;

        STRUCT_INIT(EffectClient_CastSpell)
    };

    DESC("随机播放动作")
    STRUCT EffectClient_RandPlayAnimation
    {
        int     nID;

        DESC("动作组")
        sp_char_array(strName,32);

        DESC("间隔(毫秒)")
        int nInterval;

        STRUCT_INIT(EffectClient_RandPlayAnimation)
    };

    DESC("改变轻功状态")
    STRUCT EffectClient_ChangeDodgeState
    {
        int     nID;

		DESC("地面上时加速的速度")
		float   fSpeedUpGround;

        DESC("第一次加速的速度")
        float   fSpeed0;

		DESC("第二次加速的速度")
		float   fSpeed1;

		DESC("第三次加速的速度")
		float   fSpeed2;

        DESC("退出加速技能ID")
        int     nSpellID;

        STRUCT_INIT(EffectClient_ChangeDodgeState)
    };

    DESC("显示选择目标")
    STRUCT EffectClient_ShowChooseTarget
    {
        int     nID;

        DESC("后续使用技能ID")
        int		nSpellID;

        DESC("选择目标类型")
        SHOW_CHOOSE_TARGET_TYPE nTargetType;

        DESC("距离（米）")
        int     nDistance;

        STRUCT_INIT(EffectClient_ShowChooseTarget)
    };

    DESC("改变狙击镜状态")
    STRUCT EffectClient_ChangeSniperscope
    {
        int     nID;

        DESC("视角偏移值")
        float   fFOV;

        STRUCT_INIT(EffectClient_ChangeSniperscope)
    };

    DESC("显示公共消息框")
    STRUCT EffectClient_ShowCommonMessageBox
    {
        int     nID;

        DESC("消息框类型")
        int nType;

        DESC("消息框标题")
        sp_char_array(szCaption, COMMON_MESSAGE_BOX_CAPTION_LEN);

        DESC("消息框文本")
        sp_char_array(szText, COMMON_MESSAGE_BOX_TEXT_LEN);

        STRUCT_INIT(EffectClient_ShowCommonMessageBox)
    };

    DESC("技能询问")
    STRUCT EffectClient_SpellAttribute
    {
        int     nID;

        DESC("技能ID")
        int     nSpellID;

        DESC("槽位置")
        int		nSlotIndex;

        DESC("槽位类型")
        SLOT_TYPE	nSlotType;

        STRUCT_INIT(EffectClient_SpellAttribute)
    };

    DESC("订阅动作播放动作")
    STRUCT EffectClient_SubscibePlayAnimation
    {
        int     nID;

        DESC("选择对象")
        MA_TAGET_TYPE nTagetType;

        DESC("订阅动作")
        sp_char_array(strSubscibeAnimationName,32);

        DESC("播放动作")
        sp_char_array(strPlayAnimationName,32);

        STRUCT_INIT(EffectClient_SubscibePlayAnimation)
    };

    DESC("(客户端)进入移动被控制状态,作废")
    STRUCT EffectClient_BeSneered
    {
        int     nID;

        STRUCT_INIT(EffectClient_BeSneered)
    };

    DESC("超级跳")
    STRUCT EffectClient_LongJump
    {
        int     nID;

        DESC("上升时间")
        float fUpTime;

        DESC("上升高度")
        float fUpHeight;

        STRUCT_INIT(EffectClient_LongJump)
    };

    DESC("滑翔功能")
    STRUCT EffectClient_Glide
    {
        int     nID;

        DESC("滑翔速度(float)")
        float	fGlideSpeed;

        DESC("下落速度(float)")
        float	fFallSpeed;

        STRUCT_INIT(EffectClient_Glide)
    };

    DESC("设置摄像机高度(Y坐标)")
    STRUCT EffectClient_SetCameraHeight
    {
        int     nID;

        DESC("高度(Y坐标)")
        float   fHeight;

        STRUCT_INIT(EffectClient_SetCameraHeight)
    };

    DESC("右键按下时上报")
    STRUCT EffectClient_ReportOnRightButtonDown
    {
        int    nID;

        STRUCT_INIT(EffectClient_ReportOnRightButtonDown)
    };

	DESC("向技能目标地点翻滚")
	STRUCT EffectClient_Roll2Target
	{
		int nID;

		STRUCT_INIT(EffectClient_Roll2Target);
	};

	DESC("自动前进")
	STRUCT EffectClient_AutoMove
	{
		int nID;
		float fStartSpeed;    // 初始速度
		float fAcceleration;  // 加速度
		float fMaxSpeed;	  // 最大速度

		STRUCT_INIT(EffectClient_AutoMove);
	};

	DESC("开关子弹面板")
	STRUCT EffectClient_SwitchBulletPanel
	{
		int nID;

		DESC("效果开始打开面板结束时关闭(这个字段不用配)")
		OPEN_OR_CLOSE_BULLET_PANEL eSwitch;

		DESC("最大子弹数量")
		int nMaxBulletNum;

		STRUCT_INIT(EffectClient_SwitchBulletPanel);
	};

	DESC("debuff类型")
	ENUM TypeDebuff
	{
		DESC("定身")
		DEBUFF_TYPE_ROOT,
		DESC("封魔")
		DEBUFF_TYPE_SILIENCE,
		DESC("晕眩(定身+封魔)")
		DEBUFF_TYPE_ROOT_AND_SILIENCE,
		DESC("减速")
		DEBUFF_TYPE_SLOW_DOWN,
	};

	DESC("显示debuff进度条")
	STRUCT EffectClient_ShowDebuffProgressBar
	{
		int  nID;

		DESC("debuff类型")
		TypeDebuff eType;

		DESC("时间ms")
		int nTime;

		STRUCT_INIT(EffectClient_ShowDebuffProgressBar)
	};
};
