/*******************************************************************
** 文件名:	SchemeGuideStep.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/4/2017
** 版  本:	1.0
** 描  述:	新手引导步骤配置 GuideStep.csv

			
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"

// 定义
class CSchemeGuideStep : public ISchemeGuideStep, public ISchemeUpdateSink, public CSchemeCache<CSchemeGuideStep>
{
    // 场外引导步骤ID	查询ID	场外引导步骤描述（不读）

    enum CFG_GUIDE_COLUMN
    {
        COL_PID = 0,					    // 场外引导步骤ID	
        COL_STEP,                           // 查询ID	
        COL_REMARK,                         // 场外引导步骤描述（不读）
    };

public:
	//////////////////////////////// ISchemeGuideStep ////////////////////////////////
    // 取得指定ID对应的步骤数字
    virtual int                                     getStepByID( int nPID );

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

	/////////////////////////CSchemeGuideStep//////////////////////////
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
	CSchemeGuideStep(void);

private:
    typedef int STEP_ID;
    typedef int P_ID;

	typedef std::map<STEP_ID, P_ID>	            TMAP_PID2STEPID;

	TMAP_PID2STEPID                             m_map;
};