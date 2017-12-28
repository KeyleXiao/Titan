-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	��ҫ��
-- ��  ��:	3/31/2017
-- ��  ��:	1.0
-- ��  ��:  ʦͽ��Ϸ
-------------------------------------------------------------------

require "TaskFramePlayDotaGame"

-------------------------------------------------------------------
-- ʦͽ��Ϸ
-------------------------------------------------------------------
Task_38_MentorshipGame = TaskFramePlayDotaGame:new
{
}

----------------------------------------------------------------------------
-- ��DOTA��Ϸִ��

-- param1 : �����б�
-- param2 : �������ͳ��
-- param2������1.Ӣ������2.��������
-- 			   3.�Ƿ��������4.�Ƿ�ʦͽ���5.�Ƿ�ʤ��6.�Ƿ���7.�Ƿ�MVP8.�Ƿ�������9.�Ƿ�������10.�Ƿ��������11.�Ƿ��Ǯ���
-- 			   12.�����ɱ����13.�������ɱ����14.��ɱ����15.��������16.ս������17.��ý�Ǯ����18.��ʤ��������19.���վ�������20.��������21.��ɱС������22.��ɱ��������23.��������
----------------------------------------------------------------------------
function Task_38_MentorshipGame:OnEventPlayDotaGame(tGameData)

	if self.holder_id == nil or self.holder_id <= 0 then
		return
	end

	if tGameData == nil then
		return
	end

	if tGameData.nParam2 == nil then
		return
	end
	
	local tWarResultData = split_string(tGameData.nParam2, ";", tonumber)
	if tWarResultData == nil or table_count(tWarResultData) < 23 then
		return
	end

	-- �ж�ʦͽ�����Ƿ�ƥ��
	if tWarResultData[4] ~= 1 then
		return
	end

	TaskFramePlayDotaGame.OnEventPlayDotaGame(self, tGameData)
end