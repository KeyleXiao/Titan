/*******************************************************************
** �ļ���:	DistrictList.cpp
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

#include "stdafx.h"
#include "DistrictList.h"
#include "GameIni.h"
#include "ILoginServerProtocol.h"
#include "IKeyWordClient.h"
#include "ShellAPI.h"
#include <tlhelp32.h>
#include "ISchemeCenter.h"
#include <sstream>
#include "Event_ManagedDef.h"
#include "EventDef.h"

//////////////////////////////////////////////////////////////////////////////
#define  DISTRICT_FILE_ROOT            "Services"		      // xml�����ļ��еĸ��ڵ�
#define  DISTRICT_GROUP_NAME           "DistrictGroup"        // ����Ⱥ����
#define  DISTRICT_NODE_NAME            "District"             // �����ڵ���
#define  SERVER_NODE_NAME              "Server"               // �������ڵ���
#define  ATTRIBUTE_NAME_NAME           "name"                 // name����
#define  ATTRIBUTE_NAME_SHOWNAME       "showname"             // ��ʾ������
#define  ATTRIBUTE_NAME_VERNAME        "vername"              // �汾name����
#define  ATTRIBUTE_NAME_VERSHOWNAME    "vershow"              // �汾��ʾ������
#define  ATTRIBUTE_NAME_DESC           "desc"                 // desc����
#define  ATTRIBUTE_NAME_ANNOUNCE       "announce"             // ����������
#define  ATTRIBUTE_NAME_MAINTEN        "mainten"              // ά����Ϣ
#define  ATTRIBUTE_NAME_ID             "id"                   // ID
#define  ATTRIBUTE_NAME_IP             "ip"                   // ip����
#define  ATTRIBUTE_NAME_PORT           "port"                 // port����
#define  ATTRIBUTE_NAME_LATENCY        "latency"              // Latency����
#define  ATTRIBUTE_NAME_STATE          "state"                // state����
#define  ATTRIBUTE_NAME_RECOMMEND      "recommend"            // recommend����
#define  ATTRIBUTE_NAME_VERMODE	       "vermode"              // vermode����
#define  ATTRIBUTE_NAME_ISP            "isp"                  // isp����,ָ���ǵ��Ż�����ͨ         
#define  MIRROR_NODE_NAME              "Mirror"               // ���������
#define  ATTRIBUTE_TIMESERVER_IP       "timeServerIp"         // ʱ�������IP
#define  ATTRIBUTE_TIMESERVER_PORT     "timeServerPort"       // ʱ��������˿�
#define  ATTRIBUTE_NAME_HID            "hid"                  // �б������ط�������
#define  ATTRIBUTE_SUPPORT_UDP         "udp"             // �Ƿ�֧��UDP


DistrictList::DistrictList() : m_nSelectedServer(0)
{
	m_dwIDInfoBufIndex	= 0;		            // ��IDȡ��Ϣ�Ǳ��������õ�ǰ���
}


 DistrictList::~DistrictList()
 {
 }




bool DistrictList::onViewEvent(int32 eventID, int nParam, const char * strParam, void * ptrParam) 
{
	switch (eventID)
	{
	    case GVIEWCMD_REQUEST_DISTRICT_LIST:
	    {
			
			CheckGameVersionID();
			
			UpdateDistrictList();

			UpdateCurrentDistrictInfo();
	    }
	    break;
		case GVIEWCMD_REQUEST_SELECT_SERVER: 
		{
			if (ptrParam == NULL)
			{
				return false;
			}
			gameview_select_server* pCmd = (gameview_select_server*)ptrParam;
			SetSelectGameServerInfo(pCmd->nGroup, pCmd->nDistrict, pCmd->nServer);
		}
		break;

		case GVIEWCMD_REQUEST_LASTENTER_DISTRICT_LIST: 
		{
			SendLastEnterDistrictList();
		}
		break;

	    default:
		    return false;
	}
	return true;
} 


// ��ʼ����Ϸ�����б�
bool DistrictList::Initialize()
{
	LoadLastEnterServerList(true);

	// ���뱾�ط������б�
	if (Load())
	{
		// �����ط������б�ɹ�
		OnServerListLoad();
	}
	
	//��ʾ����Ϣ
	IEntityClient *pEntityClient = gClientGlobal->getEntityClient();
	if (pEntityClient != NULL)
	{
		pEntityClient->registerGameViewEventHandler(GVIEWCMD_REQUEST_DISTRICT_LIST, this);
		pEntityClient->registerGameViewEventHandler(GVIEWCMD_REQUEST_SELECT_SERVER, this);
		pEntityClient->registerGameViewEventHandler(GVIEWCMD_REQUEST_LASTENTER_DISTRICT_LIST, this);
	}
	return true;
}


// �ر�
void DistrictList::Close()
{
	//��ʾ����Ϣ
	IEntityClient *pEntityClient = gClientGlobal->getEntityClient();
	if (pEntityClient == NULL)
		return;

	pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_REQUEST_DISTRICT_LIST);
	pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_REQUEST_SELECT_SERVER);
	pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_REQUEST_LASTENTER_DISTRICT_LIST);
}




//////////////////////////////////////////////////////////////////////////
void DistrictList::CheckGameVersionID() 
{
	DWORD dwVersion = (DWORD)UpdateIni::getInstance().getInt("Login", "version", 0);
	TraceLn("Server Latest Game Version��" << dwVersion);

	gamelogic_game_version version;
	version.nVersion = dwVersion;

	obuf obVersionData;
	obVersionData.push_back(&version, sizeof(gamelogic_game_version));

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CHECK_GAME_VERSION, 0, 0, obVersionData.data(), obVersionData.size());
}


void DistrictList::UpdateDistrictList() 
{
	gameview_district_info_list_head dataHead;
	dataHead.nCount = 0;

	obuf4096 obNode;
	for (TMAP_ServerIndexList::iterator iter = m_mapServerIndex.begin(); iter != m_mapServerIndex.end(); iter++)
	{
		ServerIndexInfo &IndexNode = iter->second;
		SClientGameServerInfo &serverInfo = getBufIDGameServerInfo();
		if (GetGameServerInfoNode(IndexNode, serverInfo))
		{
			if (serverInfo.dwHid > 0)
				continue;

			gamelogic_district_info sendNode;
			sendNode.nWorldID = serverInfo.nWorldID;
			sendNode.nGroup = serverInfo.nGroup;
			sendNode.nDistrict = serverInfo.nDistrict;
			sendNode.nServer = serverInfo.nServer;
			sendNode.nLatencty = serverInfo.nLatencty;
			sendNode.nRecommend = serverInfo.nRecommend;
			sendNode.nHid = serverInfo.dwHid;
			sendNode.nState = serverInfo.nState;

			sstrcpyn(sendNode.szName, a2utf8(serverInfo.szName), sizeof(sendNode.szName));
			sstrcpyn(sendNode.szGroupName, a2utf8(serverInfo.szGroupName), sizeof(sendNode.szGroupName));
			sstrcpyn(sendNode.szDesc, a2utf8(serverInfo.szDesc), sizeof(sendNode.szDesc));

			++dataHead.nCount;
			obNode.push_back(&sendNode, sizeof(sendNode));


		}
	}

	obuf4096 obData;
	obData.push_back(&dataHead, sizeof(dataHead));
	obData.push_back(obNode.data(), obNode.size());

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_RECV_DISTRICT_LIST, 0, 0, obData.data(), obData.size());
}


void DistrictList::UpdateCurrentDistrictInfo() 
{
	//�������ڴ�����Ϣ
	SClientGameServerInfo &currentServerInfo = getBufIDGameServerInfo();
	if (GetGameServerInfoNode(m_selectedServer, currentServerInfo))
	{
		gamelogic_district_info currentInfo;
		currentInfo.nWorldID = currentServerInfo.nWorldID;
		currentInfo.nGroup = currentServerInfo.nGroup;
		currentInfo.nDistrict = currentServerInfo.nDistrict;
		currentInfo.nServer = currentServerInfo.nServer;
		currentInfo.nLatencty = currentServerInfo.nLatencty;
		currentInfo.nRecommend = currentServerInfo.nRecommend;
		currentInfo.nHid = currentServerInfo.dwHid;
		currentServerInfo.nState = currentServerInfo.nState;

		sstrcpyn(currentInfo.szName, a2utf8(currentServerInfo.szName), sizeof(currentInfo.szName));
		sstrcpyn(currentInfo.szGroupName, a2utf8(currentServerInfo.szGroupName), sizeof(currentInfo.szGroupName));
		sstrcpyn(currentInfo.szDesc, a2utf8(currentServerInfo.szDesc), sizeof(currentInfo.szDesc));

		obuf obCurrentServerData;
		obCurrentServerData.push_back(&currentInfo, sizeof(currentInfo));

		gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_RECV_SELECT_SERVER_RESULT, 0, 0, obCurrentServerData.data(), obCurrentServerData.size());
	}
}


void DistrictList::SendLastEnterDistrictList() 
{
	obuf obNode;

	gamelogic_lastEnter_district_info_head head;
	head.nCount = m_lastEnterServerList.size();

	obNode.push_back(&head, sizeof(head));

	for (TLIST_ServerIndexList::iterator it = m_lastEnterServerList.begin(); it != m_lastEnterServerList.end(); it++)
	{
		ServerIndexInfo &node = (*it);
		
		gamelogic_lastEnter_district_info info;
		info.nGroup = node.nGroup;
		info.nDistrict = node.nDistrict;
		info.nServer = node.nServer;

		obNode.push_back(&info, sizeof(info));
	}
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_RECV_LASTENTER_DISTRICT_LIST, 0, 0, obNode.data(), obNode.size());
}
//////////////////////////////////////////////////////////////////////////




bool DistrictList::Load()
{
	TiXmlDocument doc;
	std::string fullpath = UpdateIni::getInstance().GetGamePath();
	fullpath += "\\";
	fullpath += DISTRICT_LIST_FILE;

	TraceLn(a2utf8("���ر��ط������б��ļ�:")<<fullpath.c_str());

	if ( !doc.LoadFile(fullpath.c_str()) )
	{
		WarningLn("���ر��ط������б��ļ�ʧ��!" << fullpath.c_str());
		return false;
	}

	return Load(doc);
}

bool DistrictList::Load( const char * pContent,DWORD dwLen )
{
	TiXmlDocument doc;
	SafeParseXML( doc,pContent,dwLen );

	return Load(doc);
}

bool DistrictList::Load( TiXmlDocument & doc )
{
	TiXmlElement * root = doc.FirstChildElement(DISTRICT_FILE_ROOT);
	if ( root==0 )
	{
		ErrorLn("invalid district file.[lack root node]" << DISTRICT_LIST_FILE);
		return false;
	}

	// ������м�¼
	m_districtTable.clear();

	// ������м�¼�����б�
	m_mapServerIndex.clear();

	// �����������������б�
	m_mapServerPort.clear();
	
	// ��������ͨ���ǵ���
	DWORD nMyIsp = GetMyISP();

	TiXmlElement * group = root->FirstChildElement(DISTRICT_GROUP_NAME);
	for( ;group;group=group->NextSiblingElement(DISTRICT_GROUP_NAME))
	{
		DistrictGroup districtGroup;

		const char * str = group->Attribute(ATTRIBUTE_NAME_NAME);
		if (str==0)
		{
			ErrorLn("invalid district file.[lack base attribute 'name']" << DISTRICT_LIST_FILE);
			return false;
		}

		districtGroup.m_name = str;

		TiXmlElement * pDistrict = group->FirstChildElement(DISTRICT_NODE_NAME);
		for ( ;pDistrict;pDistrict=pDistrict->NextSiblingElement(DISTRICT_NODE_NAME))
		{
			District district;

			const char * str = pDistrict->Attribute(ATTRIBUTE_NAME_NAME);
			if (str==0)
			{
				ErrorLn("invalid district file.[lack base attribute 'name']" << DISTRICT_LIST_FILE);
				return false;
			}

			district.m_name = str;

			TiXmlElement * pServer = pDistrict->FirstChildElement(SERVER_NODE_NAME);
			for ( ;pServer;pServer=pServer->NextSiblingElement(SERVER_NODE_NAME))
			{
				Server server;

				int nWorldID = 0;	// ��Ϸ����ID
				pServer->QueryIntAttribute(ATTRIBUTE_NAME_ID, &nWorldID);

				const char * name = pServer->Attribute(ATTRIBUTE_NAME_NAME);
				if (name==0)
				{
					ErrorLn("invalid district file.[lack base attribute 'name']" << DISTRICT_LIST_FILE);
					return false;
				}
				const char * strVerName = pServer->Attribute(ATTRIBUTE_NAME_VERNAME);
				if (strVerName!=0 && strlen(strVerName)>0)
				{
					server.m_vername = strVerName;
				}
				else
				{
					server.m_vername = district.m_name;		// �Ҳ����÷�����,����ǰ�İ汾
				}

				const char * strVerShowName = pServer->Attribute(ATTRIBUTE_NAME_VERSHOWNAME);
				if (strVerShowName!=0 && strlen(strVerShowName)>0)
				{
					server.m_vershow = strVerShowName;
				}
				else
				{
					server.m_vershow = server.m_vername;
				}

				const char * desc = pServer->Attribute(ATTRIBUTE_NAME_DESC);
				if ( desc==0 )
				{
					desc = "";
				}

				const char * announce = pServer->Attribute(ATTRIBUTE_NAME_ANNOUNCE);
				if ( announce==0 )
				{
					announce = "";
				}
	
				const char * mainten = pServer->Attribute(ATTRIBUTE_NAME_MAINTEN);
				if ( mainten==0 )
				{
					mainten = "";
				}

				std::string serverkey;
				StringHelper::format(serverkey,"%d_%s",nWorldID,name);

				SERVER_PORT_LIST::iterator it = m_mapServerPort.find(serverkey);
				if (it == m_mapServerPort.end())
				{
					ServerPort portNode;
					m_mapServerPort[serverkey] = portNode;
					it = m_mapServerPort.find(serverkey);
				}

				// ���������
				TiXmlElement * pMirror = pServer->FirstChildElement(MIRROR_NODE_NAME);
				for ( ;pMirror; pMirror=pMirror->NextSiblingElement(MIRROR_NODE_NAME) )
				{
					const char * ip = pMirror->Attribute(ATTRIBUTE_NAME_IP);
					const char * port = pMirror->Attribute(ATTRIBUTE_NAME_PORT);
					int isp = 2;
					pMirror->QueryIntAttribute(ATTRIBUTE_NAME_ISP, &isp);

					if (ip && port)
					{
						int nGetInt = 0;
						vector<string> strList;
						StringHelper::split(strList, port, ';', " ");

						for (WORD i = 0; i < strList.size(); i++)
						{
							nGetInt = StringHelper::toInt(strList[i]);
							server.PushServerIpAndPort(ip, nGetInt, isp);
							it->second.PushServerIpAndPort(ip, nGetInt, isp);
						}
					}
				}

				const char * timeServerIp = pServer->Attribute(ATTRIBUTE_TIMESERVER_IP);
				if (timeServerIp == 0)
				{
					ErrorLn("invalid district file.[lack base attribute 'timeServerIP']" << DISTRICT_LIST_FILE);
					return false;
				}

				int timeServerPort = 0;
				pServer->QueryIntAttribute(ATTRIBUTE_TIMESERVER_PORT, &timeServerPort);

				int latencty = 0;
				pServer->QueryIntAttribute(ATTRIBUTE_NAME_LATENCY,&latencty);

				int recommend = 0;
				pServer->QueryIntAttribute(ATTRIBUTE_NAME_RECOMMEND, &recommend);

				int state = 0;
				pServer->QueryIntAttribute(ATTRIBUTE_NAME_STATE, &state);

				int dwHid = 0;
				pServer->QueryIntAttribute(ATTRIBUTE_NAME_HID,&dwHid);

				//int verMode = 0;
				//pServer->QueryIntAttribute(ATTRIBUTE_NAME_VERMODE,&verMode);

				int support_udp = 1;
				pServer->QueryIntAttribute(ATTRIBUTE_SUPPORT_UDP, &support_udp);
			
				server.m_nWorldID = nWorldID;
				server.m_name = name;
				server.m_desc = desc;
				server.m_announce = announce;
				server.m_mainten  = mainten;
				server.m_timeServerIpInfo.m_ip = timeServerIp;
				server.m_timeServerIpInfo.m_port = timeServerPort;
				server.m_latencty = latencty;
				server.m_recommend = recommend;
				server.m_state= state;
				server.m_dwHid = dwHid;
				server.m_support_udp = support_udp;

				district.m_serverList.push_back(server);
			}

			districtGroup.m_districtList.push_back(district);
		}

		m_districtTable.push_back(districtGroup);
	}

	// ���������������б�
	DWORD nServerIndex = 0;
	for (DWORD i=0;i<m_districtTable.size();i++)
	{
		DISTRICT_LIST &districtList = m_districtTable[i].m_districtList;
		for (DWORD j=0;j<districtList.size();j++)
		{
			SERVER_LIST &serverList = districtList[j].m_serverList;
			for (DWORD k=0;k<serverList.size();k++)
			{
				nServerIndex++;
				ServerIndexInfo addNode;
				addNode.nID			= nServerIndex;		// ����ID
				addNode.nGroup		= i;				// �������
				addNode.nDistrict	= j;				// �汾�������
				addNode.nServer		= k;				// ��Ϸ���������
				addNode.nNumber		= serverList[k].m_nWorldID;	// ��Ϸ����ID

				serverList[k].m_nID = nServerIndex;		// ����������ID

				m_mapServerIndex[nServerIndex] = addNode;
			}
		}
	}
	return true;
}

// ��ȫ����XML�ַ���
bool DistrictList::SafeParseXML( TiXmlDocument & doc,const char * p,DWORD dwLen)
{
	if ( dwLen==0 || p==0 )
		return false;

	char * pContent = (char*)p;

	// �������0��β���ַ����򴴽�һ����ʱ������
	if ( pContent[dwLen-1]!=0 )
	{
		pContent = (char *)malloc(dwLen+1);
		memcpy(pContent,p,dwLen);
		pContent[dwLen] = 0;
	}

	doc.Parse(pContent);

	if ( pContent!=p )
	{
		free( pContent );
		pContent = 0;
	}

	return true;
}

// ��������ͨ���ǵ���
DWORD DistrictList::GetMyISP()
{
	int isp = UpdateIni::getInstance().getInt("Network", "isp", 0);
	if (isp < 1 || isp > 3)  //����0Ϊ�Զ�ѡ��
	{
		isp = NetSniffer::getInstance().GetCurNetClass();
	}
	return isp;

}

// �趨��������ͨ���ǵ��� ��������ͨ���ǵ��� 0.����,1.��ͨ,2:�Զ��ж�����
void DistrictList::SetMyISP(DWORD dwISP)
{
	UpdateIni::getInstance().setInt("Network","isp",dwISP);
}


/// ��ȡ����    
uint DistrictList::UpdateIni_getInt(const char* section, const char* key, int def)
{
	if (section==NULL || key==NULL)
	{
		return 0;
	}
	return UpdateIni::getInstance().getInt(section,key,def);
}
/// ��������    
bool DistrictList::UpdateIni_setInt(const char* section, const char* key, int value)
{
	if (section==NULL || key==NULL)
	{
		return false;
	}
	return UpdateIni::getInstance().setInt(section,key,value);
}
/// ��ȡ�ַ���
std::string DistrictList::UpdateIni_getString(const char* section, const char* key, const char* def)
{
	if (section==NULL || key==NULL || def==NULL)
	{
		return "";
	}
	return UpdateIni::getInstance().getString(section,key,def);
}
/// �����ַ���
bool DistrictList::UpdateIni_setString(const char* section, const char* key, const char* value)
{
	if (section==NULL || key==NULL || value==NULL)
	{
		return false;
	}
	return UpdateIni::getInstance().setString(section,key,value);
}


//////////////// IClientDistrictList //////////////////////////////////////////////////////////

//  ������IDȡ����Ϸ����������Ϣ
const SClientGameServerInfo * DistrictList::GetGameServerInfoByID(DWORD nIndex)
{
	TMAP_ServerIndexList::iterator iter = m_mapServerIndex.find(nIndex);
	if (iter==m_mapServerIndex.end())
	{
		return NULL;
	}
	SClientGameServerInfo &bufIDGameServerInfo = getBufIDGameServerInfo();
	if (GetGameServerInfoNode(iter->second,bufIDGameServerInfo)==false)
	{
		return NULL;
	}
	return &bufIDGameServerInfo;
}

//  ����������Ϸ��ȡ����Ϸ����������Ϣ szAreaName:������,��:����һ��,szWorldName:��Ϸ����,bAutoMatch:�羫ȷ�Ҳ�����ֻ����Ϸ������,�����û������ҵ�ƥ����Ϸ��
const SClientGameServerInfo * DistrictList::GetGameServerInfoByName(const char * szAreaName,const char * szWorldName,bool bAutoMatch)
{
	std::string strAreaName = "";	// ������
	std::string strWorldName = "";	// ��Ϸ����

	if (szAreaName!=NULL)
	{
		strAreaName = szAreaName;
	}
	if (szWorldName!=NULL)
	{
		strWorldName = szWorldName;
	}

	//  ȡ�õ�ǰ����Ϸ����������Ϣ
	std::string strMyVer = "";	// ��ǰ�汾��
	const SClientGameServerInfo * pSelectServer = GetSelectGameServerInfo();
	if(pSelectServer!=NULL)
	{
		strMyVer = pSelectServer->szVerName;
	}

	DWORD nMyIsp = GetMyISP();	// ��������ͨ���ǵ��� 0.����,1.��ͨ,2:�Զ��ж�����

	std::string strIspName = "��";
	if (nMyIsp==1)
	{
		strIspName = "����";
	}
	else if(nMyIsp==2)
	{
		strIspName = "��ͨ";
	}
	else if (nMyIsp == 3)
	{
		strIspName = "�ƶ�";
	}

	// �ҵ����б�
	TMAP_ServerIndexList findList;		// ͬ��Ϸ������
	TMAP_ServerIndexList findgoodList;	// �Ƽ�
	ServerIndexInfo findnode;
	findnode.nID = 0;

	// ����ָ�����Ƶ���
	for (DWORD i=0;i<m_districtTable.size();i++)
	{
		DISTRICT_LIST &districtList = m_districtTable[i].m_districtList;
		for (DWORD j=0;j<districtList.size();j++)
		{
			SERVER_LIST &serverList = districtList[j].m_serverList;
			for (DWORD k=0;k<serverList.size();k++)
			{
				Server &serverNode = serverList[k]; 

				// ��Ϸ����
				if (strWorldName==serverNode.m_name || serverNode.m_recommend==1)
				{
					ServerIndexInfo node;
					node.nID		= serverNode.m_nID;			// ����ID
					node.nGroup		= i;		// �������
					node.nDistrict	= j;		// �汾�������
					node.nServer	= k;		// ��Ϸ���������
					node.nNumber	= 100000-serverNode.m_latencty;		// ���ȼ�

					// �������ȼ�
					std::string strGroupName = m_districtTable[i].m_name;
					if(strGroupName.find(strIspName) != std::string::npos)
					{
						node.nNumber += 100000;
					}
					// ͬһ���汾������
					std::string strVerName = serverNode.m_vername;
					if (strMyVer==strVerName)
					{
						node.nNumber += 10000;
					}

					// �ֿ��治ͬ���б�
					if (strWorldName==serverNode.m_name)
					{
						findList[serverNode.m_nID] = node;
					}
					else
					{
						findgoodList[serverNode.m_nID] = node;
					}

					// ����������,����ֻȡ��һ��
					if (strWorldName==serverNode.m_name && strAreaName==m_districtTable[i].m_name && findnode.nID==0)
					{
						findnode = node;
					}
				}
			}
		}
	}

	// �羫ȷ�Ҳ�����ֻ����Ϸ������,�����û������ҵ�ƥ����Ϸ��
	if (findnode.nID==0 && bAutoMatch)
	{
		if (findList.size()>0)
		{
			// ȡ���ȼ�����
			for (TMAP_ServerIndexList::iterator iter = findList.begin();iter!=findList.end(); iter++)
			{			
				if (iter->second.nNumber> findnode.nNumber)
				{
					findnode = iter->second;
				}
			}
		}
		// �����Ҳ���,�͸��Ƽ�
		if (findnode.nID==0 && findgoodList.size()>0)
		{
			// ȡ���ȼ�����
			for (TMAP_ServerIndexList::iterator iter = findgoodList.begin();iter!=findgoodList.end(); iter++)
			{			
				if (iter->second.nNumber> findnode.nNumber)
				{
					findnode = iter->second;
				}
			}
		}
	}
	SClientGameServerInfo &bufIDGameServerInfo = getBufIDGameServerInfo();
	if (GetGameServerInfoNode(findnode,bufIDGameServerInfo)==false)
	{
		return NULL;
	}
	return &bufIDGameServerInfo;
}

//  ȡ���Ƽ�����Ϸ����������Ϣ
const SClientGameServerInfo * DistrictList::GetRecommendGameServerInfo()
{
	//  ȡ�õ�ǰ����Ϸ����������Ϣ
	std::string strMyVer = "";	// ��ǰ�汾��
	const SClientGameServerInfo * pSelectServer = GetSelectGameServerInfo();
	if(pSelectServer!=NULL)
	{
		strMyVer = pSelectServer->szVerName;
	}

	DWORD nMyIsp = GetMyISP();	// ��������ͨ���ǵ��� 0.����,1.��ͨ,2:�Զ��ж�����

	std::string strIspName = "��";
	if (nMyIsp==1)
	{
		strIspName = "����";
	}
	else if(nMyIsp==2)
	{
		strIspName = "��ͨ";
	}
	else if (nMyIsp == 3)
	{
		strIspName = "�ƶ�";
	}

	// �ҵ����б�
	TMAP_ServerIndexList findList;

	// ����ָ�����Ƶ���
	for (DWORD i=0;i<m_districtTable.size();i++)
	{
		DISTRICT_LIST &districtList = m_districtTable[i].m_districtList;
		for (DWORD j=0;j<districtList.size();j++)
		{
			SERVER_LIST &serverList = districtList[j].m_serverList;
			for (DWORD k=0;k<serverList.size();k++)
			{
				Server &serverNode = serverList[k]; 
				// ���Ƽ�����
				if (serverNode.m_recommend==1)
				{
					ServerIndexInfo node;
					node.nID		= serverNode.m_nID;			// ����ID
					node.nGroup		= i;		// �������
					node.nDistrict	= j;		// �汾�������
					node.nServer	= k;		// ��Ϸ���������
					node.nNumber	= 100000-serverNode.m_latencty;		// ���ȼ�

					// �������ȼ�
					std::string strGroupName = m_districtTable[i].m_name;
					if(strGroupName.find(strIspName) != std::string::npos)
					{
						node.nNumber += 100000;
					}
					// ͬһ���汾������
					std::string strVerName = serverNode.m_vername;
					if (strMyVer==strVerName)
					{
						node.nNumber += 10000;
					}

					findList[serverNode.m_nID] = node;
				}
			}
		}
	}
	
	ServerIndexInfo node;
	node.nID = 1;
	node.nGroup = 0;
	node.nDistrict = 0;
	node.nServer = 0;
	node.nNumber = 0;

	if (findList.size()>0)
	{
		// ȡ���ȼ�����
		for (TMAP_ServerIndexList::iterator iter = findList.begin();iter!=findList.end(); iter++)
		{			
			if (iter->second.nNumber> node.nNumber)
			{
				node = iter->second;
			}
		}
	}
	SClientGameServerInfo &bufIDGameServerInfo = getBufIDGameServerInfo();
	if (GetGameServerInfoNode(node,bufIDGameServerInfo)==false)
	{
		return NULL;
	}
	return &bufIDGameServerInfo;
}

//  ȡ�õ�ǰ����Ϸ����������Ϣ
const SClientGameServerInfo * DistrictList::GetSelectGameServerInfo()
{
	SClientGameServerInfo &bufIDGameServerInfo = getBufIDGameServerInfo();
	if (GetGameServerInfoNode(m_selectedServer,bufIDGameServerInfo)==false)
	{
		return NULL;
	}
	return &bufIDGameServerInfo;
	
}

//  �趨��ǰ��Ϸ����������Ϣ
bool DistrictList::SetSelectGameServerInfo(DWORD nGroup,DWORD nDistrict,DWORD nServer,bool bSendEvent)
{
	if (nGroup>=m_districtTable.size())
	{
		WarningLn(a2utf8("�趨��ǰ��Ϸ����������Ϣʧ��! nGroup=")<<nGroup<<"nDistrict="<<nDistrict<<",nServer="<<nServer<<",Groupsize="<<m_districtTable.size());
		return false;
	}	

	DistrictGroup &groupNode = m_districtTable[nGroup];	// ����

	if ( nDistrict>=groupNode.m_districtList.size() )
	{
		WarningLn(a2utf8("�趨��ǰ��Ϸ����������Ϣʧ��! nGroup=")<<nGroup<<"nDistrict="<<nDistrict<<",nServer="<<nServer<<",Districtsize="<<groupNode.m_districtList.size());
		return false;
	}

	District &districtNode = groupNode.m_districtList[nDistrict];	// �汾����

	if ( nServer>=districtNode.m_serverList.size())
	{
		WarningLn(a2utf8("�趨��ǰ��Ϸ����������Ϣʧ��! nGroup=")<<nGroup<<"nDistrict="<<nDistrict<<",nServer="<<nServer<<",serversize="<<districtNode.m_serverList.size());
		return false;
	}

	Server &serverNode = districtNode.m_serverList[nServer];	// ��Ϸ����

	string strOldVerName;	// �ɵİ汾��

	SClientGameServerInfo &bufIDGameServerInfo = getBufIDGameServerInfo();
	if (GetGameServerInfoNode(m_selectedServer,bufIDGameServerInfo))
	{
		strOldVerName = bufIDGameServerInfo.szVerName;
	}

	// ԭ������Ϣ
	ServerIndexInfo oldInfo;
	oldInfo = m_selectedServer;

	string strNewVerName = serverNode.m_vername;	// �µİ汾��
	// �汾��������һ��,���汾
	if (strOldVerName.size()<2)
	{
		m_selectedServer.nNumber	= 0;	// ������ʶ 0.������game.exe ,-1.Ҫ��game.exe,���汾,-2.����game.exe
	}
	else if (strNewVerName==strOldVerName)
	{
		m_selectedServer.nNumber	= -2;	// ������ʶ 0.������game.exe ,-1.Ҫ��game.exe,���汾,-2.����game.exe
	}
	else
	{
		m_selectedServer.nNumber	= -1;	// ������ʶ 0.������game.exe ,-1.Ҫ��game.exe,���汾,-2.����game.exe
	}

	m_selectedServer.nID		= serverNode.m_nID;		// ����������ID
	m_selectedServer.nGroup		= nGroup;				// �������
	m_selectedServer.nDistrict	= nDistrict;			// �汾�������
	m_selectedServer.nServer	= nServer;				// ��Ϸ���������

	// ����һ�µ�ǰѡ������Ϣ,������һ��
	UpdateSelectServerIni(true);

	// ���¼�
	if(bSendEvent)
	{
		// Ĭ�����仯��,�·�����IP,�˿ڿɴ�config.ini��ȡ
		event_system_default_server_changed eventData;
		// ������������Ϣ
		eventData.nID			= m_selectedServer.nID;				// ����������ID
		eventData.nGroup		= m_selectedServer.nGroup;			// �������
		eventData.nDistrict		= m_selectedServer.nDistrict;		// �汾�������
		eventData.nServer		= m_selectedServer.nServer;			// ��Ϸ���������
		// ԭ����������Ϣ
		eventData.nID0			= oldInfo.nID;			// ����������ID
		eventData.nGroup0		= oldInfo.nGroup;		// �������
		eventData.nDistrict0	= oldInfo.nDistrict;	// �汾�������
		eventData.nServer0		= oldInfo.nServer;		// ��Ϸ���������
		eventData.bIsVerChange	= false;//IsGameVerChange();	// �����Ƿ�仯�汾��,�о�Ҫ�˳���Ϸ������


		IEventEngine * pEventEngine = gClientGlobal->getEventEngine();
		if(pEventEngine != NULL)
		{
			 //����ִ���¼�
			pEventEngine->FireExecute(EVENT_SYSTEM_DEFAULTSERVER_CHANGED, SOURCE_TYPE_SYSTEM, 0, (LPCSTR)&eventData, sizeof(eventData));
		}
	}

	WarningLn(a2utf8("�趨��ǰ��Ϸ��:")<<serverNode.m_name.c_str());

	//����ʾ�㷵��������
	UpdateCurrentDistrictInfo();

	return true;
}

// ���ݵ�ǰѡ������Ϣ����һ��ini����
bool DistrictList::UpdateSelectServerIni(bool isSelectArea)
{
	ServerIndexInfo &node = m_selectedServer;

	if (node.nGroup>=m_districtTable.size())
	{
		return false;
	}	

	DistrictGroup &groupNode = m_districtTable[node.nGroup];	// ����

	if ( node.nDistrict>=groupNode.m_districtList.size() )
	{
		return false;
	}

	District &districtNode = groupNode.m_districtList[node.nDistrict];	// �汾����

	if ( node.nServer>=districtNode.m_serverList.size())
	{
		return false;
	}

	Server &serverNode = districtNode.m_serverList[node.nServer];	// ��Ϸ����

	// ����һ��ini�е���Ϣ
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	string strDistrictName;	// ��������
	strDistrictName += districtNode.m_name;
	strDistrictName += " ";
	strDistrictName += serverNode.m_name;
	// д��汾��Ϣ
	GameIni::getInstance().setString("Login","server",a2utf8(strDistrictName.c_str()));
	UpdateIni::getInstance().setString("Login","server", a2utf8(strDistrictName.c_str()));

	// ����Ϊ�����½���ķ�����
	UpdateIni::getInstance().setInt("Login","serverid",m_selectedServer.nServer);
	UpdateIni::getInstance().setInt("Login","districtid",m_selectedServer.nDistrict);
	UpdateIni::getInstance().setInt("Login","groupid",m_selectedServer.nGroup);
	UpdateIni::getInstance().setString("Login","areaname", a2utf8(groupNode.m_name.c_str()));
	UpdateIni::getInstance().setString("Login","worldname", a2utf8(serverNode.m_name.c_str()));

	GameIni::getInstance().setInt("Login","serverid",m_selectedServer.nServer);
	GameIni::getInstance().setInt("Login","districtid",m_selectedServer.nDistrict);
	GameIni::getInstance().setInt("Login","groupid",m_selectedServer.nGroup);
	GameIni::getInstance().setString("Login","areaname", a2utf8(groupNode.m_name.c_str()));
	GameIni::getInstance().setString("Login","worldname", a2utf8(serverNode.m_name.c_str()));


	// д�������IP���˿��б�
	int isp = GetMyISP();
	string strIP, strPort;
	stringstream IPBuf, PortBuf;

	IP_INFO::iterator iter;
	IP_INFO::iterator end;

    iter = serverNode.m_ipInfo.begin();
	end = serverNode.m_ipInfo.end();

	int find_count = 0;

	for (;iter != end; iter++)
	{
		if (iter->m_isp == isp)
		{
			IPBuf << iter->m_ip.c_str() << ',';
			PortBuf << iter-> m_port << ',';
			find_count++;
		}		
	}

	// ����������ָ���������͵����û���
	if (find_count == 0)
	{
		iter = serverNode.m_ipInfo.begin();
		end = serverNode.m_ipInfo.end();
		for (;iter != end; iter++)
		{
			IPBuf << iter->m_ip.c_str() << ',';
			PortBuf << iter-> m_port << ',';
			find_count++;
		}
	}

	strIP = IPBuf.str();
	strPort = PortBuf.str();

	GameIni::getInstance().setString("Network", "ip", strIP.c_str());
    GameIni::getInstance().setString("Network","port", strPort.c_str());
	int isNated = UpdateIni::getInstance().getInt("Network","is_nated",255);
	GameIni::getInstance().setInt("Network","is_nated", isNated);

	GameIni::getInstance().setString("NetWork","timeServerIp", serverNode.m_timeServerIpInfo.m_ip.c_str());
	GameIni::getInstance().setInt("NetWork","timeServerPort", serverNode.m_timeServerIpInfo.m_port);
	GameIni::getInstance().setInt("NetWork", "support_udp", serverNode.m_support_udp); 

	return true;
}

//  ȡ����Ϸ����������Ϣ
const SClientGameServerInfo * DistrictList::GetGameServerInfo(DWORD nGroup,DWORD nDistrict,DWORD nServer)
{
	ServerIndexInfo node;
	node.nGroup = nGroup;
	node.nDistrict = nDistrict;
	node.nServer = nServer;


	SClientGameServerInfo &bufIDGameServerInfo = getBufIDGameServerInfo();

	if (GetGameServerInfoNode(node,bufIDGameServerInfo)==false)
	{
		return NULL;
	}
	return &bufIDGameServerInfo;
}

//  ��������Ϣȡ����Ϸ����������Ϣ
bool DistrictList::GetGameServerInfoNode(ServerIndexInfo &node,SClientGameServerInfo &retNode)
{
	if (node.nGroup>=m_districtTable.size())
	{
		return false;
	}	

	DistrictGroup &groupNode = m_districtTable[node.nGroup];	// ����

	if ( node.nDistrict>=groupNode.m_districtList.size() )
	{
		return false;
	}

	District &districtNode = groupNode.m_districtList[node.nDistrict];	// �汾����

	if ( node.nServer>=districtNode.m_serverList.size())
	{
		return false;
	}

	Server &serverNode = districtNode.m_serverList[node.nServer];	// ��Ϸ����

	retNode.nID				= serverNode.m_nID;				// ����ID
	retNode.nGroup			= node.nGroup;			// �������
	retNode.nDistrict		= node.nDistrict;		// �汾�������
	retNode.nServer			= node.nServer;			// ��Ϸ���������

	retNode.nWorldID		= serverNode.m_nWorldID;		// ��Ϸ����ID
	retNode.nLatencty		= serverNode.m_latencty;		// �����ӳ�
	retNode.nState			= serverNode.m_state;			// ״̬ 0 ���� 1 ��æ 2 ���� 3 ά��
	retNode.nRecommend		= serverNode.m_recommend;		// �Ƽ�ֵ 1:�Ƽ� 0:��
	retNode.dwHid			= serverNode.m_dwHid;			// �Ƿ����ط�������

	sstrcpyn(retNode.szGroupName, groupNode.m_name.c_str(),sizeof(retNode.szGroupName));		// ��������
	sstrcpyn(retNode.szVerName, serverNode.m_vername.c_str(),sizeof(retNode.szVerName));		// �汾����
	sstrcpyn(retNode.szVerShowName, serverNode.m_vershow.c_str(),sizeof(retNode.szVerShowName));	// �汾��ʾ����
	sstrcpyn(retNode.szName, serverNode.m_name.c_str(),sizeof(retNode.szName));					// ����������
	sstrcpyn(retNode.szDesc, serverNode.m_desc.c_str(),sizeof(retNode.szDesc));					// ����������
	sstrcpyn(retNode.szAnnounce, serverNode.m_announce.c_str(),sizeof(retNode.szAnnounce));		// ����������
	sstrcpyn(retNode.szMainten, serverNode.m_mainten.c_str(),sizeof(retNode.szMainten));		// ά����Ϣ

	std::string strSameWorld;	// �ϳ�һ�����������б�
	DWORD dwFindCounts = 0;
	for(TMAP_ServerIndexList::iterator iter = m_mapServerIndex.begin();iter!=m_mapServerIndex.end();iter++)
	{
		ServerIndexInfo &findNode =iter->second;
		// ͬһ����Ϸ����ID��,���Ǳ����
		if (serverNode.m_nWorldID>0 && serverNode.m_nWorldID==findNode.nNumber && serverNode.m_nID!=findNode.nID)
		{
			if (findNode.nGroup<m_districtTable.size())
			{
				DistrictGroup &findGroup = m_districtTable[findNode.nGroup];	// ����
				if (findNode.nDistrict<findGroup.m_districtList.size())
				{
					District &findDistrict = findGroup.m_districtList[findNode.nDistrict];	// �汾����
					if ( findNode.nServer<findDistrict.m_serverList.size())
					{
						Server &findServer = findDistrict.m_serverList[findNode.nServer];	// ��Ϸ����
						if (findServer.m_nWorldID==serverNode.m_nWorldID && findServer.m_name!=serverNode.m_name && strSameWorld.find(findServer.m_name) == string::npos)
						{
							dwFindCounts++;
							if (dwFindCounts>1)
							{
								strSameWorld += ",";
							}
							// ��Ҫ ǰ�ӡ�
							if (findServer.m_recommend==2)
							{
								strSameWorld += "��";
							}
							strSameWorld += findServer.m_name;
						}
					}
				}
			}
		}
	}

	sstrcpyn(retNode.szSameWorld, strSameWorld.c_str(),sizeof(retNode.szSameWorld));		// ά����Ϣ

	return true;
}

// �����ط������б�ɹ�
void DistrictList::OnServerListLoad()
{
	// ��ini��ȡ�õ�ǰ��Ϸ����Ϣ,udateĿ¼�µ�config.iniȡ,�Ҳ����Ŵ�game
	int nServer = UpdateIni::getInstance().getInt("Login","serverid",GameIni::getInstance().getInt("Login","serverid",-1));
	int nDistrict = UpdateIni::getInstance().getInt("Login","districtid",GameIni::getInstance().getInt("Login","districtid",-1));
	int nGroup    = UpdateIni::getInstance().getInt("Login","groupid",GameIni::getInstance().getInt("Login","groupid",-1));

	std::string strAreaName = UpdateIni::getInstance().getString("Login","areaname",GameIni::getInstance().getString("Login","areaname","").c_str());
	std::string strWorldName = UpdateIni::getInstance().getString("Login","worldname",GameIni::getInstance().getString("Login","worldname","").c_str());

	// ��һ�μ���web�ϵķ������б�ų�ʼ��,�����ڴ��е���Ϣ
	if (nServer>=0 && nDistrict>=0 && nGroup>=0 )
	{
		//  ȡ����Ϸ����������Ϣ,���IDȡ�õĺ����Ʋ�һ����������ȡ�õ�
		const SClientGameServerInfo * pServer = NULL;
		const SClientGameServerInfo * pIDServer = GetGameServerInfo(nGroup,nDistrict,nServer);
		const SClientGameServerInfo * pNameServer = GetGameServerInfoByName(strAreaName.c_str(),strWorldName.c_str(),true);
		if (pIDServer!=NULL)
		{
			if (pNameServer!=NULL)
			{
				std::string strIDWorld = pIDServer->szName;
				std::string strNameWorld = pNameServer->szName;
				std::string strIDVerName = pIDServer->szVerName;
				std::string strNameVerName = pNameServer->szVerName;
				// ���Ʋ�һ��,��Ҫ�汾һ��,����ԭ����
				if (strIDWorld!=strNameWorld && strIDVerName==strNameVerName && strWorldName.size()>2)
				{
					pServer = pNameServer;
					WarningLn("��ID[nGroup="<<nGroup<<",nDistrict="<<nDistrict<<",nServer="<<nServer<<",]�ҵ���Ϸ��["<<pIDServer->szGroupName<<","<<pIDServer->szName<<"]�뱣����Ϸ����["
						<<strAreaName.c_str()<<","<<strWorldName.c_str()<<"]��ͬ!");
				}
				pServer = pIDServer;
			}
			else
			{
				pServer = pIDServer;
			}
		}
		else if (pNameServer!=NULL)
		{
			pServer = pNameServer;
			WarningLn("��ID[nGroup="<<nGroup<<",nDistrict="<<nDistrict<<",nServer="<<nServer<<",]�Ҳ���ѡ����Ϸ��,������Ϸ����["
				<<pServer->szGroupName<<","<<pServer->szName<<"]��Ӧ����!");
		}

		if (pServer!=NULL)
		{
			m_selectedServer.nID		= pServer->nID;			// ����������ID
			m_selectedServer.nGroup		= pServer->nGroup;		// �������
			m_selectedServer.nDistrict	= pServer->nDistrict;	// �汾�������
			m_selectedServer.nServer	= pServer->nServer;		// ��Ϸ���������

			// ����һ�µ�ǰѡ������Ϣ,������һ��
			UpdateSelectServerIni(false);
		}
	}

	// ��gameini��ip,portΪ��ʱ,Ҫ��ʼ��һ��Ĭ����
	std::string strIP		      = GameIni::getInstance().getString("Network","ip","");
	std::string strPort		      = GameIni::getInstance().getString("Network","port","");
	std::string strTimeserverIp	  = GameIni::getInstance().getString("Network","timeServerIp","");
	std::string strTimeServerPort = GameIni::getInstance().getString("Network","timeServerPort","");
	// �ж���IP,port,������
	if (strIP.size()>1 && strPort.size()>1 && strTimeserverIp.size()>1 && strTimeServerPort.size()>1)
	{
		return;
	}

	//  ȡ���Ƽ�����Ϸ����������Ϣ
	const SClientGameServerInfo * pServer = GetRecommendGameServerInfo();
	if (pServer==NULL)
	{
		WarningLn(a2utf8("��ʼ��Ĭ�Ϸ�����ʧ��! �Ҳ���Ĭ�Ϸ�����"));
		return;
	}

	// �趨��ǰ��Ϸ����������Ϣ
	bool bOK = SetSelectGameServerInfo(pServer->nGroup,pServer->nDistrict,pServer->nServer,true);


	if (bOK)
	{
		WarningLn(a2utf8("��ʼ���Ƽ�����Ϸ����ɹ�! nGroup=")<<pServer->nGroup<<",nDistrict="<<pServer->nDistrict<<",nServer="<<pServer->nServer<<",Name="<<pServer->szName);
	}
	else
	{
		WarningLn(a2utf8("��ʼ���Ƽ�����Ϸ����ʧ��! nGroup=")<<pServer->nGroup<<",nDistrict="<<pServer->nDistrict<<",nServer="<<pServer->nServer<<",Name="<<pServer->szName);
	}
}

/**  ȡ���÷ֺŷֿ������ݴ�����������
@param   int * pArray: ����
@param   int nArrayLen:���鳤��
@param	char * szStringBuf : �������ִ�
@param   int nBufLen:�ִ�Buffer����
@return  int :����
*/
int DistrictList::GetStringIntArray(int * pArray,int nArrayLen,const char * szStringBuf,int nBufLen)
{
	if (pArray==NULL || szStringBuf==NULL || nArrayLen<=0 || nBufLen<=0)
	{
		return 0;
	}

	// 1;2;3;4;5

	int nGetInt = 0;
	int nGetCount = 0;
	vector<string> strList;
	StringHelper::split(strList, szStringBuf, ';', " ");

	for (WORD i=0;i<strList.size();i++)
	{
		nGetInt = StringHelper::toInt(strList[i]);
		if (nGetCount<nArrayLen)
		{
			(*pArray) = nGetInt;
			pArray++;
			nGetCount++;
		}
	}

	return nGetCount;

}

