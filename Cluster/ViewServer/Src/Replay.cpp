#include "stdafx.h"
#include "Replay.h"
#include "RedisMng.h"
#include "ViewRedisKey.h"


void Replay::Start()
{
	Redis::IRedisConnector* pRedis = gRedisMng.GetRedis();
	string key = ViewRedisKey::BuildReplayKey(m_rID);

	long length = 0;
	pRedis->listOp()->lLen(key, &length);

	//int aArray[10];
	//pRedis->listOp()->lRange(key, 0, -1, )
}
