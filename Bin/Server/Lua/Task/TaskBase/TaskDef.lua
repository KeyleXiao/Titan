-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	��ҫ��
-- ��  ��:	3/20/2017
-- ��  ��:	1.0
-- ��  ��:	������
-------------------------------------------------------------------
-- ��������
TaskType =
{
	FirstWinType	= 1,				-- ��ʤ����
	ClanType		= 2,				-- ��������
	CycleType		= 3,				-- ѭ������
	MainType		= 4,				-- ��������
	ContacType		= 5,				-- ��������
	DailyType		= 6,				-- �ճ�����
	RankType		= 7,				-- ��λ����
}

-- ����״̬
TaskState =
{
	BeginState		= 0,				-- ��ʼ
	AcceptState		= 1,				-- ����
	FinishState		= 2,				-- ���
	EndState		= 3,				-- ����ȡ
}

-- �������ݴ����¼�
TaskGameEventType =
{
	EventType_LEVEL 			= 1,			-- ��ҵȼ��仯�¼�
	EventType_FINISHMATCH		= 2,			-- ���һ������
	EventType_ADDHERO			= 3,			-- ���һ������Ӣ��
	EventType_ADDSKIN			= 4,			-- ���һ������Ƥ��
	EventType_OPENCHEST			= 5,			-- ��������
	EventType_INLAYGEM			= 6,			-- ��Ƕ��ʯ
	EventType_HEROUPGRADE		= 7,			-- Ӣ�۽���
	EventType_BUIDMENTORSHIP	= 8,			-- ����ʦͽ��ϵ
	EventType_CLANDONATE		= 9,			-- ���˾���
	EventType_APPLYFRIEND		= 10,			-- �����������
	EventType_CONTACTREPLY		= 11,			-- �ظ���������
	EventType_PK				= 12,			-- PK
	EventType_NovideGuide		= 13,			-- ��������
}

