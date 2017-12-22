#include "stdafx.h"
#include "RedisMng.h"
#include "buffer.h"
#include "Setting.h"


RedisMng& gRedisMng = RedisMng::getInstance();

RedisMng::RedisMng()
	: m_pRedis(nullptr)
{
	m_pRedis = CreateRedisConnectorProc();
}

bool RedisMng::Init()
{
	const string& str = gSetting.m_strRedisIp;
	const WORD& port = gSetting.m_wRedisPort;

	return m_pRedis->create(str.c_str(), port);
}
