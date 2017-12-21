#include "Stdafx.h"
#include <string>
#include "IServerGlobal.h"
#include "SchemeSystemMessage.h"

CSchemeSystemMessage::CSchemeSystemMessage(void)
{

}

CSchemeSystemMessage::~CSchemeSystemMessage(void)
{

}

// ����ű�
bool CSchemeSystemMessage::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    // ���ؽű�
    string stringPath = SCP_PATH_FORMAT(SYSTEMMESSAGE_SCHEME_FILENAME);
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if (!bResult)
    {
        ErrorLn("config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// �ر�
void CSchemeSystemMessage::Close(void)
{
    m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV��������ʱ֪ͨ
bool CSchemeSystemMessage::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == NULL || reader.type != READER_CSV)
        return false;

    m_mapScheme.clear();

    int nLen = 0;
	char szDesc[SYSTEMMESSAGE_MAX_LEN] = {0};
	char szTipPos[128] = {0};

    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
        SSchemeSystemMessage node;

        // ʹ�ñ�ʶ EMTEXT_USE_FLAG
        node.nFlag = pCSVReader->GetInt(nRow, COLUNM_FLAG, 0);
        // �ų��ǳ�������¼
        if( (node.nFlag & CHAT_TEXTUSEFLAG_SCENE) != CHAT_TEXTUSEFLAG_SCENE)
            continue;

        // ��ϢId
        node.nId = pCSVReader->GetInt(nRow, COLUNM_ID, 0);
        // ��������
        nLen = sizeof(szDesc);
        pCSVReader->GetString(nRow, COLUNM_TEXT, szDesc, nLen);
        node.strDesc = szDesc;
        // ��ʾ����
        node.nType = pCSVReader->GetInt(nRow, COLUNM_SHOWTYPE, 0);
		// ��ʾλ��
		nLen = sizeof(szTipPos);
		pCSVReader->GetString(nRow, COLUNM_SHOWPOS, szTipPos, nLen);
		const int nMaxShowPosCount = 8;
		int posList[nMaxShowPosCount] = {0};
		int nCount = parseIntArray(szTipPos, posList, nMaxShowPosCount, ';');
		if (nCount > 0)
		{
			for (int i = 0; i < nCount; ++i)
			{
				if (posList[i] > 0)
					node.nPos |= (1 << (posList[i] - 1));
			}
		}
        // ������ʾʱ�䣨��λms��
        node.nShowTime = pCSVReader->GetInt(nRow, COLUNM_SHOWTIME, 0);

        m_mapScheme[node.nId] = node;
    }
    return true;
}

	
// ���ö�̬����ʱ֪ͨ
bool CSchemeSystemMessage::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeSystemMessage* pNewInstance = NewInstance();
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

/////////////////////////////////ISchemeSystemMessage/////////////////////////////////////////
// ȡ��������Ϣ
SSchemeSystemMessage* CSchemeSystemMessage::getSystemMessageInfo(int nId)
{
    std::map<int, SSchemeSystemMessage>::iterator iter = m_mapScheme.find(nId);
    if (iter == m_mapScheme.end())
    {
        WarningLn(__FUNCTION__": not found item, check scheme file config! id="<<nId);
        return NULL;
    }

    return &(iter->second);
}
