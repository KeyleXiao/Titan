/*******************************************************************
** 文件名:	SceneServerControl.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-6-23
** 版  本:	1.0
** 描  述:	
            
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "StdAfx.h"
#include "Tlhelp32.h"
#include "psapi.h"
#include "ITimerService.h"
#include "IDBEngineService.h"
#include "SocialGlobalHelper.h"
#include "SocialServerDlg.h"
#include "DManagerModule.h"
#include "ILuaManager.h"
#include "ILuaService.h"

#include "ServerControl.h"


//////////////////////////////////////////////////////////////////////////
// 内存变化自动调节缓存配置结构
struct SMemoryBalanceCache
{
	DWORD   dwMinMemory;			// 最低内存
	DWORD   dwMaxMemory;			// 最高内存
	DWORD   dwBuffCacheMaxSize;		// BUFF缓存数量
	DWORD   dwEffectCacheMaxSize;	// EFFECT缓存数量
};


// 内存监视器间隔时长
#define TIMER_MEMORYMONITOR_INTERVAL		(10 * 60 * 1000)


CServerControl   g_ServerControl;

//////////////////////////////////////////////////////////////////////////
/**
@param
@param
@return
*/
CServerControl::CServerControl(void)
{
    // 服务器运行状态 SERVER_RUN_STATE
    m_dwServerRunState = SERVER_RUN_STATE_IDLE;

	// 当前UI操作状态 SERVER_UI_CONTROL
	m_dwUIControlState = UICONTROL_IDLESSE;

	// 上次提醒内存使用警告的时间
	m_dwLastMemoryGuardLineTick = 0;
}

/**
@param
@param
@return
*/
CServerControl::~CServerControl(void)
{
    // 当前UI操作状态 SERVER_UI_CONTROL
    m_dwUIControlState = UICONTROL_IDLESSE;
}

/** 
@param   
@param   
@return  
*/
void CServerControl::Create(void)
{
	ITimerService * pTimerAxis = gSocialGlobal->getTimerService();
	if(pTimerAxis == NULL)
	{
		return;
	}
	pTimerAxis->SetTimer(ETimerEventID_MemoryMonitor, TIMER_MEMORYMONITOR_INTERVAL, this, INFINITY_CALL, __FUNCTION__);
}

/** 
@param   
@param   
@return  
*/
void  CServerControl::Close(void)
{
	ITimerService * pTimerAxis = gSocialGlobal->getTimerService();
	if(pTimerAxis != NULL)
	{
		pTimerAxis->KillTimer(ETimerEventID_MemoryMonitor, this);
	}
}

/**
@param
@param
@return
*/
void CServerControl::SetServerRunState(DWORD dwState)
{
	m_dwServerRunState = dwState;
}

/**
@param
@param
@return
*/
DWORD CServerControl::GetServerRunState(void)
{
	return m_dwServerRunState;
}

/** 逻辑线程信号到时，会调此接口
@param
@param
@return  返回true:表示操作已处理完，返回false:表示操作未处理完
*/
typedef void * (CServerControl::* UICONTROL)(void);
bool CServerControl::OnDo(DWORD dwState)
{
    static UICONTROL uicontrol[UICONTROL_MAX] =
    {
        (UICONTROL)0,										// 空闲状态
        (UICONTROL)&CServerControl::OnInitalize,			// 初始化完成 
        (UICONTROL)&CServerControl::OnCheckAccount,			// 校验账号有效性

        (UICONTROL)&CServerControl::OnStartService,			// 启动服务
        (UICONTROL)&CServerControl::OnStopService,			// 停止服务
        (UICONTROL)&CServerControl::OnUpdateScheme,		    // 更新配置
        (UICONTROL)&CServerControl::OnUpdateLua,		    // 更新LUA
        (UICONTROL)&CServerControl::OnQueueState,			// 数据库队列信息
        (UICONTROL)&CServerControl::OnMemoryInfo,			// 内存信息
        (UICONTROL)&CServerControl::OnSetGameWorldID,		// 触发器信息
        (UICONTROL)&CServerControl::OnForceUnlock,		    // 强制解锁
        // 最大态
    };

    if(dwState <= UICONTROL_IDLESSE || dwState >= UICONTROL_MAX)
    {
        return true;
    }

    (this->*uicontrol[dwState])();
	m_dwUIControlState = UICONTROL_IDLESSE;

    return true;
}


