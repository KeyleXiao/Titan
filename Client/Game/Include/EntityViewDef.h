/*******************************************************************
** 文件名:	EntityViewDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	6/21/2014
** 版  本:	1.0
** 描  述:	

定义显示层和逻辑层通讯的实体相关命令
** 修  改:	定义命令相关的结构体, 减少与显示层的耦合		彭璐		11/13/2014
********************************************************************/


#pragma once

#include "GameDef.h"
#include "ManagedCode.h"
#include "CampDef.h"
#include "SpellDef_ManagedDef.h"
#include "DamageDef_ManagedDef.h"
#include "EffectCommon_ManagedDef.h"
#include "War_ManagedDef.h"
#include "DamageDef_ManagedDef.h"
#include "Slot_ManagedDef.h"
#include "Chat_ManagedDef.h"
#include "Entity_ManagedDef.h"
#include "Clan_ManagedDef.h"
#include "PlayerSignal_ManagedDef.h"
#include "Team_ManagedDef.h"
#include "Order_ManagedDef.h"

using namespace War;
using namespace SPELL;
namespace GameLogic
{
    // 实体状态
    ENUM EntityState
    {
        Standing = 0,
            Jumping  = 1,
            Dead     = 2,
            Swimming = 3,
            Floating = 4,
			WallWalking = 5,
            Glide = 6,
			MaxCount = 7,
			
    };

    // 托管代码和非托管代码共用一个定义 
    ENUM EntityLogicDef
    {
        INVALID_ENTITY_ID =  0,	// 无效实体

            /*****************************************************************************
            * 逻辑层调用显示层
            ******************************************************************************/
            // 开始命令
            ENTITY_TOVIEW_BEGIN =		(ENTITY_VIEW_CMD_OFFSET+1),

            // 加载模型
            ENTITY_TOVIEW_LOAD_MODEL,
            // 加载预制体
            ENTITY_TOVIEW_LOAD_PREFAB,
            // 加载皮肤
            ENTITY_TOVIEW_LOAD_SKIN,
            // 显示层创建实体
            ENTITY_TOVIEW_CREATE_ENTITY_VIEW,
            // 显示层销毁实体
            ENTITY_TOVIEW_DESTROY_ENTITY_VIEW,


            //// 同步是否是主角
            //ENTITY_TOVIEW_SYNC_HERO =		(ENTITY_VIEW_CMD_OFFSET+1),
            // 设置名字
            ENTITY_TOVIEW_SYNC_NAME,

            // 同步是否显示血条
            ENTITY_TOVIEW_SYNC_BLOODSHOW,

            // 同步血量,带2个参数,param1为当前血量 param2为最大血量
            ENTITY_TOVIEW_SYNC_BLOOD,

            //// 同步等级
            //ENTITY_TOVIEW_SYNC_HEROVISION_LEVEL,

            // 同步实体位置
            ENTITY_TOVIEW_SYNC_POS,

            // 操作同步
            ENTITY_TOVIEW_SYNC_CTRL,

            // 朝向锁定目标
            ENTITY_TOVIEW_LOOK_AT,

            // 添加光效  
            ENTITY_TOVIEW_ADD_LIGHTING,

            // 移除光效
            ENTITY_TOVIEW_REMOVE_LIGHTING,

            // 播放动画
            ENTITY_TOVIEW_PLAY_ANIMATION,

			// AI翻滚
			ENTITY_TOVIEW_AI_ROLL,

            // 切换状态
            ENTITY_TOVIEW_SWITCH_STATE,

            // 停止移动
            ENTITY_TOVIEW_STOP_MOVE,

            // 继续移动
            ENTITY_TOVIEW_RESUME_MOVE,

            // 死亡复活消息
            ENTITY_TOVIEW_RELIVE,

            // 冲锋消息
            ENTITY_TOVIEW_CHARGE,

            // 通知改变槽位信息
            ENTITY_TOVIEW_CHANGE_SLOT,

            // 通知槽位信息映射快捷键
            ENTITY_TOVIEW_SLOT_MAPED_SHORTCUT,

            // 通知移除槽位信息
            ENTITY_TOVIEW_REMOVE_SLOT,

            // 设置技能相关属性
            ENTITY_TOVIEW_SETSPELLATTRIBUTE,

            // 施法前请求技能目标信息(针对准备较长时间类型技能，用于修正施法时和实际效果不要偏差太远)
            ENTITY_TOVIEW_SPELL_RECALC_TARGET_INFO,

            // 设置施法技能点(显示层转向用)
            ENTITY_TOVIEW_SETCASTSPELL_POS,

            // 中断正在播放的攻击动作
            ENTITY_TOVIEW_BREAK_ATTACK_ANIMATION,

            // 设置buff(增加buff,移除buff)
            ENTITY_TOVIEW_SET_BUFF,

            // 进入载具
            ENTITY_TOVIEW_ENTER_TANK,

            // 设置技能叠加
            ENTITY_TOVIEW_SET_SPELL_OVERLAY,

            // 设置更新人物战场信息
            ENTITY_TOVIEW_SET_WAR_PERSONINFO,

            // 设置更新怪物战场信息
            ENTITY_TOVIEW_SET_WAR_MONSTERINFO,

            // 效果-浮空
            ENTITY_TOVIEW_EFFECT_FLIGHT,

            // 效果-扫射
            ENTITY_TOVIEW_EFFECT_STRAFE,

            // 调整速度
            ENTITY_TOVIEW_SET_SPEED,

            // 阵营成员UID
            ENTITY_TOVIEW_CAMP_PLAYER_UIDINFO,

            // 实体进入视野
            ENTITY_TOVIEW_ENTITY_ENTER_VIEW,

            // 实体离开视野
            ENTITY_TOVIEW_ENTITY_LEAVE_VIEW,

            // 设置动画
            ENTITY_TOVIEW_ENTITY_SET_ANIMATION,

            //传送（强制设置位置）
            ENTITY_TOVIEW_SYNC_TRANSPORT,

            // 更换皮肤
            ENTITY_TOVIEW_CHANGE_SKIN,

            //广播战场重要实体被击杀
            ENTITY_TOVIEW_SET_WAR_BDIMPENTITYINFO,

            // 广播战场击杀数
            ENTITY_TOVIEW_SET_WAR_BROADKILLINFO,

            // 战场结束显示失败胜利
            ENTITY_TOVIEW_SET_WAR_SHOW_WIN_OR_FIAL,

            // 战场结束人物信息处理
            ENTITY_TOVIEW_SET_WAR_ENDINFO,

            // 天赋点数
            ENTITY_TOVIEW_SET_TALENT_POINT,

            // 显示层需要天赋信息
            ENTITY_TOVIEW_UPDATE_TALENT,

            //通知显示层天赋状态
            ENTITY_TOVIEW_TALENT_STATE,

            // 小地图消息显示
            ENTITY_TOVIEW_MINIMAP_INFO,

            // 替换槽位信息
            ENTITY_TOVIEW_REPLACE_SLOT,

            //更新实体的数值属性
            ENTITY_TOVIEW_UPDATE_INTEGERPROPERTY,

            // 小地图图标显示
            ENTITY_TOVIEW_MINIMAP_ICON,


            //龙珠显示服务器坐标
            ENTITY_TOVIEW_DRAGON_SHOW_POSITION,


            // 伤害信息
            ENTITY_TOVIEW_DAMAGE,

            // 不可转向
            ENTITY_TOVIEW_NOTURNTO,

            //出生时摄像机欧拉角
            ENTITY_TOVIEW_CAMERA_ANGLE,

            // 停止移动操作
            ENTITY_TOVIEW_STOP_MOVE_HANDLE,

            // 人物死亡复活时间
            ENTITY_TOVIEW_RELIVE_TIME,

            // 重要实体复活
            ENTITY_TOVIEW_IMPORTANT_MONSTER_RELIVE,

            // 重要实体死亡文字广播
            ENTITY_TOVIEW_IMPORTANT_MONSTER_DIE_TXT,

            // 删除所有小地图信息
            ENTITY_TOVIEW_CLEAR_ALL_MINIMAP_INFO,

            // 实体死亡
            ENTITY_TOVIEW_ENTITY_DEAD,

			// 重登陆实体死亡
			ENTITY_TOVIEW_RELOGIN_ENTITY_DEAD,

            // 进入警戒/非警戒状态
            ENTITY_TOVIEW_GOTO_GUARD,

            // 禁用技能
            ENTITY_TOVIEW_DISABLE_SPELL,

            // 实体战斗小地图信息
            ENTITY_TOVIEW_ENTITY_FIGHT_MINIMAP_INFO,

            // 显示提示文字
            ENTITY_TOVIEW_SHOW_TIPS_TEXT,

            // 改变操作方式
            ENTITY_TOVIEW_CHANGE_OPERATION,

            // 魔法类型
            ENTITY_TOVIEW_MAGIC_TYPE,

            // 移动位置
            ENTITY_TOVIEW_MOVE_POS,

            // 停止移动位置
            ENTITY_TOVIEW_STOP_MOVE_POS,

            // 转向方向
            ENTITY_TOVIEW_TURN_TO_DIRECTION,

            // 设置技能槽
            ENTITY_TOVIEW_SET_SPELL_SLOT,

            // 技能点数
            ENTITY_TOVIEW_SPELL_POINT,

            // 隐藏尸体
            ENTITY_TOVIEW_HIDE_BODY,

            // 战场人物物品信息
            ENTITY_TOVIEW_SET_WAR_PERSON_GOOD_INFO,

            // 连击
            ENTITY_TOVIEW_COMBO,

            // 设置是否能够购买物品
            ENTITY_TOVIEW_SET_CAN_BUY_OR_SELL,

            // 设置是否能够撤销操作
            ENTITY_TOVIEW_SET_CANCEL_OPRATION,

            // 广播玩家技能状态
            ENTITY_TOVIEW_PLAYER_SKILL_STATE,

            // 设置实体透明度
            ENTITY_TOVIEW_SET_ALPHA,

            // 禁止鼠标带动人物转向
            ENTITY_TOVIEW_FORBID_MOUSE_TURN_TO,

            // 播放声音
            ENTITY_TOVIEW_PLAY_SOUND,

            // 将连杀数发向显示层 显示图片
            ENTITY_TOVIEW_SHOW_KILL_ICON,

            // 第一次请求战绩表数据完成通告显示层
            ENTITY_TOVIEW_REQUEST_PERSONINFO_COMPLETE,

            // 充能信息改变
            ENTITY_TOVIEW_CHARGE_COUNT_CHANGE,

            // 改变上帝模式
            ENTITY_TOVIEW_CHANGE_GOD_EYE_MODE,


            // 创建战后统计表
            ENTITY_TOVIEW_END_CREATE_RECORD,

            // 向CS发送所需要的UID和PBUID
            ENTITY_TOVIEW_SEND_UIDANDPUBUID,

            // 画技能贴花
            ENTITY_TOVIEW_DRAW_SPELL_DECAL,

            // 魔法影响技能
            ENTITY_TOVIEW_MP_EFFECT_SPELL,

            // 镜头自动转向攻击对象
            ENTITY_TOVIEW_ROTATE_CAMERA_TO_TARGET,

            // 设置英雄熟练度数据记录信息
            ENTITY_TOVIEW_SET_MATCH_HEROSCORE_RECORD,

            // 改变翅膀状态
            ENTITY_TOVIEW_CHANGE_WING_STATE,

            // 显示进度条
            ENTITY_TOVIEW_PROGRESS_BAR,

            // 清除进度条
            ENTITY_TOVIEW_CLEAR_PROGRESS_BAR,

            // 不可选择
            ENTITY_TOVIEW_NO_SELECT,

            // 显示提示UI
            ENTITY_TOVIEW_SHOW_PROMPT_UI,

            // 技能打断
            ENTITY_TOVIEW_SPELL_BREAK,

            // 发送全部天赋页的信息
            ENTITY_TOVIEW_SET_TALENT_PAGE_INFO_ALL,

            // 镜头跟随 
            ENTITY_TOVIEW_STICKY_CAMERRA,

            // 将英雄信息发送到显示层
            ENTITY_TOVIEW_HERO_INFO,

			// 将英雄更新信息发送到显示层
			ENTITY_TOVIEW_UPDATE_HERO_INFO,

            // 将玩家信息发送到显示层
            ENTITY_TOVIEW_PLAYER_BASE_INFO,

            // 返回升星前后英雄信息
            ENTITY_TOVIEW_UPGRADESTARTAPROANDEXPEND,

            // 返回给显示层某个英雄的详细信息
			ENTITY_TOVIEW_RETURN_HERO_DETAIL_INFO,

            // 返回给显示层升星成功
            ENTITY_TOVIEW_UPGRADE_SUCESS,

            // 获取未拥有英雄
            ENTITY_TOVIEW_RETURN_NOHAVE_HERO,

            // 更新显示层卡牌数据
            ENTITY_TOVIEW_RETURN_UPGRADE_CARD_INFO,
            // 发送全部符文页的信息
            ENTITY_TOVIEW_SET_RUNE_PAGE_INFO_ALL,
			// 更新激活单个符文
			ENTITY_TOVIEW_HERO_RUNESTOREUPDATE,

			// 更新战绩总览数据
			ENTITY_TOVIEW_UPDATE_WARSTATISTIC_TOTAL,
			// 更新历史战绩数据
			ENTITY_TOVIEW_UPDATE_WARSTATISTIC_WAR,

            // 战场结束返回玩家获得的奖励
            ENTITY_TOVIEW_RETURN_PLAYER_AWARK,

            // 静态场景英雄升级返回玩家英雄信息
            ENTITY_TOVIEW_RETURN_STATICHERO_UPGRADE,

            // 通知显示层是否显示可升级卡牌提示
            ENTITY_TOVIEW_HINT_PLAYER_UPGRADE_STAR,

			// 通知显示层更新首胜提示
			ENTITY_TOVIEW_FIRSTWIN_NOTICE,

            // 显示称号信息
            ENTITY_TOVIEW_SHOW_TITLE,

			// 发送全部天赋页的信息
			ENTITY_TOVIEW_HERO_TALENTINFO,

			// 更新激活单个英雄天赋
			ENTITY_TOVIEW_HERO_TALENTUPDATE,

            // 设置目标范围提示
            ENTITY_TOVIEW_SET_OBJ_RANGE_TIP,

			// 显示其他玩家的符文信息
			ENTITY_TOVIEW_SET_OTHER_RUNE_PAGE_INFO_ALL,

            // 右上角玩家战绩以及阵营信息
            ENTITY_TOVIEW_WAR_HERO_INFO_SHOW,

            // 玩家自己和视野中的实体等级改变发出的事件
            ENTITY_TOVIEW_SYNC_HEROVISION_LEVEL,

            // 主角场外等级改变事件
            ENTITY_TOVIEW_SYNC_MAINHERO_LEVEL_OUTWAR,

            // 大厅显示技能描述
            ENTITY_TOVIEW_MATCH_ROOM_SHOW_SKILL_DESC,

            // 冷却时间改变
            ENTITY_TOVIEW_COOL_TIME,

			// 更新宝箱信息
			ENTITY_TOVIEW_UPDATE_CHEST_INFO,
			// 打开宝箱结果
			ENTITY_TOVIEW_OPEN_CHEST_RESULT,
			// 返回宝箱随机卡牌结果
			ENTITY_TOVIEW_CHEST_RANDCARD_RESULT,

            // 通知显示层请求PK提示
            ENTITY_TOVIEW_REQUEST_PK,

            // 通知显示层显示PK信息
            ENTITY_TOVIEW_SHOW_PK_INFO,
            // 发送全部英雄皮肤信息
            ENTITY_TOVIEW_HERO_SKIN_INFO,
            // 更新皮肤信息
            ENTITY_TOVIEW_HERO_SKIN_UPDATE,

            // 通知显示层开始PK
            ENTITY_TOVIEW_START_PK,

			// 更新商城购买物品数量信息
			ENTITY_TOVIEW_UPDATE_POINTSHOP_BUYNUM_INFO,
			// 刷新商城购买物品数量信息
			ENTITY_TOVIEW_REFLUSH_POINTSHOP_BUYNUM_INFO,
			// 商城购买物品结果
			ENTITY_TOVIEW_POINTSHOP_BUY_RESULT,
			// 商城在售卡牌列表
			ENTITY_TOVIEW_POINTSHOP_UPDATE_CARDLIST,
            // 角色信息改变改变
            ENTITY_TOVIEW_ACTOR_INFO_CAHNGE,
			// xp技能随机到自己去开始倒计时
			ENTITY_TOVIEW_XP_RANDOMSELF_BEGINCOOL,
			// 发送已经激活的英雄天赋
			ENTITY_TOVIEW_HERO_ACTIVE_TALENT_INRECONNECT,
			// 特殊实体搜索状态改变
			ENTITY_TOVIEW_SPECIAL_ENTITY_SEARCH_STATE_CHANGE,

            // 改变轻功状态
            ENTITY_TOVIEW_CHANGE_DODGE_STATE,

			// 龙珠指定实体属性信息
			ENTITY_TOVIEW_ENTITY_PROPERTY_DRAGONBALL,

            // 显示选择目标
            ENTITY_TOVIEW_SHOW_CHOOSE_TARGET,

            // 透视显示
            ENTITY_TOVIEW_PERSPECTIVE_SHOW,

            // 改变狙击镜
            ENTITY_TOVIEW_CHANGE_SNIPERSCOPE,

			//玩家特殊贡献信息同步(惊现救治 夺命控制 团控)	
			ENTITY_TOVIEW_SPECCIAL_CONTRIBUTION,

			//玩家改名结果
			ENTITY_TOVIEW_RENAME_RESUNLE,

			// 增益卡信息
			ENTITY_TOVIEW_BENEFITCARD,


			// 更新玩家是否成年人
			ENTITY_TOVIEW_UPDATE_PLAYER_ADULT,

			// 更新玩家游戏状态
			ENTITY_TOVIEW_UPDATE_GAMESTATE,

		    ////////////////////////////////////皇冠///////////////////
			// 初始数据全部宝石更新
			ENTITY_TOVIEW_CROWNG_ALLGEMSTONE_UPDATE,			
			// 初始数据全部皇冠页更新
			ENTITY_TOVIEW_CROWNG_ALLCROWNPAGE_UPDATE,
			// 宝石变化同步
			ENTITY_TOVIEW_CROWNG_GEMSTONE_UPDATE,	
			// 宝石产出同步
			ENTITY_TOVIEW_CROWNG_GEMSTONE_PRODUCE,	
			// 开窗扣除点券成功
			ENTITY_TOVIEW_CROWNG_GEMSTONE_WINSUCCESS,	
			 // 其他玩家的宝石镶嵌情况
			ENTITY_TOVIEW_CROWNG_GEMSTONE_OTHER_INLAY,
			// 宝石鉴定结果
			ENTITY_TOVIEW_CROWNG_GEMSTONE_APPRAISALRET,
			////////////////////////////////////皇冠///////////////////

			// 筛选出到点赞平台条件的人
			ENTITY_TOVIEW_WAR_FILTER_SUPPORT_PERSON,
			// 添加标签
			ENTITY_TOVIEW_WAR_ADD_LABEL,
			// 玩家英雄xp技能同步
			ENTITY_TOVIEW_HERO_XP_SKILL_UPDATE,	
			// 玩家英雄xp技能碎片同步
			ENTITY_TOVIEW_HERO_XP_FRAGMENT_UPDATE,	

            // 进入飞檐走壁状态
            ENTITY_TOVIEW_CHANGE_STATE_WALL_WALKING,

			// NPC打开战场商店
			ENTITY_TOVIEW_NPC_OPEN_SHOP,
			// NPC回应
			ENTITY_TOVIEW_NPC_RESPONSE,
			// 关闭NPC对话框
			ENTITY_TOVIEW_NPC_CLOSE_DIALOGBOX,

            // 超级跳
            ENTITY_TOVIEW_LONGJUMP,

            // 滑翔
            ENTITY_TOVIEW_GLIDE,

            // 第一次初始化战场时间
            ENTITY_TOVIEW_INI_WARCONTINUE_TIME,

			// 英雄升星奖励解锁
			ENTITY_TOVIEW_HEROUPSTAR_UNLOCKPRIZE,

            // 同步通知命令
            ENTITY_TOVIEW_NOTIFY_DATA,

			// 标签列表
			ENTITY_TOVIEW_LABEL_LIST,

			// 装备方案列表
			ENTITY_TOVIEW_EQUIPSCHEME_LIST,

			// 天赋激活列表
			ENTITY_TOVIEW_ACTIVE_TALENT_LIST,

			// 挂机提示通知
			ENTITY_TOVIEW_BOTTING_NOTIFY,

			// 战场信息
			ENTITY_TOVIEW_WAR_INFO,

			// 自动前进
			ENTITY_TOVIEW_AUTO_MOVE,

            // 战场信息
            ENTITY_TOVIEW_WAR_END_LEGENDCUP_INFO,

			// 通知view更新本命英雄
			ENTITY_TOVIEW_SET_ALL_LIFEHERO,

			// 更新新手引导数据
			ENTITY_TOVIEW_UPDATE_NOVICEGUIDEDATA,

			// 显示debuff进度条
			ENTITY_TOVIEW_SHOW_DEBUFF_PROGRESS_BAR,

			// 通知view更新玩家历史战绩
			ENTITY_TOVIEW_UPDATE_HISTORY_RECORD,

			// 更新单个本命英雄信息
			ENTITY_TOVIEW_UPDATE_LIFEHERO,

			// 玩家等级解锁奖励
			ENTITY_TOVIEW_ACTOR_UNLOCK,

            // 玩家段位上升或者降低
            ENTITY_TOVIEW_RANK_DWON_OR_UPGRADE,

			// 本命英雄奖励任务数据
			ENTITY_TOVIEW_LIFEHERO_PRIZE_TASKDATA,

            // 通知VIEWNPC装备购买成功
            ENTITY_TOVIEW_NPC_BUY_GOODS_SUCCESS,

            // 显示层接收游戏导航栏
            ENTITY_TOVIEW_PROCESS_NAVGROUP,

            // 显示层接收URL字符串
            ENTITY_TOVIEW_PROCESS_WEBURL,


			// 返回排位赛季详细数据
			ENTITY_TOVIEW_RANK_SEASON_DETAIL,

			// 返回排位赛季历史数据
			ENTITY_TOVIEW_RANK_SEASON_RECORD,

			// 更新宝箱信息
			ENTITY_TOVIEW_UPDATE_EMOJI_INFO,

            ///////////////////////////////////////////////////////////////////////////////
            // ！！！别插到这个命令下，请在上面加发送到显示层命令
            ENTITY_TOVIEW_END,
			/*****************************************************************************
			* 显示层调用逻辑层
            ******************************************************************************/
            ENTITY_CMD_BEGIN = ENTITY_TOVIEW_END+20,
            ///////////////////////////////////////////////////////////////////////////////
            // 显示层输出信息到LOG
            ENTITY_CMD_TRACE,

            // 显示层实体加载完成
            ENTITY_CMD_LOAD_COMPLETED,

            // 位置同步
            ENTITY_CMD_SET_POS,

            ENTITY_CMD_SET_CTRL,

            // 切换状态
            ENTITY_CMD_SWITCH_STATE,

            // 实体被击中
            ENTITY_CMD_HIT_NOW,

            // 使用技能
            ENTITY_CMD_CAST_SPELL,

            // 施法前请求技能目标信息(针对准备较长时间类型技能，用于修正施法时和实际效果不要偏差太远)
            ENTITY_CMD_SPELL_RECALC_TARGET_INFO,

            // 通知逻辑层已冲锋到目标点
            ENTITY_CMD_CHARGE,

            // 改变槽位信息
            ENTITY_CMD_CHANGE_SLOT,

            // 询问技能相关属性
            ENTITY_CMD_ASKSPELLATTRIBUTE,

            // 操作七龙珠相关
            ENTITY_CMD_DRAGONBALL,

            // 进入载具
            ENTITY_CMD_ENTER_TANK,

            //跌落状态着陆，大于最大下落速度的一半才上报，上报时附带着陆速度
            ENTITY_CMD_LANDING_SPEED,

            //实体设置立足点类型
            ENTITY_CMD_SET_FORMTYPE,

            // 传递扫射数据
            ENTITY_CMD_STRAFE_DATA,

            // 蓄力
            ENTITY_CMD_FOCO_ENERGIA,

            // 小地图消息显示
            ENTITY_CMD_MINIMAP_INFO,

            // 激活天赋
            ENTITY_CMD_ACTIVE_TALENT,
			
            // 取消施法
            ENTITY_CMD_CANCEL_CAST,

            // 请求初始化槽位信息
            ENTITY_CMD_INITAL_SLOT,

            // 操作类型
            ENTITY_CMD_OPERATION_TYPE,

            // 升级技能
            ENTITY_CMD_UPGRADE_SPELL,

            // 购买装备信息
            ENTITY_CMD_WAR_BUY_GOOD,

            // 出售装备信息
            ENTITY_CMD_WAR_SELL_GOOD,

            // 拖动槽位
            ENTITY_CMD_WAR_EXCHANGE_SLOT,

            // 使用物品
            ENTITY_CMD_WAR_USE_GOOD,

            // 使用物品
            ENTITY_CMD_WAR_REVOCATION_OPERATION,

            // 第一次点击tab键时请求玩家基本信息
            ENTITY_CMD_PLAYER_BASE_INFO,

            // 发送tab战绩表是否打开
            ENTITY_CMD_PALYER_TAB_ISOPEN,


            // 请求匹配战绩相关记录信息
            ENTITY_CMD_MATCHTYPE_RANK_RECORD,

            // 请求英雄熟练度数据记录信息
            ENTITY_CMD_MATCH_HEROSCORE_RECORD,

            // 鼠标进入槽位
            ENTITY_CMD_MOUSE_ENTER_SLOT,

            // 主角聚焦位置
            ENTITY_CMD_MASTER_EYE_FOCUS,

            // 发送召唤师技能
            ENTITY_CMD_SUMMONER_SKILL_INFO,

            // 转发召唤师技能给其他玩家
            ENTITY_CMD_SUMMONER_BROAD_INFO,

            // 设置翅膀状态
            ENTITY_CMD_SET_WING_STATE,

            // 确定游戏后调用效果
            ENTITY_CMD_BEGIN_TALENT_PAGE_EFFECT,

            // 删除页操作
            ENTITY_CMD_BEGIN_TALENT_PAGE_DELETE,

            // 设置保存单个天赋页
            ENTITY_CMD_SAVE_TALENT_PAGE_SINGLE,

            // 请求服务器所有天赋加点
            ENTITY_CMD_REQUEST_TALENT_PAGE_ALL,

            // 设置目标信息
            ENTITY_CMD_SET_TARGET_INFO,

            // 请求玩家符文信息
            ENTITY_CMD_QUERY_RUNE,

			// 请求玩家上侧边提示按钮
			ENTITY_CMD_QUERY_TOP_SIDE_BUTTON_LIST,

            // 玩家调用那一页的符文效果
            ENTITY_CMD_BEGIN_RUNE_PAGE_EFFECT,

            // 保存玩家符文页配置信息
            ENTITY_CMD_SAVE_RUNE_PAGE,

            // 删除指定符文页
            ENTITY_CMD_DELETE_RUNE_PAGE,


            // 请求玩家基本数据
            ENTITY_CMD_REQ_PLAYER_BASE_INFO,

			// 请求玩家英雄数据
			ENTITY_CMD_REQ_HERO_INFO,

            // 请求某个英雄的详细数据
			ENTITY_CMD_QUERY_HERO_DETAIL_INFO,

            // 请求升星消耗及当前英雄属性及升星后属性
            ENTITY_CMD_REQ_UPGRADESTARANDHEROPRO,

            // 请求升星
			ENTITY_CMD_REQ_UPGRADE_STAR,

			// 请求战绩总览信息
			ENTITY_CMD_REQ_WARSTATISTIC_TOTAL,
			// 请求指定战绩信息
			ENTITY_CMD_REQ_WARSTATISTIC_WAR,

            // 设置智能施法
            ENTITY_CMD_SET_SMART_CAST,

            // 设置打断循环技能
            ENTITY_CMD_BREAK_LOOK_ATTACK,

            // 请求切换英雄
            ENTITY_CMD_REQ_CHANGEHERO,

			// 请求其他玩家符文信息
			ENTITY_CMD_QUERY_OTHER_RUNE,

			// 请求打开宝箱
			ENTITY_CMD_REQ_OPENCHEST,

			// 请求广播开宝箱结果
			ENTITY_CMD_REQ_BROADCAST_CHEST_RESULT,

            // PK选择目标
            ENTITY_CMD_PK_CHOOSE_TARGET,

            // PK点击提示
            ENTITY_CMD_PK_HIT_PROMPT,

            // 操作信息
            ENTITY_CMD_PK_HANDLE_INFO,

            // 取消PK
            ENTITY_CMD_PK_CANCEL_PK,

			// 购买商城物品
			ENTITY_CMD_POINTSHOP_BUYGOODS,

			// AI控制加入跟随
			ENTITY_CMD_AI_COMMAND_FOLLOW,

			// AI控制取消跟随
			ENTITY_CMD_AI_COMMAND_BREAK_FOLLOW,

			// 特殊实体选取 目前有玩家选取祭坛
			ENTITY_CMD_SPECIAL_SELECT,

			// 特殊实体范围搜寻 目前有瞭望台搜寻玩家
			ENTITY_CMD_SPECIAL_SEARCH,

			// 七龙珠指定实体属性值显示
			ENTITY_CMD_ENTITY_PROPERTY_DRAGONBALL,

			// 七龙珠指定实体属性值关闭
			ENTITY_CMD_ENTITY_PROPERTY_DRAGONBALL_CLOSE,

			// 修改角色名称
			ENTITY_CMD_ACTOR_NAME_FIX,

			// 设置新手引导类型
			ENTITY_CMD_ENTITY_SET_NOVICEGUIDE,

			//////////////////////////皇冠////////////////////
			 // 宝石鉴定
			 ENTITY_CMD_CROWN_AGEMSTONEPPRAISAL,
			 // 保存皇冠页
			 ENTITY_CMD_CROWN_SAVEPAGE,
			 // 删除皇冠页
			 ENTITY_CMD_CROWN_DELETEPAGE,		
			 // 大厅选择皇冠页
			 ENTITY_CMD_CROWN_ROOMSELECTPAGE,	
			 // 选择原石
			 ENTITY_CMD_CROWN_DCOBBLESTON_SELECTED,	
			 // 花钱开窗
			 ENTITY_CMD_CROWN_DWINDOWSTON,
			 // 选择开窗原石
			 ENTITY_CMD_CROWN_DWINDOWSTONE_SELECTED,	
			 // 宝石出售
			 ENTITY_CMD_CROWN_DGEMSTONE_SEIL,	
			 // 请求别人皇冠页
			 ENTITY_CMD_CROWN_DOTHER_PAGE,					
			/////////////////////////皇冠////////////////////

			/////////////////////////标签//////////////////
			// 客户端添加标签操作
			ENTITY_CMD_WAR_ADD_LABEL,

			// 客户端点击解锁xp技能操作
			ENTITY_CMD_XP_SKILL_OPERATE_UNLOCK,

			// 客户端选取xp技能操作
			ENTITY_CMD_XP_SKILL_OPERATE_SELECT,

			////////////////////NPC////////////////////////
			// 客户端触发NPC
			ENTITY_CMD_NPC_TRIGGER,
			// 客户端点击NPC功能
			ENTITY_CMD_NPC_FUNC,

			ENTITY_CMD_PICK_HP,

            // 主角在显示层创建好了
            ENTITY_CMD_MASTER_LOADED,

			////////////////////新手引导////////////////////////
			//通知已达成旋转相机
			ENTITY_CMD_GUIDE_ROTATECAMERA,

			///////////////////装备方案////////////////////////
			// 增加装备方案
			ENTITY_CMD_WAR_ADD_EQUIPSCHEME,
			// 删除装备方案
			ENTITY_CMD_WAR_DEL_EQUIPSCHEME,
			// 修改装备方案
			ENTITY_CMD_WAR_MODIFY_EQUIPSCHEME,
			// 读取装备方案
			ENTITY_CMD_WAR_READ_EQUIPSCHEME,

			// 刷新卡牌
			ENTITY_CMD_REFLUSH_CARD,
			
			// 挂机状态撤销
			ENTITY_CMD_BOTTING_CANCEL,

			// 添加本命英雄
			ENTITY_CMD_LIFEHERO_ADD,
			// 删除本命英雄
			ENTITY_CMD_LIFEHERO_DEL,
			// 替换新的本命英雄 
			ENTITY_CMD_LIFEHERO_REPLACE,
			// 互换两个本命英雄培养仓的位置
			ENTITY_CMD_LIFEHERO_EXCHANGE,
			// 查询所有本命英雄
			ENTITY_CMD_LIFEHERO_QUERY,
			// 查询玩家最近历史战绩
			ENTITY_CMD_REQ_HISTORY_RECORD,

			// 玩家游戏状态是否离开
			ENTITY_CMD_GAMESTATE_LEAVE,

			// 本命英雄领取奖励
			ENTITY_CMD_LIFEHERO_OBTAIN_PRIZE,

			// 玩家主动退出战场（战场中途）
			ENTITY_CMD_REQ_EXIT_WAR,

			// 战场切换职业
			ENTITY_CMD_WAR_CHANGE_VOCATION,

            // 请求游戏导航栏
            ENTITY_CMD_REQUEST_NAVGROUP,

            // 请求URL字符串
            ENTITY_CMD_REQUEST_WEBURL,

			// 请求排位赛季详细数据
			ENTITY_CMD_REQ_SEASON_DETAIL,

			// 请求排位赛季历史数据
			ENTITY_CMD_REQ_SEASON_RECORD,


            ENTITY_CMD_MAX,
            // END 显示层上发到逻辑层命令
            ////////////////////////////////////////////////////////////////////////////////////
    };


