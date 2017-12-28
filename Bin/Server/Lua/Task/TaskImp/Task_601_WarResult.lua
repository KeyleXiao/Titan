-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	周坤
-- 日  期:	8/4/2017
-- 版  本:	1.0
-- 描  述:	最佳队友
-------------------------------------------------------------------

require "TaskFramePlayDotaGame"

-------------------------------------------------------------------
-- 最佳队友
-------------------------------------------------------------------
Task_601_WarResult = TaskFramePlayDotaGame:new
{
}

----------------------------------------------------------------------------
-- 在一局比赛中助攻达到5

-- param1 : 队友列表
-- param2 : 比赛结果统计
-- param2解析：1.英雄类型2.比赛类型
-- 			   3.是否联盟组队4.是否师徒组队5.是否胜利6.是否超神7.是否MVP8.是否输出最高9.是否拆塔最高10.是否助攻最高11.是否金钱最高
-- 			   12.最大连杀数量13.最大不死击杀数量14.击杀数量15.助攻数量16.战绩评分17.获得金钱数量18.决胜控制数量19.惊险救治数量20.拆塔数量21.击杀小龙数量22.击杀大龙数量23.死亡数量
----------------------------------------------------------------------------
function Task_601_WarResult:OnEventPlayDotaGame(tGameData)

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

	-- 判断助攻数量
	if tWarResultData[15] < 5 then
		return
	end

	TaskFramePlayDotaGame.OnEventPlayDotaGame(self, tGameData)
end