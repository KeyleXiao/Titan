/*******************************************************************
** 文件名:	VOCATION_ManagedDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-5-16
** 版  本:	1.0
** 描  述:	职业相关托管公共定义

			
********************************************************************/

#ifndef __COMMON_VOCATION_MANAGED_DEF_H__
#define __COMMON_VOCATION_MANAGED_DEF_H__

#include "ManagedCode.h"


/// <summary>
/// 职业
/// </summary>
ENUM VOCATION
{
	DESC("随机")
	VOCATION_RANDOM = -1,

	DESC("无效")
	VOCATION_INVALID = 0,

	DESC("剑魔")
	VOCATION_JIANMO,

	DESC("耶哥蕊特")
	VOCATION_YZRT,

	DESC("梅丽珊卓")
	VOCATION_MLSZ,

	DESC("自然之灵")
	VOCATION_ZRZL,

	DESC("钢铁之心")
	VOCATION_GTZX,

	DESC("冰霜女神")
	VOCATION_BSNS,

	DESC("提里昂")
	VOCATION_TILIANG,

	DESC("卓戈")
	VOCATION_ZHUOGE,

	DESC("魔山")
	VOCATION_MOSHAN,

    DESC("红毒蛇")
    VOCATION_SNAKE,

    DESC("俳雅菩厉")
    VOCATION_FYPL,
    
    DESC("索罗斯")
    VOCATION_SUOLS,

    DESC("劳勃")
    VOCATION_NAOBO,

    DESC("艾莉亚")
    VOCATION_AILIYA,

    DESC("尸鬼")
    VOCATION_SHIGUI,

    DESC("布兰登")
    VOCATION_BULANDENG,

    DESC("绿先知")
    VOCATION_LVXIANZHI,

    DESC("阿莎")
    VOCATION_ASHA,

    DESC("丹妮莉丝")
    VOCATION_DNLS,

    DESC("诺格")
    VOCATION_NUOGE,

    DESC("巴格")
    VOCATION_BAGE,

    DESC("迪迪")
    VOCATION_DIDI,

    DESC("乌妮")
    VOCATION_WUNI,

    DESC("法拉斯")
    VOCATION_FALASI,

    DESC("时光旅行者")
    VOCATION_SGLXZ,

    DESC("异鬼")
    VOCATION_YIGUI,

    DESC("李小龙")
    VOCATION_LIXIAOLONG,

    DESC("矮人火炮手")
    VOCATION_ARHPS,

    DESC("地精炸弹人")
    VOCATION_DJZDR,

    DESC("北境游神")
    VOCATION_BJYS,

    DESC("新手卓格")
    VOCATION_PRIMARY_ZHUOGE,

    DESC("御魂游神")
    VOCATION_YHYS,

    DESC("芳野鹿灵")
    VOCATION_FYLL,

    DESC("奥术魔导士")
    VOCATION_ASMDS,

    DESC("绝地战鹰")
    VOCATION_JDZY,

    DESC("银河战警")
    VOCATION_YHZJ,

    DESC("黑夜女王")
    VOCATION_HYNW,

    DESC("末日战甲")
    VOCATION_MRZJ,

    DESC("邪能恶魔")
    VOCATION_XNEM,

    DESC("傀儡双骄")
    VOCATION_KLSJ,

    DESC("契约者")
    VOCATION_QYZ,

    DESC("战争祸源")
    VOCATION_ZZHY,

	DESC("最大职业")
	VOCATION_MAX,
};


/// <summary>
/// 职业类型
/// </summary>
ENUM VOCATION_TYPE
{
    VOC_ZHANSHI,            // 战士
    VOC_FASHI,              // 法师
    VOC_CIKE,               // 刺客
    VOC_FUZU,               // 辅助
    VOC_ADC,                // 输出
    VOC_MAX,                // 最大类型
};


/// <summary>
/// 操作模式
/// </summary>
ENUM PLAY_MODE
{
    PLAY_MODE_DEFAULT,       // 默认游戏操作
    PLAY_MODE_FPS,           // FPS操作
    PLAY_MODE_MAX,           // 最大类型
};

/// <summary>
/// 英雄特征（1近战2远程4物理8法术）
/// </summary>
ENUM HERO_TRAITS
{
    HERO_MELEE   = 0x1,        // 近战
    HERO_DISTANT = 0x2,        // 远程
    HERO_PHYSICS = 0x4,        // 物理
    HERO_MAGIC   = 0x8,        // 法术
};

#endif	// __COMMON_VOCATION_MANAGED_DEF_H__