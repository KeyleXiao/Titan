/*******************************************************************
** 文件名:	TalentPageManager.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/8/16
** 版  本:	1.0
** 描  述:	天赋页系统

********************************************************************/
#ifndef __TALENTPAGEMANAGER_H__
#define __TALENTPAGEMANAGER_H__

#include "IBankPart.h"
#include "EntityDef.h"
#include "IDBEngineService.h"
#include "ITalentPageManager.h"

class CPlayerBankPart;
class TalentEffectProxy;

#ifdef FUNCTION_TALENTPAGE_OPEN
// 符文系统管理类
class CTalentPageManager : public ITalentPageManager, public IDBRetSink
{
public:
	/////////////////////////////////IDBRetSink/////////////////////////////////////////
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	//////////////////////////////////ITalentPageManager//////////////////////
	// 获取当前天赋页
	int virtual GetCurTalentPage();

public:
	// 构造函数
	CTalentPageManager();
	// 析构函数
	virtual ~CTalentPageManager();

public:
	bool onSerialize(rkt::ibuffer& in);

    bool onSerializePage(rkt::ibuffer& in);


	bool deSerialize(rkt::obuf& out,int flag );

	bool deSerializePage(rkt::obuf& out);

	/** 创建
	*/
	bool onLoad( __IEntity* pEntity, CPlayerBankPart *pBankPart, TalentEffectProxy* pEffectProxy);

	/** 重置 
	*/
	void restore();

    // 释放
    void release(void);

    bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);

public:
	// 从数据库请求所有天赋点
	void RequestDataBaseAllTalentPageInfo();

	// 调用当前选定页的效果
	void SetTalentPageEffect();

private:
	//////////////////////////////////////天赋页////////////////////////////////////
	// 客户端请求保存某个天赋页的加点信息
	bool OnClientRequestSetTalentPageAddPointsInfo(void* pData, int nLen);
	// 客户端请求删除当前选定页
	void OnClientRequestDeleteTalentPage(int nCurTalentPage);

	// 数据库所有天赋页的加点数据
	void OnDBReturnTalentPagesPoints(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	// 发送天赋页数据到客户端
	void SendTalentPageInfoAllToClient(void * pData, int nOutLen);

private:
	__IEntity *                     m_pMaster;

	CPlayerBankPart *               m_pBankPart;

	TalentEffectProxy*              m_pEffectProxy;

	// 天赋页加点情况，效验数据
	typedef map<int, TALENT_INFO_VECTOR> TALENTPAGE_MAP;
	TALENTPAGE_MAP                  m_mapTalentPage;

	int								m_nTalentEffectPageIndex;
};

#endif

#endif	// __TALENTPAGEMANAGER_H__