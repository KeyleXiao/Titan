/*******************************************************************
** 文件名:	Property.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			属性相关定义
            所有属性字段定义，和序列化标识都必须在此文件中声明或定义
********************************************************************/

#pragma once

#include "ISerializable.h"
#include "TypeName.h"
#include "Entity_ManagedDef.h"

// 属性集中返回的无效整数
#define  MAX_INT            (2147483647)
#define  MIN_INT            (-2147483647 - 1)
#define  MAX_SHORT          (32767)
#define  INVALID_INT_VALUE  (-2147483647 - 1)

#pragma pack(1)

struct DataChunkPropertyDeclare
{
    char             szType[16];	// 类型
    int              nDataOffset;	// 属性数据偏移
    int              len;			// 属性数据长度
    int              nMinLimit;		// 最小值限定
    int              nMaxLimit;		// 最大值限定
    int              nFlag;			// 序列化标签,参见ISerializable.h

    DataChunkPropertyDeclare() { memset( this,0,sizeof(DataChunkPropertyDeclare) ); }
};

// offsetof 宏实现
#define		GET_OFFSET(C,member) (int)(&((C*)0)->member);

#define     DATA_CHUNK_DECLARE(DATA)                                    \
    DataChunkPropertyDeclare EntityProperty<DATA>::s_PropertyDeclare[]; \
    extern "C" \
    { \
        __declspec(dllexport) DataChunkPropertyDeclare DATA##_Declare[];\
    }

#define		DECLARE_PROPERTY( DATA,id,member,minLimit,maxLimit,serializeFlag)     \
    if ( id<0 || id>=nDeclareLen || s_PropertyDeclare[id].len>0 ) {ErrorLn("DECLARE_PROPERTY Error!!!!!!!!!");} else{\
    s_PropertyDeclare[id].nDataOffset =GET_OFFSET(DATA,member);   \
    s_PropertyDeclare[id].len =sizeof(m_data.member);             \
    sstrcpyn(s_PropertyDeclare[id].szType,TypeName(m_data.member),sizeof(s_PropertyDeclare[id].szType)); \
    s_PropertyDeclare[id].nMinLimit =minLimit;                    \
    s_PropertyDeclare[id].nMaxLimit =maxLimit;					  \
    s_PropertyDeclare[id].nFlag = (serializeFlag); };

//////////////////////////////////////////////////////////////////////////
// 属性buffer内部相关处理函数
//////////////////////////////////////////////////////////////////////////
extern "C"
{
#ifdef ENTITY_EXPORTS
extern __declspec(dllexport) DataChunkPropertyDeclare PlayerRoleProperty_Declare[PROPERTY_MAX];
extern __declspec(dllexport) DataChunkPropertyDeclare MonsterProperty_Declare[PROPERTY_MAX];
extern __declspec(dllexport) DataChunkPropertyDeclare TankProperty_Declare[PROPERTY_MAX];
#else
extern __declspec(dllimport) DataChunkPropertyDeclare PlayerRoleProperty_Declare[PROPERTY_MAX];
extern __declspec(dllimport) DataChunkPropertyDeclare MonsterProperty_Declare[PROPERTY_MAX];
extern __declspec(dllimport) DataChunkPropertyDeclare TankProperty_Declare[PROPERTY_MAX];
#endif
};

static inline void Propery_DeclareCopy( DataChunkPropertyDeclare * dest,DataChunkPropertyDeclare * src, size_t size )
{
    for (size_t i=0;i<size;++i )
    {
        dest[i] = src[i];
    }
}

