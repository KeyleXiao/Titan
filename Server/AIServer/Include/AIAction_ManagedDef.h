#pragma once
#include "ManagedCode.h"
#include "AI_ManagedDef.h"
#include "DamageDef_ManagedDef.h"

DESC("Ѳ������")
ENUM PATROL_TYPE
{
    DESC("��·��(���տ����з����صķ����Զ�ѡ��Ѳ�߷���)")
    EPATROL_ALONG_PATH_FWD_ENEMY_HOME,
    DESC("���")
    EPATROL_RANDOM,
	DESC("��·��(�ϸ���·��Ѳ��)")
	EPATROL_ALONG_PATH_NORMAL,
};

DESC("Ѳ����Ϊ")
STRUCT AIActionServer_Patrol
{
    DESC("��ΪID")
    int nID;

    DESC("Ѳ������")
    PATROL_TYPE ePatrolType;

    DESC("ֵ(��·��=����;���=�뾶)")
    int nValue;

    DESC("�����ֶ�")
    int nReserved;

    STRUCT_INIT(AIActionServer_Patrol)
};

DESC("�ع���Ϊ")
STRUCT AIActionServer_Retrive
{
    DESC("��ΪID")
    int nID;
    DESC("�Ƿ���Ҫ��Ѫ��ħ")
    MANAGED_BOOL eFullHpMp;
    DESC("�����(���ع����)")
    int nCheckInterval;

    STRUCT_INIT(AIActionServer_Retrive) 
};

DESC("ʹ�ü�����Ϊ")
STRUCT AIActionServer_CastSpell
{
    DESC("��ΪID")
    int nID;
    DESC("Ŀ������")
    AI_TARGET_TYPE eBestTargetType;
    DESC("��ѡĿ������")
    AI_TARGET_TYPE eAltTargetType;
    DESC("�Ƿ��������")
    MANAGED_BOOL eKeepAttack;
    DESC("�������ms/��ֱʱ��(B�سǲ���Ҫ���������Ϊ)")
    int nAttackInterval;
    DESC("�Ƿ�ָ������")
    MANAGED_BOOL eSpecifySpell;
    DESC("���ܲ�����")
    int nSlotIndex;
    DESC("�Ƿ񿿽�Ŀ��")
    MANAGED_BOOL eKeepClose;
    DESC("�����������Ƿ���ع�")
    MANAGED_BOOL eCheckRetrieve;
    DESC("��������")
    float fCloseDis;
    DESC("��ֹ����,��Ŀ�����̫������ֹ")
    float fStopDis;
    DESC("�Ƿ�����Ŀ��")
    MANAGED_BOOL eLockTarget;
    DESC("��������������Χ��(float)")
    float fCloseRandRange;
    DESC("���û����������ms(0��ʾ������)")
    int nQuitInterval;

    STRUCT_INIT(AIActionServer_CastSpell)
};

DESC("����Ŀ����Ϊ")
STRUCT AIActionServer_ClostTo
{
    DESC("��ΪID")
    int nID;
    DESC("����Ŀ������")
    AI_TARGET_TYPE eTargetType;
    DESC("�Ƿ����")
    MANAGED_BOOL eKeepFollow;
    DESC("�����������Ƿ���ع�")
    MANAGED_BOOL eCheckRetrieve;
    DESC("ʱ����ms")
    int nCloseInterval;
    DESC("������������")
    float fCloseDis;
    DESC("��������λ��(��)") 
    float fDisFromStartPointLimit;

    STRUCT_INIT(AIActionServer_ClostTo)
};

DESC("����")
STRUCT AIActionServer_Jump
{
    DESC("��ΪID")
    int nID;
    DESC("�����Ŀ��")
    AI_TARGET_TYPE eTargetType;
    DESC("�Ƿ��Զ�����ŵ�")
    MANAGED_BOOL eCheckGround;
    DESC("���溣�θ߶�(���Զ�����ŵ�ʱ��)")
    float fGroundY;

    STRUCT_INIT(AIActionServer_Jump)
};

