/*******************************************************************
** 文件名:	IGlobalViewMng.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/22
** 版  本:	1.0
** 描  述:	观战管理服全局接口
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "Common.h"
#include "ViewDef.h"
#include "DGlobalMessage.h"

class ServerUser;
class GatewayUser;


/// 逻辑线程任务接口
struct ILogicThreadTask
{
	/** 逻辑线程信号到时，会调此接口
	@return  返回true:表示操作已处理完，返回false:表示操作未处理完
	*/
	virtual bool						OnDo(void) = NULL;
};

/// 逻辑线程任务管理器
struct ILogicThread
{
	/// 在逻辑线程上挂一个任务
	virtual bool						AttachTask(ILogicThreadTask * pTask, LPCSTR pszDebug) = NULL;

	/// 在逻辑线程上取消一个任务
	virtual void						DetachTask(ILogicThreadTask * pTask) = NULL;
};


// 游戏世界信息
struct IVoiceServerWorldInfo
{
	// 游戏世界ID
	virtual DWORD GetWorldID(void) = 0;
	// 游戏世界名称
	virtual const char * GetWorldName(void) = 0;
	// 服务器个数
	virtual DWORD GetServerCounts(void) = 0;
};



/// 语音服务器全局对象
struct IGlobalViewMng
{
	///// 网关网络消息处理
	//virtual void onGatewayMessage(GatewayUser &server, ulong actionId, SGameMsgHead* head, void* data, size_t len) = 0;

	///// 客户端网络消息处理
	//virtual void onClientMessage(GatewayUser &server, CClientUser & client, ulong actionId, SGameMsgHead* head, void* data, size_t len) = 0;

	/// 获取时间轴
	virtual TimerAxis* getTimerAxis() = 0;

	/// 获取事件引擎
	virtual IEventEngine* getEventEngine() = 0;

	// 启用定时器
	virtual bool SetTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo, DWORD callTimes = INFINITY_CALL)=0;

	// 销毁定时器
	virtual bool KillTimer(DWORD timerID, ITimerHandler * handler)=0;



	//// 发数据给指定游戏客户端 dwClientID:客户端ID 
	//virtual bool SendDataToClient(DWORD dwClientID,const char * pData,DWORD dwLen, BYTE nSendChennel = VoiceSendChannel_Control ) = 0;

	//// 发数据给指定语音网关 dwGatewayID:语音网关ID 
	//virtual bool SendDataToGateway(DWORD dwGatewayID,const char * pData,DWORD dwLen,bool bZeroBroadcast=false) = 0;

	// 广播数据给指定列表语音网关 pServerArray:语音网关ID列表,wClientNum:列表个数 pServerArray==NULL && wGatewayNum==VOICE_BROADCASTALL_NUM 广播所有
	virtual bool BroadcastDataToGateway(DWORD * pGatewayArray,WORD wGatewayNum,const char * pData,DWORD dwLen) = 0;


	// 网关断线
	virtual void OnGatewayUserLeave(GatewayUser *pServer, DWORD dwReason = 0) = 0;


	// 设置当前操作代码  DWORD dwCode 参考 EMVoiceServerControlCode
	virtual void SetControlCode(DWORD dwCode) = 0;


	////// 常用服务器列表相关接口 ////////////////////////////////////////////////////////////////////

	/**  取得指定游戏世界的指定类型的服务器ID
	@param   DWORD dwWorldID		: 游戏世界ID
	@param   WORD wServerType		: 服务器类型
	@param   WORD wSubID			: 服务器序号
	@return  找到的服务器ID,为0表示没找到
	*/
	virtual DWORD GetGameServerIDByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID) = 0;

	////// 常用游戏世界列表相关接口 ////////////////////////////////////////////////////////////////////

	/**  取得游戏世界ID列表
	@param   DWORD* pWorldArray	: 保存找到的ID数组
	@param   WORD dwArrayLen		: 数组长度
	@param   WORD &dwGetCounts		: 找到的个数
	@return  找到的个数
	*/
	virtual DWORD GetGameWorldIDList(DWORD * pWorldArray,DWORD dwArrayLen,DWORD &dwGetCounts) = 0;

	/**  取得指定游戏世界信息
	@param   DWORD dwWorldID		: 游戏世界ID
	@return  找到的服务器ID,为0表示没找到
	*/
	virtual IVoiceServerWorldInfo * GetGameWorldInfo(DWORD dwWorldID) = 0;

	/** 列出所有游戏世界信息
	@return 
	*/
	virtual void PrintGameWorldListInfo() = 0;


};
extern IGlobalViewMng* g_pViewMng;
