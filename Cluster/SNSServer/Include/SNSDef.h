/*******************************************************************
** �ļ���:	ControllerDef.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	��绪
** ��  ��:	11/24/2016
** ��  ��:	1.0
** ��  ��:	�пط�������RCMS-Root Controller Manage Server)ģ�鶨��	
** Ӧ  ��:  	
	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/

#ifndef __SNS_DEF_H__
#define __SNS_DEF_H__


/************************************************************************/
/* ������Ϣ����궨��                                                   */
/************************************************************************/
// ���DEBUG��Ϣ
#ifdef VERSEION_DEBUG
#   define SNS_DEBUG_OUTPUT
#endif
#ifdef SNS_DEBUG_OUTPUT
#	define SNS_TRACELN(x)			TraceLn(x)
#	define SNS_EMPHASISLN(x)		EmphasisLn(x)
#	define SNS_ERRORLN(x)			ErrorLn(x)
#	define SNS_WARNINGLN(x)			WarningLn(x)
#else
#	define SNS_TRACELN(x)
#	define SNS_EMPHASISLN(x)
#	define SNS_ERRORLN(x)
#	define SNS_WARNINGLN(x)
#endif


#define     SERVER_NAME_MAXSIZE             32
#define     SNS_DEFAULT_LISTEN_PORT         8001		// �пط�����Ĭ�ϼ����˿�

/***************************************************************/
// �ϲ����Ϣ��ض��� MSG_MODULEID_SNS
/***************************************************************/

///////////////////////////////////////////////////////////////////
// ��Ϣ��ö�ٶ���: ENUM + ��Ϣ����
enum EM_MSG_SNS
{
    MSG_SNS_INVALID = 0,
    MSG_SNS_HANDSHAKE,						// ������Ϣ
    MSG_SNS_HANDSHAKE_RESPONSE,				// ��Ӧ������Ϣ
    MSG_SNS_KEEPALIVE,						// �������
    MSG_SNS_ANSWER_KEEPALIVE,				// ��Ӧ�������
    

    MSG_SNS_MAXID,							// �����Ϣ��
};

// MSG_SNS_HANDSHAKE		            �������������������Ϣ���ݽṹ
struct SMsgSNSHandshake_SR
{
    DWORD	dwWorldID;							// ������������Ϸ����ID
    WORD	nServerType;						// ����������
    WORD	wSubID;								// ��n�ŷ�����
    char	szName[SERVER_NAME_MAXSIZE];		// ��������

    SMsgSNSHandshake_SR(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// MSG_SNS_HANDSHAKE_RESPONSE		 ��������Ӧ������Ϣ���ݽṹ
struct SMsgSNSHandshakeResponse	
{
    DWORD	dwID;								// ����ID
    DWORD	dwServerTime;						// ������ʱ��

    SMsgSNSHandshakeResponse(void)
    {
        memset(this, 0, sizeof(*this));
    }
};


#endif //__SNS_DEF_H__
