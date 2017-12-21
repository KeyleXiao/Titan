#include "Stdafx.h"
#include <string>
#include "SchemeSystemMessage.h"

CSchemeSystemMessage::CSchemeSystemMessage(void)
{

}

CSchemeSystemMessage::~CSchemeSystemMessage(void)
{

}

// 载入脚本
bool CSchemeSystemMessage::LoadScheme()
{
    ISchemeEngine* pSchemeEngine = gClientGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return false;
    }

    string stringPath = SCP_PATH_FORMAT( SYSTEMMESSAGE_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink*)this);
    if ( !bResult )
    {
        ErrorLn(__FUNCTION__":config file load failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}

// 关闭
void CSchemeSystemMessage::Close(void)
{
    m_mapScheme.clear();
}


/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
// CSV配置载入时通知
bool CSchemeSystemMessage::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if ( pCSVReader == 0 || reader.type != READER_CSV)
        return false;

    m_mapScheme.clear();

    int nLen = 0;
    char szDesc[SYSTEMMESSAGE_MAX_LEN] = {0};
	char szTipPos[128] = {0};

    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for ( int nRow = 0; nRow < nRecordCount; ++nRow )
    {
        SSchemeSystemMessage node;

        // 使用标识 EMTEXT_USE_FLAG
        node.nFlag = pCSVReader->GetInt(nRow, COLUNM_FLAG, 0);
        // 排除非客户端记录
        if( (node.nFlag & CHAT_TEXTUSEFLAG_CLIENT) != CHAT_TEXTUSEFLAG_CLIENT )
            continue;

		// 消息Id
        node.nId = pCSVReader->GetInt(nRow, COLUNM_ID, 0);
        // 文字描述
        nLen = sizeof(szDesc);
        pCSVReader->GetString(nRow, COLUNM_TEXT, szDesc, nLen);
        node.strDesc = szDesc;
		// 把'符号自动替换成物件开始符
		size_t nPos = node.strDesc.find('\'', 0);
		while (nPos != string::npos)
		{
			node.strDesc.replace(nPos, 1, 1, (char)21);
			nPos = node.strDesc.find('\'', nPos + 1);
		}
		// 显示类型
        node.nType = pCSVReader->GetInt(nRow, COLUNM_SHOWTYPE, 0);
        // 显示位置
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
        // 文字显示时间（单位ms）
        node.nShowTime = pCSVReader->GetInt(nRow, COLUNM_SHOWTIME, 0);
		
		// 窗口标题
		nLen = sizeof(szDesc);
		pCSVReader->GetString(nRow, COLUNM_TITLE, szDesc, nLen);
		node.strTitle = szDesc;

        m_mapScheme[node.nId] = node;
    }
    return true;
}

	
// 配置动态更新时通知
bool CSchemeSystemMessage::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return OnSchemeLoad(reader, szFileName);
}

/////////////////////////////////ISchemeSystemMessage/////////////////////////////////////////
// 取得配置信息
SSchemeSystemMessage* CSchemeSystemMessage::getSystemMessageInfo(int nId)
{
    std::map<int, SSchemeSystemMessage>::iterator iter = m_mapScheme.find(nId);
    if(iter == m_mapScheme.end())
    {
        WarningLn(__FUNCTION__": not found item, check scheme file config! id="<<nId);
        return NULL;
    }

    return &(iter->second);
}
