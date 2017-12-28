-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	������
-- ��  ��:	12/13/2016
-- ��  ��:	1.0
-- ��  ��:	ʹ�÷�ʦ,�����Ϸ
-------------------------------------------------------------------

require "TaskFramePlayDotaGame"

-------------------------------------------------------------------
-- ʹ�÷�ʦ,�����Ϸ
-------------------------------------------------------------------
Task6_MageGame = TaskFramePlayDotaGame:new
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
function Task6_MageGame:OnEventPlayDotaGame(tGameData)

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

	-- �ж�Ӣ������
	if self.finish_condition_hero_type ~= nil then
		local bFind = false
		for index,hero_type in pairs(self.finish_condition_hero_type) do
			if tWarResultData[1] == hero_type then
				bFind = true
				break
			end
		end
		if bFind == false then
			return
		end
	end

	TaskFramePlayDotaGame.OnEventPlayDotaGame(self, tGameData)
end