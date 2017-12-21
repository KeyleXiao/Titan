#pragma once
#include "ManagedCode.h"
#include "GameDef.h"

DESC("�Ƿ�")
ENUM MANAGED_BOOL
{
    DESC("��")
    ENUM_NO,
    DESC("��")
    ENUM_YES,
};

DESC("AI��Ϣ")
ENUM AI_MESSAGE
{
    DESC("�ƶ��޷�������Ϣ")
    AI_MESSAGE_MOVE_START_FAIL,
    DESC("ֹͣ�ƶ���Ϣ")
    AI_MESSAGE_MOVE_STOPED,
};

DESC("AI�¼�")
ENUM AI_EVENT
{
    DESC("֡�¼�")
    AI_EVENT_FRAME_UPDATE,
    DESC("�����¼�")
    AI_EVENT_BORN,
    DESC("�����¼�")
    AI_EVENT_DIE,
    DESC("�����¼�")
    AI_EVENT_REBORN,
    DESC("�Լ��������¼�")
    AI_EVENT_BEHIT,  
    DESC("�ѷ���λ������(������������ȫͼ�㲥������/���˺�Ұ�ֱ��������Ǹ����㲥)")
    AI_EVENT_FRIEND_ENTITY_BEHIT,
    DESC("�з���λ������")
    AI_EVENT_ENEMY_ENTITY_BEHIT,
    DESC("�Լ������¼�")
    AI_EVENT_LEVEL_UPGRADE,
    DESC("���ع��¼�")
    AI_EVENT_CHECK_RETRIEVE,
    DESC("�ع�����¼�")
    AI_EVENT_RETRIEVE_END,
    DESC("�����ο�ס�¼�")
    AI_EVENT_GOT_STUCK,
    DESC("���н����¼�")
    AI_EVENT_IDLE_END,
    DESC("���䱨���¼�")
    AI_EVENT_WATCHOUT_ALARM_FIRE,
    DESC("���䱨������¼�")
    AI_EVENT_WATCHOUT_ALARM_CANCEL,
    DESC("�յ����˱�ǹ����¼�")
    AI_EVENT_RECV_MASTER_MARK_ATTACK_SIGNAL,
    DESC("�������¼�")
    AI_EVENT_PICKED_UP,
    DESC("�����ﱻ�ݻ��¼�")
    AI_EVENT_BUILDING_DESTROIED,
    DESC("�յ������ź��¼�")
    AI_EVENT_ATTACK_SIGNAL,
    DESC("���˹����¼�")
    AI_EVENT_MASTER_ATTACK,
    DESC("�յ���������¼�")
    AI_EVENT_RECV_REQ_FOLLOW_OTHER,
    DESC("�յ�ȡ�������¼�")
    AI_EVENT_RECV_BREAK_FOLLOW_OTHER,
    DESC("�Լ�����з���Ұ")
    AI_EVENT_ENTER_ENEMY_SIGHT,
    DESC("�Լ��뿪�з���Ұ")
    AI_EVENT_LEAVE_ENEMY_SIGHT,
    DESC("�����¼�")
    AI_EVENT_IDLE,
    DESC("ɨ��װ�ñ����¼�")
    AI_EVENT_RADAR_ALARM,
    DESC("���������¼�")
    AI_EVENT_MASTER_DIE,

	////////////����Ӣ��AI�¼�///////////////
	DESC("�����¼�")
	AI_EVENT_FIRE,
	DESC("�ѻس��¼�")
	AI_EVENT_BACK_HOME,
	DESC("(����С��)������")
	AI_EVENT_BOSS_BEHIT,
	DESC("(Ӣ��)����ս��")
	AI_EVENT_EXIT_FIGHT,

    ////////////////////////////////////////
    DESC("����С������")
    AI_EVENT_SUMMON_SMALL_DRAGON_BORN,

    DESC("�������Ž׶�")
    AI_EVENT_ENTER_ALL_MID_STAGE,

    DESC("think�¼�")
    AI_EVENT_THINK,
};


DESC("��������")
ENUM AI_DECISION_TYPE
{
    DESC("������")
    AI_DECISION_TYPE_NONE,
    DESC("�ƶ�����")
    AI_DECISION_TYPE_MOVE,
    DESC("ʩ������")
    AI_DECISION_TYPE_CAST_SPELL,
    DESC("�Զ�������1")
    AI_DECISION_TYPE_CUSTOM_1,
    DESC("�Զ�������2")
    AI_DECISION_TYPE_CUSTOM_2,
    DESC("�����������ֵ")
    AI_DECISION_TYPE_MAX,
};

