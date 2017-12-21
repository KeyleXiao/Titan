#include "stdafx.h"
#include "HandleCommandClose.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"
#include "SpellMessageDef.h"
#include "EntityDef.h"
#include "IMovePart.h"

/////////////////////////////////////////IHandleCommand/////////////////////////////////////////
void HandleCommandClose::release()
{
	delete this;
}

bool HandleCommandClose::run()
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

	Vector3 vEndTile = m_Data.ptTargetTile;
	if (equalto0(vEndTile.getLength()))
	{
		return true;
	}
	
	// 取得英雄位置
	Vector3 vLoc = pClientEntity->getLocation();
	float fDis = (vEndTile - vLoc).getLength();
	if (fDis <= m_Data.fDistance)
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

void HandleCommandClose::update()
{

}

DWORD HandleCommandClose::getTimeOut()
{
	return m_dwTimeOut;
}

/////////////////////////////////////////HandleCommandClose/////////////////////////////////////////
// 构造函数
HandleCommandClose::HandleCommandClose(const SHandleCommand_Close &data):m_Data(data),m_dwLastMovePosTick(0)
{
	m_dwTimeOut = getTickCountEx() + COMMAND_TIME_OUT;
}

// 析构函数
HandleCommandClose::~HandleCommandClose()
{

}