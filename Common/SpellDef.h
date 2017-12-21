/*******************************************************************
** 文件名:	SpellDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	1/8/2015
** 版  本:	1.0
** 描  述:	

技能系统定义
********************************************************************/

#pragma once

#include "SpellDef_ManagedDef.h"

#ifndef _MANAGED
#include "ICondition.h"
#include "IEffect.h"
#include <set>
namespace SPELL
{
	// 技能配置脚本
	#define SPELL_FILENAME						"Scp\\Spell.bin"
	// 技能逻辑脚本
	#define SPELL_LOGIC_FILENAME				"Scp\\SpellLogic.csv"
	// 技能配置校验
	#define CHECK_SPELL_DATA					"SPELL_DATA"
	// 技能浮点数相关值(数据全部用int表示,int值除此值就是需要float值,float乘此值就是int值，天赋增加值需特别注意)
	#define SPELL_FLOAT_VALUE					100.0f
	// 飞行技能默认速度(防止配0，影响逻辑，默认值)
	#define SPELL_DEFAULT_SPEED					10.0f
	// 条件无效值(有些技能无条件,但有效果)
	#define CONDITION_INVALID					0
	// 效果无效值(有些技能无效果,但有条件)
	#define EFFECT_INVALID						0
	// 打断冷却时间
	#define SPELL_BREAK_COOL_TIME				5000

	// 技能槽位显示数量
	#define SPELL_SLOT_SHOW_COUNT				4
	// 技能槽位最大数量
	#define SPELL_SLOT_MAX_COUNT				30
	// 技能槽位脚本
	#define SPELL_SLOT_FILENAME					"Scp\\SpellSlot.csv"

	// 技能标志位最大值
	#define	SPELL_FLAG_MAX						4
	// 效果扩大倍率(float默认扩大倍数)
	#define EFFECT_RATE							100.0f

    // 翻滚技能起始槽位
    #define ROLL_SPELL_START_SLOT_INDEX		6

    // 扇形技能起始一小块去掉
    #define SPELL_SECTOR_START_LEN     1.5f

	// 技能执行/学习结果
	enum SpellResult
	{
		SpellResult_Success = 0,		// 成功
		SpellResult_IDError,			// ID错误
		SpellResult_Busying,			// 繁忙中
		SpellResult_NoMp,				// MP不够
		SpellResult_NoStamina,			// 体力不够
		SpellResult_Energy,				// 能量不够
		SpellResult_NoTarget,			// 没有目标
		SpellResult_Cooling,			// 冷却中
		SpellResult_Far,				// 距离太远
        SpellResult_OverlayNoUse,		// 累积技能不可使用
		SpellResult_Unknow,				// 未知错误
		SpellResult_Max,
	};

	// 技能施法失败结果
	enum SpellCastFailResult
	{
		SpellCastFailResult_Normal = 0,		// 正常
		SpellCastFailResult_Silence,		// 技能封魔
        SpellCastFailResult_Condition,		// 条件不足
        SpellCastFailResult_CountLimit,		// 使用次数限制
        SPellCastFailResult_LocVerify,      // 位置校验失败
		SpellCastFailResult_Max,
	};

	enum SpellIndexType
	{
		SpellIndex_Unknow=0,
		SpellIndex_Int,
		SpellIndex_Str,
	};

	// 返回技能索引类型
	static int getSpellIndexType(int nIndex)
	{
		SPELL_DATA temp;
		if ( temp.getStr( nIndex )==0 )
			return SpellIndex_Int;
		else
			return SpellIndex_Str;
	}

	struct SPELL_TALENT                                
    {
        struct Element
        {
            void* pTalentEffect;  // 天赋效果
            void* pData;          // 额外数据（技能条件或效果）
            Element(){memset(this, 0, sizeof(*this));}
        };
        int nTalentID;                                 // 天赋ID 
        std::vector<Element> vecElements;              // 效果列表

        SPELL_TALENT() : nTalentID(0)
        {
            vecElements.clear();
        }
	};

	struct SPELL_SLOT
	{
		char			szImage[32];								// 图片
		char			szVocation[32];								// 职业
		char			szLocate[32];								// 定位
		int				nHp;										// 生命
		int				nPA;										// 物理攻击
		int				nMA;										// 魔法攻击
		int				nOperateFactor;								// 操作系数难度
		char			szDescription[256];							// 描述
		int				nCount;										// 槽位技能数量
		int				nSpellID[SPELL_SLOT_MAX_COUNT];				// 槽位技能ID
        char            nSpellDesc[SPELL_SLOT_SHOW_COUNT][256];     // 槽位技能描述（5个，目前只用于大厅人物技能描述）
		SPELL_SLOT()
		{
			memset(this, 0, sizeof(*this));
		}
	};

    struct SSmartCastKey
    {
        int		nSlotType;		// 槽位类型	(1、装备 2、技能)
        int		nSlotIndex;		// 槽索引
		int        nHeroID;          //英雄职业ID
        SSmartCastKey()
        {
            memset(this, 0, sizeof(*this));
        }

        bool operator<(const SSmartCastKey& other) const
        {
            if (nSlotType != other.nSlotType)
            {
                return nSlotType < other.nSlotType;
            }

            if (nSlotIndex != other.nSlotIndex)
            {
                return nSlotIndex < other.nSlotIndex;
            }

			if (nHeroID != other.nHeroID)
			{
				return nHeroID < other.nHeroID;
			}

            return false;
        }
    };
}

#endif // _MANAGED