/*******************************************************************
** 文件名:	ViewMngSetting.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/22
** 版  本:	1.0
** 描  述:	观战管理服配置信息
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/
#pragma once
#include "singleton.h"
#include <string>
#include "ViewDef.h"

class ViewMngSetting : public Singleton<ViewMngSetting>
{
public:
	ViewMngSetting() :
		m_wPortForGame(0)
		, m_wPortForView(0)
		, m_wPortForGate(0)
		, m_dwTraceLevel(0)
		, m_bAutoStart(FALSE)
		, m_bAutoHide(FALSE)
		, m_dwMaxNetSpeed(0)
		, m_wMinGateCount(1)
		, m_wMinViewCount(1)
		, m_bReady(false)
		, m_wRedisPort(0)
	{}

	// 读入配置文件
	bool LoadSetting(void);

	bool CheckReady();

	inline	bool	IsReady() const { return m_bReady; }
	inline	void	SetReady(bool val) { m_bReady = val; }

public:
	string			m_strGameName;		// 本服务器所服务的游戏名称
	string			m_strVersion;		// 版本号

	WORD			m_wPortForGame;		// 监听游戏服的端口
	WORD			m_wPortForView;		// 监听ViewServer的端口
	WORD			m_wPortForGate;		// 监听网关的端口

	string			m_strRedisIp;		// RedisIP
	WORD			m_wRedisPort;		// Redis端口

	WORD			m_wMinGateCount;	// 至少有这个数量的Gate，才能对外服务
	WORD			m_wMinViewCount;	// 至少有这个数量的View，才能对外服务

	DWORD           m_dwTraceLevel;		// trace日志级别
	BOOL			m_bAutoStart;		// 自动启动
	BOOL			m_bAutoHide;		// 自动隐藏窗口
	BOOL			m_bIsHide;

	DWORD			m_dwMaxNetSpeed;	// 最大网络带宽(Mbps)
private:
	bool			m_bReady;			// 是否准备好对外服务了
};
extern ViewMngSetting& gSetting;