/*******************************************************************
** 文件名:	Stage_ManagedDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/3/2015
** 版  本:	1.0
** 描  述:	游戏状态定义

			
********************************************************************/

#ifndef __COMMON_STAGE_MANAGED_DEF_H__
#define __COMMON_STAGE_MANAGED_DEF_H__


#include "ManagedCode.h"

namespace  Stage_ManagerDef
{
	//////////////////////////////////////////////////////////////////////////
	/*						C++和C#公用数据结构								*/
	//////////////////////////////////////////////////////////////////////////
    // 游戏状态  	注：这个修改要关注下“用户登陆状态定义 GATEWAY_CLIENT_STATE”
	ENUM EStage_State
	{
        ESS_NONE,
		ESS_LOGIN,
        ESS_CHOOSE, 
        ESS_QUEUE,
		ESS_GAME ,
        ESS_LOADSCENE,
		ESS_MATCHROOM	,
		ESS_BATTLE		,
        ESS_MAX		    ,
	};

	// 房间状态
	ENUM EMatchRoomState
	{
		// 开始游戏前，等待
		MatchRoomState_Wait = 0, 	
		// 选人
		MatchRoomState_SelectHero, 	
					
		// 全部玩家确认英雄 开始倒计时中
		MatchRoomState_ReadyGo,	

        // 战斗中
        MatchRoomState_Fighting,

		MatchRoomStates,
	};

	// MatchStage房间状态
	ENUM ECLMatchRoomState
	{
		// 点击开始后 等待服务器返回开始结果
		CLMatchRoomState_Starting = EMatchRoomState::MatchRoomStates, 	
		// 选英雄结束 等待服务器要求加载地图
		CLMatchRoomState_WaitToLoadMap,
		// 加载地图中 
		CLMatchRoomState_LoadingMap,
		// 加载地图完 直接进
		CLMatchRoomState_LoadEndEnterMap,
		// 等待进入场景 
		CLMatchRoomState_WaitEnterMap,

		CLMatchRoomStates
	};

	ENUM EMRoomSelectRuleType
	{
		EMSRT_NONE = 0,
		EMSRT_BANHERO,
		EMSRT_SETSELECTHERO,
	};

	ENUM EMHeroSelectControlType
	{
		EMHSCT_NONE = 0,
		EMHSCT_LIMITCAMP,   // 阵营通知
		EMHSCT_LIMITALL,    // 全部通知
	};

