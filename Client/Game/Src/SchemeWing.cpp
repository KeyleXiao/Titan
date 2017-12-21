#include "stdafx.h"
#include "SchemeWing.h"
#include "IClientGlobal.h"

bool CSchemeWing::LoadScheme()
{
    ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载ai配置相关
    string stringPath = SCP_PATH_FORMAT( WING_SCHEME_FILENAME );
    if(!pSchemeEngine->LoadScheme(stringPath.c_str(),this,true))
    {
        ErrorLn("load " << WING_SCHEME_FILENAME << " failed");
        return false;
    }

    return true;
}

void CSchemeWing::Close(void)
{
    m_schemeMap.clear();
}

bool CSchemeWing::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    // 表清除
    m_schemeMap.clear();

    int nLen = 0;
    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow=0; nRow<nRecordCount; ++nRow)
    {
        // 当前列数
        int nCol = 0;

        SWing scheme;
        // ID
        scheme.nID = pCSVReader->GetInt(nRow, nCol++, 0);

        // 名字
        nLen = sizeof(scheme.szName);
        pCSVReader->GetString(nRow, nCol++, scheme.szName, nLen);

        // 描述
        nLen = sizeof(scheme.szDes);
        pCSVReader->GetString(nRow, nCol++, scheme.szDes, nLen);

        // 预制体路径
        nLen = sizeof(scheme.szPath);
        pCSVReader->GetString(nRow, nCol++, scheme.szPath, nLen);

        // 图标
        scheme.nIcon = pCSVReader->GetInt(nRow, nCol++, 0);

        // 速度
        scheme.fSpeed = pCSVReader->GetFloat(nRow, nCol++, 0);

        // 加速度
        scheme.fAcceleration = pCSVReader->GetFloat(nRow, nCol++, 0);

        // 最高速度
        scheme.fMaxSpeed = pCSVReader->GetFloat(nRow, nCol++, 0);

        // 上升速度
        scheme.fUpSpeed = pCSVReader->GetFloat(nRow, nCol++, 0);

        // 最小飞行高度
        scheme.fMinFlyHeight = pCSVReader->GetFloat(nRow, nCol++, 0);

        // 最高飞行高度
        scheme.fMaxFlyHeight = pCSVReader->GetFloat(nRow, nCol++, 0);

        // 镜头最小仰角
        scheme.fMinCameraAngle = pCSVReader->GetFloat(nRow, nCol++, 0);

        // 镜头最大仰角
        scheme.fMaxCameraAngle = pCSVReader->GetFloat(nRow, nCol++, 0);

        // 飞行需要类型
        scheme.nFlyNeedType = pCSVReader->GetInt(nRow, nCol++, 0);

        // 起飞需要体力
        scheme.nFlyNeedStamina = pCSVReader->GetInt(nRow, nCol++, 0);

        // 飞行消耗体力
        scheme.nFlyCostStamina = pCSVReader->GetInt(nRow, nCol++, 0);

        // 加速飞行需要体力
        scheme.nAccelerationNeedStamina = pCSVReader->GetInt(nRow, nCol++, 0);

        // 加速飞行消耗体力
        scheme.nAccelerationCostStamina = pCSVReader->GetInt(nRow, nCol++, 0);

        // 退出加速飞行体力
        scheme.nExitAccelerationStamina = pCSVReader->GetInt(nRow, nCol++, 0);

        // 降落体力
        scheme.nFloorStamina = pCSVReader->GetInt(nRow, nCol++, 0);

        // 起飞技能ID
        scheme.nStartFlySpellID = pCSVReader->GetInt(nRow, nCol++, 0);

        // 允许加速
        scheme.nAllowAcceleration = pCSVReader->GetInt(nRow, nCol++, 0);

        // 加速技能ID
        scheme.nAccelerationID = pCSVReader->GetInt(nRow, nCol++, 0);

        // 飞行类型
        scheme.nFlyType = pCSVReader->GetInt(nRow, nCol++, 0);

        // 飞行时间
        scheme.nFlyTime = pCSVReader->GetInt(nRow, nCol++, 0);

        // 是否显示进度条
        scheme.bShowProgressBar = pCSVReader->GetBool(nRow, nCol++, 0);

        // 请求降落增加buff
        char szBuf[256];
        int nLen = sizeof(szBuf);
        memset(szBuf, 0, nLen);
        if(pCSVReader->GetString(nRow, nCol++, szBuf, nLen))
        {
            std::vector<string> vecItem;
            StringHelper::split(vecItem, szBuf, ';');
            if(vecItem.size() == 2)
            {
                scheme.nReqFloorAddBuffID = StringHelper::toInt(vecItem[0]);
                scheme.nReqFloorAddBuffLevel = StringHelper::toInt(vecItem[1]); 
            }
        }

        // 遭遇距离
        scheme.nEncounterDistance = pCSVReader->GetInt(nRow, nCol++, 0);

        // 被击掉落
        scheme.bFloorOnBeHit = pCSVReader->GetBool(nRow, nCol++, 0);

        // 戴上翅膀附加BUFF
        memset(szBuf,0,sizeof(szBuf));
        nLen = sizeof(szBuf);
        pCSVReader->GetString(nRow, nCol++, szBuf, nLen);
        parseIntArray(szBuf, scheme.nLoadWingBuffs, _countof(scheme.nLoadWingBuffs), ';');

        // 卸装翅膀附加BUFF
        memset(szBuf,0,sizeof(szBuf));
        nLen = sizeof(szBuf);
        pCSVReader->GetString(nRow, nCol++, szBuf, nLen);
        parseIntArray(szBuf, scheme.nUnloadWingBuffs, _countof(scheme.nLoadWingBuffs), ';');


        // 基础属性表插入
        pair<SCHEME_MAP::iterator, bool> result = m_schemeMap.insert(SCHEME_MAP::value_type(scheme.nID, scheme));	
        if (!result.second)
        {
            ErrorLn("SchemeWing There is repeated,nID="<<scheme.nID);
            return false;
        }
    }

    return true;
}

bool CSchemeWing::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return OnSchemeLoad(reader,szFileName);
}

SWing* CSchemeWing::getWing(int nID)
{
    SCHEME_MAP::iterator Iter = m_schemeMap.find(nID);
    if (Iter == m_schemeMap.end())
    {
        return NULL;
    }

    return &Iter->second;
}
