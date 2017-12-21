/*******************************************************************
** 文件名:	GameViewDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	6/21/2014
** 版  本:	1.0
** 描  述:	

			定义显示层和逻辑层通讯的游戏全局消息
** 约  定:   
            1. _RESULT后缀: 逻辑层返回的CMD以此结尾，此时int参数表示返回的错误码，str表示提示信息(一般是error!=0时有效)
********************************************************************/

#pragma once

#include "GameDef.h"
#include "ManagedCode.h"
#include "Clan_ManagedDef.h"
#include "War_ManagedDef.h"
#include "Mentorship_ManagedDef.h"
#include "SNS_ManagedDef.h"
#include "NameLenDef.h"

namespace GameLogic
{
	// 托管代码和非托管代码共用一个定义 
#ifdef _MANAGED
	public enum class GameLogicDef
#else
	enum
#endif
	{
        /*****************************************************************************
        * 显示层调用逻辑层
        ******************************************************************************/
        // 通知逻辑层登陆
        GVIEWCMD_USER_LOGIN = GAME_VIEW_CMD_OFFSET,

		// 显示层调用每帧的update,用来做线程同步
		GVIEWCMD_FRAME_UPDATE,

        GVIEWCMD_PROGRESSING,

		// 通告逻辑层进入游戏
		//GVIEWCMD_USER_ENTER_GAME,

		// 通知逻辑层大退
		GVIEWCMD_USER_EXITGAME,

		// 显示进度条
		GVIEWCMD_PLAY_PROGRESS_BAR,

		// 清除进度条
		GVIEWCMD_CLEAR_PROGRESS_BAR,

        // 创建角色
        GVIEWCMD_USER_CREATE_PLAYER,
        // 创建角色扩展信息
        GVIEWCMD_USER_CREATE_PLAYER_EXTEND,

        // 删除角色
        GVIEWCMD_USER_DELETE_PLAYER,

        // 通知逻辑层载入游戏完成
        //GVIEWCMD_LOAD_GAME_FINISHED,

		// 通知逻辑层切换英雄信息
		GVIEWCMD_USER_SELECT_HERO,

		// 准备进入大厅 view2c++ c++2view
		GVIEWCMD_MATCH_ENTER_HALL,

		// 离开大厅 view2c++ c++2view
		GVIEWCMD_MATCH_EXIT_HALL,

		// 离开大厅进入选择角色界面 view2c++ c++2view
		GVIEWCMD_MATCH_EXIT_HALL_TO_CHOOSE,

		// 比赛大厅 view2c++ 自己要离开房间
		// 比赛大厅 c++2view 有玩家离开房间
		GVIEWCMD_MATCH_ROOM_PLAYER_EXIT,

		// 比赛大厅 view2c++ 请求打开创建房间界面
		// 比赛大厅 c++2view 打开创建房间界面
		GVIEWCMD_MATCH_OPEN_CREATE_ROOM,

		// 比赛大厅 开始撮合
		GVIEWCMD_MATCH_BEGIN_MATCH,

        // 取消撮合命令
        GVIEWCMD_MATCH_CANCEL_MATCH,

        // 创建引导房间
        GVIEWCMD_CREATE_GUIDE_ROOM,

		// 比赛大厅 view2c++ 创建房间
		GVIEWCMD_MATCH_CREATE_ROOM,

        // 引导ID记录
        GVIEWCMD_MATCH_GUIDE_INFO,

		// 比赛大厅 view2c++ 请求进入房间 
		GVIEWCMD_MATCH_ENTER_ROOM,

		// 比赛大厅 view2c++ 点击开始游戏
		GVIEWCMD_MATCH_START_GAME,

		// 比赛大厅 c++2view 加载场景 view2c++加载完成
		// 比赛大厅 view2c++ 场景加载完成
		GVIEWCMD_MATCH_LOAD_SCENE_PROGREESE,

        // 比赛大厅 c++2view 加载场景进度
        // 比赛大厅 view2c++ 加载场景进度
        GVIEWCMD_MATCH_LOAD_SCENE_PROGREESE_RATE,

		// 比赛大厅 c++2view 加载场景 view2c++加载完成
		// 比赛大厅 view2c++ 场景加载完成
		GVIEWCMD_MATCH_LOAD_SCENE,

		// 比赛大厅 view2c++ 选英雄
		// 比赛大厅 c++2view 房间英雄改变
		GVIEWCMD_MATCH_ROOM_HERO_CHANGE,

        // 比赛大厅 view2c++ 房间选择英雄
        GVIEWCMD_MATCH_ROOM_MASTER_COVER_COMPUTER,

        // 比赛大厅 view2c++ 预选英雄改变
        GVIEWCMD_MATCH_ROOM_PRE_HERO_CHANGE_LIST,

        // 比赛大厅 view2c++ 优先需要选的英雄
        // 比赛大厅 c++2view 优先需要预选的英雄
        GVIEWCMD_MATCH_ROOM_PRE_SELECT_HERO,

		// 比赛大厅 view2c++ 选英雄皮肤
		GVIEWCMD_MATCH_ROOM_SELECT_HERO_SKIN,

        // 比赛大厅 view2c++ ban选英雄
        GVIEWCMD_MATCH_ROOM_BAN_HERO,

		// 比赛大厅 view2c++ 换到空位置
		// 比赛大厅 c++2view 位置换到空位置
		GVIEWCMD_MATCH_ROOM_HERO_POS_CHANGE,

		//比赛大厅 view2c++  选英雄前显示英雄信息
		//比赛大厅 C++2view  将英雄信息丢到显示层显示
		GVIEWCMD_MATCH_ROOM_HERO_INFO,

		//比赛大厅 C++2view  将英雄的技能信息丢到显示层显示
		GVIEWCMD_MATCH_ROOM_HERO_SKILL,

		// view2c++ 加载大厅完成，可以去选角色界面了
		GVIEWCMD_LOGIN_LOAD_HALL,

		// c++2view 战斗结束，准备返回房间
		// view2c++ 可以返回房间
		GVIEWCMD_MATCH_FROM_GAME_TO_ROOM,

		//view2c++ 哪个位置的玩家要被踢出
		//C++2 view 玩家被踢出后，位置清空
		GVIEWCM_MATCH_MASTER_KICK_ROLE,

		//view2C++ 点击房间列表，丢房间ID
		//C++2view 显示房间地图信息等等
		GVIEWCM_MATCH_HALL_ROOM_DATA,

		//view2C++ 请求从比赛大厅返回选角色界面
		//C++2view 允许从比赛大厅返回选角色界面
		GVIEWCM_MATCH_TO_CHOOSE_VOCATION,

		//view2C++ 比赛结束，显示结果UI 点击返回房间
		GVIEWCM_MATCH_FIGHT_END_RETURN_ROOM,

        //选择英雄时鼠标停留的英雄区域（鼠标未点击）
        GVIEWCM_MATCH_MOUSE_LEFT_HERO_AREA,

		// view2c++ 直接进战场 
		// c++2view 直接进战场
		GVIEWCMD_ENTER_TEST_MAP,

		// 通知逻辑层退出战场
		GVIEWCMD_USER_EXIT_WAR,

        // 战场结束返回静态地图
        GVIEWCMD_USER_BACK_GAME,

		// c++2view 玩家退出游戏，准备返回大厅
		// view2c++ 可以返回房间
		GVIEWCMD_MATCH_FROM_GAME_TO_HALL,
		
		// 发送游戏状态
		GVIEWCMD_GAME_STAGE_STATE,

		// 没有英雄时直接跳转到创建角色界面
		GVIEWCMD_JUDGE_CREATE,

		// 聊天用消息
		// 发送聊天消息
		GVIEWCMD_SEND_CHAT_MESSAGE,
		// 显示聊天消息
		GVIEWCMD_SHOW_CHAT_MESSAGE,
		// 显示私聊消息
		GVIEWCMD_SHOW_PRIVATE_CHAT_MESSAGE,
		// 显示系统消息
		GVIEWCMD_SHOW_SYSTEM_MESSAGE,
		// 显示喇叭
		GVIEWCMD_SHOW_LOUDSPEAKER,
		// 设置聊天分频
		GVIEWCMD_CHAT_SET_CHANNELMASK,
		// 聊天用消息结束

		// 发送战场类型id
		GVIEWCMD_PLAY_SEND_WAR_TYPEID,

		// 房间规则：禁用英雄
		GVIEWCMD_MATCH_ROOM_RULE_BAN_HERO,

		// 房间规则：选择英雄
		GVIEWCMD_MATCH_ROOM_RULE_SELECT_HERO,

        // 全局英雄禁用列表
        GVIEWCMD_MATCH_ROOM_GLOBLE_BAN_HERO_LIST,

		// 禁用英雄列表
		GVIEWCMD_MATCH_ROOM_RULE_BAN_HERO_LIST,

		// 禁用英雄
		GVIEWCMD_MATCH_ROOM_HERO_CAN_SELECT,

        // 发送当前玩家的默认召唤师技能
        GVIEWCMD_MATCH_SUMMONER_DEFAULT_SKILL,

        // 返回给显示层所有的召唤师技能
        GVIEWCMD_MATCH_SUMMONER_ALL_SKILL,

        // 打开天梯排行榜
        GVIEWCMD_OPEN_ORDER_HIGH_LADDER,

        // 打开战队排行榜
        GVIEWCMD_OPEN_ORDER_KIN,

        // 打开帮会排行榜
        GVIEWCMD_OPEN_ORDER_CLAN,

        // 打开击杀排行榜
        GVIEWCMD_OPEN_ORDER_PLAYERKILL,

        // 打开捐赠排行榜
        GVIEWCMD_OPEN_ORDER_DONATE,

        // 打开英雄排行榜
        GVIEWCMD_OPEN_ORDER_HERO,

        // 打开英雄类型排行榜
        GVIEWCMD_OPEN_ORDER_HEROTYPE,

		// view2c++ 打开本周导师排行榜
		GVIEWCMD_OPEN_ORDER_CURWEEK_MASTER,

		// view2c++ 打开本周学徒排行榜
		GVIEWCMD_OPEN_ORDER_CURWEEK_PRENTICE,

		// view2c++ 打开上周导师排行榜
		GVIEWCMD_OPEN_ORDER_LASTWEEK_MASTER,

		// view2c++ 打开上周学徒排行榜
		GVIEWCMD_OPEN_ORDER_LASTWEEK_PRENTICE,

		// 打开上周学徒排行榜
		GVIEWCMD_OPEN_ORDER_GEMSTONE,

		// 打开皮肤排行榜
		GVIEWCMD_OPEN_ORDER_SKIN,

		// 打开英雄数量排行榜
		GVIEWCMD_OPEN_ORDER_HEROCOUNT,

        // 打开英雄王者榜
		GVIEWCMD_OPEN_KING_OF_HERO_ORDER,

        // 添加假人
        GVIEWCMD_MATCH_COMPUTERPLAYER_ADD,

        // 移除假人
        GVIEWCMD_MATCH_COMPUTERPLAYER_REMOVE,

        // 假人选择英雄
        GVIEWCMD_MATCH_COMPUTERPLAYER_HEROCHANGE,

        // 假人英雄AI改变
        GVIEWCMD_MATCH_COMPUTERPLAYER_HEROAI_CHANGE,

        // 设置个人排行榜显示
        GVIEWCMD_SET_ORDER_PERSONINFO,

        // 画出动态障碍
        GVIEWCMD_DRAW_DYNAMICOBSTACLE,

        // 关闭战队信息
        GVIEWCMD_SOCIAL_CLOSE,

        // 创建战队
        GVIEWCMD_KIN_CREATE,

        // 战队改名 
        GVIEWCMD_KIN_RENAME,

        // view2c++ 申请战队列表 
        // c++2view 申请战队列表
        GVIEWCMD_KIN_REQUESTKINLIST,

        // 邀请答复 
        GVIEWCMD_KIN_ANSWER_INVITE,

        // 邀请加入战队 
        GVIEWCMD_KIN_INVITE,

        // 退出战队 
        GVIEWCMD_KIN_QUIT,

        // 开除成员 
        GVIEWCMD_KIN_KICK,

        // 接收成员
        GVIEWCMD_KIN_ADMIT,

        // 申请加入战队
        GVIEWCMD_KIN_APPLY,

        // 打开战队信息
        GVIEWCMD_KIN_OPEN,

        // 战队事件列表
        GVIEWCMD_REQUEST_KIN_EVENTLIST,

        // 解散战队 c++2view view2c++
        GVIEWCMD_KIN_DISMISS,

        // c++2view 请求申请加入战队人员列表
        // view2c++ 请求申请加入战队人员列表
        GVIEWCMD_KIN_APPLY_LIST,

        // c++2view 自动接收设置
        // view2c++ 自动接收设置
        GVIEWCMD_KIN_AUTOACCEPT_SAVE,

        // c++2view 自动接收设置
        // view2c++ 自动接收请求
        GVIEWCMD_KIN_AUTOACCEPT_REQUEST,

        // view2c++ 移除申请
        // c++2view 移除申请
        GVIEWCMD_KIN_REMOVEAPPLY,

        // view2c++ 移除申请
        // c++2view 移除申请
        GVIEWCMD_KIN_ADDAPPLY,

        // view2c++ 移除成员
        // c++2view 移除成员
        GVIEWCMD_KIN_REMOVEMEMBER,

        // view2c++ 添加成员
        // c++2view 添加成员
        GVIEWCMD_KIN_ADDMEMBER,

        // view2c++ 更新成员
        // c++2view 更新成员
        GVIEWCMD_KIN_UPDATEMEMBER,
        
        // view2c++ 编辑公告
        // c++2view 编辑公告
        GVIEWCMD_KIN_MOTIFYNOTICE,

        // view2c++ 更新自己战队信息
        // c++2view 更新自己战队信息
        GVIEWCMD_KIN_UPDATESELF,

        // 设置家族职位
        GVIEWCMD_KIN_APPOINT_IDENTITY,

        // view2c++ 请求帮会列表 
        // c++2view 请求帮会列表
        GVIEWCMD_CLAN_REQUESTCLANLIST,

        // view2c++ 创建战队成功 
        // c++2view 创建战队成功
        GVIEWCMD_KIN_CREATE_SUCCESS,

        // view2c++ 加入战队成功 
        GVIEWCMD_KIN_JOIN_SUCCESS,

        // 响应邀请成功
        GVIEWCMD_KIN_ANSWERINVITE_SUCCESS,

        // 战队字符串属性更新
        GVIEWCMD_KIN_STRING_PROP_UPDATE,

        // 创建帮会
        GVIEWCMD_CLAN_CREATE,

        // 加入帮会
        GVIEWCMD_CLAN_APPLY,

        // 打开帮会界面
        GVIEWCMD_CLAN_REQUESTCLANBASEINFO,

        // 退出帮会
        GVIEWCMD_CLAN_QUIT,

        // 解散帮会
		GVIEWCMD_CLAN_DISMISS,

        // 取消解散帮会
        GVIEWCMD_CLAN_CANCEL_DISMISS,

		// 帮会任命指挥官
		GVIEWCMD_CLAN_SETLEADER,

		// 帮会踢出战队
		GVIEWCMD_CLAN_KICKKIN,

        // view2c++ 请求帮会申请人员列表 
        // c++2view 请求帮会申请人员列表
        GVIEWCMD_CLAN_APPLY_LIST,

        // 接收成员
        GVIEWCMD_CLAN_ADMIT,

        // view2c++ 帮会移除成员
        // c++2view 帮会移除成员
        GVIEWCMD_CLAN_REMOVEMEMBER,

        // view2c++ 帮会添加成员
        // c++2view 帮会添加成员
        GVIEWCMD_CLAN_ADDMEMBER,

        // c++2view 帮会添加成员
        GVIEWCMD_CLAN_UPDATECLIENT,

        // view2c++ 请求卡牌列表
        // c++2view 请求卡牌列表
        GVIEWCMD_CLAN_REQUEST_CARDLIST,

        // view2c++ 请求支援
        // c++2view 请求支援
        GVIEWCMD_CLAN_REQUEST_CARD,

        // view2c++ 捐赠卡牌
        GVIEWCMD_CLAN_DONATE_CARD,

        // view2c++ 移除请求卡牌
        GVIEWCMD_CLAN_REMOVE_REQUEST_CARD,

        // view2c++ 帮会事件列表
        // c++2view 帮会事件列表
        GVIEWCMD_CLAN_EVENT_LIST,

		// view2c++ 帮会修改公告
		GVIEWCMD_CLAN_MODIFY_NOTICE,

		// view2c++ 帮会设置自动接受
		GVIEWCMD_CLAN_SET_AUTOACCEPT,

        // view2c++ 邀请组队
        GVIEWCMD_TEAM_INVITE,

        // view2c++ 邀请组队回应
        GVIEWCMD_TEAM_INVITE_RESPON,

        // view2c++ 申请组队
        GVIEWCMD_TEAM_REQUEST,

        // view2c++ 申请组队回应
        GVIEWCMD_TEAM_REQUEST_RESPON,

		// view2c++ 提升队长
		GVIEWCMD_TEAM_APPOINTCAPTAIN,

		// view2c++ 踢出目标
		GVIEWCMD_TEAM_KICKOUTMEMBER,

		// view2c++ 离开队伍
		GVIEWCMD_TEAM_LEAVETEAM,

		// view2c++ 解散队伍
		GVIEWCMD_TEAM_DISBANDTEAM,

        // 随机名字
        GVIEWCMD_USER_RANDOM_PLAYER_NAME,

        // 联盟请求组队
        GVIEWCMD_TEAM_CLAN_INVITE_JOIN_TEAM,

		// view2c++ 按键信息
		GVIEWCMD_INPUTMANAGER_KEYSTATE,

		// view2c++ 防沉迷注册
		GVIEWCMD_ANTIADDICT_REGIST,
		
		// view2c++ 请求个人数据信息 
		// c++2view 请求个人数据信息 
		GVIEWCMD_MENTORSHIP_REQUESTPERSONDATA,

		// view2c++ 请求导师的学徒数据信息 
		GVIEWCMD_MENTORSHIP_REQUESTPRENTICELIST,

		// view2c++ 请求发布列表数据信息 
		// c++2view 请求发布列表数据信息 
		GVIEWCMD_MENTORSHIP_REQUESTISSUELIST,

		// view2c++ 请求发布信息 
		GVIEWCMD_MENTORSHIP_REQUESTISSUEINFO,

		// view2c++ 请求拜师或收徒信息 
		GVIEWCMD_MENTORSHIP_REQUESTAPPLYSHIP,

		// view2c++ 请求拜师或收徒应答信息 
		// c++2view 请求拜师或收徒应答信息  
		GVIEWCMD_MENTORSHIP_REQUESTAPPLYANSWER,

		// view2c++ 请求解散师徒信息 
		// c++2view 请求解散师徒信息 
		GVIEWCMD_MENTORSHIP_REQUESTDISMISS,

		// view2c++ 请求提示学徒信息 
		GVIEWCMD_MENTORSHIP_REQUESTPROMOTE,

		// view2c++ 请求自动发布信息 
		// c++2view 请求自动发布信息 
		GVIEWCMD_MENTORSHIP_REQUESTISSUEAUTOSET,

		// view2c++ 请求领取首胜奖励
		GVIEWCMD_MENTORSHIP_REQUESTFIRSTAWARD,

        ///////////////////view2c++///////语音////////////////////
        // 开关麦
        GVIEWCMD_VOICE_REQUEST_OPEN_MIC,
        // 开关听筒
        GVIEWCMD_VOICE_REQUEST_CHANGE_MUTE_STATE,
        // 设置房间麦序模式(主席麦 麦序 自由麦)
        GVIEWCMD_VOICE_REQUEST_CHANGE_MIC_ORDER_MODE,
        // 设置房间状态标记(麦序模式下可抢麦 麦序模式下控麦 自由麦下只能按键发音 有密码)
        GVIEWCMD_VOICE_REQUEST_SET_ROOM_FLAG,
        // 请求进入语音房间
        GVIEWCMD_VOICE_REQUEST_ENTER_ROOM,
        // 修改房间密码
        GVIEWCMD_VOICE_REQUEST_LOCK_ROOM,
        // 抢麦/下麦
        GVIEWCMD_VOICE_REQUEST_MIC_ORDER,
        // 设置麦序时间
        GVIEWCMD_VOICE_REQUEST_SET_MIC_ORDER_TIME,
        // 添加马甲
        GVIEWCMD_VOICE_REQUEST_ADD_VEST,
        /////////////////////////语音////////////////////

		///////////////////view2c++///////杯赛////////////////////
		// 请求战队荣誉信息
		GVIEWCMD_KIN_REQUESTKINGLORY,
		// 请求战队总历史战绩信息
		GVIEWCMD_KIN_REQ_WARRECORD_TOTAL,
	
		// 请求所有杯赛列表
		GVIEWCMD_LEGENDCUP_REQUEST_CUP_LIST,
		// 请求创建杯赛
		GVIEWCMD_LEGENDCUP_REQUEST_CUP_CREATE,
		// 请求查看某杯赛
		GVIEWCMD_LEGENDCUP_VIEW_SINGLE_CUP,
		// 请求发布某杯赛 
		GVIEWCMD_LEGENDCUP_RELEASE_SINGLE_CUP,

		// 请求加入某杯赛
		GVIEWCMD_LEGENDCUP_ADD_REGIST_KINLIST,
		// 请求退出某杯赛
		GVIEWCMD_LEGENDCUP_QUIT_REGIST_KINLIST,
		// 请求某战队成员列表
		GVIEWCMD_LEGENDCUP_REQUEST_REGIST_MEMBERLIST,
		// 请求某杯赛黑名单列表
		GVIEWCMD_LEGENDCUP_REQUEST_REGIST_BLACKLIST,
		// 请求加入某杯赛黑名单
		GVIEWCMD_LEGENDCUP_ADD_REGIST_BLACKLIST,
		// 请求移除某杯赛黑名单
		GVIEWCMD_LEGENDCUP_DELETE_REGIST_BLACKLIST,

		// 请求查看某节点比赛 
		GVIEWCMD_LEGENDCUP_VIEW_COMPETITION_NODE,
		// 请求加入某节点比赛 
		GVIEWCMD_LEGENDCUP_ENTER_COMPETITION_NODE,
		// 请求裁决某节点比赛
		GVIEWCMD_LEGENDCUP_JUDGE_COMPETITION_NODE,

		// 杯赛家族信息界面打开
		GVIEWCMD_KIN_LEGENDCUPKININFO_OPEN,
		// 杯赛家族荣誉界面打开
		GVIEWCMD_KIN_LEGENDCUPKINGLORY_OPEN,
		// 请求创建类型
		GVIEWCMD_LEGENDCUP_CREATE_TYPE_REQ,
		// 杯赛界面退出观察
		GVIEWCMD_LEGENDCUP_QUITOBSERVE,

        // 预先结束线下报名阶段
        GVIEWCMD_LEGENDCUP_OFFLINE_PRE_END_REGISTER,

		/////////////////////////杯赛////////////////////


		///////////////////////SNS view2c++ 开始///////////////////////
		// 登陆SNSServer
		GVIEWCMD_SNS_LOGIN,
		// 更新好友操作
		GVIEWCMD_SNS_UPDATE_BUDDY,
		// 请求推荐好友
		GVIEWCMD_SNS_REQ_RECOMMAND_BUDDY,
		// 修改好友备注
		GVIEWCMD_SNS_UPDATE_REMARK,
		// 回应添加好友请求
		GVIEWCMD_SNS_RSP_ADD_BUDDY,
		// 更新好友分组
		GVIEWCMD_SNS_UPDATE_BUDDYGROUP,
		// 发送聊听信息
		GVIEWCMD_SNS_SEND_MSG_DATA,
		// 请求一个好友信息
		GVIEWCMD_SNS_REQ_BUDDY_INFO,
		// 请求好友的关系信息
		GVIEWCMD_SNS_REQ_BUDDY_RELATION,
		// 请求通过名字添加好友
		GVIEWCMD_SNS_REQ_ADD_BUDDY_BY_NAME,
		// 请求用户信息
		GVIEWCMD_SNS_REQ_USER_INFO,
		// 请求修改权限
		GVIEWCMD_SNS_REQ_CHANGE_PERMISSION,
		// 请求修改心情
		GVIEWCMD_SNS_REQ_CHANGE_MOOD,
		// 改变好友分组
		GVIEWCMD_SNS_REQ_CHANGE_BUDDYGROUP,
		// 更新在线状态
		GVIEWCMD_SNS_REQ_UPDATE_BUDDYSTATUS,
		// 邀请好友一起游戏
		GVIEWCMD_SNS_REQ_INVITE_PLAYGAME,
		// 邀请好友一起组队匹配
		GVIEWCMD_SNS_REQ_INVITE_MATCH_TEAM,
		//////////////////////SNS  view2c++ 结束///////////////////////////

		///////////////////////MAIL view2c++ 开始///////////////////////
		// view2c++ 请求邮件基础信息列表
		// c++2view 请求邮件基础信息列表
		GVIEWCMD_MAIL_REQUESTBASEDATALIST,
		// view2c++ 请求邮件详细信息
		GVIEWCMD_MAIL_REQUESTDETAILDATA,
		// view2c++ 请求快速取件
		// c++2view 请求快速取件
		GVIEWCMD_MAIL_REQUESTFASTRECVPLUS,

		///////////////////////MAIL view2c++ 结束///////////////////////

		// view2c++ 客户端返回推荐排位结果
		GVIEWCMD_MATCH_RECOMMEND_RANK_RESULT,

        /*****************************************************************************
        * 逻辑层调用显示层
        ******************************************************************************/
        // 通知显示层载入登陆界面
        GVIEWCMD_LOAD_LOGIN		=			(GAME_VIEW_CMD_OFFSET+800),
        // 返回登录状态
        GVIEWCMD_RETURN_LOGIN,
        // 当前登陆状态 1..正在连接 2..连接成功 -1..连接断开 3..验证密码成功 4..验证密码失败
        GVIEWCMD_LOGIN_CONNECTING,

        // 通告登陆结果
        GVIEWCMD_USER_LOGIN_RESULT,

        // 通告创建角色结果
        GVIEWCMD_USER_CREATE_PLAYER_RESULT,

        // 通告删除角色结果
        GVIEWCMD_USER_DELETE_PLAYER_RESULT,

        // 通知显示层进入游戏状态
        GVIEWCMD_LOAD_GAME,

        // 比赛大厅 c++2view 创建房间界面 模式
        GVIEWCMD_MATCH_OPEN_CREATE_ROOM_MODES,

        // 比赛大厅 c++2view 自己离开房间进入大厅
        GVIEWCMD_MATCH_EXIT_ROOM_ENTER_HALL,

        // 比赛大厅 c++2view 创建房间成功，进入自己创建的房间 
        // ！！！ 废除
        GVIEWCMD_MATCH_CREATE_ROOM_ENTER,
		// 比赛大厅 c++2view 删除所有房间 
		GVIEWCMD_MATCH_HALL_REMOVE_ALL_ROOM,
        // 比赛大厅 c++2view 给大厅添加房间 
        GVIEWCMD_MATCH_HALL_ADD_ROOM,
        // 比赛大厅 c++2view 大厅移除房间 
        GVIEWCMD_MATCH_HALL_REMOVE_ROOM,

        // 比赛大厅 c++2view 进入者添加房间其他角色数据
        GVIEWCMD_MATCH_ENTER_ROOM_OTHER,
        // 比赛大厅 c++2view 进入者添加房间自己角色数据
        GVIEWCMD_MATCH_ENTER_ROOM_SELF,
        // 比赛大厅 c++2view 房间其他角色添加进入者角色数据
        GVIEWCMD_MATCH_ENTER_ROOM_DATA,

        // 比赛大厅 c++2view 大厅房间数据变化，需要刷新
        GVIEWCMD_MATCH_HALL_ROOM_FRESH,

        // 比赛大厅 c++2view 进入场景 
        GVIEWCMD_MATCH_ENTER_SCENE,

        // 比赛大厅 c++2view 房主改变
        GVIEWCMD_MATCH_ROOM_MASTER_CHANGE,

        // c++2view 比赛结束返回房间场景
        GVIEWCM_MATCH_OVER_ENTER_ROOM,

        // c++2view 有人离开比赛房间，房间回到准备状态
        GVIEWCM_MATCH_PLAYER_EXIT_ROOM_STATE_2_WAIT,

        // c++2view 打开LOG文件
        GVIEWCMD_OPEN_LOG_FILE,

        //C++2View 显示层初始化选择英雄界面英雄插槽
        GVIEWCMD_MATCH_SELECT_HERO_SLOT,

        //C++2View 显示层初始化房间英雄插槽
        GVIEWCMD_MATCH_ROOM_HERO_SLOT,

        // view2c++ 确认所选英雄
        // c++2view 哪个位置确认所选英雄
        GVIEWCMD_MATCH_ROOM_CONFIRM_HERO,

        // c++2view 所有玩家都确认了英雄
        GVIEWCMD_MATCH_ROOM_ALL_CONFIRM_HERO,

        // 比赛大厅 c++2view 开始选英雄倒计时
        GVIEWCMD_MATCH_START_SELECT_HERO,

        // 比赛大厅 c++2view 结束选英雄倒计时
        GVIEWCMD_MATCH_READY_TO_FIGHT,

        // C++2View 加载时显示的信息
        GVIEWCMD_MATCH_LOAD_SCENE_PLAYERINFO,

        //比赛大厅 view2c++  确定参赛
        GVIEWCMD_MATCH_CONFIRM_ACCEPTCHECK,

        //比赛大厅 view2c++  拒绝参赛
        GVIEWCMD_MATCH_REFUSE_ACCEPTCHECK,

        // 自定义邀请其他玩家比赛
        GVIEWCMD_MATCH_CUSTOMER_INVITE_REQUEST,

        // 自定义邀请比赛回应
        GVIEWCMD_MATCH_CUSTOMER_INVITE_RESPON,

        // 调用逻辑层寻路
        GVIEWCMD_MAP_FINDPATH,

		// 房间状态
		GVIEWCMD_MATCH_ROOM_STATE,

		// 开启房间规则
		GVIEWCMD_MATCH_START_ROOM_RULE,

        // 将选择后的召唤师技能返回给逻辑层二(单纯的只是给matchRoom消息，防止覆盖)
        GVIEWCMD_SUMMONER_SKILL_RETURNEX,

        // 通知显示层显示消息框
        GVIEWCMD_SHOW_MESSAGEBOX,

        // 显示层通知逻辑层强制登录
        GVIEWCMD_FORCE_LOGIN,

        // 排行榜相关
        GVIEWCMD_MATCHTYPE_ORDER_RANK_INFO,

        // 王者排位榜相关
        GVIEWCMD_RANK_KING_ORDER_INFO,

		//战队榜
        GVIEWCMD_KIN_ORDER_RANK_INFO,
		
		// 段位榜
        GVIEWCMD_CLAN_ORDER_RANK_INFO,

		// 击杀榜
        GVIEWCMD_KILL_ORDER_RANK_INFO,

		// 捐赠榜
        GVIEWCMD_DONATE_ORDER_RANK_INFO,

		// 英雄养成榜
        GVIEWCMD_HERO_ORDER_RANK_INFO,

		//英雄类型榜
        GVIEWCMD_HEROTYPE_ORDER_RANK_INFO,

		// 本周导师榜
		GVIEWCMD_CURWEEK_MASTER_ORDER_RANK_INFO,

		// 本周学徒榜
		GVIEWCMD_CURWEEK_PRENTICE_ORDER_RANK_INFO,

		// 上周导师榜
		GVIEWCMD_LASTWEEK_MASTER_ORDER_RANK_INFO,

		// 上周学徒榜
		GVIEWCMD_LASTWEEK_PRENTICE_ORDER_RANK_INFO,

		// 宝石榜
		GVIEWCMD_GEMSTONE_ORDER_RANK_INFO,

		// 皮肤榜
		GVIEWCMD_SKIN_ORDER_RANK_INFO,

		// 英雄数量榜
		GVIEWCMD_HEROCOUNT_ORDER_RANK_INFO,

        // 英雄王者榜
        GVIEWCMD_KING_OF_HERO_RANK_INFO,

        //个人排行设置
        GVIEWCMD_PERSON_ORDER_SET_INFO,

        // 在房间内时广播召唤师技能信息
        GVIEWCMD_SUMMONER_BROAD_SKILL,

        // 当有人登陆时设置召唤师技能
        GVIEWCMD_SUMMONER_SET_SKILLPIC_LOGIN,

        // 返回有障碍物不显示在小地图上的敌人id
        GVIEWCMD_ENEMY_ID_BARRIER,

        // 假人英雄改变
        GVIEWCMD_MATCH_ROOM_COMPUTER_HERO_CHANGE,

		// 假人英雄是否能选择
        GVIEWCMD_MATCH_ROOM_COMPUTER_HERO_CAN_SELECT,

        // 禁用英雄列表
        GVIEWCMD_MATCH_ROOM_COMPUTER_RULE_BAN_HERO_LIST,

        // 可以禁用英雄标示
        GVIEWCMD_MATCH_CAN_BAN_HERO_FLAG,

        // ban选时间
        GVIEWCMD_MATCH_CAN_BAN_HERO_COUNT_DOWN,

        // 禁用英雄信息
        GVIEWCMD_MATCH_ROOM_SINGLE_BAN_HERO,

        // 全部禁用英雄信息
        GVIEWCMD_MATCH_ROOM_ALL_BAN_HERO,

        // 杯赛直接胜利客户端消息显示
        GVIEWCMD_MATCH_ROOM_WIN_LEGENDCUP_MSGINFO,

        // c++2view 战队成员列表
        GVIEWCMD_KIN_REQUESTMEMBERLIST,

        // c++2view 战队基础数据
        GVIEWCMD_KIN_REQUESTBASEDATA,

        // c++2view 战队引导信息
        GVIEWCMD_KIN_GUIDE_INFO,

        // 要预加载皮肤ID列表
        GVIEWCMD_PRELOAD_SKIN_LIST,

        // c++2view 帮会基础数据
        GVIEWCMD_CLAN_BASEDATA,

        // c++2view 设置帮会基础数据
        GVIEWCMD_CLAN_SET_BASEDATA,

        // c++2view 移除帮会申请
        GVIEWCMD_CLAN_REMOVEAPPLY,

        // c++2view 增加帮会申请
        GVIEWCMD_CLAN_ADDAPPLY,

        // c++2view 创建帮会成功
        GVIEWCMD_CLAN_CREATE_SUCCEED,

        // c++2view 离开帮会通知
        GVIEWCMD_CLAN_ONQUIT,

        // c++2view 请求捐赠卡牌记录
		GVIEWCMD_CLAN_REQUEST_DONATELOG,

		// c++2view 更新捐赠卡牌记录
		GVIEWCMD_CLAN_UPDATE_DONATELOG,

		// c++2view 更新捐赠卡牌提示
		GVIEWCMD_CLAN_ADD_REQUESTCARD_TIP,

		// c++2view 更新捐卡信息
		GVIEWCMD_CLAN_REQUESTCARD_INFO,

        // c++2view 更新捐卡信息
        GVIEWCMD_CLAN_CREATE_LEGENDCUP_DIDA,

		// c++2view 申请返回
		GVIEWCMD_CLAN_APPLY_RET,

		// c++2view 帮会自动接受标示
		GVIEWCMD_CLAN_SET_AUTOACCEPT_FLAG,
        // c++2view 帮会状态改变通知
        GVIEWCMD_CLAN_STATE_NOTICE,

        // c++2view 响应邀请组队消息命令
        GVIEWCMD_TEAM_SUBINVITED,
         // c++2view 响应请求组队消息命令
        GVIEWCMD_TEAM_SUBREQUEST,
		//  c++2view 响应更新队友消息命令
		GVIEWCMD_TEAM_UPDATETEAMMEMBER,
		//  c++2view 响应改变队长消息命令
		GVIEWCMD_TEAM_CHANGECAPTAIN,
		//  c++2view 响应踢出队员消息命令
		GVIEWCMD_TEAM_LEAVEMEMBER_KICKOUT,
		//  c++2view 响应离开队员消息命令
		GVIEWCMD_TEAM_LEAVEMEMBER_LEAVE,
		//  c++2view 响应解散队伍消息命令
		GVIEWCMD_TEAM_LEAVEMEMBER_DISBAND,

        // 撮合界面显示
        GVIEWCMD_MATCH_WND_SHOW,

        // 撮合界面隐藏
        GVIEWCMD_MATCH_WND_HIDE,

        // 撮合接受or拒绝界面
        GVIEWCMD_MATCH_SHOW_ACCEPT_CHECK,

        // 撮合接受or拒绝界面
        GVIEWCMD_MATCH_CUR_MATCH_COUNT_INFO,

        // 撮合接受or拒绝界面
        GVIEWCMD_MATCH_HIDE_ACCEPT_CHECK,

        // 撮合成功
        GVIEWCMD_MATCH_OK,

        // 撮合失败
        GVIEWCMD_MATCH_FAIL,

		// 线上杯赛房间等待倒计时
		GVIEWCMD_LEGENDCUP_ROOM_WAIT_COUNTDOWN,

        // 通知随机名字结果
        GVIEWCMD_USER_RANDOM_PLAYER_NAME_RESULT,

        // 战队事件列表
        GVIEWCMD_RECV_KIN_EVENTLIST,

        // 通知显示层显示公共消息框
        GVIEWCMD_SHOW_COMMON_MESSAGEBOX,

		// c++2view防沉迷验证结果
		GVIEWCMD_ANTIADDICT_REGIST_RESULT,

		// c++2view防沉迷信息提醒
		GVIEWCMD_ANTIADDICT_TIP,
		
		// c++2view 更新师傅数据信息 
		GVIEWCMD_MENTORSHIP_UPDATESHIPINFO,

		// c++2view 更新师徒个人基础数据信息
		GVIEWCMD_MENTORSHIP_UPDATEPERSONBASE,

		// c++2view 更新师徒个人师徒列表信息
		GVIEWCMD_MENTORSHIP_UPDATEMENTORSHIPLIST,

        ///////////////////////c++2view/////////////语音///////////////////
        // 更新频道列表
        GVIEWCMD_VOICE_CHANNEL_LIST,

        // 通知显示层进入房间
        GVIEWCMD_VOICE_ENTER_ROOM,

        // 更新房间基本信息到显示层
        GVIEWCMD_VOICE_UPDATE_ROOM_BASE_INFO,

        // 更新房间详细信息到显示层
        GVIEWCMD_VOICE_UPDATE_ROOM_INFO,

        // 更新房间内用户信息到显示层
        GVIEWCMD_VOICE_UPDATE_USER_INFO,

        // 更新房间内用户是否正在说话到显示层
        GVIEWCMD_VOICE_UPDATE_USER_ISSPEAKING,
        ////////////////////////////////////语音///////////////////

        // c++2view显示任务列表
        GVIEWCMD_SHOW_TASK_LIST,

        // c++2view更新任务列表
        GVIEWCMD_UPDATE_TASK_LIST,

        // view2c++ 删除任务列表
        // c++2view 删除任务列表
        GVIEWCMD_DELETE_TASK_LIST,

        // c++2view发送任务奖励
        GVIEWCMD_SEND_TASK_PRIZE,

        // view2c++ 完成任务
        GVIEWCMD_FINISH_TASK,

		// view2c++ 刷新任务
		GVIEWCMD_FRESH_TASK,



		// view2c++ 查看任务基础
		// c++2view 查看任务基础
		GVIEWCMD_VIEW_TASK_BASE,

		// view2c++ 查看任务回复
		// c++2view 查看任务回复
		GVIEWCMD_VIEW_TASK_REPLY,

		// view2c++ 发布任务
		// c++2view 发布任务
		GVIEWCMD_PUBLISH_TASK,

		// view2c++ 预请求任务
		// c++2view 预请求信息
		GVIEWCMD_PRE_CATCH_TASK,

		// view2c++ 请求任务  
		// c++2view 请求信息
		GVIEWCMD_CATCH_TASK,

		// view2c++ 添加回复
		GVIEWCMD_REPLY_TASK,

		// view2c++ 刷新任务操作
		GVIEWCMD_REFLUSH_OPT_TASK,

		// c++2view 玩家发布的任务
		GVIEWCMD_ACTOR_PUBLISH_CONTACT,

		// 通知任务已删除
		GVIEWCMD_VIEW_TASK_DELETE,

		///////////////////////c++2view/////////////比赛///////////////////
		// 战队荣誉数据
		GVIEWCMD_RECV_KIN_GLORYDATA,
		// 接收战队总历史战绩信息
		GVIEWCMD_KIN_RECV_WARRECORD_TOTAL,
		
		// 接收所有比赛列表
		GVIEWCMD_LEGENDCUP_RECV_CUP_LIST,

		// 接收某杯赛战队列表 
		GVIEWCMD_LEGENDCUP_RECV_REGIST_KINLIST,
		// 接收某战队成员列表 
		GVIEWCMD_LEGENDCUP_RECV_REGIST_MEMBERLIST,
		// 接收某杯赛黑名单列表 
		GVIEWCMD_LEGENDCUP_RECV_REGIST_BLACKLIST,

		// 接收某杯赛对战节点信息 
		GVIEWCMD_LEGENDCUP_RECV_COMPETITION_KIN,
		GVIEWCMD_LEGENDCUP_RECV_COMPETITION_NODE,

		// 接收某杯赛节点对战结果
		GVIEWCMD_LEGENDCUP_RECV_COMPETITION_RESULT,

		// 更新单个杯赛基础信息
		GVIEWCMD_LEGENDCUP_UPDATE_SINGLE_CUP,

		// 更新单个杯赛节点信息
		GVIEWCMD_LEGENDCUP_UPDATE_SINGLE_COMPETITION_NODE,

		// 加入杯赛成功
        GVIEWCMD_LEGENDCUP_REGISTER_SUCCESS,

        // 请求创建杯赛类型
        GVIEWCMD_LEGENDCUP_CREATE_CUP_TYPEINFO,

        // 参赛成员知晓杯赛成功
        GVIEWCMD_LEGENDCUP_PUBLIC_SUCCESS,

        // 举办者知晓杯赛成功
        GVIEWCMD_LEGENDCUP_CREATER_PUBLIC_SUCCESS,

		// 邀请加入战队
		GVIEWCMD_KIN_RECVKININVITE,

		// 杯赛家族信息界面接收数据
		GVIEWCMD_KIN_LEGENDCUPKININFO_RECV,

		// 杯赛家族荣誉界面接收数据
		GVIEWCMD_KIN_LEGENDCUPKINGLORY_RECV,

		// 某杯赛所有轮次时间数据接收
		GVIEWCMD_LEGENDCUP_ROUNDTIME_RECV,

		////////////////////////////////////比赛///////////////////


		/////////////////////////SNS c++2view 开始 ////////////////////
		// 自己的信息
		GLOGICCMD_SNS_BUDDY_SELF,
		// 聊听消息
		GLOGICCMD_SNS_MSG_DATA,
		// 好友关系状态
		GLOGICCMD_SNS_UPDATE_RELATIONSHIP,
		// 好友状态（在线、离线等）
		GLOGICCMD_SNS_BUDDY_STATUS,
		// 好友关系列表
		GLOGICCMD_SNS_BUDDYRELATION_LIST,
		// 所有好友信息
		GLOGICCMD_SNS_BUDDY_LIST,
		// 好友分组信息
		GLOGICCMD_SNS_BUDDYGROUP_LIST,
		// 好友信息
		GLOGICCMD_SNS_BUDDY,
		// 好友备注
		GLOGICCMD_SNS_BUDDY_REMARK,
		// 好友关系
		GLOGICCMD_SNS_BUDDYRELATION,
		// 用户信息
		GLOGICCMD_SNS_USERINFO,
		// 好友信息发生改变
		GLOGICCMD_SNS_BUDDYINFO_CHNAGE_NOTIFY,
		// 一起游戏
		GLOGICCMD_SNS_INVITE_PLAYGAME_NOTIFY,
		// 更新场景
		GLOGICCMD_SNS_UPDATE_SCENE_NOTIFY,
		// 一起组队匹配
		GLOGICCMD_SNS_INVITE_MATCH_TEAM_NOTIFY,
		

		/////////////////////////SNS c++2view 结束////////////////////

		///////////////////////MAIL c++2view 开始///////////////////////
		// c++2view 响应阅读邮件
		GVIEWCMD_MAIL_RESPONSEREADMAIL,
		// c++2view 玩家未读邮件总数
		GVIEWCMD_MAIL_NEWMAILCOUNT,
		// c++2view 邮件收件成功
		GVIEWCMD_MAIL_RECVPLUSSUCCESS,

		///////////////////////MAIL c++2view 结束///////////////////////
        // c++2view 设置机器人模式 例如被嘲讽后不能自己操作
        GVIEWCMD_SET_ROBOT_MODE,

        // c++2view 设置摄像机高度
        GVIEWCMD_SET_CAMERA_HEIGHT,

		/////////////////////////////////选区/////////////////////////////////////////
		GVIEWCMD_REQUEST_DISTRICT_LIST,
		GVIEWCMD_RECV_DISTRICT_LIST,
		GVIEWCMD_REQUEST_SELECT_SERVER,
		GVIEWCMD_RECV_SELECT_SERVER_RESULT,
		GVIEWCMD_CHECK_GAME_VERSION,
        GVIEWCMD_REQUEST_LASTENTER_DISTRICT_LIST,
		GVIEWCMD_RECV_LASTENTER_DISTRICT_LIST,
        // 返回网络服务供应商信息
        GVIEWCMD_NETSERVICE_PROVIDER_RESULT,

		// c++2view 更新战队列表
		GVIEWCMD_KIN_UPDATEKINLIST,

        // c++2view view2c++右键按下时上报
        GVIEWCMD_REPORT_ON_RIGHTBUTTON_DOWN,

        // 登陆排队界面
        GVIEWCMD_LOGIN_OPENQUEUE_VIEW,

        // 登陆消息界面
        GVIEWCMD_LOGIN_OPEN_SERVERMSG_VIEW,

        /////////////////////////////////战场全局命令/////////////////////////////////////////
        GVIEWCMD_WAR_MOBA_GOODS_INFO,

		///////////////////////////////////屏幕录像///////////////////////////////////////
		GVIEWCMD_SCREENOPEN_OPEN,
		GVIEWCMD_SCREENOPEN_STOP,
		
		///////////////////////////////惩罚信息///////////////////////////////////////////
        // c++2view 自己的惩罚信息
        GVIEWCMD_MATCH_SELF_PUNISHINFO,

        // c++2view 请求撮合的时候惩罚信息相关
        GVIEWCMD_MATCH_REQUEST_PUNISHINFO,

        // c++2view 删除相关
        GVIEWCMD_DELETE_PUNISH_TYPE_INFO,

        GVIEWCMD_MASTER_VIEW_LOADED,

		////////////////////////////////迁移/////////////////////////////////////
		// view2c++  切换场景（换线）
		GVIEWCMD_MIGRATE_SWITCH_SCENE,

        ////////////////////////////////////撮合关闭开启//////////////////////////////////////
        // c++2view 删除相关
        GVIEWCMD_MATCH_OPEN_OR_CLOSE,

		//////////////////////////////////////////////////////////////////////////////////////
		// c++2view 开关子弹计数面板
		GVIEWCMD_SWITCH_BULLET_PANEL,

		// C++2view 战队数值属性更新
		GVIEWCMD_KIN_NUM_PROP_UPDATE,

		// view2c++邀请加入联盟
		GVIEWCMD_CLAN_INVITE,

		// C++2view邀请加入联盟返回
		GVIEWCMD_CLAN_INVITE_RET,

		// view2c++答复邀请
		GVIEWCMD_CLAN_ANSWER_INVITE,
		// C++2view 邀请加入联盟成功
		GVIEWCMD_CLAN_ANSWER_INVITE_SUCCESS,

		// c++2view 服务器推荐排位
		GVIEWCMD_MATCH_RECOMMEND_RANK,

		//////////////////////战场全局命令////////////////////
		GVIEWCMD_WAR_SAFETYZONE_INFO,			// 安全圈信息
		GVIEWCMD_WAR_NOX_SPREAD_CD,		// 扩散倒计时信息
		GVIEWCMD_WAR_PLAYER_RECONNECTION,	// 战场断线重连
		GVIEWCMD_WAR_SERVER_TO_VIEW_MSG,	


		// 比赛大厅 c++2view 进入者添加房间其他Ob数据
		GVIEWCMD_MATCH_ENTER_ROOM_OTHER_OB,
		// 比赛大厅 c++2view 进入者添加房间自己Ob数据
		GVIEWCMD_MATCH_ENTER_ROOM_SELF_OB,
		// 比赛大厅 c++2view Ob位置改变
		GVIEWCMD_MATCH_ROOM_HERO_POS_CHANGE_OB,
	};

