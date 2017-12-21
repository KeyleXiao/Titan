/*******************************************************************
** 文件名:	DBRequester.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2013-04-03
** 版  本:	1.0
** 描  述:	数据库请求管理器
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "DBRequester.h"
#include "GameDatabaseAccessor.h"



////////////////////////// 所有的请求接口 ///////////////////////////////////////////////

// 语音服务器开启了,要清理一下上线状态等初始化操作
void CDBRequester::DB_VoiceUser_Start(DB_Param_VoiceUser_Start &param,IDBRetSink * pDBRetSink)
{
	DWORD dwIndex = (param.bIsStart?1:0);
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICEUSER_START, dwIndex,	(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT("语音服务器开启初始化 操作失败！ 语音服所属游戏世界ID=") << param.dwSvrWorldID);
	}
}
// 语音角色用户登录
void CDBRequester::DB_VoiceUser_Login(DB_Param_VoiceUser_Login &param,IDBRetSink * pDBRetSink)
{
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICEUSER_LOGIN, param.dwPDBID,(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT("语音角色用户登录 操作失败！ 角色的PDBID=") << param.dwPDBID);
	}
}
// 语音角色用户登出
void CDBRequester::DB_VoiceUser_Logout(DB_Param_VoiceUser_Logout &param,IDBRetSink * pDBRetSink)
{
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICEUSER_LOGOUT, param.dwPDBID,(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT("语音角色用户登出 操作失败！ 角色的PDBID=") << param.dwPDBID);
	}
}
// 语音角色用户进入频道
void CDBRequester::DB_VoiceUser_Enter(DB_Param_VoiceUser_Enter &param,IDBRetSink * pDBRetSink)
{
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICEUSER_ENTER, param.dwPDBID,(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT("语音角色用户进入频道 操作失败！ 角色的PDBID=") << param.dwPDBID);
	}
}
// 语音角色用户更新数据
void CDBRequester::DB_VoiceUser_Update(DB_Param_VoiceUser_Update &param,IDBRetSink * pDBRetSink)
{
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICEUSER_UPDATE, param.dwUDBID,(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT(" 语音角色用户更新数据 操作失败！ 通行证UDBID=") << param.dwUDBID);
	}
}
// 读取语音频道信息
void CDBRequester::DB_VoiceChannel_Read(DB_Param_VoiceChannel_Read &param,IDBRetSink * pDBRetSink)
{
	SVoiceChannelID * pIDInfo = (SVoiceChannelID *)(&param.nChannelID);
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICECHANNEL_READ, pIDInfo->dwID,	(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT("读取语音频道信息 操作失败！ 频道类型=") << pIDInfo->dwType << _GT(", 子房间ID=") << pIDInfo->dwRoomID << _GT(", 世界ID=") << pIDInfo->dwWorldID << _GT(", ID=") << pIDInfo->dwID);
	}
}
// 保存语音频道信息
void CDBRequester::DB_VoiceChannel_Save(DB_Param_VoiceChannel_Save &param,IDBRetSink * pDBRetSink)
{
	SVoiceChannelID * pIDInfo = (SVoiceChannelID *)(&param.nChannelID);
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICECHANNEL_SAVE, pIDInfo->dwID,	(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT("保存语音频道信息 操作失败！ 频道类型=") << pIDInfo->dwType << _GT(", 子房间ID=") << pIDInfo->dwRoomID << _GT(", 世界ID=") << pIDInfo->dwWorldID << ", ID=" << pIDInfo->dwID);
	}
}
// 读取语音频道房间信息
void CDBRequester::DB_VoiceRoom_Read(DB_Param_VoiceRoom_Read &param,IDBRetSink * pDBRetSink)
{
	SVoiceChannelID * pIDInfo = (SVoiceChannelID *)(&param.nChannelID);
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICEROOM_READ, pIDInfo->dwRoomID,(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT(" 读取语音频道房间信息 操作失败！ 频道类型=") << pIDInfo->dwType << _GT(", 子房间ID=") << pIDInfo->dwRoomID << _GT(", 世界ID=") << pIDInfo->dwWorldID << ", ID=" << pIDInfo->dwID);
	}
}
// 保存语音频道房间信息
void CDBRequester::DB_VoiceRoom_Save(DB_Param_VoiceRoom_Save &param,IDBRetSink * pDBRetSink)
{
	SVoiceChannelID * pIDInfo = (SVoiceChannelID *)(&param.nChannelID);
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICEROOM_SAVE, param.dwRoomID,(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT("保存语音频道房间信息 操作失败！ 频道类型=") << pIDInfo->dwType << _GT(", 子房间ID=") << param.dwRoomID << _GT(", 世界ID=") << pIDInfo->dwWorldID << ", ID=" << pIDInfo->dwID);
	}
}
// 读取马甲权限用户信息
void CDBRequester::DB_VoiceVestInfo_Read(DB_Param_VoiceVestInfo_Read &param,IDBRetSink * pDBRetSink)
{
	SVoiceChannelID * pIDInfo = (SVoiceChannelID *)(&param.nChannelID);
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICEVEST_READ, pIDInfo->dwID,(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT("读取马甲权限用户信息 操作失败！ 频道类型=") << pIDInfo->dwType << _GT(", 子房间ID=") << pIDInfo->dwRoomID << _GT(", 世界ID=") << pIDInfo->dwWorldID << ", ID=" << pIDInfo->dwID);
	}
}
// 新增马甲权限用户信息
void CDBRequester::DB_VoiceVestInfo_Add(DB_Param_VoiceVestInfo_Add &param,IDBRetSink * pDBRetSink)
{
	SVoiceChannelID * pIDInfo = (SVoiceChannelID *)(&param.nChannelID);
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICEVEST_ADD, param.dwPDBID,	(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT("新增马甲权限用户信息 操作失败！ 用户角色ID=") << param.dwPDBID << _GT(", 频道类型=") << pIDInfo->dwType << _GT(", 子房间ID=") << pIDInfo->dwRoomID << _GT(", 世界ID=") << pIDInfo->dwWorldID << ", ID=" << pIDInfo->dwID);
	}
}
// 删除马甲权限用户信息
void CDBRequester::DB_VoiceVestInfo_Del(DB_Param_VoiceVestInfo_Del &param,IDBRetSink * pDBRetSink)
{
	SVoiceChannelID * pIDInfo = (SVoiceChannelID *)(&param.nChannelID);
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICEVEST_DEL, param.dwPDBID,(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT("删除马甲权限用户信息 操作失败！ 用户角色ID=") << param.dwPDBID << _GT(",频道类型=") << pIDInfo->dwType << _GT(", 子房间ID=") << pIDInfo->dwRoomID << _GT(", 世界ID=") << pIDInfo->dwWorldID << ", ID=" << pIDInfo->dwID);
	}
}
// 读取粉丝关系信息
void CDBRequester::DB_VoiceFansUser_Read(DB_Param_VoiceFansUser_Read &param,IDBRetSink * pDBRetSink)
{
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICEFANS_READ, param.dwFansUDBID,(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT("读取粉丝关系信息 操作失败！ 粉丝的通行证UDBID=") << param.dwFansUDBID);
	}
}
// 新增粉丝关系
void CDBRequester::DB_VoiceFansUser_Add(DB_Param_VoiceFansUser_Add &param,IDBRetSink * pDBRetSink)
{
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICEFANS_ADD, param.dwFansUDBID,(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT(" 新增粉丝关系 操作失败！ 粉丝的通行证UDBID=") << param.dwFansUDBID);
	}
}
// 删除粉丝关系
void CDBRequester::DB_VoiceFansUser_Del(DB_Param_VoiceFansUser_Del &param,IDBRetSink * pDBRetSink)
{
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICEFANS_DEL, param.dwFansUDBID,(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT("删除粉丝关系 操作失败！ 粉丝的通行证UDBID=") << param.dwFansUDBID);
	}
}
// 查找粉丝
void CDBRequester::DB_VoiceFansUser_Find(DB_Param_VoiceFansUser_Find &param,IDBRetSink * pDBRetSink)
{
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICEFANS_FIND, param.dwUDBID,(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT("查找粉丝 操作失败！ 关注的用户通行证UDBID=") << param.dwUDBID);
	}
}

// 读取粉丝列表
void CDBRequester::DB_VoiceFansUser_TopList(DB_Param_VoiceFansUser_TopList &param,IDBRetSink * pDBRetSink)
{
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICEFANS_TOPLIST, param.dwSvrWorldID,(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT("读取粉丝列表 操作失败！"));
	}
}


// 读取语音数值
void CDBRequester::DB_VoiceNumValue_Rread(DB_Param_VoiceNumValue_Read &param, IDBRetSink * pDBRetSink )
{
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICENUMVALUE_READ, param.dwUDBID,(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT("读取语音数值 操作失败！ 关注的用户通行证UDBID=") << param.dwUDBID);
	}
}

// 增加蓝钻数
void CDBRequester::DB_VoiceNumValue_AddBlueDiamond(DB_Param_VoiceNumValue_AddBlueDiamond &param, IDBRetSink * pDBRetSink )
{
	if(!GameDatabaseAccessor::getInstance().ExecuteSP(VOICEDB_REQUEST_VOICENUMVALUE_ADDBLUECOUNT, param.dwUDBID,(LPCSTR)&param,sizeof(param),pDBRetSink ))
	{
		ErrorLn(_GT("增加蓝钻数 操作失败！ 关注的用户通行证UDBID=") << param.dwUDBID << _GT("，增加蓝钻数=") << param.dwDiamondCount << _GT("，赠送人=") << param.dwPDBIDSend );
	}
}