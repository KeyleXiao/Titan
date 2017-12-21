/*******************************************************************
** 文件名:	GameDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	9/9/2014
** 版  本:	1.0
** 描  述:	

			游戏全局规则定义
********************************************************************/

#pragma once

#ifndef _MANAGED
	#include "Vector3.h"
#endif
#include "NameLenDef.h"
/**
	概念描述：
	1. 事件(Event) : 游戏中实体或者系统触发的消息，其他系统可订阅该事件，这样可在事件执行时得到通知
	                 也可以否决事件的执行(同进程通讯)
    2. 命令(Command) : 通过客户端，脚本，服务器控制台，GM命令等，向实体发出命令，实体执行后会触发事件
	                   命令是从外到内的，事件是从内到外的(同进程通讯)
    3. 网络消息码(NetMsg) : 服务器客户端之间的通讯协议(跨进程通讯)

	某些情况下，三个概念很容易混淆，比如服务器收到客户端一个攻击数据包，
	首先经过MessageDispatch解码成对实体的控制命令，实体执行时，触发内部事件
*/

#define    SEGMENT_SPAN            8000                             // 为方便事件ID消息之间的转换，分段定义ID，默认最多支持1000个事件命令和消息
#define    NETMSG_OFFSET           80 			                    // 这个偏移值可以定期变化,这样客户端的消息码经常调整，外挂会比较难以分析
#define    EVENT_OFFSET            (SEGMENT_SPAN)                   // 事件ID取值
#define    CMD_OFFSET              (EVENT_OFFSET+SEGMENT_SPAN)      // 命令ID取值

//#define    NETMSG_2_CMD(id)        (id)                           // 不用去变，只是概念上变了，因为取值跨度进行了区分，所以不会冲突
//#define    CMD_2_EVENT(id)         (id)                           // 不用去变，只是概念上变了，因为取值跨度进行了区分，所以不会冲突

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define    GAME_VIEW_CMD_OFFSET    CMD_OFFSET   		            // 客户端显示层命令偏移
#define    ENTITY_VIEW_CMD_OFFSET  CMD_OFFSET+200                   // 客户端实体显示层命令偏移

// 全局数据类型定义: 唯一实体ID
typedef unsigned long  UID;                                         // 实体唯一UID
typedef unsigned long  UDBID;                                       // 账号ID，账号是唯一的，一个账号可以在多个区有多个角色
typedef unsigned long  PDBID;                                       // 角色ID
typedef unsigned long long LargeID;                                 // 

//////////////////////////////////////////////////////////////////////////
#define  MAX_ACTOR_NUM                      6                       // 每个账号最多能创建的角色数


//////////////////////////////////////////////////////////////////////////
// 无效地图id
#define INVALID_MAPID                           0
// 无效场景ID
#define INVALID_SCENE_ID                       -1
// 无效的BUFF ID
#define INVALID_BUFF_ID                         0
// 无效的效果ID
#define INVALID_EFFECT_ID                       0
// 无效的WarID
#define INVALID_WAR_ID                         -1
// 无效的角色ID
#define INVALID_PDBID                           0
// 无效的游戏ID
#define INVALID_WORLDID                         0
// 数组长度
#define ARRAY_LENGTH                            2048

// 最大静态地图数
#define MAP_MAXCOUNT                            200
// 主城镜像地图最小值
#define MIN_MIRROR_MAPID                        1
// 主城镜像地图最大值
#define MAX_MIRROR_MAPID                        170

#define MIGRATE_SKEYLEN                         32

// 槽位对应快捷键名称最大长度
#define  GAME_SHORTCUT_NAME_MAXSIZE             48

// 逻辑定时帧率（每20毫秒更新一次)
#define  UPDATE_FRAME_TIMER                     20

// 技能名字长度
#define SPELL_NAME_LEN                          64
// 技能描述长度
#define SPELL_DESC_LEN                          1024

// 动作名字长度
#define ANIMATION_NAME_LEN                      16
// 光效名字长度
#define LIGHTING_NAME_LEN                       64

// 声音名字长度
#define SOUND_NAME_LEN                          16

// 槽位描述长度
#define SLOT_DESC_LEN                           1024

// 召唤师技能数量
#define SUMMONER_SKILL_COUNT                    2

// 召唤师技能名字长度
#define SUMMONER_SKILL_NAME                     32
// 召唤师技能描述
#define SUMMONER_SKILL_DESC                     256

// 最强英雄数量
#define STRONGEST_HERO_COUNT                    5

#define INVALID_UDBID                           0

// 人物最大等级
#define PERSON_MAX_LEVEL                        120

// 单次获得卡牌的最大数量(策划说要1000……)
#define GETCARD_MAX_NUM                         1000

