
MIRROR_MAP_BORN_MAX_PALERY = 80
--�����ľ����ͼ
MIRROR_MAP_LIST_OPEN_BORN = {1}

-- ����ľ����ͼ
MIRROR_MAP_LIST_OPEN_ENTER =
			{
				[1]	= {1, 91, 92},

			}

--------------------------------------------------------------------------------
--�жϳ������ĸ���ͼ
--������mapid
--����0����ʾ��Ч����֮��Ч
--------------------------------------------------------------------------------
function AnalyseBornWhoMap()
	-- �ڶ�������ͼ�����������ٵ�
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
--�жϳ������ĸ���ͼ
--������mapid
--����0����ʾ��Ч����֮��Ч
--------------------------------------------------------------------------------
function AnalyseEnterWhoMap(mapidInput)

	-- �ڶ�������ͼ�����������ٵ�
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
