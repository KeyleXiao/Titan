/*******************************************************************
** 文件名:	ServerUser.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015-8-17
** 版  本:	1.0
** 描  述:	数据监护服务器用户
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "ServerUser.h"
#include "ServerList.h"
#include "GlobalGuardServer.h"
#include "GuardSetting.h"
#include "ImageDataManager.h"

#include <strstream>

// 打印该服务器信息
#define  DUMP_THIS_SERVER  ToString()

///////////////////////////////////////////////////////////////////////////////////////////////
ServerUser::ServerUser(IConnection * conn) : m_pConnection(conn),m_dwID(0)
{
	const char * ip = 0;
	conn->GetRemoteAddr(&ip,m_dwRemotePort);
	assert(ip!=0);
	m_szRemoteIP = ip;

	m_bIsPublic = false;						// 是否是公共区的
	m_bIsAlive	= true;							// 是否有心跳
	m_bIsAnswer	= true;							// 是否有回应
	m_dwLastAnswerTime	= GetTickCount();		// 最后收到心跳Tick时间

	m_dwAddTime = (DWORD)time(NULL);			// 连入时间
	m_dwWorldID	= 0;							// 服务器所在游戏世界ID
	m_wServerType = 0;							// 服务器类型,会根据参见消息结构头第一个字段转发消息给不同的服务器
	m_wSubID = 0;								// 第n号服务器
	m_dwStateFlag = 0;							// 服务器状态

	memset(m_szName,0,sizeof(m_szName));

	TraceLn("A server connected!" << DUMP_THIS_SERVER);

}

///////////////////////////////////////////////////////////////////////////////////////////////
ServerUser::~ServerUser()
{
}


void ServerUser::Release()
{
	IGuardServer* pGlobalServer = gGlobalServer;
	if (pGlobalServer!=NULL)
	{
		pGlobalServer->OnServerUserLeave(this);
	}

	ServerList::getInstance().DelUser(this);
	if ( m_pConnection )
	{
		m_pConnection->Release();
		m_pConnection=0;
	}
	delete this;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void ServerUser::OnDisconnected( IConnection * conn,DWORD reason )
{
	TraceLn("Connection disconnected!" << DUMP_THIS_SERVER);
	Release();
}

///////////////////////////////////////////////////////////////////////////////////////////////
void ServerUser::OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen )
{
	assert(conn==m_pConnection);
	if ( dwDataLen<sizeof(SGameMsgHead))
	{
		WarningLn("Package length is too short.");
		return;
	}

	Assert(pData != NULL);
	SGameMsgHead* head = (SGameMsgHead*)pData;
	ulong moduleId = head->byKeyModule;
	uchar* data = (uchar*)pData + sizeof(SGameMsgHead);
	size_t new_len =(size_t)(dwDataLen-sizeof(SGameMsgHead));
	if (new_len == 0) data = 0;

	if (head->byKeyModule!=MSG_MODULEID_GUARD)
	{
		TraceLn("Received an invalid package from server " << DUMP_THIS_SERVER << " module=" << head->byKeyModule);
		return;
	}


	switch (head->byKeyAction)
	{
	case MSG_GUARD_WRITE_DATA:
		{
			OnMsgWriteImageData(head->byKeyAction, head, data, new_len);
		}
		break;

	case MSG_GUARD_READ_DATA:
		{
			OnMsgReadImageData(head->byKeyAction, head, data, new_len);
		}
		break;

	default:
		{
			gGlobalServer->onServerMessage((*this), head->byKeyAction, head, data, new_len);
		}
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////
void ServerUser::OnError( IConnection * conn,DWORD dwErrorCode )
{
	TraceLn("Connection occured an error!" << dwErrorCode << DUMP_THIS_SERVER);
	Release();
}

///////////////////////////////////////////////////////////////////////////////////////////////
DWORD ServerUser::GetServerID()
{
	return m_dwID;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void ServerUser::SetServerID(DWORD dwID)
{
	m_dwID = dwID;
}

///////////////////////////////////////////////////////////////////////////////////////////////
WORD ServerUser::GetServerType()
{
	return m_wServerType;
}


const char * ServerUser::GetRemoteIP()
{
	return m_szRemoteIP.c_str();
}

DWORD ServerUser::GetRemotePort()
{
	return m_dwRemotePort;
}

///////////////////////////////////////////////////////////////////////////////////////////////
bool ServerUser::SendData( const char * pData,DWORD dwDataLen,DWORD dwFlag)
{
	Assert(m_pConnection);
	if (!m_pConnection)	{	return false;	}
	return m_pConnection->SendData(pData,dwDataLen,dwFlag);
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::string ServerUser::ToString()
{
	ostrstream buffer;
	buffer << " server["<<m_szName<<",ID="<< m_dwID << ",WorldID="<<m_dwWorldID<<",Type=" << m_wServerType<< ",SubID="<<m_wSubID<<"][" << m_szRemoteIP <<":" << m_dwRemotePort <<"]" << std::ends;
	return buffer.str();
}

/** 收到桥服连接器的检测请求，向各个服务器转发，并将桥服本身的数据传回
@param   
@param   
@return  
*/
void ServerUser::HandleServerCheckReq(LPVOID pData,DWORD dwDataLen)
{
	if (dwDataLen != sizeof(SGameMsgHead) + sizeof(SMsgGuardServerCheck))
	{
		ErrorLn("ServerConnection::HandleServerCheckReq(): Length of Message is Invalid, dwDataLen=" << dwDataLen);
		return;
	}

	SGameMsgHead* pMsgHead = (SGameMsgHead*)pData;

	SMsgGuardServerCheck* pServerCheck = (SMsgGuardServerCheck*)(pMsgHead + 1);


	// 将数据发回到原始来源处
	if (pServerCheck->wServerType == MSG_ENDPOINT_BRIDGE)
	{
		pMsgHead->SrcEndPoint = MSG_ENDPOINT_GUARD;
		pMsgHead->DestEndPoint= (BYTE)m_wServerType;
		pMsgHead->byKeyModule  = MSG_MODULEID_GUARD;
		pMsgHead->byKeyAction  = MSG_GUARD_SERVERCHECKRES;

		m_pConnection->SendData((LPCSTR)pData, dwDataLen);
		return;
	}

	// 实行数据转发
	ServerUser* pServerUser = ServerList::getInstance().GetUser(pServerCheck->nToServerID);
	if (pServerUser == NULL)
	{
		return;
	}

	pMsgHead->SrcEndPoint = MSG_ENDPOINT_BRIDGE;
	pMsgHead->DestEndPoint= (BYTE)pServerUser->GetServerType();
	pMsgHead->byKeyModule  = MSG_MODULEID_GUARD;
	pMsgHead->byKeyAction  = MSG_GUARD_SERVERCHECKREQ;

	pServerUser->SendData((LPCSTR)pData, dwDataLen);


}
/** 收到中心服连接器的回应，进行转发
@param   
@param   
@return  
*/
void ServerUser::HandleServerCheckRes(LPVOID pData,DWORD dwDataLen)
{
	if (dwDataLen != sizeof(SGameMsgHead) + sizeof(SMsgGuardServerCheck))
	{
		ErrorLn("ServerConnection::HandleServerCheckRes():Length of Message is Invalid, dwDataLen=" << dwDataLen);
		return;
	}

	SGameMsgHead* pMsgHead = (SGameMsgHead*)pData;

	SMsgGuardServerCheck* pServerCheck = (SMsgGuardServerCheck*)(pMsgHead + 1);


	// 实行数据转发
	ServerUser* pServerUser = ServerList::getInstance().GetUser(pServerCheck->nFromServerID);
	if (pServerUser == NULL)
	{
		return;
	}

	pMsgHead->SrcEndPoint = MSG_ENDPOINT_BRIDGE;
	pMsgHead->DestEndPoint= (BYTE)pServerUser->GetServerType();
	pMsgHead->byKeyModule  = MSG_MODULEID_GUARD;
	pMsgHead->byKeyAction  = MSG_GUARD_SERVERCHECKRES;

	pServerUser->SendData((LPCSTR)pData, dwDataLen);
}

