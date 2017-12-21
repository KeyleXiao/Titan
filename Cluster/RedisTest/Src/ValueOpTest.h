#pragma once
#include "IRedisTest.h"


struct IValueOp;

// 基本类型测试
class ValueOpTest : public IRedisTest
{
public:
	ValueOpTest(Redis::IValueOp* pValueOp);

	virtual const char* GetName() override;

	virtual bool Run() override;

private:
	// 空类
	bool TestEmptyStruct();
	// 固定大小的类
	bool TestFixStruct();
	// 测试继承了 ISerializableData 的类（动态大小的类）
	bool TestSerializableData();
private:
	Redis::IValueOp*	m_pValueOp;
};