/** 设置当前控制态
@param
@param
@return
*/
void CServerControl::SetUIControl(DWORD dwState)
{
    // 当前UI操作状态
    m_dwUIControlState = dwState;

    // 要跨Service驱动才能正常获取到场景数据
    ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
    if (ptr != NULL)
    {
        ptr->requestHandleServerControl(dwState);
    }
}


void CServerControl::GotoState(int state)
{
    LeaveState(m_dwServerRunState);

    m_dwServerRunState = state;

    EnterState(m_dwServerRunState);
}


void CServerControl::EnterState(int nRunState)
{
    switch (nRunState)
    {
    case SERVER_RUN_STATE_IDLE:     // 未启动
        break;

    case SERVER_RUN_STATE_INIT_SUCCESS:		// 初始化成功:
        {
            // 控制下自动启动
            Ini ini("config.ini");
            int nAutoStart = ini.getInt("app","autostart",0);
            if(nAutoStart)
            {
                g_pServerDlg->OnBnClickedStartservice();
            }
        }
        break;

    case SERVER_RUN_STATE_INIT_FAIL:        // 初始化失败
        {
            ErrorLn(_GT("初始化社会服环境失败!!"));
        }
        break;

    case SERVER_RUN_STATE_GET_AUTH_RESOURCE:    // 获取校验资源
        {
            SocialGlobalHelper helper;
            if(helper.m_ptr)
            {
                helper.m_ptr->requestManagerStart(MANAGER_STARTFLAG_DBSERVER | MANAGER_STARTFLAG_DLL, 0);
            }
        }
        break;

    case SERVER_RUN_STATE_GET_SCRIPT_RESOURCE:   // 校验游戏ID
        {
            SocialGlobalHelper helper;
            if(helper.m_ptr)
            {
                helper.m_ptr->requestManagerStart(MANAGER_STARTFLAG_GETSCRIPT, 0);
            }
        }
        break;

    case SERVER_RUN_STATE_CREATE_SERVICE:   // 创建服务
        {
            // 设定状态
            ReportServerState(MG_SERVERSTATE_STARTING);

            //if (gGlobalSocialServer->IsLoadFromPack() && pManagerConnector != 0)
            //{
            //    EmphasisLn("============== 开始请求管理器获取启动脚本 ==============");
            //    pManagerConnector->RequestStart(MANAGER_STARTFLAG_GETSCRIPT);
            //}
            //else
            //{
            //    EmphasisLn("============== 开始创建服务 ==============");
            //    if(!((GlobalSocialServer*)::rkt::getGlobal())->create())
            //    {
            //        gotoState(ERunState_StartFailed);
            //    }
            //    else
            //    {
            //        gotoState(ERunState_InitDataFromDB);
            //    }
            //}
        }
        break;

    case SERVER_RUN_STATE_INIT_FROMDB:      // 从数据库初始化数据
        {
            EmphasisLn(_GT("============== 开始从数据库初始化模块数据 ==============") );
            //gGlobalSocialServer->GetEventEngine()->FireExecute(EVENT_START_SERVICE_LOAD_DATA, SOURCE_TYPE_SYSTEM, 0, 0, 0);
            //gGlobalSocialServer->getTimerAxis()->SetTimer(ETimerEventID_VanStop, TIMER_INTERVAL, static_cast<ITimerHandler *>(this), INFINITY_CALL, "CPanelControl::enterState1");
        }
        break;

    case SERVER_RUN_STATE_CONNECT_CENTERSERVER:     // 连接中心服
        {
            EmphasisLn(_GT("============== 开始连接中心服务器 =============="));
            //if (!((GlobalSocialServer*)::rkt::getGlobal())->connectCenterServer())
            //    gotoState(ERunState_StartFailed);

            //// gGlobalSocialServer->GetEventEngine()->FireExecute(EVENT_START_SERVICE_CONNECT_CENTERSERVER, SOURCE_TYPE_SYSTEM, 0, 0, 0); // 改到连接后
            //gGlobalSocialServer->getTimerAxis()->SetTimer(ETimerEventID_VanStop, TIMER_INTERVAL, static_cast<ITimerHandler *>(this), INFINITY_CALL, "CPanelControl::enterState2");
        }
        break;

    case SERVER_RUN_STATE_START_SUCCESS:        // 启动成功
        {
            EmphasisLn(_GT("启动服务成功"));
            setTraceHasTime(true);

            // 获取场景服的用户
            //gGlobalSocialServer->GetEventEngine()->FireExecute(EVENT_START_SERVICE_USER_LOGIN, SOURCE_TYPE_SYSTEM, 0, 0, 0);

            // 已启动，正在远行
            ReportServerState(MG_SERVERSTATE_RUNING);
        }
        break;

    case SERVER_RUN_STATE_START_FAIL:       // 启动失败
        {
            EmphasisLn(_GT("启动服务失败"));
            // 启动失败
            ReportServerState(MG_SERVERSTATE_STARTFAIL);
        }
        break;

    case SERVER_RUN_STATE_STOPPING:     // 正在关闭
        {
            // 正在关闭
            ReportServerState(MG_SERVERSTATE_STOPING);

            EmphasisLn(_GT("============== 开始关闭社会服 =============="));

            EmphasisLn(_GT("============== 断开中心服连接 =============="));
            // 断开中心服连接
            //((GlobalSocialServer*)::rkt::getGlobal())->unconnectCenterServer();

            // 发出事件通知其他模块保存数据
            EmphasisLn(_GT("============== 通知数据存储 =============="));
            //gGlobalSocialServer->GetEventEngine()->FireExecute(EVENT_STOP_SERVICE_SAVE_DATA, SOURCE_TYPE_SYSTEM, 0, 0, 0);
            //gGlobalSocialServer->getTimerAxis()->SetTimer(ETimerEventID_VanStop, TIMER_INTERVAL, static_cast<ITimerHandler *>(this), INFINITY_CALL, "CPanelControl::enterState3");

            //// 停止一些可能要写数据库的服务
            //((GlobalSocialServer*)::rkt::getGlobal())->stop();
        }
        break;

    case SERVER_RUN_STATE_STOPPED:      // 关闭延时	
        {
            EmphasisLn(_GT("============== 数据处理完成，断开跟前端的连接，并延时退出 =============="));
        }
        break;

    default:break;
    }
}

