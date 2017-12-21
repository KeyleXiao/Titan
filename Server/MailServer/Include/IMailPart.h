/*******************************************************************
** �ļ���:	__IMailPart.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��ҫ��
** ��  ��:	2/7/2017
** ��  ��:	1.0
** ��  ��:	�ʼ�	

********************************************************************/

#pragma once
#include "IEntity.h"
#include "MailDef.h"

struct __IMailPart:public __IEntityPart
{
    // ɾ���ʼ�
    virtual bool deleteMail(LONGLONG llMailID) = 0;
	// �����ʼ���ϸ����
	virtual bool dealPlayerMailDetailData(LONGLONG llMailID, bool isAutoRecvPlus = false) = 0;
	// ����ʼ���ϸ����
	virtual void addPlayerMailDetailData(SMailDetailData sDetailData, bool isAutoRecvPlus = false) = 0;
	// �����ʼ����͸�������
	virtual bool dealSendMailPlusData(SMailSendData sendData) = 0;

	// ��ȡ��ȡ����ʱ��
	virtual DWORD getLastRecvPlusTime() = 0;

	// ������ȡ����ʱ��
	virtual void setLastRecvPlusTime(DWORD dwTime) = 0;

	// �ʼ��Ƿ�����ȡ
	virtual bool isAreadyRecvMail(LONGLONG llMailID) = 0;

	// �Ƿ��ڿ���ȡ��
	virtual bool isFastRecvMailPlus() = 0;

	// ���ÿ���ȡ����ʶ
	virtual void setFastRecvMailPlus(bool bFlag, int nRecvCount) = 0;
};

//////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IMailPart * CreateMailPart();
#	define	CreateMailPartProc	CreateMailPart
#else													/// ��̬��汾
typedef __IMailPart * (RKT_CDECL * procCreateMailPart)();
#	define	CreateMailPartProc	DllApi<procCreateMailPart>::load(MAKE_DLL_NAME(MailServer), "CreateMailPart")
#endif
