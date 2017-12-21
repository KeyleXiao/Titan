/*******************************************************************
** 文件名:	ImageDataBox.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Long)（Forlion@126.com）
** 日  期:	2009/10/11 17:01
** 版  本:	1.0
** 描  述:	数据盒子（表示一个对像的数据集合）
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 李界华
** 日  期: 2015-08-19
** 描  述: 将字符串的处理分离出来，不用占用额外的内存
********************************************************************/
#include "stdafx.h"
#include "ImageDataBox.h"

/** 构造 
@param   
@param   
@return  
*/
CImageDataBox::CImageDataBox(void)
{
	// 空间名字
	m_nSpaceName = IMAGE_DATA_NAMESPACE_UNKNOWN;

	// 整型镜像数据
	m_mapDataIntUnit.clear();

	// 字符镜像数据
	m_mapDataStrUnit.clear();
}

/** 析构
@param   
@param   
@return  
*/
CImageDataBox::~CImageDataBox(void)
{

}

/** 清掉数据
@param   
@param   
@return  
*/
void CImageDataBox::Clean(void)
{
	// 释放字符new的内存（字型临时任务数据）
	TMAP_STRUNIT::iterator it  = m_mapDataStrUnit.begin();
	for( ; it != m_mapDataStrUnit.end(); ++it)
	{
		((*it).second)->Clear();
	}

	// 整型镜像数据
	m_mapDataIntUnit.clear();

	// 字符镜像数据
	m_mapDataStrUnit.clear();
}

/** 
@param   
@param   
@return  
*/
bool CImageDataBox::Create(int nSpaceName)
{
	m_nSpaceName = nSpaceName;

	return true;
}

/** 
@param   
@param   
@return  
*/
bool CImageDataBox::SetValue(int nKey, int nValue)
{
	TMAP_INTUNIT::iterator it = m_mapDataIntUnit.find(nKey);
	if(it != m_mapDataIntUnit.end())	
	{
		CImageDataIntUnit * pIntUnit = &((*it).second);
		pIntUnit->Set(nValue);
	}
	else
	{
		CImageDataIntUnit intunit(nValue);
		m_mapDataIntUnit[nKey] = intunit;
	}

	return true;
}

/** 
@param   
@param   
@return  
*/
bool CImageDataBox::SetValue(int nKey, LPCSTR pValue, int nLen)
{
	if(nLen <= 0 || pValue == NULL)
	{
		TMAP_STRUNIT::iterator it = m_mapDataStrUnit.find(nKey);
		if(it != m_mapDataStrUnit.end())
		{
			IImageDataChars * pStrUnit = (*it).second;
			pStrUnit->Set(pValue, nLen);
		}		
	}
	else
	{
		TMAP_STRUNIT::iterator it = m_mapDataStrUnit.find(nKey);
		if(it != m_mapDataStrUnit.end())
		{
			IImageDataChars * pStrUnit = (*it).second;
			
			if(nLen>= STRUNIT_INIT_LEN)
			{
				if(pStrUnit->GetLength() < STRUNIT_INIT_LEN)
				{
					SAFE_RELEASE(pStrUnit);
					pStrUnit = new CImageDataString(pValue, nLen);
				}
			}
			else
			{
				if(pStrUnit->GetLength() >= STRUNIT_INIT_LEN)
				{
					SAFE_RELEASE(pStrUnit);
					pStrUnit = new CImageDataCharArray(pValue, nLen);
				}
			}

			if(pStrUnit)
			{
				pStrUnit->Set(pValue, nLen);
			}

		}
		else
		{
			IImageDataChars *pStrUnit = NULL;
			if (nLen >= STRUNIT_INIT_LEN)
			{
				pStrUnit = new CImageDataString(pValue, nLen);
			}
			else
			{
				pStrUnit = new CImageDataCharArray(pValue, nLen);
			}

			if (pStrUnit)
			{
				m_mapDataStrUnit[nKey] = pStrUnit;
			}
		}
	}

#ifdef GUARD_PRINT_DEBUG_INF
	TraceLn("set image data, key="<<nKey<<", datalen="<<nLen);
#endif
	return true;
}

/** 
@param   
@param   
@return  
*/
int CImageDataBox::GetValue(int nKey)
{
	TMAP_INTUNIT::iterator it = m_mapDataIntUnit.find(nKey);
	if(it != m_mapDataIntUnit.end())
	{
		return ((*it).second).Get();
	}

	return 0;
}

/** 
@param   
@param   
@return  
*/
LPCSTR CImageDataBox::GetValue(int nKey, int &nDataLen)
{
	TMAP_STRUNIT::iterator it = m_mapDataStrUnit.find(nKey);
	if(it != m_mapDataStrUnit.end())
	{
		return ((*it).second)->Get(nDataLen);
	}

	nDataLen = 0;
	return NULL;
}