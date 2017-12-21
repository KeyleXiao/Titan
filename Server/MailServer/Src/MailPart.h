/*******************************************************************
** 文件名:	MailPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/7/2017
** 版  本:	1.0
** 描  述:	邮件部件类

**************************** 修改记录 ******************************
** 修改人: 李界华
** 日  期: 12/15/2017
** 描  述: 将玩家邮件消息直接转接到部件中，这样更统一和直观
********************************************************************/

#ifndef __MAILPART_H__
#define __MAILPART_H__

#include "IMailPart.h"
#include "MailMsgDef.h"
#include <unordered_map>
#include "IDBRetSink.h"



// 邮件部件类
class MailPart : public __IMailPart, public IEventExecuteSink, public IDBRetSink
{
// 玩家邮件列表
typedef unordered_map<LONGLONG, SMailDetailData>					TMap_MailList;
// 玩家邮件列表迭代器
typedef unordered_map<LONGLONG, SMailDetailData>::iterator			TMap_MailListIt;
public:
	// 构造函数
	MailPart(void);

	// 析构函数
	~MailPart(void);

public:
	// IEntityPart
	//////////////////////////////////////////////////////////////////////////
	virtual short getPartID();
	virtual bool onLoad( __IEntity* pEntity, int nLoginMode);
	virtual __IEntity * getHost();
	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);
	virtual void release();

	virtual bool onSerialize( rkt::ibuffer & in,int flag );

	virtual bool deSerialize( rkt::obuf & out,int flag );

	// 切换英雄
	virtual void onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore);

    virtual void restore();
	// IMailPart
	//////////////////////////////////////////////////////////////////////////
    // 删除邮件
    virtual bool    deleteMail(LONGLONG llMailID);


	// 处理邮件详细数据
	virtual bool dealPlayerMailDetailData(LONGLONG llMailID, bool isAutoRecvPlus = false);
	// 添加邮件详细数据
	virtual void addPlayerMailDetailData(SMailDetailData sDetailData, bool isAutoRecvPlus = false);
	// 处理邮件发送附件数据
	virtual bool dealSendMailPlusData(SMailSendData sendData);

	// 获取领取附件时间
	virtual DWORD getLastRecvPlusTime();

	// 设置领取附件时间
	virtual void setLastRecvPlusTime(DWORD dwTime);

	// 邮件是否已领取
	virtual bool isAreadyRecvMail(LONGLONG llMailID);

	// 是否在快速取件
	virtual bool isFastRecvMailPlus();

	// 设置快速取件标识
	virtual void setFastRecvMailPlus(bool bFlag, int nRecvCount);

    /////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    //////////////////////////////////IDBRetSink///////////////////////
    // 数据库请求返回回调方法
    virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

private:
    // 收到客户端请求邮件列表
    void onMessageRequestMailList(void * data, size_t len);

    // 收到客户端请求读取邮件
    void onMessageRequestReadMail(void * data, size_t len);

    // 收到客户端快速领取
    void onMessageRequestAsfRecv(void * data, size_t len);

    // 读取邮件详细数据
    void onReturnReadMailData(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

    // 删除邮件
    void onReturnDelMail(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);


	// 收取邮件附件
	bool dealRecvMailPlus(SMailDetailData *pDetailData);
	// 通知收件成功
	void NotifyMailRecvSuccess(DWORD dwPDBID, LONGLONG llMailID);
	// 通知快速收件结果
	void NotifyFastRecvRet(DWORD dwPDBID, LONGLONG llMailID, bool bRecvRet, bool bStopFlag);

private:
	__IEntity* m_pMaster;

	DWORD	m_dwLastRecvPlusTime;			// 最后收取附件时间
	bool	m_bIsFastRecv;					// 是否在快速取件
	int		m_FastRecvCount;				// 快速收件数目
	DWORD	m_FastRecvOverTime;				// 快速收件时间限制
	
	TMap_MailList	m_MailList;					// 邮件详细数据列表
	vector<LONGLONG>	m_RecvMailList;			// 已领取邮件列表

};

#endif // __MAILPART_H__