bool DistrictList::AddLastEnterServerList(ServerIndexInfo &addNode)
{
	for (TLIST_ServerIndexList::iterator it = m_lastEnterServerList.begin();it!=m_lastEnterServerList.end();)
	{
		ServerIndexInfo &node = (*it);
		// ɾ��ԭ����
		if (node.nGroup==addNode.nGroup && node.nDistrict==addNode.nDistrict && node.nServer==addNode.nServer )
		{
			it = m_lastEnterServerList.erase(it);
		}
		else
		{
			it++;
		}
	}

	m_lastEnterServerList.push_back(addNode);

	// ɾ���������ݵĽڵ� 18
	while(m_lastEnterServerList.size()>18)
	{
		m_lastEnterServerList.pop_front();
	}

	LoadLastEnterServerList(false);	// ����

	return true;
}

// ��������¼���ķ����� bIsLoad:false ����,true:Ϊ����
bool DistrictList::LoadLastEnterServerList(bool bIsLoad)
{
	// ����
	if (!bIsLoad)
	{
		char szBuf[256] = {0};
		string strAllServerList;
		DWORD dwIndex = 0;

		for (TLIST_ServerIndexList::iterator it = m_lastEnterServerList.begin();it!=m_lastEnterServerList.end();it++)
		{
			ServerIndexInfo &node = (*it);
			dwIndex++;
			if (dwIndex>1)
			{
				strAllServerList += ";";
			}
			DWORD dwSeralNo = node.nGroup*10000+node.nDistrict*100+node.nServer+1;
			sprintf_s(szBuf, _countof(szBuf),"%d",dwSeralNo);
			strAllServerList += szBuf;
		}

		UpdateIni::getInstance().setString("Login","lastenterserver",strAllServerList.c_str());
	}
	else
	{
		int nServerIDList[128] = {0};
		std::string strAllServerList = UpdateIni::getInstance().getString("Login","lastenterserver","");
		DWORD dwCounts = GetStringIntArray(nServerIDList,128,strAllServerList.c_str(),strAllServerList.size());

		m_lastEnterServerList.clear();

		for (DWORD i =0;i<dwCounts;i++)
		{
			DWORD dwSeralNo = (DWORD)nServerIDList[i];
			if (dwSeralNo>0)
			{
				DWORD dwServer = dwSeralNo%100;
				if (dwServer>0)
				{
					ServerIndexInfo addNode;
					addNode.nID			= i+1;						// ����ID
					addNode.nGroup		= dwSeralNo/10000;			// �������
					addNode.nDistrict	= (dwSeralNo%10000)/100;	// �汾�������
					addNode.nServer		= dwServer-1;				// ��Ϸ���������
					addNode.nNumber		= i+1;		// ��������

					m_lastEnterServerList.push_back(addNode);
				}
			}
		}
	}

	return true;
}


