#ifndef __I_StackWalk_H__
#define __I_StackWalk_H__

#ifdef SUPPORT_STACKWALK

#include <string>

enum
{
	///是否输出Log
	dumpLog,

	///基本信息
	dumpBasicInfo,

	///模块列表
	dumpModuleList,

	//窗口列表
	dumpWindowList,

	///进程列表
	dumpProcessList,

	///寄存器
	dumpRegister,

	///调用栈
	dumpCallStack,

	///输出dmp文件
	dumpMiniDump,

	dumpCount
};

struct IStackWalk;

struct IStackWalkCallback
{
	///开始dump信息会调用这个函数-
	virtual void onDumpStart(IStackWalk*) = 0;

	///可以用IStackWalk的dumpLn Dump想输出到LogFile的信息
	virtual void onDump(IStackWalk*,PEXCEPTION_POINTERS) = 0;

	///Dump结束会调用此函数，此函数返回值会被当作异常捕获的返回值
	virtual long onDumpFinish(IStackWalk*,PEXCEPTION_POINTERS) = 0;
};

struct IStackWalk
{
	///挂一个IStackWalkCallback的回调
	virtual bool attachStackWalkCallback(IStackWalkCallback *) = 0;

	///删除回调
	virtual bool detachStackWalkCallback(IStackWalkCallback *) = 0;

	///设置Log文件名
	virtual void setLogFileName(LPCTSTR szLogFileName) = 0;

	///设置Dmp文件名
	virtual void setDumpFileName(LPCTSTR szDumpFileName) = 0;

	///对上面的枚举的开关
	virtual void setDumpOn(int iEnum,bool bOn) = 0;

	///设置Dump类型
	virtual void setDumpType(long lDumpType) = 0;

	///得到异常码
	virtual unsigned long getExceptionCode(PEXCEPTION_POINTERS pExceptionInfo) = 0;

	///得到异常描述
	virtual LPCTSTR getExceptionDesc(PEXCEPTION_POINTERS pExceptionInfo) = 0;

	///得到崩溃的模块文件偏移
#if defined(_M_IA64) || defined(_M_IX64)	/* 64bit Itanium处理器 */
	virtual ULONGLONG getCrashFileOffset(PEXCEPTION_POINTERS pExceptionInfo) = 0;
#elif defined(_M_X64) || defined(_M_AMD64)	/* 64bit AMD and Intel处理器 */
	virtual DWORD64 getCrashFileOffset(PEXCEPTION_POINTERS pExceptionInfo) = 0;
#elif _M_IX86	/* 32bit处理器 */
	virtual DWORD getCrashFileOffset(PEXCEPTION_POINTERS pExceptionInfo) = 0;
#endif	

	///得到崩溃的模块文件名
	virtual LPCTSTR	getCrashModuleName(PEXCEPTION_POINTERS pExceptionInfo) = 0;

	///输出一些Dump信息到Log文件
	virtual void dumpLn(LPCTSTR szMsg) = 0;

	///取得崩溃模块的产品版本号
	virtual void getCrashModuleProductVer(PEXCEPTION_POINTERS pExceptionInfo,unsigned short pwVer[4]) = 0;

	///取得当前调用栈
	virtual std::string getCallStack(CONTEXT& Context,HANDLE hThread) = 0;

	///取得崩溃模块的产品版本号
	virtual void getCrashModuleFileVer(PEXCEPTION_POINTERS pExceptionInfo,unsigned short pwVer[4]) = 0;

	/// 主动执行minidump过程
	virtual void miniDump(PEXCEPTION_POINTERS pExceptionInfo, int nCoolingTime = 0) = 0;
};

RKT_API IStackWalk* createStackWalk();


#define	createStackWalkProc	createStackWalk

#endif // #ifdef SUPPORT_STACKWALK

#endif