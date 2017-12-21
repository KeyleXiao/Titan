/*******************************************************************
** 文件名:	AOI_Forest.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	3/24/2015
** 版  本:	1.0
** 描  述:	DOTA类AOI之丛林类
********************************************************************/

#include "stdafx.h"
#include "AOI_Forest.h"
#include "FileSystem/VirtualFileSystem.h"
#include "ISchemeCenter.h"
#include "SchemeDef.h"
#include <windows.h>

bool AOI_Forest::Load( int mapID )
{
    m_nMapID = mapID;

    return true;
}

int AOI_Forest::GetForestID( float fLocX, float fLocY )
{
	if (NULL == m_pMapArea)
	{
		// 直接以区域ID作为丛林ID
		ISchemeAllMapsArea * pAllMapsArea = gServerGlobal->getSchemeCenter()->getSchemeMapArea();
		if (pAllMapsArea == NULL)
			return 0;

		m_pMapArea = pAllMapsArea->getMapArea(m_nMapID, eAreaTypeGrass);
	}

    return ( m_pMapArea==NULL )? -1 : m_pMapArea->getAreaID( Vector3(fLocX, 0, fLocY) );
}