/** 收到往数据监护服务器写入数据
@param actionId		: 
@param head	        : 
@param data	        : 数据
@param len	        : 数据长度
@return
*/
void ServerUser::OnMsgWriteImageData(ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	if (len < sizeof(SMsgGuardWriteData))
	{
		ErrorLn("ServerUser::OnMsgWriteImageData():length of message is invalid, len="<< len <<",servertype="<<m_wServerType);
		return;
	}
	SMsgGuardWriteData *pMsg = (SMsgGuardWriteData*)data;
	if (len != sizeof(SMsgGuardWriteData) + pMsg->dwDataLen)
	{
		ErrorLn("ServerUser::OnMsgWriteImageData():message length is invalid, len="<< len <<",servertype="<<m_wServerType);
		return;
	}

	CImageDataBank * pBank = CImageDataManager::getInstance().GetImageDataBank(m_wServerType);
	if (pBank == NULL)
	{
		ErrorLn("ServerUser::OnMsgWriteImageData():not obtain the bank object of server, servertype="<<m_wServerType);
		return;
	}
	char* pdata = (char*)(pMsg+1);
	bool bResult = pBank->SetValue(pMsg->nNameSpace, pMsg->nKey, pdata, pMsg->dwDataLen);

#ifdef GUARD_PRINT_DEBUG_INF
	TraceLn("OnMsgWriteImageData():servertype["<<m_dwID<<"]="<<m_wServerType<<",namespace="<<pMsg->nNameSpace<<",key="<<pMsg->nKey<<",datalen="<<pMsg->dwDataLen<<(bResult?",Success":"Fail"));
#endif

	// 返回结果给目标
	SGameMsgHead header;
	header.SrcEndPoint	= MSG_ENDPOINT_GUARD;
	header.DestEndPoint	= (BYTE)m_wServerType;
	header.byKeyModule  = MSG_MODULEID_GUARD;
	header.byKeyAction  = MSG_GUARD_WRITE_DATA_RESULT;

	obuf obufData;
	obufData.push_back(&header, sizeof(SGameMsgHead));

	SMsgGuardWriteDataResult result;
	result.nNameSpace = pMsg->nNameSpace;	// 镜像数据命名类型 IMAGE_DATA_NAMESPACE
	result.nKey			= pMsg->nKey;		// 键值
	result.nBackPtr		= pMsg->nBackPtr;	// 
	result.bResult		= bResult;			// 返回结果
	obufData.push_back(&result, sizeof(result));

	SendData(obufData.data(), obufData.size());
}

