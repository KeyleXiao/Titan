/*******************************************************************
** 文件名:	E:\speed\Client\Game\Src\SchemeTextInfo.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/30/2015	17:09
** 版  本:	1.0
** 描  述:	
            文字信息配置
********************************************************************/

#pragma once

#include <map>
#include <vector>
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "MatchSchemeDef.h"

class CSchemeWarBroadCastInfo : public ISchemeWarBroadCastInfo, public ISchemeUpdateSink 
{
public:
	typedef map<int, SWarBroadCastInfoScheme >		VCT_SchemeInfo;

public:
	///////////////////////////////ISchemeUpdateSink///////////////////	
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);
	
	///////////////////////////////ISchemeWarBroadCastInfo//////////////////
	// 通过序列号，获得广播信息
	virtual SWarBroadCastInfoScheme* getWarBroadCastInfoByIndex(int nIndex);

	bool LoadScheme(void);
	void Close(void);

 

private:
	VCT_SchemeInfo	m_vctSchemeInfo;
};