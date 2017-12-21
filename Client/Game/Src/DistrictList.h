/*******************************************************************
** 文件名:	DistrictList.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 版  本:	1.0
** 描  述:	游戏分区列表
** 应  用:		

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "Singleton.h"
#include "tinyxml.h"
#include <string>
#include <vector>
#include <map>
#include <list>
#include "IDistrictListClient.h"
#include "IClientGlobal.h"
#include "GameIni.h"
#include "IEntityClient.h"
#include "dns.h"
#include "NetServiceProvider.h"

using rkt::Singleton;
/// 分区定义文件
#define  DISTRICT_LIST_FILE     "serverlist.xml"




/**
@name : 游戏分区列表
@brief: 
*/
class DistrictList : public IGameViewEventHandler, public IDistrictListClient,public Singleton<DistrictList>
{

public:
	// 一个网关IP,端口
	struct IpAndPort
	{
		std::string		m_ip;		// 服务器ip或域名
		unsigned short  m_port;     // 服务器端口
		int				m_isp;		// ip所属网络类型

		IpAndPort()
		{
			m_ip = "";
			m_port = 0;
			m_isp = 0;
		};

		IpAndPort(const std::string& ip, unsigned short port, int isp):m_ip(ip),m_port(port),m_isp(isp){}
	};
	typedef std::vector<IpAndPort> IP_INFO; // 所有一个网关IP,端口列表

	struct ServerPort
	{		
		int     m_nDx;
		int     m_nLt;
		int     m_nCC;
		IP_INFO m_ipInfo;
		
		ServerPort()
		{
			m_nDx = 0;
			m_nLt = 0;
			m_ipInfo.clear();
		}

		void PushServerIpAndPort(const std::string& ip, unsigned short port, int isp)
		{
			if (isp < 0 || isp > 2) return;			
			// 去重
			for ( int i=0;i<m_ipInfo.size();++i )
			{
				if ( m_ipInfo[i].m_ip==ip && m_ipInfo[i].m_port==port && m_ipInfo[i].m_isp == isp)
				{
					return;
				}
			}
			if (isp == 0) m_nDx++;
			if (isp == 1) m_nLt++;
			if (isp == 2) m_nCC++;
			m_ipInfo.push_back(IpAndPort(ip,port,isp));
		}

		int Count(int isp)
		{
			if (isp == 0) return m_nDx;
			if (isp == 1) return m_nLt;
			if (isp == 2) return m_nCC;
			{
			}
			return 0;
		}
	};

	typedef std::map<std::string, ServerPort> SERVER_PORT_LIST;

	// 一个游戏世界区的等信息
	struct Server
	{
		DWORD			m_nID;		  // 服务器索引ID
		DWORD			m_nWorldID;	  // 游戏世界ID
		DWORD           m_latencty;   // 网络延迟
		DWORD			m_state;      // 状态 0 良好 1 繁忙 2 爆满 3 维护
		DWORD           m_recommend;  // 推荐值 1:推荐 0:无
		DWORD			m_dwHid;      // 隐藏服务器名称
		DWORD           m_support_udp;// 是否支持UDP

		std::string		m_name;		  // 服务器名称
		std::string		m_vername;	  // 服务器版本
		std::string		m_vershow;	  // 服务器版本显示
		std::string     m_desc;       // 服务器描述
		std::string     m_announce;   // 服务器公告
		std::string     m_mainten;    // 维护信息
		
		IP_INFO         m_ipInfo;   // 可用的服务ip，port
		IpAndPort       m_timeServerIpInfo; //时间服务器ip

		// 存储服务器ID端口
		void PushServerIpAndPort(const std::string& ip, unsigned short port, int isp)
		{
			// 去重
			for ( int i=0;i<m_ipInfo.size();++i )
			{
				if ( m_ipInfo[i].m_ip==ip && m_ipInfo[i].m_port==port && m_ipInfo[i].m_isp == isp)
				{
					return;
				}
			}

			m_ipInfo.push_back(IpAndPort(ip,port,isp));
		}
	};

