/*******************************************************************
** 文件名:	DistrictList.cpp
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
#define  DISTRICT_FILE_ROOT            "Services"		      // xml配置文件中的根节点
#define  DISTRICT_GROUP_NAME           "DistrictGroup"        // 分区群组名
#define  DISTRICT_NODE_NAME            "District"             // 分区节点名
#define  SERVER_NODE_NAME              "Server"               // 服务器节点名
#define  ATTRIBUTE_NAME_NAME           "name"                 // name属性
#define  ATTRIBUTE_NAME_SHOWNAME       "showname"             // 显示名属性
#define  ATTRIBUTE_NAME_VERNAME        "vername"              // 版本name属性
#define  ATTRIBUTE_NAME_VERSHOWNAME    "vershow"              // 版本显示名属性
#define  ATTRIBUTE_NAME_DESC           "desc"                 // desc属性
#define  ATTRIBUTE_NAME_ANNOUNCE       "announce"             // 服务器公告
#define  ATTRIBUTE_NAME_MAINTEN        "mainten"              // 维护信息
#define  ATTRIBUTE_NAME_ID             "id"                   // ID
#define  ATTRIBUTE_NAME_IP             "ip"                   // ip属性
#define  ATTRIBUTE_NAME_PORT           "port"                 // port属性
#define  ATTRIBUTE_NAME_LATENCY        "latency"              // Latency属性
#define  ATTRIBUTE_NAME_STATE          "state"                // state属性
#define  ATTRIBUTE_NAME_RECOMMEND      "recommend"            // recommend属性
#define  ATTRIBUTE_NAME_VERMODE	       "vermode"              // vermode属性
#define  ATTRIBUTE_NAME_ISP            "isp"                  // isp属性,指名是电信还是联通         
#define  MIRROR_NODE_NAME              "Mirror"               // 镜像服务器
#define  ATTRIBUTE_TIMESERVER_IP       "timeServerIp"         // 时间服务器IP
#define  ATTRIBUTE_TIMESERVER_PORT     "timeServerPort"       // 时间服务器端口
#define  ATTRIBUTE_NAME_HID            "hid"                  // 列表中隐藏服务器名
#define  ATTRIBUTE_SUPPORT_UDP         "udp"             // 是否支持UDP


DistrictList::DistrictList() : m_nSelectedServer(0)
{
	m_dwIDInfoBufIndex	= 0;		            // 用ID取信息是保存数据用当前序号
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


// 初始化游戏分区列表
bool DistrictList::Initialize()
{
	LoadLastEnterServerList(true);

	// 载入本地服务器列表
	if (Load())
	{
		// 当加载服务器列表成功
		OnServerListLoad();
	}
	
	//显示层消息
	IEntityClient *pEntityClient = gClientGlobal->getEntityClient();
	if (pEntityClient != NULL)
	{
		pEntityClient->registerGameViewEventHandler(GVIEWCMD_REQUEST_DISTRICT_LIST, this);
		pEntityClient->registerGameViewEventHandler(GVIEWCMD_REQUEST_SELECT_SERVER, this);
		pEntityClient->registerGameViewEventHandler(GVIEWCMD_REQUEST_LASTENTER_DISTRICT_LIST, this);
	}
	return true;
}


// 关闭
void DistrictList::Close()
{
	//显示层消息
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
	TraceLn("Server Latest Game Version：" << dwVersion);

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
	//发送所在大区信息
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

	TraceLn(a2utf8("加载本地服务器列表文件:")<<fullpath.c_str());

	if ( !doc.LoadFile(fullpath.c_str()) )
	{
		WarningLn("加载本地服务器列表文件失败!" << fullpath.c_str());
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

	// 清除所有记录
	m_districtTable.clear();

	// 清除所有记录索引列表
	m_mapServerIndex.clear();

	// 清楚服务器入口索引列表
	m_mapServerPort.clear();
	
	// 本地是联通还是电信
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

				int nWorldID = 0;	// 游戏世界ID
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
					server.m_vername = district.m_name;		// 找不到用分类名,兼以前的版本
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

				// 镜像服务器
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

	// 建立服务器索引列表
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
				addNode.nID			= nServerIndex;		// 索引ID
				addNode.nGroup		= i;				// 区组序号
				addNode.nDistrict	= j;				// 版本分类序号
				addNode.nServer		= k;				// 游戏世界区序号
				addNode.nNumber		= serverList[k].m_nWorldID;	// 游戏世界ID

				serverList[k].m_nID = nServerIndex;		// 服务器索引ID

				m_mapServerIndex[nServerIndex] = addNode;
			}
		}
	}
	return true;
}

// 安全解析XML字符串
bool DistrictList::SafeParseXML( TiXmlDocument & doc,const char * p,DWORD dwLen)
{
	if ( dwLen==0 || p==0 )
		return false;

	char * pContent = (char*)p;

	// 如果不是0结尾的字符串则创建一个临时缓冲区
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

// 本地是联通还是电信
DWORD DistrictList::GetMyISP()
{
	int isp = UpdateIni::getInstance().getInt("Network", "isp", 0);
	if (isp < 1 || isp > 3)  //等于0为自动选择
	{
		isp = NetSniffer::getInstance().GetCurNetClass();
	}
	return isp;

}

// 设定本地是联通还是电信 本地是联通还是电信 0.电信,1.联通,2:自动判断网络
void DistrictList::SetMyISP(DWORD dwISP)
{
	UpdateIni::getInstance().setInt("Network","isp",dwISP);
}


/// 获取整数    
uint DistrictList::UpdateIni_getInt(const char* section, const char* key, int def)
{
	if (section==NULL || key==NULL)
	{
		return 0;
	}
	return UpdateIni::getInstance().getInt(section,key,def);
}
/// 设置整数    
bool DistrictList::UpdateIni_setInt(const char* section, const char* key, int value)
{
	if (section==NULL || key==NULL)
	{
		return false;
	}
	return UpdateIni::getInstance().setInt(section,key,value);
}
/// 获取字符串
std::string DistrictList::UpdateIni_getString(const char* section, const char* key, const char* def)
{
	if (section==NULL || key==NULL || def==NULL)
	{
		return "";
	}
	return UpdateIni::getInstance().getString(section,key,def);
}
/// 设置字符串
bool DistrictList::UpdateIni_setString(const char* section, const char* key, const char* value)
{
	if (section==NULL || key==NULL || value==NULL)
	{
		return false;
	}
	return UpdateIni::getInstance().setString(section,key,value);
}


//////////////// IClientDistrictList //////////////////////////////////////////////////////////

//  按索引ID取得游戏世界区的信息
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

//  按大区和游戏名取得游戏世界区的信息 szAreaName:大区名,如:电信一区,szWorldName:游戏区名,bAutoMatch:如精确找不到就只按游戏区名找,并按用户网络找到匹配游戏区
const SClientGameServerInfo * DistrictList::GetGameServerInfoByName(const char * szAreaName,const char * szWorldName,bool bAutoMatch)
{
	std::string strAreaName = "";	// 大区名
	std::string strWorldName = "";	// 游戏区名

	if (szAreaName!=NULL)
	{
		strAreaName = szAreaName;
	}
	if (szWorldName!=NULL)
	{
		strWorldName = szWorldName;
	}

	//  取得当前的游戏世界区的信息
	std::string strMyVer = "";	// 当前版本名
	const SClientGameServerInfo * pSelectServer = GetSelectGameServerInfo();
	if(pSelectServer!=NULL)
	{
		strMyVer = pSelectServer->szVerName;
	}

	DWORD nMyIsp = GetMyISP();	// 本地是联通还是电信 0.电信,1.联通,2:自动判断网络

	std::string strIspName = "区";
	if (nMyIsp==1)
	{
		strIspName = "电信";
	}
	else if(nMyIsp==2)
	{
		strIspName = "联通";
	}
	else if (nMyIsp == 3)
	{
		strIspName = "移动";
	}

	// 找到的列表
	TMAP_ServerIndexList findList;		// 同游戏区名的
	TMAP_ServerIndexList findgoodList;	// 推荐
	ServerIndexInfo findnode;
	findnode.nID = 0;

	// 搜索指定名称的区
	for (DWORD i=0;i<m_districtTable.size();i++)
	{
		DISTRICT_LIST &districtList = m_districtTable[i].m_districtList;
		for (DWORD j=0;j<districtList.size();j++)
		{
			SERVER_LIST &serverList = districtList[j].m_serverList;
			for (DWORD k=0;k<serverList.size();k++)
			{
				Server &serverNode = serverList[k]; 

				// 游戏区名
				if (strWorldName==serverNode.m_name || serverNode.m_recommend==1)
				{
					ServerIndexInfo node;
					node.nID		= serverNode.m_nID;			// 索引ID
					node.nGroup		= i;		// 区组序号
					node.nDistrict	= j;		// 版本分类序号
					node.nServer	= k;		// 游戏世界区序号
					node.nNumber	= 100000-serverNode.m_latencty;		// 优先级

					// 计算优先级
					std::string strGroupName = m_districtTable[i].m_name;
					if(strGroupName.find(strIspName) != std::string::npos)
					{
						node.nNumber += 100000;
					}
					// 同一个版本的优先
					std::string strVerName = serverNode.m_vername;
					if (strMyVer==strVerName)
					{
						node.nNumber += 10000;
					}

					// 分开存不同的列表
					if (strWorldName==serverNode.m_name)
					{
						findList[serverNode.m_nID] = node;
					}
					else
					{
						findgoodList[serverNode.m_nID] = node;
					}

					// 大区名对了,并且只取第一个
					if (strWorldName==serverNode.m_name && strAreaName==m_districtTable[i].m_name && findnode.nID==0)
					{
						findnode = node;
					}
				}
			}
		}
	}

	// 如精确找不到就只按游戏区名找,并按用户网络找到匹配游戏区
	if (findnode.nID==0 && bAutoMatch)
	{
		if (findList.size()>0)
		{
			// 取优先级最大的
			for (TMAP_ServerIndexList::iterator iter = findList.begin();iter!=findList.end(); iter++)
			{			
				if (iter->second.nNumber> findnode.nNumber)
				{
					findnode = iter->second;
				}
			}
		}
		// 还是找不到,就给推荐
		if (findnode.nID==0 && findgoodList.size()>0)
		{
			// 取优先级最大的
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

//  取得推荐的游戏世界区的信息
const SClientGameServerInfo * DistrictList::GetRecommendGameServerInfo()
{
	//  取得当前的游戏世界区的信息
	std::string strMyVer = "";	// 当前版本名
	const SClientGameServerInfo * pSelectServer = GetSelectGameServerInfo();
	if(pSelectServer!=NULL)
	{
		strMyVer = pSelectServer->szVerName;
	}

	DWORD nMyIsp = GetMyISP();	// 本地是联通还是电信 0.电信,1.联通,2:自动判断网络

	std::string strIspName = "区";
	if (nMyIsp==1)
	{
		strIspName = "电信";
	}
	else if(nMyIsp==2)
	{
		strIspName = "联通";
	}
	else if (nMyIsp == 3)
	{
		strIspName = "移动";
	}

	// 找到的列表
	TMAP_ServerIndexList findList;

	// 搜索指定名称的区
	for (DWORD i=0;i<m_districtTable.size();i++)
	{
		DISTRICT_LIST &districtList = m_districtTable[i].m_districtList;
		for (DWORD j=0;j<districtList.size();j++)
		{
			SERVER_LIST &serverList = districtList[j].m_serverList;
			for (DWORD k=0;k<serverList.size();k++)
			{
				Server &serverNode = serverList[k]; 
				// 是推荐新区
				if (serverNode.m_recommend==1)
				{
					ServerIndexInfo node;
					node.nID		= serverNode.m_nID;			// 索引ID
					node.nGroup		= i;		// 区组序号
					node.nDistrict	= j;		// 版本分类序号
					node.nServer	= k;		// 游戏世界区序号
					node.nNumber	= 100000-serverNode.m_latencty;		// 优先级

					// 计算优先级
					std::string strGroupName = m_districtTable[i].m_name;
					if(strGroupName.find(strIspName) != std::string::npos)
					{
						node.nNumber += 100000;
					}
					// 同一个版本的优先
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
		// 取优先级最大的
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

//  取得当前的游戏世界区的信息
const SClientGameServerInfo * DistrictList::GetSelectGameServerInfo()
{
	SClientGameServerInfo &bufIDGameServerInfo = getBufIDGameServerInfo();
	if (GetGameServerInfoNode(m_selectedServer,bufIDGameServerInfo)==false)
	{
		return NULL;
	}
	return &bufIDGameServerInfo;
	
}

//  设定当前游戏世界区的信息
bool DistrictList::SetSelectGameServerInfo(DWORD nGroup,DWORD nDistrict,DWORD nServer,bool bSendEvent)
{
	if (nGroup>=m_districtTable.size())
	{
		WarningLn(a2utf8("设定当前游戏世界区的信息失败! nGroup=")<<nGroup<<"nDistrict="<<nDistrict<<",nServer="<<nServer<<",Groupsize="<<m_districtTable.size());
		return false;
	}	

	DistrictGroup &groupNode = m_districtTable[nGroup];	// 区组

	if ( nDistrict>=groupNode.m_districtList.size() )
	{
		WarningLn(a2utf8("设定当前游戏世界区的信息失败! nGroup=")<<nGroup<<"nDistrict="<<nDistrict<<",nServer="<<nServer<<",Districtsize="<<groupNode.m_districtList.size());
		return false;
	}

	District &districtNode = groupNode.m_districtList[nDistrict];	// 版本分类

	if ( nServer>=districtNode.m_serverList.size())
	{
		WarningLn(a2utf8("设定当前游戏世界区的信息失败! nGroup=")<<nGroup<<"nDistrict="<<nDistrict<<",nServer="<<nServer<<",serversize="<<districtNode.m_serverList.size());
		return false;
	}

	Server &serverNode = districtNode.m_serverList[nServer];	// 游戏世界

	string strOldVerName;	// 旧的版本名

	SClientGameServerInfo &bufIDGameServerInfo = getBufIDGameServerInfo();
	if (GetGameServerInfoNode(m_selectedServer,bufIDGameServerInfo))
	{
		strOldVerName = bufIDGameServerInfo.szVerName;
	}

	// 原来的信息
	ServerIndexInfo oldInfo;
	oldInfo = m_selectedServer;

	string strNewVerName = serverNode.m_vername;	// 新的版本名
	// 版本分类名不一样,换版本
	if (strOldVerName.size()<2)
	{
		m_selectedServer.nNumber	= 0;	// 换区标识 0.不用退game.exe ,-1.要退game.exe,换版本,-2.重起game.exe
	}
	else if (strNewVerName==strOldVerName)
	{
		m_selectedServer.nNumber	= -2;	// 换区标识 0.不用退game.exe ,-1.要退game.exe,换版本,-2.重起game.exe
	}
	else
	{
		m_selectedServer.nNumber	= -1;	// 换区标识 0.不用退game.exe ,-1.要退game.exe,换版本,-2.重起game.exe
	}

	m_selectedServer.nID		= serverNode.m_nID;		// 服务器索引ID
	m_selectedServer.nGroup		= nGroup;				// 区组序号
	m_selectedServer.nDistrict	= nDistrict;			// 版本分类序号
	m_selectedServer.nServer	= nServer;				// 游戏世界区序号

	// 更新一下当前选中区信息,并保存一下
	UpdateSelectServerIni(true);

	// 发事件
	if(bSendEvent)
	{
		// 默认区变化了,新服务器IP,端口可从config.ini中取
		event_system_default_server_changed eventData;
		// 新区服务器信息
		eventData.nID			= m_selectedServer.nID;				// 服务器索引ID
		eventData.nGroup		= m_selectedServer.nGroup;			// 区组序号
		eventData.nDistrict		= m_selectedServer.nDistrict;		// 版本分类序号
		eventData.nServer		= m_selectedServer.nServer;			// 游戏世界区序号
		// 原区服务器信息
		eventData.nID0			= oldInfo.nID;			// 服务器索引ID
		eventData.nGroup0		= oldInfo.nGroup;		// 区组序号
		eventData.nDistrict0	= oldInfo.nDistrict;	// 版本分类序号
		eventData.nServer0		= oldInfo.nServer;		// 游戏世界区序号
		eventData.bIsVerChange	= false;//IsGameVerChange();	// 换区是否变化版本了,有就要退出游戏并更新


		IEventEngine * pEventEngine = gClientGlobal->getEventEngine();
		if(pEventEngine != NULL)
		{
			 //发送执行事件
			pEventEngine->FireExecute(EVENT_SYSTEM_DEFAULTSERVER_CHANGED, SOURCE_TYPE_SYSTEM, 0, (LPCSTR)&eventData, sizeof(eventData));
		}
	}

	WarningLn(a2utf8("设定当前游戏区:")<<serverNode.m_name.c_str());

	//向显示层返回请求结果
	UpdateCurrentDistrictInfo();

	return true;
}

// 跟据当前选中区信息更新一下ini配置
bool DistrictList::UpdateSelectServerIni(bool isSelectArea)
{
	ServerIndexInfo &node = m_selectedServer;

	if (node.nGroup>=m_districtTable.size())
	{
		return false;
	}	

	DistrictGroup &groupNode = m_districtTable[node.nGroup];	// 区组

	if ( node.nDistrict>=groupNode.m_districtList.size() )
	{
		return false;
	}

	District &districtNode = groupNode.m_districtList[node.nDistrict];	// 版本分类

	if ( node.nServer>=districtNode.m_serverList.size())
	{
		return false;
	}

	Server &serverNode = districtNode.m_serverList[node.nServer];	// 游戏世界

	// 保存一下ini中的信息
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	string strDistrictName;	// 服务器名
	strDistrictName += districtNode.m_name;
	strDistrictName += " ";
	strDistrictName += serverNode.m_name;
	// 写入版本信息
	GameIni::getInstance().setString("Login","server",a2utf8(strDistrictName.c_str()));
	UpdateIni::getInstance().setString("Login","server", a2utf8(strDistrictName.c_str()));

	// 保存为最近登陆过的服务器
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


	// 写入服务器IP及端口列表
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

	// 兼容内网无指定网络类型的配置环境
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

//  取得游戏世界区的信息
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

//  按索引信息取得游戏世界区的信息
bool DistrictList::GetGameServerInfoNode(ServerIndexInfo &node,SClientGameServerInfo &retNode)
{
	if (node.nGroup>=m_districtTable.size())
	{
		return false;
	}	

	DistrictGroup &groupNode = m_districtTable[node.nGroup];	// 区组

	if ( node.nDistrict>=groupNode.m_districtList.size() )
	{
		return false;
	}

	District &districtNode = groupNode.m_districtList[node.nDistrict];	// 版本分类

	if ( node.nServer>=districtNode.m_serverList.size())
	{
		return false;
	}

	Server &serverNode = districtNode.m_serverList[node.nServer];	// 游戏世界

	retNode.nID				= serverNode.m_nID;				// 索引ID
	retNode.nGroup			= node.nGroup;			// 区组序号
	retNode.nDistrict		= node.nDistrict;		// 版本分类序号
	retNode.nServer			= node.nServer;			// 游戏世界区序号

	retNode.nWorldID		= serverNode.m_nWorldID;		// 游戏世界ID
	retNode.nLatencty		= serverNode.m_latencty;		// 网络延迟
	retNode.nState			= serverNode.m_state;			// 状态 0 良好 1 繁忙 2 爆满 3 维护
	retNode.nRecommend		= serverNode.m_recommend;		// 推荐值 1:推荐 0:无
	retNode.dwHid			= serverNode.m_dwHid;			// 是否隐藏服务器名

	sstrcpyn(retNode.szGroupName, groupNode.m_name.c_str(),sizeof(retNode.szGroupName));		// 分组名称
	sstrcpyn(retNode.szVerName, serverNode.m_vername.c_str(),sizeof(retNode.szVerName));		// 版本名称
	sstrcpyn(retNode.szVerShowName, serverNode.m_vershow.c_str(),sizeof(retNode.szVerShowName));	// 版本显示名称
	sstrcpyn(retNode.szName, serverNode.m_name.c_str(),sizeof(retNode.szName));					// 服务器名称
	sstrcpyn(retNode.szDesc, serverNode.m_desc.c_str(),sizeof(retNode.szDesc));					// 服务器描述
	sstrcpyn(retNode.szAnnounce, serverNode.m_announce.c_str(),sizeof(retNode.szAnnounce));		// 服务器公告
	sstrcpyn(retNode.szMainten, serverNode.m_mainten.c_str(),sizeof(retNode.szMainten));		// 维护信息

	std::string strSameWorld;	// 合成一个区的名称列表
	DWORD dwFindCounts = 0;
	for(TMAP_ServerIndexList::iterator iter = m_mapServerIndex.begin();iter!=m_mapServerIndex.end();iter++)
	{
		ServerIndexInfo &findNode =iter->second;
		// 同一个游戏世界ID的,不是本身的
		if (serverNode.m_nWorldID>0 && serverNode.m_nWorldID==findNode.nNumber && serverNode.m_nID!=findNode.nID)
		{
			if (findNode.nGroup<m_districtTable.size())
			{
				DistrictGroup &findGroup = m_districtTable[findNode.nGroup];	// 区组
				if (findNode.nDistrict<findGroup.m_districtList.size())
				{
					District &findDistrict = findGroup.m_districtList[findNode.nDistrict];	// 版本分类
					if ( findNode.nServer<findDistrict.m_serverList.size())
					{
						Server &findServer = findDistrict.m_serverList[findNode.nServer];	// 游戏世界
						if (findServer.m_nWorldID==serverNode.m_nWorldID && findServer.m_name!=serverNode.m_name && strSameWorld.find(findServer.m_name) == string::npos)
						{
							dwFindCounts++;
							if (dwFindCounts>1)
							{
								strSameWorld += ",";
							}
							// 主要 前加★
							if (findServer.m_recommend==2)
							{
								strSameWorld += "★";
							}
							strSameWorld += findServer.m_name;
						}
					}
				}
			}
		}
	}

	sstrcpyn(retNode.szSameWorld, strSameWorld.c_str(),sizeof(retNode.szSameWorld));		// 维护信息

	return true;
}

// 当加载服务器列表成功
void DistrictList::OnServerListLoad()
{
	// 从ini中取得当前游戏区信息,udate目录下的config.ini取,找不到才从game
	int nServer = UpdateIni::getInstance().getInt("Login","serverid",GameIni::getInstance().getInt("Login","serverid",-1));
	int nDistrict = UpdateIni::getInstance().getInt("Login","districtid",GameIni::getInstance().getInt("Login","districtid",-1));
	int nGroup    = UpdateIni::getInstance().getInt("Login","groupid",GameIni::getInstance().getInt("Login","groupid",-1));

	std::string strAreaName = UpdateIni::getInstance().getString("Login","areaname",GameIni::getInstance().getString("Login","areaname","").c_str());
	std::string strWorldName = UpdateIni::getInstance().getString("Login","worldname",GameIni::getInstance().getString("Login","worldname","").c_str());

	// 第一次加载web上的服务器列表才初始化,否则内存中的信息
	if (nServer>=0 && nDistrict>=0 && nGroup>=0 )
	{
		//  取得游戏世界区的信息,如果ID取得的和名称不一样就用名称取得的
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
				// 名称不一样,但要版本一样,否则按原来的
				if (strIDWorld!=strNameWorld && strIDVerName==strNameVerName && strWorldName.size()>2)
				{
					pServer = pNameServer;
					WarningLn("用ID[nGroup="<<nGroup<<",nDistrict="<<nDistrict<<",nServer="<<nServer<<",]找到游戏区["<<pIDServer->szGroupName<<","<<pIDServer->szName<<"]与保存游戏区名["
						<<strAreaName.c_str()<<","<<strWorldName.c_str()<<"]不同!");
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
			WarningLn("用ID[nGroup="<<nGroup<<",nDistrict="<<nDistrict<<",nServer="<<nServer<<",]找不到选中游戏区,换成游戏区名["
				<<pServer->szGroupName<<","<<pServer->szName<<"]对应的区!");
		}

		if (pServer!=NULL)
		{
			m_selectedServer.nID		= pServer->nID;			// 服务器索引ID
			m_selectedServer.nGroup		= pServer->nGroup;		// 区组序号
			m_selectedServer.nDistrict	= pServer->nDistrict;	// 版本分类序号
			m_selectedServer.nServer	= pServer->nServer;		// 游戏世界区序号

			// 更新一下当前选中区信息,并保存一下
			UpdateSelectServerIni(false);
		}
	}

	// 当gameini中ip,port为空时,要初始化一下默认区
	std::string strIP		      = GameIni::getInstance().getString("Network","ip","");
	std::string strPort		      = GameIni::getInstance().getString("Network","port","");
	std::string strTimeserverIp	  = GameIni::getInstance().getString("Network","timeServerIp","");
	std::string strTimeServerPort = GameIni::getInstance().getString("Network","timeServerPort","");
	// 有定义IP,port,不处理
	if (strIP.size()>1 && strPort.size()>1 && strTimeserverIp.size()>1 && strTimeServerPort.size()>1)
	{
		return;
	}

	//  取得推荐的游戏世界区的信息
	const SClientGameServerInfo * pServer = GetRecommendGameServerInfo();
	if (pServer==NULL)
	{
		WarningLn(a2utf8("初始化默认服务器失败! 找不到默认服务器"));
		return;
	}

	// 设定当前游戏世界区的信息
	bool bOK = SetSelectGameServerInfo(pServer->nGroup,pServer->nDistrict,pServer->nServer,true);


	if (bOK)
	{
		WarningLn(a2utf8("初始化推荐的游戏世界成功! nGroup=")<<pServer->nGroup<<",nDistrict="<<pServer->nDistrict<<",nServer="<<pServer->nServer<<",Name="<<pServer->szName);
	}
	else
	{
		WarningLn(a2utf8("初始化推荐的游戏世界失败! nGroup=")<<pServer->nGroup<<",nDistrict="<<pServer->nDistrict<<",nServer="<<pServer->nServer<<",Name="<<pServer->szName);
	}
}

/**  取得用分号分开的数据串，存入数组
@param   int * pArray: 数组
@param   int nArrayLen:数组长度
@param	char * szStringBuf : 分析的字串
@param   int nBufLen:字串Buffer长度
@return  int :个数
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
		// 删除原来的
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

	// 删除超过数据的节点 18
	while(m_lastEnterServerList.size()>18)
	{
		m_lastEnterServerList.pop_front();
	}

	LoadLastEnterServerList(false);	// 保存

	return true;
}

// 加载最后登录过的服务器 bIsLoad:false 保存,true:为加载
bool DistrictList::LoadLastEnterServerList(bool bIsLoad)
{
	// 保存
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
					addNode.nID			= i+1;						// 索引ID
					addNode.nGroup		= dwSeralNo/10000;			// 区组序号
					addNode.nDistrict	= (dwSeralNo%10000)/100;	// 版本分类序号
					addNode.nServer		= dwServer-1;				// 游戏世界区序号
					addNode.nNumber		= i+1;		// 备用数字

					m_lastEnterServerList.push_back(addNode);
				}
			}
		}
	}

	return true;
}


// 进入游戏区事件处理
void DistrictList::OnEnterServer(DWORD nGroup,DWORD nDistrict,DWORD nServer)
{
	ServerIndexInfo addNode;
	addNode.nID			= 0;					// 索引ID
	addNode.nGroup		= nGroup;					// 区组序号
	addNode.nDistrict	= nDistrict;				// 版本分类序号
	addNode.nServer		= nServer;					// 游戏世界区序号
	addNode.nNumber		= 0;					// 备用数字

	AddLastEnterServerList(addNode);
}


// 游戏区名是否和输入的关键字匹配,包括汉字首字母匹配
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
	//  取得游戏世界区的信息
	const SClientGameServerInfo * pServer = GetGameServerInfo(nGroup,nDistrict,nServer);
	if (pServer==NULL)
	{
		return false;
	}
	bool bMatch = false;
	std::string strServerName = pServer->szName;	// 服务器名

	size_t nPos = strServerName.find(szKeyWord);
	// 用关键字匹配
	if (nPos!= std::string::npos)
	{
		bMatch = true;
	}
	// 汉字首字母匹配
	if (!bMatch)
	{
		/// 获得关键字过滤模块
		IKeyWordClient * pKeyWordClient = gClientGlobal->getKeyWordClient();
		if (pKeyWordClient!=NULL)
		{
			char szPYAName[128] = {0};
			pKeyWordClient->HZ2PYA(strServerName.c_str(),strServerName.size(),szPYAName,128);
			std::string strServerNamePYA = szPYAName;	// 服务器名汉拼音字首字母

			std::string strKeyWord = szKeyWord;
			strKeyWord = StringHelper::toLower(strKeyWord);	// 换成小写字母

			nPos = strServerNamePYA.find(strKeyWord.c_str());
			if (nPos!= std::string::npos)
			{
				bMatch = true;
			}
			else
			{
				pKeyWordClient->HZ2PY(strServerName.c_str(),strServerName.size(),szPYAName,128);
				std::string strServerNamePY = szPYAName;	// 服务器名汉字拼音

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


// 取一个临时ID取信息是保存数据用的内存
SClientGameServerInfo & DistrictList::getBufIDGameServerInfo()
{
	m_dwIDInfoBufIndex++;
	if (m_dwIDInfoBufIndex>=10)
	{
		m_dwIDInfoBufIndex = 0;
	}

	return m_getIDGameServerInfo[m_dwIDInfoBufIndex];
}


// 取得游戏世界名称
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