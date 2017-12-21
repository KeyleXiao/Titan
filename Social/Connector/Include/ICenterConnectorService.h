/*******************************************************************
** 文件名:	ICenterConnectorService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/28/2014
** 版  本:	1.0
** 描  述:	

			中心服务器连接服务
********************************************************************/

#pragma once

#include "ICenterServerConnector.h"
#include "IServiceContainer.h"
#include "buffer.h"
#include <vector>

using namespace CenterServer;

/**
	职责: 
	1.连接中心服务器
	2.根据中心服务器上返回的网关数量，对网关服务器发起连接
*/
struct ICenterConnectorService
{
		/**
		@name                 : 连接中心服务器
		@brief                :
		@return               :
		*/
		virtual bool            connectServer(void) = 0;

		/**
		@name                 : 向中心服务器更新服务器信息
		@brief                :
		@param  pServerInfo   : 服务器信息内容,不同的服务器有不同的扩展信息,例如区域服务器就有该服务器负责的区域ID等
		@param  wLen          : 服务器信息长度
		@return               :
		*/
		virtual bool            updateServerInfo(ibuffer & info) = 0;

		/**
		@name         : 取得本地服务器在中心服务器上的ID
		@brief        :
		*/
		virtual CSID            getLocalServerCSID() = 0;

		/**
		@name         : 取得网络上所有服务器的个数
		@brief        :
		*/
		virtual DWORD           getAllServersCount() = 0;

		/**
		@name                : 取得服务器列表
		@brief               :
		@param serverPtrArray: 服务器指针数组,用来返回指向服务器信息结构的指针
		@param dwArratSize   : 指针数组的大小,防止越界
		@param GroupFilter   : 按组ID进行过滤,如果GroupFilter为INVALID_SERVER_ID则表示不进行组过滤
		@param wTypeFilter   : 按服务器类型过滤,如果wTypeFilter为MSG_SERVERID_UNKNOW则表示不进行服务器类型过滤
		@note                : 例如返回组ID等于1的所有网关服务器:
		@note                : ServerInfo * serverPtrArray[MAX_SERVER_COUNT];
		@note                : DWORD count=ICenterServerConnector::GetServerList(serverPtrArray,1024,1,MSG_SERVER_GATE);
		@return              :
		*/
		virtual DWORD           getServerList(ServerData ** serverPtrArray,DWORD dwArratSize,CGID GroupFilter=INVALID_SERVER_ID,WORD wTypeFilter=MSG_ENDPOINT_UNKNOW) = 0;

		/**
		@name                : 取得服务器信息
		@brief               :
		@param  ServerID     : 服务器ID
		@return              : 如果目标服务器存在则返回服务器信息,否则返回0
		*/
		virtual ServerData      getServerInfo(CSID ServerID) = 0;

		/**
		@name                : 取得本服务器类型
		@return              : 
		*/
		virtual WORD        getServerType(void) = 0;

		/**
		@name                : 向某个服务器发送一条消息
		@brief               : 服务器之间的消息转发
		@param DestServerID  : 目标服务器ID
		@param data			 : 消息内容
		@param len			 : 消息长度
		@return              : 是否成功发到了中心服务器
		*/
		virtual void            postMessage(CSID DestServerID, void * data, int len) = 0;

		/**
		@name                : 向多个服务器广播消息
		@brief               :
		@param DestServerArray:目标服务器数组
		@param wDestServerCount:目标服务器个数
		@param pMessage      : 消息内容
		@param wMessageLen   : 消息长度
		@return              : 是否成功发到了中心服务器
		*/
		virtual void            broadcastMessage(string & id_buff,ibuffer & msg) = 0;

		/** 向场景服发送消息
		@param   
		@param   
		@return  
		*/
		virtual void            sendDataToZoneSvr(LPCSTR pData, size_t nLen, CSID ServerID = INVALID_SERVER_ID) = 0;

        /** 向HttpServer服发送消息
        @param   
        @param   
        @return  
        */
        virtual void            sendDataToDBHttpSvr(BYTE byKeyModule, BYTE byKeyAction, LPCSTR pData, size_t nLen) = 0;
};
