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
		TraceLn(a2utf8("数据库引擎连接正常！！！INFOID ＝ ")<<nInfoID<< a2utf8("描述：")<<pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_DISCONNECT)
	{
		ErrorLn(a2utf8("数据库引擎断开连接！！！INFOID ＝ ")<<nInfoID<< a2utf8("描述：")<<pszDesc);
		AlertBad(MG_AlertSrcType_DataBase, pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_RENEWCONNECT)
	{
		TraceLn(a2utf8("数据库引擎重连通过！！！INFOID ＝ ")<<nInfoID<< a2utf8("描述：")<<pszDesc);
		AlertPrompt(MG_AlertSrcType_DataBase, pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_THROW)
	{
		ErrorLn(a2utf8("数据库引擎抛掉请求！！！INFOID ＝ ")<<nInfoID<< a2utf8("描述：")<<pszDesc);
		AlertBad(MG_AlertSrcType_DataBase, pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_EXCEPTION)
	{
		ErrorLn(a2utf8("数据库引擎抛出异常！！！INFOID ＝ ")<<nInfoID<< a2utf8("描述：")<<pszDesc);
		AlertError(MG_AlertSrcType_DataBase, pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_PERFORMANCE)
	{
		WarningLn(a2utf8("数据库引擎性能警告！！！INFOID ＝ ")<<nInfoID<< a2utf8("描述：")<<pszDesc);
		AlertWarning(MG_AlertSrcType_Performance, pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_GAMEWORLDERROR)
	{
		ErrorLn(a2utf8("数据库验证调错数据库！倒！INFOID ＝ ")<<nInfoID<< a2utf8("描述：")<<pszDesc);
		AlertBad(MG_AlertSrcType_DataBase, pszDesc);
	}
	else if(nInfoID == DBENGINE_STATEINFO_DBCONNECTERROR)
	{
		ErrorLn(a2utf8("数据库连接失败可能会宕机！请速联系王钢刷脚本 INFOID ＝ ")<<nInfoID<< a2utf8("描述：")<<pszDesc);
		AlertBad(MG_AlertSrcType_DataBase, pszDesc);
	}
	else
	{
		ErrorLn(a2utf8("数据库引擎返回信息！！！INFOID ＝ ")<<nInfoID<< a2utf8("描述：")<<pszDesc);
		AlertBad(MG_AlertSrcType_DataBase, pszDesc);
	}
}

