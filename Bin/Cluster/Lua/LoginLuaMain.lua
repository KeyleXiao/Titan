-------------------------------------------------------------------
-- ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
-- ������:	������
-- ��  ��:	12/27/2016
-- ��  ��:	1.0
-- ��  ��:	���������
-------------------------------------------------------------------

---------------------------------------------------
-- LUA��ں��������ڼ���LUA�ļ�
---------------------------------------------------
function lua_main()
	require("AnalyseMap")
	RegisterPartner();
	RegisterWebApi();
end


function RegisterPartner()
	-- ����
	partnerHelper.RegisterPartner( 100, "�����½", "Web__","123456","http://openapi.game.q1.com/account/SzGla_callback.aspx?access_token=%s&state=server" )
	-- ����
	--partnerHelper.RegisterPartner( 100, "�����½", "Web__","123456","http://openapi.wxg.dev.q1.com/account/SzGla_callback.aspx?access_token=%s&state=server" )
end
	
function UnRegisterPartner( nID )
	partnerHelper.UnRegisterPartner( nID)
end

-- ����WEB�е�API����
-- id: ���ڱ�ʶ
-- key
-- weburl
-- ��������(�Ƽ�.��ϷID)
function RegisterWebApi()
	-- ����
	WebApi.Register( 1, "9895FD16-FD7B-1B1E-8BC7-DC53144A39C4", "http://tg.q1.com/ws/RecommendApi.asmx/GetRecommendUserList", 7 );	
	-- ����
	--WebApi.Register( 1, "9895FD16-FD7B-1B1E-8BC7-DC53144A39C4", "http://tg.szg1a.net/ws/RecommendApi.asmx/GetRecommendUserList", 7 );
end