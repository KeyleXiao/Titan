/**
* Created by PeakGao 2008.01.03
*/

#ifndef RKT_API_H
#define RKT_API_H

#include "Common.h"
#include <string>

#ifdef SUPPORT_API

namespace rkt {

	/// 获得系统Tick数
	RKT_API ulong getTickCount();

	/// 采取高性能计时器获取的Tick数
	RKT_API ulong getTickCountEx();
	RKT_API float getTickCountExf();

	/// 线程休眠（毫秒）
	RKT_API void sleep(ulong milliSeconds);

	/// 获取系统高性能计数
	RKT_API int64 getPerformanceCount();

	/// 获取系统当前CPU频率
	RKT_API LONGLONG getPerformanceFrequency();

	/// 是否是调试器模式
	RKT_API bool isDebuggerPresent();

	/// 调试中断
//	RKT_API void debuggerBreak(const char* exp, const char* file, uint line, int* staticIntVarEqualZero = 0);

	/// 获取全局变量（用于IGlobalClient和IGlobalServer，没有想到什么好办法）
	RKT_API void* getGlobal();

	/// 设置全局对象（用于IGlobalClient和IGlobalServer，没有想到什么好办法）
	RKT_API void setGlobal(void* global);

	/// 获取是否是调试模式
	RKT_API bool isDebugMode();

	/// 设置是否是调试模式
	RKT_API void setDebugMode(bool debugMode);

	/// 本进程是否为公共区
	RKT_API	bool isPublicGameWorld();

	/// 设置本进程为公共区
	RKT_API	void setPublicGameWorld();

	// 设置本进程为普通区
	RKT_API	void setGeneralGameWorld();

	/// 获得当前游戏世界ID
	RKT_API	int  getThisGameWorldID();

	/// 设置当前游戏世界ID
	RKT_API	void setThisGameWorldID(int nGameWorldID, bool bAgreeReset = false);

	/// 获得当前公共区游戏世界ID
	RKT_API	int  getPublicGameWorldID();

	/// 设置公共区游戏世界ID
	RKT_API	void setPublicGameWorldID(int nGameWorldID, bool bAgreeReset = false);

	/// 设置游戏服务器类型和SubID
	RKT_API	void setGameServerTypeSubID(int nServerType, int nServerSubID);

	/// 获得当前游戏服务器类型
	RKT_API	int getGameServerType();

	/// 获得当前游戏服务器SubID
	RKT_API	int getGameServerSubID();

    /// 时间数值转换成时间结构
    RKT_API tm ConvertTimeValueToStruct(DWORD_PTR dwTime);

    /// 判断浮点数是否为0
    RKT_API bool isFloatEqualZero(float f);
    /// 浮点数为0时将返回默认值，否则返回原值
    RKT_API float floatZeroSet(float f, float defVal);

	//-------------------------------------------------------------------------------------
/*
	/// 几个安全的字符串函数
	RKT_API char* sstrcat(char* dest, const char* src);
	RKT_API char* sstrcpy(char* dest, const char* src);
	RKT_API char* sstrncpy(char* dest, const char* src, size_t count);

	/// 下一个字符
	RKT_API char* charNext(const char* str);

	/// 前一个字符
	RKT_API char* charPrev(const char* start, const char* cur);*/

	/// 获取本台机器的物理内存大小
	RKT_API size_t getPhysicalMemorySize();

	/// 获得本台机器的物理内存已使用大小
	RKT_API size_t getPhysicalMemoryUsedSize();

	/// 获取本台机器的虚拟内存大小
	RKT_API size_t getVirtualMemorySize();

	/// 获取本台机器的虚拟内存已使用大小
	RKT_API	size_t getVirtualMemoryUsedSize();

	/// 获取当前进程的物理内存已使用大小
	RKT_API	size_t getPhysicalMemoryUsedSizeByCurrentProcess();

	/// 获取当前进程的虚拟内存已使用大小
	RKT_API	size_t getVirtualMemoryUsedSizeByCurrentProcess();

	/// 获得本台机器的CPU占用百分比
	RKT_API double getCPUCurrentUsedValue();

	/// 获得当前进程的CPU占用百分比
	RKT_API double getCPUCurrentUsedValueByCurrentProcess();

	// 检测进程占用CPU资源
	// 参数int nProcessID: 目标进程ID,为0当前进程
	// 返回值:返回该进程占用CPU量,单核上的占用量
	RKT_API double getCPUCurrentUsedValueByProcessID(int nProcessID);

	/// 获取物理处理器个数（如双核就有2个）
	RKT_API	size_t getPhysicalProcessorCount();

	/// 获取屏幕分辨率
	RKT_API bool getDisplayMode(int* w, int* h, int* bits = 0, int* frequency = 0);

	/// 执行shell程序
	RKT_API void shellExecute(const char* operation, const char* file, const char* parameters = 0);

