/*******************************************************************
** 文件名:	SchemeMatchRoom.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/30/2015	15:46
** 版  本:	1.0
** 描  述:	比赛大厅房间配置
            
********************************************************************/

#pragma once

#include <map>
#include <vector>
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "MatchSchemeDef.h"


// Match配置
class CSchemeMatchRoom : public ISchemeMatchRoom, public ISchemeUpdateSink, public CSchemeCache<CSchemeMatchRoom>
{
public:
	typedef vector<SSchemeMatchRoom* >		T_SchemeInfo;
	typedef map<uchar, T_SchemeInfo>		T_Mode2Datas;
	typedef map<DWORD, T_SchemeInfo>		T_Map2Datas;
	typedef	vector<uchar>					T_ModeIDs;

	static T_SchemeInfo	ms_NullSchemeInfo;

public:
	///////////////////////////////ISchemeUpdateSink///////////////////	
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////ISchemeMatchRoomMode//////////////////////////
	virtual vector<SSchemeMatchRoom* >&	getByMode(uchar ModeID);
	virtual vector<SSchemeMatchRoom* >&	getByMap(DWORD MapID);
	virtual vector<uchar>&				getAllMode(){return m_ModeIDs;};
	virtual SSchemeMatchRoom*			getSchemeMatchRoom(uchar ModeID, DWORD MapID, int nSchemeConfigWarID);
	virtual SSchemeSelectControl *		getSelectControlScheme(int);
    virtual SSchemeMatchRoom*			getSchemeMatchRoomByMatchTypeID(DWORD dwMatchTypeID);
    virtual SSchemeComputerSumSkill*	getSchemeComputerSumSkill(int nHeroID);

	bool LoadScheme(void);
	void Close(void);



private:
	T_SchemeInfo	m_SchemeInfo;

	T_Mode2Datas	m_Mode2Datas;
	T_Map2Datas		m_Map2Datas;
	T_ModeIDs		m_ModeIDs;


private:
	bool	getModes(ICSVReader* pCSVReader, int nRow, int nIndex, bool bStates[MatchRoomStates]);
};