DESC("����������Ϊ")
STRUCT AIActionServer_UpgradeSpell
{
    DESC("��ΪID")
    int nID;
    DESC("1�������ܲ�����")
    int n1SlotIndex;
    DESC("2�������ܲ�����")
    int n2SlotIndex;
    DESC("3�������ܲ�����")
    int n3SlotIndex;
    DESC("4�������ܲ�����")
    int n4SlotIndex;
    DESC("5�������ܲ�����")
    int n5SlotIndex;
    DESC("6�������ܲ�����")
    int n6SlotIndex;
    DESC("7�������ܲ�����")
    int n7SlotIndex;
    DESC("8�������ܲ�����")
    int n8SlotIndex;
    DESC("9�������ܲ�����")
    int n9SlotIndex;
    DESC("10�������ܲ�����")
    int n10SlotIndex;
    DESC("11�������ܲ�����")
    int n11SlotIndex;
    DESC("12�������ܲ�����")
    int n12SlotIndex;
    DESC("13�������ܲ�����")
    int n13SlotIndex;
    DESC("14�������ܲ�����")
    int n14SlotIndex;
    DESC("15�������ܲ�����")
    int n15SlotIndex;
    DESC("16�������ܲ�����")
    int n16SlotIndex;
    DESC("17�������ܲ�����")
    int n17SlotIndex;
    DESC("18�������ܲ�����")
    int n18SlotIndex;
    DESC("19�������ܲ�����")
    int n19SlotIndex;
    DESC("20�������ܲ�����")
    int n20SlotIndex;

    STRUCT_INIT(AIActionServer_UpgradeSpell) 
};

