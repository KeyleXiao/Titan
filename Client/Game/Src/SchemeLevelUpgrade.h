/*******************************************************************
** 文件名:	SchemeLevelUpgrade.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林飞
** 日  期:	1/9/2015
** 版  本:	1.0
** 描  述:	升级配置表 LevelUpgrade.csv

			
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"

// 定义
class CSchemeLevelUpgrade : public ISchemeLevelUpgrade, public ISchemeUpdateSink
{
public:
	/////////////////////////ISchemeLevelUpgrade/////////////////////////	
	/** 取得某等级经验配置信息
	@param nlevel:	等级
	@return  @exp 经验
	*/
	virtual int				GetNextLevelExp(int nlevel, int nAttributton = ENEXPTYPE_WARHEROEXP, int nExpTypeID = 0);
	
	/** 取得升级到某等级所需最低总经验值
	@param nlevel:	等级
	*/
	virtual int				GetLevelAllExp(int nlevel, int nAttributton = ENEXPTYPE_WARHEROEXP, int nExpTypeID = 0);

	/** 取得配置中最大等级配置信息
	@param 
	@return  最大等级
	*/
	virtual int				GetSchemeLevelUpgradeMaxLevel(int nAttributton = ENEXPTYPE_WARHEROEXP);
	/** 取得配置中最小等级配置信息
	@param 
	@return  最小等级
	*/
	virtual int				GetSchemeLevelUpgradeMinLevel(int nAttributton = ENEXPTYPE_WARHEROEXP);	

	/////////////////////////ISchemeUpdateSink/////////////////////////
	/** CSV配置载入时通知
	@param   type   : 读取器类型
		            : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
					: READER_XML : 就可以把pReader转换成TiXmlDocument*
	@param   pReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/ 
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	/** 配置动态更新时通知
	@param   pCSVReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeLevelUpgrade//////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool					LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void					Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeLevelUpgrade(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeLevelUpgrade(void);

private:

    typedef std::vector<SLevelUpgradeScheme>        LEVEL_UPGRADE_SCHEMEVEC;        // key为等级
    typedef std::map<int, LEVEL_UPGRADE_SCHEMEVEC>  EXP_ARRAY_SCHEMEMAP;            // key为经验所属
    typedef std::map<int, int>                      LEVEL_MAX_SCHEMEMAP;            // key为经验所属
    typedef std::map<int, int>                      LEVEL_MIN_SCHEMEMAP;            // key为经验所属
    EXP_ARRAY_SCHEMEMAP                 m_mapExpScheme;
    LEVEL_MAX_SCHEMEMAP                 m_mapLvMaxScheme;
    LEVEL_MIN_SCHEMEMAP                 m_mapLvMinScheme;
};