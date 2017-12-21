/*******************************************************************
** 文件名:	Tank.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			载具对象
********************************************************************/


#pragma once

#include "DTankModule.h"
#include "EntityBase.h"

class Tank : public EntityBase
{
protected:
	STankSchemeInfo	*			m_pTankSchemeInfo;	// 本载具配置信息
	std::map<int, ushort>		m_mPKSkillMainID;	// postion为索引的mainID表

public:
	Tank()
    {

    }

	virtual~ Tank()
    {

    }

	virtual bool isHero() const
    {
        return false;
    }

	virtual ENTITY_MASK getTypeMask() { return (ENTITY_MASK)(MASK_TANK | MASK_CREATURE); }

	/////////IClientEntity//////////////////////////////////////////////////////
	// 取得数值属性
	virtual int			getIntProperty(int nPropID);

	// 取得字符串属性
	virtual char*		getStringProperty(int nPropID);

	// 取得来源游戏世界ID
	virtual	int			getFromGameWorldID();

protected:
    //
    virtual IEntityPart * createEntityPart( ENTITY_PART id );

    /** 获取实体显示层的基础信息(用于createView)
    @name				: 
    @param				: 
    @return
    */
    virtual bool getBasicViewInfo( EntityViewItem & item );

public:
	/** 取得载具配置数据
	@param   
	@param   
	@return  
	*/
	virtual STankSchemeInfo *	GetTankSchemeInfo(void);

	/////////////Tank/////////////////////////////////////////////////////////////
	/**
	@param
	@param
	@return
	*/
	virtual LPCSTR				GetName(void);
	
	//////////////////////// CTank ////////////////////////////////////////
	/** 
	@param   
	@param   
	@return  
	*/
	virtual DWORD				GetMaxSkillIndex(void);
	

	/** 取技能ID
	@param   
	@param   
	@return  
	*/
	virtual DWORD				GetSkillIDByIndex(BYTE nSkillIndex);

	/** 是否是PK载具
	@param   
	@param   
	@return  
	*/
	virtual bool				IsPKTank();


	/** 设置pk载具技能
	@param   
	@param   
	@return  
	*/
	virtual void				SetPKTankSkill(int nPostion, long nSkillID);

	/** 设置pk载具技能mainID
	@param   
	@param   
	@return  
	*/
	virtual void				SetPKTankSkillMainID(int nPostion, ushort nSkillMainID);

	/** 检测是否存在PK技能
	@param   
	@param   
	@return  
	*/
	virtual bool				HavePKTankSkill(long nSkillID);


	/** 取指定主位置的Skillid
	@param   
	@param   
	@return  
	*/
	virtual int				    GetPKTankSkillByPostion(int nPostion);


	/** 清除mianid列表
	@param   
	@param   
	@return  
	*/
	virtual void				ClearPKTankMainID();

	/** 取PK载具技能个数
	@return  
	*/
	virtual int					GetPKSkillNum();

	/** 判断是否能随时跳出载具
	@name				: 
	@param				: 
	@return	
	*/
	virtual bool				CanJumpOut();

	/** 队友在此载具中时, 左键头像点击直接选中载具
	@name				: 
	@param				: 
	@return
	*/
	virtual bool				IsSelectByFace();
};
