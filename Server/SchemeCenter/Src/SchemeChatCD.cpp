/*******************************************************************
** �ļ���:	CSchemeChatCD.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/1/3
** ��  ��:	1.0
** ��  ��:	
            ����CDʱ��
********************************************************************/


#include "Stdafx.h"
#include <string>
#include "IServerGlobal.h"
#include "SchemeChatCD.h"

CSchemeChatCD::CSchemeChatCD(void)
{

}

CSchemeChatCD::~CSchemeChatCD(void)
{

}

// ����ű�
bool CSchemeChatCD::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // ���ؽű�
    string stringPath = SCP_PATH_FORMAT(CHATCD_SCHEME_FILENAME);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// �ر�
void CSchemeChatCD::Close(void)
{
    m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV��������ʱ֪ͨ
bool CSchemeChatCD::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_mapScheme.clear();

    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
		SSchemeChatCD node;

		// Ƶ��ID
		node.nChannel = pCSVReader->GetInt(nRow, COLNUM_CHANNEL, -1);
		if (node.nChannel < 0)
		{
			continue;
		}

		// �ȼ�����
		node.nLimitLevel = pCSVReader->GetInt(nRow, COLNUM_LEVEL, 1);

		// ��ȴʱ��
		node.nCDTime = pCSVReader->GetInt(nRow, COLNUM_TIME, 0);

        m_mapScheme[node.nChannel] = node;
    }

	AfterSchemeLoaded();

    return true;
}

	
// ���ö�̬����ʱ֪ͨ
bool CSchemeChatCD::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeChatCD* pNewInstance = NewInstance();
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


int CSchemeChatCD::getLimitLevel( int nChannel )
{
	std::map<int, SSchemeChatCD>::iterator iter = m_mapScheme.find(nChannel);
	if (iter == m_mapScheme.end())
	{
		WarningLn(__FUNCTION__": not found item, check scheme file config! channel="<<nChannel);
		return 0;
	}

	return iter->second.nLimitLevel;
}

int CSchemeChatCD::getCDTime( int nChannel )
{
	std::map<int, SSchemeChatCD>::iterator iter = m_mapScheme.find(nChannel);
	if (iter == m_mapScheme.end())
	{
		WarningLn(__FUNCTION__": not found item, check scheme file config! channel="<<nChannel);
		return 0;
	}

	return iter->second.nCDTime;
}

