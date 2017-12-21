/*******************************************************************
** 文件名:	MiscManager.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-12-9
** 版  本:	1.0
** 描  述:	辅助全局对象的一个杂项管理器

********************************************************************/

#include "stdafx.h"

#include "ISocialGlobal.h"
#include "IFramework.h"
#include "IDBEngineService.h"
#include "IGameDatabaseDef.h"
#include "IServerController.h"
#include "MiscManager.h"

//////////////////////////////////////////////////////////////////////////
// 构造函数
CMiscManager::CMiscManager()
{

}

// 析构函数
CMiscManager::~CMiscManager()
{

}

////////////////// IMiscManager ////////////////////////////////////////////////////////
/** 加载
@return  
*/
bool CMiscManager::load()
{
    return true;
}

/** 执行Service的on_stop
@return  
*/
void CMiscManager::onStop(void)
{
    // 取消注册数据库服务
    IDBEngineService* pDBEngineService =  gSocialGlobal->getDBEngineService();
    if(pDBEngineService == NULL)
    {
        return;
    }
    pDBEngineService->unregisterHandler(CAST_TYPE(IDBRetSink*, this));
}

/** 释放
@note     释放所有资源，并且销毁此对像
@warning 
*/
void CMiscManager::release(void)
{
    delete this;
}


/* 用Service配置名称创建Service
@param pszServiceSchemeName: Service配置名称
*/
SERVICE_PTR CMiscManager::create_service(const char * pszServiceSchemeName)
{
    do 
    {
        if (!pszServiceSchemeName || strlen(pszServiceSchemeName) < 2 )
        {
            ErrorLn(__FUNCTION__": create service failed because input parameter is invalid!");
            break;
        }

        IFramework * pFrameowork = gSocialGlobal->getFramework();
        if(!pFrameowork)
        {
            ErrorLn(__FUNCTION__": don't get framework object!");
            break;
        }

        SERVICE_SCHEME* pServiceScheme = pFrameowork->get_service_scheme(pszServiceSchemeName);
        if(pServiceScheme == 0)
        {
            ErrorLn(__FUNCTION__": don't get scheme config info! scheme="<< pszServiceSchemeName);
            break;
        }

        TraceLn(__FUNCTION__": fire create service, schemename="<< pszServiceSchemeName);
        return pFrameowork->create_service(pServiceScheme);

    } while (false);
    
    return SERVICE_PTR(0);
}


/* 注册到DBEngineService，这个有点尴尬*/
bool  CMiscManager::registerDBEngineService()
{
    // 注册数据库服务
    IDBEngineService* pDBEngineService =  gSocialGlobal->getDBEngineService();
    if(pDBEngineService == NULL)
    {
        return false;
    }

    return pDBEngineService->registerHandler(CAST_TYPE(IDBRetSink*, this), __FUNCTION__);
}

/* 校验游戏世界ID有效性*/
bool  CMiscManager::checkWorldID(void)
{
    DWORD dwWorldID = getThisGameWorldID();
    // 请求游戏世界信息
    DBREQ_PARAM_READ_GAMEAREAINFO dbreqareainfo;
    dbreqareainfo.dwWorldID = dwWorldID;	
    IDBEngineService* pDBEngineService =  gSocialGlobal->getDBEngineService();
    if(pDBEngineService == NULL)
    {
        return false;
    }
    if(!pDBEngineService->executeSP(GAMEDB_REQUEST_READ_GAMEAREAINFO, 0, (LPCSTR)&dbreqareainfo, sizeof(dbreqareainfo), this, dwWorldID))
    {
        EmphasisLn( _GT("调用GAMEDB_REQUEST_READ_GAMEAREAINFO数据库请求失败.") );
        return false;
    }
    return true;
}

////////////////// IDBRetSink ////////////////////////////////////////////////////////
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
void CMiscManager::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    if(nDBRetCode < DBRET_CODE_SUCCEED)
    {
        if(nCmdID == GAMEDB_REQUEST_READ_GAMEAREAINFO)
        {
            if(pszDBRetDesc != NULL)
            {
                ErrorLn("Execute cmdid="<<nCmdID<<" failure! RetCode="<<nDBRetCode<<"; Desc="<<pszDBRetDesc);
            }
            else
            {
                ErrorLn("Execute cmdid="<<nCmdID<<"failure! RetCode="<<nDBRetCode);
            }
        }
    }
    else
    {
        if(nCmdID == GAMEDB_REQUEST_READ_GAMEAREAINFO)
        {
            if(nOutLen != sizeof(DBREQ_RESULT_READ_GAMEAREAINFO))
            {
                ErrorLn( _GT("执行DBREQ_RESULT_READ_GAMEAREAINFO请求返回的长度与头文件里定义的不符；描述=")<<nDBRetCode);
            }
            else
            {
                // 校验游戏世界ID通过后，开始校验账号有效性
                gSocialGlobal->getServerControllor()->SetUIControl(UICONTROL_CHECK_ACCOUNT);
            }
        }
    }
}