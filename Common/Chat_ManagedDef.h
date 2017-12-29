/*******************************************************************
** 文件名:	Chat_ManagedDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	侯森
** 日  期:	3/31/2016
** 版  本:	1.0
** 描  述:	聊天相关托管公共定义
********************************************************************/

#ifndef __COMMON_CHAT_MANAGED_DEF_H__
#define __COMMON_CHAT_MANAGED_DEF_H__

#include "GameDef.h"
#include "ManagedCode.h"

// C++和C#公用数据结构
// 聊天频道定义
ENUM EMChatChannelType
{
	CHAT_CHANNEL_SYSTEM,		// 系统频道
	CHAT_CHANNEL_WORLD,			// 世界频道
	CHAT_CHANNEL_MATCHROOM,		// 房间频道
	CHAT_CHANNEL_TEAMMATE,		// 队伍频道
	CHAT_CHANNEL_WARSCENE,		// 战场频道
	CHAT_CHANNEL_PRIVATE,		// 私聊频道
	CHAT_CHANNEL_KIN,			// 战队频道
	CHAT_CHANNEL_CLAN,			// 帮派频道
	CHAT_CHANNEL_NEARBY,		// 附近频道
	CHAT_CHANNEL_ZONE,			// 区域频道
	CHAT_CHANNEL_CAMP,			// 阵营频道

	CHAT_CHANNEL_MAX			// 最大频道数
};

