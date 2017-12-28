#include "stdafx.h"
#include "GameList.h"
#include "GameMsgRegister.h"


GameList& gGameList = GameList::getInstance();

GameList::GameList()
{
	RegisterHandlers();
}

GameList::~GameList()
{
}

//注册这里要处理的所有消息，到m_Processer中
void GameList::RegisterHandlers()
{
	GameMsgRegister::RegisterMsgs(m_Processer);
}

GameID GameList::GetMaxID()
{
	return MAX_GAME_SERVER_COUNT;
}
