/*******************************************************************
** 文件名:	SchemeHeroCardGradeColor.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	2016-6-13
** 版  本:	1.0
** 描  述:	英雄卡牌等级颜色配置 HeroCardGradeColorConfig.csv

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "SchemeHeroCardGradeColor.h"
#include "IClientGlobal.h"
#include "SpellDef.h"
#include <string>
using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemeHeroCardGradeColor::CSchemeHeroCardGradeColor(void)
{	

}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeHeroCardGradeColor::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载csv脚本
    string stringPath = SCP_PATH_FORMAT( HEROCARDGRADECOLOR_SCHEME_FILENAME );
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
void CSchemeHeroCardGradeColor::Close(void)
{	
	// 清空
	m_mapHeroCardGradeColor.clear();
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
bool CSchemeHeroCardGradeColor::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;

	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
	m_mapHeroCardGradeColor.clear();

    int nLen = 0;

	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
		SHeroCardGradeColorScheme node;

		// 卡牌等级
		node.nCardGrade		= pCSVReader->GetInt(nRow, COL_GRADE, 0);

        // 卡牌颜色
        nLen = sizeof(node.szCardColor) / sizeof(char);
        pCSVReader->GetString(nRow, COL_COLOR, node.szCardColor, nLen);      

        m_mapHeroCardGradeColor[node.nCardGrade] = node;
	}

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
bool CSchemeHeroCardGradeColor::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}


//////////////////////////////////////////////////////////////////////////
/** 取得指定卡牌的配置信息
@param nCardID: 卡牌ID 
@return  
*/
SHeroCardGradeColorScheme* CSchemeHeroCardGradeColor::getByCardGrade( int nCardGrade )
{
    TMAP_HEROCARDGRADECOLOR::iterator iter = m_mapHeroCardGradeColor.find(nCardGrade);
    if(iter == m_mapHeroCardGradeColor.end())
    {
        return NULL;
    }

    return &(iter->second);
}
