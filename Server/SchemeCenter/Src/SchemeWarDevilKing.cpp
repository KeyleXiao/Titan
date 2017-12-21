/*******************************************************************
** �ļ���:	SchemeWarDevilKing.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/11/30
** ��  ��:	1.0
** ��  ��:   ս��ħ���淨����
********************************************************************/


#include "Stdafx.h"
#include <string>
#include "IServerGlobal.h"
#include "SchemeWarDevilKing.h"
#include "SchemeWarDef.h"

CSchemeWarDevilKing::CSchemeWarDevilKing(void)
{

}

CSchemeWarDevilKing::~CSchemeWarDevilKing(void)
{

}

// ����ű�
bool CSchemeWarDevilKing::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // ���ؽű�
    string stringPath = SCP_PATH_FORMAT(WAR_SCHEME_DEVILKING);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// �ر�
void CSchemeWarDevilKing::Close(void)
{
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV��������ʱ֪ͨ
bool CSchemeWarDevilKing::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;


    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {

		SWarDevilKing& node = m_Scheme;

		int nCol = 0;
		node.nKillNumCond = pCSVReader->GetInt(nRow, nCol++, 0);
		node.nDevilKingBuff = pCSVReader->GetInt(nRow, nCol++, 0);
		node.nCloseTime = pCSVReader->GetInt(nRow, nCol++, 0) * 1000; // ת���ɺ���
		node.nBcastTime = pCSVReader->GetInt(nRow, nCol++, 0) * 1000; // ת���ɺ���

		// ��ʱֻ��һ������
		break;
    }

	AfterSchemeLoaded();

    return true;
}

	
// ���ö�̬����ʱ֪ͨ
bool CSchemeWarDevilKing::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeWarDevilKing* pNewInstance = NewInstance();
    if (pNewInstance == NULL)
    {
        return false;
    }

    if (pNewInstance->OnSchemeLoad(reader, szFileName))
    {
        return true;
    }
    else
    {
        safeDelete(pNewInstance);
        return false;
    }
}

SWarDevilKing * CSchemeWarDevilKing::getWarDevilKing()
{
	return &m_Scheme;
}

