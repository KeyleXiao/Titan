#include "stdafx.h"
#include "SchemeMinimap.h"
#include "IClientGlobal.h"

#define MINIMAP_INFO_FILENAME "Scp\\MinimapInfo.csv"
#define MINIMAP_ICON_FILENAME "Scp\\MinimapIcon.csv"
#define MINIMAP_STATIC_ICON_FILENAME "Scp\\MinimapStaticIcon.csv"

bool CMinimapScheme::Load()
{
    ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }

    // 载入小地图脚本
    string stringPath = SCP_PATH_FORMAT( MINIMAP_INFO_FILENAME );
    if(!pSchemeEngine->LoadScheme(stringPath.c_str(),this,true))
    {
        Error("config file load failed! filename=" << stringPath.c_str() << endl);
        return false;
    }

    // 载入小地图图标脚本
    stringPath = SCP_PATH_FORMAT( MINIMAP_ICON_FILENAME );
    if(!pSchemeEngine->LoadScheme(stringPath.c_str(),this,true))
    {
        Error("config file load failed! filename=" << stringPath.c_str() << endl);
        return false;
    }

    // 载入静态打点脚本
    stringPath = SCP_PATH_FORMAT( MINIMAP_STATIC_ICON_FILENAME );
    if(!pSchemeEngine->LoadScheme(stringPath.c_str(),this,true))
    {
        Error("config file load failed! filename=" << stringPath.c_str() << endl);
        return false;
    }

    return true;
}

bool CMinimapScheme::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    string strMapInfo = SCP_PATH_FORMAT( MINIMAP_INFO_FILENAME );
    string strIconInfo = SCP_PATH_FORMAT( MINIMAP_ICON_FILENAME );
    string strStaticInfo = SCP_PATH_FORMAT( MINIMAP_STATIC_ICON_FILENAME );

    if(strcmp(szFileName, strMapInfo.c_str()) == 0)
    {
        return LoadMinimapInfo(reader.pCSVReader);
    }
    else if(strcmp(szFileName, strIconInfo.c_str()) == 0)
    {
        return LoadMinimapIcon(reader.pCSVReader);
    }
    else if(strcmp(szFileName, strStaticInfo.c_str()) == 0)
    {
        return LoadMinimapStaticIcon(reader.pCSVReader);
    }

    return false;
}

bool CMinimapScheme::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return OnSchemeLoad(reader, szFileName);
}

bool CMinimapScheme::LoadMinimapInfo(ICSVReader* pCsvReader)
{
    // 读取
    int nRecordCount = pCsvReader->GetRecordCount();
    for(int nRow = 0; nRow < nRecordCount; ++nRow)
    {
        int nCol = 0;

        SMinimapInfo sInfo;

        // 小地图ID
        sInfo.nMapID = pCsvReader->GetInt(nRow, nCol++, 0);

        // 无视野地图文件
        int nLen = sizeof(sInfo.szDarkBg);
        pCsvReader->GetString(nRow, nCol++, sInfo.szDarkBg, nLen);

        // 有视野地图文件
        nLen = sizeof(sInfo.szLightBg);
        pCsvReader->GetString(nRow, nCol++, sInfo.szLightBg, nLen);

        // 小地图底图信息
        nLen = sizeof(sInfo.szBaseMap);
        pCsvReader->GetString(nRow, nCol++, sInfo.szBaseMap, nLen );

        // 小地图左边框
        sInfo.fLeft = pCsvReader->GetFloat(nRow, nCol++);

        // 小地图右边框
        sInfo.fRight = pCsvReader->GetFloat(nRow, nCol++);

        // 小地图上边框
        sInfo.fTop = pCsvReader->GetFloat(nRow, nCol++);

        // 小地图下边框
        sInfo.fBottom = pCsvReader->GetFloat(nRow, nCol++);

        // 默认旋转信息
        nLen = sizeof(sInfo.szDefaultRotation);
        pCsvReader->GetString(nRow, nCol++, sInfo.szDefaultRotation, nLen);

        // 原始旋转角度
        sInfo.nOriRotation = pCsvReader->GetInt(nRow, nCol++); 

		// 警告按钮位置
		nLen = sizeof(sInfo.szAlertBtnPos);
		pCsvReader->GetString(nRow, nCol++, sInfo.szAlertBtnPos, nLen);

		// 标记按钮位置
		nLen = sizeof(sInfo.szMarkBtnPos);
		pCsvReader->GetString(nRow, nCol++, sInfo.szMarkBtnPos, nLen);

        // 插入小地图信息表
        m_mapMinimap[sInfo.nMapID] = sInfo;
    }
    return true;
}