void CServerControl::LeaveState(int nRunState)
{
    switch (nRunState)
    {
    case SERVER_RUN_STATE_IDLE:             // 未启动
        break;
    case SERVER_RUN_STATE_INIT_SUCCESS:     // 初始化成功
        break;
    case SERVER_RUN_STATE_INIT_FAIL:        // 初始化失败
        break;
    case SERVER_RUN_STATE_CREATE_SERVICE:   // 创建服务
        break;
    case SERVER_RUN_STATE_INIT_FROMDB:      // 从数据库初始化数据
        break;
    case SERVER_RUN_STATE_CONNECT_CENTERSERVER: // 连接中心服
        break;
    case SERVER_RUN_STATE_START_SUCCESS:    // 启动成功
        break;
    case SERVER_RUN_STATE_START_FAIL:       // 启动失败
        break;
    case SERVER_RUN_STATE_STOPPING:         // 正在关闭
        break;
    case SERVER_RUN_STATE_STOPPED:          // 关闭延时	
        break;

    default:break;
    }
}

// 获取管理器连接服务
IManagerConnectorService*  CServerControl::GetManagerConnectorService()
{
    IManagerConnectorService * pManagerConnectorService = gSocialGlobal->getManagerConnectorService();
    return pManagerConnectorService ? pManagerConnectorService : NULL;
}

