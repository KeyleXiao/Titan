/*******************************************************************
** 文件名:	Event_ManagedDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015/8/28
** 版  本:	1.0
** 描  述:	

			游戏事件定义
********************************************************************/


#pragma once

#include "ManagedCode.h"
#include "GameDef.h"
#include "Entity_ManagedDef.h"

/**
	实体事件（个体）
*/
/**
	备注：事件分实体事件和全局事件，实体事件通过IEntityEvent.h调用，全局事件通过IEeventEngine调用
	实体事件是不跨线程的，全局事件是跨线程的
	全局事件的源ID和类型，指发出事件的对象标识，可以是一个实体的UID，也可以是一个场景的场景ID
	如果传入特定的源ID或者类型ID，则只关心这类对象的事件，比如只关心怪物的事件，或者只关心人物的事件
*/
ENUM ENTITYEVENT_KEY
{
	EVENT_ENTITY_CREATE	= (EVENT_OFFSET+1),		// 实体创建
	EVENT_ENTITY_DESTROY,						// 实体销毁
    EVENT_ENTITY_VIEW_LOADED,					// 实体显示层加载完成
    EVENT_ENTITY_MOVE,							// 实体移动	
    EVENT_ENTITY_MOVE_BEHAVIOUR_CHANGE,                      // 实体移动同步（主要处理状态改变时广播）
	EVENT_ENTITY_ENTER_VIEWPORT,				// 进入视野
	EVENT_ENTITY_LEAVE_VIEWPORT,				// 离开视野

	EVENT_FREEZE_START,							// 开始冷却,客户端用来转圈的

	EVENT_ENTITY_PROPERTY_CHANGE,				// 实体属性发生变化 
    EVENT_ENTITY_PROPERTY_CHANGE_END = EVENT_ENTITY_PROPERTY_CHANGE + ENTITY_PROPERTY::PROPERTY_MAX,

	EVENT_ENTITY_PROPERTY_BROADCAST,			// 实体属性发生广播
	EVENT_ENTITY_ATTACK, 						// 攻击事件
    EVENT_ENTITY_ATTACK_EX,                     // 攻击别人
	EVENT_ENTITY_UNDER_ATTACK, 					// 被攻击
    EVENT_ENTITY_PREPARE_CACL_DAMAGE,           // 准备计算伤害事件
	EVENT_ENTITY_PREPARE_DAMAGE,                // 准备伤害事件
	EVENT_ENTITY_DAMAGE,                        // 受到伤害,伤害不一定等同被攻击，例如从悬崖上掉下，毒伤害
	EVENT_ENTITY_DAMAGE_RESULT,                 // 最终受到伤害
    EVENT_ENTITY_TARGET_DAMAGE_RESULT,          // 目标最终受到伤害
    EVENT_ENTITY_DAMAGE_VOTE_FAKEDEAD,          // 伤害后投票假死事件

	EVENT_ENTITY_CHANGE_STATE,                  // 切换状态
	EVENT_ENTITY_DIE,          					// 死亡事件
	EVENT_ENTITY_KILLSELF,     					// 自杀事件
	EVENT_ENTITY_RELIVE,                        // 复活事件
	EVENT_ENTITY_BEFORE_RELIVE,					// 预复活事件
	EVENT_ENTITY_TRANSPORT,                     // 传送事件

	EVENT_ENTITY_ADDBUFF,						// 实体增加buff
	EVENT_ENTITY_REMOVEBUFF,					// 实体移除BUFF
	EVENT_ENTITY_CREATE_VIEW,					// 实体创建显示层

	EVENT_ENTITY_PROGRESS_OK,					// 读条成功
	EVENT_ENTITY_PROGRESS_BREAK,	     		// 读条中断
	
	EVENT_TANKPART_ACTION,						// 载具相关操作
	EVENT_TANKPART_MOVE,						// 载具移动

	EVENT_ENTITY_CLIENTOPT_MOVE,				// 客户端操作移动消息
	EVENT_ENTITY_CAST_SPELL,					// 施法技能

	EVENT_ENTITY_PLAY_ANIMATION,				// 实体播放动作

	EVENT_ENTITY_EFFECT_BREAK,          		// 效果打断

	EVENT_ENTITY_UPGRADE_LEVEL,					// 实体等级改变事件

    EVENT_ENTITY_SPELL_CHANGE,                  // 技能改变事件(技能增加、技能移除)

	EVENT_ENTITY_CANCEL_CAST,					// 取消施法

	EVENT_ENTITY_PREPARE_CURE,					// 准备治疗

    EVENT_SLOTINDEX_UPDATE,				        // 槽位索引更新

	EVENT_ENTITY_SKIN_CHANGED,				    // 皮肤改变

    EVENT_FLUSH_PROPERTY_2_ClIENT_IMMEDIATELY,  // 立即刷新属性到客户端

	EVENT_ENTITY_LEARN_SPELL,					// 学习技能事件

	EVENT_ENTITY_FREEZE_CHANGED,				// 冷却改变

	EVENT_ENTITY_PREPARE_CHANGE_PROPERTY,		// 准备改变属性

	EVENT_ENTITY_BEAT_BACK,						// 击退事件

    EVENT_ENTITY_SEND_CAST_SPELL,               // 向服务器发送施法技能事件

    EVENT_ENTITY_CHANGE_WING,                   // 改变翅膀事件

    EVENT_ENTITY_FOREST,					    // 草丛事件

    EVENT_ENTITY_CHANGE_HERO,                   // 英雄改变事件

    EVENT_ENTITY_JOINTEAM,                       // 加入队伍

    EVENT_ENTITY_LEAVETEAM,                       // 离开队伍

    EVENT_ENTITY_SECONDARY_CAST_SPELL,          // 二次技能施法技能

    EVENT_ENTITY_FLY_STATE_CHANGE,              // 实体飞行状态改变事件

    EVENT_UPDATE_SPELL,				            // 升级技能

    EVENT_ENTITY_AFTER_CREATE,                  // 实体创建完成事件

    EVENT_REMOVE_SPELL,                         // 移除技能事件

    EVENT_BUY_ITEM,                             // 购买物品事件

    EVENT_ITEM_IN_SLOT_CHANGE,                  // 物品槽里的物品改变事件

    EVENT_USE_ITEM,                             // 使用物品事件 
    
    EVENT_GOT_STUCK,                            // 被地形卡住事件

    EVENT_ENTITY_ADD_SHIELD,                    // 增加护盾事件

	EVENT_ENTITY_CHANGE_OPERATION,              // 改变模式事件

	EVENT_ACTOR_READTASKDATA,                   // 完成读取任务数据
	
	EVENT_ACTOR_UPDATEPERDAY,					// 角色每日更新事件

    EVENT_ACTOR_PREPARE_LOGOUT,                 // 角色预登出场景服
    EVENT_ACTOR_PREPARE_LOGOUT_SAVE,            // 角色预登出场景服存盘
    EVENT_ACTOR_LOGIN,                          // 角色上线(进入场景服)
    EVENT_ACTOR_LOGOUT,                         // 角色下线(退出场景服)

	EVENT_ENTITY_ACTOR_CHANGE_LEVEL,			// 玩家静态角色等级改变事件

    EVENT_ENTITY_PREPARE_CAST_SPELL,			// 准备施法施法技能

    EVENT_ENTITY_INPUT_MANAGER,                 // 键盘事件
    
    EVENT_ENTITY_CAN_BUY_GOOD,                  // 能否购买物品

    EVENT_ENTITY_CLEAR_MULTI_KILL,              // 清除连杀

    EVENT_ACTOR_ENTER_SCENE,                    // 角色进入场景

    EVENT_ENTITY_ABSENT,                        // 玩家(掉线)退出战场

    EVENT_ENTITY_CONTINUE_PLAY,                 // 玩家续玩

    EVENT_ENTITY_ACTOR_CHANGE_NAME,				// 玩家角色名改变事件

    EVENT_ENTITY_ACTOR_SET_ACTIVITY,            // 玩家修改活动度
    
    EVENT_ENTITY_CLIENT_REMOVE_BUFF,			// 客户端移除buff

    EVENT_ACTOR_CHANGE_MONEY,                   // 角色金钱变化

    EVENT_ACTOR_RELIVE,                         // 玩家复活事件

    EVENT_ENTITY_ACTIVE_TALENT,					// 玩家激活天赋

    EVENT_ENTITY_HIT_WALL,                      // 实体撞墙

	EVENT_ACTOR_CHANGE_GAMESTATE,				// 切换游戏状态

    EVENT_ACTOR_PK_TARGET_CHANGE,               // pk目标改变

    EVENT_SELF_SHIFT_STATE_CHANGE,              // 自己冲锋状态改变

    EVENT_ENTITY_PRE_REPLACE_SPELL,             // 准备替换技能

    EVENT_ENTITY_SIGHT_DISTANCE_CHANGE,         // 视野距离改变

    EVENT_ENTITY_RESPONSE_SPELL_ATTR_QUERY,     // 回复技能属性查询

    EVENT_ENTITY_RIGHTBUTTON_DOWN,              // 右键按下事件

    EVENT_ENTITY_BELONG_MONSTER_NOTIFY,         // 附属怪物通知事件

    EVENT_ACTOR_PRE_EXPORT_DATA,				// 角色跨场景预导出数据事件

    EVENT_ENTITY_SPELL_PREPARE,                 // 技能准备事件

    EVENT_ENTITY_ENTER_A_OB_SIGHT,              // 自己进入某个OB视野

    EVENT_ENTITY_ENTER_A_CAMP_SIGHT,            // 进入某个阵营视野

    EVENT_ENTITY_LEAVE_A_CAMP_SIGHT,            // 离开某个阵营视野

    EVENT_ENTITY_STEALTH_CHANGE,                // 自己隐身状态发生改变

    EVENT_ACTOR_TOPUP_SUCCESS,                  // 玩家充值点券成功

	EVENT_ENTITY_MAX,                           // 最大角色事件ID（不能超过256个）

};

