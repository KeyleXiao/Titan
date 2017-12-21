/*******************************************************************
** 文件名:	Stage_Base.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	6/24/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/

/**
	游戏状态 - 用来处理当前处在游戏的不同阶段，而需要处理的不同逻辑
    常用的阶段:
    1.Stage_Login       : 登陆阶段,处理登陆逻辑
    2.Stage_Choose      : 选择角色阶段
    3.Stage_Game        : 进入游戏主城
    4.Stage_MatchRoom	: MOBA撮合成功之后进入选择英雄阶段
    5.Stage_Battle      : MOBA战场中
	(如果是竞技游戏，就还有撮合阶段，对局阶段，结算阶段)
*/

#pragma once

class Stage_Base : public IConnectionEventHandler
{
public :
	///////////////////////////IConnectionEventHandler//////////////////////////////////////
	virtual void OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler );

	virtual void OnConnected( IConnection * conn );

	virtual void OnDisconnected( IConnection * conn,DWORD reason );

	virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen );

	virtual void OnError( IConnection * conn,DWORD dwErrorCode );

	///////////////////////////Stage_Base////////////////////////////////////////////////////
	// 进入状态
	virtual void onEnter( void * context,int len ) = 0;

    // 离开状态
    virtual void onLeave() = 0;

	// 逻辑更新
	virtual void update() = 0;

    // 寻路(void* : cmd_map_findpath*)
    virtual bool findPath(void* pCmd){return false;} 

    /** 默认区变化了,新服务器IP,端口可从config.ini中取
	*/
	virtual bool onGameServerChanged(LPCSTR pData, int nLen) { return true; }
};
