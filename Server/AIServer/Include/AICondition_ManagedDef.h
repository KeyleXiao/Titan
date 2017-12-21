#pragma once
#include "ManagedCode.h"
#include "AI_ManagedDef.h"

// ע:�е����������Ƿ�����Ŀ�꣬����Ϊ�е�Ŀ�겻ȷ����
//    ��Ҫ����������������Ŀ�꣬�������������������Ŀ�꣬��Ϊ��Ŀ���ֱ������Ϊ
//    Ŀ�����;Ϳ����ˣ���������������

DESC("�Ƚ�")
ENUM COMPARE
{
    DESC("���ڵ���")
    GREATER_THAN,
    DESC("С��")
    LESS_THAN,
};

DESC("Ѫ��/����")
ENUM HP_MP_TYPE
{
    DESC("Ѫ��")
    HP_MP_TYPE_HP,
    DESC("����")
    HP_MP_TYPE_MP,
};

DESC("�ٷֱ�/ֵ")
ENUM VALUE_TYPE
{
    DESC("�ٷֱ�")
    VALUE_TYPE_RATE,
    DESC("ֵ")
    VALUE_TYPE_VALUE,
};

DESC("ʵ�������ж�")
STRUCT AIConditionServer_EntityType
{
    DESC("����ID")
    int nID;
    DESC("Ŀ������")
    AI_TARGET_TYPE eTargetType;
    DESC("�����ֶ�")
    float fReserved;
    DESC("�Ƿ��Ǹ�����")
    AI_ENTITY_TYPE eEntityType;

    STRUCT_INIT(AIConditionServer_EntityType)
};

DESC("ʵ����Ӫ��ϵ�ж�")
STRUCT AIConditionServer_CampRelationship
{
    DESC("����ID")
    int nID;
    DESC("Ŀ������")
    AI_TARGET_TYPE eTargetType;
    DESC("��Ӫ��ϵ")
    AI_CAMP_RELATIONSHIP eCampRelation;
    DESC("�Ƿ�����Ŀ��")
    MANAGED_BOOL eSetTarget;

    STRUCT_INIT(AIConditionServer_CampRelationship)
};

DESC("��Ŀ������ж�")
STRUCT AIConditionServer_Distance2Target
{
    DESC("����ID")
    int nID;
    DESC("Ŀ������")
    AI_TARGET_TYPE eTargetType;
    DESC("�����ֶ�")
    float fReserved;
    DESC("�Ƚϲ���")
    COMPARE eCompare;
    DESC("����")
    float fDistance;
    DESC("�Ƿ�����Ŀ��")
    MANAGED_BOOL eSetTarget;

    STRUCT_INIT(AIConditionServer_Distance2Target)
};

DESC("���¼��ص�����ж�")
STRUCT AIConditionServer_Distance2EventLoc
{
    DESC("����ID")
    int nID;
    DESC("���¼��ص�������/С��")
    COMPARE eCompare;
    DESC("����")
    float fDistance;

    STRUCT_INIT(AIConditionServer_Distance2EventLoc)
};

DESC("�������ľ����ж�")
STRUCT AIConditionServer_Distance2BornLoc
{
    DESC("����ID")
    int nID;
    DESC("�Ƚϲ���")
    COMPARE eCompare;
    DESC("����")
    float fDistance;

    STRUCT_INIT(AIConditionServer_Distance2BornLoc)
};

DESC("�Լ�������ʵ���Ƿ����")
STRUCT AIConditionServer_NearbyEntityExist
{
    DESC("����ID")
    int nID;
    DESC("ʵ����Ӫ")
    AI_CAMP_RELATIONSHIP eCamp;
    DESC("ʵ������")
    AI_ENTITY_TYPE eEntityType;
    DESC("����/������")
    MANAGED_BOOL eExist;

    STRUCT_INIT(AIConditionServer_NearbyEntityExist)
};

DESC("Ŀ����Χʵ�������ж�")
STRUCT  AIConditionServer_NearbyEntityNum
{
    DESC("����ID")
    int nID;
    DESC("Ŀ��")
    AI_TARGET_TYPE eTargetType;
    DESC("�����ֶ�")
    float fReserved;
    DESC("������Χ")
    float fNearbyRaddi;
    DESC("ʵ�����Լ�����Ӫ��ϵ")
    AI_CAMP_RELATIONSHIP eCamp;
    DESC("ʵ������")
    AI_ENTITY_TYPE eEntityType;
    DESC("�Ƚϲ���")
    COMPARE eCompare;
    DESC("����")
    int nValue;
    DESC("�Ƿ�����Ŀ��")
    MANAGED_BOOL eSetTarget;

    STRUCT_INIT( AIConditionServer_NearbyEntityNum)
};

