using System;
using System.Reflection;
using System.IO;
using System.Text;

struct ibuffer
{
};

struct obuf
{
};

struct CSID  
{
};

struct CGID  
{
};

struct ServerData
{
};

struct DWORD
{
};

struct WORD
{
};


interface CenterConnectorService
{
		/**
		@name                 : 向中心服务器更新服务器信息
		@brief                :
		@param  pServerInfo   : 服务器信息内容,不同的服务器有不同的扩展信息,例如区域服务器就有该服务器负责的区域ID等
		@param  wLen          : 服务器信息长度
		@return               :
		*/
		bool UpdateServerInfo(ibuffer info);

		/**
		@name         : 取得本地服务器在中心服务器上的ID
		@brief        :
		*/
		CSID  GetLocalServerCSID();

		/**
		@name         : 取得网络上所有服务器的个数
		@brief        :
		*/
		DWORD GetAllServersCount();

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
		unsafe DWORD GetServerList(ServerData ** serverPtrArray,DWORD dwArratSize,CGID GroupFilter,WORD wTypeFilter);

		/**
		@name                : 取得服务器信息
		@brief               :
		@param  ServerID     : 服务器ID
		@return              : 如果目标服务器存在则返回服务器信息,否则返回0
		*/
		ServerData GetServerInfo(CSID ServerID);

		/**
		@name                : 查找场景服务器
		@brief               :
		@param    groupID    : 区ID
		@param    dwMapID    : 地图ID
		@param    x          : 位置 (因为同一个地图可能由多个服务器共同维护,所以还需要传入位置)
		@param    y          : 位置
		@return              :
		*/
		ServerData FindZoneServer(CGID groupID,DWORD wMapID,DWORD x,DWORD y);

		/**
		@name                : 向某个服务器发送一条消息
		@brief               : 服务器之间的消息转发
		@param DestServerID  : 目标服务器ID
		@param pMessage      : 消息内容
		@param wMessageLen   : 消息长度
		@return              : 是否成功发到了中心服务器
		*/
		bool  PostMessage(CSID DestServerID,ibuffer msg);

		/**
		@name                : 向多个服务器广播消息
		@brief               :
		@param DestServerArray:目标服务器数组
		@param wDestServerCount:目标服务器个数
		@param pMessage      : 消息内容
		@param wMessageLen   : 消息长度
		@return              : 是否成功发到了中心服务器
		*/
		bool  BroadcastMessage(ibuffer id_buff,ibuffer msg);
};
