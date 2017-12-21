/*******************************************************************
** 文件名:	DBRequester.h 
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
#pragma once

#ifndef __DBRequester_H__
#define __DBRequester_H__


#include "IDBEngine.h"
#include "Singleton.h"
#include "IVoiceDataBaseDef.h"
using namespace rkt;

/**
@name : 数据库请求管理器
@brief: 所有的数据库请求在这里实现接口,以方便各模块调用
*/

class CDBRequester : public SingletonEx<CDBRequester>
{
public:

	////////////////////////// 所有的请求接口 ///////////////////////////////////////////////
	// 注意:所有的返回对像在删除时要调用关闭数据库回调 GameDatabaseAccessor::getInstance().RemoveDBRetSink(this);
	// 否则会出现还没返回就删除了,就会崩溃

	// 语音服务器开启了,要清理一下上线状态等初始化操作
	void DB_VoiceUser_Start(DB_Param_VoiceUser_Start &param ,IDBRetSink * pDBRetSink=0 );
	// 语音角色用户登录
	void DB_VoiceUser_Login(DB_Param_VoiceUser_Login &param ,IDBRetSink * pDBRetSink=0 );
	// 语音角色用户登出
	void DB_VoiceUser_Logout(DB_Param_VoiceUser_Logout &param ,IDBRetSink * pDBRetSink=0 );
	// 语音角色用户进入频道
	void DB_VoiceUser_Enter(DB_Param_VoiceUser_Enter &param ,IDBRetSink * pDBRetSink=0 );
	// 语音角色用户更新数据
	void DB_VoiceUser_Update(DB_Param_VoiceUser_Update &param ,IDBRetSink * pDBRetSink=0 );
	// 读取语音频道信息
	void DB_VoiceChannel_Read(DB_Param_VoiceChannel_Read &param ,IDBRetSink * pDBRetSink=0 );
	// 保存语音频道信息
	void DB_VoiceChannel_Save(DB_Param_VoiceChannel_Save &param ,IDBRetSink * pDBRetSink=0 );
	// 读取语音频道房间信息
	void DB_VoiceRoom_Read(DB_Param_VoiceRoom_Read &param ,IDBRetSink * pDBRetSink=0 );
	// 保存语音频道房间信息
	void DB_VoiceRoom_Save(DB_Param_VoiceRoom_Save &param ,IDBRetSink * pDBRetSink=0 );
	// 读取马甲权限用户信息
	void DB_VoiceVestInfo_Read(DB_Param_VoiceVestInfo_Read &param ,IDBRetSink * pDBRetSink=0 );
	// 新增马甲权限用户信息
	void DB_VoiceVestInfo_Add(DB_Param_VoiceVestInfo_Add &param ,IDBRetSink * pDBRetSink=0 );
	// 删除马甲权限用户信息
	void DB_VoiceVestInfo_Del(DB_Param_VoiceVestInfo_Del &param ,IDBRetSink * pDBRetSink=0 );
	// 读取粉丝关系信息
	void DB_VoiceFansUser_Read(DB_Param_VoiceFansUser_Read &param ,IDBRetSink * pDBRetSink=0 );
	// 新增粉丝关系
	void DB_VoiceFansUser_Add(DB_Param_VoiceFansUser_Add &param ,IDBRetSink * pDBRetSink=0 );
	// 删除粉丝关系
	void DB_VoiceFansUser_Del(DB_Param_VoiceFansUser_Del &param ,IDBRetSink * pDBRetSink=0 );
	// 查找粉丝
	void DB_VoiceFansUser_Find(DB_Param_VoiceFansUser_Find &param ,IDBRetSink * pDBRetSink=0 );
	// 读取粉丝列表
	void DB_VoiceFansUser_TopList(DB_Param_VoiceFansUser_TopList &param,IDBRetSink * pDBRetSink=0 );
	// 读取语音数值
	void DB_VoiceNumValue_Rread(DB_Param_VoiceNumValue_Read &param, IDBRetSink * pDBRetSink=0 );
	// 增加蓝钻数
	void DB_VoiceNumValue_AddBlueDiamond(DB_Param_VoiceNumValue_AddBlueDiamond &param, IDBRetSink * pDBRetSink=0 );

};


#endif//__DBRequester_H__