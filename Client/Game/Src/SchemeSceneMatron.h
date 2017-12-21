/*******************************************************************
** 文件名:	SchemeSceneMatron.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-10-13
** 版  本:	1.0
** 描  述:	场景保姆配置 SceneMatron.csv

			
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeDef.h"

// 定义
class CSchemeSceneMatron : public ISchemeSceneMatron, public ISchemeUpdateSink
{
    enum EM_SCHEME_COLUMN
    {
        COL_MAPID = 0,					    // 地图ID
        COL_FORBIT_WING,                    // 禁止使用翅膀
        COL_FORBIT_PK,						// 禁止决斗
        COL_FORBIT_SWITCH_HERO,	    	    // 禁止切换英雄
        COL_ENABLE_RIGHT_MENU,	    	    // 实体右键菜单（0不开启，1开启）
        COL_HOTKEY_INDEX,	    	        // 快捷键方案调用序号（场景脚本配置）
        COL_FORBIT_ACCELERATE,              // 禁止加速跑
        COL_FLAG,                           // 标志位
    };

public:
	//////////////////////////////// ISchemeSceneMatron ////////////////////////////////
	/** 目标地图是否禁止使用翅膀
	@param   nMapID：地图ID
	@return  
	*/
    virtual bool                                    IsForbitWing(int nMapID);

	/** 目标地图是否禁止决斗
	@param   nMapID：地图ID
	@return  
	*/
    virtual bool                                    IsForbitPK(int nMapID);

	/** 目标地图是否禁止切换英雄
	@param   nMapID：地图ID
	@return  
	*/
    virtual bool                                    IsForbitSwitchHero(int nMapID);

    /** 实体右键菜单（0不开启，1开启）
    @param   nMapID：地图ID
    @return
    */
    virtual bool                                    IsEnableEntityRightMenu(int nMapID);

    /** 获取快捷键方案调用序号（场景脚本配置）
    @param   nMapID：地图ID
    @return
    */
    virtual int                                     GetHotkeyIndex(int nMapID);

    /** 是否开启加速跑
    @param   nMapID：地图ID
    @return
    */
    virtual bool                                    CanAccelerate(int nMapID);

	/** 取得地图对应的场景保姆配置信息
	@param   nMapID：地图ID
	@return  
	*/
    virtual SSchemeSceneMatron *                    getByMapID( int nMapID );

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

	///////////////////////// CSchemeSceneMatron //////////////////////////
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
	CSchemeSceneMatron(void);

private:
    typedef int MAP_ID;
    typedef std::map<MAP_ID, SSchemeSceneMatron>	    TMAP_SCHEMESCENEMATRON;

    TMAP_SCHEMESCENEMATRON                              m_mapSchemeSceneMatron;
};