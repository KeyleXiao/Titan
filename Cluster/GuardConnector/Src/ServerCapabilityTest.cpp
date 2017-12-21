/*******************************************************************
** 文件名:	d:\Rocket20100701\Cluster\LoginServer\Src\ServerCapabilityTest.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	彭智锋 （9671690@qq.com）
** 日  期:	2010/9/10 14:40
** 版  本:	1.0
** 描  述:	
** 应  用:  服务器性能检测	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"

#include "GuardConnector.h"
#include "GuardDef.h"

#include "Common.h"
#include "net.h"
using namespace rkt;
#include <strstream>
#include "time.h"

#include "ServerCapabilityTest.h"

/** 构造
@param   
@param   
@return  
*/
CServerCapabilityTest::CServerCapabilityTest()
{
	// 服务器检测项
	m_serverItem.clear();
	
	// 序号
	m_nSerial = 0;

	// 是否检测已启动
	m_bStartTest = false;

	// 连接器对象
	m_pConnector = NULL;

	// 检测完成的结点
	m_completeNode.clear();

	// 最新监控结点数据
	m_monitorNode.clear();
}

/** 析构
@param   
@param   
@return  
*/
CServerCapabilityTest::~CServerCapabilityTest()
{
	
}

/** 性能测试对象创建
@param   
@param   
@return  
*/
bool CServerCapabilityTest::Create(GuardConnector* pConnector)
{
	m_pConnector = pConnector;
	return true;
}

/** 性能测试对象关闭
@param   
@param   
@return  
*/
void CServerCapabilityTest::Close()
{
	StopCapabilityTest();
}

/** 是否已启动
@param   
@param   
@return  
*/
bool CServerCapabilityTest::IsStartup()
{
	return m_bStartTest;
}

/** 启动测试
@param   
@param   
@return  
*/
void CServerCapabilityTest::StartCapabilityTest(int nCheckInterval)
{
	if (m_bStartTest)
	{
		return;
	}

	if (m_pConnector==NULL)
	{
		return;
	}

	TimerAxis* pTimerAxis = m_pConnector->GetTimerAxis();
	if (pTimerAxis == NULL)
	{
		return;
	}

	// 服务器检测项
	m_serverItem.clear();

	// 检测完成的结点
	m_completeNode.clear();

	// 最新监控结点数据
	m_monitorNode.clear();

	// 是否检测已启动
	m_bStartTest = true;

	if (isPublicGameWorld())
	{
		return;
	}

	if(nCheckInterval<=0 || nCheckInterval>3600)
	{
		nCheckInterval = 5;	
	}

	EmphasisLn("=============== 启动跨区服务器性能测试 间隔时长为" << nCheckInterval << "秒 ===============");
	pTimerAxis->SetTimer(DTIMER_ID_TESTSTART, 1, this);

	pTimerAxis->SetTimer(DTIMER_ID_SERVERCHECK, nCheckInterval*1000, this);

	pTimerAxis->SetTimer(DTIMER_ID_SAVERESULT, DSAVERESULT_INTERVAL, this);
}

/** 关闭测试
@param   
@param   
@return  
*/
void CServerCapabilityTest::StopCapabilityTest()
{
	if (!m_bStartTest)
	{
		return;
	}

	if (m_pConnector == NULL)
	{
		return;
	}

	TimerAxis* pTimerAxis = m_pConnector->GetTimerAxis();
	if (pTimerAxis == NULL)
	{
		return;
	}

	pTimerAxis->KillTimer(DTIMER_ID_SERVERCHECK, this);

	pTimerAxis->KillTimer(DTIMER_ID_SAVERESULT, this);

	pTimerAxis->KillTimer(DTIMER_ID_TESTSTART, this);

	Save(true);

	EmphasisLn("=============== 服务器性能测试结束 ===============");

	// 是否检测已启动
	m_bStartTest = false;
}

/** 保存数据
@param   
@param   
@return  
*/
void CServerCapabilityTest::Save(bool bForce)
{
	if (m_serverItem.size() <= 0)
	{
		return;
	}

	int nLastTick = getTickCountEx();

	for (TSEVERITEM::iterator Iter=m_serverItem.begin(); Iter!=m_serverItem.end();)
	{
		// 间隔毫秒
		if (Iter->second.nEndTick<0)
		{
			if (!bForce && nLastTick - Iter->second.nBeginTick>(120*1000))
			{
				Iter++;
				continue;
			}
		}
		
		AddCompleteNode(Iter->second);	

		m_serverItem.erase(Iter++);
	}
}


