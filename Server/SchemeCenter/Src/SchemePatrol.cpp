/*******************************************************************
** 文件名:	SchemePatrol.cpp
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
#include "SchemePatrol.h"
#include "IServerGlobal.h"
#include <string>
using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemePatrol::CSchemePatrol(void)
{	

}

/** 
@param   
@param   
@return  
*/
CSchemePatrol::~CSchemePatrol(void)
{	
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemePatrol::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载Patrol.csv脚本
    string stringPath = SCP_PATH_FORMAT( PATROL_SCHEME_FILENAME );
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
void CSchemePatrol::Close(void)
{	
	// 清空
	m_mapPatrolScheme.clear();
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
bool CSchemePatrol::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
	m_mapPatrolScheme.clear();
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();	
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
		int nPointColStart = 2;
		vector<Vector3> vctPatrolPath;
		// 怪物ID
		int nPatrolID = pCSVReader->GetInt(nRow, 0, 0);
		float fPointX = pCSVReader->GetFloat(nRow, nPointColStart++, 0.0f);
		float fPointY = pCSVReader->GetFloat(nRow, nPointColStart++, 0.0f);
		float fPointZ = pCSVReader->GetFloat(nRow, nPointColStart++, 0.0f);
		while (fPointX != 0.0f || fPointY != 0.0f || fPointZ != 0.0f )
		{
			vctPatrolPath.push_back(Vector3(fPointX, fPointY, fPointZ));
			fPointX = pCSVReader->GetFloat(nRow, nPointColStart++, 0.0f);
			fPointY = pCSVReader->GetFloat(nRow, nPointColStart++, 0.0f);
			fPointZ = pCSVReader->GetFloat(nRow, nPointColStart++, 0.0f);
		}
		if (nPatrolID != 0 && vctPatrolPath.size() > 0)
		{
			m_mapPatrolScheme[nPatrolID] = vctPatrolPath;
		}		
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
bool CSchemePatrol::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemePatrol* pNewInstance = NewInstance();
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


/////////////////////////ISchemePatrol/////////////////////////
/** 取得巡逻配置信息
@param nPatrolID:	巡逻配置ID
@return  
*/
bool CSchemePatrol::getPatrolShemeInfo(int nPatrolID, vector<Vector3> &vctPatrolPath)
{
	TMAP_PATROLSCHEME::iterator iter = m_mapPatrolScheme.find(nPatrolID);
	if (iter == m_mapPatrolScheme.end() || iter->second.size() == 0)
	{
		return false;
	}
	vctPatrolPath.clear();
	vctPatrolPath.insert(vctPatrolPath.begin(), iter->second.begin(), iter->second.end());
	return true;
}