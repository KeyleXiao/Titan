/*******************************************************************
** 文件名:	IGameDatabaseDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	03/13/2008
** 版  本:	1.0
** 描  述:	定义访问游戏数据库时需要用到的各种数据结构

**************************** 修改记录 ******************************
********************************************************************/

#ifndef __INETERFACE_GAME_DATABASE_DEFINE_H__
#define __INETERFACE_GAME_DATABASE_DEFINE_H__


#pragma pack(1)
#pragma warning(push) // warning C4200: 使用了非标准扩展 : 结构/联合中的零大小数组
#pragma warning(disable : 4200)

namespace rkt
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*********************************** 常量定义 **********************************************************/
#	define  DBDEF_ACTOR_MAX_NUM             3       // 最大显示角色数量
#   define  DBDEF_MAX_GAMENAME_MAXSIZE      32		// 游戏区名称长度
#   define  DBDEF_ACTORNAME_MAXSIZE         32      // 角色名称字节长度
#	define  DBDEF_USERNAME_MAXSIZE          32      // 账号名称字节长度
#	define  DBDEF_PASSWORD_MAXSIZE          42      // 密码字节长度
#   define  DBDEF_CLIENT_IP_MAXSIZE         24      // IP字节长度
#	define  DBDEF_MAC_ADDRESS_MAXSIZE       24      // MAC长度
#   define  DBDEF_IDCARD_MAXSIZE	        32      // 身份证号码长度
#   define  DBDEF_GESTURE_BUFF_MAXSIZE	    32      // 手势码长度
#   define  DBDEF_EQUIP_MAX_INDEX           7       // 装备最大槽序号
#   define  DBDEF_TALENTNAME_MAXSIZE        32      // 天赋名称长度
#   define  DBDEF_SUMMONER_SKILL_MAXCOUNT   8       // 召唤师技能最大数量
#   define  DBDEF_CAMPUSE_HEROID_MAXCOUNT   10      // 上次本阵营使用英雄id数最大数量
#   define  DBDEF_GLORY_MAXCOUNT            27      // 个人荣耀最大数量  枚举ENACHIEVE  4个字节记录一个荣耀
#   define  DBDEF_GLORY_BUFFSIZE            DBDEF_GLORY_MAXCOUNT*(4 + 8)      // 个人荣耀(int)和战场ID(long long)的BUFF
#   define  DBDEF_EXCLUSIVE_MAXCOUNT        12      // 专属天赋id最大数量
#   define  DBDEF_IS_ACTIVATED_SIZE         1       // 是否激活占一个字节
#   define  DBDEF_EXCLUSIVE_BUFFSIZE        DBDEF_EXCLUSIVE_MAXCOUNT*(4 + DBDEF_IS_ACTIVATED_SIZE)  // 专属天赋长度
#   define  DBDEF_RANK_BUFF_SIZE            9
#   define  DBDEF_BAN_HERO_SIZE             5		// 每个阵营禁用英雄最大个数
#   define  DBDEF_BAN_HERO_TOTAL_SIZE       10		// 一局比赛最多禁用英雄最大个数
#   define  DBDEF_WAR_ACTIVE_TALENT_SIZE    6		// 战场可激活天赋个数
#	define	DBDEF_SEASON_HERO_RECORD_COUNT	15		// 赛季历史记录英雄数据数组长度（英雄ID，使用场次，胜利场次）
/*************************** 家族相关 **************************************/
#   define  DBDEF_MAX_KINSKILL_LEN		    16
#	define	DBDEF_KIN_ELDER_LEN			    4		// 家族长老的个数
#	define	DBDEF_KIN_SLOGAN_MAXSIZE		256		// 家族宣言的长度
#	define	DBDEF_KIN_NOTICE_MAXSIZE		256		// 家族公告的长度
#	define	DBDEF_KIN_BOSSDATA_MAXSIZE		16		// 家族boss数据长度
#	define	DBDEF_KIN_LABEL_MAXSIZE			32		// 家族标签的长度
#   define  DBDEF_KIN_LEGENDGLORY_MAXCOUNT	11		// 家族荣耀最大数量  
#   define  DBDEF_KIN_LEGENDGLORY_BUFFSIZE	11*4    // 家族荣耀BUFF  枚举ELegendAchieve  4个字节记录一个荣耀

#	define	DBDEF_KINNAME_MAXSIZE		    32		// 家族名字长度
	/*************************** 帮会相关 **************************************/
#   define  DBDEF_CLANNAME_MAXSIZE			32      // 帮会名称字节长度
#	define	DBDEF_CLAN_MANIFESTO_LEN		40		// 帮会宣言的长度
#	define	DBDEF_CLAN_NOTICE_LEN			256		// 帮会公告的长度
	/**************************  公共任务数据 ********************/
#   define  DBDEF_MAX_DATASET_BYTEDATA_LEN	1536		// 512个BYTE型
#   define  DBDEF_MAX_DATASET_SHORTDATA_LEN	512			// 128个short型
#   define  DBDEF_MAX_DATASET_INTDATA_LEN	384			// 64个int型
#	define	DBDEF_CONTACTTASK_CONTENT_LEN	256			// 交互任务回复内容长度
	/**************************  OSS ********************/
#   define  DBDEF_OSS_VALUE_DESC_LEN	    32		    // 监控玩家数值
#	define  DBDEF_OSS_NOTE_DESC_LEN			512         // 文字描述长度
#	define  DBDEF_OSS_GOODNAME_MAXSIZE		32	        // 监控 点券的详细消耗日志
#   define  DBDEF_OSS_ACTOR_GOODS_DESC_LEN  16          // 玩家物品日记描述长度
#   define  DBDEF_OSS_ACTOR_FORGE_DESC_LEN  64          // 玩家打造描述长度
#	define	DBDEF_OSS_MONITOR_TASK_TIME_LEN	13			// 监控 任务数据时间长度
	/**************************  皇冠 ********************/
#   define  DBDEF_WAR_ACTOR_AK_TK_TC_GROUP_LEN_MAX   50 // 玩家单个战绩储存的连杀多杀团控数组长度 
#   define  DBDEF_CROWN_PAGE_INLAYSlOT_MAX	5			// 皇冠页宝石镶嵌个数
#   define  DBDEF_PRODUCEGEMSTONECOUNTMAX	9			// 宝石宝箱产出宝石个数
	/**************************  杯赛 ********************/
#   define  DBDEF_LEGENDCUP_NAME_LEN	        48		    // 杯赛名称长度
#   define  DBDEF_LEGENDCUP_BALCKLIST_LEN	    64		    // 杯赛黑名单
#   define  DBDEF_LEGENDCUP_KINSCOUNT_LEN	    64		    // 杯赛最大参赛队
#   define  DBDEF_LEGENDCUP_KINS_MEMBER_LEN	    10		// 战队最大参赛人数
#   define  DBDEF_LEGENDCUP_COMPETITION_WARID_COUNT	    5	// 每个比赛节点最多打多少场
	/**************************  杯赛 ********************/
#   define  DBDEF_MAIL_TITLE_LEN	        24		    // 邮件标题长度
#   define  DBDEF_MAIL_CONTENT_LEN			384		    // 邮件内容长度
#   define  DBDEF_MAIL_GOODS_NUM_MAX	    10		    // 邮件附件最大个数
	/**************************  装备方案功能 ********************/
