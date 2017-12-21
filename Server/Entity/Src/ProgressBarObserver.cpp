/*******************************************************************
** 文件名:	ProgressBar.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/11/23
** 版  本:	1.0
** 描  述:	进度条观察者

**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#include "StdAfx.h"
#include "ProgressBarObserver.h"
#include "IServerGlobal.h"
#include "IEntity.h"
#include "CommonPart.h"
#include "IProgressBarPart.h"
#include "ISpell.h"
#include "SpellContextDef.h"


///////////////////////////////////////////////////////////////////
/** 
@param   
@param   
@return  
*/
CProgressBarObserver::CProgressBarObserver(void)
{

}

/** 
@param   
@param   
@return  
*/
CProgressBarObserver::~CProgressBarObserver(void)
{

}

/** 
@param   
@param   
@return  
*/
bool CProgressBarObserver::create(__IEntity* pMaster)
{
	if(pMaster == nullptr)
		return false;

	m_pMaster = pMaster;

	return true;
}

/** 
@param   
@param   
@return  
*/
void CProgressBarObserver::release(void)
{
	m_pMaster = nullptr;

	delete this;
}

void CProgressBarObserver::start(int nSpellID, int nReverse)
{
	__IEntity* pMaster = m_pMaster;
	if (pMaster == NULL)
	{
		return;
	}

	__ISpellPart *pSpellPart = (__ISpellPart *)pMaster->getEntityPart(PART_SPELL);
	if (pSpellPart == NULL)
	{
		return;
	}

	// 没有该技能则学习该技能
	ISpell* pSpell = pSpellPart->findSpell(nSpellID);
	if (pSpell == NULL)
	{
		if (pSpellPart->addSpell(nSpellID) == false)
		{
			ErrorLn(__FUNCTION__":Add Spell failed. nSpellID=" << nSpellID);
			return;
		}
	}

	// 施放该技能
	UID uidSelf = pMaster->getUID();
	Vector3 vLoc = pMaster->getLocation();
	DWORD dwStartTick = getTickCountEx();
	// 技能施法现场
	SPELL_CONTEXT spellContext;
	spellContext.CC.pEntity = pMaster;
	spellContext.CC.uidOperator = uidSelf;
	spellContext.CC.uidTarget = uidSelf;
	spellContext.CC.ptStartTile = vLoc;
	spellContext.CC.ptTargetTile = vLoc;
	spellContext.CC.nID = nSpellID;
	spellContext.CC.dwStartTick = dwStartTick;
	spellContext.CC.dwUpdateFrame = 0;
	spellContext.EC.pEntity = pMaster;
	spellContext.EC.uidOperator = uidSelf;
	spellContext.EC.uidTarget = uidSelf;
	spellContext.EC.ptStartTile = vLoc;
	spellContext.EC.ptTargetTile = vLoc;
	spellContext.EC.nID = nSpellID;
	spellContext.EC.vSelfLoc = vLoc;
	spellContext.EC.dwStartTick = dwStartTick;
	spellContext.EC.nReverse1 = nReverse;

	if (pSpellPart->castSpell(&spellContext) == false)
	{
		WarningLn(__FUNCTION__":cast spell failed.");
		return;
	}
}

void CProgressBarObserver::stop()
{
}

void CProgressBarObserver::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_ENTITY_PROGRESS_OK:
		{
			onProgressOK();
		}
		break;
	case EVENT_ENTITY_PROGRESS_BREAK:
		{
			onProgressBreak();
		}
		break;
	default:
		break;
	}
}