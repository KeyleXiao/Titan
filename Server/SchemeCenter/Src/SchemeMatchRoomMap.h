/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Src\SchemeMatchRoomMap.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/18/2015	17:01
** 版  本:	1.0
** 描  述:	
            比赛大厅房间地图配置
********************************************************************/

#pragma once

#include <map>
#include <vector>
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "Match_SchemeDef.h"

 
class C_SchemeMatchRoomMap : public ISchemeMatchRoomMap, public ISchemeUpdateSink, public CSchemeCache<C_SchemeMatchRoomMap>
{
public:
	typedef map<unsigned int, SSchemeMatchRoomMap>	T_SchemeInfo;

public:
	///////////////////////////////ISchemeUpdateSink///////////////////	
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////ISchemeMatchRoomMode//////////////////////////
	virtual SSchemeMatchRoomMap * getSchemeInfo(T_MDMapID MapID);

	bool LoadScheme(void);
	void Close(void);

private:
	T_SchemeInfo m_SchemeInfo;
};