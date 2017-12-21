/*******************************************************************
** 文件名:	NameLenDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-3-23
** 版  本:	1.0
** 描  述:	游戏名字长度定义
** 注  意:  如果和IGameDatabaseDef.h 有交替的，
            要和IGameDatabaseDef.h定义的要保持对应
			
********************************************************************/

#ifndef __COMMON_NAMELENDEF_H__
#define __COMMON_NAMELENDEF_H__

// tolua_begin

// 角色名长度
#ifndef     ACTORNAME_MAXSIZE
#   define  ACTORNAME_MAXSIZE		32
#endif

// 角色名长度
#ifndef     USERNAME_MAXSIZE
#   define  USERNAME_MAXSIZE		32
#endif

// 名称最大长度
#ifndef     GAME_NAME_MAXSIZE
#   define  GAME_NAME_MAXSIZE		32
#endif

// 密码最大长度
#ifndef      GAME_PWD_MAXSIZE
#   define  GAME_PWD_MAXSIZE		42
#endif

// ID最大长度
#ifndef		GAME_CARD_MAXSIZE		
#	define	GAME_CARD_MAXSIZE		32
#endif

// 信息文字最大长度,64
#ifndef     GAME_MSGSTR_MAXSIZE
#   define  GAME_MSGSTR_MAXSIZE		64
#endif

// 标题字串的长度
#ifndef     GAME_TITLE_MAXSIZE
#   define  GAME_TITLE_MAXSIZE		128
#endif

// 长字串的长度
#ifndef     GAME_LONGSTR_MAXSIZE
#   define  GAME_LONGSTR_MAXSIZE	256
#endif

// 大字串的长度
#ifndef     GAME_TEXTSTR_MAXSIZE
#   define  GAME_TEXTSTR_MAXSIZE	512
#endif

// 家族名字长度
#ifndef     KIN_NAME_SIZE
#   define  KIN_NAME_SIZE			32
#endif

// 家族技能名字长度
#ifndef     KINSKILL_NAME_SIZE
#   define  KINSKILL_NAME_SIZE		17
#endif

// 家族宣言的长度
#ifndef     KIN_MANIFESTO_SIZE
#   define  KIN_MANIFESTO_SIZE		256
#endif

// 家族公告的长度
#ifndef     KIN_NOTICE_SIZE
#   define  KIN_NOTICE_SIZE		256
#endif

// 家族口号长度
#ifndef     KIN_SLOGAN_SIZE
#   define  KIN_SLOGAN_SIZE		256
#endif

// 家族标签长度
#ifndef     KIN_LABEL_SIZE
#   define  KIN_LABEL_SIZE		32
#endif

// 帮会名字长度
#ifndef     CLAN_NAME_SIZE
#   define  CLAN_NAME_SIZE		32	
#endif

// 帮会宣言的长度
#ifndef     CLAN_MANIFESTO_SIZE
#	define	CLAN_MANIFESTO_SIZE	40
#endif

// 帮会公告的长度
#ifndef     CLAN_NOTICE_SIZE
#	define	CLAN_NOTICE_SIZE    256
#endif

// title最大长度
#ifndef     TITLE_NAME_SIZE
#	define	TITLE_NAME_SIZE    32
#endif

// 名称最大长度
#ifndef     GAME_ROOMPWD_MAXSIZE
#   define  GAME_ROOMPWD_MAXSIZE		64
#endif

// 杯赛战队最大数量
#ifndef     GAME_LEGENDCUP_MAXKINSIZE
#   define  GAME_LEGENDCUP_MAXKINSIZE		64
#endif

// 好友分组名称最大长度
#ifndef		BUDDYGROUP_NAME_MAXSIZE
#	define	BUDDYGROUP_NAME_MAXSIZE		32
#endif

// 字体名称最大长度
#ifndef FONT_NAME_MAXSIZE
#	define FONT_NAME_MAXSIZE	32
#endif


// 好友备注名最大长度
#ifndef REMARK_NAME_MAXSIZE
#	define REMARK_NAME_MAXSIZE 32
#endif

// 心情最大长度
#ifndef		MOOD_CONTENT_MAXSIZE
#	define	MOOD_CONTENT_MAXSIZE		128
#endif

// 好友在线状态保留字符串最大长度
#ifndef STATUS_REVERSE_STR_MAXSIZE
#	define STATUS_REVERSE_STR_MAXSIZE 32
#endif

// 段位名称最大长度
#ifndef		RANK_NAME_MAXSIZE
#	define RANK_NAME_MAXSIZE	32
#endif

// 游戏世界名称最大长度
#ifndef GAMEWORLD_NAME_LEN
#	define GAMEWORLD_NAME_LEN	32
#endif

// 邮件标题最大长度
#ifndef		MAIL_TITLE_MAXSIZE
#	define	MAIL_TITLE_MAXSIZE		24
#endif

// 邮件内容最大长度
#ifndef		MAIL_CONTENT_MAXSIZE
#	define	MAIL_CONTENT_MAXSIZE		384
#endif

// NPC中HTML请求的最大长度
#ifndef		NPC_HTMLREQ_MAXSIZE
#	define	NPC_HTMLREQ_MAXSIZE			512
#endif

// 禁用英雄最大个数
#ifndef		BAN_HERO_MAXSIZE
#	define	BAN_HERO_MAXSIZE	5
#endif

// 捕获交互任务最大数量
#ifndef		CONTACT_TASK_MAXSIZE
#	define	CONTACT_TASK_MAXSIZE	10
#endif

// 存储发布交互任务最大数量
#ifndef		PUBLISH_CONTACT_MAXSIZE
#	define	PUBLISH_CONTACT_MAXSIZE	20
#endif

// 排位赛季奖励个数
#ifndef MATCH_SEASON_PRIZE_COUNT
#	define MATCH_SEASON_PRIZE_COUNT 2
#endif

// tolua_end
#endif/// __COMMON_NAMELENDEF_H__