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

//ע������Ҫ�����������Ϣ����m_Processer��
void GameList::RegisterHandlers()
{
	GameMsgRegister::RegisterMsgs(m_Processer);
}

GameID GameList::GetMaxID()
{
	return MAX_GAME_SERVER_COUNT;
}
