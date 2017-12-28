/*******************************************************************
** 文件名:	STime.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/12/27
** 版  本:	1.0
** 描  述:	标识时间的结构体（精确到毫秒）
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma  once


// 标识时间的结构体（精确到毫秒）
struct STime
{
	time_t	tSec;	// 从1970年1月1日00:00:00到现在总共的秒数
	WORD	tMS;	// 毫秒数(0-999)

	// 默认构造
	STime() : tSec(0), tMS(0) {}

	STime(time_t tStartTime, DWORD tick)
	{
		tSec = tStartTime + tick / CLOCKS_PER_SEC;
		tMS = tick % CLOCKS_PER_SEC;
	}

	// 初始化为当前时间
	inline void Init()
	{
		tSec = time(nullptr);

		SYSTEMTIME SysTime;
		GetLocalTime(&SysTime);
		tMS = SysTime.wMilliseconds;
	}

	// 是否为有效值
	inline bool IsValid() { return tSec > 0; }


	// 重载+ ，m的单位必须是毫秒
	inline STime operator+(const DWORD & m) {
		DWORD tmpMS = m + tMS;
		return STime(tSec + tmpMS / CLOCKS_PER_SEC, tmpMS % CLOCKS_PER_SEC);
	}

	// 重载比较操作符 <
	bool operator<(const STime &rhs) const {
		if (this->tSec < rhs.tSec)
			return true;
		else if (this->tSec > rhs.tSec)
			return false;

		return this->tMS < rhs.tMS;
	}
};