static inline bool Propery_GetBuffer( void * data,DataChunkPropertyDeclare * declares,int flag,rkt::obuf & out )
{
    for ( int i=0;i<PROPERTY_MAX;++i )
    {
        if ( declares[i].len>0 && (declares[i].nFlag & flag) )
        {
            out.push_back( ((BYTE*)data)+declares[i].nDataOffset,declares[i].len );
            if ( out.fail() )
            {
                ErrorLn("property =" << i << ", offset=" <<  declares[i].nDataOffset << ", len=" << declares[i].len);
                return false;
            }
        }
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////
// 角色属性
//////////////////////////////////////////////////////////////////////////
// 玩家基础属性(需要存储到数据库的属性)
struct RoleProperty_Base
{
	int         nPDBID;			// 角色ID
    char		szName[32];
    int         nSceneID;       // 所在地图
    Vector3     location;       // 在地图上的位置
    Vector3     angle;          // 朝向
    Vector3     velocity;       // 瞬时速度
    short       nVocation;		// 预设英雄,玩家职业
    BYTE		bySex;			// 性别
    int			nSkin;			// 皮肤 
    short       nLevel;		    // 等级  
    int         nExp;			// 经验
    int			nHP;		    // 血量
    int			nMaxHP;		    // 最大血量
    int			nMP;		    // 法术值
    int			nMaxMP;		    // 最大法术值
    int         nMoney;         // 金钱
    unsigned char byCamp;       // 阵营
    int         nKinID;         // 战队ID
    int         nClanID;        // 帮会ID
    int         nFightCapacity;   // 战力
    int         nGold;          // 金币
    int         nBindGold;      // 绑金
    int         nTicket;        // 点券
    int         nFaceID;        // 头像ID
    int         nTeamID;        // 队伍ID

    BYTE        byMatchType;     // 排位段位信息 类型nMatchTypeID = 8  
    int         nRankScore;
    int         nRankGrade;
    BYTE        byObserver;     // 是否是观察者
    uint16      nVisibleFlag;   // 对各个阵营可见性标志掩码
    int         nRankKingOrder; // 王者排位排名


    //TODO 新增属性动态障碍

    RoleProperty_Base() { memset(this,0,sizeof(RoleProperty_Base)); }
};

// 这些属性不保存数据库
struct PlayerRoleProperty : public RoleProperty_Base
{
    UID         uid;            // 角色UID
    UID         uidMaster;      // 主人UID(战场下线玩家可以被其他玩家控制)
    int         nAttack;        // 攻击力
    int         nDefence;       // 防御力
    int			nShield;        // 护盾值（额外生命）
	int			nStamina;       // 体力
	int			nMaxStamina;	// 最大体力
	int			nMagicType;		// 魔法类型
	int			nEnergy;		// 能量值
	int			nMaxEnergy;		// 最大能量值
	int         nTalentPoint;   // 天赋点数
	int         nMask;		    // 标志位	ENTITY_MASKFLAG
    int         nObstacleFlag;  // 可否穿越动态障碍标志位
    int         nFormType;      // 立足点类型 FORM_TYPE

    int			nPA;			// 物理攻击
    int			nMA;			// 法术攻击
    int			nPD;			// 物理防御
    int			nMD;			// 法术防御
    int         nSD;			// 移动速度
    int         nPASD;			// 攻击速度
    int         nAF;			// 普攻系数
	int			nCRC;			// 暴击概率
	int			nHpReply;		// 血量回复
	int			nMpReply;		// 魔法回复

	int			nCRD;			// 暴击伤害
	int			nPDP;			// 物理防御穿透
	int			nPCTPDP;		// 百分比物理防御穿透
	int			nMDP;			// 法术防御穿透
	int			nPCTMDP;		// 百分比法术防御穿透
	int			nPCTImmunePA;	// 百分比物理伤害减免
	int			nPCTImmuneMA;	// 百分比法术伤害减免
	int			nPCTCoolReduce; // 百分比冷却缩减
	int			nPCTPDrains;	// 百分比物理吸血
	int			nPCTMDrains;	// 百分比法术吸血
	int			nTenacity;		// 韧性
    int         nAimDistance;   // 瞄准距离

    int			nBaseHP;		    // 基础血量
    int			nBaseMP;		    // 基础魔法
    int         nBaseEnergy;        // 基础能量
    int			nBasePA;		    // 基础物理攻击
    int			nBaseMA;		    // 基础法术攻击
    int			nBasePD;		    // 基础物理防御
    int			nBaseMD;		    // 基础法术防御
    int			nBaseSD;		    // 基础移动速度
    int			nBasePASD;		    // 基础攻击速度
    int			nBaseAF;		    // 基础普攻系数
	int			nBaseCRC;		    // 基础暴击概率
	int			nBaseHpReply;	    // 基础血量回复
	int			nBaseMpReply;	    // 基础魔法回复
    int         nBaseEnergyReply;   // 基础能量回复

    int			nAppendHP;		    // 附加血量
    int			nAppendMP;		    // 附加魔法
    int         nAppendEnergy;      // 附加能量
    int			nAppendPA;		    // 附加物理攻击
    int			nAppendMA;		    // 附加法术攻击
    int			nAppendPD;		    // 附加物理防御
    int			nAppendMD;		    // 附加法术防御
    int			nAppendSD;		    // 附加移动速度
    int			nAppendPASD;	    // 附加攻击速度
    int			nAppendAF;	        // 附加普攻系数
	int			nAppendCRC;		    // 附加暴击概率
	int			nAppendHpReply;	    // 附加血量回复
	int			nAppendMpReply;	    // 附加魔法回复
    int         nAppendEnergyReply; // 附加能量回复

    int			nAppendPCTHP;		    // 附加百分比血量
    int			nAppendPCTMP;		    // 附加百分比魔法
    int         nAppendPCTEnergy;       // 附加百分比能量
    int			nAppendPCTPA;		    // 附加百分比物理攻击
    int			nAppendPCTMA;		    // 附加百分比法术攻击
    int			nAppendPCTPD;		    // 附加百分比物理防御
    int			nAppendPCTMD;		    // 附加百分比法术防御
    int			nAppendPCTSD;		    // 附加百分比移动速度
    int			nAppendPCTPASD;		    // 附加百分比攻击速度
    int			nAppendPCTAF;		    // 附加百分比普攻系数
	int			nAppendPCTCRC;		    // 附加百分比暴击概率
	int			nAppendPCTHpReply;	    // 附加百分比血量回复
	int			nAppendPCTMpReply;	    // 附加百分比魔法回复
    int         nAppendPCTEnergyReply;  // 附加百分比能量回复
    int         nAppendPCTPDP;          // 附加额外百分比护甲穿透
    int         nAppendPCTMDP;          // 附加额外百分比魔抗穿透

	int			nBasePCTCoolReduce;	    // 基础百分比冷却缩减
    int			nBaseEnacity;		    // 基础韧性

	int         nMonneyCoefficient;	// 额外金币系数
	int         nExpCoefficient;	// 额外经验系数

    int         nBaseDamageBonus;	// 基础伤害加成(固定值)
    int         nBaseDamageBonusPCT;// 基础伤害加成百分比
    int         nDamageBonus;	    // 伤害加成(固定值)
    int         nDamageBonusPCT;    // 伤害加成百分比

    int         nControlInmune;     // 控制免疫(减少控制时间)
    int         nAISchemeID;        // AI配置ID
    int			nPromoteShield;     // 提升护盾
	int			nBulletCount;		// 子弹数量

	int			nPCTAF;		// 百分比普攻系数
    

    PlayerRoleProperty() { memset(this,0,sizeof(PlayerRoleProperty)); }

    // 对序列化时需要用到的数据进初始化
    static void serialize_init( DataChunkPropertyDeclare * s_PropertyDeclare,size_t nDeclareLen )
    {
        static PlayerRoleProperty m_data;

        // 因为c++没有反射机制,所以需要手动设置
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_ID, nPDBID,0,MAX_INT, TO_PUBLIC );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_NAME,szName,0,0, TO_PUBLIC );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_SCENEID,nSceneID,0,MAX_SHORT, TO_PRIVATE );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_LOCATION,location,0,0, TO_DATABASE|TO_OTHER_SEVER );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_ANGLE,angle,0,0, TO_DATABASE|TO_OTHER_SEVER );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_VELOCITY,velocity,0,0,TO_INVALID );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_VOCATION,nVocation,VOCATION_JIANMO,VOCATION_MAX-1, TO_PUBLIC );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_SEX,bySex,SEX_MALE,SEX_MAX-1, TO_PUBLIC );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_SKIN,nSkin,0,MAX_INT, TO_PUBLIC );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_LEVEL,nLevel,0,MAX_SHORT, TO_PUBLIC|TO_BACKUP );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_EXP,nExp,0,MAX_SHORT, TO_PRIVATE|TO_BACKUP );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_HP,nHP,0,MAX_INT, TO_PUBLIC );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_MAX_HP,nMaxHP,0,MAX_INT, TO_PUBLIC );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_MP,nMP,0,MAX_INT, TO_PUBLIC );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_MAX_MP,nMaxMP,0,MAX_INT, TO_PUBLIC );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_TICKET,nTicket,0,MAX_INT, TO_PRIVATE);
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_CAMP,byCamp,0,255,TO_PUBLIC );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_KIN,nKinID,0,MAX_INT,TO_PUBLIC|ACTOR_TO_HERO|HERO_TO_ACTOR );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_CLAN,nClanID,0,MAX_INT,TO_PUBLIC|ACTOR_TO_HERO|HERO_TO_ACTOR );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_FIGHTCAPACITY, nFightCapacity,0,MAX_INT,TO_PUBLIC );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BINDGOLD, nBindGold,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_GOLD, nGold,0,MAX_INT,TO_PRIVATE );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_FACEID, nFaceID,0,MAX_INT,TO_PUBLIC );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_TEAMID, nTeamID,0,MAX_INT, TO_OTHER_SEVER|TO_CLIENT_SELF|TO_CLIENT_OTHER|ACTOR_TO_HERO|HERO_TO_ACTOR);
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_MATCHTYPE, byMatchType,0,255,TO_OTHER_SEVER | TO_CLIENT_SELF | TO_CLIENT_OTHER);
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_RANKSCORE, nRankScore,0,MAX_INT,TO_OTHER_SEVER | TO_CLIENT_SELF | TO_CLIENT_OTHER);
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_RANKGRADE, nRankGrade,0,MAX_INT,TO_OTHER_SEVER | TO_CLIENT_SELF | TO_CLIENT_OTHER);
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_KINGRANK_ORDER, nRankKingOrder,0,MAX_INT,TO_OTHER_SEVER | TO_CLIENT_SELF | TO_CLIENT_OTHER);
        

        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_UID,uid,0,MAX_INT,TO_CLIENT_SELF|TO_CLIENT_OTHER );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_MASTER_UID,uidMaster,0,MAX_INT,TO_CLIENT_SELF|TO_CLIENT_OTHER );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_ATTACK,nAttack,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_DEFENCE,nDefence,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_SHIELD,nShield,0,MAX_INT,TO_CLIENT_SELF|TO_CLIENT_OTHER );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_STAMINA,nStamina,0,MAX_INT,TO_CLIENT_SELF | TO_CLIENT_OTHER);
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_MAX_STAMINA,nMaxStamina,0,MAX_INT,TO_CLIENT_SELF | TO_CLIENT_OTHER)
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_MAGIC_TYPE,nMagicType,0,MAX_INT,TO_PUBLIC )
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_ENERGY,nEnergy,0,MAX_INT,TO_PUBLIC )
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_MAX_ENERGY,nMaxEnergy,0,MAX_INT,TO_PUBLIC )
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_TALENT_POINT,nTalentPoint,0,MAX_INT,TO_CLIENT_SELF|TO_OTHER_SEVER|TO_DATABASE )	
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_MASK,nMask,0,MAX_INT,TO_PUBLIC )
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_OBSFLAG,nObstacleFlag,0,MAX_INT,TO_PUBLIC )
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_FORMTYPE,nFormType,0,MAX_INT,TO_PUBLIC )

        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_PA,nPA,0,MAX_INT,TO_CLIENT_SELF | TO_CLIENT_OTHER);
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_MA,nMA,0,MAX_INT,TO_CLIENT_SELF | TO_CLIENT_OTHER);
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_PD,nPD,0,MAX_INT,TO_CLIENT_SELF | TO_CLIENT_OTHER);
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_MD,nMD,0,MAX_INT,TO_CLIENT_SELF | TO_CLIENT_OTHER);
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_SD,nSD,0,MAX_INT,TO_CLIENT_SELF | TO_CLIENT_OTHER);
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_PASD,nPASD,0,MAX_INT,TO_CLIENT_SELF|TO_CLIENT_OTHER );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_AF,nAF,0,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_CRC,nCRC,0,MAX_INT,TO_CLIENT_SELF | TO_CLIENT_OTHER);
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_HP_REPLY,nHpReply,0,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_MP_REPLY,nMpReply,0,MAX_INT,TO_CLIENT_SELF );

		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_CRD,nCRD,0,MAX_INT,TO_CLIENT_SELF )
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_PDP,nPDP,0,MAX_INT,TO_CLIENT_SELF )
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_PCT_PDP,nPCTPDP,MIN_INT,MAX_INT,TO_CLIENT_SELF )
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_MDP,nMDP,0,MAX_INT,TO_CLIENT_SELF )
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_PCT_MDP,nPCTMDP,MIN_INT,MAX_INT,TO_CLIENT_SELF )
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_PCT_PAIMM,nPCTImmunePA,MIN_INT,MAX_INT,TO_CLIENT_SELF|TO_CLIENT_OTHER )
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_PCT_MAIMM,nPCTImmuneMA,MIN_INT,MAX_INT,TO_CLIENT_SELF|TO_CLIENT_OTHER )
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_PCT_COOL_REDUCE,nPCTCoolReduce,MIN_INT,MAX_INT,TO_CLIENT_SELF | TO_CLIENT_OTHER)
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_PCT_PDRAINS,nPCTPDrains,MIN_INT,MAX_INT,TO_CLIENT_SELF )
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_PCT_MDRAINS,nPCTMDrains,MIN_INT,MAX_INT,TO_CLIENT_SELF )
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_TENACITY,nTenacity,0,MAX_INT,TO_CLIENT_SELF )
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_AIM_DISTANCE,nAimDistance,0,MAX_INT,TO_CLIENT_SELF )

        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_HP,nBaseHP,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_MP,nBaseMP,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_ENERGY,nBaseEnergy,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_PA,nBasePA,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_MA,nBaseMA,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_PD,nBasePD,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_MD,nBaseMD,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_SD,nBaseSD,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_PASD,nBasePASD,0,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_AF,nBaseAF,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_CRC,nBaseCRC,0,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_HP_REPLY,nBaseHpReply,0,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_MP_REPLY,nBaseMpReply,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_ENERGY_REPLY,nBaseEnergyReply,0,MAX_INT,TO_CLIENT_SELF );

        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_HP,nAppendHP,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_MP,nAppendMP,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_ENERGY,nAppendEnergy,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PA,nAppendPA,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_MA,nAppendMA,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PD,nAppendPD,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_MD,nAppendMD,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_SD,nAppendSD,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PASD,nAppendPASD,MIN_INT,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_AF,nAppendAF,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_CRC,nAppendCRC,MIN_INT,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_HP_REPLY,nAppendHpReply,MIN_INT,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_MP_REPLY,nAppendMpReply,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_ENERGY_REPLY,nAppendEnergyReply,0,MAX_INT,TO_CLIENT_SELF );

        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PCT_HP,nAppendPCTHP,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PCT_MP,nAppendPCTMP,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PCT_ENERGY,nAppendPCTEnergy,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PCT_PA,nAppendPCTPA,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PCT_MA,nAppendPCTMA,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PCT_PD,nAppendPCTPD,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PCT_MD,nAppendPCTMD,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PCT_SD,nAppendPCTSD,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PCT_PASD,nAppendPCTPASD,MIN_INT,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PCT_AF,nAppendPCTAF,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PCT_CRC,nAppendPCTCRC,MIN_INT,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PCT_HP_REPLY,nAppendPCTHpReply,MIN_INT,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PCT_MP_REPLY,nAppendPCTMpReply,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_APPEND_PCT_ENERGY_REPLY,nAppendPCTEnergyReply,0,MAX_INT,TO_CLIENT_SELF );

        DECLARE_PROPERTY(PlayerRoleProperty, PROPERTY_APPEND_PCT_PDP, nAppendPCTPDP, 0, MAX_INT, TO_CLIENT_SELF);
        DECLARE_PROPERTY(PlayerRoleProperty, PROPERTY_APPEND_PCT_MDP, nAppendPCTMDP, 0, MAX_INT, TO_CLIENT_SELF);

		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_PCT_COOL_REDUCE,nBasePCTCoolReduce,MIN_INT,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_ENACITY,nBaseEnacity,MIN_INT,MAX_INT,TO_CLIENT_SELF );

		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_MONNEY_COEFFICIENT,nMonneyCoefficient,0,MAX_INT,TO_CLIENT_SELF )
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_EXP_COEFFICIENT,nExpCoefficient,0,MAX_INT,TO_CLIENT_SELF )

        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_DAMAGE_BONUS,nBaseDamageBonus,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BASE_DAMAGE_BONUS_PCT,nBaseDamageBonusPCT,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_DAMAGE_BONUS,nDamageBonus,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_DAMAGE_BONUS_PCT,nDamageBonusPCT,MIN_INT,MAX_INT,TO_CLIENT_SELF );

        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_CONTROL_IMMUNE,nControlInmune,MIN_INT,MAX_INT,TO_CLIENT_SELF );

        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_AI_SCHEME_ID,nAISchemeID,MIN_INT,MAX_INT,TO_CLIENT_OTHER|TO_OTHER_SEVER );
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_PROMOTE_SHIELD,nPromoteShield,0,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_BULLET_COUNT,nBulletCount,0,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_PCT_AF, nPCTAF,MIN_INT,MAX_INT,TO_CLIENT_SELF | TO_CLIENT_OTHER);
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_OBSERVER, byObserver,0,255,TO_CLIENT_SELF |TO_OTHER_SEVER);
        DECLARE_PROPERTY( PlayerRoleProperty,PROPERTY_VISIBLE_FLAG, nVisibleFlag,0,65535,TO_CLIENT_OTHER);
        
		Propery_DeclareCopy( PlayerRoleProperty_Declare,s_PropertyDeclare,nDeclareLen );
    }

	bool getDataBuffer( rkt::obuf & out,int flag )
	{
        return Propery_GetBuffer( this,PlayerRoleProperty_Declare,flag,out );
    }
};

