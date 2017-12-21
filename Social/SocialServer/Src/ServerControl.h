/*******************************************************************
** 文件名:	ServerControl.h 
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
#pragma once


#include "TimerHandler.h"
#include "IServerController.h"


struct IManagerConnectorService;

// 场景服控制器
class CServerControl : public TimerHandler,public IServerController
{
	enum
	{
		ETimerEventID_Placard = 0,			// 公告定时器
		ETimerEventID_Kickout,				// 踢角色定时器
		ETimerEventID_ScoutSave,			// 监视存盘定时器
		ETimerEventID_PrepCloseDlg,			// 准备关闭窗口
		ETimerEventID_MemoryMonitor,		// 内存监视器
	};
public:
	//////////////////////////IServerController///////////////////////////
    virtual bool                    OnDo(DWORD dwState);

    /** 设置当前控制态
	@param   
	@return  
	*/
	virtual void			        SetUIControl(DWORD dwUIState);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual DWORD					GetServerRunState(void);

	/** 
	@param   
	@param   
	@return  
	*/
    virtual void                    GotoState(int nRunState);
	//////////////////////////TimerHandler///////////////////////////
	/**
	@purpose          : 定时器触发后回调,你可以在这里编写处理代码
	@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
	@return		      : empty
	*/
	virtual void			OnTimer(unsigned long dwTimerID);

	/////////////////////////////CServerControl////////////////////
	/** 
	@param   
	@param   
	@return  
	*/
	void					SetServerRunState(DWORD dwState);

	/** 
	@param   
	@param   
	@return  
	*/
	void					Create(void);

	/** 
	@param   
	@param   
	@return  
	*/
	void					Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CServerControl(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CServerControl(void);

private:
	/** 
	@param   
	@param   
	@return  
	*/
    void                    OnInitalize(void);

	/** 
	@param   
	@param   
	@return  
	*/
    void                    OnCheckAccount(void);

    /**
    @param
    @param
    @return
    */
    void                    OnStartService(void);

	/** 
	@param   
	@param   
	@return  
	*/
	void					OnStopService(void);

	/** 
	@param   
	@param   
	@return  
	*/
	void					OnUpdateScheme(void);

	/** 
	@param   
	@param   
	@return  
	*/
	void					OnUpdateLua(void);

	/** 
	@param   
	@param   
	@return  
	*/
	void					OnQueueState(void);

	/** 
	@param   
	@param   
	@return  
	*/
	void					OnMemoryInfo(void);

    /** 
	@param   
	@param   
	@return  
	*/
	void					OnSetGameWorldID(void);

    /** 
	@param   
	@param   
	@return  
	*/
	void					OnForceUnlock(void);

	/** 
	@param   
	@param   
	@return  
	*/
	void					OnTimerKickout(void);

	/** 
	@param   
	@param   
	@return  
	*/
	void					OnTimerScoutSave(void);

	/** 
	@param   
	@param   
	@return  
	*/
	void					OnTimePrepCloseDlg(void);

	/** 
	@param   
	@param   
	@return  
	*/
	void					OnTimerMemoryMonitor(void);


private:
	/** 取进程使用内存情况
	@param   
	@param   
	@return  返回以MB为单位
	*/
	DWORD					GetCurProcessUsedMemory(void);

    void                    EnterState(int nRunState);
    void                    LeaveState(int nRunState);

    // 获取管理器连接服务
    IManagerConnectorService *      GetManagerConnectorService(void);
    // 上报服务器状态到管理器
    void                            ReportServerState(int nState);

private:
    // 服务器运行状态 SERVER_RUN_STATE
    DWORD					m_dwServerRunState;

	// 当前UI操作状态 SERVER_UI_CONTROL
	DWORD					m_dwUIControlState;


public:
	string					m_stringWaitDynamic;
	string					m_stringUnlockOperator;

	// 上次提醒内存使用警告的时间
	DWORD					m_dwLastMemoryGuardLineTick;
	
};
extern CServerControl   g_ServerControl;