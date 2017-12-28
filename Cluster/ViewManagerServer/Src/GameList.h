/*******************************************************************
** 文件名:	GameList.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/12/08
** 版  本:	1.0
** 描  述:	游戏服连接列表
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "UserList.h"

class GameUser;


class GameList : public UserList<GameUser, GameID>, public Singleton<GameList>
{
public:
	GameList();
	virtual ~GameList();

	//// 取所有已准备好对外开放的View数量
	//int		GetReadyCount();
	//// 将所有已握手的Gate信息推送给所有View
	//void	BroadcastGatesInfoToView();

protected:
	virtual void	OnDelUser(ISessionUser<GameID>* pUser) {};

	//注册这里要处理的所有消息，到m_Processer中
	virtual void	RegisterHandlers();

	virtual GameID	GetMaxID();

};
extern GameList& gGameList;