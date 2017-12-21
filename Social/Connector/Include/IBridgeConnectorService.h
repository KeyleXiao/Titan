/*******************************************************************
** 文件名:	IBridgeConnectorService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-07-09
** 版  本:	1.0
** 描  述:	桥服务器连接服务			
********************************************************************/

#pragma once

#include "IBridgeConnector.h"
#include "IServiceContainer.h"
#include "buffer.h"
#include <vector>

using namespace Bridge;

/**
	职责: 
	1.连接桥服务器
*/
struct IBridgeConnectorService
{
		/** 取得本地服务器在跨区桥服务器上的ID
		*/
		virtual DWORD                       getServerID() = 0;

		/** 取得公共游戏世界ID
		*/
		virtual DWORD                       getPublicWorldID() = 0;

		/** 直接发送数据给跨区桥服务器
		@param pData         : 要发送的数据
		@param dwLen         : 数据长度
		*/
		virtual void                        sendData(ibuffer & msg) = 0;

		////// 常用发数据接口 ////////////////////////////////////////////////////////////////////

		/** 发数据给指定游戏服务器 dwServerID:服务器ID 为0时广播给所有服务器,bZeroBroadcast:为true时打开为0广播,否则无效
		*/
		virtual void                        sendDataToServer(DWORD dwServerID,ibuffer & msg,bool bZeroBroadcast=false) = 0;

		/** 广播数据给指定列表游戏服务器 pServerArray:服务器ID列表,wServerNum:列表个数
		*/
		virtual void                        broadcastDataToServer(DWORD * pServerArray,WORD wServerNum,ibuffer & msg) = 0;

		/**
		@name         : 发服务器扩展信息消息 dwServerID:服务器ID 为0时广播给所有桥服务器连接器,跨区桥服务器连接器同步服务器扩展信息专用
		@brief        :
		*/
		virtual bool                        sendExtDataToServer(DWORD dwServerID,DWORD dwMsgCode,DWORD dwUserData1=0,DWORD dwUserData2=0,DWORD dwUserData3=0,const char * pData=0,DWORD dwLen=0) = 0;
		
		////// 常用服务器列表相关接口 ////////////////////////////////////////////////////////////////////
		/**  取得服务器ID列表
		@param   DWORD* pServerArray	: 保存找到的ID数组
		@param   WORD dwArrayLen		: 数组长度
		@param   WORD &dwGetCounts		: 找到的个数
		@param   DWORD dwWorldID		: 游戏世界ID，0为全部
		@param   WORD wServerType		: 服务器类型,0为全部
		@return  找到的个数
		*/
		virtual DWORD                       getGameServerIDList(DWORD * pServerArray,DWORD dwArrayLen,DWORD &dwGetCounts,DWORD dwWorldID=0,WORD wServerType=0) = 0;

		/**  取得指定游戏世界的指定类型的服务器ID
		@param   DWORD dwWorldID		: 游戏世界ID
		@param   WORD wServerType		: 服务器类型
		@param   WORD wSubID			: 服务器序号
		@return  找到的服务器ID,为0表示没找到
		*/
		virtual DWORD                       getGameServerIDByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID) = 0;

		////// 常用游戏世界列表相关接口 ////////////////////////////////////////////////////////////////////

		/**  取得游戏世界ID列表
		@param   DWORD* pWorldArray	: 保存找到的ID数组
		@param   WORD dwArrayLen		: 数组长度
		@param   WORD &dwGetCounts		: 找到的个数
		@return  找到的个数
		*/
		virtual DWORD                       getGameWorldIDList(DWORD * pWorldArray,DWORD dwArrayLen,DWORD &dwGetCounts) = 0;

		/**  取得指定游戏世界信息
		@param   DWORD dwWorldID		: 游戏世界ID
		@return  找到的服务器ID,为0表示没找到
		*/
		virtual IBridgeWorldInfo *          getGameWorldInfo(DWORD dwWorldID) = 0;

		////// 常用扩展信息类列表相关接口 ////////////////////////////////////////////////////////////////////

		/** 用服务器ID 取得扩展信息类
		@param   DWORD dwServerID		: 服务器ID
		@return  IBridgePart*	:服务器扩展信息类
		*/
		virtual IBridgePart*                getGameServerPart(DWORD dwServerID) = 0;

		/** 用TypeSubID取得扩展信息类
		@param   DWORD dwWorldID		: 游戏世界ID
		@param   WORD wServerType		: 服务器类型
		@param   WORD wSubID			: 服务器序号
		@return  IBridgePart*	:服务器扩展信息类
		*/
		virtual IBridgePart*                getGameServerPartByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID) = 0;

		/** 本服务器扩展信息类指针
		@return  IBridgePart*	:服务器扩展信息类
		*/
		virtual IBridgePart*                getLocalBridgePart() = 0;

		/** 通过指定服务器发中转消息数据给目标游戏服务器 dwServerID:服务器ID 为0时广播给所有服务器,DWORD dwTransitServerID 中转到服务器ID
		*/
		virtual void                        sendTransitDataToServer(DWORD dwServerID,DWORD dwTransitServerID,const char * pData,DWORD dwLen) = 0;
};