    /*****************************************************************************
    * 定义显示层和逻辑层通讯的全局游戏相关结构体
    * NOTE!!! 请务必遵守调用约定
    * 1. gameview_  前缀: 表示此结构体是供显示层调用逻辑层使用
    * 2. gamelogic_ 前缀: 表示此结构体是供逻辑层调用显示层使用
    * 3. game_      前缀: 表示内部使用结构
    ******************************************************************************/

    // GVIEWCMD_USER_LOGIN
    // 显示层通告逻辑层用户要登陆了
    STRUCT gameview_login
    {
        int nClientVersion;                         // 本地版本号
        int worldid;                                // 区ID
        int isLocalLogin;                           // 是否本地
        sp_char_array(account, GAME_NAME_MAXSIZE);  // 账号名
        sp_char_array(password, GAME_PWD_MAXSIZE);  // 密码
        sp_char_array(svrip, GAME_MSGSTR_MAXSIZE);  // 服务器IP:仅供调试版使用!!!
        sp_char_array(access_token, GAME_LONGSTR_MAXSIZE);  // 用于标识其他类型登录数据

        STRUCT_INIT(gameview_login)
    };

    // GVIEWCMD_USER_CREATE_PLAYER
    STRUCT gameview_create_player
    {
        sp_char_array(name, GAME_NAME_MAXSIZE);     // 角色名
        int heroid;                                 // 英雄ID
        int skinid;                                 // 皮肤ID
        int sex;                                    // 性别

        STRUCT_INIT(gameview_create_player)
    };
    // GVIEWCMD_USER_CREATE_PLAYER_EXTEND
    STRUCT gameview_create_player_extend
    {
        int heroid;                                 // 英雄ID
        int skinid;                                 // 皮肤ID

        STRUCT_INIT(gameview_create_player_extend)
    };