    ///////////////////////////////////////////////////////////////////////////////////////////////
    /********************************** 以下定义显示层与逻辑层通讯要用的结构 ***********************/

    ENUM MoveSyncFlag
    {
        NOT_2_SERVER  = 0,  // 不发给服务器
		TO_SERVER     = 1,  // 发给服务器
		TO_BROADCAST  = 2,  // 需要广播
    };

    // 角色网络同步的关键方法：
    // 位置同步:
    // 1.取各点快照，还原出各时间节点推测位置 
    // 2.根据角色当前所在位置，进行推测
    // 动作同步:
    // 1.取每次输入时，角色当前的状态
    // 涉及到需要同步位置的消息时，务必携带此结构
    STRUCT cmd_creature_rigidbody_sync
    {
    public:
		int nActorID;
        int nCreatureState;     // 当前的客户端状态机类型
        int nDelayTick;			// 延时[该命令收到时的网络延时]
        float fPosition_x;      // 角色位置
        float fPosition_y;      // 角色位置
        float fPosition_z;      // 角色位置
        float fRotation_x;		// 角色朝向绕x轴旋转的角度
        float fRotation_y;      // 角色朝向绕y轴旋转的角度
        float fRotation_z;      // 角色朝向绕z轴旋转的角度
        float fVelocity_x;      // 角色速度
        float fVelocity_y;      // 角色速度
        float fVelocity_z;      // 角色速度
        int nSyn2Server;        // 取值参考MoveSyncFlag
		int nDirection;         //角色面向前后0前1后


        STRUCT_INIT(cmd_creature_rigidbody_sync)
    };

    STRUCT cmd_creature_state_sync 
    {
    public:
        int nPostureID;			// 当前的状态	
        int bIsMoving;			// 是否运动中 (C#的bool是4个字节)
        float fForward;			// 向前状态控制
        float fLeft;			// 方向
        int bJump;				// 是否正在跳跃 (C#的bool是4个字节)
        int bFall;				// 是否正在下落 (C#的bool是4个字节)
        int nSkillID;			// 技能ID

        STRUCT_INIT(cmd_creature_state_sync)
    };

    STRUCT cmd_creature_act_sync 
    {
    public:
        cmd_creature_rigidbody_sync pos;
        cmd_creature_state_sync state;

        int nCtrID;				// 操作码
        int nCtrParam;			// 操作参数
        int nCtrParam2;			// 操作参数2
        float fCtrParam_x;		// 操作参数,比如技能方向
        float fCtrParam_y;		// 操作参数,比如技能方向
        float fCtrParam_z;		// 操作参数,比如技能方向
        int			nSlotIndex;		// 槽索引
        SLOT_TYPE	nSlotType;		// 槽位类型	(1、装备 2、技能)
        unsigned int  operateTick;     // 显示层本地操作时间

        STRUCT_INIT(cmd_creature_act_sync)
    };

    STRUCT cmd_creature_hitNow
    {
    public:
        int nActorID;			// 角色ID[这2个C++里会自动填充]
        int nTick;				// 快照时间点[这2个C++里会自动填充]

        int nHitTargetID;		// 击中的目标ID
        int nSerialID;			// 序列号ID

        float fCollisionPos_x;	// 碰撞点位置x
        float fCollisionPos_y;	// 碰撞点位置y
        float fCollisionPos_z;	// 碰撞点位置z

        STRUCT_INIT(cmd_creature_hitNow)
    };

    // 施法前请求技能目标信息(针对准备较长时间类型技能，用于修正施法时和实际效果不要偏差太远) ENTITY_CMD_SPELL_RECALC_TARGET_INFO 
    STRUCT cmd_creature_recalc_target_info
    {
        float self_pos_x;
        float self_pos_y;
        float self_pos_z;

        float target_pos_x;
        float target_pos_y;
        float target_pos_z;

		int   nStauts;//状态0-表示失败，1表示成功
        STRUCT_INIT(cmd_creature_recalc_target_info)
    };

    STRUCT cmd_creature_skillview
    {
    public:
        int nSrcEntityID;		// 攻击者EntityID
        int nTargetEntityID;	// 受击者EntityID
        int nFeedBackID;		// 反馈ID，当发生碰撞时返回此ID(客户端做一些碰撞效果使用，暂时无用，先留着)

        float fSrcPos_x;        // 起点坐标x
        float fSrcPos_y;        // 起点坐标y
        float fSrcPos_z;        // 起点坐标z

        float fTargetPos_x;		// 目标坐标x
        float fTargetPos_y;		// 目标坐标y
        float fTargetPos_z;		// 目标坐标z

        float fDistance;		// 技能距离
        float fAttackRange;		// 攻击范围

        float fPASD;			// 攻击速度
		int nCreatorSide;		// 光效来自 0-敌方  1-友方 

        int bFromSrcPos;        // 从src pos发出 0-false 1-true
		int bFlyToTargetPos;	// 是否飞到目标位置(使用技能时鼠标点击的位置) 0-false 1-true


        STRUCT_INIT(cmd_creature_skillview)
    };

    STRUCT cmd_creature_playAnimation
    {
    public:
        UID	uidOperator;		// 操作者
        UID uidTarget;			// 目标
        float fPASD;			// 攻击速度
        sp_char_array(szName,ANIMATION_NAME_LEN);		// 动作名字
        int bAlreadyOver;       // 通知显示层时动作应该已经播放完，直接播放最后一帧或者快速播放到最后一帧(如进视野时) 

        STRUCT_INIT(cmd_creature_playAnimation)
    };

    STRUCT cmd_creature_stopAnimation
    {
public:
        sp_char_array(szName,ANIMATION_NAME_LEN);		// 没有动作名时为通用停止动画

        STRUCT_INIT(cmd_creature_stopAnimation)
    };

    STRUCT cmd_cancel_skillview
    {
    public:
        int nSkillviewID;		// 技能显示方面的配置表ID
        int nSrcEntityID;		// 攻击者EntityID    

        STRUCT_INIT(cmd_cancel_skillview)
    };

    // 强制移动角色，主要用于冲锋等技能
    // 无论是否是主角，都会被拉动
    // 在移动中如果撞墙，会停下来，所以最好在强制移动结束后再上报一遍位置
    STRUCT cmd_force_move
    {
    public:
        UID	uidOperator;		// 操作者
        UID uidTarget;			// 目标
        float fPosition_x;      // 角色目标位置
        float fPosition_y;      // 角色目标位置
        float fPosition_z;      // 角色目标位置
        float fSpeed;			// 速度
        float fTime;			// 移动需要的时间
        float fHeight;			// 击飞高度
        MOVE_TYPE nMoveType;    // 移动类型
        int	nLockTarget;		// 1表示锁定目标
        int nLockTime;			// 锁定目标时间,超过时间打断
        float fLocationDistance;// 离目标点距离
        int nPassive;           // 被动位移标识(0主动位移,不为0被动位移)
        float fMaxTime;         // 最大时间

        STRUCT_INIT(cmd_force_move)
    };

    ENUM SoltViewType
    {
        ViewType_NONE           = 0x00,     //清除格式
            ViewType_LeftCircle     = 0x01,    // 向左转圈圈
            ViewType_RightCircle    = 0x02,    // 向右转圈圈
            ViewType_Up             = 0x04,    // 向上转圈圈
            ViewType_Down           = 0x08,    // Bit 3       
            ViewType_Splash         = 0x10,    // 闪
    };

