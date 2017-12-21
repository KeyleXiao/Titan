/*******************************************************************
** �ļ���:	SchemePersonBaseProp.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	2016-3-31
** ��  ��:	1.0
** ��  ��:	�����������ñ�
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

    // �������Ա����
    m_schemeMap.clear();

    // ��������
    int nPropID[PERSON_BASE_PROP_COUNT] = 
    {
        PROPERTY_BASE_HP, PROPERTY_BASE_MP, PROPERTY_BASE_PA, PROPERTY_BASE_MA, 
        PROPERTY_BASE_PD, PROPERTY_BASE_MD, PROPERTY_BASE_SD, PROPERTY_BASE_PASD, 
        PROPERTY_BASE_AF, PROPERTY_BASE_HP_REPLY, PROPERTY_BASE_MP_REPLY, PROPERTY_MAX_STAMINA

    };

    // ���Ӱٷֱ�����
    int nAppendPCTPropID[PERSON_APPEND_PCT_PROP_COUNT] = 
    {
        PROPERTY_APPEND_PCT_AF
    };

    // ��������
    int nPropLimitID[PERSON_PROP_LIMIT_COUNT] = 
    {		
        PROPERTY_PASD, PROPERTY_AF, PROPERTY_SD, PROPERTY_PCT_COOL_REDUCE,
        PROPERTY_TENACITY, PROPERTY_CRC, PROPERTY_DAMAGE_BONUS,
        PROPERTY_DAMAGE_BONUS_PCT
    };
    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow=0; nRow<nRecordCount; ++nRow)
    {
        // ��ǰ����
        int nCol = 0;

        SPersonBasePropScheme scheme;
        // ְҵ
        scheme.nVocation = pCSVReader->GetInt(nRow, nCol++, 0);

        // �ڶ�����ְҵ����,����߻���
        nCol++;

        // ��������
        for (int nPropIndex=0; nPropIndex<PERSON_BASE_PROP_COUNT; ++nPropIndex)
        {
            // ��������ID
            scheme.sProp[nPropIndex].nPropID = nPropID[nPropIndex];
            // ����ֵ
            scheme.sProp[nPropIndex].nBase = pCSVReader->GetInt(nRow, nCol++, 0);
            // �ɳ���
            scheme.sProp[nPropIndex].fGrowRate = pCSVReader->GetFloat(nRow, nCol++, 0);

        }

        // ���Ӱٷֱ�����
        for (int nPropIndex=0; nPropIndex<PERSON_APPEND_PCT_PROP_COUNT; ++nPropIndex)
        {
            // ��������ID
            scheme.sAppendPCTProp[nPropIndex].nPropID = nAppendPCTPropID[nPropIndex];
            // ����ֵ
            scheme.sAppendPCTProp[nPropIndex].nBase = pCSVReader->GetInt(nRow, nCol++, 0);
            // �ɳ���
            scheme.sAppendPCTProp[nPropIndex].fGrowRate = pCSVReader->GetFloat(nRow, nCol++, 0);
        }

        // ħ������/����ֵ����˥����ʽ
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

        //������������ֵ
        for (int nPropIndex=0; nPropIndex<PERSON_PROP_LIMIT_COUNT; ++nPropIndex)
        {
            //������������ID
            scheme.sLimitProp[nPropIndex].nPropID = nPropLimitID[nPropIndex];
            // ��Сֵ����
            scheme.sLimitProp[nPropIndex].nMinLimit = pCSVReader->GetInt(nRow, nCol++, 0);
            // ���ֵ����
            scheme.sLimitProp[nPropIndex].nMaxLimit = pCSVReader->GetInt(nRow, nCol++, 0);
        }

		//for (int i=0;i<2;++i)
		//{
		//	nCol++;
		//}

        // ��׼����
        scheme.nAimDistance = pCSVReader->GetInt(nRow, nCol++, 0);

		// ��������
		scheme.nAttackRange = pCSVReader->GetInt(nRow, nCol++, 0);

        // �������Ա����
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

    // ���ؽű�
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