    // GVIEWCMD_USER_DELETE_PLAYER
    STRUCT gameview_delete_player
    {
        sp_char_array(name, GAME_NAME_MAXSIZE);     // 角色名
        int pid;                                    // 角色ID

        STRUCT_INIT(gameview_delete_player)
    };

    STRUCT game_playerinfo
    {
        sp_char_array(name, GAME_NAME_MAXSIZE);     // 角色名
        unsigned long pid;                          // 角色ID
        int heroid;                                 // 该角色选择的英雄ID
        int skinid;

        STRUCT_INIT(game_playerinfo)
    };

    // GVIEWCMD_USER_RANDOM_PLAYER_NAME
    STRUCT game_player_request_name
    {
        BYTE	nSex;								// 性别
        sp_char_array(szOldName, ACTORNAME_MAXSIZE);// 上次获取角色名称
        BYTE	nOldSex;							// 上次获取角色名称对应性别
        BYTE	nFlag;								// 0.获取新的角色名称（角色名不为空时先需返还）；1.返还后不需要再获取

        STRUCT_INIT(game_player_request_name)
    };

    // 逻辑层通知显示层 GVIEWCMD_USER_CREATE_PLAYER_RESULT  创建角色结果
    STRUCT gamelogic_create_player_result
    {
    public:
        int		nResult;
        sp_char_array(szText, 256);

        STRUCT_INIT(gamelogic_create_player_result)
    };

    // GVIEWCMD_USER_DELETE_PLAYER_RESULT
    STRUCT gamelogic_updateplayers
    {
        int playercnt;                              // 角色数量
        // 角色数组
        sp_struct_array(game_playerinfo,players,MAX_ACTOR_NUM);

        STRUCT_INIT(gamelogic_updateplayers)
    };

    // GVIEWCMD_USER_RANDOM_PLAYER_NAME_RESULT
    STRUCT game_player_request_name_result
    {
        BYTE nSex;
        sp_char_array(szName, ACTORNAME_MAXSIZE);// 获取角色名称
        STRUCT_INIT(game_player_request_name_result)
    };

    // 逻辑层通告加载地图
	STRUCT game_LoadSceneInfo
	{
		sp_char_array(strSceneName, GAME_LONGSTR_MAXSIZE);      // 要加载的场景名字
		int nMapID;                                             // 地图ID
        int nSoundID;                                           // 背景音乐ID
		int nCmdID;                                             // 执行加载场景的命令
        int nXAxisLen;                                          // x轴长度
        int nZAxisLen;                                          // z轴长度
        float fOffx;                                            // 坐标偏移值 m
        float fOffy;                                            // 坐标偏移值 m
        float fOffz;                                            // 坐标偏移值 m

        float fBornPosX;
        float fBornPosY;
        float fBornPosZ;

		STRUCT_INIT(game_LoadSceneInfo)
	};

    // 鼠标进入槽位       GVIEWCMD_MATCH_LOAD_SCENE_PROGREESE_RATE
    STRUCT cmd_load_scene_rate
    {
    public:
        PDBID idActor;     // 角色ID
        int   nRate;     // 场景加载进度
        int   nPing;     // 网络延迟

        STRUCT_INIT(cmd_load_scene_rate);
    };

    // 显示消息框       GVIEWCMD_SHOW_MESSAGEBOX
    STRUCT cmd_show_message_box
    {
    public:
        int _type;                                  // 消息框类型
        sp_char_array( caption, 64 );			    // 消息框标题
        sp_char_array( text, 200 );			        // 消息框文本
        sp_char_array( okCallBack, 64);             // ok的回调函数
        sp_char_array(cancelCallback, 64);          // cancel的回调函数  

        STRUCT_INIT(cmd_show_message_box)
    };


    // 假人选择英雄       GVIEWCMD_MATCH_COMPUTERPLAYER_HEROCHANGE
    STRUCT cmd_computerplayer_add
    {
    public:
        int     nCamp;    // 阵营
        int     nPos;     // 添加位置
        

        STRUCT_INIT(cmd_computerplayer_add);
    };

    STRUCT cmd_computerplayer_remove
    {
    public:
        int     nCamp;    // 阵营
        int     nPos;     //  移除位置

        STRUCT_INIT(cmd_computerplayer_remove);
    };

    // 假人选择英雄       GVIEWCMD_MATCH_COMPUTERPLAYER_HEROCHANGE
    STRUCT cmd_computerplayer_herochange
    {
    public:
        int   nCamp;    // 阵营
        int   nPos;     // 哪个位置改变
        int   nHeroID;  // 选择的英雄

        STRUCT_INIT(cmd_computerplayer_herochange);
    };

    // 假人选择英雄AI       GVIEWCMD_MATCH_COMPUTERPLAYER_HEROAI_CHANGE
    STRUCT cmd_computerplayer_heroaichange
    {
    public:
        int   nCamp;    // 阵营
        int   nPos;     // 哪个位置改变
        int   nHeroID;  // 选择的英雄
        int   nAIID;    // 英雄AIID

        STRUCT_INIT(cmd_computerplayer_heroaichange);
    };

    // ban选英雄       GVIEWCMD_MATCH_ROOM_BAN_HERO
    STRUCT cmd_ban_hero
    {
    public:
        int   nHeroID;  // ban选的英雄

        STRUCT_INIT(cmd_ban_hero);
    };


    // GVIEWCMD_MATCH_SHOW_ACCEPT_CHECK
    STRUCT cmd_wait_accept_wnd
    {
    public:
        unsigned long dwAcceptCheckID;
        int     byMatchType;
        int     nWaitTime;					

        STRUCT_INIT(cmd_wait_accept_wnd);
    };


    //   GVIEWCMD_LOGIN_OPENQUEUE_VIEW
    STRUCT cmd_match_cur_player_count
    {
    public:
        int    nMaxCount;
        int    nCurCount;
        int    nLeftTime;					

        STRUCT_INIT(cmd_match_cur_player_count);
    };

	// 比赛大厅 view2c++ 选英雄皮肤  GVIEWCMD_MATCH_ROOM_SELECT_HERO_SKIN
	STRUCT cmd_matchroom_selectheroskin
	{
	public:
		int nHeroID;                                 // 该角色选择的英雄ID
		int nSkinID;

		STRUCT_INIT(cmd_matchroom_selectheroskin)
	};

    STRUCT cmd_DynamicObstacle_Info
    {
    public:
        UID   uidTarget;    // uid
        int   nIsAdd;       // 添加或者删除1添加0删除
        int   type;			// 障碍物形状类型
		float angle;		// 障碍物旋转角度
        sp_float_array(nDatas,6);// 障碍物形状数据（float[6]）
        STRUCT_INIT(cmd_DynamicObstacle_Info);
	};

	// GVIEWCMD_SEND_CHAT_MESSAGE
	// 显示层通知逻辑层发送聊天消息
	STRUCT gameview_send_chat_message
	{
	public:
		int channel;
		PDBID targetPdbid;
		sp_char_array(targetName, ACTORNAME_MAXSIZE);
		sp_char_array(message, 1024);
		bool isDetectTextRepeat;	// 是否要“相似度检测”
		

		STRUCT_INIT(gameview_send_chat_message)
	};

	// GVIEWCMD_SHOW_CHAT_MESSAGE
	// 逻辑层通知显示层显示聊天消息
	STRUCT gamelogic_show_chat_message
	{
	public:
		int channel;
		UID senderUID;
		PDBID senderPdbid;
		UDBID senderUdbid;
		int senderClanID;
		int senderKinID;
		sp_char_array(senderName, ACTORNAME_MAXSIZE);
		sp_char_array(message, 1024);

		STRUCT_INIT(gamelogic_show_chat_message)
	};

	// GVIEWCMD_SHOW_PRIVATE_CHAT_MESSAGE
	// 逻辑层通知显示层显示私聊消息
	STRUCT gamelogic_show_private_chat_message
	{
	public:
		int channel;
		PDBID senderPdbid;
		PDBID senderUdbid;
		int senderKinID;
		sp_char_array(senderName, ACTORNAME_MAXSIZE);
		PDBID targetPdbid;
		sp_char_array(targetName, ACTORNAME_MAXSIZE);
		sp_char_array(message, 1024);

		STRUCT_INIT(gamelogic_show_private_chat_message)
	};

