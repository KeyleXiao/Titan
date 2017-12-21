/*******************************************************************
** 文件名:	IServerController.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-6-23
** 版  本:	1.0
** 描  述:	

			游戏全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#pragma once
#include <string>


// 场景服运行状态
enum SERVER_RUN_STATE
{
    SERVER_RUN_STATE_IDLE = 0,                  // 未启动
    SERVER_RUN_STATE_INIT_SUCCESS,				// 初始化成功
    SERVER_RUN_STATE_INIT_FAIL,				    // 初始化失败

    SERVER_RUN_STATE_GET_AUTH_RESOURCE,         // 获取校验资源
    SERVER_RUN_STATE_CHECK_ACCOUNT_FAIL,		// 账号校验失败
    SERVER_RUN_STATE_GET_SCRIPT_RESOURCE,       // 获取脚本资源    

    SERVER_RUN_STATE_CREATE_SERVICE,    	    // 创建服务
    SERVER_RUN_STATE_START_SUCCESS,             // 启动成功
    SERVER_RUN_STATE_STOPPING,					// 正在关闭
    SERVER_RUN_STATE_STOPPED,					// 关闭延时	
};


// UI控制类型
enum SERVER_UI_CONTROL
{
    UICONTROL_IDLESSE = 0,		                // 空闲状态
    UICONTROL_INIT_SUCCESS,                     // 初始化成功
    UICONTROL_CHECK_ACCOUNT,                    // 校验账号有效性

    UICONTROL_START_SERVICE,		            // 启动服务
    UICONTROL_STOP_SERVICE,		                // 停止服务
    UICONTROL_UPDATE_SCHEME,		            // 更新配置
    UICONTROL_UPDATE_LUA,		                // 更新LUA
    UICONTROL_UPDATEMAPINFO,	                // 更新地图信息
    UICONTROL_QUEUESTATE,		                // 数据库队列信息
    UICONTROL_MEMORYINFO,		                // 内存信息
    UICONTROL_ONSETGAMEWORLDID,	                // 设置游戏世界ID
    UICONTROL_REFRESH_SETTING,                  // 刷新场景服设置

    UICONTROL_MAX				                // 最大态
};


struct IServerController
{
    virtual bool                    OnDo(DWORD dwState) = 0;

    /** 设置当前控制态
	@param dwState: UI控制类型  SERVER_UI_CONTROL
	@return  
	*/
	virtual void                    SetUIControl(DWORD dwUIState) = 0;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual DWORD					GetServerRunState(void) = 0;

	/** 
	@param   
	@param   
	@return  
	*/
    virtual void                    GotoState(int nRunState) = 0;
};
