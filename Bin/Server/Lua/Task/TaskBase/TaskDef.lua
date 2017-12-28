-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	易耀辉
-- 日  期:	3/20/2017
-- 版  本:	1.0
-- 描  述:	任务定义
-------------------------------------------------------------------
-- 任务类型
TaskType =
{
	FirstWinType	= 1,				-- 首胜任务
	ClanType		= 2,				-- 联盟任务
	CycleType		= 3,				-- 循环任务
	MainType		= 4,				-- 主线任务
	ContacType		= 5,				-- 交互任务
	DailyType		= 6,				-- 日常任务
	RankType		= 7,				-- 排位任务
}

-- 任务状态
TaskState =
{
	BeginState		= 0,				-- 起始
	AcceptState		= 1,				-- 接受
	FinishState		= 2,				-- 完成
	EndState		= 3,				-- 已领取
}

-- 任务数据触发事件
TaskGameEventType =
{
	EventType_LEVEL 			= 1,			-- 玩家等级变化事件
	EventType_FINISHMATCH		= 2,			-- 完成一场比赛
	EventType_ADDHERO			= 3,			-- 获得一个永久英雄
	EventType_ADDSKIN			= 4,			-- 获得一个永久皮肤
	EventType_OPENCHEST			= 5,			-- 开启宝箱
	EventType_INLAYGEM			= 6,			-- 镶嵌宝石
	EventType_HEROUPGRADE		= 7,			-- 英雄晋级
	EventType_BUIDMENTORSHIP	= 8,			-- 建立师徒关系
	EventType_CLANDONATE		= 9,			-- 联盟捐赠
	EventType_APPLYFRIEND		= 10,			-- 发起好友申请
	EventType_CONTACTREPLY		= 11,			-- 回复交互任务
	EventType_PK				= 12,			-- PK
	EventType_NovideGuide		= 13,			-- 新手引导
}