	// GVIEWCMD_SHOW_SYSTEM_MESSAGE
	// 逻辑层通知显示层显示系统消息
	STRUCT gamelogic_show_system_message
	{
	public:
		int		nIndexId;
		int     nType;
		int     nPos;
		int     nShowTime;
		int		nPlusData;
		sp_char_array(message, 1024);
		sp_char_array(title, 128);

		STRUCT_INIT(gamelogic_show_system_message)
	};
	// 显示层通知逻辑层显示系统消息
	STRUCT gameview_show_system_message
	{
	public:
		int		nIndexId;
		sp_char_array(param, 1024);

		STRUCT_INIT(gameview_show_system_message)
	};

	// GVIEWCMD_SHOW_LOUDSPEAKER
	// 逻辑层通知显示层显示喇叭
	STRUCT gamelogic_show_loudspeaker
	{
	public:
		int		nIndexId;
		int     nType;
		int		nPlusData;
		sp_char_array(message, 1024);

		STRUCT_INIT(gamelogic_show_loudspeaker)
	};
	// 显示层通知逻辑层显示喇叭
	STRUCT gameview_show_loudspeaker
	{
	public:
		int		nIndexId;
		sp_char_array(param, 1024);

		STRUCT_INIT(gameview_show_loudspeaker)
	};

	// GVIEWCMD_CHAT_SET_CHANNELMASK
	STRUCT gameview_chat_set_channelmask
	{
	public:
		int		nChannelMask;

		STRUCT_INIT(gameview_chat_set_channelmask)
	};

    // 申请战队列表 
    //GVIEWCMD_KIN_REQUESTKINLIST
    STRUCT cmd_kin_requestkinlist
    {
    public:
        sp_char_array(szKinName,KIN_NAME_SIZE);             // 战队名
        int nPageNum;										// 页码
        int nState;											// 战队类型
        int  nCurIndex;										// 当前下标

        STRUCT_INIT(cmd_kin_requestkinlist);
    };

    // GVIEWCMD_KIN_CREATE
    STRUCT cmd_kin_create
    {
    public:
        sp_char_array(szKinName,KIN_NAME_SIZE);             // 战队名
		sp_char_array(szKinLabel,KIN_LABEL_SIZE);             // 战队标签
        //sp_float_array(szKinName,KIN_NAME_SIZE);             // 战队口号

        STRUCT_INIT(cmd_kin_create);
    };

    // 邀请答复 
    //GVIEWCMD_KIN_ANSWER_INVITE
	STRUCT cmd_kin_answer_Invite
	{
	public:
		int dwPDBID;
		int bIsAgree;
		STRUCT_INIT(cmd_kin_answer_Invite);
	};

    // 邀请加入战队 
    //    GVIEWCMD_KIN_INVITE
    STRUCT cmd_kin_invite
    {
    public:
        int nAnswerPDBID;

        STRUCT_INIT(cmd_kin_invite);
    };

	STRUCT cmd_recv_kin_invite
	{
	public:
		int dwInviterPDBID;								// 邀请者ID
		sp_char_array(szInviteName, ACTORNAME_MAXSIZE);
		sp_char_array(szKinName, KIN_NAME_SIZE);

		STRUCT_INIT(cmd_recv_kin_invite);
	};

    // 开除成员 
    //    GVIEWCMD_KIN_KICK
    STRUCT cmd_kin_kick
    {
    public:
        int     nMemberPDBID;									// 申请者ID
        STRUCT_INIT(cmd_kin_kick);
    };


    // 接收成员
    //    GVIEWCMD_KIN_ADMIT
    STRUCT cmd_kin_admit
    {
    public:
        int     nApplyPDBID;									// 申请者ID
        int     bAgree;										    // 同意/拒绝

        STRUCT_INIT(cmd_kin_admit);
    };

    // 编辑公告
    //    GVIEWCMD_KIN_MOTIFYNOTICE
    STRUCT cmd_kin_motifynotice
    {
    public:
        sp_char_array(szText, KIN_NOTICE_SIZE);                 // 编辑公告

        STRUCT_INIT(cmd_kin_motifynotice);
    };

    STRUCT cmd_kin_appoint_identity
    {
    public:
        int dwPDBID;
        int nIdentity;
        STRUCT_INIT(cmd_kin_appoint_identity);
    };
    


    STRUCT cmd_kin_reqest_rename
    {
    public:
        sp_char_array(szKinName,KIN_NAME_SIZE);             // 战队名
        STRUCT_INIT(cmd_kin_reqest_rename);
    };

    // 申请加入战队
    //    GVIEWCMD_KIN_APPLY
    STRUCT cmd_kin_apply
    {
    public:
        int nOldKinID;									    // 以前的战队ID
        int nKinID;										    // 现在的战队ID		
        sp_char_array(szName,KIN_NAME_SIZE);                 // 玩家名字
        BYTE  nLevel;										// 玩家等级
        BYTE  nVocation;									// 玩家职业
        STRUCT_INIT(cmd_kin_apply);
    };

     // 战队列表头
    STRUCT cmd_kin_list_head
    {
    public:
        int nKinCount;										// 战队个数

        STRUCT_INIT(cmd_kin_list_head);
    };

    // 战队列表节点 
    STRUCT cmd_kin_list_node
    {
    public:
        int     dwID;										// 战队ID
        int     nMemberCount;								// 战队人数
        BYTE    nLevel;										// 战队等级
        BYTE    nState;										// 战队状态
        int     nActivity;									// 活跃度
        int     bIsApply;									// 是否已经申请
        int     bAutoAccpet;								// 是否自动收人
        BYTE    nLevelLimit;								// 招人等级限制
        int     dwAcceptFlag;							    // 招人职业限制
        int     dwLeftMemberCount;							// 剩余位置
		int     nWeekActivity;                              // 周活跃度
        sp_char_array(szName,KIN_NAME_SIZE);				// 战队名
        sp_char_array(szShaikhName,KIN_NAME_SIZE);		    // 战队族长名字

        STRUCT_INIT(cmd_kin_list_node);
    };

    // 战队基础数据
    //    GVIEWCMD_KIN_BASEDATA
    STRUCT cmd_kin_basedata
    {
    public:
        int     nKinID;                                     // 战队ID
        BYTE    nKinLevel;                                  // 战队等级 
        int     dwShaikhID;                                 // 族长
        sp_char_array (szKinName, KIN_NAME_SIZE);			// 战队名						   
        sp_char_array(szShaikhName, ACTORNAME_MAXSIZE);		// 族长名					
        sp_char_array(szNotice,KIN_NOTICE_SIZE);			// 战队的每日公告
        int     nMemberCount;
        int     nOnlineCount;
        int     nKinMaxMember;
        int     nApplyCount;							// 申请战队人数
        int     nTotalFight;							// 战队总战力
        int     nFightOrder;							// 战力排名
		sp_char_array(szKinLabel, KIN_LABEL_SIZE);		// 战队标签
		int		nMatchWinNum;							// 战队胜场
		int		nMatchTotalNum;							// 战队总场数
		int		nGoldNum;								// 金奖次数
		int		nSilverNum;								// 银奖次数
		int     nWeekActivity;                          // 周活跃度
        STRUCT_INIT(cmd_kin_basedata);
    };

    // 战队基础数据
    //    GVIEWCMD_KIN_STRING_PROP_UPDATE
    STRUCT cmd_kin_stringprop_update
    {
    public:
        int nPropID;
        sp_char_array(szText,KIN_NOTICE_SIZE);			// 战队文字属性更新
        STRUCT_INIT(cmd_kin_stringprop_update);
    };

	// 战队数值属性
	//    GVIEWCMD_KIN_NUM_PROP_UPDATE
	STRUCT cmd_kin_numprop_update
	{
	public:
		int nPropID;
		int nValue;			                           // 战队数值属性更新
		STRUCT_INIT(cmd_kin_numprop_update);
	};
    

    // 自己的战队数据
    //    GVIEWCMD_KIN_UPDATESELF
    STRUCT cmd_kin_self_hero_member
    {
    public:
        int     dwPDBID;								// 玩家角色ID
        sp_char_array(szName, ACTORNAME_MAXSIZE);	// 角色名字
        int     dwKinID;							// 战队ID
        int	    nIdentity;							// 战队职位
        int     nClanCtrb;							// 帮会贡献度
        int     nTotalClanCtrb;						// 累计帮会贡献度

        int     dwPopedomFlag;						// 权限标志（一个位表示一种权限，一个整数表示玩家所拥有的权限）
        int     bIsOnline;							// 是否在线
        int	    nLogoutTime;						// 最近下线时间

        int     nVocation;                          // 职业
        int     nLevel;								// 等级
        short   nFaceID;							// 玩家头像

        int     nLoginTime;							// 成员上线时间（刚加入的成员，加入时候为他的上线时间）
        int     dwBattleVal;						// 战力值

        int     dwSrcBattleVal;						// 旧战力值
        int     dwBVOrder;							// 战力值排名
        int     dwLastOrder;						// 上一次战力值排行

        STRUCT_INIT(cmd_kin_self_hero_member);
    };

    // 自己的战队数据
    //    GVIEWCMD_KIN_QUIT
    STRUCT cmd_kin_quit
    {
    public: 
        bool    bQuit;

        STRUCT_INIT(cmd_kin_quit);
    };

    //    GVIEWCMD_KIN_MEMBERLIST
    STRUCT cmd_kin_membernode
    {
    public:
        int     dwUDBID;                                // 账号ID
        int	    dwPDBID;							    // 玩家ID
        int	    bIsOnline;							// 玩家是否在线
        int		nLogoutTime;						// 玩家最后下线时间
        int 	bIsPublicOnline;					// 玩家是否在公共区在线
        int     dwBattleVal;						// 战力值
        int     dwSrcBattleVal;						// 上一次战力值
        int	    dwBVOrder;							// 战力值排行
        int 	dwLastOrder;						// 上一次战力值排行
        BYTE	nLevel;								// 玩家等级
        BYTE	nIdentity;							// 族内职位
        BYTE	nVocation;							// 玩家职业
        BYTE	nFaceID;							// 头像ID
        BYTE	nSex;							    // 性别
        BYTE    byGameState;                         // 游戏状态

		unsigned long   dwWorldID;					// 世界ID
		unsigned long	dwSceneID;					// 场景ID
		unsigned long	serverID;					// 服务器ID

        sp_char_array( szGradeName, GAME_MSGSTR_MAXSIZE);       // 段位描述
        
        sp_char_array(szName, ACTORNAME_MAXSIZE);	// 玩家名字

        STRUCT_INIT(cmd_kin_membernode);
    };

    //    GVIEWCMD_KIN_MEMBERLIST
    STRUCT cmd_kin_eventlist
    {
    public:

        int	    nYear;							    // 年
        int	    nMonth;							    // 月
        int	    nDay;							    // 日
        int	    nHour;							    // 小时
        int	    nMinute;							// 分钟
        sp_char_array(szCoutext, GAME_LONGSTR_MAXSIZE);	// 玩家名字

        STRUCT_INIT(cmd_kin_eventlist);
    };


    
    //GVIEWCMD_KIN_APPLY_LIST
    STRUCT cmd_kin_applylist_head
    {
    public:
        int nCount;                                 // 数量
        sp_char_array(szSlogan, KIN_SLOGAN_SIZE);	// 口号

        STRUCT_INIT(cmd_kin_applylist_head);
    };

    //GVIEWCMD_KIN_AUTOACCEPT
    STRUCT cmd_kin_auto_accept
    {
    public:
        int bAutoAccept;                            // 自动收人设置
        int nAcceptLevel;							// 自动收人等级
        int dwAcceptFlag;							// 自动收人设置

        STRUCT_INIT(cmd_kin_auto_accept);
    };

    // 战队列表头
    STRUCT cmd_kin_remove_apply
    {
    public:
        int nPlayerPDBID;

        STRUCT_INIT(cmd_kin_remove_apply);
    };

    // 战队列表头
    STRUCT cmd_kin_create_success
    {
    public:
        int bSuccess;

        STRUCT_INIT(cmd_kin_create_success);
    };

    STRUCT cmd_kin_join_success
    {
    public:
        int bSuccess;

        STRUCT_INIT(cmd_kin_join_success);
    };
    
    STRUCT cmd_kin_applylist_node
    {
    public:
        int nPlayerPDBID;
        int nLevel;
        int nPower;
        sp_char_array(szName, ACTORNAME_MAXSIZE);	// 玩家名字
        STRUCT_INIT(cmd_kin_applylist_node);
    };

    // 战队列表头
    STRUCT cmd_kin_remove_member
    {
    public:
        int nPlayerPDBID;

        STRUCT_INIT(cmd_kin_remove_member);
    };

    

    STRUCT cmd_kin_update_member
    {
    public:
        int	    dwPDBID;							    // 玩家ID
        int	    bIsOnline;							// 玩家是否在线
        int		nLogoutTime;						// 玩家最后下线时间
        int 	bIsPublicOnline;					// 玩家是否在公共区在线
        int     dwBattleVal;						// 战力值
        int     dwSrcBattleVal;						// 上一次战力值
        BYTE	dwBVOrder;							// 战力值排行
        BYTE	dwLastOrder;						// 上一次战力值排行
        BYTE	nLevel;								// 玩家等级
        BYTE	nIdentity;							// 族内职位
        BYTE	nVocation;							// 玩家职业
        BYTE	nFaceID;							// 头像ID

        sp_char_array(szState, ACTORNAME_MAXSIZE);	// 游戏状态
        sp_char_array(szName, ACTORNAME_MAXSIZE);	// 玩家名字
        sp_char_array(szKinName, ACTORNAME_MAXSIZE);	// 战队名字
        sp_char_array( szGradeName, GAME_MSGSTR_MAXSIZE);       // 段位描述

        STRUCT_INIT(cmd_kin_update_member);
    };


	// 请求战队荣誉信息	GVIEWCMD_KIN_REQUESTKINGLORY
	STRUCT cmd_kin_glory_head
	{
		sp_char_array(szKinName, ACTORNAME_MAXSIZE);	// 战队名称
		int nWinNum;									// 比赛胜场
		int nLoseNum;									// 比赛负场
		int nGodNum;									// 团队超神次数
		int n5KillNum;									// 团队5杀次数
		int n4KillNum;									// 团队4杀次数
		int n3KillNum;									// 团队3杀次数
		int nDeathNum;									// 团队死亡次数
		int nKillNum;									// 团队击杀次数
		int nAssistNum;									// 团队助攻次数
		int nCount;										// 比赛获奖次数
		// nCount * cmd_kin_glory_node

		STRUCT_INIT(cmd_kin_glory_head);
	};

	STRUCT cmd_kin_glory_node
	{
		__int64		llLegendCupId;							// 杯赛ID
		int			nCupConfigID;							// 杯赛配置ID
		int			nEndTime;								// 杯赛配置时间
		int			nCupRank;								// 杯赛名次
		sp_char_array(szCupName, ACTORNAME_MAXSIZE);		// 杯赛名称

		STRUCT_INIT(cmd_kin_glory_node);
	};


	// 战队历史战绩数据节点  GVIEWCMD_KIN_RECV_WARRECORD_TOTAL,
	STRUCT cmd_kin_warrecord_total_warnode
	{
		__int64	nWarID;										// 战场ID
		int		nKin1ID;									// 战队1ID
		int		nKin2ID;									// 战队2ID
		sp_char_array(szKin1Name,ACTORNAME_MAXSIZE);        // 战队1名称
		sp_char_array(szKin2Name,ACTORNAME_MAXSIZE);        // 战队2名称
		int		nWinKinID;									// 胜利战队ID
		int		nBeginTime;									// 战场开始时间
		BYTE    nBoCount;									// 第几场(裁决)
		BYTE    byWinFlag;									// 胜利标志 EJudgeWar
		
		STRUCT_INIT(cmd_kin_warrecord_total_warnode)
	};

	// 杯赛家族信息界面打开 GVIEWCMD_KIN_LEGENDCUPKININFO_OPEN
	STRUCT cmd_kin_legendcup_kininfo
	{
		int nKinID;

		STRUCT_INIT(cmd_kin_legendcup_kininfo)
	};

	// 杯赛家族荣誉界面打开 GVIEWCMD_KIN_LEGENDCUPKINGLORY_OPEN
	STRUCT cmd_kin_legendcup_kinglory
	{
		int nKinID;

		STRUCT_INIT(cmd_kin_legendcup_kinglory)
	};
	
    // 请求帮会列表       GVIEWCMD_CLAN_REQUESTCLANLIST
    STRUCT cmd_clan_request_clanlist
    {
    public:
        int nPageNum;										// 页码
        sp_char_array(szClanName,CLAN_NAME_SIZE);           // 帮会名

        STRUCT_INIT(cmd_clan_request_clanlist);
    };

    // 帮会列表头
    STRUCT cmd_clan_list_head
    {
    public:
        int nClanCount;					// 帮会个数
        int nPageNum;					// 返回页码
        int nTotalNum;					// 帮会总个数
        int nApplyClanID;				// 自己申请的帮会

        STRUCT_INIT(cmd_clan_list_head);
    };

    // 帮会列表
    STRUCT cmd_clan_clanlist_node
    {
        int   nClanID;										// 帮会ID
        sp_char_array(szName,CLAN_NAME_SIZE);               // 帮会名
        sp_char_array(szShaikhName,ACTORNAME_MAXSIZE);      // 帮主名字
		int   nFlag;										// 帮会旗帜
        int   nLevel;										// 帮会等级
		int   nKinCount;									// 帮会战队数
        sp_char_array(szManifesto,CLAN_MANIFESTO_SIZE);     // 帮会宣言
        int   nMemberCount;									// 帮会人数
        int   nState;										// 帮会状态
        int   nClanTotalFight;                              // 帮会总战力
		int   nWeekActivity;                                // 周活跃度

        STRUCT_INIT(cmd_clan_clanlist_node);
    };

    // 创建帮会     GVIEWCMD_CLAN_CREATE
    STRUCT cmd_clan_create
    {
    public:
		sp_char_array(szClanName,CLAN_NAME_SIZE);           // 帮会名
		sp_char_array(szManifesto,CLAN_MANIFESTO_SIZE);     // 帮会宣言
		int nClanFlag;										// 帮会旗帜

        STRUCT_INIT(cmd_clan_create);
    };

    // 申请加入帮会   GVIEWCMD_CLAN_APPLY
    STRUCT cmd_clan_apply
    {
    public:
        int nClanID;									    // 帮会ID

        STRUCT_INIT(cmd_clan_apply);
    };

    // 帮会任命指挥官   GVIEWCMD_CLAN_SETLEADER
    STRUCT cmd_clan_setleader
    {
    public:
        int nActorID;									    // 角色ID

        STRUCT_INIT(cmd_clan_setleader);
    };

    // 帮会踢出战队   GVIEWCMD_CLAN_KICKKIN
    STRUCT cmd_clan_kickkin
    {
    public:
        int nActorID;									    // 角色ID

        STRUCT_INIT(cmd_clan_kickkin);
    };

