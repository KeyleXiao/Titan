/*******************************************************************
** 文件名:	SchemeSceneMatron.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-10-13
** 版  本:	1.0
** 描  述:	场景保姆配置 SceneMatron.csv

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "SchemeSceneMatron.h"
#include "IServerGlobal.h"
#include <string>
using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemeSceneMatron::CSchemeSceneMatron(void)
{	

}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeSceneMatron::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载csv脚本
    string stringPath = SCP_PATH_FORMAT( SCENE_MATRON_SCHEME_FILENAME );
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
void CSchemeSceneMatron::Close(void)
{	
	// 清空
	m_mapSchemeSceneMatron.clear();
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
bool CSchemeSceneMatron::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;

	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
	m_mapSchemeSceneMatron.clear();

    int nLen = 0;

	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
		SSchemeSceneMatron node;

		// 地图ID
		node.nMapID		        = pCSVReader->GetInt(nRow, COL_MAPID, 0);

        // 禁止使用翅膀
        node.bForbitWing        = pCSVReader->GetBool(nRow, COL_FORBIT_WING, false);

        // 禁止决斗
        node.bForbitPK          = pCSVReader->GetBool(nRow, COL_FORBIT_PK, false);

        // 禁止切换英雄
        node.bForbitSwitchHero  = pCSVReader->GetBool(nRow, COL_FORBIT_SWITCH_HERO, false);

        // 实体右键菜单（0不开启，1开启）
        node.bEnableRightMenu   = pCSVReader->GetBool(nRow, COL_ENABLE_RIGHT_MENU, false);

        // 快捷键方案调用序号（场景脚本配置）
        node.nHotkeyIndex       = pCSVReader->GetInt(nRow, COL_HOTKEY_INDEX, 0);
        // 默认没有填将用第一套方案
        if (node.nHotkeyIndex <= 0) {
            node.nHotkeyIndex = 1;
        }

        // 能加速跑
        node.bAccelerateRun     = (false == pCSVReader->GetBool(nRow, COL_FORBIT_ACCELERATE, false));
        
        // 控制标识
        node.nFlag		        = pCSVReader->GetInt(nRow, COL_FLAG, 0);

        m_mapSchemeSceneMatron[node.nMapID] = node;
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
bool CSchemeSceneMatron::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeSceneMatron* pNewInstance = NewInstance();
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
/** 目标地图是否禁止使用翅膀
@param   nMapID：地图ID
@return  
*/
bool CSchemeSceneMatron::IsForbitWing(int nMapID)
{
	nMapID = FormatMap(nMapID);

	TMAP_SCHEMESCENEMATRON::iterator iter = m_mapSchemeSceneMatron.find(nMapID);
    if(iter == m_mapSchemeSceneMatron.end())
    {
        return false;
    }
    return iter->second.bForbitWing;
}

/** 目标地图是否禁止决斗
@param   nMapID：地图ID
@return  
*/
bool CSchemeSceneMatron::IsForbitPK(int nMapID)
{
	nMapID = FormatMap(nMapID);

	TMAP_SCHEMESCENEMATRON::iterator iter = m_mapSchemeSceneMatron.find(nMapID);
    if(iter == m_mapSchemeSceneMatron.end())
    {
        return false;
    }
    return iter->second.bForbitPK;
}

/** 目标地图是否禁止切换英雄
@param   nMapID：地图ID
@return  
*/
bool CSchemeSceneMatron::IsForbitSwitchHero(int nMapID)
{
	nMapID = FormatMap(nMapID);

	TMAP_SCHEMESCENEMATRON::iterator iter = m_mapSchemeSceneMatron.find(nMapID);
    if(iter == m_mapSchemeSceneMatron.end())
    {
        return false;
    }
    return iter->second.bForbitSwitchHero;
}

/** 实体右键菜单（0不开启，1开启）
@param   nMapID：地图ID
@return
*/
bool CSchemeSceneMatron::IsEnableEntityRightMenu(int nMapID)
{
	nMapID = FormatMap(nMapID);

	TMAP_SCHEMESCENEMATRON::iterator iter = m_mapSchemeSceneMatron.find(nMapID);
    return ((iter == m_mapSchemeSceneMatron.end()) ? false : iter->second.bEnableRightMenu);
}


/** 获取快捷键方案调用序号（场景脚本配置）
@param   nMapID：地图ID
@return
*/
int CSchemeSceneMatron::GetHotkeyIndex(int nMapID)
{
	nMapID = FormatMap(nMapID);

	TMAP_SCHEMESCENEMATRON::iterator iter = m_mapSchemeSceneMatron.find(nMapID);
    return ((iter == m_mapSchemeSceneMatron.end()) ? 1 : iter->second.nHotkeyIndex);
}

bool CSchemeSceneMatron::CanAccelerate(int nMapID)
{
    nMapID = FormatMap(nMapID);

    TMAP_SCHEMESCENEMATRON::iterator iter = m_mapSchemeSceneMatron.find(nMapID);
    return ((iter == m_mapSchemeSceneMatron.end()) ? true : (iter->second.bAccelerateRun) );
}

/** 取得地图对应的场景保姆配置信息
@param   nMapID：地图ID
@return  
*/
SSchemeSceneMatron *  CSchemeSceneMatron::getByMapID( int nMapID )
{
	nMapID = FormatMap(nMapID);

    TMAP_SCHEMESCENEMATRON::iterator iter = m_mapSchemeSceneMatron.find(nMapID);
    if(iter == m_mapSchemeSceneMatron.end())
    {
        return nullptr;
    }
    return &(iter->second);
}