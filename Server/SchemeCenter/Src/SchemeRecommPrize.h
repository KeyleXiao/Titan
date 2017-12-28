/*******************************************************************
** 文件名:	SchemeRecommPrize.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	王全申
** 日  期:	2017-12-22
** 版  本:	1.0
** 描  述:	推广奖励配置 RecommendPrize.csv

			
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeCache.h"


// 定义
class CSchemeRecommPrize : public ISchemeRecommPrize, public ISchemeUpdateSink, public CSchemeCache<CSchemeRecommPrize>
{
    enum EMRECOMMPRIZE_SCHEME_COLUMN
    {
        COL_TARGET_ID = 0,
        COL_MATCH_NUM,
        COL_PRIZE_ID,
    };

public:
	//////////////////////////////// ISchemeRecommPrize ////////////////////////////////
	/** 取得推广奖励的配置信息
	@param nTargetID: 奖励ID 
	@return  
	*/
    virtual SSchemeRecommPrize*                        getByTargetID( int nTargetID );

	//////////////////////////////// ISchemeUpdateSink ////////////////////////////////
	/** CSV配置载入时通知
	@param   type   : 读取器类型
		            : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
					: READER_XML : 就可以把pReader转换成TiXmlDocument*
	@param   pReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/ 
	virtual bool			                        OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	/** 配置动态更新时通知
	@param   pCSVReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool			                        OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeRecommPrize//////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool											LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void											Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeRecommPrize(void);

private:
    std::map<int, SSchemeRecommPrize> m_mapConfig;
};