/** 定时器
@param   
@param   
@return  
*/
void CServerCapabilityTest::OnTimer( unsigned long dwTimerID )
{
	switch(dwTimerID)
	{
	case DTIMER_ID_SERVERCHECK:
		{
			SendCapabilityTestRequest();
		}
		break;
	case DTIMER_ID_SAVERESULT:
		{
			Save(false);
		}
		break;
	case DTIMER_ID_TESTSTART:
		{
			TimerAxis* pTimerAxis = m_pConnector->GetTimerAxis();
			if (pTimerAxis == NULL)
			{
				return;
			}

			pTimerAxis->KillTimer(DTIMER_ID_TESTSTART, this);

			SendCapabilityTestRequest();
		}
		break;
	default:
		ErrorLn("CServerCapabilityTest::OnTimer 不存在的定时器ID " << dwTimerID);
		break;
	}
}

/** 收到中心服连接器的检测请求
@param   
@param   
@return  
*/
void CServerCapabilityTest::HandleServerCheckReq(LPVOID pData,DWORD dwDataLen)
{
	//if (m_pConnector==NULL)
	//{
	//	return;
	//}

	//IBridgePart* pLocalBridgePart = m_pConnector->GetLocalBridgePart();
	//if (pLocalBridgePart == NULL)
	//{
	//	return;
	//}

	//if (dwDataLen != sizeof(SGameMsgHead) + sizeof(SMsgBridgeServerCheck))
	//{
	//	ErrorLn("CenterServerConnector::HandleServerCheckReq 数据长度不匹配 dwDataLen=" << dwDataLen);
	//	return;
	//}

	//SGameMsgHead* pMsgHead = (SGameMsgHead*)pData;

	//SMsgBridgeServerCheck* pServerCheck = (SMsgBridgeServerCheck*)(pMsgHead + 1);


	//// 将数据发回到原始来源处
	//{
	//	pMsgHead->SrcEndPoint = pLocalBridgePart->GetServerType();
	//	pMsgHead->DestEndPoint= MSG_ENDPOINT_BRIDGE;
	//	pMsgHead->byKeyModule  = MSG_MODULEID_BRIDGE;
	//	pMsgHead->byKeyAction  = MSG_BRIDGE_SERVERCHECKRES;

	//	m_pConnector->SendData((LPCSTR)pData, dwDataLen);
	//}
}

/** 收到其它连接器或是中心服的回应
@param   
@param   
@return  
*/
void CServerCapabilityTest::HandleServerCheckRes(LPVOID pData,DWORD dwDataLen)
{
	//if (dwDataLen != sizeof(SGameMsgHead) + sizeof(SMsgBridgeServerCheck))
	//{
	//	ErrorLn("CenterServerConnector::HandleServerCheckReq 数据长度不匹配 dwDataLen=" << dwDataLen);
	//	return;
	//}

	//SGameMsgHead* pMsgHead = (SGameMsgHead*)pData;

	//SMsgBridgeServerCheck* pServerCheck = (SMsgBridgeServerCheck*)(pMsgHead + 1);

	//AddCompleteNode(*pServerCheck);

	//m_serverItem.erase(pServerCheck->nSerial);
}

/** 
@param   
@param   
@return  
*/
void CServerCapabilityTest::AddCompleteNode(SMsgGuardServerCheck& data)
{
	//TSEVERITEM::iterator Iter = m_serverItem.find(data.nSerial);
	//if (Iter == m_serverItem.end())
	//{
	//	return;
	//}

	//SServerCapability node;
	//node.nGameWorldID = Iter->second.nGameWorldID;	// 游戏世界ID
	//node.wServerType = Iter->second.wServerType;	// 服务器类型
	//node.wServerNo = Iter->second.wServerNo;		// 服务器编号（同类型时）
	//node.nCheckTime = Iter->second.nCheckTime;		// 开始检测采集的时间点
	//node.nInterval = getTickCountEx() - Iter->second.nBeginTick;// 间隔时长(毫秒)
	//m_completeNode.push_back(node);

	//if (m_completeNode.size() > DHODENODE_MAX)
	//{
	//	m_completeNode.pop_front();
	//}

	//// 刷新即时监控结点数据
	//LONGLONG nGameWorldID = node.nGameWorldID;	nGameWorldID = nGameWorldID << 32;
	//LONGLONG nServerType = node.wServerType;	nServerType = nServerType << 16;
	//LONGLONG nKey = nGameWorldID + nServerType + node.wServerNo;
	//SServerCapability& serverNode = m_monitorNode[nKey];
	//if (serverNode.nCheckTime < node.nCheckTime)
	//{
	//	m_monitorNode[nKey] = node;
	//}
}