bool CMinimapScheme::LoadMinimapIcon(ICSVReader* pCsvReader)
{
    // 读取
    int nRecordCount = pCsvReader->GetRecordCount();
    for(int nRow = 0; nRow < nRecordCount; ++nRow)
    {
        int nCol = 0;

        SMinimapIcon sIcon;

        // 图标表示职业
        sIcon.nIcon = pCsvReader->GetInt(nRow, nCol++);

        // 是否建筑物
        sIcon.nIsTower = pCsvReader->GetInt(nRow, nCol++);

        // 友方前景
        int nLen = sizeof(sIcon.szFriendCampFg);
        pCsvReader->GetString(nRow, nCol++, sIcon.szFriendCampFg, nLen);

        // 友方背景
        nLen = sizeof(sIcon.szFriendCampBg);
        pCsvReader->GetString(nRow, nCol++, sIcon.szFriendCampBg, nLen);

        // 敌方前景
        nLen = sizeof(sIcon.szEnemyCampFg);
        pCsvReader->GetString(nRow, nCol++, sIcon.szEnemyCampFg, nLen);

        // 敌方背景
        nLen = sizeof(sIcon.szEnemyCampBg);
        pCsvReader->GetString(nRow, nCol++, sIcon.szEnemyCampBg, nLen);

        // 中立方前景
        nLen = sizeof(sIcon.szMidCampFg);
        pCsvReader->GetString(nRow, nCol++, sIcon.szMidCampFg, nLen);

        // 中立方背景
        nLen = sizeof(sIcon.szMidCampBg);
        pCsvReader->GetString(nRow, nCol++, sIcon.szMidCampBg, nLen);

        // 主角前景
        nLen = sizeof(sIcon.szMainHeroFg);
        pCsvReader->GetString(nRow, nCol++, sIcon.szMainHeroFg, nLen);

        // 主角背景
        nLen = sizeof(sIcon.szMainHeroBg);
        pCsvReader->GetString(nRow, nCol++, sIcon.szMainHeroBg, nLen);

        sIcon.nShowType=pCsvReader->GetInt(nRow,nCol++);
        // 插入表
        m_mapMinimapIcon[sIcon.nIcon] = sIcon;
    }
    return true;
}

bool CMinimapScheme::LoadMinimapStaticIcon(ICSVReader* pCsvReader)
{
    // 读取
    int nRecordCount = pCsvReader->GetRecordCount();
    for(int nRow = 0; nRow < nRecordCount; ++nRow)
    {
        int nCol = 0;

        SMinimapStaticIcon sIcon;

        // 地图ID
        int nMapID = pCsvReader->GetInt(nRow, nCol++);

        // 序号
        sIcon.nIndex = pCsvReader->GetInt(nRow, nCol++);

        // 自动创建
        sIcon.nAutoCreate = (bool)(pCsvReader->GetInt(nRow, nCol++));

        // 名称
        int nLen = sizeof(sIcon.szName); 
        pCsvReader->GetString(nRow, nCol++, sIcon.szName, nLen);

        // 图标
        nLen = sizeof(sIcon.szIconName);
        pCsvReader->GetString(nRow, nCol++, sIcon.szIconName, nLen);
        
        // x坐标
        sIcon.nX = pCsvReader->GetInt(nRow, nCol++);

        // z坐标
        sIcon.nZ = pCsvReader->GetInt(nRow, nCol++);

        // tips字符串
        nLen = sizeof(sIcon.szTipStr);
        pCsvReader->GetString(nRow, nCol++, sIcon.szTipStr, nLen);

        // tips背景颜色
        char szBgColor[256] = {0};
        nLen = sizeof(szBgColor);
        pCsvReader->GetString(nRow, nCol++, szBgColor, nLen);
        char* pEnd = NULL;
        sIcon.nTipBgColor = strtol(szBgColor, &pEnd, 16);

        // 保存
        m_mapStaticIcon[nMapID][sIcon.nIndex] = sIcon;
    }
    return true;
}

// 获取小地图信息
void* CMinimapScheme::GetMinimapInfo(int nMapID)
{
	nMapID = FormatMap(nMapID);

    std::map<int, SMinimapInfo>::iterator it = m_mapMinimap.find(nMapID);
    if(it == m_mapMinimap.end())
    {
        return 0;
    }

    return &it->second;
}

// 获取小地图图标信息
void* CMinimapScheme::GetMinimapIconInfo(int nIconID)
{
    std::map<int, SMinimapIcon>::iterator it = m_mapMinimapIcon.find(nIconID);
    if(it == m_mapMinimapIcon.end())
    {
        return 0;
    }

    return &it->second;
}

// 根据地图ID、静态打点序号 获得小地图静态打点信息
void* CMinimapScheme::GetMinimapStaticIcon(int nMapID, int nIndex)
{
	nMapID = FormatMap(nMapID);

	std::map<int, std::map<int, SMinimapStaticIcon>>::iterator it = m_mapStaticIcon.find(nMapID);
    if(it == m_mapStaticIcon.end())
    {
        return 0;
    }

    std::map<int, SMinimapStaticIcon>::iterator it1 = it->second.find(nIndex);
    if(it1 == it->second.end())
    {
        return 0;
    }

    return &it1->second;
}

// 根据序号，获取下一个静态打点信息
void* CMinimapScheme::GetNextMinimapStaticIcon(int nMapID, int nFromIndex)
{
	nMapID = FormatMap(nMapID);

	std::map<int, std::map<int, SMinimapStaticIcon>>::iterator it = m_mapStaticIcon.find(nMapID);
    if(it == m_mapStaticIcon.end())
    {
        return 0;
    }

    std::map<int, SMinimapStaticIcon>::iterator itNext = it->second.upper_bound(nFromIndex);
    if(itNext == it->second.end())
    {
        return 0;
    }

    return &itNext->second;
}