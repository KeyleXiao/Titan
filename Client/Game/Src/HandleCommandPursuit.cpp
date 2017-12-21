#include "stdafx.h"
#include "HandleCommandPursuit.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"
#include "SpellMessageDef.h"
#include "EntityDef.h"
#include "IMovePart.h"

/////////////////////////////////////////IHandleCommand/////////////////////////////////////////
void HandleCommandPursuit::release()
{
	delete this;
}

bool HandleCommandPursuit::run()
{
	/// 取得主角对象
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		return true;
	}
	IMovePart *pMovePart = (IMovePart *)pClientEntity->getEntityPart(PART_MOVE);
	if (pMovePart == NULL)
	{
		return true;
	}

    // 取得玩家自己位置
    Vector3 vSelfLoc = pClientEntity->getLocation();
    // 超出距离,删除命令
    if (!vSelfLoc.checkDistance(m_Data.ptSelfTile, COMMAND_DISTANCE))
    {
        return true;
    }

	if (m_Data.uidTarget == INVALID_UID || m_Data.uidTarget == 0)
	{
		return true;
	}
	IEntityFactory * pEntityWorld = gClientGlobal->getEntityFactory();
	if(pEntityWorld == NULL)
	{
		return true;
	}

	IEntity *pTargetEntity = pEntityWorld->getEntity(m_Data.uidTarget);
	if (pTargetEntity == NULL)
	{
		return true;
	}
	
	// 取得目标坐标
	Vector3 vEndTile = pTargetEntity->getLocation();
	// 取得英雄位置
	Vector3 vLoc = pClientEntity->getLocation();
	float fDis = (vEndTile - vLoc).getLength();
	if (fDis <= m_Data.fDistance || fDis >= m_Data.fGiveUpDistance)
	{
		return true;
	}

	// 不要太频繁发送移动位置命令
	if (getTickCountEx() < m_dwLastMovePosTick + MOVE_POS_TIME)
	{
		return false;
	}
	m_dwLastMovePosTick = getTickCountEx();

	// 移动位置
	cmd_creature_move_pos movePos;
	movePos.end[0] = vEndTile.x;
	movePos.end[1] = vEndTile.y;
	movePos.end[2] = vEndTile.z;
	pMovePart->onMovePos(movePos);

	// 返回false表示继续执行
	return false;
}

void HandleCommandPursuit::update()
{

}

DWORD HandleCommandPursuit::getTimeOut()
{
	return m_dwTimeOut;
}

/////////////////////////////////////////HandleCommandPursuit/////////////////////////////////////////
// 构造函数
HandleCommandPursuit::HandleCommandPursuit(const SHandleCommand_Pursuit &data):m_Data(data),m_dwLastMovePosTick(0)
{
	m_dwTimeOut = getTickCountEx() + COMMAND_TIME_OUT;
}

// 析构函数
HandleCommandPursuit::~HandleCommandPursuit()
{

}