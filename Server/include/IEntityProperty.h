/*******************************************************************
** 文件名:	IEntityProperty.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/28/2014
** 版  本:	1.0
** 描  述:	

			属性集接口
********************************************************************/

#pragma once

// 最大支持的属性长度
#define  MAX_PROPERTY_DATA_LEN   256

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 实体属性
//enum ENTITY_PROPERTY
//{
//	// 玩家基础属性,需要保存到数据库的
//	PROPERTY_NAME = 0,   // 名字
//	PROPERTY_SCENEID ,   // 场景ID
//	PROPERTY_LOCATION,   // 位置
//	PROPERTY_PREFABHERO, // 预设英雄,玩家职业
//	PROPERTY_LEVEL,      // 等级
//	PROPERTY_EXP,        // 经验
//	PROPERTY_HP,         // 血量
//	PROPERTY_MONEY,      // 金钱
//
//	//临时属性,通过计算出来的
//	PROPERTY_UID,        // 角色UID
//	PROPERTY_ATTACK,     // 攻击力
//	PROPERTY_DEFENCE,    // 防御力
//	PROPERTY_SPEED,      // 移动速度
//	PROPERTY_MAX,		 // 最大属性
//};

// 实体属性部件，负责维护实体的属性存储及其同步给客户端以及是否广播的逻辑
struct __IEntityProperty : public __IEntityPart
{
	// 取得属性数量
	virtual int getPropertyCount() = 0;

	// 取得某个属性的类型
	virtual const char * getPropertyType( int prop_id ) = 0;

	// 取得某个属性的标志数据(通常是序列化标志)
	virtual int getPropertyFlag( int prop_id ) = 0;

	// 获取实体属性(布尔类型）
	virtual bool getProperty_Bool( int prop_id) = 0;

	// 设置实体属性(布尔类型）
	virtual bool setProperty_Bool( int prop_id,bool value) = 0;

	// 获取实体属性(数字,实际内部支持用int,byte,short三种类型存)
	virtual int getProperty_Integer( int prop_id) = 0;

	// 设置实体属性(数字,实际内部支持用int,byte,short三种类型存)
	virtual bool setProperty_Integer( int prop_id,int value) = 0;

	// 增加属性数值(注意内部需要控制上下限)
	virtual bool addProperty_Integer( int prop_id,int delta) = 0;

	// 获取实体属性(字符串)
	virtual std::string getProperty_String( int prop_id ) = 0;

	// 设置实体属性(字符串)
	virtual bool setProperty_String( int prop_id,std::string value) = 0;

	// 获取实体属性(自定义结构,如果属性存在返回)
	virtual int getProperty_custom( int prop_id,void * data,int len) = 0;

	// 设置实体属性(自定义结构)
	virtual bool setProperty_custom( int prop_id,void * data,int len) = 0;

    // 销毁
    virtual void destroy() = 0;
};