    // 帮会基础数据   GVIEWCMD_CLAN_BASEDATA
    STRUCT cmd_clan_basedata
    {
    public:
        sp_char_array(szClanName, CLAN_NAME_SIZE);			// 帮会名
        sp_char_array(szShaikhName, ACTORNAME_MAXSIZE);	    // 帮主名					
        sp_char_array(szManifesto,CLAN_MANIFESTO_SIZE);		// 帮会宣言
		sp_char_array(szNotice,CLAN_NOTICE_SIZE);			// 帮会公告
        int     nShaikhID;								    // 族长
        int     nConstructDegree;							// 建设度
        int     nGold;										// 帮会金钱
        int     nActivity;									// 活跃度
		int     nWeekActivity;								// 周活跃度
        int     nPlace;										// 排名
        int     nMemberCount;                               // 成员数量
        BYTE    nClanLevel;								    // 帮会等级
        BYTE    nApplyMemberNum;                            // 申请成员数量
        int     nTotalFight;                                // 总战力
        int     nKinCount;                                  // 战队数量
        int     nMaxKinCount;                               // 最大战队数量
		int		nLegendCupCount;							// 杯赛举办次数
		__int64 lLegendCupID;								// 杯赛ID
		int		nAutoAcceptFlag;							// 自动接受标示
		int		nServerTime;								// 服务器当前时间
		int     nState;                                     // 联盟状态
		int     nShaikhState;                               // 盟主状态
        int     nDismissApplyTime;                          // 联盟申请解散时间
        STRUCT_INIT(cmd_clan_basedata);
    };

    // 个人帮会数据   GVIEWCMD_CLAN_UPDATECLIENT
    STRUCT cmd_clan_selfdata
    {
    public:
        int     nShaikhID;								    // 族长
        sp_char_array(szClanName, CLAN_NAME_SIZE);			// 帮会名
        sp_char_array(szShaikhName, ACTORNAME_MAXSIZE);	    // 帮主名					

        STRUCT_INIT(cmd_clan_selfdata);
    };

	// 邀请加入联盟 
	// GVIEWCMD_CLAN_INVITE
	STRUCT cmd_clan_invite
	{
	public:
		int nInviteePDBID;

		STRUCT_INIT(cmd_clan_invite);
	};

	// 邀请加入联盟 
	// GVIEWCMD_CLAN_INVITE_RET
	STRUCT cmd_clan_invite_ret
	{
	public:
		int nInviterPDBID;                                   // 邀请者角色ID
		sp_char_array(szInviterName, GAME_NAME_MAXSIZE);     // 角色名
		sp_char_array(szClanName, CLAN_NAME_SIZE);			 // 联盟名
		STRUCT_INIT(cmd_clan_invite_ret);
	};

	// 邀请答复 
	//GVIEWCMD_CLAN_ANSWER_INVITE
	STRUCT cmd_clan_answer_invite
	{
	public:
		int dwPDBID;
		int bIsAgree;
		STRUCT_INIT(cmd_clan_answer_invite);
	};

    // 登陆排队   GVIEWCMD_LOGIN_OPENQUEUE_VIEW
    STRUCT cmd_open_queue_wnd
    {
    public:
        int RemainTime;
        int order;                // 在队列中的位置,=0表示移出了队列					

        STRUCT_INIT(cmd_open_queue_wnd);
    };

    // 服务器返回的消息   GVIEWCMD_LOGIN_OPEN_SERVERMSG_VIEW
    STRUCT cmd_open_servermsg_wnd
    {
    public:
        sp_char_array(szMsg, GAME_LONGSTR_MAXSIZE);	// 玩家名字				

        STRUCT_INIT(cmd_open_servermsg_wnd);
    };

    // 客户端帮会成员信息结构      GVIEWCMD_CLAN_ADDMEMBER
    STRUCT cmd_clan_memberlist
    {
        int nPDBID;								    // 玩家ID
        sp_char_array(szName, ACTORNAME_MAXSIZE);	// 玩家名字
        int nKinID;								    // 隶属战队
		sp_char_array(szKinName, KIN_NAME_SIZE);	// 隶属战队名字
		BYTE  nFaceID;								// 玩家头像
        BYTE  nLevel;								// 玩家等级
		BYTE  nIdentity;							// 族内职位
		BYTE  nIsOnline;							// 玩家是否在线
		int   nGameStatus;							// 游戏状态
		int   nDonateThisWeek;						// 本周捐赠
		int   nDonateTotal;							// 总捐赠
		sp_char_array(szGrade, GAME_MSGSTR_MAXSIZE);// 段位名称
		int   dwBattleVal;							// 战力值
		int   dwSrcBattleVal;						// 上一次战力值
		int   dwBVOrder;							// 战力值排行
		int   dwLastOrder;							// 上一次战力值排行
        int   nLogoutTime;							// 玩家最后下线时间
        BYTE  nSex;                                 // 玩家性别
		unsigned long dwWorldID;					// 世界ID
		unsigned long dwSceneID;					// 场景ID
		unsigned long serverID;					    // 服务器ID

        STRUCT_INIT(cmd_clan_memberlist);
    };

    // 客户端申请帮会成员头结构
    STRUCT cmd_clan_applymember_head
    {
        int nCount;								            // 申请数量

        STRUCT_INIT(cmd_clan_applymember_head);
    };

    // 客户端申请帮会成员信息结构
    STRUCT cmd_clan_applymember
    {
        int nKinID;								            // 战队ID
        sp_char_array(szName, KIN_NAME_SIZE);	            // 战队名字
        sp_char_array(szShaikhName, ACTORNAME_MAXSIZE);	    // 族长名字
		int nMemberCount;									// 战队人数

        STRUCT_INIT(cmd_clan_applymember);
    };

    // 移除帮会申请       GVIEWCMD_CLAN_REMOVEAPPLY 
    STRUCT cmd_clan_remove_apply
    {
    public:
        int nKinID;                                         // 战队ID

        STRUCT_INIT(cmd_clan_remove_apply);
    };

    // 增加帮会申请       GVIEWCMD_CLAN_ADDAPPLY
    STRUCT cmd_clan_add_apply
    {
    public:
        int nKinID;                                         // 战队ID
        sp_char_array(szName, KIN_NAME_SIZE);	            // 战队名字
        sp_char_array(szShaikhName, ACTORNAME_MAXSIZE);	    // 族长名字
        int nMemberCount;                                   // 战队人数

        STRUCT_INIT(cmd_clan_add_apply);
    };

    // 接收成员     GVIEWCMD_CLAN_ADMIT
    STRUCT cmd_clan_admit
    {
    public:
        int     nKinID;								        // 战队ID
        int     bAgree;										// 同意/拒绝

        STRUCT_INIT(cmd_clan_admit);
    };

    // 帮会移除成员       GVIEWCMD_CLAN_REMOVEMEMBER
    STRUCT cmd_clan_remove_member
    {
    public:
        int nDBID;                                          // 移除玩家ID

        STRUCT_INIT(cmd_clan_remove_member);
    };


    // 卡牌列表头结构     GVIEWCMD_CLAN_REQUEST_CARDLIST
    STRUCT cmd_clan_request_cardlist_head
    {
        int nCount;								            // 卡牌列表数量

        STRUCT_INIT(cmd_clan_request_cardlist_head);
    };
    
    // 卡牌列表结构
    STRUCT cmd_clan_request_cardlist
    {
        int nPDBID;			    // 请求支援玩家
        sp_char_array(szName, KIN_NAME_SIZE);	            // 请求支援玩家名字
        int nCardID;            // 卡牌ID
        int nMyCurCount;        // 自己当前卡牌数量
        int nMyMaxCount;        // 自己升级卡牌数量
        int nCurCount;          // 当前卡牌数量
        int nMaxCount;          // 最大卡牌数量
        int nDonateCardCount;   // 可捐赠卡牌次数
        int nDayDonateCardCount;            // 一天可捐赠卡牌次数

        STRUCT_INIT(cmd_clan_request_cardlist);
	};

	// 捐赠卡牌记录头结构     GVIEWCMD_CLAN_REQUEST_DONATELOG
	STRUCT cmd_clan_request_donatelog_head
	{
		int nCount;								            // 数量

		STRUCT_INIT(cmd_clan_request_donatelog_head);
	};

	// 更新捐赠卡牌记录结构     GVIEWCMD_CLAN_UPDATE_DONATELOG
	STRUCT cmd_clan_update_donatelog
	{
		PDBID nGaveActorID;							// 赠与玩家ID
		sp_char_array(szGaveActorName, ACTORNAME_MAXSIZE);	            // 赠与玩家名字
		PDBID nTakeActorID;							// 被赠与玩家ID
		sp_char_array(szTakeActorName, ACTORNAME_MAXSIZE);	            // 被赠与玩家名字
		int nCardID;								// 卡牌ID
		sp_char_array(szCardName, GAME_LONGSTR_MAXSIZE);	                // 卡牌名称
		int nCardNum;								// 卡牌数量

		STRUCT_INIT(cmd_clan_update_donatelog);
	};

	// 更新捐赠卡牌提示结构     GVIEWCMD_CLAN_ADD_REQUESTCARD_TIP
	STRUCT cmd_clan_add_requestcard_tip
	{
		int nIsInit;
		int nCount;

		STRUCT_INIT(cmd_clan_add_requestcard_tip);
	};

	// 更新捐赠卡牌信息结构     GVIEWCMD_CLAN_REQUESTCARD_INFO
	STRUCT cmd_clan_requestcard_info
	{
		int nLeftCooldownTime;								// 剩余请求冷却时间
		int nLeftRequestNum;								// 剩余请求次数

		STRUCT_INIT(cmd_clan_requestcard_info);
	};

	// 更新被捐卡提示结构     GVIEWCMD_CLAN_ADD_DONATECARD_TIP
	STRUCT cmd_clan_add_donatecard_tip
	{
		PDBID nPDBID;			// 捐赠者ID
		sp_char_array(szActorName, ACTORNAME_MAXSIZE);	// 捐赠者名字
		int nCardID;            // 卡牌ID
		sp_char_array(szCardName, GAME_LONGSTR_MAXSIZE);
		int nCardCount;         // 卡牌数量

		STRUCT_INIT(cmd_clan_add_donatecard_tip);
	};

	// 申请返回 GVIEWCMD_CLAN_APPLY_RET
	STRUCT cmd_clan_apply_ret
	{
		int nOldClanID;
		int nClanID;
		int nApplyState;

		STRUCT_INIT(cmd_clan_apply_ret);
	};

	// 帮会自动接受标示 GVIEWCMD_CLAN_SET_AUTOACCEPT_FLAG
	STRUCT cmd_clan_set_autoacceptflag
	{
		int nAutoAcceptFlag;

		STRUCT_INIT(cmd_clan_set_autoacceptflag);
	};

    // 帮会状态改变通知 GVIEWCMD_CLAN_STATE_NOTICE
    STRUCT cmd_clan_set_state
    {
        int nState;

        STRUCT_INIT(cmd_clan_set_state);
    };

    // 自己英雄结构     GVIEWCMD_CLAN_REQUEST_SELFHERO
    STRUCT cmd_clan_request_selfhero
    {
        int nHeroID;								        // 英雄ID
		int nHeroStar;										// 英雄星级
		int nCardCount;										// 当前卡片数量
		int nNeedCardCount;									// 升级需要卡片数量

        STRUCT_INIT(cmd_clan_request_selfhero);
    };

    // 请求支援     GVIEWCMD_CLAN_REQUEST_CARD
    STRUCT cmd_clan_request_card
    {
    public:
        int     nHeroID;								    // 英雄ID

        STRUCT_INIT(cmd_clan_request_card);
    };

    STRUCT cmd_clan_update_request_card
    {
    public:
        int nPDBID;			                // 请求支援玩家
        sp_char_array(szName, ACTORNAME_MAXSIZE);   // 请求支援玩家名字
        int nCardID;                        // 卡牌ID
        int nMyCurCount;                    // 自己当前卡牌数量
        int nMyMaxCount;                    // 自己升级卡牌数量
        int nCurCount;                      // 当前卡牌数量
        int nMaxCount;                      // 最大卡牌数量
        int nDonateCardCount;               // 可捐赠卡牌次数
        int nDayDonateCardCount;            // 一天可捐赠卡牌次数

        STRUCT_INIT(cmd_clan_update_request_card);
    };

    // 捐赠卡牌     GVIEWCMD_CLAN_DONATE_CARD
    STRUCT cmd_clan_donate_card
    {
    public:
        int     nDBID;								        // 捐赠DBID
        int     nCardID;                                    // 捐赠卡牌ID
        int     nCardCount;                                 // 捐赠卡牌数量

        STRUCT_INIT(cmd_clan_donate_card);
    };

    // 移除请求卡牌   GVIEWCMD_CLAN_REMOVE_REQUEST_CARD
    STRUCT cmd_clan_remove_request_card
    {
    public:
        int nPDBID;			                // 移除请求支援玩家

        STRUCT_INIT(cmd_clan_remove_request_card);
    };

    // 创建帮会成功   GVIEWCMD_CLAN_CREATE_SUCCEED
    STRUCT cmd_clan_create_succeed
    {
    public:
        int nSucceed;

        STRUCT_INIT(cmd_clan_create_succeed);
    };

    // 帮会事件列表   GVIEWCMD_CLAN_EVENT_LIST
    STRUCT cmd_clan_eventlist
    {
    public:
        int	    nTime;                                  // 事件时间
        sp_char_array(szCoutext, GAME_LONGSTR_MAXSIZE);	// 事件内容

        STRUCT_INIT(cmd_clan_eventlist);
    };

	// 帮会修改公告 GVIEWCMD_CLAN_MODIFY_NOTICE
	STRUCT cmd_clan_modify_notice
	{
	public:
		sp_char_array(szText, CLAN_NOTICE_SIZE);                 // 编辑公告

		STRUCT_INIT(cmd_clan_modify_notice);
	};

    //邀请组队命令 GVIEWCMD_TEAM_SUBINVITED
    STRUCT cmd_team_invited2view
    {
        unsigned long nBuildingSN;			// 组队流程流水号
        int nMemberCount;           // 队伍成员数量
        int nCaptain;               // 队长PDBID
        int nInviterPDBID;           // 邀请者PDBID
        sp_char_array(szInviterName, ACTORNAME_MAXSIZE);	// 邀请者名字

        STRUCT_INIT(cmd_team_invited2view);
    };

    //响应邀请组队命令 GVIEWCMD_TEAM_SUBINVITED
    STRUCT cmd_team_invited_respon
    {
        unsigned long nBuildingSN;			// 组队流程流水号
        int nReslut;                // 队伍成员数量

        STRUCT_INIT(cmd_team_invited_respon);
    };

    //请求组队命令 GVIEWCMD_TEAM_SUBREQUEST
    STRUCT cmd_team_request2logic
    {
        int nTargetPDBID;           // 请求目标PDBID
        STRUCT_INIT(cmd_team_request2logic);
    };

    STRUCT cmd_team_invite2logic
    {
        int nTargetPDBID;           // 邀请目标PDBID
        STRUCT_INIT(cmd_team_invite2logic);
    };

    //请求组队命令 GVIEWCMD_TEAM_SUBREQUEST
    STRUCT cmd_team_subrequest2view
    {
        unsigned long nBuildingSN;			// 组队流程流水号
        int bTargeInTeamFlag;        // 被请求组队的玩家是否有队伍
        int nRequestPDBID;           // 邀请者PDBID
        sp_char_array(szRequestName, ACTORNAME_MAXSIZE);	// 邀请者名字

        STRUCT_INIT(cmd_team_subrequest2view);
    };

    //响应请求组队命令 GVIEWCMD_TEAM_SUBINVITED
    STRUCT cmd_team_request_respon
    {
        unsigned long nBuildingSN;			// 组队流程流水号
        int nReslut;                // 队伍成员数量

        STRUCT_INIT(cmd_team_request_respon);
    };

	//提升队长 GVIEWCMD_TEAM_APPOINTCAPTAIN
	STRUCT cmd_team_appointCaptain2logic
	{
		int nTargetPDBID;           // 目标队长PDBID
		STRUCT_INIT(cmd_team_appointCaptain2logic);
	};

	//改变队长 GVIEWCMD_TEAM_CHANGECAPTAIN
	STRUCT cmd_team_changeCaptain2view
	{
		int nCaptainPDBID;           // 新队长PDBID
		STRUCT_INIT(cmd_team_changeCaptain2view);
	};

	//踢出队员 GVIEWCMD_TEAM_KICKOUTMEMBER
	STRUCT cmd_team_kickoutMember2logic
	{
		int nTargetPDBID;           // 踢出目标PDBID
		STRUCT_INIT(cmd_team_kickoutMember2logic);
	};

	// 踢出队员 GVIEWCMD_TEAM_LEAVEMEMBER_KICKOUT
	STRUCT cmd_team_kickoutMember2view
	{
		int nKickoutPDBID;           // 踢出目标PDBID
		STRUCT_INIT(cmd_team_kickoutMember2view);
	};

	// 离开队伍 GVIEWCMD_TEAM_LEAVEMEMBER_LEAVE
	STRUCT cmd_team_leaveTeam2view
	{
		int nLeaveMemberPDBID;           // 离开队员PDBID
		STRUCT_INIT(cmd_team_leaveTeam2view);
	};

    ///GVIEWCMD_MATCH_ROOM_STATE
    STRUCT cmd_change_roomstate
    {
        int nStateID;           // 切换房间状态
        STRUCT_INIT(cmd_change_roomstate);
    };

    // 显示公共消息框       GVIEWCMD_SHOW_COMMON_MESSAGEBOX
    STRUCT cmd_show_common_message_box
    {
    public:
        int nType;                                  // 消息框类型
        sp_char_array(szCaption, COMMON_MESSAGE_BOX_CAPTION_LEN);			// 消息框标题
        sp_char_array(szText, COMMON_MESSAGE_BOX_TEXT_LEN);			        // 消息框文本

        STRUCT_INIT(cmd_show_common_message_box)
    };

	//显示层通知逻辑层当前输入按键信息 GVIEWCMD_INPUTMANAGER_KEYSTATE
	STRUCT cmd_inputmanager_keystate
	{
	public:
		int nKeyCode;	//键码
		int nKeyState;	//按键状态

        STRUCT_INIT(cmd_inputmanager_keystate)
	};

	// view2c++ 防沉迷注册 GVIEWCMD_ANTIADDICT_REGIST
	STRUCT gameview_antiaddict_regist
	{
	public:
		sp_char_array(entityName, GAME_NAME_MAXSIZE);			
		sp_char_array(card, GAME_CARD_MAXSIZE);			        

		STRUCT_INIT(gameview_antiaddict_regist)
	};

	// c++2view 防沉迷验证结果 GVIEWCMD_ANTIADDICT_REGIST_RESULT
	STRUCT gamelogic_antiaddict_regist_result
	{
		int ret;
		sp_char_array(szRetDes, GAME_TITLE_MAXSIZE);
		
		STRUCT_INIT(gamelogic_antiaddict_regist_result)
	};

	// c++2view防沉迷信息提醒 GVIEWCMD_ANTIADDICT_TIP
	STRUCT gamelogic_antiaddict_tip
	{
		sp_char_array(szRetDes, GAME_TEXTSTR_MAXSIZE);

		STRUCT_INIT(gamelogic_antiaddict_tip)
	};
	
	// 请求个人数据信息 GVIEWCMD_MENTORSHIP_REQUESTPERSONDATA
	STRUCT cmd_mentorship_request_persondata
	{
	public:
		int nPDBID;									

		STRUCT_INIT(cmd_mentorship_request_persondata);
	};

	// 请求导师的学徒数据信息 		GVIEWCMD_MENTORSHIP_REQUESTPRENTICELIST
	STRUCT cmd_mentorship_prenticeinfo
	{
		int nMasterPDBID;							// 师傅玩家ID
		int	nPDBID;									// 学徒玩家ID
		sp_char_array(szName, ACTORNAME_MAXSIZE);	// 学徒玩家名字
		int   nFaceID;								// 头像ID
		BYTE  nIsOnline;							// 学徒玩家是否在线
		BYTE  nRelationType;						// 关系类型
		BYTE  nState;								// 关系状态

		STRUCT_INIT(cmd_mentorship_prenticeinfo);
	};