//////////////////////////////////////////////////////////////////////////
// 怪物属性
//////////////////////////////////////////////////////////////////////////
struct MonsterProperty
{
    int			nID;			// 怪物ID
    char		szName[32];
    int         nSceneID;       // 所在地图
    Vector3     location;       // 在地图上的位置
    Vector3     angle;			// 在地图上的朝向
    Vector3     velocity;       // 瞬时速度
    short       nVocation;      // 怪物子类型 MONSTER_SUB_TYPE
    short       nMonsType;		// 怪物类型
    int			nSkin;			// 皮肤 
    short       nLevel;		    // 等级
    int			nHP;		    // 血量
    int			nMaxHP;		    // 最大血量
    int			nMP;		    // 法术值
    int			nMaxMP;		    // 最大法术值
    BYTE	    byCamp;			// 阵营
    int         nKinID;         // 战队ID
    int         nClanID;        // 帮会ID
    int         nFaceID;        // 头像ID
    int         nTeamID;        // 队伍ID

    UID         uid;            // 角色UID
    UID         uidMaster;      // 主人UID(召唤怪主人UID)
    int         nAttack;        // 攻击力
    int         nDefence;       // 防御力
    int			nShield;		// 护盾值（额外生命）
	int			nStamina;		// 体力
	int			nMaxStamina;	// 最大体力
	int         nMask;		    // 标志位 ENTITY_MASKFLAG
    int         nObstacleFlag;  // 可否穿越动态障碍标志位

