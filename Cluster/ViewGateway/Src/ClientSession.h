/*******************************************************************
** 文件名:	ClientSession.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/12/06
** 版  本:	1.0
** 描  述:	Gate上的Client连接对象
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "SessionUser.h"


class ClientSession : public SessionUser<ClientSession>
{
public:
	ClientSession(IConnection * conn, IUserList& pUserList, Processer<ClientSession>& processer, TimerAxis& timerAxis, IByteRecord* pByteRecord);
	virtual ~ClientSession();

	inline	time_t		GetConnectTime(void) { return m_tConnectTime; }

	virtual std::string ToString();

	// 保存Mac地址，虽然暂时不知道有什么用
	void SetMacAddress(char szMacAddress[]);

protected:
	virtual WORD		GetKeepAliveID();

	virtual	const obuf&	GetAnswerKeepAliveMsg();

protected:
	time_t		m_tConnectTime;						//	连入时间
	char		m_szMacAddress[MAC_ADDRESS_SIZE];	// 网卡地址
};