	// 客户端申请学徒成员头结构
	STRUCT cmd_mentorship_prenticelist_head
	{
		int nPDBID;
		int nCount;									           

		STRUCT_INIT(cmd_mentorship_prenticelist_head);
	};

	// 请求发布列表数据信息 GVIEWCMD_MENTORSHIP_REQUESTISSUELIST
	STRUCT cmd_mentorship_issueinfo
	{
		int nPDBID;											// 玩家ID
		sp_char_array(szName, ACTORNAME_MAXSIZE);			// 玩家名字
		int  nLevel;										// 玩家等级
		BYTE  bySex;										// 性别	
		sp_char_array(szCorpsName, KIN_NAME_SIZE);			// 战队名
		sp_char_array(szClanName, CLAN_NAME_SIZE);			// 战队名
		sp_char_array(szIssueInfo, MENTORSHIP_ISSUE_LEN);	// 发布内容

		STRUCT_INIT(cmd_mentorship_issueinfo);
	};

	// 客户端发布列表头结构
	STRUCT cmd_mentorship_issuelist_head
	{
		BYTE nIssueType;
		int nCount;									           

		STRUCT_INIT(cmd_mentorship_issuelist_head);
	};

	// 请求发布信息 GVIEWCMD_MENTORSHIP_REQUESTISSUEINFO
	STRUCT cmd_mentorship_request_issue_info
	{
		BYTE  nIssueType;
		sp_char_array(szIssueInfo, MENTORSHIP_ISSUE_LEN);	// 发布内容

		STRUCT_INIT(cmd_mentorship_request_issue_info);
	};

	// view2c++ 请求拜师或收徒信息 GVIEWCMD_MENTORSHIP_REQUESTAPPLYSHIP
	STRUCT cmd_mentorship_request_apply
	{
		int   nDesPDBID;
		BYTE  nRelationType;
		BYTE  nApplyPost;				           

		STRUCT_INIT(cmd_mentorship_request_apply);
	};

	// view2c++ 请求拜师或收徒应答信息  GVIEWCMD_MENTORSHIP_REQUESTAPPLYANSWER
	STRUCT cmd_mentorship_request_applyanswer
	{
		int	  nSrcPDBID;
		BYTE  nRelationType;
		BYTE  nApplyPost;
		sp_char_array(szSrcName, ACTORNAME_MAXSIZE);
		BYTE  nAnwser;			           

		STRUCT_INIT(cmd_mentorship_request_applyanswer);
	};

	// view2c++ 请求解散师徒信息 GVIEWCMD_MENTORSHIP_REQUESTDISMISS
	STRUCT cmd_mentorship_request_dismiss
	{
		int   nMentorshipID;				           

		STRUCT_INIT(cmd_mentorship_request_dismiss);
	};

	// view2c++ 请求提示学徒信息 GVIEWCMD_MENTORSHIP_REQUESTPROMOTE
	STRUCT cmd_mentorship_request_promote
	{
		int   nMentorshipID;
		int	  nDesPDBID;

		STRUCT_INIT(cmd_mentorship_request_promote);
	};

	// c++2view 请求自动发布信息 GVIEWCMD_MENTORSHIP_REQUESTISSUEAUTOSET
	STRUCT cmd_mentorship_request_issueautoset
	{
		BYTE nIssueType;
		BYTE nAutoSet;

		STRUCT_INIT(cmd_mentorship_request_issueautoset);
	};

	// view2c++ 请求领取首胜奖励 GVIEWCMD_MENTORSHIP_REQUESTFIRSTAWARD
	STRUCT cmd_mentorship_request_firstaward
	{
		BYTE nPost;

		STRUCT_INIT(cmd_mentorship_request_firstaward);
	};

	// c++2view 更新师傅数据信息 GVIEWCMD_MENTORSHIP_UPDATESHIPINFO
	STRUCT cmd_mentorship_updateshipinfo
	{
		int   nMentorshipID;
		BYTE  nRelationType;
		BYTE  nState;
		int   nSeverPDBID;
		int   nSeverTime;
		sp_char_array(szName, ACTORNAME_MAXSIZE);

		STRUCT_INIT(cmd_mentorship_updateshipinfo);
	};

	// c++2view 更新师徒个人基础数据信息 GVIEWCMD_MENTORSHIP_UPDATEPERSONBASE
	STRUCT cmd_mentorship_person_baseinfo
	{
		int	  nPDBID;								// 玩家ID
		BYTE  nAutoIssueMFlag;						// 自动发布师傅信息标识
		BYTE  nAutoIssuePFlag;						// 自动发布学徒信息标识
		int   nFormalCount;							// 正式个数
		int   nInternCount;							// 见习个数
		int	  nMSumShipVal;							// 导师关系总值
		int	  nMWeekShipVal;						// 导师本周关系值
		int   nMLastWeekShipVal;					// 导师上周关系值
		int   nMLastSVOrder;						// 导师上周关系排名
		int   nMRecordTime;							// 导师记录时间
		int	  nPSumShipVal;							// 学徒关系总值
		int	  nPWeekShipVal;						// 学徒本周关系值
		int   nPLastWeekShipVal;					// 学徒上周关系值
		int   nPLastSVOrder;						// 学徒上周关系排名
		int   nPRecordTime;							// 学徒记录时间
		BYTE  nMFWinAwardPreCount;					// 导师首胜奖励次数
		BYTE  nMFWinAwardCount;						// 导师首胜奖励次数
		int   nMFWinAwardTime;						// 导师首胜奖励时间
		BYTE  nPFWinAwardPreCount;					// 学徒首胜奖励次数
		BYTE  nPFWinAwardCount;						// 学徒首胜奖励次数
		int   nPFWinAwardTime;						// 学徒首胜奖励时间

		STRUCT_INIT(cmd_mentorship_person_baseinfo);
	};

	// c++2view 更新师徒个人师徒列表信息 GVIEWCMD_MENTORSHIP_UPDATEMENTORSHIPLIST
	STRUCT cmd_mentorship_personshipinfo
	{
	public:
		int   nMentorshipID;						// 关系ID
		int   nPDBID;								// 玩家ID
		sp_char_array(szName, ACTORNAME_MAXSIZE);	// 玩家名字
		int nFaceID;								// 头像ID
		BYTE  nIsOnline;							// 玩家是否在线
		BYTE  nPost;								// 关系中职位
		BYTE  nRelationType;						// 关系类型
		BYTE  nState;								// 关系状态
		int   nBuildTime;							// 建立时间
		int   nSeverPDBID;							// 解除者玩家ID
		int   nSeverTime;							// 解除时间									

		STRUCT_INIT(cmd_mentorship_personshipinfo);
	};

	STRUCT cmd_mentorship_mentorshiplist_head
	{
	public:
		int nCount;										

		STRUCT_INIT(cmd_mentorship_mentorshiplist_head);
	};


    ////////////////////////////////////语音//////toview/////////////
    // GVIEWCMD_VOICE_CHANNEL_LIST 更新显示层 频道ID列表 频道ID为0 表示频道没开
    STRUCT cmd_voice_update_channel_list
    {
        sp_int_array(channelList, 10);

        STRUCT_INIT(cmd_voice_update_channel_list)
    };

    // GVIEWCMD_VOICE_ENTER_ROOM 通知显示层进入房间
    STRUCT cmd_voice_enter_room
    {
        int nWorldID;  // 世界ID
        int nType;     // 频道类型  EMVoiceChannelType
        int nID;       // 频道ID    
        int nRoomID;   // 当前房间ID EMVoiceRoomType
        int nRight;    // 用户类型 EMVoiceRightFlag

        STRUCT_INIT(cmd_voice_enter_room)
    };

    // GVIEWCMD_VOICE_UPDATE_ROOM_BASE_INFO 更新房间基本信息到显示层
    STRUCT cmd_voice_update_room_base_info
    {
        int nRoomID;     // id   EMVoiceRoomType
        int nUserNum;    // 用户数
        int isSpeaking;  // 是否正在语音
        int isLocked;    // 是否有密码

        STRUCT_INIT(cmd_voice_update_room_base_info)
    };

    // GVIEWCMD_VOICE_UPDATE_ROOM_INFO 更新房间详细信息到显示层
    STRUCT cmd_voice_update_room_info
    {
        cmd_voice_update_room_base_info baseInfo;  // 基本信息
        int nRoomFlag;      // 房间标志  EMVoiceRoomStateFlag
        int nMicOrderMode;  // 麦序模式  EMVoiceMicState
        int nMicOrderInterval; // 麦序时间
        int nCurMicUserClientID;      // 当前麦上的用户(0表示没有人在麦上)

        STRUCT_INIT(cmd_voice_update_room_info)
    };

    // GVIEWCMD_VOICE_UPDATE_USER_INFO 更新语音玩家信息到显示层
    STRUCT cmd_voice_update_user_info
    {
        int isAdd;  // 1 增加 0 删除
        int nClientID;
        int nPDBID;
        int nUDBID;
        int nWorldID;
        int nUserType;    // 语音用户类型  EMVoiceUserType
        int nUserRight;   // 用户的权限掩码  EMVoiceRightFlag
        int nUserState;   // 当前状态掩码 EMVoiceUserStateFlag
        sp_char_array(szUserName,GAME_NAME_MAXSIZE);  // 玩家名称
        int nSex;         // 性别
        int nRankLevel;   // 段位
        sp_char_array(szKinName, KIN_NAME_SIZE); // 战队名称
        int nMicIndex;  // 麦序

        STRUCT_INIT(cmd_voice_update_user_info)
    };

    // GVIEWCMD_VOICE_UPDATE_USER_ISSPEAKING 更新语音用户是否正在说话信息到显示层
    STRUCT cmd_voice_update_user_ispeaking
    {
        int nClientID;
        int isSpeaking; // 0-没说话 1-正在说话

        STRUCT_INIT(cmd_voice_update_user_ispeaking)
    };

    ///////////////////////////////////////////以下to逻辑层////////////////////
    // GVIEWCMD_VOICE_REQUEST_OPEN_MIC 请求开关麦
    STRUCT cmd_voice_req_open_mic
    {
        int isOpen;  
        STRUCT_INIT(cmd_voice_req_open_mic)
    };

    // GVIEWCMD_VOICE_REQUEST_CHANGE_MUTE_STATE 请求开关听筒
    STRUCT cmd_voice_req_change_mute_state
    {
        int isMute;
        STRUCT_INIT(cmd_voice_req_change_mute_state)
    };

    // GVIEWCMD_VOICE_REQUEST_ENTER_ROOM 请求进入房间
    STRUCT cmd_voice_req_enter_room
    {
        int nWorldID;  // 世界ID
        int nType;     // 频道类型  EMVoiceChannelType
        int nID;       // 频道ID
        int nRoomID;   // 当前房间ID  EMVoiceRoomType
        sp_char_array(szPassword, 32);  // 密码

        STRUCT_INIT(cmd_voice_req_enter_room)
    };

    // GVIEWCMD_VOICE_REQUEST_CHANGE_MIC_ORDER_MODE 请求设置房间麦序模式(主席麦 麦序 自由麦)
    STRUCT cmd_voice_req_change_mic_order_mode
    {
        int nMode;  // EMVoiceMicState

        STRUCT_INIT(cmd_voice_req_change_mic_order_mode)
    };

    // GVIEWCMD_VOICE_REQUEST_SET_ROOM_FLAG 请求设置房间状态标记(麦序模式下可抢麦 麦序模式下控麦 自由麦下只能按键发音 有密码)
    STRUCT cmd_voice_req_set_room_flag
    {
        int nFalg;  // EMVoiceRoomStateFlag
        int isSet;   // 1-设置 0-取消

        STRUCT_INIT(cmd_voice_req_set_room_flag)
    };

    // GVIEWCMD_VOICE_REQUEST_LOCK_ROOM 请求修改房间密码
    STRUCT cmd_voice_req_lock_room
    {
        int nRoomID;  // EMVoiceRoomType
        int isLock;   // 1-加密码 0-取消密码
        sp_char_array(szPassword, 32);

        STRUCT_INIT(cmd_voice_req_lock_room)
    };

    // GVIEWCMD_VOICE_REQUEST_MIC_ORDER 抢麦/下麦
    STRUCT cmd_voice_req_mic_order
    {
        int isAcquire;

        STRUCT_INIT(cmd_voice_req_mic_order)
    };

    // GVIEWCMD_VOICE_REQUEST_SET_MIC_ORDER_TIME 设置麦序时间
    STRUCT cmd_voice_req_set_mic_order_time
    {
        int nRoomID;  // EMVoiceRoomType
        int nSec;

        STRUCT_INIT(cmd_voice_req_set_mic_order_time)
    };

    // GVIEWCMD_VOICE_REQUEST_ADD_VEST 添加马甲
    STRUCT cmd_voice_req_add_vest
    {
        int nUserID;    // udbid 目标
        int isAdd;      // 1--添加 0--删除
        int nVestType;  // EMVoiceUserType  如VoiceUserType_Admin

        STRUCT_INIT(cmd_voice_req_add_vest)
    };

    ////////////////////////////////////语音///////////////////
	
    // GVIEWCMD_DELETE_TASK_LIST 删除任务
    STRUCT gamelogic_delete_task_list
    {
        int nTaskID;			// 任务ID

        STRUCT_INIT(gamelogic_delete_task_list)
    };

    // GVIEWCMD_FINISH_TASK 完成任务
    STRUCT gamelogic_finish_task
    {
        int nTaskID;			// 任务ID

        STRUCT_INIT(gamelogic_finish_task)
    };

	// GVIEWCMD_FRESH_TASK	刷新任务
	STRUCT gamelogic_fresh_task
	{
		int nTaskType;			// 任务类型

		STRUCT_INIT(gamelogic_fresh_task)
	};



	//GVIEWCMD_VIEW_TASK_BASE,查看任务基础
	STRUCT cmd_task_view_base_v2c
	{
		__int64 llContactID;

		STRUCT_INIT(cmd_task_view_base_v2c)
	};

	//GVIEWCMD_VIEW_TASK_REPLY,查看任务回复
	STRUCT cmd_task_view_reply_v2c
	{
		int nTaskCount;
		sp_longlong_array(arrContactId, CONTACT_TASK_MAXSIZE + PUBLISH_CONTACT_MAXSIZE);
		sp_int_array(arrRelpyCount, CONTACT_TASK_MAXSIZE + PUBLISH_CONTACT_MAXSIZE);

		STRUCT_INIT(cmd_task_view_reply_v2c)
	};

	STRUCT cmd_task_node_c2v
	{
		__int64		llContactID;
		int			nPublishPDBID;
		int			nPublishUserID;
		int			nPublishHeadID;
		int			nPublishSex;
		__int64		llTagID;
		int			nServerTime;
		int			nPublishTime;
		sp_char_array(szPublishName, ACTORNAME_MAXSIZE);
		sp_char_array(szEmotionWords, GAME_LONGSTR_MAXSIZE);

		STRUCT_INIT(cmd_task_node_c2v)
	};

	STRUCT cmd_task_reply_node_c2v
	{
		__int64		llContactID;
		int			nReplyPDBID;
		int			nReplyUserID;
		int			nRelpyHeadID;
		int			nReplyTime;
		BYTE		byReplySex;
		sp_char_array(szReplyName, ACTORNAME_MAXSIZE);
		sp_char_array(szReplyContent, GAME_LONGSTR_MAXSIZE);

		STRUCT_INIT(cmd_task_reply_node_c2v)
	};

	//GVIEWCMD_PUBLISH_TASK,发布任务
	STRUCT cmd_task_publish_v2c
	{
		int			nSubType;
		int			nSpecificType;
		sp_char_array(szEmotionWords, GAME_LONGSTR_MAXSIZE);
		__int64		llTagID;

		STRUCT_INIT(cmd_task_publish_v2c)
	};

	//GVIEWCMD_PRE_CATCH_TASK,预获取任务
	STRUCT cmd_task_pre_catch_v2c
	{
		int			nCount;
		sp_longlong_array(arrContactId, CONTACT_TASK_MAXSIZE);

		STRUCT_INIT(cmd_task_pre_catch_v2c)
	};


	STRUCT cmd_task_pre_catch_c2v
	{
		int			nTaskID;
		__int64		llContactID;
		int			nTaskType;
		int			nTaskSubType;
		int			nTaskSpecificType;
		int			nPublisherDBID;

		STRUCT_INIT(cmd_task_pre_catch_c2v)
	};

	//GVIEWCMD_CATCH_TASK,获取任务
	STRUCT cmd_task_catch_v2c
	{
		int			nTaskID;
		__int64		llContactID;
		int			nTaskType;
		int			nTaskSubType;
		int			nTaskSpecificType;
		int			nPublisherDBID;

		STRUCT_INIT(cmd_task_catch_v2c)
	};

	//GVIEWCMD_REPLY_TASK,添加回复
	STRUCT cmd_task_add_reply_v2c
	{
		__int64		llContactID;
		int			nCurReplyCount;
		sp_char_array(szReplyContent, GAME_LONGSTR_MAXSIZE);

		STRUCT_INIT(cmd_task_add_reply_v2c)
	};

	//GVIEWCMD_ACTOR_PUBLISH_CONTACT 玩家发布的任务	
	STRUCT cmd_task_actor_publish_contact_c2v
	{
		int			nActorID;
		int			nCount;
		//			nCount * cmd_task_node_c2v

		STRUCT_INIT(cmd_task_actor_publish_contact_c2v)
	};


	////////////////////////////////////杯赛//////toview/////////////
	// 请求创建杯赛类型
	STRUCT cmd_legendcup_req_create_cup_type
	{
		BYTE byLegendCupType;								// 创建的类型ID 
		int nActorID;										// 玩家ID 

		STRUCT_INIT(cmd_legendcup_req_create_cup_type)
	};

    // 创建杯赛类型显示
    STRUCT cmd_legendcup_recv_create_cup_type_info
    {
        BYTE byLegendCupType;								// ELegendCupType
        int bIsFreeCreate;									// 是否免费创建


        STRUCT_INIT(cmd_legendcup_recv_create_cup_type_info)
    };

	// 接收杯赛列表 GVIEWCMD_LEGENDCUP_RECV_CUP_LIST
	// 更新单个杯赛基础信息 GVIEWCMD_LEGENDCUP_UPDATE_SINGLE_CUP
	STRUCT cmd_legendcup_recv_cuplist_head
	{
		int nCount;												// 杯赛数量

		STRUCT_INIT(cmd_legendcup_recv_cuplist_head)
	};

	STRUCT cmd_legendcup_recv_cuplist_node
	{
		__int64 nLegendCupID;										// 杯赛ID
		int nCreaterPDBID;										// 创建者ID
		int nIsSelfIn;											// 是否自己参赛
		int nIsLock;											// 是否加密
		int nIsClanLimit;										// 是否有同联盟限制
		int nIsOnline;											// 是否线上比赛
		int nIsEnd;												// 是否结束
		int nIsSystemCreate;									// 是否系统创建
		int nIsNewCreate;										// 是否新创建
		int nCupConfigID;										// 杯赛配置ID
		int nRegistLevelLimit;									// 报名等级限制
		int nRegistLeftTime;									// 报名剩余时间
		int nMaxKinNum;											// 总队伍数量
		int nCurrentKinNum;										// 当前队伍数量
		BYTE byCreateCupType;									// 创建类型（个人，系统）
		sp_char_array(szCupName,GAME_NAME_MAXSIZE);				// 杯赛名称
		sp_char_array(szCreateActorName,GAME_NAME_MAXSIZE);		// 发起人名称
		int	nNeedRoundCount;									// 杯赛需要打几轮
		int	nBeginRound;										// 本场比赛由第几轮开始
		int	nCurrentRound;										// 当前打到第几轮
		int nCompetitionBonus;									// 比赛奖金
		int nRegistGold;										// 报名金币
		int nIsSelfCreate;										// 是否自己创建

		STRUCT_INIT(cmd_legendcup_recv_cuplist_node)
	};

