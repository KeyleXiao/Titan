/*******************************************************************
** 文件名: EquipSchemeMgr.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/4/18
** 版  本: 1.0
** 描  述: 装备管理
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once
#include "StdAfx.h"
#include "GameDef.h"
#include "WarDef.h"
#include "IEquipSchemeMgr.h"
#include <map>

#define WarOftenUseEquipTime	1000*60*8	// 8分钟

class CEquipSchemeMgr : public IEquipSchemeMgr, public TimerHandler, public IEventExecuteSink, public IDBRetSink
{
	enum
	{
		ETimerEventID_OftenUseEquip = 1,		// 计算常用装备
	};

	enum EWarEquipType
	{
		EWarEquipType_Early,		// 前期装备
		EWarEquipType_Main,			// 核心装备
	};


	// <英雄ID, <方案ID, 方案> >
	typedef std::map<int, map<int, SWarEquipScheme> > WAR_EQUIPSCHEME_MAP;
	typedef WAR_EQUIPSCHEME_MAP::iterator WAR_EQUIPSCHEME_MAP_ITER;

public:
	CEquipSchemeMgr();
	~CEquipSchemeMgr();


public:
	/////////////////////////////IEquipSchemeMgr/////////////////////////////////////////////
	// 初始化
	virtual bool init(__IEntity* pMaster, LONGLONG WarDBKey, int nMatchTypeID);

	// 释放
	virtual void release();

	// 序列化
	virtual bool onSerialize(rkt::ibuffer & in, int flag);

	// 反序列化
	virtual bool deSerialize(rkt::obuf & out, int flag);

	// 添加装备
	virtual void addEquip(int nEquipID);

	// 撤销装备
	virtual void revocationEquip();

	// 战场结束
	virtual void onWarEnd();

	// 客户端消息
	virtual void onMessage(int msgId, void * data, size_t len);

	// 是否开启了常用装备保存
	virtual bool isSaveOftenUseEquip() { return m_isSaveOftenUseEquip; }
public:
    /////////////////////////////TimerHandler/////////////////////////////////////////////
    virtual void OnTimer( unsigned long dwTimerID );

public:
	///////////////////////////////////////IDBRetSink///////////////////////////////////////
	virtual void			OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

public:
	/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/**
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return
	@note
	@warning
	@retval buffer
	*/
	virtual void			OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

private:
	///////////////////////////////////////保存常用装备//////////////////////////////
	// 初始化常用装备保存功能
	void initOftenUseEquipFunc();
	// 计算常用前期装备
	void calcOftenUseEquipOfEarly();
	// 计算常用核心装备
	void calcOftenUseEquipOfMain();
	// 保存常用装备
	void saveOftenUseEquip(const vector<SEquipWeightInfo>& allEquipWeidhtList, EWarEquipType nEquipType);
	// 获取常用前期装备
	void getOftenUseEquipOfEarly(vector<SEquipWeightInfo>& infoList);
	// 获取常用核心装备
	void getOftenUseEquipOfMain(vector<SEquipWeightInfo>& infoList);
	// 计算新的常用装备
	void calcNewOftenUseEquip(vector<SEquipWeightInfo>& curInfoList, const vector<SEquipWeightInfo>& oldInfoList, vector<SEquipWeightInfo>& newInfoList);
	// 给常用方案填充一个方案（根据脚本配置）
	void initDefaultOftenUseScheme();

private:
	/////////////////////////////////////////数据库返回函数///////////////////////////////
	// 读取装备方案
	void onDBRetEquipSchemeList_Read(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// 保存装备方案
	void onDBRetEquipSchemeList_Save(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// 删除装备方案
	void onDBRetEquipSchemeList_Del(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

private:
	// 打印装备方案,For Debug
	void printEquipSchemeInfo(int nHeroID, const SWarEquipScheme& scheme, const char* debugInfo);
	// 获取装备方案
	const SWarEquipScheme* getEquipScheme(int nHeroID, byte bySchemeID);
	// 获取空闲的装备方案ID
	byte getFreeEquipSchemeID(int nHeroID);
	// 是否存在装备方案
	bool isExistEquipSchemeID(int nHeroID, byte bySchemeID);
	// 去数据库查找英雄装备方案
	bool readEquipSchemeFromDB(int nHeroID);

private:
	// 发送消息到客户端的MobaGoods部件
	void sendClientMessage(BYTE byKeyAction, LPCSTR pszData, int nDataLen);
	// 发送装备方案到客户端
	void sendEquipSchemeToClient(int nHeroID);
	void sendEquipSchemeToClient(int nHeroID, const SWarEquipScheme& scheme);

private:
	// 客户端请求增加装备方案
	void onClientAddEquipScheme(void * data, size_t len);
	// 客户端请求删除装备方案
	void onClientDelEquipScheme(void * data, size_t len);
	// 客户端请求修改装备方案
	void onClientModifyEquipScheme(void * data, size_t len);
	// 客户端请求查询装备方案
	void onClientReadEquipScheme(void * data, size_t len);

private:
	__IEntity *	    m_pMaster;

	// 比赛类型
	int		m_nMatchTypeID;

	// 装备方案
	WAR_EQUIPSCHEME_MAP m_EquipSchemeMap;

	// 装备购买记录
	std::stack<int>	m_AllEquipList;

	PDBID m_Pdbid;
	// 当前战场内使用的英雄ID
	int m_nCurHeroID;
	// 世界ID
	int m_nWorldID;



	///////////////////////////////////////保存常用装备//////////////////////////////
	// 是否开启常用装备保存功能
	bool m_isSaveOftenUseEquip;
	// 基本权重
	int m_nBaseWeight;
	// 衰减率
	float m_fDampRatio;
};