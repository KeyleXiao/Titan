/*******************************************************************
** 文件名:	SchemeTalentPage.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	1/3/2016	
** 版  本:	1.0
** 描  述:	天赋页效果脚本配置

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "SchemeTalentPageEffect.h"
#include "IServerGlobal.h"
#include <string>
using namespace std;


#ifdef FUNCTION_TALENTPAGE_OPEN
/** 
@param   
@param   
@return  
*/
CSchemeTalentPageEffect::CSchemeTalentPageEffect(void)
{	

}

/** 
@param   
@param   
@return  
*/
CSchemeTalentPageEffect::~CSchemeTalentPageEffect(void)
{	
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeTalentPageEffect::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载TalentPagEffect.csv脚本
	string stringPath = SCP_PATH_FORMAT(TALENT_PAGE_EFFECT_CSV);
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("config file load failed! filename=" << stringPath.c_str() << endl);
		return false;
	}

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeTalentPageEffect::Close(void)
{	
	// 清空
	m_mapTalentEffectScheme.clear();
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
bool CSchemeTalentPageEffect::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
	m_mapTalentEffectScheme.clear();

	int nLen = 0;
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
        int nColumnIndex = 0;
		STalentPageEffect item;
		// 天赋加点id
		item.nEffectId = pCSVReader->GetInt(nRow,  nColumnIndex++, 0);
        /// 天赋效果组
        nLen = sizeof(item.stEffectArray);
        pCSVReader->GetString(nRow,  nColumnIndex++, item.stEffectArray, nLen);						

		// 插入到列表中
		m_mapTalentEffectScheme[item.nEffectId] = item;		
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
bool CSchemeTalentPageEffect::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeTalentPageEffect* pNewInstance = NewInstance();
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


/////////////////////////ISchemeTalentPageEffect/////////////////////////
/** 取得怪物配置信息
@param nTalentArrayId:	效果组ID
@return  
*/
STalentPageEffect* CSchemeTalentPageEffect::getTalentEffectSchemeInfo(int nTalentArrayId)
{
    TMAP_TALENTEFFECTSCHEME::iterator iter = m_mapTalentEffectScheme.find(nTalentArrayId);
    if (iter == m_mapTalentEffectScheme.end())
    {
        return NULL;
    }
    return &(iter->second);
}

#endif