// 上报服务器状态到管理器
void  CServerControl::ReportServerState(int nState)
{
   IManagerConnectorService * pManagerConnectorService =  GetManagerConnectorService();
   if(pManagerConnectorService)
   {
        pManagerConnectorService->setServerState(nState);
   }
}
//////////////////////////////////////////////////////////////////////////
/** 
@param   
@param   
@return  
*/
void CServerControl::OnInitalize(void)
{
    EmphasisLn(_GT("初始化社会服环境成功"));
    // 正在启动
    ReportServerState(MG_SERVERSTATE_STARTING);

    int nRunState = GetServerRunState();
    if(nRunState == SERVER_RUN_STATE_CREATE_SERVICE)
    {
        return;
    }

    SetUIControl(UICONTROL_START_SERVICE);
}

/** 
@param   
@param   
@return  
*/
void CServerControl::OnCheckAccount(void)
{
    GotoState(SERVER_RUN_STATE_GET_SCRIPT_RESOURCE);
}

/** 已经获取到游戏世界ID，并进入启动其他服务状态
@param
@param
@return
*/
void CServerControl::OnStartService(void)
{
    if (m_dwServerRunState != SERVER_RUN_STATE_STOPPING && m_dwServerRunState != SERVER_RUN_STATE_INIT_SUCCESS)
    {
        return;
    }

    GotoState(SERVER_RUN_STATE_CREATE_SERVICE);
}


/**
@param
@param
@return
*/
void CServerControl::OnStopService(void)
{
	if(GetServerRunState() != SERVER_RUN_STATE_START_SUCCESS)
	{
		return;
	}

    g_pServerDlg->OnBnClickedStopservice();

	// 公告停机维护态
	SetServerRunState(SERVER_RUN_STATE_STOPPING);
}

/**
@param
@param
@return
*/
void CServerControl::OnUpdateScheme(void)
{
	if(gSocialGlobal != NULL && !m_stringWaitDynamic.empty())
	{
		ISchemeEngine * pEngine = gSocialGlobal->getSchemeEngine();
		if(pEngine == NULL)
		{
			return;
		}

		char szBuf[512] = {0};
		bool bResu = pEngine->UpdateScheme(m_stringWaitDynamic.c_str());
		if(bResu)
		{
			sprintf_s(szBuf, sizeof(szBuf), "Dynamic Update Scheme success!%s", m_stringWaitDynamic.c_str());
		}
		else
		{
			sprintf_s(szBuf, sizeof(szBuf), "Dynamic Update Scheme failed!!!%s", m_stringWaitDynamic.c_str());
		}

		// 输出到管理器
        //WarningLn(szBuf);
		AlertEmphasis(MG_AlertSrcType_Script, szBuf);
	}
}


/** 
@param   
@param   
@return  
*/
void CServerControl::OnUpdateLua(void)
{
    if(gSocialGlobal != NULL && !m_stringWaitDynamic.empty())
    {
        ILuaManager* pLuaManager = gSocialGlobal->getLuaManager();
        if(pLuaManager)
        {
            SERVICE_PTR luaService = pLuaManager->getLuaService();
            if( !luaService )
            {
                return;
            }
            ILuaService * pLuaService = (ILuaService*)luaService->query_interface();
            if (pLuaService == 0)
            {
                return;
            }

            pLuaService->UpdateLuaFile(m_stringWaitDynamic.c_str());
        }
    }
}

/**
@param
@param
@return
*/
void CServerControl::OnQueueState(void)
{
	IDBEngineService * pDBEngine = gSocialGlobal->getDBEngineService();
	if(pDBEngine == NULL)
	{
		return;
	}

    pDBEngine->outputQueue();
}

