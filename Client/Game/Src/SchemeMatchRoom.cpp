#include "StdAfx.h"
#include "IClientGlobal.h"
#include "SchemeMatchRoom.h"
#include <string>
#include "XlsValue.h"

bool CSchemeMatchRoom::LoadScheme( void )
{
	ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

    string stringPath = SCP_PATH_FORMAT( DMATCHROOM_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("config file load failed! filename=" << stringPath.c_str() << endl);
		return false;
	}

	return true;
}

void CSchemeMatchRoom::Close( void )
{
	for (size_t i = 0; i < m_vctSchemeInfo.size(); ++i)
	{
		delete m_vctSchemeInfo[i];
	}
	m_vctSchemeInfo.clear();
	m_mapMode2Datas.clear();
	m_mapSchemeData.clear();
}


bool CSchemeMatchRoom::OnSchemeUpdate( SCRIPT_READER reader, const char* szFileName )
{
	return OnSchemeLoad(reader, szFileName);
}

bool CSchemeMatchRoom::OnSchemeLoad( SCRIPT_READER reader, const char* szFileName )
{

	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	Close(); 

	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	m_vctSchemeInfo.reserve( nRecordCount );
	for (int nRow = 0; nRow < nRecordCount; ++nRow)
	{
		int nIndex = 0;

        int nMatchTypeID = pCSVReader->GetInt(nRow, nIndex++, 0);
        // 校验ID是否有效
        if(nMatchTypeID <= MatchType_None || nMatchTypeID >= MatchType_MaxID)
        {
            ErrorLn(__FUNCTION__": match_type_id is invalid ! row="<< (nRow+1) << ", col="<<nIndex);
            return false;
        }

		SSchemeMatchRoom* pData     = new SSchemeMatchRoom();
        pData->m_dwMatchTypeID      = static_cast<DWORD>( nMatchTypeID );

        int nDesLen = sizeof(pData->m_szMatchTypeDes);
        pCSVReader->GetString(nRow, nIndex++, pData->m_szMatchTypeDes, nDesLen);
        
  		pData->m_nModeID			= static_cast<BYTE>( pCSVReader->GetInt(nRow, nIndex++, 0) );
		pData->m_dwMapID			= static_cast<DWORD>( pCSVReader->GetInt(nRow, nIndex++, 0) );
		pData->m_NameTextInfoID		= pCSVReader->GetInt(nRow, nIndex++, 0);
		pData->m_WarID				= pCSVReader->GetInt(nRow, nIndex++, 0);
		pData->m_CampCount			= static_cast<BYTE>( pCSVReader->GetInt(nRow, nIndex++, 0) );
		pData->m_CampRoleMax		= static_cast<BYTE>( pCSVReader->GetInt(nRow, nIndex++, 0) );
		pData->m_StartSelectHeroNeedRoleMin		= static_cast<BYTE>( pCSVReader->GetInt(nRow, nIndex++, 0) );
		pData->m_StartSelectHeroNeedCampRoleMin	= static_cast<BYTE>( pCSVReader->GetInt(nRow, nIndex++, 0) );
		pData->m_SelectHeroTime		= pCSVReader->GetInt(nRow, nIndex++, 0);
		pData->m_ReadyGoTime	= pCSVReader->GetInt(nRow, nIndex++, 0);
		pData->m_DescTextInfoID		= pCSVReader->GetInt(nRow, nIndex++, 0);
		pData->m_PicPathTextInfoID	= pCSVReader->GetInt(nRow, nIndex++, 0);
		if ( !getModes(pCSVReader, nRow, nIndex++, pData->m_RoomStateShow) )
		{
			ErrorLn("CSchemeMatchRoom::OnSchemeLoad !getModes(pCSVReader, nRow, nIndex++, pData->m_RoomStateShow)");
			return false;
		}
		if ( !getModes(pCSVReader, nRow, nIndex++, pData->m_RoomStateChangePos2SelfCamp) )
		{
			ErrorLn("CSchemeMatchRoom::OnSchemeLoad !getModes(pCSVReader, nRow, nIndex++, pData->m_RoomStateChangePos2SelfCamp)");
			return false;
		}
		if ( !getModes(pCSVReader, nRow, nIndex++, pData->m_RoomStateChangePos2OtherCamp) )
		{
			ErrorLn("CSchemeMatchRoom::OnSchemeLoad !getModes(pCSVReader, nRow, nIndex++, pData->m_RoomStateChangePos2OtherCamp)");
			return false;
		}
		if ( !getModes(pCSVReader, nRow, nIndex++, pData->m_RoomStateEnter) )
		{
			ErrorLn("CSchemeMatchRoom::OnSchemeLoad !getModes(pCSVReader, nRow, nIndex++, pData->m_RoomStateEnter)");
			return false;
		}
		if ( !getModes(pCSVReader, nRow, nIndex++, pData->m_RoomStateExit) )
		{
			ErrorLn("CSchemeMatchRoom::OnSchemeLoad !getModes(pCSVReader, nRow, nIndex++, pData->m_RoomStateExit)");
			return false;
		}
		if ( !getModes(pCSVReader, nRow, nIndex++, pData->m_RoomStateKick) )
		{
			ErrorLn("CSchemeMatchRoom::OnSchemeLoad !getModes(pCSVReader, nRow, nIndex++, pData->m_RoomStateKick)");
			return false;
		}
		pData->m_SelectHeroCount		= pCSVReader->GetInt(nRow, nIndex++, 0);
		pData->m_CLLoadSceneTimeMS		= pCSVReader->GetInt(nRow, nIndex++, 0);
		pData->m_SameHeroCountMax		= static_cast<BYTE>( pCSVReader->GetInt(nRow, nIndex++, 0) );
		pData->m_HeroSelectRuleID		= static_cast<BYTE>( pCSVReader->GetInt(nRow, nIndex++, 0) );

		//选择英雄规则ID （重复可选控制）
		pData->m_SelectHeroControlID	= static_cast<BYTE>( pCSVReader->GetInt(nRow, nIndex++, 0) );

        // 全局英雄标示 EGlobleSelectFlag
        pData->m_byGlobleSelectFlag	= static_cast<BYTE>( pCSVReader->GetInt(nRow, nIndex++, 0) );

        char strHeroList[128] = {0};
        int nLen = sizeof(strHeroList);
        pCSVReader->GetString(nRow, nIndex++, strHeroList, nLen);
        int nParmCount = parseIntArray(strHeroList, pData->m_SelectFlagHeroList, MAX_SELECT_FLAG_COUT, ';');

        //延迟进入房间
        pData->m_bCanDeLayJoinRoom		= (pCSVReader->GetInt(nRow, nIndex++, 0) == 1);

        //延迟进入房间时间	
        pData->m_nDeLayJoinTime		    = pCSVReader->GetInt(nRow, nIndex++, 0);

        //进入房间离线检测
        pData->m_bDisconnectCheck		= (pCSVReader->GetInt(nRow, nIndex++, 0) == 1);

        // 开启自动添加电脑
        pData->m_bOpenAddComputer		= (pCSVReader->GetInt(nRow, nIndex++, 0) == 1);

        // 开启自动添加电脑时间
        pData->m_nAddComputerTime		= pCSVReader->GetInt(nRow, nIndex++, 0);

        // 假人选人定时器
        pData->m_nComputerStartSelectTime = pCSVReader->GetInt(nRow, nIndex++, 0);

        //ban选开关	
        pData->m_bOpenBan               = (pCSVReader->GetInt(nRow, nIndex++, 0) == 1);

        //开始房间ban选时间(秒)	
        pData->m_nBanTime               = pCSVReader->GetInt(nRow, nIndex++, 0);

        //ban选预选英雄广播其他阵营
        pData->m_bBroadCastOther        = (pCSVReader->GetInt(nRow, nIndex++, 0) == 1);

        //阵营ban选数量
        pData->m_nCampBanCount              = pCSVReader->GetInt(nRow, nIndex++, 0);

        //ban选位置
        char strBanPosList[128] = {0};
        nLen = sizeof(strBanPosList);
        pCSVReader->GetString(nRow, nIndex++, strBanPosList, nLen);
        pData->m_nBanPosCount = parseIntArray(strBanPosList, pData->m_BanPosList, MAX_BAN_COUNT, ';');
        if (pData->m_nBanPosCount < pData->m_nCampBanCount * pData->m_CampCount)
        {
            ErrorLn(__FUNCTION__": nBanPosCount < pData->m_nCampBanCount * pData->m_CampCount! nRow = "<<nRow<<" nBanCount="<<pData->m_nCampBanCount * pData->m_CampCount<<" nBanPosCount="<<pData->m_nBanPosCount);
            return false;
        }

        //英雄星级显示相关
        pData->m_bShowHeroStarInfo          = (pCSVReader->GetInt(nRow, nIndex++, 0) == 1);

        // 显示升级降级界面
        pData->m_nbShowDownOrUpWnd          = pCSVReader->GetInt(nRow, nIndex++, 0);

        // 匹配接受确定时间
        pData->m_nMatchAcceptTime           = pCSVReader->GetInt(nRow, nIndex++, 0);

        // 战场中退出
        pData->m_bCanLogOutInWar            = (pCSVReader->GetInt(nRow, nIndex++, 0) == 1);
        
        // 最大OB人数
        pData->m_nMaxOBCount                = pCSVReader->GetInt(nRow, nIndex++, 0);

        // 随机选择英雄开关
        pData->m_bRandomSelectHero          = (pCSVReader->GetInt(nRow, nIndex++, 0) == 1);
		// todo:pt 数据检查
		m_vctSchemeInfo.push_back( pData );
		m_mapMode2Datas[ pData->m_nModeID ].push_back( pData );
		m_mapSchemeData[ pData->m_dwMapID ].push_back( pData );
	}

	MAP_Mode2Datas::iterator itor = m_mapMode2Datas.begin();
	for (; itor != m_mapMode2Datas.end(); ++itor)
	{
		m_vctModeID.push_back( itor->first );
	}
	
	//AfterSchemeLoaded();
	return true;
}