// 系统消息ID定义
// 每个功能留2000ID区间
// 服务器与服务器、服务器与客户端、客户端与客户端之间涉及到的字符串传输统一使用UTF-8格式
ENUM EMChatTipID
{
    CHAT_TIP_CHATBOX_AND_MIDDLE_TIP = -4,			// {1}配置的的自定义描述（聊天框和屏幕中间显示）
    CHAT_TIP_CHATBOX_CUSTOMER_TIP = -3,				// {1}配置的的自定义描述（聊天框显示）
    CHAT_TIP_COMMON_OPT_CD_TIP = -2,				// 你操作太频繁,{1}秒再试
    CHAT_TIP_CUSTOMER_TIP = -1,						// {1}配置的自定义描述（屏幕正下方显示）

	CHAT_TIP_ENTITY_BEGIN,						    // 实体用提示起始
	CHAT_TIP_ENTITY_SKILL_COOLING,					// 技能冷却中
	CHAT_TIP_ENTITY_HP_NOT_ENOUGH,					// HP不足
	CHAT_TIP_ENTITY_MP_NOT_ENOUGH,					// MP不足
	CHAT_TIP_ENTITY_RAGE_NOT_ENOUGH,				// 怒气不足
	CHAT_TIP_ENTITY_STAMINA_NOT_ENOUGH,				// 体力不足
	CHAT_TIP_ENTITY_MONEY_NOT_ENOUGH,				// 金钱不足
	CHAT_TIP_ENTITY_NULL_TARGET,					// 无效目标
	CHAT_TIP_ENTITY_NOT_SELECT_TARGET,				// 未选中目标
	CHAT_TIP_ENTITY_CANNOT_USE_SKILL,				// 无法使用技能
	CHAT_TIP_ENTITY_IS_DYING,						// 处于死亡状态
	CHAT_TIP_ENTITY_TARGET_BLOCK,					// 目标有阻挡
	CHAT_TIP_ENTITY_TARGET_IMMUNE_ATTACK,			// 目标无敌
	CHAT_TIP_ENTITY_TARGET_IMMUNE_MAGIC,			// 目标魔免
	CHAT_TIP_ENTITY_TARGET_TOO_FAR,					// 目标距离过远
	CHAT_TIP_ENTITY_TALENT_ACTIVE_FAIL,				// 激活天赋失败
	CHAT_TIP_ENTITY_TALENT_NEEDACTIVE,				// 需要先激活天赋
	CHAT_TIP_ENTITY_TALENT_SKILL_NOT_READY,			// 技能未准备好
	CHAT_TIP_ENTITY_SKILL_NULL_TARGET,				// 选择对象无效
	CHAT_TIP_ENTITY_SKILL_BLOCKED,					// 与目标之间有障碍物
	CHAT_TIP_ENTITY_WING_NEEDSTAMINA,				// 飞行体力不够
	CHAT_TIP_ENTITY_WING_NEEDTIME,					// 飞行时间不够
	CHAT_TIP_ENTITY_WING_CANNOTACCELERATION,		// 不允许加速飞行
	CHAT_TIP_ENTITY_WING_ACCELERATIONNEEDSTAMINA,	// 飞行加速体力不够
    CHAT_TIP_ENTITY_SKILL_CTRL_NOT_USE,	            // 当前处于控制状态,无法使用技能
    CHAT_TIP_ENTITY_ACTIVE_TALENT_COST_POINT,	    // 激活{1}天赋，消耗{2}技能点
    CHAT_TIP_ENTITY_USE_FOR_LOCATE_TARGET,	        // 只能对{1}释放
    CHAT_TIP_ENTITY_PK_NO_CHOOSE_SELF,	            // 不能和自己决斗
    CHAT_TIP_ENTITY_PK_NO_CHOOSE_TARGET,	        // 请选择有效的决斗目标
    CHAT_TIP_ENTITY_PK_TARGET_NO_EXIST,	            // 目标不存在
    CHAT_TIP_ENTITY_PK_DISTANCE_FAR,	            // 决斗距离过远
    CHAT_TIP_ENTITY_PK_REQUEST_OFTEN,	            // 你操作太频繁,{1}秒再试
    CHAT_TIP_ENTITY_PK_NO_SAME_SCENE,	            // 不在同场景,不可决斗
    CHAT_TIP_ENTITY_PK_TARGET_BUSY,	                // 目标正忙,请稍后再试
    CHAT_TIP_ENTITY_PK_NO_AGREE,	                // {1}拒绝了你的决斗请求
    CHAT_TIP_ENTITY_PK_NO_REQUEST,	                // 无有效决斗请求,或者决斗请求已过期
    CHAT_TIP_ENTITY_PK_TARGET_OFFLINE,	            // 玩家已下线或者不在本场景,操作失败
    CHAT_TIP_ENTITY_PK_PK_STATE_ERROR,	            // 目标当前状态不可决斗,请稍后再试
    CHAT_TIP_ENTITY_PK_ALREADY_PK_STATE,	        // 已经在PK状态
    CHAT_TIP_ENTITY_PK_DISTANCE_FAR_FAIL,	        // 你离开战斗范围过远,决斗结束
    CHAT_TIP_ENTITY_PK_BY_KILL,	                    // 你被击杀,决斗结束
    CHAT_TIP_ENTITY_PK_TIME_OUT,	                // 超过战斗时间,决斗结束
    CHAT_TIP_ENTITY_PK_LOGOUT,	                    // 决斗中下线,决斗结束
    CHAT_TIP_ENTITY_PK_END_PK_PROMPT,	            // {1}
    CHAT_TIP_ENTITY_PK_CANCEL_PK,	                // 你主动取消决斗，决斗结束
    CHAT_TIP_ENTITY_PK_SCENE_FLAT,	                // {1}与{2}激烈决斗,不分上下
    CHAT_TIP_ENTITY_PK_SCENE_WIN,	                // {1}战胜{2}
    CHAT_TIP_ENTITY_PK_TARGET_REPEAT_REQUEST,	    // 你已向目标发送决斗请求，请不要重复操作
    CHAT_TIP_ENTITY_PK_REQUEST_TIP,	                // 向{1}发起决斗请求成功
    CHAT_TIP_ENTITY_FORBIT_PK,                      // 本地图禁止决斗
    CHAT_TIP_ENTITY_NOT_ACTIVE_TALENT,	            // 当前状态不允许激活天赋
    CHAT_TIP_ENTITY_WING_NEEDMP,				    // 飞行魔法不够
    CHAT_TIP_ENTITY_WING_NEEDENERGY,				// 飞行能量不够
    CHAT_TIP_ENTITY_WING_ACCELERATIONNEEDMP,	    // 飞行加速魔法不够
    CHAT_TIP_ENTITY_WING_ACCELERATIONNEEDENERGY,	// 飞行加速能量不够
	CHAT_TIP_ENTITY_PK_MATCHSTATE,                  // 匹配状态下不可PK
    CHAT_TIP_ENTITY_GOLD_OPERATE_ERROR,			    // 金币操作失败！
    CHAT_TIP_ENTITY_TICKET_OPERATE_ERROR,			// 点券操作失败！
    CHAT_TIP_ENTITY_NOTUSE_IN_WAR,					// 在战场中，不可使用

	CHAT_TIP_ENTITY_DIDA_CANNOT_OPEN = 1000,		// 打开失败：你还有滴答没有处理，必须处理后才能继续打开

	CHAT_TIP_ANTIADDICT_INCOME_REDUCE = 2000,		// {1}
	CHAT_TIP_ANTIADDICT_IDTYOE_WRONG,				// 请输入正确格式的身份证号

    CHAT_TIP_ENTITY_SPELL_CAST_FAIL = 3900,         // 技能使用失败结果(3900~3999)
    CHAT_TIP_ENTITY_SPELL_CAST_FAIL_SILENCE,        // 当前处于控制状态，无法使用技能
    CHAT_TIP_ENTITY_SPELL_CAST_FAIL_CONDITION,      // 条件不足，无法释放
    CHAT_TIP_ENTITY_SPELL_CAST_FAIL_COUNT_LIMIT,    // 当前技能使用次数达到上限
    CHAT_TIP_ENTITY_SPELL_CAST_FAIL_LOC_VERIFY,     // 位置校验失败
    CHAT_TIP_ENTITY_SPELL_CAST_FAIL_CHARGED_NOTENOUGH, // 充能层数不足!
    CHAT_TIP_ENTITY_SPELL_CAST_FAIL_CUSTOM_BEGIN=3960,// 给策划使用
    CHAT_TIP_ENTITY_SPELL_CAST_FAIL_END = 3999,     // 技能使用失败最大提示

	CHAT_TIP_WAR_BEGIN = 4000,						// 战场用提示起始
	CHAT_TIP_WAR_MINIMAP_FLAG_BUSY,					// 小地图插旗过快
	CHAT_TIP_WAR_CHANGE_GODEYE_IN_BATTLE,			// 战斗中切换上帝视角
	CHAT_TIP_WAR_MAPSIGN_A_ENEMY_HERO,				// 小地图标记攻击敌方英雄
	CHAT_TIP_WAR_MAPSIGN_R_ENEMY_HERO,				// 小地图标记远离敌方英雄
	CHAT_TIP_WAR_MAPSIGN_R_FRIEND_TOWER,			// 小地图标记防御己方塔
	CHAT_TIP_WAR_MAPSIGN_A_ENEMY_TOWER,				// 小地图标记攻击敌方塔
	CHAT_TIP_WAR_MAPSIGN_R_ENEMY_TOWER,				// 小地图标记远离敌方塔
	CHAT_TIP_WAR_MAPSIGN_A_ENEMY_MONSTER,			// 小地图标记攻击野怪
	CHAT_TIP_WAR_MAPSIGN_R_ENEMY_MONSTER,			// 小地图标记远离野怪
	CHAT_TIP_WAR_MAPSIGN_DANGER,					// 小地图标记-危险
	CHAT_TIP_WAR_MAPSIGN_ONWAY,						// 小地图标记-正在赶来
	CHAT_TIP_WAR_MAPSIGN_HELP,						// 小地图标记-请求协助
	CHAT_TIP_WAR_MAPSIGN_LOST,						// 小地图标记-敌人不见了
	CHAT_TIP_WAR_GOODS_MAXHOLD,						// 购物-已达最大数量
	CHAT_TIP_WAR_GOODS_MONEYNOTENOUGH,				// 购物-金钱不足
	CHAT_TIP_WAR_GOODS_NOSUCHGOODS,					// 购物-无物品信息
	CHAT_TIP_WAR_GOODS_CANNOTSELL_STATE,			// 购物-回城才能出售物品
    CHAT_TIP_WAR_GOODS_CANNOTBUY_STATE,				// 购物-回城才能购买物品

	CHAT_TIP_WAR_AICOMMAND_JOINFOLLOW,				// AI控制-加入跟随
	CHAT_TIP_WAR_AICOMMAND_CANCELFOLLOW,			// AI控制-取消跟随
	CHAT_TIP_WAR_AICOMMAND_ATTACKPLAYER,			// AI控制-攻击敌方玩家
	CHAT_TIP_WAR_AICOMMAND_ATTACKBASE,				// AI控制-攻击敌方基地
	CHAT_TIP_WAR_AICOMMAND_ATTACKTOWER,				// AI控制-攻击敌方防御塔
	CHAT_TIP_WAR_AICOMMAND_ATTACKSOLDIER,			// AI控制-攻击敌方士兵
	CHAT_TIP_WAR_AICOMMAND_ATTACKMONSTER,			// AI控制-攻击中立生物
	CHAT_TIP_WAR_AICOMMAND_HEADSOMEWHERE,			// AI控制-前往某地
	CHAT_TIP_WAR_AICOMMAND_APPOINTCOMMAND,			// AI控制-移交控制权

    CHAT_TIP_WAR_GOODS_SLOT_ISFULL,				    // 购物-物品已满
    CHAT_TIP_WAR_GOODS_BUY_LIMIT,				    // 购物-物品限制购买
    CHAT_TIP_WAR_GOODS_BUY_FREEZE,				    // 购物-购买物品冷却中

    CHAT_TIP_WAR_FORCE_END_COUNTDOWN_MIN,			// 强制结算-最后5min倒计时
    CHAT_TIP_WAR_FORCE_END_COUNTDOWN_SEC_START,		// 强制结算-最后10s倒计时提示
    CHAT_TIP_WAR_FORCE_END_COUNTDOWN_SEC,			// 强制结算-最后10s倒计时

    CHAT_TIP_WAR_BADBEHAVIOR_BOTTING_KICKOUT,		// 恶意行为挂机-被踢下线
    CHAT_TIP_WAR_GOODS_COOLDOWN,		            // 正在冷却中，还剩{1}秒

    CHAT_TIP_WAR_BATTLEWAR_KILLTIP,		            // 淘汰1人-剩余{1}人
    CHAT_TIP_WAR_BATTLEWAR_GENIC_MUTEX,				// 与本体基因相同，无需收集
    CHAT_TIP_WAR_BATTLEWAR_GENIC_ALREADYOWNED,		// 该基因已经被收集
    CHAT_TIP_WAR_BATTLEWAR_GENIC_OWNEDTIP,			// 淘汰{1}玩家，收集{2}的影响基因
    CHAT_TIP_WAR_BATTLEWAR_DIE_IN_NOX,				// {1}在安全区外被毒圈淘汰
    CHAT_TIP_WAR_BATTLEWAR_FAKED_KILL,				// 您击倒了{1}
	CHAT_TIP_WAR_GOODS_FREEZE,						// 正在冷却中


	CHAT_TIP_CHAT_BEGIN = 6000,						// 聊天用提示起始
	CHAT_TIP_CHAT_SEND_TEXT_BUSY,					// 发送类似信息过快
	CHAT_TIP_CHAT_BAN_TEAMMATE_CHANNEL,				// 切换队伍频道失败
	CHAT_TIP_CHAT_BAN_WARSCENE_CHANNEL,				// 切换战场频道失败
	CHAT_TIP_CHAT_BAN_MATCHROOM_CHANNEL,			// 切换房间频道失败
	CHAT_TIP_CHAT_BAN_KIN_CHANNEL,					// 切换战队频道失败
	CHAT_TIP_CHAT_BAN_CLAN_CHANNEL,					// 切换联盟频道失败
	CHAT_TIP_CHAT_CHANNEL_COOLDOWN,					// 指定频道正在冷却中
	CHAT_TIP_CHAT_LIMIT_LEVEL,						// 在指定频道发言不够等级
	CHAT_TIP_CHAT_BAN_CAMP_CHANNEL,					// 切换阵营频道失败
	CHAT_TIP_CHAT_CLICK_RICHTEXT_ITEM_COOLDOWN,		// 富文本点击冷却

	CHAT_TIP_LOGIN_BEGIN = 8000,					// 登录用提示起始
	CHAT_TIP_LOGIN_SERVER_MESSAGE,					// 服务器返回信息
	CHAT_TIP_LOGIN_FORCE_LOGINING,					// 强制登录中
	CHAT_TIP_LOGIN_TREK_FAIL,						// 迁移失败
	CHAT_TIP_LOGIN_HEROCREATE_BADNAME,				// 创建角色名称太短或不合法
	CHAT_TIP_LOGIN_HEROCREATE_BADID,				// 未选择英雄
	CHAT_TIP_LOGIN_HEROCREATE_BADSEX,				// 未选择性别

	CHAT_TIP_KIN_BEGIN = 10000,						// 战队用提示起始
    CHAT_TIP_KIN_DISMISS,                           // 您的战队已经解散了
    CHAT_TIP_KIN_HAD_JOINED_KIN,                    // 您已经拥有战队
    CHAT_TIP_KIN_HAD_NO_RIGHT,                      // 您没有操作权限
    CHAT_TIP_KIN_CANT_QUIT,                         // 族长不能退出战队
    CHAT_TIP_KIN_REPEAT_APPLY,                      // 重复申请
    CHAT_TIP_KIN_APPLY_OVER,                        // 该战队申请列表已经达到20人，不能申请
    CHAT_TIP_KIN_HAD_JOINED_KIN_MUST_QUIT,          // 您有战队了，请先退出战队再申请
    CHAT_TIP_KIN_APPLY_TIMES_OVER,                  // 申请战队过多，无法申请加入战队
    CHAT_TIP_KIN_APPLY_FAILD,                       // 申请加入失败
    CHAT_TIP_KIN_NOT_IN_APPLY_LIST,                 // 对方已经不在本战队的申请列表了，请重新打开界面刷新
    CHAT_TIP_KIN_LEAVE_TIME_NOT_SATISFY,            // 您离开战队还不满{1}小时
    CHAT_TIP_KIN_APPLY_HAVE_KIN,                    // 申请人员已经有战队
    CHAT_TIP_KIN_CREATE_SUCCESS,                    // 创建战队成功
    CHAT_TIP_KIN_ADMIT_REJECT,                     //  您拒绝{1}加入您的战队
    CHAT_TIP_KIN_ADMIT_WAS_REJECT,                 //  您被拒绝加入战队:{1}
    CHAT_TIP_KIN_HAVE_NO_KIN,                       // 您当前没有战队
    CHAT_TIP_KIN_CANT_KICK_SHAIKH,                  // 不能开除族长
    CHAT_TIP_KIN_CANT_KICK_ELDER,                   // 不能开除副族长
    CHAT_TIP_KIN_APPLY_SUCCESS,                     // 申请加入成功
    CHAT_TIP_KIN_MODIFY_NOTICE_SUCCESS,             // 修改公告成功
    CHAT_TIP_KIN_AUTOACCEPT_SAVE_SUCCESS,           // 修改自动设置成功
    CHAT_TIP_KIN_SELECT_ACCEPT_MEMBER,              // 未选中接收的人物
	CHAT_TIP_KIN_CREATE_NAME_ISEMPTY,               // 创建战队人物名称为空
	CHAT_TIP_KIN_ACTOR_JOIN_TIP,	                // 大家好，我是新来的成员
    CHAT_TIP_KIN_MEMBER_FULL,                       // 该战队已经满员，不能申请
    CHAT_TIP_KIN_SHAIKH_CAN_APPOINT,                // 只有族长才可以任命职位
    CHAT_TIP_KIN_CANT_APPOINT_SELF,                 // 不能任命自己职位
    CHAT_TIP_KIN_MEMBER_HAVE_SAME_IDENTITY,         // 此玩家已经是[{1}]了
    CHAT_TIP_KIN_CANT_APPOINT_MORE_IDENTITY,        // 您的战队已经拥有[{1}]个副族长，不能任命更多
    CHAT_TIP_KIN_APPOINT_UP_IDENTITY,               // 您被升职为{1}
    CHAT_TIP_KIN_APPOINT_LOW_IDENTITY,               // 您被降职为{1}
	CHAT_TIP_KIN_CREATE_LABEL_ISEMPTY,				// 创建战队标签名称为空
	CHAT_TIP_KIN_CREATE_NAME_LEN_WRONG,				// 创建战队战队名称长度有误
	CHAT_TIP_KIN_CREATE_LABEL_LEN_WRONG,			// 创建战队标签名称长度有误
	CHAT_TIP_KIN_NAME_HAS_SHIELD_WORLD,				// 战队名称含有敏感字符，请修改后重试
	CHAT_TIP_KIN_LABEL_HAS_SHIELD_WORLD,			// 战队标签含有敏感字符，请修改后重试    
	CHAT_TIP_KIN_CREATE_NEED_MONEY,                 // 金币不够，创建战队需要金币:{1}！    
	CHAT_TIP_KIN_RENAME_KIN_HAVE_NOT_KIN,           // 您没有战队，无法修改战队名称！
    CHAT_TIP_KIN_RENAME_KIN_IN_OTHER_KIN,           // 您已加入别的战队，无法修改战队名称！
    CHAT_TIP_KIN_SHAIKH_CAN_RENAME_KIN,             // 您不是族长，无法修改！
    CHAT_TIP_KIN_HAVE_NOT_KIN_CANT_INVITE,          // 您没有加入家族，不能邀请！
    CHAT_TIP_KIN_HAVE_NOT_RIGHT_INVITE,             // 您没有邀请成员的权限，只有族长和副族长才可以招收成员
    CHAT_TIP_KIN_INVITE_PERON_NOT_ONLINE,           // 您邀请的玩家不在线
    CHAT_TIP_KIN_INVITE_PERON_HAVE_KIN,             // 对方已经有家族了，不能邀请
    CHAT_TIP_KIN_INVITE_HAVE_SEND,                  // 邀请已经发送，等待对方回复
    CHAT_TIP_KIN_NOT_EXIST,                         // 邀请您的战队已经不存在了
    CHAT_TIP_KIN_YOU_HAVE_KIN,                      // 您已经有家族了
    CHAT_TIP_KIN_MEMBER_IS_FULL,                    // 对方战队人数已经满了
    CHAT_TIP_KIN_NAME_CANT_HAVE_SPACE,              // 家族名字中间不能有空格！
    CHAT_TIP_KIN_CREATE_LEVEL_LIMIT,                // 创建家族至少要{1}级！
	CHAT_TIP_KIN_NOTICE_HAS_SHIELD_WORLD,			// 战队公告含有敏感字符，请修改后重试  
    CHAT_TIP_KIN_MEMBER_JOIN,			            // 恭喜玩家[{1}]加入战队
    CHAT_TIP_KIN_CANT_DISMISS,			            // 战队正在参加杯赛，无法解散战队！
	CHAT_TIP_KIN_KICK_NOT_SELECT,			        // 请选中要操作的成员

    CHAT_TIP_CLAN_BEGIN = 10500,					// 帮会文字提示起始
    CHAT_TIP_CLAN_NO_KIN,                           // 您不是正式战队成员，无法创建帮会
    CHAT_TIP_CLAN_NO_KIN_SHAIKH,                    // 您不是战队族长，不能创建帮会
    CHAT_TIP_CLAN_HAVE_CLAN,                        // 您的战队已经加入了帮会，不能创建帮会
    CHAT_TIP_CLAN_DISMISS_PRIVILEGE,                // 只有帮主才可以解散帮会
    CHAT_TIP_CLAN_CANEL_DISMISS_PRIVILEGE,          // 只有帮主才可以取消解散帮会
    CHAT_TIP_CLAN_RESPOND_NO_KIN,                   // 您不是正式战队成员，不能响应帮会
    CHAT_TIP_CLAN_RESPOND_NO_KIN_SHAIKH,            // 您不是战队族长，不能响应帮会
    CHAT_TIP_CLAN_RESPOND_HAVE_CLAN,                // 您战队已经加入帮会了，不能响应帮会
    CHAT_TIP_CLAN_APPLY_NO_KIN,                     // 只有正式的战队才可以申请加入帮会
    CHAT_TIP_CLAN_APPLY_NO_KIN_SHAIKH,              // 只有战队的族长才可以申请加入帮会
    CHAT_TIP_CLAN_APPLY_CLAN_MEMBER_FULL,           // 该帮会已经满员，不能申请
    CHAT_TIP_CLAN_APPLY_LIST_FULL,                  // 该帮会申请列表已经达到10人，不能申请
    CHAT_TIP_CLAN_APPLY_CLAN_COUNT_FULL,            // 帮会人数已经达到上限,不能申请
    CHAT_TIP_CLAN_APPLY_HAVE_CLAN,                  // 您战队已经加入帮会了，请先退出帮会再申请
    CHAT_TIP_CLAN_APPLY_HAVE_APPLY,                 // 您已经申请过了, 请等待该帮会处理
    CHAT_TIP_CLAN_APPLY_SUCCEED,                    // 您已经向{1}帮会发出了申请，请等待该帮会处理
    CHAT_TIP_CLAN_ADMIT_PRIVILEGE,                  // 只有帮主才可以接收战队
    CHAT_TIP_CLAN_ADMIT_NO_MY_CLAN,                 // 对方已经不在本帮会的申请列表了，请重新打开界面刷新
    CHAT_TIP_CLAN_ADMIT_NO_INFORMAL_KIN,            // 对方不是正式的战队，不能接收
    CHAT_TIP_CLAN_ADMIT_HAVE_CLAN,                  // 对方已经加入帮会了
    CHAT_TIP_CLAN_ADMIT_MEMBER_MAX,                 // 您的帮会的战队数量已经到达上限
    CHAT_TIP_CLAN_ADMIT_COUNT_MAX,                  // 您的帮会人数已经达到上限
    CHAT_TIP_CLAN_ADMIT_REJECT,                     // 您拒绝{1}加入您的帮会
    CHAT_TIP_CLAN_KICK_PRIVILEGE,                   // 您不是帮会帮主，不能开除战队
    CHAT_TIP_CLAN_KICK_NO_KIN,                      // 该玩家不在战队
    CHAT_TIP_CLAN_KICK_NO_CLAN,                     // 该玩家不在本帮会
    CHAT_TIP_CLAN_KICK_NO_KIN_SHAIKH,               // 对方不是战队族长，不能开除，开除战队族长，其战队所有成员都退出帮会
    CHAT_TIP_CLAN_KICK_SUCCEED,                     // 开除{1}成功
    CHAT_TIP_CLAN_KICK_KM_PRIVILEGE,                // 您不是帮会帮主，不能开除战队或成员
    CHAT_TIP_CLAN_KICK_KM_NO_KIN,                   // 该玩家不在战队
    CHAT_TIP_CLAN_KICK_KM_NO_CLAN,                  // 该玩家不在本帮会
    CHAT_TIP_CLAN_KICK_KM_KIN_SHAIKH,               // 对方是战队族长或副族长，不能单独开除
	CHAT_TIP_CLAN_DAY_LIMIT_KICK_COUNT,             // 每天最多仅能开除｛0｝个家族
    CHAT_TIP_CLAN_KICK_KM_SUCCEED,                  // 开除{1}成功
    CHAT_TIP_CLAN_MOTIFY_MANIFESTO,                 // 只有帮主和副帮主才可以修改宣言
    CHAT_TIP_CLAN_MOTIFY_NOTICE,                    // 只有帮主和副帮主才可以修改公告
    CHAT_TIP_CLAN_FIND_NO,                          // 找不到帮会[*{1}*]
    CHAT_TIP_CLAN_FIND_EXIST,                       // 帮会不存在
    CHAT_TIP_CLAN_FIND_END,                         // 已经是最后一页了
    CHAT_TIP_CLAN_QUIT_SHAIKH,                      // 帮主不能退出帮会，请把帮主任命给其他人副帮主后再退出
    CHAT_TIP_CLAN_QUIT_ELDER,                       // 您不是战队族长，不能退出帮会，战队族长一旦退出帮会，整个战队的成员都离开帮会
    CHAT_TIP_CLAN_CARD_NO_CLAN,                     // 加入联盟,才可请求
    CHAT_TIP_CLAN_CARD_NO_MY_CLAN,                  // 只可操作自己联盟卡牌信息
    CHAT_TIP_CLAN_CARD_HAVE_REQUEST,                // 不可重复请求支援
    CHAT_TIP_CLAN_CARD_COOL,                        // 请求支援冷却中
    CHAT_TIP_CLAN_CARD_NO_FORMALHERO,               // 申请捐赠的卡牌需要是自已已经拥有的英雄而不是临时英雄
    CHAT_TIP_CLAN_CARD_DAY_COUNT,                   // 请求支援今日已达到上限，请明日再试
    CHAT_TIP_CLAN_CARD_DAY_DONATE_COUNT,            // 今日你已捐献太多，请明日再试
    CHAT_TIP_CLAN_CARD_DONATE_SINGLE_LIMIT,         // 今日你对此玩家已捐献太多，请明日再试
    CHAT_TIP_CLAN_CARD_DONATE_NO_CLAN,              // 加入联盟,才可捐赠
    CHAT_TIP_CLAN_CARD_DONATE_NO_MY_CLAN,           // 只可对本联盟玩家捐赠卡牌
    CHAT_TIP_CLAN_CARD_DONATE_SUCCEED,              // 捐赠卡牌成功
    CHAT_TIP_CLAN_CARD_GET_DONATE_SUCCEED,          // 获得捐赠卡牌成功
    CHAT_TIP_CLAN_CARD_DONATE_CHANGE,               // 捐赠信息已改变,请重新刷新再试
    CHAT_TIP_CLAN_CARD_DONATE_SELF,                 // 不能给自己捐赠
    CHAT_TIP_CLAN_CREATE_SUCCEED,                   // [{1}]联盟正式成立
    CHAT_TIP_CLAN_ADMIT_BY_REJECT,                  // {1}拒绝了您的加入请求
    CHAT_TIP_CLAN_CREATE_TIP,                       // {1}
    CHAT_TIP_CLAN_CREATE_HAVE_CLAN,                 // 你已经有帮会了，不能创建帮会
	CHAT_TIP_CLAN_OPENWND_NO_CLAN,                  // 请先加入一个联盟
    CHAT_TIP_CLAN_DISMISS_STATE,                    // 只有在正式帮会才可解散，或您的帮会已在解散状态了
    CHAT_TIP_CLAN_DISMISS_TIP,                      // [{1}]解散了帮会，{2}天后您的帮会将会解散
    CHAT_TIP_CLAN_CANEL_DISMISS_TIP,                // [{1}]取消解散帮会
    CHAT_TIP_CLAN_ADMIT_ADD,                        // [{1}]加入了帮会
    CHAT_TIP_CLAN_DISMISS_SUCCEED,                  // 您的帮会已经解散了
	CHAT_TIP_CLAN_KICK_KIN_MEMBER,                  // [{1}]被[{2}]踢出战队
	CHAT_TIP_CLAN_ACTOR_REQUEST_CARD_TIP,           // 发布捐赠请求XX卡牌,前往捐赠
	CHAT_TIP_CLAN_ACTOR_DONATE_CARD_TIP,            // 向你捐赠了X张XX卡牌
    CHAT_TIP_CLAN_BY_KICK,                          // 您战队被踢出{1}帮会
    CHAT_TIP_CLAN_MUST_SHAIKH_APPOINT,              // 只有帮主才可以任命职位
    CHAT_TIP_CLAN_SHAIKH_APPOINT_SELF,              // 不能任命自己职位
    CHAT_TIP_CLAN_APPOINT_FAIL,                     // 对方不是战队族长，不能任命他为帮主
    CHAT_TIP_CLAN_APPOINT_SHAIKH,                   // 您被任命为{1}
    CHAT_TIP_CLAN_APPOINT_SUCCEED,                  // 任命[{1}]为{2}成功
    CHAT_TIP_CLAN_CREATE_NAME_SHIELD_WORD,	        // 联盟名字含有屏蔽字符
    CHAT_TIP_CLAN_MANIFESTO_SHIELD_WORD,	        // 联盟口号含有屏蔽字符
	CHAT_TIP_CLAN_LEGEND_CUP_REGISTER_TIP,	        // 盟主[{1}]发布【{2}】，战队队长可{3}[LegendCup color=255+255+255 CupID={4}]点击报名[/LegendCup]({5}/{6})
	CHAT_TIP_CLAN_LEGEND_CUP_PROCEED_TIP,	        // 联盟杯赛【{1}】{2}强争夺，{3}[LegendCup color=255+255+255 CupID={4}]点击查看[/LegendCup]
	CHAT_TIP_CLAN_NOTICE_SHIELD_WORD,	            // 联盟公告含有屏蔽字符
	CHAT_TIP_CLAN_MODIFY_ENROLL_STUDENT,	        // 修改接受成员设置	
    CHAT_TIP_CLAN_DISMISSING_NO_ACCEPT,             // 该联盟正在解散状态，不允许加入
    CHAT_TIP_CLAN_CREATE_LIMIT_LEVEL,               // 您的等级不足{0}级
    CHAT_TIP_CLAN_CREATE_LIMIT_MONEY,               // 您的金钱不足{0}
	CHAT_TIP_CLAN_NOT_SLELECT,                      // 请选中一个联盟
	CHAT_TIP_CLAN_NAME_IS_EMPTY,                    // 请输入联盟名字
	CHAT_TIP_CLAN_INVITE_HAVA_CLAN,                 // 对方已经有联盟
	CHAT_TIP_CLAN_INVITE_KIN_SHAIKH_OFFLINE,        // 队长不在线
    CHAT_TIP_CLAN_SHAIKH_CAN_PUBLIC_CUP,            // 联盟杯赛已达到免费要求，请{1}[createclancupwnd color=255+255+255]前往发布[/createclancupwnd]杯赛!
    CHAT_TIP_CLAN_MEMBER_REQ_SHAIKH_PUBLIC_CUP,     // 联盟杯赛已达到发布条件，请盟主发布!

    CHAT_TIP_PLAYER_BEGIN = 11000,                  // 玩家信息
    CHAT_TIP_PLAYER_MONEYNOTENOUGH,                 // 玩家金钱不足
    CHAT_TIP_PLAYER_CARDNOTENOUGH,                  // 玩家卡牌不足
    CHAT_TIP_PLAYER_STARLVISMAX,                    // 卡牌星级达到最大
    CHAT_TIP_PLAYER_NOTHAVETHISHERO,                // 玩家没有此英雄
    CHAT_TIP_PLAYER_BUYHERO_SUCESS,                 // 购买英雄成功
    CHAT_TIP_PLAYER_BUYHERO_ISHAVE,                 // 玩家已经拥有此英雄
    CHAT_TIP_PLAYER_CHANGEHERO_SUCESS,              // 切换英雄成功
    CHAT_TIP_PLAYER_BUYHERO_BINDGOLD_NOTENOUGH,     // 购买英雄时绑定金币不足
    CHAT_TIP_PLAYER_CHANGEHERO_FAILED_REASON_FLY,   // 飞行状态下不能切换英雄
	CHAT_TIP_PLAYER_ADDVALUE_HAS_NO_SINGLE_WAR,		// 累加数据无对应单场战绩信息
	CHAT_TIP_PLAYER_VALUE_HAS_NO_SOURCE,			// 该数据暂无对应战绩信息
	CHAT_TIP_PLAYER_CHANGEHERO_FAILED_REASON_PK,	// PK状态下不能切换英雄
    CHAT_TIP_PLAYER_CANT_DO_THIS_OPRATION,	        // 等待进入房间无法进行该操作！
	CHAT_TIP_PLAYER_CROWNPAGENAME_SHIELD_WORD,	    // 宝石页名字含有屏蔽字符
	CHAT_TIP_PLAYER_CHANGEHERO_FAIL,				// 切换英雄失败
	CHAT_TIP_PLAYER_CHANGEHERO_FAIL_NO_HERO,		// 切换英雄失败无英雄
	CHAT_TIP_PLAYER_CHANGEHERO_FAIL_NO_SKIN,		// 切换英雄失败无皮肤
	CHAT_TIP_PLAYER_CHANGEHERO_FAIL_NO_SELECT,		// 切换英雄失败未选择
	CHAT_TIP_PLAYER_CHANGEHERO_FAIL_SAME_SKIN,		// 切换英雄失败与当前皮肤相同
	CHAT_TIP_PLAYER_CAN_UPGRADE_TIP,				// 完成英雄晋升，让他变的更强
	CHAT_TIP_PLAYER_CAN_NOT_UPGRADE_TIP,			// 您的英雄还不符合晋升要求，可以通过完成每日任务加快进度
	CHAT_TIP_PLAYER_CAN_NOT_LOOK_AI_INFO,			// 电脑英雄不能查看
	CHAT_TIP_PLAYER_FORBID_CHANGE,			        // 当前禁止切换英雄和皮肤
	CHAT_TIP_PLAYER_TOPUP_SUCCESS,					// 您已成功充值点券{1}

    CHAT_TIP_TEAM_BEGIN = 12000,                        // 组队相关开始
    CHAT_TIP_TEAM_SERVICE_BUSY_TRY_LATER,                   // 服务器繁忙，请稍候邀请组队...
    CHAT_TIP_TEAM_INVITED_NOT_ONLINE,                       // 对方玩家已经不在线！无法组队！
    CHAT_TIP_TEAM_HAVE_HAD_TEAM,                            // 对方已有队伍！
    CHAT_TIP_TEAM_WARIT_FOR_THIS_PLAYER_INVITE_RESPON,      // 您正等候此玩家响应上次请求，请稍候再次请求！
    CHAT_TIP_TEAM_MEMBER_IS_FULL_CANT_JOIN,                 //您所在的队伍已满，无法加入新队员！
    CHAT_TIP_TEAM_LOWER_GROUP_LIMIT_LEVEL,                  // [{1}]的等级低于团队等级限制，无法邀请加入！
    CHAT_TIP_TEAM_CANT_JOIN_GROUP,                          // 您的团设置了禁止入团，无法邀请加入！
    CHAT_TIP_TEAM_HAVE_SEND_INVITE_DIDA,                    // 已向对方发送了邀请请求，请耐心等待对方响应！
    CHAT_TIP_TEAM_PLAYER_NOT_ONLINE,                        // 玩家[{1}]已经不在线！无法响应你的请求！
    CHAT_TIP_TEAM_PLAYER_REFUSE_YOUR_INVITE,                // 对不起，[{1}]拒绝了您的邀请组队请求！
    CHAT_TIP_TEAM_PLAYER_SET_REFUSE_INVITE,                 // 对不起，[{1}]设置了[拒绝组队]，自动拒绝您的邀请组队请求！
    CHAT_TIP_TEAM_PLAYER_HAVE_TEAM,                         // 对不起，[{1}]已有队伍，邀请组队失败！
    CHAT_TIP_TEAM_YOU_HAVE_TEAM,                            // 对不起，你已有队伍，加入他人队伍失败！
    CHAT_TIP_TEAM_PLAYER_TEAM_IS_FULL,                      // 对不起，[{1}]已有队伍，并且队伍已满！
    CHAT_TIP_TEAM_PLAYER_HAVE_TEAM_CANT_JOIN,               //对不起，[{1}]已有队伍，无法加入！
    CHAT_TIP_TEAM_HAVE_DISMISS,                             //对不起，队伍已解散，加入队伍失败！
    CHAT_TIP_TEAM_IS_FULL,                                  //对不起，队伍已满，加入队伍失败！
    CHAT_TIP_TEAM_IS_FULL_INVITE_FAIL,                      //对不起，队伍已满，邀请[{1}]入队失败！
    CHAT_TIP_TEAM_CAPTAIN_OFFLINE_PLEASE_WAIT,              //对不起，因队长不在线，请稍候再申请加入！
    CHAT_TIP_TEAM_CAPTAIN_OFFLINE_INVITE_FAIL,              //对不起，因队长不在线，邀请[{1}]入队失败，请稍候再邀请加入
    CHAT_TIP_TEAM_HAVE_SEND_REQUEST_PLEASE_WAIT,            //已提交队长批准您的入队请求，请耐心等待！
    CHAT_TIP_TEAM_YOUR_REQUEST_TEAM_HAVE_DISMISS,           //对不起，等待队长批准您入队请求的队伍已解散！
    CHAT_TIP_TEAM_CAPTAIN_REFUSE_YOUR_REQUEST,              //对不起，队长拒绝您的入队请求！
    CHAT_TIP_TEAM_PLAYER_OFFLINE,                           //对不起，您刚批准的[{1}]已不在线，加入队伍失败！
    CHAT_TIP_TEAM_REQUEST_PLAYER_HAVE_TEAM,                 //对不起，您刚批准的角色已加入其他队伍，加入队伍失败！
    CHAT_TIP_TEAM_YOU_HAVE_LEAVE_CANT_APPROVE,              //对不起，您已离开了队伍，本次批准无效！
    CHAT_TIP_TEAM_CAPTAIN_LEAVE_PLEASE_REQUEST_AGAIN,       //对不起，队长中途离开队伍，请您再次申请加入！
    CHAT_TIP_TEAM_YOU_HAVE_CHANGE_TEAM_CANT_APPROVE,        //对不起，您已更换了队伍，本次批准无效！
    CHAT_TIP_TEAM_CAPTAIN_CHANGE_TEAM_PLEASE_REQUEST_AGAIN, //对不起，队长中途更换队伍，请您再次申请加入！
    CHAT_TIP_TEAM_YOU_ARE_INVITED_TEAM_IS_FULL,             //您刚被邀请的队伍已满，加入队伍失败！
    CHAT_TIP_TEAM_YOUR_TEAM_IS_FULL_PLAYER_CANT_IN,         //您所在的队伍已满，[{1}]无法加入队伍！
    CHAT_TIP_TEAM_SEND_YOUR_REQUEST_PLEASE_WAIT,            //已提交给对方批准您的入队请求，请耐心等待！
    CHAT_TIP_TEAM_PLAYER_OFFLINE_CANT_RESPON_REQUEST,       //对不起，[{1}]已不在线，无法响应您的组队请求！
    CHAT_TIP_TEAM_PLAYER_REFUSE_YOUR_REQUEST,               //对不起，[{1}]拒绝了您的组队请求！
    CHAT_TIP_TEAM_PLAYER_AUTO_REFUSE_YOUR_REQUEST,          //对不起，[{1}]设置了[拒绝组队]，自动拒绝您的请求！

    CHAT_TIP_TEAM_PLEASE_WAIT_FOR_CAPTAIN_APPROVE,          //已提交给对方队长批准，请耐心等待！
    CHAT_TIP_TEAM_PLAYER_OFFLINE_CANT_RESPON_YOUR_REQUEST,  //对不起，[{1}]已不在线，无法响应您的组队请求！
    CHAT_TIP_TEAM_YOUR_TEAM_ISFULL,                         //对不起，您的队伍已满，无法加入更多角色！
    CHAT_TIP_TEAM_TARGET_TEAM_ISFULL,                       //对不起，对方队伍已满，无法加入更多角色！

    CHAT_TIP_TEAM_REQUEST_PLAYER_NOT_ONLINE,                // 您申请组队的玩家不在线！
    CHAT_TIP_TEAM_REAPEAT_REQUEST,                          // 您正等候此队伍响应上次请求，请稍候再次请求！
    CHAT_TIP_TEAM_REQUEST_TEAM_ISFULL,                      // 您申请的队伍已满，无法申请加入新队伍！
    CHAT_TIP_TEAM_CAPTAIN_OFFLINE_REQUEST_LATER,            // 队长不在线，请稍候申请加入
    CHAT_TIP_TEAM_PLAYER_LEVLE_LOWER_LIMIT,                 // 您的等级低于[{1}]所在团的等级限制，无法申请加入！
    CHAT_TIP_TEAM_GROUP_CANT_JOIN,                          // 申请的团设置了禁止入团，无法申请加入！
    CHAT_TIP_TEAM_REQUEST_PLAYER_HAVE_HAD_TEAM,             // 您有队伍，需要申请别的队伍，需先退出队伍！
    CHAT_TIP_TEAM_REQUEST_PLAYER_HAVE_TEAM_TARGET_ISFULL,   // 您有队伍，需要申请别的队伍，需先退出队伍，并且对方队伍现在已满！
	CHAT_TIP_TEAM_SELECT_ONE_TARGET,						// 请先左键选中一个组队目标
	CHAT_TIP_TEAM_DO_NOT_SELECT_YOURSELF,					// 请选择自己以外的组队目标
    CHAT_TIP_TARGET_CANT_OPREATE,                           // 目标目前无法进行组队操作
    CHAT_TIP_TEAM_CLAN_INVITEJOIN_TEAM,                     // 加入我的队伍
    CHAT_TIP_TEAM_JOIN_SUCCESS_EX,							// 你已加入[{1}]的队伍，当前与队长不在一个线路，选择传线
    CHAT_TIP_TEAM_LEAVE_SUCCESS,							// 您已经退出了队伍！
    CHAT_TIP_TEAM_BE_KICK,                                  // 被队长开除了队伍！
    CHAT_TIP_TEAM_DISBAND,                                  // 您的队伍已经解散！
    CHAT_TIP_TEAM_SYSKICK,                                  // 系统强制解散！
    CHAT_TIP_TEAM_PLAYER_ALREADY_JOINED_TEAM,               // 您已加入了当前队伍

    CHAT_TIP_MATCH_BEGIN = 13000,                           //  撮合匹配相关
    CHAT_TIP_MATCH_GAMESTATE_CANT_REQUEST,                  //  {1}[{2}]无法进行撮合操作
    CHAT_TIP_MATCH_NOTCAPTAIN_CANT_REQUEST,                 //  您不是队长无法请求撮合操作
    CHAT_TIP_MATCH_TEAMMEMBER_OFFLINE,                      //  玩家[{1}]当前不在线!
    CHAT_TIP_MATCH_YOU_HAVEREQUEST,                         //  你已经提交了请求，请等待系统撮合！
    CHAT_TIP_MATCH_PLEASE_QUIT_MATCH,                       //  请先退出撮合！
    CHAT_TIP_MATCH_FAIL_RETRY,                              //  撮合失败，请稍后重试！
    CHAT_TIP_MATCH_ACTOR_EXIT_MATCH,                        //  玩家[{1}]退出当前撮合，请重新申请撮合！
    CHAT_TIP_MATCH_REQUEST_OVER_CONFIG_LIMIT,               //  请求比赛人数超过配置人数限制！
    CHAT_TIP_MATCH_RESTORE_DATA_CANT_REQUEST,               //  正在恢复数据中无法请求，请稍后再试！
    CHAT_TIP_MATCH_PERSONINROOM_CANT_REQUEST,               //  玩家[{1}]在房间中无法申请撮合！
    CHAT_TIP_MATCH_YOU_ARE_INROOM_CANT_REQUEST,             //  您在房间中无法申请撮合！
    CHAT_TIP_MATCH_ENTERROOM_PASSWORD_WRONG,                //  进入房间密码错误，请重试！
    CHAT_TIP_MATCH_CREATEROOM_PASSWORD_HAVE_CN,				//  房间密码不能包含中文字符！
    CHAT_TIP_MATCH_CREATEROOM_PASSWORD_HAVE_SHEILD,			//  房间密码不能包含屏蔽字符！
    CHAT_TIP_MATCH_CREATEROOM_PASSWORD_LEN_OVER,			//  房间密码太长！
    CHAT_TIP_MATCH_CREATEROOM_ROOMNAME_HAVE_SHEILD,			//  房间名不能包含屏蔽字符！
    CHAT_TIP_MATCH_CREATEROOM_ROOMNAME_LEN_OVER,			//  房间名太长！
    CHAT_TIP_MATCH_CREATEROOM_ROOMNAME_IS_EMPTY,			//  房间名不能为空！
    CHAT_TIP_MATCH_HERO_IS_INVALID,			                //  英雄无效，请重新选择！
    CHAT_TIP_MATCH_HERO_CANSELECT_COUNT_EMPTY,			    //  没有足够选择的英雄！
    CHAT_TIP_MATCH_LEGENDCUPROOM_FULL,                      //  房间队伍人数足够，无法再进入房间！
    CHAT_TIP_MATCH_NOT_FINDPOS,                             //  无法进入房间！没有找到空闲的位置！
    CHAT_TIP_MATCH_CREATE_GUIDEROOM_FAILED,                 //  创建新手引导房间失败！
    CHAT_TIP_MATCH_TEAM_ISMATCHING_CANT_REQUEST,            //  队伍在撮合中！无法申请撮合！
    CHAT_TIP_MATCH_WAITROOM_XP_UNLOCK,					    //  XP未解锁
    CHAT_TIP_MATCH_REFUSE_MATCH,                            //  玩家[{1}]拒绝了比赛，请重新申请撮合！
    CHAT_TIP_MATCH_REFUSE_REMATCH,                          //  有玩家拒绝了比赛，重新进入撮合！
    CHAT_TIP_MATCH_ROOM_IS_NULL,                            //  房间已经不存在！
    CHAT_TIP_MATCH_ROOM_IS_CANT_JOIN,                       //  房间已经开始比赛无法加入！
    CHAT_TIP_MATCH_ROOM_IS_FULL,                            //  房间人满无法加入！
    CHAT_TIP_MATCH_PLAYER_NOT_IDLE,                         //  当前状态无法接受邀请！
    CHAT_TIP_MATCH_PLAYER_BAN_HERO_INFO,                    //  ({1})申请禁选({2})
    CHAT_TIP_CREATE_LIMIT_ISNOT_CAPTAIN,                    //  您在队伍中，只有房主才能创建房间，想创建房间必须先退出队伍！
    CHAT_TIP_MATCH_ROOM_STATE_CANT_JOIN,                    //  房间当前状态无法进入！
    CHAT_TIP_MATCH_ROOM_CANT_FIND,                          //  没有找到房间！请刷新房间！
    CHAT_TIP_MATCH_ACTOR_OUT_REMATCH,                       //  玩家退出撮合，重新进入撮合！
    CHAT_TIP_MATCH_FINAL_BAN_LIST,                          //  最终禁用英雄:{1}
    CHAT_TIP_MATCH_SELF_REFUSE_MATCH,                       //  您已经拒绝了比赛！
    CHAT_TIP_MATCH_CREATEROOM_FAIL_REFUSE_MATCH,            //  房间创建失败，重新进入撮合！
    CHAT_TIP_MATCH_PLAYER_REPEAT_MATCH,                     //  玩家{1}已经在撮合中！
    CHAT_TIP_MATCH_YOU_REPEAT_MATCH,                        //  您已经在撮合中！
    CHAT_TIP_MATCH_GROUP_CAMP_FAIL_REMATCH,                 //  分组阵营失败，重新进入撮合！
    CHAT_TIP_MATCH_NOT_ENOUGH_PERSON_REMATCH,               //  人数不够！无法开始比赛！重新进入匹配！
    CHAT_TIP_MATCH_CANT_REQUEST,                            //  您无法进行匹配操作！
    CHAT_TIP_MATCH_CANT_KICK_OUT_GM,                        //  无法提出该用户！
	CHAT_TIP_MATCH_CALN_TEAM_INVITE_TOO_QUICK,              //  联盟邀请过于频繁，请稍后再试！

	CHAT_TIP_POINTSHOP_BEGIN = 14000,						// 商城相关
	CHAT_TIP_POINTSHOP_SERVERBUSY,							// 服务器繁忙提示
	CHAT_TIP_POINTSHOP_BUYFAILED,							// 不能购买提示
	CHAT_TIP_POINTSHOP_PRICE_DISMATCH,						// 价格不匹配提示
	CHAT_TIP_POINTSHOP_PRIZEFAILED,							// 联系客服找回提示
	CHAT_TIP_POINTSHOP_BUYSUCCEED,							// 购买成功提示
	CHAT_TIP_ACTORPRIZE_SINGLEPRIZEBROADCAST,				// 单个奖励广播提示
	CHAT_TIP_ACTORPRIZE_PRIZEGROUPBROADCAST,				// 奖励组默认广播提示
	CHAT_TIP_ACTORPRIZE_CHESTBROADCAST,						// 打开宝箱提示
	CHAT_TIP_ACTORPRIZE_ALREADYBUYBENEFITCARD,				// 购买重复类型增益卡提示
	CHAT_TIP_POINTSHOP_NOTHAVE_REFLUSHNUM,					// 刷新卡牌次数不足
	CHAT_TIP_POINTSHOP_REFLUSH_SUCCESS,						// 手动刷新卡牌成功
	CHAT_TIP_POINTSHOP_CANT_BUY_INWAR,						// 战场不能购买商品
	CHAT_TIP_POINTSHOP_NOT_IN_SELL_LIST,					// 节日赠送商品，不可购买

    CHAT_TIP_ORDER_BEGIN = 15000,						    // 排行榜相关
    CHAT_TIP_ORDER_SAVE_PERSONSET_SUCESS,                   // 个人排行榜设置成功
    CHAT_TIP_ORDER_SAVE_PERSONSET_FAILED,                   // 个人排行榜设置失败

	CHAT_TIP_ACTOR_INFO_CHANGE_BEGIN = 16000,				// 角色信息修改相关
	CHAT_TIP_ACTOR_INFO_NAME_EXISTED,						// 角色名字重复
	CHAT_TIP_ACTOR_INFO_NAME_CHANGE_FAIL,					// 角色名字修改失败
	CHAT_TIP_ACTOR_INFO_NAME_CHANGE_SUCESS,					// 角色名字修改成功

	CHAT_TIP_BENEFITCARD_BEGIN = 16500,						// 增益卡相关
	CHAT_TIP_BENEFITCARD_ADD_FAIL,							// 添加增益卡失败

	CHAT_TIP_CROWNSYSTEM_BEGIN= 16600,						// 皇冠系统相关
	CHAT_TIP_CROWNS_GEMSTONEAPPRAISA_SUCESS ,				// 充满能量
	CHAT_TIP_CROWNS_GEMSTONEAPPRAISA_FAIL,					// 充能失败
	CHAT_TIP_CROWNS_GEMSTONEAPPRAISA_BRODCAST_SUCESS,		// 充能成功频道广播    
	CHAT_TIP_CROWNS_GEMSTONESEILSUCCESS,					// 出售水晶成功  
	CHAT_TIP_CROWNS_GEMSTONE_APPRAISAL_NOT_MAIN,			// 请先放入主水晶
	CHAT_TIP_CROWNS_GEMSTONE_APPRAISAL_ASS_GRADE,			// 辅助水晶品级太高
	CHAT_TIP_CROWNS_GEMSTONE_APPRAISAL_MAIN_GRADE,			// 主水晶必须是陈旧水晶
	CHAT_TIP_CROWNS_GEMSTONEAPPRAISA_CRITPROCESS,			// {1}倍暴击，能量提升{2}
	CHAT_TIP_CROWNS_GEMSTONEAPPRAISA_PROCESS,				// 能量提升{2}
	CHAT_TIP_CROWNS_GEMSTONEINLAY_PUT,						// 右键点击宝石即可放入
	CHAT_TIP_CROWNS_GEMSTONEINLAY_NOT_SELL_OBSOLETE,		// 不能出售陈旧宝石

	CHAT_TIP_SNS_BEGIN = 16800,								// 好友系统相关
	CHAT_TIP_SNS_BLACKLIST_BY_OTHER,						// 被对方加入了黑名单
	CHAT_TIP_SNS_UPDATE_NAME_HAVE_SHEILD,					// 名称不能有屏蔽词
	CHAT_TIP_SNS_BUDDY_ONLINE_TIPS,							// 好友上线提醒
	CHAT_TIP_SNS_SEND_APPLY_SUCCESS,						// 发送好友请求成功
	CHAT_TIP_SNS_ALREADY_BUDDY_RELATION,					// 已经是好友关系
	CHAT_TIP_SNS_REFUSE_ADD_BUDDY,							// 拒绝添加好友


	CHAT_TIP_MENTORSHIP_BEGIN = 17000,						// 师徒系统相关
	CHAT_TIP_MENTORSHIP_MASTER_ISSUE_SUCESS,				// 成功发布收徒信息
	CHAT_TIP_MENTORSHIP_PRENTICE_ISSUE_SUCESS,				// 成功发布拜师信息
	CHAT_TIP_MENTORSHIP_MASTER_APPLY,						// 您对[{1}]发出了收徒申请，等待对方回复
	CHAT_TIP_MENTORSHIP_PRENTICE_APPLY,						// 您对[{1}]发出了拜师申请，等待对方回复
	CHAT_TIP_MENTORSHIP_DES_IS_PRENTICE,					// [{1}]已经成为学徒了！
	CHAT_TIP_MENTORSHIP_DES_IS_MASTER,						// [{1}]已经成为导师了！
	CHAT_TIP_MENTORSHIP_SRC_IS_MASTER,						// 您已经成为导师了！
	CHAT_TIP_MENTORSHIP_SRC_IS_PRENTICE,					// 您已经成为学徒了！
	CHAT_TIP_MENTORSHIP_DES_FORMAL_MASTER_FULL,				// [{1}]已有很多正式导师了，不能再增加了！
	CHAT_TIP_MENTORSHIP_DES_FORMAL_PRENTICE_FULL,			// [{1}]已有很多正式学徒了，不能再增加了！
	CHAT_TIP_MENTORSHIP_DES_INTERN_MASTER_FULL,				// [{1}]已有很多见习导师了，不能再增加了！
	CHAT_TIP_MENTORSHIP_DES_INTERN_PRENTICE_FULL,			// [{1}]已有很多见习学徒了，不能再增加了！
	CHAT_TIP_MENTORSHIP_APPLY_M_ANSWER_REFUSE,				// [{1}]拒绝了您的收徒请求
	CHAT_TIP_MENTORSHIP_APPLY_ANSWER_LEAVELINE,				// [{1}]已经不在线了，不能建立师徒关系了！
	CHAT_TIP_MENTORSHIP_MASTER_NEED_LEVEL,					// 您等级不够10级！
	CHAT_TIP_MENTORSHIP_PRENTICE_NEED_LEVEL,				// 您等级不够2级！
	CHAT_TIP_MENTORSHIP_FORMAL_MASTER_FULL,					// 您已有很多正式导师了，不能再增加了！
	CHAT_TIP_MENTORSHIP_FORMAL_PRENTICE_FULL,				// 您已有很多正式学徒了，不能再增加了！
	CHAT_TIP_MENTORSHIP_INTERN_MASTER_FULL,					// 您已有很多见习导师了，不能再增加了！
	CHAT_TIP_MENTORSHIP_INTERN_PRENTICE_FULL,				// 您已有很多见习学徒了，不能再增加了！
	CHAT_TIP_MENTORSHIP_APPLY_P_ANSWER_REFUSE,				// [{1}]拒绝了您的拜师请求
	CHAT_TIP_MENTORSHIP_BLUID_SUCCESS,						// 您成功与[{1}]建立师徒关系，自动将对方加为好友
	CHAT_TIP_MENTORSHIP_BLUID_SUCCESS_EX,					// 您成功与[{1}]建立师徒关系，自动将对方加为好友,选择传送
	CHAT_TIP_MENTORSHIP_APPLY_FREQUENT,						// 您发送请求过于频繁，请稍后再试
	CHAT_TIP_MENTORSHIP_APPLY_DISMISS,						// 您对[{1}]发出了解除师徒关系申请，[{2}]小时后自动解除
	CHAT_TIP_MENTORSHIP_DISMISS_SUCCESS,					// 您与[{1}]的师徒关系已解除
	CHAT_TIP_MENTORSHIP_PROMOTE_PRENTICE,					// 您与[{1}]的师徒关系已提升为正式师徒
	CHAT_TIP_MENTORSHIP_NOT_APPLY_PRENTICE,					// 您不能收自己为徒
	CHAT_TIP_MENTORSHIP_NOT_APPLY_MASTER,					// 您不能拜自己为师
	CHAT_TIP_MENTORSHIP_MASTER_LOGIN,						// 您的导师上线
	CHAT_TIP_MENTORSHIP_MASTER_LOGIN_EX,					// 您的导师上线（附加换线提醒）
	CHAT_TIP_MENTORSHIP_PRENTICE_LOGIN,						// 您的学徒上线
	CHAT_TIP_MENTORSHIP_PRENTICE_LOGIN_EX,					// 您的学徒上线（附加换线提醒）
	CHAT_TIP_MENTORSHIP_OTHER_APPLY_DISMISS,				// [{1}]您对发出了解除师徒关系申请，[{2}]小时后自动解除
	CHAT_TIP_MENTORSHIP_DISMISS_STATE,						// 师徒关系正在解除中
	CHAT_TIP_MENTORSHIP_NOT_SELECTED,						// 请选择收徒目标
	CHAT_TIP_MENTORSHIP_TUTOR_NOT_SELECTED,					// 请选拜师目标

	CHAT_TIP_LEGENDCUP_BEGIN = 17200,						// 杯赛相关
    CHAT_TIP_LEGENDCUP_CREATE_SUCCESS,                      // 创建杯赛成功
	CHAT_TIP_LEGENDCUP_NOT_ENOUGH_MONEY,					// 没有足够的金币创建比赛
	CHAT_TIP_LEGENDCUP_CANT_REQUEST_REGISTER,			    // 已经结束了申请，无法申请
	CHAT_TIP_LEGENDCUP_REPEAT_REQUEST_REGISTER,             // 无法重复申请
	CHAT_TIP_LEGENDCUP_CANT_REMOVE_TO_BLACKLIST,            // 无法移动到黑名单
	CHAT_TIP_LEGENDCUP_BLACKLIST_ISFULL,                    // 黑名单满了，可以尝试删除一些黑名单！
	CHAT_TIP_LEGENDCUP_NOT_SHAIKH_CANT_REQUEST,             // 不是战队队长无法申请
	CHAT_TIP_LEGENDCUP_CANT_EXIT_REGISTER,                  // 当前已无法主动退出比赛申请
	CHAT_TIP_LEGENDCUP_HAVE_NORIGHT_AT_THIS_OPERATE,        // 您没有该操作权限！请联系创建者！
    CHAT_TIP_LEGENDCUP_PUBLIC_COMPETITION_KINCOUNT_SATISFY, // 队伍数量不满足！无法发布比赛！
	CHAT_TIP_LEGENDCUP_CANT_JOINED_SAME_TYPE_CUP,           // 无法同时参与相同类型的比赛，请先退出其他比赛申请！
	CHAT_TIP_LEGENDCUP_PASSWORD_NOT_RIGHT,			        // 参赛密码不对！请重试！
	CHAT_TIP_LEGENDCUP_END_CANT_CHANGE_TIEM,                // 比赛已经结束了无法调整时间
	CHAT_TIP_LEGENDCUP_TOO_SMALL_TIME,                      // 结束时间小于当前时间无法调整
	CHAT_TIP_LEGENDCUP_IN_BLACK_LIST,                       // 您被被拉黑了，无法申请！
	CHAT_TIP_LEGENDCUP_EXIT_SUCCESS,                        // 退出申请比赛成功！
	CHAT_TIP_LEGENDCUP_NOT_REGISTER,                        // 您没有申请该比赛！
	CHAT_TIP_LEGENDCUP_REGISTER_FULL,                       // 申请比赛战队已经满员！
	CHAT_TIP_LEGENDCUP_REGISTER_SUCCESS,                    // 加入杯赛申请成功！
	CHAT_TIP_LEGENDCUP_KIN_LEVEL_MINI,                      // 战队等级不够！
	CHAT_TIP_LEGENDCUP_STATE_CANT_JUDGED_WAR,               // 比赛或者结束状态无法裁定！
	CHAT_TIP_LEGENDCUP_JUDGED_SUCCESS,                      // 判定成功！
	CHAT_TIP_LEGENDCUP_ROOM_CANT_JOIN,                      // 未到比赛房间进入时间！
	CHAT_TIP_LEGENDCUP_KIN_CANT_JOIN,                       // 您的战队与参赛战队不匹配！
	CHAT_TIP_LEGENDCUP_NOT_CUPMEMBER_CANT_JOIN,             // 不是参赛成员，无法参加比赛！
	CHAT_TIP_LEGENDCUP_NOT_SELECT_ANY_CUP,	                // 未选择任何杯赛信息
	CHAT_TIP_LEGENDCUP_NOT_ENTER_ANY_CUP_NAME,	            // 未输入任何杯赛名称信息
	CHAT_TIP_LEGENDCUP_NOT_ENTER_ANY_PASSWORD,	            // 未输入任何参赛密码信息
	CHAT_TIP_LEGENDCUP_NOT_SELECT_ANY_COMPETITION_NODE,	    // 未选择任何杯赛节点信息
	CHAT_TIP_LEGENDCUP_NOT_SELECT_ANY_WAR_RECORD,		    // 未选择任何杯赛比赛记录
    CHAT_TIP_LEGENDCUP_CANT_REGISTER_CLANLIMITE,            // 帮会限定比赛，你不是帮会成员无法申请！
    CHAT_TIP_LEGENDCUP_HAVE_NOT_CLAN_CANT_CREATE_CLANLIMITE,// 您没有帮会，无法创建联盟限定比赛，您可以修改联盟限定选项！
    CHAT_TIP_LEGENDCUP_CANT_REPEAT_CREATE_CUP,              // 无法重复创建杯赛
    CHAT_TIP_LEGENDCUP_ONLINE_CUP_CANT_PUBLIC,              // 线上无法发布比赛
	CHAT_TIP_LEGENDCUP_CUP_NAME_LEN_NOT_CORRECT,	        // 杯赛名称长度错误
	CHAT_TIP_LEGENDCUP_CUP_NAME_CONTAIN_SHIELD_WORD,	    // 比赛名称含有敏感字符，请修改后重试
	CHAT_TIP_LEGENDCUP_CANT_REPEATE_JUDGED,                 // 已经裁定的比赛！无法重复裁定比赛！
	CHAT_TIP_LEGENDCUP_ROOM_WAIVER_OVER_WIN,                // 对方弃权，我方获胜！
    CHAT_TIP_LEGENDCUP_CANT_LESS_THAN_ZERO,					// 出资奖励不能小于0！
    CHAT_TIP_LEGENDCUP_NOT_ENOUGH_GOLD,					    // 没有足够的出资奖励金币！
    CHAT_TIP_LEGENDCUP_NOT_ENOUGH_REGISTER_GOLD,			// 没有足够的报名金币！
	CHAT_TIP_LEGENDCUP_NOT_MATCH_CANT_RESET_WAR,			// 未进行比赛，无法重置比赛
	CHAT_TIP_LEGENDCUP_NOT_MATCH_CANT_CONFIRM_WAR,			// 未进行比赛，无法确认比赛
	CHAT_TIP_LEGENDCUP_NOT_SELECT_CANT_JUDGE_WAR,			// 请选择一个裁决方案
	CHAT_TIP_LEGENDCUP_JUDGED_FAIL,                         // 判定失败！请重新打开界面！
	CHAT_TIP_LEGENDCUP_ROOM_START_GAME_FAIL,                // 杯赛参加人数不够，暂不能开启！
	CHAT_TIP_LEGENDCUP_NOT_SELECT_ANY_CUP_TYPE,	            // 未选择任何杯赛类型
	CHAT_TIP_LEGENDCUP_PLAYER_ISNOT_CLAN_SHAIKH,			// 不是盟主无法创建联盟杯赛！
    CHAT_TIP_LEGENDCUP_CLAN_ACTIVITY_LESS_CONFIG,			// 联盟活跃度少于[{1}],无法创建杯赛！
	CHAT_TIP_LEGENDCUP_CLAN_WAIT_SHAIKH_CREATE,				// 等待盟主创建比赛
    CHAT_TIP_LEGENDCUP_NOT_IN_COFIRMREGISTER,               // 非确定参赛队伍状态，无法发布比赛！
    CHAT_TIP_LEGENDCUP_KIN_LOGIN_CHAT_INFO,	                // 战队正在参加杯赛【{1}】，{2}[LegendCup color=255+255+255 CupID={3}]点击查看[/LegendCup]！
    CHAT_TIP_LEGENDCUP_ONLINE_STATE_CANT_JUDGED_WAR,        // 重置比赛必须要该比赛在等待开始或者结束节点才能重置比赛！
    CHAT_TIP_LEGENDCUP_SELECT_RIGHT_KINID,                  // 请选择有效的队伍！
    CHAT_TIP_LEGENDCUP_REPEAT_JUDGED,                       // 相同结果无需重复判定比赛！
    CHAT_TIP_LEGENDCUP_RUNNING_CANT_SELECT_WINNER,          // 线上比赛开始无法裁定比赛！
    CHAT_TIP_LEGENDCUP_RESET_TIME_LESS_THAN_NOW,            // 选择重赛的时间不能小于当前时间
    CHAT_TIP_LEGENDCUP_RESET_TIME_LARGE_THAN_END_RESETTIME, // 选择重赛时间不能超过可重置比赛时间点
    CHAT_TIP_LEGENDCUP_RESET_TIME_NOT_CURRENT_ROUND,        // 不是当前比赛轮数，无法重置！
    CHAT_TIP_LEGENDCUP_PLEASE_INPUT_RIGHT_TIME,             // 请输出正确的时间！
    CHAT_TIP_LEGENDCUP_RESET_SINGLE_SCORE_ERROR,            // 重置比分有误，单个战队得分 【{1}】分 就能决出胜负！
    CHAT_TIP_LEGENDCUP_RESET_TOTAL_SCORE_ERROR,             // 重置比分有误，总分必须小于 【{1}】分 就能决出胜负！
    CHAT_TIP_LEGENDCUP_CANENTER_CHAT_INFO,	                // 杯赛时间到！参赛队员需前往参加杯赛【{1}】，{2}[LegendCup color=255+255+255 CupID={3}]点击查看[/LegendCup]！
    CHAT_TIP_LEGENDCUP_CONTRIBONUS_MONEY_LESS_ZERO,			// 出资奖金不能小于0！
    CHAT_TIP_LEGENDCUP_REGISTER_MONEY_LESS_ZERO,			// 报名比赛设置不能小于0！
	CHAT_TIP_LEGENDCUP_NOT_MATCH_CANT_VIEW_WAR,				// 未进行比赛，无法查看比赛

    CHAT_TIP_LEGENDCUP_END_WIN_MSGBOX_INFO,				    // 恭喜你获得胜利，杯赛已经结束，请等待发放奖励！
    CHAT_TIP_LEGENDCUP_END_FAIL_MSGBOX_INFO,				// 小输一局，再接再厉，杯赛已经结束，请等待发放奖励！
    CHAT_TIP_LEGENDCUP_NODE_WIN_MSGBOX_INFO,				// 恭喜你获得胜利，下场比赛开始时间为【{1}】分钟后，请准时参加！
    CHAT_TIP_LEGENDCUP_NODE_FAIL_MSGBOX_INFO,				// 小输一局，再接再厉，下场比赛开始时间为【{1}】分钟后，请准时参加！

    CHAT_TIP_LEGENDCUP_NODE_END_WIN_MSGBOX_INFO,			// 恭喜你获得胜利，<br>您的下一场比赛时间:【{1}月{2}日 {3}:{4}】请注意参赛时间！
    CHAT_TIP_LEGENDCUP_NODE_END_FAIL_MSGBOX_INFO,			// 小输一局，再接再厉，<br>您的下一场比赛时间:【{1}月{2}日 {3}:{4}】请注意参赛时间！
    CHAT_TIP_LEGENDCUP_OFFLINE_END_WAR_MSGBOX_INFO,			// 线下比赛完成，您可以通知裁判裁决比赛！

    CHAT_TIP_LEGENDCUP_GROUP_NODE_END_WIN_MSGBOX_INFO,			// 恭喜你获得胜利，等待小组赛结算！<br>若战队晋级下一场比赛时间:【{1}月{2}日 {3}:{4}】请注意参赛时间！
    CHAT_TIP_LEGENDCUP_GROUP_NODE_END_FAIL_MSGBOX_INFO,			// 小输一局，再接再厉，等待小组赛结算！<br>若战队晋级下一场比赛时间:【{1}月{2}日 {3}:{4}】请注意参赛时间！

	CHAT_TIP_LEGENDCUP_CREATE_REGISTMONEY_NOTENOUGH,		// 创建杯赛报名经费不等少于1000，请修改后重试
    
    CHAT_TIP_LEGENDCUP_KNOCKOUT_NODE_END_WIN_MSGBOX_INFO,			// 恭喜你获得胜利，等待淘汰赛结算！<br>若战队晋级下一场比赛时间:【{1}月{2}日 {3}:{4}】请注意参赛时间！
    CHAT_TIP_LEGENDCUP_KNOCKOUT_NODE_END_FAIL_MSGBOX_INFO,			// 小输一局，再接再厉，等待淘汰赛结算！<br>若战队晋级下一场比赛时间:【{1}月{2}日 {3}:{4}】请注意参赛时间！
    CHAT_TIP_LEGENDCUP_JUDGED_NODE_MSGBOX_INFO,			    // 您战队参加的比赛已经被举办人确定，前去查看杯赛结果！
    CHAT_TIP_LEGENDCUP_NEED_JUDGED_MSGBOX_INFO,			    // 【{1}】 与 【{2}】 的比赛已打完，请前往确定比赛
    CHAT_TIP_LEGENDCUP_CLAN_CUP_MUST_NO_PASSWORD,			    // 联盟比赛不能设置参赛密码！
    CHAT_TIP_LEGENDCUP_OVER_HIGHEST_REGISTER_MONEY,			// 申请加入比赛金币超过最高限制【{1}】，请重新修改！
    CHAT_TIP_LEGENDCUP_NOT_CREATER_CANT_END_REGISTER,         // 您不是创建者！无法结束报名！
    CHAT_TIP_LEGENDCUP_ONLINE_NOT_PRE_END_REGISTER,         // 线上杯赛无法提前结束报名！
    CHAT_TIP_LEGENDCUP_OFFLINE_NOT_IN_REGISTER_STATE,         // 不在报名阶段，无法结束报名！

	CHAT_TIP_LEGENDCUP_CREATE_INVITE_REGIST_TIP,	        // 杯赛【{1}】({2}/{3})正在火热进行中，报名费用金币{4}，{5}[createinvitelegendcup color=255+255+255 cupid={6}]点击报名[/createinvitelegendcup]

	CHAT_TIP_MAIL_BEGIN = 17400,							// 邮件系统相关
	CHAT_TIP_MAIL_RECV_PLUS_MONEY_FAIL,						// 您当前金币已达上限，邮件附件暂不能提取！
	CHAT_TIP_MAIL_RECV_PLUS_PRIZE_FAIL,						// 您当前宝箱数已达上限，邮件附件暂不能提取！
	CHAT_TIP_MAIL_RECV_PLUS_ING,							// 正在提取邮件附件，请稍后！
	CHAT_TIP_MAIL_FAST_RECV_PLUS_ING,						// 正在快速取件中，请稍后！
	CHAT_TIP_MAIL_NOT_HAVE_PLUS,							// 当前没有邮件附件可领取！
	CHAT_TIP_MAIL_CONFIRM_RECV_PLUS,						// 收取附件成功，请确认！

    CHAT_TIP_LUA_BEGIN = 18000,						        // LUA飘字相关
    CHAT_TIP_LUA_TASK_NO_DELETE,                            // 此任务不可删除
    CHAT_TIP_LUA_TASK_ACCEPT_COUNT_MAX,                     // 该时段可捕获任务已达上限
	CHAT_TIP_LUA_TASK_CHANGE_ONCE_DAILY,					// 每天仅能更换一次任务
	CHAT_TIP_LUA_TASK_CHANGE_SUCCEED,						// 更换任务成功

	CHAT_TIP_LUA_TASK_PUBLISH_COUNT_MAX,					// 该时段可发布任务已达上限
	CHAT_TIP_LUA_TASK_NO_CAN_ACCEPT,						// 该时段无可接受任务
	CHAT_TIP_LUA_TASK_SEARCHING,							// 搜索任务中，请稍后...
	CHAT_TIP_LUA_TASK_SEARCH_NONE,							// 未搜索到任务，请重试
	CHAT_TIP_LUA_TASK_NO_INPUT,								// 请输入回复文字
	CHAT_TIP_LUA_TASK_REPLY_QUICK,							// 回复过于频繁，请稍后..
	CHAT_TIP_LUA_TASK_NO_REFLUSH_COUNT,						// 刷新次数不足，请稍后再试
	CHAT_TIP_LUA_TASK_NO_MONEY,								// 点券不足，无法刷新
	CHAT_TIP_LUA_TASK_PUBLISH_SUCCESSS,						// 发布任务成功
	CHAT_TIP_LUA_TASK_CATCH_MAX,							// 该时段可接收信号已达上限
	CHAT_TIP_LUA_TASK_MAX,									// 捕获总信号已达上限
	CHAT_TIP_LUA_TASK_NO_MATCH_TASK,						// 该时段无满足要求的信号，请完成或删除自身信号后重试
	CHAT_TIP_LUA_TASK_REPLY_CONTAIN_SHIELD_WORD,			// 回复含有敏感字符，请修改后重试
	CHAT_TIP_LUA_TASK_EMOTION_CONTAIN_SHIELD_WORD,			// 心情话语含有敏感字符，请修改后重试
	CHAT_TIP_LUA_TASK_CONFIRM_CATCH_ONE_TASK,				// 请先确定选择一个任务

    CHAT_TIP_VOICE_BEGIN = 20000,
    CHAT_TIP_VOICE_ENTER_ROOM_PASSWD_ERROR,                  // 进入房间密码错误
    CHAT_TIP_VOICE_FREQUENT_SWITCH,                          //切换过于频繁 请[{1}]秒后再试
	CHAT_TIP_VOICE_ENTER_ROOM_SUCCESS,                       //进入 [{1}]频道成功

    CHAT_TIP_SYSTEM_BEGIN = 21000,
    CHAT_TIP_SYSTEM_DISCONNECT,                              // 与服务器断开连接
    CHAT_TIP_SYSTEM_SCENE_PLAYER_FULL,                        // 该场景人数满了
	
	CHAT_TIP_LIFEHERO_BEGIN = 21200,                         // 本命英雄相关
	CHAT_TIP_LIFEHERO_CHOICE_ONE,                            // 请选择一名本命英雄
	CHAT_TIP_LIFEHERO_NO_FULL_NO_REPLACE,                    // 亲，本命英雄未收集满，不能替换哦
    
    CHAT_TIP_GAMEPROMOTION_BEGIN = 21300,                   // 游戏推广界面飘字
    CHAT_TIP_GAMEPROMOTION_COPY_SUCCESS,                    // 复制成功
    CHAT_TIP_GAMEPROMOTION_OBTAIN_SUCCESS,                  // 领取成功
    CHAT_TIP_GAMEPROMOTION_OBTAIN_FAILED,                   // 领取失败
    CHAT_TIP_GAMEPROMOTION_ALREADY_OBTAIN,                  // 已经领取过
    CHAT_TIP_GAMEPROMOTION_NO_ENOUGH_MATCHNUM,              // 局数不足，无法领取

	CHAT_TIP_MIGRATE_BEGIN= 21500,							 // 迁移相关
	CHAT_TIP_MIGRATE_CANT_SWITCHSCENE,						 // 不能切换场景

	CHAT_TIP_DESIGN_BEGIN = 100000,					// 策划自定义ID起始
	CHAT_TIP_DESIGN_END = 200000,					// 策划自定义ID结束

	CHAT_TIP_MAX									// 系统消息ID最大值
};