DESC("����ʵ�������Ƚ�")
STRUCT AIConditionServer_NearbyEntityNumCompare
{
    DESC("����ID")
    int nID;
    DESC("x�׷�Χ��")
    float fSearchRaddi;
    DESC("ʵ������")
    AI_ENTITY_TYPE eEntityType;
    DESC("���ڵ���/С�ڵз�")
    COMPARE eCompare;

    STRUCT_INIT(AIConditionServer_NearbyEntityNumCompare)
};

DESC("Ѫ��/�����ж�")
STRUCT AIConditionServer_HpMp
{
    DESC("����ID")
    int nID;
    DESC("Ŀ������")
    AI_TARGET_TYPE eTargetType;
    DESC("�����ֶ�")
    float fReserved;
    DESC("Ѫ��/����")
    HP_MP_TYPE eHpMp;
    DESC("�Ƚϲ���")
    COMPARE eCompare;
    DESC("�ٷֱ�/Ѫ��ֵ")
    VALUE_TYPE eValueType;
    DESC("ֵ")
    float fValue;
    DESC("�Ƿ�����Ŀ��")
    MANAGED_BOOL eSetTarget;

    STRUCT_INIT(AIConditionServer_HpMp)
};

DESC("�Լ������Ƿ���CD")
STRUCT AIConditionServer_SpellCD
{
    DESC("����ID")
    int nID;
    DESC("���ܲ�����")
    int nSlotIndex;
    DESC("�Ƿ���CD")
    MANAGED_BOOL eIsCD;

    STRUCT_INIT(AIConditionServer_SpellCD)
};

DESC("�Ƿ����ʹ����Ʒ")
STRUCT AIConditionServer_CanUseItem
{
    DESC("����ID")
    int nID;
    DESC("��ƷID")
    int nItemID;
    DESC("��ȴʱ��(ms)")
    int nCD;

    STRUCT_INIT(AIConditionServer_CanUseItem)
};

DESC("���Լ�·�ߵ���̾����ж�")
STRUCT AIConditionServer_MinDistance2SelfPath
{
    DESC("����ID")
    int nID;
    DESC("���Լ�·����̾�����ڵ���/С��")
    COMPARE eCompare;
    DESC("����")
    int nDistance;

    STRUCT_INIT(AIConditionServer_MinDistance2SelfPath)
};

DESC("�ж�buff�Ƿ����")
STRUCT AIConditionServer_BuffExist
{
    DESC("����ID")
    int nID;
    DESC("���ж�Ŀ��")
    AI_TARGET_TYPE eTarget;
    DESC("buffID")
    int nBuffID;
    DESC("�Ƿ����")
    MANAGED_BOOL eExist;
    DESC("buff�ȼ���0��ʾ����ȼ�")
    int nBuffLevel;

    STRUCT_INIT(AIConditionServer_BuffExist)
};

DESC("�ж�����ʩ�����ܲ�λ")
STRUCT AIConditionServer_MasterCastSpellSlot
{
    DESC("����ID")
    int nID;
    DESC("��λ")
    int nSlotIndex;

    STRUCT_INIT(AIConditionServer_MasterCastSpellSlot)
};

DESC("�߶�(Y����)�ж�")
STRUCT AIConditionServer_Height
{
    DESC("����ID")
    int nID;
    DESC("���ж�Ŀ��")
    AI_TARGET_TYPE eTarget;
    DESC("Y���ڵ���/YС��")
    COMPARE eCompare;
    DESC("��")
    float fHeight;

    STRUCT_INIT(AIConditionServer_Height)
};

DESC("ĳ·")
ENUM PATH_DESC
{
    DESC("����·")
    PATH_ANY,
    DESC("��·")
    PATH_UP,
    DESC("��·")
    PATH_MID,
    DESC("��·")
    PATH_DOWN,
};

DESC("ĳ��ĳ·ĳ���Ƿ�����")
STRUCT AIConditionServer_TowerLost
{
    DESC("����ID")
    int nID;
    DESC("ĳ��")
    AI_CAMP_RELATIONSHIP eCamp;
    DESC("ĳ·")
    PATH_DESC ePath;
    DESC("ĳ��[��Ŵ�1��ʼ]")
    int nSN;

    STRUCT_INIT(AIConditionServer_TowerLost)
};

DESC("�Լ��ȼ�����")
STRUCT AIConditionServer_Level
{
    DESC("����ID")
    int nID;
    DESC("���ڵ���/С��")
    COMPARE eCompare;
    DESC("��")
    int nLevel;

    STRUCT_INIT(AIConditionServer_Level)
};
