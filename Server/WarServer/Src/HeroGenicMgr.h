/*******************************************************************
** 文件名: HeroGenicMgr.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/30
** 版  本: 1.0
** 描  述: 英雄基因玩法
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/
#pragma once

#include "IHeroGenicMgr.h"

class IWar;

class CHeroGenicMgr : public IHeroGenicMgr, public TimerHandler, public IEventExecuteSink
{
	enum ETimerEventID
	{
	};

	enum ETimerInternal
	{
	};

public:
	CHeroGenicMgr(IWar* pWar);
	~CHeroGenicMgr();

public:
	//////////////////////TimerHandler///////////////////
	virtual void OnTimer(unsigned long dwTimerID) override;

	///////////////////////////////////////IEventExecuteSink///////////////////////////////////
	/** 容器事件触发后执行函数
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return
	*/
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

public:
	////////////////////////IHeroGenicMgr start/////////////////////////
	virtual bool Init(int nMatchType) override;

	virtual void Release() override;

	// 战场正式开始
	virtual void onWarRealStart() override;

	// 战场结束
	virtual void onWarEnd() override;

	// 获得新的英雄基因
	virtual void onNewGenic(UID uid, int nAddHeroID, int nAddSkinID, const char* szDeadName);

	// 切换职业
	virtual void changeVocation(UID uid, int nSlotID);
	////////////////////////IHeroGenicMgr end/////////////////////////

	// 功能函数
private:

	// 事件处理
private:

	// 获取配置数据
private:

	// 发送消息到客户端
private:
	// 发送基因信息到客户端
	void sendGenicInfoToClient(UID uid);

private:
	// 战场
	IWar*	m_pWar;

	int m_nMatchTypeID;

};