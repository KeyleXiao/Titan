/*******************************************************************
** 文件名:	SchemeTalentPage.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	1/3/2016	
** 版  本:	1.0
** 描  述:	天赋页脚本配置


**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "SchemeTalentPage.h"
#include "IServerGlobal.h"
#include <string>

#ifdef FUNCTION_TALENTPAGE_OPEN
using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemeTalentPage::CSchemeTalentPage(void)
{	

}

/** 
@param   
@param   
@return  
*/
CSchemeTalentPage::~CSchemeTalentPage(void)
{	
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeTalentPage::LoadScheme(void)
{
    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }

    // 加载TalentPage.csv脚本
    string stringPath = SCP_PATH_FORMAT(TALENT_PAGE_CSV);
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
void CSchemeTalentPage::Close(void)
{	
    // 清空
    m_mapTalentScheme.clear();
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
bool CSchemeTalentPage::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    // 清空
    m_mapTalentScheme.clear();

    int nLen = 0;
    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow=0; nRow<nRecordCount; ++nRow)
    {
        int nColumnIndex = 0;
        STalentPageData item;
        // 天赋id
        item.nId = pCSVReader->GetInt(nRow,  nColumnIndex++, 0);
        /// 天赋名称
        nLen = sizeof(item.szName);
        pCSVReader->GetString(nRow,  nColumnIndex++, item.szName, nLen);						
        // 天赋图片id
        item.nIconId = pCSVReader->GetInt(nRow,  nColumnIndex++, 0);				
        // 天赋类型
        item.nTalentType = pCSVReader->GetInt(nRow,  nColumnIndex++, 0);				
        // 天赋等级
        item.nLevel = pCSVReader->GetInt(nRow,  nColumnIndex++, 0);					
        // 天赋前置点
        item.nAllowNum = pCSVReader->GetInt(nRow,  nColumnIndex++, 0);					
        // 最大加点数
        item.nMaxNum = pCSVReader->GetInt(nRow,  nColumnIndex++, 0);		

        // 效果组
        nLen = sizeof(item.szEffectIdArray);
        pCSVReader->GetString(nRow,  nColumnIndex++, item.szEffectIdArray, nLen);
        // 此处后续会加效验：效果id个数和最大加点数目，目前策划未配无法效验
        // 插入到列表中
        m_mapTalentScheme[item.nId] = item;		
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
bool CSchemeTalentPage::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeTalentPage* pNewInstance = NewInstance();
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


/////////////////////////ISchemeTalentPage/////////////////////////
/** 取得怪物配置信息
@param nTalentId:	天赋ID
@return  
*/
STalentPageData* CSchemeTalentPage::getTalentPageSchemeInfo(int nTalentId)
{
    TMAP_TALENTSCHEME::iterator iter = m_mapTalentScheme.find(nTalentId);
    if (iter == m_mapTalentScheme.end())
    {
        return NULL;
    }
    return &(iter->second);
}

TMAP_TALENTSCHEME* CSchemeTalentPage::getTalentPageSchemeInfoMapAll()
{
    return &m_mapTalentScheme;
}


#endif