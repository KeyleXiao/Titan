/*******************************************************************
** 文件名:	IKeyWord.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武
** 日  期:	2008/12/1
** 版  本:	1.0
** 描  述:	关键字处理公共模块接口定义
** 应  用:	
********************************************************************/
#ifndef __I_KeyWord_H__
#define __I_KeyWord_H__


// tolua_begin

//关键字处理公共模块
struct IKeyWord
{	 
	// tolua_end

	// 创建
	virtual bool Create(void) = NULL;

	// 指定关键字典文件创建
	virtual bool Create(const char* szKeyDictFileName) = NULL;

	// 释放对象
	virtual void Release(void) = NULL;

	// tolua_begin

	/** 是否有关键字
	@param   char * szInStr : 要检查的字串buffer
	@param   int nInSize	: 要检查的字串的大小
	@return  bool :	true:找到关键字，false:没有找到
	@note	 nInSize不能大于等于 65535 [64K]
	@warning 
	*/
	virtual bool ExistKeyWord(const char * szInStr,const int nInSize) = NULL;

	/** 过滤关键字
	@param   char * szInStr : 要过滤的字串buffer
	@param   int nInSize	: 要过滤的字串的大小
	@param   char * szOutStr : 过滤后输出字串buffer
	@param   int nOutSize	: 输出字串buffer大小
	@return  bool :	true:找到关键字，false:没有找到
	@note	 nOutSize不能小于nInSize,nInSize,nOutSize要大于0,nInSize不能大于等于 65535 [64K]
	@warning 
	*/
	virtual bool FilterKeyWord(const char * szInStr,const int nInSize,const char * szOutStr,const int nOutSize) = NULL;

	/** 过滤关键字
	@param   char * szInStr : 要过滤的字串buffer
	@param   int nInSize	: 要过滤的字串的大小
	@param   char * szOutStr : 过滤后输出字串buffer
	@param   int nOutSize	: 输出字串buffer大小
	@return  int :	输出字串的长度  0:出错 ,>0:输出字串的长度
	@note	 nOutSize不能小于nInSize,nInSize,nOutSize要大于0,nInSize不能大于等于 65535 [64K]
	@warning 
	*/
	virtual int Filter(const char * szInStr,const int nInSize,const char * szOutStr,const int nOutSize) = NULL;


	/** 过滤关键字,输出使用内部buffer指针
	@param   char * szInStr : 要过滤的字串buffer
	@param   int nInSize	: 要过滤的字串的大小
	@return  const char * :	输出使用内部字串buffer指针
	@note		nInSize不能大于等于 65535 [64K]
	@warning	请在调用过滤后立即处理输出字串,以后不再用此内存指针
	*/
	virtual const char * FilterString(const char * szInStr,const int nInSize) = NULL;


	/** 取得能过滤的最大字串长度
	@return  能过滤的最大字串长度
	@note	
	@warning
	*/
	virtual int getFilterStringMaxSize() = NULL;

	//////////////////////////////////////////////////////////////////////////

	/** 转换Gb码繁体到Gb码简体
	@param   char * szInStr : 要转换Gb码繁体的字串buffer
	@param   int nInSize	: 要转换的字串的大小
	@param   char * szOutStr : 转换后输出Gb码简体字串buffer
	@param   int nOutSize	: 输出字串buffer大小
	@return  int :	输出字串的长度  0:出错 ,>0:输出字串的长度
	@note	 nOutSize不能小于nInSize,nInSize,nOutSize要大于0,nInSize不能大于等于 65535 [64K]
	@warning 
	*/
	virtual int GB_T2S(const WCHAR * szInStr,const int nInSize,const WCHAR * szOutStr,const int nOutSize) = NULL;

	/** 转换Gb码简体到Gb码繁体
	@param   char * szInStr : 要转换Gb码简体的字串buffer
	@param   int nInSize	: 要转换的字串的大小
	@param   char * szOutStr : 转换后输出Gb码繁体字串buffer
	@param   int nOutSize	: 输出字串buffer大小
	@return  int :	输出字串的长度  0:出错 ,>0:输出字串的长度
	@note	 nOutSize不能小于nInSize,nInSize,nOutSize要大于0,nInSize不能大于等于 65535 [64K]
	@warning 
	*/
	virtual int GB_S2T(const WCHAR * szInStr,const int nInSize,const WCHAR * szOutStr,const int nOutSize) = NULL;

	//////////////////////////////////////////////////////////////////////////
	//  中文汉字转拼音

	/** 中文转拼音，非中文汉字不变,nOutSize>nInSize*3
	@param   char * szInStr : 要转换中文的字串buffer
	@param   int nInSize	: 要转换的字串的大小
	@param   char * szOutStr : 转换后输出拼音字串buffer
	@param   int nOutSize	: 输出字串buffer大小
	@return  int :	输出字串的长度  0:出错 ,>0:输出字串的长度
	@note	 nOutSize>nInSize*3,nInSize,nOutSize要大于0,nInSize不能大于等于 65535 [64K]
	@warning 
	*/
	virtual int HZ2PY(const char * szInStr,const int nInSize,const char * szOutStr,const int nOutSize) = NULL;

	/** 中文转拼音只取首字母，非中文汉字不变
	@param   char * szInStr : 要转换中文的字串buffer
	@param   int nInSize	: 要转换的字串的大小
	@param   char * szOutStr : 转换后输出拼音字串buffer
	@param   int nOutSize	: 输出字串buffer大小
	@return  int :	输出字串的长度  0:出错 ,>0:输出字串的长度
	@note	 nOutSize>nInSize,nInSize,nOutSize要大于0,nInSize不能大于等于 65535 [64K]
	@warning 
	*/
	virtual int HZ2PYA(const char * szInStr,const int nInSize,const char * szOutStr,const int nOutSize) = NULL;

	/** 取得一个汉字的所有同音字,szOutStr的空间要足够大,来保存所有同音字
	@param   const WCHAR wch : 要取同音字的中文字符
	@param   char * szOutStr : 转换后输出所有同音字字串buffer
	@param   int nOutSize	: 输出字串buffer大小
	@return  int :	输出字串的长度  0:出错 ,>0:输出字串的长度
	@note	 
	@warning szOutStr的空间要足够大,来保存所有同音字
	*/
	virtual int getTYZ(const WCHAR wch,const char * szOutStr,const int nOutSize) = NULL;

	// tolua_end

	/** 指定要动态加入的关键字列表
	@param   keys
	*/
	virtual bool SetAddDicKeyList(std::list<std::string>&keys) = NULL;

	/**  要动态删除的关键字列表
	@param   keys
	*/
	virtual bool SetRemoveDicKeyList(std::list<std::string>&keys) = NULL;

	// tolua_begin

};



// tolua_end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_LIB) || defined(KEYWORD_STATIC_LIB)	/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(KeyWord))
extern "C" IKeyWord * CreateKeyWord(const char* szKeyDictFileName);
#	define	CreateKeyWordProc	CreateKeyWord
#else												/// 动态库版本
typedef IKeyWord * (RKT_CDECL *procKeyWord)(const char* szKeyDictFileName);
#	define	CreateKeyWordProc	DllApi<procKeyWord>::load(MAKE_DLL_NAME(KeyWord), "CreateKeyWord")
#endif


#endif	//__I_KeyWord_H__
