/*******************************************************************
** 文件名:	SchemeDieLighting.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/16/2015
** 版  本:	1.0
** 描  述:	死亡光效配置
********************************************************************/

#include "StdAfx.h"
#include "SchemeDieLighting.h"
#include "IClientGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeDieLighting::CSchemeDieLighting(void)
{
}

/** 
@param   
@param   
@return  
*/
CSchemeDieLighting::~CSchemeDieLighting(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeDieLighting::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( DIE_LIGHTING_SCHEME_FILENAME );
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
void CSchemeDieLighting::Close(void)
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
bool CSchemeDieLighting::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 表清除
	m_schemeMap.clear();

	SDieLightingKey key;
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
		// 当前列数
		int nCol = 0;

		SDieLighting scheme;
		// 职业ID，怪物ID
		int nVocation = pCSVReader->GetInt(nRow, nCol++, 0);
		// 实体类型
		int nType = pCSVReader->GetInt(nRow, nCol++, 0);

        // 死亡贡献金币光效
        int nLen = sizeof(scheme.szCoinLighting);
        pCSVReader->GetString(nRow, nCol++, scheme.szCoinLighting, nLen);

		// 必播死亡光效
		nLen = sizeof(scheme.szName);
		pCSVReader->GetString(nRow, nCol++, scheme.szName, nLen);

		// 读取是否隐藏尸体
		for (int i=0; i<DeadEffect_Max; ++i)
		{
			scheme.nHideBody[i] = pCSVReader->GetInt(nRow, nCol++, 0);
		}

		// 读取死亡光效
		for (int i=0; i<DeadEffect_Max; ++i)
		{
			nLen = sizeof(scheme.szDielName[i]);
			pCSVReader->GetString(nRow, nCol++, scheme.szDielName[i], nLen);
		}

		// 读取必播死亡音效
		scheme.nSoundID = pCSVReader->GetInt(nRow, nCol++, 0);

		// 读取死亡音效ID
		for (int i=0; i<DeadEffect_Max; ++i)
		{
			scheme.nDieSoundID[i] = pCSVReader->GetInt(nRow, nCol++, 0);
		}

        // 必播死亡动作
        nLen = sizeof(scheme.szAnimationName);
        pCSVReader->GetString(nRow, nCol++, scheme.szAnimationName, nLen);

        // 读取死亡动作
        for (int i=0; i<DeadEffect_Max; ++i)
        {
            nLen = sizeof(scheme.szDielAnimationName[i]);
            pCSVReader->GetString(nRow, nCol++, scheme.szDielAnimationName[i], nLen);
        }

		key.nVocation = nVocation;
		key.nType = nType;
		// 基础属性表插入
		pair<SCHEME_MAP::iterator, bool> result = m_schemeMap.insert(SCHEME_MAP::value_type(key, scheme));	
		if (!result.second)
		{
			ErrorLn("SchemeDieLighting There is repeated,nVocation="<<nVocation<<",nType="<<nType);
			return false;
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
bool CSchemeDieLighting::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}


/////////////////////////ISchemeDieLighting/////////////////////////
/** 取得死亡光效配置信息
@param nVocation:	职业ID,怪物ID
@param nType:		实体类型
@return 死亡光效配置信息
*/
SDieLighting *CSchemeDieLighting::getDieLighting(int nVocation, int nType)
{
	SDieLightingKey key;
	key.nVocation = nVocation;
	key.nType = nType;

	SCHEME_MAP::iterator Iter = m_schemeMap.find(key);
	if (Iter == m_schemeMap.end())
	{
		return NULL;
	}

	return &Iter->second;
}