    int			nPA;			// 物理攻击
    int			nMA;			// 法术攻击
    int			nPD;			// 物理防御
    int			nMD;			// 法术防御
    int         nSD;			// 移动速度
    int         nPASD;			// 攻击速度
    int         nAF;			// 普攻系数
	int			nCRC;			// 暴击概率
	int			nHpReply;		// 血量回复
	int			nMpReply;		// 魔法回复

	int			nCRD;			// 暴击伤害
	int			nPDP;			// 物理防御穿透
	int			nPCTPDP;		// 百分比物理防御穿透
	int			nMDP;			// 法术防御穿透
	int			nPCTMDP;		// 百分比法术防御穿透
	int			nPCTImmunePA;	// 百分比物理伤害减免
	int			nPCTImmuneMA;	// 百分比法术伤害减免
	int			nPCTCoolReduce; // 百分比冷却缩减
	int			nPCTPDrains;	// 百分比物理吸血
	int			nPCTMDrains;	// 百分比法术吸血
	int			nTenacity;		// 韧性
    int         nAimDistance;   // 瞄准距离

    int			nBaseHP;		    // 基础血量
    int			nBaseMP;		    // 基础魔法
    int         nBaseEnergy;        // 基础能量
    int			nBasePA;		    // 基础物理攻击
    int			nBaseMA;		    // 基础法术攻击
    int			nBasePD;		    // 基础物理防御
    int			nBaseMD;		    // 基础法术防御
    int			nBaseSD;		    // 基础移动速度
    int			nBasePASD;		    // 基础攻击速度
    int			nBaseAF;		    // 基础普攻系数
	int			nBaseCRC;		    // 基础暴击概率
	int			nBaseHpReply;	    // 基础血量回复
	int			nBaseMpReply;	    // 基础魔法回复
    int         nBaseEnergyReply;   // 基础能量回复

    int			nAppendHP;		    // 附加血量
    int			nAppendMP;		    // 附加魔法
    int         nAppendEnergy;      // 附加能量
    int			nAppendPA;		    // 附加物理攻击
    int			nAppendMA;		    // 附加法术攻击
    int			nAppendPD;		    // 附加物理防御
    int			nAppendMD;		    // 附加法术防御
    int			nAppendSD;		    // 附加移动速度
    int			nAppendPASD;	    // 附加攻击速度
    int			nAppendAF;	        // 附加普攻系数
	int			nAppendCRC;		    // 附加暴击概率
	int			nAppendHpReply;	    // 附加血量回复
	int			nAppendMpReply;	    // 附加魔法回复
    int         nAppendEnergyReply; // 附加能量回复

