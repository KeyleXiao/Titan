#pragma once


struct TestStruct : public ISerializableData
{
	int		a;
	string	str;
	double	b;

	TestStruct()
	{
		a = 142857;
		str = "test";
		b = 1.23456789;
	}

	// 反序列化
	virtual	void fromBytes(const char* p, size_t len)
	{
		ibuffer ibufData(p, len);
		ibufData >> a >> str >> b;
	}

	// 序列化
	virtual	void toBytes(obuf& obufData) const
	{
		obufData << a << str << b;
	}

	bool operator == (const TestStruct& rhs)//重载 ==  
	{
		return this->a == rhs.a
			&& (abs(this->b) - abs(rhs.b) < 0.0001)
			&& this->str == rhs.str;
	}

};
inline TestStruct MokeEmptyTestStruct()
{
	TestStruct tmp;
	tmp.a = 0;
	tmp.b = 0.0;
	tmp.str = "";

	return tmp;
}

struct EmptyFixStruct
{
};
struct FixStruct
{
	EmptyFixStruct ef;
	char	sz[32];
	int		a;
	short	b;
	double	f;

	FixStruct() { memset(this, 0, sizeof(*this)); }

	bool operator == (const FixStruct& rhs)//重载 ==  
	{
		return this->a == rhs.a
			&& this->b == rhs.b
			&& (abs(this->f) - abs(rhs.f) < 0.0001);
	}
};

inline FixStruct MokeFixStruct()
{
	FixStruct fixIn;
	char* sztmp = "qwertyuiop";
	strcpy(fixIn.sz, sztmp);
	fixIn.a = 100859;
	fixIn.b = 110;
	fixIn.f = 9.99;

	return fixIn;
}


#define DCheck(TYPE, a, b)	{			\
	if(a != b)							\
	{									\
		string strErr = #TYPE;			\
		ErrorLn(strErr.c_str() << " is not equal! a=" << a << ", b=" << b);	\
		return false;					\
	}									\
}