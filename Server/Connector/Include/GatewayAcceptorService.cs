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
	ְ��: 
	1.�����û����ӣ�����Actor����
	2.���յ���������Ϣ��ת����Actor����
	3.����UID,��ȡActor����ӿ�
*/
interface GatewayAcceptorService
{
	/**
	����UID����ĳ�����
	*/
	SERVICE_ID findActor( UID nActor );
};
