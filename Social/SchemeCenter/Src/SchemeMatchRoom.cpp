#include "StdAfx.h"
#include "SchemeMatchRoom.h"
#include "ISocialGlobal.h"
#include <string>
#include "XlsValue.h"

bool CSchemeMatchRoom::LoadScheme( void )
{
	ISchemeEngine * pSchemeEngine = gSocialGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL )
	{
		return false;
	}

    string stringPath = SCP_PATH_FORMAT( DMATCHROOM_SELECT_CONTROL_SCHEME_RULE );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		ErrorLn(__FUNCTION__":config file load failed! filename=" << stringPath.c_str());
		return false;
	}

    stringPath = SCP_PATH_FORMAT( DMATCHROOM_SCHEME_FILENAME );
	bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		ErrorLn(__FUNCTION__":config file load failed! filename=" << stringPath.c_str());
		return false;
	}

	
    stringPath = SCP_PATH_FORMAT( DMATCHROOM_SCHEME_COMPUTER_SUM_SKILL );
    bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        ErrorLn(__FUNCTION__":config file load failed! filename=" << stringPath.c_str());
        return false;
    }


	return true;
}

void CSchemeMatchRoom::Close( void )
{
	for (size_t i = 0; i < m_SchemeInfo.size(); ++i)
	{
		delete m_SchemeInfo[i];
	}
	m_SchemeInfo.clear();
	m_Mode2Datas.clear();
	m_Map2Datas.clear();
}


bool CSchemeMatchRoom::OnSchemeUpdate( SCRIPT_READER reader, const char* szFileName )
{
    CSchemeMatchRoom* pNewInstance = NewInstance();
    if(pNewInstance == NULL)
    {
        return false;
    }

    if(pNewInstance->OnSchemeLoad(reader, szFileName))
    {
        return true;
    }
    else
    {
        safeDelete(pNewInstance);
        return false;
    }
    return false;
}

bool CSchemeMatchRoom::OnSchemeLoad( SCRIPT_READER reader, const char* szFileName )
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	bool bRet = false;
	string strRulePath = SCP_PATH_FORMAT( DMATCHROOM_SELECT_CONTROL_SCHEME_RULE );
    string strRoomPath = SCP_PATH_FORMAT( DMATCHROOM_SCHEME_FILENAME );
    string strComputerSumSkill = SCP_PATH_FORMAT( DMATCHROOM_SCHEME_COMPUTER_SUM_SKILL);
	if(rkt::StringHelper::casecmp(szFileName, strRulePath.c_str()) == 0)
	{
		bRet = LoadSelectControlScheme(reader.pCSVReader);
	}

	if(rkt::StringHelper::casecmp(szFileName, strRoomPath.c_str()) == 0)
	{
		bRet = LoadMatchRoomScheme(reader.pCSVReader);
	}

    if(rkt::StringHelper::casecmp(szFileName, strComputerSumSkill.c_str()) == 0)
    {
        bRet = LoadComputerSumSkillScheme(reader.pCSVReader);
    }

	if(!bRet)
	{
		return false;
	}	
	
	AfterSchemeLoaded();
	return true;
}

vector<SSchemeMatchRoom* >& CSchemeMatchRoom::getByMode( uchar ModeID )
{
	GameMode2Schemes::iterator itor = m_Mode2Datas.find( ModeID );
	if (itor != m_Mode2Datas.end())
	{
		return itor->second;
	}
	return ms_NullSchemeInfo;
}

vector<SSchemeMatchRoom* >& CSchemeMatchRoom::getByMap( DWORD MapID )
{
	Map2Schemes::iterator itor = m_Map2Datas.find( MapID );
	if (itor != m_Map2Datas.end())
	{
		return itor->second;
	}
	return ms_NullSchemeInfo;
}

bool CSchemeMatchRoom::getModes(ICSVReader* pCSVReader, int nRow, int nIndex, bool bStates[MatchRoomStates])
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

