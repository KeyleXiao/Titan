/*******************************************************************
** 文件名:	SchemeSpellSlot.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/25/2015
** 版  本:	1.0
** 描  述:	技能槽位配置
********************************************************************/

#include "StdAfx.h"
#include "SchemeSpellSlot.h"
#include "IClientGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeSpellSlot::CSchemeSpellSlot(void)
{
}

/** 
@param   
@param   
@return  
*/
CSchemeSpellSlot::~CSchemeSpellSlot(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeSpellSlot::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( SPELL_SLOT_FILENAME );
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
void CSchemeSpellSlot::Close(void)
{
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
bool CSchemeSpellSlot::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 表清除
	m_schemeMap.clear();

	// 读取行数
	int nRecordCount = pCSVReader->GetRecordCount();
	// 当前列数
	int nCol = 0;
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
		// 初始化列数
		nCol = 0;

		// 取得职业
		int nVocation = pCSVReader->GetInt(nRow, nCol++, 0);

		SCHEME_MAP::iterator IterMap = m_schemeMap.find(nVocation);
		if (IterMap != m_schemeMap.end())
		{
			ErrorLn("Load "<< szFileName <<" find repetition nVocation="<<nVocation<<",nRow="<<nRow);
			return false;
		}

		SPELL_SLOT spellSlot;
		// 图片
		int nImageLen = sizeof(spellSlot.szImage);
		pCSVReader->GetString(nRow, nCol++, spellSlot.szImage, nImageLen);

		// 职业
		int nVocationLen = sizeof(spellSlot.szVocation);
		pCSVReader->GetString(nRow, nCol++, spellSlot.szVocation, nVocationLen);

		// 定位
		int nLocateLen = sizeof(spellSlot.szLocate);
		pCSVReader->GetString(nRow, nCol++, spellSlot.szLocate, nLocateLen);

		// 生命
		spellSlot.nHp = pCSVReader->GetInt(nRow, nCol++, 0);

		// 物理攻击
		spellSlot.nPA = pCSVReader->GetInt(nRow, nCol++, 0);

		// 魔法攻击
		spellSlot.nMA = pCSVReader->GetInt(nRow, nCol++, 0);

		// 操作系数难度
		spellSlot.nOperateFactor = pCSVReader->GetInt(nRow, nCol++, 0);

		// 描述
		int nDescriptionLen = sizeof(spellSlot.szDescription);
		pCSVReader->GetString(nRow, nCol++, spellSlot.szDescription, nDescriptionLen);
        // 技能描述
        for (int nSlotIndex=0; nSlotIndex<SPELL_SLOT_SHOW_COUNT; ++nSlotIndex)
        {
            int nDescriptionLen = sizeof( spellSlot.nSpellDesc[nSlotIndex]);
            pCSVReader->GetString(nRow, nCol++,  spellSlot.nSpellDesc[nSlotIndex],nDescriptionLen);
        }
		// 取得槽位数量
		spellSlot.nCount = pCSVReader->GetInt(nRow, nCol++, 0);
		if (spellSlot.nCount > SPELL_SLOT_MAX_COUNT)
		{
			ErrorLn("Load "<< szFileName <<" spellSlot.nCount>="<<SPELL_SLOT_MAX_COUNT<<",nRow="<<nRow);
			return false;
		}
		for (int nSlotIndex=0; nSlotIndex<spellSlot.nCount; ++nSlotIndex)
		{
			spellSlot.nSpellID[nSlotIndex] = pCSVReader->GetInt(nRow, nCol++, 0);
		}

		// 表插入
		m_schemeMap.insert(SCHEME_MAP::value_type(nVocation, spellSlot));
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
bool CSchemeSpellSlot::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}


/////////////////////////ISchemeSpellSlot/////////////////////////
/** 取得技能槽位配置信息
@param nVocation:	职业ID
@return 技能槽位配置信息
*/
SPELL_SLOT *CSchemeSpellSlot::getSpellSlot(int nVocation)
{
	SCHEME_MAP::iterator Iter = m_schemeMap.find(nVocation);
	if (Iter == m_schemeMap.end())
	{
		return NULL;
	}

	return &Iter->second;
}