// ������Ϸ���¼�����
void DistrictList::OnEnterServer(DWORD nGroup,DWORD nDistrict,DWORD nServer)
{
	ServerIndexInfo addNode;
	addNode.nID			= 0;					// ����ID
	addNode.nGroup		= nGroup;					// �������
	addNode.nDistrict	= nDistrict;				// �汾�������
	addNode.nServer		= nServer;					// ��Ϸ���������
	addNode.nNumber		= 0;					// ��������

	AddLastEnterServerList(addNode);
}


// ��Ϸ�����Ƿ������Ĺؼ���ƥ��,������������ĸƥ��
bool DistrictList::IsKeyWordMatchServer(DWORD nGroup,DWORD nDistrict,DWORD nServer,const char * szKeyWord)
{
	if (szKeyWord==NULL)
	{
		return false;
	}
	if (strlen(szKeyWord)<=0)
	{
		return false;
	}
	//  ȡ����Ϸ����������Ϣ
	const SClientGameServerInfo * pServer = GetGameServerInfo(nGroup,nDistrict,nServer);
	if (pServer==NULL)
	{
		return false;
	}
	bool bMatch = false;
	std::string strServerName = pServer->szName;	// ��������

	size_t nPos = strServerName.find(szKeyWord);
	// �ùؼ���ƥ��
	if (nPos!= std::string::npos)
	{
		bMatch = true;
	}
	// ��������ĸƥ��
	if (!bMatch)
	{
		/// ��ùؼ��ֹ���ģ��
		IKeyWordClient * pKeyWordClient = gClientGlobal->getKeyWordClient();
		if (pKeyWordClient!=NULL)
		{
			char szPYAName[128] = {0};
			pKeyWordClient->HZ2PYA(strServerName.c_str(),strServerName.size(),szPYAName,128);
			std::string strServerNamePYA = szPYAName;	// ����������ƴ��������ĸ

			std::string strKeyWord = szKeyWord;
			strKeyWord = StringHelper::toLower(strKeyWord);	// ����Сд��ĸ

			nPos = strServerNamePYA.find(strKeyWord.c_str());
			if (nPos!= std::string::npos)
			{
				bMatch = true;
			}
			else
			{
				pKeyWordClient->HZ2PY(strServerName.c_str(),strServerName.size(),szPYAName,128);
				std::string strServerNamePY = szPYAName;	// ������������ƴ��

				nPos = strServerNamePY.find(strKeyWord.c_str());
				if (nPos!= std::string::npos)
				{
					bMatch = true;
				}
			}
		}
	}
	return bMatch;
}