    int			nAppendPCTHP;	        // 附加百分比血量
    int			nAppendPCTMP;	        // 附加百分比魔法
    int         nAppendPCTEnergy;       // 附加百分比能量
    int			nAppendPCTPA;	        // 附加百分比物理攻击
    int			nAppendPCTMA;	        // 附加百分比法术攻击
    int			nAppendPCTPD;	        // 附加百分比物理防御
    int			nAppendPCTMD;	        // 附加百分比法术防御
    int			nAppendPCTSD;	        // 附加百分比移动速度
    int			nAppendPCTPASD;	        // 附加百分比攻击速度
    int			nAppendPCTAF;	        // 附加百分比普攻系数
	int			nAppendPCTCRC;	        // 附加百分比暴击概率
	int			nAppendPCTHpReply;	    // 附加百分比血量回复
	int			nAppendPCTMpReply;	    // 附加百分比魔法回复
    int         nAppendPCTEnergyReply;  // 附加百分比能量回复
    int         nAppendPCTPDP;          // 附加额外百分比护甲穿透
    int         nAppendPCTMDP;          // 附加额外百分比魔抗穿透

	int			nBasePCTCoolReduce;	    // 基础百分比冷却缩减
	int			nBaseEnacity;		    // 基础韧性

	int         nMonneyCoefficient;	//额外金币系数
	int         nExpCoefficient;	//额外经验系数

    int         nBaseDamageBonus;	// 基础伤害加成(固定值)
    int         nBaseDamageBonusPCT;// 基础伤害加成百分比
    int         nDamageBonus;	    // 伤害加成(固定值)
    int         nDamageBonusPCT;    // 伤害加成百分比

    int         nControlInmune;     // 控制免疫(减少控制时间)
    int         nAISchemeID;        // AI配置ID
    int			nPromoteShield;     // 提升护盾

	int			nPCTAF;		// 百分比普攻系数
    BYTE        byObserver;     // 是否是观察者
    uint16      nVisibleFlag;   // 对各个阵营可见性标志掩码

    MonsterProperty() { memset(this,0,sizeof(MonsterProperty)); }

    // 对序列化时需要用到的数据进初始化
    static void serialize_init( DataChunkPropertyDeclare * s_PropertyDeclare,size_t nDeclareLen )
    {
        static MonsterProperty m_data;

        // 因为c++没有反射机制,所以需要手动设置
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_ID,nID,0,0,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_UID,uid,0,0,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_MASTER_UID,uidMaster,0,0,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_NAME,szName,0,0, TO_INVALID);
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_SCENEID,nSceneID,0,MAX_SHORT, TO_INVALID);
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_LOCATION,location,0,0,TO_DATABASE );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_ANGLE,angle,0,0,TO_DATABASE );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_VELOCITY,velocity,0,0,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_VOCATION,nVocation,0,MAX_SHORT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_SEX,nMonsType,0,MAX_SHORT,TO_CLIENT_OTHER );			
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_SKIN,nSkin,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_LEVEL,nLevel,0,MAX_SHORT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_HP,nHP,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_MAX_HP,nMaxHP,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_MP,nMP,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_MAX_MP,nMaxMP,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_CAMP,byCamp,0,255,TO_CLIENT_OTHER );	
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_KIN,nKinID,0,MAX_INT, TO_INVALID);
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_CLAN,nClanID,0,MAX_INT, TO_INVALID);
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_FACEID, nFaceID,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_TEAMID, nTeamID,0,MAX_INT, TO_INVALID);

        DECLARE_PROPERTY( MonsterProperty,PROPERTY_ATTACK,nAttack,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_DEFENCE,nDefence,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_SHIELD,nShield,0,MAX_INT,TO_INVALID );
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_STAMINA,nStamina,0,MAX_INT,TO_INVALID );
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_MAX_STAMINA,nMaxStamina,0,MAX_INT,TO_INVALID )
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_MASK,nMask,0,MAX_INT,TO_CLIENT_OTHER )
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_OBSFLAG,nObstacleFlag,0,MAX_INT,TO_PUBLIC )

        DECLARE_PROPERTY( MonsterProperty,PROPERTY_PA,nPA,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_MA,nMA,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_PD,nPD,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_MD,nMD,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_SD,nSD,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_PASD,nPASD,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_AF,nAF,0,MAX_INT,TO_INVALID );
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_CRC,nCRC,0,MAX_INT,TO_INVALID )
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_HP_REPLY,nHpReply,0,MAX_INT,TO_INVALID );
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_MP_REPLY,nMpReply,0,MAX_INT,TO_INVALID );

		DECLARE_PROPERTY( MonsterProperty,PROPERTY_CRD,nCRD,0,MAX_INT,TO_INVALID )
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_PDP,nPDP,0,MAX_INT,TO_INVALID )
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_PCT_PDP,nPCTPDP,MIN_INT,MAX_INT,TO_INVALID )
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_MDP,nMDP,0,MAX_INT,TO_INVALID )
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_PCT_MDP,nPCTMDP,MIN_INT,MAX_INT,TO_INVALID )
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_PCT_PAIMM,nPCTImmunePA,MIN_INT,MAX_INT,TO_INVALID )
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_PCT_MAIMM,nPCTImmuneMA,MIN_INT,MAX_INT,TO_INVALID )
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_PCT_COOL_REDUCE,nPCTCoolReduce,MIN_INT,MAX_INT,TO_INVALID )
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_PCT_PDRAINS,nPCTPDrains,MIN_INT,MAX_INT,TO_INVALID )
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_PCT_MDRAINS,nPCTMDrains,MIN_INT,MAX_INT,TO_INVALID )
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_TENACITY,nTenacity,0,MAX_INT,TO_INVALID )
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_AIM_DISTANCE,nAimDistance,0,MAX_INT,TO_INVALID )

        DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_HP,nBaseHP,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_MP,nBaseMP,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_ENERGY,nBaseEnergy,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_PA,nBasePA,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_MA,nBaseMA,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_PD,nBasePD,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_MD,nBaseMD,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_SD,nBaseSD,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_PASD,nBasePASD,0,MAX_INT,TO_INVALID );
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_AF,nBaseAF,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_CRC,nBaseCRC,0,MAX_INT,TO_INVALID );
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_HP_REPLY,nBaseHpReply,0,MAX_INT,TO_INVALID );
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_MP_REPLY,nBaseMpReply,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_ENERGY_REPLY,nBaseEnergyReply,0,MAX_INT,TO_INVALID );

        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_HP,nAppendHP,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_MP,nAppendMP,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_ENERGY,nAppendEnergy,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PA,nAppendPA,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_MA,nAppendMA,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PD,nAppendPD,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_MD,nAppendMD,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_SD,nAppendSD,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PASD,nAppendPASD,MIN_INT,MAX_INT,TO_INVALID );
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_AF,nAppendAF,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_CRC,nAppendCRC,MIN_INT,MAX_INT,TO_INVALID );
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_HP_REPLY,nAppendHpReply,MIN_INT,MAX_INT,TO_INVALID );
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_MP_REPLY,nAppendMpReply,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_ENERGY_REPLY,nAppendEnergyReply,0,MAX_INT,TO_INVALID );

        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PCT_HP,nAppendPCTHP,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PCT_MP,nAppendPCTMP,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PCT_ENERGY,nAppendPCTEnergy,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PCT_PA,nAppendPCTPA,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PCT_MA,nAppendPCTMA,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PCT_PD,nAppendPCTPD,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PCT_MD,nAppendPCTMD,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PCT_SD,nAppendPCTSD,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PCT_PASD,nAppendPCTPASD,MIN_INT,MAX_INT,TO_INVALID );       
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PCT_AF,nAppendPCTAF,MIN_INT,MAX_INT,TO_INVALID );  
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PCT_CRC,nAppendPCTCRC,MIN_INT,MAX_INT,TO_INVALID );
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PCT_HP_REPLY,nAppendPCTHpReply,MIN_INT,MAX_INT,TO_INVALID );
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PCT_MP_REPLY,nAppendPCTMpReply,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_APPEND_PCT_ENERGY_REPLY,nAppendPCTEnergyReply,0,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty, PROPERTY_APPEND_PCT_PDP, nAppendPCTPDP, 0, MAX_INT, TO_INVALID);
        DECLARE_PROPERTY( MonsterProperty, PROPERTY_APPEND_PCT_MDP, nAppendPCTMDP, 0, MAX_INT, TO_INVALID);
		
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_PCT_COOL_REDUCE,nBasePCTCoolReduce,MIN_INT,MAX_INT,TO_INVALID );
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_ENACITY,nBaseEnacity,MIN_INT,MAX_INT,TO_INVALID );

		DECLARE_PROPERTY( MonsterProperty,PROPERTY_MONNEY_COEFFICIENT,nMonneyCoefficient,0,MAX_INT,TO_INVALID );
		DECLARE_PROPERTY( MonsterProperty,PROPERTY_EXP_COEFFICIENT,nExpCoefficient,0,MAX_INT,TO_INVALID );
		
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_DAMAGE_BONUS,nBaseDamageBonus,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_BASE_DAMAGE_BONUS_PCT,nBaseDamageBonusPCT,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_DAMAGE_BONUS,nDamageBonus,MIN_INT,MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_DAMAGE_BONUS_PCT,nDamageBonusPCT,MIN_INT,MAX_INT,TO_INVALID );

        DECLARE_PROPERTY( MonsterProperty,PROPERTY_CONTROL_IMMUNE,nControlInmune,MIN_INT,MAX_INT,TO_INVALID );

        DECLARE_PROPERTY( MonsterProperty,PROPERTY_AI_SCHEME_ID,nAISchemeID,0,0,TO_OTHER_SEVER );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_PROMOTE_SHIELD,nPromoteShield,0,0,TO_OTHER_SEVER );

        DECLARE_PROPERTY( MonsterProperty,PROPERTY_PCT_AF,nPCTAF, MIN_INT, MAX_INT,TO_INVALID );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_OBSERVER, byObserver,0,255,TO_CLIENT_SELF );
        DECLARE_PROPERTY( MonsterProperty,PROPERTY_VISIBLE_FLAG, nVisibleFlag,0,65535,TO_CLIENT_OTHER);

        Propery_DeclareCopy( MonsterProperty_Declare,s_PropertyDeclare,nDeclareLen);
    }

    bool getDataBuffer( rkt::obuf & out,int flag )
    {
        return Propery_GetBuffer( this,MonsterProperty_Declare,flag,out );
    }
};

