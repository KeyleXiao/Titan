-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	���к�
-- ��  ��:	2017/1/18
-- ��  ��:	1.0
-- ��  ��:	����NPC����
-------------------------------------------------------------------

-- NPC����ID���� 
require("NPCFunctionDef")

--------------------------------------------------------------------------------
-- NPC���÷��� (��Ϸ�е�NPC��Ҫ��NPC_TABLE�н��ж���,��������������˵��)
-- 1.name          : npc����
-- 2.id            : npc id
-- 4.location      : ����λ�� { ��ͼ,x����,y����,��Ҫ�������(��ѡ) }
-- 5.mirror(��ѡ)  : ����,��ЩNPC��Ҫ�ڶ��λ�ô��� { {��ͼ1,x����1,y����1},{��ͼ2,x����2,y����2} }
-- 6.dialog        :  Ĭ�϶԰�,������Ի����������д���Ի�(�Զ��ŷָ�),ϵͳ�����ѡһ�仰˵
-- 7.dialog_handler: �԰״�����,Ĭ��ΪDefaultNPCDialogHandler
-- 8.function(��ѡ): NPC�����б�,�����й����б���,���ֻ����һ���,����ֱ����,����ж���������һ����
--------------------------------------------------------------------------------

NPC_TABLE = {

	[10001] = { 
				name = ("ս���̵�"),										 -- ����
				id = 10001,                      			 				 -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				--["mirror"] = {{0;0;0},{0;0;0}}
				["dialog"] = { ("�ɵ���ҩ,���ðٻ�,���治��,��������")},		 -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_WAR_SHOP,                             	 -- ��NPC�Ĺ���
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                         -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			  },
	[10002] = { 
				name = ("����ʹ��"),											 -- ����
				id = 10002,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������ÿ��ֻҪ100���Ŷ��")},			 -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_WINGS,									 -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_DEFAULT                          -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			  },

	[2178] = { 
				name = ("��ʱ���̵�"),											 -- ����
				id = 2178,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			  },
	[2179] = { 
				name = ("��ʱ���̵�"),											 -- ����
				id = 2179,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2180] = { 
				name = ("�����̳"),											 -- ����
				id = 2180,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("������ѣ�")},			 -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_CURE_MEMBER,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_DEFAULT                         -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			  },
	[2181] = { 
				name = ("��ʱ���̵�"),											 -- ����
				id = 2181,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2182] = { 
				name = ("��ʱ���̵�"),											 -- ����
				id = 2182,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2183] = { 
				name = ("��ʱ���̵�"),											 -- ����
				id = 2183,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2184] = { 
				name = ("��ʱ���̵�"),											 -- ����
				id = 2184,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
			 
	[2185] = { 
				name = ("��ʱ���̵�"),											 -- ����
				id = 2185,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },

	[2186] = { 
				name = ("װ��"),											 -- ����
				id = 2186,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2187] = { 
				name = ("װ��"),											 -- ����
				id = 2187,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2188] = { 
				name = ("װ��"),											 -- ����
				id = 2188,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2189] = { 
				name = ("װ��"),											 -- ����
				id = 2189,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2190] = { 
				name = ("װ��"),											 -- ����
				id = 2190,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2191] = { 
				name = ("װ��"),											 -- ����
				id = 2187,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2192] = { 
				name = ("װ��"),											 -- ����
				id = 2192,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2193] = { 
				name = ("װ��"),											 -- ����
				id = 2193,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2194] = { 
				name = ("װ��"),											 -- ����
				id = 2194,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2195] = { 
				name = ("װ��"),											 -- ����
				id = 2195,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2205] = { 
				name = ("װ��"),											 -- ����
				id = 2205,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2206] = { 
				name = ("װ��"),											 -- ����
				id = 2206,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2207] = { 
				name = ("װ��"),											 -- ����
				id = 2207,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2208] = { 
				name = ("װ��"),											 -- ����
				id = 2208,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
	[2209] = { 
				name = ("װ��"),											 -- ����
				id = 2209,							                         -- id
				["location"] = {0,0,0},                             	 	 -- Ĭ��λ��
				["rotation"] = {0,0,0},                             	 	 -- ��ת����
				["dialog"] = { ("�������⹺��װ���ɣ�")},			         -- Ĭ�϶԰�
				["dialog_handler"] = "DefaultNPCDialogHandler",         	 -- �Ի�������
				["function"] = NPC_GOODS_SHOP,							     -- ��NPC�Ĺ���
				show_default_dialog = true,									 -- ��ʾĬ�϶Ի�
				npc_view_type = NPC_VIEW_WAR_GOODS_SHOP                      -- NPC �������ͣ��ͻ�����ʾʲô���Ľ���
			 },
--------------------------------------------------------------------------------

}    ---------�����Ŷ�Ӧ��ʼ����