// 队友最大值
#define TEAM_MAX_COUNT                          10

// 数据库与实际角色坐标放大倍数
#define ACTOR_LOCATION_SCALE                    10000.0
#define ACTOR_LOCATION_SCALE_RESTORE            0.0001

// 永久的皮肤时间
#define PERMANENT_SKIN_TIME                     0

// 公共消息框标题长度
#define COMMON_MESSAGE_BOX_CAPTION_LEN          64

// 公共消息框文本长度
#define COMMON_MESSAGE_BOX_TEXT_LEN             256

// 公共消息框文本长度
#define COMMON_LEGENDCUP_NAME_LEN               64

// 师徒发布信息内容的长度
#define MENTORSHIP_ISSUE_LEN                    96

// 邮件物品最大个数
#define MAIL_GOODS_MAX_NUM                      10

// 客户端与服务器实际坐标放大倍数
#define LOCATION_SCALE							10000.0
#define LOCATION_SCALE_RESTORE					0.0001

//#define PRINTF_PLANLOG
//#define DEBUG_HAVESAMECLAN
//////////////////////////////////////////////////////////////////////////


// 槽位中 技能的类型枚举
enum ESlot_Skill_Type
{
    EST_SUMMONER1 = 1,              // 召唤师技能槽位1
    EST_SUMMONER2,                  // 召唤师技能槽位2

    EST_MAX
};

// xp技能添加类型
enum ESkillXp_AddState
{
	ESKILLXP_ADDSTATE_RANDOMPLAYER = 1,         // XP技能随机状态冷却中
	ESKILLXP_ADDSTATE_ADDXPBUFF,                // XP技能添加BUFF使用冷却完毕

	ESKILLXP_ADDSTATE_MAX,
};
//////////////////////////////////////////////////////////////////////////

// 获得英雄的途径
enum ENGETHEROWAY
{
    ENGETHEROWAY_NORMAL = 0,                    // 正常途径

    ENGETHEROWAY_MAX,
};

// 英雄卡牌概率类型
enum ENHEROCARDPROTYPE
{
    ENHEROCARDPROTYPE_MAINHERO = 0,             // 主英雄概率
    ENHEROCARDPROTYPE_TEAMMATE,                 // 队友英雄概率
	ENHEROCARDPROTYPE_PRIVATEHERO,              // 私有英雄概率
    ENHEROCARDPROTYPE_WEEKFREE,                 // 周免英雄概率

    ENHEROCARDPROTYPE_MAX,
};

// 经验类型
enum ENEXPCREATETYPE
{
    ENEXPCREATETYPE_PLAYER = 1,                 // 玩家的经验产出
    ENEXPCREATETYPE_HERO,                       // 英雄的经验产出

    ENEXPCREATETYPE_MAX,
};

//////////////////////////////////////////////////////////////////////////

// 是否输出STUB函数执行情况
//#define SERVICE_STUB_FUNCTION_OUTPUT

// 强制转换类型
#ifndef CAST_TYPE
#define CAST_TYPE(TYPE, EXP)               ((TYPE)(EXP))
#endif
#ifndef STATIC_CAST
#define STATIC_CAST(TYPE, EXP)             (static_cast<TYPE>(EXP))
#endif
#ifndef CONST_CAST
#define CONST_CAST(TYPE, EXP)              (const_cast<TYPE>(EXP))
#endif


#ifdef _WIN64
#   define SIZE_INT(x)                      (int)(x)
#   define SIZE_DWORD(x)                    (DWORD)(x)
#else
#   define SIZE_INT(x)                      (x)
#   define SIZE_DWORD(x)                    (x)
#endif

// 是否动态场景
#define IsDynamicScene(nSceneID)            ((nSceneID) > MAP_MAXCOUNT)
// 是否为主城地图（包括镜像)
#define IsMainMap(nMapID)                   ((nMapID) >= MIN_MIRROR_MAPID && (nMapID) <= MAX_MIRROR_MAPID)
// 失败主城地图ID
#define FormatMap(nMapID)                   (IsMainMap(nMapID) ? 1 : (nMapID ))
// 是否为GM号
#define IsGM(ActorDBID)                     ( (ActorDBID) < 80000 )

// int->longlong
#define MAKELONGLONG(a, b)      ((LONGLONG)((((ULONG64)(a)) & 0xffffffff) | ((((ULONG64)(b)) & 0xffffffff) << 32)))
// longlong->DWORD
#define LOWLONG(ll)           ((DWORD)(((ULONG64)(ll)) & 0xffffffff))
#define HIGHLONG(ll)           ((DWORD)((((ULONG64)(ll)) >> 32) & 0xffffffff))

// 用于探测实体在各个端之间的网络消息时间
//#define ENTITY_MESSAGE_ROUTE_TIME_HACK