DESC("������Ʒ��Ϊ�����ϣ�")
STRUCT AIActionServer_BuyItem
{
    DESC("��ΪID")
    int nID;
    DESC("ѪƿID")
    short nHpBottleID;
    DESC("Ѫƿ����")
    short nHpBottleNum;
    DESC("ħƿID")
    short nMpBottleID;
    DESC("ħƿ����")
    short nMpBottleNum;

    DESC("�����1��ǰ��Ҫ����")
    short nItem1Sell;
    DESC("��1������ID")
    short nItem1Buy;
    DESC("��1������")
    short nItem1Cost;
    
    DESC("�����2��ǰ��Ҫ����")
    short nItem2Sell;
    DESC("��2������ID")
    short nItem2Buy;
    DESC("��2������")
    short nItem2Cost;

    DESC("�����3��ǰ��Ҫ����")
    short nItem3Sell;
    DESC("��3������ID")
    short nItem3Buy;
    DESC("��3������")
    short nItem3Cost;

    DESC("�����4��ǰ��Ҫ����")
    short nItem4Sell;
    DESC("��4������ID")
    short nItem4Buy;
    DESC("��4������")
    short nItem4Cost;

    DESC("�����5��ǰ��Ҫ����")
    short nItem5Sell;
    DESC("��5������ID")
    short nItem5Buy;
    DESC("��5������")
    short nItem5Cost;

    DESC("�����6��ǰ��Ҫ����")
    short nItem6Sell;
    DESC("��6������ID")
    short nItem6Buy;
    DESC("��6������")
    short nItem6Cost;

    DESC("�����7��ǰ��Ҫ����")
    short nItem7Sell;
    DESC("��7������ID")
    short nItem7Buy;
    DESC("��7������")
    short nItem7Cost;

    DESC("�����8��ǰ��Ҫ����")
    short nItem8Sell;
    DESC("��8������ID")
    short nItem8Buy;
    DESC("��8������")
    short nItem8Cost;

    DESC("�����9��ǰ��Ҫ����")
    short nItem9Sell;
    DESC("��9������ID")
    short nItem9Buy;
    DESC("��9������")
    short nItem9Cost;

    DESC("�����10��ǰ��Ҫ����")
    short nItem10Sell;
    DESC("��10������ID")
    short nItem10Buy;
    DESC("��10������")
    short nItem10Cost;

    DESC("�����11��ǰ��Ҫ����")
    short nItem11Sell;
    DESC("��11������ID")
    short nItem11Buy;
    DESC("��11������")
    short nItem11Cost;

    DESC("�����12��ǰ��Ҫ����")
    short nItem12Sell;
    DESC("��12������ID")
    short nItem12Buy;
    DESC("��12������")
    short nItem12Cost;

    DESC("�����13��ǰ��Ҫ����")
    short nItem13Sell;
    DESC("��13������ID")
    short nItem13Buy;
    DESC("��13������")
    short nItem13Cost;

    DESC("�����14��ǰ��Ҫ����")
    short nItem14Sell;
    DESC("��14������ID")
    short nItem14Buy;
    DESC("��14������")
    short nItem14Cost;

    DESC("�����15��ǰ��Ҫ����")
    short nItem15Sell;
    DESC("��15������ID")
    short nItem15Buy;
    DESC("��15������")
    short nItem15Cost;

    DESC("�����16��ǰ��Ҫ����")
    short nItem16Sell;
    DESC("��16������ID")
    short nItem16Buy;
    DESC("��16������")
    short nItem16Cost;

    DESC("�����17��ǰ��Ҫ����")
    short nItem17Sell;
    DESC("��17������ID")
    short nItem17Buy;
    DESC("��17������")
    short nItem17Cost;

    DESC("�����18��ǰ��Ҫ����")
    short nItem18Sell;
    DESC("��18������ID")
    short nItem18Buy;
    DESC("��18������")
    short nItem18Cost;

    DESC("�����19��ǰ��Ҫ����")
    short nItem19Sell;
    DESC("��19������ID")
    short nItem19Buy;
    DESC("��19������")
    short nItem19Cost;

    DESC("�����20��ǰ��Ҫ����")
    short nItem20Sell;
    DESC("��20������ID")
    short nItem20Buy;
    DESC("��20������")
    short nItem20Cost;

    DESC("�����21��ǰ��Ҫ����")
    short nItem21Sell;
    DESC("��21������ID")
    short nItem21Buy;
    DESC("��21������")
    short nItem21Cost;

    DESC("�����22��ǰ��Ҫ����")
    short nItem22Sell;
    DESC("��22������ID")
    short nItem22Buy;
    DESC("��22������")
    short nItem22Cost;

    DESC("�����23��ǰ��Ҫ����")
    short nItem23Sell;
    DESC("��23������ID")
    short nItem23Buy;
    DESC("��23������")
    short nItem23Cost;

    DESC("�����24��ǰ��Ҫ����")
    short nItem24Sell;
    DESC("��24������ID")
    short nItem24Buy;
    DESC("��24������")
    short nItem24Cost;

    DESC("�����25��ǰ��Ҫ����")
    short nItem25Sell;
    DESC("��25������ID")
    short nItem25Buy;
    DESC("��25������")
    short nItem25Cost;

    DESC("�����26��ǰ��Ҫ����")
    short nItem26Sell;
    DESC("��26������ID")
    short nItem26Buy;
    DESC("��26������")
    short nItem26Cost;

    DESC("�����27��ǰ��Ҫ����")
    short nItem27Sell;
    DESC("��27������ID")
    short nItem27Buy;
    DESC("��27������")
    short nItem27Cost;

    DESC("�����28��ǰ��Ҫ����")
    short nItem28Sell;
    DESC("��28������ID")
    short nItem28Buy;
    DESC("��28������")
    short nItem28Cost;

    DESC("�����29��ǰ��Ҫ����")
    short nItem29Sell;
    DESC("��29������ID")
    short nItem29Buy;
    DESC("��29������")
    short nItem29Cost;

    DESC("�����30��ǰ��Ҫ����")
    short nItem30Sell;
    DESC("��30������ID")
    short nItem30Buy;
    DESC("��30������")
    short nItem30Cost;

    DESC("�����31��ǰ��Ҫ����")
    short nItem31Sell;
    DESC("��31������ID")
    short nItem31Buy;
    DESC("��31������")
    short nItem31Cost;

    DESC("�����32��ǰ��Ҫ����")
    short nItem32Sell;
    DESC("��32������ID")
    short nItem32Buy;
    DESC("��32������")
    short nItem32Cost;

    STRUCT_INIT(AIActionServer_BuyItem)
};

