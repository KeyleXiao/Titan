#pragma once
#include "TFunc.h"


namespace Redis
{
	/*�������ο� http://redis.cn/commands.html*/
	// Serverָ�������
	struct IServerOp
	{
		// ����������ݿ� (ȷʵҪɾ������Keyʱ���ã��������˽��������߿��Բο�������ɾ�⵽��·��...)
		virtual	bool	FlushAll() = 0;

		// ��յ�ǰ���ݿ� (ȷʵҪɾ������Keyʱ���ã��������˽��������߿��Բο�������ɾ�⵽��·��...)
		virtual	bool	FlushDb() = 0;
	};

};