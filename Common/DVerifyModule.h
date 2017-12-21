/*******************************************************************
** 文件名:	e:\Rocket\Common\DVerifyModule.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2008/11/8 15:59
** 版  本:	1.0
** 描  述:	验证码模块定义	
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "GameDef.h"
#include <list>
using namespace std;

#pragma pack(1)

/******************************************************************/
/////////////////////验证码的公共结构体定义 ////////////////////////
/******************************************************************/
///////////////////////////////////////////////////////////////////

// 公用定义 ( 打头: VERIFY )

//#define VERIFY_PRINT_DEBUG_INF						// 打印调试信息,并设为测试模式
//#define VERIFY_BIG_TEST								// 压力测试模式

#define VERIFY_SENDDATABUFFERSIZE		0x030000	// 用来存发送数据的内存大小,根据图片大小设
#define VERIFYFONT_PIC_HEIGHT			200			// 图片高度 ，高度 和 宽度必须相等
#define VERIFYFONT_PIC_WIDTH			200			// 图片宽度
#define VERIFYFONT_TEXTURE_WIDTH		256			// 图片纹理宽度,为2的n次方

//------------------------------------------------------------------
//验证码处理服务器定义  ( 打头: VERIFYSERVICE_ )
#define VERIFYSERVICE_SCHEME_FILENAME		"\\Data\\Scp\\VerifyCode.xml"	// 验证配置名字
#define VERIFYSERVICE_CHECKTIMEOUT		5000		//[5秒] 设置定时查过期,毫秒
#define VERIFYSERVICE_TIMEOUTDELAY		20			//[20秒] 服务器过期延迟多少秒,避免中间途中的是时间损耗,导致客户端未到时间就被判过期
													// 已移到在VerifyCode.xmc中配置timeoutdelay

//------------------------------------------------------------------
//验证码服务端定义  ( 打头: VERIFYSERVER_ )
#define VERIFYSERVER_SCHEME_FILENAME		"Scp\\Verify.xml"	// 验证配置名字
#define VERIFYSERVER_DELAYADDTIMES		20000		//[20秒] 创建时如果有验证状态，多少毫秒后请求验证,已移到在Verify.xmc中配置startverifydelay
#define VERIFYSERVER_BUFFPRIZEMAXINDEX	50			//有益状态列表个数
#define VERIFYSERVER_NODIECARDID		2000		//免死金牌ID,已移到在Verify.xmc中配置vipcardid: 免死金牌物品ID
#define VERIFYSERVER_TRYTIMES			2			//触发后下线多少次数就算不通过
#define VERIFYSERVER_LOGINARRESTDELAYS	1000		//多少毫秒后送入牢中,为等待玩家实体创建完毕

// 检查上次触发时间,0:表示没触发,主要是防，已触发时验证服务器没开等原因导致,已触发但客户端一直得不到验证
// 如果上次触发时间到现在太长，补发一次验证,直到得到验证
#define VERIFYSERVER_CHECKTIMES			600000		//[10分钟] 补发一次验证时间间隔 多少毫秒,已移到在Verify.xmc中配置checkaddverifydelay

//------------------------------------------------------------------
//验证码客户端定义  ( 打头: VERIFYCLIENT_ )
#define VERIFYCLIENT_UPDATEREMAINTIME	1000		//[10秒] 设置定时器，定时更新剩余时间,毫秒

//------------------------------------------------------------------
#define VERIFYSERVER_GETLIST_MAXNUM	80000			// 所有在线玩家列表数组最大数

#define VERIFYSERVER_SAVEFAIL_MAXNUM	10			// 验证失败最大次数,到达后不再加

#define VERIFYCODE_MAXNUM	32						// 验证码最大长度

//////////////////////////////////////////////////////////////////////////

// 验证的数据,玩家身上存储,存入数据库,共16字节
struct SVerifyTotalData
{
	bool	bFlag;			//已触发验证码状态
	WORD	nKills;			//累计杀怪数量
	DWORD	nExps;			//累计杀怪得到的经验
	WORD	nTryTimes;		//触发后请求验证次数
	WORD	nKillPersons;	//累计杀人
	DWORD	dwResetTime;	//最后清数据时间
	BYTE	nFailTimes;		//验证失败次数
	
