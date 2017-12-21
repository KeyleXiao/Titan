/**
* Created by PeakGao 2008.01.02
*/


#ifndef RKT_CONFIG_H
#define RKT_CONFIG_H

/*
	该文件用于对整个引擎进行配置编译，下面是所有可以用于条件编译的宏定义
*/

/////////////////////////////////////////////////////////////////////////////////////

/// 打开此宏，服务器则可在各个关键热点加了性能评测
//#define OPEN_BVTTEST

/// 是否打开性能测试
//#define OPEN_PROFILE

/// 是否打开关键监控(开了OPEN_PROFILE和OPEN_BVTTEST可以不开此宏，此宏是上面宏的子集)
//#define OPEN_PROFILEBYKEY

/// 是否支持性能测试
//#define SUPPORT_PROFILE

// 是否打开延迟计时,保存最后收到包的系统高性能计数,用来计算处理包时延迟用
#define CONN_OPEN_RECVDATATIME

// 手动调整下面的宏来确定需求
/// 是否支持Singleton
#define SUPPORT_SINGLETON

/// 是否支持RPC的相关结构
#define SUPPORT_RPC

/// 是否支持RLE加解压
#define SUPPORT_RLE

/// 是否支持安全List
#define SUPPORT_SAFELIST

/// 是否支持字符串扩展(autostring,StringHash,StringHelper)
//#define SUPPORT_AUTOSTRING
#define SUPPORT_STRINGHASH
#define SUPPORT_STRINGHELPER

/// 是否支持LoadLib
#define SUPPORT_LOADLIB

/// 是否支持Handle
#define SUPPORT_HANDLE

/// 是否支持路径搜索算法
#define SUPPORT_PATHFINDER

/// 是否支持IME的处理
#define SUPPORT_IME

/// 是否支持随机数创建器
#define SUPPORT_RANDGENERATOR

/// 是否支持TRACE(依赖THREAD)
#define SUPPORT_TRACE

/// 是否支持APP对象
#define SUPPORT_APP

/// 是否支持tinyxml
#define SUPPORT_TINYXML

/// 是否支持动态对象(依赖TRACE)
//#define SUPPORT_OBJECT

/// 是否支持线程(依赖TRACE)
#define SUPPORT_THREAD

/// 是否支持API(依赖TRACE,FILESYSTEM,APP)
#define SUPPORT_API

/// 是否支持时间对象和时间轴(依赖API,TRACE)
#define SUPPORT_TIMER


/// 是否支持事件引擎(依赖 TRACE)
#define SUPPORT_EVENTENGINE

/// 是否支持配置引擎(依赖 XML,FILESYSTEM,TRACE)
#define SUPPORT_SCHEMEENGINE

/// 是否支持StackWalk
#define SUPPORT_STACKWALK

/// 是否支持LuaEngine(依赖内存定义，FILESYSTEM,TRACE)
#define SUPPORT_LUAENGINE

/// 是否支持网络(依赖TRACE,THREAD)
#define SUPPORT_NET

/// 是否支持网络名称信息,主要用来调试用,占用64字节存名称信息,默认不开
#define SUPPORT_NET_NAMEINF

/// 是否支持虚拟文件系统(依赖THREAD)
#define SUPPORT_STREAM
#define SUPPORT_ASYNC_STREAM //(依赖网络的异步IO)
#define SUPPORT_OBJECTPOOL

/// 是否支持内存
#define SUPPORT_MEMORY

/// 是否支持命令工厂
#define SUPPORT_COMMANDFACTORY

/// 是否支持base64编码解码
#define SUPPORT_BASE64

/// 是否支持url编码解码
#define SUPPORT_URL

/// 是否支持DES加密解密
#define SUPPORT_DES

/// 是否支持CRC循环校验
#define SUPPORT_CRC

/// 是否支持实时报警(依赖Alert)
#define SUPPORT_ALERT

/// 是否支持统计功能
#define SUPPORT_STAT

/// 是否支持国际化相关逻辑
#ifdef UNICODE
#define SUPPORT_INTL
#endif

/// 是否支持国际化相关配置
#ifdef UNICODE
#define SUPPORT_LOCALECONFIG
#endif
/// 是否支持使用安全的字符串函数替换旧的函数
//#define SUPPORT_SAFE_STRING

/// 是否支持文件行号等调试信息
//#define SUPPORT_FILE_LINE_DEBUG

/// 是否强制取消sprintf等不安全函数
//#define DEPRECATE_UNSAFE_STRING_FUNC

/////////////////////////////////////////////////////////////////////////////////////
// 开关宏
/// 调试宏，定义此行则会加入调试信息（注：不同于编译器的_DEBUG宏）
#define RKT_DEBUG

/// 是否支持UI测试
//#define RKT_UI_TEST

/// 是否打开某些局部测试宏
//#define RKT_TEST

/// 是否使用系统内存
//#define USE_SYSTEM_MEMORY

/// 是否开启统计信息
#define USE_STAT_INFO

/// 是否使用汇编实现的快速互斥体（否则使用临界区）
//#define USE_ASSEMBLY_MUTEX

// 输出PKG相关信息
//#define PKG_DEBUG

/////////////////////////////////////////////////////////////////////////////////////
// 检查(不要修改)

#if !defined(SUPPORT_TINYXML) && defined(SUPPORT_SCHEMEENGINE)
#	undef SUPPORT_SCHEMEENGINE
#endif

//////////////////////////////////////////////////////////////////////////
/*
	ID		Version		Lauguage	Nation
	0000	多字节版	简体		中国大陆
	0101	国际版		简体		马来西亚
	0201	国际版		繁体		港澳
	0202	国际版		繁体		台湾
	0301	国际版		越南语		越南
*/

//#define RKT_LAUGUAGE_ID		1	/// 0-简体多字节版 1-简体 2-繁体 3-东亚其他语系
//#define RKT_NATION_ID		1	/// 0-中国大陆版 1-马来版


#endif // RKT_CONFIG_H