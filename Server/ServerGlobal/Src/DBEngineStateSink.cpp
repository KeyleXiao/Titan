/*******************************************************************
** 文件名:	e:\Rocket\Server\ZoneServer\Src\DBEngineStateSink.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/3/14  21:59
** 版  本:	1.0
** 描  述:	数据库状态信息回调接口
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "DBEngineStateSink.h"

///////////////////////////////////////////////////////////////////
/** 
@param   
@param   
@return  
*/
CDBEngineStateSink::CDBEngineStateSink(void)
{

}

/** 
@param   
@param   
@return  
*/
CDBEngineStateSink::~CDBEngineStateSink(void)
{

}

/** 如果数据引擎有状态信息回调给上层
@param   nInforID：信息ID，参考上层定义
@param   pszDesc：信息描述
@param   
@return  
@note     
@warning 
@retval buffer 
*/
void CDBEngineStateSink::OnState(int nInfoID, LPCSTR pszDesc)
{
	if(nInfoID == DBENGINE_STATEINFO_OK)
	{
		TraceLn("Database Engine Connection is ok!!! INFOID="<<nInfoID<<",DESC="<<pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_DISCONNECT)
	{
		ErrorLn("Database Engine Disconnection!!! INFOID="<<nInfoID<<",DESC="<<pszDesc);
		AlertBad(MG_AlertSrcType_DataBase, pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_RENEWCONNECT)
	{
		TraceLn("Database Engine Reconnection is ok!!! INFOID="<<nInfoID<<",DESC="<<pszDesc);
		AlertPrompt(MG_AlertSrcType_DataBase, pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_THROW)
	{
		ErrorLn("Database Engine throw request!!! INFOID="<<nInfoID<<",DESC="<<pszDesc);
		AlertBad(MG_AlertSrcType_DataBase, pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_EXCEPTION)
	{
		ErrorLn("Database Engine throw exception!!! INFOID="<<nInfoID<<",DESC="<<pszDesc);
		AlertError(MG_AlertSrcType_DataBase, pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_PERFORMANCE)
	{
		WarningLn("Database Engine performance warnings!!! INFOID="<<nInfoID<<",DESC="<<pszDesc);
		AlertWarning(MG_AlertSrcType_Performance, pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_GAMEWORLDERROR)
	{
		ErrorLn("DataBase To Reflect The Wrong Game World!! DBA check DBS file!! INFOID="<<nInfoID<<",DESC="<<pszDesc);
		AlertBad(MG_AlertSrcType_DataBase, pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_DBCONNECTERROR)
	{
		ErrorLn("DataBase Connect fail for Server Close! INFOID="<<nInfoID<<",DESC="<<pszDesc);
		AlertBad(MG_AlertSrcType_DataBase, pszDesc);
	}
	else
	{
		ErrorLn("DataBase return info!!! INFOID="<<nInfoID<<",DESC="<<pszDesc);
		AlertBad(MG_AlertSrcType_DataBase, pszDesc);
	}
}