	//与显示层对应的按键类型，
	//当前显示层为Unity => EViewKeyCode == UnityEngine.KeyCode
	ENUM EViewKeyCode
	{
		EVKC_None = 0,
		EVKC_Backspace = 8,
		EVKC_Tab = 9,
		EVKC_Clear = 12,
		EVKC_Return = 13,
		EVKC_Pause = 19,
		EVKC_Escape = 27,
		EVKC_Space = 32,
		EVKC_Exclaim = 33,
		EVKC_DoubleQuote = 34,
		EVKC_Hash = 35,
		EVKC_Dollar = 36,
		EVKC_Ampersand = 38,
		EVKC_Quote = 39,
		EVKC_LeftParen = 40,
		EVKC_RightParen = 41,
		EVKC_Asterisk = 42,
		EVKC_Plus = 43,
		EVKC_Comma = 44,
		EVKC_Minus = 45,
		EVKC_Period = 46,
		EVKC_Slash = 47,
		EVKC_Alpha0 = 48,
		EVKC_Alpha1 = 49,
		EVKC_Alpha2 = 50,
		EVKC_Alpha3 = 51,
		EVKC_Alpha4 = 52,
		EVKC_Alpha5 = 53,
		EVKC_Alpha6 = 54,
		EVKC_Alpha7 = 55,
		EVKC_Alpha8 = 56,
		EVKC_Alpha9 = 57,
		EVKC_Colon = 58,
		EVKC_Semicolon = 59,
		EVKC_Less = 60,
		EVKC_Equals = 61,
		EVKC_Greater = 62,
		EVKC_Question = 63,
		EVKC_At = 64,
		EVKC_LeftBracket = 91,
		EVKC_Backslash = 92,
		EVKC_RightBracket = 93,
		EVKC_Caret = 94,
		EVKC_Underscore = 95,
		EVKC_BackQuote = 96,
		EVKC_A = 97,
		EVKC_B = 98,
		EVKC_C = 99,
		EVKC_D = 100,
		EVKC_E = 101,
		EVKC_F = 102,
		EVKC_G = 103,
		EVKC_H = 104,
		EVKC_I = 105,
		EVKC_J = 106,
		EVKC_K = 107,
		EVKC_L = 108,
		EVKC_M = 109,
		EVKC_N = 110,
		EVKC_O = 111,
		EVKC_P = 112,
		EVKC_Q = 113,
		EVKC_R = 114,
		EVKC_S = 115,
		EVKC_T = 116,
		EVKC_U = 117,
		EVKC_V = 118,
		EVKC_W = 119,
		EVKC_X = 120,
		EVKC_Y = 121,
		EVKC_Z = 122,
		EVKC_Delete = 127,
		EVKC_Keypad0 = 256,
		EVKC_Keypad1 = 257,
		EVKC_Keypad2 = 258,
		EVKC_Keypad3 = 259,
		EVKC_Keypad4 = 260,
		EVKC_Keypad5 = 261,
		EVKC_Keypad6 = 262,
		EVKC_Keypad7 = 263,
		EVKC_Keypad8 = 264,
		EVKC_Keypad9 = 265,
		EVKC_KeypadPeriod = 266,
		EVKC_KeypadDivide = 267,
		EVKC_KeypadMultiply = 268,
		EVKC_KeypadMinus = 269,
		EVKC_KeypadPlus = 270,
		EVKC_KeypadEnter = 271,
		EVKC_KeypadEquals = 272,
		EVKC_UpArrow = 273,
		EVKC_DownArrow = 274,
		EVKC_RightArrow = 275,
		EVKC_LeftArrow = 276,
		EVKC_Insert = 277,
		EVKC_Home = 278,
		EVKC_End = 279,
		EVKC_PageUp = 280,
		EVKC_PageDown = 281,
		EVKC_F1 = 282,
		EVKC_F2 = 283,
		EVKC_F3 = 284,
		EVKC_F4 = 285,
		EVKC_F5 = 286,
		EVKC_F6 = 287,
		EVKC_F7 = 288,
		EVKC_F8 = 289,
		EVKC_F9 = 290,
		EVKC_F10 = 291,
		EVKC_F11 = 292,
		EVKC_F12 = 293,
		EVKC_F13 = 294,
		EVKC_F14 = 295,
		EVKC_F15 = 296,
		EVKC_Numlock = 300,
		EVKC_CapsLock = 301,
		EVKC_ScrollLock = 302,
		EVKC_RightShift = 303,
		EVKC_LeftShift = 304,
		EVKC_RightControl = 305,
		EVKC_LeftControl = 306,
		EVKC_RightAlt = 307,
		EVKC_LeftAlt = 308,
		EVKC_RightCommand = 309,
		EVKC_RightApple = 309,
		EVKC_LeftCommand = 310,
		EVKC_LeftApple = 310,
		EVKC_LeftWindows = 311,
		EVKC_RightWindows = 312,
		EVKC_AltGr = 313,
		EVKC_Help = 315,
		EVKC_Print = 316,
		EVKC_SysReq = 317,
		EVKC_Break = 318,
		EVKC_Menu = 319,
		EVKC_Mouse0 = 323,
		EVKC_Mouse1 = 324,
		EVKC_Mouse2 = 325,
		EVKC_Mouse3 = 326,
		EVKC_Mouse4 = 327,
		EVKC_Mouse5 = 328,
		EVKC_Mouse6 = 329,
		//手柄按键没有搞过来，有需要的时候再搞过来吧
	};

	//按键状态
	ENUM EViewKeyState
	{
		EVKS_None = 0,
		EVKS_Up = 1,
		EVKS_Down = 2
	};
}
#endif	// __COMMON_STAGE_MANAGED_DEF_H__