/*******************************************************************
** 文件名:	SchemeHeroCard.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-5-10
** 版  本:	1.0
** 描  述:	英雄卡牌配置 HeroCard.csv

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "SchemeHeroCard.h"
#include "IServerGlobal.h"
#include "SpellDef.h"
#include <string>
using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemeHeroCard::CSchemeHeroCard(void)
{	

}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeHeroCard::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载csv脚本
    string stringPath = SCP_PATH_FORMAT( HEROCARD_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		ErrorLn("config file load failed! filename=" << stringPath.c_str());
		return false;
	}

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeHeroCard::Close(void)
{	
	// 清空
	m_mapHeroCard.clear();
}

/** CSV配置载入时通知
@param   pCSVReader：读取CSV的返回接口
@param   szFileName：配置文件名
@param   
@return  
@note     
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/ 
bool CSchemeHeroCard::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;

	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
	m_mapHeroCard.clear();

    int nLen = 0;

    const int HERO_ARRAY_SIZE = 256;
    // 使用卡牌目标英雄ID
    char szArrayHeroID[HERO_ARRAY_SIZE] = {0};

	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
		SHeroCardScheme node;

		// 卡牌ID
		node.nCardID		= pCSVReader->GetInt(nRow, COL_CARDID, 0);

        // 卡牌等级
        node.nCardGrade     = pCSVReader->GetInt(nRow, COL_CARDGRADE, 0);

        // 卡牌名称
        nLen = sizeof(node.szCardName) / sizeof(char);
        pCSVReader->GetString(nRow, COL_CARDNAME, node.szCardName, nLen);

        // 卡牌图标
        node.nCardIcon		= pCSVReader->GetInt(nRow, COL_CARDICON, 0);
        // 卡牌外框id
        node.nCardFrameIcon		= pCSVReader->GetInt(nRow, COL_CARDFRAMEICON, 0);
        // 使用卡牌目标英雄ID列表，0为所有英雄
        memset(szArrayHeroID, 0, sizeof(szArrayHeroID));
        nLen = sizeof(szArrayHeroID) / sizeof(char);
        pCSVReader->GetString(nRow, COL_HEROLIST, szArrayHeroID, nLen);

        parseIntArray(szArrayHeroID, node.arrayHeroID, sizeof(node.arrayHeroID)/sizeof(int));

        // 如果第一列为０，将强制为所有英雄公用
        if(node.arrayHeroID[0] == 0)
        {
            for (int i=0; i<CARD_HERO_MAXCOUNT; ++i)
            {
                node.arrayHeroID[i]=VOCATION_MAX;
            }
        }
        
        // 卡牌描述
        nLen = sizeof(node.szRemark) / sizeof(char);
        pCSVReader->GetString(nRow, COL_REMARK, node.szRemark, nLen);

        // 控制标识
        node.nFlag		    = pCSVReader->GetInt(nRow, COL_FLAG, 0);

        m_mapHeroCard[node.nCardID] = node;
	}

	AfterSchemeLoaded();

	return true;
}


/** 配置动态更新时通知
@param   pCSVReader：读取CSV的返回接口
@param   szFileName：配置文件名
@param   
@return  
@note     
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/
bool CSchemeHeroCard::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeHeroCard* pNewInstance = NewInstance();
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
}


//////////////////////////////////////////////////////////////////////////
/** 取得指定卡牌的配置信息
@param nCardID: 卡牌ID 
@return  
*/
SHeroCardScheme* CSchemeHeroCard::getByCardID( int nCardID )
{
    TMAP_HEROCARD::iterator iter = m_mapHeroCard.find(nCardID);
    if(iter == m_mapHeroCard.end())
    {
        return NULL;
    }

    return &(iter->second);
}

/** 取得某个英雄配置的所有卡牌配置信息
@param nHeroID: 英雄ID 
@param arrayCard: 输输出值，所有符合条件的卡牌
@param nArrayCount: 数组大小
@return  
*/
int CSchemeHeroCard::getCardsByHeroID(int nHeroID, const SHeroCardScheme** arrayCard, const int nArrayCount)
{
    if(nHeroID <= VOCATION_INVALID || nHeroID >= VOCATION_MAX)
    {
        return 0;
    }

    int nCount = 0;
    for (TMAP_HEROCARD::iterator iter = m_mapHeroCard.begin(); iter != m_mapHeroCard.end(); ++iter)
    {
        if(nCount >= nArrayCount)
        {
            break;
        }

        SHeroCardScheme & node = iter->second;

        // 全职业公用
        if(node.arrayHeroID[0] == VOCATION_MAX)
        {
            arrayCard[nCount++] = &node;
            continue;
        }

        // 遍历职业
        for (int i=0; i<CARD_HERO_MAXCOUNT; i++)
        {
            if(node.arrayHeroID[i] == nHeroID)
            {
                arrayCard[nCount++] = &node;
                break;
            }
        }
    }

    return nCount;
}


bool CSchemeHeroCard::isHaveThisCard( int nCardID )
{
    TMAP_HEROCARD::iterator iter = m_mapHeroCard.find(nCardID);
    if(iter == m_mapHeroCard.end())
    {
        return false;
    }

    return true;
}