DESC("������Ʒ��Ϊ(��csv����)")
STRUCT AIActionServer_BuyItems
{
    DESC("��ΪID")
    int nID;

    STRUCT_INIT(AIActionServer_BuyItems)
};

DESC("ʹ����Ʒ��Ϊ")
STRUCT AIActionServer_UseItem
{
    DESC("��ΪID")
    int nID;
    DESC("��ƷID")
    int nItemID;

    STRUCT_INIT(AIActionServer_UseItem)
};

DESC("����npc��Ϊ")
STRUCT AIActionServer_CreateNpc
{
    DESC("��ΪID")
    int nID;
    DESC("NPCID")
    int nNpcID;
    DESC("�����ֶ�")
    int nSubType;
    DESC("npc�ȼ�")
    int nLevel;
    DESC("ͼ��ID")
    int nIcon;
    DESC("����x")
    float fDirX;
    DESC("����y")
    float fDirY;
    DESC("����z")
    float fDirZ;

    STRUCT_INIT(AIActionServer_CreateNpc)
};

DESC("����������Ϊ(��ɱ)")
STRUCT AIActionServer_DestroySelf
{
    DESC("��ΪID")
    int nID;
    DESC("���ʱ��ms")
    int nLifeTime;

    DESC("�˺�����DAMAGE_TYPE")
    int nDamageType;

    DESC("����Ч������SPELL_EFFECT_TYPE")
    int	nSpellEffectType;

    DESC("����Ч������")
    DeadEffectType nDeadType;

    STRUCT_INIT(AIActionServer_DestroySelf)
};

DESC("��buff")
STRUCT AIActionServer_AddBuff
{
    DESC("��ΪID")
    int nID;
    DESC("Ŀ��")
    AI_TARGET_TYPE eTargetType;
    DESC("�����ֶ�")
    float fReserved;
    DESC("buffID")
    int nBuffID;
    DESC("buff�ȼ�")
    int nBuffLevel;
    DESC("����/�Ƴ�")
    MANAGED_BOOL eAdd;

    STRUCT_INIT(AIActionServer_AddBuff)
};

DESC("������Ϊ")
STRUCT AIActionServer_Idle
{
    DESC("��ΪID")
    int nID;
    DESC("����ʱ��ms")
    int nIdleTime;

    STRUCT_INIT(AIActionServer_Idle)
};


DESC("������Ϊ(��Ұ���������������������������)")
STRUCT AIActionServer_WatchOut
{
    DESC("��ΪID")
    int nID;
    DESC("����ĸ���Ӫ��ʵ��")
    AI_CAMP_RELATIONSHIP eCamp;
    DESC("�������ʵ��")
    AI_ENTITY_TYPE eEntityType;
    DESC("�������ms")
    int nInterval;

    STRUCT_INIT(AIActionServer_WatchOut)
};

DESC("���������Ϊ(��ʼ���ֱ�����ֵ��˱���)")
STRUCT AIActionServer_WatchOutEnemy
{
    DESC("��ΪID")
    int nID;
    DESC("�ѵм��ms")
    int nInterval;
    DESC("�������")
    int nDistance;
	DESC("����AI��������")
	int nMask;

    STRUCT_INIT(AIActionServer_WatchOutEnemy)
};

DESC("Ԥ����Ϊ[��Χ�ڼ���Ӣ����С�ڵз��㲥Ԥ���¼�]")
STRUCT AIActionServer_RadarScan
{
    DESC("��ΪID")
    int nID;
    DESC("ɨ����ms(Ԥ��������ļ����һЩ)")
    int nInterval;
    DESC("Ԥ��ɨ��뾶m(Ԥ����Ҫ��İ뾶��һЩ)")
    int nScanRaddi;
    DESC("Ԥ��ʱ�����Χ���ез�����������־")
    int nEmergentRaddi;

    STRUCT_INIT(AIActionServer_RadarScan)
};

//---------------------------------------------------------------

DESC("������λ��")
ENUM BUILDING_POS
{
    DESC("������")
    OUTER_MOST,
    DESC("���ڲ��")
    INNER_MOST,
};

