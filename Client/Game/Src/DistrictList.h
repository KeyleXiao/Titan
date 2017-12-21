/*******************************************************************
** �ļ���:	DistrictList.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	������ (sww8@163.com)
** ��  ��:	1.0
** ��  ��:	��Ϸ�����б�
** Ӧ  ��:		

**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
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
/// ���������ļ�
#define  DISTRICT_LIST_FILE     "serverlist.xml"




/**
@name : ��Ϸ�����б�
@brief: 
*/
class DistrictList : public IGameViewEventHandler, public IDistrictListClient,public Singleton<DistrictList>
{

public:
	// һ������IP,�˿�
	struct IpAndPort
	{
		std::string		m_ip;		// ������ip������
		unsigned short  m_port;     // �������˿�
		int				m_isp;		// ip������������

		IpAndPort()
		{
			m_ip = "";
			m_port = 0;
			m_isp = 0;
		};

		IpAndPort(const std::string& ip, unsigned short port, int isp):m_ip(ip),m_port(port),m_isp(isp){}
	};
	typedef std::vector<IpAndPort> IP_INFO; // ����һ������IP,�˿��б�

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
			// ȥ��
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

	// һ����Ϸ�������ĵ���Ϣ
	struct Server
	{
		DWORD			m_nID;		  // ����������ID
		DWORD			m_nWorldID;	  // ��Ϸ����ID
		DWORD           m_latencty;   // �����ӳ�
		DWORD			m_state;      // ״̬ 0 ���� 1 ��æ 2 ���� 3 ά��
		DWORD           m_recommend;  // �Ƽ�ֵ 1:�Ƽ� 0:��
		DWORD			m_dwHid;      // ���ط���������
		DWORD           m_support_udp;// �Ƿ�֧��UDP

		std::string		m_name;		  // ����������
		std::string		m_vername;	  // �������汾
		std::string		m_vershow;	  // �������汾��ʾ
		std::string     m_desc;       // ����������
		std::string     m_announce;   // ����������
		std::string     m_mainten;    // ά����Ϣ
		
		IP_INFO         m_ipInfo;   // ���õķ���ip��port
		IpAndPort       m_timeServerIpInfo; //ʱ�������ip

