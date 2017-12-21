/*******************************************************************
** 文件名:	SchemeSkinSpell.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/14/2017
** 版  本:	1.0
** 描  述:	皮肤技能
********************************************************************/

#include "StdAfx.h"
#include "SchemeSkinSpell.h"
#include "IClientGlobal.h"

//////////////////////////////////////////////////////////////////////////
struct Skin_Spell_Compare
{
    Skin_Spell_Compare(SSkinSpell data) : _data(data)
    {
    }

    bool operator()(SSkinSpell& data)
    {
        return _data.nType == data.nType && _data.nSkinID == data.nSkinID && _data.nOldValue == data.nOldValue;
    }
private:
    SSkinSpell  _data;
};

//////////////////////////////////////////////////////////////////////////
/** 
@param   
@param   
@return  
*/
CSchemeSkinSpell::CSchemeSkinSpell(void)
{
    m_schemeMap.clear();
}

/** 
@param   
@param   
@return  
*/
CSchemeSkinSpell::~CSchemeSkinSpell(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeSkinSpell::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( SKIN_SPELL_SCHEME_FILENAME );
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
void CSchemeSkinSpell::Close(void)
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
bool CSchemeSkinSpell::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

    m_schemeMap.clear();

    int nLen = 0;
    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow=0; nRow<nRecordCount; ++nRow)
    {
        SSkinSpell data;
        // 当前列数
        int nCol = 0;
        // 皮肤ID
        data.nSkinID = pCSVReader->GetInt(nRow, nCol++, 0);
        // 皮肤名字
        nCol++;
        // 类型
        data.nType = pCSVReader->GetInt(nRow, nCol++, 0);
        // 旧数据
        data.nOldValue = pCSVReader->GetInt(nRow, nCol++, 0);
        // 新数据
        data.nNewValue = pCSVReader->GetInt(nRow, nCol++, 0);


        SKIN_MAP & mapSkin = m_schemeMap[data.nType];
        SKIN_VECTOR & skinVector = mapSkin[data.nSkinID];

        // 除重
        SKIN_VECTOR::iterator iter = find_if(skinVector.begin(), skinVector.end(), Skin_Spell_Compare(data));
        if (iter != skinVector.end())
        {
            ErrorLn("CSchemeSkinSpell There is repeated,nSkinID="<< data.nSkinID <<",nType="<< data.nType <<",nOldValue="<< data.nOldValue);
            return false;
        }

        skinVector.push_back(data);
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
bool CSchemeSkinSpell::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}


/////////////////////////ISchemeSkinSpell/////////////////////////
/** 取得PK配置信息
@return PK配置信息
*/
SSkinSpell *CSchemeSkinSpell::getSkinSpell(int nSkinID, int nType, int nValue)
{
    SKIN_SPELL_MAP::iterator iter = m_schemeMap.find(nType);
    if (iter == m_schemeMap.end())
    {
        return NULL;
    }

    SKIN_MAP & mapSkin = iter->second;
    SKIN_MAP::iterator skinIter = mapSkin.find(nSkinID);
    if (skinIter == mapSkin.end())
    {
        return NULL;
    }

    SSkinSpell data;
    data.nSkinID = nSkinID;     // 皮肤ID
    data.nType = nType;         // 类型
    data.nOldValue = nValue;    // 旧数据

    SKIN_VECTOR & skinVector = skinIter->second;
    SKIN_VECTOR::iterator it = find_if(skinVector.begin(), skinVector.end(), Skin_Spell_Compare(data));
    if (it == skinVector.end())
    {
        return NULL;
    }

    return &(*it);
}

/** 取得皮肤技能配置信息
@return 皮肤技能配置
*/
int CSchemeSkinSpell::getSkinSpell(int nSkinID, int nType, SSkinSpell* arraySkinSpell, int nArrayCount)
{
    int nReturnCount = 0;

    if (arraySkinSpell && nArrayCount)
    {
        SKIN_SPELL_MAP::iterator iter = m_schemeMap.find(nType);
        if (iter != m_schemeMap.end())
        {
            SKIN_MAP & mapSkin = iter->second;
            SKIN_MAP::iterator skinIter = mapSkin.find(nSkinID);
            if (skinIter != mapSkin.end())
            {
                SKIN_VECTOR & skinVector = skinIter->second;

                for (SKIN_VECTOR::iterator it = skinVector.begin(); (nReturnCount < nArrayCount && it != skinVector.end()); ++iter)
                {
                    arraySkinSpell[nReturnCount++] = (*it);
                }
            }
        }
    }

    return nReturnCount;
}