vector<SSchemeMatchRoom* >& CSchemeMatchRoom::getByMode( uchar ModeID )
{
	MAP_Mode2Datas::iterator itor = m_mapMode2Datas.find( ModeID );
	if (itor != m_mapMode2Datas.end())
	{
		return itor->second;
	}
	return ms_NullSchemeInfo;
}

vector<SSchemeMatchRoom* >& CSchemeMatchRoom::getByMap( DWORD MapID )
{
	MAP_SchemeData::iterator itor = m_mapSchemeData.find( MapID );
	if (itor != m_mapSchemeData.end())
	{
		return itor->second;
	}
	return ms_NullSchemeInfo;
}

SSchemeMatchRoom* CSchemeMatchRoom::getSchemeMatchRoom( uchar ModeID, DWORD MapID ,int nSchemeConfigWarID)
{
    vector<SSchemeMatchRoom* >& vecAll = getByMode(ModeID);
    for (size_t i = 0; i < vecAll.size(); ++i)
    {
        if (vecAll[i]->m_dwMapID == MapID && vecAll[i]->m_WarID == nSchemeConfigWarID)
        {
            return vecAll[i];
        }
    }
    return nullptr;
}

bool CSchemeMatchRoom::getModes( ICSVReader* pCSVReader, int nRow, int nIndex, bool bStates[MatchRoomStates] )
{
	//TraceLn("CSchemeMatchRoom::getModes");
	memset(bStates, 0, sizeof(bStates[0]) * MatchRoomStates);

	char szModes[1024];
	int nLen = sizeof(szModes);
	pCSVReader->GetString(nRow, nIndex, szModes, nLen);
	int nModes[ MatchRoomStates + 1 ] = {0};
	int nCount = parseIntArray(szModes, nModes, MatchRoomStates + 1, ';');
	if (nCount > MatchRoomStates)
	{
		ErrorLn("CSchemeMatchRoom::getModes nCount > MatchRoomStates nCount= " << nCount);
		return false;
	}

	for (int i = 0; i < nCount; ++i)
	{
		int nMode = nModes[i];
		if (nMode >= MatchRoomStates)
		{
			ErrorLn("CSchemeMatchRoom::getModes nMode >= MatchRoomStates nMode= " << nMode);
			return false;
		}
		if (bStates[ nMode ])
		{
			ErrorLn("CSchemeMatchRoom::getModes bStates[ nMode ] nMode= " << nMode);

			return false;
		}
		bStates[ nMode ] = true;
	}
	return true;
}

SSchemeSelectControl * CSchemeMatchRoom::getSelectControlScheme(int)
{
	return nullptr;
}

SSchemeMatchRoom* CSchemeMatchRoom::getSchemeMatchRoomByMatchTypeID(DWORD dwMatchTypeID)
{
    VCT_SchemeInfo::iterator iter = m_vctSchemeInfo.begin();
    for (; iter != m_vctSchemeInfo.end(); ++iter)
    {
        if ((*iter)->m_dwMatchTypeID == dwMatchTypeID)
        {
            return (*iter);
        }
    }
    return nullptr;
}

SSchemeComputerSumSkill* CSchemeMatchRoom::getSchemeComputerSumSkill(int nHeroID)
{
    return nullptr;
}

CSchemeMatchRoom::VCT_SchemeInfo CSchemeMatchRoom::ms_NullSchemeInfo;