bool CSchemeMatchRoom::LoadMatchRoomScheme(ICSVReader* pCSVReader)
{
    m_SchemeInfo.clear();
    m_Mode2Datas.clear();
    m_Map2Datas.clear();

	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	m_SchemeInfo.reserve( nRecordCount );
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

		SSchemeMatchRoom* pData = new SSchemeMatchRoom();
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
		pData->m_ReadyGoTime	    = pCSVReader->GetInt(nRow, nIndex++, 0);
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
        pData->m_bShowHeroStarInfo      = (pCSVReader->GetInt(nRow, nIndex++, 0) == 1);

        // 显示升级降级界面
        pData->m_nbShowDownOrUpWnd      = (pCSVReader->GetInt(nRow, nIndex++, 0) == 1);

        // 匹配接受确定时间
        pData->m_nMatchAcceptTime       = pCSVReader->GetInt(nRow, nIndex++, 0);

        // 战场中退出
        pData->m_bCanLogOutInWar        = (pCSVReader->GetInt(nRow, nIndex++, 0) == 1);

        // 最大OB人数
        pData->m_nMaxOBCount            = pCSVReader->GetInt(nRow, nIndex++, 0);
	
        // 随机选择英雄开关
        pData->m_bRandomSelectHero      = (pCSVReader->GetInt(nRow, nIndex++, 0) == 1);
        
        // todo:pt 数据检查

		m_SchemeInfo.push_back( pData );
		m_Mode2Datas[ pData->m_nModeID ].push_back( pData );
		m_Map2Datas[ pData->m_dwMapID ].push_back( pData );
	}

	GameMode2Schemes::iterator itor = m_Mode2Datas.begin();
	for (; itor != m_Mode2Datas.end(); ++itor)
	{
		m_ModeIDs.push_back( itor->first );
	}

	return true;
}

bool CSchemeMatchRoom::LoadSelectControlScheme(ICSVReader* pCSVReader)
{
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow = 0; nRow < nRecordCount; ++nRow)
	{
		int nIndex = 0;
		SSchemeSelectControl* pData = new SSchemeSelectControl();

		// 选择英雄规则ID
		pData->m_nID =  pCSVReader->GetInt(nRow, nIndex++, 0);
		// 限制类型
		pData->m_nType = (EMHeroSelectControlType)pCSVReader->GetInt(nRow, nIndex++, 0);
		// 最大数量
		pData->m_HeroMaxCount = pCSVReader->GetInt(nRow, nIndex++, 0);
		m_SchemeSelectControl[pData->m_nID] = *pData;

        delete pData;
	}
	return true;
}

bool CSchemeMatchRoom::LoadComputerSumSkillScheme(ICSVReader* pCSVReader)
{
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow = 0; nRow < nRecordCount; ++nRow)
    {
        int nIndex = 0;
        SSchemeComputerSumSkill* pData = new SSchemeComputerSumSkill();
        // 英雄ID
        pData->m_nHeroID =  pCSVReader->GetInt(nRow, nIndex++, 0);
       
        // 技能ID
        pData->m_nSkillID = pCSVReader->GetInt(nRow, nIndex++, 0);

        // XP技能ID
        pData->m_nXPSkillID = pCSVReader->GetInt(nRow, nIndex++, 0);
        
        m_SchemeComputerSumSkill[pData->m_nHeroID] = *pData;

        delete pData;

    }
    return true;
}


SSchemeSelectControl* CSchemeMatchRoom::getSelectControlScheme(int nID)
{
	MapSelectControlSchemes::iterator iter = m_SchemeSelectControl.find(nID);
	if (iter == m_SchemeSelectControl.end())
	{
		return nullptr;
	}
	return &(iter->second);
}

SSchemeMatchRoom* CSchemeMatchRoom::getSchemeMatchRoomByMatchTypeID(DWORD dwMatchTypeID)
{
    MatchRoomSchemes::iterator iter = m_SchemeInfo.begin();
    for (; iter != m_SchemeInfo.end(); ++iter)
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
    MapComputerSumSkillSchemes::iterator iter = m_SchemeComputerSumSkill.begin();
    for (; iter != m_SchemeComputerSumSkill.end(); ++iter)
    {
        if (iter->second.m_nHeroID == nHeroID)
        {
            return &(iter->second);
        }
    }
    return nullptr;
}

CSchemeMatchRoom::MatchRoomSchemes CSchemeMatchRoom::ms_NullSchemeInfo;