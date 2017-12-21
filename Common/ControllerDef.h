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

#ifndef __CONTROLLER_DEF_H__
#define __CONTROLLER_DEF_H__


/************************************************************************/
/* ������Ϣ����궨��                                                   */
/************************************************************************/
// ���DEBUG��Ϣ
#ifdef VERSEION_DEBUG
#   define CONTROLLER_DEBUG_OUTPUT
#endif
#ifdef CONTROLLER_DEBUG_OUTPUT
#	define CONTROLLER_TRACELN(x)			TraceLn(x)
#	define CONTROLLER_EMPHASISLN(x)		    EmphasisLn(x)
#	define CONTROLLER_ERRORLN(x)			ErrorLn(x)
#	define CONTROLLER_WARNINGLN(x)			WarningLn(x)

#else
#	define CONTROLLER_TRACELN(x)
#	define CONTROLLER_EMPHASISLN(x)
#	define CONTROLLER_ERRORLN(x)
#	define CONTROLLER_WARNINGLN(x)
#endif

#define     SERVER_NAME_MAXSIZE             32
#define     CONTROL_DEFAULT_LISTEN_PORT     8001    // �пط�����Ĭ�ϼ����˿�
#define     DATABASE_SVR_TYPE               1       // ����һ�����ݿ�ID
#define     CONTROL_ROOT_DBID               1       // �����������������ݿ�ID


//////////////////////////////////////////////////////////////////////////
// ��Դ����
enum CONTROL_RESOURCE_TYPE
{
    RESOURCE_TYPE_NONE = 0x0,                   // ��Ч
    RESOURCE_TYPE_DBSERVER = 0x1,               // DBServer
    RESOURCE_TYPE_DLL = 0x2,                    // DLL
    RESOURCE_TYPE_SCRIPT = 0x4,                 // �ű�

    RESOURCE_TYPE_SERVER_EXCLUE_SCRIPT      = RESOURCE_TYPE_DBSERVER | RESOURCE_TYPE_DLL,
    RESOURCE_TYPE_SERVER_EXCLUE_DBSERVER    = RESOURCE_TYPE_DLL | RESOURCE_TYPE_SCRIPT,
    RESOURCE_TYPE_SERVER_ALL    = RESOURCE_TYPE_DBSERVER|RESOURCE_TYPE_DLL|RESOURCE_TYPE_SCRIPT,  // ������ȫ����Դ

    RESOURCE_TYPE_MAX,                          // ���
};

// ����DBServer����
enum LOAD_DATABASE_DBS_TYPE
{
    LOAD_DATABASE_DBS_TYPE_NORMAL = 0,          // ��ͨ��
    LOAD_DATABASE_DBS_TYPE_GLOBAL = 2,          // ȫ�������飨���ڶ�ȡ�����ԡ����������ѵ�ȫ�������ã�
};

/***************************************************************/
// �ϲ����Ϣ��ض��� MSG_MODULEID_CONTROLLER
/***************************************************************/
#pragma pack(1)
// �п�������Ϣ����Ϣͷ
struct SControlMsgHead
{
    // Դ�˵���
    BYTE		SrcEndPoint;

    // Ŀ��˵���
    BYTE		DestEndPoint;	

    // Ŀ��ģ����Ϣ��
    BYTE		KeyModule;

    // ��Ϊ��Ϣ��
    BYTE		KeyAction;
};

///////////////////////////////////////////////////////////////////
// ��Ϣ��ö�ٶ���: ENUM + ��Ϣ����
enum EM_MSG_CONTROLLER
{
    MSG_CONTROLLER_INVALID = 0,
    MSG_CONTROLLER_HANDSHAKE,						// ������Ϣ
    MSG_CONTROLLER_HANDSHAKE_RESPONSE,				// ��Ӧ������Ϣ
    MSG_CONTROLLER_KEEPALIVE,						// �������
    MSG_CONTROLLER_ANSWER_KEEPALIVE,				// ��Ӧ�������

    MSG_CONTROLLER_REQUEST_LOCALRUN_AREALIST,       // ���󱾵���Դ������Ϸ�����б�
    MSG_CONTROLLER_REQUEST_LOCALRUN_AREALIST_RESULT,// ���󱾵���Դ������Ϸ�����б�ش�
    MSG_CONTROLLER_REQUEST_RESOURCE,                // ������Դ
    MSG_CONTROLLER_REQUEST_RESULT,                  // ��Դ����ش�
    
    MSG_CONTROLLER_RELOAD_RESOURCE,                 // �ؼ�����Դ

    MSG_CONTROLLER_MAXID,							// �����Ϣ��
};

// MSG_CONTROLLER_HANDSHAKE		            �������������������Ϣ���ݽṹ
struct SMsgControllerHandshake
{
    char	szName[SERVER_NAME_MAXSIZE];		// ��������

    SMsgControllerHandshake(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// MSG_CONTROLLER_HANDSHAKE_RESPONSE		 ��������Ӧ������Ϣ���ݽṹ
struct SMsgControllerHandshakeResponse	
{
    DWORD	dwID;								// ����ID
    DWORD	dwServerTime;						// ������ʱ��

    SMsgControllerHandshakeResponse(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

// MSG_CONTROLLER_REQUEST_LOCALRUN_AREALIST          ���󱾵���Դ������Ϸ�����б�
struct SMsgControlRequestLocalRunAreaList
{
    DWORD    dwGameID;                   // ��ϷID

    SMsgControlRequestLocalRunAreaList()
    {
        memset(this, 0, sizeof(*this));
    }
};

// MSG_CONTROLLER_REQUEST_LOCALRUN_AREALIST_RESULT   ���󱾵���Դ������Ϸ�����б�ش�
struct SMsgControlRequestLocalRunAreaListResult
{
    DWORD    dwGameID;                  // ��ϷID
    WORD    wCount;                     // ��Ϸ����ID����
    // wCount * DWORD

    SMsgControlRequestLocalRunAreaListResult()
    {
        memset(this, 0, sizeof(*this));
    }
};


// MSG_CONTROLLER_REQUEST_RESOURCE           ������Դ
struct SMsgControlRequestResource
{
    DWORD dwGameID;                     // ��ϷID
    DWORD dwWorldID;					// ��Ϸ����ID
    BYTE nSourceServerType;				// ����������
    DWORD dwResourceType;               // ��Դ���� CONTROL_RESOURCE_TYPE

    SMsgControlRequestResource()
    {
        memset(this, 0, sizeof(*this) );
    }
};

// MSG_CONTROLLER_REQUEST_RESULT               ��Դ����ش�
struct SMsgControlRequestResourceResult
{
    BYTE nEncry;                        // ���ܷ�ʽ
    BYTE nResourceType;                 // ��Դ����
    DWORD dwScriptGroupID;			    // �ű��ļ���ID
    BYTE nMaxSchemeIndex;				// ������ú�
    BYTE nSchemeIndex;					// ���úţ���һ�Žű���ʼ��ȡ��
    DWORD dwDataLens;					// �����ú����ݳ���
    // BYTE * pData;					// ��������

    SMsgControlRequestResourceResult(void)
    {
        memset(this, 0, sizeof(*this));
    }
};

//MSG_CONTROLLER_RELOAD_RESOURCE                 �ؼ�����Դ
//SMsgControlRequestResource

#pragma pack()


#endif //__CONTROLLER_DEF_H__
