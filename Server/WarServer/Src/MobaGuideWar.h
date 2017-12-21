/*******************************************************************
** 文件名: MobaGuideWar.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/4/1
** 版  本: 1.0
** 描  述: 新手引导战场
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include <map>
#include "StdAfx.h"
#include "WarDef.h"
#include "IMonsterExec.h"
#include "MobaWar.h"
#include "EntityDef.h"


#include "War_ManagedDef.h"
#include "IMigrateService.h"

using namespace War;
using namespace std;

class CMobaGuideWar : public CMobaWar
{
public:
	CMobaGuideWar();
	~CMobaGuideWar();

	///////////////////////////////CWar///////////////////////////////////////////
	// 初始化
	virtual bool    create(IWarService* pWarService, SWarCreateContext& context) override;

	// 初始化
	virtual bool    Init()override;

	// 反初始化
	virtual void    UnInit()override;

	// 战场基地被摧毁
	virtual void    End()override;

	virtual void    Release()override;

	//////////////////////TimerHandler///////////////////////////////
	virtual void    OnTimer( unsigned long dwTimerID )override;

	///////////////////////IEventExecuteSink///////////////////////////////////////////////////
	/** 容器事件触发后执行函数
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	*/
	virtual void		OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)override;

private:
	///////////////////////////////////////新手任务相关/////////////////////////////////
	// 完成新手战场任务
	void FinishGuidWar();
};