/** 全局事件定义
@return  
*/
/**
	备注：事件分实体事件和全局事件，实体事件通过IEntityEvent.h调用，全局事件通过IEeventEngine调用
	实体事件是不跨线程的，全局事件是跨线程的
	全局事件的源ID和类型，指发出事件的对象标识，可以是一个实体的UID，也可以是一个场景的场景ID
	如果传入特定的源ID或者类型ID，则只关心这类对象的事件，比如只关心怪物的事件，或者只关心人物的事件
*/
ENUM GAMEEVENT_KEY
{
	EVENT_SCENE_CREATED	= (EVENT_OFFSET+512),	// 场景创建成功，场景逻辑服务加载完
	EVENT_SCENE_DESTROY,						// 场景销毁

	EVENT_SCENE_ENTITY_CREATE,					// 场景创建实体
	EVENT_SCENE_ENTITY_REMOVE,					// 场景移除实体

    EVENT_NEW_DAY,                              // 新一天

    EVENT_SCENE_WAR_END,					    // 场景战场结束

	EVENT_SCENE_PRE_ENTER_SCENE_UPDATE,			// 预进入场景玩家数更新

	///////// 服务开关事件 ////////////////////////////////////////////////////////////
    EVENT_SYSTEM_CLIENT_DISCONNECT,				// 客户端断开连接

	EVENT_SYSTEM_SERVERINFOUPDATED,				// 某台服务器信息更新

	EVENT_SYSTEM_GLOBAL_STARTED,				// Global服务已启动
	EVENT_SYSTEM_GLOBAL_STOP,					// Global服务已关闭

	EVENT_SYSTEM_MIGRATE_STARTED,				// 迁移服务已启动
	EVENT_SYSTEM_MIGRATE_STOP,					// 迁移服务已关闭

	EVENT_SYSTEM_MANAGECONNECTOR_STARTED,		// 管理器连接器服务已启动
	EVENT_SYSTEM_MANAGECONNECTOR_STOP,			// 管理器连接器服务已关闭

	EVENT_SYSTEM_CENTERCONNECTOR_STARTED,		// 中心连接器服务已启动
	EVENT_SYSTEM_CENTERCONNECTOR_STOP,			// 中心连接器服务已关闭
	EVENT_SYSTEM_CENTER_HANDSHAKE_OK,			// 与中心服握手完成

	EVENT_SYSTEM_BRIDGECONNECTOR_STARTED,		// 桥连接器服务已启动
	EVENT_SYSTEM_BRIDGECONNECTOR_STOP,			// 桥连接器服务已关闭

	EVENT_SYSTEM_GATEWAYCONNECTOR_STARTED,		// 网关连接器服务已启动
	EVENT_SYSTEM_GATEWAYCONNECTOR_STOP,			// 网关连接器服务已关闭

	EVENT_SYSTEM_GUARDCONNECTOR_STARTED,		// 数据守护连接器服务已启动
	EVENT_SYSTEM_GUARDCONNECTOR_STOP,			// 数据守护连接器服务已关闭

	EVENT_SYSTEM_DBENGINE_STARTED,				// DB引擎服务已启动
	EVENT_SYSTEM_DBENGINE_STOP,					// DB引擎服务已关闭

    EVENT_SYSTEM_STARTFINISH,                   // 服务器启动完成    
    EVENT_STOP_SERVER_SERVICE_SAVE_DATA,        // 通知停机保存数据
    EVENT_STOP_SERVER_SERVICE,                  // 停机投票事件

    EVENT_SYSTEM_DEFAULTSERVER_CHANGED,         // 默认区变化了,新服务器IP,端口可从config.ini中取

    ////////////////////////////////////////////////////////////////////////////
    EVENT_GAME_OBSTACLE_CREATE,		            // 障碍创建
    EVENT_GAME_OBSTACLE_DESTROY,		        // 障碍销毁

    EVENT_GAME_BLOCK_BARRIER_CREATE,           // 阻挡类结界创建
    EVENT_GAME_BLOCK_BARRIER_DESTROY,          // 阻挡类结界销毁

    EVENT_GAME_ENTITY_CHANGE_STATE,             // 切换状态
    EVENT_GAME_ENTITY_DAMAGE,                   // 受到伤害,伤害不一定等同被攻击，例如从悬崖上掉下，毒伤害
    EVENT_GAME_ENTITY_LEARN_SPELL,				// 学习技能事件
    EVENT_GAME_ENTITY_FREEZE_CHANGED,			// 冷却改变
    EVENT_GAME_ENTITY_PREPARE_CURE,				// 准备治疗
    EVENT_GAME_ENTITY_DIE,          			// 死亡事件

    EVENT_GAME_MONSTER_PREPARE_CREATE,		    // 实体预创建，比实体创建早一些


    EVENT_GAME_ACTOR_KILL_ADD_TALENT_POINT,		// 击杀获得天赋点数
    EVENT_GAME_ACTOR_KILL_BELONGS,              // 击杀归属事件
    EVENT_GAME_ACTOR_ASSIST_CHANGE,				// 助攻增加事件
    EVENT_GAME_ACTOR_SYNC_PROPERTY,		        // 执行属性同步
    EVENT_GAME_ACTOR_READTASKDATA,              // 完成读取任务数据
    EVENT_GAME_ACTOR_UPGRADE_LEVEL,				// 实体等级改变事件
    EVENT_GAME_ACTOR_TALENT_CHANGE,             // 天赋点改变事件
    EVENT_GAME_ACTOR_ACTIVE_TALENT,             // 玩家激活天赋事件（只发手工激活）
    EVENT_GAME_ACTOR_WAR_REC_DAMAGE,            // 战场实体需要记录的伤害
    EVENT_GAME_ACTOR_KILL_MONSTER,				// 人物击杀怪物事件
    EVENT_GAME_ACTOR_LOGIN,                     // 角色登进场景服 
    EVENT_GAME_ACTOR_LOGOUT,                    // 角色登出场景服
    EVENT_GAME_ACTOR_ADD_HERO,                  // 添加英雄事件
    EVENT_GAME_ACTOR_CHANGE_GAMESTATE,          // 切换游戏状态
    EVENT_GAME_ACTOR_UPDATE_MATCHRANK,          // 更新比赛Rank
    EVENT_GAME_ACTOR_UPDATEPERDAY,				// 角色每日更新事件
    EVENT_GAME_ACTOR_WAR_GOOD_LIST,             // 玩家战场物品列表
	EVENT_GAME_ACTOR_CONTINUE_PLAY,             // 玩家续玩
    EVENT_GAME_ACTOR_CHANGE_NAME,		        // 玩家角色名改变事件
    EVENT_GAME_ACTOR_SET_ACTIVITY,              // 玩家修改活动度
    EVENT_GAME_ACTOR_MOVE,                      // 玩家移动


    //EVENT_ACTOR_REENTER_SCENE,                // 重进场景事件(挂机角色重新启用)
    EVENT_GAME_CLEAR_MULTI_KILL,                // 清除连杀

    EVENT_GAME_THRILLINGCURE,                   // 惊险救治
    EVENT_GAME_KILLINGCONTROL,                  // 夺命控制
    EVENT_GAME_GROUPCONTROL,                    // 团体控制
    EVENT_GAME_WAR_PERSON_TASK,                 // 战场人物任务数据
	
    EVENT_CLIENT_ENTER_SCENE,                   // 客户端进入场景
	
	EVENT_GAME_NPC_TRIGGER,						// 玩家触发NPC
	EVENT_GAME_NPC_FUNC,						// 玩家点击NPC功能

	EVENT_GAME_ACTOR_DATA_TASK,					// 玩家数据任务事件

	EVENT_GAME_ADD_ASSIST_BUFF,					// 添加助攻BUFF
    ///////////////////////////////////战队/////////////////////////////////////////
    EVENT_KIN_READ_DB_FINISHED,                  // 战队从数据库数据读取完毕

    EVENT_KIN_UPDATEID,                         // 更新战队ID事件

    EVENT_KIN_ADDAPPLY,                          // 申请加入战队

    EVENT_KIN_REMOVEAPPLY,                       // 拒绝申请加入战队

    EVENT_KIN_BASEDATAUPDATE,                    // 战队列表项刷新

    EVENT_KIN_LOOKUPMEMBER,                      // 成员信息返回
    
    EVENT_CONTACT_BASEDATAUPDATE,                // 战队成员联系信息列表项刷新

    EVENT_KIN_ADD_MEMBER,                        // 添加成员

    EVENT_KIN_REMOVEMEMBER,                      // 移除成员

    EVENT_KIN_CREATE_SUCCESS,                    // 创建战队成功事件

    EVENT_KIN_UPDATEHEROMEMBER,                  // 成员信息返回

    EVENT_KIN_QUIT,                              // 退出战队

    EVENT_KIN_ADDMEMBER,                         // 成员加入

    EVENT_KIN_DELMEMBER,                         // 删除成员

    EVENT_KIN_UPDATEMEMBER,                      // 更新成员

    EVENT_KIN_UPDATE_NUMPROP,                    // 战队整型属性改变

    EVENT_KIN_UPDATE_STRINGPROP,                 // 战队字符型属性改变

    EVENT_KIN_CREATE,                            // 战队创建
    
    EVENT_KIN_TOFORMAL,                          // 战队成为正式战队

    EVENT_KIN_DISMISS,                           // 战队解散
    
    EVENT_KIN_UPDATE_BOSSINFO,                   // 刷新Boss信息
    
    EVENT_KIN_LOGIN,	                         // 成员登陆
    
    EVENT_KIN_LOGOUT,                            // 成员下线

    EVENT_KIN_CHANGE_SHAIKH,	                 // 战队族长更换事件
    
    EVENT_KIN_EDITAUTOACCEPT,                    // 编辑自动接收

    EVENT_KIN_AUTOACCEPTREQUEST,                 // 请求自动接收设置
    
    EVENT_KIN_PERSONJOINEDKIN,                   // 请求自动接收设置

    EVENT_KIN_ONSOLDIERPAYUPDATE,                // 更新战队军饷信息

	EVENT_KIN_CHANGE_ELDER,                      // 战队副族长更换事件

    EVENT_KIN_UPDATE_ORDERINFO,                  // 战队排行榜更新

    EVENT_KIN_ANSWERINVITE_SUCCESS,              // 响应邀请成功

	///////////////////////////////////联盟/////////////////////////////////////////
	EVENT_CLAN_CREATE,							// 联盟创建事件
	EVENT_CLAN_DISMISS,							// 联盟解散事件
	EVENT_CLAN_MEMBERLOGIN,						// 联盟成员登入事件
	EVENT_CLAN_SYNCHRO_FINISH,					// 场景服接收联盟社会服数据完成
	EVENT_CLAN_READLIST_FINISH,					// 联盟列表读取完毕
	EVENT_CLAN_SHAIKH_CHANGE,					// 联盟领导变更
	EVENT_CLAN_UPDATEID,						// 联盟战队更新事件
    EVENT_CLAN_UPDATE_ORDERINFO,                // 更新帮会排行榜信息


    /// 收到玩家信号
    EVENT_PLAYER_SIGNAL_RECVD, 
	////////////////////////////////////组队//////////////////////////////////////
    EVENT_TEAM_CREATETEAM,
    EVENT_TEAM_JOINTEAM,
    EVENT_TEAM_LEAVETEAM,
    EVENT_TEAM_CAPTAIN,
    EVENT_TEAM_MEMBERCHANGE,
    EVENT_TEAM_QUITTEAM,
    EVENT_TEAM_DISBAND,
    EVENT_TEAM_POSTPLACE,
    EVENT_TEAM_SIMULATETRACK,
    EVENT_GROUP_CAPTAIN,    

    /////////////////////////////////////排行榜/////////////////////////////////////
    EVENT_ORDER_UPDATE_RANK,
    EVENT_ORDER_REQUEST_ALL_CLAN_DATA,
    

    EVENT_CONTINUE_PLAY_CLIENT_INCOMMING,

    ///////////////////////////////////房间////////////////////////////////////////////////
    EVENT_ROOM_ADD_ACTOR,           // 进入房间
    EVENT_ROOM_DEL_ACTOR,           // 离开房间
    EVENT_ROOM_ACTOR_CHANGE_CAMP,   // 房间内换阵营

    EVENT_GAME_ENTITY_REPLICATION_MONSTER,          // 分身怪事件

	/////////////////////////////////////好友/////////////////////////////////////////////
	EVENT_SNS_ADD_BUDDY,		// 加好友事件

    ////////////////////////////////////联盟战队/////////////////////////////////////////
    EVENT_KIN_SHAIKHID_CHANGED,       // 战队长改变事件
    EVENT_CLAN_SHAIKHID_CHANGED,      // 联盟盟主改变事件

	/////////////////////////////////// 任务 ///////////////////////////////////////////
	EVENT_TASK_UPDATE_RANK_TASK,		// 更新排位任务数据

	/////////////////////////////////// 排位 ///////////////////////////////////////////
	EVENT_MATCH_RANK_SEASON_BEGIN,		// 排位赛季开始
	EVENT_MATCH_RANK_SEASON_END,		// 排位赛季结束

	EVEN_MAX_ID,                                // 最大事件ID
};

//////////////////////////发送源类型 ///////////////////////////
/// 发送源类型
enum
{
	SOURCE_TYPE_UNKNOW = 0,			// 类型ID根
	SOURCE_TYPE_SCENE,				// 场景
	SOURCE_TYPE_PERSON,				// 玩家实体
	SOURCE_TYPE_MONSTER,			// 怪物实体
    SOURCE_TYPE_MIGRATE,    		// 迁移事件
    SOURCE_TYPE_SYSTEM,	    		// 系统事件
    SOURCE_TYPE_KIN,                // 战队事件
    SOURCE_TYPE_CLAN,               // 帮会事件
    SOURCE_TYPE_ODER,               // 排行榜事件
    SOURCE_TYPE_TEAM,               // 组队事件
    SOURCE_TYPE_ROOM,               // 房间事件
    SOURCE_TYPE_SNS,                // 好友事件

	SOURCE_TYPE_MAX,
};
