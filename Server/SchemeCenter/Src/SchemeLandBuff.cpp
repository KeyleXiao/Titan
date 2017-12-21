/*******************************************************************
** 文件名:	SchemeLandBuff.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	1/6/2015
** 版  本:	1.0
** 描  述:	人物模型 LandBuff.csv

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "SchemeLandBuff.h"
#include "IServerGlobal.h"
#include "SpellDef.h"
#include <string>
using namespace std;

#define  LANDBUFF_SCHEME_FILENAME		"Scp\\LandBuff.csv"
/** 
@param   
@param   
@return  
*/
CSchemeLandBuff::CSchemeLandBuff(void)
{	

}

/** 
@param   
@param   
@return  
*/
CSchemeLandBuff::~CSchemeLandBuff(void)
{	
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeLandBuff::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载PersonModel.csv脚本
    string stringPath = SCP_PATH_FORMAT( LANDBUFF_SCHEME_FILENAME );
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
void CSchemeLandBuff::Close(void)
{	
	// 清空
	m_mapLandSpeedBuffNode.clear();
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
bool CSchemeLandBuff::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;

	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
	m_mapLandSpeedBuffNode.clear();

	int nLen = 0;
	WORD wVocation = 0;
	bool finded = false;
	int nSpeed = 0;

	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
		SBuffNode item;

		// 着地速度
		nSpeed = pCSVReader->GetInt(nRow, COLUNM_SPEED, 0);
		// 附加ＢＵＦＦ
		item.dwID = (DWORD)pCSVReader->GetInt(nRow, COLUNM_BUFF_ID, 0);
		// ＢＵＦＦ等级
		item.dwLevel = (DWORD)pCSVReader->GetInt(nRow, COLUNM_BUFF_LEVEL, 0);

		m_mapLandSpeedBuffNode[nSpeed] = item;
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
bool CSchemeLandBuff::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeLandBuff* pNewInstance = NewInstance();
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
/** 取得着陆对应的BUFF配置信息
@param nLandSpeed:	着陆高度
@return  
*/
SBuffNode* CSchemeLandBuff::GetBuffNode(int nLandSpeed)
{
	//TMAP_LANDSPEEDBUFFNODE::iterator iter = m_mapLandSpeedBuffNode.find(nLandSpeed);
	//if (iter == m_mapLandSpeedBuffNode.end())
	//{
	//	return NULL;
	//}

	//return &(iter->second);
    SBuffNode* pNode = NULL;

    for (TMAP_LANDSPEEDBUFFNODE::iterator iter = m_mapLandSpeedBuffNode.begin(); iter != m_mapLandSpeedBuffNode.end(); ++iter)
    {
        if( nLandSpeed < iter->first)
        {
            break;
        }
        
        pNode = &(iter->second);
    }

    return pNode;
}
