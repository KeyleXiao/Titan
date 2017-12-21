/*******************************************************************
** 文件名:	SchemeLandBuff.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	1/6/2015
** 版  本:	1.0
** 描  述:	着陆BUFF LandBuff.csv

			
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "BuffDef.h"


// 实体着陆BUFF配置
class CSchemeLandBuff : public ISchemeLandBuff, public ISchemeUpdateSink, public CSchemeCache<CSchemeLandBuff>
{
	// 配置文件对应的列
	enum SCHEME_COLUMN
	{
		COLUNM_SPEED = 0,					// 着地速度
		COLUNM_BUFF_ID,						// 附加ＢＵＦＦ
		COLUNM_BUFF_LEVEL,					// ＢＵＦＦ等级
	};
			

public:
	/////////////////////////ISchemeLandBuff/////////////////////////
	
	/** 取得着陆对应的BUFF配置信息
	@param nLandSpeed:	着陆高度
	@return  
	*/
	virtual SBuffNode*							GetBuffNode(int nLandSpeed);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool								OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool								OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeLandBuff//////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool										LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void										Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeLandBuff(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeLandBuff(void);

private:
	typedef std::map<int, SBuffNode>	TMAP_LANDSPEEDBUFFNODE;
	// 着陆速度BUFF列表
	TMAP_LANDSPEEDBUFFNODE			m_mapLandSpeedBuffNode;
};