/**
@param
@param
@return
*/
void CServerControl::OnMemoryInfo(void)
{
	//ulong old = setMemoryDebugFlag(MDF_MEMORY_STAT|MDF_REPORT_TO_FILE);
	//reportMemoryStatistics();
	//setMemoryDebugFlag(old);

	// 查看事件引擎统计信息
	//OnViewEventStatInfo();
}

/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void CServerControl::OnTimer(unsigned long dwTimerID)
{
	switch(dwTimerID)
	{
	case ETimerEventID_Placard:
		{

		}
		break;
	case ETimerEventID_Kickout:
		{
			OnTimerKickout();
		}
		break;
	case ETimerEventID_ScoutSave:
		{
			OnTimerScoutSave();
		}
		break;
	case ETimerEventID_PrepCloseDlg:
		{
			OnTimePrepCloseDlg();
		}
		break;
	case ETimerEventID_MemoryMonitor:
		{
			OnTimerMemoryMonitor();
		}
		break;
	default:break;
	}
}

/** 
@param   
@param   
@return  
*/
void CServerControl::OnSetGameWorldID(void)
{

}

/** 
@param   
@param   
@return  
*/
void CServerControl::OnForceUnlock(void)
{
    ISocialGlobalService * ptr = SocialGlobalHelper().m_ptr;
    if (ptr != NULL)
    {
        ptr->requstExecuteUnlock();
    }
}

/**
@param
@param
@return
*/
void CServerControl::OnTimerKickout(void)
{
	ITimerService * pTimerAxis = gSocialGlobal->getTimerService();
    if(pTimerAxis != NULL)
    {
        pTimerAxis->KillTimer(ETimerEventID_Kickout, this);
    }
}

/**
@param
@param
@return
*/
void CServerControl::OnTimerScoutSave(void)
{

}

/**
@param
@param
@return
*/
void CServerControl::OnTimePrepCloseDlg(void)
{
	ITimerService * pTimerAxis = gSocialGlobal->getTimerService();
	if(pTimerAxis != NULL)
	{
		// 关闭准备关闭窗口
		pTimerAxis->KillTimer(ETimerEventID_PrepCloseDlg, this);
	}

	// 关闭服务
	// gSocialGlobal->CloseService();

	// 关闭窗口
	//theApp.PostMessage(WM_CTR_CLOSEDLG, 0, 0);
}

/** 
@param   
@param   
@return  
*/
void CServerControl::OnTimerMemoryMonitor(void)
{
    if((::GetTickCount() - m_dwLastMemoryGuardLineTick) < (10 * 60 * 1000))
    {
        return;
    }

    time_t long_time; struct tm * local_time;
    time(&long_time); local_time = localtime(&long_time);
    if(local_time == NULL)
    {
        return;
    }

    int nMemoryGuardLine = 1000;// 平时不能超过1G
    if(local_time->tm_hour > 2 && local_time->tm_hour < 8)
    {
        nMemoryGuardLine = 800;
    }

    int nCurProcessUsedMemory = GetCurProcessUsedMemory();
    if(nCurProcessUsedMemory > nMemoryGuardLine)
    {
        m_dwLastMemoryGuardLineTick = ::GetTickCount();

        char szWarnDesc[512] = { 0 };
        sprintf_s(szWarnDesc, "进程内存已越过警界线%dM, 当前=%dM.", nMemoryGuardLine, nCurProcessUsedMemory);

        ErrorLn(szWarnDesc);
        AlertError(MG_AlertSrcType_Memory, szWarnDesc);   
    }
}


/** 取进程使用内存情况
@param   
@param   
@return  返回以MB为单位
*/
DWORD CServerControl::GetCurProcessUsedMemory(void)
{  
	return getVirtualMemoryUsedSizeByCurrentProcess()/1024/1024/*折算成MB返回*/;
}