	SVerifyTotalData(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


/******************************************************************/
///////////////////////// 验证码的消息码定义 ////////////////////////
// 上层的消息码必定是MSG_MODULEID_VERIFY
/******************************************************************/
///////////////////////////////////////////////////////////////////

// 验证分类
enum
{ 
	// 未定义
	ENUM_VERIFY_TYPE_UNKNOW = 0,

	// 场景服务器
	ENUM_VERIFY_TYPE_ZONE,

	// 登录服务器
	ENUM_VERIFY_TYPE_LOGIN,

	// 网关服务器
	ENUM_VERIFY_TYPE_GATEWAY,

	// 最大
	ENUM_VERIFY_TYPE_MAXID
};


// 验证码类型枚举
enum VerifyCodeType
{
	VerifyCodeType_Login = 0,		/// 登录验证用
	VerifyCodeType_Reg,				/// 注册帐号验证用

	VerifyCodeType_Max,				/// 最大称号数
};

// 验证码处理服务器发送给客户端的题目消息
#define MSG_VERIFY_SEND_QUESTION							1

// 验证码处理服务器发送给客户端的题目数据结构
struct SMsgActionVerifyQuestionInfo_YC	
{
	int		nType;				//验证分类
	DWORD	nID;				//题目ID
	int		nIndex;				//第几道题目
	DWORD	nEndTime; 			//题目生命,超过时间就算答错,生成时间+答题时限
	DWORD	nServerTime; 		//服务器时间,用来计算客户端计时
	char	szQuestionText[50];	//题目字串,如: 请问下面图片中有几个“中”字？
	DWORD	nBitMapLens;		//图片BitMap原始数据长度
	DWORD	nLzwLens;			//图片Lzw压缩数据长度
	// BYTE * pData;			//图片Lzw压缩数据,后面加入此数据

	SMsgActionVerifyQuestionInfo_YC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 验证码处理服务器发送给客户端清除答题命令
#define MSG_VERIFY_CLS_QUESTION							2

// 发送给客户端的清除答题命令结构
struct SMsgActionVerifyQuestionCls_YC	
{
	DWORD	nID;				//清除题目ID

	SMsgActionVerifyQuestionCls_YC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 客户端发送给验证码处理服务器答案
#define MSG_VERIFY_CLIENT_ANSWER							3

// 客户端发送给验证码处理服务器的答案数据结构
struct SMsgActionVerifyAnswerInfo_CY	
{
	DWORD	dwPDBID;			//客户端PDBID
	ulong	nClientID;			//客户端ID
	DWORD	nID;				//题目ID
	WORD	nAnswerID;			//题目答案ID : 1,2,3,4

	SMsgActionVerifyAnswerInfo_CY(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 场景服务器发送给验证码处理服务器验证请求
#define MSG_VERIFY_ZONE_REQUEST							4

// 场景服务器发送验证码处理服务器验证请求数据结构
struct SMsgActionVerifyRequestInfo_SY	
{
	DWORD	dwPDBID;			//客户端PDBID
	ulong	nClientID;			//客户端ID
	DWORD	nServerID;			//场景服务器ID
	DWORD	dwlevel;			//难度,0~100,数字大,就更难

	SMsgActionVerifyRequestInfo_SY(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


///////////////////////////////////////////////////////////////////
// 验证码处理服务器发送给场景服务器验证结果
#define MSG_VERIFY_ANSWER_ZONE							5

// 验证码处理服务器发送给场景服务器验证结果
struct SMsgActionVerifyAnswerZoneServer_YS	
{
	DWORD	dwPDBID;			//客户端PDBID
	ulong	nClientID;			//客户端ID
	DWORD	nServerID;			//场景服务器ID
	int nResult;				//验证结果,0:失败,1:成功

	SMsgActionVerifyAnswerZoneServer_YS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 玩家上下线
#define MSG_VERIFY_PERSON_LOGIN							6
// 场景服发送玩家上下线信息
struct SMsgActionVerifyPersonLogin_SY
{
	DWORD	dwPDBID;			//客户端PDBID
	ulong	nClientID;			//客户端ID
	DWORD	nServerID;			//场景服务器ID
	bool	bOffline;			// 离钱/跨场景
	bool	bLoginType;			// 上下线类型，true:上线，false:下线
};




///////////////////////////////////////////////////////////////////
// 登录服务器发送给验证码处理服务器验证请求
#define MSG_VERIFY_LOGIN_REQUEST						7

// 登录服务器发送验证码处理服务器验证请求数据结构
struct SMsgActionVerifyLoginRequestInfo_LY	
{
	DWORD	dwPDBID;			//客户端PDBID
	ulong	nClientID;			//客户端ID
	DWORD	nServerID;			//登录服务器ID
	DWORD	dwlevel;			//难度,0~100,数字大,就更难

	SMsgActionVerifyLoginRequestInfo_LY(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


///////////////////////////////////////////////////////////////////
// 验证码处理服务器发送给登录服务器验证结果
#define MSG_VERIFY_ANSWER_LOGIN							8

// 验证码处理服务器发送给登录服务器验证结果
struct SMsgActionVerifyAnswerLoginServer_YL	
{
	DWORD	dwPDBID;			//客户端PDBID
	ulong	nClientID;			//客户端ID
	DWORD	nServerID;			//登录服务器ID
	int nResult;				//验证结果,0:失败,1:成功

	SMsgActionVerifyAnswerLoginServer_YL(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 发送验证码答错状态给客户端
#define MSG_VERIFY_SEND_VERIFY_DATA						9
// 消息体
struct SMsgVerifySendVerifyData_SC
{
	bool	bFlag;			//已触发验证码状态
	WORD	nKills;			//累计杀怪数量
	DWORD	nExps;			//累计杀怪得到的经验
	WORD	nTryTimes;		//触发后请求验证次数
	WORD	nKillPersons;	//累计杀人
	DWORD	dwResetTime;	//最后清数据时间
	BYTE	nFailTimes;		//验证失败次数

	SMsgVerifySendVerifyData_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 发送验证码答错状态给客户端
#define MSG_VERIFY_SEND_ENERGY_DATA						10

// 导入精力附加数据信息
struct SMsgVerifySendEnergyNode
{
	// 是否读取称号记录
	BYTE	nTypeID;					// 类型ID
	DWORD	dwTimes;					// 时间
	DWORD	dwValue1;					// 数值1
	DWORD	dwValue2;					// 数值2
	DWORD	dwValue3;					// 数值3

	SMsgVerifySendEnergyNode(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

// 消息体
struct SMsgVerifySendEnergyData_SC
{
	BYTE	nCount;		// 记录数

	//所有精力数据 nCount * SMsgVerifySendEnergyNode

	SMsgVerifySendEnergyData_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


///////////////////////////////////////////////////////////////////
// 登陆服务器发送给验证码处理服务器验证请求
#define MSG_VERIFY_TYPE_REQUEST_POPWINDOW						11

// 登陆服务器发送验证码处理服务器验证请求数据结构
struct SMsgActionVerifyTypeRequestInfo
{
	DWORD	dwPDBID;							//客户端PDBID
	ulong	nClientID;							//客户端ID
	char    szVerifyCode[VERIFYCODE_MAXNUM];	//验证码字符串
	int		nType;								//应用类型 VerifyCodeType
	DWORD	dwlevel;							//难度,0~100,数字大,就更难

	SMsgActionVerifyTypeRequestInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
//////////////////////////////////////////////////////////////////////////
// 验证码服务器发送数据给客户端
#define MSG_VERIFYCODE_IMG										12

// 验证码处理服务器发送给客户端的题目数据结构
struct SMsgActionVerifyCodeImg_YC	
{
	int		nType;				//验证分类
	DWORD	nBitMapLens;		//图片BitMap原始数据长度
	DWORD	nLzwLens;			//图片Lzw压缩数据长度

	SMsgActionVerifyCodeImg_YC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////
// 验证码处理服器配置参数信息
struct SVerifyServiceSchemeInfo
{

	int nWaitDelay;				// 服务器端等待答题的时间(秒),超过就算过期,默认为180
	int	nMaxVerifyCount;		// 答错最大次数count,例如为4,算法为count初始为1,答对-1,答错或过期+1,当总数<=0就通过，当总数>=4就不通过，中间数继续答题，自到<=0或>=4,默认为4
								// [汉字编码的范围] 取GB2312汉字编码的范围,第一字节0xB0-0xF7(176-247),第二字节0xA0-0xFE（160-254）,不得超出以上范围
	int	nWordRangeMin1;			// 第一字节开始	,默认为176	
	int nWordRangeMax1;			// 第一字节结束	,默认为247
	int nWordRangeMin2;			// 第二字节开始	,默认为160
	int nWordRangeMax2;			// 第二字节结束	,默认为254
	int	nFontSizeMin;			// 字体大小范围,最小,默认为20
	int nFontSizeMax;			// 字体大小范围,最大,默认为60
	double fNoiseRangeX;		// X震荡幅度,为浮点数,范围:0.0~1.0,指相对字体大小震荡的比例,0.0为不震荡,默认为0.15,用来控制清晰度
	double fNoiseRangeY;		// Y震荡幅度,为浮点数,范围:0.0~1.0,指相对字体大小震荡的比例,0.0为不震荡,默认为0.15,用来控制清晰度
	double fNoiseRate;			// 震荡幅度调节系数,为浮点数,范围:0.0~10.0,指用户指定的难度最大调节幅度,0.0为不调节,默认为0.5,用来控制清晰度
	double fRotary;				// 旋转度,为浮点数,范围:0.0~1.0 ,0.0为不旋转,默认为0.0,建议不旋转,字体变形较厉害,还得小心玩家投诉看旋转字转坏了脖子:)
	int nOffsetX;				// 字位置X方向最大偏移象素个数,默认为40,用来控制粘连字
	int nOffsetY;				// 字位置Y方向最大偏移象素个数,默认为30,用来控制粘连字
	double fOffsetRate;			// 字位置方向最大偏移象素调节系数,为浮点数,范围:0.0~10.0,指用户指定的难度最大调节幅度,0.0为不调节,默认为0.3,用来控制粘连字
	int nWord1X;				// 第一个字的x位置(象素),默认为0
	int nWord2X;				// 第二个字的x位置(象素),默认为80
	int nWordRowSpace;			// 字行间距离(象素),默认为50
	int nTimeoutDelay;			// [20秒] 服务器过期延迟多少秒,避免中间途中的是时间损耗,导致客户端未到时间就被判过期
	int nMaxOnline;				// 最大在线题目数,当到最大数时,不接收请求,默认为2000
	int nStartX;				// 开始写第一个字的X坐标
	int nStartY;				// 开始写第一个字的X坐标
	int nlineType;				// 线类型画虚线0表示画实线，1表示画虚线
	char szFontFaceName[64];	// 字体名称,必须为TrueType矢量字体,默认为"宋体"
	SVerifyServiceSchemeInfo(void)
	{
		Init();
	};

	void Init(void)
	{
		memset(this, 0, sizeof(*this));

		nWaitDelay		= 180;		// 服务器端等待答题的时间(秒),超过就算过期,默认为180
		nMaxVerifyCount = 4;		// 答错最大次数count,例如为4,
		nWordRangeMin1	= 176;		// 第一字节开始	,默认为176	
		nWordRangeMax1	= 247;		// 第一字节结束	,默认为247
		nWordRangeMin2	= 160;		// 第二字节开始	,默认为160
		nWordRangeMax2	= 245;		// 第二字节结束	,默认为254
		nFontSizeMin	= 20;		// 字体大小范围,最小,默认为20
		nFontSizeMax	= 60;		// 字体大小范围,最大,默认为60
		fNoiseRangeX	= 0.15;		// X震荡幅度,为浮点数,范围:0.0~1.0,默认为0.15
		fNoiseRangeY	= 0.15;		// Y震荡幅度,为浮点数,范围:0.0~1.0,默认为0.15
		fNoiseRate		= 0.5;		// 震荡幅度调节系数,,范围:0.0~10.0,默认为0.5
		fRotary			= 0.0;		// 旋转度,为浮点数,范围:0.0~1.0 ,0.0为不旋转,默认为0.0
		nOffsetX		= 40;		// 字位置X方向最大偏移象素个数,默认为40
		nOffsetY		= 30;		// 字位置Y方向最大偏移象素个数,默认为30
		fOffsetRate		= 0.3;		// 字位置方向最大偏移象素调节系数,范围:0.0~10.0,默认为0.3
		nWord1X			= 0;		// 第一个字的x位置(象素),默认为0
		nWord2X			= 80;		// 第二个字的x位置(象素),默认为80
		nWordRowSpace	= 50;		// 字行间距离(象素),默认为50
		nTimeoutDelay	= 20;		// [20秒] 服务器过期延迟多少秒,避免中间途中的是时间损耗,导致客户端未到时间就被判过期
		nMaxOnline		= 2000;		// 最大在线题目数,当到最大数时,不接收请求,默认为2000
		nStartX			= 0;		// 开始写第一个字的X坐标
		nStartY			= 0;		// 开始写第一个字的X坐标
		nlineType		= 0;		// 线类型画虚线0表示画实线，1表示画虚线
		sstrcpyn(szFontFaceName,"宋体",sizeof(szFontFaceName));	// 字体名称,必须为TrueType矢量字体,默认为"宋体"
	};
};

//////////////////////////////////////////////////////////////////////////

// 所有的有益状态信息
struct SVerifyServerSchemeBuffPrizeInfo
{
	int  nBuffID;							// 有益状态ID
	int  nBuffLevel;						// 有益状态的等级
	char szDesc[256];						// 状态说明

	SVerifyServerSchemeBuffPrizeInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 验证码服端配置参数信息
struct SVerifyServerSchemeInfo
{

	int nEnabledVerify;					// 是否启用验证码 1:是,0:否
	int nStartVerifyDelay;				// 创建人物时如果有验证状态，多少秒后请求验证,为0不请求验证,默认为20秒
	int nCheckAddVerifyDelay;			// 补发一次验证,直到触发验证状态没了的时间间隔 多少秒,为0不补发,,默认为600秒,主要是防已触发时验证服务器没开等原因导致已触发,但验证码服务器没处理,
										// 客户端一直得不到验证,一定要大于验证码处理服务器参数中的[服务器端等待答题的时间]*[答错最大次数count],否则还没答完题,可能被就补发验证
	int nEexpeParam;					// 当获得的经验值达到玩家等级的expeparam倍时触发验证（为0忽略）
	int nMonsterParam;					// 当杀死的怪物数量达到monsterparam时触发验证（为0忽略）
	int nOldFailPunishID;				// 玩家上次下线前回答错误（或未回答）的惩罚状态号
	int nOldFailPunishLevel;			// 惩罚状态OldFailPunish的等级
	int nOpenBuffPrize;					//  1:表示开启有益状态奖励 0:表示关闭
	int nBuffPrizeListCount;			// 有益状态列表个数
	int nVipCardID;						// 免死金牌物品ID

	int nOpenExpPrize;					// 开放经验奖励 0.不 1.是
	int nGiveExpBase;					// 经验奖励基数 1200
	int nGiveExpRate;					// 经验奖励系数 90
	char szGiveExpMsg[256];				// 给经验奖励时提示

	int nMaxKillPerson;					// 非法杀到多少人就加红名,0为不限
	int nPunishPKNum;					// 触发加红名后，给加多少PK值
	int nPunishCatch;					// 触发加红名后，是否送入大牢,1:是,0:否
	int nCatchMin;						// 送入大牢关多少分钟,10分钟

	SVerifyServerSchemeBuffPrizeInfo BuffPrizeList[VERIFYSERVER_BUFFPRIZEMAXINDEX];	// 所有的有益状态列表,子节点中是所有的状态信息

	SVerifyServerSchemeInfo(void)
	{
		Init();
	};

	void Init(void)
	{
		memset(this, 0, sizeof(*this));

		nEnabledVerify			= 1;		// 是否启用验证码 1:是,0:否
		nStartVerifyDelay		= 20;		// 创建人物时如果有验证状态，多少秒后请求验证,为0不请求验证,默认为20秒
		nCheckAddVerifyDelay	= 600;		// 补发一次验证,的时间间隔 多少秒,为0不补发,,默认为600秒
		nEexpeParam				= 100;		// 当获得的经验值达到玩家等级的expeparam倍时触发验证（为0忽略）
		nMonsterParam			= 10;		// 当杀死的怪物数量达到monsterparam时触发验证（为0忽略）
		nOldFailPunishID		= 0;		// 玩家上次下线前回答错误（或未回答）的惩罚状态号
		nOldFailPunishLevel		= 0;		// 惩罚状态OldFailPunish的等级
		nOpenBuffPrize			= 0;		//  1:表示开启有益状态奖励 0:表示关闭
		nBuffPrizeListCount		= 0;		// 有益状态列表个数
		nVipCardID				= 2000;		// 免死金牌物品ID

		nOpenExpPrize			= 1;		// 开放经验奖励 0.不 1.是
		nGiveExpBase			= 1200;		// 经验奖励基数 1200
		nGiveExpRate			= 90;		// 经验奖励系数 90

		nMaxKillPerson			= 0;		// 非法杀到多少人就加红名,0为不限
		nPunishPKNum			= 0;		// 触发加红名后，给加多少PK值
		nPunishCatch			= 0;		// 触发加红名后，是否送入大牢,1:是,0:否
		nCatchMin				= 0;		// 送入大牢关多少分钟,10分钟

	}
};


#pragma pack()