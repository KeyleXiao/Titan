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
#include "stdafx.h"
#include "ImageDataBank.h"

/** 构造
@param   
@param   
@return  
*/
CImageDataBank::CImageDataBank(void)
{
	for(int nSpace = IMAGE_DATA_NAMESPACE_UNKNOWN; nSpace < IMAGE_DATA_NAMESPACE_MAX; nSpace++)
	{
		m_ImageDataBox[nSpace].Create(nSpace);
		m_mapImageKey[nSpace].clear();
	}
}

/** 析构
@param   
@param   
@return  
*/
CImageDataBank::~CImageDataBank(void)
{
	Close();
}

/** 关闭
@param   
@param   
@return  
*/
void CImageDataBank::Close(void)
{	
	for(int nSpace = IMAGE_DATA_NAMESPACE_UNKNOWN; nSpace < IMAGE_DATA_NAMESPACE_MAX; nSpace++)
	{
		m_ImageDataBox[nSpace].Clean();
		m_mapImageKey[nSpace].clear();
	}	
}

/** 清掉数据
@param   
@param   
@return  
*/
void CImageDataBank::Clean(int nSpaceName)
{
	if(nSpaceName <= IMAGE_DATA_NAMESPACE_UNKNOWN || nSpaceName >= IMAGE_DATA_NAMESPACE_MAX)
	{
		return;
	}

	m_ImageDataBox[nSpaceName].Clean();
}


#ifdef GUARD_SUPPORT_IMAGE_INT_UNIT
/** key的注册
@param   
@param   
@return  重复的key注册
*/
bool CImageDataBank::RegisterKey(int nSpaceName, int nKey, bool bIsInt)
{
	if(nKey <= 0 || nKey >= 0x7fffffff)
	{
		ErrorLn("spacename="<<nSpaceName<<" 无效的镜像数据key（不能小于等于0或者大于0x7fffffff）key="<<nKey);
		return false;
	}

	if(nSpaceName <= IMAGE_DATA_NAMESPACE_UNKNOWN || nSpaceName >= IMAGE_DATA_NAMESPACE_MAX)
	{
		ErrorLn("spacename="<<nSpaceName<<" 无效的镜像数据key（不能小于等于0或者大于0x7fffffff）key="<<nKey);
		return false;
	}

	TMAP_KEY::iterator itKey = m_mapImageKey[nSpaceName].find(nKey);
	if(itKey != m_mapImageKey[nSpaceName].end())
	{
		ErrorLn("spacename="<<nSpaceName<<" 重复注册了镜像数据key，key="<<nKey);
		return false;
	}

	m_mapImageKey[nSpaceName][nKey] = bIsInt;

	return true;
}

/** 
@param   
@param   
@return  
*/
bool CImageDataBank::SetValue(int nSpaceName, int nKey, int nValue)
{
	if(nSpaceName <= IMAGE_DATA_NAMESPACE_UNKNOWN || nSpaceName >= IMAGE_DATA_NAMESPACE_MAX)
	{
		return false;
	}

	int nType = GetImageDataType(nSpaceName, nKey);
	if(nType == -1)
	{
		ErrorLn("spacename="<<nSpaceName<<"的key="<<nKey<<" 没有注册");
		return false;
	}

	// 一定要卡死注册与使用的数据类型是否相同，否则有可能会出现一个key有字符型数据，又有整型数据
	if(nKey == 0)
	{
		ErrorLn("spacename="<<nSpaceName<<"的key="<<nKey<<" 注册的是字符型，不能当作int使用");
		return false;
	}

	return m_ImageDataBox[nSpaceName].SetValue(nKey, nValue);
}

/** 
@param   
@param   
@return  
*/
int CImageDataBank::GetValue(int nSpaceName, int nKey)
{
	if(nSpaceName <= IMAGE_DATA_NAMESPACE_UNKNOWN || nSpaceName >= IMAGE_DATA_NAMESPACE_MAX)
	{
		return false;
	}

	int nType = GetImageDataType(nSpaceName, nKey);
	if(nType == -1)
	{
		ErrorLn("spacename="<<nSpaceName<<"的key="<<nKey<<" 没有注册");
		return false;
	}

	// 一定要卡死注册与使用的数据类型是否相同，否则有可能会出现一个key有字符型数据，又有整型数据
	if(nKey == 0)
	{		
		return false;
	}

	return m_ImageDataBox[nSpaceName].GetValue(nKey);
}

#endif

/** 取得数据类型
@param   
@param   
@return  -1:表示没有注册,0:表示为字符型,1:表示整型
*/
int CImageDataBank::GetImageDataType(int nSpaceName, int nKey)
{
	if(nSpaceName <= IMAGE_DATA_NAMESPACE_UNKNOWN || nSpaceName >= IMAGE_DATA_NAMESPACE_MAX)
	{		
		return -1;
	}

#ifdef GUARD_SUPPORT_IMAGE_INT_UNIT
	TMAP_KEY::iterator itKey = m_mapImageKey[nSpaceName].find(nKey);
	if(itKey == m_mapImageKey[nSpaceName].end())
	{		
		return -1;
	}

	return (*itKey).second;

#else
	// 只支持字符串
	return 0;
#endif
}

/** 
@param   
@param   
@return  
*/
bool CImageDataBank::SetValue(int nSpaceName, int nKey, LPCSTR pValue, int nLen)
{
	if(nSpaceName <= IMAGE_DATA_NAMESPACE_UNKNOWN || nSpaceName >= IMAGE_DATA_NAMESPACE_MAX)
	{
		return false;
	}

	int nType = GetImageDataType(nSpaceName, nKey);
	if(nType == -1)
	{
		ErrorLn("spacename="<<nSpaceName<<",key="<<nKey<<" not register");
		return false;
	}

	// 一定要卡死注册与使用的数据类型是否相同，否则有可能会出现一个key有字符型数据，又有整型数据
	if(nKey == 1)
	{
		ErrorLn("spacename="<<nSpaceName<<",key="<<nKey<<" register int type, can't use by string type");
		return false;
	}

	return m_ImageDataBox[nSpaceName].SetValue(nKey, pValue, nLen);
}


/** 
@param   
@param   
@return  
*/
LPCSTR CImageDataBank::GetValue(int nSpaceName, int nKey, int &nDataLen)
{
	if(nSpaceName <= IMAGE_DATA_NAMESPACE_UNKNOWN || nSpaceName >= IMAGE_DATA_NAMESPACE_MAX)
	{
		return false;
	}

	int nType = GetImageDataType(nSpaceName, nKey);
	if(nType == -1)
	{
		ErrorLn("spacename="<<nSpaceName<<",key="<<nKey<<" not register");
		return false;
	}

	// 一定要卡死注册与使用的数据类型是否相同，否则有可能会出现一个key有字符型数据，又有整型数据
	if(nKey == 1)
	{		
		return false;
	}

	return m_ImageDataBox[nSpaceName].GetValue(nKey, nDataLen);
}