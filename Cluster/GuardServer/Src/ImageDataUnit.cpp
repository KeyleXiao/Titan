/*******************************************************************
** 文件名:	ImageDataUnit.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Long)（Forlion@126.com）
** 日  期:	2009/10/11 16:57
** 版  本:	1.0
** 描  述:	镜像数据单元
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 李界华
** 日  期: 2015-08-19
** 描  述: 将字符串的处理分离出来，不用占用额外的内存
********************************************************************/
#include "stdafx.h"
#include "ImageDataUnit.h"

/** 
@param   
@param   
@return  
*/
CImageDataIntUnit::CImageDataIntUnit(void)
{
	m_nData = 0x7fffffff;
	m_dwLastUpdateTime = 0;
}

/** 
@param   
@param   
@return  
*/
CImageDataIntUnit::CImageDataIntUnit(int nData)
{
	m_nData = nData;
	m_dwLastUpdateTime = 0;
}

/** 
@param   
@param   
@return  
*/
CImageDataIntUnit::~CImageDataIntUnit(void)
{
	m_nData = 0x7fffffff;
	m_dwLastUpdateTime = 0;
}

/** 赋值
@param   
@param   
@return  
*/ 
void CImageDataIntUnit::Set(int nData)
{
	m_nData = nData;
}

/** 获得
@param   
@param   
@return  
*/
int	CImageDataIntUnit::Get(void)
{
	return m_nData;
}

/** 取最后更新时间
@param   
@return  
*/
DWORD CImageDataIntUnit::GetLastUpdateTime(void)
{
	return m_dwLastUpdateTime;
}

/** 
@param   
@param   
@return  
*/ 
CImageDataIntUnit::operator int(void) const
{
	return m_nData;
}

//////// CImageDataCharArray /////////////////////////////////////////////////////
/** 
@param   
@param   
@return  
*/
CImageDataCharArray::CImageDataCharArray(void)
{
	// 初始buff
	memset(m_szBuff, 0 , sizeof(m_szBuff));	

	// 数据长度
	m_nDataLen = 0;
}

/** 
@param   
@param   
@return  
*/
CImageDataCharArray::CImageDataCharArray(LPCSTR pszData, int nLen)
{
	// 初始buff
	memset(m_szBuff, 0 , sizeof(m_szBuff));

	// 数据长度
	m_nDataLen = 0;

	if(pszData != NULL && nLen > 0)
	{	
		memcpy(m_szBuff, pszData, nLen);
		m_nDataLen = nLen;
	}
}

/** 
@param   
@param   
@return  
*/
CImageDataCharArray::~CImageDataCharArray(void)
{
	Clear();
}

/** 释放
@param   
@return  
*/
void CImageDataCharArray::Release(void)
{
	delete this;
}

/** 
@param   
@param   
@return  
*/
void CImageDataCharArray::Clear(void)
{
	// 如果是初始化内存，则不能释放
	memset(m_szBuff, 0 , sizeof(m_szBuff));

	// 数据长度
	m_nDataLen = 0;
}


/** 赋值
@param   
@param   
@return  
*/
void CImageDataCharArray::Set(LPCSTR pszData, int nLen)
{
	if(pszData != NULL && nLen > 0)
	{
		memcpy(m_szBuff, pszData, nLen);
		m_nDataLen = nLen;
	}
	else
	{
		Clear();
	}
}

/** 
@param   
@param   
@return  
*/
LPCSTR CImageDataCharArray::Get(int &nLen)
{
	nLen = m_nDataLen;

	return m_szBuff;
}

/** 取长度
@param   
@return  
*/
int	CImageDataCharArray::GetLength(void)
{
	return m_nDataLen;
}

/** 取最后更新时间
@param   
@return  
*/
DWORD CImageDataCharArray::GetLastUpdateTime(void)
{
	return m_dwLastUpdateTime;
}

//////// CImageDataString /////////////////////////////////////////////////////
/** 
@param   
@param   
@return  
*/
CImageDataString::CImageDataString(void)
{
	// 数据指针
	m_pData = NULL;

	// 数据长度
	m_nDataLen = 0;

	// 更新时间
	m_dwLastUpdateTime = 0;
}

/** 
@param   
@param   
@return  
*/
CImageDataString::CImageDataString(LPCSTR pszData, int nLen)
{
	// 数据指针
	m_pData = NULL; m_nDataLen = 0; m_dwLastUpdateTime = 0;
	
	if(pszData != NULL && nLen > 0)
	{
		if(NewBuff(nLen))
		{
			memcpy(m_pData, pszData, nLen);
			m_nDataLen = nLen;
			m_dwLastUpdateTime = getTickCount();
		}
	}
}

/** 
@param   
@param   
@return  
*/
CImageDataString::~CImageDataString(void)
{
	Clear();
}

/** 释放
@param   
@return  
*/
void CImageDataString::Release(void)
{
	delete this;
}

/** 
@param   
@param   
@return  
*/
void CImageDataString::Clear(void)
{
	// 如果是初始化内存，则不能释放
	if(m_pData != NULL)
	{
		SAFE_DELETEARRAY(m_pData);
	}

	// 数据长度
	m_nDataLen = 0;

	// 更新时间
	m_dwLastUpdateTime = 0;
}

/** 赋值
@param   
@param   
@return  
*/
void CImageDataString::Set(LPCSTR pszData, int nLen)
{
	bool bSet = false;
	if(pszData != NULL && nLen > 0)
	{
		if(NewBuff(nLen))
		{
			memcpy(m_pData, pszData, nLen);
			m_nDataLen = nLen;
			m_dwLastUpdateTime = getTickCount();

			bSet = true;
		}		
	}

	if(!bSet)
	{
		Clear();
	}
}

/** 
@param   
@param   
@return  
*/
LPCSTR CImageDataString::Get(int &nLen)
{
	nLen = m_nDataLen;

	return m_pData;
}

/** 取长度
@param   
@return  
*/
int	CImageDataString::GetLength(void)
{
	return m_nDataLen;
}

/** 取最后更新时间
@param   
@return  
*/
DWORD CImageDataString::GetLastUpdateTime(void)
{
	return m_dwLastUpdateTime;
}

/** 扩展内存
@param   
@param   
@return  
*/
bool CImageDataString::NewBuff(int nNewLen)
{
	if (nNewLen <= 0)
	{
		return false;
	}

	// 如果是初始化内存，则不能释放
	if(m_pData != NULL)
	{
		SAFE_DELETEARRAY(m_pData);
		m_nDataLen = 0;
	}

	// 新生成一片内存
	m_pData = new char[nNewLen];

	return true;
}