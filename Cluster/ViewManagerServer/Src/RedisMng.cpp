#include "stdafx.h"
#include "RedisMng.h"
#include "ViewMngSetting.h"
#include "buffer.h"


RedisMng& gRedisMng = RedisMng::getInstance();

RedisMng::RedisMng()
	: m_pRedis(nullptr)
{
	m_pRedis = CreateRedisConnectorProc();
}

bool RedisMng::Init()
{
	string& str = gSetting.m_strRedisIp;
	WORD port = gSetting.m_wRedisPort;

	// 初始化Redis连接
	D_IF_FALSE_RETURN_FALSE(m_pRedis->create(str.c_str(), port));

	// 检查版本号
	D_IF_FALSE_RETURN_FALSE(CheckVersion());

	return true;
}

bool RedisMng::CheckVersion()
{
	string key("version");
	string strVersion;
	D_IF_FALSE_RETURN_FALSE(m_pRedis->valueOp()->get(key, strVersion));

	if (strVersion == gSetting.m_strVersion)
		return true;

	D_IF_FALSE_RETURN_FALSE(m_pRedis->serverOp()->FlushAll());
	D_IF_FALSE_RETURN_FALSE(m_pRedis->valueOp()->set(key, gSetting.m_strVersion));

	return true;
}

