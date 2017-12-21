/*******************************************************************
** 文件名:	e:\Rocket\Common\SecurityDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	彭智锋 （9671690@qq.com）
** 日  期:	2009/8/17 16:13
** 版  本:	1.0
** 描  述:	安全服务
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#pragma pack(1)

// tolua_begin

// 最小保护时限（分钟）
#define SECURITY_PROTECT_TIMELIMIT_MIN 1


// 最大保护时限（分钟）
#define SECURITY_PROTECT_TIMELIMIT_MXN 180

// 缩短保护时限设置后生效时间（秒）
#define SECURITY_PROTECT_MODITY_VALID (24 * 60 * 60)

// 密码长度
#define SECURITY_L2PASSWORD_LEN_MAX 42

// 玩家登陆位置信息长度
#define SECURITY_MAX_LOGIN_POSINFO_LEN 50

// 玩家登陆位置信息长度
#define SECURITY_MAX_KEY_LEN 128

// 密码小长度
#define SECURITY_MIN_PASSWORD_LEN 8

// 密码最大长度
#define SECURITY_MAX_PASSWORD_LEN 16

// 操作冷却时间(秒)
#define SECURITY_COOLING_TIME 60

// 财产保护的项目内容
enum
{
	SECURITY_PROPERTY_ITEM_WARE = 0x01,	// 仓库篮
	SECURITY_PROPERTY_ITEM_SHOP = 0x02,	// 商城
	SECURITY_PROPERTY_ITEM_EQUIP = 0x04,// 装备
	SECURITY_PROPERTY_ITEM_STOCK = 0x08,// 金币交易
	SECURITY_PROPERTY_ITEM_PROPRELET = 0x10,// 扩容道具续费(续时)
	SECURITY_PROPERTY_ITEM_HORSE = 0x20,// 座骑
	SECURITY_PROPERTY_ITEM_AUCTION = 0x40,// 竞拍
	// (目前暂时只支持8种)
};

// 玩家每天从资产账户中提取银两金币的默认上限(用户勾选[不限制]时将用这两个值代替提取额度)
#define	UNLIMITED_GETSILVER_PERDAY		1000000000		// 银两最大额度文数
#define UNLIMITED_GETGOLD_PERDAY		100000000		// 金币最大额度点数


// 安全部件数据
struct SSecurityPartData
{
	WORD	wCurLockInterval;		// 当前锁定时长
	WORD    wNextLockInterval;		// 下次锁定时长
	int		nLockSetValidTime;		// 设置下次锁定时长后下次生效的时间
	BYTE	byProtectItem;			// 财产保护当前支持设置8种，其实只有仓库，装备，商城三种。剩下供扩展

	bool	bHaveL2Password;		// 是否已有财产保护锁
	bool	bUnlockL2Password;		// 是否已解锁

	int		nLoginTime;				// 登陆时间
	
	int		nExtendLimitTime;		// 延长的安全时限

	char	szLoginPos1[SECURITY_MAX_LOGIN_POSINFO_LEN];		// 最近登陆情况1
	char	szLoginPos2[SECURITY_MAX_LOGIN_POSINFO_LEN];		// 最近登陆情况2
	char	szLoginPos3[SECURITY_MAX_LOGIN_POSINFO_LEN];		// 最近登陆情况3

	BYTE     nBlackFlags;             // 1--正常，2--异地，3--异地+黑名单，4--被盗永久锁号
	
	SSecurityPartData()
	{
		memset(this, 0, sizeof(*this));
	};

};

enum
{
	SEnum_OperateOption_SetPassword = 1,		// 设置密码
	SEnum_OperateOption_CheckPassword = 2,		// 检测密码
	SEnum_OperateOption_ModifyPassword = 3,		// 修改密码
	SEnum_OperateOption_UnchainPassword = 4,	// 解除密码
	SEnum_OperateOption_SetProtectItem = 5,		// 设置保护项
	SEnum_OperateOption_SetTimeLimit = 6,		// 设置保护时限
};

enum
{
	SEnum_CheckOption_Min,					// 权限检测最小值
	SEnum_CheckOption_Trade,				// 交易
	SEnum_CheckOption_EquipRebuild,			// 装备打造
	SEnum_CheckOption_Mail,					// 邮件
	SEnum_CheckOption_Consign,				// 寄售
	SEnum_CheckOption_Stall,				// 摆摊
	SEnum_CheckOption_GiveUp,				// 物品丢弃
	SEnum_CheckOption_Shop,					// 商城购买
	SEnum_CheckOption_Stove,				// 乾坤炉
	SEnum_CheckOption_Sale,					// 出售
	SEnum_CheckOption_Fusion,				// 合成与熔炼
	SEnum_CheckOption_ClanContribute,		// 氏族捐献
	SEnum_CheckOption_Ware,					// 仓库取出
	SEnum_CheckOption_Equip,				// 装备卸载
	SEnum_CheckOption_Horse,				// 骑乘丢弃
	SEnum_CheckOption_LingHun,				// 灵魂操作（封印）
	SEnum_CheckOption_Stock,				// 金币交易
	SEnum_CheckOption_PropRelet,			// 扩容道具续费
	SEnum_CheckOption_Auction,				// 竞拍
	SEnum_CheckOption_RoleVended,			// 角色出售
	SEnum_CheckOption_CardCollect,			// 卡片收集
	SEnum_CheckOption_TakeTicket,			// 提取金币
	SEnum_CheckOption_PayGift,				// 礼包兑换
	SEnum_CheckOption_Buy,					// 买入物品
	SEnum_CheckOption_Max,					// 权限检测最大值
};

// 登陆状态
enum
{
	SEnum_PState_LoginFromBlank		= 0x00000001,			// 黑名单登陆
	SEnum_PState_LoginHf			= 0x00000002,			// 频繁登陆
	SEnum_PState_AccountStop		= 0x00000004,			// 帐号禁用
	SEnum_PState_PrelongLTime		= 0x00000008,			// 延长安全时限时间 
	SEnum_PState_LimitUseRes		= 0x00000010,			// 启用短信安全级别
	SEnum_PState_AccreditTerminal	= 0x00000020,			// 可信任终端
};

// 安全时限控制结构
struct SSecurityInfo
{
	WORD	 wUseLimitTime;			// 已用去的安全时限的时间
	int		 wAddLimitTimeDot;		// 开始启用安全时限的时间点

	SSecurityInfo()
	{
		memset(this, 0, sizeof(*this));
	};
};

//add by lgf 11.02.12 随机数截断数据
enum //随机数应用类型
{
	RANDOMCUT_USERID_COMMON = 0,	//公共
	RANDOMCUT_USERID_REBULID,			//重铸
	RANDOMCUT_USERID_BIND,				//绑定
	RANDOMCUT_USERID_COMBINE,			//合成
	RANDOMCUT_USERID_MAKESLOT,		//打孔
	RANDOMCUT_USERID_ESCORT,			//运镖
	RANDOMCUT_USERID_KINESCORT,		//家族运镖
	RANDOMCUT_USERID_FORGE,				//锻造
	RANDOMCUT_USERID_EMBEDPLUS,		//镶嵌（附加）
	RANDOMCUT_USERID_WUJITIANSHU,	//无极天书
	RANDOMCUT_USERID_SMELT,				//强化
	RANDOMCUT_USERID_SMELTPROPERTY,//精炼
	RANDOMCUT_USERID_REBUILDLEVEL,//重铸档次属性
	RANDOMCUT_USERID_REBUILDTYPE,//重铸档次类型
	RANDOMCUT_USERID_REBUILDPLUS,//重铸附加属性
	RANDOMCUT_USERID_EMBED1,			//镶嵌位置1
	RANDOMCUT_USERID_EMBED2,			//镶嵌位置2

	RANDOMCUT_USERID_WSCOMBINE	, // 材料合成翅膀
	RANDOMCUT_USERID_WWCOMBINE	, // 翅膀合成翅膀
	RANDOMCUT_USERID_WPCOMBINE	, // 翅膀提升
	RANDOMCUT_USERID_STRIKETASK	, // 竞技场任务
	RANDOMCUT_USERID_SECONDBIND	, // 第二绑定属性
	RANDOMCUT_USERID_WARRESOUCE	, // 战场资源创建

	RANDOMCUT_USERID_RONGLIAN	, // 熔炼
	RANDOMCUT_USERID_RONGLIAN_1	, // 熔炼 1档
	RANDOMCUT_USERID_RONGLIAN_2	, // 熔炼 2档
	RANDOMCUT_USERID_RONGLIAN_3	, // 熔炼 3档
	RANDOMCUT_USERID_RONGLIAN_4	, // 熔炼 4档
	RANDOMCUT_USERID_RONGLIAN_5	, // 熔炼 5档
	RANDOMCUT_USERID_RONGLIAN_6	, // 熔炼 6档
	RANDOMCUT_USERID_RONGLIAN_7	, // 熔炼 7档
	RANDOMCUT_USERID_RONGLIAN_8	, // 熔炼 8档

	RANDOMCUT_USERID_THIRDEMBED	,	// 镶嵌第三孔
	RANDOMCUT_USERID_TEMP_STONE_1	,	// 偷鼎熔炼（弱国）
	RANDOMCUT_USERID_TEMP_STONE_2	,	// 偷鼎熔炼（均衡国）
	RANDOMCUT_USERID_TEMP_STONE_3	,	// 偷鼎熔炼（强国）

	RANDOMCUT_USERID_MAX,
};

struct SRandomCutInfo
{
	DWORD iRandomReallyValueFrom;	//随机数截断时真正的起点
	DWORD iRandomReallyValueTo;	//随机数截断时真正的终点
	DWORD iRandomCutValue;		//随机数截断后的值
	DWORD iCardinal;					//概率基数
	DWORD iCutCount;					//截断次数
	DWORD iCount;					//截断次数
	DWORD iUseKeyID;				//应用附加ID
	DWORD iUseKeyID2;				//应用附加ID2

	SRandomCutInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

#define RANDOMCUT_MAKEUP_VALUE	0.07
#define RANDOMCUTUSINGNUM_MAX		50

// tolua_end

// 二级密码表长度
#define L2PASSWORDTABLE_LEN		(SECURITY_L2PASSWORD_LEN_MAX * 2)

// 二级密码字符集
static const char L2PasswordCharSet[L2PASSWORDTABLE_LEN] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a',  'b', 'c', 'd', 'e', 'f', 'g', 'h', 
'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '`', ' ', '-', '?', '~', '!', '@', '#', '$', '%', '^', '&', 
'*', '(', ')', '_'};
// 密码字符串对应的密码表索引串分隔符
static const char CH_L2PWDSPLIT = ';';

#pragma pack()