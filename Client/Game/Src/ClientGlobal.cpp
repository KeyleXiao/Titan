/*******************************************************************
** 文件名:	ClientGlobal.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	1/7/2015
** 版  本:	1.0
** 描  述:

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


#pragma once
#include "stdafx.h"
#include "ClientGlobal.h"
#include "SchemeCenter.h"
#include "HandleCommandAttack.h"
#include "RenderTaskMgr.h"
#include "Migrate.h"
#include "GameVoiceClient.h"
#include "CrashReport\CrashReport.h"
#include "DistrictList.h"
#include "MiscManager.h"
#include "HeightFieldMgr.h"

#include "DbgHelp.h"
#include "IStackWalk.h"

using namespace rkt;

HWND  g_MainWnd = 0;

bool CheckOSVer()
{
	OSVERSIONINFO   osver;
	osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osver);

	if (osver.dwMajorVersion > 6)
	{
		return true;
	}
	
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion > 0)
	{
		return true;
	}

	return false;
}

extern CrashReport g_CrashReport;

CClientGlobal::CClientGlobal() 
	:m_pMessageDispatch(0)
	,m_pStageManager(0)
	,m_pEventEngine(0)
	,m_pHero(0)
	,m_pEntityClient(0)
	,m_traceListener(0)
	,m_pSchemeEngine(0)
	,m_pSchemeCenter(0)
	,m_pConditionFactory(0)
	,m_pEffectFactory(0)
	,m_pSpellFactory(0)
	,m_pFreezeClient(0)
	,m_pBuffClient(0)
	,m_pTankClient(0)
	,m_pWarClient(0)
	,m_pPathFindManager(0)
	,m_pHandleCommandManager(0)
    ,m_pRenderView(0)
	,m_pChatClient(0)
	,m_pOrderClient(0)
    ,m_pMatchClient(0)
	,m_pAntiAddictClient(0)
	,m_bCreated(false)
	,m_pSNSClient(0)
	,m_pMentorshipClient(0)
	,m_pMailClient(0)
	,m_pKeyWordClient(0)
    ,m_pMiscManager(0)
	,m_pScreenCaptureClient(0)
    ,m_pHeightFieldMgr(0)
{
	memset(szLogFileName, 0, sizeof(szLogFileName));
}

void CClientGlobal::openLog()
{
    // 是否有log目录
    char szLogAbsolutePath[512] = { 0 };
    sprintf_s(szLogAbsolutePath, "%s\\Log", getWorkDir() );
    bool isAbsolutePath = true; uint attrib = 0;
    if(!rkt::checkPath(szLogAbsolutePath, isAbsolutePath, attrib))
    {
        rkt::createDirectoryRecursive(szLogAbsolutePath);
    }

    time_t	long_time; struct tm * local_time;
    time(&long_time); local_time = localtime(&long_time);
	ZeroMemory(szLogFileName,GAME_TEXTSTR_MAXSIZE);
    sprintf_s(szLogFileName, "%s\\%d-%02d-%02d[%d-%d-%d]log.html",szLogAbsolutePath , local_time->tm_year+1900,
        local_time->tm_mon+1, local_time->tm_mday, local_time->tm_hour, local_time->tm_min, local_time->tm_sec);

    m_traceListener = output2File(szLogFileName,levelAll,true);
}

IRenderViewProxy * CClientGlobal::getRenderView() 
{ 
    return m_pRenderView;
}

bool CClientGlobal::create()
{
	// 是否已创建
	if (m_bCreated)
	{
		return true;
	}

    outputMachineDetail();

	rkt::setGlobal(this);
    m_pRenderView = &CRenderTaskMgr::getInstance(); 


    if (!NetSniffer::getInstance().QueryDns())
    {
        NetSniffer::getInstance().CancelQuery();
    }

	srand(getTickCount());

#ifdef SUPPORT_STACKWALK	
	//if (!isDebugMode)
	//{
		pStackWalk = createStackWalkProc();
		if (pStackWalk)
			pStackWalk->attachStackWalkCallback(&g_CrashReport);
	//}
#endif

	m_Connection.Release();

	//registerTrace(m_traceListener);

	m_pMessageDispatch = new MessageDispatch;
	m_pStageManager    = new Stage_Manager;

    string strWorkDir = rkt::getWorkDir();
    // 设置搜索路径
    string stringScpDir = strWorkDir;
    stringScpDir += "\\Data";

    std::string strPatchName = strWorkDir + "\\Data\\Data.mpk";


    rkt::Ini config("config.ini");
    // 是否读外包先
    bool bOutFileFirst =  config.getInt("App", "ReadOutFileFirst", 0);
    if(bOutFileFirst)
    {
        getFileSystem()->addFindPath(stringScpDir.c_str());

        if (isFile(strPatchName.c_str())) {
            getFileSystem()->addFindPath( strPatchName.c_str() );
        }
    }else
    {
        if (isFile(strPatchName.c_str())) {
            getFileSystem()->addFindPath( strPatchName.c_str() );
        }
        getFileSystem()->addFindPath(stringScpDir.c_str());
    }

	TraceLn("Start loading EventEngine...");
	m_pEventEngine = CreateEventEngine();

    m_pHeightFieldMgr = new HeightFieldMgr();

	// 配置引擎
	TraceLn("Start loading SchemeEngine......");
	if ((m_pSchemeEngine = CreateSchemeEngineProc()) == NULL)
	{
		return false;
	}
	m_pSchemeEngine->AllowSameSchemeFileName();

	TraceLn("Start loading SchemeCenter...");
	// 游戏脚本配置中心
	m_pSchemeCenter = new SchemeCenter;	
	if(!m_pSchemeCenter->load())
	{
		ErrorLn("SchemeCenter load failure!");
		return false;
	}

	TraceLn("Start loading EntityClient...");
	m_pEntityClient = CreateEntityClientProc();
	if (!m_pEntityClient)
	{
		ErrorLn("EntityClient load failure!");
		return false;
	}

	Migrate::getInstance().create();

	TraceLn("Start loading ConditionFactory....");
	m_pConditionFactory = CreateConditionFactoryProc();
	if ( m_pConditionFactory ==0 || !m_pConditionFactory->Load("Scp\\condition.bin"))
	{
		ErrorLn("ConditionFactory load failure!");
		return false;
	}

	TraceLn("Start loading EffectFactory....");
	m_pEffectFactory = CreateEffectFactoryProc();
	if ( m_pEffectFactory ==0 /*|| !m_pEffectFactory->Load("Scp\\effect.csv")*/ || !m_pEffectFactory->Load("Scp\\effect.bin"))
	{
		ErrorLn("EffectFactory load failure!");
		return false;
	}

	TraceLn("Start loading SpellFactory....");
	m_pSpellFactory = CreateSpellFactoryProc();
	if ( m_pSpellFactory ==0 || !m_pSpellFactory->Load(SPELL_FILENAME, SPELL_LOGIC_FILENAME))
	{
		ErrorLn("SpellFactory load failure!");
		return false;
	}	

	TraceLn("Start loading FreezeClient....");
	m_pFreezeClient = CreateFreezeClientProc();
	if ( m_pFreezeClient == 0)
	{
		ErrorLn("FreezeClient load failure!");
		return false;
	}

	TraceLn("Start loading BuffClient....");
	m_pBuffClient = CreateBuffClientProc();
	if ( m_pBuffClient == 0)
	{
		ErrorLn("BuffClient load failure!");
		return false;
	}

	TraceLn("Start loading TankClient....");
	m_pTankClient = CreateTankClientProc();
	if ( m_pTankClient == 0)
	{
		ErrorLn("TankClient load failure!");
		return false;
	}

	TraceLn("Start loading WarClient....");
	m_pWarClient = CreateWarClientProc();
	if ( m_pWarClient == 0)
	{
		ErrorLn("WarClient load failure!");
		return false;
	}
	
    TraceLn("Start loading PathFindManager....");
    m_pPathFindManager = CreatePathFindManagerProc();
    if ( m_pPathFindManager == 0)
    {
        ErrorLn("PathFindManager load failure!");
        return false;
    }

	TraceLn("Start loading ChatClient..." );
	m_pChatClient = CreateChatClientProc();
	if ( m_pChatClient == 0 )
	{
		ErrorLn("ChatClient load failure!" );
		return false;
	}

	TraceLn("Start loading SNSClient..." );
	m_pSNSClient = CreateSNSClientProc();
	if ( m_pSNSClient == 0 )
	{
		ErrorLn("SNSClient load failure!" );
		return false;
	}

    TraceLn("Start loading MatchClient..." );
    m_pMatchClient = CreateMatchClientProc();
    if ( m_pMatchClient == 0 )
    {
        ErrorLn("m_pMatchClient load failure!" );
        return false;
    }

    TraceLn("Start loading OrderClient..." );
    m_pOrderClient = CreateOrderClientProc();
    if ( m_pOrderClient == 0 )
    {
        ErrorLn("OrderClient load failure!" );
        return false;
    }

    TraceLn("Start loading ClanClient..." );
    m_pClanClient = CreateClanClientProc();
    if ( m_pClanClient == 0 )
    {
        ErrorLn("ClanClient load failure!" );
        return false;
    }

    TraceLn("Start loading KinClient..." );
    m_pKinClient = CreateKinClientProc();
    if ( m_pKinClient == 0 )
    {
        ErrorLn("KinClient load failure!" );
        return false;
    }

    TraceLn("Start loading TeamClient..." );
    m_pTeamClient = CreateTeamClientProc();
    if ( m_pTeamClient == 0 )
    {
        ErrorLn("TeamClient load failure!" );
        return false;
    }

	TraceLn("Start loading AntiAddictClient...");
	m_pAntiAddictClient = CreateAntiAddictClientProc();
	if ( m_pAntiAddictClient == 0)
	{
		ErrorLn("AntiAddictClient load failure!");
		return false;
	}

    TraceLn("Start loading TaskClient...");
    m_pTaskClient = CreateTaskClientProc();
    if ( m_pTaskClient == 0)
    {
        ErrorLn("TaskClient load failure!");
        return false;
    }

    TraceLn("Start loading LegendCupClient...");
    m_pLegendCupClient = CreateLegendCupClientProc();
    if ( m_pLegendCupClient == 0)
    {
        ErrorLn("LegendCupClient load failure!");
        return false;
    }

	TraceLn("Start loading MentorshipClient..." );
	m_pMentorshipClient = CreateMentorshipClientProc();
	if ( m_pMentorshipClient == 0 )
	{
		ErrorLn("MentorshipClient load failure!" );
		return false;
	}

	TraceLn("Start loading MailClient..." );
	m_pMailClient = CreateMailClientProc();
	if ( m_pMailClient == 0 )
	{
		ErrorLn("MailClient load failure!" );
		return false;
	}

	TraceLn("Start loading KeyWordClient...");
	m_pKeyWordClient = CreateKeyWordClientProc();
	if (m_pKeyWordClient == 0)
	{
		ErrorLn("KeyWordClient load failure!" );
		return false;
	}

	if (CheckOSVer())
	{
		TraceLn("Start loading screenCaptureClient...");
		m_pScreenCaptureClient = CreateScreenCaptureClientProc();
		if (m_pScreenCaptureClient == 0)
		{
			ErrorLn("screenCaptureClient load failure!");
			return false;
		}
	}
	else
	{
		WarningLn("OS version too low, close screenCaptureClient");
	}

	m_pHandleCommandManager = new HandleCommandManager();
	if (m_pHandleCommandManager == 0 || !m_pHandleCommandManager->create())
	{
		ErrorLn("m_pHandleCommandManager load failure!");
		return false;
	}

    m_pMiscManager = new CMiscManager();
    if(m_pMiscManager == 0)
    {
        ErrorLn("MiscManager create failure!");
        return false;
    }

    CGameVoiceClient::getInstance().Create();

	new AttackCmdQueue();
	AttackCmdQueue::getInstance().create();

	// 初始化游戏分区列表
	DistrictList::getInstance().Initialize();

	// 启动成功事件
	if (m_pEventEngine)
	{
		m_pEventEngine->FireExecute(EVENT_SYSTEM_GLOBAL_STARTED, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
	}

	//将LOG文件名发送给Unity
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_OPEN_LOG_FILE,0,szLogFileName,0,0/*GAME_TEXTSTR_MAXSIZE*/);

