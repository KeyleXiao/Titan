
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"
#include <vector>
#include <string>

using namespace std;

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持



#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

/************************************************************************/
/* 网络模块通用头                                                          */
/************************************************************************/
#include "Common.h"

using namespace rkt;

#include "Trace.h"
#include "MatchDef.h"
#include "Net/net.h"
#include "NetMessageDef.h"


#include "ISerializable.h"
#include "TimerAxis.h"
#include "TimeSyncClient.h"
#include "EntityDef.h"
#include "MatchMsgC2MSDef.h"

enum GAME_MODE
{
    MODE_MMO,
    MODE_MOBA
};


#pragma pack(1)

/////////////////////////// 技能模块消息  ////////////////////////////
// 消息码定义键值
enum SPELL_NET_MESSAGE
{
	SPELL_MSG_PREPARE = SC_MSG_SPELL_START,		    // 预施法
	SPELL_MSG_CAST,                                 // 技能施法
	SPELL_MSG_HIT,									// 击中消息
	SPELL_MSG_TALENT_CHANGE,						// 天赋数据改变
};


// 技能现场对象实现
struct SSpellContextImp
{
	UID			uidOperator;		// 使用技能的操作者
	UID			uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
	Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
	int			nID;				// 技能ID
	int			nViewID;			// 技能视图ID
	DWORD		dwSerialID;			// 序列号ID

	SSpellContextImp()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 客户端-场景服 施法技能消息
struct SSpellContextImp_CS
{
	UID			uidOperator;		// 使用技能的操作者（不一定是发起者，如载具上面的人），如果填0，表示操作者为技能部件拥有者
	UID			uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
	Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
	int			nID;				// 技能ID

	SSpellContextImp_CS()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-客户端 施法技能消息
struct SSpellContextImp_SC
{
	ulong		uidOperator;		// 使用技能的操作者（不一定是发起者，如载具上面的人），如果填0，表示操作者为技能部件拥有者
	ulong		uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
	Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
	int			nID;				// 技能ID

	SSpellContextImp_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 天赋改变类型
enum TALENT_CHANGE_TYPE
{
	TALENT_ADD,			// 天赋增加
	TALENT_REMOVE,		// 天赋移除
	TALENT_CHANGE,		// 天赋改变
};


// 场景服-客户端 天赋数据改变
struct SSpellTalentChange_SC
{
	TALENT_CHANGE_TYPE		nChangeType;	// 天赋改变类型
	int						nID;			// 技能ID
	int						nTalentID;      // 天赋ID
	int						nType;			// 天赋类型
	int						nIndex;			// 技能数据ID
	char					szData[256];	// 文字类数据
	int						nValue;			// 数值

	SSpellTalentChange_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-场景服 击中消息
struct SSpellHit_SS
{
	ulong		uidOperator;		// 使用技能的操作者（不一定是发起者，如载具上面的人），如果填0，表示操作者为技能部件拥有者
	ulong		uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
	Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
	int			nID;				// 技能ID

	SSpellHit_SS()
	{
		memset(this, 0, sizeof(*this));
	}
};


// 场景服-客户端 击中消息
struct SSpellHit_SC
{
	ulong		uidOperator;		// 使用技能的操作者（不一定是发起者，如载具上面的人），如果填0，表示操作者为技能部件拥有者
	ulong		uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
	Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
	int			nID;				// 技能ID

	SSpellHit_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};

//-----------------------------技能相关---------------------------------
#pragma pack()
