/*******************************************************************
** 文件名:	SchemeMonster.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	1/9/2015
** 版  本:	1.0
** 描  述:	人物模型 PersonModel.csv

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "SchemeMonster.h"
#include "IServerGlobal.h"
#include <string>
using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemeMonster::CSchemeMonster(void)
{	

}

/** 
@param   
@param   
@return  
*/
CSchemeMonster::~CSchemeMonster(void)
{	
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeMonster::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载PersonModel.csv脚本
    string stringPath = SCP_PATH_FORMAT( MONSTER_SCHEME_FILENAME );
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
void CSchemeMonster::Close(void)
{	
	// 清空
	m_mapMonsterScheme.clear();
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
bool CSchemeMonster::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
#define MONSTER_LOAD_SKILL(index)	\
	{		\
		long lTemp[3];		\
		for (int i=0; i<3; i++){		\
			lTemp[i] = pCSVReader->GetInt(nRow, MONSTER_COL_SKILL##index + i, 0);		\
		}		\
		item.stMonstoerSkill[##index-1].lSkillID	= lTemp[0];		\
		item.stMonstoerSkill[##index-1].lSkillPer	= lTemp[1];		\
		item.stMonstoerSkill[##index-1].lSkillCD	= lTemp[2];		\
	}

	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
	m_mapMonsterScheme.clear();

	int nLen = 0;
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
		SMonsterScheme item;
		ZeroMemory(&item,sizeof(SMonsterScheme));

		//EntityViewItem entityInfo;
		//ZeroMemory(&entityInfo,sizeof(EntityViewItem));
		// 怪物ID
		item.nMonsterID = pCSVReader->GetInt(nRow, MONSTER_COL_ID, 0);
		// 怪物类型 MONSTER_TYPE
		item.nMonsType = pCSVReader->GetInt(nRow, MONSTER_COL_TYPE, 0);
        // 怪物子类型 MONSTER_COL_SUBTYPE
        item.nSubType = pCSVReader->GetInt(nRow, MONSTER_COL_SUBTYPE, 0);
		// 怪物图标ID
		item.nIconID = pCSVReader->GetInt(nRow, MONSTER_COL_ICON, 0);
		// 皮肤
		item.nDefaultSkin =  pCSVReader->GetInt(nRow, MONSTER_COL_DEFAULT_SKIN, 0);
		// 生命值
		item.nMaxHP = pCSVReader->GetInt(nRow, MONSTER_COL_HP, 0);	
		// 生命增长值
		item.fAddHP = pCSVReader->GetFloat(nRow, MONSTER_COL_ADD_HP, 0.0f);
		// 法力值
		item.nMaxMP = pCSVReader->GetInt(nRow, MONSTER_COL_MP, 0);
		// 法力增长值
		item.fAddMP = pCSVReader->GetFloat(nRow, MONSTER_COL_ADD_MP, 0.0f);
		// 物理攻击力
		item.nPA	= pCSVReader->GetInt(nRow, MONSTER_COL_PA, 0);	
		// 物理攻击增长值
		item.fAddPA = pCSVReader->GetFloat(nRow, MONSTER_COL_ADD_PA, 0.0f);
		// 法术攻击力
		item.nMA	= pCSVReader->GetInt(nRow, MONSTER_COL_MA, 0);	
		// 法术攻击增长值
		item.fAddMA = pCSVReader->GetFloat(nRow, MONSTER_COL_ADD_MA, 0.0f);
		// 物理防御力
		item.nPD	= pCSVReader->GetInt(nRow, MONSTER_COL_PD, 0);	
		// 物理防御增长值
		item.fAddPD = pCSVReader->GetFloat(nRow, MONSTER_COL_ADD_PD, 0.0f);
		// 法术防御力
		item.nMD	= pCSVReader->GetInt(nRow, MONSTER_COL_MD, 0);	
		// 法术防御增长值
		item.fAddMD = pCSVReader->GetFloat(nRow, MONSTER_COL_ADD_MD, 0.0f);
		// 攻击速度
		item.nASD	= pCSVReader->GetInt(nRow, MONSTER_COL_ASD, 0);	
		// 攻击速度增长值
		item.fAddAS = pCSVReader->GetFloat(nRow, MONSTER_COL_ADD_ASD, 0.0f);
		// 暴击率
		item.nCRC	= pCSVReader->GetInt(nRow, MONSTER_COL_CRC, 0);	
		// 暴击增长值
		item.fAddCRC = pCSVReader->GetFloat(nRow, MONSTER_COL_ADD_CRC, 0.0f);
		// 移动速度
		item.nMoveSpeed = pCSVReader->GetInt(nRow, MONSTER_COL_SPEED, 0);
		// 攻击速度
		item.nASD = pCSVReader->GetInt(nRow, MONSTER_COL_ASD, 0);
		// 视野距离
		item.fEyeshot = pCSVReader->GetFloat(nRow, MONSTER_COL_EYESHOT, 0.0f);	
        // 搜索范围
        item.fSearchRadii = pCSVReader->GetFloat(nRow, MONSTER_COL_SEARCH_RADII, 0.0f);
		// 阵营
		item.byCamp = (BYTE)pCSVReader->GetInt(nRow, MONSTER_COL_CAMP, 0);	
		// 搜敌策略
		item.lStrategy = pCSVReader->GetInt(nRow, MONSTER_COL_STRATEGY, 0);
		// 搜敌仇恨
		item.lSeachEnmity = pCSVReader->GetInt(nRow, MONSTER_COL_SEACH_ENMITY, DEFAULT_MONSTER_ENMITY);		
		// 搜敌频率
		item.lStrategyVal = pCSVReader->GetInt(nRow, MONSTER_COL_STRATEGY_VAL, 0);
		// 技能1~4
		MONSTER_LOAD_SKILL(1);
		MONSTER_LOAD_SKILL(2);
		MONSTER_LOAD_SKILL(3);
		MONSTER_LOAD_SKILL(4);
		// 经验值
		item.nExpVal = pCSVReader->GetInt(nRow, MONSTER_COL_EXPVAL, 0);
		// 经验范围
		item.nExpRange = pCSVReader->GetInt(nRow, MONSTER_COL_EXPRANGE, -1);
		// 经验类型
		item.byExpType = (BYTE)pCSVReader->GetInt(nRow, MONSTER_COL_EXPTYPE, 0);
		// 回归距离
		item.nRetrieveDis = pCSVReader->GetInt(nRow, MONSTER_COL_RETURN_DIS, 0);
		// 回归标志
		item.nRetrieveMask = pCSVReader->GetInt(nRow, MONSTER_COL_RETURN_MASK, 0);		
		// 怪物标志
		item.nMonsterMask = pCSVReader->GetInt(nRow, MONSTER_COL_MASK, 0);	
		// boss血条显示距离
		item.nBossBloodShowDistance = pCSVReader->GetInt(nRow, MONSTER_COL_BOSSBLOODSHOWDISTASNCE, 0);
		// boss血条显示优先级
		item.nBossBloodShowPriority = pCSVReader->GetInt(nRow, MONSTER_COL_BOSSBLOODSHOWPRIORITY, 0);

		// 怪物名
		nLen = sizeof(item.szName);
		pCSVReader->GetString(nRow, MONSTER_COL_NAME, item.szName, nLen);

        // 保留数值; 注:动态怪.障碍配置(DynamicObstacle.csv)索引号
        item.nReserve = pCSVReader->GetInt(nRow, MONSTER_COL_RESERVE, 0);

		// 插入到列表中
		m_mapMonsterScheme[item.nMonsterID] = item;		
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
bool CSchemeMonster::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeMonster* pNewInstance = NewInstance();
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


/////////////////////////ISchemeMonster/////////////////////////
/** 取得怪物配置信息
@param nMonsterId:	怪物ID
@return  
*/
SMonsterScheme* CSchemeMonster::getMonsterShemeInfo(int nMonsterId)
{
	TMAP_MONSTERSCHEME::iterator iter = m_mapMonsterScheme.find(nMonsterId);
	if (iter == m_mapMonsterScheme.end())
	{
		return NULL;
	}
	return &(iter->second);
}