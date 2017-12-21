#pragma once
#include "IVoiceConnector.h"
#include "IServiceContainer.h"
#include "buffer.h"
#include <vector>

using namespace Voice;

struct IVoiceConnectorService
{
    /**
    @name                : 连接语音服
    @brief  
	*/
	virtual bool            connectServer(void) = 0;

	/**
	@name                : 直接发送数据给语音服务器
	@brief               : 
	@param pData         : 要发送的数据
	@param dwLen         : 数据长度
	*/
	virtual void  SendData(const char * pData,DWORD dwLen) = 0;


	/** 注册消息处理者
	@param moduleId 模块ID，参考InputMessage.h中的枚举定义
	@param hander 消息处理者实例
	*/
	virtual bool registerMessageHandler(DWORD moduleId, IVoiceMessageHandler* handler) = 0;

	/** 注销消息处理者
	@param moduleId 模块ID，参考DGlobalMessage.h中的枚举定义
	*/
	virtual bool unregisterMessageHandler(DWORD moduleId) = 0;

	/** 注册事件处理者
	@param wEventID 事件ID	参考 VoiceDef.h   EMEventVoice
	@param hander 事件处理者实例
	*/
	virtual bool registerEventHandler(WORD wEventID,IVoiceEventHandler* handler) = 0;

	/** 注销事件处理者
	@param wEventID 事件ID	参考 VoiceDef.h   EMEventVoice
	@param  hander 事件处理者实例
	*/
	virtual bool unregisterEventHandler(WORD wEventID,IVoiceEventHandler* handler) = 0;


	////// 常用发数据接口 ////////////////////////////////////////////////////////////////////

	// 发数据给指定游戏服务器 dwServerID:服务器ID 为0时广播给所有服务器,bZeroBroadcast:为true时打开为0广播,否则无效
	virtual void SendDataToServer(DWORD dwServerID,const char * pData,DWORD dwLen,bool bZeroBroadcast=false) = 0;

	// 广播数据给指定列表游戏服务器 pServerArray:服务器ID列表,wServerNum:列表个数
	virtual void BroadcastDataToServer(DWORD * pServerArray,WORD wServerNum,const char * pData,DWORD dwLen) = 0;

    /** 广播数据到客户端
	@param	pData		: 数据
	@param	dwLen		: 数据长度
	@param	nBroadcastType : 广播类型 EMVoiceChannelType
	@param	dwPDBID1/dwPDBID2 : 用户PDBID
	@param	dwWorldID1/dwWorldID2 : 世界ID，
	@param  dwValueID1/dwValueID2 : ID值,参考SVoiceChannelID
	@param  bRoom : 是否广播用户dwPDBID1所在房间
	*/
	virtual void BroadcastDataToClient( BYTE nBroadcastType, void * pData, DWORD dwLen, DWORD dwPDBID1 = 0, DWORD dwPDBID2 = 0, DWORD dwWorldID1 = 0, DWORD dwWorldID2 = 0, DWORD dwValueID1 = 0, DWORD dwValueID2 = 0, bool bRoom = false ) = 0;
};
