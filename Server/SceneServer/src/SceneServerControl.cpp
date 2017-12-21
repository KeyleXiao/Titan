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
#include "SceneServerControl.h"
#include "ITimerService.h"
#include "IDBEngineService.h"
#include "ServerGlobalHelper.h"
#include "DialogPageStart.h"
#include "DialogPageMapinfo.h"
#include "DialogPageSetting.h"
#include "EntityHelper.h"
#include "DManagerModule.h"
#include "IManagerConnectorService.h"
#include "IMiscManager.h"
#include "EventDef.h"


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


CSceneServerControl   g_SceneServerControl;

//////////////////////////////////////////////////////////////////////////
/**
@param
@param
@return
*/
CSceneServerControl::CSceneServerControl(void)
{

    // 场景服运行状态 SERVER_RUN_STATE
    m_dwServerRunState = SERVER_RUN_STATE_IDLE;

	// 当前UI操作状态 SERVER_UI_CONTROL
	m_dwUIControlState = UICONTROL_IDLESSE;

    // 设置动态更新配置
    m_stringWaitDynamic.clear();

    // 启动停机公告tick
    m_dwStartPlacardTick = 0;

	// 当前场景服是否完全关闭了
	m_bShutupState = false;

    // 启动服务器的帐号
    m_stringUserName.clear();

    // 启动服务器的密码
    m_stringPassword.clear();

	// 上次提醒内存使用警告的时间
	m_dwLastMemoryGuardLineTick = 0;

	// 上次打印内存使用占有量时间
	m_dwLastMemoryPrintLineTick = 0;
}

/**
@param
@param
@return
*/

CSceneServerControl::~CSceneServerControl(void)
{
    // 场景服运行状态 SERVER_RUN_STATE
    m_dwServerRunState = SERVER_RUN_STATE_IDLE;

    // 当前UI操作状态 SERVER_UI_CONTROL
    m_dwUIControlState = UICONTROL_IDLESSE;

	// 当前场景服是否完全关闭了
	m_bShutupState = false;
}

