/*******************************************************************
** 文件名:	e:\CoGame\DBEngine\include\IDBEngine.h
** 版  权:	(C) 深圳未名网络技术有限公司(www.weimingtech.com)
** 创建人:	隆寒辉(Forlion Lon)
** 日  期:	2007/6/20  20:50
** 版  本:	1.0
** 描  述:	数据库引擎接口文件
** 应  用:  
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 

不定长的输出参数字段使用方法：(forlion:2009-12-24支持)
　1、输出参数字段要定义成不定长，在DBProcedure.xml将source定义成'nr'
  2、输出参数字段要定义成不定长，datatype必须定义成'bigbin'
  3、***当执行存储过程返回后，数据库返回的真正长度放在返回buffer的前面四个字节（int字节）

不定长的结果集字段使用方法：
　1、结果集字段要定义成不定长，在DBProcedure.xml将source定义成'nr'
  2、结果集字段要定义成不定长，datatype必须定义成'bin'
  3、***当执行存储过程返回后，数据库返回的真正长度放在返回buffer的前面四个字节（int字节）

不定长的输入参数使用方法：
　1、如果输入参数定义成不定长，"inputdata"->"size"定义成0，"param"->"size"定义成0
  2、****数据库引擎当在载入输入参数时，遇到这种不定长的输入参数，则会取前面四个字节，当作此参数的长度
********************************************************************/
#pragma once

#include "IDBRetSink.h"

// 申明
struct ILogicThread;

/*! \addtogroup DBEngine
*  数据库引擎
*  @{
*/

/// 数据库请求返回值定义
enum
{	/// .....................			 /// 以上为成功的方法，只要返回值大于等于0，则表示成功
	DBRET_CODE_SUCCEED = 1,				/// 成功
	// ..........................		/// 数据库返回错误，具体由数据库自定义
	DERET_CODE_GAMEWORLDERROR = -9996,	/// 严重的错误，调错游戏世界
	DBRET_CODE_RETRY = -9997,			/// 数据库引擎要返回重试
	DBRET_CODE_EXCEPTION = -9998,		/// 引擎异常（不会重试）
	DERET_CODE_THROW = -9999,			/// 被抛包（不会重试，指那些直接抛包的请求，还没执行过请求）		
};

/// 数据库引擎状态信息ID
enum
{
	DBENGINE_STATEINFO_OK = 0,			/// 首次连接成功
	DBENGINE_STATEINFO_DISCONNECT,		/// 与数据库断开连接
	DBENGINE_STATEINFO_RENEWCONNECT,	/// 恢复连接成功
	DBENGINE_STATEINFO_THROW,			/// 执行抛包策略
	DBENGINE_STATEINFO_EXCEPTION,		/// 数据库引擎异常
	DBENGINE_STATEINFO_PERFORMANCE,		/// 数据库性能报擎
	DBENGINE_STATEINFO_SWITCHHOST,		/// 切换数据库服务器	
	DBENGINE_STATEINFO_GAMEWORLDERROR,	/// 调错游戏世界，超严重问题
	DBENGINE_STATEINFO_DBCONNECTERROR,	/// DB连接错误，超超级严重问题
};

/// 数据源定义
enum
{
	DATA_SOURCE_DD = 0,					/// 直通数据
	DATA_SOURCE_SP,						/// 存储过程

	DATA_SOURCE_NR,						/// 不定长字段
};

/// 存储过程返回值长度
#define SP_RETURNVALUE_SIZE				4

/// 返回值的宏定义
#define RETUREVAL_PARAM_DESC			"@returnvalue"

/// 存储过程返回描述长度
#define SP_RETURNDESC_SIZE				128

/// 返回描述的宏定义
#define RETUREDESC_PARAM_DESC			"@returndesc"

/// 是否为引擎错误
#define ISDBENGINE_ERROR(x)				(x==DBRET_CODE_RETRY||x==DBRET_CODE_EXCEPTION||x==DERET_CODE_THROW)