    STRUCT cmd_creature_slotview
    {
    public:
        int				bRemove;		
        int				nType;			// 冷却类型
		int				nCoolID;		// 冷却id
        int				nId;			// 技能或者物品ID,为0时不表示不无数据
        SoltViewType    nViewType;		// 显示类型（方式）：0101 1111
        int             nCurData;		// 槽位当前相关数据（根据显示类型（方式）判定是否启用 nMaxData）
        int				nMaxData;		// 最大相关数据
        int				nIconID;		// 图片ID
        int				bEnable;		// 是否可用
        int				bIsOnDrag;		// 拖动状态
        int				bCanDrag;		// 是否能拖动
        int				nSlotGroupType;	// 槽位组类型
        sp_char_array(szSlotDesc,SLOT_DESC_LEN);		// 槽位描述

        STRUCT_INIT(cmd_creature_slotview)
    };

    STRUCT cmd_creature_slot_maped_shortcut
    {
    public:
        int				nSlotIndex;		
        int				nSlotType;		// 冷却类型
        int				nAirCons;
		int                nAdvanceUseType;
		bool             isAdd;
        sp_char_array	(szKeyName,GAME_SHORTCUT_NAME_MAXSIZE);		// 快捷键名称

        STRUCT_INIT(cmd_creature_slot_maped_shortcut)
    };

    // 询问技能相关属性，由渲染层向逻辑层询问
    STRUCT cmd_creature_spellAttribute
    {
    public:
        int				nSlotIndex;
        int				nID;			// 技能ID
        SLOT_TYPE		nSlotType;

        STRUCT_INIT(cmd_creature_spellAttribute)
    };

    // 技能相关属性，由逻辑层传设置渲染层，
    STRUCT cmd_spell_attribute
    {
    public:
        int				nAttributeType;	// 属性询问类型，0失败，不为0成功
        int				nSelectType;	// 技能释放选择的类型，详细参见SPELL_SELECT_TYPE
        float			fDistance;		// 技能距离
        float			fAttackRange;	// 攻击范围
        float			fHeight;		// 技能高度
        float           fChooseRange;   // 选择范围
        int             nDrawType;		// 技能使用前描画类型
        int				nTargetType;	// 目标类型TARGET_FILTER
        int				nCastType;		// 技能使用类型，详细参见SPELL_CAST_TYPE
        int				nSlotIndex;		// 槽索引
        SLOT_TYPE		nSlotType;		// 槽位类型	(1、装备 2、技能)
        int				nSpellID;		// 技能ID
        int				nAttackType;	// 攻击类型ATTACK_TYPE
        int				nSpellType;	    // 技能类型SPELL_TYPE
        int				nFocoEnergiaTime;		// 蓄力时间
        int				nCastFlag;		// 技能使用标识
        int				nSpellFlag;		// 技能标识
        int				nRollSpellStartID;		// 翻滚技能起始ID
        int             nDefaultAimFlag;        // 默认瞄准标志(0默认,-1关闭,1开启)

        STRUCT_INIT(cmd_spell_attribute)
    };

    // 施法技能点(显示层转向用)
    STRUCT cmd_creature_castSpellPos
    {
    public:
        float			fPosition_x;      // 角色目标位置X
        float			fPosition_y;      // 角色目标位置Y
        float			fPosition_z;      // 角色目标位置Z
        int				nRigidityTime;	  // 僵直时间
        int				nMoveRigidityTime;// 移动僵直时间
        int				nTurnRigidityTime;// 转向僵直时间
        int				nCastType;		  // 技能使用类型
        UID             uidTarget;		  // 目标

        STRUCT_INIT(cmd_creature_castSpellPos)
    };

    // 设置buff
    STRUCT cmd_creature_setBuff
    {
    public:
        int				nType;			// buff操作类型
        unsigned long	dwIndex;		// buff索引
        int				nIconID;		// buff图标ID
        unsigned long	dwTime;			// buff剩余时间
        sp_char_array(szName,32);		// buff名字
        sp_char_array(szTips,256);		// buff描述
        int             nLevel;         // buff等级
        int            nIsShow;        // buff显示
        STRUCT_INIT(cmd_creature_setBuff)
    };

    // 传送
    STRUCT cmd_creature_transport
    {
    public:		
        float fPosition_x;      // 角色目标位置
        float fPosition_y;      // 角色目标位置
        float fPosition_z;      // 角色目标位置

        float fRotation_x;		// 角色朝向绕x轴旋转的角度
        float fRotation_y;      // 角色朝向绕y轴旋转的角度
        float fRotation_z;      // 角色朝向绕z轴旋转的角度

        int	  nDestMapID;		// 目标地图ID，暂时没有考虑夸地图传送
        int	  nSrcMapID;		// 源地图ID，暂时没有考虑夸地图传送

        int  bUseAngle;		//是否使用角度(C#bool 是四字节，用int代替)
        int  nReason;       // TRANSPORT_REASON
        STRUCT_INIT(cmd_creature_transport)
    };	

    STRUCT cmd_choose_hero_sync
    {
    public:

        BYTE			HeroID;		// 英雄ID 相当于职业ID
        BYTE			byIndex;	// 展示技能槽位索引
        SPELL_DATA		SpellData;  // 索引位置技能数据
        int				nCD;		// 冷却时间

        int				nHp;		// 生命
        int				nPA;		// 物理攻击
        int				nMA;		// 魔法攻击
        int				nOperateFactor;	// 操作系数难度
        sp_char_array(szVocation,32)	// 职业类型
            sp_char_array(szImage,32);		// 资源名称
        sp_char_array(szLocate,32);		// 定位

        sp_char_array(szHeroDes,256);		// 英雄描述

        STRUCT_INIT(cmd_choose_hero_sync);
    };

    STRUCT cmd_set_spell_overlay
    {
    public:
        int				nID;		// 技能ID
        SLOT_TYPE       nSlotType;  // 槽位类型
        int             nSlotIndex; // 槽位索引
        int				nFlag;		// 标志值
        int				nType;		// 转圈类型(0不转圈，1转圈)
        int				nLeftTime;	// 转圈剩余时间
        int				nTotalTime;	// 转圈总时间
        int				nFreezeLeftTime;		// 冷却剩余时间

        STRUCT_INIT(cmd_set_spell_overlay);
    };

    // 实体下落着陆速度
    STRUCT cmd_creature_act_landspeed
    {
    public:
        float			fValue;		// 速度

        STRUCT_INIT(cmd_creature_act_landspeed)
    };

    // 实体死亡
    STRUCT cmd_creature_die
    {
    public:
        int             nDeadInterval;  // 已经死亡刻度 ms
        int				nInterval;		// 死亡至复活时时间刻度 ms
        unsigned long   uMurderID;      // 击杀者ID
		BYTE			isNoRelive;		// 是否能复活

        STRUCT_INIT(cmd_creature_die)
    };

    // 实体设置立足点类型
    STRUCT cmd_creature_act_set_formtype
    {
    public:
        int			nValue;		// 立足点类型 FORM_TYPE

        STRUCT_INIT(cmd_creature_act_set_formtype)
    };

    //////////////////////////////////////////////////////////////////////////
    // 创建房间 一个地图的显示
    STRUCT cmd_match_create_room_config
    {
    public:
        // 地图ID
        int			nMapID;
        // 地图名称
        sp_char_array(szName,32); 
        // 创建房子时的地图图片显示路径
        sp_char_array(szPicPath,32); 

        STRUCT_INIT(cmd_match_create_room_config);
    };

    // c2v创建房间 一个模式的显示
    STRUCT cmd_match_create_room_mode
    {
    public:
        int		nModeID;
        // 是否选中
        BYTE	bSelect;
        // 名称
        sp_char_array(szName,32); 


        STRUCT_INIT(cmd_match_create_room_mode);
    };
    // c2v创建房间  一个地图的显示
    STRUCT cmd_match_create_room_map
    {
    public:
        int		nMapID;
        int		nSchemeConfigWarID;
        // 是否选中
        BYTE	bSelect;
        // 名称
        sp_char_array(szName,32); 

        STRUCT_INIT(cmd_match_create_room_map);
    };

    // v2c返回选择的地图信息
    STRUCT cmd_match_create_room_click_map
    {
    public:
        int		nMapID;
        int		nSchemeConfigWarID;

        STRUCT_INIT(cmd_match_create_room_click_map);
    };

    // c2v创建房间 一个地图的信息显示
    STRUCT cmd_match_create_room_map_info
    {
    public:
        // 图片路径
        sp_char_array(szPicPath,32);
        // 描述
        sp_char_array(szDesc,512); 

        STRUCT_INIT(cmd_match_create_room_map_info);
    };
    // v2c创建房间 
    STRUCT cmd_match_create_room
    {
    public:
        // 房间名
        sp_char_array(szName,GAME_NAME_MAXSIZE);
        sp_char_array(szPassword,GAME_ROOMPWD_MAXSIZE);
        int bHavePassWord;
        int nMatchType;
		BYTE byPermitOb;

        STRUCT_INIT(cmd_match_create_room);
    };

    // v2c创建引导房间 
    STRUCT cmd_match_create_guide_room
    {
    public:
        // 房间名
        int nGuidHeroID;
        int nMatchType;

        STRUCT_INIT(cmd_match_create_guide_room);
    };

    // 引导ID 
    STRUCT cmd_guide_info
    {
    public:
        int nGuideID;

        STRUCT_INIT(cmd_guide_info);
    };

    // 自定义房间邀请回复 
    STRUCT cmd_customer_invite_respon
    {
    public:
        int nRoomID;
        int dwInviteActorID;
        sp_char_array(szPlayerName,GAME_NAME_MAXSIZE);

        STRUCT_INIT(cmd_customer_invite_respon);
    };

    // v2c创建房间 
    STRUCT cmd_match_enter_room
    {
    public:
        // 房间名
        int nRoomID;
        sp_char_array(szPassword,GAME_ROOMPWD_MAXSIZE);

        STRUCT_INIT(cmd_match_enter_room);
    };

    STRUCT cmd_match_hall_rooms_head
    {
    public:
         int nRoomCount;

         STRUCT_INIT(cmd_match_hall_rooms_head);
    };

    //刷新大厅房间数据
    STRUCT cmd_match_hall_add_room
    {
    public:
        // 是否是匹配房间
        int bIsMatchRoom;
        //房间序号(房间序号与房间ID是一致?)
        int nRoomIndex;
        //房间当前玩家数量
        int nCurPlayerNum;
        //房间总玩家数量
        int nMaxPlayerNum;
		//房间总Ob数量
		int nMaxObNum;

        //房间名
        sp_char_array(szRoomName,32); 
        //模式 
        sp_char_array(szMode,32); 
        //地图名
        sp_char_array(szMapName,32); 
        //房间状态 
        sp_char_array(szState,32); 

        //游戏模式
        int nRoomMode;
        //地图ID
        int nMapID;

        // 配置的WarID
        int nConfigWarID;

        //地图描述
        sp_char_array(szMapDes,256); 

        // 是否延迟进入
        int bIsDelayJoin;

        // 延迟时间
        int nDelayTime;

        int bLock;

		// 房间类型
		int nRoomType;

		// 是否是线上杯赛
		int bOnlineLegendCup;

        // MatchTypeID
        int nMatchTypeID;

		int bPermitOb;

        STRUCT_INIT(cmd_match_hall_add_room);
    };

    STRUCT cmd_war_personInfo_count
    {
        int nCount;
        STRUCT_INIT(cmd_war_personInfo_count);
    };
    // 创建房间 一个地图的显示
    STRUCT cmd_war_personinfo
    {
    public:
        unsigned long uPDBID;
        int		nCamp;		// 玩家阵营
        int		nIndex;		// 玩家位置
        int		uID;		// 玩家ID
        int		nKillNum;	// 玩家击杀数
        int		nDeadNum;	// 死亡数
        int		nHeroID;	// 英雄ID
		int		nSkinID;	// 英雄皮肤ID
        int		nLevel;		// 等级
        int		nEXP;		// 经验
        int		nContKill;	// 连杀
        int		nAssists;	// 助攻
        int		nKillMonsterNum; //怪物击杀数量  临时改成金币。 之后换界面的时候再优化
        int		bIsOnGame; // 是否在游戏中
        sp_char_array(szName,32);
        int		isProtag;	// 是否是主角
        int     nTalentId1; // 天赋id1
        int     nTalentId2; // 天赋id2
		int		nSummerIconID; // 召唤师技能图标id
		int		nIsAi;		   // 是否是ai 
		int		nAliveContAssist;// 不死连助攻
		int		nSex;	// 性别
		int		nPlayerType;	// 玩家类型
        int     nDestroyTower;  // 推塔数
		sp_char_array(szSummerDesc,SUMMONER_SKILL_DESC); // 召唤师技能描述
        STRUCT_INIT(cmd_war_personinfo);
    };

    // 创建房间 一个地图的显示
    STRUCT cmd_war_monsterinfo
    {
    public:
        // 怪物小地图索引
        int nIconIndex;

        // 怪物类型
        int nType;
        // 倒计时
        int nTime;
        // 战绩表位置
        int nTableIndex;

        STRUCT_INIT(cmd_war_monsterinfo);
    };

	// 线上杯赛房间等待倒计时
	STRUCT cmd_legendcup_room_wait_countdown
	{
	public:
		// 倒计时时长
		int nCountdown;

		STRUCT_INIT(cmd_legendcup_room_wait_countdown);
	};

    //玩家进入别人创建的房间,接收玩家数据
    STRUCT cmd_match_room_role_data
    {
    public:
        //哪个阵营 2个阵营0,1
        int nCamp;
        //房间内位置ID 5个人 0-4
        int nPlayerSlotID;
        //玩家名字
        sp_char_array(szPlayerName,32); 
        //玩家英雄ID
        int nHeroID;
        // 是不是房主
        int bIsMaster;
        // 是不是假人
        int bIsComputerPlayer;
		// 是不是Ob
		int bIsOb;

        STRUCT_INIT(cmd_match_room_role_data);
    };

	//进入别人创建的房间,接收Ob数据
	STRUCT cmd_match_room_ob_data
	{
	public:
		// Ob位置
		int nObPos;
		// 玩家ID
		int nActorID;
		// 头像ID
		int nHeadID;
		// 玩家性别
		int nSex;
		// 玩家名字
		sp_char_array(szPlayerName, 32); 

		STRUCT_INIT(cmd_match_room_ob_data);
	};

    // 实体播放动画
    STRUCT cmd_entity_set_animation
    {
        // 动画名称
        sp_char_array(szAnimationName, 32);
        // 动画bool参数
        sp_char_array(szAnimationBool, 32);
        // 0--停止动画  1--开始动画
        int bEnable;  

        STRUCT_INIT(cmd_entity_set_animation);
    };

    STRUCT cmd_pre_select_head
    {
    public:
        int nCount;

        STRUCT_INIT(cmd_pre_select_head);
    };

    // 房间英雄改变
    STRUCT cmd_match_room_hero_change
    {
    public:
        //房间内位置ID
        int nPlayerSlotID;
        //玩家英雄ID
        int nHeroID;
        //阵营
        int nCamp;
        // 英雄星级
        int nHeroStarLv;
        // 英雄等级
        int nHeroStaticLv;
		// 是否服务器随机
		int bServerRand;

        STRUCT_INIT(cmd_match_room_hero_change);
    };

    STRUCT cmd_match_room_computer_hero_change
    {
    public:
        //房间内位置ID
        int nPlayerSlotID;
        //玩家英雄ID
        int nHeroID;
        //阵营
        int nCamp;
        //AIID
        int nAIID;

        sp_char_array(szHeroName,32);   //  英雄名字

        STRUCT_INIT(cmd_match_room_computer_hero_change);
    };

    // 房间英雄位置改变
    STRUCT cmd_match_room_hero_pos_change
    {
    public:
        int nCampFrom;
        int nPlayerSlotIDFrom;

        int nCampTo;
        int nPlayerSlotIDTo;

        STRUCT_INIT(cmd_match_room_hero_pos_change);
    };

	// 房间英雄位置改变
	STRUCT cmd_match_room_ob_pos_change
	{
	public:
		int		bSelfIsOBPos;
		int		idActor;

		int		bDesIsOBPos;
		int		nPos;

		STRUCT_INIT(cmd_match_room_ob_pos_change);
	};

	STRUCT cmd_match_room_ob_pos_change_oc
	{
	public:
		int		bSrcIsOBPos;
		int		nSrcPos;

		int		bDesIsOBPos;
		int		nDesPos;

		int		nActorID;
		int		nHeadID;
		int		nSex;
		sp_char_array(szPlayerName, 32); 

		STRUCT_INIT(cmd_match_room_ob_pos_change_oc);
	};

    // 房间英雄离开
    STRUCT cmd_match_room_hero_exit
    {
    public:
		int bIsOb;
		int nPos;

        STRUCT_INIT(cmd_match_room_hero_exit);
    };

    // 房间英雄确认
    STRUCT cmd_match_room_confirm_hero
    {
    public:
        int nCamp;
        int nPlayerSlotID;
        int nSelf;

        STRUCT_INIT(cmd_match_room_confirm_hero);
    };

    // 房间房主改变
    STRUCT cmd_match_room_master_change
    {
    public:
        // 旧房主位置
        int nCampOld;
        int nPlayerSlotIDOld;
        // 新房主位置
        int nCamp;
        int nPlayerSlotID;
        // 新房主是不是自己 1是 0不是
        int nSlef;

        STRUCT_INIT(cmd_match_room_master_change);
    };
    // 鼠标停留的人物职业id
    STRUCT cmd_match_room_Mouse_HeroArea 
    {
        int             nHeroVocationID;    // 鼠标停留的英雄职业id
        STRUCT_INIT(cmd_match_room_Mouse_HeroArea);
    };

    // 效果-浮空
    STRUCT cmd_effect_Flight
    {
        float	fHeight;				// 浮空高度
        int		nResidenceTime;			// 空中滞留时间
        int		nGoUpAcceleration;		// 上升加速度
        int		nDropAcceleration;		// 下落加速度
        int     nFlight;                // 为0停止浮空,不为0开始浮空

        STRUCT_INIT(cmd_effect_Flight)
    };

	// 助攻者信息
	STRUCT cmd_war_assist_info
	{
		int MasterEntityType;					// 助攻者实体类型
		UID	uidAssist;							// 助攻者id
		int nHerorID;							// 英雄ID
		sp_char_array(AssistName, 32);			// 助攻者名字
		sp_char_array(AssistVocationName, 32);  // 助攻者职业名称

		STRUCT_INIT(cmd_war_assist_info)
	};

    //广播特殊击杀数据
    STRUCT cmd_war_important_entiydieinfo
    {
    public:
        int isSampCampKill;		// 是否是同阵营击杀
        int	bIsFirstBlood;	//	首杀
        int	bIsCampSkill;	//	是否是阵营击杀	
        int	nCamp;			//  被哪方击杀
        int MasterEntityType; // 被杀人实体类型
        int MurdererEntityType;// 击杀人实体类型
        int	nMasterHeroID;	// 被杀人职业ID
        int	nMurdererHeroID;// 击杀人职业ID
        sp_char_array(MasterName,32); //  被击者名称
        sp_char_array(MurdererName,32); //  击杀者名称
        sp_char_array(MasterVocationName,32); //  被击者职业名称
        sp_char_array(MurdererVocationName,32); //  击杀者职业名称
        int nKillCount;
        int bIsAce;			// 是否团灭
        int bIsOver;		// 是否被终结
		int	nAddTalentPoint;	// 终结和第一滴血有这个数据

		int nSoundID;
		sp_char_array(szDieDes,64); // 描述
		int uidMaster;   	//	被杀者ID
		int uidMurderer;   	//	击杀者ID

		int nAssistCount;	// 助攻者数目
        STRUCT_INIT(cmd_war_important_entiydieinfo);
    };

    //广播击杀数据
    STRUCT cmd_war_entity_killcountinfo
    {
    public:
        int     isSampCampKill;		// 是否是同阵营击杀
        int		nCamp;			//  被哪方击杀
        int		nTimeKillCount;// 战斗中连续击杀

        int MasterEntityType; // 被杀人实体类型
        int MurdererEntityType;// 击杀人实体类型

        int		nMurdererHeroID; // 击杀人职业ID
        sp_char_array(MurdererName,32);//  击杀者名称

		int		nMasterHeroID; // 被杀人职业ID
		sp_char_array(MasterName,32);//  被杀者名称
		sp_char_array(MasterVocationName,32); //  被击者职业名称
		sp_char_array(MurdererVocationName,32); //  击杀者职业名称
		int		nIsAce;			// 是否团灭
		int uidMaster;   	//	被杀者ID
		int uidMurderer;   	//	击杀者ID

		int nAssistCount;	// 助攻者数目

		STRUCT_INIT(cmd_war_entity_killcountinfo);
	};

    STRUCT cmd_war_entity_end_warinfo_count
    {
    public:
        int nCount;

        STRUCT_INIT(cmd_war_entity_end_warinfo_count);
    };

