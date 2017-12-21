#pragma once
#include "IRedisTest.h"


struct IValueOp;

// �������Ͳ���
class ValueOpTest : public IRedisTest
{
public:
	ValueOpTest(Redis::IValueOp* pValueOp);

	virtual const char* GetName() override;

	virtual bool Run() override;

private:
	// ����
	bool TestEmptyStruct();
	// �̶���С����
	bool TestFixStruct();
	// ���Լ̳��� ISerializableData ���ࣨ��̬��С���ࣩ
	bool TestSerializableData();
private:
	Redis::IValueOp*	m_pValueOp;
};