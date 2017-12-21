#pragma once
#include "IRedisConnector.h"



class RedisMng : public Singleton<RedisMng>
{
public:
	RedisMng();

	bool	Init();

private:
	//************************************
	// Returns:   bool ֻ��Redis�޷����ӵ�����·���false
	// Qualifier: ���汾�š������Redis�а汾�����ҵĲ�һ�£������Redis����д���µİ汾�ţ�
	//************************************
	bool	CheckVersion();

private:
	Redis::IRedisConnector*		m_pRedis;
};
extern RedisMng& gRedisMng;