    STRUCT cmd_war_entity_end_warinfo
    {
    public:		
        int nCamp;  // 玩家所在阵营
        int nIndex;  // 玩家序号位
        int uID;   	// 玩家ID
        int nHeroID;		// 英雄ID
        sp_char_array(PlayerName,32);//  玩家名称
        int nLevel;  	// 玩家等级
        int nExperien;		// 经验
        int nKill; 	// 击杀
        int nDeadNum; 	// 死亡次数
        int nAssists; 	// 助攻数
        int nKillMonsterNum;		// 击杀怪物数量
        int  WarMedal;		// 战斗勋章
        int WarContinueKillMedal;	// 战斗中连杀
        int WarDeadKillMedal;	// 死亡连杀

        STRUCT_INIT(cmd_war_entity_end_warinfo);
    };


    // 效果-扫射
    STRUCT cmd_effect_Strafe
    {
    public:
        int				nSelectType;	// 技能释放选择的类型，详细参见SPELL_SELECT_TYPE
        float			fDistance;		// 技能距离
        float			fAttackRange;	// 攻击范围
        int				nID;			// 技能ID
        int				nTime;			// 更新坐标时间
        int				nCount;			// 更新坐标次数
        int				nLightID;		// 光效ID
        float           fFlyingSpeed;   // 飞行速度
        int             nStrafeType;    // 扫射类型
        int             nDrawType;      // 技能使用前描画类型
        int				nTargetType;	// 目标类型TARGET_FILTER

        STRUCT_INIT(cmd_effect_Strafe)
    };

    STRUCT cmd_match_room_hero_Info
    {
    public:
        //英雄ID
        int nHeroID;

        //英雄名字
        sp_char_array(szHeroName,32); 
        //英雄阵营
        sp_char_array(szHeroCamp,32);
        //英雄定位
        sp_char_array(szHeroGank,32); 

        STRUCT_INIT(cmd_match_room_hero_Info)
    };

    //更换皮肤
    STRUCT cmd_change_entity_skin
    {
    public:

        int		nSkinID;			//皮肤ID
        int		nPart;				//哪个部分

        STRUCT_INIT(cmd_change_entity_skin)
    };

    // 阵营人员信息
    STRUCT cmd_entity_camp_palyer_uidinfo
    {
    public:

        unsigned long uUID;
        unsigned long uPDBID;
        int nHeroID;
        int nPlayerCount;
        int nCamp;
		int nIsAI;

        STRUCT_INIT(cmd_entity_camp_palyer_uidinfo)
    };

    STRUCT cmd_match_room_hero_slot
    {
        int		nSlotIndex;					//	插槽序号
        int		nHeroID;					//	插槽显示的英雄ID
        int		nHeroType;					//	英雄类型
		int		nPractisedLevel;			//	熟练度等级
        BYTE	bIsLock;					//	插槽是否可选
        BYTE	bCurrentIsHave;				//	当前是否拥有
		BYTE	bIsWeekFree;				//	是否周免
        sp_char_array(szHeroTooltip,512);	//	插槽显示英雄的Tooltip
	    sp_char_array(szHeroName,32);		//	英雄名字
        sp_int_array(nSkinIDList, PERSONSKIN_SKIN_COUNT);   //全部的皮肤id列表
		int		dwUnlockTime;				//	解锁时间
		int		nMatchNum;					//	比赛场次 
        STRUCT_INIT(cmd_match_room_hero_slot)
    };

    // 蓄力
    STRUCT cmd_foco_energia
    {
        int		nID;				// 技能ID

        STRUCT_INIT(cmd_foco_energia)
    };

    // 天赋描述长度
#define TALENT_DES_MAX_LEN  512 

    // 天赋名字长度
#define TALENT_NAME_LEN 256

    ENUM ETalentState
    {
        Talent_State_None,               // 初始状态
            Talent_State_Can_Not_Active,     // 不可激活
            Talent_State_Can_Active,         // 可激活
            Talent_State_Actived,            // 已激活
    };

    // 逻辑通知显示层天赋信息
    STRUCT TALENT_VIEW
    {
        int     nTalentID;                                     // 天赋ID
        sp_char_array(szName, TALENT_NAME_LEN)	               // 天赋描述
            int     nIconID;                                       // 天赋图标ID
        int     nLevelCond;                                    // 等级条件
        int     nCostPoint;                                    // 消耗点数
        sp_char_array(szCondPreTalent, TALENT_NAME_LEN)        // 前置天赋
            sp_char_array(szDescription, TALENT_DES_MAX_LEN)	   // 天赋描述
            ETalentState eState;                                   // 状态

        STRUCT_INIT(TALENT_VIEW)
    };

    // 逻辑通知显示层天赋状态
    STRUCT cmd_notify_talent_state
    {
        int             nTalentID;                              // 天赋ID
        ETalentState    eState;                                 // 状态
    };

    STRUCT cmd_war_mini_map_info
    {
        // 小地图消息类型
        int nType;
        // 消息坐标点
        float x;
        float y;
        float z;
        // 消息目标实体ID
        int nEntityID;

        sp_char_array(PlayerName,32);//  发消息人物名称
        sp_char_array(VocationName,32);//  职业名称
        sp_char_array(TargetName,32);//  目标人物名称
        sp_char_array(TargetVocationName,32);// 目标职业名称
        STRUCT_INIT(cmd_war_mini_map_info)
    };

	STRUCT cmd_war_ai_command_follow
	{
		// 实体UID
		int uidTarget;

		STRUCT_INIT(cmd_war_ai_command_follow)
	};

	STRUCT cmd_war_ai_command_break_follow
	{
		// 实体UID
		int uidTarget;

		STRUCT_INIT(cmd_war_ai_command_break_follow)
	};

	STRUCT cmd_war_ai_command_info
	{
		// 实体PDBID
		int nPDBID;
		// AIcommand类型
		int nType;
		// 实体坐标点
		float x;
		float y;
		float z;

		STRUCT_INIT(cmd_war_ai_command_info)
	};

    #define GARRLORYMAX 27
	//ENTITY_TOVIEW_SET_HONOR_PAGE_INFO_ALL
	STRUCT cmd_honor_page_info
	{
        sp_int_array(arrGlory, GARRLORYMAX);		// 个人荣耀信息 枚举ENACHIEVE
		sp_longlong_array(llWarID, GARRLORYMAX);	// 个人荣耀信息对应的战场ID

		STRUCT_INIT(cmd_honor_page_info)
	};

    STRUCT cmd_war_clear_all_mini_map_info
    {
        int isClearAll;
        STRUCT_INIT(cmd_war_clear_all_mini_map_info)
    };

    STRUCT cmd_creature_replace_slotview
    {
        int			nSpellID;						// 要替换的技能ID
        cmd_creature_slotview	slotview;			// 槽位信息

        STRUCT_INIT(cmd_creature_replace_slotview)
    };

    STRUCT cmd_war_mini_map_icon
    {
        int bIsShow;
        // 小地图图标
        int nIconIndex;
        // 消息坐标点
        int nMapID;

        STRUCT_INIT(cmd_war_mini_map_icon)
    };

    // 伤害信息
    STRUCT cmd_creature_damage
    {
        UID					uidOperator;	/// 操作者的UID
        UID					uidTarget;		/// 目标对象UID
        int					nID;			/// 技能ID

        int					nDeltaHP;		/// 要增减的HP值
        int					nEqualHP;		/// 增减后的HP值

        int					nDeltaMP;		/// 要增减的MP值
        int					nEqualMP;		/// 增减后的MP值

        int					nAttackType;	/// 结果类型(EAttackResultType)
        DeadEffectType		nDeadType;		/// 死亡类型(DeadEffectType)

        int					nMask;				/// 实体标志位
        int 				bDead;				/// 是否死亡
        int					nSpellEffectType;	/// 技能效果类型
        int					nSpellFlag;			/// 技能标识

        STRUCT_INIT(cmd_creature_damage)
    };


    // 不可转向
    STRUCT cmd_creature_NoTurnTo
    {
        int		nType;				// 类型(0增加buffID,不为0移除)
        int		nBuffID;			// BuffID

        STRUCT_INIT(cmd_creature_NoTurnTo)
    };

    //位置
    STRUCT cmd_pos
    {
        unsigned long uUID;				//显示位置的UID
        float fPosition_x;				//位置X
        float fPosition_y;				//位置Y
        float fPosition_z;				//位置Z
        STRUCT_INIT(cmd_pos);
    };

    //位置
    STRUCT cmd_show_win_or_fail
    {
        int nWin;					//0：输 1：赢
        unsigned long nUseTime;     // 使用时间
		float fBasePos_x;			// 失败方水晶位置X
		float fBasePos_y;			// 失败方水晶位置Y
		float fBasePos_z;			// 失败方水晶位置Z
		BYTE isShowResultWnd;		// 是否显示结果窗口
		BYTE isPlayEndAnim;			// 是否播战场结束动画

        STRUCT_INIT(cmd_show_win_or_fail);
    };

    //人物复活数据
    STRUCT cmd_war_important_entiyreliveinfo
    {
    public:

        unsigned long dwRevliveTime;		//  复活时间:毫秒
        unsigned long uReliveUID;			//	复活人物UID
        unsigned long uPBUID;               //  PBUID
        int           nHeroIconID;          //  复活人物的图片id
        int           nCamp;                //  复活人物的阵营
        STRUCT_INIT(cmd_war_important_entiyreliveinfo);
    };

    //重要怪物复活数据
    STRUCT cmd_war_important_monsterliveinfo
    {
    public:
        int		isSampCamp;			//  是否是同阵营
        int		nCamp;				//  复活实体阵营
        int		MasterEntityType;	//  复活实体类型
        int		MasterIconID;		//  复活实体图标ID
        int     isEarlyRelive;      //  是否为提前通知复活
        sp_char_array(szName,32);	//  复活实体名称

        int		nSoundID;
        sp_char_array(szDes,64);	//  描述

        STRUCT_INIT(cmd_war_important_monsterliveinfo);
    };

    //重要怪物复活数据
    STRUCT cmd_war_important_monsterdietxtinfo
    {
    public:
        int		isSampCamp;			//  是否是同阵营
        int		nCamp;				//  死亡实体阵营
        int		MasterEntityType;	//  死亡实体类型
        int		MasterIconID;		//  死亡实体图标ID
        sp_char_array(szName,32);	//  死亡实体名称

        int		nSoundID;
        sp_char_array(szDes,64);	//  描述

        STRUCT_INIT(cmd_war_important_monsterdietxtinfo);
    };

    // 禁用技能数量
#define DISABLE_SPELL_COUNT 10
    // 禁用技能
    STRUCT cmd_creature_disable_spell
    {
    public:
        int		nType;										// 类型(0禁用,非0恢复)
        sp_int_array(nSlotType, DISABLE_SPELL_COUNT);		// 禁用技能类型
        sp_int_array(nSlotIndex, DISABLE_SPELL_COUNT);		// 禁用技能索引
        int		nSpellCount;								// 禁用技能数量

        STRUCT_INIT(cmd_creature_disable_spell);
    };


    //实体战斗小地图信息
    STRUCT cmd_war_entity_fight_minimap_info
    {
    public:
        UID		SrcUID;			//  UID
        int		nType;			//  类型

        STRUCT_INIT(cmd_war_entity_fight_minimap_info);
    };

	//大厅房间进入战场时 玩家对战界面显示的详细信息
	STRUCT cmd_match_playervs_details
	{
	public:
		int		nSex;										// 性别
		sp_char_array( szGradeName, GAME_MSGSTR_MAXSIZE);   // 段位描述
		int		nHeroGameNum;								// 该英雄游戏总数
		float	fHeroWinRate;								// 该英雄游戏胜率

		STRUCT_INIT(cmd_match_playervs_details);
	};

    //大厅房间进入战场时 加载界面显示的信息
    STRUCT cmd_match_load_scene_playerinfo
    {
    public:
        PDBID	idActor;
		int		nSelfInfo;									// 是否为自身信息
        int		nCamp;				                        // 玩家所处的阵营
        int		nHeroID;			                        // 玩家控制英雄的ID
		int     nXPSkillID;									// xp技能ID
		int		nSkinID;									// 玩家控制英雄的皮肤ID
        sp_int_array(nSupportSkill, SUMMONER_SKILL_COUNT);  // 玩家的辅助技能图标ID
        sp_char_array(szVocationName,32);	                // 玩家控制的英雄名字
        sp_char_array(szPlayerName,32);		                // 玩家名字
        int     nLoadSceneRate;                             // 加载场景进度
        int     nPing;
        int     nIsComputer;                                // 是否电脑
		int		nCampIndex;									// 阵营位置
		int		nSelfCamp;									// 当前自己阵营
		cmd_match_playervs_details cmdPlayerVsDetails;		// 详细对战信息
        STRUCT_INIT(cmd_match_load_scene_playerinfo);
    };

    // 显示提示文字
    STRUCT cmd_creature_showTipsText
    {
    public:
        int		nType;							// 提示类型
        sp_char_array(strText,TIPS_TEXT_LEN);	// 提示文字

        STRUCT_INIT(cmd_creature_showTipsText);
    };

    // 改变操作
    STRUCT cmd_creature_changeOperation
    {
    public:
        int		nType;							// 0不切换模式，1切换模式

        STRUCT_INIT(cmd_creature_changeOperation);
    };

    // 魔法类型
    STRUCT cmd_creature_magicType
    {
    public:
        MAGIC_TYPE		nType;						// 魔法类型
        sp_char_array(strMagicName,MAGIC_NAME_LEN);	// 魔法名字
        int				nProgressBarType;			// 进度条类型

        STRUCT_INIT(cmd_creature_magicType);
    };

    // view to logic
#define MAX_MAP_PATH_NUM    128
    STRUCT cmd_map_findpath
    {
    public:
        sp_float_array(start, 3);                   // input：起始点
        sp_float_array(end, 3);                     // input：终点
        int pathcnt;                                // out:   路径长度
        sp_float_array(path, 3*MAX_MAP_PATH_NUM);   // out:   返回寻路的路径点

        STRUCT_INIT(cmd_map_findpath);
    };

    // 移动位置
    STRUCT cmd_creature_move_pos
    {
    public:
        sp_float_array(end, 3);                   // 终点

        STRUCT_INIT(cmd_creature_move_pos);
    };

    // 操作类型
    STRUCT cmd_creature_operation_type
    {
    public:
        int				nType;			// 操作类型

        STRUCT_INIT(cmd_creature_operation_type)
    };

    // 转向方向			ENTITY_TOVIEW_TURN_TO_DIRECTION
    STRUCT cmd_creature_turn_to_direction
    {
    public:
        sp_float_array(ptLoc, 3);       // 转向方向点

        STRUCT_INIT(cmd_creature_turn_to_direction)
    };

    // 设置技能槽			ENTITY_TOVIEW_SET_SPELL_SLOT
    STRUCT cmd_creature_set_slot
    {
    public:
        int			nSlotIndex;		// 槽位置
        int			nSlotLevel;		// 槽等级
        SLOT_TYPE	nSlotType;		// 槽位类型
        int         nOverlaySpell;  // 累积技能(0非累积技能,1累积技能)
        cmd_creature_slotview	slotview;			// 槽位信息

        STRUCT_INIT(cmd_creature_set_slot)
    };


    // 技能点数			ENTITY_TOVIEW_SPELL_POINT
    STRUCT cmd_creature_spell_point
    {
    public:
        int				nSpellPoint;	// 技能点数
        sp_int_array (nAddSpellPoint,SPELL_UPGRADE_SLOT_COUNT);		// 槽可加点

        STRUCT_INIT(cmd_creature_spell_point)
    };


    // 取消施法			ENTITY_CMD_CANCEL_CAST
    STRUCT cmd_creature_cancel_cast
    {
    public:
        int			nSpellID;		// 技能

        STRUCT_INIT(cmd_creature_cancel_cast)
    };


    // 升级技能			ENTITY_CMD_UPGRADE_SPELL
    STRUCT cmd_creature_upgrade_spell
    {
    public:
        int			nSlotIndex;		// 槽位置

        STRUCT_INIT(cmd_creature_upgrade_spell)
    };

    // 隐藏尸体			ENTITY_TOVIEW_HIDE_BODY
    STRUCT cmd_creature_hide_body
    {
    public:
        int			nHideBody;		// 隐藏尸体(0不隐藏，1隐藏)

        STRUCT_INIT(cmd_creature_hide_body)
    };

    STRUCT cmd_war_persongoodsinfo_count
    {
    public:
        int nCount;
		PDBID idActor;

        STRUCT_INIT(cmd_war_persongoodsinfo_count);
    };

    // 人物物品相关信息
    STRUCT cmd_war_persongoodsinfo
    {
    public:
        PDBID	idActor;
        unsigned long		uID;			// 玩家ID
        int		nSlotIndex;		// 索引
        int		nSlotType;		// 槽位类型
        int		nGoodID;		// 物品ID
        int		nCanUse;		// 能否主动使用(0不能，1能)
        int		nCanUseTimes;	// 能使用次数
        int		nFreezeID;		// 使用冷却ID
        int		nSkillID;		// 使用技能ID
        int		nBuffID;		// 使用buffID

        STRUCT_INIT(cmd_war_persongoodsinfo);
    };
    // 购买物品
    STRUCT cmd_war_personbuygoods
    {
    public:
        int		nGoodID;		// 物品ID
        STRUCT_INIT(cmd_war_personbuygoods);
    };

    // 出售物品
    STRUCT cmd_war_personsellgoods
    {
    public:
        int		nGoodID;		// 物品ID
        int		nSlotIndex;		// 物品槽位	
        STRUCT_INIT(cmd_war_personsellgoods);
    };

    // 调整物品位置
    STRUCT cmd_war_personexchangeslot
    {
    public:
        int		nSrcSlotIndex;		// 源物品槽位	
        int		nTargetSlotIndex;	// 目标物品槽位
        STRUCT_INIT(cmd_war_personexchangeslot);
    };

    // 使用物品
    STRUCT cmd_war_usegoods
    {
    public:
        int		nSlotIndex;		// 源物品槽位	
        STRUCT_INIT(cmd_war_usegoods);
    };

    // 连击				ENTITY_TOVIEW_COMBO
    STRUCT cmd_creature_combo
    {
    public:
        int		nComboCount;		// 连击数

        STRUCT_INIT(cmd_creature_combo);
    };

    // 撤销操作        
    STRUCT cmd_war_revocation_operation
    {
    public:
        unsigned long uUID;		// 操作人UID
        STRUCT_INIT(cmd_war_revocation_operation);
    };

    STRUCT cmd_creature_goods_slotview					//物品槽位显示
    {
        int			nSlotIndex;		// 槽位置
        int			nSlotType;		// 槽位类型
        cmd_creature_slotview	slotview;			// 槽位信息

        STRUCT_INIT(cmd_creature_goods_slotview)
    };

    STRUCT cmd_player_skill_state
    {
        int		nID;					// 技能ID
        UID		uOperator;				// 操作者
        int 	nCoolTime;				// 冷却时间
        UID		uPBUID;				    // 操作者PBUID
        STRUCT_INIT(cmd_player_skill_state);
    };

    //ENTITY_CAMP_KILLS_SHOW 显示层双方阵营的杀敌数 
    STRUCT cmd_info_camp_kills
    {
        int		BKills;				// 红方击杀数
        int		RKills;				// 蓝方击杀数
        STRUCT_INIT(cmd_info_camp_kills);
    };

    // 禁止鼠标带动人物转向			ENTITY_TOVIEW_FORBID_MOUSE_TURN_TO
    STRUCT cmd_creature_forbid_mouse_turn_to
    {
    public:
        int		nTime;				// 禁止时间

        STRUCT_INIT(cmd_creature_forbid_mouse_turn_to)
    };

    STRUCT cmd_info_cool_time
    {
    public:
        int			    nID;	    // 冷却id
        float			nCD;		// 冷却时间	
        STRUCT_INIT(cmd_info_cool_time)
    };

    // 播放声音			ENTITY_TOVIEW_PLAY_SOUND
    STRUCT cmd_creature_play_sound
    {
    public:
        int		nSoundID;			// 声音ID
        int		nStop;				// 是否停止,为0播放,1停止

        STRUCT_INIT(cmd_creature_play_sound)
    };

    // 改变上帝模式			ENTITY_TOVIEW_CHANGE_GOD_EYE_MODE
    STRUCT cmd_creature_change_god_eye_mode
    {
    public:
        int		nEnter;			// 进入上帝模式,0退出,不为0进入

        STRUCT_INIT(cmd_creature_change_god_eye_mode)
    };

    // 画技能贴花			ENTITY_TOVIEW_DRAW_SPELL_DECAL
    STRUCT cmd_creature_draw_spell_decal
    {
    public:
        int		nDraw;          // 0不画技能贴花,不为0画技能贴花
        int     nDrawType;      // 技能使用前描画类型
        float	fDistance;		// 技能距离
        float   fAttackRange;	// 攻击范围
        float   fChooseRange;   // 选择范围
        int     nShowTime;      // 显示时间,0一直显示,直到通知关闭,不为0,为显示时间

        STRUCT_INIT(cmd_creature_draw_spell_decal)
    };