DESC("ѡ���������")
STRUCT AIActionServer_ChooseNearestPath
{
    DESC("��ΪID")
    int nID;
    DESC("��·1")
    int nPath1;
    DESC("��·2")
    int nPath2;
    DESC("��·3")
    int nPath3;

    STRUCT_INIT(AIActionServer_ChooseNearestPath)
};

DESC("��Χ�㲥һ�½����ź�")
STRUCT AIActionServer_BroadcastAttackSignal
{
    DESC("��ΪID")
    int nID;
    DESC("����Ŀ��")
    AI_TARGET_TYPE eTargetType;
    DESC("�㲥��Χ(��)")
    int nBroadcastRange;

    STRUCT_INIT(AIActionServer_BroadcastAttackSignal)
};

DESC("Ӣ�۷����ʼ��·")
STRUCT AIActionServer_AllotPath
{
    DESC("��ΪID")
    int nID;

    STRUCT_INIT(AIActionServer_AllotPath)
};

DESC("��̬ѡ��,���߲�����Ĳ�����Ϊ��ѡ�߹�����ʱд��")
STRUCT AIActionServer_SelectPath
{
    DESC("��ΪID")
    int nID;

    STRUCT_INIT(AIActionServer_SelectPath)
};

DESC("��Ŀ�걣�־���")
STRUCT AIActionServer_KeepDistance
{
    DESC("��ΪID")
    int nID;
    DESC("Ŀ��")
    AI_TARGET_TYPE eTarget;
    DESC("��С����,��λ��")
    float fMinDis;
    DESC("������,��λ��")
    float fMaxDis;

    STRUCT_INIT(AIActionServer_KeepDistance)
};

DESC("�����츳")
STRUCT AIActionServer_ActiveTalent
{
    DESC("��ΪID")
    int nID;
    DESC("1���츳")
    int nTalent_1;
    DESC("2���츳")
    int nTalent_2;
    DESC("3���츳")
    int nTalent_3;
    DESC("4���츳")
    int nTalent_4;
    DESC("5���츳")
    int nTalent_5;
    DESC("6���츳")
    int nTalent_6;
    DESC("7���츳")
    int nTalent_7;
    DESC("8���츳")
    int nTalent_8;
    DESC("9���츳")
    int nTalent_9;
    DESC("10���츳")
    int nTalent_10;
    DESC("11���츳")
    int nTalent_11;
    DESC("12���츳")
    int nTalent_12;
    DESC("13���츳")
    int nTalent_13;
    DESC("14���츳")
    int nTalent_14;
    DESC("15���츳")
    int nTalent_15;
    DESC("16���츳")
    int nTalent_16;
    DESC("17���츳")
    int nTalent_17;
    DESC("18���츳")
    int nTalent_18;
    DESC("19���츳")
    int nTalent_19;
    DESC("20���츳")
    int nTalent_20;

    STRUCT_INIT(AIActionServer_ActiveTalent)
};

DESC("ִ���ڲ�AI��Ϊ")
STRUCT AIActionServer_InnerAILogic
{
    DESC("��ΪID")
    int nID;
    DESC("�ڲ�AISchemeID")
    int nInnerAIScheme;

    STRUCT_INIT(AIActionServer_InnerAILogic)
};

DESC("����(ʵ��)��Ϊ")
STRUCT AIActionServer_PickUpItem
{
    DESC("��ΪID")
    int nID;
    DESC("Ŀ��")
    AI_TARGET_TYPE eTargetType;

    STRUCT_INIT(AIActionServer_PickUpItem)
};

DESC("�з���Ұ���,�ڵз���Ұ���򷢽��з���Ұ�¼����Լ�(����)")
STRUCT AIActionServer_SightCheck
{
    DESC("��ΪID")
    int nID;

    STRUCT_INIT(AIActionServer_SightCheck)
};

DESC("�������Ž׶�")
STRUCT AIActionServer_AllMid
{
    DESC("��ΪID")
    int nID;

    STRUCT_INIT(AIActionServer_AllMid)
};