/** 
@param   
@param   
@return  
*/
void CSceneServerControl::Create(void)
{
	ITimerService * pTimerAxis = gServerGlobal->getTimerService();
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
void  CSceneServerControl::Close(void)
{
	ITimerService * pTimerAxis = gServerGlobal->getTimerService();
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
void CSceneServerControl::SetServerRunState(DWORD dwState)
{
	m_dwServerRunState = dwState;
}

/**
@param
@param
@return
*/
DWORD CSceneServerControl::GetServerRunState(void)
{
	return m_dwServerRunState;
}

/** 逻辑线程信号到时，会调此接口
@param
@param
@return  返回true:表示操作已处理完，返回false:表示操作未处理完
*/
typedef void * (CSceneServerControl::* UICONTROL)(void);
bool CSceneServerControl::OnDo(DWORD dwState)
{
    static UICONTROL uicontrol[UICONTROL_MAX] =
    {
        (UICONTROL)0,											// 空闲状态
        (UICONTROL)&CSceneServerControl::OnInitalize,			// 初始化完成 
        (UICONTROL)&CSceneServerControl::OnCheckAccount,		// 校验账号有效性
        (UICONTROL)0,	                                        // 启动服务
        (UICONTROL)&CSceneServerControl::OnStopService,			// 停止服务
        (UICONTROL)&CSceneServerControl::OnUpdateScheme,		// 更新配置
        (UICONTROL)&CSceneServerControl::OnUpdateLua,		    // 更新LUA
        (UICONTROL)&CSceneServerControl::OnUpdateMapInfo,		// 更新地图信息
        (UICONTROL)&CSceneServerControl::OnQueueState,			// 数据库队列信息
        (UICONTROL)&CSceneServerControl::OnMemoryInfo,			// 内存信息
        (UICONTROL)&CSceneServerControl::OnSetGameWorldID,		// 触发器信息
        (UICONTROL)&CSceneServerControl::OnRefreshSetting,		// 刷新场景服设置
        // 最大态
    };

    if(dwState <= UICONTROL_IDLESSE || dwState >= UICONTROL_MAX)
    {
        return true;
    }

    (this->*uicontrol[dwState])();

    return true;
}


/** 设置当前控制态
@param
@param
@return
*/
void CSceneServerControl::SetUIControl(DWORD dwState)
{
    // 当前UI操作状态
    m_dwUIControlState = dwState;

    // 要跨Service驱动才能正常获取到场景数据
    IServerGlobalService * ptr = ServerGlobalHelper().m_ptr;
    if (ptr != NULL)
    {
        ptr->requestHandleServerControl(dwState);
    }
}


void CSceneServerControl::GotoState(int state)
{
    LeaveState(m_dwServerRunState);

    m_dwServerRunState = state;

    EnterState(m_dwServerRunState);
}


void CSceneServerControl::EnterState(int nRunState)
{
    switch (nRunState)
    {
    case SERVER_RUN_STATE_IDLE:     // 未启动
        break;

    case SERVER_RUN_STATE_INIT_SUCCESS:		// 初始化成功:
        {
            EmphasisLn("Scene Server Initialize Environment Success... ...");

            // 控制下自动启动
            Ini ini("config.ini");
            int nAutoStart = ini.getInt("setting","autostart",0);
            if(nAutoStart)
            {
                g_pDialogPageStart->OnBnClickedBtnStartservice();
            }
        }
        break;

    case SERVER_RUN_STATE_INIT_FAIL:        // 初始化失败
        {
            ErrorLn("Scene Server Initialize Environment Failed!!!");
        }
        break;

    case SERVER_RUN_STATE_GET_AUTH_RESOURCE:    // 获取校验资源
        {
            ServerGlobalHelper helper;
            if(helper.m_ptr)
            {
                helper.m_ptr->requestManagerStart(MANAGER_STARTFLAG_DBSERVER | MANAGER_STARTFLAG_DLL, 0);
            }
        }
        break;

    case SERVER_RUN_STATE_GET_SCRIPT_RESOURCE:   // 校验游戏ID
        {
            ServerGlobalHelper helper;
            if(helper.m_ptr)
            {
                helper.m_ptr->requestManagerStart(MANAGER_STARTFLAG_GETSCRIPT, 0);
            }
        }
        break;

    case SERVER_RUN_STATE_CHECK_ACCOUNT_FAIL:   // 账号校验失败
        {            
            ErrorLn(_GT("账号密码校验失败！请重新输入校验！UserName=")<< m_stringUserName.data() <<", Password="<< m_stringPassword.data());

            m_stringUserName.clear();
            m_stringPassword.clear();
            
            g_pDialogPageStart->ResetStartState();
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

    case SERVER_RUN_STATE_STOPPING:     // 正在关闭
        {
            // 正在关闭
            ReportServerState(MG_SERVERSTATE_STOPING);

            EmphasisLn(_GT("============== 开始关闭场景服 =============="));

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
            EmphasisLn(_GT("============== 数据处理完成，并延时退出 =============="));
        }
        break;

    default:break;
    }
}

void CSceneServerControl::LeaveState(int nRunState)
{
    switch (nRunState)
    {
    case SERVER_RUN_STATE_IDLE:             // 未启动
        break;
    case SERVER_RUN_STATE_INIT_SUCCESS:     // 初始化成功
        break;
    case SERVER_RUN_STATE_INIT_FAIL:        // 初始化失败
        break;
    case SERVER_RUN_STATE_GET_AUTH_RESOURCE:    // 获取校验资源
        break;
    case SERVER_RUN_STATE_CHECK_ACCOUNT_FAIL:	// 账号校验失败
        break;
    case SERVER_RUN_STATE_GET_SCRIPT_RESOURCE:	// 获取脚本资源
        break;
    case SERVER_RUN_STATE_CREATE_SERVICE:   // 创建服务
        break;
    case SERVER_RUN_STATE_START_SUCCESS:    // 启动成功
        break;
    case SERVER_RUN_STATE_STOPPING:         // 正在关闭
        break;
    case SERVER_RUN_STATE_STOPPED:          // 关闭延时	
        break;

    default:break;
    }
}


// 获取管理器连接服务
IManagerConnectorService*  CSceneServerControl::GetManagerConnectorService()
{
    return gServerGlobal->getManagerConnectorService();
}

// 上报服务器状态到管理器
void  CSceneServerControl::ReportServerState(int nState)
{
    IManagerConnectorService * pManagerConnectorService =  GetManagerConnectorService();
    if(pManagerConnectorService)
    {
        pManagerConnectorService->SetServerState(nState);
    }
}

//////////////////////////////////////////////////////////////////////////
/** 
@param   
@param   
@return  
*/
void CSceneServerControl::OnInitalize(void)
{
    EmphasisLn(_GT("初始化场景服环境成功"));
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
void CSceneServerControl::OnCheckAccount(void)
{
    IMiscManager* pMiscManager = gServerGlobal->getMiscManager();
    bool bRet = pMiscManager->checkAccount(m_stringUserName.data(), m_stringPassword.data());
    if(bRet)
    {
        m_stringUserName.clear();
        m_stringPassword.clear();

        GotoState(SERVER_RUN_STATE_GET_SCRIPT_RESOURCE);
    }
    else
    {
        GotoState(SERVER_RUN_STATE_CHECK_ACCOUNT_FAIL);
    }
}

/**
@param
@param
@return
*/
void CSceneServerControl::OnLocalStart(void)
{
    g_pDialogPageStart->OnBnClickedBtnStartservice();

	// 立即清掉密码，以绝后患，免得被监控内存
	m_stringUserName.clear(); m_stringPassword.clear();
}

/**
@param
@param
@return
*/
void CSceneServerControl::OnStopService(void)
{
	if(GetServerRunState() != SERVER_RUN_STATE_START_SUCCESS)
	{
		return;
	}

    // 公告停机维护态
    SetServerRunState(SERVER_RUN_STATE_STOPPING);

    // 发布停机事件    
    // 发服务器关闭事件，主要是通知场景服模块做善后事件
    IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
    if(pEventEngine != NULL)
    {
        pEventEngine->FireExecute(EVENT_STOP_SERVER_SERVICE_SAVE_DATA, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
    }

    // 开始处理GlobalService
    SERVICE_ID serviceID = gServerGlobal->getServerGlobalService();
    if (serviceID != INVALID_SERVICE_ID)
    {
        SERVICE_PTR pContainer = gServerGlobal->getFramework()->get_service_manager()->get_service(serviceID);
        if (pContainer)
        {
            pContainer->stop();
        }
    }

}

/**
@param
@param
@return
*/
void CSceneServerControl::OnUpdateScheme(void)
{
	if(gServerGlobal != NULL && !m_stringWaitDynamic.empty())
	{
		ISchemeEngine * pEngine = gServerGlobal->getSchemeEngine();
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
void CSceneServerControl::OnUpdateLua(void)
{
    if(gServerGlobal != NULL && !m_stringWaitDynamic.empty())
    {
        // 遍历所有的场景来刷脚本
        int nArrayScene[MAX_SCENE_COUNT];
        int nSceneCount = gServerGlobal->getSceneManager()->getAllSceneID(nArrayScene, sizeof(nArrayScene) / sizeof(int));
        for (int i=0; i<nSceneCount; ++i)
        {
           SceneLuaHelper helper(nArrayScene[i]);
           ILuaService * pLuaService = helper.m_ptr;
           if(pLuaService)
           {
               pLuaService->UpdateLuaFile(m_stringWaitDynamic.c_str());
           }
        }
    }
}

/**
@param
@param
@return
*/
void CSceneServerControl::OnUpdateMapInfo(void)
{
    g_pDialogPageMapinfo->UpdateSceneInfoToUI();
}

/**
@param
@param
@return
*/
void CSceneServerControl::OnQueueState(void)
{
	IDBEngineService * pDBEngine = gServerGlobal->getDBEngineService();
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
void CSceneServerControl::OnMemoryInfo(void)
{
    static DWORD s_dwLastCountMemoryInfo = GetTickCount();
    // TODO: 在此添加控件通知处理程序代码
    if(GetTickCount() - s_dwLastCountMemoryInfo <= 60000)
    {
        WarningLn("Refresh memory output too fast, one minute for refresh memory!");
        return;
    }
    s_dwLastCountMemoryInfo = GetTickCount();
}

/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void CSceneServerControl::OnTimer(unsigned long dwTimerID)
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
void CSceneServerControl::OnSetGameWorldID(void)
{

}

/** 
@param   
@param   
@return  
*/
void CSceneServerControl::OnRefreshSetting(void)
{
    g_pDialogPageSetting->OnRefresh();
}

/**
@param
@param
@return
*/
void CSceneServerControl::OnTimerKickout(void)
{
	ITimerService * pTimerAxis = gServerGlobal->getTimerService();
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
void CSceneServerControl::OnTimerScoutSave(void)
{

}

/**
@param
@param
@return
*/
void CSceneServerControl::OnTimePrepCloseDlg(void)
{
	ITimerService * pTimerAxis = gServerGlobal->getTimerService();
	if(pTimerAxis != NULL)
	{
		// 关闭准备关闭窗口
		pTimerAxis->KillTimer(ETimerEventID_PrepCloseDlg, this);
	}

	// 关闭服务
	// gServerGlobal->CloseService();

	// 关闭窗口
	//theApp.PostMessage(WM_CTR_CLOSEDLG, 0, 0);
}


/**
@param
@param
@return
*/
bool CSceneServerControl::IsZoneShutupState(void)
{
	return m_bShutupState;
}

/** 
@param   
@param   
@return  
*/
void CSceneServerControl::OnTimerMemoryMonitor(void)
{
	if((::GetTickCount() - m_dwLastMemoryGuardLineTick) < (10 * 60 * 1000))
	{
		return;
	}

	// 上次打印内存使用占有量时间
	int nCurProcessUsedMemory = GetCurProcessUsedMemory();
	if((::GetTickCount() - m_dwLastMemoryPrintLineTick) >= (60 * 60 * 1000))
	{
		EmphasisLn("本场景服当前内存占有量="<<nCurProcessUsedMemory<<"M.");
		m_dwLastMemoryPrintLineTick = ::GetTickCount();
	}

	time_t long_time; struct tm * local_time;
	time(&long_time); local_time = localtime(&long_time);
	if(local_time == NULL)
	{
		return;
	}

	int nMemoryGuardLine = 1400;// 平时不能超过1.4G
	if(local_time->tm_hour > 2 && local_time->tm_hour < 8)
	{
		nMemoryGuardLine = 1200;
	}
	
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
DWORD CSceneServerControl::GetCurProcessUsedMemory(void)
{  
	return getVirtualMemoryUsedSizeByCurrentProcess()/1024/1024/*折算成MB返回*/;
}