//////////////////////////////////////////////////////////////////////////
// 载具属性
//////////////////////////////////////////////////////////////////////////
struct TankProperty
{
    int			nID;			// 载具ID
    char		szName[32];
    int         nSceneID;       // 所在地图
    Vector3     location;       // 在地图上的位置
    Vector3     angle;			// 在地图上的朝向
    Vector3     velocity;       // 瞬时速度
    short       nVocation;      // ID
    short       nLevel;		    // 等级
    int			nHP;		    // 血量
    int			nMaxHP;		    // 最大血量
    int			nMP;		    // 法术值
    int			nMaxMP;		    // 最大法术值
    unsigned char   byCamp;     // 阵营
    int         nKinID;         // 战队ID
    int         nClanID;        // 帮会ID
    int         nFaceID;        // 头像ID
    int         nTeamID;        // 队伍ID

    UID         uid;            // 角色UID
    UID         uidMaster;      // 主人UID
    int         nAttack;        // 攻击力
    int         nDefence;       // 防御力
    int			nShield;		// 护盾值（额外生命）
	int			nStamina;		// 体力
	int			nMaxStamina;	// 最大体力
	int         nMask;		    // 标志位	ENTITY_MASKFLAG
    int         nObstacleFlag;  // 可否穿越动态障碍标志位

    int			nPA;			// 物理攻击
    int			nMA;			// 法术攻击
    int			nPD;			// 物理防御
    int			nMD;			// 法术防御
    int         nSD;			// 移动速度
    int         nPASD;			// 攻击速度
    int         nAF;			// 普攻系数
	int			nCRC;			// 暴击概率
	int			nHpReply;		// 血量回复
	int			nMpReply;		// 魔法回复

	int			nCRD;			// 暴击伤害
	int			nPDP;			// 物理防御穿透
	int			nPCTPDP;		// 百分比物理防御穿透
	int			nMDP;			// 法术防御穿透
	int			nPCTMDP;		// 百分比法术防御穿透
	int			nPCTImmunePA;	// 百分比物理伤害减免
	int			nPCTImmuneMA;	// 百分比法术伤害减免
	int			nPCTCoolReduce; // 百分比冷却缩减
	int			nPCTPDrains;	// 百分比物理吸血
	int			nPCTMDrains;	// 百分比法术吸血
	int			nTenacity;		// 韧性
    int         nAimDistance;   // 瞄准距离

