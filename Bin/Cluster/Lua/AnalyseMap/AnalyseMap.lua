
MIRROR_MAP_BORN_MAX_PALERY = 80
--出生的镜像地图
MIRROR_MAP_LIST_OPEN_BORN = {1}

-- 进入的镜像地图
MIRROR_MAP_LIST_OPEN_ENTER =
			{
				[1]	= {1, 91, 92},

			}

--------------------------------------------------------------------------------
--判断出生在哪个地图
--参数：mapid
--返回0：表示无效，反之有效
--------------------------------------------------------------------------------
function AnalyseBornWhoMap()
	-- 在多个镜像地图里挑个人最少的
	local openMirrorMapString = ""
	for i, openMapID in ipairs(MIRROR_MAP_LIST_OPEN_BORN)  do
		if openMirrorMapString == "" then
			openMirrorMapString = MIRROR_MAP_BORN_MAX_PALERY..","..openMapID
		else
			openMirrorMapString = openMirrorMapString..","..openMapID
		end

	end

	local resultMapID = mapAnalyseHelper.GetAnalyseBornMap(openMirrorMapString)
	if resultMapID == 0 then
		return 1
	end

	return resultMapID
end

--------------------------------------------------------------------------------
--判断出生在哪个地图
--参数：mapid
--返回0：表示无效，反之有效
--------------------------------------------------------------------------------
function AnalyseEnterWhoMap(mapidInput)

	-- 在多个镜像地图里挑个人最少的
	--local openMirrorMapString = ""
	--for i, openMapID in MIRROR_MAP_LIST_OPEN_ENTER[groupID] do
	--	openMirrorMapString = openMirrorMapString..","..NoNationToTrueMap(mapidInput, openMapID)
	--end

	local resultMapID = 0 --GetPersonLeastInMapList(openMirrorMapString)
	if resultMapID == 0 then
		return 0
	end

	return resultMapID
end