    // 魔法影响技能			ENTITY_TOVIEW_MP_EFFECT_SPELL
    STRUCT cmd_creature_mp_effect_spell
    {
    public:
        int			nSlotIndex;		// 槽索引
        SLOT_TYPE	nSlotType;		// 槽位类型	(1、装备 2、技能)
        int         nShowType;      // 显示类型,0不显示灰色图标,1显示灰色图标

        STRUCT_INIT(cmd_creature_mp_effect_spell)
    };

    // 镜头自动转向攻击对象			ENTITY_TOVIEW_ROTATE_CAMERA_TO_TARGET
    STRUCT cmd_creature_rotate_camera_to_target
    {
    public:
        UID         uidTarget;

        STRUCT_INIT(cmd_creature_rotate_camera_to_target)
    };

    STRUCT cmd_Role_Level_Change
    {
    public:
        int			    nUID;	    // 玩家UID
        int			nLevel;		   // 玩家当前等级	

        STRUCT_INIT(cmd_Role_Level_Change)
    };

    STRUCT cmd_match_room_ban_hero_count
    {
    public:
        int nCount;
        STRUCT_INIT(cmd_match_room_ban_hero_count);
    };

    STRUCT cmd_match_room_rule_ban_hero
    {
    public:
        int			nStepID;		// 流程ID 
        int			nRuleType;		//状态
        int			nTimeInterval; // 流程持续时间
        int			bIsSelf;		// 是否是自己可操作
        int			nCamp;
        int			nPos;
        STRUCT_INIT(cmd_match_room_rule_ban_hero);
    };

    STRUCT cmd_match_room_rule_select_hero
    {
    public:
        int			nStepID;		// 流程ID
        int			nRuleType;		//状态
        int			nTimeInterval; // 流程持续时间
        int			bIsSelf;		// 是否是自己可操作
        int			nCamp;
        int			nPos;
        STRUCT_INIT(cmd_match_room_rule_select_hero);
    };

    STRUCT cmd_match_room_hero_can_select
    {
    public:
        int				bCanSelect;		// 是否能选
        int				nHeroID;		// 英雄ID
        STRUCT_INIT(cmd_match_room_hero_can_select);
    };

    STRUCT cmd_match_room_computer_hero_can_select
    {
    public:
        int             nCamp;          // 哪方阵营
        int				bCanSelect;		// 是否能选
        int				nHeroID;		// 英雄ID
        STRUCT_INIT(cmd_match_room_computer_hero_can_select);
    };

    // 	Buff充能层数改变					BUFF_MSG_CHARGE_COUNT_CHANGE
    STRUCT cmd_buff_charge_count_change 
    {
        int             chargeCount;    // 充能层数
        int             nChargeValue;   // 充能数值
        int             nBuffID;        // BuffID
        STRUCT_INIT(cmd_buff_charge_count_change);
    };

    // 插入数据的数量结构
    STRUCT cmd_count
    {
    public:
        int nCount;		// 数量
        STRUCT_INIT(cmd_count);
    };

	// 插入数据的PDBID和数量结构
	STRUCT cmd_pdbid_count
	{
	public:
		int dwPDBID;	// PDBID
		int nCount;		// 数量

		STRUCT_INIT(cmd_pdbid_count);
	};

    // 选人大厅的人物的技能描述
    STRUCT cmd_match_room_hero_skill_desc
    {
    public:
        sp_char_array( heroSkillName, 64 );					// 技能名字
        sp_char_array( heroSkillDes,  256 );                    // 技能描述
        int nSpellIconID;                                       // 技能图片ID
        int nSlotID;                                            // 槽位id
        STRUCT_INIT(cmd_match_room_hero_skill_desc);
    };

    // 召唤师当前默认天赋技能
    STRUCT cmd_SummonerSkill_Info
    {
    public:
        int nIndex;         // 默认召唤师技能位置
        int nSkillId;       // 默认召唤师技能ID
        int nIconId;        // 技能iConId；
        sp_char_array(summonerSkillDesc,SUMMONER_SKILL_DESC); // 技能描述
        STRUCT_INIT(cmd_SummonerSkill_Info);
    };

    // 召唤师技能
    STRUCT cmd_SmAllSkillInfo
    {
    public:
        int nId;                // 技能Id
        int nIconId;            // 图片ID
        int nIsUnlock;          // 是否解锁
        sp_char_array(summonerSkillDesc,SUMMONER_SKILL_DESC); // 技能描述
        STRUCT_INIT(cmd_SmAllSkillInfo);
    };

    //GVIEWCMD_PLAY_SEND_WAR_TYPEID
    STRUCT cmd_send_war_typeid
    {
    public:
        int nConfigWarID;
        int nMatchTypeID;
        STRUCT_INIT(cmd_send_war_typeid);
    };

    // 召唤师可拥有数量
#define SUMMONER_SKILL_COUNT 2
    // 返回选择后的召唤师技能id
    STRUCT cmd_SmSillInfo_Return
    {
    public:
        sp_int_array(nAllSoltSkillId, SUMMONER_SKILL_COUNT);        // 召唤师技能id

        STRUCT_INIT(cmd_SmSillInfo_Return);
    };
    // 大厅人物显示头像总数量（自己英雄数量和系统英雄数量）
    STRUCT cmd_match_room_hero_count
    {
        int		nHeroCount;			//显示英雄数量
        STRUCT_INIT(cmd_match_room_hero_count)
    };
    // 大厅人物技能描述数量
    STRUCT cmd_match_room_hero_skill_desc_count
    {
		int		nHeroID;					// 英雄ID
        int		nSkillDescCount;			//显示技能数量
		//int     nDifficultLevel;			//英雄操作难度
        STRUCT_INIT(cmd_match_room_hero_skill_desc_count);
    };
    // 冷却技能数量
    STRUCT cmd_info_cool_time_count
    {
    public:
        int			    nCDCount;	    // 冷却数量
        STRUCT_INIT(cmd_info_cool_time_count)
    };

    // 快捷键配置信息
    STRUCT cmd_Slot_Shortcut
    {
    public:
        int             nSlotId;        // 槽位id
        int             nType;          // 类型
        int             nSkillId;       // 技能id
        sp_char_array( szShortcut, GAME_SHORTCUT_NAME_MAXSIZE);
        STRUCT_INIT(cmd_Slot_Shortcut);
    };

    STRUCT cmd_entity_matchtype_rank
    {
    public:
        int         bIsRequestSelf;     // 是否请求自己的
        PDBID		RequestFriendID;	// 请求哪个账号的战绩记录

        STRUCT_INIT(cmd_entity_matchtype_rank);
    };

    STRUCT cmd_entity_match_heroscore
    {
    public:
        int        bIsRequestSelf;     // 是否请求自己的
        PDBID		RequestFriendID;	//  请求哪个账号的英雄信息

        STRUCT_INIT(cmd_entity_match_heroscore);
    };


    // 结束后数据信息
    
    STRUCT cmd_entity_end_player_data
    {
    public:
        PDBID   pDbid;                                          // 玩家pdbid
        int     nCamp;                                          // 阵营
        int     nScore;                                         // 获得的分数
        int     nTitle;                                         // 所有称号信息   EWarMedal
        int     nMatchScore;									// 一局天梯分数
        int     nPlayerMatchScore;								// 当前天梯分数
        int     nOldPlayerMatchScore;                           // 上次天梯分数
        int     nMatchType;                                     // 战场类型
		int		nUpgradeRank;									// 天梯段位变化情况(0为不变，小于0为降段位，大于0为升段位)
		int		nPlayerType;									// 玩家类型
        sp_char_array( szPlayerName, GAME_NAME_MAXSIZE);        // 玩家名字
        sp_char_array( szMatchTypeName, GAME_NAME_MAXSIZE);     // 天梯名字
        sp_int_array(gzAllData, ERDT_MAXR);                     // 所有数据

        int nIconId;                                            // 图片id
        int bIsSelf;                                            // 是否是自己的英雄
        int nTalentSkillId1;                                    // 召唤师天赋技能图片id1
        int nTalentSkillId2;                                    // 召唤师天赋技能图片id2
        sp_char_array(summonerSkillDesc1,SUMMONER_SKILL_DESC);  // 技能1描述
        sp_char_array(summonerSkillDesc2,SUMMONER_SKILL_DESC);  // 技能2描述
        sp_char_array( szHeroName, GAME_NAME_MAXSIZE);          // 英雄名字

        STRUCT_INIT(cmd_entity_end_player_data);
    };

    // 召唤师当前默认天赋技能
    STRUCT cmd_Send_UID_And_PBUID
    {
    public:
        int nCamp;          // 人物的阵营
        UID uUID;	        // 人物的uid
        PDBID uPDBID;       // 人物的PDBID
        STRUCT_INIT(cmd_Send_UID_And_PBUID);
    };

    // 鼠标进入槽位       ENTITY_CMD_MOUSE_ENTER_SLOT
    STRUCT cmd_entity_mouse_enter_slot
    {
    public:
        int nEnterSlot;     // 0离开槽位,不为0进入槽位
        int nSlotType;      // 槽位类型
        int nSlotIndex;     // 槽位索引

        STRUCT_INIT(cmd_entity_mouse_enter_slot);
    };

    // 主角聚焦位置       ENTITY_CMD_MASTER_EYE_FOCUS
    STRUCT cmd_entity_master_eye_focus
    {
    public:
        int     isGodMode;      // 0.用户模式，1.上帝模式
        float   x;
        float   y;
        float   z;
        STRUCT_INIT(cmd_entity_master_eye_focus);
    };

    STRUCT cmd_entity_match_type_rank
    {
    public:
        BYTE    nMatchType;
        int     nRankScore;
        int     nGrade;
        sp_char_array( szGradeName, GAME_MSGSTR_MAXSIZE);        // 段位描述
        STRUCT_INIT(cmd_entity_match_type_rank);
    };

    STRUCT cmd_entity_match_hero_score
    {
    public:
        int     nHeroID;	    // 英雄ID
        int     nHeroScore;     // 英雄熟练度得分
        int     nHeroGrade;     // 英雄熟练度段位
        STRUCT_INIT(cmd_entity_match_hero_score);
    };

    STRUCT cmd_Entity_Rank_Info_head
    {
        int nCount;             // 天梯人员数量
        int nOtherCondition;    // 附加数据
        STRUCT_INIT(cmd_Entity_Rank_Info_head);
    };

    // 暂时写一个天梯排名的结构体
    STRUCT cmd_Entity_HighLadder_Rank_Info
    {
    public:
        int nOrderID;           // 排名ID
        unsigned long dwUserID;    // 角色ID
        unsigned long idActor;    // 角色ID
        unsigned long dwHeadID; // 头像ID
        int nType;              // 天梯类型
        int nRankIconID;        // 段位图片ID
        int nRankGrade;	        // 段位
        int nRankScore;         // 天梯分数
        unsigned long   dwClanID;			                    // 帮会ID 
        sp_char_array( szGradeName, GAME_MSGSTR_MAXSIZE);       // 段位描述
        sp_char_array( szPlayerName, GAME_NAME_MAXSIZE);        // 玩家名字
        sp_char_array( szClanName, GAME_NAME_MAXSIZE);        // 联盟名字
        
        STRUCT_INIT(cmd_Entity_HighLadder_Rank_Info);
    };

    STRUCT cmd_entity_kin_order_info
    {
    public:
        int nOrderID;                                           // 排名ID
        unsigned long   dwKinID;			                    // 战队ID
        unsigned long   dwTotalFightCapacity;                   // 战力总和
        unsigned long   dwShaikhID;							    // 族长ID
        unsigned long   dwPlayerCount;                          // 战队人数
        
        sp_char_array( szKinName, GAME_MSGSTR_MAXSIZE);         // 段位描述
        sp_char_array( szShaikhName, GAME_NAME_MAXSIZE);        // 玩家名字

        STRUCT_INIT(cmd_entity_kin_order_info);
    };

    STRUCT cmd_entity_clan_order_info
    {
    public:
        int nOrderID;                                           // 排名ID
        unsigned long   dwClanID;			                    // 帮会ID
        unsigned long   dwTotalFightCapacity;                   // 战力总和
        unsigned long   dwShaikhID;							    // 帮主ID
        unsigned long   dwPlayerCount;                          // 帮会人数

        sp_char_array( szKinName, GAME_MSGSTR_MAXSIZE);         // 段位描述
        sp_char_array( szShaikhName, GAME_NAME_MAXSIZE);        // 玩家名字

        STRUCT_INIT(cmd_entity_clan_order_info);
    };

    STRUCT cmd_entity_kill_order_info
    {
    public:
        int nOrderID;                                           // 排名ID
        unsigned long   dwUserID;                               // 账号ID
        unsigned long   nPDBID;                                 // 玩家ID 
        unsigned long   dwHeadID;                               // 头像ID 
        int		n5KillNum;                                      // 五杀次数
        int		nSuperGodNum;                                   // 超神次数
        int     nMatchTypeID;                                   // 段位类型
        int     nMatchTypeRankGrad;                             // 段位
        int     nRankIconID;                                    // 段位图片ID
        unsigned long   dwClanID;			                    // 帮会ID

        sp_char_array( szGradeName, GAME_MSGSTR_MAXSIZE);       // 段位描述
        sp_char_array( szClanName, GAME_NAME_MAXSIZE);         // 帮会名字
        sp_char_array( szName, GAME_NAME_MAXSIZE);              // 玩家名字

        STRUCT_INIT(cmd_entity_kill_order_info);
    };

    STRUCT cmd_entity_king_of_hero_order_info
    {
    public:
        int nOrderID;                                           // 排名ID
        unsigned long   dwUserID;                               // 账号ID
        unsigned long   nPDBID;                                 // 玩家ID 
        unsigned long   dwHeadID;                               // 头像ID 
        int		nWinMatch;                                      // 胜利场次
        int		nTotalMatch;                                    // 比赛场次
        int     nMatchTypeID;                                   // 段位类型
        int     nMatchTypeRankGrad;                             // 段位
        int     nRankIconID;                                    // 段位图片ID
        unsigned long   dwClanID;			                    // 帮会ID

        sp_char_array( szGradeName, GAME_MSGSTR_MAXSIZE);       // 段位描述
        sp_char_array( szClanName, GAME_NAME_MAXSIZE);         // 帮会名字
        sp_char_array( szName, GAME_NAME_MAXSIZE);              // 玩家名字

        STRUCT_INIT(cmd_entity_king_of_hero_order_info);
    };

    STRUCT cmd_entity_donate_order_info
    {
    public:
        unsigned long   dwUserID;                                   // 账号ID
        unsigned long   nPDBID;                                     // 玩家ID
        unsigned long   dwHeadID;                                   // 头像ID
        int		        nOrder;                                     // 排名 
        int             nClanCtrb;							        // 帮会贡献度 2009.07.29     -- 捐赠
        int             nTotalClanCtrb;						        // 累计帮会贡献度 2009.07.29 -- 总捐赠
        unsigned long   dwClanID;			                        // 帮会ID

        sp_char_array( szClanName, GAME_NAME_MAXSIZE);              // 帮会名字
        sp_char_array( szName, GAME_NAME_MAXSIZE);                  // 玩家名字

        STRUCT_INIT(cmd_entity_donate_order_info);
    };

    STRUCT cmd_entity_hero_order_info
    {
    public:
        int             nOrder;                                   // 排名
        unsigned long   dwHeadID;                                 // 头像ID
        unsigned long   dwUserID;                                 // 账号ID
        unsigned long   dwPlayerID;                               // 玩家ID
        unsigned long   dwHeroID;                                 // 英雄ID
        unsigned long   nHeroScore;                               // 战绩
        int             nHeroGrade;                               // 熟练等级
        int             nHeroStar;                                // 星级
        int             nStarIcon;                                // 星级图标
        unsigned long   dwWinNum;                                 // 比赛胜利场次
        unsigned long   dwMatchNum;                               // 比赛场次
        unsigned long   dwClanID;                                 // 帮会ID

        sp_char_array( szClanName, GAME_NAME_MAXSIZE);            // 帮会名字
        sp_char_array( szName, GAME_NAME_MAXSIZE);                // 玩家名字
        sp_char_array( szHeroName, GAME_NAME_MAXSIZE);            // 英雄名字
        sp_char_array( szStarDesc, GAME_NAME_MAXSIZE);            // 星级描述 

        STRUCT_INIT(cmd_entity_hero_order_info);
    };

    STRUCT cmd_entity_herotype_order_info
    {
    public:
        BYTE    byStaMatchType;          // 0为综合类型 然后为统计某个比赛类型的
        BYTE    byHeroType;             // 英雄的类型（战士 法师 刺客 ADC 辅助 ）

        unsigned long   dwWinHeroID;    // 胜率最高英雄ID
        unsigned long   WinNum;         // 胜利率
        sp_char_array( szWinHeroName, GAME_NAME_MAXSIZE);          // 英雄名字
        
        unsigned long   dwOnStageHeroID;    // 上场率最高英雄ID
        unsigned long   OnStageNum;         // 上场率
        sp_char_array( szOnStageHeroName, GAME_NAME_MAXSIZE);          // 英雄名字

        unsigned long   dwUpperFastestHeroID;    // 最快提升英雄ID
        unsigned long   dwUpperFastestNum;  // 最快提升
        sp_char_array( szUpperFastHeroName, GAME_NAME_MAXSIZE);          // 英雄名字

        STRUCT_INIT(cmd_entity_herotype_order_info);
    };

	STRUCT cmd_entity_mentorship_order_info
	{
	public:
		int		        nOrder;                                     // 排名
        unsigned long   dwUserID;
		unsigned long   dwPDBID;									// 玩家ID
		unsigned long   dwShipVal;									// 师徒值
		sp_char_array( szName, GAME_NAME_MAXSIZE);                  // 玩家名字

		STRUCT_INIT(cmd_entity_mentorship_order_info);
	};

	STRUCT cmd_entity_gemstone_order_info
	{
	public:
		int		        nOrder;                                     // 排名
        unsigned long   dwUserID;									// 玩家ID
		unsigned long   dwPDBID;									// 玩家ID
		unsigned long   dwFaceScore;								// 颜值
		unsigned long   dwPropVal;									// 威力值
        unsigned long   dwClanID;                                   // 联盟ID
		sp_char_array( szName, GAME_NAME_MAXSIZE);                  // 玩家名字

		STRUCT_INIT(cmd_entity_gemstone_order_info);
	};

	// GVIEWCMD_SKIN_ORDER_RANK_INFO 皮肤榜
	STRUCT cmd_entity_skin_order_info
	{
	public:
		int nOrder;										// 排名
		int dwPDBID;			                        // 玩家PDBID
        int dwUserID;                                   // 账号ID
		int	dwSkinCount;								// 皮肤总数量
		int	dwLimitedSkinCount;							// 限量皮肤数量
		int	dwLegendSkinCount;							// 传说皮肤数量
		int	dwEpicSkinCount;							// 史诗皮肤数量
		int	dwNormalSkinCount;							// 普通皮肤数量
        unsigned long   dwClanID;			            // 帮会ID
		sp_char_array(szName, GAME_NAME_MAXSIZE);		// 玩家名称

		STRUCT_INIT(cmd_entity_skin_order_info);
	};

	// GVIEWCMD_HEROCOUNT_ORDER_RANK_INFO 英雄数量榜
	STRUCT cmd_entity_herocount_order_info
	{
	public:
		int nOrder;										// 排名
		int dwPDBID;			                        // 玩家PDBID
        int dwUserID;			                        // 账号ID
		int	dwHeroCount;								// 英雄总数量
        unsigned long   dwClanID;			             // 帮会ID
		sp_char_array(szName, GAME_NAME_MAXSIZE);		// 玩家名称

		STRUCT_INIT(cmd_entity_herocount_order_info);
	};

    // 在选英雄房间内显示其他玩家更换召唤师技能信息
    STRUCT cmd_Summoner_Skill_Change
    {
    public:
        PDBID pDbid;                // 角色pdbid
        int     nPos;               // 角色位置
        int     nCamp;              // 阵营
        sp_int_array( nSummonerSkill, SUMMONER_SKILL_COUNT);
        int     bIsSelf;            // 是否是自己
        STRUCT_INIT(cmd_Summoner_Skill_Change);
    };

    // 被障碍物挡到需要屏蔽不显示在小地图上的
#define ENEMY_HERO_MAX_COUNT 5
    STRUCT cmd_Enemy_Id_Barrier
    {
    public:
        sp_int_array( nEnemyId, ENEMY_HERO_MAX_COUNT)
            STRUCT_INIT(cmd_Enemy_Id_Barrier);
    };

