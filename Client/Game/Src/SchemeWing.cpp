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

    // ����ai�������
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

    // �����
    m_schemeMap.clear();

    int nLen = 0;
    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow=0; nRow<nRecordCount; ++nRow)
    {
        // ��ǰ����
        int nCol = 0;

        SWing scheme;
        // ID
        scheme.nID = pCSVReader->GetInt(nRow, nCol++, 0);

        // ����
        nLen = sizeof(scheme.szName);
        pCSVReader->GetString(nRow, nCol++, scheme.szName, nLen);

        // ����
        nLen = sizeof(scheme.szDes);
        pCSVReader->GetString(nRow, nCol++, scheme.szDes, nLen);

        // Ԥ����·��
        nLen = sizeof(scheme.szPath);
        pCSVReader->GetString(nRow, nCol++, scheme.szPath, nLen);

        // ͼ��
        scheme.nIcon = pCSVReader->GetInt(nRow, nCol++, 0);

        // �ٶ�
        scheme.fSpeed = pCSVReader->GetFloat(nRow, nCol++, 0);

        // ���ٶ�
        scheme.fAcceleration = pCSVReader->GetFloat(nRow, nCol++, 0);

        // ����ٶ�
        scheme.fMaxSpeed = pCSVReader->GetFloat(nRow, nCol++, 0);

        // �����ٶ�
        scheme.fUpSpeed = pCSVReader->GetFloat(nRow, nCol++, 0);

        // ��С���и߶�
        scheme.fMinFlyHeight = pCSVReader->GetFloat(nRow, nCol++, 0);

        // ��߷��и߶�
        scheme.fMaxFlyHeight = pCSVReader->GetFloat(nRow, nCol++, 0);

        // ��ͷ��С����
        scheme.fMinCameraAngle = pCSVReader->GetFloat(nRow, nCol++, 0);

        // ��ͷ�������
        scheme.fMaxCameraAngle = pCSVReader->GetFloat(nRow, nCol++, 0);

        // ������Ҫ����
        scheme.nFlyNeedType = pCSVReader->GetInt(nRow, nCol++, 0);

        // �����Ҫ����
        scheme.nFlyNeedStamina = pCSVReader->GetInt(nRow, nCol++, 0);

        // ������������
        scheme.nFlyCostStamina = pCSVReader->GetInt(nRow, nCol++, 0);

        // ���ٷ�����Ҫ����
        scheme.nAccelerationNeedStamina = pCSVReader->GetInt(nRow, nCol++, 0);

        // ���ٷ�����������
        scheme.nAccelerationCostStamina = pCSVReader->GetInt(nRow, nCol++, 0);

        // �˳����ٷ�������
        scheme.nExitAccelerationStamina = pCSVReader->GetInt(nRow, nCol++, 0);

        // ��������
        scheme.nFloorStamina = pCSVReader->GetInt(nRow, nCol++, 0);

        // ��ɼ���ID
        scheme.nStartFlySpellID = pCSVReader->GetInt(nRow, nCol++, 0);

        // �������
        scheme.nAllowAcceleration = pCSVReader->GetInt(nRow, nCol++, 0);

        // ���ټ���ID
        scheme.nAccelerationID = pCSVReader->GetInt(nRow, nCol++, 0);

        // ��������
        scheme.nFlyType = pCSVReader->GetInt(nRow, nCol++, 0);

        // ����ʱ��
        scheme.nFlyTime = pCSVReader->GetInt(nRow, nCol++, 0);

        // �Ƿ���ʾ������
        scheme.bShowProgressBar = pCSVReader->GetBool(nRow, nCol++, 0);

        // ����������buff
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

        // ��������
        scheme.nEncounterDistance = pCSVReader->GetInt(nRow, nCol++, 0);

        // ��������
        scheme.bFloorOnBeHit = pCSVReader->GetBool(nRow, nCol++, 0);

        // ���ϳ�򸽼�BUFF
        memset(szBuf,0,sizeof(szBuf));
        nLen = sizeof(szBuf);
        pCSVReader->GetString(nRow, nCol++, szBuf, nLen);
        parseIntArray(szBuf, scheme.nLoadWingBuffs, _countof(scheme.nLoadWingBuffs), ';');

        // жװ��򸽼�BUFF
        memset(szBuf,0,sizeof(szBuf));
        nLen = sizeof(szBuf);
        pCSVReader->GetString(nRow, nCol++, szBuf, nLen);
        parseIntArray(szBuf, scheme.nUnloadWingBuffs, _countof(scheme.nLoadWingBuffs), ';');


        // �������Ա����
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
