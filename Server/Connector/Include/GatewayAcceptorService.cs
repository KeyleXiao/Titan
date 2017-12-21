using System;
using System.Reflection;
using System.IO;
using System.Text;

struct UID
{
};

struct SERVICE_ID
{
};

/**
	职责: 
	1.接受用户连接，创建Actor服务
	2.将收到的网络消息，转发给Actor处理
	3.根据UID,获取Actor代理接口
*/
interface GatewayAcceptorService
{
	/**
	根据UID查找某个玩家
	*/
	SERVICE_ID findActor( UID nActor );
};
