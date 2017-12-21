/*******************************************************************
** 文件名:	SchemeSlotMapedShortcut.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	1/7/2015
** 版  本:	1.0
** 描  述:	快捷键映射

			
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"

enum  ShortCutSlotType
{
    ShortCutSlotType_Goods=1,
    ShortCutSlotType_Skills,
    ShortCutSlotType_Function,
};
// 定义
class CSchemeSlotMapedShortcut : public ISchemeSlotMapedShortcut, public ISchemeUpdateSink
{
public:
	/////////////////////////ISchemeSlotMapedShortcut/////////////////////////
	
	/** 取得皮肤配置信息
	@param nSkinID:	皮肤ID
	@return  
	*/
	virtual SSlotMapedShortcutScheme* getSlotMapedShortcutShemeInfo(int nSlotIndex, int nSlotType);


    // 根据技能技能获得槽位id
    virtual int getSlotAccordSkillType( int nSkillType );

    // 获得所有技能快捷键
    virtual vector<SSlotMapedShortcutScheme> getAllSlotInfo();

    // 获得某一类型的所有快捷键
    virtual vector<SSlotMapedShortcutScheme> getOneTypeSlotInfo(int nSlotType);
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
	CSchemeSlotMapedShortcut(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeSlotMapedShortcut(void);

private:
	// 索引对应的槽位快捷键信息
	typedef map<int,SSlotMapedShortcutScheme> T_MAP_SLOT_INDEX_MAPED_INFO;
	typedef map<int,T_MAP_SLOT_INDEX_MAPED_INFO> T_MAP_SLOT_TYPE_MAPED_INFO;
	T_MAP_SLOT_TYPE_MAPED_INFO m_AllSlotMapedShortcut;
};