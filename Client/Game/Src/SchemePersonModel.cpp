/*******************************************************************
** 文件名:	SchemePersonModel.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	1/7/2015
** 版  本:	1.0
** 描  述:	人物模型 PersonModel.csv

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "SchemePersonModel.h"
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
CSchemePersonModel::CSchemePersonModel(void)
{	

}

/** 
@param   
@param   
@return  
*/
CSchemePersonModel::~CSchemePersonModel(void)
{	
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemePersonModel::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载PersonModel.csv脚本
    string stringPath = SCP_PATH_FORMAT( PERSONMODEL_SCHEME_FILENAME );
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
void CSchemePersonModel::Close(void)
{	
	// 清空
	m_mapPersonModel.clear();
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
bool CSchemePersonModel::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;

	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
	m_mapPersonModel.clear();
	//m_mapVocation.clear();

	int nLen = 0;
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
		// 职业
		int nHeroID	= (short)pCSVReader->GetInt(nRow, PERSONMODEL_COL_VOCATION, 0);
		if (nHeroID >= VOCATION_MAX)
		{
			ErrorLn("["<< szFileName <<"] row="<<(nRow+1)<<" config error! hero="<< nHeroID);
			return false;
		}

        // 人物皮肤
		PersonSkinModelItem item;
        item.nHeroID  = nHeroID;

        // 英雄类型 VOCATION_TYPE
        item.nType		= (BYTE)pCSVReader->GetInt(nRow, PERSONMODEL_COL_TYPE, 0);
        if (item.nType >= VOC_MAX)
        {
            ErrorLn("["<< szFileName <<"] row="<<(nRow+1)<<" config error! type="<< item.nType);
            return false;
        }
        // 操作模式 PLAY_MODE
        item.nPlayMode		= (BYTE)pCSVReader->GetInt(nRow, PERSONMODEL_COL_PLAY_MODE, 0);
        if (item.nPlayMode >= PLAY_MODE_MAX)
        {
            ErrorLn("["<< szFileName <<"] row="<<(nRow+1)<<" config error! nPlayMode="<< item.nPlayMode);
            return false;
        }
        // 英雄特征 HERO_TRAITS
        item.nTraits    = (BYTE)pCSVReader->GetInt(nRow, PERSONMODEL_COL_TRAITS, 0);
		// 性别
		item.bySex		= (BYTE)pCSVReader->GetInt(nRow, PERSONMODEL_COL_SEX, 0);
		if (item.bySex >= SEX_MAX)
		{
			ErrorLn("["<< szFileName <<"] row="<<(nRow+1)<<" config error! sex="<< item.bySex);
			return false;
		}
		// 皮肤ID
		item.nDefaultSkinID	= pCSVReader->GetInt(nRow, PERSONMODEL_COL_DEFAULT_SKIN, 0);

		// 英雄名
		nLen = sizeof(item.szHeroName);
		pCSVReader->GetString(nRow, PERSONMODEL_COL_HERONAME, item.szHeroName, nLen);

		// 英雄称号
		nLen = sizeof(item.szHeroTitle);
		pCSVReader->GetString(nRow, PERSONMODEL_COL_HEROTITLE, item.szHeroTitle, nLen);

        // 小地图图标ID
        item.nMinimapIconID = pCSVReader->GetInt(nRow, PERSONMODEL_COL_MINIMAP_ICON, 0); 

        // 视野距离
        char szBuf[256];
        memset(szBuf, 0, sizeof(szBuf));
        nLen = sizeof(szBuf);
        pCSVReader->GetString(nRow, PERSONMODEL_COL_SIGHT, szBuf, nLen);
        vector<string> vecString;
        StringHelper::split(vecString, szBuf, ';', " ");
        for (vector<string>::iterator it = vecString.begin(); it!=vecString.end(); ++it)
        {
            vector<string> vecPair;
            StringHelper::split(vecPair, it->c_str(), ':', " ");
            if(vecPair.size() < 2)
            {
                ErrorLn(__FUNCTION__ << __LINE__ << ", read sight error, heroid=" << nHeroID);
                continue;
            }

            int nMapID = atoi(vecPair[0].c_str());
            if(nMapID >= MAP_MAXCOUNT)
            {
                ErrorLn(__FUNCTION__ << __LINE__ << ", read sight error, invalid mapid, heroid=" << nHeroID);
                continue;
            }

            item.nSightArray[nMapID] = atoi(vecPair[1].c_str()); 
        }

        // 英雄开关
        item.nOnOff = pCSVReader->GetInt( nRow, PERSONMODEL_COL_ONOFF, 0);
        // 0代表正常开启， 1代表不开启
        if(item.nOnOff != 0)
            continue;

		// 全部皮肤列表
        char szSkinArray[128] = {0};
		nLen = sizeof(szSkinArray);
		pCSVReader->GetString(nRow, PERSONMODEL_COL_ALL_SKIN, szSkinArray, nLen);
        // 解析出所有包含皮肤
        int nSkinCount = parseIntArray(szSkinArray, item.nSkinIDList, sizeof(item.nSkinIDList)/sizeof(int), ';');

        // nUnlockLevel英雄解锁等级
        item.nUnlockLevel = pCSVReader->GetInt(nRow, PERSONMODEL_COL_UNLOCK_LEVEL, 0);

		// 已经存在的职业
		if(m_mapPersonModel.find(nHeroID) != m_mapPersonModel.end())
		{
            continue;
        }

		m_mapPersonModel[nHeroID] = item;
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
bool CSchemePersonModel::OnSchemeUpdate(SCRIPT_READER reader,const char* szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}


//////////////////////////////////////////////////////////////////////////
/** 取得英雄名称
@param wVocation: 英雄ID  
@return  
*/
const char * CSchemePersonModel::getHeroName(int nHeroID)
{
    TMAP_PERSONMODEL::iterator iter = m_mapPersonModel.find(nHeroID);
    if (iter == m_mapPersonModel.end())
    {
        return "Unknown";
    }

    return iter->second.szHeroName;
}

/** 取得人物英雄指定的皮肤模型项
@param wVocation:	英雄ID
@param nSkinID:		皮肤ID
@return  
*/
const PersonSkinModelItem*	CSchemePersonModel::getPersonSkinModelItem(int nHeroID,int nSkinID/*=0*/)
{
    TMAP_PERSONMODEL::iterator iter = m_mapPersonModel.find(nHeroID);
    if (iter == m_mapPersonModel.end())
    {
        return NULL; 
    }

    if(nSkinID > 0)
    {
        bool isExisted = false;

        PersonSkinModelItem & node = iter->second;
        if(node.nDefaultSkinID == nSkinID)
        {
            isExisted = true;
        }
        else
        {
            for (int i=0; i< sizeof(node.nSkinIDList)/sizeof(int); ++i)
            {
                if(node.nSkinIDList[i] == nSkinID)
                {
                    isExisted = true;
                    break;
                }
            }
        }

        if(!isExisted)
        {
            return NULL;
        }
    }

    return &(iter->second);
}



 int  CSchemePersonModel::getAllVocation(PersonSkinModelItem * pArray, const int nArrayCount)
 {
     int nCount = 0;
     for(TMAP_PERSONMODEL::iterator iter = m_mapPersonModel.begin(); iter != m_mapPersonModel.end(); ++iter)
     {
         if(nCount >= nArrayCount)
             break;

         pArray[nCount++] = (iter->second);
	 }

	 return nCount;
 }

 int  CSchemePersonModel:: getVocationType(int nHeroID) 
 {
	 const	PersonSkinModelItem* pPersonSkinModelItem  = getPersonSkinModelItem(nHeroID);
     return (pPersonSkinModelItem ? pPersonSkinModelItem->nType : -1);
 }

int	CSchemePersonModel::getUnlockLevel(int nHeroID)
{
    const	PersonSkinModelItem* pPersonSkinModelItem  = getPersonSkinModelItem(nHeroID);
    return (pPersonSkinModelItem ? pPersonSkinModelItem->nUnlockLevel : -1);
}

bool CSchemePersonModel::checkIsFpsHero(int nHeroID)
{
    const	PersonSkinModelItem* pPersonSkinModelItem  = getPersonSkinModelItem(nHeroID);
    return (pPersonSkinModelItem ? (pPersonSkinModelItem->nPlayMode > 0) : false);
}

// 0代表正常开启， 1代表不开启
int	CSchemePersonModel::getHeroSwitchState(int nHeroID)
{
    const	PersonSkinModelItem* pPersonSkinModelItem  = getPersonSkinModelItem(nHeroID);
    return (pPersonSkinModelItem ? pPersonSkinModelItem->nOnOff : -1);
}
