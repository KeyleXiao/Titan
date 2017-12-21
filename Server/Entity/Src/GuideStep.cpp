/*******************************************************************
** 文件名:	GuideStep.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	13/4/2017
** 版  本:	1.0
** 描  述:	引导步骤数据库回调接口

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "StdAfx.h"
#include "IServerGlobal.h"
#include "IEntity.h"
#include "IDBEngineService.h"
#include "CommonPart.h"
#include "IBankPart.h"
#include "GuideStep.h"

///////////////////////////////////////////////////////////////////
/** 
@param   
@param   
@return  
*/
CGuideStep::CGuideStep(void)
	: m_pCommonPart(nullptr)
{

}

/** 
@param   
@param   
@return  
*/
CGuideStep::~CGuideStep(void)
{

}

/** 
@param   
@param   
@return  
*/
bool CGuideStep::create(CommonPart * pCommonPart)
{
	if( pCommonPart == nullptr)
		return false;

	m_pCommonPart = pCommonPart;

	// 取得DBEngine服务
	IDBEngineService* pDBService = gServerGlobal->getDBEngineService();
	// 数据库接口注册
	if(pDBService)
	{
		pDBService->registerHandler(this, __FUNCTION__);	
	}	
	

	return true;
}

/** 
@param   
@param   
@return  
*/
void CGuideStep::release(void)
{
	// 取得DBEngine服务
	IDBEngineService* pDBService = gServerGlobal->getDBEngineService();
	// 数据库接口反注册
	if(pDBService)
	{	
		pDBService->unregisterHandler(this);
	}
	
	m_pCommonPart = nullptr;

	delete this;
}

/** 
@param   
@param   
@return  
*/
bool CGuideStep::save(int nStep)
{
	if(m_pCommonPart == nullptr)
	{
		return false;
	}
	__IEntity *  pMaster = m_pCommonPart->getHost();
	if (pMaster == nullptr)
	{
		return false;
	}
    __IBankPart * pBankPart = CAST_TYPE( __IBankPart*, pMaster->getEntityPart(PART_BANK) );
    if(pBankPart == nullptr)
    {
        return false;
    }
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
		return false;
	}

	DBREQ_OSS_ACTOR_GUIDESTEP_LOG dbParam;
    dbParam.dwUserID = pBankPart->getUserID();                  // 账号ID
    dbParam.dwActorID = pMaster->getIntProperty(PROPERTY_ID);   // 角色ID
    dbParam.nStep = nStep;									    // 步骤

	if (pDBEngine->executeSP(GAMEDB_REQUEST_ACTOR_GUIDESTEP_LOG, dbParam.dwUserID, (LPCSTR)&dbParam, sizeof(dbParam), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_ACTOR_GUIDESTEP_LOG failed");
        return false;
	}

    TraceLn(__FUNCTION__": name="<< pMaster->getName()<<", step="<< nStep);
	return true;
}

///////////////////////////////// IDBRetSink ////////////////////////////
/** 数据库请求返回回调方法
@param   pDBRetSink		：结果返回接口	
@param   nCmdID ：命令ＩＤ
@param   nDBRetCode：数据库请求返回值，参考上面定义
@param   pszDBRetDesc：数据库请求返回描述，由ＳＰ返回
@param   nQueueIndex：队列定义
@param   pOutData：输出数据
@param   nOutLen：输出数据长度
@return  
@note     
@warning 此对像千万不能在数据库返回前释放，否则会非法！
@retval buffer 
*/	
void CGuideStep::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{

}