	typedef std::vector<Server> SERVER_LIST;
	// 一个版本分类,如:开测区,压测区
	struct District
	{
		std::string     m_name;       // 游戏世界区名
		SERVER_LIST     m_serverList; // 服务器列表 
	};

	typedef std::vector<District>		DISTRICT_LIST;
	// 一个分组别显示名,如:电信一区,联通三区
	struct DistrictGroup
	{
		std::string     m_name;				// 区组名
		DISTRICT_LIST   m_districtList;		// 区列表 
	};

	// 所有分组别列表
	typedef  std::vector<DistrictGroup>	DISTRICT_TABLE;

	// 服务器索引列表,以索引ID为关键字
	typedef std::map<int,ServerIndexInfo> TMAP_ServerIndexList;

	// 服务器索引列表
	typedef std::list<ServerIndexInfo> TLIST_ServerIndexList;

	// 所有分组别列表
	DISTRICT_TABLE	m_districtTable;

	// 服务器索引列表
	TMAP_ServerIndexList m_mapServerIndex;

	// 服务器入口列表
	SERVER_PORT_LIST	m_mapServerPort;
	// 最近登陆的服务器列表
	TLIST_ServerIndexList m_lastEnterServerList;

	DWORD	m_dwIDInfoBufIndex;		// 用ID取信息是保存数据用当前序号
	SClientGameServerInfo m_getIDGameServerInfo[10];	// 用ID取信息是保存数据用
	SClientGameServerInfo m_getGameServerInfo;	// 列表取信息是保存数据用
	SClientGameGroupInfo	m_getGameGroupInfo;	// 取组信息保存数据用

	int m_nSelectedServer;		// 选中的区ID
	ServerIndexInfo		m_selectedServer;		// 当前游戏区信息

public:

	DistrictList();

	virtual ~DistrictList();

	virtual bool onViewEvent(int32 eventID, int nParam, const char * strParam, void * ptrParam);
	
	//////////////// IDistrictListClient //////////////////////////////////////////////////////////
	virtual bool Initialize();

	virtual void Close();

	virtual const SClientGameServerInfo * GetGameServerInfoByID(DWORD nIndex);

	virtual const SClientGameServerInfo *GetGameServerInfoByWorldID(DWORD dwWorldID);

	virtual const SClientGameServerInfo * GetGameServerInfo(DWORD nGroup,DWORD nDistrict,DWORD nServer);

	//  按大区和游戏名取得游戏世界区的信息 szAreaName:大区名,如:电信一区,szWorldName:游戏区名,bAutoMatch:如精确找不到就只按游戏区名找,并按用户网络找到匹配游戏区
	virtual const SClientGameServerInfo * GetGameServerInfoByName(const char * szAreaName,const char * szWorldName,bool bAutoMatch=true);

	//  取得推荐的游戏世界区的信息
	virtual const SClientGameServerInfo * GetRecommendGameServerInfo();

	//  取得当前的游戏世界区的信息
	virtual const SClientGameServerInfo * GetSelectGameServerInfo();

	virtual const char* GetWorldNameByID(DWORD dwWorldID);

	//  设定当前游戏世界区的信息
	virtual bool SetSelectGameServerInfo(DWORD nGroup,DWORD nDistrict,DWORD nServer,bool bSendEvent=true);

	// 进入游戏区事件处理
	virtual void OnEnterServer(DWORD nGroup,DWORD nDistrict,DWORD nServer);

	// 游戏区名是否和输入的关键字匹配,包括汉字首字母匹配
	virtual bool IsKeyWordMatchServer(DWORD nGroup,DWORD nDistrict,DWORD nServer,const char * szKeyWord);

	virtual DWORD GetMyISP();

	virtual void SetMyISP(DWORD dwISP);
	//////////////////////////////////////////////////////////////////////////

private:

	// 当加载服务器列表成功
	void OnServerListLoad();

	// 跟据当前选中区信息更新一下ini配置
	bool UpdateSelectServerIni(bool isSelectArea = false);

	// 加入服务器到最近登录
	bool AddLastEnterServerList(ServerIndexInfo &addNode);