    // 敌人实体Id
    STRUCT cmd_Eemey_Id_Request
    {
    public:
        sp_int_array( nEnemyId, ENEMY_HERO_MAX_COUNT)
            STRUCT_INIT(cmd_Eemey_Id_Request);
    };
    // 敌人周围所有 我方阵营实体
    STRUCT cmd_Enemy_Around
    {
    public:
        int nEnemyId;       // 敌人实体id



        STRUCT_INIT(cmd_Enemy_Around);
    };

    // 改变翅膀状态   ENTITY_TOVIEW_CHANGE_WING_STATE
    STRUCT cmd_Entity_Change_Wing_State
    {
    public:
        WING_STATE_TYPE     nType;       // 翅膀状态
        sp_char_array(szPath, 256);      // 翅膀模型
        float   fSpeed;                  // 飞行速度
        float   fAcceleration;           // 加速度
        float   fMaxSpeed;               // 最高速度
        float   fMinFlyHeight;           // 最小飞行高度
        float   fMaxFlyHeight;           // 最高飞行高度
        float   fUpSpeed;                // 上升速度
		float   fMinCameraAngle;	     // 镜头最小仰角
		float   fMaxCameraAngle;		 // 镜头最大仰角
        float   fCameraDistance;         // 摄像机距离
        int     nEncounterDistance;      // 遭遇距离

        STRUCT_INIT(cmd_Entity_Change_Wing_State);
    };

    // 设置翅膀状态			ENTITY_CMD_SET_WING_STATE
    STRUCT cmd_creature_set_wing_state
    {
    public:
        WING_STATE_TYPE     nType;       // 翅膀状态
        int nEncounterFlag;              // 遭遇标志

        STRUCT_INIT(cmd_creature_set_wing_state)
    };


	//进入飞檐走壁状态      ENTITY_TOVIEW_CHANGE_STATE_WALL_WALKING
	STRUCT cmd_Entity_Change_State_WallWalking
	{
	public:
		float wallCornerAPos_x;
		float wallCornerAPos_z;
		float wallCornerBPos_x;
		float wallCornerBPos_z;
		float wallCornerCPos_x;
		float wallCornerCPos_z;
		float wallCornerDPos_x;
		float wallCornerDPos_z;
		float walkHeight;
		float faceDirection_x;
		float faceDirection_y;
		float faceDirection_z;
		STRUCT_INIT(cmd_Entity_Change_State_WallWalking)
	};



    // 不可选择             ENTITY_TOVIEW_NO_SELECT
    STRUCT cmd_creature_NoSelect
    {
        int     nType;                  // 类型(0增加buffID,不为0移除)
        int     nBuffID;                // BuffID
        int     nSelectTargetFilter;    // 选取目标过滤 TARGET_FILTER

        STRUCT_INIT(cmd_creature_NoSelect)
    };

    // 显示提示UI             ENTITY_TOVIEW_SHOW_PROMPT_UI
    STRUCT cmd_creature_ShowPromptUI
    {
        int		nType;				// 类型(0隐藏,不为0显示)
        int		nPromptID;			// 提示ID
        UID	    uid;	            // 玩家uid

        STRUCT_INIT(cmd_creature_ShowPromptUI)
    };

    // 技能打断             ENTITY_TOVIEW_SPELL_BREAK
    STRUCT cmd_creature_SpellBreak
    {
        int		nSpellID;			// 技能ID

        STRUCT_INIT(cmd_creature_SpellBreak)
    };

#define TALENTPAGE_POINTS_CHANGE 40
#define TALENTPAGE_POINTS_NAME_LEN 32
    STRUCT cmd_TalentPage_Points_Single
    {
        sp_char_array(szTalentPageName,TALENTPAGE_POINTS_NAME_LEN);	      			 // 该天赋页名称
        int    nCurPageIndex;                                           			 // 第几页天赋页
        sp_int_array(nTalentId,TALENTPAGE_POINTS_CHANGE);                            // 当前页的天赋id
        sp_int_array(nTalentPoints,TALENTPAGE_POINTS_CHANGE);                        // 当前页的天赋加点数
        int    nTalentNum;                                                           // 改动的天赋个数
        STRUCT_INIT(cmd_TalentPage_Points_Single)
    };

    STRUCT cmd_Entity_Request_Talent_Page
    {
    public:
        int         bIsRequestSelf;     // 是否请求自己的
        PDBID		RequestFriendID;	// 请求哪个账号的战绩记录

        STRUCT_INIT(cmd_Entity_Request_Talent_Page);
    };

    STRUCT cmd_Entity_Talent_Start_Effect
    {
    public:
        int         nCurTalentPage;     // 哪一天赋页
        STRUCT_INIT(cmd_Entity_Talent_Start_Effect);
    };

    STRUCT cmd_Entity_Talent_Page_Detele
    {
    public:
        int         nCurTalentPage;     // 哪一天赋页
        STRUCT_INIT(cmd_Entity_Talent_Page_Detele);
    };


    // 镜头跟随             ENTITY_TOVIEW_STICKY_CAMERRA
    STRUCT cmd_creature_Sticky_Camerra
    {
        UID     uid;	        // 镜头跟随UID

        STRUCT_INIT(cmd_creature_Sticky_Camerra)
    };

    // 设置目标信息         ENTITY_CMD_SET_TARGET_INFO
    STRUCT cmd_creature_Set_Target_Info
    {
        UID     uidTarget;	        // 目标UID
        float   fX;
        float   fY;
        float   fZ;

        STRUCT_INIT(cmd_creature_Set_Target_Info)
    };

    /////////////////// 符文 Rune //////////////////////
    // 请求符文信息
    // ENTITY_CMD_QUERY_RUNE
    STRUCT cmd_Request_Rune_Info
    {
        int         bIsRequestSelf;     // 是否请求自己的
        PDBID		RequestFriendID;	// 请求哪个账号的符文
        STRUCT_INIT(cmd_Request_Rune_Info)
    };

    STRUCT cmd_Rune_InStore_Item   // （玩家拥有的全部符文）这个结构体是单个id的符文拥有情况，我打包全部的id符文的给你，你解析出数量，再解析cmd_Rune_Store_Single
    {
        int nRuneId;               // 符文id
        int nRuneNum;              // 同id符文数量

        STRUCT_INIT(cmd_Rune_InStore_Item)
    };


	STRUCT cmd_Rune_Info_Total   // 玩家总符文信息
	{
		int	dwActorID;		// 角色ID
		BYTE     nPageNum;  
		STRUCT_INIT(cmd_Rune_Info_Total)
	};

#define RUNE_GROOVE_MAX 7
    STRUCT cmd_Rune_Single_PageInfo   // 这个是每一页的符文镶嵌情况，你选哪页cmd_Rune_Request_Info我发数据给你(或者我可以一开始全部发给你)
    {
        int nPageIndex;
        sp_int_array(nRuneGroove,RUNE_GROOVE_MAX);     // 符文ID数组，每一个符文数组成员对应固定槽位,一个玩家有30个槽位,仅当槽位值为非0值时，该槽位有镶嵌的符文
        sp_char_array(sRunePageName, GAME_NAME_MAXSIZE);
        int nRuneNum;
        STRUCT_INIT(cmd_Rune_Single_PageInfo)
    };

    // 删除符文页
    // ENTITY_CMD_SAVE_RUNE
    STRUCT cmd_Delete_Rune_Page
    {
        int nPageIdx;               // 要保存的符文页页码
        STRUCT_INIT(cmd_Delete_Rune_Page)
    };
    // 效果符文页
    // ENTITY_CMD_SAVE_RUNE
    STRUCT cmd_Effect_Rune_Page
    {
        int nPageIdx;               // 要保存的符文页页码
        STRUCT_INIT(cmd_Effect_Rune_Page)
	};

	STRUCT cmd_Hero_Talent_Info
	{
		int nHeroId;
		int nTalentId;
		int nActive; 
		STRUCT_INIT(cmd_Hero_Talent_Info)
	};

 //   ///////////////////////////////////////////////////

	// ENTITY_CMD_REQ_PLAYER_BASE_INFO
	STRUCT cmd_Request_Player_Info
	{
		PDBID		nActorID;	// 请求哪个账号的玩家信息
        int         bFake;      // 是否要造假数据
		STRUCT_INIT(cmd_Request_Player_Info)
	};

	// ENTITY_CMD_REQ_HERO_INFO
	STRUCT cmd_Request_Hero_Info
	{
		PDBID		nActorID;	// 请求哪个账号的玩家信息
        int         bFake;      // 是否要造假数据
		STRUCT_INIT(cmd_Request_Hero_Info)
	};

    // 玩家基本数据
#define GLORYMAX 27
    STRUCT cmd_player_info_return
    {
        int     bFake;
        PDBID   pDbid;
        int     nLv;
        unsigned long ulCurrentExp;
        unsigned long ulNeedExp;
        unsigned short usHeadID;
		int nHeroID;
		int nSkinID;
        sp_char_array( chClanName, GAME_NAME_MAXSIZE);
        sp_char_array( chClanPos, GAME_NAME_MAXSIZE);
        sp_char_array( chKinName, GAME_NAME_MAXSIZE);
        sp_int_array( nGlory, GLORYMAX );
		sp_longlong_array(llWarID, GLORYMAX);
        unsigned long ulFighting;
        unsigned long ulGloryScore;
        sp_char_array( chMostGlory, GAME_NAME_MAXSIZE);
        int     nRankScore;
		int		nNextRankScore;
        int     nRankIconID;
        sp_char_array( chRankName, GAME_NAME_MAXSIZE);
        sp_char_array( chPlayerName, GAME_NAME_MAXSIZE);
		int		nPKWinNum;
        int nCount;
		int nSupportNum;
		int		nPKTotalNum;
        int     nRankMatchNum;
        int     nRankWinNum;
        // nCount * cmd_entity_hero_info
        STRUCT_INIT(cmd_player_info_return)
    };

    //改为KDA，比赛场次，胜率，杀人数
    // 单个英雄的信息
#define ICONGROUP_COUNT_MAX 10
    STRUCT cmd_entity_hero_info
    {
        int             nHeroID;
        sp_char_array(chHeroName, GAME_NAME_MAXSIZE);
        int             nLv;
        unsigned long   ulUpNeedExp;
        unsigned long   ulExp;
        BYTE            byStarLv;
        unsigned short  usWinPro;
        int             nGameCount;
        unsigned long   ulScore;
        int             nRank;
        unsigned long   ulUseTime;
        int             nCardCount;
        int             nNeedCardCount;
        sp_int_array( nIconID, ICONGROUP_COUNT_MAX);
        int             nCount;
        int             bIsUse;     // 是否正在使用
		int				bIsBuy;		// 是否已购买
		int				bIsWeekFree;// 是否周免
		int				nUseXPSkillID;	// 使用的xp技能id
		unsigned long	ulUnlockTime;
		int				nDiscountCardCount;
        STRUCT_INIT(cmd_entity_hero_info)
    };

	// 本命英雄相关数据
	STRUCT cmd_entity_lifehero_info
	{
		int     nHeroID;                                        // 英雄ID
		int     nCultureID;                                     // 培养仓ID
		STRUCT_INIT(cmd_entity_lifehero_info)
	};

	// 删除指定英雄ID的本命英雄
	STRUCT cmd_entity_lifehero_del
	{
		int heroID;
		STRUCT_INIT(cmd_entity_lifehero_del)
	};

	// 替换新的英雄
	STRUCT cmd_entity_lifehero_replace
	{
		int heroID;
		cmd_entity_lifehero_info hero;
		STRUCT_INIT(cmd_entity_lifehero_replace)
	};

	// 两个英雄互换培养仓
	STRUCT cmd_entity_lifehero_exchange
	{
		int heroID1;
		int heroID2;
		STRUCT_INIT(cmd_entity_lifehero_exchange)
	};
	// 查找所有本命英雄
	STRUCT cmd_entity_lifehero_query
	{
		int actorID;
		STRUCT_INIT(cmd_entity_lifehero_query)
	};

    // 实体状态
    ENUM PROPERTY_TYPE
    {
        PT_BASEVALUE = 0,           // 基础数值
        PT_GROWRATE,                // 成长率
        PT_BASEVALUEADD,            // 基础数值增加量/下一等级的基础值
        PT_GROWRATEADD,             // 成长率增加量/下已登记的成长率
        PT_MAX
    };
    // 单个英雄的详细信息
    STRUCT cmd_entity_hero_detail_info
    {
        bool            bIsHaveHero;                    // 是否拥有这个英雄
        int             nHeroID;                        // 英雄ID
        int             nLv;                            // 英雄等级
        int             nLimitLv;                       // 英雄等级上限
        sp_char_array( chHeroName, GAME_NAME_MAXSIZE);  // 英雄名字
		sp_char_array( chStarDesc, GAME_NAME_MAXSIZE);	// 星级描述
        unsigned long   nExp;                           // 经验
        unsigned long   nNeedExp;                       // 升级需要经验
        int             nStar;                          // 星级
        int             nStarExp;                       // 升星经验
        int             nStarNeedExp;                   // 升星需要经验
        unsigned long   nScore;                         // 评分
        int             nGameCount;                     // 出场次数
        unsigned short  usWinPro;                       // 胜利几率
        unsigned long   ulRecordScore;                  // 战绩得分
        sp_int_array( nIconID, ICONGROUP_COUNT_MAX);
        int             nCount;

        sp_float_array( fHP, 4);                        // 血量信息 0代表基础数值， 1代表成长 2,代表下一级的基本数值， 3代表下一级的成长
        sp_float_array( fMagic, 4);                     // 魔法信息 0代表基础数值， 1代表成长 2,代表下一级的基本数值， 3代表下一级的成长
        sp_float_array( fPA, 4);                        // 物理攻击信息 0代表基础数值， 1代表成长 2,代表下一级的基本数值， 3代表下一级的成长
        sp_float_array( fMA, 4);                        // 魔法攻击信息 0代表基础数值， 1代表成长 2,代表下一级的基本数值， 3代表下一级的成长
        sp_float_array( fPD, 4);                        // 物理防御信息 0代表基础数值， 1代表成长 2,代表下一级的基本数值， 3代表下一级的成长
        sp_float_array( fMD, 4);                        // 魔法防御信息 0代表基础数值， 1代表成长 2,代表下一级的基本数值， 3代表下一级的成长

        STRUCT_INIT(cmd_entity_hero_detail_info)
    };

    // 升星前后英雄信息差别及消耗
    STRUCT cmd_entity_hero_diff_expend
    {
        int nHeroID;                                    // 英雄ID
        int nStarLv;                                    // 星级
        sp_char_array( chHeroName, GAME_NAME_MAXSIZE);  // 英雄名字
        sp_float_array( fHP, 4);                        // 血量信息 0代表基础数值， 1代表成长,2代表基础值增加， 3代表成长增加
        sp_float_array( fMagic, 4);                     // 魔法信息 0代表基础数值， 1代表成长,2代表基础值增加， 3代表成长增加
        sp_float_array( fPA, 4);                        // 物理攻击信息 0代表基础数值， 1代表成长,2代表基础值增加， 3代表成长增加
        sp_float_array( fMA, 4);                        // 魔法攻击信息 0代表基础数值， 1代表成长,2代表基础值增加， 3代表成长增加
        sp_float_array( fPD, 4);                        // 物理防御信息 0代表基础数值， 1代表成长,2代表基础值增加， 3代表成长增加
        sp_float_array( fMD, 4);                        // 魔法防御信息 0代表基础数值， 1代表成长,2代表基础值增加， 3代表成长增加
        int nExpedGold;                                 // 消耗金币
        int nHaveGold;                                  // 当前拥有的金币卡数量
        int nExpendCard;                                // 消耗英雄卡
        int nHaveCardCount;                             // 当前拥有的英雄卡数量
        int nLimitLv;                                   // 英雄等级上限
        int nLimitLvAdd;                                // 英雄等级上限增加值
		int nDiscountGold;								// 打折金币
		int nDiscountCard;								// 打折卡牌
        STRUCT_INIT(cmd_entity_hero_diff_expend)
    };

    // 填充未有英雄信息
    STRUCT cmd_entity_nohanve_hero
    {
        int     nHeroID;                                // 英雄id
        int     nNeedCardCount;                         // 升星需要的卡牌
        int     nHaveCardCount;                         // 拥有的英雄卡牌的数量
        sp_char_array(chHeroName, GAME_NAME_MAXSIZE);   // 英雄名字
		int		nUseXPSkillID;							// 使用的xp技能id
		int     nDiscountCardCount;                     // 升星打折的卡牌
        STRUCT_INIT(cmd_entity_nohanve_hero)
    };

    // 更新的卡牌信息
    STRUCT cmd_entity_update_card_info
    {
        int             nCardID;      
		int             nCardCount;
        STRUCT_INIT(cmd_entity_update_card_info)
	};

	// ENTITY_CMD_QUERY_HERO_DETAIL_INFO
	STRUCT cmd_Request_Hero_Detail_Info
	{
		PDBID		nActorID;			// 请求哪个账号的玩家信息
		int			nHeroID;			// 对应英雄ID
		cmd_entity_hero_info heroInfo;  // 对应英雄信息
		STRUCT_INIT(cmd_Request_Hero_Detail_Info)
	};

	// ENTITY_TOVIEW_RETURN_HERO_DETAIL_INFO
	STRUCT cmd_Return_Hero_Detail_Info
	{
		int	nPDBID;						// 请求哪个账号的玩家信息
		int nHeroID;					// 对应英雄ID
		STRUCT_INIT(cmd_Return_Hero_Detail_Info)
	};

	//ENTITY_TOVIEW_UPGRADE_SUCESS
	STRUCT cmd_Return_HeroDetail_Upgrade_Success
	{
		int  nCurStarLv;
		int  nSucess;
		int  nSpecialEffect;
		int	 nOldStarIconID;
		int  nCurStarIconID;
		sp_char_array( chStarDesc, GAME_NAME_MAXSIZE);	// 星级描述
		STRUCT_INIT(cmd_Return_HeroDetail_Upgrade_Success)
	};

	// ENTITY_TOVIEW_UPDATE_WARSTATISTIC_TOTAL
	// 更新战绩总览数据
	STRUCT cmd_entity_update_warstatistic_total
	{
		int		nActorID;				// 角色ID
		int		nRecentKillCount;		// 近30天杀敌数
		int		nRecentDieCount;		// 近30天死亡数
		int		nRecentAssistCount;		// 近30天助攻数
		int		nRecentKDAScore;		// 近30天KDA值
		int		nRecentWinCount;		// 近30天胜场
		int		nRecentLoseCount;		// 近30天负场
		int		nRecentScore;			// 近30天战绩评分

		int		nHeroInfoCount;			// 英雄信息数量
		int		nMapInfoCount;			// 地图信息数量
		int		nWarInfoCount;			// 战绩信息数量

		STRUCT_INIT(cmd_entity_update_warstatistic_total)
	};

	// 战绩总览英雄信息数据
	STRUCT cmd_entity_update_warstatistic_total_heroinfo
	{
		int		nHeroID;				// 英雄ID
        sp_char_array(szHeroName, GAME_NAME_MAXSIZE);   // 英雄名字
		int		nHeroWinNum;			// 胜利场次
		int		nHeroMatchNum;			// 总场次

		STRUCT_INIT(cmd_entity_update_warstatistic_total_heroinfo)
	};

	// 战绩总览地图信息数据
	STRUCT cmd_entity_update_warstatistic_total_mapinfo
	{
		int		nMatchType;		// 比赛类型
		int		nMatchNum;		// 比赛次数
		int		nWinNum;		// 胜利场次
		int		nGrade;			// 段位等级
		int		nWinPoint;		// 胜点

		STRUCT_INIT(cmd_entity_update_warstatistic_total_mapinfo)
	};

	// 战绩总览历史战绩信息数据
	STRUCT cmd_entity_update_warstatistic_total_warinfo
	{
		__int64	nWarID;			// 战场ID
		int		nBeginTime;		// 战场开始时间
		int		nHeroID;		// 英雄ID
		int		nWarType;		// 战场类型
		int		nWin;			// 是否胜利(0:失败，1:胜利)
		int		nWinType;		// 胜利类型(1:甩锅局, 2:躺赢局, 3:Carry局, 4:碾压局, 5:福利局, 6:尽力局, 7:挂机局, 8:翻盘局)
		sp_int_array(nBanHeroID1, BAN_HERO_MAXSIZE);	// 阵营1禁用英雄列表
		sp_int_array(nBanHeroID2, BAN_HERO_MAXSIZE);	// 阵营2禁用英雄列表

		STRUCT_INIT(cmd_entity_update_warstatistic_total_warinfo)
	};