/// 最大“不定长输出参数”长度(ADO最多8K长度)
#define NR_OUTPUTVARIABLE_MAXLEN		8000

/// 若多久ms内没连上数据库，则切换数据库IP
#define RECONNECT_TIMEOUT_SWITCHHOST	(10 * 60 * 1000)

/// 最大存储过程ID
#define SP_MAX_CMDID					8092

/// 异步队列内部信息
struct SAsynQueueStateInfo
{
	// 数据库ＩＤ
	DWORD		dwDataBaseID;

	// 数据库昵称
	char		szDataBaseNickName[256];

	// 异步队列ID
	DWORD		dwQueueID;

	// 运行队列长度
	int			nRunRequestQty;

	// 重试队列长度
	int			nRetryRequestQty;

	SAsynQueueStateInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}

	void Clean(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

/// 数据库状态信息回调接口
struct IDBEngineStateSink
{
	/** 如果数据引擎有状态信息回调给上层
	@param   nInforID：信息ID，参考上层定义
	@param   pszDesc：信息描述
	@param   
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void			OnState(int nInfoID, LPCSTR pszDesc) = 0;
};

/// 数据库引擎接口
struct IDBEngine
{
	/** 释放数据库引擎
	@param   
	@param   
	@param   
	@return  
	@note     
	@warning 
	@retval buffer 
	*/	
	virtual void			Release(void) = 0;

	/** 增加状态信息回调接口
	@param   pSink :回调接口 
	@param   
	@param   
	@return  成功返回true，失败返回false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			AddStateSink(IDBEngineStateSink * pSink) = 0;

	/** 移队状态信息回调接口
	@param   pSink :回调接口 
	@param   
	@param   
	@return  成功返回true，失败返回false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			RemoveStateSink(IDBEngineStateSink * pSink) = 0;

	/** 执行一个存储过程
	@param   dwCmdID		：cmdid
	@param   dwQueueIndex	: 异步队列索引号
	@param   pszInData		：输入数据
	@param   nInDataLen		：输入数据长度
	@param   pDBRetSink		：结果返回接口	
	@param   nGameWorldID	: 向何个游戏世界数据库执行，若为公共区,必填游戏世界id
	@return  		
	@retval buffer 
	*/
	virtual bool			ExecuteSP(DWORD dwCmdID, DWORD dwQueueIndex, LPCSTR pszInData, int nInDataLen, IDBRetSink * pDBRetSink, int nGameWorldID = 0) = 0;


	/** 执行一个存储过程
	@param   dwCmdID		：cmdid
	@param   dwQueueIndex	: 异步队列索引号
	@param   pszInData		：输入数据
	@param   nInDataLen		：输入数据长度
	@param   pDBRetSink		：结果返回接口	
	@param   nGameWorldID	: 向何个游戏世界数据库执行，若为公共区,必填游戏世界id
	@return  		
	@retval buffer 
	*/
	virtual bool			ExecuteSP(DWORD dwCmdID, DWORD dwQueueIndex, LPCSTR pszInData, int nInDataLen, IDBRetSink * pDBRetSink, IDBRetSink * pRealDBRetSink, int nGameWorldID = 0) = 0;

	/** 取得某存储过程的输出数据大小和结果集大小
	@param   测试程序使用
	@param   
	@return  找到：返回成功，未找到：返回失败
	*/
	virtual bool			GetSPOutdataSize(DWORD dwCmdID, int &nOutParamSize, int &nResetSize) = 0;

	/** 取得数据库异步队列运行状态
	@param   pInfoArray ：存放信息的数组，返回此数组内
	@param   nArrayCount：输入:数组大小，输出:信息条数
	@return  返回失败，表示数组太小，否则成功
	*/
	virtual bool			GetAsynQueueRunState(SAsynQueueStateInfo * pInfoArray, int &nArrayCount) = 0;

	/** 在公共区内，需要调用某个存储过程，必须先注册方能调用
	@param   
	@param   
	@return  返回false：表示dwCmdID太大或者无效
	*/
	virtual bool			RegisterSP(DWORD dwCmdID) = 0;

	/** 校验是否有某个游戏世界的有效
	@param   
	@param   
	@return  
	*/
	virtual bool			CheckGameWorldValid(int nGameWorldID) = 0;

	/** 某个存储过程是否注册过了
	@param   
	@param   
	@return  
	*/
	virtual bool			IsRegisterSP(DWORD dwCmdID) = 0;

	/** 设置执行存储过程时是否对QueueIndex的合法性进行校验
	@param   
	@return  
	*/
	virtual void			SetCheckQueueIndex(bool bCheckValid) = 0;

	/** 返回是否对QueueIndex的合法性进行校验
	@param   
	@return  
	*/
	virtual bool			IsCheckQueueIndex(void) = 0;

	/** 创建对某个游戏世界的DB连接
	@param		: 游戏世界ID
	@return  
	*/
	virtual bool			CreateConnect(int nGameWorldID) = 0;

	/** 释放对某个游戏世界的DB连接
	@param		: 游戏世界ID
	@param		: 是否强制释放
	@return  
	*/
	virtual bool			ReleaseConnect(int nGameWorldID, bool bForce) = 0;

	/** 设定:切换到备用库一段时间之后是否自动切换回主用库
	@param		: 游戏世界ID
	@param		: 数据库ID
	@param		: 是否自动切换回主用库
	@return
	*/
	virtual bool			SetAutoSwitchMain(int nGameWorldID, int nDatabaseID, bool bAutoSwitchMain) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
/** 创建数据库引擎
@param   pThreadPool ：外部传入的线程池，内部用来创建队列线程
@param   pLogicThread：逻辑线程，用来回调返回结果
@param   pszDBServerPath：数据库配置名
@param   pszDBProcedurePath：存储过程配置名
@param   nSvrType：服务器类型，主要是用来标明在不同的服务器上创建数量不等的线程队列，以前没有这个参数，
后来发现每个服务器创建的线程多得吓人，很多线程不必要创建的。比如说：场景服，根本不用写日志（日志都交给
数据库前端去处理了），但也创建了12个线程，影响场景服性能
@param   
@return  
*/
#if defined(_LIB) || defined(DBENGINE_STATIC_LIB)	/// 静态库版本
	#	pragma comment(lib, MAKE_LIB_NAME(DBEngine))
	extern "C" IDBEngine * CreateDBEngine(ThreadPool * pThreadPool, ILogicThread * pLogicThread, LPCSTR pszDBServerPath, LPCSTR pszDBProcedurePath, int nSvrType, LPCSTR pszLogPath, LPCSTR pszPerforLog, int nThisGameWorldID, bool bPublicGameWorld);
	#	define	CreateDBEngineProc	CreateDBEngine
#else													/// 动态库版本
	typedef IDBEngine * (RKT_CDECL *procCreateDBEngine)(ThreadPool * pThreadPool, ILogicThread * pLogicThread, LPCSTR pszDBServerPath, LPCSTR pszDBProcedurePath, int nSvrType, LPCSTR pszLogPath, LPCSTR pszPerforLog, int nThisGameWorldID, bool bPublicGameWorld);
	#	define	CreateDBEngineProc	    DllApi<procCreateDBEngine>::load(MAKE_DLL_NAME(DBEngine), "CreateDBEngine")

    typedef IDBEngine * (RKT_CDECL *procCreateDBEngineEx)(ThreadPool * pThreadPool, ILogicThread * pLogicThread, LPVOID pDBServerData, int nSize, LPCSTR pszDBProcedurePath, int nSvrType, LPCSTR pszLogPath, LPCSTR pszPerforLog, int nThisGameWorldID, bool bPublicGameWorld);
    #	define	CreateDBEngineExProc	DllApi<procCreateDBEngineEx>::load(MAKE_DLL_NAME(DBEngine), "CreateDBEngineEx")
#endif

