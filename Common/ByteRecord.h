/*******************************************************************
** �ļ���:	IByteRecord.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/11/28
** ��  ��:	1.0
** ��  ��:	������¼�ӿ�
** Ӧ  ��:  	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "IByteRecord.h"

class ByteRecord : public IByteRecord
{
public:
	ByteRecord() :m_dwRecv(0), m_dwSend(0) {}

	virtual void OnRecvData(DWORD dwDataLen) { m_dwRecv += dwDataLen; }

	virtual void OnSendData(DWORD dwDataLen) { m_dwSend += dwDataLen; }

	// ��ȡ�յ����ֽ���
	DWORD	GetRecvDataLength() { return m_dwRecv; }
	// ��ȡ���͵��ֽ���
	DWORD	GetSendDataLength() { return m_dwSend; }

	// ��ռ�¼
	void	Clean() { m_dwRecv = 0; m_dwSend = 0; }

private:
	DWORD	m_dwRecv;		// �հ��ֽ���
	DWORD	m_dwSend;		// �����ֽ���
};