	// ENTITY_TOVIEW_UPDATE_WARSTATISTIC_WAR
	// 更新历史战绩数据
	STRUCT cmd_entity_update_warstatistic_war
	{
		__int64	nWarID;			// 战场ID

		int		nBeginTime;		// 战场开始时间
		int		nEndTime;		// 战场结束时间
		int		nWarType;		// 战场类型
		int		nEndBecause;	// 结束原因 1、代表阵营1胜利 2、代表阵营2胜利 3、代表阵营1投降 4、代表阵营2投降
		sp_int_array(nBanHeroID1, BAN_HERO_MAXSIZE);	// 阵营1禁用英雄列表
		sp_int_array(nBanHeroID2, BAN_HERO_MAXSIZE);	// 阵营2禁用英雄列表

		int		nActorCount;	// 参与玩家数量

		STRUCT_INIT(cmd_entity_update_warstatistic_war)
	};

	// 历史战绩单个角色数据
	STRUCT cmd_entity_update_warstatistic_war_actorinfo
	{
		int		nWorldID;                              // 游戏世界ID
		int		nUserID;                               // 用户ID
		int		nActorID;                              // 角色ID
		sp_char_array(szActorName, ACTORNAME_MAXSIZE); // 角色名称
		sp_int_array(nEquipID, MAX_GOODS_SLOT_COUNT);  // 装备id  下标代表槽位
		int		nHeroID;                               // 英雄ID
		int		nHeroLevel;                            // 英雄等级
		int		nScore;								   // 综合评分
		int		nGetMoney;                             // 累计金币数
		int		nDieCount;                             // 死亡次数
		int		nAssistCount;                          // 助攻次数
		int		nKillPersonNum;                        // 击杀人物数量
		int		nKillMonsterNum;                       // 击杀怪物数
		int		nContinueKillMax;                      // 最大连杀数
		int		nMultiKillMax;                         // 最大多杀
		int		nCrildDamageMax;					   // 最大暴击伤害
		int		nKillTowerNum;                         // 推塔数
		int		nOutHeroPhysics;                       // 对英雄的总物理伤害
		int		nOutHeroMagic;                         // 对英雄的总法术伤害
		int		nOutHeroTrue;						   // 对英雄的总真实伤害
		int		nOutTotal;                             // 总输出
		int		nDamageTotal;                          // 承受的总伤害
		int		nBearMagic;							   // 承受的法术伤害
		int		nBearPhysics;						   // 承受的物理伤害
		int		nBearTrue;
		int		nCureTotal;                            // 总治疗量
		int		nCamp;                                 // 所属阵营  CAMP_TYPE  1代表阵营1 2代表阵营2
		int		nRecoveryHP;						   // 恢复的生命值

		int		nHeroType;                             // 英雄类型
		int		nKdaScore;                             // kda得分
		int		nGlory;                                // 获得的荣耀
		int		nWinType;                              // 胜利类型
		int		nPlayerType;						   // 玩家类型 玩家类型 (1 玩家人 2 机器人 待续)
		int		nAliveAssistMax;					   // 连助攻最大
		bool	bFirstBlood;						   // 是否第一滴血
		int		nGPM;									// 每分钟金钱
		int		nDeadlyControl;							// 夺命控制
		int     nMoneyRate;								// 金钱比 扩大一百倍
		int     nOutputRate;							// 输出率 扩大一百倍
		int     nJoinBattleRate;						// 参战率 扩大一百倍
		int		nThrillSave;							// 惊险救治
		int		nKillDragen;							// 击杀小龙
		int		nXPSkillID;								// XP技能ID
		int		nSummerID;								// 召唤师技能ID
		BYTE	bySex;									// 性别：0未知，1男，2女

		STRUCT_INIT(cmd_entity_update_warstatistic_war_actorinfo)
	};

	// ENTITY_CMD_REQ_WARSTATISTIC_TOTAL
	// 请求战绩总览信息
	STRUCT cmd_warstatistic_req_total
	{
		int		nActorID;		// 角色ID

		STRUCT_INIT(cmd_warstatistic_req_total)
	};

	// ENTITY_CMD_REQ_WARSTATISTIC_WAR
	// 请求指定战绩信息
	STRUCT cmd_warstatistic_req_war
	{
		__int64	nWarID;			// 战场ID
        BYTE    byWnd;          // 返回到哪个界面 EGetSingleWarType

		STRUCT_INIT(cmd_warstatistic_req_war)
	};

	// 请求玩家历史战绩列表，用于入选本命英雄依据
	STRUCT cmd_warstatistic_req_history_record
	{
		unsigned long      dwActorID;                                // 角色ID
		STRUCT_INIT(cmd_warstatistic_req_history_record)
	};

	// 返回玩家历史战绩列表
	STRUCT cmd_warstatistic_history_record
	{
		int               wWarType;                               // 战场类型
		bool              bIsFull;                                // 是否满人
		int               nHeroID;                                // 英雄ID
		int               wDieCount;                              // 死亡次数
		int               wAssistCount;                           // 助攻次数
		int               wKillPersonNum;                         // 击杀人物数量
		STRUCT_INIT(cmd_warstatistic_history_record)
	};

    // 设置智能施法     ENTITY_CMD_SET_SMART_CAST
    STRUCT cmd_entity_set_smart_cast_head
    {
        int     nCount;         // 条数

        STRUCT_INIT(cmd_entity_set_smart_cast_head)
    };
    
    STRUCT cmd_entity_set_smart_cast
    {
        int		nSlotType;		// 槽位类型	(1、装备 2、技能)
        int		nSlotIndex;		// 槽索引
        int     nSmartCast;     // 施法类型(为0默认施法,1选择施法,2设置智能施法)

        STRUCT_INIT(cmd_entity_set_smart_cast)
    };

    // 战场结束返回奖励 ENTITY_TOVIEW_RETURN_PLAYER_AWARK
    STRUCT cmd_entity_return_player_awark
    {
        __int64   ulWarID;            // 战场唯一id
        bool    bIsWin;             // 是否胜利
        int     nActorGetExp;       // 获得角色经验
		int     nActorCurExp;       // 当前角色经验
		int     nActorOldExp;       // 以前角色经验
	    int     nActorOldNeedExp;   // 角色上一级所需角色经验
		int     nActorNeedExp;		// 角色当前等级所需角色经验
        int     nHeroExp;           // 英雄获得经验
		int     nHeroOldNeedExp;    // 英雄上一级所需角色经验
        int     nHeroNeedExp;       // 英雄升级需要经验
        int     nGold;              // 获得绑定金币
        int     nSuccessScore;      // 成就得分
        int     nTime;              // 剩余冷却时间
        int     nHeroOldExp;        // 英雄以前的经验
        int     nHeroOldLv;         // 英雄以前的英雄等级
        int     nCurrentExp;        // 英雄当前经验
        int     nCurrentLv;         // 英雄当前等级
        int     nPlayerOldLv;       // 玩家旧等级
        int     nPlayerNewLv;       // 玩家新等级 
		int		nAntiReduceRate;	// 防沉迷衰减速率
        int     nCount;             // 英雄类型的数量
        // nCount * cmd_entity_hero_card
        STRUCT_INIT(cmd_entity_return_player_awark)
    };

    STRUCT cmd_entity_hero_card
    {
        int nHeroID;
        int nCount;
        sp_char_array(chHeroCardName, GAME_LONGSTR_MAXSIZE);
        sp_char_array(chHeroCardDesc, GAME_LONGSTR_MAXSIZE);
        int nCardFrameID;
        STRUCT_INIT(cmd_entity_hero_card)
    };

    // 静态场景英雄升级 ENTITY_TOVIEW_RETURN_STATICHERO_UPGRADE
    STRUCT cmd_entity_statichero_upgrade
    {
        int nHeroID;
        int nStarLv;                                    // 星级
        sp_char_array( chHeroName, GAME_NAME_MAXSIZE);  // 英雄名字
        int nFrontLv;
        int nCurrntLv;
        int nCurrntExp;

        sp_float_array( fHP, 4);                        // 血量信息 0代表基础数值， 1代表成长,2代表基础值增加， 3代表成长增加
        sp_float_array( fMagic, 4);                     // 魔法信息 0代表基础数值， 1代表成长,2代表基础值增加， 3代表成长增加
        sp_float_array( fPA, 4);                        // 物理攻击信息 0代表基础数值， 1代表成长,2代表基础值增加， 3代表成长增加
        sp_float_array( fMA, 4);                        // 魔法攻击信息 0代表基础数值， 1代表成长,2代表基础值增加， 3代表成长增加
        sp_float_array( fPD, 4);                        // 物理防御信息 0代表基础数值， 1代表成长,2代表基础值增加， 3代表成长增加
        sp_float_array( fMD, 4);                        // 魔法防御信息 0代表基础数值， 1代表成长,2代表基础值增加， 3代表成长增加

        STRUCT_INIT(cmd_entity_statichero_upgrade);
    };

    // 是否显示升星信息
    STRUCT cmd_entity_is_show_hintinfo
    {
        int nIsShow;
        int nHeroID;
        STRUCT_INIT(cmd_entity_is_show_hintinfo);
    };

    // 首胜提示 ENTITY_TOVIEW_FIRSTWIN_NOTICE
    STRUCT cmd_entity_firstwin_notice
    {
        int nLeftTime;

        STRUCT_INIT(cmd_entity_firstwin_notice)
    };

	// ENTITY_TOVIEW_UPDATE_NOVICEGUIDEDATA
	STRUCT cmd_entity_update_voviceguidedata
	{
		int guidedata;

		STRUCT_INIT(cmd_entity_update_voviceguidedata)
	};

    // 打断循环技能         ENTITY_CMD_BREAK_LOOK_ATTACK
    STRUCT cmd_creature_break_look_attack
    {
        int nBreak;     // 0不打断，不为0打断

        STRUCT_INIT(cmd_creature_break_look_attack)
    };

    //// 购买英雄结果 ENTITY_TOVIEW_BUYHERO_RESULT
    //STRUCT cmd_entity_buyhero_result
    //{
    //    int nHeroID;        // 英雄id
    //    int nHeroStaticLv;  // 英雄静态等级
    //    BYTE byHeroStarLv;    // 英雄星级
    //    int nResult;    // 返回结果 结果类型ENBUYHEROTYPE
    //    STRUCT_INIT(cmd_entity_buyhero_result);
    //};

    // ENTITY_CMD_REQ_CHANGEHERO  请求切换英雄信息（英雄、皮肤）
    STRUCT cmd_entity_change_hero
    {
        int nOptType;       // 当前操作类型:0.切英雄（带皮肤ID），1.切皮肤(带英雄ID)
        int nChangeID;      // 改变id
        int nReserveData;   // 备用数据

        STRUCT_INIT(cmd_entity_change_hero);
    };

    // ENTITY_TOVIEW_SHOW_TITLE  显示称号信息
    STRUCT cmd_entity_show_title_node
    {
        int nTitleID;      // 称号ID
        sp_char_array(szTitleName, GAME_NAME_MAXSIZE);

        STRUCT_INIT(cmd_entity_show_title_node);
    };


	STRUCT cmd_entity_teammate_info_count
	{
		int nteammateCount;
		int nCaptainPDBID;
		STRUCT_INIT(cmd_entity_teammate_info_count);
	};

	STRUCT cmd_entity_teammate_info
	{
		int nUserID;
		int nPDBID;
		int nHeadID;
		int nLevel;
		int isCaptain;
		int nRankScore;
        int nRankGrade;
		BYTE byMatchType;
		sp_char_array(szName, GAME_NAME_MAXSIZE);
		STRUCT_INIT(cmd_entity_teammate_info);
	};

	STRUCT cmd_Entity_Hero_Talent
	{
		int nHeroTalentId;
		STRUCT_INIT(cmd_Entity_Hero_Talent);
	};

    // ENTITY_TOVIEW_SET_OBJ_RANGE_TIP  设置目标范围提示
    STRUCT cmd_entity_SetObjRangeTip
    {
    public:
        int bAdd;           // 不为0添加,为0删除
        int nObjID;         // 目标ID
        int nRange;         // 范围

        STRUCT_INIT(cmd_entity_SetObjRangeTip)
    };
    // ENTITY_CMD_REQ_OPENCHEST  请求打开宝箱
    STRUCT cmd_entity_open_chest
    {
		int nChestType;

        STRUCT_INIT(cmd_entity_open_chest);
    };

	// ENTITY_TOVIEW_UPDATE_CHEST_INFO
	STRUCT cmd_entity_update_chest_info
	{
		sp_int_array(nChestCount, PRIZECHEST_TYPE_MAX);

		STRUCT_INIT(cmd_entity_update_chest_info);
	};

	// ENTITY_TOVIEW_OPEN_CHEST_RESULT
	STRUCT cmd_entity_open_chest_result
	{
		int nChestType;
		sp_int_array(nPrizeID, PRIZEGROUP_RESULT_MAX);

		STRUCT_INIT(cmd_entity_open_chest_result);
	};

    // PK选择目标			ENTITY_CMD_PK_CHOOSE_TARGET
    STRUCT cmd_creature_pk_choose_target
    {
    public:
        UID uidTarget;

        STRUCT_INIT(cmd_creature_pk_choose_target);
    };

  // 请求PK			    ENTITY_TOVIEW_REQUEST_PK
    STRUCT cmd_creature_pk_request_pk
    {
    public:
        int bShowPKPrompt;     // 显示PK提示

        STRUCT_INIT(cmd_creature_pk_request_pk)
    };
    
    // 显示PK信息			ENTITY_TOVIEW_SHOW_PK_INFO
    STRUCT cmd_creature_pk_show_pk_info
    {
    public:
        UID     uidOperator;         // 请求切磋者
        sp_char_array(szInfo,256);   // 切磋提示信息
        unsigned long dwTime;        // 失效时间

        STRUCT_INIT(cmd_creature_pk_show_pk_info)
    };

    // 操作信息			    ENTITY_CMD_PK_HANDLE_INFO
    STRUCT cmd_creature_pk_handle_info
    {
    public:
        UID     uidOperator;    // 切磋者
        int     nAgree;         // 是否同意PK,1同意PK,0不同意PK

        STRUCT_INIT(cmd_creature_pk_handle_info)
    };

    // 取消PK			    ENTITY_CMD_PK_CANCEL_PK
    STRUCT cmd_creature_pk_cancel_pk
    {
    public:
        int     nCancel;         // 取消PK,0不取消PK,不为0取消PK

        STRUCT_INIT(cmd_creature_pk_cancel_pk)
    };

	// 皮肤 ENTITY_TOVIEW_HERO_SKIN_INFO
    STRUCT cmd_Hero_Skin_Info
    {
    public:
        int nHeroId;           // 英雄id 
        int nSkinId;           // 皮肤id
        unsigned long dwDurationTime;    // 皮肤时限
        int nSkinLevel;        // 皮肤品质
        STRUCT_INIT(cmd_Hero_Skin_Info);
    };

    // 通知显示层开始PK			ENTITY_TOVIEW_START_PK
    STRUCT cmd_creature_pk_start_pk
    {
    public:
        int     bStart;            // 为0结束PK,不为0开始PK
        UID     uidTarget;         // PK目标
        sp_float_array(vLoc, 3);   // PK中点
        int     nResult;           // PK结果(0输,1胜,2平)
        int     nCamp;             // 阵营关系(显示层需要)
        int     bForceEnd;          // 是否是强制结束

        STRUCT_INIT(cmd_creature_pk_start_pk)
    };

	// ENTITY_TOVIEW_UPDATE_POINTSHOP_BUYNUM_INFO
	STRUCT cmd_entity_update_pointshop_buynum_info
	{
		int nSellID;
		int nBoughtNum;

		STRUCT_INIT(cmd_entity_update_pointshop_buynum_info);
	};

	// ENTITY_TOVIEW_POINTSHOP_BUY_RESULT
	STRUCT cmd_entity_pointshop_buyresult
	{
		int nSucceed;

		STRUCT_INIT(cmd_entity_pointshop_buyresult);
	};

	// ENTITY_TOVIEW_POINTSHOP_UPDATE_CARDLIST
	STRUCT cmd_entity_pointshop_update_cardlist
	{
		unsigned long nLeftReflushTime;     // 剩余刷新时间
		int nLeftReflushNum;				// 剩余手动刷新次数
		sp_int_array(nSellID, POINTSHOP_CARDLIST_MAXCOUNT);

		STRUCT_INIT(cmd_entity_pointshop_update_cardlist);
	};

	// ENTITY_CMD_POINTSHOP_BUYGOODS
	STRUCT cmd_entity_pointshop_buygoods
	{
		int nSellID;
		int nResType;
		int nClientPrice;
		int nBuyNum;

		STRUCT_INIT(cmd_entity_pointshop_buygoods);
	};

	// ENTITY_TOVIEW_HEROUPSTAR_UNLOCKPRIZE
	STRUCT cmd_entity_heroupstar_unlockprize
	{
		int nHeroID;
		int nStarLevel;

		STRUCT_INIT(cmd_entity_heroupstar_unlockprize);
	};

    //  同步通知命令 ENTITY_TOVIEW_NOTIFY_DATA,
    STRUCT cmd_entity_notify_data
    {
        int nType;          // 通知类型 NOTIFY_DATA_TYPE
        int nValue;         // 通知数值
        int nStrLen;        // 文本长度

        STRUCT_INIT(cmd_entity_notify_data);
    };

    // 某种类型对应数据
    STRUCT cmd_Type_data
    {
        int     nID;                    // 皮肤ID
        int     nType;                  // 类型ID
        int     nOldValue;              // 旧数据
        int     nNewValue;              // 新数据

        STRUCT_INIT(cmd_Type_data);
    };
    // 英雄数据
    STRUCT cmd_hero_data
    {
    int     nHeroID;                // 英雄ID
    int     nXpID;                  // Xp技能ID

    STRUCT_INIT(cmd_hero_data);
    };
    //////////////////////////////////////////////////////////////////////////

    STRUCT cmd_entity_set_orderinfo
    {
        sp_int_array(szInfo,GEOS_MAX);   // 个人排行榜设置

        STRUCT_INIT(cmd_entity_set_orderinfo);
    };

    // v2c创建房间 
    STRUCT cmd_order_herotype_request_info
    {
    public:
        // 房间名
        int nMatchType;

        STRUCT_INIT(cmd_order_herotype_request_info);
    };

	// 特殊实体选取 目前有玩家选取祭坛
	STRUCT cmd_Select_Special_Entity
	{
	public:
		UID uidOperator;   // 操作者
		UID uidTarget;     // 选中的怪物uid
		int nFunctionType; // 为了以后扩展 加了类型 ESpecial_Select_Function_Type 目前只有 祭坛读条 ESSEFT_ALTARHANDIN

		STRUCT_INIT(cmd_Select_Special_Entity);
	};

	// 特殊实体范围搜寻 目前有瞭望台搜寻玩家
	STRUCT cmd_Special_Entity_Range_Search
	{
	public:
		UID uidOperator;   // 操作者
		UID uidTarget;     // 选中的怪物uid
		int nFunctionType; // 为了以后扩展 加了类型 ESpecial_Entity_Range_Search_Function_Type目前只有 瞭望台搜寻玩家 ESRSFT_WATCHOUTTOWER
		bool bSearchOrBreak;

		STRUCT_INIT(cmd_Special_Entity_Range_Search);
	};

	// 特殊实体范围搜寻 状态改变
	STRUCT cmd_Special_Entity_Search_State_Change
	{
	public:
		UID uidOperator;     // 特殊实体搜索者uid
		bool bSearch;

		STRUCT_INIT(cmd_Special_Entity_Search_State_Change);
	};

    // 改变轻功状态   ENTITY_TOVIEW_CHANGE_DODGE_STATE
    STRUCT cmd_Entity_Change_Dodge_State
    {
    public:
        int     nType;              // 轻功状态(0退出,不为0进入)
        float   fSpeedUpGround;     // 地面上时加速的速度
        float   fSpeed0;            // 第一次加速的速度
        float   fSpeed1;            // 第二次加速的速度
        float   fSpeed2;            // 第三次加速的速度
        int     nSpellID;           // 退出加速技能ID

        STRUCT_INIT(cmd_Entity_Change_Dodge_State);
    };

	
	// 龙珠指定实体属性信息 ENTITY_CMD_ENTITY_PROPERTY_DRAGONBALL
	STRUCT cmd_Entity_Property_Prefamce_Req
	{
		UID		uidReq;           

		STRUCT_INIT(cmd_Entity_Property_Prefamce_Req);
	};

	// 龙珠指定实体属性信息 ENTITY_TOVIEW_WAR_ENTITY_PROPERTY
	STRUCT cmd_Entity_Property_Prefamce_Return
	{
		UID		uidReq;           
		int		nPa;           
		int     nHp;
		int		nPd;
		int		nMd;

		STRUCT_INIT(cmd_Entity_Property_Prefamce_Return);
	};

