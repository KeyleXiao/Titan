-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	���к�
-- ��  ��:	2017/1/18
-- ��  ��:	1.0
-- ��  ��:	ά��NPC���ʱ��������
-------------------------------------------------------------------

------------------------------------------------------------------- 
-- NPC���ܶ���ķ���:
-- 1.�����ﶨ��һ��NPC����ID,ע�ⲻҪ�������˵Ĺ���ID�ظ�
-- 2.�ں����g_NPCFunctionHandler�ﶨ��������ܶ�Ӧ�Ĵ�����
-- 3.��NPC.LUA�и�ָ����NPC�����������
-- 4.����ҵ����NPCʱ�ͻ��Զ����õ�g_NPCFunctionHandler�ﶨ��ĺ��� 
------------------------------------------------------------------- 
NPC_WAR_SHOP		= 1				-- ս���̵�NPC
NPC_WINGS			= 2				-- ���NPC
NPC_GOODS_SHOP      = 3             -- ����װ��
NPC_CURE_MEMBER     = 4             -- �������

NPC_VIEW_WAR_DEFAULT      = 1             -- ������
NPC_VIEW_WAR_GOODS_SHOP   = 2             -- Ұ��NPC�̵����

------------------------------------------------------------------
-- NPC���ܴ����
-- @param note : ����NPC�Ĺ���ID����ָ������Ӧ���� 
-- ��ʽ: [����ID] = { "�Ի���Ŀ","��Ӧ����",�������ȼ�, "�Ի���ɫ", �Ƿ��Լ����ɶԻ���Ŀ}
------------------------------------------------------------------- 
g_NPCFunctionHandler = {}

g_NPCFunctionHandler[NPC_WAR_SHOP]		= {"������Ʒ", "RequestOpenWarShop", 10, "#FFDD00"}
g_NPCFunctionHandler[NPC_WINGS]			= {"������", "BuildBuyWings", 10, "#FFDD00", true}
g_NPCFunctionHandler[NPC_GOODS_SHOP]	= {"����װ��", "BuildBuyShopGoods", 10, "#FFDD00", true}
g_NPCFunctionHandler[NPC_CURE_MEMBER]	= {"�������", "BuildWarCureMember", 10, "#FFDD00", true}