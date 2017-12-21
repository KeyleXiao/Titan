/*******************************************************************
** 文件名: SoliderVigour.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/3/24
** 版  本: 1.0
** 描  述: 士兵气势
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#include "StdAfx.h"
#include "SoliderVigour.h"
#include "IWar.h"

CSoliderVigour::CSoliderVigour(IWar* pWar)
	:m_pWar(pWar)
{

}

CSoliderVigour::~CSoliderVigour()
{
	m_pWar = NULL;
}


bool CSoliderVigour::Init(int nMatchType)
{
	if (m_pWar == NULL)
		return false;

	return true;
}

void CSoliderVigour::Release()
{
	delete this;
}

void CSoliderVigour::onWarRealStart()
{
}

void CSoliderVigour::onWarEnd()
{
}


bool CSoliderVigour::addCampPathTypeVigour(int nCamp, EWarSoliderPathType eType, int nVigourValue)
{
	if (m_pWar == NULL)
	{
		return false;
	}
	
	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return false;
	}

	SWarCamp* pWarCamp = m_pWar->getWarCamp(nCamp);
	if(pWarCamp == NULL)
	{
		return false;
	}

	if(eType <= EWSPT_INVALID || eType > EWSPT_MAX)
	{
		return false;
	}

	if(nVigourValue <= 0 )
	{
		return false;
	}

	if(m_mapSoliderVigour.find(nCamp) == m_mapSoliderVigour.end())
	{
		map<EWarSoliderPathType,SPathTypeVigour> mapPathSoliderVigour;
		m_mapSoliderVigour[nCamp] = mapPathSoliderVigour;
	} 


	// 得到敌对阵营
	int nOppositeCamp = warMiscMgr->getEnemyCamp(nCamp);

	if(m_mapSoliderVigour.find(nOppositeCamp) == m_mapSoliderVigour.end())
	{
		map<EWarSoliderPathType,SPathTypeVigour> mapPathSoliderVigour;
		m_mapSoliderVigour[nOppositeCamp] = mapPathSoliderVigour;
	} 

	map<EWarSoliderPathType,SPathTypeVigour>* pMapSoliderVigour = &m_mapSoliderVigour[nCamp];
	map<EWarSoliderPathType,SPathTypeVigour>* pMapOppositeSoliderVigour = &m_mapSoliderVigour[nOppositeCamp];

	vector<EWarSoliderPathType> vecSoliderPathType;
	if((eType&EWSPT_TOP) == EWSPT_TOP)
	{
		vecSoliderPathType.push_back(EWSPT_TOP);
	}

	if((eType&EWSPT_MID) == EWSPT_MID)
	{
		vecSoliderPathType.push_back(EWSPT_MID);
	}

	if((eType&EWSPT_BOT) == EWSPT_BOT)
	{
		vecSoliderPathType.push_back(EWSPT_BOT);
	}

	vector<EWarSoliderPathType>::iterator iterType  = vecSoliderPathType.begin();
	for(;iterType!=vecSoliderPathType.end();++iterType)
	{
		EWarSoliderPathType eTypeResult  = *iterType;
		// 本阵营更新相对和绝对气势值
		{
			if(pMapSoliderVigour->find(eTypeResult) == pMapSoliderVigour->end())
			{
				SPathTypeVigour pathTypeVigour;
				pMapSoliderVigour->insert(pair<EWarSoliderPathType,SPathTypeVigour> (eTypeResult ,pathTypeVigour));
			}

			int nAbsoluteVigour = 0;						
			nAbsoluteVigour = pMapSoliderVigour->at(eTypeResult).nAbsoluteVigour + nVigourValue;
			pMapSoliderVigour->at(eTypeResult).nAbsoluteVigour =  nAbsoluteVigour;

			int nRelativeVigour = 0;
			int nOppositenAbsoluteVigour = getCampPathTypeAbsoluteVigour(nOppositeCamp,eTypeResult);
			nRelativeVigour = nAbsoluteVigour - nOppositenAbsoluteVigour ;
			nRelativeVigour = max(nRelativeVigour,0);
			pMapSoliderVigour->at(eTypeResult).nRelativeVigour =  nRelativeVigour;	
		}

		// 敌对阵营同时也需要更新一次相对气势值 
		{
			if(pMapOppositeSoliderVigour->find(eTypeResult) == pMapOppositeSoliderVigour->end())
			{
				SPathTypeVigour pathTypeVigour;
				pMapOppositeSoliderVigour->insert(pair<EWarSoliderPathType,SPathTypeVigour> (eTypeResult ,pathTypeVigour));
			}

			int nAbsoluteVigour = getCampPathTypeAbsoluteVigour(nCamp,eTypeResult);
			int nOppositeRelativeVigour = pMapOppositeSoliderVigour->at(eTypeResult).nAbsoluteVigour - nAbsoluteVigour ;
			nOppositeRelativeVigour = max(nOppositeRelativeVigour,0);
			pMapOppositeSoliderVigour->at(eTypeResult).nRelativeVigour =  nOppositeRelativeVigour;	
		}			
	}
	return true;
}

int CSoliderVigour::getCampPathTypeAbsoluteVigour(int nCamp, EWarSoliderPathType eType) 
{
	if(m_mapSoliderVigour.find(nCamp) == m_mapSoliderVigour.end())
	{
		return 0;
	} 

	map<EWarSoliderPathType,SPathTypeVigour>* pMapSoliderVigour = &m_mapSoliderVigour[nCamp];
	map<EWarSoliderPathType,SPathTypeVigour>::iterator iterResult = pMapSoliderVigour->find(eType);
	if(iterResult == pMapSoliderVigour->end())
	{
		return 0;
	}

	return iterResult->second.nAbsoluteVigour;
}

int CSoliderVigour::getCampPathTypeRelativeVigour(int nCamp, EWarSoliderPathType eType)
{
	if(m_mapSoliderVigour.find(nCamp) == m_mapSoliderVigour.end())
	{
		return 0;
	} 

	map<EWarSoliderPathType,SPathTypeVigour>* pMapSoliderVigour = &m_mapSoliderVigour[nCamp];
	map<EWarSoliderPathType,SPathTypeVigour>::iterator iterResult = pMapSoliderVigour->find(eType);
	if(iterResult == pMapSoliderVigour->end())
	{
		return 0;
	}

	return iterResult->second.nRelativeVigour;
}
