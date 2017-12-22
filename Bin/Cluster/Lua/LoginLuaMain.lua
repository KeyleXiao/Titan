-------------------------------------------------------------------
-- 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
-- 创建人:	秦其勇
-- 日  期:	12/27/2016
-- 版  本:	1.0
-- 描  述:	主函数入口
-------------------------------------------------------------------

---------------------------------------------------
-- LUA入口函数，用于加载LUA文件
---------------------------------------------------
function lua_main()
	require("AnalyseMap")
	RegisterPartner();
	RegisterWebApi();
end


function RegisterPartner()
	-- 外网
	partnerHelper.RegisterPartner( 100, "共享登陆", "Web__","123456","http://openapi.game.q1.com/account/SzGla_callback.aspx?access_token=%s&state=server" )
	-- 内网
	--partnerHelper.RegisterPartner( 100, "共享登陆", "Web__","123456","http://openapi.wxg.dev.q1.com/account/SzGla_callback.aspx?access_token=%s&state=server" )
end
	
function UnRegisterPartner( nID )
	partnerHelper.UnRegisterPartner( nID)
end

-- 设置WEB中的API数据
-- id: 用于标识
-- key
-- weburl
-- 附加数据(推荐.游戏ID)
function RegisterWebApi()
	-- 外网
	WebApi.Register( 1, "9895FD16-FD7B-1B1E-8BC7-DC53144A39C4", "http://tg.q1.com/ws/RecommendApi.asmx/GetRecommendUserList", 7 );	
	-- 内网
	--WebApi.Register( 1, "9895FD16-FD7B-1B1E-8BC7-DC53144A39C4", "http://tg.szg1a.net/ws/RecommendApi.asmx/GetRecommendUserList", 7 );
end