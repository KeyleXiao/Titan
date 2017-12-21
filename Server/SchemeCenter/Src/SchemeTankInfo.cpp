/*******************************************************************
** 文件名:	SchemeTankInfo.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	1/9/2015
** 版  本:	1.0
** 描  述:	人物模型 Tank.csv

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "SchemeTankInfo.h"
#include "IServerGlobal.h"
#include <string>
using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemeTankInfo::CSchemeTankInfo(void)
{	

}

/** 
@param   
@param   
@return  
*/
CSchemeTankInfo::~CSchemeTankInfo(void)
{	
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeTankInfo::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载Tank.csv脚本
    string stringPath = SCP_PATH_FORMAT( TANK_SCHEME_FILENAME );
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
void CSchemeTankInfo::Close(void)
{	
	// 清空
	m_mapTankScheme.clear();
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
bool CSchemeTankInfo::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
	m_mapTankScheme.clear();

	int nLen = 0;
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
		STankSchemeInfo item;
		// 怪物ID
		item.nTankID = pCSVReader->GetInt(nRow, TANK_COL_ID, 0);
		// 生命值
		item.nMaxHP = pCSVReader->GetInt(nRow, TANK_COL_HP, 0);								
		// 移动速度
		item.nMoveSpeed = pCSVReader->GetInt(nRow, TANK_COL_SPEED, 0);				
		// 攻击力
		item.nAttackValue = pCSVReader->GetInt(nRow, TANK_COL_ATTACK, 0);				
		// 视野
		item.fEyeshot = pCSVReader->GetFloat(nRow, TANK_COL_EYESHOT, 0.0f);					
		// 阵营
		item.byCamp = (BYTE)pCSVReader->GetInt(nRow, TANK_COL_CAMP, 0);							
		// 怪物名
		nLen = sizeof(item.szName);
		pCSVReader->GetString(nRow, TANK_COL_NAME, item.szName, nLen);
		// 预设体路径
		nLen = sizeof(item.szPrefabPath);
		pCSVReader->GetString(nRow, TANK_COL_PREFABPATH, item.szPrefabPath, nLen);

		// 插入到列表中
		m_mapTankScheme[item.nTankID] = item;		
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
bool CSchemeTankInfo::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeTankInfo* pNewInstance = NewInstance();
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


/////////////////////////ISchemeTankInfo/////////////////////////
/** 取得怪物配置信息
@param nTankId:	怪物ID
@return  
*/
STankSchemeInfo* CSchemeTankInfo::getTankShemeInfo(int nTankId)
{
	TMAP_TANKSCHEME::iterator iter = m_mapTankScheme.find(nTankId);
	if (iter == m_mapTankScheme.end())
	{
		return NULL;
	}
	return &(iter->second);
}