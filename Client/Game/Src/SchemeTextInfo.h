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

class CSchemeTextInfo : public ISchemeTextInfo, public ISchemeUpdateSink 
{
public:
	typedef map<int, string >		VCT_SchemeInfo;

	static string ms_NullString;

public:
	///////////////////////////////ISchemeUpdateSink///////////////////	
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////ISchemeTextInfoMode//////////////////////////
	virtual string&	getText(int nID);

	bool LoadScheme(void);
	void Close(void);

 

private:
	VCT_SchemeInfo	m_vctSchemeInfo;
};