/*******************************************************************
** 文件名:	IDataChunk.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			数据集合接口
********************************************************************/

#pragma once

// 属性集中返回的无效整数
#define  MAX_INT            (2147483647)
#define  MAX_SHORT          (32767)
#define  INVALID_INT_VALUE  (-2147483648)



struct IDataChunk 
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
	virtual const char * getProperty_String( int prop_id ) = 0;

	// 设置实体属性(字符串)
	virtual bool setProperty_String( int prop_id,std::string value) = 0;

	// 获取实体属性(自定义结构,如果属性存在返回)
	virtual bool getProperty_custom( int prop_id,void *& data,int & len) = 0;

	// 设置实体属性(自定义结构)
	virtual bool setProperty_custom( int prop_id,void * data,int len) = 0;
};