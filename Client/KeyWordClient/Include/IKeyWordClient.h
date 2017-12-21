/*******************************************************************
** 文件名:	IKeyWordClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武
** 日  期:	2008/12/1
** 版  本:	1.0
** 描  述:	关键字处理客户端接口定义
** 应  用:	
********************************************************************/
#ifndef __I_KeyWordClient_H__
#define __I_KeyWordClient_H__

#include "IKeyWord.h"

// 打印调试信息,并设为测试模式
#define KEYWORD_PRINT_DEBUG_INF


// tolua_begin

/// 过滤关键字接口类型定义
enum EmKeyWordInterfaceType
{
	KeyWordInterface_Default,		// 默认类型
	KeyWordInterface_Name,			// 名称类型
	
	KeyWordInterface_Max,			/// 最大类型数
};

//关键字处理客户端
struct IKeyWordClient
{	   
	// 释放对象
	virtual void release(void) = NULL;



	/** 是否有关键字(名称屏蔽词)
	@param   char * szInStr : 要检查的字串buffer
	@param   int nInSize	: 要检查的字串的大小
	@return  bool :	true:找到关键字，false:没有找到
	@note	 nInSize不能大于等于 65535 [64K]
	@warning 
	*/
	virtual bool ExistKeyWordOfName(const char * szInStr,const int nInSize) = NULL;

	/** 过滤关键字(名称屏蔽词)
	@param   char * szInStr : 要过滤的字串buffer
	@param   int nInSize	: 要过滤的字串的大小
	@param   char * szOutStr : 过滤后输出字串buffer
	@param   int nOutSize	: 输出字串buffer大小
	@return  bool :	true:找到关键字，false:没有找到
	@note	 nOutSize不能小于nInSize,nInSize,nOutSize要大于0,nInSize不能大于等于 65535 [64K]
	@warning 
	*/
	virtual bool FilterKeyWordOfName(const char * szInStr,const int nInSize,const char * szOutStr,const int nOutSize) = NULL;


	/** 过滤关键字
	@param   char * szInStr : 要过滤的字串buffer
	@param   int nInSize	: 要过滤的字串的大小
	@param   char * szOutStr : 过滤后输出字串buffer
	@param   int nOutSize	: 输出字串buffer大小
	@return  bool :	true:找到关键字，false:没有找到
	@note	 nOutSize不能小于nInSize,nInSize,nOutSize要大于0,nInSize不能大于等于 65535 [64K]
	@warning
	*/
	virtual bool FilterKeyWord(const char * szInStr, const int nInSize, const char * szOutStr, const int nOutSize) = NULL;

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


	/** 过滤关键字，针对格式化文本
	@param   char * szInStr : 要过滤的字符串，字符串不要太长。
	*/
	virtual std::string FilterStringEx(const char* szInStr) = NULL;


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


	/////类型过滤关键字接口///////////////////////////////////////////////////////////////////// 

	/** 取得指定类型过滤关键字接口
	@param   cDWORD dwType  参考 EmKeyWordInterfaceType
	@return  IKeyWord * pKeyWord 过滤关键字接口
	*/
	virtual IKeyWord * GetKeyWordInterface(DWORD dwType) = NULL;

	// tolua_end

	/** 重新加载关键字列表，支持定义要加入和删除的关键字列表
	@param   cDWORD dwType  参考 EmKeyWordInterfaceType
	@param   addKeys 要加入的关键字列表
	@param   removeKeys 要加入的关键字列表
	@return  bool 成功否
	*/
	virtual bool ReLoadKeyDictList(DWORD dwType,std::list<std::string>&addKeys,std::list<std::string>&removeKeys) = NULL;

	// tolua_begin
};



// tolua_end


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_LIB) || defined(KEYWORDCLIENT_STATIC_LIB)	// 静态链接
#	pragma comment(lib, MAKE_LIB_NAME(KeyWordClient))
extern "C" IKeyWordClient * CreateKeyWordClient(void);
#	define	CreateKeyWordClientProc	CreateKeyWordClient
#	define	CloseKeyWordClient()
#elif !defined(KEYWORDCLIENT_EXPORTS) && defined(DYN_LIB) // 动态链接
#	pragma comment(lib, MAKE_DLL_LIB_NAME(KeyWordClient))
API_IMPORT IKeyWordClient * CreateKeyWordClient(void);
#	define	CreateKeyWordClientProc	CreateKeyWordClient
#	define	CloseKeyWordClient()
#else												// 动态加载
typedef IKeyWordClient * (RKT_CDECL *procKeyWordClient)(void);
#	define	CreateKeyWordClientProc	DllApi<procKeyWordClient>::load(MAKE_DLL_NAME(KeyWordClient), "CreateKeyWordClient")
#	define	CloseKeyWordClient		DllApi<procKeyWordClient>::freelib
#endif


#endif	//__I_KeyWordClient_H__