/** 发起测试请求到各个服务器
@param   
@param   
@return  
*/
void CServerCapabilityTest::SendCapabilityTestRequest()
{
	//if (m_pConnector==NULL)
	//{
	//	return;
	//}

	//IBridgePart* pLocalBridgePart = m_pConnector->GetLocalBridgePart();
	//if (pLocalBridgePart == NULL)
	//{
	//	return;
	//}

	//int nCurTime = time(NULL);

	//obuf1024 buf;
	//SGameMsgHead* pGameMsgHead = NULL;
	//SMsgBridgeServerCheck* pMsgServerCheck = NULL;
	//{
	//	SGameMsgHead head;
	//	SMsgBridgeServerCheck serverCheckData;
	//	buf.push_back(&head, sizeof(head));
	//	buf.push_back(&serverCheckData, sizeof(serverCheckData));
	//	pGameMsgHead = (SGameMsgHead*)(buf.data());
	//	pMsgServerCheck = (SMsgBridgeServerCheck*)(pGameMsgHead + 1);
	//}

	//if (pGameMsgHead==NULL || pMsgServerCheck==NULL)
	//{
	//	return;
	//}

	//// 取得场景服务器列表
	//DWORD ServerArray[2048]; int nArraySize = 2048; DWORD nGetCount = 0;
	//nGetCount = m_pConnector->GetGameServerIDList(ServerArray, nArraySize, nGetCount, getPublicGameWorldID(), MSG_ENDPOINT_UNKNOW);;
	//for (int i=0; i<nGetCount; i++)
	//{
	//	IBridgePart* pBridgePart = m_pConnector->GetGameServerPart(ServerArray[i]);
	//	if (pBridgePart == NULL)
	//	{
	//		continue;
	//	}

	//	{// 数据 结构体初始化
	//		pMsgServerCheck->nCheckTime = nCurTime;					// 开始检测采集的时间点
	//		pMsgServerCheck->nSerial = m_nSerial++;					// 序号
	//		pMsgServerCheck->nGameWorldID = getPublicGameWorldID();	// 游戏世界ID
	//		pMsgServerCheck->nBeginTick = getTickCountEx();			// 检测开始的TICK数
	//		pMsgServerCheck->nEndTick = -1;							// 检测结数的TICK数
	//		pMsgServerCheck->wServerType = pBridgePart->GetServerType();	// 服务器类型
	//		pMsgServerCheck->wServerNo = pBridgePart->GetServerSubID();		// 服务器编号（同类型时）
	//		pMsgServerCheck->nFromServerID = m_pConnector->GetServerID();
	//		pMsgServerCheck->nToServerID = pBridgePart->GetServerID();
	//	}

	//	{// 头部修改
	//		pGameMsgHead->SrcEndPoint = pLocalBridgePart->GetServerType();
	//		pGameMsgHead->DestEndPoint = pBridgePart->GetServerType();
	//		pGameMsgHead->byKeyModule = MSG_MODULEID_BRIDGE;
	//		pGameMsgHead->byKeyAction = MSG_BRIDGE_SERVERCHECKREQ;
	//	}

	//	m_serverItem[pMsgServerCheck->nSerial] = *pMsgServerCheck;
	//	m_pConnector->SendData(buf.data(), buf.size());
	//}

	//// 由于取不到中心服务器，所以做此特殊处理
	//{
	//	{// 数据 结构体初始化
	//		pMsgServerCheck->nCheckTime = nCurTime;					// 开始检测采集的时间点
	//		pMsgServerCheck->nSerial = m_nSerial++;					// 序号
	//		pMsgServerCheck->nGameWorldID = getThisGameWorldID();	// 游戏世界ID
	//		pMsgServerCheck->nBeginTick = getTickCountEx();			// 检测开始的TICK数
	//		pMsgServerCheck->nEndTick = -1;							// 检测结数的TICK数
	//		pMsgServerCheck->wServerType = MSG_ENDPOINT_BRIDGE;		// 服务器类型
	//		pMsgServerCheck->wServerNo = 1;							// 服务器编号（同类型时）
	//		pMsgServerCheck->nFromServerID = m_pConnector->GetServerID();
	//		pMsgServerCheck->nToServerID = -1;
	//	}

	//	{// 头部修改
	//		pGameMsgHead->SrcEndPoint = pLocalBridgePart->GetServerType();
	//		pGameMsgHead->DestEndPoint = MSG_ENDPOINT_BRIDGE;
	//		pGameMsgHead->byKeyModule = MSG_MODULEID_BRIDGE;
	//		pGameMsgHead->byKeyAction = MSG_BRIDGE_SERVERCHECKREQ;
	//	}

	//	m_serverItem[pMsgServerCheck->nSerial] = *pMsgServerCheck;
	//	m_pConnector->SendData(buf.data(), buf.size());
	//}
}

/** 取完成性能测试的结点
@param   
@param   
@return  
*/
TCAPABILITYNODE& CServerCapabilityTest::GetCapabilityNode()
{
	return m_completeNode;
}

/** 取最新监控结点数据
@param   
@param   
@return  
*/
TMONITORNODE& CServerCapabilityTest::GetMonitorNode()
{
	return m_monitorNode;
}