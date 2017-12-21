/*******************************************************************
** 文件名:	IByteRecord.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/28
** 版  本:	1.0
** 描  述:	流量记录接口
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "IByteRecord.h"

class ByteRecord : public IByteRecord
{
public:
	ByteRecord() :m_dwRecv(0), m_dwSend(0) {}

	virtual void OnRecvData(DWORD dwDataLen) { m_dwRecv += dwDataLen; }

	virtual void OnSendData(DWORD dwDataLen) { m_dwSend += dwDataLen; }

	// 获取收到的字节数
	DWORD	GetRecvDataLength() { return m_dwRecv; }
	// 获取发送的字节数
	DWORD	GetSendDataLength() { return m_dwSend; }

	// 清空记录
	void	Clean() { m_dwRecv = 0; m_dwSend = 0; }

private:
	DWORD	m_dwRecv;		// 收包字节数
	DWORD	m_dwSend;		// 发包字节数
};