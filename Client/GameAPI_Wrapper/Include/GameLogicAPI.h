// RenderProxy.h

#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;

typedef int ENTITY_ID;

#include "EntityViewDef.h"
//#include "GameAPI.h"
//#include "vcclr.h"

namespace  GameLogic{

	///**
	//	封装游戏API,由Unity3d中得c#调用c++
	//*/
	//public ref class GameLogicAPI
	//{
	//public:
	//	/**
	//	@name   客户端加载启动
	//	*/
	//	static public bool GameStartService()
	//	{
	//		return ::GameStartService();
	//	}

	//	/** 设置游戏在登录状态
	//	*/
	//	static public void GameLogin()
	//	{
	//		::GameLogin();
	//	}

	//	/** 接受实体显示层发出的事件
	//	@param entityID   : 实体ID
	//	@param eventID    : 事件ID,具体定义见(EntityViewDef.h)
	//	@param nParam  : 整形参数
	//	@param strParam: 字符串参数
	//	@param ptrParam: 结构指针参数
	//	@return           : 是否接收成功
	//	*/
	//	static public bool onEntityEvent( ENTITY_ID entityID,int eventID,int nParam,String ^ strParam,IntPtr ptrParam  )
	//	{
	//		onEntityEvent( entityID,eventID,nParam,strParam,ptrParam );
	//	}

	//	/** 接受显示层发出的事件
	//	@param eventID : 事件ID,具体定义见(GameViewDef.h)
	//	@param nParam  : 整形参数
	//	@param strParam: 字符串参数
	//	@param ptrParam: 结构指针参数
	//	@return        : 是否接收成功
	//	*/
	//	static public bool onGameViewEvent( int eventID,int nParam,String ^ strParam,IntPtr ptrParam )
	//	{
	//		return true;
	//		//char* pChars = (char*)Marshal::StringToHGlobalAnsi(strParam).ToPointer(); 
	//		//bool ret = ::onGameViewEvent(eventID,nParam,pChars,ptrParam.ToPointer());
	//		//Marshal::FreeHGlobal((IntPtr)pChars);
	//		//return ret;
	//	}


	//	/** 输出调试信息
	//	*/
	//	static public void traceLog(String ^ msg,int level)
	//	{
	//		return ;
	//		//char* pChars = (char*)Marshal::StringToHGlobalAnsi(msg).ToPointer(); 
	//		//::traceLog(pChars,level);
	//		//Marshal::FreeHGlobal((IntPtr)pChars);
	//	}

	//	/** 获得当前Tick数
	//	*/
	//	static public UInt32 getSyncTickCount()
	//	{
	//		return ::getSyncTickCount();
	//	}
	//};
}