DESC("��Ӫ��ϵ")
ENUM AI_CAMP_RELATIONSHIP
{
    DESC("������Ӫ")
    AI_ANY_CAMP,
    DESC("����")
    AI_SELF_CAMP,
    DESC("�з�")
    AI_ENEMY_CAMP,
    DESC("��Ӫ��ϵ���ֵ(����ѡ)")
    AI_CAMP_MAX,
};

DESC("ʵ������")
ENUM AI_ENTITY_TYPE
{
	DESC("��������(С����������Ӣ�ۡ�С��)")
	AI_ENTITY_TYPE_ANY,
	DESC("Ӣ��")
	AI_ENTITY_TYPE_HERO,
	DESC("С��")
	AI_ENTITY_TYPE_SOLDIER,
	DESC("������")
	AI_ENTITY_TYPE_DEFENSIVE_TOWER,
	DESC("����")
	AI_ENTITY_TYPE_BASE,
	DESC("ˮ��")
	AI_ENTITY_TYPE_CRYSTAL_TOWER,
	DESC("Ұ����")
	AI_ENTITY_TYPE_WILD_MONSTER,
	DESC("��ʯ")
	AI_ENTITY_TYPE_EYE_STONE,
	DESC("Ѫ��")
	AI_ENTITY_TYPE_HP_SUPPLIES,
	DESC("С��")
	AI_ENTITY_TYPE_SMALL_DRAGON,
	DESC("����")
	AI_ENTITY_TYPE_HUGE_DRAGON,
	DESC("Ȫˮ")
	AI_ENTITY_TYPE_SPRING,
    DESC("����С��")
    AI_ENTITY_TYPE_SUMMON_SMALL_DRAGON,
	DESC("δ֪����")
	AI_ENTITY_TYPE_UNKNOWN,
	DESC("ʵ���������ֵ(����ѡ)")
	AI_ENTITY_TYPE_MAX,
};

DESC("ʵ������")
ENUM AI_ENTITY_TYPE_MASK
{
	DESC("δ֪����")
	AI_ENTITY_TYPE_UNKNOWN_M = 0,
	DESC("Ȫˮ")
	AI_ENTITY_TYPE_SPRING_M = 0x1,
	DESC("����")
	AI_ENTITY_TYPE_BASE_M = 0x2,
	DESC("������")
	AI_ENTITY_TYPE_DEFENSIVE_TOWER_M = 0x4,
	DESC("ˮ��")
	AI_ENTITY_TYPE_CRYSTAL_TOWER_M = 0x8,
    DESC("Ӣ��")
    AI_ENTITY_TYPE_HERO_M = 0x10,
    DESC("С��")
    AI_ENTITY_TYPE_SOLDIER_M = 0x20,
	DESC("�ٻ�С��")
	AI_ENTITY_TYPE_SUMMON_SMALL_DRAGON_M = 0x40,
    DESC("Ұ����")
    AI_ENTITY_TYPE_WILD_MONSTER_M = 0x80,
	DESC("С��")
	AI_ENTITY_TYPE_SMALL_DRAGON_M = 0x100,
	DESC("����")
	AI_ENTITY_TYPE_HUGE_DRAGON_M = 0x200,
    DESC("��ʯ")
    AI_ENTITY_TYPE_EYE_STONE_M = 0x400,
    DESC("Ѫ��")
    AI_ENTITY_TYPE_HP_SUPPLIES_M = 0x800,
    DESC("ʵ���������ֵ(����ѡ)")
    AI_ENTITY_TYPE_MAX_M,
};

