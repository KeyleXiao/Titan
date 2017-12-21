/*******************************************************************
** 文件名:	e:\Rocket\Server\Common\DMirrorServer.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Long)（Forlion@126.com）
** 日  期:	2008/10/30 17:54
** 版  本:	1.0
** 描  述:	镜像服务
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "Entity_ManagedDef.h"

#pragma pack(1)


// 镜像同步间隔时间
#define MIRROR_PERSON_INTERVAL								3000

// 人物镜像到社会服的现场
struct SMirrorPersonContext
{
	// UID
	LONGLONG		uid;

	// 名字
	char			szName[GAME_NAME_MAXSIZE];

	// 地图ID
	int				nMapID;

	// 场景ID
	DWORD			nSceneID;

	// 位置
	POINT			ptTile;

	// 其他属性
	int				nNumProp[PROPERTY_FACEID];

	// 来自哪个游戏世界
	int				nFromGameWorldID;

    // 服务器ID
    int             nServerID;

	SMirrorPersonContext(void)
	{
		memset(this, 0, sizeof(SMirrorPersonContext));
	}
};

/******************************************************************/
//////////////////////// 场景服的行为层的消息码/////////////////////
// 上层的消息码必定是MSG_MODULEID_MIRROR
/*****************************************************************/
///////////////////////////////////////////////////////////////////
// 观察某个人物
#define MSG_MIRROR_OBSERVEPERSON								1
// 消息体
struct SMsgMirrorObservePerson_OS
{
	DWORD	dwPDBID;				// 观察人物PDBID
	bool	bObserve;				// true：观察 false：取消
};

// 更新镜像对像
#define MSG_MIRROR_UPDATEPERSON									2
// 消息体
struct SMsgMirrorUpdatePerson_SO
{
	DWORD	dwPDBID;				// 更新人物PDBID
	bool	bLogin;					// 是否上线
	bool	bLogout;				// 是否下线
	bool	bOffline;				// true=新登陆，false=切换场景登陆

	//...........					// 镜像数据
};

// 清除所有观察镜像
#define MSG_MIRROR_CLEANOBSERVER								3
// 消息体
struct SMsgMirrorCleanObserver_OS
{
	
};


#pragma pack()