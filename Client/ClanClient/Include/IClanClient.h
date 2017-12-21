/*******************************************************************
** �ļ���:	IClanClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	4/7/2016
** ��  ��:	1.0
** ��  ��:	���ͻ��˽ӿ�
********************************************************************/

#ifndef __ICLANCLIENT_H__
#define __ICLANCLIENT_H__

struct IClanClient
{
	// �ͷ�
	virtual void release(void) = 0;

    // �ܷ񷢲����˱���
    virtual bool canPublicClanLegenCup() = 0;
};


#if defined(_LIB) || defined(CLANCLIENT_STATIC_LIB)	// ��̬����
#	pragma comment(lib, MAKE_LIB_NAME(ClanClient))
extern "C" IClanClient * CreateClanClient(void);
#	define	CreateClanClientProc	CreateClanClient
#	define	CloseClanClientProc()
#elif !defined(CLANCLIENT_EXPORTS) && defined(DYN_LIB) // ��̬����
#	pragma comment(lib, MAKE_DLL_LIB_NAME(ClanClient))
API_IMPORT IClanClient * CreateClanClient(void);
#	define	CreateClanClientProc	CreateClanClient
#	define	CloseClanClientProc()
#else													// ��̬����
typedef IClanClient * (RKT_CDECL *procCreateClanClient)(void);
#	define	CreateClanClientProc	DllApi<procCreateClanClient>::load(MAKE_DLL_NAME(ClanClient), "CreateClanClient")
#	define	CloseClanClientProc	DllApi<procCreateClanClient>::freelib
#endif

#endif // __ICLANCLIENT_H__