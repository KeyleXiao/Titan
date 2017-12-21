//==========================================================================
/**
* @file	  : Ime.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-9-23   9:38
* purpose : 输入法相关
*/
//==========================================================================

#ifndef __IME_H__
#define __IME_H__

#include "Common.h"

#ifdef SUPPORT_IME

namespace rkt {

	/// 初始化IME
	RKT_API void initIme();

	/// 处理窗口的输入法消息
	RKT_API void handleImeMessage(uint message, uint wParam, uint lParam, bool brecord);

	/// 切换输入法
	RKT_API void toggleIme(bool active);
	
	/// 判断当前是否开启了中文输入法
	RKT_API bool isIme();

	/// 获取输入法的数目
	RKT_API size_t getImeCount();

	/// 获取指定输入法的描述
	RKT_API void getImeDesc(int index, TCHAR* buf, size_t len);

	/// 设置指定的输入法
	RKT_API void setIme(int index);

	/// 设置输入法改变通知
	typedef void (*ProcImeChanged) (int index);
	RKT_API void setImeChangedNotify(ProcImeChanged proc);

} // namespace

#endif // #ifdef SUPPORT_IME

#endif // __IME_H__