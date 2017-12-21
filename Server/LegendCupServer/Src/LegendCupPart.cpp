#include "stdafx.h"
#include "LegendCupPart.h"
#include "EntityHelper.h"
#include "IPlayerRole.h"
#include "ICenterConnectorService.h"
#include "IWarPart.h"
#include "IWarManager.h"
#include "IWarService.h"

LegendCupPart::LegendCupPart()
	:m_pMaster(NULL)
{

}
LegendCupPart::~LegendCupPart()
{

}

short LegendCupPart::getPartID()
{
	return PART_LEGENDCUP;
}
bool LegendCupPart::onLoad( __IEntity* pEntity, int nLoginMode)
{
	if ( !pEntity )
	{
		return false;
	}

	m_pMaster = pEntity;

	if ( m_pMaster->getType() != TYPE_PLAYER_ROLE)
	{
	}

	// 	__IEntityEvent* pEntityEvent = m_pMaster->getEntityEvent();
	// 	if ( pEntityEvent == NULL )
	// 	{
	// 		return false;
	// 	}

	return true;
}
__IEntity * LegendCupPart::getHost()
{
	return nullptr;
}
bool LegendCupPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
	return true;
}
void LegendCupPart::release()
{
	m_pMaster = 0;
	delete this;
}

bool LegendCupPart::onSerialize( rkt::ibuffer & in,int flag )
{
	return true;
}

bool LegendCupPart::deSerialize( rkt::obuf & out,int flag )
{
	return true;
}

void LegendCupPart::restore()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) __ILegendCupPart * CreateLegendCupPart()
{
	return new LegendCupPart;
}