	// 加载最后登录过的服务器 bIsLoad:false 保存,true:为加载
	bool LoadLastEnterServerList(bool bIsLoad = true);

	// 取一个临时ID取信息是保存数据用的内存
	SClientGameServerInfo & getBufIDGameServerInfo();

	//  按索引信息取得游戏世界区的信息
	bool GetGameServerInfoNode(ServerIndexInfo &node, SClientGameServerInfo &retNode);

	void CheckGameVersionID();

	void UpdateDistrictList();

	void UpdateCurrentDistrictInfo();

	void SendLastEnterDistrictList();

	bool Load();

	bool Load( const char * pContent,DWORD dwLen );

	bool Load( TiXmlDocument & doc );

	// 安全解析XML字符串
	bool SafeParseXML( TiXmlDocument & doc,const char * p,DWORD dwLen);

	/**  取得用分号分开的数据串，存入数组
	@param   int * pArray: 数组
	@param   int nArrayLen:数组长度
	@param	char * szStringBuf : 分析的字串
	@param   int nBufLen:字串Buffer长度
	@return  int :个数
	*/
	int GetStringIntArray(int * pArray, int nArrayLen, const char * szStringBuf, int nBufLen);

	/// 获取整数    
	virtual uint UpdateIni_getInt(const char* section, const char* key, int def = 0);
	/// 设置整数    
	virtual bool UpdateIni_setInt(const char* section, const char* key, int value);

	/// 获取字符串
	virtual std::string UpdateIni_getString(const char* section, const char* key, const char* def = 0);
	/// 设置字符串
	virtual bool UpdateIni_setString(const char* section, const char* key, const char* value);
};


//#define YZ_CHECK_DOMAIN "logintest.q1.com"
//#define YZ_DOMAIN_IP_DX "183.60.126.132"
//#define YZ_DOMAIN_IP_LT "163.177.24.110"
#define WEB_SERVICE_URL			"http://update1.yz.q1.com/service3.xml"
#define WEB_SERVERLIST_URL		"http://update1.yz.q1.com/down/serverlist.xml"
#define WEB_SERVICE_BACKUP		"http://61.142.250.151/update/service.xml"

/**
域名解析器，查询玩家网络状况
*/
class NetSniffer : public Singleton<NetSniffer>
{
public:

    
	NetSniffer()
	{
		m_bQuery = false;
		m_nIsp = NET_PROVIDER_YD;
	}

    void setISP(int nISP)
    {
        m_nIsp = nISP;
    }

	int GetCurNetClass() {return m_nIsp;}

    /** 查询玩家的网络供应商
    */
    bool queryNetworkServiceProvider()
    {
        return NetworkSerivceProviderQuery( NetSniffer::OnNetworkServiceProviderQueryCallback );
    }

    bool cancelQuerynetworkSerivceProvider()
    {
        return NetworkSerivceProviderCancelQuery();
    }

    // 返回网络服务供应商信息
    static void OnNetworkServiceProviderQueryCallback(int nServiceType)
    {
        NetSniffer::getInstance().setISP(nServiceType);
        //// 名字
        //switch(nServiceType)
        //{
        //case NET_PROVIDER_DX:            // 电信网络
        //    {
        //        sstrcpyn(result.szName, a2utf8("电信"), sizeof(result.szName));
        //    }
        //    break;
        //case NET_PROVIDER_LT:            // 联通网络
        //    {
        //        sstrcpyn(result.szName, a2utf8("联通"), sizeof(result.szName));
        //    }
        //    break;
        //case NET_PROVIDER_YD:            // 移动网络
        //    {
        //        sstrcpyn(result.szName, a2utf8("移动"), sizeof(result.szName));
        //    }
        //    break;

    }

    bool QueryDns()
    {
        if (m_bQuery) return true;
        m_bQuery = queryNetworkServiceProvider();	
        return m_bQuery;
    }

    void CancelQuery()
    {
        cancelQuerynetworkSerivceProvider();
    }

private:
	bool m_bQuery;	
	int  m_nIsp;
};