// 系统消息类型定义
ENUM EMChatTipType
{
	CHAT_TIPTYPE_NULL = 0,		// 无定义
	CHAT_TIPTYPE_SYSTEM,		// 系统提示
	CHAT_TIPTYPE_OPERATE,		// 操作提示
	CHAT_TIPTYPE_FIGHT,			// 战斗、PK提示
	CHAT_TIPTYPE_TASK,			// 任务提示
	CHAT_TIPTYPE_ERROR,			// 错误提示
	CHAT_TIPTYPE_WARNING,		// 警告提示

	CHAT_TIPTYPE_MAX			// 最大种类数
};

// 系统消息位置定义
ENUM EMChatInfoPos
{
	CHAT_INFOPOS_CHATWINDOW		= 1<<0,		// 聊天框
	CHAT_INFOPOS_SKILL_POS1		= 1<<1,		// 技能位置1
	CHAT_INFOPOS_SKILL_POS2		= 1<<2,		// 技能位置2
	CHAT_INFOPOS_UNDER_SCREEN	= 1<<3,		// 屏幕正中下方
	CHAT_INFOPOS_MESSAGE_BOX	= 1<<4,		// 通用弹窗

	CHAT_INFOPOS_MAX			= 0x3FFF	// 最大位置数
};

#endif	// __COMMON_CHAT_MANAGED_DEF_H__