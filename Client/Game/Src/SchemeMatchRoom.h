/*******************************************************************
** 文件名:	E:\speed\Client\Game\Src\SchemeMatchRoom.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/30/2015	16:25
** 版  本:	1.0
** 描  述:	
            比赛大厅房间配置
********************************************************************/

#pragma once


#include <map>
#include <vector>
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "MatchSchemeDef.h"

class CSchemeMatchRoom : public ISchemeMatchRoom, public ISchemeUpdateSink 
{
public:
	typedef vector<SSchemeMatchRoom* >	VCT_SchemeInfo;
	typedef map<uchar, VCT_SchemeInfo>	MAP_Mode2Datas;
	typedef map<DWORD, VCT_SchemeInfo>	MAP_SchemeData;
	typedef	vector<uchar>				VCT_ModeID;

	static VCT_SchemeInfo	ms_NullSchemeInfo;

public:
	///////////////////////////////ISchemeUpdateSink///////////////////	
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////ISchemeMatchRoom//////////////////////////
	virtual vector<SSchemeMatchRoom* >&	getByMode(uchar ModeID);
	virtual vector<SSchemeMatchRoom* >&	getByMap(DWORD MapID);
	virtual vector<uchar>&			getAllMode(){return m_vctModeID;};
	virtual SSchemeMatchRoom*			getSchemeMatchRoom(uchar ModeID, DWORD MapID, int nSchemeConfigWarID);
	virtual SSchemeSelectControl *		getSelectControlScheme(int);
    virtual SSchemeMatchRoom*			getSchemeMatchRoomByMatchTypeID(DWORD dwMatchTypeID);
    virtual SSchemeComputerSumSkill*	getSchemeComputerSumSkill(int nHeroID);

	bool LoadScheme(void);
	void Close(void);

 

private:
	VCT_SchemeInfo	m_vctSchemeInfo;
	MAP_Mode2Datas	m_mapMode2Datas;
	MAP_SchemeData	m_mapSchemeData;
	VCT_ModeID		m_vctModeID;

private:
	bool	getModes(ICSVReader* pCSVReader, int nRow, int nIndex, bool bStates[MatchRoomStates]);
};