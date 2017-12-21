/*******************************************************************
** 文件名:	SchemeSlotMapedShortcut.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	1/7/2015
** 版  本:	1.0
** 描  述:	快捷键映射

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "SchemeSlotMapedShortcut.h"
#include "IClientGlobal.h"
#include <string>
using namespace std;

#include "SpellDef.h"
#include "EntityViewDef.h"
using namespace SPELL;

/** 
@param   
@param   
@return  
*/
CSchemeSlotMapedShortcut::CSchemeSlotMapedShortcut(void)
{	

}

/** 
@param   
@param   
@return  
*/
CSchemeSlotMapedShortcut::~CSchemeSlotMapedShortcut(void)
{	
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeSlotMapedShortcut::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

    string stringPath = SCP_PATH_FORMAT( SLOT_MAPED_SHORTCUT_SCHEME_FILENAME );
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
void CSchemeSlotMapedShortcut::Close(void)
{	
	m_AllSlotMapedShortcut.clear();
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
bool CSchemeSlotMapedShortcut::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if ( pCSVReader==0 || reader.type!=READER_CSV)
		return false;

	// 清空
    m_AllSlotMapedShortcut.clear();

	int nLen = 0;
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
		SSlotMapedShortcutScheme item;
		ZeroMemory(&item,sizeof(SSlotMapedShortcutScheme));
		int nCol = 0;
		// 槽位索引
		item.nSlotIndex		= pCSVReader->GetInt(nRow, nCol++, 0);

		// 槽位类型
		item.nSlotType		=  pCSVReader->GetInt(nRow, nCol++, 0);

		// 槽位名称
		int nlen = sizeof(item.szKeyName);
		pCSVReader->GetString(nRow, nCol++, item.szKeyName, nlen);

		// 空中限制
		item.nAirCons		=  pCSVReader->GetInt(nRow, nCol++, 0);

        //技能类型
        item.nSkillType     =  pCSVReader->GetInt(nRow, nCol++, 0);

		T_MAP_SLOT_TYPE_MAPED_INFO::iterator iter = m_AllSlotMapedShortcut.find(item.nSlotType);
		if (iter == m_AllSlotMapedShortcut.end())
		{
			T_MAP_SLOT_INDEX_MAPED_INFO tmpSlotIndexMapedInfo;
			tmpSlotIndexMapedInfo.insert(pair<int,SSlotMapedShortcutScheme>(item.nSlotIndex, item));
			m_AllSlotMapedShortcut.insert(pair<int, T_MAP_SLOT_INDEX_MAPED_INFO>(item.nSlotType, tmpSlotIndexMapedInfo));
		}
		else
		{
			iter->second.insert(pair<int,SSlotMapedShortcutScheme>(item.nSlotIndex, item));
		}
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
bool CSchemeSlotMapedShortcut::OnSchemeUpdate(SCRIPT_READER reader,const char* szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}


SSlotMapedShortcutScheme* CSchemeSlotMapedShortcut::getSlotMapedShortcutShemeInfo(int nSlotIndex, int nSlotType)
{

	T_MAP_SLOT_TYPE_MAPED_INFO::iterator iter = m_AllSlotMapedShortcut.find(nSlotType);
	if (iter == m_AllSlotMapedShortcut.end())
	{
		return NULL;
	}
	else
	{
		
		T_MAP_SLOT_INDEX_MAPED_INFO::iterator findIter = iter->second.find(nSlotIndex);
		if (findIter == iter->second.end())
		{
			return NULL;
		}
		else
		{
			return &(findIter->second);
		}
	}
}

int CSchemeSlotMapedShortcut::getSlotAccordSkillType( int nSkillType )
{
    T_MAP_SLOT_TYPE_MAPED_INFO::iterator iter = m_AllSlotMapedShortcut.begin();
    for (; iter != m_AllSlotMapedShortcut.end(); ++iter)
    {

        T_MAP_SLOT_INDEX_MAPED_INFO::iterator iterIndex = iter->second.begin();

        for (; iterIndex != iter->second.end(); ++iterIndex )
        {

            SSlotMapedShortcutScheme slotInfo = iterIndex->second;
            if ( slotInfo.nSkillType == nSkillType )
            {
                return slotInfo.nSlotIndex;
            }
        }
    }
    return -1;
}

vector<SSlotMapedShortcutScheme> CSchemeSlotMapedShortcut::getAllSlotInfo()
{
    vector<SSlotMapedShortcutScheme> slotInfoVec;
    T_MAP_SLOT_TYPE_MAPED_INFO::iterator itBegin = m_AllSlotMapedShortcut.begin();
    for (; itBegin != m_AllSlotMapedShortcut.end(); ++itBegin )
    {
        T_MAP_SLOT_INDEX_MAPED_INFO::iterator itInfoBegin = itBegin->second.begin();
        for (; itInfoBegin != itBegin->second.end(); ++itInfoBegin )
        {
            SSlotMapedShortcutScheme slotInfo = itInfoBegin->second;
            slotInfoVec.push_back(slotInfo);
        }
    }
    return slotInfoVec;
}

vector<SSlotMapedShortcutScheme> CSchemeSlotMapedShortcut::getOneTypeSlotInfo(int nSlotType)
{
    vector<SSlotMapedShortcutScheme> slotInfoVec;
    T_MAP_SLOT_INDEX_MAPED_INFO::iterator itInfoBegin =m_AllSlotMapedShortcut[ShortCutSlotType_Function].begin();
    for (; itInfoBegin != m_AllSlotMapedShortcut[ShortCutSlotType_Function].end(); ++itInfoBegin )
    {
        SSlotMapedShortcutScheme slotInfo = itInfoBegin->second;
        slotInfoVec.push_back(slotInfo);
    }
    return slotInfoVec;
}