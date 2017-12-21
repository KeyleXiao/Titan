/*******************************************************************
** 文件名:	OSSLogService_Stub.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-7-19
** 版  本:	1.0
** 描  述:	日记服务


********************************************************************/

#include "stdafx.h"
#include "IServerGlobal.h"
#include "OSSLogService_Stub.h"


COSSLogService_Stub::COSSLogService_Stub(COSSLogService * pReal)
	: m_real_service(pReal)
{

}

COSSLogService_Stub::~COSSLogService_Stub()
{

}


////////////////// IServiceStub ////////////////////////////////////////////////////////

// 启动
// @param data : 在目前这版本中,data里的内容是指向配置信息的TiXmlElement *指针
bool COSSLogService_Stub::on_start( SERVICE_PTR pContainer,void * data,int len )
{
	m_proxy.m_pContainer = pContainer;
	m_real_service->create();
	return true;
}

// 停止
bool COSSLogService_Stub::on_stop()
{
	return true;
}

// 处理消息
bool COSSLogService_Stub::handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{
	if ( pMsg->context_len==0 || pMsg->context==0 )
	return false;

	ibuffer in(pMsg->context,pMsg->context_len);

	////////////////////////////////////////////////////////////////////////////

	// 以下自动生成的消息解包，你需要检查
	if ( TEST_SAME_FUNCTION(IOSSLogService::addGameNoteLog) )
	{
        int nGameWorldID; int nNoteTrackType; DWORD dwIndex;size_t nLen;
        in >> nGameWorldID >> nNoteTrackType >> dwIndex >> nLen;
		m_real_service->addGameNoteLog(nGameWorldID, nNoteTrackType, dwIndex, in.current() );
		return true;
	}

    if ( TEST_SAME_FUNCTION(IOSSLogService::addPlazaTicketLog) )
    {
        int nGameWorldID;  size_t nLogSize;
        in >> nGameWorldID >> nLogSize;
        m_real_service->addPlazaTicketLog(nGameWorldID, in.current(), nLogSize);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IOSSLogService::addTicketLog) )
    {
        int nGameWorldID; DWORD dwMaster; int nTarget; int nNum; WORD wApproach; int nTime;  DWORD dwReserve; string strRemark;
        in >> nGameWorldID >> dwMaster >> nTarget >> nNum >> wApproach >> nTime >> dwReserve >> strRemark;
        m_real_service->addTicketLog(nGameWorldID, dwMaster, nTarget, nNum, wApproach, nTime, strRemark.c_str(), dwReserve);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IOSSLogService::transTickLog) )
    {
        int nGameWorldID; DWORD dwMaster; int nTarget; int nNum; WORD wApproach; int nTime;  DWORD dwReserve; string strRemark;
        in >> nGameWorldID >> dwMaster >> nTarget >> nNum >> wApproach >> nTime >> dwReserve >> strRemark;
        m_real_service->transTickLog(nGameWorldID, dwMaster, nTarget, nNum, wApproach, nTime, strRemark.c_str(), dwReserve);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IOSSLogService::addMoneyLog) )
    {
        int nGameWorldID; DWORD dwMaster; int nTarget; int nNum; WORD wApproach; int nTime;  DWORD dwReserve; string strRemark;
        in >> nGameWorldID >> dwMaster >> nTarget >> nNum >> wApproach >> nTime >> dwReserve >> strRemark;
        m_real_service->addMoneyLog(nGameWorldID, dwMaster, nTarget, nNum, wApproach, nTime, strRemark.c_str(), dwReserve);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IOSSLogService::transMoneyLog) )
    {
        int nGameWorldID; DWORD dwMaster; int nTarget; int nNum; WORD wApproach; int nTime;  DWORD dwReserve; string strRemark;
        in >> nGameWorldID >> dwMaster >> nTarget >> nNum >> wApproach >> nTime >> dwReserve >> strRemark;
        m_real_service->transMoneyLog(nGameWorldID, dwMaster, nTarget, nNum, wApproach, nTime, strRemark.c_str(), dwReserve);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IOSSLogService::addBindMoneyLog) )
    {
        int nGameWorldID; DWORD dwMaster; int nTarget; int nNum; WORD wApproach; int nTime;  DWORD dwReserve; string strRemark;
        in >> nGameWorldID >> dwMaster >> nTarget >> nNum >> wApproach >> nTime >> dwReserve >> strRemark;
        m_real_service->addBindMoneyLog(nGameWorldID, dwMaster, nTarget, nNum, wApproach, nTime, strRemark.c_str(), dwReserve);
        return true;
    }


    if ( TEST_SAME_FUNCTION(IOSSLogService::transBindMoneyLog) )
    {
        int nGameWorldID; DWORD dwMaster; int nTarget; int nNum; WORD wApproach; int nTime;  DWORD dwReserve; string strRemark;
        in >> nGameWorldID >> dwMaster >> nTarget >> nNum >> wApproach >> nTime >> dwReserve >> strRemark;
        m_real_service->transBindMoneyLog(nGameWorldID, dwMaster, nTarget, nNum, wApproach, nTime, strRemark.c_str(), dwReserve);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IOSSLogService::addExpLog) )
    {
        int nGameWorldID; DWORD dwMaster; int nTarget; int nNum; WORD wApproach; int nTime;  DWORD dwReserve; string strRemark;
        in >> nGameWorldID >> dwMaster >> nTarget >> nNum >> wApproach >> nTime >> dwReserve >> strRemark;
        m_real_service->addExpLog(nGameWorldID, dwMaster, nTarget, nNum, wApproach, nTime, strRemark.c_str(), dwReserve);
        return true;
    }

    if ( TEST_SAME_FUNCTION(IOSSLogService::addNumericLog) )
    {
        int nGameWorldID; BYTE byObjType; int nObjectID; int nTarget; int nNum; BYTE byNumType; BYTE byOpType; WORD wApproach; int nTime; DWORD dwReserve;string strRemark;

        in >> nGameWorldID >> byObjType >> nObjectID >> nTarget >> nNum >> byNumType >> byOpType >> wApproach >> nTime >> dwReserve >> strRemark;
        m_real_service->addNumericLog(nGameWorldID, byObjType, nObjectID, nTarget, nNum, byNumType, byOpType, wApproach, nTime, strRemark.c_str(), dwReserve);
        return true;
    }


    if ( TEST_SAME_FUNCTION(IOSSLogService::addActorGoodsLog) )
    {
        int nGameWorldID; BYTE byFromType; long lFromObjID; BYTE byToType; long lToObjID; BYTE nGoodsType; BYTE byOpType; WORD wApproach; int nTypeID;
        int nNum; int nTime;  DWORD dwReserveID1; DWORD dwReserveID2; string strRemark;
        in >> nGameWorldID >> byFromType >> lFromObjID >> byToType >> lToObjID >> nGoodsType >> byOpType >> wApproach >> nTypeID >> nNum >> nTime >> dwReserveID1 >> dwReserveID2 >> strRemark;
        m_real_service->addActorGoodsLog(nGameWorldID, byFromType, lFromObjID, byToType, lToObjID, nGoodsType, byOpType, wApproach, nTypeID, nNum, nTime, strRemark.c_str(), dwReserveID1, dwReserveID2);
        return true;
    }


    if ( TEST_SAME_FUNCTION(IOSSLogService::addActorUpgradeLog) )
    {
		int nGameWorldID; DWORD dwUserID; DWORD dwActorID; BYTE nUpgradeType; DWORD dwValue; int nTime; DWORD dwReserve;
		GET_MSG_PARAM_7(int, nGameWorldID, DWORD, dwUserID, DWORD, dwActorID, BYTE, nUpgradeType, DWORD, dwValue, int, nTime, DWORD, dwReserve);
		m_real_service->addActorUpgradeLog( nGameWorldID, dwUserID, dwActorID, nUpgradeType, dwValue, nTime, dwReserve);
		return true;
	}

	if ( TEST_SAME_FUNCTION(IOSSLogService::addActorForgeLog) )
	{
		int nGameWorldID; DWORD dwUserID; DWORD dwActorID; BYTE nOperateType; DWORD dwGoodID;int nTime;DWORD dwReserve;
		LPCSTR szGoodName = NULL;LPCSTR szForgeDesc = NULL;
		in >> nGameWorldID >> dwUserID >>dwActorID >> nOperateType >> dwGoodID  >> nTime  >> dwReserve>>szGoodName>>szForgeDesc ;

		m_real_service->addActorForgeLog( nGameWorldID, dwUserID, dwActorID, nOperateType, dwGoodID, szGoodName, nTime,szForgeDesc,dwReserve);
		return true;
	}

	if (TEST_SAME_FUNCTION(IOSSLogService::addTaskInfo))
	{
		int nTaskID; int nAcceptNum; int nFinishNum; int nGiveUpNum;
		GET_MSG_PARAM_4(int, nTaskID, int, nAcceptNum, int, nFinishNum, int, nGiveUpNum);
		m_real_service->addTaskInfo(nTaskID, nAcceptNum, nFinishNum, nGiveUpNum);
		return true;
	}

	return false;
}

// 返回代理对象即可
void * COSSLogService_Stub::query_interface( int iid )
{
	return &m_proxy;
}

// 销毁
void COSSLogService_Stub::release()
{
	if ( m_real_service )
	{
		m_real_service->release();
		m_real_service = 0;
	}

	delete this;
}


//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateOSSLogService()
{
	return new COSSLogService_Stub(new COSSLogService);
}