    // 显示提示UI             ENTITY_TOVIEW_SHOW_CHOOSE_TARGET
    STRUCT cmd_creature_ShowChooseTarget
    {
        int		nType;				// 类型(0隐藏,不为0显示)
        int     nSpellID;           // 后续技能ID
        int     nTargetType;        // 显示选择目标类型 SHOW_CHOOSE_TARGET_TYPE
        int     nDistance;          // 距离

        STRUCT_INIT(cmd_creature_ShowChooseTarget)
    };

    // 禁用技能数量
    #define PERSPECTIVE_SHOW_COUNT 5
    // 透视显示               ENTITY_TOVIEW_PERSPECTIVE_SHOW
    STRUCT cmd_creature_PerspectiveShow
    {
        int		nType;				// 类型(0隐藏,不为0显示)
        int     nCount;             // 玩家数量
        sp_int_array(uidTarget, PERSPECTIVE_SHOW_COUNT); // 显示玩家

        STRUCT_INIT(cmd_creature_PerspectiveShow)
    };

    // 改变狙击镜   ENTITY_TOVIEW_CHANGE_SNIPERSCOPE
    STRUCT cmd_Entity_Change_Sniperscope
    {
    public:
        int     nType;                  // 狙击镜状态(0退出,不为0进入)
        float   fFOV;                   // 视角偏移值
        sp_float_array(vTargetTile,3);  // 目标点,0为面向

        STRUCT_INIT(cmd_Entity_Change_Sniperscope);
    };

	// 阵营队伍人数
	STRUCT cmd_Camp_Count
	{
	public:
		sp_int_array(vCampCount, DOTA2_CAMP_MAX_NUM);	// 阵营玩家数量
        int nMapID;                 // 地图ID
        int nMatchType;				// 战场类型
		int bObEnter;				// 是否Ob
		STRUCT_INIT(cmd_Camp_Count);
	};

	// ENTITY_CMD_ACTOR_NAME_FIX
	STRUCT cmd_Actor_Name_Fix
	{
		sp_char_array(szNewName,32);   // 名字
		int nCostType;				   // 金币花费类型 (0点券1金币)	
		STRUCT_INIT(cmd_Actor_Name_Fix)
	};


	// ENTITY_TOVIEW_SPECCIAL_CONTRIBUTION，玩家特殊贡献信息同步(惊现救治 夺命控制 团控)
	STRUCT cmd_War_Special_Contribution
	{
		int		nType;
		int		nOperatorHeroID;
		int		nParam1;
		sp_char_array(szOperatorName,GAME_NAME_MAXSIZE);
		STRUCT_INIT(cmd_War_Special_Contribution)
	};

	// ENTITY_TOVIEW_ACTOR_INFO_CAHNGE
	STRUCT cmd_actor_info_change
	{
		int nType;								// 改变类型
		int nValue1;							// 改变数值1
		int nValue2;							// 改变数值2
		sp_char_array(str1,GAME_NAME_MAXSIZE);	// 改变字符串1
		sp_int_array( nGlory, GLORYMAX );		// 改变荣耀
		sp_longlong_array(llWarID, GLORYMAX);	// 改变战绩ID
		STRUCT_INIT(cmd_actor_info_change)
	};

	// ENTITY_TOVIEW_BENEFITCARD 增益卡
	STRUCT cmd_benefit_card_list_head
	{
		int nCount;
		int nListType;
		STRUCT_INIT(cmd_benefit_card_list_head)
	};
	// ENTITY_TOVIEW_BENEFITCARD 增益卡
	STRUCT cmd_benefit_card
	{
		int nBenefitCardType;			// 增益卡类型（1：金币增益卡 2：角色经验增益卡 3：英雄经验增益卡）
		int nBenefitCardSubType;		// 增益卡使用范围类型（1：时间天数 2：次数，一局消耗一次）
		unsigned long nBenefitValue;    // 当nBenefitCardSubType的值为1时，该值是一个Unix时间戳，表示过期时间；为2时，表示剩余使用次数
		int nMultiple;					// 增益卡的增益倍数
		STRUCT_INIT(cmd_benefit_card)
	};

	// ENTITY_CMD_ENTITY_SET_NOVICEGUIDE 设置新手引导类型
	STRUCT cmd_entity_set_noviceguide
	{
		int nNoviceGuideType;
		STRUCT_INIT(cmd_entity_set_noviceguide);
	};
    ////////////////////////////////////皇冠///////////////////
	// 宝石个体信息
	STRUCT cmd_Crown_Gemstone_Item_Info
	{
	public:
		int nGemstoneID;				// 宝石类型id 如红宝石 蓝宝石
		int	nGemstoneDBID;				// 宝石个体唯一id
		BYTE byGrade;					// 宝石品级
		int nExp;						// 宝石当前经验
		int bAppraisal;					// 是否鉴定成功过
		int nAppraisalCount;			// 宝石鉴定次数   
		int	nSlotID;					// 位置
		STRUCT_INIT(cmd_Crown_Gemstone_Item_Info);
	};

	STRUCT cmd_Crown_Page_Item_Info
	{
	public:
		int	nPageDBID;																		// 皇冠页数			
		sp_int_array(crownInlaySlotList,CROWN_PAGE_INLAY_MAX);								// 皇冠宝石镶嵌槽位列表
		sp_char_array(szName,GAME_NAME_MAXSIZE);	                                 
		STRUCT_INIT(cmd_Crown_Page_Item_Info);
	};

	STRUCT cmd_Crown_Gemstone_Product_Info
	{
	public:
		int bGemstoneProduceEnd;														// 产出是否结束 结束:数据清0 
		sp_int_array (cobblestonGemstoneIDList,CROWN_PAGE_PRODUCEGEMSTONEMAX);			// 原矿宝石(未选取)随机列表
		sp_int_array (windowGemstoneIDList,CROWN_PAGE_PRODUCEGEMSTONEMAX);				// 开窗宝石(花点券开窗)列表
		sp_int_array (cobStoneIDList,CROWN_PAGE_PRODUCEGEMSTONEMAX);					// 开窗宝石(花点券开窗)列表
		int bOpenWinEnd;		                                 
		STRUCT_INIT(cmd_Crown_Gemstone_Product_Info);
	};

	// 宝石鉴定 ENTITY_CMD_CROWN_AGEMSTONEPPRAISAL,	
	STRUCT cmd_Crown_Gemstone_Appraisal_Info
	{
	public:
		sp_int_array (gemstoneDBIDList,2);												// 鉴定宝石列表	                                 
		STRUCT_INIT(cmd_Crown_Gemstone_Appraisal_Info);
	};

	// 删除皇冠页 ENTITY_CMD_CROWN_DELETEPAGE,	
	STRUCT cmd_Crown_Page_Delete_Info
	{
	public:
		int nPageDBID;																   // 皇冠页页码                          
		STRUCT_INIT(cmd_Crown_Page_Delete_Info);
	};

	// 选择原石 ENTITY_CMD_CROWN_DCOBBLESTON_SELECTED,	
	STRUCT cmd_Crown_Cobbleston_Selected
	{
	public:
		int nGemstoneID;														                        
		STRUCT_INIT(cmd_Crown_Cobbleston_Selected);
	};

	// 选择开窗原石 ENTITY_CMD_CROWN_DWINDOWSTONE_SELECTED,	
	STRUCT cmd_Crown_WindowStone_Selected
	{
	public:
		int nGemstoneID;														                        
		int nSlotID;
		STRUCT_INIT(cmd_Crown_WindowStone_Selected);
	};

	// 宝石出售 ENTITY_CMD_CROWN_DGEMSTONE_SEIL,
	STRUCT cmd_Crown_Gemstone_Seil
	{
	public:
		int nGemstoneDBID;														                        
		STRUCT_INIT(cmd_Crown_Gemstone_Seil);
	};

	// 请求别人皇冠页 ENTITY_CMD_CROWN_DOTHER_PAGE,	
	STRUCT cmd_Crown_Request_Other_Page
	{
	public:
		PDBID		RequestFriendID;														                        
		STRUCT_INIT(cmd_Crown_Request_Other_Page);
	};

	// 宝石鉴定结果 ENTITY_TOVIEW_CROWNG_GEMSTONE_APPRAISALRET
	STRUCT cmd_Crown_Gemstone_Appraisal_Ret
	{
	public:
		int nGemstoneDBID;
		int nCritNum;
		BYTE nAppreaisalRet;

		STRUCT_INIT(cmd_Crown_Gemstone_Appraisal_Ret);
	};
    ////////////////////////////////////皇冠 end///////////////////
	// 点赞ENTITY_TOVIEW_WAR_FILTER_SUPPORT_PERSON
	STRUCT cmd_update_support_vote_person
	{
		PDBID uPDBIU;
		int nVoteID;		// 点赞评选类型
		int nSortID;
		STRUCT_INIT (cmd_update_support_vote_person);
	};
	// 添加标签	ENTITY_CMD_WAR_ADD_LABEL
	STRUCT cmd_war_add_label
	{
		PDBID targetPdbid;
		int nLabelID;
		sp_char_array(szOperatorName, ACTORNAME_MAXSIZE);
		sp_char_array(szTargetName, ACTORNAME_MAXSIZE);

		STRUCT_INIT (cmd_war_add_label);
	};
	
	// 标签列表 ENTITY_TOVIEW_LABEL_LIST
	STRUCT cmd_label_info
	{
		int nLabelID;
		int nLabelNum;

		STRUCT_INIT(cmd_label_info);
	};

	// 英雄通用结构首
	STRUCT cmd_Hero_Additional_Info
	{
	public:
		int nHeroID;
		int nAdditionalCount;
		STRUCT_INIT(cmd_Hero_Additional_Info);
	};

	//  玩家英雄xp技能同步 ENTITY_TOVIEW_HERO_XP_SKILL_UPDATE
	STRUCT cmd_Hero_XP_Skill_Info
	{
	public:
		int nHeroID;           // 英雄id 
		int nXPSkillID;        // XP技能id
		int	nGuide;			   // 是否需要xp引导	
		STRUCT_INIT(cmd_Hero_XP_Skill_Info);
	};
	
	// 玩家英雄xp技能碎片同步 ENTITY_TOVIEW_HERO_XP_FRAGMENT_UPDATE
	STRUCT cmd_Hero_XP_Fragment_Info
	{
	public:
		int nHeroID;           // 英雄id 
		int nXPSkillID;		   // XP技能id
		int nXPFragmentCount;  // XP技能碎片数量
		STRUCT_INIT(cmd_Hero_XP_Fragment_Info);
	};



	//////////////////////////////////////NPC///////////////////////////////
	// 玩家触发NPC
	STRUCT cmd_entity_npc_trigger
	{
	public:
		UID uidNPC;

		STRUCT_INIT(cmd_entity_npc_trigger);
	};

	// 玩家点击NPC功能
	STRUCT cmd_entity_npc_func
	{
	public:
		UID uidNPC;										// NPC
		sp_char_array(szHTMLRequest, NPC_HTMLREQ_MAXSIZE);	// 请求字符串

		STRUCT_INIT(cmd_entity_npc_func);
	};

    // 超级跳
    STRUCT cmd_creature_longjump
    {
        float fUpTime;  // 上升时间
        float fUpHeight; // 上升高度

        STRUCT_INIT(cmd_creature_longjump);
    };

    // 滑翔
    STRUCT cmd_creature_glide
    {
        float fGlideSpeed;        // 滑翔速度
        float fFallSpeed;         // 滑翔速度

        STRUCT_INIT(cmd_creature_glide)
    };

	// 拾取金币
	STRUCT cmd_pick_hp
	{
		int entityID;
		STRUCT_INIT(cmd_pick_hp)
	};

	// 更新玩家游戏状态
	STRUCT cmd_entity_update_gamestate
	{
		int nGameState;		// 游戏状态

		STRUCT_INIT(cmd_entity_update_gamestate)
	};

	// 初始化战场持续时间,ENTITY_TOVIEW_INI_WARCONTINUE_TIME
	STRUCT cmd_war_init_war_continue_time
	{
		unsigned int nWarStartTime;			// 战场正式开始时间

		STRUCT_INIT(cmd_war_init_war_continue_time)
	};
	
#define WARQUIPSCHEME_EQUIP_MAXSIZE 12
	// 装备方案
	STRUCT cmd_war_equipscheme
	{
		int nHeroID;												// 英雄ID
		BYTE bySchemeID;											// 装备方案ID
		sp_char_array(szSchemeName, GAME_NAME_MAXSIZE);				// 装备方案名字
		sp_char_array(szAuthorName, ACTORNAME_MAXSIZE);				// 装备方案作者名字
		sp_char_array(szTitle, GAME_NAME_MAXSIZE);					// 装备称号
		sp_int_array(nEquipIDList, WARQUIPSCHEME_EQUIP_MAXSIZE);	// 装备ID列表

		STRUCT_INIT(cmd_war_equipscheme);
	};

	// 玩家英雄装备方案列表		ENTITY_TOVIEW_EQUIPSCHEME_LIST
	STRUCT cmd_war_equipscheme_list
	{
		unsigned long pdbid;	// 玩家ID
		int nHeroID;			// 英雄ID
		int nSchemeCount;		// 方案个数

		STRUCT_INIT(cmd_war_equipscheme_list);
	};

	// 增加装备方案 ENTITY_CMD_WAR_ADD_EQUIPSCHEME
	STRUCT cmd_war_add_equipscheme
	{
		int nHeroID;
		sp_char_array(szSchemeName, GAME_NAME_MAXSIZE);				// 装备方案名字
		sp_char_array(szAuthorName, ACTORNAME_MAXSIZE);				// 装备方案作者名字
		sp_char_array(szTitle, GAME_NAME_MAXSIZE);					// 装备方案称号
		sp_int_array(nEquipIDList, WARQUIPSCHEME_EQUIP_MAXSIZE);	// 装备ID列表

		STRUCT_INIT(cmd_war_add_equipscheme);
	};

	// 读取装备方案 ENTITY_CMD_WAR_DEL_EQUIPSCHEME
	STRUCT cmd_war_del_equipscheme
	{
		int nHeroID;
		BYTE bySchemeID;											// 装备方案ID

		STRUCT_INIT(cmd_war_del_equipscheme);
	};

	// 修改装备方案 ENTITY_CMD_WAR_MODIFY_EQUIPSCHEME
	STRUCT cmd_war_modify_equipscheme
	{
		int nHeroID;
		BYTE bySchemeID;											// 装备方案ID
		sp_char_array(szSchemeName, GAME_NAME_MAXSIZE);				// 装备方案名字
		sp_char_array(szAuthorName, ACTORNAME_MAXSIZE);				// 装备方案作者名字
		sp_char_array(szTitle, GAME_NAME_MAXSIZE);					// 装备方案称号
		sp_int_array(nEquipIDList, WARQUIPSCHEME_EQUIP_MAXSIZE);	// 装备ID列表

		STRUCT_INIT(cmd_war_modify_equipscheme);
	};

	// 读取装备方案 ENTITY_CMD_WAR_READ_EQUIPSCHEME
	STRUCT cmd_war_read_equipscheme
	{
		int nHeroID;

		STRUCT_INIT(cmd_war_read_equipscheme);
	};


	// 已激活天赋的列表 ENTITY_TOVIEW_ACTIVE_TALENT_LIST
	STRUCT cmd_war_active_talent_list
	{
		int pdbid;
		int nCurTalent;
		sp_int_array(nTalentList, MAX_ACTIVE_TALENT_COUNT);

		STRUCT_INIT(cmd_war_active_talent_list);
	};

	// 战场信息 ENTITY_TOVIEW_WAR_INFO
	STRUCT cmd_war_info
	{
		unsigned int nWarStartTick;
		long long lDBWarID;
		int nConfigWarID;
		bool isFull;
		int nMtachType;

		STRUCT_INIT(cmd_war_info);
	};

    // 战场信息 ENTITY_TOVIEW_WAR_INFO
    STRUCT cmd_war_end_legendcup_info
    {
        int             eRoundType;
        int             bRoundLastNode;
        int             bIsOnlineCup;
        int             bIsValid;
        int             bIsLastRound;
        int             bNodeEnd;
        int             bWin;
        unsigned int    dwNextStartInterval;
        unsigned int    dwNextRoundStartTime;

        STRUCT_INIT(cmd_war_end_legendcup_info);
    };


	// 翻滚到目标点(AI) ENTITY_TOVIEW_AI_ROLL
	STRUCT cmd_roll_2_target
	{
		sp_float_array(loc, 3);
		
		STRUCT_INIT(cmd_roll_2_target);
	};

	// 自动前进 ENTITY_TOVIEW_AUTO_MOVE
	STRUCT cmd_auto_move
	{
		int nOpenFlag;        // 1开始 0结束
		float fStartSpeed;    // 初始速度
		float fAcceleration;  // 加速度
		float fMaxSpeed;	  // 最大速度

		STRUCT_INIT(cmd_auto_move);
	};

	// 显示debuff进度条 ENTITY_TOVIEW_SHOW_DEBUFF_PROGRESS_BAR
	STRUCT cmd_show_debuff_progress_bar
	{
		int nOpenFlag;  // 1开始 0结束
		int nType; // TypeDebuff  0定身 1封魔 2晕眩 3减速
		int nTime; // 持续时间 ms

		STRUCT_INIT(cmd_show_debuff_progress_bar)
	};

	// 英雄详细信息 ENTITY_CMD_REQ_UPGRADESTARANDHEROPRO
	STRUCT cmd_get_hero_detail_info
	{
		int nHeroID;
		int nStarLv;
		int nStaticLv;

		STRUCT_INIT(cmd_get_hero_detail_info)
	};

    // 玩家段位降级或者升级 ENTITY_TOVIEW_RANK_DWON_OR_UPGRADE
    STRUCT cmd_entity_rank_down_or_upgrade
    {
    public:
        int     nDownOrUp;  // 0：不变 1： 升 -1：降
        int     nPreGrade;
        int     nPreScore;

        int     nRankScore;
        int     nGrade;
        int     nRankIconID;
        sp_char_array( chRankName, GAME_NAME_MAXSIZE);
        STRUCT_INIT(cmd_entity_rank_down_or_upgrade);
    };

	// 玩家是否离开   ENTITY_CMD_GAMESTATE_LEAVE
	STRUCT cmd_entity_gamestate_leave
	{
	public:
		int isLeave;		// 0:未离开，1：离开(C# bool值为4个字节)

        STRUCT_INIT(cmd_entity_gamestate_leave);
	};

	STRUCT cmd_add_match_info
	{
	public:
		int nMatchTypeID;				// 类型ID
		int nAvgWaitTimes;				// 平均等待时间
        int nCurMatchPlayerCount;       // 匹配到的玩家数量
        int nMaxPlayerCount;
		STRUCT_INIT(cmd_add_match_info);
	};

	// 战场切换职业  ENTITY_CMD_WAR_CHANGE_VOCATION
	STRUCT cmd_entity_war_change_vocation
	{
	public:
		int nGenicSlotID;		// 基因槽位ID

		STRUCT_INIT(cmd_entity_war_change_vocation);
	};

    STRUCT SlotSpellID
    {
    public:
        sp_int_array(arrSpellID, 4);                                // 槽位技能ID
        STRUCT_INIT(SlotSpellID)
    };

	// 返回排位赛季历史数据 ENTITY_TOVIEW_RANK_SEASON_DETAIL
	STRUCT cmd_entity_rank_season_detail	
	{
		int	dwEndTime;				// 赛季截止时间

		int	wTopRankGrade;			// 最高段位等级
		int	wMatchCount;			// 比赛场次
		int	wWinCount;				// 胜利场次
		int	wContinueWinCount;		// 最高连胜场次
		int	wMvpCount;				// MVP数量
		int	wSuperGodCount;			// 超神数量
		int	wKill5Count;			// 5杀数量
		int	wKill4Count;			// 4杀数量
		int	wKill3Count;			// 3杀数量
		
		int wPrizeConfigID;			// 赛季奖励ID

		STRUCT_INIT(cmd_entity_rank_season_detail);
	};

	// 返回排位赛季历史数据 ENTITY_TOVIEW_RANK_SEASON_RECORD
	#define MATCH_SEASON_HERO_RECORD_COUNT 15
	STRUCT cmd_entity_rank_season_record_node	
	{
		int	wSeasonID;		// 赛季ID
		int	wRankGrade;		// 段位等级
		int	wMatchCount;	// 总场次
		int	wWinCount;		// 胜场
		sp_int_array(wHeroRecord, MATCH_SEASON_HERO_RECORD_COUNT);	// 英雄数据
		sp_char_array(szSeasonTitle, GAME_NAME_MAXSIZE);			// 赛季名称
		int nBeginTime;		// 赛季开始时间
		int nEndTime;		// 赛季结束时间

		STRUCT_INIT(cmd_entity_rank_season_record_node);
	};
}
using namespace GameLogic;
