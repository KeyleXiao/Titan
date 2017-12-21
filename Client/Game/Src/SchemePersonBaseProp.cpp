/*******************************************************************
** 文件名:	SchemePersonBaseProp.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016-3-31
** 版  本:	1.0
** 描  述:	基础属性配置表
********************************************************************/
#include "Stdafx.h"
#include "SchemePersonBaseProp.h"
#include "IClientGlobal.h"

SCHEME_MAP* CSchemePersonBaseProp::getAllHeroInfo()
{
    if ( m_schemeMap.size() == 0)
        return NULL;

    return &m_schemeMap;
}

SPersonBasePropScheme CSchemePersonBaseProp::getOneHeroDetailInfo(int nHeroID)
{
    SCHEME_MAP::iterator it = m_schemeMap.find(nHeroID);
    if ( it == m_schemeMap.end() )
        return SPersonBasePropScheme();
    return it->second;
}

/////////////////////////ISchemeUpdateSink/////////////////////////
bool CSchemePersonBaseProp::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    // 基础属性表清除
    m_schemeMap.clear();

    // 基础属性
    int nPropID[PERSON_BASE_PROP_COUNT] = 
    {
        PROPERTY_BASE_HP, PROPERTY_BASE_MP, PROPERTY_BASE_PA, PROPERTY_BASE_MA, 
        PROPERTY_BASE_PD, PROPERTY_BASE_MD, PROPERTY_BASE_SD, PROPERTY_BASE_PASD, 
        PROPERTY_BASE_AF, PROPERTY_BASE_HP_REPLY, PROPERTY_BASE_MP_REPLY, PROPERTY_MAX_STAMINA

    };

    // 附加百分比属性
    int nAppendPCTPropID[PERSON_APPEND_PCT_PROP_COUNT] = 
    {
        PROPERTY_APPEND_PCT_AF
    };

    // 限制属性
    int nPropLimitID[PERSON_PROP_LIMIT_COUNT] = 
    {		
        PROPERTY_PASD, PROPERTY_AF, PROPERTY_SD, PROPERTY_PCT_COOL_REDUCE,
        PROPERTY_TENACITY, PROPERTY_CRC, PROPERTY_DAMAGE_BONUS,
        PROPERTY_DAMAGE_BONUS_PCT
    };
    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow=0; nRow<nRecordCount; ++nRow)
    {
        // 当前列数
        int nCol = 0;

        SPersonBasePropScheme scheme;
        // 职业
        scheme.nVocation = pCSVReader->GetInt(nRow, nCol++, 0);

        // 第二列是职业名字,方便策划看
        nCol++;

        // 设置属性
        for (int nPropIndex=0; nPropIndex<PERSON_BASE_PROP_COUNT; ++nPropIndex)
        {
            // 设置属性ID
            scheme.sProp[nPropIndex].nPropID = nPropID[nPropIndex];
            // 基础值
            scheme.sProp[nPropIndex].nBase = pCSVReader->GetInt(nRow, nCol++, 0);
            // 成长率
            scheme.sProp[nPropIndex].fGrowRate = pCSVReader->GetFloat(nRow, nCol++, 0);

        }

        // 附加百分比属性
        for (int nPropIndex=0; nPropIndex<PERSON_APPEND_PCT_PROP_COUNT; ++nPropIndex)
        {
            // 设置属性ID
            scheme.sAppendPCTProp[nPropIndex].nPropID = nAppendPCTPropID[nPropIndex];
            // 基础值
            scheme.sAppendPCTProp[nPropIndex].nBase = pCSVReader->GetInt(nRow, nCol++, 0);
            // 成长率
            scheme.sAppendPCTProp[nPropIndex].fGrowRate = pCSVReader->GetFloat(nRow, nCol++, 0);
        }

        // 魔法类型/能量值增加衰减方式
        char szBuf[256];
        memset(szBuf, 0, sizeof(szBuf));
        int nLen = sizeof(szBuf);
        if(pCSVReader->GetString(nRow, nCol++, szBuf, nLen))
        {
            std::vector<string> vecStr;
            StringHelper::split(vecStr, szBuf, '/', " ");
            if(vecStr.size() != 2)
            {
                ErrorLn(__FUNCTION__ << __LINE__ << ", parse faile");
                return false;
            }
            scheme.nMagicType = StringHelper::toInt(vecStr[0]);
            scheme.nEnergyGetMode = StringHelper::toInt(vecStr[1]); 
        }
        else
        {
            ErrorLn(__FUNCTION__ << __LINE__ << ", parse faile");
            return false;
        }

        //设置上下限数值
        for (int nPropIndex=0; nPropIndex<PERSON_PROP_LIMIT_COUNT; ++nPropIndex)
        {
            //设置限制属性ID
            scheme.sLimitProp[nPropIndex].nPropID = nPropLimitID[nPropIndex];
            // 最小值下限
            scheme.sLimitProp[nPropIndex].nMinLimit = pCSVReader->GetInt(nRow, nCol++, 0);
            // 最大值上限
            scheme.sLimitProp[nPropIndex].nMaxLimit = pCSVReader->GetInt(nRow, nCol++, 0);
        }

		//for (int i=0;i<2;++i)
		//{
		//	nCol++;
		//}

        // 瞄准距离
        scheme.nAimDistance = pCSVReader->GetInt(nRow, nCol++, 0);

		// 攻击距离
		scheme.nAttackRange = pCSVReader->GetInt(nRow, nCol++, 0);

        // 基础属性表插入
        pair<SCHEME_MAP::iterator, bool> result = m_schemeMap.insert(SCHEME_MAP::value_type(scheme.nVocation, scheme));	
        if (!result.second)
        {
            ErrorLn("People Base Prop There is repeated,nVocation="<<scheme.nVocation);
            return false;
        }
    }

    return true;
}
bool CSchemePersonBaseProp::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return OnSchemeLoad(reader, szFileName);
}

/////////////////////////CSchemePersonBaseProp//////////////////////////
bool CSchemePersonBaseProp::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载脚本
    string stringPath = SCP_PATH_FORMAT( PERSON_BASE_PROP_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        Error("config file load failed! filename=" << stringPath.c_str() << endl);
        return false;
    }

    return true;
}
void CSchemePersonBaseProp::Close(void)
{

}

CSchemePersonBaseProp::CSchemePersonBaseProp(void)
{

}
CSchemePersonBaseProp::~CSchemePersonBaseProp(void)
{

}