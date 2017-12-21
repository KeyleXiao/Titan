/*******************************************************************
** 文件名:	SchemePersonModel.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	1/7/2015
** 版  本:	1.0
** 描  述:	人物模型 PersonModel.csv

			
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"



// 定义
class CSchemePersonModel : public ISchemePersonModel, public ISchemeUpdateSink
{
    // 英雄ID	英雄类型（0坦克1战士2法师3射手4辅助）	操作类型（0默认操作1fps）	英雄特征（1近战2远程4物理8法术）性别	默认皮肤ID
    // 英雄称号	英雄名字	小地图图标ID	视野距离	英雄开关（0代表正常开启， 1代表不开启）	包含皮肤ID	英雄解锁等级
    enum EMPERSONMODEL_SCHEME_COLUMN
    {
        PERSONMODEL_COL_VOCATION = 0,					// 英雄ID
        PERSONMODEL_COL_TYPE,							// 英雄类型 VOCATION_TYPE
        PERSONMODEL_COL_PLAY_MODE,					    // 操作模式 PLAY_MODE
        PERSONMODEL_COL_TRAITS,					        // 英雄特征 HERO_TRAITS
        PERSONMODEL_COL_SEX,							// 性别
        PERSONMODEL_COL_DEFAULT_SKIN,					// 默认皮肤ID
        PERSONMODEL_COL_HERONAME,						// 英雄名
		PERSONMODEL_COL_HEROTITLE,						// 英雄称号
        PERSONMODEL_COL_MINIMAP_ICON,                   // 小地图图标ID
        PERSONMODEL_COL_SIGHT,                          // 视野距离
        PERSONMODEL_COL_ONOFF,                          // 职业开关
        PERSONMODEL_COL_ALL_SKIN,                       // 全部皮肤
        PERSONMODEL_COL_UNLOCK_LEVEL,                   // 英雄解锁等级
    };

public:
	/////////////////////////ISchemePersonModel/////////////////////////
	
	/** 取得英雄名称
	@param wVocation: 英雄ID  
	@return  
	*/
	virtual const char *							getHeroName(int nHeroID);

	/** 取得人物英雄指定的皮肤模型项
	@param wVocation:	英雄ID
	@param nSkinID:		皮肤ID
	@return  
	*/
	virtual const PersonSkinModelItem*				getPersonSkinModelItem(int nHeroID,int nSkinID=0);

    virtual int                                     getAllVocation(PersonSkinModelItem * pArray, const int nArrayCount);

	virtual	int										getVocationType(int nHeroID) ;

	virtual int										getUnlockLevel(int nHeroID);

	virtual	int										getHeroSwitchState(int nHeroID);

	virtual  bool									checkIsFpsHero(int nHeroID);
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

	/////////////////////////CSchemePersonModel//////////////////////////
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
	CSchemePersonModel(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemePersonModel(void);

private:
	typedef std::map<short, PersonSkinModelItem>	TMAP_PERSONMODEL;

	TMAP_PERSONMODEL            m_mapPersonModel;

};