/** 收到从数据监护服务器读取数据
@param actionId		: 
@param head	        : 
@param data	        : 数据
@param len	        : 数据长度
@return
*/
void ServerUser::OnMsgReadImageData(ulong actionId, SGameMsgHead* head, void* data, size_t len)
{
	if (len < sizeof(SMsgGuardReadData))
	{
		ErrorLn("ServerUser::OnMsgReadImageData():length of message is invalid, len="<< len <<",servertype="<<m_wServerType);
		return;
	}
	SMsgGuardReadData *pMsg = (SMsgGuardReadData*)data;

	CImageDataBank * pBank = CImageDataManager::getInstance().GetImageDataBank(m_wServerType);
	if (pBank == NULL)
	{
		ErrorLn("ServerUser::OnMsgReadImageData():not obtain the bank object of server, servertype="<<m_wServerType);
		return;
	}

	int nDataLen = 0;
	char* pBuff = (char*)pBank->GetValue(pMsg->nNameSpace, pMsg->nKey, nDataLen);
	if (pBuff==NULL)
	{
		nDataLen = 0;
	}

#ifdef GUARD_PRINT_DEBUG_INF
	TraceLn("OnMsgReadImageData(),servertype["<<m_dwID<<"]="<<m_wServerType<<",namespace="<<pMsg->nNameSpace<<",key="<<pMsg->nKey <<", len="<<nDataLen);
#endif

	// 返回结果给目标
	SGameMsgHead header;
	header.SrcEndPoint	= MSG_ENDPOINT_GUARD;
	header.DestEndPoint	= (BYTE)m_wServerType;
	header.byKeyModule  = MSG_MODULEID_GUARD;
	header.byKeyAction  = MSG_GUARD_READ_DATA_RESULT;

	obuf obufData;
	obufData.push_back(&header, sizeof(SGameMsgHead));

	SMsgGuardReadDataResult result;
	result.nNameSpace	= pMsg->nNameSpace;		// 镜像数据命名类型 IMAGE_DATA_NAMESPACE
	result.nKey			= pMsg->nKey;			// 键值
	result.nBackPtr		= pMsg->nBackPtr;		// 
	result.nDataLen		= nDataLen;				// 数据长度
	obufData.push_back(&result, sizeof(result));

	if (pBuff)
	{
		obufData.push_back(pBuff, nDataLen);
	}

	SendData(obufData.data(), obufData.size());
}