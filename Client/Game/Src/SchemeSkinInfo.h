/*******************************************************************
** 文件名:	SchemeSkinInfo.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	4/2/2015
** 版  本:	1.0
** 描  述:	皮肤配置表 SkinInfo.csv

			
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"



// 定义
class CSchemeSkinInfo : public ISchemeSkinInfo, public ISchemeUpdateSink
{
    enum EMSKININFO_SCHEME_COLUMN
    {
		SKININFO_COL_ID = 0,						// 皮肤ID
		SKININFO_COL_SKIN_TYPE,						// 皮肤类型
		SKININFO_COL_SKINNAME,						// 皮肤名
		SKININFO_COL_PREFABPATH,					// 皮肤路径
		SKININFO_COL_PRELOAD,						// 是否预加载
		SKININFO_COL_SKINSCALE,						// 皮肤缩放
		SKININFO_COL_SKINLEVEL,						// 皮肤等级
		SKININFO_COL_SKINICON,						// 皮肤图片ID
        SKININFO_COL_SELECT_EFFECT, 				// 选人光效ID
		SKININFO_COL_SELECT_SOUND,	 				// 选人音效ID
    };
public:
	/////////////////////////ISchemeMonster/////////////////////////
	/** 取得怪物配置信息
	@param nMonsterId:	怪物ID
	@return  
	*/
	virtual SkinInfoScheme* getSkinShemeInfo(int nSkinID);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	/** CSV配置载入时通知
	@param   pCSVReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/ 
	virtual bool									OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);


	/** 配置动态更新时通知
	@param   pCSVReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool									OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeMonster//////////////////////////
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
	CSchemeSkinInfo(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeSkinInfo(void);

private:
	// 皮肤ID为索引
	typedef std::map<int, SkinInfoScheme> TMAP_SKININFOSCHEME;
	TMAP_SKININFOSCHEME				m_mapSkinInfoScheme;
};