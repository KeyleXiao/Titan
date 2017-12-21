#pragma once


struct IRedisTest
{
	virtual	const char*	GetName() = 0;

	virtual	bool		Run() = 0;
};