// ȡһ����ʱIDȡ��Ϣ�Ǳ��������õ��ڴ�
SClientGameServerInfo & DistrictList::getBufIDGameServerInfo()
{
	m_dwIDInfoBufIndex++;
	if (m_dwIDInfoBufIndex>=10)
	{
		m_dwIDInfoBufIndex = 0;
	}

	return m_getIDGameServerInfo[m_dwIDInfoBufIndex];
}


// ȡ����Ϸ��������
const char * DistrictList::GetWorldNameByID(DWORD dwWorldID)
{
	if (dwWorldID == 0)
	{
		return "";
	}

	for (DWORD i=0;i<m_districtTable.size();i++)
	{
		DISTRICT_LIST &districtList = m_districtTable[i].m_districtList;
		for (DWORD j=0;j<districtList.size();j++)
		{
			SERVER_LIST &serverList = districtList[j].m_serverList;
			for (SERVER_LIST::iterator iter = serverList.begin(); iter != serverList.end(); ++iter)
			{
				if ( (*iter).m_nWorldID == dwWorldID )
				{
					return (*iter).m_name.c_str();
				}
			}
		}
	}

	return "";
}

const SClientGameServerInfo * DistrictList::GetGameServerInfoByWorldID(DWORD dwWorldID)
{
    if (dwWorldID == 0)
    {
        return NULL;
    }

    for (DWORD i=0;i<m_districtTable.size();i++)
    {
        DISTRICT_LIST &districtList = m_districtTable[i].m_districtList;
        for (DWORD j=0;j<districtList.size();j++)
        {
            SERVER_LIST &serverList = districtList[j].m_serverList;
            for (SERVER_LIST::iterator iter = serverList.begin(); iter != serverList.end(); ++iter)
            {
                if ( (*iter).m_nWorldID == dwWorldID )
                {
                    return GetGameServerInfoByID((*iter).m_nID);
                }
            }
        }
    }
	return NULL;
}