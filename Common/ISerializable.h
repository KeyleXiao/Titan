/*******************************************************************
** 文件名:	ISerializable.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			序列化接口
********************************************************************/


#pragma once

#include "buffer.h"

/**
	服务器宕机不回档机制设计:
	关于重要数据:(例如经验金钱等)
	1.游戏服务器以n秒一次的频率取全部玩家属性中,标注为TO_BACKUP的重要数据,打包后做成一个snapshot发给备份服务器
	2.备份服务器简单的保存这些数据的snapshot,一但出现宕服,则发通知给备份服务器要求恢复
	3.为减少数据通讯量和服务器计算时间,可仅对发生了改变的玩家进行存储
	
	关于重要物品:
	1.重要物品的交易和使用立即存档
	
	存档时机:
	1.服务器上可配置常规定时存档时间,例如1小时
	2.跨服务器可立即存档一次,但这样设计,遇到大规模活动,突然大量玩家瞬间切换到一个场景时数据库压力将非常巨大,所以跨服务器最好不要存档
*/

enum SERIALIZE_FLAG
{
	TO_INVALID		= 0,		// 无效，不用序列化
	TO_DATABASE     = 1<<1,		// 序列化到数据库
	TO_BACKUP       = 1<<2,		// 共享内存实时备份
	TO_OTHER_SEVER  = 1<<3,		// 跨服传送去其他服务器
	TO_CLIENT_SELF  = 1<<4,		// 序列化到自己的客户端 
	TO_CLIENT_OTHER = 1<<5,		// 序列化到其他人的客户端

    ACTOR_TO_HERO   = 1<<6,     // 静态角色-->战场角色 (静态角色并没有战场角色的所有数据 只作更新用)
    HERO_TO_ACTOR   = 1<<7,     // 战场角色-->静态角色 (临时角色只有静态角色的少部分数据 只作更新用)

	TO_PRIVATE      = TO_DATABASE | TO_OTHER_SEVER | TO_CLIENT_SELF,
	TO_PUBLIC       = TO_DATABASE | TO_OTHER_SEVER | TO_CLIENT_SELF | TO_CLIENT_OTHER,
    TO_ALL_CLIENT   = TO_CLIENT_SELF | TO_CLIENT_OTHER,
};

// 序列化支持接口
struct ISerializable
{
	virtual bool onSerialize( rkt::ibuffer & in,int flag ) = 0;

	virtual bool deSerialize( rkt::obuf & out,int flag ) = 0;
};
