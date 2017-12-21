/*******************************************************************
** 文件名:	ImageDataBox.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Long)（Forlion@126.com）
** 日  期:	2009/10/11 17:01
** 版  本:	1.0
** 描  述:	数据盒子（表示一个对像的数据集合）
** 应  用:  	
	1、数据管理，以前的版本基本上每个dataunit都需要new内存，并且每
	个datanunit都是new出来，这样很容易把内存new碎了，此场景如果要想稳定，
	效率高，尽量用整块内存。

	2、这个版本添加需要先注册key，否则添加不了数据，这个有好处：
　	1）就是能够规范写lua的人员，像华夏二，想用的时候，直接添加就行，这样
	key管理非常混乱，到最后不晓得这个key是什么用的，是什么类型的数据....
	这样促使lua会集中注册，统一管理。
	2）防止使用相同的key，至少在注册的时候就能提醒

**************************** 修改记录 ******************************
** 修改人: 李界华
** 日  期: 2015-08-19
** 描  述: 将字符串的处理分离出来，不用占用额外的内存
********************************************************************/

#pragma once
#include "ImageDataUnit.h"
#include <hash_map>
using namespace stdext;

class CImageDataBox 
{
	// 整型数据表
	typedef hash_map<int, CImageDataIntUnit>	TMAP_INTUNIT;

	// 字符数据表
	typedef hash_map<int, IImageDataChars*>		TMAP_STRUNIT;
public:
	/** 
	@param   
	@param   
	@return  
	*/
	bool					Create(int nSpaceName);

	/** 清掉数据
	@param   
	@param   
	@return  
	*/
	void					Clean(void);

	/** 
	@param   
	@param   
	@return  
	*/
	bool					SetValue(int nKey, int nValue);


	/** 
	@param   
	@param   
	@return  
	*/
	int						GetValue(int nKey);

	/** 
	@param   
	@param   
	@return  
	*/
	bool					SetValue(int nKey, LPCSTR pValue, int nLen);

	/** 
	@param   
	@param   
	@return  
	*/
	LPCSTR					GetValue(int nKey, int &nDataLen);

	/**  
	@param   
	@param   
	@return  
	*/
	CImageDataBox(void);

	/** 析构
	@param   
	@param   
	@return  
	*/
	virtual ~CImageDataBox(void);

private:
	// 空间名字
	int						m_nSpaceName;

	//// 整型镜像数据
	TMAP_INTUNIT			m_mapDataIntUnit;

	// 字符镜像数据
	TMAP_STRUNIT			m_mapDataStrUnit;
};