	// MessageBox
	enum
	{
		btnOk					= 0x00000000L,
		btnOkCancel             = 0x00000001L,
		btnAbortRetryIgnore     = 0x00000002L,
		btnYesNoCancel          = 0x00000003L,
		btnYesNo                = 0x00000004L,
		btnRetryCancel          = 0x00000005L,

		iconError				= 0x00000010L,
		iconQuestion			= 0x00000020L,
		iconWarning				= 0x00000030L,
		iconInfomation			= 0x00000040L,

		idOK					= 1,
		idCancel,
		idAbort,
		idRetry,
		idIgnore,
		idYes,
		idNo,
	};

	/// 消息框(type参考上面的枚举定义，如果需要其他定义的值，直接用win32定义的)
	RKT_API int msgbox(const char* text, const char* caption, uint type = 0);

	/// 光闭windows操作系统
	RKT_API bool shutdownWindows();

	/// 获取调试中断的使能状态
	RKT_API bool isDebuggerBreakEnabled();

	/// 设置调试中断的使能状态
	RKT_API void setDebuggerBreakEnabled(bool setting);

#ifdef RKT_DEBUG
#	define DebuggerBreak(exp)		{static bool __s_ignore__ = false; (void)( (exp) || (debuggerBreak(#exp, __FILE__, __LINE__, &__s_once__), 0) );}
#else
#	define DebuggerBreak(exp)		((void)0)
#endif

    // 快速置换整数数值
    RKT_API void fast_swap(int& a, int& b);

    // 多线程安全，产生一个随机数
    RKT_API int mt_rand();

    // 多线程安全，产生一个指定范围内[_min, _max]的随机数
    RKT_API int mt_range_rand(int _min, int _max);

	/// 产生一个[0, 32767]之间的随机数，如果 _seek 为0，则使用全局的随机种子，否则使用局部随机种子
	RKT_API int rkt_rand(ulong* _seek);

	/// 产生一个指定范围内[_min, _max]的随机数，如果 _seek 为0，则使用全局的随机种子，否则使用局部随机种子
	RKT_API int rkt_range_rand(int _min, int _max, ulong* _seek = 0);


	#define GlobalRand(r)				{(r) = rkt_rand(0);}
	#define GlobalRangeRand(r, l, u)	{(r) = rkt_range_rand((l), (u), 0);}
	#define LocalRand(r)				{static ulong __seek__##__LINE__ = rand(); (r) = rkt_rand(&__seek__##__LINE__);}
	#define LocalRangeRand(r, l, u)		{static ulong __seek__##__LINE__ = rand(); (r) = rkt_range_rand((l), (u), &__seek__##__LINE__);}


#ifdef USE_GLOBAL_SEEK
#	define rktRand(r)					GlobalRand(r)
#	define rktRangeRand(r, l, u)		GlobalRangeRand(r, l, u)
#else
#	define rktRand(r)					LocalRand(r)
#	define rktRangeRand(r, l, u)		LocalRangeRand(r, l, u)
#endif


	/// 获取windows的版本信息，返回版本描述信息，如果要关心具体的版本号，可以传入有效的 major, minor 变量地址
	RKT_API const char* getWinVersion(ulong* major, ulong* minor);

	/// 获取QQ信息（防止人家查看函数导出，这里把QQ改为IM）
	RKT_API bool getIMInfo(std::string& qqinfo);

	/// 获取指定的进程的数目，可用于判断进程是否存在
	RKT_API int getProcessCount(const char* exename, bool onlyInclude = false);

	/// 扫描进程列表，以'|'分隔
	RKT_API void scanProcess(std::string& processlist);

	/// 终止指定的进程
	RKT_API void terminateProcess(const char* exename, bool onlyInclude = false);

	/// 拷贝到剪贴板
	RKT_API void copyToClipboard(const char* text);

	/// 从剪贴板粘贴文本
	RKT_API const char* copyFromClipboard();

	/// 获取模块(exe/dll)的文件版本号(x.y.z.w)
	RKT_API bool getModuleFileVersion(const char* szFileName, std::string& strVer);

	/// 获取模块的信息，忽略的填写0
	RKT_API bool getModuleInfo(const char* szFileName, std::string* strVer = 0, std::string* strCompanyName = 0, std::string* strFileDesc = 0);

	/// 获取CPU的标识
	RKT_API bool getProcessorName(std::string& cpu);

	/// 获取显卡的标识
	RKT_API bool getVideoCardInfo(std::string& video);

	RKT_API const char* getHardDriveSerialNumber();

	/// 获取计算机的ID（0:获取物理硬盘的序列号产生ID, 1:获取MAC产生ID, 2:获取C盘卷标产生ID, -1:依次采取0-2(当ID不为0时停止)）
	RKT_API ulong getComputerID(int type = -1);

	/// 粗略判断是否是网吧用户
	RKT_API bool isNetworkBarUser();

	/// 创建跟指定QQ的连接
	RKT_API bool createQQSession(const char* qq);