    int			nBaseHP;		    // 基础血量
    int			nBaseMP;		    // 基础魔法
    int         nBaseEnergy;        // 基础能量
    int			nBasePA;		    // 基础物理攻击
    int			nBaseMA;		    // 基础法术攻击
    int			nBasePD;		    // 基础物理防御
    int			nBaseMD;		    // 基础法术防御
    int			nBaseSD;		    // 基础移动速度
    int			nBasePASD;		    // 基础攻击速度
    int			nBaseAF;		    // 基础普攻系数
    int			nBaseCRC;		    // 基础暴击概率
    int			nBaseHpReply;	    // 基础血量回复
    int			nBaseMpReply;	    // 基础魔法回复
    int         nBaseEnergyReply;   // 基础能量回复

    int			nAppendHP;		    // 附加血量
    int			nAppendMP;		    // 附加魔法
    int         nAppendEnergy;      // 附加能量
    int			nAppendPA;		    // 附加物理攻击
    int			nAppendMA;		    // 附加法术攻击
    int			nAppendPD;		    // 附加物理防御
    int			nAppendMD;		    // 附加法术防御
    int			nAppendSD;		    // 附加移动速度
    int			nAppendPASD;	    // 附加攻击速度
    int			nAppendAF;	        // 附加普攻系数
    int			nAppendCRC;		    // 附加暴击概率
    int			nAppendHpReply;	    // 附加血量回复
    int			nAppendMpReply;	    // 附加魔法回复
    int         nAppendEnergyReply; // 附加能量回复

    int			nAppendPCTHP;	        // 附加百分比血量
    int			nAppendPCTMP;	        // 附加百分比魔法
    int         nAppendPCTEnergy;       // 附加百分比能量
    int			nAppendPCTPA;	        // 附加百分比物理攻击
    int			nAppendPCTMA;	        // 附加百分比法术攻击
    int			nAppendPCTPD;	        // 附加百分比物理防御
    int			nAppendPCTMD;	        // 附加百分比法术防御
    int			nAppendPCTSD;	        // 附加百分比移动速度
    int			nAppendPCTPASD;	        // 附加百分比攻击速度
    int			nAppendPCTAF;	        // 附加百分比普攻系数
    int			nAppendPCTCRC;	        // 附加百分比暴击概率
    int			nAppendPCTHpReply;	    // 附加百分比血量回复
    int			nAppendPCTMpReply;	    // 附加百分比魔法回复
    int         nAppendPCTEnergyReply;  // 附加百分比能量回复
    int         nAppendPCTPDP;          // 附加额外百分比护甲穿透
    int         nAppendPCTMDP;          // 附加额外百分比魔抗穿透

    int			nBasePCTCoolReduce;	    // 基础百分比冷却缩减
    int			nBaseEnacity;		    // 基础韧性

	int         nMonneyCoefficient;	//额外金币系数
	int         nExpCoefficient;	//额外经验系数

    int         nBaseDamageBonus;	// 基础伤害加成(固定值)
    int         nBaseDamageBonusPCT;// 基础伤害加成百分比
    int         nDamageBonus;	    // 伤害加成(固定值)
    int         nDamageBonusPCT;    // 伤害加成百分比

    int         nControlInmune;     // 控制免疫(减少控制时间)
    int			nPromoteShield;     // 提升护盾

	int			nPCTAF;		// 百分比普攻系数

    TankProperty() { memset(this,0,sizeof(TankProperty)); }

