/*******************************************************************
** 文件名:	SchemeLifeHero.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	王全申
** 日  期:	2017-9-13
** 版  本:	1.0
** 描  述:	英雄卡牌配置 LifeHero.csv、LifeHeroLight.csv、LifeHeroPrize.csv

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "SchemeLifeHero.h"
#include "IServerGlobal.h"
#include "SpellDef.h"
#include <string>

using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemeLifeHero::CSchemeLifeHero(void)
{	

}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeLifeHero::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载csv脚本
    string stringPath = SCP_PATH_FORMAT( LIFEHERO_PRIZE_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		ErrorLn("config file load failed! filename=" << stringPath.c_str());
		return false;
	}

	// 加载csv脚本
	stringPath = SCP_PATH_FORMAT(LIFEHERO_CONFIG_SCHEME_FILENAME);
	bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if (!bResult)
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
void CSchemeLifeHero::Close(void)
{	
	// 清空
	m_vecPrize.clear();
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
bool CSchemeLifeHero::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;

	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	bool isSuccess = false;
	if ( rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(LIFEHERO_PRIZE_SCHEME_FILENAME)) == 0 )
	{
		isSuccess = LoadPrizeConfig(pCSVReader);
	}
	else if ( rkt::StringHelper::casecmp(szFileName, SCP_PATH_FORMAT(LIFEHERO_CONFIG_SCHEME_FILENAME)) == 0)
	{
		isSuccess = LoadLifeHeroConfig(pCSVReader);
	}
	else
	{
		return false;
	}

	if (!isSuccess)
		return false;

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
bool CSchemeLifeHero::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeLifeHero* pNewInstance = NewInstance();
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

bool CSchemeLifeHero::LoadPrizeConfig(ICSVReader * pCSVReader)
{
	if (!pCSVReader)
		return false;

	// 清空
	m_vecPrize.clear();

	// 读取
	int idArray[10];
	char prizeIDs[128] = {0};
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow = 0; nRow<nRecordCount; ++nRow)
	{
		SLifeHeroPrizeScheme node;
		int col = 0;
		node.targetID = pCSVReader->GetInt(nRow, col++, 0);
		node.heroCount = pCSVReader->GetInt(nRow, col++, 0);
		node.starLv = pCSVReader->GetInt(nRow, col++, 0);
		prizeIDs[0] = '\0';
		int len = sizeof(prizeIDs) / sizeof(char);
		if (pCSVReader->GetString(nRow, col++, prizeIDs, len))
		{
			int count = parseIntArray(prizeIDs, idArray, 10);
			for (int i = 0; i < count; ++i)
				node.prizeIDVec.push_back(idArray[i]);
		}
		else
		{
			ErrorLn("本命英雄获取奖励ID失败 = " << LIFEHERO_PRIZE_SCHEME_FILENAME);
			return false;
		}

		m_vecPrize.push_back(node);
	}

	return true;
}

bool CSchemeLifeHero::LoadLifeHeroConfig(ICSVReader * pCSVReader)
{
	if (!pCSVReader)
		return false;

	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	if (nRecordCount != ELifeHeroType::LIFEHERO_MAX_COUNT)
	{
		ErrorLn("本命英雄脚本 = " << LIFEHERO_CONFIG_SCHEME_FILENAME << ",配置有误,行数=" <<  nRecordCount << ",不等于本命英雄个数" << ELifeHeroType::LIFEHERO_MAX_COUNT);
		return false;
	}

	for (int nRow = 0; nRow < nRecordCount; ++nRow)
	{
		SLifeHeroScheme node;
		int col = 1;
		char warTypes[64];
		memset(warTypes, 0, sizeof(warTypes));
		int len = sizeof(warTypes) / sizeof(char);
		if (pCSVReader->GetString(nRow, col++, warTypes, len))
		{
			node.szWarTypeID = warTypes;
		}
		else
		{
			ErrorLn("本命英雄获取 = " << LIFEHERO_CONFIG_SCHEME_FILENAME);
			return false;
		}
		node.rule = pCSVReader->GetInt(nRow, col++, 0);
		node.starLv = pCSVReader->GetInt(nRow, col++, 0);
		node.prizeID = pCSVReader->GetInt(nRow, col++, 0);
		node.goldCount = pCSVReader->GetInt(nRow, col++, 0);
		node.cardCount = pCSVReader->GetInt(nRow, col++, 0);

		m_lifeHeroConfig[nRow] = node;
	}

	return true;
}