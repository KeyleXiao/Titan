/*******************************************************************
** 文件名:	IProgressBarPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	2/26/2015
** 版  本:	1.0
** 描  述:	

			进度条部件
********************************************************************/


#pragma once

#include "IEntity.h"
#include "IEventEngine.h"


// 读条现场
struct ProgressBarContext
{
	int         nWndStyle;                // 窗口风格,根据这个风格可以显示不同的进度条，以为不同的位置，长度等等
	std::string szWndTxt;                 // 窗口文字,告诉玩家当前正在做什么
	unsigned long  nTime;                 // 读条时间(毫秒)  
	int			nBreakFlag;               // 打断标识
	IEventExecuteSink * pCallBack;        // 回调接口,成功会发送 EVENT_ENTITY_PROGRESS_OK 事件
	bool                bSendClient;      // 是否发送给客户端,默认为true,除非为了减少数据包量，仅仅需要他的打断逻辑
	UID                 uidTarget;       // 如果打断标识中包含距离检测，则这个变量必须填写，与该对象超出一定距离则打断
	float               fDistanceRange;   // 如果打断标识中包含距离检测，则这个变量必须填写
    int                 nSpellID;         // 技能ID，不被同技能打断，因为有些技能配了施法打断，然后使用类型又有打断进度条
};

/**
	进度条部件 : 可用于
	1、技能读条
	2、开启物品读条
	3、上载具读条
	等等一切需要读条的场合
*/

struct __IProgressBarPart : public __IEntityPart
{
	
	/**
	启动进度条
	@param context   : 读条现场
	@return          : 当前正在读条则返回false,可接收事件
						EVENT_ENTITY_PROGRESS_OK,					// 读条成功
						EVENT_ENTITY_PROGRESS_BREAK,	     		// 读条中断

	@特别注意： 启动了进度条的部件，如果回调函数要销毁，请调用CloseProgressBar将当前的进度条停止，以免产生野指针
	*/
	virtual bool StartProgressBar( ProgressBarContext * context ) = 0;

	/**
	关闭进度条
	@param pSink : 如果pSink=0则直接关闭进度条，如果不为0，则只有pSink匹配了才关闭,用来清理野指针
	@param bBreak: 是否发送被打断事件
	*/
	virtual void CloseProgressBar(IEventExecuteSink * pSink = 0,bool bBreak=true) = 0;

	/**
	取得当前的读条现场,为0则表示当前没有读条
	*/
	virtual ProgressBarContext * GetCurrentContext() = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IProgressBarPart * CreateEntityProgressBar();
#	define	CreateEntityProgressBarProc	CreateEntityProgressBar
#else													/// 动态库版本
typedef __IProgressBarPart * (RKT_CDECL * procCreateEntityProgressBar)();
#	define	CreateEntityProgressBarProc	DllApi<procCreateEntityProgressBar>::load(MAKE_DLL_NAME(ENTITY), "CreateEntityProgressBar")
#endif