#ifdef SUPPORT_STACKWALK
    char szCrashFileName[512] = { 0 };
    sprintf_s(szCrashFileName, "%s\\Client_Crash.txt", getWorkDir());

    char szDumpFileName[512] = { 0 };
    sprintf_s(szDumpFileName, "%s\\Client_Stack.dmp", getWorkDir());
    // dump
    IStackWalk * pStackWalk = createStackWalkProc();
    if(pStackWalk == NULL)
    {
        return false;
    }	

    // dump类型
    pStackWalk->setDumpType(MiniDumpNormal);

    // dump文件
    pStackWalk->setLogFileName(my_utf82t(szCrashFileName));
    pStackWalk->setDumpFileName(my_utf82t(szDumpFileName));
#endif

	EmphasisLn("Client create success!");
	m_bCreated = true;
	return true;
}

bool  CClientGlobal::destroy()
{
	ErrorLn("client destroy()");
    
    net_setHandler(NULL);
    PingReplyer::getInstance().Stop();
    GatewayClient::getInstance().Stop();

	NetSniffer::getInstance().CancelQuery();
	// 启动成功事件
	if (m_pEventEngine)
	{
		m_pEventEngine->FireExecute(EVENT_SYSTEM_GLOBAL_STOP, SOURCE_TYPE_SYSTEM, 0, NULL, 0);
	}
    
	AttackCmdQueue::getInstance().release();
	delete AttackCmdQueue::getInstancePtr();

    CGameVoiceClient::getInstance().Release();
    getTimeSyncer()->Stop();

	DistrictList::getInstance().Close();

	if (m_pHandleCommandManager != 0)
	{
		m_pHandleCommandManager->close();
		m_pHandleCommandManager = NULL;
	}

	m_TimeSyncer.Stop();

    if(m_pHeightFieldMgr)
    {
        delete m_pHeightFieldMgr;
        m_pHeightFieldMgr = 0;
    }

    safeRelease(m_pMiscManager);
    safeRelease(m_pKinClient);
    safeRelease(m_pClanClient);
    safeRelease(m_pMatchClient);
    safeRelease(m_pOrderClient);
	safeRelease(m_pWarClient);
	safeRelease(m_pTankClient);
	safeRelease(m_pBuffClient);
	safeRelease(m_pFreezeClient);
    safeRelease(m_pPathFindManager);
    safeRelease(m_pChatClient);
	safeRelease(m_pTeamClient);
	safeRelease(m_pMentorshipClient);
	SAFERELEASE(m_pAntiAddictClient);
    SAFERELEASE(m_pTaskClient);
    SAFERELEASE(m_pLegendCupClient);
	SAFERELEASE(m_pEntityClient);
	safeRelease(m_pSNSClient);
	safeRelease(m_pMailClient);
	safeRelease(m_pKeyWordClient);
	safeRelease(m_pScreenCaptureClient);

    /* 
    这种与实体无关系的可以放到后面释放，因为m_pEntityClient里面会释放实体,
    实体释放会释放部件，比如BUFF部件释放，会调用m_pEffectFactory里面停止光效
    */
    safeRelease(m_pConditionFactory);
    safeRelease(m_pEffectFactory);
    safeRelease(m_pSpellFactory);

	safeDelete(m_pMessageDispatch);
	safeRelease(m_pStageManager);

	if ( m_traceListener)
	{
		unregisterTrace(m_traceListener);
		safeDelete(m_traceListener);
	}

	SAFERELEASE(m_pEventEngine);
	SAFERELEASE(m_pSchemeEngine);

    Migrate::getInstance().release();


    m_Connection.Release();

	m_bCreated = false;
    m_pRenderView = 0;

#ifdef SUPPORT_STACKWALK
	if (pStackWalk)
		pStackWalk->detachStackWalkCallback(&g_CrashReport);
#endif

	releaseInstance();

    rkt::setGlobal(NULL);

	return true;
}

