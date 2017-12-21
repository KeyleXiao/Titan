/*******************************************************************
** 文件名:	EffectClient_Roll2Target.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	6/30/2017
** 版  本:	1.0
** 描  述:	向技能目标翻滚
** 应  用:  玩家翻滚是客户端翻滚然后同步位置到服务器的 动作决定位移.
			另外mode2下状态是只在客户端控制的 朝向服务器暂时不能精确控制. AI翻滚的时候服务器因为不知道朝向不能确定选择前后左右翻滚技能
			所以AI的翻滚技能需要另外配一套 服务器大体算出翻滚目标地点 至于播放前滚还是后滚动作 由显示层自己根据当前朝向和目标地点自己决定
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IClientEntity.h"
#include "IClientGlobal.h"
#include "IFreezeClient.h "
using namespace EFFECT_CLIENT;

class CEffectRoll2Target : public IEffect
{
public:
	typedef EffectClient_Roll2Target SCHEME_DATA;

	CEffectRoll2Target(SCHEME_DATA & data) :m_data(data)
	{
	}

	// 效果启用
	virtual bool			Start(EFFECT_CONTEXT * pContext)
	{
		if (pContext == 0 || pContext->pEntity == 0)
			return false;

		cmd_roll_2_target cmd;
		cmd.loc[0] = pContext->ptTargetTile.x;
		cmd.loc[1] = pContext->ptTargetTile.y;
		cmd.loc[2] = pContext->ptTargetTile.z;

		// 设置技能槽
		pContext->pEntity->sendCommandToEntityView(ENTITY_TOVIEW_AI_ROLL, 0, 0, &cmd, sizeof(cmd));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectRoll2Target(m_data); }

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	// 取得效果ID
	virtual int				GetEffectID() { return m_data.nID; }
private:
	SCHEME_DATA             m_data;
};