	// 接收某杯赛战队列表 GVIEWCMD_LEGENDCUP_RECV_REGIST_KINLIST
	STRUCT cmd_legendcup_recv_regist_kinlisthead
	{
		__int64 nLegendCupID;										// 杯赛ID
		int nRegistLeftTime;									// 报名剩余时间
		int nConfirmKinLeftTime;								// 比赛名单确认剩余时间
		int nMaxKinNum;											// 总队伍数量
		int nCurKinCount;										// 当前战队数
		// nCurKinCount * cmd_legendcup_recv_regist_kinlistnode

		STRUCT_INIT(cmd_legendcup_recv_regist_kinlisthead)
	};

	STRUCT cmd_legendcup_recv_regist_kinlistnode
	{
		__int64 nLegendCupID;									// 杯赛ID
		int nKinID;												// 战队ID
		int nKinLevel;											// 战队等级
		int nKinWinNum;											// 战队杯赛总胜场
		int nKinMaxNum;											// 战队杯赛总场数
		sp_char_array(szKinName,GAME_NAME_MAXSIZE);				// 战队名称							

		STRUCT_INIT(cmd_legendcup_recv_regist_kinlistnode)
	};

	// 接收某战队成员列表 GVIEWCMD_LEGENDCUP_RECV_REGIST_MEMBERLIST
	STRUCT cmd_legendcup_recv_regist_memberlisthead
	{
		int nKinID;												// 战队ID
		int nMemberCount;										// 当前战队人数
		// nMemberCount * cmd_legendcup_recv_regist_memberlistnode

		STRUCT_INIT(cmd_legendcup_recv_regist_memberlisthead)
	};

	STRUCT cmd_legendcup_recv_regist_memberlistnode
	{
        int uDBID;                                              // 账号ID
		int nPDBID;												// 玩家PDBID
		int nActorLevel;										// 玩家等级
		int nActorSex;											// 玩家性别
		sp_char_array(szActorName,GAME_NAME_MAXSIZE);			// 玩家名字

		int nType;												// 天梯类型
		int nRankGrade;											// 天梯段位
		int nRankScore;											// 天梯分数
		int nRankIconId;										// 段位图片ID
		sp_char_array( szGradeName, GAME_MSGSTR_MAXSIZE);       // 段位描述

		STRUCT_INIT(cmd_legendcup_recv_regist_memberlistnode)
	};

	// 接收某杯赛黑名单列表 GVIEWCMD_LEGENDCUP_RECV_REGIST_BLACKLIST
	STRUCT cmd_legendcup_recv_regist_balcklisthead
	{
		__int64 nLegendCupID;										// 杯赛ID
		int nCurBlacklistNum;									// 当前黑名单数
		// nCurBlacklistNum * cmd_legendcup_recv_regist_balcklistnode

		STRUCT_INIT(cmd_legendcup_recv_regist_balcklisthead)
	};

	STRUCT cmd_legendcup_recv_regist_balcklistnode
	{
		int nKinID;												// 战队ID
        int nKinWinNum;								// 战队杯赛总胜场
        int nKinMaxNum;								// 战队杯赛总场数
		sp_char_array(szKinName,GAME_NAME_MAXSIZE);				// 战队名称	

		STRUCT_INIT(cmd_legendcup_recv_regist_balcklistnode)
	};

	// 接收某杯赛对战成员信息 GVIEWCMD_LEGENDCUP_RECV_COMPETITION_KIN
	STRUCT cmd_legendcup_competition_kinmemberhead
	{
		__int64 nLegendCupID;										// 杯赛ID
		int nMemberCount;										// 玩家数量
		// nMemberCount * cmd_legendcup_competition_kinmembernode

		STRUCT_INIT(cmd_legendcup_competition_kinmemberhead)
	};
	STRUCT cmd_legendcup_competition_kinmembernode
	{
		int nKinID;												// 战队ID
		int nUserID;											// 玩家UserID
		int nPDBID;												// 玩家PDBID
		int nActorSex;											// 玩家性别
		int nActorLevel;										// 玩家等级
		sp_char_array(szActorName,GAME_NAME_MAXSIZE);			// 玩家名字 

		int nType;												// 天梯类型
		int nRankGrade;											// 天梯段位
		int nRankScore;											// 天梯分数
		int nRankIconId;										// 段位图片ID
		sp_char_array( szGradeName, GAME_MSGSTR_MAXSIZE);       // 段位描述

		STRUCT_INIT(cmd_legendcup_competition_kinmembernode)
	};

	// 接收某杯赛对战节点信息 GVIEWCMD_LEGENDCUP_RECV_COMPETITION_NODE
	// 更新单个杯赛节点信息 GVIEWCMD_LEGENDCUP_UPDATE_SINGLE_COMPETITION_NODE,
	STRUCT legendcup_kininfo
	{
		int nKinID;												// 战队ID
		sp_char_array(szKinName, GAME_NAME_MAXSIZE);			// 战队名称

		STRUCT_INIT(legendcup_kininfo)
	};

	STRUCT legendcup_kinvsinfo
	{
		int nKin1ID;											// 战队1ID
		sp_char_array(szKin1Name, GAME_NAME_MAXSIZE);			// 战队1名称
		int nKin2ID;											// 战队2ID
		sp_char_array(szKin2Name, GAME_NAME_MAXSIZE);			// 战队2名称

		STRUCT_INIT(legendcup_kinvsinfo)
	};

	STRUCT cmd_legendcup_recv_competition_roundinfo
	{
		int nRoundID;											// 轮数
		//int nRoundBeginTime;									// 该轮开始时间
		int nRoundCreateRoomTme;								// 本轮第一次创建房间的时间
		int nBeginInterval;										// 房间创建后开始间隔
		int nRoundEndTime;										// 本轮结束时间
		sp_char_array(szRoundName, GAME_NAME_MAXSIZE);			// 轮次名称

		STRUCT_INIT(cmd_legendcup_recv_competition_roundinfo)
	};

	STRUCT cmd_legendcup_recv_competition_head
	{
		__int64 nLegendCupID;										// 杯赛ID
		int nCompetitionNodeCount;								// 杯赛节点数量
		int nRoundID;											// 当前轮数
		int nIsFinalRound;										// 是否决赛轮
		int nGroupCount;										// 分组数量 
		int nGroupMaxKin;										// 每组最大战队数量
		BYTE byCompetitionType;									// ERoundCompetitionType
		sp_struct_array(legendcup_kininfo,kinInfos,GAME_LEGENDCUP_MAXKINSIZE);	// 战队信息
		// nCompetitionNodeCount * cmd_legendcup_recv_competition_node

		STRUCT_INIT(cmd_legendcup_recv_competition_head)
	};

	STRUCT cmd_legendcup_recv_competition_node
	{
		BYTE byCompetitionType;								// 1小组赛 2淘汰赛
		int nSearchID;										// 生成的快速定位ID
		int nRoundID;										// 第几轮比赛
		int nGroupID;										// 分配到第几组
		int nDay;											// 第几天
		int nPos;											// 位置索引 从1开始
		int nKin1ID;										// 比赛战队1
		int nKin1Score;										//
		sp_char_array(szKin1Name, GAME_NAME_MAXSIZE);		//
		int nKin2ID;										// 比赛战队2
		int nKin2Score;										//
		sp_char_array(szKin2Name, GAME_NAME_MAXSIZE);		//
		BYTE byCompetitionNodeState;						// 节点状态 ECompetitionNodeState
		int nWinnerKinID;									// 胜利战队ID
		BYTE byBoType;										// BO类型 ELegendCupBO
		int nCurrentBo;										// 已经打了多少场
		int nBeginTime;										// 节点比赛开始时间
		int nEnterTime;										// 节点进场开始时间
		int nServerTime;									// 服务器当前时间
		
		STRUCT_INIT(cmd_legendcup_recv_competition_node)
	};

	// 接收杯赛某节点历史战绩 GVIEWCMD_LEGENDCUP_RECV_COMPETITION_RESULT
	STRUCT cmd_legendcup_recv_competitionnode_head
	{
		int nKin1ID;										// 战队1ID
        int nKin1Score;                                     // 战队1得分
		int nKin2ID;										// 战队2ID
        int nKin2Score;                                     // 战队1得分
		sp_char_array(szKin1Name, GAME_NAME_MAXSIZE);		// 战队1名称
		sp_char_array(szKin2Name, GAME_NAME_MAXSIZE);		// 战队2名称
        int nResetEndTime;                                  // 可重置时间点
		int nCount;											// 比赛场次

		STRUCT_INIT(cmd_legendcup_recv_competitionnode_head)
	};

	////////////////////////////////////杯赛//////tologic/////////////
	// 请求创建杯赛 GVIEWCMD_LEGENDCUP_REQUEST_CUP_CREATE
	STRUCT cmd_legendcup_req_cup_create
	{
		BYTE byCupConfigID;										// 配置ID
		BYTE byClanLimit;										// 是否联盟限制（0为非联盟限制，1为联盟限制）
		BYTE byCupIsOnine;										// 是否线上比赛（0为线下，1为线上）
		BYTE byCreateCupType;									// 杯赛类型 ELegendCupType(个人,系统)
		int nCreaterPDBID;										// 创建者PDBID
		int nDayInterval;										// 报名持续时间(天)
		int nLevelLimit;										// 报名战队等级限制
		int nRegistGold;										// 报名金币
		int nContriBonus;										// 出资奖金
		sp_char_array(szLegendCupName, GAME_NAME_MAXSIZE);		// 杯赛名称
		sp_char_array(szPassword, GAME_PWD_MAXSIZE);			// 参赛密码

		int nClanID;											// 帮会ID
		BYTE byTicketAllocateMode;								//杯赛奖励分配方式 ETicketAllocateMode
		

		STRUCT_INIT(cmd_legendcup_req_cup_create)
	};

	// 请求查看某杯赛 GVIEWCMD_LEGENDCUP_VIEW_SINGLE_CUP,
	STRUCT cmd_legendcup_view_single_cup
	{
		__int64 nLegendCupID;										// 杯赛ID

		STRUCT_INIT(cmd_legendcup_view_single_cup)
	};

	// 请求发布某杯赛 GVIEWCMD_LEGENDCUP_RELEASE_SINGLE_CUP,
	STRUCT cmd_legendcup_release_single_cup
	{
		__int64 nLegendCupID;										// 杯赛ID
		int nOperatorID;										// 操作者ID
		BYTE byCreateCupType;									// 创建类型（个人 系统）

		STRUCT_INIT(cmd_legendcup_release_single_cup)
	};


	// 请求某战队成员列表 GVIEWCMD_LEGENDCUP_REQUEST_REGIST_MEMBERLIST
	STRUCT cmd_legendcup_req_regist_memberlist
	{
		__int64 nLegendCupID;										// 杯赛ID
		int nKinID;												// 战队ID

		STRUCT_INIT(cmd_legendcup_req_regist_memberlist)
	};

	// 请求加入某杯赛战队列表 GVIEWCMD_LEGENDCUP_ADD_REGIST_KINLIST
	STRUCT cmd_legendcup_add_regist_kinlist
	{
		__int64 nLegendCupID;										// 杯赛ID
		int nKinID;												// 战队ID
		int nActorID;											// 玩家PDBID
		BYTE byCreateCupType;									// 杯赛类型(系统 个人) 
		sp_char_array(szPassword, GAME_PWD_MAXSIZE);			// 参赛密码

		STRUCT_INIT(cmd_legendcup_add_regist_kinlist)
	};

	// 请求退出某杯赛战队列表 GVIEWCMD_LEGENDCUP_QUIT_REGIST_KINLIST
	STRUCT cmd_legendcup_quit_regist_kinlist
	{
		__int64 nLegendCupID;										// 杯赛ID
		int nKinID;												// 战队ID
		int nActorID;											// 玩家PDBID
		BYTE byCreateCupType;									// 杯赛类型(系统 个人) 

		STRUCT_INIT(cmd_legendcup_quit_regist_kinlist)
	};

	// 请求某杯赛黑名单列表 GVIEWCMD_LEGENDCUP_REQUEST_REGIST_BLACKLIST
	STRUCT cmd_legendcup_req_regist_blacklist
	{
		__int64 nLegendCupID;										// 杯赛ID

		STRUCT_INIT(cmd_legendcup_req_regist_blacklist)
	};

	// 请求加入某杯赛黑名单 GVIEWCMD_LEGENDCUP_ADD_REGIST_BLACKLIST
	STRUCT cmd_legendcup_add_regist_blacklist
	{
		__int64 nLegendCupID;										// 杯赛ID
		int nKinID;												// 战队ID
		int nOperatorID;										// 操作者ID
		BYTE byCreateCupType;									// 杯赛类型 ELegendCupType

		STRUCT_INIT(cmd_legendcup_add_regist_blacklist)
	};

	// 请求删除某杯赛黑名单 GVIEWCMD_LEGENDCUP_DELETE_REGIST_BLACKLIST
	STRUCT cmd_legendcup_delete_regist_blacklist
	{
		__int64 nLegendCupID;										// 杯赛ID
		int nKinID;												// 战队ID
		int nOperatorID;										// 操作者ID
		BYTE byCreateCupType;									// 杯赛类型 ELegendCupType

		STRUCT_INIT(cmd_legendcup_delete_regist_blacklist)
	};

	// 请求查看某节点比赛 GVIEWCMD_LEGENDCUP_VIEW_COMPETITION_NODE
	STRUCT cmd_legendcup_view_competition_node
	{
		__int64 nLegendCupID;										// 杯赛ID
		int nRoundID;											// 轮次ID
		int nSearchID;											// 节点ID

		STRUCT_INIT(cmd_legendcup_view_competition_node)
	};

	// 请求加入某节点比赛 GVIEWCMD_LEGENDCUP_ENTER_COMPETITION_NODE
	STRUCT cmd_legendcup_enter_competition_node
	{
		__int64 nLegendCupID;										// 杯赛ID
		int nRoundID;											// 轮次ID
		int nSearchID;											// 节点ID

		STRUCT_INIT(cmd_legendcup_enter_competition_node)
	};

	// 请求裁决某节点比赛 GVIEWCMD_LEGENDCUP_JUDGE_COMPETITION_NODE
	STRUCT cmd_legendcup_judge_competition_node
	{
		__int64		nLegendCupID;									// 杯赛ID
		int		nRoundID;										// 轮次ID
		int		nSearchID;										// 节点ID
		int     nBoCount;										// 第几场
		__int64 llWarID;										// 比赛ID
		BYTE    byJudgeFlag;									// EJudgeNodeWar 
		int     dwWinnerKinID;									// 根据byJudgeFlag 来确定是否选择WinnerID
        __int64 lResetNewTime;									// 线上比赛重置新的比赛时间
        int     nKin1;                                         // 队伍1
        int     nKin1Score;									// 队伍1得分
        int     nKin2;                                         // 队伍2
        int     nKin2Score;									// 队伍2得分

		STRUCT_INIT(cmd_legendcup_judge_competition_node)
	};

	// 杯赛界面退出观察 GVIEWCMD_LEGENDCUP_QUITOBSERVE
	STRUCT  cmd_legendcup_quit_observe
	{
		__int64		nLegendCupID;									// 杯赛ID

		STRUCT_INIT(cmd_legendcup_quit_observe);
	};

    //提前结束报名阶段 GVIEWCMD_LEGENDCUP_OFFLINE_PRE_END_REGISTER
    STRUCT  cmd_legendcup_offline_pre_end_register
    {
        __int64		nLegendCupID;									// 杯赛ID

        STRUCT_INIT(cmd_legendcup_offline_pre_end_register);
    };
    
	////////////////////////////////////杯赛//////end/////////////
	


	////////////////////////////SNS 开始////////////////////////////

	/////////////////////////////Logic2View/////////////////////////
	// GLOGICCMD_SNS_MSG_DATA 聊听消息
	STRUCT gamelogic_sns_msg_data
	{
		int dwMsgID;										// 消息ID
		int dwTime;											// 发送时间
		int dwFromAccountID;								// 发送者账号ID
		sp_char_array(szFromName, ACTORNAME_MAXSIZE);		// 发送者名字
		int dwToAccountID;									// 目标账号ID
		sp_char_array(szFont, FONT_NAME_MAXSIZE);			// 字体名称
		int nFontSize;										// 字体大小
		int nFontColor;										// 字体颜色
		sp_char_array(szContent, 1024);						// 聊听内容

		STRUCT_INIT(gamelogic_sns_msg_data);
	};

	// GLOGICCMD_SNS_UPDATE_RELATIONSHIP 好友关系
	STRUCT gamelogic_sns_update_relationship
	{
		int dwUserID;									// 玩家ID
		BYTE nStatus;									// 关系状态
		sp_char_array(szActorName, REMARK_NAME_MAXSIZE);	// 好友备注

		STRUCT_INIT(gamelogic_sns_update_relationship);	
	};

	// GLOGICCMD_SNS_BUDDY_STATUS 好友在线状态
	STRUCT gamelogic_sns_buddy_status
	{
		int dwUserID;			// 好友账号ID
		BYTE nStatus;			// 在线状态
		sp_char_array(szStatusStr, STATUS_REVERSE_STR_MAXSIZE);	// 状态保留字符串
		
		int dwLastLogout;		// 最后一次登录时间
		
		STRUCT_INIT(gamelogic_sns_buddy_status);
	};

	// GLOGICCMD_SNS_BUDDYGROUP_INFO 所有好友分组信息个数
	STRUCT gamelogic_sns_buddygroup_list_head
	{
		int nCount;		// 好友分组个数

		STRUCT_INIT(gamelogic_sns_buddygroup_list_head);
	};
	// 好友分组详细信息
	STRUCT gamelogic_sns_buddygroup
	{
		BYTE nRelationStatus;										// 好友分组关系，EMSNS_BUDDY_BUDDYGROUP_RELATIONSHIP
		int nBuddyGroupID;											// 好友分组ID	
		sp_char_array(szBuddyGroupName,BUDDYGROUP_NAME_MAXSIZE);    // 好友分组名称

		STRUCT_INIT(gamelogic_sns_buddygroup)
	};

	//GLOGICCMD_SNS_BUDDYRELATION_LIST 好友关系列表
	STRUCT gamelogic_sns_buddyrelation_list_head
	{
		int nCount;		// 好友个数
		STRUCT_INIT(gamelogic_sns_buddyrelation_list_head);
	};
	// 好友关系详情
	STRUCT gamelogic_sns_buddyrelation
	{
		int dwUserID1;									// 玩家1ID
		int dwUserID2;									// 玩家2ID
		int dwBuddyGroupID;								// 好友分组ID
		BYTE nStatus;									// 关系状态
		sp_char_array(szRemark, REMARK_NAME_MAXSIZE);	// 好友备注
		BYTE	nIntimacy;								// 亲密度

		STRUCT_INIT(gamelogic_sns_buddyrelation);
	};