/// 获取网络连接
IClientConnection*  CClientGlobal::getNetConnection()
{
    return &m_Connection;
}

/**
@name         : 获取队列里未处理的数据包个数
@name         : 获取队列是发包,否则是收包
@return       : 
*/
unsigned long CClientGlobal::getPendingCount(bool bIsSend)
{
    return m_Connection.GetPendingCount(bIsSend);
}

/// 获取网络延迟
unsigned long  CClientGlobal::getNetLatency()
{
    return PingReplyer::getInstance().GetLatency();
}

bool CClientGlobal::gotoState(BYTE nState,void * context/*=NULL*/,int len/*=0 */)
{
    return m_pStageManager->gotoState((EStage_State)nState, context, len );
}

BYTE CClientGlobal::getCurState()
{
    return m_pStageManager->getCurState();
}

IDistrictListClient* CClientGlobal::getDistrictListClient() 
{
	return &(DistrictList::getInstance());
}


void CClientGlobal::outputMachineDetail()
{
    TCHAR pcName[256] = {0};
    DWORD dwSize = 256;
    ::GetComputerName(pcName, &dwSize);
    EmphasisLn("Computer Name:"<<my_t2utf8(pcName));

    if( isNetworkBarUser() ) {
        EmphasisLn("Guess:Network-bar User");
    } else {
        EmphasisLn("Guess:Home User");
    }

    // 内存状态
    MEMORYSTATUSEX ms;
    ms.dwLength = sizeof(ms);
    ::GlobalMemoryStatusEx(&ms);

    TCHAR szFileName[1024];
    std::string ver;
    GetModuleFileName(GetModuleHandle(NULL), szFileName, sizeof(szFileName));
    getModuleFileVersion(my_t2utf8(szFileName), ver);
    EmphasisLn("Version:"<<ver);

    // 输出操作系统信息
    DWORD major, minor;
    std::string winver = getWinVersion(&major, &minor);
    EmphasisLn("OS:"<<winver);

    // 输出cpu信息
    std::string cpu;
    getProcessorName(cpu);
    EmphasisLn(_f("CPU: %s", cpu.c_str()));

    // 输出内存状态
    EmphasisLn("PhysMemory:"<<((ms.ullAvailPhys)/1048576)<<"/"<<((ms.ullTotalPhys)/1048576)<<"M");

    // 输出显卡信息
    std::string video;
    getVideoCardInfo(video);
    EmphasisLn("Video card:"<<video);

    // 输出显示模式
    int w,h,depth,freq;
    getDisplayMode(&w, &h, &depth, &freq);
    EmphasisLn("Display Mode:"<<w<<" X "<<h<<" "<<depth<<"Bits "<<freq<<"Hz");
}
