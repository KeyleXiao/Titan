/*******************************************************************
** 文件名:	CommonManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/3/20
** 版  本:	1.0
** 描  述:

********************************************************************/

#pragma once
#include "IGameDatabaseDef.h"
#include "IEntity.h"
#include "TimerHandler.h"
#include "IDBEngine.h"
#include "IBankPart.h"
#include "SchemeDef.h"
#include "EntityDef.h"
#include <list>

class CPlayerBankPart;

typedef DBREQ_TABLE_LABEL LabelInfo;
typedef std::list<LabelInfo> LABEL_LIST;

class CCommonManager: public IDBRetSink
{
public:
	CCommonManager();
	~CCommonManager();

	/////////////////////////////////IDBRetSink/////////////////////////////////////////
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	///////////////////////////////////////////////从数据库读取数据/////////////////////////////////////////
private:
	// 读取玩家所有标签
	void readAllLabel();

	///////////////////////////////////////////////数据库返回数据/////////////////////////////////////////
private:
	// 玩家所有标签
	void onDBReturnAllLabel_Read(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	///////////////////////////////////////////////发送数据到客户端/////////////////////////////////////////
private:
	// 发送所有标签到客户端
	void sendAllLabelToClient();

private:
	// 发送数据到客户端
	void sendDataToClient(int nMsgID, void* data, size_t len);

public:
	bool onSerialize(rkt::ibuffer& in, int flag);

	bool deSerialize(rkt::obuf& out, int flag);

	bool init( __IEntity* pEntity,CPlayerBankPart* pPlayerBank);

	void restore();

    void release(void);

	__IEntity *		                            m_pMaster;

	CPlayerBankPart*                            m_pBankPart;

	// 标签数据
	LABEL_LIST									m_LabelList;
};
