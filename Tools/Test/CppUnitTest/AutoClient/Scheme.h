/*******************************************************************
** 文件名:	Scheme.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	1/26/2015
** 版  本:	1.0
** 描  述:	相关配置结构体头文件

********************************************************************/

#pragma once

#include "CSVReader.h"

struct SchemeAccount
{
	int		a_id;				// 账号ID
	string	a_name;				// 账号名
	string	a_pwd;				// 账号密码
	string	user_name;			// 角色名
	int		user_id;			// 角色ID
	float	user_loc[3];		// 坐标
};
extern CCSVReader<int, SchemeAccount> g_accountReader;

enum
{
	ACTION_MOVE = 1,			// 移动
	ACTION_USESKILL,			// 使用技能
};

struct SchemeAction
{
	int		a_id;				// 动作id
	string	a_name;				// 动作名
	int		a_secs;				// 间隔时间
	int		params[4];			// 参数
};
extern CCSVReader<int, SchemeAction> g_actionReader;
