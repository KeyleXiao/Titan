/*******************************************************************
** 文件名:	IPropertySet.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	5/30/2016
** 版  本:	1.0
** 描  述:	多线程安全的属性集访问对象

			
********************************************************************/

#pragma once

/**
	设计思路：
	1.因为IServiceContainer的Post_Message方法比较消耗性能，而属性获得查询获取是最频繁的
	跨线程调用需求，所以这里提供一些由ServiceContainer管理的属性集，可以安全的在多个线程
	存取数据，而不需要通过post_message的调用
*/
struct IPropertySet
{
	// 多线程安全获取属性[会独立加锁]
	// @param offset : 字节偏移数
	// @param bytes  : 字节数
	// @param data   : [输出]数据内容
	// @param len    : [输出]数据长度
	virtual bool get_Property(size_t offset, size_t bytes, void* data, size_t len) = 0;

	// 多线程安全设置属性[会独立加锁]
	// @param offset : 字节偏移数
	// @param bytes  : 字节数
	// @param data   : [输出]数据内容
	// @param len    : [输出]数据长度
	// @param camp   : 是否比较数据是否一致
	virtual bool set_Property(size_t offset,size_t bytes, void* data,bool camp=false) = 0;

	virtual void release() = 0;
};