	/* LNID
	LNID	Version		Lauguage	Nation
	0000	多字节版	简体		中国大陆
	0101	国际版		简体		马来西亚
	0201	国际版		繁体		港澳
	0202	国际版		繁体		台湾
	0301	国际版		越南语		越南
	*/
	RKT_API ulong getLNID();

	enum EncodingType
	{
		EncodingType_Unknow,
		EncodingType_Legacy,
		EncodingType_UTF8,
		//EncodingType_UNICODE, // 这种暂时不支持
	};

	/// 检查缓冲区的编码标识
	RKT_API EncodingType checkBufferEncodingType(const void* buff, size_t buffSize);

	// 考虑到一些Window API老是在那里转换编码，这里对场景的进行封装，让上层使用方便
	RKT_API int copyFile(const char* lpExistingFileName, const char* lpNewFileName, int bFailIfExists);
	RKT_API int moveFile(const char* lpExistingFileName, const char* lpNewFileName);
	RKT_API int deleteFile(const char* lpFileName);


#ifdef SUPPORT_SAFE_STRING

	// lstrcpynA 需要在Base.h包含前，先包含Windows.h，否则这里编译出错
	RKT_API char* rkt_lstrcpynA(FILE_LINE_DESC char* dest, const char* src, size_t destSize);
	#define lstrcpynA(...)	rkt::rkt_lstrcpynA(FILE_LINE_MACRO __VA_ARGS__)
#ifndef RKT_UNICODE
	#ifdef lstrcpyn
	#undef lstrcpyn
	#endif
	#define lstrcpyn(...)	rkt::rkt_lstrcpynA(FILE_LINE_MACRO __VA_ARGS__)
#endif

	// sprintf_s
	RKT_API int rkt_vsprintf_s(FILE_LINE_DESC char* string, size_t sizeInBytes, const char* format, va_list ap);
	RKT_API int rkt_sprintf_s(FILE_LINE_DESC char* string, size_t sizeInBytes, const char* format, ...);
	template <size_t _Size> inline int rkt_sprintf_s(FILE_LINE_DESC char (&_Dest)[_Size], const char * _Format, ...){
		va_list _ArgList; _crt_va_start(_ArgList, _Format);
		return rkt_vsprintf_s(FILE_LINE_VARIANT _Dest, _Size, _Format, _ArgList);
	}
	#define sprintf_s(...)		rkt::rkt_sprintf_s(FILE_LINE_MACRO __VA_ARGS__)

#ifdef DEPRECATE_UNSAFE_STRING_FUNC
	#define sprintf		sprintf_s
	/* 如果一定要使用老的 sprintf ，应该写下面类似的宏处理过程
	// for .h file
	#ifdef SUPPORT_SAFE_STRING
	#pragma push_macro("sprintf") 
	#undef sprintf
	#endif
		ret = sprintf(buffer, "%sabcd=%d", "1234567890", 12345);
	#ifdef SUPPORT_SAFE_STRING
	#pragma pop_macro("sprintf") 
	#endif

	// for .cpp file
	#ifdef SUPPORT_SAFE_STRING
	#undef sprintf
	#endif
	*/
#else
	// sprintf
	RKT_API int rkt_sprintf(FILE_LINE_DESC char* string, const char* format, ...);
	template <size_t _Size> inline int rkt_sprintf(FILE_LINE_DESC char (&_Dest)[_Size], const char * _Format, ...){
		va_list _ArgList; _crt_va_start(_ArgList, _Format);
		return rkt_vsprintf_s(FILE_LINE_VARIANT _Dest, _Size, _Format, _ArgList);
	}
	#define sprintf(...)		rkt::rkt_sprintf(FILE_LINE_MACRO __VA_ARGS__)
#endif

#endif // USE_SAFE_STRING

	RKT_API ulong getLastError();
	RKT_API const std::string& getSysErrorMessage(ulong errId);

#define SUPPORT_PERF_OPTION

#ifdef SUPPORT_PERF_OPTION
	enum PerfOption
	{
		PerfOption_MpwRequestRes	= 0x1,
		PerfOption_MzRequestRes		= 0x2,

		PerfOption_MpwUpdate		= 0x4, // unused
		PerfOption_MzUpdate			= 0x8,

		PerfOption_MpwRender		= 0x10,
		PerfOption_MzRender			= 0x20,

		PerfOption_CreateModel		= 0x40,
		PerfOption_PreLoadModel		= 0x80,
		PerfOption_LoadModel		= 0x100,
		PerfOption_LoadFB			= 0x200,
		PerfOption_LoadMPW			= 0x400,
		PerfOption_LoadTexture		= 0x800,
		PerfOption_LoadGround		= 0x1000,
		PerfOption_LoadMiniGround	= 0x2000,
	};

	RKT_API ulong getPerfOption();
	RKT_API void setPerfOption(ulong opt);
#endif

	
} // namespace rkt


#endif // #ifdef SUPPORT_API

#endif // RKT_API_H