DESC("Ŀ������")
ENUM AI_TARGET_TYPE
{
    DESC("û��Ŀ��")
    AI_TARGET_NO,
    DESC("�Լ�")
    AI_TARGET_SELF,
    DESC("������")
    AI_TARGET_OPERATOR,
    DESC("Ŀ��")
    AI_TARGET_TARGET,
    DESC("(��Ӫ��Ұ��Χ��)������ѷ�Ӣ��")
    AI_TARGET_NEAREST_FRIEND_HERO,
    DESC("(��Ӫ��Ұ��Χ��)������ѷ�������")
    AI_TARGET_NEAREST_FRIEND_DEFENSIVE_TOWER,
    DESC("(�Լ���Ұ��Χ��)���Ѫ���ѷ�Ӣ��")
    AI_TARGET_LOWEST_HP_FRIEND_HERO,
    DESC("(��Ӫ��Ұ��Χ��)����ĵз�Ӣ��")
    AI_TARGET_NEAREST_ENEMY_HERO,
    DESC("(�Լ���Ұ��Χ��)����ĵз�С��")
    AI_TARGET_NEAREST_ENEMY_SOLDIER,
    DESC("(�Լ���Ұ��Χ��)����ĵз���λ")
    AI_TARGET_NEAREST_ENEMY_ENTITY,
    DESC("(�Լ���Ұ��Χ��)���Ѫ�ĵз�Ӣ��")
    AI_TARGET_LOWEST_HP_ENEMY_HERO,
    DESC("(�Լ���Ұ��Χ��)���Ѫ�ĵз�С��")
    AI_TARGET_LOWEST_HP_ENEMY_SOLDIER,
    DESC("(��Ӫ��Ұ��Χ��)����ĵз�������")
    AI_TARGET_NEAREST_ENEMY_DEFENSIVE_TOWER,
    DESC("�Լ���Ұ��Χ�������ѷ���λ")
    AI_TARGET_ANY_FRIEND_ENTITY,
    DESC("ǰ���ѷ���λ")
    AI_TARGET_FRONT_FRIEND_ENTITY,
    DESC("����")
    AI_TARGET_MASTER,
    DESC("��Ҫ֧Ԯ��Ŀ��")
    AI_TARGET_NEED_HELP,
    DESC("���˱�ǵ�Ŀ��")
    AI_TARGETT_MARKED_BY_MASTER,
    DESC("��������")
    AI_TARGET_SELF_BASE,
    DESC("�з�����")
    AI_TARGET_ENEMY_BASE,
    DESC("(��Ӫ��Ұ��Χ��)����ĵз�ˮ��")
    AI_TARGET_NEAREST_ENEMY_CRYSTAL_TOWER,
    DESC("(�Լ���Ұ��Χ��)�����Ѫ��")
    AI_TARGET_NEAREST_HP_SUPPLIES,
    DESC("(��Ӫ��Ұ��Χ��)����ĵз�С��")
    AI_TARGET_NEAREST_ENEMY_SMALL_DRAGON,
    DESC("(�Լ���Ұ��Χ��)����ĵз�С����С��--��������ѡĿ��") // ��ʱ
    AI_TARGET_NEAREST_ENEMY_SOLDIER_OR_SAMLLDRAGON,
    DESC("ǰ�ߵз���λ")
    AI_TARGET_FRONT_ENEMY_ENTITY,
    DESC("����С��")
    AI_TARGET_NEUTRAL_SMALL_DRAGON,
    DESC("��������")
    AI_TARGET_NEUTRAL_HUGE_DRAGON,
    DESC("(ȫͼ)�����Ұ��")
    AI_TARGET_NEAREST_WILD_MONSTER_GLOBAL,
    DESC("(�Լ���Ұ��)���Ѫ��Ұ��")
    AI_TARGET_LOWEST_HP_WILD_MONSTER,
	DESC("�Ƽ���ѹ���Ŀ��")
	AI_BEST_ATTACK_TARGET,
	DESC("�Ƽ�����Ŀ��_A")
	AI_ATTACK_TARGET_A,
	DESC("�Ƽ�����Ŀ��_Q")
	AI_ATTACK_TARGET_Q,
	DESC("�Ƽ�����Ŀ��_E")
	AI_ATTACK_TARGET_E,
	DESC("�Ƽ�����Ŀ��R")
	AI_ATTACK_TARGET_R,
	DESC("AIӢ�۵ĵ�ǰ����Ҫ��������")
	AI_HERO_BEST_PROTECT_HERO,
	DESC("����AIӢ�������Ϯ����")
	AI_HERO_NEAREST_ATTACKER,
	DESC("����ĵз�NPC")
	AI_TARGET_NEAREST_ENEMY_NPC = 100,
    DESC("Ŀ���������ֵ")
    AI_TARGET_MAX = 256,
    DESC("����ص�")
    AI_TARGET_TASK_LOC,
};

struct CAIProxy; 
class AIDecision;
// AI��������Ϊ������
STRUCT AI_CONTEXT
{
    int        nEventID;
    CAIProxy*  pAIProxy; 
    AIDecision* pDecision;
    UID        uidOperator;
    UID        uidTarget;
    int        nSchemeID;
    int        nDecisionSN;
    sp_float_array(loc, 3);
    int        nParam1;
    int        nParam2;
    int        nParam3;

    STRUCT_INIT(AI_CONTEXT)
};