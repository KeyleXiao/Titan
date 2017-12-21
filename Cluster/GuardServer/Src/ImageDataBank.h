/*******************************************************************
** 文件名:	e:\Rocket20090925\Server\VanConnector\Src\ImageDataBank.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Long)（Forlion@126.com）
** 日  期:	2009/10/11 18:36
** 版  本:	1.0
** 描  述:	镜像数据银行
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "ImageDataBox.h"
#include <hash_map>
#include <list>
using namespace std;
using namespace stdext;

class CImageDataBank
{	
	typedef hash_map<int, bool>					TMAP_KEY;
	typedef hash_map<DWORD, CImageDataBox *>	TMAP_DATABANK;
public:
	/** 清掉数据
	@param   
	@param   
	@return  
	*/
	void					Clean(int nSpaceName);

#ifdef GUARD_SUPPORT_IMAGE_INT_UNIT
	/** key的注册
	@param   
	@param   
	@return  重复的key注册
	*/
	bool					RegisterKey(int nSpaceName, int nKey, bool bIsInt);

	/** 
	@param   
	@param   
	@return  
	*/
	bool					SetValue(int nSpaceName, int nKey, int nValue);

	/** 
	@param   
	@param   
	@return  
	*/
	int						GetValue(int nSpaceName, int nKey);
#endif


	/** 
	@param   
	@param   
	@return  
	*/
	bool					SetValue(int nSpaceName, int nKey, LPCSTR pValue, int nLen);


	/** 
	@param   
	@param   
	@return  
	*/
	LPCSTR					GetValue(int nSpaceName, int nKey, int &nDataLen);

	/** 
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
	CImageDataBank(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CImageDataBank(void);

private:
	/** 取得数据类型
	@param   
	@param   
	@return  -1:表示没有注册,0:表示为字符型,1:表示整型
	*/
	int						GetImageDataType(int nSpaceName, int nKey);

private:
	TMAP_KEY				m_mapImageKey[IMAGE_DATA_NAMESPACE_MAX];

	CImageDataBox			m_ImageDataBox[IMAGE_DATA_NAMESPACE_MAX];
};