#	define DBDEF_EQUIPSCHEME_EQUIPCOUNT_MAXSIZE	12		// 装备方案最大装备数

	/**
	@name : 定义访问游戏数据库时需要用到的存储过程及存储过程参数结构
	@brief:
	*/

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*********************************** 角色数据 **********************************************************/
	enum
	{
		GAMEDB_REQUEST_USERLOGIN = 1,						// 通行证登陆请求
		GAMEDB_REQUEST_QUERYUSERLOGIN,						// 跨公共区登录查询通行证信息
		GAMEDB_REQUEST_CENTERDBCREATEACTOR,		            // 创建角色 (中心库) 
		GAMEDB_REQUEST_CREATEACTOR,							// 创建角色 (游戏库)
		GAMEDB_REQUEST_CENTERACTORFLAG,					    // 更新创建角色标志
		GAMEDB_REQUEST_CENTERACTORCHK,						// 查询创建角色失败标志
		GAMEDB_REQUEST_CENTERDBDELETEACTOR,		            // 删除角色(中心库) 
		GAMEDB_REQUEST_DELETEACTOR,							// 删除角色(游戏库)
		GAMEDB_REQUEST_CHECKUSERLOCK,						// 查询角色列表并检查用户锁状态
		GAMEDB_REQUEST_SELECTACTOR,							// 选择角色进入游戏10
		GAMEDB_REQUEST_QUERYACTORLOGIN,					    // 跨公共区登录查询角色信息
		GAMEDB_REQUEST_SAVEACTOR,							// 更新角色信息
		GAMEDB_REQUEST_RANDACTORNAME,						// 从中心数据库, 获取创建角色时
		GAMEDB_REQUEST_RANDACTORNAMERECYCLE,				// 回收随机角色名 

        GAMEDB_REQUEST_QUERY_HISTORY_RECORD,                // 查询玩家历史战绩，用于本命英雄判断依据
		GAMEDB_REQUEST_FAKE_ACTORLOGIN_LIST,                // 请求假人列表

		GAMEDB_REQUEST_SAVE_VALUE_GOODS,					// 请求存储价值物品信息
		GAMEDB_REQUEST_READ_VALUE_GOODS,					// 请求读取价值物品信息

        GAMEDB_REQUEST_UPDATEACTORINFO,                     // 更新中心数据库, 角色信息
        GAMEDB_RQEUEST_QUERYRECOMWARRECORD,                 // 查询推广下家战绩信息
	};
    /***********************************  英雄相关 **********************************************************/
    enum {
        GAMEDB_REQUEST_QUERYALLHERO = 31,                   // 查询个人所有英雄信息
		GAMEDB_REQUEST_UPDATEHERO,                          // 更新个人英雄信息
		GAMEDB_REQUEST_DELETEHERO,                          // 删除个人英雄信息
		
		GAMEDB_REQUEST_SAVE_HERO_CARD,                      // 写入英雄卡牌信息
		GAMEDB_REQUEST_GET_HERO_CARD,                       // 获取英雄卡牌信息

		GAMEDB_REQUEST_WRITE_LIFEHERO,                      // 写入本命英雄信息
		GAMEDB_REQUEST_READ_LIFEHERO,                       // 读取本命英雄信息
		GAMEDB_REQUEST_DEL_LIFEHERO,                        // 删除本命英雄信息
    };
	/***********************************  加锁/解锁 **********************************************************/
	enum {
		GAMEDB_REQUEST_ACTORFINISH = 41,					// 角色数据存盘完成，通知前端解锁  ok 
		GAMEDB_REQUEST_SETUSERLOCK,							// 设置帐号锁  ok,  
		GAMEDB_REQUEST_AUTOUNLOCKZONESVR,					// 前端自动解锁某场景服内的角色
		GAMEDB_REQUEST_FORCEUNLOCK,							// 强制除提交的帐号ID外，本游戏世界里的所有角色全部解了
		GAMEDB_REQUEST_QUERYLOCKFLAG,						// 查询某个帐号的锁标志
	};

	/************************************************************************/
	/* 后台记录玩家基本数据                                                 */
	/************************************************************************/
	enum
	{
		GAMEDB_REQUEST_WAR_SAVE_GAME = 50,                      // 战场比赛记录
		GAMEDB_REQUEST_WAR_SAVE_BUYEQUIP,                       // 购买装备记录
		GAMEDB_REQUEST_WAR_SAVE_MONSTERINFO,                    // 战场怪物记录
		GAMEDB_REQUEST_WAR_GET_ACTORGAME,						// 获取比赛记录
		GAMEDB_REQUEST_WAR_GET_WARGAME,							// 获取指定比赛记录
		GAMEDB_REQUEST_WAR_SAVE_BAN_HERO,						// 保存被Ban英雄列表（后台统计输出使用） 
	};
	/************************************************************************/
	/* 后台记录撮合类型分值                                                 */
	/************************************************************************/
	enum
	{
		GAMEDB_REQUEST_GET_MATCHTYPE_ALL_RANK = 70,            // 获取Rank分值
		GAMEDB_REQUEST_SET_MATCHTYPE_RANK,                     // 设置更新Rank分值段位

		GAMEDB_REQUEST_GET_MATCH_ALL_HEROSCORE,                // 获取英雄熟练度得分
		GAMEDB_REQUEST_SET_MATCH_HEROSCORE,                    // 设置英雄熟练度得分
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*****************************************   排行榜  **************************************************/
	enum
	{
		GAMEDB_REQUEST_READ_MATCHTYPE_ORDER_RANK_INFO = 90,			// 读取排行榜排位数据 （批量读出）  ok 	

		//公共接口查询
		GAMEDB_REQUEST_READ_HERO_ORDER_CLASS,                    // 英雄榜大类 1、英雄养成榜

		// 英雄类型排行榜相关
		GAMEDB_UPDATE_HEROTYPE_ORDER_CLASS,                     // 更新英雄类型榜
		GAMEDB_REQUEST_READ_HEROTYPE_ORDER_CLASS,               // 查询英雄类型榜

		GAMEDB_REQUEST_READ_ORDERINFO_SETTING,			        // 设置用户是否在排行榜中显示
		GAMEDB_REQUEST_READ_ORDERINFO_SELECTSETTING,	        // 查询用户是否设置在排行榜中显示

		// 角色键值排行榜
		GAMEDB_REQUEST_READ_ORDERINFO_PERSON,					// 读取排行榜个人信息

        // 王者排位排行榜
        GAMEDB_REQUEST_READ_KING_RANK_ORDERINFO,				// 读取王者排位信息
	};
	/*****************************************   天赋页加点数  **************************************************/
	enum
	{
		GAMEDB_REQUEST_GET_TALENTPAGE_POINTS_ALL = 110,               // 获得英雄的所有天赋页的加点数
		GAMEDB_REQUEST_SET_TALENTPAGE_POINTS,			             // 设置英雄的某个天赋页的加点数  
		GAMEDB_REQUEST_DELETE_TALENTPAGE_POINTS,                     // 删除天赋页
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*****************************************   符文  **************************************************/
	enum
	{
		GAMEDB_REQUEST_QUERY_RUNE_INFO = 120,                      // 获取角色所有的符文数据
		GAMEDB_REQUEST_SAVE_RUNE_INFO,                             // 请求更新符文数据
		GAMEDB_REQUEST_DELETE_RUNE_INFO,                           // 删除角色的符文页数据
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/***********************************     家 族    ******************************************************/
	enum
	{
		GAMEDB_REQUEST_CENTERDBCREATEKIN = 130,			// 中心数据库 创建家族
		GAMEDB_REQUEST_CREATEKIN,					        // 创建家族  
		GAMEDB_REQUEST_UPDATEKIN,							// 更新家族基本信息  
		GAMEDB_REQUEST_CENTERDBDISMISSKIN,					// 中心数据库 解散家族  
		GAMEDB_REQUEST_DISMISSKIN,							// 解散家族  
		GAMEDB_REQUEST_READKINBASE,							// 读取家族详细资料 
		GAMEDB_REQUEST_READKINLIST,							// 读取家族列表

		GAMEDB_REQUEST_UPDATE_KINMEMBER,					// 更新成员（没有则添加）
		GAMEDB_REQUEST_DELETE_KINMEMBER,					// 删除成员  
		GAMEDB_REQUEST_READ_KIN_MEMBERLIST,					// 读取家族成员列表

		GAMEDB_REQUEST_CENTERKINFLAG,						// 游戏世界创建家族后，返回中心数据库置成败标志
		GAMEDB_REQUEST_CENTERKINCHK,						// 中心数据库 家族创建成败标志检查

		GAMEDB_REQUEST_CENTERKINNAMECHK,					// 校验和修改中心数据库家族名
		GAMEDB_REQUEST_CENTERKINNAMEFLAG,					// 修改中心数据库家族名后，返回中心数据库置成败标志

		GAMEDB_REQUEST_READ_KIN_LEGENDAWARD,				// 读取家族杯赛奖励列表     
		GAMEDB_REQUEST_ADD_KINLEGENDAWARD,					// 添加杯赛奖励
		GAMEDB_REQUEST_READ_KIN_LEGENDWAR,					// 读取家族战绩列表(=kin1ID,=kin2ID都返回)
		GAMEDB_REQUEST_ADD_KINLEGENDWAR,					// 添加杯赛战绩     
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/***********************************     帮 会   ******************************************************/
	enum
	{
		GAMEDB_REQUEST_CENTERDBCREATECLAN = 200,		    // 中心数据库创建帮会 通过后 再在游戏库创建
		GAMEDB_REQUEST_CREATECLAN,							// 游戏世界库创建帮会 
		GAMEDB_REQUEST_UPDATECLAN,							// 更新帮会基本信息 
		GAMEDB_REQUEST_CENTERDBDISMISSCLAN,					// 解散帮会 
		GAMEDB_REQUEST_DISMISSCLAN,							// 解散帮会.
		GAMEDB_REQUEST_READCLANLIST,						// 读取帮会列表 

		GAMEDB_REQUEST_UPDATEMEMBER,						// 更新成员（没有则添加） 
		GAMEDB_REQUEST_DELETEMEMBER,						// 删除成员  
		GAMEDB_REQUEST_READMEMBERLIST,						// 读取帮会成员列表 

		GAMEDB_REQUEST_CENTERCLANFLAG,						// 游戏库创建帮会 后 返回中心数据库 置成败标志 
		GAMEDB_REQUEST_CENTERCLANCHK,						// 中心数据库 帮会创建成败标志检查 

		GAMEDB_REQUEST_ADDCLANDONATEINFO,					// 增加帮会捐赠信息
		GAMEDB_REQUEST_REMOVECLANDONATEINFO,				// 移除帮会捐赠信息
		GAMEDB_REQUEST_GETCLANDONATEINFO,					// 获取帮会捐赠信息
	};
	/***********************************     任务   ******************************************************/
	enum
	{	// 公共任务数据
		GAMEDB_REQUEST_DATASET_READ = 250,					// 读取数据集合      
		GAMEDB_REQUEST_DATASET_SAVE,						// 存储数据集合      
		GAMEDB_REQUEST_DATASET_DEL,							// 删除一条数据集合  
		//个人任务数据
		GAMEDB_REQUEST_READTASKDATA,						// 读取任务数据
		GAMEDB_REQUEST_SAVETASKDATA,						// 存储任务数据
		// 交互任务数据
		GAMEDB_REQUEST_READ_CONTACTTASK,					// 读取交互任务
		GAMEDB_REQUEST_ADD_CONTACTTASK,						// 添加交互任务
		GAMEDB_REQUEST_UPDATE_CONTACTTASK,					// 更新交互任务
		GAMEDB_REQUEST_READ_CONTACTTASK_REPLY,				// 读取交互任务回复
		GAMEDB_REQUEST_SAVE_CONTACTTASK_REPLY,				// 存储交互任务回复
		GAMEDB_REQUEST_DEL_CONTACTTASK,						// 删除交互任务+回复
		GAMEDB_REQUEST_READ_ACTOR_CONTACTTASK,				// 读取玩家交互任务
	};

	/*********************************** OSS相关 ******************************************************/
	enum
	{
		GAMEDB_REQUEST_MONITOR_ACTORVALUES = 301,           // 监控 玩家数值
		GAMEDB_REQUEST_MONITOR_ACTORTICKET,			        // 监控 点券的详细消耗日志
		GAMEDB_REQUEST_LOG_NOTE,					        // 文字型日志
		GAMEDB_REQUEST_SET_MATCHTYPE_RANK_DETAIL,           // 添加rank相关记录详细信息
		GAMEDB_REQUEST_ACTORLOGIN_LOG,      				// 角色登录日志
		GAMEDB_REQUEST_ONLINE_LOG,					        // 角色上下线日志
		GAMEDB_REQUEST_ONLINENUM_LOG,                       // 每分钟用户在线人数统计
		GAMEDB_REQUEST_ACTOR_GOODS_LOG,                     // 玩家物品日记
		GAMEDB_REQUEST_ACTOR_UPGRADE_LEVEL_LOG,             // 玩家升级日记
		GAMEDB_REQUEST_ACTOR_GOOD_FROGE_LOG,				// 玩家物品打造日记
		GAMEDB_REQUEST_ACTOR_GUIDESTEP_LOG,				    // 玩家引导步骤日记
		GAMEDB_REQUEST_MONITOR_TASK,						// 监控 任务数据
	};
	/***********************************  点劵 ****************************************************/
	enum
	{
		GAMEDB_REQUEST_GAMETICKET_ADDMINE = 350,			// 游戏世界库用户点券操作
		GAMEDB_REQUEST_PAYUSERTICKET_GET,   	    	    // 充值中心库读取待提取账号列表
		GAMEDB_REQUEST_PAYTICKET_REDUCE,				    // 充值中心库请求领取充值点券
		GAMEDB_REQUEST_GAMETICKET_CHARGE,				    // 游戏世界库增加领取点券
		GAMEDB_REQUEST_PAYTICKETCHARGE_CONFIRM,		        // 充值中心库领取点券确认
	};
	/*********************************** 英雄附加信息 ****************************************************/
	enum
	{
		GAMEDB_REQUEST_SAVE_HEROADDITIONAL = 360,			// 保存英雄附加信息
		GAMEDB_REQUEST_GET_HEROADDITIONAL,                  // 查询英雄附加信息
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	/***************************************   防沉迷 /激活卡 **********************************************/
	enum
	{
		GAMEDB_REQUEST_READ_ADDICTINFO = 370,			// 读取玩家的防沉迷信息  
		GAMEDB_REQUEST_WRITE_ADDICTINFO,				    // 写入玩家的防沉迷信息（批量写入） 
		GAMEDB_REQUEST_WRITE_AUTHENT,                       // 增加玩家实名认证
	};
	/*********************************** 喇叭系统 **********************************************************/
	enum
	{
		GAMEDB_REQUEST_CHATSPEAKER_READ = 380,				// 查询喇叭
		GAMEDB_REQUEST_CHATSPEAKER_ADD,						// 添加喇叭
		GAMEDB_REQUEST_CHATSPEAKER_EDIT,					// 更新喇叭
	};
	/*********************************** 增值服务 ****************************************************/
	enum
	{
		GAMEDB_REQUEST_ADDSERVICES_USER_CHK = 400,			// 检查用户信息是否符合数据库规则
		GAMEDB_REQUEST_UPDATECENTERDBCREATEACTOR,		    // 更新创建角色 (中心库) 
	};

	/*********************************** 增益卡 **********************************************************/
	enum
	{
		GAMEDB_REQUEST_BENEFITCARD_READ = 420,				// 查询增益卡
		GAMEDB_REQUEST_BENEFITCARD_ADD,						// 添加增益卡
		GAMEDB_REQUEST_BENEFITCARD_UPDATE,					// 更新增益卡信息
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*****************************************   皇冠  **************************************************/
	enum
	{
		GAMEDB_REQUEST_QUERY_CROWN_INFO = 440,                      // 请求所有皇冠页数据                       	
		GAMEDB_REQUEST_UPDATE_CROWN_INFO,							// 更新皇冠页数据
		GAMEDB_REQUEST_DELETE_CROWN_INFO,                           // 删除皇冠页数据

		GAMEDB_REQUEST_QUERY_CROWN_GMESTONEITEM,					// 请求所有宝石数据         
		GAMEDB_REQUEST_UPDATE_CROWN_GMESTONEITEM,					// 更新宝石
		GAMEDB_REQUEST_DELETE_CROWN_GMESTONEITEM,					// 删除宝石

		GAMEDB_REQUEST_QUERY_GEMSTONE_PRODUCE_DISTRIBUTION,			// 请求宝石产出分布情况
		GAMEDB_REQUEST_UPDATE_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION,  // 更新宝石产出分布
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*********************************** 杯赛 **********************************************************/
	enum
	{
		GAMEDB_REQUEST_CREATE_LEGEND_CUP = 470,		            // 创建杯赛
		GAMEDB_REQUEST_READ_ALL_LEGEND_CUP,                     // 查询正在比赛的杯赛信息
		GAMEDB_REQUEST_UPDATE_LEGEND_CUP_BASEINFO,              // 更新比赛基础信息

		GAMEDB_REQUEST_UPDATE_CUP_KININFO,                      // 更新比赛单个战队记录信息
		GAMEDB_REQUEST_READ_ALL_KININFO,                        // 查询比赛的所有战队信息
		GAMEDB_REQUEST_SAVE_ALL_KININFO,                        // 记录所有战队的信息

		GAMEDB_REQUEST_UPDATE_CUP_MEMBERLIST,                   // 记录杯赛参赛玩家信息
		GAMEDB_REQUEST_LEGEND_CUP_MEMBERLIST,                   // 请求杯赛参赛玩家信息

		GAMEDB_REQUEST_EX_LIST_DATA_INFO,                   	// 读取杯赛列表数据信息
		GAMEDB_REQUEST_UPDATE_RIGESTERLIST,                     // 更新杯赛申请列表
		GAMEDB_REQUEST_UPDATE_BLACKLIST,                        // 更新杯赛黑名单列表
		GAMEDB_REQUEST_UPDATE_RANKLIST,                         // 更新杯赛排名列表

		GAMEDB_REQUEST_SAVE_CUP_COMPETITION_NODE,               // 生成比赛记录信息
		GAMEDB_REQUEST_UPDATE_CUP_COMPETITION_NODE,             // 更新单个比赛记录信息
		GAMEDB_REQUEST_READ_ALL_CUP_COMPETITION_NODE,           // 读取所有比赛记录信息

		GAMEDB_REQUEST_READ_ROUND_GROUP_KIN_POS,                // 获得比赛每轮分组战队位置列表
		GAMEDB_REQUEST_SAVE_ROUND_GROUP_KIN_POS,                // 更新一轮战队分组列表位置列表

		GAMEDB_REQUEST_SAVE_CUP_ROUND_TIMEINFO,                 // 保存比赛每轮的比赛时间相关
		GAMEDB_REQUEST_READ_CUP_ROUND_TIMEINFO,                 // 读取每轮比赛的时间相关
		GAMEDB_REQUEST_UPDATE_CUP_ROUND_TIMEINFO,               // 单独更新某轮比赛的时间

		GAMEDB_REQUEST_LEGEND_CUP_CUP_DELETE_KINMEMBER,         // 删除参赛玩家

		GAMEDB_REQUEST_READ_PLAYER_LEGENDCUP_DATA,              // 读取玩家杯赛数据信息
		GAMEDB_REQUEST_UPDATE_PLAYER_LEGENDCUP_DATA,            // 更新玩家杯赛数据信息

	};

	/***********************************     师徒    ******************************************************/
	enum
	{
		GAMEDB_REQUEST_READ_MENTORSHIP = 500,				// 读取师徒关系数据
		GAMEDB_REQUEST_ADD_MENTORSHIP,						// 添加师徒关系数据
		GAMEDB_REQUEST_UPDATE_MENTORSHIP,					// 更新师徒关系数据
		GAMEDB_REQUEST_DEL_MENTORSHIP,						// 删除师徒关系数据
		GAMEDB_REQUEST_READ_MENTORSHIPPERSON,				// 读取师徒个人数据
		GAMEDB_REQUEST_UPDATE_MENTORSHIPPERSON,				// 更新师徒个人数据(没有则添加)
		GAMEDB_REQUEST_ADD_MFWINAWARDPRECOUNT,				// 增加个人数据(师傅首胜预存次数)字段
		GAMEDB_REQUEST_ADD_PFWINAWARDPRECOUNT,				// 增加个人数据(学徒首胜预存次数)字段
	};

	/*********************************** 游戏区信息 ********************************************************/
	enum
	{
		GAMEDB_REQUEST_READ_GAMEAREAINFO = 520,			// 读取游戏世界信息，传入游戏世界的ID须为服务器管理系统中配置的ID
		// 服务器启动时调用此请求校验世界库配置是否正确，正确返回成功，否则返回失败
	};

	/***********************************     邮件    ******************************************************/
	enum
	{
		GAMEDB_REQUEST_READ_MAIL_LIST = 530,				// 读取玩家邮件列表
		GAMEDB_REQUEST_READ_MAIL,							// 读取邮件数据
		GAMEDB_REQUEST_DEL_MAIL,							// 删除邮件数据
		GAMEDB_REQUEST_ADD_MAIL,							// 添加邮件数据
	};

	/***********************************     标签功能    ******************************************************/
	enum
	{
		GAMEDB_REQUEST_READ_LABEL_LIST = 540,				// 读取玩家的所有标签
		GAMEDB_REQUEST_UPDATE_LABEL,						// 增加玩家的某个标签的个数,没有则添加
	};

	/***********************************     点赞功能    ******************************************************/
	enum
	{
		GAMEDB_REQUEST_ADD_SUPOORT = 550,					// 增加玩家的点赞
	};

	/***********************************     装备方案功能    ******************************************************/
	enum
	{
		GAMEDB_REQUEST_READ_EQUIPSCHEME_LIST = 560,				// 读取装备方案
		GAMEDB_REQUEST_SAVE_EQUIPSCHEME,					// 保存装备方案
		GAMEDB_REQUEST_DEL_EQUIPSCHEME,						// 删除装备方案
	};


    /***********************************     惩罚系统    ******************************************************/
    enum
    {
        GAMEDB_REQUEST_PLAYER_PUNISHMENT = 570,			    // 读取惩罚
        GAMEDB_REQUEST_UPDATE_PLAYER_PUNISHMENT_TIME,		// 添加更新惩罚时间
        GAMEDB_REQUEST_DEL_PLAYER_PUNISHMENT_TIME,			// 移除惩罚时间时间
    };

	/***********************************     英雄皮肤使用次数    ******************************************************/
	enum
	{
		GAMEDB_REQUEST_ADD_HEROSKIN_USECOUNT = 580,				// 新增英雄皮肤使用次数
	};

	/***********************************     好友系统    ******************************************************/
	enum
	{
		GAMEDB_REQUEST_SNS_READ_PLAYERINFO_BY_NAME = 590,
		GAMEDB_REQUEST_SNS_READ_PLAYERINFO_BY_USERID
	};

	/***********************************     赛季管理    ******************************************************/
	enum
	{
		GAMEDB_REQUEST_SET_RANK_SEASON_DETAIL = 600,		// 存当前赛季详细信息
		GAMEDB_REQUEST_GET_RANK_SEASON_DETAIL,				// 取当前赛季详细信息
		GAMEDB_REQUEST_ADD_RANK_SEASON_RECORD,				// 添加赛季历史信息
		GAMEDB_REQUEST_GET_RANK_SEASON_RECORD,				// 取赛季历史信息
	};

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    /***************************************   通行证标识    **********************************************/
    enum PassportFlags {
        RealNameAuthent = 0x1,                              // 完成实名认证，填写完成身份证和真实姓名
        RegisterEmail = 0x2,								// 填写了安全邮箱，未通过邮箱认
        EmailAuthent = 0x4,									// 通过邮箱认证，安全邮箱设置完成
        SetQA = 0x8,										// 设置了提示问答
        SetSafeCode = 0x10,									// 设置了安全码
        SetIDCard = 0x20,									// 设置了通行证归属身份证
        KillUser = 0x40,									// 永久封号 2010.04.06
        RecomUser = 0x80,									// 被推荐用户
        BindMatrixCard = 0x100,								// 绑定矩阵卡
        LostMaxtrixCard = 0x200,							// 丢失矩阵卡
        DisabledAccount = 0x400,							// 禁用通行证账号，不能进行登录。
        BindPhone = 0x1000,									// 手机密保绑定电话
        LostPhone = 0x2000,									// 手机密保挂失电话
    };

    /****************************************服务标识************************************************/
    enum ServiceFlags{
        RecommendUser 			= 0x1,                      // 被推荐用户
        Recommend		        = 0x2,                      // 推广用户
    };
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    /***************************************   读取游戏区信息   *****************************************/

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_READ_GAMEAREAINFO 读取游戏区信息: 传入的参数结构
    struct DBREQ_PARAM_READ_GAMEAREAINFO
    {
        DWORD dwWorldID;			// 世界ID

        DBREQ_PARAM_READ_GAMEAREAINFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 返回读取游戏区信息结果
    struct DBREQ_RESULT_READ_GAMEAREAINFO
    {
        DWORD dwLaunchTime;		// 开区时间time(NULL)

        DBREQ_RESULT_READ_GAMEAREAINFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };	

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_USERLOGIN 
    /// 登陆请求: 传入的参数结构
    struct DBREQ_PARAM_USERLOGIN
    {
        char     szUserName[DBDEF_USERNAME_MAXSIZE];		// 用户名
        char     szPassword[DBDEF_PASSWORD_MAXSIZE];		// 用户密码
        int	     dwGameWorldID;			    		        // 游戏世界ID
        char     szClientIP[DBDEF_CLIENT_IP_MAXSIZE];       // 客户端IP
        char     szMacAddress[DBDEF_MAC_ADDRESS_MAXSIZE];   // 客户端网卡地址
        int      nPartnerID;                                // 游戏合作商ID
        DWORD	 dwDiskSN;									// 硬盘序列号

        DBREQ_PARAM_USERLOGIN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 登陆结果: 登陆返回的结果数据
    struct DBREQ_RESULT_USERLOGIN
    {
        DWORD	 dwUDBID;									// 账号ID

        char     szUserName[DBDEF_USERNAME_MAXSIZE];		// 账号名，直通
        char     szPassword[DBDEF_PASSWORD_MAXSIZE];		// 账号密码，直通
        char     szClientIP[DBDEF_CLIENT_IP_MAXSIZE];       // 客户端IP，直通
        char     szMacAddress[DBDEF_MAC_ADDRESS_MAXSIZE];   // 客户端网卡地址，直通

        bool     bIsAdult;									// 是否成年,0:未成年，1;已成年
        DWORD	 dwPassportFlags;							// 通行证相关标识 2009.09.05
        DWORD	 dwServiceFlags;							// 服务标识: 0x1被推荐用户;0x2推荐用户;0x4 VIP
        int		 nTotalPay;									// 总充值金额
        DWORD	 dwIdCard;									// 身份证号码
        BYTE	 nGestureFlags;								// 手势密码标识 0--未启用提醒 1--启用 3--未启用不提醒
        char	 szGesture[DBDEF_GESTURE_BUFF_MAXSIZE];

        DWORD    dwUserUserBirthday;                        // 玩家出生日期 格式：20110622，其中0表示玩家没有实名认证，没有出生日期 
        char     szPhoneNum[16];                            // 手机号码
        char     szMobileNo[16];                            // 预约服务器手机号码
        char	 szKillRemark[512];							// 封号信息 返回值为2时 
        DWORD    dwRegTime;								    // 账号注册时间

        DBREQ_RESULT_USERLOGIN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    ///GAMEDB_REQUEST_QUERYUSERLOGIN 
    ///查询通行证登陆请求信息
    ///传入的参数结构
    struct DBREQ_PARAM_QUERYUSERLOGIN
    {
        DWORD    dwUserID;									// 用户ID
        DWORD	 dwPDBID;									// 角色ID
        char     szClientIP[DBDEF_CLIENT_IP_MAXSIZE];       // 客户端IP
        char     szMacAddress[DBDEF_MAC_ADDRESS_MAXSIZE];   // 客户端网卡地址

        DBREQ_PARAM_QUERYUSERLOGIN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 查询结果: 查询通行证登陆请求信息
    struct DBREQ_RESULT_QUERYUSERLOGIN
    {
        DWORD	dwUDBID;									// 用户ID
        DWORD	dwPDBID;									// 角色ID
        char	szUserName[DBDEF_USERNAME_MAXSIZE];	        // 用户名
        bool	bIsAdult;							        // 是否成年,0:未成年，1;已成年
        DWORD   dwUserUserBirthday;						    // 玩家出生日期 格式：20110622，其中0表示玩家没有实名认证，没有出生日期 
        DWORD	dwPassportFlags;							// 通行证相关标识
        DWORD	dwServiceFlags;								// 服务标识: 0x1被推荐用户;0x2推荐用户;0x4 VIP
        DWORD	dwIdCard;									// 身份证号码
        int		nTotalPay;							        // 总充值金额
        BYTE	nGestureFlags;								// 手势密码标识 0--未启用提醒 1--启用 3--未启用不提醒
        char	szGesture[DBDEF_GESTURE_BUFF_MAXSIZE];		// 手势密码
        char	szPhoneNum[16];								// 手机号码
        char	szMobileNo[16];								// 预约服务器手机号码
        char	szKillRemark[512];							// 封号信息 返回值为2时 
        DWORD   dwRegTime;									// 账号注册时间

        DBREQ_RESULT_QUERYUSERLOGIN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    //////////////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_CENTERDBCREATEACTOR 中心数据库创建角色 ： 传入结构体
    struct DBREQ_PARAM_CENTERDBCREATEACTOR
    {
        DWORD    dwEnterWorldID;							// 入口游戏世界ID
        DWORD    dwUserID;									// 账号ID
        char     szUserName[DBDEF_USERNAME_MAXSIZE];		// 账号名
        char     szActorName[DBDEF_ACTORNAME_MAXSIZE];		// 角色名
        bool  	 bSex;										// 角色性别
        char     szClientIP[DBDEF_CLIENT_IP_MAXSIZE];       // 客户端IP
        char     szMacAddress[DBDEF_MAC_ADDRESS_MAXSIZE];   // 客户端网卡地址
        int 	 nRecomUserID;                				// 推荐人用户ID

        DBREQ_PARAM_CENTERDBCREATEACTOR(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    // 中心数据库创建角色 ： 返回数据结构体
    // 数据库返回值为1 创建角色成功 执行 游戏数据库的角色创建
    // -1 不执行 给玩家相应提示
    struct DBREQ_RESULT_CENTERDBCREATEACTOR
    {
        DWORD	dwActorID;									// 角色ID
        DWORD   dwEnterWorldID;								// 入口游戏世界ID
        DWORD	dwUserID;									// 用户ID
        char	szUserName[DBDEF_USERNAME_MAXSIZE];			// 用户名
        char    szActorName[DBDEF_ACTORNAME_MAXSIZE];		// 角色名
        bool  	bSex;										// 角色性别

        DBREQ_RESULT_CENTERDBCREATEACTOR(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_CHECKUSERLOCK
    struct DBREQ_PARAM_CHECKUSERLOCK
    {
        DWORD   dwEnterWorldID;						        // 玩家选择进入游戏世界ID
        DWORD   dwUserID;									// 用户ID
        char    szUserName[DBDEF_USERNAME_MAXSIZE];		    // 用户名
    };


    /// 用户锁检查结果: 提取用户角色列表
    struct DBREQ_RESULT_CHECKUSERLOCK
    {
        // 角色登陆相关简洁数据
        struct ACTOR_INFO
        {
            DWORD dwWorldID;                                      // 当前角色所属的游戏世界ID
            DWORD dwActorID;                                      // 角色ID
            char  szActorName[DBDEF_ACTORNAME_MAXSIZE];           // 角色名
            WORD  nHeroID;                                        // 静态英雄ID
            WORD  nSkinID;                                        // 静态皮肤ID
            WORD  nLevel;                                         // 静态角色等级
            bool  bSex;                                           // 性别
            DWORD dwMapId;                                        // 地图ID
            int   nLocX;                                          // x坐标
            int   nLocY;                                          // y坐标
            int   nLocZ;                                          // z坐标            
            BYTE  byRankbuffdata[DBDEF_RANK_BUFF_SIZE];           // 排位数据相关        
            int   nPrivilege;                                     // GM权限
            ACTOR_INFO()
            {
                memset(this, 0, sizeof(*this));
            }
        };

        DWORD       dwUserID;								    // 用户ID
        BYTE	    nLockFlag;								    // 当前锁标志
        WORD        nPlayerCounts;                              // 当前账号下的角色数
        // DBREQ_RESULT_CHECKUSERLOCK::ACTOR_INFO players[];    // 一个账号下角色信息
    };
    typedef DBREQ_RESULT_CHECKUSERLOCK::ACTOR_INFO  ACTOR_BASEINFO;

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_CREATEACTOR 
    /// 世界库创建角色
    /// 传入参数结构
    struct DBREQ_PARAM_CREATEACTOR
    {
        DWORD	dwEnterWorldID;								// 入口游戏世界ID
        DWORD   dwUserID;									// 用户ID
        DWORD	dwActorID;									// 角色ID
        char    szUserName[DBDEF_USERNAME_MAXSIZE];		    // 用户名
        char    szActorName[DBDEF_ACTORNAME_MAXSIZE];	    // 角色名
        bool  	bSex;										// 角色性别
        WORD	nHeroID;									// 英雄ID
        WORD    nSkinID;                                    // 皮肤ID
        int		nInitMapID;								    // 初始化mapid
        DWORD	dwPassportFlags;							// 2009.09.22:通行证相关标识 
        DWORD	dwServiceFlags;								// 服务标识
        char	szClientIP[DBDEF_CLIENT_IP_MAXSIZE];		// 客户端IP
        char    szMacAddress[DBDEF_MAC_ADDRESS_MAXSIZE];    // 客户端网卡MAC地址

        DBREQ_PARAM_CREATEACTOR(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 创建角色: 返回结果和登陆返回的结果一样,都是角色信息列表
    typedef DBREQ_RESULT_CHECKUSERLOCK DBREQ_RESULT_CREATEACTOR;

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_CENTERDBDELETEACTOR 中心数据库 删除角色: 传入参数结构
    // 返回值为 1 表示成功 执行 游戏数据库删除角色
    struct DBREQ_PARAM_CENTERDBDELETEACTOR
    {
        DWORD    dwUserID;									    // 帐号ID
        DWORD    dwActorID;									    // 角色ID
        DWORD    dwLevel;										// 角色当前等级

        DBREQ_PARAM_CENTERDBDELETEACTOR(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    ///GAMEDB_REQUEST_CENTERACTORFLAG
    /// 中心数据库 置创建角色成败标志:传入参数结构
    struct DBREQ_PARAM_CENTERACTORFLAG
    {
        DWORD    dwActorID;									// 角色ID
        BYTE	 byFlag;									// 标志:1 游戏世界角色创建成功，0失败 
        DBREQ_PARAM_CENTERACTORFLAG(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 中心数据库 置创建角色成败标志 返回结构体：无，根据接口返回的标准结果1成功，-1失败判断
    ///////////////////////////////////////////////////////////////////
    ///GAMEDB_REQUEST_CENTERACTORCHK
    /// 中心数据库 角色创建成败标志检查:传入参数结构
    struct DBREQ_PARAM_CENTERACTORCHK
    {
        DWORD    dwWorldID;									// 世界ID
        DWORD    dwUserID;									// 用户ID
        DBREQ_PARAM_CENTERACTORCHK(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 中心数据库 角色创建成败标志检查:返回结构体
    struct DBREQ_RESULT_CENTERACTORCHK
    {
        DWORD	 dwCount;									// 未确认成功的角色ID数
        // dwCount * dwActorID;								// 未确认成功的角色ID列表
        DBREQ_RESULT_CENTERACTORCHK(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_DELETEACTOR 删除角色: 传入参数结构
    struct DBREQ_PARAM_DELETEACTOR
    {
        DWORD	 dwEnterWorldID;						    // 玩家选择进入游戏世界ID
        DWORD    dwUserID;									// 帐号ID	
        DWORD    dwActorID;									// 角色ID

        DBREQ_PARAM_DELETEACTOR(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 删除角色: 返回结果和登陆返回的结果一样,都是角色信息列表
    typedef DBREQ_RESULT_CHECKUSERLOCK DBREQ_RESULT_DELETEACTOR;

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_SELECTACTOR 选择角色: 传入参数结构
    struct DBREQ_PARAM_SELECTACTOR
    {
        DWORD	dwUserID;						    	    // 账号ID
        char    szUserName[DBDEF_USERNAME_MAXSIZE];			// 用户名
        DWORD   dwActorID;								    // 角色ID
        char	szClientIP[DBDEF_CLIENT_IP_MAXSIZE];		// 客户端IP 2009.08.24
        char    szMacAddress[DBDEF_MAC_ADDRESS_MAXSIZE];    // 客户端网卡MAC地址
        int     nPassportFlags;								// 标识
        DWORD	dwServiceFlags;								// 服务标识

        DBREQ_PARAM_SELECTACTOR(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 英雄相关数据
    struct DBREQ_PARAM_HEROINFO
    {
        int     nHeroID;                                        // 英雄ID
        WORD    nSkinID;                                        // 皮肤ID
        WORD    nLevel;                                         // 等级
        DWORD   dwDeadLine;                                     // 使用期限(秒)，0不限时
        DWORD   dwFlags;                                        // 英雄标识 HERO_FLAG

        DWORD   dwHeroExp;                                      // 英雄经验
        DWORD   dwWinNum;                                       // 比赛胜利场次
        DWORD   dwMatchNum;                                     // 比赛场次
        BYTE    nHeroStar;                                      // 英雄星级
        DWORD   dwUseTime;                                      // 最后一次使用时间
        BYTE    byExclusive[DBDEF_EXCLUSIVE_BUFFSIZE];          // 特权信息
        int     nHeroScore;                                     // 英雄成就得分
		int		nUseXPID;										// 使用的xp技能id
		DWORD	dwUnlockTime;									// 解锁时间
        DBREQ_PARAM_HEROINFO()
        {
            memset(this, 0, sizeof(*this));
        }
    };
    typedef DBREQ_PARAM_HEROINFO SHeroInfo;

    /// GAMEDB_REQUEST_SELECTACTOR 选择角色: 返回结果是角色详细信息    
    struct DBREQ_RESULT_SELECTACTOR
    {
        DWORD dwUserID;										  // 用户ID
        DWORD dwWorldID;                                      // 当前角色所属的游戏世界ID
        DWORD dwActorID;                                      // 角色ID
        char  szActorName[DBDEF_ACTORNAME_MAXSIZE];           // 角色名
        WORD  nHeroID;                                        // 静态英雄ID: 这里的nHeroID和SHeroInfo.nHeroID没有任何关系
        WORD  nSkinID;                                        // 静态皮肤ID   ****(策划提出可能和英雄皮肤不一样是单独的一个皮肤， 之后还可能是表情高低可以改变的情况 策划说不会这么搞)*****
        WORD  nLevel;                                         // 静态角色等级
        bool  bSex;                                           // 性别
        DWORD dwMapId;                                        // 地图ID
        int   nLocX;                                          // x坐标，由于坐标会有小数将扩10000倍
        int   nLocY;                                          // y坐标，由于坐标会有小数将扩10000倍
        int   nLocZ;                                          // z坐标，由于坐标会有小数将扩10000倍
        int   nAngle;                                         // 角度，由于会有小数将扩10000倍
        DWORD dwExp;                                          // 经验
        DWORD dwBindTicket;                                   // 金币
        DWORD dwTicket;                                       // 点券
        DWORD dwHeroPoolLimit;                                // 英雄池上限
        WORD  wHeadID;                                        // 头像Id
        DWORD dwClanID;                                       // 工会id
        DWORD dwKinID;                                        // 家族id
        DWORD dwTeamID;                                       // 战队id
        BYTE  bySummonerSkill[DBDEF_SUMMONER_SKILL_MAXCOUNT]; // 召唤师技能
        BYTE  byGlory[DBDEF_GLORY_BUFFSIZE];                  // 个人荣耀BUFF
        DWORD dwFightCapacity;                                // 玩家战力
        BYTE  byCampHeroID[DBDEF_CAMPUSE_HEROID_MAXCOUNT];    // 上一次队友使用英雄id
        DWORD dwPKWinNum;									  // PK胜利场次
        DWORD dwSupportNum;									  // 点赞次数
        DWORD dwPKTotalNum;									  // PK总场次
        int   nPrivilege;                                     // GM权限
        WORD  herocnt;                                        // 英雄数目
        // herocnt * DBREQ_PARAM_HEROINFO                     // 拥有的英雄 结果集

        DBREQ_RESULT_SELECTACTOR(void)
        {
            memset(this, 0, sizeof(DBREQ_RESULT_SELECTACTOR));
        };
    };
    /////////////////////////////////////////////////////////////////////////////////////////	
    //////GAMEDB_REQUEST_QUERYACTORLOGIN    
    // 跨公共区登录查询角色信息 : 传入的参数结构
    struct DBREQ_PARAM_QUERYACTORLOGIN
    {
        DWORD	dwPDBID;									// 角色ID
        BYTE	nCheckExistPDBID;							// 是否需要检查dwActorID是否存在(0:不需要 1:需要)
		bool	bFake;										// 互通字段 假人标示

        DBREQ_PARAM_QUERYACTORLOGIN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// GAMEDB_REQUEST_QUERYACTORLOGIN 查询结果: 跨公共区登录查询角色信息   
    struct DBREQ_RESULT_QUERYACTORLOGIN
    {
		bool	bFake;											 // 互通字段 假人标示
        DWORD	dwUserID;									 	 // 用户ID
		DWORD	dwWorldID;                                   	 // 当前角色所属的游戏世界ID
		DWORD	dwActorID;                                   	 // 角色ID
		char	szActorName[DBDEF_ACTORNAME_MAXSIZE];        	 // 角色名
		WORD	nHeroID;                                     	 // 静态英雄ID: 这里的nHeroID和SHeroInfo.nHeroID没有任何关系
		WORD	nSkinID;                                     	 // 静态皮肤ID   ****(策划提出可能和英雄皮肤不一样是单独的一个皮肤， 之后还可能是表情高低可以改变的情况 策划说不会这么搞)*****
		WORD    nLevel;                                      	 // 静态角色等级
		bool    bSex;                                        	 // 性别
		DWORD   dwMapId;                                     	 // 地图ID
		int     nLocX;                                       	 // x坐标，由于坐标会有小数将扩10000倍
		int     nLocY;                                       	 // y坐标，由于坐标会有小数将扩10000倍
		int     nLocZ;                                       	 // z坐标，由于坐标会有小数将扩10000倍

		DWORD   dwExp;                                          // 经验
		DWORD   dwBindTicket;                                   // 金币
		DWORD   dwTicket;                                       // 点券
		DWORD   dwHeroPoolLimit;                                // 英雄池上限
		WORD    wHeadID;                                        // 头像Id
		DWORD   dwClanID;                                       // 工会id
		DWORD   dwKinID;                                        // 家族id
		DWORD   dwTeamID;                                       // 战队id
		BYTE    bySummonerSkill[DBDEF_SUMMONER_SKILL_MAXCOUNT]; // 召唤师技能
		BYTE    byGlory[DBDEF_GLORY_BUFFSIZE];                  // 个人荣耀BUFF
		DWORD   dwFightCapacity;                                // 玩家战力
		BYTE    byCampHeroID[DBDEF_CAMPUSE_HEROID_MAXCOUNT];    // 上一次队友使用英雄id
		DWORD	dwPKWinNum;										// PK胜利场次
		DWORD	dwPKTotalNum;									// PK总场次
		BYTE    byRankbuffdata[DBDEF_RANK_BUFF_SIZE];           // 排位数据相关
		DWORD	dwSupportNum;									// 点赞数

        DBREQ_RESULT_QUERYACTORLOGIN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_SAVEACTOR 保存角色: 传入参数结构
    struct DBREQ_PARAM_SAVEACTOR
    {
		DWORD	dwUserID;                                       // 用户ID
		DWORD	dwWorldID;                                      // 当前角色所属的游戏世界ID
		DWORD	dwActorID;                                      // 角色ID
		char	szActorName[DBDEF_ACTORNAME_MAXSIZE];           // 角色名
		WORD	nHeroID;                                        // 静态英雄ID: 这里的nHeroID和SHeroInfo.nHeroID没有任何关系
		WORD	nSkinID;                                        // 静态皮肤ID   ****(策划提出可能和英雄皮肤不一样是单独的一个皮肤， 之后还可能是表情高低可以改变的情况 策划说不会这么搞)*****
		WORD    nLevel;                                         // 静态角色等级
		bool    bSex;                                           // 性别
		DWORD   dwMapId;                                        // 地图ID
		int     nLocX;                                          // x坐标，由于坐标会有小数将扩10000倍
		int     nLocY;                                          // y坐标，由于坐标会有小数将扩10000倍
		int     nLocZ;                                          // z坐标，由于坐标会有小数将扩10000倍
        int     nAngle;                                         // 角度，由于会有小数将扩10000倍
		DWORD   dwExp;                                          // 经验
		DWORD   dwHeroPoolLimit;                                // 英雄池上限
		WORD    wHeadID;                                        // 头像Id
		DWORD   dwClanID;                                       // 工会id
		DWORD   dwKinID;                                        // 家族id
		DWORD   dwTeamID;                                       // 战队id
		BYTE    bySummonerSkill[DBDEF_SUMMONER_SKILL_MAXCOUNT]; // 召唤师技能
		BYTE    byGlory[DBDEF_GLORY_BUFFSIZE];                  // 个人荣耀BUFF
		DWORD   dwFightCapacity;                                // 玩家战力
		BYTE    byCampHeroID[DBDEF_CAMPUSE_HEROID_MAXCOUNT];    // 上一次队友使用英雄id
		DWORD	dwPKWinNum;										// PK胜利场次
		DWORD	dwPKTotalNum;									// PK总场次
		BYTE    byRankbuffdata[DBDEF_RANK_BUFF_SIZE];           // 排位数据相关
        DBREQ_PARAM_SAVEACTOR(void)
        {
            memset(this, 0, sizeof(DBREQ_PARAM_SAVEACTOR));
        }
    };

    /// 保存角色: 返回结果,暂无,根据返回值既可判断是否成功
    struct DBREQ_RESULT_SAVEACTOR
    {

    };
    ///////////////////////////////////////////////////////////////////
    ///GAMEDB_REQUEST_RANDACTORNAME

    ///从中心数据库, 获取创建角色时，随机给定一个名字:传入参数结构
    struct DBREQ_PARAM_RANDACTORNAME
    {
        BYTE     nSex;										// 性别 0男 1女 2不区分
        int      nInCount;									// 要求查询记录数
        int      nWorldID;									// 所在游戏世界ID

        DBREQ_PARAM_RANDACTORNAME(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 从中心数据库, 返回创建角色时，随机给定的一个不重名的名字:返回结构体
    struct DBREQ_RESULT_RANDACTORNAME
    {	
        BYTE     nSex;													// 性别 0男 1女 2不区分
        int      nReCount;												// 返回记录数
        // nReCount * char   szRandActorName[MAX_PERSONNAME_LEN];		// 角色名

        DBREQ_RESULT_RANDACTORNAME(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///GAMEDB_REQUEST_RANDACTORNAMERECYCLE
    // @NOTE 回收随机角色名
    // 传入结构
    struct DBREQ_PARAM_RANDACTORNAMERECYCLE
    {
        int      nWorldID;									// 所在游戏世界ID

        DBREQ_PARAM_RANDACTORNAMERECYCLE(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 返回结构
    // 无
 
	// 查询玩家历史战绩列表，作为本命英雄的判断依据   GAMEDB_REQUEST_QUERY_HISTORY_RECORD
	struct DBREQ_PARAM_QUERY_HISTORY_RECORD
	{
		DWORD   dwActorID;                                      // 角色ID
		DBREQ_PARAM_QUERY_HISTORY_RECORD()
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 返回玩家历史战绩列表       GAMEDB_REQUEST_QUERY_HISTORY_RECORD
	struct DBREQ_RESULT_QUERY_HISTORY_RECORD
	{
		LONGLONG    nWarID;                                 // 战场ID
		WORD        wWarType;                               // 战场类型
		bool        bIsFull;                                // 是否满人
		WORD        wHeroID;                                // 英雄ID
		WORD        wDieCount;                              // 死亡次数
		WORD        wAssistCount;                           // 助攻次数
		WORD        wKillPersonNum;                         // 击杀人物数量

		// TODO 返回个数 nCount+DBREQ_RESULT_QUERY_HISTORY_RECORD*count
		// int nCount;                                       // 返回获取的比赛场次,最大值100
		// 返回 nCount * DBREQ_RESULT_QUERY_HISTORY_RECORD
		DBREQ_RESULT_QUERY_HISTORY_RECORD()
		{
			memset(this, 0, sizeof(*this));
		}
	}; 

    //////GAMEDB_REQUEST_FAKE_ACTORLOGIN_LIST   	
    // 查询假人数据返回的列表
    struct DBREQ_RESULT_FAKE_ACTORLOGIN_LIST
    {
        DWORD	dwCount;									// 返回假人ID列表 (策划假人角色模板的ID列表) 
															// 最多100个 如果模板不够取真人数据 
                                                            // 模板数据 80000 ~ 100000 之间 真人数据大于 100000 所以查询角色的时候添加互通标示
															// 后期可增加 段位 控制
        // dwCount * sizeof(DWORD)

        DBREQ_RESULT_FAKE_ACTORLOGIN_LIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/****************************************** 价值物品数量 ***********************************************/
	// GAMEDB_REQUEST_SAVE_VALUE_GOODS 请求存储价值物品信息	
	// 价值物品信息
	struct DBREQ_TABLE_VALUE_GOODS
	{
		DWORD	dwActorID;		// 玩家ID
		BYTE	byType;			// 物品类型
		DWORD	dwID;			// 物品ID
		DWORD	dwCount;		// 物品数量

		DBREQ_TABLE_VALUE_GOODS()
		{
			memset(this, 0, sizeof(DBREQ_TABLE_VALUE_GOODS));
		}
	};
	// 存价值物品
	struct DBREQ_PARAM_SAVE_VALUE_GOODS 
	{
		int		nCount;			// 物品个数
		// 不定长
		// nCount * DBREQ_TABLE_VALUE_GOODS

		DBREQ_PARAM_SAVE_VALUE_GOODS()
		{
			memset(this, 0, sizeof(DBREQ_PARAM_SAVE_VALUE_GOODS));
		}
	};
	// GAMEDB_REQUEST_READ_VALUE_GOODS 请求读取价值物品信息
	// 取价值物品
	struct DBREQ_PARAM_GET_VALUE_GOODS
	{
		DWORD	dwActorID;		// 玩家ID
		BYTE	byType;			// 物品类型

		DBREQ_PARAM_GET_VALUE_GOODS()
		{
			memset(this, 0, sizeof(DBREQ_PARAM_GET_VALUE_GOODS));
		}
	};
	struct DBREQ_RESULT_GET_VALUE_GOODS 
	{
		int		nCount;			// 物品个数
		// nCount * DBREQ_TABLE_VALUE_GOODS

		DBREQ_RESULT_GET_VALUE_GOODS()
		{
			memset(this, 0, sizeof(DBREQ_RESULT_GET_VALUE_GOODS));
		}
	};

    // GAMEDB_REQUEST_UPDATEACTORINFO
    // 更新中心数据库, 角色信息.
    // 战场战绩结构体
    struct DBREQ_WARRECORD_INFO
    {
        BYTE    nMatchType;   //撮合类型
		DWORD	dwMatchNum;
		DWORD	dwWinNum;
        DBREQ_WARRECORD_INFO()
        {
            memset(this, 0, sizeof(*this));
        }
    };
    // 输入结构
    struct DBREQ_PARAM_UPDATEACTORINFO
    {
        DWORD    dwPDBID;                    // 玩家角色DBID
        WORD     wLevel;                     // 最高等级
        int      nLastlogout;                // 玩家最近下线时间
        int      nRecordsCount;              // 比赛战绩记录数
        // nRecordsCount *   DBREQ_WARRECORD_INFO
        DBREQ_PARAM_UPDATEACTORINFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    // 返回结构
    // 无
    
    //GAMEDB_REQUEST_QUERYRECOMWARRECORD      
    // 查询推广下家战绩信息
    // 输入结构
    struct DBREQ_PARAM_QUERYRECOMWARRECORD   
    {
        DWORD    dwRecomUserID;                    // 上家账号ID
        DBREQ_PARAM_QUERYRECOMWARRECORD(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    // 返回结构
    struct DBREQ_RESULT_QUERYRECOMWARRECORD   
    {
        DWORD   dwRecomUserID;                      // 上家账号ID 直通
        DWORD	dwMatchNum;                         // 下家总场次
		DWORD	dwWinNum;                           // 下家总胜场次

        DBREQ_RESULT_QUERYRECOMWARRECORD(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
	////////////////////////////////////英雄相关///////////////////////////////////   
    /////////////英雄/////////////////////////////////////////////////////////////
    // GAMEDB_REQUEST_QUERYALLHERO,                        // 查询个人所有英雄信息
    struct DBREQ_PARAM_QUERY_ALL_HERO
    {
        DWORD   dwActorID;                                  // 角色ID
        bool    bFake;                                      // 互通字段 假人标识
        DBREQ_PARAM_QUERY_ALL_HERO()
        {
            memset(this, 0, sizeof(*this));
        }
    };
    /// 返回结果是英雄相关更新数据
    struct DBREQ_RESULT_QUERY_ALL_HERO
    {
        bool    bFake;                                      // 互通字段 假人标识
        DWORD   dwActorID;                                  // 角色ID
        int     nHeroCount;                                 // 英雄数量
        // nHeroCount * DBREQ_PARAM_HEROINFO
    };

    //////////////////////////////////////////////////////////////////////////
    // GAMEDB_REQUEST_UPDATEHERO,                           // 更新个人英雄信息
    // 英雄相关更新数据
    struct DBREQ_PARAM_UPDATE_HEROINFO
    {
        DWORD   dwActorID;                                  // 角色ID
        int     nHeroID;                                    // 英雄ID
        WORD    nSkinID;                                    // 皮肤ID
        WORD    nLevel;                                     // 成长等级
        DWORD   dwDeadLine;                                 // 使用期限(秒)，0不限时
        DWORD   dwFlags;                                    // 英雄标识

        DWORD   dwHeroExp;                                  // 英雄经验
        DWORD   dwWinNum;                                   // 比赛胜利场次
        DWORD   dwMatchCount;                               // 比赛场次
        BYTE    nHeroStar;                                  // 英雄星级
        DWORD   dwUseTime;                                  // 最后一次使用时间
        BYTE    byExclusive[DBDEF_EXCLUSIVE_BUFFSIZE];      // 特权信息
		int     nHeroScore;                                 // 英雄成就得分
		int		nUseXPID;									// 使用的xp技能id
		DWORD	dwUnlockTime;								// 解锁时间
        DBREQ_PARAM_UPDATE_HEROINFO()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // GAMEDB_REQUEST_DELETEHERO,                           // 删除个人英雄信息
    // 英雄相关删除数据
    struct DBREQ_PARAM_DELETE_HEROINFO
    {
        DWORD   dwActorID;                                  // 角色ID
        int     nHeroID;                                    // 英雄ID

        DBREQ_PARAM_DELETE_HEROINFO()
        {
            memset(this, 0, sizeof(*this));
        }
    };
    /// 返回结果是英雄相关删除数据
    struct DBREQ_RESULT_DELETE_HEROINFO
    {
        DWORD   dwActorID;                                  // 角色ID
        int     nHeroID;                                    // 英雄ID
        DBREQ_RESULT_DELETE_HEROINFO()
        {
            memset(this, 0, sizeof(*this));
        }
    };
    /////////////////////////////////英雄单独的卡牌信息/////////////////////////////////////////
    struct DBREQ_PARAM_HERO_CARD
    {
        DWORD   dwCardID;                   // 卡牌唯一ID
        int     nCardCount;                 // 英雄卡牌类型数量
        DBREQ_PARAM_HERO_CARD()
        {
            memset( this, 0, sizeof(*this) );
        }
    };

    // 保存角色英雄卡牌信息 GAMEDB_REQUEST_SAVE_HERO_CARD
    struct DBREQ_REQUEST_SAVE_HERO_CARD
    {
        DWORD   dwActorID;                  // 角色id
        int     nCount;                     // 英雄数量

        // nCount * DBREQ_PARAM_HERO_CARD
        DBREQ_REQUEST_SAVE_HERO_CARD()
        {
            memset( this, 0, sizeof(*this) );
        }
    };

    // 请求角色英雄卡牌信息  GAMEDB_REQUEST_GET_HERO_CARD
    struct DBREQ_REQUEST_GET_HERO_CARD
    {
        DWORD dwActorID;                 // 角色ID
        DBREQ_REQUEST_GET_HERO_CARD()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 角色英雄卡牌的返回信息GAMEDB_REQUEST_GET_HERO_CARD
    struct DBREQ_RESULT_GET_HERO_CARD
    {
        DWORD   dwActorID;
        int     nCount;
        // nCount * DBREQ_PARAM_HERO_CARD
        DBREQ_RESULT_GET_HERO_CARD()
        {
            memset( this, 0, sizeof(*this) );
        }
    };

    ///////////////////////////////////本命英雄/////////////////////////////////////////////////////
	// 本命英雄表结构
	struct DBREQ_TABLE_LIFEHERO
	{
		DWORD   dwActorID;                                      // 角色ID
		WORD     nHeroID;                                       // 英雄ID
		BYTE     nCultureID;                                     // 培养仓ID
		DBREQ_TABLE_LIFEHERO()
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 写入本命英雄的相关数据       GAMEDB_REQUEST_WRITE_LIFEHERO
	struct DBREQ_PARAM_WRITE_LIFEHERO
	{
		DWORD   dwActorID;                                      // 角色ID
		WORD    nHeroID;                                        // 英雄ID
		BYTE    nCultureID;                                     // 培养仓ID
		DBREQ_PARAM_WRITE_LIFEHERO()
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 读取本命英雄的相关数据       GAMEDB_REQUEST_READ_LIFEHERO
	struct DBREQ_PARAM_READ_LIFEHERO
	{
		DWORD   dwActorID;                                      // 角色ID
		DBREQ_PARAM_READ_LIFEHERO()
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 返回本命英雄的操作结果       GAMEDB_REQUEST_READ_LIFEHERO
	struct DBREQ_RESULT_READ_LIFEHERO
	{
		int nCount;
		// DBREQ_TABLE_LIFEHERO * nCount
		DBREQ_RESULT_READ_LIFEHERO()
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 删除本命英雄的相关数据       GAMEDB_REQUEST_DEL_LIFEHERO
	struct DBREQ_PARAM_DEL_LIFEHERO
	{
		DWORD   dwActorID;                                      // 角色ID
		WORD     nHeroID;                                       // 英雄ID
		DBREQ_PARAM_DEL_LIFEHERO()
		{
			memset(this, 0, sizeof(*this));
		}
	};

    ////////////////////////////////////英雄相关结束//////////////////////////////   

    /// GAMEDB_REQUEST_ACTORFINISH 角色数据存盘完成 
    /// 传入参数结构
    struct DBREQ_PARAM_ACTORFINISH
    {
        DWORD	dwUserID;								/// 帐号ID
        DWORD   dwActorID;								/// 角色ID
        BYTE    byOnOffLine;							/// 上下线标志:0:跨进程切换 1:上线(也包括因跨区切换而上线) 2:下线(也包括因跨区切换而下线)
        BYTE	nCheckExistPDBID;						/// 是否需要检查PDBID是否存在(0:不需要 1:需要)

        DBREQ_PARAM_ACTORFINISH(void)
        {
            memset(this, 0, sizeof(DBREQ_PARAM_ACTORFINISH));
        }
    };

    /// 角色数据存盘完成: 返回结果
    struct DBREQ_RESULT_ACTORFINISH
    {
        DWORD	dwUserID;								/// 帐号ID
        DWORD   dwActorID;								/// 角色ID
        BYTE    byOnOffLine;							/// 上下线标志:0:非上下线，1:上线，2:下线

        DBREQ_RESULT_ACTORFINISH(void)
        {
            memset(this, 0, sizeof(DBREQ_RESULT_ACTORFINISH));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_SETUSERLOCK 设置帐号锁 : 传入参数结构
    struct DBREQ_PARAM_SETUSERLOCK
    {
        DWORD	dwUserID;								/// 帐号ID
        DWORD   dwActorID;								/// 角色ID
        BYTE    OnOffLine;								/// 上下线标志,0:跨进程切换 1:上线(也包括因跨区切换而上线) 2:下线(也包括因跨区切换而下线) 3:因创建人物失败而取消永久锁
        int		nLockFlag;								/// 锁标志
        bool	bOnlySetLock;							/// 仅加锁（为0是采用下面的规则，为1时如果以前没有锁，则只加一把永久锁，如果有锁就直接返回，
        ///         如果当前没有锁的时候，要报警）
        BYTE	nZoneSvrHardID;							/// 场景服ID

        // 因为一个帐号有可能会两个游戏世界来锁住（普通区，公共区）
        // 比如：如果是普通区加的锁，那么只能由普通区来解锁，公共区就能解了，否则
        // 有可能角色在游戏里，就会被解了
        int		nGameWorldID;							/// 何个游戏世界加的锁
        // 是否需要验证角色存在
        BYTE	nCheckActorExist;						/// 0:不需验证 1: 需要验证
        //char	szClientIP[32];							/// 客户端IP
        // （0：无保护锁;1：一分钟保护锁;2：永久保护锁）
        /* 加锁规则：
        一、如果申请“无保护锁”，则表示解锁
        1、原锁为0：报警，修改为0，返回0, 若worldID不等，则报警, 并将worldID改为0
        2、原锁为1：报警，修改为0，返回0, 若worldID不等，则报警, 并将worldID改为0
        3、原锁为2：正常，修改为0，返回0, 若worldID不等，则报警, 并将worldID改为0
        4、*** 必须成功设置标志为0的时候，才清掉场景服ID, worldID

        二、如果申请“一分钟保护锁“
        1、原锁为0：修改为1，并记录当前时间，返回1,　若worldID不等于0, 则报警, 并更新worldID
        2、原锁为1：1) 如果前记录时间未过一分钟，则返回2
        2) 过了一分钟，将当前时间记录下来，返回1, 若worldID不等于0, 则报警, 并更新worldID
        3、原锁为2：返回2
        4、*** 必须成功设置标志为1的时候，才更新场景服ID, worldID

        三、如果申请“永久保护锁”
        1、原锁为0：报警，修改为2，返回2, 若worldID不等，则报警, 并更新worldID
        2、原锁为1：正常，修改为2，返回2, 若worldID不等，则报警, 并更新worldID
        3、原锁为2：报警，返回2, 若worldID不等，则报警, 并更新worldID
        4、*** 必须成功设置标志为2的时候，才更新场景服ID, worldID
        //*/

        // 如果游戏世界不对，按正常流程走，但一定要报警，并且输出原因

        DBREQ_PARAM_SETUSERLOCK(void)
        {
            memset(this, 0, sizeof(DBREQ_PARAM_SETUSERLOCK));
        }
    };

    /// 角色数据存盘完成: 返回结果
    struct DBREQ_RESULT_SETUSERLOCK
    {
        DWORD	dwUserID;									/// 帐号ID
        int		nLockFlag;									/// 当前锁标志
        int		nWarn;										/// 依上规测，是否报警，如果报警，则会
        char	szErrorDesc[256];							/// 登陆不上的原因
        BYTE	nActorExistResult;							/// 0:不存在 1:存在(如果不需要验证,也是返回1)

        DBREQ_RESULT_SETUSERLOCK(void)
        {
            memset(this, 0, sizeof(DBREQ_RESULT_SETUSERLOCK));
        }
    }; 

    //////////////////////////////////////////////////////////////////////////
    // GAMEDB_REQUEST_AUTOUNLOCKZONESVR 自动解某台场景服的锁: 传入参数结构
    struct DBREQ_PARAM_AUTOUNLOCKZONESVR
    {
        /* 当碰到此请求时，遇到那些锁标志不为0，并且场景服id等于nZoneSvrHardID
        并为了保险起见：要为那些通知这种方式解锁的帐号作好日志*/
        BYTE		nZoneSvrHardID;							/// 场景服ID

        // 因为一个帐号有可能会两个游戏世界来锁住（普通区，公共区）
        // 比如：如果是普通区加的锁，那么只能由普通区来解锁，公共区就能解了，否则
        // 有可能角色在游戏里，就会被解了
        int			nGameWorldID;							/// 解何个游戏世界加的锁
        DBREQ_PARAM_AUTOUNLOCKZONESVR(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // GAMEDB_REQUEST_FORCEUNLOCK 强制除提交的帐号ID外，本游戏世界里的所有角色全部解了
    struct DBREQ_PARAM_FORCEUNLOCK
    {
        // 因为一个帐号有可能会两个游戏世界来锁住（普通区，公共区）
        // 比如：如果是普通区加的锁，那么只能由普通区来解锁，公共区就能解了，否则
        // 有可能角色在游戏里，就会被解了
        int			nGameWorldID;							/// 解何个游戏世界加的锁
        int			nInGameUserCount;						/// 在本游戏世界里的dwActorID
        // ........											/// 不定长的数据（所有dwActorID）

        DBREQ_PARAM_FORCEUNLOCK(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_QUERYLOCKFLAG 查询锁标志 : 传入参数结构
    struct DBREQ_PARAM_QUERYLOCKFLAG
    {
        DWORD	dwUserID;								/// 帐号ID
        DWORD	dwActorID;								/// 如果不为0，表示需查询是否本区角色

        DBREQ_PARAM_QUERYLOCKFLAG(void)
        {
            memset(this, 0, sizeof(DBREQ_PARAM_QUERYLOCKFLAG));
        }
    };

    /// 查询锁标志: 返回结果
    struct DBREQ_RESULT_QUERYLOCKFLAG
    {
        DWORD	dwUserID;							/// 帐号ID
        int		nLockFlag;							/// 锁标志(0:表示没锁 2:表示锁号)
        BYTE	nHavePerson;						/// 本区是否有此角色(0:没有 1:有)

        DBREQ_RESULT_QUERYLOCKFLAG(void)
        {
            memset(this, 0, sizeof(DBREQ_RESULT_QUERYLOCKFLAG));
        }
    };
 
    // 战场记录
    struct DBREQ_PARAM_WAR_GAMERECORD
    {
        LONGLONG    nWarID;                                 // 战场ID
        DWORD       dwWorldID;                              // 游戏世界ID
        WORD        wWarType;                               // 战场类型
        DWORD       dwBeginTime;                            // 战场开始时间
        DWORD       dwEnTime;                               // 战场结束时间
        DWORD       dwActorID;		                        // 房创建者PDBID
        BYTE        byEndBecause;                           // 结束原因 1、代表阵营1胜利 2、代表阵营2胜利 3、代表阵营1投降 4、代表阵营2投降
        BYTE        byCamp1Num;                             // 阵营1人数 
        BYTE        byCamp2Num;                             // 阵营2人数
        BYTE        byCamp3Num;                             // 阵营3人数 
        WORD        wCamp1Score;                            // 阵营1得分数
        WORD        wCamp2Score;                            // 阵营2得分数
        WORD        wCamp3Score;                            // 阵营3得分数
        DWORD       wWatchNum;                              // 观看人数
        DWORD       dwOperateTime;                          // 操作时间
        bool        bIsEnd;                                 // 是否已经结束  0 带表未结束  1 代表已经结束
		WORD		wBanHeroID1[DBDEF_BAN_HERO_SIZE];		// 阵营1禁用英雄列表
		WORD		wBanHeroID2[DBDEF_BAN_HERO_SIZE];		// 阵营2禁用英雄列表
		DWORD		dwMatchScore;							// 本局比赛平均分
		bool		bIsFull;								// 是否满人

        DBREQ_PARAM_WAR_GAMERECORD()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // GAMEDB_REQUEST_WAR_SAVE_GAME   比赛记录
    // 写入时数据结构
    struct DB_REQUEST_WARACTOR_SAVE
    {
        DBREQ_PARAM_WAR_GAMERECORD warInfo;
        int  nCount;			    // DBREQ_PARAM_WAR_ACTOR_INFO 记录数量
        // 后面压入数据：nCount*DBREQ_PARAM_WAR_ACTOR_INFO

        DB_REQUEST_WARACTOR_SAVE(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // DBREQ_PARAM_WAR_ACTOR_INFO   每局个人详细对战信息
    struct DBREQ_PARAM_WAR_ACTOR_INFO
    {
        DWORD       dwWorldID;                              // 游戏世界ID
        DWORD       dwUserID;                               // 用户ID
        DWORD       dwActorID;                              // 角色ID
        char        szActorName[DBDEF_USERNAME_MAXSIZE];    // 角色名称
        DWORD       dwEquipID[DBDEF_EQUIP_MAX_INDEX];       // 装备id  下标代表槽位
        WORD        wHeroID;                                // 英雄ID
        WORD        wHeroLevel;                             // 英雄等级
        DWORD       dwGetMoney;                             // 累计金币数
		DWORD		dwScore;								// 战绩评分
        WORD        wDieCount;                              // 死亡次数
        WORD        wAssistCount;                           // 助攻次数
        WORD        wKillPersonNum;                         // 击杀人物数量
        WORD        wKillMonsterNum;                        // 击杀怪物数
        WORD        wContinueKillMax;                       // 最大连杀数
        BYTE        byMultiKillMax;                         // 最大多杀
        DWORD		dwCrildDamageMax;                       // 最大暴击
        BYTE        byKillTowerNum;                         // 推塔数
        DWORD       dwOutHeroPhysics;                       // 对英雄的总物理伤害
        DWORD       dwOutHeroMagic;                         // 对英雄的总法术伤害
        DWORD       dwOutHeroTrue;                          // 对英雄的总真实伤害
        DWORD       dwOutTotal;                             // 总输出
		DWORD		dwBearMagic;							// 承受的法术伤害
		DWORD		dwBearPhysics;							// 承受的物理伤害
		DWORD		dwBearTrue;								// 承受的真实伤害
        DWORD       dwDamageTotal;                          // 承受的总伤害
        DWORD       dwCureTotal;                            // 总治疗量
        BYTE        byCamp;                                 // 所属阵营  CAMP_TYPE  1代表阵营1 2代表阵营2
		DWORD		dwRecoveryHP;							// 自身恢复的生命值

        BYTE        byHeroType;                             // 英雄类型
        WORD        wKdaScore;                              // kda得分
        WORD        wGlory;                                 // 获得的荣耀
        BYTE        byWinType;                              // 胜利类型（1 甩锅局， 2躺赢局， 3Carray局， 4年雅居， 5福利局 6尽力局 7挂机局  8翻盘局）
		BYTE		byPlayerType;							// 玩家类型 (1 玩家 2 机器人 待续)

		WORD		byThrillSave;							// 惊现救治次数
		WORD		byDeadlyControl;						// 夺命控制次数
		BYTE		byTeamBattleControlMax;					// 团控人数最大值
		WORD		byAliveAssistMax;						// 活着连助攻最大值

		WORD        wContinueKill[DBDEF_WAR_ACTOR_AK_TK_TC_GROUP_LEN_MAX];        // 连杀数组
		BYTE        byMultiKill[DBDEF_WAR_ACTOR_AK_TK_TC_GROUP_LEN_MAX];		     // 多杀数组
		BYTE        byTeamBattleControl[DBDEF_WAR_ACTOR_AK_TK_TC_GROUP_LEN_MAX];  // 团控数组
		WORD		wGPM;										// 每分钟金钱
		BYTE        byMoneyRate;									// 金钱比 扩大一百倍
		BYTE        byOutputRate;								// 输出率 扩大一百倍
		BYTE        byJoinBattleRate;							// 参战率 扩大一百倍
		BYTE        byKillDragen;								// 控龙
		WORD        wKillDestroyScore;							// 击杀破塔得分
		WORD        wTeamContributionScore;						// 团队贡献得分
		WORD        wSpeicalContributionScore;					// 特殊贡献得分
		WORD        wComprehensiveScore;							// 总表现分
        WORD        wSkinID;                               		// 皮肤ID		

		BYTE	    byFirstBlood;								// 是否拿了一血
		DWORD		dwSummerID;									// 召唤师技能
		DWORD		dwXPSkillID;								// XP技能
        BYTE     	nSex;										// 性别 0男 1女 2不区分
		int			nTalentID[DBDEF_WAR_ACTIVE_TALENT_SIZE];	// 天赋激活历史记录
        BYTE		nHeroStar;                                  // 英雄星级

		DBREQ_PARAM_WAR_ACTOR_INFO()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // GAMEDB_REQUEST_WAR_SAVE_BUYEQUIP                       购买装备记录
    // 写入时数据结构
    struct DBREQ_PARAM_WAR_BUYEQUIP_SAVE
    {
        LONGLONG    nWarID;             // 战场ID,DBREQ_PARAM_WAR_GAMERECORD.nWarID
        int  nCount;			        // DBREQ_PARAM_WAR_BUYEQUIP_DETAIL 记录数量
        // 后面压入数据：nCount*DBREQ_PARAM_WAR_BUYEQUIP_DETAIL

        DBREQ_PARAM_WAR_BUYEQUIP_SAVE(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    // DBREQ_PARAM_WAR_BUYEQUIP_DETAIL   购买装备记录
    struct DBREQ_PARAM_WAR_BUYEQUIP_DETAIL
    {
        DWORD       dwActorID;                              // 角色ID
        DWORD       dwEquipID;                              // 装备id
        BYTE        byBuyType;                              // 购买类型
        DWORD       dwOperateTime;                          // 操作时间

        DBREQ_PARAM_WAR_BUYEQUIP_DETAIL()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // GAMEDB_REQUEST_WAR_SAVE_MONSTERINFO                    战场怪物记录
    // 写入时数据结构
    struct DBREQ_PARAM_WAR_MONSTERINFO_SAVE
    {
        LONGLONG    nWarID;             // 战场ID,DBREQ_PARAM_WAR_GAMERECORD.nWarID
        int  nCount;			        // DBREQ_PARAM_WAR_MONSTERDIE 记录数量
        // 后面压入数据：nCount*DBREQ_PARAM_WAR_MONSTERDIE

        DBREQ_PARAM_WAR_MONSTERINFO_SAVE(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    // 怪物死亡记录
    struct DBREQ_PARAM_WAR_MONSTERDIE
    {
        DWORD       dwMonsterID;                            // 怪物id
        BYTE        byCamp;                                 // 阵营
        DWORD       dwActorID;                              // 角色ID
        DWORD       dwOperateTime;                          // 操作时间

        DBREQ_PARAM_WAR_MONSTERDIE()
        {
            memset(this, 0, sizeof(*this));
        }
    };

	//////////////////////////////////////////////////////////////////////////
	// GAMEDB_REQUEST_WAR_GET_ACTORGAME,						// 获取比赛记录
	// 获取角色比赛记录数据结构
	struct DBREQ_PARAM_WAR_GET_ACTORGAME
	{
		DWORD dwActorID;

		DBREQ_PARAM_WAR_GET_ACTORGAME(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 获取角色比赛记录返回信息
	struct DBREQ_RESULT_WAR_GET_ACTORGAME
	{
		DWORD   dwActorID;				// 角色ID
		DWORD	dwRecentKillCount;		// 近30天杀敌数
		DWORD	dwRecentDieCount;		// 近30天死亡数
		DWORD	dwRecentAssistCount;	// 近30天助攻数
		DWORD	dwRecentScore;			// 近30天战绩评分
		WORD	wRecentWinCount;		// 近30天胜场
		WORD	wRecentLoseCount;		// 近30天负场

		DWORD   dwCount;				// 历史战绩数量，按时间排序，不超过100条
		// dwCount * DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO

		DBREQ_RESULT_WAR_GET_ACTORGAME()
		{
			memset( this, 0, sizeof(*this) );
		}
	};

	// 获取角色比赛记录返回信息子项
	struct DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO
	{
		LONGLONG    nWarID;                                 // 战场ID
		DWORD       dwBeginTime;                            // 战场开始时间
		WORD        wHeroID;                                // 英雄ID
		WORD        wWarType;                               // 战场类型
		BYTE		byWin;									// 是否胜利(0:失败，1:胜利)
		BYTE        byWinType;                              // 胜利类型(1:甩锅局, 2:躺赢局, 3:Carray局, 4:年雅居, 5:福利局, 6:尽力局, 7:挂机局, 8:翻盘局)
		WORD		wBanHeroID1[DBDEF_BAN_HERO_SIZE];		// 阵营1禁用英雄列表
		WORD		wBanHeroID2[DBDEF_BAN_HERO_SIZE];		// 阵营2禁用英雄列表

		DBREQ_RESULT_WAR_GET_ACTORGAME_WARINFO(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	//////////////////////////////////////////////////////////////////////////
	// GAMEDB_REQUEST_WAR_GET_WARGAME,							// 获取指定比赛记录
	// 获取比赛记录数据结构
	struct DBREQ_PARAM_WAR_GET_WARGAME
	{
		LONGLONG    nWarID;                                 // 战场ID
        BYTE        byWnd;                                  // 返回到哪个窗体 1、角色（默认） 2、杯赛

		DBREQ_PARAM_WAR_GET_WARGAME(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 获取比赛记录数据结构返回信息
	struct DBREQ_RESULT_WAR_GET_WARGAME
	{
		LONGLONG    nWarID;                                 // 战场ID（直通）
        BYTE        byWnd;                                  // 返回到哪个窗体 1、角色（默认） 2、杯赛（直通）
		DWORD       dwBeginTime;                            // 战场开始时间
		DWORD       dwEndTime;                              // 战场结束时间
		WORD        wWarType;                               // 战场类型
		BYTE        byEndBecause;                           // 结束原因 1、代表阵营1胜利 2、代表阵营2胜利 3、代表阵营1投降 4、代表阵营2投降
		WORD		wBanHeroID1[DBDEF_BAN_HERO_SIZE];		// 阵营1禁用英雄列表
		WORD		wBanHeroID2[DBDEF_BAN_HERO_SIZE];		// 阵营2禁用英雄列表

		BYTE		byActorCount;							// 参与人数
		// dwActorCount * DBREQ_PARAM_WAR_ACTOR_INFO

		DBREQ_RESULT_WAR_GET_WARGAME(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	///////////////////////////////////////////////////////////////////////////
	// GAMEDB_REQUEST_WAR_SAVE_BAN_HERO						传入参数
	// 保存被Ban英雄列表
	struct DBREQ_PARAM_WAR_SAVE_BAN_HERO
	{
		LONGLONG    nWarID;                                 // 战场ID
		DWORD       dwBeginTime;                            // 战场开始时间
		DWORD       dwEndTime;                              // 战场结束时间
		WORD        wWarType;                               // 战场类型
		bool		bIsFull;								// 是否满人
		bool		bIsHasComputer;							// 是否有电脑玩家
		DWORD		dwAvgRankScore;							// 本局平均匹配分
		WORD		wBanHeroList[DBDEF_BAN_HERO_TOTAL_SIZE];// 被禁英雄列表

		DBREQ_PARAM_WAR_SAVE_BAN_HERO(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

    /////////////////////////////////START/////////////////////////////////////////
    // GAMEDB_REQUEST_GET_MATCHTYPE_ALL_RANK,                // 获取Rank分值
    // GAMEDB_REQUEST_SET_MATCHTYPE_RANK,                // 设置更新Rank分值段位

    // 获取Rank段位信息 GAMEDB_REQUEST_MATCHTYPE_GET_ALL_RANK
    struct DBREQ_GET_MATCHTYPE_ALL_RANK     // 传入参数
    {
        DWORD    dwUserID;          // 玩家账号ID
        DWORD    dwActorID;         // 角色ID

        DBREQ_GET_MATCHTYPE_ALL_RANK(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    // 获取Rank段位信息 GAMEDB_REQUEST_MATCHTYPE_GET_ALL_RANK
    struct DBREQ_RESULT_MATCHTYPE_ALL_RANK    // 传出参数
    {
        DWORD    dwUserID;          // 玩家账号ID 直通
        DWORD    dwActorID;         // 角色ID 直通
        int      nReCount;          // 实际返回的记录数量

        // nReCount *DBREQ_RANK_RECORD_INFO

        DBREQ_RESULT_MATCHTYPE_ALL_RANK(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 单个撮合记录详细信息
    struct DBREQ_RANK_RECORD_INFO
    {
        BYTE    nMatchType;
        DWORD   nRankScore;
        DWORD   nGrade;
		DWORD	dwMatchNum;
		DWORD	dwWinNum;
        DWORD   nHideRankScore;

        DBREQ_RANK_RECORD_INFO()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    typedef  DBREQ_RANK_RECORD_INFO MatchTypeRankNode;

    // 设置撮合详细信息 GAMEDB_REQUEST_SET_MATCHTYPE_RANK
    struct DBREQ_SET_MATCHTYPE_RANK  // 传入参数
    {
        DWORD    dwUserID;      // 玩家账号ID
        DWORD    dwActorID;     // 角色ID
        BYTE     nMatchType;	// 撮合类型
        int      nRankScore;    // 得分 增加减少多少得分
        int      nGrade;        // 段位 上升或者下降段位
		DWORD	 dwMatchNum;	// 匹配场数
		DWORD	 dwWinNum;		// 胜利场数
        int      nHideRankScore;// 隐藏分
        DBREQ_SET_MATCHTYPE_RANK()
        {
            memset(this, 0, sizeof(*this));
        }
    };
	///////////////////////////////////////END////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // GAMEDB_REQUEST_GET_MATCH_ALL_HEROSCORE,           // 获取英雄熟练度得分
    // GAMEDB_REQUEST_SET_MATCH_HEROSCORE,           // 设置英雄熟练度得分
    // 获取Rank段位信息 GAMEDB_REQUEST_GET_MATCH_ALL_HEROSCORE
    // 输入结构
    struct DBREQ_GET_MATCH_ALL_HEROSCORE
    {
        DWORD    dwUserID;          // 玩家账号ID
        DWORD    dwActorID;         // 角色ID

        DBREQ_GET_MATCH_ALL_HEROSCORE(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    // 返回结构
    struct DBREQ_RESULT_MATCH_ALL_HEROSCORE
    {
        DWORD    dwUserID;          // 玩家账号ID
        DWORD    dwActorID;         // 角色ID
        int      nReCount;          // 返回的记录数量

        //  nReCount * DBREQ_HEROSCORE_RECORD_INFO
        DBREQ_RESULT_MATCH_ALL_HEROSCORE(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };


    // 设置撮合详细信息
    struct DBREQ_HEROSCORE_RECORD_INFO
    {
        int     nHeroID;	    // 英雄ID
        DWORD   nHeroScore;     // 英雄熟练度得分
        int     nHeroGrade;     // 英雄熟练度段位

        DBREQ_HEROSCORE_RECORD_INFO()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    typedef  DBREQ_HEROSCORE_RECORD_INFO MatchHeroScoreNode;

    // 设置英雄得分详细信息 GAMEDB_REQUEST_SET_MATCH_HEROSCORE
    struct DBREQ_SET_MATCH_HEROSCORE
    {
        DWORD    dwUserID;      // 玩家账号ID
        DWORD    dwActorID;     // 角色ID
        int      nHeroID;	    // 英雄ID
        int      nHeroScore;    // 英雄熟练度得分 增加减少多少得分
        int      nHeroGrade;    // 英雄熟练度段位

        DBREQ_SET_MATCH_HEROSCORE()
        {
            memset(this, 0, sizeof(*this));
        }
    };


    // 读取排行榜信息查询参数 GAMEDB_REQUEST_READ_MATCHTYPE_ORDER_RANK_INFO
    struct DBREQ_PARAM_MATCHTYPE_ORDERINFO
    {
        BYTE    byMatchType;  								// 查询的类型类型

        DBREQ_PARAM_MATCHTYPE_ORDERINFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 读取排行榜信息 - 返回结果 
    struct DBREQ_RESULT_READ_MATCHTYPE_ORDERINFO_LIST
    {
        BYTE    byMatchType;									// 排行类		
        int     nCount;										// 记录条数 最大200
        // nCount * DBREQ_PARAM_ORDERINFO_PERSON

        DBREQ_RESULT_READ_MATCHTYPE_ORDERINFO_LIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    struct DBREQ_RESULT_MATCHTYPE_ORDERINFO_PERSON
    {
        int		nOrder;  								// 排行名次
        DWORD   dwUserID;                               // 账号ID
        DWORD	dwActorID;								// 角色ID
        char	szActorName[DBDEF_ACTORNAME_MAXSIZE];	// 角色名称
        WORD    wHeadID;                                // 头像Id
        BYTE    byMatchType;	                        // 撮合类型
        DWORD   nRankScore;                             // 得分
        DWORD   nGrade;                                 // 段位等级
		DWORD   dwClanID;			                    // 帮会ID
        char    szClanName[DBDEF_CLANNAME_MAXSIZE];		// 隶属帮会名字

        DBREQ_RESULT_MATCHTYPE_ORDERINFO_PERSON(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_READ_KING_RANK_ORDERINFO 读取排行榜个人信息
    //@ note:
    /// 读取王者排位榜信息	 - 传入参数结构
    struct DBREQ_PARAM_READ_KINGRANK_LIST
    {
        BYTE    byMatchType;                                // 互通字段
        int     nScore;                                     // 互通字段 指定的段位以上分数 >= nScore 
        int     nPageSize;              					// 每页显示记录条数，默认为100
        int     nPageIndex;            						// 当前提取要显示的页码，默认为1，数据库根据PageSize,PageNumber计算返回一页数据

        DBREQ_PARAM_READ_KINGRANK_LIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 读取王者段位分页信息	 - 结果返回
    struct DBREQ_RESULT_READ_KINGRANK_LIST
    {
        BYTE    byMatchType;                        // 互通字段
        int     nScore;                             // 互通字段 返回指定的段位以上分数 >= nScore 
        int     nPageSize;              			// 互通字段 每页显示记录条数，默认为100
        int     nPageIndex;							// 互通字段 当前页码
        int     nReTotalCount;						// 返回总数
        int     nRetCount;							// 当次返回家族的个数

        // 结果集：nRetCount * DBREQ_RESULT_MATCHTYPE_ORDERINFO_PERSON
        DBREQ_RESULT_READ_KINGRANK_LIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    //GAMEDB_REQUEST_SET_MATCHTYPE_RANK_DETAIL
    struct DBREQ_SET_MATCHTYPE_RANK_DETAIL
    {
        DWORD   dwWarID;                                 // 战场ID 
        DWORD   dwWordID;                                // 世界ID
        DWORD   dwActorID;                               // 角色ID 
        BYTE    byMatchType;                             // 比赛类型 
        DWORD   dwPreMatchTypeRank;                      // 原天梯分
        int     nGetMatchTypeRank;                       // 添加或者减少天梯分
        DWORD   dwSelfAvgMatchTypeRank;                  // 友平均天梯分 
        DWORD   dwEnemyAvgMatchTypeRank;                 // 敌方平均天梯分 
        BYTE    byEndWarReason;                          // 结束原因。

        DBREQ_SET_MATCHTYPE_RANK_DETAIL()
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    // GAMEDB_REQUEST_ACTORLOGIN_LOG,      				
    // 角色登录日志 
    // 传入结构 
	struct DBREQ_PARAM_ACTORLOGIN_LOG
	{
		DWORD	dwWorldID;								   // 世界ID
		DWORD	dwUserID;								   // 账号ID
		char    szUserName[DBDEF_USERNAME_MAXSIZE];        // 用户名
		DWORD	dwActorID;								   // 角色ID
		char    szActorName[DBDEF_ACTORNAME_MAXSIZE];      // 角色名称
		WORD    nLevel;                                    // 等级
        char    szClientIP[DBDEF_CLIENT_IP_MAXSIZE];       // 客户端IP
        char    szMacAddress[DBDEF_MAC_ADDRESS_MAXSIZE];   // 客户端网卡地址
		int     nPartnerID;                                // 网吧ID（合作商ID）
		DWORD   dwDiskSN;                                  // 硬盘序列号

		BYTE  byRankbuffdata[DBDEF_RANK_BUFF_SIZE];        // 排位数据相关
        
		DBREQ_PARAM_ACTORLOGIN_LOG(void)
		{
            memset(this, 0, sizeof(*this));
        }
	};

    ///////////////////////////////////////////////////////////////////
	// GAMEDB_REQUEST_ONLINE_LOG         
    // 角色在线日志
	// 传入参数
	struct GAMEDB_PARAM_ONLINE_LOG   
	{	
		int  nWorldID;				//世界ID
		int  nUserID;				//用户ID
        int  nActorID;				//角色ID
		WORD nLevel;                //等级
		int  nLoginIdentity;    	//登录标识
		BYTE byLogoutFlag;			//下线状态 0默认无效值 1正常下线 2关服下线 3断线自动下线 4挤号下线 5GM号踢下线 6其它

		GAMEDB_PARAM_ONLINE_LOG(void)
		{
			memset(this, 0, sizeof(*this));
		}

	};
    ///////////////////////////////////////////////////////////////////
    //GAMEDB_REQUEST_ONLINENUM_LOG   
    //每一分钟用户在线人数统计
	//传入参数
	struct GAMEDB_PARAM_ONLINENUM_LOG
	{	
		int   nWorldID;				//游戏世界ID
		int   nMonitorTime;			//写入时间
		int   nOnlineMaxNum;		//本区一分钟内最高在线人数
		int   nOnlineAvgNum;		//本区一分钟内平均在线人数
		int   nOnlineMinNum;		//本区一分钟内最小在线人数
		int   nOnlineAreaMaxNum;    //跨区一分钟内最高在线人数 
		int   nOnlineAreaAvgNum;    //跨区一分钟内平均在线人数 
		int   nOnlineAreaMinNum;    //跨区一分钟内最小在线人数 

		GAMEDB_PARAM_ONLINENUM_LOG(void)
		{
			memset(this, 0, sizeof(*this));
		}
	
	};


    //////////////////////////////////////////////////////////////////////////              
    // GAMEDB_REQUEST_READ_HERO_ORDER_CLASS,    英雄养成榜  
    struct DBREQ_RESULT_HEROORDER_HEAD
    {
        int nCount;     // 最大200
        // nCount * DBREQ_RESULT_HEROORDER_DETAIL_INFO
		DBREQ_RESULT_HEROORDER_HEAD(void)
        {
            memset(this, 0, sizeof(*this));
        }			
    };

    struct DBREQ_RESULT_HEROORDER_DETAIL_INFO
    {
        int		nOrder;                                     // 排名
        DWORD   dwUserID;                                   // 账号ID
        DWORD   dwPlayerID;                                 // 玩家ID
        char    szName[DBDEF_ACTORNAME_MAXSIZE];            // 玩家名字
        WORD    wHeadID;                                    // 头像Id
        DWORD   dwHeroID;                                   // 英雄ID
        DWORD   nHeroScore;                                 // 战绩
        WORD    nHeroGrade;                                 // 熟练等级
        BYTE    nHeroStar;                                  // 星级
        DWORD   dwWinNum;                                   // 比赛胜利场次
        DWORD   dwMatchNum;                                 // 比赛场次
        DWORD   dwClanID;			                        // 帮会ID
        char    szClanName[DBDEF_CLANNAME_MAXSIZE];			// 隶属帮会名字
		
		DBREQ_RESULT_HEROORDER_DETAIL_INFO(void)
        {
            memset(this, 0, sizeof(*this));
        }			
    };


    //////////////////////////////////////////////////////////////////////////
    // 英雄类型榜 定时刷新
    //GAMEDB_UPDATE_HEROTYPE_ORDER_CLASS
    struct DBREQ_PARAM_UPDATE_HEROTYPE_INFO
    {
        BYTE    byMatchType;   // 比赛类型
        BYTE    byHeroType;    // 英雄的类型（战士 法师 刺客 ADC 辅助 ）
        DWORD   nHeroID;       // 英雄ID
        bool    bWin;		   // 是否成年,0:输，1;赢
        bool    bHaveComputer; // 是否有电脑参加
		DWORD   nRankScore;    // 本类型的Rank分
		DWORD   nRankGrade;    // 本类型的Rank等级分
		
		DBREQ_PARAM_UPDATE_HEROTYPE_INFO(void)
        {
            memset(this, 0, sizeof(*this));
        }			
    };

    struct DBREQ_HEROTYPE_ORDER_INFO
    {
        BYTE    byMatchType;  // 0为统计所有综合类型 统计某个比赛类型的
        BYTE    byHeroType;   // 英雄的类型（战士 法师 刺客 ADC 辅助 ）
		
		DBREQ_HEROTYPE_ORDER_INFO(void)
        {
            memset(this, 0, sizeof(*this));
        }			        
    };

    struct DBREQ_RESULT_HEROTYPE_ORDER_DETAIL_INFO
    {
        BYTE    byMatchType;    // 0为综合类型 然后为统计某个比赛类型的
        BYTE    byHeroType;     // 英雄的类型（战士 法师 刺客 ADC 辅助 ）

        DWORD   dwWinHeroID;    // 胜率最高英雄ID
        DWORD   WinNum;        // 胜利率

        DWORD   dwOnStageHeroID; // 上场率最高英雄ID
        DWORD   OnStageNum;     // 上场率

        DWORD   dwUpperFastestHeroID;    // 最快提升英雄ID
        DWORD   dwUpperFastestNum;// 最快提升
		
		DBREQ_RESULT_HEROTYPE_ORDER_DETAIL_INFO(void)
        {
            memset(this, 0, sizeof(*this));
        }	
				
    };


    /// GAMEDB_REQUEST_READ_ORDERINFO_SETTING
    /// 设置用户是否在排行榜中显示
    /// 输入参数

    struct DBREQ_READ_ORDERINFO_SETTING_TABLE
    {
        int  	nOrderType;						// 排行种类
        BYTE	nEnable;                        // 0为取消  1为设置  (设置就是不在排行榜中显示)

        DBREQ_READ_ORDERINFO_SETTING_TABLE(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    struct DBREQ_PARAM_WRITE_ORDERINFO_SETTING
    {
        int		 nActorID; 					// 角色ID
        int		 nCount;                    // 数量
        // nCount * DBREQ_READ_ORDERINFO_SETTING_TABLE 不定长

        DBREQ_PARAM_WRITE_ORDERINFO_SETTING(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    //  返回结果
    struct DBREQ_RESULT_WRITE_ORDERINFO_SETTING
    {
        int		nActorID;                                   //

        DBREQ_RESULT_WRITE_ORDERINFO_SETTING(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///GAMEDB_REQUEST_READ_ORDERINFO_SELECTSETTING
    /// 查询用户是否设置在排行榜中显示
    /// 输入参数
    struct DBREQ_PARAM_READ_ORDERINFO_SELECTSETTING
    {
        int  	nActorID; 					// 角色ID
        DBREQ_PARAM_READ_ORDERINFO_SELECTSETTING(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 读取排行榜设置数据返回
    struct DBREQ_RESULT_READ_ORDERINFO_SETTING
    {
        int		nActorID;					// 角色ID
        int		nCount;						// 返回数据个数
        // nCount * DBREQ_READ_ORDERINFO_SETTING_TABLE
        DBREQ_RESULT_READ_ORDERINFO_SETTING(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };


    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_READ_ORDERINFO_PERSON 读取排行榜个人信息
    //@ note:
    /// 读取排行榜信息	 - 传入参数结构
    struct DBREQ_PARAM_READ_ORDERINFO_PERSON
    {
        int nOrderType;								// 排行类

        DBREQ_PARAM_READ_ORDERINFO_PERSON(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 读取排行榜信息 - 返回结果
    struct DBREQ_RESULT_READ_ORDERINFO_PERSON
    {
        int nOrderType;									// 排行类		
        int nCount;										// 记录条数 最大200
        // nCount * DBREQ_PARAM_TABLE_ORDERINFO_PERSON

        DBREQ_RESULT_READ_ORDERINFO_PERSON(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };


    struct DBREQ_PARAM_TABLE_ORDERINFO_PERSON
    {
        int		nOrder;  								// 排行名次
        int		nLastOrder;								// 上次排名
        DWORD   dwUserID;                               // 账号ID
        DWORD	dwActorID;								// 角色ID
        char	szActorName[DBDEF_ACTORNAME_MAXSIZE];	// 角色名称
        WORD    wHeadID;                                // 头像Id		
        DWORD   dwClanID;                               // 帮会ID
        DWORD   dwKinID;                                // 战队ID
        BYTE    byMatchType;                            // 排位相关信息
        DWORD   nRankScore;
        DWORD   nGrade;

        long	lValue1;								// 具体排行值1
        long	lValue2;								// 具体排行值2
        
        DBREQ_PARAM_TABLE_ORDERINFO_PERSON(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    

    //////////////////////////////////////////////////////////////////////////
    /////////////////天赋/////////////////////////////////////////////////////////
    // GAMEDB_REQUEST_GET_TALENTPAGE_POINTS_ALL = 110,               // 获得英雄的所有天赋页的加点数
    // GAMEDB_REQUEST_SET_TALENTPAGE_POINTS,			             // 设置英雄的某个天赋页的加点数  

    struct DBREQ_PARAM_TALENTPAGE
    {
        int     nCurPageIndex;                                  // 第几页天赋页
        char	szTalentPageName[DBDEF_TALENTNAME_MAXSIZE];	        // 该天赋页名称	 
        //BYTE byRecordInfo[]                                   客户端传进来的天赋个数nTalentNum * DBREQ_SINGLE_TALENT_RECORD_INFO,不定长首先压入nTalentNum
    };
    typedef DBREQ_PARAM_TALENTPAGE                      TALENT_PAGE;

    struct DBREQ_SINGLE_TALENT_RECORD_INFO 
    {
        BYTE byTalentID;                                 // 天赋id
        BYTE byPoints;                                   // 天赋加点数
    };
    typedef DBREQ_SINGLE_TALENT_RECORD_INFO             TALENT_INFO;


    // 获取天赋信息 GAMEDB_REQUEST_GET_TALENTPAGE_POINTS_ALL
    // 输入结构
    struct DBREQ_GET_TALENTPAGES_ALL_POINTS
    {
        DWORD    dwActorID;         // 角色ID

        DBREQ_GET_TALENTPAGES_ALL_POINTS(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    typedef DBREQ_GET_TALENTPAGES_ALL_POINTS             TALENT_DATA_REQUEST;
    //返回结构
    struct DBREQ_RESULT_TALENTPAGES_ALL_POINTS
    {
        DWORD    dwActorID;                         // 角色ID 直通
        BYTE     byPageNum;                         // 实际返回的记录数量

        // byPageNum *DBREQ_PARAM_TALENTPAGE

        DBREQ_RESULT_TALENTPAGES_ALL_POINTS(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    typedef DBREQ_RESULT_TALENTPAGES_ALL_POINTS          TALENT_DATA_RESULT;
    //////////////////////////////////////////////////////////////////////////
    //存DBREQ_SET_TALENTPAGE_POINTS+客户端传进来的天赋个数nTalentNum+nTalentNum*DBREQ_SINGLE_TALENT_RECORD_INFO
    //读DBREQ_RESULT_TALENTPAGES_ALL_POINTS+DBREQ_RESULT_TALENTPAGES_ALL_POINTS.byPageNum*DBREQ_PARAM_TALENTPAGE+读出首个nTalentNum*DBREQ_SINGLE_TALENT_RECORD_INFO
    //删DBREQ_DELETE_TALENTPAGE_POINTS
    //////////////////////////////////////////////////////////////////////////

    // 设置单个天赋页详细信息 GAMEDB_REQUEST_SET_TALENTPAGE_POINTS
    struct DBREQ_SET_TALENTPAGE_POINTS
    {
        DWORD	dwActorID;								       // 角色ID
        DBREQ_PARAM_TALENTPAGE page;

        DBREQ_SET_TALENTPAGE_POINTS(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    typedef DBREQ_SET_TALENTPAGE_POINTS                    TALENT_DATA_SET;

    // 删除单个天赋页详细信息 GAMEDB_REQUEST_DELETE_TALENTPAGE_POINTS
    struct DBREQ_DELETE_TALENTPAGE_POINTS
    {
        DWORD	dwActorID;								      // 角色ID
        int     nCurPageIndex;                                  // 第几页天赋页

        DBREQ_DELETE_TALENTPAGE_POINTS(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    typedef DBREQ_DELETE_TALENTPAGE_POINTS                    TALENT_DATA_DELETE;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /***************************************  符文系统 *******************************************/
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 读取数据库数据请求格式(提取一个玩家所有的符文页): DBREQ_PARAM_QUERY_RUNE_INFO
    // 读取数据库数据返回格式(提取一个玩家所有的符文页): DBREQ_RESULT_QUERYRUNEINFO + 
    //                                           DBREQ_RESULT_QUERYRUNEINFO.nPageNum * (DBREQ_PARAM_RUNE_PAGE +
    //                                           // 下面是单个符文页所包含的符文数据集的总长度数据
    //                                           nRuneNum(该符文页符文个数) * sizeof(DBREQ_PARAM_RUNE_INFO) + 
    //                                           // 下面是单个符文页所包含的符文数据集数据
    //                                           nRuneNum(该符文页符文个数) * DBREQ_PARAM_RUNE_INFO)           
    // 写入保存数据库数据格式(一次只保存一页): DBREQ_SAVE_RUNEPAGE + nRuneLen(符文数据长度) + 
    //                                      nRuneNum(该符文页符文个数) * DBREQ_PARAM_RUNE_INFO
    // 符文元素信息
    struct DBREQ_PARAM_RUNE_INFO
    {
        int     nID;                                        // 符文配置ID
        int     nItemID;                                    // 符文物品ID，每个符文的ID都不一样
        BYTE    nPostion;                                   // 对应符文页镶嵌位

        DBREQ_PARAM_RUNE_INFO()
        {
            memset(this, 0, sizeof(*this));
        }
    };
    typedef DBREQ_PARAM_RUNE_INFO                       RUNE_INFO;

    // 符文页
    struct DBREQ_PARAM_RUNE_PAGE
    {
        int     nPageIndex;                                  // 符文页页码
        char	szTalentPageName[DBDEF_TALENTNAME_MAXSIZE];	 // 该符文页名称	 
        //DBREQ_PARAM_RUNE_INFO[]                            // 符文元素集合，不定长
        DBREQ_PARAM_RUNE_PAGE()
        {
            memset(this, 0, sizeof(*this));
        }
    };
    typedef DBREQ_PARAM_RUNE_PAGE                       RUNE_PAGE;

    // 更新单个玩家符文页符文集数据                 GAMEDB_REQUEST_SAVE_RUNE_INFO
    struct DBREQ_SAVE_RUNEPAGE
    {
        DWORD	                dwActorID;						// 角色ID
        DBREQ_PARAM_RUNE_PAGE   page;

        DBREQ_SAVE_RUNEPAGE(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 获取角色所有的符文数据                      GAMEDB_REQUEST_QUERY_RUNE_INFO
    struct DBREQ_PARAM_QUERY_RUNE_INFO
    {
        DWORD	dwActorID;								    // 角色ID

        DBREQ_PARAM_QUERY_RUNE_INFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 查询结果: 角色所有的符文数据
    struct DBREQ_RESULT_QUERYRUNEINFO
    {
        DWORD	dwActorID;								    // 角色ID
        BYTE     nPageNum;                                   // 总共符文页数量
        // nPageNum *DBREQ_PARAM_RUNE_PAGE                  // 角色所有符文页数据,不定长

        DBREQ_RESULT_QUERYRUNEINFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 删除角色的符文页数据                       GAMEDB_REQUEST_DELETE_RUNE_INFO
    struct DBREQ_DELETE_RUNEPAGE
    {
        DWORD   dwActorID;								    // 角色ID
        int    nPageIndex;                                 // 符文页序号

        DBREQ_DELETE_RUNEPAGE(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /****************************************** 家 族 ******************************************************/
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_READKINLIST 读取家族列表 （社会服启动时候读取）
    /// 读取家族列表	 - 传入参数结构
    struct DBREQ_PARAM_READKINLIST
    {
        int  nPageSize;              						// 每页显示记录条数，默认为100
        int  nPageIndex;            						// 当前提取要显示的页码，默认为1，数据库根据PageSize,PageNumber计算返回一页数据

        DBREQ_PARAM_READKINLIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 家族数据库基本数据结构
    struct DBREQ_TABLE_KIN
    {
        DWORD dwID;									    // 家族ID
        char  szName[DBDEF_KINNAME_MAXSIZE];			// 家族名
        BYTE  nState;								    // 家族的状态：临时，正式，解散中
        DWORD dwShaikhID;							    // 族长ID
        char  szShaikhName[DBDEF_ACTORNAME_MAXSIZE];	// 族长名字

        BYTE  nLevel;								    // 家族等级
        int	  nEstablishTime;						    // 家族创立时间
        int   nDismissApplyTime;				        // 发起解散的时间
        int   nLastLogoutTime;					        // 成员的最后下线时间
        int   nHisActivity;							    // 历史活跃度
        int   nActivity;								// 活跃度
		int   nWeekActivity;                            // 周活跃度，一周清一次
		BYTE  nAutoAccept;					            // 是否自动接收成员
        short nAcceptLevel;						        // 自动收人等级(2字节)
        DWORD dwAcceptSetting; 			                // 自动收人设置(4字节)
        DWORD nMoney;					                // 家族金钱
        int   nLastRenameTime;					        // 家族最后改名时间

		char	szLabel[DBDEF_KIN_LABEL_MAXSIZE];		// 家族标签
		BYTE    byGlory[DBDEF_KIN_LEGENDGLORY_BUFFSIZE];// 家族荣耀BUFF

        DBREQ_TABLE_KIN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 读取家族列表	 - 结果返回
    struct DBREQ_RESULT_READKINLIST
    {
        int nPageIndex;								// 当前页码
        int nReTotalCount;							// 家族总数
        int nKinCount;								// 当次返回家族的个数

        // 结果集：nKINCount * DBREQ_TABLE_KIN
        DBREQ_RESULT_READKINLIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// 读取家族详细资料(读取家族的基本数据)            GAMEDB_REQUEST_READKINBASE 
    /// 读取家族基本数据	 - 传入参数结构
    struct DBREQ_PARAM_READKINBASE
    {
        DWORD dwKinID;								// 家族ID

        DBREQ_PARAM_READKINBASE(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 读取家族详细资料 - 返回结果
    struct DBREQ_RESULT_READKINBASE
    {
        char  szNotice[DBDEF_KIN_NOTICE_MAXSIZE];	    // 家族的每日公告
        char  szSlogan[DBDEF_KIN_SLOGAN_MAXSIZE];	    // 宣传口号
        BYTE  nAutoAccept;							    // 是否自动接收成员
        short nAcceptLevel;							    // 自动收人等级(2字节)
        DWORD dwAcceptSetting;						    // 自动收人设置(4字节)

        DWORD nMoney;						            // 家族金钱
        int   nLastRenameTime;						    // 家族最后改名时间

        DBREQ_RESULT_READKINBASE(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// 创建家族                            GAMEDB_REQUEST_CENTERDBCREATEKIN 
    struct DBREQ_PARAM_CENTERDBCREATEKIN
    {
        DWORD  dwWorldID;								// 世界ID
        DWORD dwPDBID;								    // 玩家ID，设为族长
        char  szName[DBDEF_KINNAME_MAXSIZE];			// 家族名
        int	  nEstablishTime;						    // 家族创立时间
		char  szLabel[DBDEF_KIN_LABEL_MAXSIZE];			// 家族标签
        DBREQ_PARAM_CENTERDBCREATEKIN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 创建家族 - 返回结果
    // 返回值为1 创建成功 -1 那么创建失败 给相应提示
    struct DBREQ_RESULT_CENTERDBCREATEKIN
    {
        DWORD dwKinID;								// 家族ID
        DWORD dwPDBID;								// 玩家ID，设为族长
        char  szName[DBDEF_KINNAME_MAXSIZE];		// 家族名
        int	  nEstablishTime;						// 家族创立时间
		char  szLabel[DBDEF_KIN_LABEL_MAXSIZE];			// 家族标签
        DBREQ_RESULT_CENTERDBCREATEKIN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_CREATEKIN 创建家族
    /// 创建家族	 - 传入参数结构
    struct DBREQ_PARAM_CREATEKIN
    {
        DWORD dwKinID;								// 家族ID
        DWORD dwPDBID;								// 玩家ID，设为族长
        char  szName[DBDEF_KINNAME_MAXSIZE];		// 家族名
        BYTE  nState;								// 家族的状态：临时，正式，解散中
        BYTE  nLevel;								// 家族等级
        char  szNotice[DBDEF_KIN_NOTICE_MAXSIZE];	// 家族的每日公告
        int	  nEstablishTime;						// 家族创立时间
        int   nDismissApplyTime;					// 发起解散的时间
        int   nLastLogoutTime;						// 成员的最后下线时间
		char  szLabel[DBDEF_KIN_LABEL_MAXSIZE];		// 家族标签


        DBREQ_PARAM_CREATEKIN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 创建家族 - 返回结果
    struct DBREQ_RESULT_CREATEKIN
    {
        DWORD dwKinID;								// 家族ID
        char  szName[DBDEF_KINNAME_MAXSIZE];		// 家族名
		char  szLabel[DBDEF_KIN_LABEL_MAXSIZE];		// 家族标签

        DBREQ_RESULT_CREATEKIN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// 游戏世界创建家族后，返回中心数据库 置成败标志             GAMEDB_REQUEST_CENTERKINFLAG
    struct DBREQ_PARAM_CENTERKINFLAG
    {	
        DWORD dwKinID;								// 家族ID
        BYTE  byFlag;								// 标志：1游戏世界创建成功，0失败

        DBREQ_PARAM_CENTERKINFLAG(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 中心数据库 置创建家族成败标志 返回结构体：无，根据接口返回的标准结果1成功，-1失败判断

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_CENTERKINCHK 中心数据库 家族创建成败标志检查
    /// 中心数据库 家族创建成败标志检查:传入参数结构
    struct DBREQ_PARAM_CENTERKINCHK
    {	
        DWORD    dwWorldID;									// 世界ID

        DBREQ_PARAM_CENTERKINCHK(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 中心数据库 家族创建成败标志检查: 返回结构体
    struct DBREQ_RESULT_CENTERKINCHK
    {	
        DWORD dwCount;									    // 未确认的家族ID数量
        // dwCount * dwKinID;								// 未确认的家族ID列表

        DBREQ_RESULT_CENTERKINCHK(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };


    ///////////////////////////////////////////////////////////////////
    ///  更新家族基本信息                           GAMEDB_REQUEST_UPDATEKIN
    struct DBREQ_PARAM_UPDATEKIN
    {
        DWORD dwKinID;								// 家族ID
        char  szName[DBDEF_KINNAME_MAXSIZE];		// 家族名
        BYTE  nState;								// 家族的状态：临时，正式，解散中
        BYTE  nLevel;								// 家族等级
        char  szNotice[DBDEF_KIN_NOTICE_MAXSIZE];	// 家族的每日公告
        int	  nEstablishTime;						// 家族创立时间
        int   nDismissApplyTime;					// 发起解散的时间
        int   nLastLogoutTime;						// 成员的最后下线时间
        int   nHisActivity;							// 历史活跃度
        int   nActivity;							// 活跃度
		int   nWeekActivity;                        // 周活跃度，一周清一次
        char  szSlogan[DBDEF_KIN_SLOGAN_MAXSIZE];	// 宣传口号
        BYTE  nAutoAccept;							// 是否自动接收成员
        short nAcceptLevel;							// 自动收人等级(2字节)
        DWORD dwAcceptSetting;						// 自动收人设置(4字节)
        DWORD nMoney;					            // 家族金钱
        int   nLastRenameTime;						// 家族最后改名时间

		char	szLabel[DBDEF_KIN_LABEL_MAXSIZE];		// 家族标签
		BYTE    byGlory[DBDEF_KIN_LEGENDGLORY_BUFFSIZE];// 家族荣耀BUFF

        DBREQ_PARAM_UPDATEKIN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 更新家族基本信息 - 返回结果
    struct DBREQ_RESULT_UPDATEKIN
    {
        DBREQ_RESULT_UPDATEKIN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// 解散家族(中心数据库)                         GAMEDB_REQUEST_CENTERDBDISMISSKIN
    struct DBREQ_PARAM_CENTERDBDISMISSKIN
    {
        DWORD dwKinID;								// 家族ID

        DBREQ_PARAM_CENTERDBDISMISSKIN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };


    ///////////////////////////////////////////////////////////////////
    ///  解散家族(解散家族的时候，成员数据也要一起删除)      GAMEDB_REQUEST_DISMISSKIN
    struct DBREQ_PARAM_DISMISSKIN
    {
        DWORD dwKinID;								// 家族ID

        DBREQ_PARAM_DISMISSKIN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 解散家族 - 返回结果
    struct DBREQ_RESULT_DISMISSKIN
    {
        DBREQ_RESULT_DISMISSKIN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };


	///////////////////////////////////////////////////////////////////
	///  读取家族杯赛奖励列表       GAMEDB_REQUEST_READ_KIN_LEGENDAWARD
	struct DBREQ_PARAM_READKINLEGENDAWARDLIST
	{
		DWORD dwKinID;									// 家族ID

		DBREQ_PARAM_READKINLEGENDAWARDLIST(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	struct DBREQ_TABLE_KIN_LEGENDAWARD
	{
		LONGLONG	llLegendCupID;							// 杯赛ID
		BYTE		byCupRank;								// 杯赛名次
		char		szCupName[DBDEF_LEGENDCUP_NAME_LEN];	// 杯赛名称
		int			nCupConfigID;							// 杯赛配置
		DWORD		dwCupEndTime;							// 杯赛结束时间

		DBREQ_TABLE_KIN_LEGENDAWARD(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 读取家族杯赛奖励列表 - 返回结果
	struct DBREQ_RESULT_READKINLEGENDAWARDLIST
	{
		int nLegendAwardCount;							// 杯赛奖励的个数
		// 结果集：nLegendAwardCount * DBREQ_TABLE_KIN_LEGENDAWARD

		DBREQ_RESULT_READKINLEGENDAWARDLIST(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	///////////////////////////////////////////////////////////////////
	/// 添加杯赛奖励                 GAMEDB_REQUEST_ADD_KINLEGENDAWARD
	struct DBREQ_PARAM_ADDKINLEGENDAWARD
	{
		DWORD		dwKinID;								// 家族ID
		LONGLONG	llLegendCupID;							// 杯赛ID
		BYTE		byCupRank;								// 杯赛名次
		char		szCupName[DBDEF_LEGENDCUP_NAME_LEN];	// 杯赛名称
		int			nCupConfigID;							// 杯赛配置
		DWORD		dwCupEndTime;							// 杯赛结束时间

		DBREQ_PARAM_ADDKINLEGENDAWARD(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 添加杯赛奖励 - 返回结果
	struct DBREQ_RESULT_ADDKINLEGENDAWARD
	{
		DBREQ_RESULT_ADDKINLEGENDAWARD(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	///////////////////////////////////////////////////////////////////
	///  读取家族战绩列表(=kin1ID,=kin2ID都返回)       GAMEDB_REQUEST_READ_KIN_LEGENDWAR
	struct DBREQ_PARAM_READKINLEGENDWARLIST
	{
		DWORD dwKinID;									// 家族ID

		DBREQ_PARAM_READKINLEGENDWARLIST(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	struct DBREQ_TABLE_KIN_LEGENDWAR
	{
		LONGLONG	llWarID;						// 战场ID
		DWORD		dwKin1ID;						// 家族1ID
		DWORD		dwKin2ID;						// 家族2ID
		DWORD		dwWinKin;						// 胜利家族ID
		DWORD		dwBeginTime;					// 战场开始时间
		char		szKin1Name[DBDEF_KINNAME_MAXSIZE];		// 家族1名称
		char		szKin2Name[DBDEF_KINNAME_MAXSIZE];		// 家族2名称

		DBREQ_TABLE_KIN_LEGENDWAR(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 读取家族杯赛战绩列表 - 返回结果
	struct DBREQ_RESULT_READKINLEGENDWARLIST
	{
		int nLegendWarCount;							// 杯赛战绩的个数
		// 结果集：nLegendWarCount * DBREQ_TABLE_KIN_LEGENDWAR

		DBREQ_RESULT_READKINLEGENDWARLIST(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	///////////////////////////////////////////////////////////////////
	/// 添加杯赛战绩                 GAMEDB_REQUEST_ADD_KINLEGENDWAR
	struct DBREQ_PARAM_ADDKINLEGENDWAR
	{
		LONGLONG	llWarID;						// 战场ID
		DWORD		dwKin1ID;						// 家族1ID
		DWORD		dwKin2ID;						// 家族2ID
		DWORD		dwWinKin;						// 胜利家族ID
		DWORD		dwBeginTime;					// 战场开始时间
		char		szKin1Name[DBDEF_KINNAME_MAXSIZE];		// 家族1名称
		char		szKin2Name[DBDEF_KINNAME_MAXSIZE];		// 家族2名称

		DBREQ_PARAM_ADDKINLEGENDWAR(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 添加杯赛奖励 - 返回结果
	struct DBREQ_RESULT_ADDKINLEGENDWAR
	{
		DBREQ_RESULT_ADDKINLEGENDWAR(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


    ///////////////////////////////////////////////////////////////////
    ///  读取角色惩罚信息      GAMEDB_REQUEST_PLAYER_PUNISHMENT
    struct DBREQ_PARAM_READ_PLAYER_PUNISHMENT
    {
        int  nPageSize;              						// 每页显示记录条数，默认为300
        int  nPageIndex;            						// 当前提取要显示的页码，默认为1，数据库根据PageSize,PageNumber计算返回一页数据

        DBREQ_PARAM_READ_PLAYER_PUNISHMENT(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 读取惩罚成员列表 - 返回结果
    struct DBREQ_RESULT_PUNISHMENT_HEAD
    {
        int nPageIndex;								// 当前页码
        int nReTotalCount;							// 惩罚信息总数
        int nCount;								    // 当次惩罚信息的个数

        // 结果集：nCount * DBREQ_TABLE_PUNISHMENT_INFO

        DBREQ_RESULT_PUNISHMENT_HEAD(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };


    // 惩罚成员结构
    struct DBREQ_TABLE_PUNISHMENT_INFO
    {
        DWORD dwFromWorldID;                        // 来源世界
        DWORD dwPDBID;								// 玩家角色ID
        BYTE  byPunishmentType;                     // 惩罚类型
        DWORD dwPunishStartTime;                    // 惩罚开始时间
        DWORD dwPunishEndTime;                      // 惩罚结束时间

        DBREQ_TABLE_PUNISHMENT_INFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    ///  添加 或者更新 角色惩罚信息      GAMEDB_REQUEST_UPDATE_PLAYER_PUNISHMENT_TIME
    struct DBREQ_TABLE_UPDATE_PUNISHMENT
    {
        DWORD dwFromWorldID;                        // 来源世界
        DWORD dwPDBID;								// 玩家角色ID
        BYTE  byPunishmentType;                     // 惩罚类型
        DWORD dwPunishStartTime;                    // 惩罚开始时间
        DWORD dwPunishEndTime;                      // 惩罚结束时间

        DBREQ_TABLE_UPDATE_PUNISHMENT(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };


    ///////////////////////////////////////////////////////////////////
    ///  移除角色惩罚信息      GAMEDB_REQUEST_DEL_PLAYER_PUNISHMENT_TIME
    struct DBREQ_TABLE_DEL_PUNISHMENT
    {
        DWORD dwPDBID;								// 玩家角色ID
        BYTE  byPunishmentType;                     // 惩罚类型

        DBREQ_TABLE_DEL_PUNISHMENT(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    

    ///////////////////////////////////////////////////////////////////
    ///  读取家族成员列表(家族第一个玩家上线的时候读取)       GAMEDB_REQUEST_READ_KIN_MEMBERLIST
    struct DBREQ_PARAM_READKINMEMBERLIST
    {
        DWORD dwKinID;								// 家族ID

        DBREQ_PARAM_READKINMEMBERLIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 家族成员信息结构
    struct DBREQ_TABLE_KINMEMBER
    {
        DWORD dwUDBID;                              // 玩家账号ID
        DWORD dwPDBID;								// 玩家角色ID
        char  szName[DBDEF_ACTORNAME_MAXSIZE];		// 玩家名字
        DWORD dwKinID;								// 家族ID
        BYTE  nIdentity;							// 家族职位

        int   nDayActivity;                         // 当天活跃度
        int   nWeekActivity;                        // 周活跃度
        int   nClanCtrb;							// 帮会贡献度 2009.07.29
        int   nTotalClanCtrb;						// 累计帮会贡献度 2009.07.29
        DWORD dwLastClanCtrbTime;                   // 上次捐赠时间

        DWORD dwPopedomFlag;						// 权限标志（一个位表示一种权限，一个整数表示玩家所拥有的权限）
        int	  nLogoutTime;							// 最近下线时间 如果为0 表示登陆了或者不处理
        WORD  nHeroID;                              // 静态英雄ID
        int   nLevel;								// 玩家等级
		BYTE  nSex;									// 性别 0男 1女 2不区分
        short nFaceID;	                            // 图像ID     -- add by xmk 20090326
        char  szRemark[32];						    // 成员备注
        DWORD dwBattleVal;							// 战力值 对应  DBREQ_PARAM_PLAYER_INFO dwFightCapacity
        DWORD dwSrcBattleVal;						// 上一次战力值 对应  DBREQ_PARAM_PLAYER_INFO  dwFightCapacity
        DWORD dwBVOrder;							// 战力值排名
        DWORD dwLastOrder;							// 上一次战力值排行	
													// 以下是最大段位信息（nGrade）
        BYTE    nMatchType;
        DWORD   nRankScore;
        DWORD   nGrade;

        DBREQ_TABLE_KINMEMBER(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 读取家族成员列表 - 返回结果
    struct DBREQ_RESULT_READKINMEMBERLIST
    {
        int nMemberCount;							// 家族的个数
        // 结果集：nMemberCount * DBREQ_TABLE_KINMEMBER

        DBREQ_RESULT_READKINMEMBERLIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// 更新成员（没有则添加）                 GAMEDB_REQUEST_UPDATE_KINMEMBER 
    struct DBREQ_PARAM_UPDATEKINMEMBER
    {
        DWORD dwPDBID;								// 玩家角色ID
        DWORD dwKinID;								// 家族ID
        BYTE  nIdentity;							// 家族职位

        int   nClanCtrb;							// 帮会贡献度 2009.07.29
        int   nTotalClanCtrb;						// 累计帮会贡献度 2009.07.29
        DWORD dwLastClanCtrbTime;                   // 上次捐赠时间

        DWORD dwPopedomFlag;						// 权限标志（一个位表示一种权限，一个整数表示玩家所拥有的权限）
        char  szRemark[32];						    // 成员备注
        DWORD dwBattleVal;							// 战力值 对应  DBREQ_PARAM_PLAYER_INFO  dwFightCapacity
        DWORD dwSrcBattleVal;						// 上一次战力值 对应  DBREQ_PARAM_PLAYER_INFO dwFightCapacity
        DWORD dwBVOrder;							// 战力值排名
        DWORD dwLastOrder;							// 上一次战力值排行
        int   nDayActivity;                         // 当天活跃度
        int   nWeekActivity;                        // 周活跃度

        DBREQ_PARAM_UPDATEKINMEMBER(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 更新成员（没有则添加） - 返回结果
    struct DBREQ_RESULT_UPDATEKINMEMBER
    {
        DBREQ_RESULT_UPDATEKINMEMBER(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_DELETE_KINMEMBER 删除成员
    //@ note： 
    /// 删除成员	 - 传入参数结构
    struct DBREQ_PARAM_DELETE_KINMEMBER
    {
        DWORD dwPDBID;								// 玩家角色ID

        DBREQ_PARAM_DELETE_KINMEMBER(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 删除成员 - 返回结果
    struct DBREQ_RESULT_DELETE_KINMEMBER
    {
        DBREQ_RESULT_DELETE_KINMEMBER(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };


    //////////////////////////////////////////////////////////////////////////
    //GAMEDB_REQUEST_CENTERKINNAMECHK 中心数据库检测家族名是否重复
    //@note:检测家族名是否重复
    //传入参数结构
    struct DBREQ_PARAM_CENTERKINNAMECHK
    {
        DWORD	dwKinID;								// 家族ID
        char	szKinName[DBDEF_KINNAME_MAXSIZE];		// 家族名
        DWORD	dwPDBID;								// 角色ID

        DBREQ_PARAM_CENTERKINNAMECHK(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    //返回结构体
    struct DBREQ_RESULT_CENTERKINNAMECHK
    {
        DWORD	dwKinID;								// 家族ID 直通
        char	szKinName[DBDEF_KINNAME_MAXSIZE];		// 家族名 直通
        BYTE    nFlag; 									// 0不符合规则 1符合规则

        DBREQ_RESULT_CENTERKINNAMECHK(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    //GAMEDB_REQUEST_CENTERKINNAMEFLAG
    //@note:修改中心数据库家族名后，返回中心数据库置成败标志
    //传入参数结构
    struct DBREQ_PARAM_CENTERKINNAMEFLAG
    {
        DWORD	dwKinID;								// 家族ID
        char	szKinName[DBDEF_KINNAME_MAXSIZE];		// 家族名
        BYTE	nFlag;									// 1修改成功 0修改失败

        DBREQ_PARAM_CENTERKINNAMEFLAG(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /****************************************** 帮 会 ******************************************************/
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_READCLANLIST 读取帮会列表
    //@ note: 社会服启动时候读取

    /// 读取帮会列表	 - 传入参数结构
    struct DBREQ_PARAM_READCLANLIST
    {
        DBREQ_PARAM_READCLANLIST(void)
        {
            memset(this, 0, sizeof(DBREQ_PARAM_READCLANLIST));
        }
    };

    // 帮会数据库基本数据结构
    struct DBREQ_TABLE_CLAN
    {
        DWORD dwClanID;								// 帮会ID
        char  szName[DBDEF_CLANNAME_MAXSIZE];		// 帮会名
        DWORD nShaikhID;							// 帮主ID
        char  szShaikhName[DBDEF_ACTORNAME_MAXSIZE];// 帮主名字
        BYTE  nState;								// 帮会的状态：普通，解散中
		BYTE  nLevel;								// 帮会等级
		WORD  nFlag;								// 帮会旗帜
		char  szManifesto[DBDEF_CLAN_MANIFESTO_LEN];// 帮会的宣言
		char  szNotice[DBDEF_CLAN_NOTICE_LEN];		// 帮会的公告
        int	  nEstablishTime;						// 帮会创立时间
        int   nDismissApplyTime;					// 发起解散的时间
        int   nConstructDegree;						// 建设度
        int   nGold;								// 黄金
		int	  nLegendCupCount;						// 举办杯赛次数
        int   nHisActivity;							// 历史活跃度
        int   nActivity;							// 活跃度
		int   nWeekActivity;                        // 周活跃度，一周清一次
        int   nPlace;								// 排名
        int	  nKickTime;							// 上一次踢人的时间
        int	  nKickNum;								// 上一次踢人的次数
        DBREQ_TABLE_CLAN(void)
        {
            memset(this, 0, sizeof(DBREQ_TABLE_CLAN));
        }
    };

    /// 读取帮会列表	 - 结果返回
    struct DBREQ_RESULT_READCLANLIST
    {
        int nClanCount;								// 帮会的个数
        // 结果集：nClanCount * DBREQ_TABLE_CLAN

        DBREQ_RESULT_READCLANLIST(void)
        {
            memset(this, 0, sizeof(DBREQ_RESULT_READCLANLIST));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_CENTERDBCREATECLAN 中心数据库 创建帮会 
    /// 中心数据库 创建帮会	 - 传入参数结构
    struct DBREQ_PARAM_CENTERDBCREATECLAN
    {
        DWORD dwWorldID;							// 世界ID
        DWORD dwPDBID;								// 玩家ID，设为组长
        DWORD dwKinID;								// 家族ID 

        char  szName[DBDEF_CLANNAME_MAXSIZE];		// 帮会名
        int	  nEstablishTime;						// 帮会创立时间

        DBREQ_PARAM_CENTERDBCREATECLAN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 中心数据库 创建帮会 返回结构体 
    /// 根据返回值 判断是否创建成功 
    struct DBREQ_RESULT_CENTERDBCREATECLAN
    {
        DWORD dwClanID;								// 帮会ID
        DWORD dwPDBID;								// 玩家ID，设为组长
        DWORD dwKinID;								// 家族ID  2009.07.29
        char  szName[DBDEF_CLANNAME_MAXSIZE];		// 帮会名
        int	  nEstablishTime;						// 帮会创立时间

        DBREQ_RESULT_CENTERDBCREATECLAN(void)
        {
            memset(this, 0, sizeof(DBREQ_RESULT_CENTERDBCREATECLAN));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_CREATECLAN 游戏世界数据库 创建帮会 
    /// 游戏世界创建帮会	 - 传入参数结构
    struct DBREQ_PARAM_CREATECLAN
    {
        DWORD dwClanID;								// 帮会ID
        char  szName[DBDEF_CLANNAME_MAXSIZE];		// 帮会名		
        DWORD dwKinID;								// 家族ID
        DWORD dwPDBID;								// 帮主角色ID
        char  szActorName[DBDEF_ACTORNAME_MAXSIZE]; // 帮主角色名
        BYTE  nState;								// 帮会的状态：普通，解散中
        BYTE  nLevel;								// 帮会等级
		WORD  nFlag;								// 帮会旗帜
        char  szManifesto[DBDEF_CLAN_MANIFESTO_LEN];// 帮会的宣言
        char  szNotice[DBDEF_CLAN_NOTICE_LEN];		// 帮会的公告
        int	  nEstablishTime;						// 帮会创立时间
        int   nDismissApplyTime;					// 发起解散的时间
        int   nConstructDegree;						// 建设度
        int   nGold;								// 黄金
		int	  nLegendCupCount;						// 举办杯赛次数
        int	  nKickTime;							// 上一次踢人时间
        int	  nKickNum;								// 上一次踢人次数

        DBREQ_PARAM_CREATECLAN(void)
        {
            memset(this, 0, sizeof(DBREQ_PARAM_CREATECLAN));
        }
    };

    // 创建帮会 - 返回结果
    struct DBREQ_RESULT_CREATECLAN
    {
        DWORD dwClanID;								// 帮会ID
        char  szName[DBDEF_CLANNAME_MAXSIZE];		// 帮会名
        DWORD dwKinID;								// 家族ID   2009.07.29
        //int   nResult;								// 返回结果值：-1 帮会名称重复; 0 表示正确  -- removed by xmk 20090325

        DBREQ_RESULT_CREATECLAN(void)
        {
            memset(this, 0, sizeof(DBREQ_RESULT_CREATECLAN));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_CENTERCLANFLAG 游戏世界创建帮会后，返回中心数据库 置成败标志
    /// 中心数据库 置创建帮会成败标志:传入参数结构
    struct DBREQ_PARAM_CENTERCLANFLAG
    {	
        DWORD dwClanID;								// 帮会ID
        BYTE  byFlag;								// 标志：1游戏世界创建成功，0失败

        DBREQ_PARAM_CENTERCLANFLAG(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 中心数据库 置创建帮会成败标志 返回结构体：无，根据接口返回的标准结果1成功，-1失败判断

    //////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_CENTERCLANCHK 中心数据库 帮会成败标志检查
    /// 中心数据库 帮会成败标志检查:传入参数结构
    struct DBREQ_PARAM_CENTERCLANCHK
    {	
        DWORD    dwWorldID;									// 世界ID

        DBREQ_PARAM_CENTERCLANCHK(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };


    /// 中心数据库 帮会成败标志检查： 返回结构体
    struct DBREQ_RESULT_CENTERCLANCHK
    {	
        DWORD dwCount;								//未确认的帮会ID数量
        // dwCount * dwClanID						//未确认的帮会ID列表

        DBREQ_RESULT_CENTERCLANCHK(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_UPDATECLAN 更新帮会基本信息
    //@ note： 
    /// 更新帮会基本信息	 - 传入参数结构
    typedef DBREQ_TABLE_CLAN DBREQ_PARAM_UPDATECLAN;

    // 更新帮会基本信息 - 返回结果
    struct DBREQ_RESULT_UPDATECLAN
    {
        DBREQ_RESULT_UPDATECLAN(void)
        {
            memset(this, 0, sizeof(DBREQ_RESULT_UPDATECLAN));
        }
    };


    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_CENTERDBDISMISSCLAN  中心数据库 解散帮会
    /// 解散帮会	 - 传入参数结构 返回值为1 删除成功 其他的值 失败
    struct DBREQ_PARAM_CENTERDBDISMISSCLAN
    {
        DWORD dwClanID;								// 帮会ID

        DBREQ_PARAM_CENTERDBDISMISSCLAN(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_DISMISSCLAN 解散帮会
    //@ note 解散帮会的时候，联盟关系，称号，帮会成员都需要相应的更改，这个是应用层来提交，还是数据库处理？
    /// 解散帮会	 - 传入参数结构
    struct DBREQ_PARAM_DISMISSCLAN
    {
        DWORD dwClanID;								// 帮会ID

        DBREQ_PARAM_DISMISSCLAN(void)
        {
            memset(this, 0, sizeof(DBREQ_PARAM_DISMISSCLAN));
        }
    };

    // 解散帮会 - 返回结果
    struct DBREQ_RESULT_DISMISSCLAN
    {
        DBREQ_RESULT_DISMISSCLAN(void)
        {
            memset(this, 0, sizeof(DBREQ_RESULT_DISMISSCLAN));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_READMEMBERLIST 读取帮会成员列表
    //@ note: 帮会第一个玩家上线的时候读取

    /// 读取帮会成员列表	 - 传入参数结构
    struct DBREQ_PARAM_READMEMBERLIST
    {
        DWORD dwClanID;								// 帮会ID

        DBREQ_PARAM_READMEMBERLIST(void)
        {
            memset(this, 0, sizeof(DBREQ_PARAM_READMEMBERLIST));
        }
    };

    // 帮会成员信息结构
    struct DBREQ_TABLE_CLANMEMBER
    {
        DWORD dwClanID;								// 帮会ID 
        DWORD dwKinID;								// 家族ID 

        DBREQ_TABLE_CLANMEMBER(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 读取帮会成员列表 - 返回结果
    struct DBREQ_RESULT_READMEMBERLIST
    {
        int nMemberCount;							// 帮会成员个数
        // 结果集：nMemberCount * DBREQ_TABLE_CLANMEMBER

        DBREQ_RESULT_READMEMBERLIST(void)
        {
            memset(this, 0, sizeof(DBREQ_RESULT_READMEMBERLIST));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_UPDATEMEMBER 更新成员（没有则添加）
    //@ note： 
    /// 更新成员（没有则添加）	 - 传入参数结构
    struct DBREQ_PARAM_UPDATEMEMBER
    {
        DWORD dwClanID;								// 帮会ID 2009.07.29
        DWORD dwKinID;								// 家族ID  2009.07.29

        DBREQ_PARAM_UPDATEMEMBER(void)
        {
            memset(this, 0, sizeof(DBREQ_PARAM_UPDATEMEMBER));
        }
    };

    // 更新成员（没有则添加） - 返回结果
    struct DBREQ_RESULT_UPDATEMEMBER
    {
        DBREQ_RESULT_UPDATEMEMBER(void)
        {
            memset(this, 0, sizeof(DBREQ_RESULT_UPDATEMEMBER));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_DELETEMEMBER 删除成员
    //@ note： 
    /// 删除成员	 - 传入参数结构
    struct DBREQ_PARAM_DELETEMEMBER
    {
        DWORD dwKinID;								// 家族ID 2009.07.29

        DBREQ_PARAM_DELETEMEMBER(void)
        {
            memset(this, 0, sizeof(DBREQ_PARAM_DELETEMEMBER));
        }
    };

    // 删除成员 - 返回结果
    struct DBREQ_RESULT_DELETEMEMBER
    {
        DBREQ_RESULT_DELETEMEMBER(void)
        {
            memset(this, 0, sizeof(DBREQ_RESULT_DELETEMEMBER));
        }
    };

	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_ADDCLANDONATEINFO 增加帮会捐赠信息
	/// 增加帮会捐赠信息	 - 传入参数结构
	struct DBREQ_PARAM_ADDCLANDONATEINFO
	{
		DWORD dwPDBID;										// 角色PDBID

		DWORD dwIndex;										// 捐赠索引
		DWORD dwDonatePDBID;								// 捐赠者PDBID
		char  szDonateActorName[DBDEF_ACTORNAME_MAXSIZE];	// 角色名称
		DWORD dwCardID;										// 卡牌ID
		DWORD dwCardNum;									// 卡牌数量

		DBREQ_PARAM_ADDCLANDONATEINFO(void)
		{
			memset(this, 0, sizeof(DBREQ_PARAM_ADDCLANDONATEINFO));
		}
	};

	// 增加帮会捐赠信息 - 返回结果
	struct DBREQ_RESULT_ADDCLANDONATEINFO
	{
		DBREQ_RESULT_ADDCLANDONATEINFO(void)
		{
			memset(this, 0, sizeof(DBREQ_RESULT_ADDCLANDONATEINFO));
		}
	};
	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_REMOVECLANDONATEINFO 移除帮会捐赠信息
	/// 移除帮会捐赠信息	 - 传入参数结构
	struct DBREQ_PARAM_REMOVECLANDONATEINFO
	{
		DWORD dwPDBID;										// 角色PDBID
		DWORD dwIndex;										// 捐赠索引

		DBREQ_PARAM_REMOVECLANDONATEINFO(void)
		{
			memset(this, 0, sizeof(DBREQ_PARAM_REMOVECLANDONATEINFO));
		}
	};

	// 移除帮会捐赠信息 - 返回结果
	struct DBREQ_RESULT_REMOVECLANDONATEINFO
	{
		DBREQ_RESULT_REMOVECLANDONATEINFO(void)
		{
			memset(this, 0, sizeof(DBREQ_RESULT_REMOVECLANDONATEINFO));
		}
	};

	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_GETCLANDONATEINFO  获取帮会捐赠信息
	/// 获取帮会捐赠信息	 - 传入参数结构
	struct DBREQ_PARAM_GETCLANDONATEINFO
	{
		DWORD dwPDBID;										// 角色PDBID

		DBREQ_PARAM_GETCLANDONATEINFO(void)
		{
			memset(this, 0, sizeof(DBREQ_PARAM_GETCLANDONATEINFO));
		}
	};

	// 帮会捐赠信息结构
	struct DBREQ_TABLE_CLANDONATEINFO
	{
		DWORD dwIndex;										// 捐赠索引
		DWORD dwDonatePDBID;								// 捐赠者PDBID
		char  szDonateActorName[DBDEF_ACTORNAME_MAXSIZE];	// 角色名称
		DWORD dwCardID;										// 卡牌ID
		DWORD dwCardNum;									// 卡牌数量

		DBREQ_TABLE_CLANDONATEINFO(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 获取帮会捐赠信息 - 返回结果
	struct DBREQ_RESULT_GETCLANDONATEINFO
	{
		DWORD dwPDBID;										// 角色PDBID
		int nCount;											// 捐赠信息个数
		// 结果集：nMemberCount * DBREQ_TABLE_CLANDONATEINFO

		DBREQ_RESULT_GETCLANDONATEINFO(void)
		{
			memset(this, 0, sizeof(DBREQ_RESULT_GETCLANDONATEINFO));
		}
	};

	////////////////////////任务数据 ///////////////////////////////////
	/// 公共任务数据结构
	struct DBREQ_DATASET_UNIT
	{
		BYTE	nType;	// 拥有者类型 全局， 家族, 帮会
		DWORD	dwHolder;	// 拥有者ID
		BYTE	byIntData[DBDEF_MAX_DATASET_INTDATA_LEN];	// 整型数据区
		BYTE	byShortData[DBDEF_MAX_DATASET_SHORTDATA_LEN];	// SHORT数据区
		BYTE	byByteData[DBDEF_MAX_DATASET_BYTEDATA_LEN];	// BYTE数据区

		DBREQ_DATASET_UNIT(void)
		{
			memset(this, 0, sizeof(DBREQ_DATASET_UNIT));
		}
	};
	/// GAMEDB_REQUEST_DATASET_READ 
	/// 读取数据集合 : 传入参数结构
	
	struct DBREQ_PARAM_DATASET_READ
	{
		BYTE	nType;			// 拥有者类型 1全局 2 帮会 3 家族
		DWORD	dwHolder;		// 拥有者ID（填0的时候，所有数据对象的数据都读出来） 为0 获取全部的数据集

		DBREQ_PARAM_DATASET_READ(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	///返回参数结构
	struct DBREQ_RESULT_DATASET_READ
	{
		int	nHolderQty;								// 拥有者个数
		/// nHolderQty * DBREQ_DATASET_UNIT
		DBREQ_RESULT_DATASET_READ(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// GAMEDB_REQUEST_DATASET_SAVE 
	/// 存储公共任务数据
	/// 传入参数结构
	struct DBREQ_PARAM_DATASET_SAVE : public DBREQ_DATASET_UNIT
	{
		DBREQ_PARAM_DATASET_SAVE(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};
	/// GAMEDB_REQUEST_DATASET_DEL 
	/// 删除公共任务数据 
	/// 传入参数结构
	struct DBREQ_PARAM_DATASET_DEL
	{
		BYTE	nType;			// 拥有者类型 1全局 2 帮会 3 家族
		DWORD	dwHolder;		// 拥有者ID（填0的时候，所有数据对象的数据都读出来） 为0 获取全部的数据集

		DBREQ_PARAM_DATASET_DEL(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// 任务数据块
	struct DBREQ_HOLDER_TASKDATA
	{
		DWORD	dwHolder;								// 拥有者
														// ....... 不定长结果集参数						// 整型数据区
														// ....... 不定长结果集参数						// BYTE数据区

		DBREQ_HOLDER_TASKDATA(void)
		{
			memset(this, 0, sizeof(DBREQ_HOLDER_TASKDATA));
		}
	};
	/// GAMEDB_REQUEST_READTASKDATA 
	/// 读取任务数据
	/// 传入参数结构
	struct DBREQ_PARAM_READTASKDATA
	{
		DWORD	dwHolder;								// 拥有者（０：表示读全局任务数据）
		BYTE	nCheckExistPDBID;						// 是否需要检查PDBID是否存在(0:不需要 1:需要)

		DBREQ_PARAM_READTASKDATA(void)
		{
			memset(this, 0, sizeof(DBREQ_PARAM_READTASKDATA));
		}
	};

	struct DBREQ_RESULT_READTASKDATA
	{
		short	nHolderQty;								// 拥有者个数
														// 任务数据nHolderQty * DBREQ_HOLDER_TASKDATA

		DBREQ_RESULT_READTASKDATA(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};
	/// GAMEDB_REQUEST_SAVETASKDATA 
	/// 存储个人任务数据
	/// 传入参数结构
	struct DBREQ_PARAM_SAVETASKDATA
	{
		DWORD	dwHolder;								// 拥有者
		BYTE	nCheckExistPDBID;						// 是否需要检查PDBID是否存在(0:不需要 1:需要)
														// ....... 不定长输入参数						// 整型数据区
														// ....... 不定长输入参数						// BYTE数据区

		DBREQ_PARAM_SAVETASKDATA(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};
	/// 返回结果
	struct DBREQ_RESULT_SAVETASKDATA
	{
		DWORD	dwHolder;								// 拥有者

		DBREQ_RESULT_SAVETASKDATA(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 交互任务
	struct DBREQ_TABLE_CONTACTTASK_NODE 
	{
		LONGLONG llContactID;					// 交互ID
		BYTE	bySubType;						// 子类型
		BYTE	bySpecificType;					// 具体类型（子类型下类型）
		DWORD	dwPDBID;						// 发布者ID
		DWORD	dwUserID;						// 发布者UserID
		DWORD	dwPublishTime;					// 发布时间戳
		int		nHeadID;						// 头像ID
		BYTE	bySex;							// 性别（0男1女）
		LONGLONG llTagID;						// 标记ID（附加数据使用）
		char	szName[DBDEF_ACTORNAME_MAXSIZE];				// 发布者名字
		char	szEmotionWords[DBDEF_CONTACTTASK_CONTENT_LEN];	// 心情话语

		int		nCatchCount;					// 已捕获次数
		int		nFreeCatchCount;				// 剩余捕获数
		int		nFinishCount;					// 已完成次数
		int		nReplyCount;					// 已回复数

		DBREQ_TABLE_CONTACTTASK_NODE(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	//GAMEDB_REQUEST_READ_CONTACTTASK					
	// 读取交互任务 （社会服启动，批量读取）
	struct DBREQ_PARAM_READ_CONTACTTASK 
	{
		int  nPageSize;              			// 每页读取条数，默认为300
		int  nPageIndex;            			// 当前提取页码，默认为1，数据库根据nPageSize,nPageIndex计算返回一页数据

		DBREQ_PARAM_READ_CONTACTTASK(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	struct DBREQ_RESULT_READ_CONTACTTASK 
	{
		int nPageSize;							// 本页交互任务数量
		int nPageIndex;							// 当前页码
		int	nTotalSize;							// 交互任务总数量
		//BYTE nPageSize * DBREQ_TABLE_CONTACTTASK_NODE // Buff数据

		DBREQ_RESULT_READ_CONTACTTASK(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	//GAMEDB_REQUEST_ADD_CONTACTTASK
	// 添加交互任务
	typedef DBREQ_TABLE_CONTACTTASK_NODE DBREQ_PARAM_ADD_CONTACTTASK;

	typedef DBREQ_TABLE_CONTACTTASK_NODE DBREQ_RESULT_ADD_CONTACTTASK;

	//GAMEDB_REQUEST_UPDATE_CONTACTTASK					
	// 更新交互任务
	typedef	DBREQ_TABLE_CONTACTTASK_NODE DBREQ_PARAM_UPDATE_CONTACTTASK;

	// 交互任务回复
	struct DBREQ_TABLE_CONTACTTASK_NODE_REPLY
	{
		LONGLONG llContactID;							// 交互ID
		int		nReplyIndex;							// 回复序号
		DWORD	dwActorID;								// 回复者ID
		DWORD	dwUserID;								// 回复者UserID
		DWORD	dwReplyTime;							// 回复时间
		int		nHeadID;								// 回复者头像ID
		BYTE	bySex;									// 回复者性别（0男1女）
		char	szActorName[DBDEF_ACTORNAME_MAXSIZE];	// 回复者名字
		char	szContent[DBDEF_CONTACTTASK_CONTENT_LEN];// 回复内容

		DBREQ_TABLE_CONTACTTASK_NODE_REPLY(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	//GAMEDB_REQUEST_READ_CONTACTTASK_REPLY				
	// 读取交互任务回复
	struct DBREQ_PARAM_READ_CONTACTTASK_REPLY 
	{            		
		int		nPageSize;              		// 每页读取条数，默认为100
		int		nPageIndex;            			// 当前提取页码，默认为1，数据库根据nPageSize,nPageIndex计算返回一页数据
		int		nTaskCount;						// 待读取回复关联的交互任务数
		//BYTE  nTaskCount * llContactID		// 读取的交互任务ID，返回所有关联回复

		DBREQ_PARAM_READ_CONTACTTASK_REPLY(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	struct DBREQ_RESULT_READ_CONTACTTASK_REPLY 
	{
		int		nPageSize;						// 本页交互任务数量
		int		nPageIndex;						// 当前页码
		int		nTotalSize;						// 交互任务总数量
		int		nTaskCount;						// 待读取回复关联的交互任务数
		//BYTE  nTaskCount * llContactID		// 读取的交互任务ID，返回所有关联回复
		//BYTE	nPageSize * DBREQ_TABLE_CONTACTTASK_NODE_REPLY // Buff数据

		DBREQ_RESULT_READ_CONTACTTASK_REPLY(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	//GAMEDB_REQUEST_SAVE_CONTACTTASK_REPLY				
	// 存储交互任务回复
	struct DBREQ_PARAM_SAVE_CONTACTTASK_REPLY 
	{
		int nSaveCount;							// 存储数量
		//BYTE nSaveCount * DBREQ_TABLE_CONTACTTASK_NODE_REPLY	// Buff数据

		DBREQ_PARAM_SAVE_CONTACTTASK_REPLY(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	//GAMEDB_REQUEST_DEL_CONTACTTASK					
	// 删除交互任务，批量删除相关任务+相关回复
	struct DBREQ_PARAM_DEL_CONTACTTASK 
	{
		int nDelCount;							// 删除数量
		//BYTE nDelCount * llContactID			// Buff数据

		DBREQ_PARAM_DEL_CONTACTTASK(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	//GAMEDB_REQUEST_READ_ACTOR_CONTACTTASK
	// 读取玩家交互任务
	struct DBREQ_PARAM_READ_ACTOR_CONTACTTASK 
	{
		DWORD	dwActorID;						// 玩家PDBID
	
		DBREQ_PARAM_READ_ACTOR_CONTACTTASK(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	struct DBREQ_RESULT_READ_ACTOR_CONTACTTASK
	{
		DWORD	dwActorID;						// 玩家PDBID
		int		nCount;							// 发布的交互任务个数
		//buff	nCount * llContactID			// 具体发布的交互任务ID

		DBREQ_RESULT_READ_ACTOR_CONTACTTASK(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /****************************************** OSS 相关接口 ******************************************************/
    /// GAMEDB_REQUEST_MONITOR_ACTORVALUES 监控 玩家数值
    // 玩家数值 BUFF 结构体
    struct DBREQ_OSS_MONITOR_ACTORVALUE
    {
        DWORD	dwObjectID;		                        // 类型ID（角色ID、帮会ID）
        BYTE    nObjType;                               // 类型（OSS_OPT_TYPE）
        DWORD	dwReserveID;	                        // 保留ID(任务的奖励的时候填写任务ID，交易的时候 填写对方玩家的ID等等)
        BYTE	nValueType;		                        // 数值类型 OSS_NUMERIC_TYPE(1金钱、2绑金, 3点卷) 
        int	    nNum;			                        // 数值数量
        BYTE	nOpType;		                        // 操作类型 OSS_RESOURCE_OPT_TYPE(消耗、产生、转进、转出)
        WORD  	wApproach;		                        // 途径
        char	szDesc[DBDEF_OSS_VALUE_DESC_LEN];	    // 描述字符串
		DWORD	dwReserveTime;	                        // 保留时间
        DWORD	dwReserveID2;	                        // 保留ID2(战场存匹配分升降时填写战场类型）
				
        DBREQ_OSS_MONITOR_ACTORVALUE(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    // 
    struct DBREQ_PARAM_OSS_MONITOR_ACTORVALUES
    {
        int Num;                                        // 保存 记录数量
        //BYTE BuffData[7800];                          // 保存数据的buff DBREQ_OSS_MONITOR_ACTORVALUE * Num

        DBREQ_PARAM_OSS_MONITOR_ACTORVALUES(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    
    /// GAMEDB_REQUEST_MONITOR_ACTORTICKET   监控 点券的详细消耗日志(商城购买也计入)
    struct DBREQ_OSS_MONITOR_ACTORTICKET_ITEM //记录长度：129
    {
        int nActorID;                                   // 角色ID
        WORD wActorLevel;                               // 角色等级
        int nUserID;                                    // 用户ID
        int nItemID;                                    // 商品ID或奖励ID
        char szGoodsName[DBDEF_MAX_GAMENAME_MAXSIZE];   // 商品名称或奖励名称
        int nItemNum;                                   // 数量
        int nAgio;                                      // 折扣率
        int nPrice;                                     // 单价
        int nCostTicket;                                // 花费点卷
        BYTE byTicketType;                              // 金币类型
        char szRemark[32];                              // 备注
        char szIP[DBDEF_CLIENT_IP_MAXSIZE];             // IP地址
        BYTE byUserType;                                // 用户类型  1为被推荐用户 
        int nBuyTime;                                   // 购买时间
		
        DBREQ_OSS_MONITOR_ACTORTICKET_ITEM(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    //  监控 点券的详细消耗日志  传入结构体
    struct DBREQ_OSS_MONITOR_ACTORTICKET
    {
        int Num;                                    // 保存 记录数量
        //BYTE byBuffData[7800];                    // 保存数据 DBREQ_OSS_MONITOR_ACTORTICKET_ITEM * Num

        DBREQ_OSS_MONITOR_ACTORTICKET(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // GAMEDB_REQUEST_LOG_NOTE 文字型日志 ：传入参数结构
    struct DBREQ_PARAM_LOG_NOTE
    {
        WORD	wNoteType;							// 文字日志类型 OSS_OPT_TYPE
        DWORD	dwIndex;							// 索引ID(索引有人物系统采用角色PDBID，国家系统采用国家ID，帮会系统采用帮会ID)
        char	szNote[DBDEF_OSS_NOTE_DESC_LEN];	// 文字描述

        DBREQ_PARAM_LOG_NOTE(void)
        {
            memset(this, 0, sizeof(DBREQ_PARAM_LOG_NOTE));
        };
    };

    /// GAMEDB_REQUEST_ACTOR_GOODS_LOG 玩家物品日记
    // 玩家物品日志 BUFF 结构体
    struct DBREQ_OSS_ACTOR_GOODS_LOG
    {
        long	lFrom;										// 物品原主Id  或者怪物的ID,任务Npc,Id
        BYTE	lFromType;									// 物品原主类型（OSS_OPT_TYPE）
        long	lTo;										// 物品新主Id
        BYTE	lToType;									// 物品新主类型（OSS_OPT_TYPE）

        BYTE	nLogType;		                            // 类型 OSS_GOODS_TYPE
        BYTE	nOpType;		                            // 操作类型 OSS_RESOURCE_OPT_TYPE(消耗、产生、转进、转出)    
        WORD  	wApproach;		                            // 途径 OSS_RESOURCE_OPT_TYPE各项对应细项
        
        int	    nTypeID;			                        // 类型id
        int     nNum;                                       // 数量
        DWORD	dwReserveID1;	                            // 保留ID1
        DWORD	dwReserveID2;	                            // 保留ID2

        DWORD   dwOptTime;                                  // 发生时间
        char	szDesc[DBDEF_OSS_ACTOR_GOODS_DESC_LEN];	    // 描述字符串

        DBREQ_OSS_ACTOR_GOODS_LOG(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    // 
    struct DBREQ_PARAM_OSS_ACTOR_GOODS_LOGS
    {
        int Num;                                        // 保存 记录数量
        //BYTE BuffData[7800];                          // 保存数据的buff DBREQ_OSS_ACTOR_GOODS_LOG * Num

        DBREQ_PARAM_OSS_ACTOR_GOODS_LOGS(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    //  GAMEDB_REQUEST_ACTOR_UPGRADE_LEVEL_LOG         玩家升级日记
    struct DBREQ_OSS_ACTOR_UPGRADE_LEVEL_LOG
    {
		DWORD	dwWorldID;									// 世界ID
        DWORD   dwUserID;                                   // 账号ID
        DWORD   dwActorID;                                  // 角色ID
        BYTE    nUpgradeType;                               // 升级类型（1=等级，2=段位，3=英雄评级）
        DWORD   dwValue;                                    // 数值
        DWORD   dwOptTime;                                  // 发生时间
        DWORD	dwReserveID;	                            // 保留（英雄评级时为英雄ID）

        DBREQ_OSS_ACTOR_UPGRADE_LEVEL_LOG(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    struct DBREQ_PARAM_OSS_ACTOR_UPGRADE_LEVEL_LOGS
    {
        int Num;                                        // 保存 记录数量
        //BYTE BuffData[7800];                          // 保存数据的buff DBREQ_OSS_ACTOR_UPGRADE_LEVEL_LOG * Num

        DBREQ_PARAM_OSS_ACTOR_UPGRADE_LEVEL_LOGS(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

	//  GAMEDB_REQUEST_ACTOR_GOOD_FROGE_LOG         玩家打造日记
	struct DBREQ_OSS_ACTOR_GOOD_FROGE_LOG
	{
		DWORD   dwUserID;                                   // 账号ID
		DWORD   dwActorID;                                  // 角色ID
		DWORD	dwGoodID;									// 物品id
		char	szGoodsName[DBDEF_MAX_GAMENAME_MAXSIZE];	// 物品名称
		BYTE	nOperateType;								// 操作类别
		char	szDesc[DBDEF_OSS_ACTOR_FORGE_DESC_LEN];	    // 打造描述
		DWORD   dwOptTime;                                  // 发生时间
		DWORD	dwReserveID;	                            // 保留

		DBREQ_OSS_ACTOR_GOOD_FROGE_LOG(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	struct DBREQ_PARAM_OSS_ACTOR_GOOD_FROGE_LOGS
	{
		int Num;                                        // 保存 记录数量
		//BYTE BuffData[7800];                          // 保存数据的buff DBREQ_OSS_ACTOR_GOOD_FROGE_LOG * Num

		DBREQ_PARAM_OSS_ACTOR_GOOD_FROGE_LOGS(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

    //GAMEDB_REQUEST_ACTOR_GUIDESTEP_LOG         
	//@NOTE 玩家引导步骤日记
	//传入结构
    struct DBREQ_OSS_ACTOR_GUIDESTEP_LOG
    {
        DWORD   dwUserID;                                   // 账号ID
        DWORD   dwActorID;                                  // 角色ID
        int	    nStep;									    // 步骤

        DBREQ_OSS_ACTOR_GUIDESTEP_LOG(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

	//GAMEDB_REQUEST_MONITOR_TASK         
	//@NOTE 监控 任务数据
	//传入结构
	struct DBREQ_OSS_MONITOR_TASK
	{
		int nTaskID;										// 任务ID
		int nAcceptNum;										// 接受次数
		int nFinishNum;										// 完成次数
		int nGiveUpNum;										// 放弃次数

		DBREQ_OSS_MONITOR_TASK(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	struct DBREQ_PARAM_OSS_MONITOR_TASK
	{
		int nServerID;										// 服务器ID
		char szTime[DBDEF_OSS_MONITOR_TASK_TIME_LEN];		// 监控 任务数据时间 200904081010
		int nNum;											// 记录数量
		//BYTE BuffData[7800];								// 保存数据的buff DBREQ_OSS_MONITOR_TASK * nNum

		DBREQ_PARAM_OSS_MONITOR_TASK(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};
    //////////////////////////////////////////点券////////////////////////////////////////////////
	/// GAMEDB_REQUEST_GAMETICKET_ADDMINE 
    /// @NOTE 游戏世界库用户点券操作 
    /// 传入结构
	struct DBREQ_PARAM_GAMETICKET_ADDMINE
	{
		DWORD	dwWorldID;									/// 游戏世界ID	
		DWORD	dwUserID;									/// 帐号ID	
		DWORD	dwActorID;									/// 角色ID
        int		nAddTick;									/// 点券：正数为加，负数为减
		int		nReasonID;									/// 点券 数值日志途径ID:0为充值 1.......
        int     nAddBindTick;                               /// 金币：正数为加，负数为减 
		int		nBindReasonID;								/// 金币 数值日志途径ID:0为充值 1.......
		int     nSubclassID;								///	子类ID，购买物品则填物品ID，没有子类默认0
		short   nNum;										/// 个数，子类是物品ID则填物品个数

		DBREQ_PARAM_GAMETICKET_ADDMINE(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// 返回结构
	struct DBREQ_RESULT_GAMETICKET_ADDMINE
	{
		DWORD	dwUserID;									/// 帐号ID
		DWORD	dwActorID;									/// 角色ID        
		int		nTotalTicket;								/// 当前总点券
        int		nTotalBindTicket;							/// 当前总金币

		DBREQ_RESULT_GAMETICKET_ADDMINE(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};
    ///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_PAYUSERTICKET_GET 
	/// @note 充值中心库读取待提取账号列表 
    /// 传入结构
	struct DBREQ_PARAM_PAYUSERTICKET_GET
	{
		DWORD   dwWorldID;				// 游戏世界ID

        DBREQ_PARAM_PAYUSERTICKET_GET(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

    // 待提取账号列表信息 
	struct DBREQ_PAYUSERTICKET_INFO
	{   
        LONGLONG    nOrderID;       // 订单ID
		DWORD       dwUserID;	    // 玩家账号ID
		int         nTicket;		// 点券

		DBREQ_PAYUSERTICKET_INFO(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 返回结构
	struct DBREQ_RESULT_PAYUSERTICKET_GET
	{
        int nReCount;           // 返回记录数
        // nReCount * DBREQ_PAYUSERTICKET_INFO

		DBREQ_RESULT_PAYUSERTICKET_GET(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// GAMEDB_REQUEST_PAYTICKET_REDUCE 
	/// @NOTE 充值中心库请求领取充值点券 
    /// 传入结构

	struct DBREQ_PARAM_PAYTICKET_REDUCE
	{
		LONGLONG    nOrderID;       // 订单ID
        int	        nWorldID;		// 游戏世界ID
		DWORD	    dwUserID;		// 玩家通行证I
		int		    nTicket;		// 领取点卷数量

		DBREQ_PARAM_PAYTICKET_REDUCE(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// 返回结构
	struct DBREQ_RESULT_PAYTICKET_REDUCE
	{
		DWORD	dwUserID;		// 玩家通行证ID(直通)
		int     nTicket;		// 领取点卷数量(直通)			
		int		nLogID;			// 领取明细记录ID，用于世界库操作成功后，回头确认
	
		DBREQ_RESULT_PAYTICKET_REDUCE(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_GAMETICKET_CHARGE
	/// @NOTE 游戏世界库增加领取点券
    /// 传入结构

	struct DBREQ_PARAM_GAMETICKET_CHARGE
	{
		DWORD	dwWorldID;			/// 世界ID
        DWORD	dwUserID;			/// 帐号ID
		int		nTicket;			/// 提取点券
		int		nLogID;				/// 提取明细记录ID，用于世界库操作成功后，回头确认

		DBREQ_PARAM_GAMETICKET_CHARGE(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// 返回结构
	struct DBREQ_RESULT_GAMETICKET_CHARGE
	{
		DWORD	dwUserID;			/// 帐号ID
		int		nTicket;			/// 提取点券
		int		nTotalTicket;		/// 当前游戏库总点券
        int		nLogID;				/// 扣除明细记录ID，用于世界库操作成功后，回头确认		

		DBREQ_RESULT_GAMETICKET_CHARGE(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_PAYTICKETCHARGE_CONFIRM
	/// @note 充值中心库领取点券确认 
    /// 传入结构

	struct DBREQ_PARAM_PAYTICKETCHARGE_CONFIRM
	{
		int		nLogID;										// 扣除明细记录ID

		DBREQ_PARAM_PAYTICKETCHARGE_CONFIRM(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/////////////////////////////////英雄附加信息--通用接口/////////////////////////////////////////
	//GAMEDB_REQUEST_SAVE_HEROADDITIONAL
	//@NOTE  保存英雄附加信息 
	//传入结构
	struct DBREQ_PARAM_SAVE_HEROADDITIONAL
	{
		DWORD   dwActorID;                  // 角色id
		BYTE    nType;						// 类型ID 1卡牌 2天赋 3皮肤
		DWORD   dwHeroID;                   // 英雄ID
		//BYTE  byBuffData[];				// 不定长
		DBREQ_PARAM_SAVE_HEROADDITIONAL()
		{
			memset(this, 0, sizeof(*this));
		}
	};
	// 返回结构 无

	//GAMEDB_REQUEST_GET_HEROADDITIONAL
	//@NOTE  查询英雄附加信息 
	//
	struct DBREQ_PARAM_HEROADDITIONAL
	{
		DWORD   dwHeroID;                   // 英雄ID
		//BYTE  byBuffData[];				// 不定长
		DBREQ_PARAM_HEROADDITIONAL()
		{
			memset(this, 0, sizeof(*this));
		}
	};
	//传入结构
	struct DBREQ_PARAM_GET_HEROADDITIONAL
	{
		DWORD dwActorID;					// 角色ID
		BYTE  nType;						// 类型ID 1卡牌 2天赋 3皮肤
		DBREQ_PARAM_GET_HEROADDITIONAL()
		{
			memset(this, 0, sizeof(*this));
		}
	};
	//返回结构
	struct DBREQ_RESULT_GET_HEROADDITIONAL
	{
		DWORD dwActorID;					// 直通 角色ID
		BYTE  nType;						// 直通 类型ID 1卡牌 2天赋 3皮肤
		int   nCount;
		// nCount * DBREQ_PARAM_HEROADDITIONAL
		DBREQ_RESULT_GET_HEROADDITIONAL()
		{
			memset(this, 0, sizeof(*this));
		}
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
    /***************************************   防沉迷  **********************************************/
    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_READ_ADDICTINFO 读取玩家的防沉迷信息
    //@ note:
    /// 读取玩家的防沉迷信息	 - 传入参数结构
    struct DBREQ_PARAM_READ_ADDICTINFO
    {
        DWORD dwIdCard;

        DBREQ_PARAM_READ_ADDICTINFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    // 读取玩家的防沉迷信息 - 返回结果
    struct DBREQ_RESULT_READ_ADDICTINFO
    {
        DWORD dwIdCard;
        int   nILoginTime;						// 玩家身份证上线时间
        int   nILogoutTime;						// 玩家身份证下线时间
        int   nPreOnlineTime;					// 玩家历史累计在线时间
        int   nPreOffLineTime;					// 玩家历史累计不在线时间

        DBREQ_RESULT_READ_ADDICTINFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_WRITE_ADDICTINFO 写入玩家的防沉迷信息（批量写入）
    //@ note: 因为防沉迷的数据基本存在内存，只有在某些时候（比如深夜）才存入数据库，所以一次性写入n个玩家的数据，减少调用次数
    /// 防沉迷信息结构
    struct DBREQ_TABLE_ADDICTINFO
    {
        DWORD dwIdCard;
        int   nILoginTime;						// 玩家身份证上线时间
        int   nILogoutTime;						// 玩家身份证下线时间
        int   nPreOnlineTime;					// 玩家历史累计在线时间
        int   nPreOffLineTime;					// 玩家历史累计不在线时间
        DBREQ_TABLE_ADDICTINFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    /// 写入玩家的防沉迷信息（批量写入）	 - 传入参数结构
    struct DBREQ_PARAM_WRITE_ADDICTINFO
    {
        int  nUserCount;							// 写入玩家的个数
		// nUserCount * DBREQ_TABLE_ADDICTINFO     不定长

        DBREQ_PARAM_WRITE_ADDICTINFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 写入玩家的防沉迷信息（批量写入） - 返回结果
	// 无
	
    /// GAMEDB_REQUEST_WRITE_AUTHENT  增加玩家实名认证信息
    /// @NOTE: 在游戏客户端增加玩家实名认证信息 减少玩家退出游戏后去官网增加
    struct DBREQ_PARAM_WRITE_AUTHENT
    {
        int   nUserID;                              // 用户ID
        char szRealName[DBDEF_USERNAME_MAXSIZE];    // 真实姓名
        char szIDCardNo[DBDEF_IDCARD_MAXSIZE];      // 身份证号码
        int   nCreateIP;                            // IP地址

        DBREQ_PARAM_WRITE_AUTHENT(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

	// 玩家客户端实名认证-返回结果
	struct DBREQ_RESULT_WRITE_AUTHENT 
	{
		bool bIsAdult;								// 是否成年 
		DWORD dwNewIdCard;							// 新认证身份证

		DBREQ_RESULT_WRITE_AUTHENT(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

    ////////////////////////////// 喇叭系统 ////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 操作表名: ChatSpeaker

	// 喇叭系统信息数据库表: 表名 ChatSpeaker
	struct DB_Table_Speaker
	{
		DWORD dwID;							// ID，0为无效ID,数据库设为自增
		DWORD dwDataID;						// 数据ID，0为无效ID,程序控制的自增ID,用来查找
		DWORD dwPlayerID;					// 发送者Dbid
		DWORD dwLoudType;					// 喇叭类型
		char  szName[DBDEF_ACTORNAME_MAXSIZE];// 发送角色名字
		char  szText[512];					// 发送的文本
		DWORD dwEffectType;					// 背景光效类型
		DWORD dwTime;						// 发送者发生时间
		DWORD dwFlag;						// 标识,1:新的,0:已发
		DWORD dwSpare1;						// 备用1
		DWORD dwSpare2;						// 备用2
		DWORD dwSpare3;						// 备用3

		DB_Table_Speaker(void)
		{
			memset(this, 0, sizeof(DB_Table_Speaker));
		}

	};

	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_CHATSPEAKER_READ 查询喇叭 : 传入参数结构
	struct DB_Param_Read_Speaker
	{
		// 条件参数,返回以DB_Table_Speaker.dwID 从小到大排序
		DB_Param_Read_Speaker(void)
		{
			memset(this, 0, sizeof(DB_Param_Read_Speaker));
		}
	};

	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_CHATSPEAKER_READ 查询喇叭 : 返回结果，返回标识dwFlag为0的所有记录
	struct DB_Result_Read_Speaker
	{
		int nRecordCount;					// 返回记录的个数，无论超时还是不超时都一起返回，超时的记录nRecordCount = 0

		// 结果集：nRecordCount * DB_Table_Speaker
		DB_Result_Read_Speaker(void)
		{
			memset(this, 0, sizeof(DB_Result_Read_Speaker));
		}
	};

	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_CHATSPEAKER_ADD 添加喇叭 : 传入参数结构
	struct DB_Param_Add_Speaker
	{
		// 数据记录以　dwPlayerID 和 dwTime 和　dwDataID组合唯一标识
		// 新增时以以上标识查找记录，如果存在该记录则更新，除dwFlag之外的字段
		// 如果不存在，新增
		// 条件参数
		DWORD dwDataID;						// 数据ID，0为无效ID,程序控制的自增ID,用来查找
		DWORD dwPlayerID;					// 发送者Dbid
		DWORD dwLoudType;					// 喇叭类型
		char  szName[DBDEF_ACTORNAME_MAXSIZE];// 发送角色名字
		char  szText[512];					// 发送的文本
		DWORD dwEffectType;					// 背景光效类型
		DWORD dwTime;						// 发送者发送时间
		DWORD dwFlag;						// 标识,0:新的,1:已发
		DWORD dwSpare1;						// 发送者排行榜隐藏设置信息
		DWORD dwSpare2;						// 备用2
		DWORD dwSpare3;						// 备用3

		DB_Param_Add_Speaker(void)
		{
			memset(this, 0, sizeof(DB_Param_Add_Speaker));
		}
	};

	/// 添加喇叭: 返回结果
	struct DB_Result_Add_Speaker
	{
		BYTE	nResult;						// 结果 0:出错, 1:新增成功

		DB_Result_Add_Speaker(void)
		{
			memset(this, 0, sizeof(DB_Result_Add_Speaker));
		}
	};

	///////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_CHATSPEAKER_EDIT 更新喇叭数据 : 传入参数结构
	struct DB_Param_Edit_Speaker
	{
		// 修改记录以　dwPlayerID 和 dwTime 和　dwDataID组合唯一标识
		// 修改时以以上标识查找记录，如果存在该记录则更新dwFlag字段
		// 如果不存在，则新增记录包括以下字段，dwDataID、dwPlayerID、dwTime、dwFlag
		// 条件参数
		DWORD dwDataID;						// 数据ID，0为无效ID,程序控制的自增ID,用来查找
		DWORD dwPlayerID;					// 发送者Dbid
		DWORD dwTime;						// 发送者发送时间

		// 存入的数据
		DWORD dwFlag;						// 标识,1:新的,0:已发


		DB_Param_Edit_Speaker(void)
		{
			memset(this, 0, sizeof(DB_Param_Edit_Speaker));
		}
	};

	/// 更新喇叭数据: 返回结果
	struct DB_Result_Edit_Speaker
	{
		BYTE	nResult;						// 结果 0:出错,没有修改; 1:修改成功 

		DB_Result_Edit_Speaker(void)
		{
			memset(this, 0, sizeof(DB_Result_Edit_Speaker));
		}
	};
	/////////////////////////////////增值服务////////////////////////////////////////
	// GAMEDB_REQUEST_ADDSERVICES_USER_CHK	
	// 验证是否符合规则 游戏库
	// 传入参数

	struct DBREQ_PARAM_ADDSERVICES_USER_CHK	
	{
		DWORD   dwWorldID;			//游戏世界ID
		DWORD   dwUserID;           //账号ID
		DWORD   dwActorID;          //角色ID
		BYTE    nType;				//增值服务类型（1变性别 2改角色名 ）
		BYTE	nMoneyType;			//扣钱类型（0点券 1金钱）
		char    szCurValues[32];	//当前数据
		char    szNewValues[32];    //要修改数据

		DBREQ_PARAM_ADDSERVICES_USER_CHK(void)
		{
			memset(this,0,sizeof(*this));
		}
	};

	// 返回结构
	struct DBREQ_RESULT_ADDSERVICES_USER_CHK
	{
		DWORD   dwUserID;           //账号ID 直通
		DWORD   dwActorID;          //角色ID 直通	
		BYTE    nType;				//增值服务类型（1变性别 2改角色名 ） 直通
		BYTE	nMoneyType;			//扣钱类型（0点券 1金钱）
		char    szCurValues[32];	//当前数据 直通
		char    szNewValues[32];    //要修改数据 直通
		BYTE    nFlags;				//校验结果 0不符合规则 1符合规则

		DBREQ_RESULT_ADDSERVICES_USER_CHK(void)
		{
			memset(this,0,sizeof(*this));
		}
	};

	// GAMEDB_REQUEST_UPDATECENTERDBCREATEACTOR	
	// 更新创建角色 (中心库) 
	// 传入参数
	struct DBREQ_PARAM_UPDATECENTERDBCREATEACTOR	
	{
		DWORD   dwWorldID;			//游戏世界ID
		DWORD   dwUserID;           //账号ID
		DWORD   dwActorID;          //角色ID
		BYTE    nType;				//增值服务类型（1变性别 2改角色名 ） 
		BYTE    nMoneyType;			//扣钱类型（0点券 1金钱）
		int		nMoney;				//扣钱数
		char    szCurValues[32];	//当前数据
		char    szNewValues[32];    //要修改数据
	
		DBREQ_PARAM_UPDATECENTERDBCREATEACTOR(void)
		{
			memset(this,0,sizeof(*this));
		}
	};

    ////////////////////////////// 增益卡功能 ////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 操作表名: BenefitCard

    // 增益卡系统信息数据库表：表名 BenefitCard
    // 数据记录以dwPlayerID、nBenefitCardType、nBenefitCardSubType组合唯一标识
    struct DB_Table_BenefitCard
    {
        DWORD dwPlayerID;            // 玩家pdbID
        BYTE   nBenefitCardType;      // 增益卡类型（1：金币增益卡 2：角色经验增益卡 3：英雄经验增益卡）
        BYTE   nBenefitCardSubType;   // 增益卡使用范围类型（1：时间天数 2：次数，一局消耗一次）
        DWORD nBenefitValue;         // 当nBenefitCardSubType的值为1时，该值是一个Unix时间戳，表示过期时间点；为2时，表示剩余使用次数
        int   nMultiple;			 // 增益卡的增益倍数

        DB_Table_BenefitCard(void)
        {
            memset(this, 0, sizeof(DB_Table_BenefitCard));
        }
    };

    //////////////////////////////////////////////////
    // GAMEDB_REQUEST_BENEFITCARD_READ 查询增益卡：传入参数结构，查询玩家dwPlayerID的所有增益卡记录
    struct DB_Param_Read_BenefitCard
    {
        DWORD dwPlayerID;			// 玩家pdbID	

        DB_Param_Read_BenefitCard(void)
        {
            memset(this, 0, sizeof(DB_Param_Read_BenefitCard));
        }
    };

    //////////////////////////////////////////////////
    // GAMEDB_REQUEST_BENEFITCARD_READ 查询增益卡：返回结果结构，返回某位玩家的所有记录
    struct DB_Result_Read_BenefitCard
    {
        int nRecordCount;

        // 结果集：nRecordCount * DB_Table_BenefitCard
        DB_Result_Read_BenefitCard(void)
        {
            memset(this, 0, sizeof(DB_Result_Read_BenefitCard));
        }
    };

    /////////////////////////////////////////////////////
    // GAMEDB_REQUEST_BENEFITCARD_ADD 添加增益卡：传入参数结构
    struct DB_Param_Add_BenefitCard
    {
        // 数据记录以dwPlayerID、nBenefitCardType、nBenefitCardSubType组合唯一标识
        DWORD dwPlayerID;            // 玩家pdbID
        BYTE   nBenefitCardType;      // 增益卡类型（1：金币增益卡 2：角色经验增益卡 3：英雄经验增益卡）
        BYTE   nBenefitCardSubType;   // 增益卡使用范围类型（1：时间天数 2：次数，一局消耗一次）
        DWORD nBenefitValue;         // 当nBenefitCardSubType的值为1时，该值是一个Unix时间戳，表示过期时间点；为2时，表示剩余使用次数
        int   nMultiple;			 // 增益卡的增益倍数

        DB_Param_Add_BenefitCard()
        {
            memset(this, 0, sizeof(DB_Param_Add_BenefitCard));
        }
    };

    /////////////////////////////////////////////////////
    // GAMEDB_REQUEST_BENEFITCARD_ADD 添加增益卡：返回结果结构
    struct DB_Result_Add_BenefitCard
    {
        DB_Result_Add_BenefitCard()
        {
            memset(this, 0, sizeof(DB_Result_Add_BenefitCard));
        }
    };

    /////////////////////////////////////////////////////
    // GAMEDB_REQUEST_BENEFITCARD_UPDATE 更新增益卡：传入参数结构
    struct DB_Param_Update_BenefitCard
    {
        // 数据记录以dwPlayerID、nBenefitCardType、nBenefitCardSubType组合唯一标识
        DWORD dwPlayerID;            // 玩家pdbID
        BYTE   nBenefitCardType;      // 增益卡类型（1：金币增益卡 2：角色经验增益卡 3：英雄经验增益卡）
        BYTE   nBenefitCardSubType;   // 增益卡使用范围类型（1：时间天数 2：次数，一局消耗一次）
        DWORD nBenefitValue;         // 当nBenefitCardSubType的值为1时，该值是一个Unix时间戳，表示过期时间点；为2时，表示剩余使用次数
        int   nMultiple;			 // 增益卡的增益倍数

        DB_Param_Update_BenefitCard()
        {
            memset(this, 0, sizeof(DB_Param_Update_BenefitCard));
        }
    };

    /////////////////////////////////////////////////////
    // GAMEDB_REQUEST_BENEFITCARD_UPDATE 添加增益卡：返回结果结构
    struct DB_Result_Update_BenefitCard
    {
        DB_Result_Update_BenefitCard()
        {
            memset(this, 0, sizeof(DB_Result_Update_BenefitCard));
        }
    };
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /****************************************** 师徒 ******************************************************/
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_READ_MENTORSHIP 读取师徒关系列表 （玩家上线时读取）
    /// 读取师徒关系列表	 - 传入参数结构
    struct DBREQ_PARAM_READMENTORSHIPLIST
    {
        DWORD dwPDBID;							// 玩家ID
    
        DBREQ_PARAM_READMENTORSHIPLIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    
    // 师徒关系数据库数据结构
    struct DBREQ_TABLE_MENTORSHIP
    {
        DWORD dwMasterPDBID;							// 师傅玩家ID
        char  szMasterName[DBDEF_ACTORNAME_MAXSIZE];	// 师傅玩家名
        DWORD dwPrenticePDBID;							// 学徒玩家ID
        char  szPrenticeName[DBDEF_ACTORNAME_MAXSIZE];	// 学徒玩家名
        short nPrenticeFaceID;							// 学徒头像ID
        BYTE  nRelationType;							// 关系类型
        BYTE  nState;								    // 关系状态
        BYTE  nPromoteFlag;								// 关系提升标识
        DWORD dwBuildTime;								// 建立时间
        DWORD dwSeverPDBID;								// 解除者玩家ID
        DWORD dwSeverTime;								// 解除时间
    
        DBREQ_TABLE_MENTORSHIP(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    
    /// 读取师徒关系列表	 - 结果返回(读取自己的学徒和导师以及自己导师下的学徒)
    struct DBREQ_RESULT_READMENTORSHIPLIST
    {
        int nCurCount;								// 当次返回的个数
    
        // 结果集：nCurCount * DBREQ_TABLE_MENTORSHIP
        DBREQ_RESULT_READMENTORSHIPLIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    
    /// 添加师徒关系	 - 传入参数结构
    struct DBREQ_PARAM_ADDMENTORSHIP
    {
        DWORD dwMasterPDBID;							// 师傅玩家ID
        char  szMasterName[DBDEF_ACTORNAME_MAXSIZE];	// 师傅玩家名
        DWORD dwPrenticePDBID;							// 学徒玩家ID
        char  szPrenticeName[DBDEF_ACTORNAME_MAXSIZE];	// 学徒玩家名
        short nPrenticeFaceID;							// 学徒头像ID
        BYTE  nRelationType;							// 关系类型
        BYTE  nState;								    // 关系状态
        BYTE  nPromoteFlag;								// 提升标识
        DWORD dwBuildTime;								// 建立时间
    
        DBREQ_PARAM_ADDMENTORSHIP(void)
        {
            memset(this, 0, sizeof(this));
        }
    };
    
    /// 更新师徒关系数据	 - 传入参数结构
    struct DBREQ_PARAM_UPDATEMENTORSHIP
    {
        DWORD dwMasterPDBID;							// 师傅玩家ID
		char  szMasterName[DBDEF_ACTORNAME_MAXSIZE];	// 师傅玩家名
        DWORD dwPrenticePDBID;							// 学徒玩家ID
		char  szPrenticeName[DBDEF_ACTORNAME_MAXSIZE];	// 学徒玩家名
        BYTE  nRelationType;							// 关系类型
        BYTE  nState;									// 关系状态
        BYTE  nPromoteFlag;								// 关系提升标识
        DWORD dwSeverPDBID;								// 解除者玩家ID
        DWORD dwSeverTime;								// 解除时间
    
        DBREQ_PARAM_UPDATEMENTORSHIP(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    
    /// 删除师徒关系数据	 - 传入参数结构
    struct DBREQ_PARAM_DELMENTORSHIP
    {
        DWORD dwMasterPDBID;					// 师傅玩家ID
        DWORD dwPrenticePDBID;					// 学徒玩家ID
    
        DBREQ_PARAM_DELMENTORSHIP(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    
    /// 读取师徒玩家个人数据	 - 传入参数结构
    struct DBREQ_PARAM_READMENTORSHIPPERSON
    {
        DWORD dwPDBID;
    
        DBREQ_PARAM_READMENTORSHIPPERSON(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    
    // 读取师徒玩家个人数据 - 返回结果
    struct DBREQ_RESULT_READMENTORSHIPPERSON
    {
        DWORD dwPDBID;								// 玩家ID
        BYTE  nAutoIssueMFlag;						// 自动发布师傅信息标识
        BYTE  nAutoIssuePFlag;						// 自动发布学徒信息标识
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
        BYTE  nMFWinAwardPreCount;					// 导师首胜奖励可领取次数
        BYTE  nMFWinAwardCount;						// 导师首胜奖励次数
        int   nMFWinAwardTime;						// 导师首胜奖励时间
        BYTE  nPFWinAwardPreCount;					// 学徒首胜奖励可领取次数
        BYTE  nPFWinAwardCount;						// 学徒首胜奖励次数
        int   nPFWinAwardTime;						// 学徒首胜奖励时间
    
        DBREQ_RESULT_READMENTORSHIPPERSON(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    
    /// 更新师徒玩家个人数据(没有则添加)	 - 传入参数结构
    struct DBREQ_PARAM_UPDATEMENTORSHIPPERSON
    {
        DWORD dwPDBID;								// 玩家ID
        BYTE  nAutoIssueMFlag;						// 自动发布师傅信息标识
        BYTE  nAutoIssuePFlag;						// 自动发布学徒信息标识
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
        BYTE  nMFWinAwardPreCount;					// 导师首胜奖励可领取次数
        BYTE  nMFWinAwardCount;						// 导师首胜奖励次数
        int   nMFWinAwardTime;						// 导师首胜奖励时间
        BYTE  nPFWinAwardPreCount;					// 学徒首胜奖励可领取次数
        BYTE  nPFWinAwardCount;						// 学徒首胜奖励次数
        int   nPFWinAwardTime;						// 学徒首胜奖励时间
    
        DBREQ_PARAM_UPDATEMENTORSHIPPERSON(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    
    // 增加师傅首胜预存次数   -- 增加师徒个人数据表里面 nMFWinAwardPreCount 字段值 这个值不能超过最大值
    struct DBREQ_PARAM_UPDATEMFWINAWARDPRECOUNT
    {
        DWORD dwPDBID;							// 玩家ID
        int nAddCount;							// 增加次数
        int nMaxCount;							// 最大次数
    
        DBREQ_PARAM_UPDATEMFWINAWARDPRECOUNT(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
                    
    // 增加学徒首胜预存次数 -- 增加师徒个人数据表里面 nPFWinAwardPreCount 字段值 这个值不能超过最大值
    struct DBREQ_PARAM_UPDATEPFWINAWARDPRECOUNT
    {
        DWORD dwPDBID;							// 玩家ID
        int nAddCount;							// 增加次数
        int nMaxCount;							// 最大次数
    
        DBREQ_PARAM_UPDATEPFWINAWARDPRECOUNT(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
//////////////////////////////////////////////////////////////////////////
/***************************************   角色皇冠系统  **********************************************/
//////////////////////////////////////皇冠页数据////////////////////////////////////

    // 皇冠单页详细信息
    struct DBREQ_PARAM_CROWN_PAGE_ITEM
    {
        int	nPageDBID;																		// 皇冠页数			
        int crownInlaySlotList[DBDEF_CROWN_PAGE_INLAYSlOT_MAX];								// 皇冠宝石镶嵌槽位列表
        char szName[DBDEF_TALENTNAME_MAXSIZE];														// 皇冠页名称
    
        DBREQ_PARAM_CROWN_PAGE_ITEM()
        {
            memset(this, 0, sizeof(DBREQ_PARAM_CROWN_PAGE_ITEM));
        }
    };
    
    typedef struct DBREQ_PARAM_CROWN_PAGE_ITEM CROWN_PAGE_ITEM;
    
    // GAMEDB_REQUEST_QUERY_CROWN_INFO
    // 请求皇冠页数据
    // 传入结构
    struct DBREQ_PARAM_QUERY_CROWN_INFO
    {
        DWORD	dwActorID;								    // 角色ID
    
        DBREQ_PARAM_QUERY_CROWN_INFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    
    // 返回结构
    struct DBREQ_RESULT_QUERY_CROWN_PAGE
    {
        DWORD	dwActorID;								    // 角色ID
        INT     nPagecount;
        //Pagecount*DBREQ_PARAM_CROWN_PAGE_ITEM	    		// 皇冠页列表
        DBREQ_RESULT_QUERY_CROWN_PAGE()
        {
            memset(this, 0, sizeof(DBREQ_RESULT_QUERY_CROWN_PAGE));
        }
    };
    
    // GAMEDB_REQUEST_UPDATE_CROWN_INFO
    // 更新单个玩家皇冠页数据
    // 传入结构     
    struct DBREQ_SAVE_CROWNPAGE
    {
        DWORD	                dwActorID;						// 角色ID
        DBREQ_PARAM_CROWN_PAGE_ITEM page;	   
    
        DBREQ_SAVE_CROWNPAGE(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    
    // GAMEDB_REQUEST_DELETE_CROWN_INFO
    // 删除角色的皇冠页数据
    // 传入结构                     
    struct DBREQ_DELETE_CROWNPAGE
    {
        DWORD   dwActorID;								    // 角色ID
        int    nPageDBID;								    // 符文页序号
    
        DBREQ_DELETE_CROWNPAGE(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    
    //////////////////////////////////////皇冠的宝石数据////////////////////////////////////
    // 皇冠单个宝石详细信息					
    struct DBREQ_PARAM_CROWN_GEMSTONE_POSSESS_ITEM
    {	
        int	 nGemstoneDBID;				// 宝石个体唯一id
        int  nGemstoneID;				// 宝石类型id 如红宝石 蓝宝石        
        BYTE byGrade;					// 宝石品级
        int  nExp;						// 宝石当前经验
        bool bAppraisal;				// 是否鉴定成功过
        int  nAppraisalCount;			// 宝石鉴定次数
		int  nFaceScore;				// 颜值
		int  nPropValue;				// 威力总值
    
        DBREQ_PARAM_CROWN_GEMSTONE_POSSESS_ITEM()
        {
            memset(this, 0, sizeof(DBREQ_PARAM_CROWN_GEMSTONE_POSSESS_ITEM));
        }
    };
    
    typedef struct DBREQ_PARAM_CROWN_GEMSTONE_POSSESS_ITEM GEMSTONE_ITEM;
    // GAMEDB_REQUEST_QUERY_CROWN_GMESTONEITEM
    // 请求所有宝石信息
    // 传入结构 
    struct DBREQ_PARAM_QUERY_ALLGEMSTONE_INFO
    {
        DWORD	dwActorID;												 // 角色ID
    
        DBREQ_PARAM_QUERY_ALLGEMSTONE_INFO(void)
        {
            memset(this, 0, sizeof(DBREQ_PARAM_QUERY_ALLGEMSTONE_INFO));
        }
    };
    
    // 返回所有宝石信息
    struct DBREQ_RESULT_ALLGEMSTONE_INFO
    {
        DWORD	dwActorID;												// 角色ID
        INT     nGemstoneCount;
        //GemstoneCount*DBREQ_PARAM_CROWN_GEMSTONE_POSSESS_ITEM	    	// 所有宝石信息
        DBREQ_RESULT_ALLGEMSTONE_INFO()
        {
            memset(this, 0, sizeof(DBREQ_RESULT_ALLGEMSTONE_INFO));
        }
    };
    
    // GAMEDB_REQUEST_UPDATE_CROWN_GMESTONEITEM
    // 保存单个宝石详细信息
    // 传入结构 						
    struct DBREQ_SAVE_CROWN_GEMSTONE_POSSESS_ITEM
    {	
        DWORD	dwActorID;												    // 角色ID
        DBREQ_PARAM_CROWN_GEMSTONE_POSSESS_ITEM gemstoneItem;
        DBREQ_SAVE_CROWN_GEMSTONE_POSSESS_ITEM()
        {
            memset(this, 0, sizeof(DBREQ_SAVE_CROWN_GEMSTONE_POSSESS_ITEM));
        }
    };
    
    // GAMEDB_REQUEST_DELETE_CROWN_GMESTONEITEM
    // 删除单个宝石详细信息
    // 传入结构 
    struct DBREQ_DELETE_CROWN_GEMSTONE_POSSESS_ITEM
    {	
        DWORD	dwActorID;												    // 角色ID
        int	nGemstoneDBID;												    // 宝石个体唯一id
        DBREQ_DELETE_CROWN_GEMSTONE_POSSESS_ITEM()
        {
            memset(this, 0, sizeof(DBREQ_DELETE_CROWN_GEMSTONE_POSSESS_ITEM));
        }
    };
    
    //////////////////////////////////////皇冠的宝石产出信息////////////////////////////////////
    
    // 皇冠的宝石结构
    struct DBREQ_PARAM_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION						
	{
		bool bGemstoneProduceEnd;												// 产出是否结束 结束:数据清0 
		int cobblestonGemstoneIDList[DBDEF_PRODUCEGEMSTONECOUNTMAX];			// 原矿宝石(未选取)随机列表
		int windowGemstoneIDList[DBDEF_PRODUCEGEMSTONECOUNTMAX];				// 开窗宝石(花点券开窗)列表
		bool bOpenWinEnd;														// 是否开窗结束
		int  cobStoneIDList[DBDEF_PRODUCEGEMSTONECOUNTMAX];				    	// 原石id(未选取)随机列表
		DBREQ_PARAM_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION()
		{
			memset(this, 0, sizeof(DBREQ_PARAM_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION));
        }
    };
    
    typedef struct DBREQ_PARAM_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION GEMSTONE_PRODUCE_DISTRIBUTION;
    // GAMEDB_REQUEST_QUERY_GEMSTONE_PRODUCE_DISTRIBUTION
    // 查询宝石产出						   
    // 传入结构 
    struct DBREQ_PARAM_QUERY_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION
    {
        DWORD	dwActorID;												 // 角色ID
    
        DBREQ_PARAM_QUERY_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION(void)
        {
            memset(this, 0, sizeof(DBREQ_PARAM_QUERY_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION));
        }
    };
    // 返回结构
    struct DBREQ_RESULT_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION
    {
        DWORD	dwActorID;												  // 角色ID
        DBREQ_PARAM_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION  gemstoneProduce; // 宝石产出
        DBREQ_RESULT_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION()
        {
            memset(this, 0, sizeof(DBREQ_RESULT_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION));
        }
    };
    
    // GAMEDB_REQUEST_UPDATE_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION
    // 保存宝石宝箱 宝石产出状态
    // 传入结构 				
    struct DBREQ_SAVE_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION						
    {
        DWORD	dwActorID;												    // 角色ID
        DBREQ_PARAM_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION	gemstoneProduce;									
        DBREQ_SAVE_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION()
        {
            memset(this, 0, sizeof(DBREQ_SAVE_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION));
        }
    };
    
    ////////////////////////////////杯赛////////////////////////////////
    /// GAMEDB_REQUEST_CREATE_LEGEND_CUP 
    // 请求创建杯赛
    struct DBREQ_PARAM_CREATE_LEGEND_CUP
    {
        DWORD       dwWorldID;                                          // 世界ID
        LONGLONG    llLegendCupID;                      	            // 杯赛ID
        int         nCreateConfigID;                    	            // 创建的配置ID
        BYTE        byCreateCupType;                    	            // 杯赛类型 1 系统 2 个人
        BYTE        bIsOnlineCup;                       	            // 线上比赛 线下比赛
        DWORD       dwCreataActorID;                   	 	            // 创建玩家 系统的为 0
        char        szCreaterName[DBDEF_ACTORNAME_MAXSIZE];             // 创建者名称
        char        szCupName[DBDEF_LEGENDCUP_NAME_LEN];	            // 杯赛名称
        BYTE        byMaxKinNumber;                      	            // 最大参赛队伍
        DWORD       dwRegisterDeadline;                 	            // 报名截止时间
        DWORD       dwConfirmRegisterDeadline;          	            // 确定最终报名截止时间
        bool        bClanLimit;                         	            // 只有联盟限制才能参加
        DWORD       dwClanID;                           	            // 联盟ID
        char        szPassword[DBDEF_PASSWORD_MAXSIZE];		            // 参赛密码
        BYTE        nNeedRoundCount;                    	            // 杯赛需要打几轮
        int         nLevelLimit;                        	            // 等级限制
        DWORD       dwContriBonus;                      	            // 出资奖励

        DWORD       dwTotalRegisterTicket;              	            // 报名总共累积的点卷
        DWORD       dwRegisterNeedTicket;               	            // 报名需要的点卷
        BYTE        byTicketAllocationMode;             	            // 奖励分配方式 1、 系统设置 2、玩家自定义 ETicketAllocateMode 
        BYTE        byRankTicketRatio[DBDEF_LEGENDCUP_KINSCOUNT_LEN];   // 分配奖励比例
        bool        bIsFreeCreate;                                      // 是否免费创建
		
        DBREQ_PARAM_CREATE_LEGEND_CUP(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 创建杯赛返回结果
    struct DBREQ_RESULT_CREATE_LEGEND_CUP
    {
        LONGLONG    llLegendCupID;                                      // 杯赛ID
        int         nCreateConfigID;                                    // 创建的配置ID
        BYTE        byCreateCupType;                                    // 杯赛类型 1 系统 2 个人
        BYTE        bIsOnlineCup;                                       // 线上比赛 线下比赛
        DWORD       dwCreataActorID;                                    // 创建玩家 系统的为 0
        char        szCreaterName[DBDEF_ACTORNAME_MAXSIZE];             // 创建者名称
        char        szCupName[DBDEF_LEGENDCUP_NAME_LEN];                // 杯赛名称
        BYTE        byMaxKinNumber;                                     // 最大参赛队伍
        DWORD       dwRegisterDeadline;                                 // 报名截止时间
        DWORD       dwConfirmRegisterDeadline;                          // 确定最终报名截止时间
        bool        bClanLimit;                                         // 只有联盟限制才能参加
        DWORD       dwClanID;                                           // 联盟ID
        char        szPassword[DBDEF_PASSWORD_MAXSIZE];	                // 参赛密码
        BYTE        nNeedRoundCount;                                    // 杯赛需要打几轮
        int         nLevelLimit;                                        // 等级限制
        DWORD       dwContriBonus;                                      // 出资奖励
        DWORD       dwTotalRegisterTicket;                              // 报名总共累积的点卷
        DWORD       dwRegisterNeedTicket;                               // 报名需要的点卷
        BYTE        byTicketAllocationMode;                             // 奖励分配方式 1、 系统设置 2、玩家自定义 ETicketAllocateMode  
        BYTE        byRankTicketRatio[DBDEF_LEGENDCUP_KINSCOUNT_LEN];   // 分配奖励比例
        bool        bIsFreeCreate;                                      // 是否免费创建

        DBREQ_RESULT_CREATE_LEGEND_CUP(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    /// GAMEDB_REQUEST_READ_ALL_LEGEND_CUP 
    /// 读取所有比赛基础信息

    ///- 传入参数结构
    struct DBREQ_PARAM_READCUPLIST
    {
        DBREQ_PARAM_READCUPLIST(void)
        {
            memset(this, 0, sizeof(DBREQ_PARAM_READCUPLIST));
        }
    };

    ///- 返回结构
    struct DBREQ_RESULT_READCUPLIST
    {
        int nLegendCupCount;								// 返回杯赛的数量

        // 结果集：nLegendCupCount * DBREQ_RESULT_LEGEND_CUP_BASE_INFO
        DBREQ_RESULT_READCUPLIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    struct DBREQ_RESULT_LEGEND_CUP_BASE_INFO
    {
        LONGLONG        llLegendCupID;                      // 杯赛ID
        int             nCreateConfigID;                    // 创建的配置ID
        BYTE            byCreateCupType;                    // 杯赛类型 1 系统 2 个人
        BYTE            bIsOnlineCup;                       // 线上比赛 线下比赛
        DWORD           dwCreataActorID;                    // 创建玩家 系统的为 0
        char            szCreaterName[DBDEF_ACTORNAME_MAXSIZE];// 创建者名称
        char            szCupName[DBDEF_LEGENDCUP_NAME_LEN];// 杯赛名称
        BYTE            byMaxKinNumber;                      // 最大参赛队伍
        DWORD           dwRegisterDeadline;                 // 报名截止时间
        DWORD           dwConfirmRegisterDeadline;          // 确定最终报名截止时间
        bool            bClanLimit;                         // 只有联盟限制才能参加
        DWORD           dwClanID;                           // 联盟ID
        char            szPassword[DBDEF_PASSWORD_MAXSIZE];	// 参赛密码
        BYTE            nNeedRoundCount;                    // 杯赛需要打几轮
        int             nLevelLimit;                        // 等级限制
        DWORD           dwContriBonus;                      // 出资奖励
        bool            bHaveRewards;                       // 发放奖励标示
        BYTE            eCupState;                          // 状态 ECupState
        BYTE            nBeginRound;                        // 本场比赛由第几轮开始
        BYTE            nCurrentRound;                      // 当前打到第几轮
        bool            bIsEnd;                             // 是否结束
        DWORD           dwEndTime;                          // 结束日期

        DWORD           dwTotalRegisterTicket;              // 报名总共累积的点卷
        DWORD           dwRegisterNeedTicket;               // 报名需要的点卷
        BYTE            byTicketAllocationMode;             // ETicketAllocateMode 
        BYTE            byRankTicketRatio[DBDEF_LEGENDCUP_KINSCOUNT_LEN]; // 分配奖励比例
        bool            bIsFreeCreate;                      // 是否免费创建
		
        DBREQ_RESULT_LEGEND_CUP_BASE_INFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    ///  更新杯赛基础信息     GAMEDB_REQUEST_UPDATE_LEGEND_CUP_BASEINFO
    struct DBREQ_REQUEST_UPDATE_LEGEND_CUP_BASEINFO
    {
        DWORD           dwWorldID;                          // 世界ID
        LONGLONG        llLegendCupID;                      // 杯赛ID
        BYTE            eCupState;                          // 状态 ECupState
        BYTE            nBeginRound;                        // 本场比赛由第几轮开始
        BYTE            nCurrentRound;                      // 当前打到第几轮
        DWORD           dwTotalRegisterTicket;              // 报名总共累积的点卷
        bool            bIsEnd;                             // 是否结束
        DWORD           dwEndTime;                          // 结束日期
        bool            bHaveRewards;                       // 发放奖励标示
		
        DBREQ_REQUEST_UPDATE_LEGEND_CUP_BASEINFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///  请求杯赛附加列表数据         GAMEDB_REQUEST_EX_LIST_DATA_INFO
    struct DBREQ_REQUEST_EX_LIST_DATA_INFO
    {
        DWORD          dwWorldID;                                          // 世界ID
        LONGLONG       llLegendCupID;                                     // 杯赛ID

        DBREQ_REQUEST_EX_LIST_DATA_INFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 返回附加列表数据
    struct DBREQ_RESULT_EX_LIST_DATA_INFO
    {
        DWORD           dwWorldID;                          // 世界ID
        LONGLONG        llLegendCupID;                                      // 杯赛ID
        DWORD           dwBlackList[DBDEF_LEGENDCUP_BALCKLIST_LEN];         // 黑名单
        DWORD           dwRegisterList[DBDEF_LEGENDCUP_KINSCOUNT_LEN];      // 杯赛战队列表
        DWORD           dwRegisterPDBID[DBDEF_LEGENDCUP_KINSCOUNT_LEN];     // 杯赛申请人ID列表
        int             nRegisterRankList[DBDEF_LEGENDCUP_KINSCOUNT_LEN];   // 参赛战队排名

        DBREQ_RESULT_EX_LIST_DATA_INFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    ///  更新杯赛申请列表     GAMEDB_REQUEST_UPDATE_RIGESTERLIST
    struct DBREQ_REQUST_UPDATE_RIGESTERLIST
    {
        DWORD          dwWorldID;                                           // 世界ID
        LONGLONG       llLegendCupID;                                     // 杯赛ID
        DWORD       dwRegisterList[DBDEF_LEGENDCUP_KINSCOUNT_LEN];    // 杯赛战队ID列表
        DWORD       dwRegisterPDBID[DBDEF_LEGENDCUP_KINSCOUNT_LEN];    // 杯赛申请人ID列表

        DBREQ_REQUST_UPDATE_RIGESTERLIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    ///////////////////////////////////////////////////////////////////
    ///  更新杯赛黑名单列表     GAMEDB_REQUEST_UPDATE_BLACKLIST
    struct DBREQ_REQUST_UPDATE_BLACKLIST
    {
        DWORD           dwWorldID;                              // 世界ID
        LONGLONG       llLegendCupID;                              // 杯赛ID
        DWORD       dwBlackList[DBDEF_LEGENDCUP_BALCKLIST_LEN]; // 杯赛战队ID列表

        DBREQ_REQUST_UPDATE_BLACKLIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///  更新杯赛参赛排名列表     GAMEDB_REQUEST_UPDATE_RANKLIST
    struct DBREQ_REQUST_UPDATE_RANKLIST
    {
        DWORD          dwWorldID;                                           // 世界ID
        LONGLONG       llLegendCupID;                                     	// 杯赛ID
        int            nRegisterRankList[DBDEF_LEGENDCUP_KINSCOUNT_LEN];    // 杯赛战队排名

        DBREQ_REQUST_UPDATE_RANKLIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    ///  记录所有战队的信息     GAMEDB_REQUEST_SAVE_ALL_KININFO
    struct DBREQ_REQUST_SAVE_ALL_KININFO
    {
        int nCount;                                         // 成员数量

        ///不定长
        // nCount * sizeof(DBREQ_REQUEST_UPDATE_CUP_KININFO)

        DBREQ_REQUST_SAVE_ALL_KININFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    ///  读取比赛战队所有记录信息     GAMEDB_REQUEST_READ_ALL_KININFO
    struct DBREQ_PARAM_READ_ALL_KININFO
    {
        DWORD    dwWorldID;                                 // 世界ID
        LONGLONG llLegendCupID;								// 比赛ID

        DBREQ_PARAM_READ_ALL_KININFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 杯赛战队信息列表 - 返回结果
    struct DBREQ_RESULT_READLEGENDCUP_KINLIST
    {
        LONGLONG llLegendCupID;						// 比赛ID
        int nKinCount;							    // 参赛战队数量
        // 结果集：nKinCount * DBREQ_REQUEST_UPDATE_CUP_KININFO

        DBREQ_RESULT_READLEGENDCUP_KINLIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    ///  更新比赛单个战队记录信息     GAMEDB_REQUEST_UPDATE_CUP_KININFO
    struct DBREQ_REQUEST_UPDATE_CUP_KININFO
    {
        DWORD           dwWorldID;                          // 世界ID
        LONGLONG        llLegendCupID;                      // 杯赛ID
        DWORD           dwKinID;                            // 战队ID
        int             nGroupScore;                        // 小组赛积分情况 
        int             nCurKnockOutScore;                  // 当前淘汰赛积分情况 bo1 bo3 bo5
        // 其他字段后续添加

        DBREQ_REQUEST_UPDATE_CUP_KININFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    ///  记录比赛所有成员信息     GAMEDB_REQUEST_UPDATE_CUP_MEMBERLIST
    struct DBREQ_REQUST_UPDATE_CUP_MEMBER_HEAD
    {
        DWORD   dwWorldID;                                  // 世界ID
        LONGLONG llLegendCupID;								// 比赛ID
        int nCount;                                         // 成员数量

        ///不定长
        // nCount * sizeof(DBREQ_REQUST_UPDATE_CUP_MEMBERLIST)

        DBREQ_REQUST_UPDATE_CUP_MEMBER_HEAD(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    struct DBREQ_REQUST_UPDATE_CUP_MEMBERLIST
    {
        DWORD dwKinID;								    // 战队ID
        DWORD dwActorID;								// 成员ID

        DBREQ_REQUST_UPDATE_CUP_MEMBERLIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    ///  读取比赛成员相关信息     GAMEDB_REQUEST_LEGEND_CUP_MEMBERLIST
    struct DBREQ_PARAM_READCUPMEMBERLIST
    {
        DWORD dwWorldID;                                    // 世界ID
        LONGLONG llLegendCupID;								// 比赛ID

        DBREQ_PARAM_READCUPMEMBERLIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // 杯赛成员列表 - 返回结果
    struct DBREQ_RESULT_READLEGENDCUP_MEMBERLIST
    {
        LONGLONG llLegendCupID;						// 比赛ID
        int nMemberCount;							// 参赛人员数量
        // 结果集：nMemberCount * DBREQ_PARAM_LEGENDCUP_MEMBER

        DBREQ_RESULT_READLEGENDCUP_MEMBERLIST(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    struct DBREQ_PARAM_LEGENDCUP_MEMBER
    {

        DWORD   dwKinID;                                      	// 战队id DBREQ_PARAM_PLAYER_INFO
        DWORD   dwUserID;                                     	// 账号ID
        DWORD   dwActorID;                                      // 角色ID DBREQ_PARAM_PLAYER_INFO
        char    szActorName[DBDEF_ACTORNAME_MAXSIZE];           // 角色名 DBREQ_PARAM_PLAYER_INFO
        WORD    nLevel;                                         // 静态角色等级   DBREQ_PARAM_PLAYER_INFO
        bool    bSex;                                           // 性别           DBREQ_PARAM_PLAYER_INFO
        DWORD   dwFightCapacity;                                // 玩家战力       DBREQ_PARAM_PLAYER_INFO

        													    // 以下是最大段位信息（nGrade） 和 DBREQ_TABLE_KINMEMBER的一样获取方式
        BYTE    nMatchType;
        DWORD   nRankScore;
        DWORD   nGrade;

        DBREQ_PARAM_LEGENDCUP_MEMBER(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    ///  删除参赛玩家     GAMEDB_REQUEST_LEGEND_CUP_CUP_DELETE_KINMEMBER
    struct DBREQ_PARAM_CUP_DELETE_KINMEMBER
    {
        LONGLONG llLegendCupID;								// 比赛ID
        DWORD   dwActorID;                                  // 玩家ID

        DBREQ_PARAM_CUP_DELETE_KINMEMBER(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    
       
    ////////////////////////////比赛记录信息//////////////////////////////////////////////
    // 更新比赛记录信息  GAMEDB_REQUEST_SAVE_CUP_COMPETITION_NODE
    struct DBREQ_REQUEST_SAVE_CUP_COMPETITION_NODE
    {
        DWORD   dwWorldID;                          // 世界ID
        LONGLONG llLegendCupID;						// 比赛ID
        int     nCount;                                 //记录数
        // nCount * DBREQ_PARAM_COMPETITION_NODE
        DBREQ_REQUEST_SAVE_CUP_COMPETITION_NODE()
        {
            memset( this, 0, sizeof(*this) );
        }
    };

    //////////////////////////////////////////////////////////////////////////
    ////GAMEDB_REQUEST_READ_ALL_CUP_COMPETITION_NODE 读取比赛节点信息

    struct DBREQ_PARAM_REQUEST_READ_CUP_COMPETITION_NODE
    {
        DWORD dwWorldID;                            // 世界ID
        LONGLONG llLegendCupID;						// 比赛ID

        DBREQ_PARAM_REQUEST_READ_CUP_COMPETITION_NODE()
        {
            memset( this, 0, sizeof(*this) );
        }
    };

    struct DBREQ_REQUEST_ALL_CUP_COMPETITION_NODE
    {
        LONGLONG llLegendCupID;						// 比赛ID
        int     nCount;                             // 记录数
        // nCount * DBREQ_PARAM_COMPETITION_NODE
        DBREQ_REQUEST_ALL_CUP_COMPETITION_NODE()
        {
            memset( this, 0, sizeof(*this) );
        }
    };

    // 比赛节点信息
    struct DBREQ_PARAM_COMPETITION_NODE
    {
        BYTE    byCompetitionType;   // 1小组赛 2淘汰赛
        int     nSerchID;           // 生成的快速定位ID
        int     nRoundID;           // 第几轮比赛
        int     nGroupID;           // 分配到第几组
        int     nDay;               // 第几天
        int     nPos;               // 位置索引 从1开始
        DWORD   dwKin1;             // 比赛战队1
        int     nKin1Score;         // 战队1本次比赛分数
        DWORD   dwKin1TotalKda;      // 战队1总KDA
        DWORD   dwKin2;             // 比赛战队2
        int     nKin2Score;         // 战队2本次比赛分数
        DWORD   dwKin2TotalKda;      // 战队2总KDA
        BYTE    eNodeState;         // 节点状态ECompetitionNodeState
        DWORD   dwWinnerKinID;      // 胜利战队ID
        DWORD   dwBeginTime;        // 比赛节点开始时间
        DWORD   dwEndTime;          // 比赛节点结束时间
        

        BYTE    byBoType;           // BO类型 ELegendCupBO
        int     nCurrentBo;         // 已经打了多少场
        LONGLONG WarIDList[DBDEF_LEGENDCUP_COMPETITION_WARID_COUNT]; // 战场ID列表
        DWORD    WinKinList[DBDEF_LEGENDCUP_COMPETITION_WARID_COUNT]; // 战场胜利列表
		BYTE     WinFlag[DBDEF_LEGENDCUP_COMPETITION_WARID_COUNT];   // 胜利标志EJudgeSingalWar

        DBREQ_PARAM_COMPETITION_NODE()
        {
            memset(this, 0, sizeof(*this) );
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // 单独更新某个比赛记录信息  GAMEDB_REQUEST_UPDATE_CUP_COMPETITION_NODE
    struct DBREQ_REQUEST_UPDATE_CUP_COMPETITION_NODE
    {
        DWORD   dwWorldID;              // 世界ID
        LONGLONG   llLegendCupID;	    // 比赛ID
        int     nSerchID;               // 生成的快速定位ID
        BYTE    byCompetitionType;      // 1小组赛 2淘汰赛
        int     nRoundID;               // 第几轮比赛
        int     nGroupID;               // 分配到第几组
        int     nDay;                   // 第几天
        int     nPos;                   // 位置索引
        int     nKin;                   // 比赛战队1
        int     nKin1Score;             // 战队1本次比赛分数
        DWORD   dwKin1TotalKda;         // 战队1总KDA
        int     nKin2;                  // 比赛战队2
        int     nKin2Score;             // 战队2本次比赛分数
        DWORD   dwKin2TotalKda;         // 战队2总KDA
        BYTE    eNodeState;             // 节点状态ECompetitionNodeState
        DWORD   dwWinnerKinID;          // 胜利战队ID
        DWORD   dwBeginTime;            // 比赛节点开始时间
        DWORD   dwEndTime;              // 比赛节点结束时间

        BYTE    byBoType;               // BO类型 ELegendCupBO
        int     nCurrentBo;             // 已经打了多少场 bo1 bo3 bo5

        LONGLONG WarIDList[DBDEF_LEGENDCUP_COMPETITION_WARID_COUNT]; // 战场ID列表
        DWORD    WinKinList[DBDEF_LEGENDCUP_COMPETITION_WARID_COUNT]; // 战场胜利列表
		BYTE     WinFlag[DBDEF_LEGENDCUP_COMPETITION_WARID_COUNT];   // 胜利标志EJudgeSingalWar

        DBREQ_REQUEST_UPDATE_CUP_COMPETITION_NODE()
        {
            memset(this, 0, sizeof(*this) );
        }
    };

    //////////////////////////////////////////////////////////////////////////
    //GAMEDB_REQUEST_READ_ROUND_GROUP_KIN_POS             获得比赛每轮分组战队位置列表
    struct DBREQ_PARAM_REQUEST_READ_ROUND_GROUP_KIN_POS
    {
        DWORD dwWorldID;                            // 世界ID
        LONGLONG llLegendCupID;						// 比赛ID

        DBREQ_PARAM_REQUEST_READ_ROUND_GROUP_KIN_POS()
        {
            memset( this, 0, sizeof(*this) );
        }
    };

    struct DBREQ_RESULT_ROUND_GROUP_KIN_POS
    {
        LONGLONG llLegendCupID;						// 比赛ID
        int     nCount;                             // 记录数
        // nCount * DBREQ_PARAM_ROUND_GROUP_KIN_POS
        DBREQ_RESULT_ROUND_GROUP_KIN_POS()
        {
            memset( this, 0, sizeof(*this) );
        }
    };
    // 本轮战队列表
    struct DBREQ_PARAM_ROUND_GROUP_KIN_POS
    {
        int   nRoundID;
        DWORD KinIDList[DBDEF_LEGENDCUP_KINSCOUNT_LEN];   // 本轮战队ID位置列表

        DBREQ_PARAM_ROUND_GROUP_KIN_POS()
        {
            memset(this, 0, sizeof(*this) );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    //GAMEDB_REQUEST_SAVE_ROUND_GROUP_KIN_POS                // 更新一轮战队分组列表位置列表
    struct DBREQ_REQUEST_SAVE_ROUND_GROUP_KIN_POS
    {
        DWORD dwWorldID;                                  // 世界ID
        LONGLONG llLegendCupID;						      // 比赛ID
        int   nRoundID;                                   // 第几轮
        DWORD KinIDList[DBDEF_LEGENDCUP_KINSCOUNT_LEN];   // 本轮战队ID位置列表

        DBREQ_REQUEST_SAVE_ROUND_GROUP_KIN_POS()
        {
            memset(this, 0, sizeof(*this) );
        }
    };

    ///////////////////////////////////////////////////////////////////
    ///  保存比赛每轮的比赛时间相关     GAMEDB_REQUEST_SAVE_CUP_ROUND_TIMEINFO
    struct DBREQ_REQUEST_SAVE_CUP_ROUND_TIMEINFO
    {
        DWORD dwWorldID;                                    // 世界ID
        LONGLONG llLegendCupID;								// 杯赛ID
        int nCount;                                         // 数量
        ///不定长
        // nCount * sizeof(DBREQ_PARAM_ROUND_TIMEINFO)

        DBREQ_REQUEST_SAVE_CUP_ROUND_TIMEINFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    ///////////////////////////////////////////////////////////////////
    ///  读取每轮比赛的时间相关     GAMEDB_REQUEST_READ_CUP_ROUND_TIMEINFO
    /// 输入参数
    struct DBREQ_PARAM_READ_CUP_ROUND_TIMEINFO
    {
        DWORD dwWorldID;                            // 世界ID
        LONGLONG    llLegendCupID; 					// 杯赛ID
        DBREQ_PARAM_READ_CUP_ROUND_TIMEINFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    /// 读取排行榜设置数据返回
    struct DBREQ_RESULT_READ_CUP_ROUND_TIMEINFO
    {
        LONGLONG    llLegendCupID;					// 杯赛ID
        int		    nCount;						    // 返回数据个数
        // nCount * DBREQ_PARAM_ROUND_TIMEINFO
        DBREQ_RESULT_READ_CUP_ROUND_TIMEINFO(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    struct DBREQ_PARAM_ROUND_TIMEINFO
    {
        int     nRoundID;                           // 第几轮
        DWORD   dwRoundCreateRoomTime;              // 本轮第一次创建房间的时间
        DWORD   dwRoomBeginInterval;                // 房间创建后开始间隔
        DWORD   dwRoundEndTime;                     // 本轮结束时间

        DBREQ_PARAM_ROUND_TIMEINFO()
        {
            memset(this, 0, sizeof(*this) );
        }
    };

    ///////////////////////////////////////////////////////////////////
    ///  单独更新某轮比赛的时间     GAMEDB_REQUEST_UPDATE_CUP_ROUND_TIMEINFO
    /// 输入参数
    struct DBREQ_UPDATE_SIGNAL_ROUND_TIMEINFO
    {
        DWORD       dwWorldID;                          // 世界ID
        LONGLONG    llLegendCupID;					    // 杯赛ID
        int         nRoundID;                           // 第几轮
        DWORD       dwRoundCreateRoomTime;              // 本轮第一次创建房间的时间
        DWORD       dwRoomBeginInterval;                // 房间创建后开始间隔
        DWORD       dwRoundEndTime;                     // 本轮结束时间

        DBREQ_UPDATE_SIGNAL_ROUND_TIMEINFO()
        {
            memset(this, 0, sizeof(*this) );
        }
    };

	// 读取玩家杯赛数据信息 GAMEDB_REQUEST_READ_PLAYER_LEGENDCUP_DATA     
	/// 输入参数
	struct DBREQ_PARAM_READ_PLAYER_CUP_DATA_HEAD
	{
		int    nCount; 					// 玩家个数
		// nCount * DBREQ_PARAM_READ_PLAYER_CUP_INFO
		DBREQ_PARAM_READ_PLAYER_CUP_DATA_HEAD(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	struct DBREQ_PARAM_READ_PLAYER_CUP_INFO
	{
		DWORD    dwPDBID; 					// 玩家PDBID
		DBREQ_PARAM_READ_PLAYER_CUP_INFO(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 返回
	struct DBREQ_RESULT_READ_PLAYER_CUP_DATA_HEAD
	{
		int		    nCount;						    // 返回数据个数
		// nCount * DBREQ_RESULT_READ_PLAYER_CUP_INFO
		DBREQ_RESULT_READ_PLAYER_CUP_DATA_HEAD(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	struct DBREQ_RESULT_READ_PLAYER_CUP_INFO
	{
		DWORD    dwPDBID; 					// 玩家PDBID
		int		 nClanCupPrizeCount;		// 免费联盟杯赛的奖励次数
		int		 nRecvPrizeTime;			// 收取奖励时间

		DBREQ_RESULT_READ_PLAYER_CUP_INFO(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 更新玩家杯赛数据信息 GAMEDB_REQUEST_UPDATE_PLAYER_LEGENDCUP_DATA (没有则添加)
	struct DBREQ_PARAM_UPDATE_PLAYER_LEGENDCUP_DATA
	{
		DWORD    dwPDBID; 					// 玩家PDBID
		int		 nClanCupPrizeCount;		// 免费联盟杯赛的奖励次数
		int		 nRecvPrizeTime;			// 收取奖励时间

		DBREQ_PARAM_UPDATE_PLAYER_LEGENDCUP_DATA(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/****************************************** 邮件 ******************************************************/
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// GAMEDB_REQUEST_READ_MAIL_LIST  （玩家上线时读取）
	/// 读取玩家邮件列表	 - 传入参数结构
	struct DBREQ_PARAM_READMAILLIST
	{
		DWORD dwPDBID;							// 玩家ID
		int   nCurDate;							// 当前时间
		
		DBREQ_PARAM_READMAILLIST(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 邮件基础数据库数据结构
	struct DBREQ_TABLE_MAIL_BASEDATA
	{
		LONGLONG llMailID;							// 邮件id
		BYTE  nType;								// 邮件类型
		BYTE  nSourceType;							// 资源来源类型
		char  szTitle[DBDEF_MAIL_TITLE_LEN];		// 标题			
		char  szSenderName[DBDEF_ACTORNAME_MAXSIZE];// 发件者姓名
		DWORD dwSenderPDBID;						// 发送者PDBID
		bool  bIsPlusData;							// 是否有附件
		DWORD dwRecvTime;							// 发件时间
		DWORD dwLifeTime;							// 生命时长 (单位:小时)

		DBREQ_TABLE_MAIL_BASEDATA(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// 读取邮件列表	 - 结果返回
	struct DBREQ_RESULT_READMAILLIST
	{
		DWORD dwPDBID;							// 玩家ID
		int nCurCount;							// 当次返回的个数

		// 结果集：nCurCount * DBREQ_TABLE_MAIL_BASEDATA
		DBREQ_RESULT_READMAILLIST(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// GAMEDB_REQUEST_READ_MAIL
	/// 读取邮件数据	 - 传入参数结构
	struct DBREQ_PARAM_READMAIL
	{
		DWORD dwPDBID;							// 玩家ID
		LONGLONG llMailID;						// 邮件id

		DBREQ_PARAM_READMAIL(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// 邮件物品数据
	struct DBREQ_PARAM_MAILGOODS
	{
		DWORD dwGoodsID;
		int nCount;

		DBREQ_PARAM_MAILGOODS(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// 读取邮件内容	 - 结果返回
	struct DBREQ_RESULT_READMAIL
	{
		DWORD dwPDBID;											// 玩家ID
		LONGLONG llMailID;										// 邮件id
		BYTE nType;												// 邮件类型
		char  szContext[DBDEF_MAIL_CONTENT_LEN];				// 内容
		int  nPlusMoney;										// 附送金钱
		int  nPlusExp;											// 附送经验
		BYTE  nPlusGoodsNum;									// 附件物品的个数
		DBREQ_PARAM_MAILGOODS sGoodsList[DBDEF_MAIL_GOODS_NUM_MAX];	// 附加物品

		DBREQ_RESULT_READMAIL(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// GAMEDB_REQUEST_DEL_MAIL
	/// 删除邮件数据	 - 传入参数结构
	struct DBREQ_PARAM_DELMAIL
	{
		DWORD dwPDBID;							// 玩家ID
		LONGLONG llMailID;						// 邮件id

		DBREQ_PARAM_DELMAIL(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/// GAMEDB_REQUEST_ADD_MAIL
	/// 添加邮件数据	 - 传入参数结构
	struct DBREQ_PARAM_ADDMAIL
	{
		LONGLONG llMailID;										// 邮件id
		BYTE  nType;											// 邮件类型
		BYTE  nSourceType;										// 资源来源类型
		char  szTitle[DBDEF_MAIL_TITLE_LEN];					// 标题
		char  szContext[DBDEF_MAIL_CONTENT_LEN];				// 内容
		char  szSenderName[DBDEF_ACTORNAME_MAXSIZE];			// 发件者姓名
		DWORD dwSenderPDBID;									// 发送者PDBID
		DWORD dwMasterPDBID;									// 主人PDBID
		bool  bIsPlusData;										// 是否有附件
		DWORD dwRecvTime;										// 发件时间
		DWORD dwLifeTime;										// 生命时长 (单位:小时)
		int  nPlusMoney;										// 附送金钱
		int  nPlusExp;											// 附送经验
		BYTE  nPlusGoodsNum;									// 附件物品的个数
		DBREQ_PARAM_MAILGOODS sGoodsList[DBDEF_MAIL_GOODS_NUM_MAX];	// 附加物品

		DBREQ_PARAM_ADDMAIL(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/****************************************** 标签功能 ******************************************************/
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct DBREQ_TABLE_LABEL
	{
		int	nLabelID;					// 标签ID
		int nLabelNum;					// 获得该标签的总次数

		DBREQ_TABLE_LABEL(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// GAMEDB_REQUEST_READ_LABEL_LIST   参数类型：传入参数
	// 读取玩家所有的标签
	struct DBREQ_PARAM_READ_LABEL_LIST
	{
		DWORD dwPDBID;					// 玩家ID

		DBREQ_PARAM_READ_LABEL_LIST(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// GAMEDB_REQUEST_READ_LABEL_LIST   参数类型：传出参数
	// 返回玩家所有的标签
	struct DBREQ_RESULT_READ_LABEL_LIST
	{
		DWORD dwPDBID;					// 玩家ID
		int  nCount;						// 该玩家所获得的所有标签个数
		// nCount * DBREQ_TABLE_LABEL

		DBREQ_RESULT_READ_LABEL_LIST()
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// GAMEDB_REQUEST_UPDATE_LABEL		参数类型：传入参数
	// 更新玩家的标签信息,没有则新建,
	struct DBREQ_PARAM_UPDATE_LABEL
	{
		DWORD dwPDBID;					// 玩家ID
		int	nLabelID;					// 标签ID
		int nLabelNum;					// 某个标签的新增的个数

		DBREQ_PARAM_UPDATE_LABEL(void)
		{
			memset(this, 0, sizeof(*this));
		}

	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/****************************************** 点赞功能 ******************************************************/
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// GAMEDB_REQUEST_ADD_SUPOORT		参数类型：传入参数
	// 更新玩家的点赞信息,
	// 返回DBREQ_RESULT_QUERYACTORLOGIN中的SPlayerInfo.dwSupportNum字段值
	struct DBREQ_PARAM_ADD_SUPPORT
	{
		DWORD dwPDBID;					// 玩家ID
		int nAddSupportNum;				// 本次新增的点赞数量

		DBREQ_PARAM_ADD_SUPPORT(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/****************************************** 装备方案功能 ******************************************************/
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 装备方案表
	struct DBREQ_TABLE_EQUIP_SCHEME
	{
		DWORD dwPDBID;											// 玩家ID
		WORD wHeroID;											// 英雄ID
		BYTE bySchemeID;										// 装备方案ID
		char szSchemeName[DBDEF_MAX_GAMENAME_MAXSIZE];			// 装备方案名字
		char szAuthorName[DBDEF_ACTORNAME_MAXSIZE];				// 装备方案作者名字
		char szSchemeTitle[DBDEF_MAX_GAMENAME_MAXSIZE];			// 装备方案称号
		DWORD dwEquipID[DBDEF_EQUIPSCHEME_EQUIPCOUNT_MAXSIZE];	// 装备ID
		int nWeight[DBDEF_EQUIPSCHEME_EQUIPCOUNT_MAXSIZE];		// 权重

		DBREQ_TABLE_EQUIP_SCHEME(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};
	
	// GAMEDB_REQUEST_READ_EQUIPSCHEME_LIST			参数类型：传入参数
	// 读取装备方案表
	struct DBREQ_PARAM_READ_EQUIPSCHEME_LIST
	{
		DWORD dwPDBID;											// 玩家ID
		WORD wHeroID;											// 英雄ID

		DBREQ_PARAM_READ_EQUIPSCHEME_LIST(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// GAMEDB_REQUEST_READ_EQUIPSCHEME_LIST			参数类型：传出参数
	// 返回装备方案表
	struct DBREQ_RESULT_READ_EQUIPSCHEME_LIST
	{
		WORD wHeroID;		// 英雄ID,本次请求的英雄ID
		int nCount;			// 方案个数

		//DBREQ_TABLE_EQUIP_SCHEME * nCount

		DBREQ_RESULT_READ_EQUIPSCHEME_LIST(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};


	// GAMEDB_REQUEST_SAVE_EQUIPSCHEME			参数类型：传入参数
	// 保存装备方案表
	struct DBREQ_PARAM_SAVE_EQUIPSCHEME
	{
		DWORD dwPDBID;											// 玩家ID
		WORD wHeroID;											// 英雄ID
		BYTE bySchemeID;										// 装备方案ID
		char szSchemeName[DBDEF_MAX_GAMENAME_MAXSIZE];			// 装备方案名字
		char szAuthorName[DBDEF_ACTORNAME_MAXSIZE];				// 装备方案作者名字
		char szSchemeTitle[DBDEF_MAX_GAMENAME_MAXSIZE];			// 装备方案称号
		DWORD dwEquipID[DBDEF_EQUIPSCHEME_EQUIPCOUNT_MAXSIZE];	// 装备ID
		int nWeight[DBDEF_EQUIPSCHEME_EQUIPCOUNT_MAXSIZE];		// 权重

		DBREQ_PARAM_SAVE_EQUIPSCHEME(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// GAMEDB_REQUEST_DEL_EQUIPSCHEME			参数类型：传入参数
	// 删除装备方案表
	struct DBREQ_PARAM_DEL_EQUIPSCHEME
	{
		DWORD dwPDBID;											// 玩家ID
		WORD wHeroID;											// 英雄ID
		BYTE bySchemeID;										// 装备方案ID

		DBREQ_PARAM_DEL_EQUIPSCHEME(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/****************************************** 英雄皮肤使用次数 ******************************************************/
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 英雄皮肤使用次数
	struct DBREQ_TABLE_HEROSKIN_USECOUNT
	{
		DWORD dwPDBID;			// 玩家ID
		WORD wHeroID;			// 英雄ID
		WORD wHeroSkinID;		// 皮肤ID
		DWORD dwUseCount;		// 使用次数

		DBREQ_TABLE_HEROSKIN_USECOUNT(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// GAMEDB_REQUEST_ADD_HEROSKIN_USECOUNT
	// 英雄皮肤使用次数
	struct DBREQ_ADD_HEROSKIN_USECOUNT
	{
		DWORD dwPDBID;			// 玩家ID
		WORD wHeroID;			// 英雄ID
		WORD wHeroSkinID;		// 皮肤ID
		int nAddUseCount;		// 本次新增的使用次数

		DBREQ_ADD_HEROSKIN_USECOUNT(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};

	// 查找玩家信息(通过名字)		GAMEDB_REQUEST_SNS_READ_PLAYERINFO_BY_NAME   传入参数
	struct DBREQ_PARAM_SNS_READ_PLAYINFO_BY_NAME
	{
		BYTE  byOptType;	// 操作类型
		DWORD dwOptUserID;	// 操作者UserID
		DWORD dwOptPdbid;	// 操作者pdbid

		char szName[DBDEF_ACTORNAME_MAXSIZE];	// 被查者角色名

		DBREQ_PARAM_SNS_READ_PLAYINFO_BY_NAME()
		{
			memset(this, 0, sizeof(DBREQ_PARAM_SNS_READ_PLAYINFO_BY_NAME));
		}
	};
	// 查找玩家信息(通过名字)		GAMEDB_REQUEST_SNS_READ_PLAYERINFO_BY_NAME   传出参数
	struct DBREQ_RESULT_SNS_READ_PLAYINFO_BY_NAME
	{
		BYTE  byOptType;	// 操作类型
		DWORD dwOptUserID;	// 操作者UserID
		DWORD dwOptPdbid;	// 操作者pdbid

		BYTE  bySex;		// 被查者性别
		DWORD dwUserID;		// 被查者UserID
		DWORD dwPdbid;		// 被查者pdbid
		char szName[DBDEF_ACTORNAME_MAXSIZE];	// 被查者角色名

		DBREQ_RESULT_SNS_READ_PLAYINFO_BY_NAME()
		{
			memset(this, 0, sizeof(DBREQ_RESULT_SNS_READ_PLAYINFO_BY_NAME));
		}
	};
	// 查找玩家信息(通过userid)	GAMEDB_REQUEST_SNS_READ_PLAYERINFO_BY_USERID 传入参数
	struct DBREQ_PARAM_SNS_READ_PLAYINFO_BY_USERID
	{
		BYTE  byOptType;	// 操作类型
		DWORD dwOptUserID;	// 操作者UserID
		DWORD dwOptPdbid;	// 操作者pdbid

		DWORD dwUserID;		// 被查者UserID

		DBREQ_PARAM_SNS_READ_PLAYINFO_BY_USERID()
		{
			memset(this, 0, sizeof(DBREQ_PARAM_SNS_READ_PLAYINFO_BY_USERID));
		}
	};
	// 查找玩家信息(通过userid)	GAMEDB_REQUEST_SNS_READ_PLAYERINFO_BY_USERID 传出参数
	struct DBREQ_RESULT_SNS_READ_PLAYINFO_BY_USERID
	{
		BYTE  byOptType;	// 操作类型
		DWORD dwOptUserID;	// 操作者UserID
		DWORD dwOptPdbid;	// 操作者pdbid

		BYTE  bySex;		// 被查者性别
		DWORD dwUserID;		// 被查者UserID
		DWORD dwPdbid;		// 被查者pdbid
		char szName[DBDEF_ACTORNAME_MAXSIZE];	// 被查者角色名

		DBREQ_RESULT_SNS_READ_PLAYINFO_BY_USERID()
		{
			memset(this, 0, sizeof(DBREQ_RESULT_SNS_READ_PLAYINFO_BY_USERID));
		}
	};
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/****************************************** 排位赛季数据 ***********************************************/
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 当前赛季详细信息
	struct DBREQ_TABLE_RANK_SEASON_DETAIL
	{
		DWORD	dwActorID;				// 玩家ID
		DWORD	dwBeginTime;			// 最后一场排位赛记录时间（标识新赛季的开始逻辑是否处理）
		DWORD	dwEndTime;				// 最后一场排位赛记录时间（标识旧赛季的结束逻辑是否处理）

		WORD	wReflushSeasonID;		// 重置的赛季ID（从第一次玩的赛季重置到当前赛季）
		WORD	wTopRankGrade;			// 最高段位等级
		WORD	wMatchCount;			// 比赛场次
		WORD	wWinCount;				// 胜利场次
		WORD	wContinueWinCount;		// 最高连胜场次
		WORD	wTempContinueWinCount;	// 当前连胜场次
		WORD	wMvpCount;				// MVP数量
		WORD	wSuperGodCount;			// 超神数量
		WORD	wKill5Count;			// 5杀数量
		WORD	wKill4Count;			// 4杀数量
		WORD	wKill3Count;			// 3杀数量

		WORD	wHeroUseRecordCount;	// 英雄使用记录条数
		//BYTE	3 * wHeroUseRecordCount * WORD	// 每条英雄使用记录（英雄ID，场次，胜场）

		DBREQ_TABLE_RANK_SEASON_DETAIL()
		{
			memset(this, 0, sizeof(DBREQ_TABLE_RANK_SEASON_DETAIL));
		}
	};

	// 存当前赛季详细信息 GAMEDB_REQUEST_SET_RANK_SEASON_DETAIL
	typedef DBREQ_TABLE_RANK_SEASON_DETAIL DBREQ_PARAM_SET_RANK_SEASON_DETAIL; 
	
	// 取当前赛季详细信息 GAMEDB_REQUEST_GET_RANK_SEASON_DETAIL
	struct DBREQ_PARAM_GET_RANK_SEASON_DETAIL
	{
		DWORD	dwActorID;		// 玩家ID

		DBREQ_PARAM_GET_RANK_SEASON_DETAIL()
		{
			memset(this, 0, sizeof(DBREQ_PARAM_GET_RANK_SEASON_DETAIL));
		}
	};

	typedef DBREQ_TABLE_RANK_SEASON_DETAIL DBREQ_RESULT_GET_RANK_SEASON_DETAIL;

	// 赛季历史信息
	struct DBREQ_TABLE_RANK_SEASON_RECORD
	{
		DWORD	dwActorID;		// 玩家ID

		WORD	wSeasonID;		// 赛季ID
		WORD	wRankGrade;		// 段位等级
		WORD	wMatchCount;	// 总场次
		WORD	wWinCount;		// 胜场
		WORD	wHeroRecord[DBDEF_SEASON_HERO_RECORD_COUNT];	// 英雄数据

		DBREQ_TABLE_RANK_SEASON_RECORD()
		{
			memset(this, 0, sizeof(DBREQ_TABLE_RANK_SEASON_RECORD));
		}
	};

	// 添加赛季历史信息 GAMEDB_REQUEST_ADD_RANK_SEASON_RECORD
	typedef DBREQ_TABLE_RANK_SEASON_RECORD DBREQ_PARAM_ADD_RANK_SEASON_RECORD; 

	// 取赛季历史信息 GAMEDB_REQUEST_GET_RANK_SEASON_RECORD
	struct DBREQ_PARAM_GET_RANK_SEASON_RECORD
	{
		DWORD	dwActorID;		// 玩家ID

		DBREQ_PARAM_GET_RANK_SEASON_RECORD()
		{
			memset(this, 0, sizeof(DBREQ_PARAM_GET_RANK_SEASON_RECORD));
		}
	};

	struct DBREQ_RESULT_GET_RANK_SEASON_RECORD 
	{
		int	nRecordCount;	// 历史记录数量
		// nRecordCount * DBREQ_TABLE_RANK_SEASON_RECORD
	};
}
#pragma warning(pop)
#pragma pack()
 
#endif//__INETERFACE_GAME_DATABASE_DEFINE_H__