    // 对序列化时需要用到的数据进初始化
    static void serialize_init( DataChunkPropertyDeclare * s_PropertyDeclare,size_t nDeclareLen )
    {
        static MonsterProperty m_data;

        // 因为c++没有反射机制,所以需要手动设置
        DECLARE_PROPERTY( TankProperty,PROPERTY_ID,nID,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_UID,uid,0,MAX_INT,TO_CLIENT_OTHER )
        DECLARE_PROPERTY( TankProperty,PROPERTY_MASTER_UID,uidMaster,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_NAME,szName,0,0,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_SCENEID,nSceneID,0,MAX_SHORT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_LOCATION,location,0,0,TO_DATABASE );
        DECLARE_PROPERTY( TankProperty,PROPERTY_ANGLE,angle,0,0,TO_DATABASE );
        DECLARE_PROPERTY( TankProperty,PROPERTY_VELOCITY,velocity,0,0,TO_INVALID );
        DECLARE_PROPERTY( TankProperty,PROPERTY_VOCATION,nVocation,0,MAX_SHORT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_LEVEL,nLevel,0,MAX_SHORT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_HP,nHP,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_MAX_HP,nMaxHP,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_MP,nMP,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_MAX_MP,nMaxMP,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_CAMP,byCamp,0,255,TO_PUBLIC );
        DECLARE_PROPERTY( TankProperty,PROPERTY_KIN,nKinID,0,MAX_INT,TO_PUBLIC );
        DECLARE_PROPERTY( TankProperty,PROPERTY_CLAN,nClanID,0,MAX_INT,TO_PUBLIC );
        DECLARE_PROPERTY( TankProperty,PROPERTY_FACEID,nFaceID,0,MAX_INT,TO_PUBLIC );
        DECLARE_PROPERTY( TankProperty,PROPERTY_TEAMID, nTeamID,0,MAX_INT, TO_OTHER_SEVER | TO_CLIENT_SELF | TO_CLIENT_OTHER);

        DECLARE_PROPERTY( TankProperty,PROPERTY_ATTACK,nAttack,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_DEFENCE,nDefence,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_SHIELD,nShield,0,MAX_INT,TO_CLIENT_OTHER );
		DECLARE_PROPERTY( TankProperty,PROPERTY_STAMINA,nStamina,0,MAX_INT,TO_CLIENT_OTHER );
		DECLARE_PROPERTY( TankProperty,PROPERTY_MAX_STAMINA,nMaxStamina,0,MAX_INT,TO_CLIENT_OTHER );
		DECLARE_PROPERTY( TankProperty,PROPERTY_MASK,nMask,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_OBSFLAG,nObstacleFlag,0,MAX_INT,TO_PUBLIC );

        DECLARE_PROPERTY( TankProperty,PROPERTY_PA,nPA,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_MA,nMA,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_PD,nPD,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_MD,nMD,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_SD,nSD,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_PASD,nPASD,0,MAX_INT,TO_CLIENT_OTHER );
        DECLARE_PROPERTY( TankProperty,PROPERTY_AF,nAF,0,MAX_INT,TO_CLIENT_OTHER );
		DECLARE_PROPERTY( TankProperty,PROPERTY_CRC,nCRC,0,MAX_INT,TO_CLIENT_OTHER );
		DECLARE_PROPERTY( TankProperty,PROPERTY_HP_REPLY,nHpReply,0,MAX_INT,TO_CLIENT_OTHER );
		DECLARE_PROPERTY( TankProperty,PROPERTY_MP_REPLY,nMpReply,0,MAX_INT,TO_CLIENT_OTHER );

		DECLARE_PROPERTY( TankProperty,PROPERTY_CRD,nCRD,0,MAX_INT,TO_CLIENT_OTHER )
		DECLARE_PROPERTY( TankProperty,PROPERTY_PDP,nPDP,0,MAX_INT,TO_CLIENT_OTHER )
		DECLARE_PROPERTY( TankProperty,PROPERTY_PCT_PDP,nPCTPDP,MIN_INT,MAX_INT,TO_CLIENT_OTHER )
		DECLARE_PROPERTY( TankProperty,PROPERTY_MDP,nMDP,0,MAX_INT,TO_CLIENT_OTHER )
		DECLARE_PROPERTY( TankProperty,PROPERTY_PCT_MDP,nPCTMDP,MIN_INT,MAX_INT,TO_CLIENT_OTHER )
		DECLARE_PROPERTY( TankProperty,PROPERTY_PCT_PAIMM,nPCTImmunePA,MIN_INT,MAX_INT,TO_CLIENT_OTHER )
		DECLARE_PROPERTY( TankProperty,PROPERTY_PCT_MAIMM,nPCTImmuneMA,MIN_INT,MAX_INT,TO_CLIENT_OTHER )
		DECLARE_PROPERTY( TankProperty,PROPERTY_PCT_COOL_REDUCE,nPCTCoolReduce,MIN_INT,MAX_INT,TO_CLIENT_OTHER )
		DECLARE_PROPERTY( TankProperty,PROPERTY_PCT_PDRAINS,nPCTPDrains,MIN_INT,MAX_INT,TO_CLIENT_OTHER )
		DECLARE_PROPERTY( TankProperty,PROPERTY_PCT_MDRAINS,nPCTMDrains,MIN_INT,MAX_INT,TO_CLIENT_OTHER )
		DECLARE_PROPERTY( TankProperty,PROPERTY_TENACITY,nTenacity,0,MAX_INT,TO_CLIENT_OTHER )
        DECLARE_PROPERTY( TankProperty,PROPERTY_AIM_DISTANCE,nAimDistance,0,MAX_INT,TO_CLIENT_SELF )

        DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_HP,nBaseHP,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_MP,nBaseMP,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_ENERGY,nBaseEnergy,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_PA,nBasePA,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_MA,nBaseMA,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_PD,nBasePD,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_MD,nBaseMD,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_SD,nBaseSD,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_PASD,nBasePASD,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_AF,nBaseAF,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_CRC,nBaseCRC,0,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_HP_REPLY,nBaseHpReply,0,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_MP_REPLY,nBaseMpReply,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_ENERGY_REPLY,nBaseEnergyReply,0,MAX_INT,TO_CLIENT_SELF );

        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_HP,nAppendHP,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_MP,nAppendMP,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_ENERGY,nAppendEnergy,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PA,nAppendPA,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_MA,nAppendMA,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PD,nAppendPD,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_MD,nAppendMD,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_SD,nAppendSD,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PASD,nAppendPASD,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_AF,nAppendAF,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_CRC,nAppendCRC,MIN_INT,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_HP_REPLY,nAppendHpReply,MIN_INT,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_MP_REPLY,nAppendMpReply,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_ENERGY_REPLY,nAppendEnergyReply,0,MAX_INT,TO_CLIENT_SELF );

        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PCT_HP,nAppendPCTHP,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PCT_MP,nAppendPCTMP,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PCT_ENERGY,nAppendPCTEnergy,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PCT_PA,nAppendPCTPA,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PCT_MA,nAppendPCTMA,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PCT_PD,nAppendPCTPD,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PCT_MD,nAppendPCTMD,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PCT_SD,nAppendPCTSD,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PCT_PASD,nAppendPCTPASD,MIN_INT,MAX_INT,TO_CLIENT_SELF ); 
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PCT_AF,nAppendPCTAF,MIN_INT,MAX_INT,TO_CLIENT_SELF ); 
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PCT_CRC,nAppendPCTCRC,MIN_INT,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PCT_HP_REPLY,nAppendPCTHpReply,MIN_INT,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PCT_MP_REPLY,nAppendPCTMpReply,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_APPEND_PCT_ENERGY_REPLY,nAppendPCTEnergyReply,0,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty, PROPERTY_APPEND_PCT_PDP, nAppendPCTPDP, 0, MAX_INT, TO_INVALID);
        DECLARE_PROPERTY( TankProperty, PROPERTY_APPEND_PCT_MDP, nAppendPCTMDP, 0, MAX_INT, TO_INVALID);
		DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_PCT_COOL_REDUCE,nBasePCTCoolReduce,MIN_INT,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_ENACITY,nBaseEnacity,MIN_INT,MAX_INT,TO_CLIENT_SELF );

		DECLARE_PROPERTY( TankProperty,PROPERTY_MONNEY_COEFFICIENT,nMonneyCoefficient,0,MAX_INT,TO_CLIENT_SELF );
		DECLARE_PROPERTY( TankProperty,PROPERTY_EXP_COEFFICIENT,nExpCoefficient,0,MAX_INT,TO_CLIENT_SELF );

        DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_DAMAGE_BONUS,nBaseDamageBonus,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_BASE_DAMAGE_BONUS_PCT,nBaseDamageBonusPCT,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_DAMAGE_BONUS,nDamageBonus,MIN_INT,MAX_INT,TO_CLIENT_SELF );
        DECLARE_PROPERTY( TankProperty,PROPERTY_DAMAGE_BONUS_PCT,nDamageBonusPCT,MIN_INT,MAX_INT,TO_CLIENT_SELF );

        DECLARE_PROPERTY( TankProperty,PROPERTY_CONTROL_IMMUNE,nControlInmune,MIN_INT,MAX_INT,TO_CLIENT_SELF );

		DECLARE_PROPERTY( TankProperty,PROPERTY_PCT_AF,nPCTAF,MIN_INT,MAX_INT,TO_CLIENT_SELF );

        Propery_DeclareCopy( TankProperty_Declare,s_PropertyDeclare,nDeclareLen );
    }

    bool getDataBuffer( rkt::obuf & out,int flag )
    {
        return Propery_GetBuffer( this,TankProperty_Declare,flag,out );
    }
};

#pragma pack()
