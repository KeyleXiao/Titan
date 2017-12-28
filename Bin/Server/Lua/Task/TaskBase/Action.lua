-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	������
-- ��  ��:	12/14/2016
-- ��  ��:	1.0
-- ��  ��:	����ģ��
-------------------------------------------------------------------

require "Class"

--[[
  ���ǽ������еĶ������Ƴ����һ�����Ķ�������ʵ��
  ��Щ��������������ɵķ�������ϵͳ�Ľ��ܶ����б�,�ƽ������б�,��ɶ����б���
  ʾ��:
  T1_FirstWin = Task:new{
    ...
  	tFinishAction={ActionAddPrize:new{prize_id=1,prize_count=1}},
    ...
  }
  
  ÿ������֧��һ������:
  1.run(actor,task) �Զ�������ִ��

  ��ϵͳ֧�ֵĶ�����: 
  ActionAddTaskDayCount					: ����ÿ�������� [����������Ķ���ʵ��]
  ActionAddTaskWeekCount				: ����ÿ�������� [����������Ķ���ʵ��]
  ActionBinding							: ������� [����������Ķ���ʵ��]
  ActionHeroExp							: ������ɫ���� [����������Ķ���ʵ��]
  ActionAddPrize						: ������Ʒ [����������Ķ���ʵ��]
]]--

-------------------------------------------------------------------
-- ����ÿ��������
-------------------------------------------------------------------
ActionAddTaskDayCount = Object:new
{
}

function ActionAddTaskDayCount:run(holder_id, task)

	if holder_id == nil or task == nil then
		return
	end

 	task:SetTaskDayCount(task:GetTaskDayCount() + 1)
end

-------------------------------------------------------------------
-- ����ÿ��������
-------------------------------------------------------------------
ActionAddTaskWeekCount = Object:new
{
}

function ActionAddTaskWeekCount:run(holder_id, task)

	if holder_id == nil or task == nil then
		return
	end

 	task:SetTaskWeekCount(task:GetTaskWeekCount() + 1)
end

-------------------------------------------------------------------
-- ���� : �������
-- @param binding_gold	  : �������
-------------------------------------------------------------------
ActionBinding = Object:new
{
	binding_gold = nil,
}

function ActionBinding:run(holder_id, task)
	
	if self.binding_gold == nil then
		return
	end

	if holder_id == nil or task == nil then
		return
	end

	local nTicket = 0;
	local nGlod   = self.binding_gold;
	if CanAddMoney(holder_id, nTicket, nGlod) == false then
		return
	end

	-- ���ӽ������
	AddMoney(holder_id, nTicket, nGlod, task.name)
end

-------------------------------------------------------------------
-- ���� : ������ɫ����
-- @param actor_exp	  : ������ɫ����
-------------------------------------------------------------------
ActionHeroExp = Object:new
{
	hero_exp = nil,
}

function ActionHeroExp:run(holder_id, task)
	
	if self.hero_exp == nil then
		return
	end

	if holder_id == nil or task == nil then
		return
	end

	-- ���ӽ�����ɫ����
	SetActorBaseExp(holder_id, self.hero_exp, task.name)
end

-------------------------------------------------------------------
-- ���� : ������Ʒ
-- @param prize_id	  : ������ƷID
-- @param prize_count : ������Ʒ����
-------------------------------------------------------------------
ActionAddPrize = Object:new
{
	prize_id = nil,
	prize_count = nil,
}

function ActionAddPrize:run(holder_id, task)
	
	if self.prize_id == nil or self.prize_id <= 0 then
		return
	end

	if self.prize_count == nil or self.prize_count <= 0 then
		return
	end

	if holder_id == nil or task == nil then
		return
	end

	if CanPrizeActor(holder_id, self.prize_id, self.prize_count) == false then
		return
	end

	-- ���ӽ���
	PrizeActor(holder_id, self.prize_id, self.prize_count, holder_id, task.name)
end