	// GLOGICCMD_SNS_ALL_BUDDY_INFO  所有好友信息个数
	STRUCT gamelogic_sns_buddy_list_head
	{
		int nCount;		// 好友个数
		STRUCT_INIT(gamelogic_sns_buddy_list_head);
	};
	// 好友详细信息
	STRUCT gamelogic_sns_buddy
	{
		int nUserID;									// 账号ID	
		int nPermission;								// 权限
		sp_char_array(szMood,MOOD_CONTENT_MAXSIZE);		// 心情
		sp_char_array(szName,BUDDYGROUP_NAME_MAXSIZE);	// 角色名称
		int nLevel;										// 等级
		BYTE nStatus;									// 在线状态
		sp_char_array(szStatusStr,	STATUS_REVERSE_STR_MAXSIZE);// 状态保留字符串
		int dwGameID;									// 游戏ID，属于哪个游戏
		int dwWorldID;									// 世界ID
		int dwServerID;									// 场景服ID
		int dwSceneID;									// 场景ID
		int dwPdbID;									// pdbid，玩家所在服的角色id
		int dwHeadID;									// 头像ID
		int dwLastLogout;								// 最后下线时间
		int	nMatchNum;									// 总场数
		int	nWinNum;									// 胜场数
		int	nRankIconID;								// 段位头像ID
		BYTE nSex;										// 性别
		sp_char_array(szWorldName, GAMEWORLD_NAME_LEN);	// 联盟名称
		sp_char_array(szClan, CLAN_NAME_SIZE);			// 联盟名称
		sp_char_array(szTitle, TITLE_NAME_SIZE);		// 称号
		sp_char_array(szRankName, RANK_NAME_MAXSIZE);	// 段位名称

		STRUCT_INIT(gamelogic_sns_buddy)
	};

	// GLOGICCMD_SNS_BUDDY_REMARK 好友备注
	STRUCT gamelogic_sns_buddy_remark
	{
		int dwUserID1;
		int dwUserID2;
		sp_char_array(szRemark, REMARK_NAME_MAXSIZE);		// 备注
		
		STRUCT_INIT(gamelogic_sns_buddy_remark);
	};

	// GLOGICCMD_SNS_USERINFO 用户信息
	STRUCT gamelogic_sns_userinfo
	{
		int dwUserID;									// 用户ID
		int nPermission;								// 权限
		sp_char_array(szMood, MOOD_CONTENT_MAXSIZE);	// 备注

		STRUCT_INIT(gamelogic_sns_userinfo);
	};

	// GLOGICCMD_SNS_BUDDYINFO_CHNAGE_NOTIFY
	STRUCT gamelogic_sns_buddyinfo_change_notify
	{
		int dwUserID;			// 信息发生变更的好友用户ID
		int nType;				// 变更类型

		STRUCT_INIT(gamelogic_sns_buddyinfo_change_notify);
	};

	// GLOGICCMD_SNS_INVITE_PLAYGAME_NOTIFY
	STRUCT gamelogic_sns_invite_playgame_notify
	{
		int nRoomID;
        int dwInviteActorID;
		sp_char_array(szInviterName, ACTORNAME_MAXSIZE);

		STRUCT_INIT(gamelogic_sns_invite_playgame_notify);
	};

	// GLOGICCMD_SNS_INVITE_MATCH_TEAM_NOTIFY
	STRUCT gamelogic_sns_invite_match_team_notify
	{
		int dwTargetActorID;

		STRUCT_INIT(gamelogic_sns_invite_match_team_notify);
	};

	// GLOGICCMD_SNS_UPDATE_SCENE_NOTIFY
	STRUCT gamelogic_sns_update_scene_notify
	{
		int nUserID;
		int nWroldID;
		int nSceneID;

		STRUCT_INIT(gamelogic_sns_update_scene_notify);
	};

	/////////////////////////////View2Logic/////////////////////////
	// 登陆
	STRUCT gameview_sns_login
	{
		PDBID   nPdbid;									// 玩家pDbid
		int nLevel;										// 玩家等级
		sp_char_array(szName,BUDDYGROUP_NAME_MAXSIZE);	// 角色名称
		int nHeadID;									// 头像ID
		int	nRankIconID;								// 段位头像
		sp_char_array(szRankName, GAME_NAME_MAXSIZE);	// 段位描述
		int	nMatchNum;									// 总场数
		int	nWinNum;									// 胜场数
		sp_char_array(szClan, CLAN_NAME_SIZE);			// 联盟名称
		sp_char_array(szTitle, TITLE_NAME_SIZE);		// 称号

		STRUCT_INIT(gameview_sns_login)
	};

	// 好友操作
	STRUCT gameview_sns_update_buddy
	{
		BYTE nOptType;									// 操作类型，EMSNS_BUDDY_UPDATE_TYPE
		int   dwAddAccountID;							// 被添加好友的账号ID
		sp_char_array(szActorName, ACTORNAME_MAXSIZE);	// 好友名字

		STRUCT_INIT(gameview_sns_update_buddy);
	};
	// 回应添加好友的请求
	STRUCT gameview_sns_rsp_add_buddy
	{
		int  dwOtherAccountID;								// 被添加好友ID
		BYTE nFlag;											// 是否同意,EMSNS_RSP_ADDBUDDY_TYPE
		sp_char_array(szActorName, ACTORNAME_MAXSIZE);		// 好友名字

		STRUCT_INIT(gameview_sns_rsp_add_buddy);
	};
	// 好友分组操作
	STRUCT gameview_sns_update_buddygroup
	{
		BYTE			nOptType;						// 操作类型，EMSNS_BUDDYGROUP_UPDATE_TYPE
		int	dwBuddyGroupID;								// 分组ID
		sp_char_array(szName, BUDDYGROUP_NAME_MAXSIZE);	// 新的分组名字

		STRUCT_INIT(gameview_sns_update_buddygroup);
	};
	// 修改好友备注
	STRUCT gameview_sns_update_remark
	{
		int dwUserID;										// 好友账号ID
		sp_char_array(szActorName, REMARK_NAME_MAXSIZE);	// 角色名字
		sp_char_array(szRemark, REMARK_NAME_MAXSIZE);		// 备注名

		STRUCT_INIT(gameview_sns_update_remark);
	};
	// 发送聊听信息
	STRUCT gameview_sns_send_msg_data
	{
		int dwMsgID;									// 消息ID（服务器填写）
		int dwTime;										// 发送时间（服务器填写）
		int dwFromAccountID;							// 发送者账号ID
		sp_char_array(szFromName, ACTORNAME_MAXSIZE);	// 发送者名字（服务器填写）
		int dwToAccountID;								// 目标账号ID
		sp_char_array(szFont, FONT_NAME_MAXSIZE);		// 字体名字
		int nFontSize;									// 字体大小
		int nFontColor;									// 字体颜色
		sp_char_array(szContent, 1024);					// 聊听内容

		STRUCT_INIT(gameview_sns_send_msg_data);
	};
	// 请求一个好友的信息
	STRUCT gameview_sns_req_buddy_info
	{
		int dwUserID;
		
		STRUCT_INIT(gameview_sns_req_buddy_info);
	};
	// 请求一个好友的关系信息
	STRUCT gameview_sns_req_buddyrelation
	{
		int dwUserID;
		
		STRUCT_INIT(gameview_sns_req_buddyrelation);
	};
	// 根据名字添加好友
	STRUCT gameview_sns_req_add_buddy_by_name
	{
		sp_char_array(szActorName, ACTORNAME_MAXSIZE);	// 需要添加的角色名字

		STRUCT_INIT(gameview_sns_req_add_buddy_by_name);
	};
	// 请求用户信息
	STRUCT gameview_sns_req_userinfo
	{
		int dwUserID;

		STRUCT_INIT(gameview_sns_req_userinfo);
	};
	// 请求修改权限
	STRUCT gameview_sns_req_change_permission
	{
		int nPermission;

		STRUCT_INIT(gameview_sns_req_change_permission);
	};
	// 请求修改心情
	STRUCT gameview_sns_req_change_mood
	{
		sp_char_array(szMood,MOOD_CONTENT_MAXSIZE);		// 心情

		STRUCT_INIT(gameview_sns_req_change_mood);
	};
	// 请求改变好友分组
	STRUCT gameview_sns_req_change_buddygroup
	{
		int dwUserID;
		int	dwBuddyGroupID;								// 分组ID
		STRUCT_INIT(gameview_sns_req_change_buddygroup);
	};
	// 请求更新好友在线状态
	STRUCT gameview_sns_req_update_buddystatus
	{
		BYTE nStatus;
		sp_char_array(szReverse, STATUS_REVERSE_STR_MAXSIZE);

		STRUCT_INIT(gameview_sns_req_update_buddystatus);
	};
	// 请求邀请一起游戏
	STRUCT gameview_sns_req_invite_play_game
	{
		int nUserID;
		int nRoomID;
		sp_char_array(szInviterName, ACTORNAME_MAXSIZE);

		STRUCT_INIT(gameview_sns_req_invite_play_game);
	};
	// 请求邀请一起组队
	STRUCT gameview_sns_req_invite_match_team
	{
		int nUserID;
		sp_char_array(szInviterName, ACTORNAME_MAXSIZE);

		STRUCT_INIT(gameview_sns_req_invite_match_team);
	};


	////////////////////////////SNS 结束////////////////////////////

	///////////////////////MAIL view2c++ 开始///////////////////////
	// view2c++ 请求邮件基础信息列表 GVIEWCMD_MAIL_REQUESTBASEDATALIST
	// c++2view 请求邮件基础信息列表
	STRUCT gameview_mail_req_basedatalist
	{
		int	nPDBID;
		int nCount;

		STRUCT_INIT(gameview_mail_req_basedatalist);
	};

	STRUCT gameview_mail_baseinfo
	{
		__int64 nMailID;								// 邮件id
		BYTE nType;										// 邮件类型
		BYTE nSourceType;								// 邮件来源
		sp_char_array(szTitle, MAIL_TITLE_MAXSIZE);		// 标题					
		sp_char_array(szSenderName, ACTORNAME_MAXSIZE);	// 发件者姓名
		int	nSenderPDBID;								// 发送者PDBID
		BYTE nIsPlusData;								// 是否有附件
		BYTE nIsRead;									// 是否已读
		int nRecvTime;									// 发件时间
		int nLifeTime;									// 生命时长 (单位:小时)
		BYTE nReadFlag;									// 阅读邮件标识

		STRUCT_INIT(gameview_mail_baseinfo);
	};

	// view2c++ 请求邮件详细信息 GVIEWCMD_MAIL_REQUESTDETAILDATA
	STRUCT gameview_mail_req_detaildata
	{
		__int64	nMailID;

		STRUCT_INIT(gameview_mail_req_detaildata);
	};

	// view2c++ 请求快速取件 GVIEWCMD_MAIL_REQUESTFASTRECVPLUS
	// c++2view 请求快速取件
	STRUCT gameview_mail_req_fastrecvplus
	{
		__int64	nMailID;

		STRUCT_INIT(gameview_mail_req_fastrecvplus);
	};

	// c++2view 响应阅读邮件 GVIEWCMD_MAIL_RESPONSEREADMAIL
	STRUCT gameview_mail_goodsinfo
	{
		int nGoodsID;
		int nCount;

		STRUCT_INIT(gameview_mail_goodsinfo);
	};

	STRUCT gameview_mail_rsp_detaildata
	{
		__int64 nMailID;									// 邮件id
		sp_char_array(szContext, MAIL_CONTENT_MAXSIZE);		// 内容
		int  nPlusMoney;									// 附送金钱
		int  nPlusExp;										// 附送经验
		BYTE  nPlusGoodsNum;								// 附件物品的个数
		sp_struct_array(gameview_mail_goodsinfo, sGoodsList, MAIL_GOODS_MAX_NUM); // 附加物品

		STRUCT_INIT(gameview_mail_rsp_detaildata);
	};
	// c++2view 玩家未读邮件总数 GVIEWCMD_MAIL_NEWMAILCOUNT
	STRUCT gameview_mail_newmailcount
	{
		int	nCount;
		int nPlusCount;

		STRUCT_INIT(gameview_mail_newmailcount);
	};

	// c++2view 邮件收件成功 GVIEWCMD_MAIL_RECVPLUSSUCCESS
	STRUCT gameview_mail_recvsuccess
	{
		__int64 nMailID;

		STRUCT_INIT(gameview_mail_recvsuccess);
	};

	///////////////////////MAIL view2c++ 结束///////////////////////


    // c++2view 设置机器人模式 例如被嘲讽后不能自己操作 GVIEWCMD_SET_ROBOT_MODE
    STRUCT gameview_set_robot_mode
    {
        int nEnable; // 1--进入机器人模式 0--离开机器人模式

        STRUCT_INIT(gameview_set_robot_mode);
    };

    // c++2view 设置摄像机高度(Y坐标)
    STRUCT gameview_set_camera_height
    {
        float fHeight;

        STRUCT_INIT(gameview_set_camera_height);
    };

	///////////////////////////////选区   开始///////////////////////////////////////////
	// C++2View 大区列表信息  GVIEWCMD_REQUEST_DISTRICT_LIST
	STRUCT gameview_district_info_list_head
	{
		int nCount;

	    STRUCT_INIT(gameview_district_info_list_head);
	};

	//C++2View  大区列表信息
	STRUCT gamelogic_district_info
	{
		int nWorldID;
	    int nGroup;			 // 区组序号
    	int nDistrict;		     // 版本分类序号
    	int nServer;		     // 游戏世界区序号
    	int nLatencty;	     // 网络延迟
	    int nState;			     // 状态 0 良好 1 繁忙 2 爆满 3 维护
	    int nRecommend;	 // 推荐值 1:推荐 0:无
        int nWorldActors;	 // 所在区角色数
	    int nHid;
	    sp_char_array(szName, GAME_NAME_MAXSIZE);
	    sp_char_array(szGroupName, GAME_NAME_MAXSIZE);
	    sp_char_array(szVerShowName, GAME_NAME_MAXSIZE);
	    sp_char_array(szDesc, GAME_LONGSTR_MAXSIZE);

	    STRUCT_INIT(gamelogic_district_info);
	};

	STRUCT gamelogic_lastEnter_district_info_head
	{
		int nCount;

	    STRUCT_INIT(gamelogic_lastEnter_district_info_head);
	};

	STRUCT gamelogic_lastEnter_district_info
	{
		int nGroup;
	    int nDistrict;
	    int nServer;

		STRUCT_INIT(gamelogic_lastEnter_district_info);
	};

	//View2C++  请求选区
	STRUCT gameview_select_server
	{
		int nID;
	    int nGroup;
    	int nDistrict;
    	int nServer;

	STRUCT_INIT(gameview_select_server);
	};

	//C++2View 下发游戏版本号
	STRUCT gamelogic_game_version
	{
		int nVersion;
	};

    // 返回网络服务供应商信息 GVIEWCMD_NETSERVICE_PROVIDER_RESULT,
    STRUCT gamelogic_netservice_provider_result
    {
        int  nServiceType;
        sp_char_array(szName, GAME_NAME_MAXSIZE);

        STRUCT_INIT(gamelogic_netservice_provider_result);
    };
	//////////////////////////////选区  结束////////////////////////////////////////////

   //////////////////////////////录像////////////////////////////////////////////
	STRUCT gameview_start_screenCapture
	{
		int point_x;
		int point_y;
		int width;
		int height;
		sp_char_array(szTempFilePath, GAME_LONGSTR_MAXSIZE);

		STRUCT_INIT(gameview_start_screenCapture);
	};

	STRUCT gameview_stop_screenCapture
	{
		int nValid;
		sp_char_array(szOutFilePath, GAME_LONGSTR_MAXSIZE);

		STRUCT_INIT(gameview_stop_screenCapture);
	};
    //////////////////////////////////////////////////////////////////////////
	
	STRUCT cmd_self_punish_head
    {
    public:
        int nCount;			

        STRUCT_INIT(cmd_self_punish_head);
    };

    STRUCT cmd_self_punish_info
    {
    public:

        BYTE byPunishType;
        int  nLeftTime;				

        STRUCT_INIT(cmd_self_punish_info);
    };

    STRUCT cmd_match_request_punish_head
    {
    public:
        int bIsTeamRequest;
        int nCount;			

        STRUCT_INIT(cmd_match_request_punish_head);
    };

    STRUCT cmd_match_request_punish_info
    {
    public:

        BYTE byPunishType;
        int  nLeftTime;
        sp_char_array(szName, ACTORNAME_MAXSIZE);// 获取角色名称

        STRUCT_INIT(cmd_match_request_punish_info);
    };


    STRUCT cmd_room_banInfo_head
    {
    public:
        int nCount;
        int nCampMaxPlayerCount;

        STRUCT_INIT(cmd_room_banInfo_head);
    };

    STRUCT cmd_room_banInfo_sub
    {
    public:
        int nPos;
        int nHeroID;

        STRUCT_INIT(cmd_room_banInfo_sub);
    };

    STRUCT cmd_match_close_or_open
    {
    public:
        int bActorLogin;
        int bOpen;
        int nMatchType;

        STRUCT_INIT(cmd_match_close_or_open);
    };

	// 光效预加载信息 GVIEWCMD_MATCH_SEND_PRE_LOAD_LIGHTING_EFFECT_INFO
	STRUCT gamelogic_pre_load_light_info
	{
		int nLightType;//光效类型
		int nMask;//标志位
		sp_int_array(LightEffectIDList, 128);//光效ID列表

		STRUCT_INIT(gamelogic_pre_load_light_info);
	};

	STRUCT cmd_switch_bullet_count_panel
	{
		int nOpen;  // 0-关 1-开
		int nMaxCount; // 最大数量

		STRUCT_INIT(cmd_switch_bullet_count_panel)
	};

	// 客户端主动切换场景 GVIEWCMD_MIGRATE_SWITCH_SCENE
	STRUCT gamelogic_migrate_switch_scene
	{
	public:
		int nGameWorldID;
		int nServerID;
		int nSceneID;

		STRUCT_INIT(gamelogic_migrate_switch_scene);
	};

	// 大逃杀战场-安全圈信息  GVIEWCMD_WAR_SAFETYZONE_INFO
	STRUCT gamelogic_war_safetyzone_info
	{
	public:
		float fCenterOfNoxZoneX;	// 当前毒气圈的圆心坐标X
		float fCenterOfNoxZoneY;	// 当前毒气圈的圆心坐标Y
		float fCenterOfNoxZoneZ;	// 当前毒气圈的圆心坐标Z
		float fRadiusOfNoxZone;		// 当前毒气圈的半径
		float fCenterOfSafetyZoneX;	// 当前安全圈圆心坐标
		float fCenterOfSafetyZoneY;	// 当前安全圈圆心坐标
		float fCenterOfSafetyZoneZ;	// 当前安全圈圆心坐标
		float fRadiusOfSafetyZone;	// 当前安全圈的半径
		int nSpreadBeginTick;		// 扩散开始的时间
		int nSpreadTime;			// 扩散时间
		int isSpreadBegin;			// 是否毒气扩散开始

		STRUCT_INIT(gamelogic_war_safetyzone_info);
	};

	// 玩家掉线重连  GVIEWCMD_WAR_PLAYER_RECONNECTION
	STRUCT gamelogic_war_player_reconnection
	{
	public:
		int actorID;
		int heroID;
		sp_char_array(actorName, GAME_NAME_MAXSIZE);

		STRUCT_INIT(gamelogic_war_player_reconnection);
	};

	// 大逃杀战场-毒气扩散倒计时信息 GVIEWCMD_WAR_NOX_SPREAD_CD
	STRUCT gamelogic_war_nox_spread_cd
	{
	public:
		int nSpreadCDTick;			// 扩散倒计时开始时间（时间服务器Tick数）
		int nSpreadTotalCDTime;		// 扩散倒计时总时间

		STRUCT_INIT(gamelogic_war_nox_spread_cd);
	};

	// 服务器War直接发消息到 GVIEWCMD_WAR_SERVER_TO_VIEW_MSG
	STRUCT gamelogic_war_server_toview_msg
	{
	public:
		int nKeyAction;
		// data...

		STRUCT_INIT(gamelogic_war_server_toview_msg);
	};

} // namespace GameLogic;

using namespace GameLogic;