/*******************************************************************
** 文件名:	SchemeComputerRankConfig.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/13/2015
** 版  本:	1.0
** 描  述:	撮合匹配设置
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "MatchSchemeDef.h"
// 定义
class CSchemeComputerRankConfig : public ISchemeComputerRankConfigInfo, public ISchemeUpdateSink
{
public:

    /////////////////////////ISchemeComputerRankConfigInfo/////////////////////////
	/** 随机取得撮合配置信息
	@return  
	*/
	virtual SComputerRankConfigSchemeInfo* getComputerRankConfigShemeInfo();


	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeComputerRankConfig//////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeComputerRankConfig(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeComputerRankConfig(void);

private:
    typedef std::map<int, SComputerRankConfigSchemeInfo> TMAP_COMPUTERRANKCONFIGSCHEME;
    
    TMAP_COMPUTERRANKCONFIGSCHEME  m_mapComputerRankConfigManager;
};