-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	���к�
-- ��  ��:	2017/1/18
-- ��  ��:	1.0
-- ��  ��:	NPC��ʼ��
-------------------------------------------------------------------

-- NPC����ID���� 
require("NPCHandler")
require("NPCFuncHandler")


if (InitNPCHandler() == false) then
	error("Init NPCHandler failed.")
end

if (InitNPCFuncHandler() == false) then
	error("Init NPCFuncHandler failed.")
end