		// �洢������ID�˿�
		void PushServerIpAndPort(const std::string& ip, unsigned short port, int isp)
		{
			// ȥ��
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
	// һ���汾����,��:������,ѹ����
	struct District
	{
		std::string     m_name;       // ��Ϸ��������
		SERVER_LIST     m_serverList; // �������б� 
	};

	typedef std::vector<District>		DISTRICT_LIST;
	// һ���������ʾ��,��:����һ��,��ͨ����
	struct DistrictGroup
	{
		std::string     m_name;				// ������
		DISTRICT_LIST   m_districtList;		// ���б� 
	};

	// ���з�����б�
	typedef  std::vector<DistrictGroup>	DISTRICT_TABLE;

	// �����������б�,������IDΪ�ؼ���
	typedef std::map<int,ServerIndexInfo> TMAP_ServerIndexList;

	// �����������б�
	typedef std::list<ServerIndexInfo> TLIST_ServerIndexList;

	// ���з�����б�
	DISTRICT_TABLE	m_districtTable;

	// �����������б�
	TMAP_ServerIndexList m_mapServerIndex;

	// ����������б�
	SERVER_PORT_LIST	m_mapServerPort;
	// �����½�ķ������б�
	TLIST_ServerIndexList m_lastEnterServerList;

	DWORD	m_dwIDInfoBufIndex;		// ��IDȡ��Ϣ�Ǳ��������õ�ǰ���
	SClientGameServerInfo m_getIDGameServerInfo[10];	// ��IDȡ��Ϣ�Ǳ���������
	SClientGameServerInfo m_getGameServerInfo;	// �б�ȡ��Ϣ�Ǳ���������
	SClientGameGroupInfo	m_getGameGroupInfo;	// ȡ����Ϣ����������

	int m_nSelectedServer;		// ѡ�е���ID
	ServerIndexInfo		m_selectedServer;		// ��ǰ��Ϸ����Ϣ

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

	//  ����������Ϸ��ȡ����Ϸ����������Ϣ szAreaName:������,��:����һ��,szWorldName:��Ϸ����,bAutoMatch:�羫ȷ�Ҳ�����ֻ����Ϸ������,�����û������ҵ�ƥ����Ϸ��
	virtual const SClientGameServerInfo * GetGameServerInfoByName(const char * szAreaName,const char * szWorldName,bool bAutoMatch=true);

	//  ȡ���Ƽ�����Ϸ����������Ϣ
	virtual const SClientGameServerInfo * GetRecommendGameServerInfo();

	//  ȡ�õ�ǰ����Ϸ����������Ϣ
	virtual const SClientGameServerInfo * GetSelectGameServerInfo();

	virtual const char* GetWorldNameByID(DWORD dwWorldID);

	//  �趨��ǰ��Ϸ����������Ϣ
	virtual bool SetSelectGameServerInfo(DWORD nGroup,DWORD nDistrict,DWORD nServer,bool bSendEvent=true);

	// ������Ϸ���¼�����
	virtual void OnEnterServer(DWORD nGroup,DWORD nDistrict,DWORD nServer);

	// ��Ϸ�����Ƿ������Ĺؼ���ƥ��,������������ĸƥ��
	virtual bool IsKeyWordMatchServer(DWORD nGroup,DWORD nDistrict,DWORD nServer,const char * szKeyWord);

	virtual DWORD GetMyISP();

	virtual void SetMyISP(DWORD dwISP);
	//////////////////////////////////////////////////////////////////////////

private:

	// �����ط������б�ɹ�
	void OnServerListLoad();

	// ���ݵ�ǰѡ������Ϣ����һ��ini����
	bool UpdateSelectServerIni(bool isSelectArea = false);

	// ����������������¼
	bool AddLastEnterServerList(ServerIndexInfo &addNode);

	// ��������¼���ķ����� bIsLoad:false ����,true:Ϊ����
	bool LoadLastEnterServerList(bool bIsLoad = true);

	// ȡһ����ʱIDȡ��Ϣ�Ǳ��������õ��ڴ�
	SClientGameServerInfo & getBufIDGameServerInfo();

	//  ��������Ϣȡ����Ϸ����������Ϣ
	bool GetGameServerInfoNode(ServerIndexInfo &node, SClientGameServerInfo &retNode);

	void CheckGameVersionID();

	void UpdateDistrictList();

	void UpdateCurrentDistrictInfo();

	void SendLastEnterDistrictList();

	bool Load();

	bool Load( const char * pContent,DWORD dwLen );

	bool Load( TiXmlDocument & doc );

	// ��ȫ����XML�ַ���
	bool SafeParseXML( TiXmlDocument & doc,const char * p,DWORD dwLen);

	/**  ȡ���÷ֺŷֿ������ݴ�����������
	@param   int * pArray: ����
	@param   int nArrayLen:���鳤��
	@param	char * szStringBuf : �������ִ�
	@param   int nBufLen:�ִ�Buffer����
	@return  int :����
	*/
	int GetStringIntArray(int * pArray, int nArrayLen, const char * szStringBuf, int nBufLen);

	/// ��ȡ����    
	virtual uint UpdateIni_getInt(const char* section, const char* key, int def = 0);
	/// ��������    
	virtual bool UpdateIni_setInt(const char* section, const char* key, int value);

	/// ��ȡ�ַ���
	virtual std::string UpdateIni_getString(const char* section, const char* key, const char* def = 0);
	/// �����ַ���
	virtual bool UpdateIni_setString(const char* section, const char* key, const char* value);
};


//#define YZ_CHECK_DOMAIN "logintest.q1.com"
//#define YZ_DOMAIN_IP_DX "183.60.126.132"
//#define YZ_DOMAIN_IP_LT "163.177.24.110"
#define WEB_SERVICE_URL			"http://update1.yz.q1.com/service3.xml"
#define WEB_SERVERLIST_URL		"http://update1.yz.q1.com/down/serverlist.xml"
#define WEB_SERVICE_BACKUP		"http://61.142.250.151/update/service.xml"

/**
��������������ѯ�������״��
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

    /** ��ѯ��ҵ����繩Ӧ��
    */
    bool queryNetworkServiceProvider()
    {
        return NetworkSerivceProviderQuery( NetSniffer::OnNetworkServiceProviderQueryCallback );
    }

    bool cancelQuerynetworkSerivceProvider()
    {
        return NetworkSerivceProviderCancelQuery();
    }

    // �����������Ӧ����Ϣ
    static void OnNetworkServiceProviderQueryCallback(int nServiceType)
    {
        NetSniffer::getInstance().setISP(nServiceType);
        //// ����
        //switch(nServiceType)
        //{
        //case NET_PROVIDER_DX:            // ��������
        //    {
        //        sstrcpyn(result.szName, a2utf8("����"), sizeof(result.szName));
        //    }
        //    break;
        //case NET_PROVIDER_LT:            // ��ͨ����
        //    {
        //        sstrcpyn(result.szName, a2utf8("��ͨ"), sizeof(result.szName));
        //    }
        //    break;
        //case NET_PROVIDER_YD:            // �ƶ�����
        //    {
        //        sstrcpyn(result.szName, a2utf8("�ƶ�"), sizeof(result.szName));
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