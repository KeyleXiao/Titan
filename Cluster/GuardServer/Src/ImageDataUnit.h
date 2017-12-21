/*******************************************************************
** 文件名:	ImageDataUnit.h 
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
#pragma once

// 字符串数组长度定义
#define STRUNIT_INIT_LEN		32

// 数字
class CImageDataIntUnit
{
public:
	/** 赋值
	@param   
	@param   
	@return  
	*/ 
	void			Set(int nData);

	/** 获得
	@param   
	@param   
	@return  
	*/
	int				Get(void);

	/** 取最后更新时间
	@param   
	@return  
	*/
	virtual DWORD	GetLastUpdateTime(void);

	/** 
	@param   
	@param   
	@return  
	*/ 
	operator		int(void) const;

	/** 
	@param   
	@param   
	@return  
	*/
	CImageDataIntUnit(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CImageDataIntUnit(int nData);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CImageDataIntUnit(void);
	
public:
	int				m_nData;

	// 更新时间
	DWORD			m_dwLastUpdateTime;
};


// 字符串数据类型
struct IImageDataChars
{
	/** 释放
	@param   
	@return  
	*/
	virtual void			Release(void) = 0;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual void			Clear(void) = 0;

	/** 赋值
	@param   
	@param   
	@return  
	*/
	virtual void			Set(LPCSTR pszData, int nLen) = 0;

	/** 取值
	@param   
	@return  
	*/
	virtual LPCSTR			Get(int &nLen) = 0;

	/** 取长度
	@param   
	@return  
	*/
	virtual int				GetLength(void) = 0;

	/** 取最后更新时间
	@param   
	@return  
	*/
	virtual DWORD			GetLastUpdateTime(void) = 0;
};


// 字符数组
class CImageDataCharArray : public IImageDataChars
{
public:
	//////// IImageDataChars //////////////////////////////////////////////////
	/** 释放
	@param   
	@return  
	*/
	virtual void			Release(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual void			Clear(void);

	/** 赋值
	@param   
	@param   
	@return  
	*/
	virtual void			Set(LPCSTR pszData, int nLen);

	/** 取值
	@param   
	@return  
	*/
	virtual LPCSTR			Get(int &nLen);

	/** 取长度
	@param   
	@return  
	*/
	virtual int				GetLength(void);

	/** 取最后更新时间
	@param   
	@return  
	*/
	virtual DWORD			GetLastUpdateTime(void);
	//////// CImageDataCharArray //////////////////////////////////////////////////
	/** 
	@param   
	@param   
	@return  
	*/
	CImageDataCharArray(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CImageDataCharArray(LPCSTR pszData, int nLen);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CImageDataCharArray(void);

private:
	char			m_szBuff[STRUNIT_INIT_LEN];

	// 数据长度
	int				m_nDataLen;

	// 更新时间
	DWORD			m_dwLastUpdateTime;
};


// 字符串
class CImageDataString : public IImageDataChars
{
public:
	//////// IImageDataChars //////////////////////////////////////////////////
	/** 释放
	@param   
	@return  
	*/
	virtual void			Release(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual void			Clear(void);

	/** 赋值
	@param   
	@param   
	@return  
	*/
	virtual void			Set(LPCSTR pszData, int nLen);

	/** 取值
	@param   
	@return  
	*/
	virtual LPCSTR			Get(int &nLen);

	/** 取长度
	@param   
	@return  
	*/
	virtual int				GetLength(void);
	
	/** 取最后更新时间
	@param   
	@return  
	*/
	virtual DWORD			GetLastUpdateTime(void);

	//////// CImageDataString //////////////////////////////////////////////////
	/** 
	@param   
	@param   
	@return  
	*/
	CImageDataString(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CImageDataString(LPCSTR pszData, int nLen);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CImageDataString(void);

private:
	/** 扩展内存
	@param   
	@param   
	@return  
	*/
	inline bool				NewBuff(int nNewLen);

private:
	// 数据指针
	char *			m_pData;

	// 数据长度
	int				m_nDataLen;

	// 更新时间
	DWORD			m_dwLastUpdateTime;
};
