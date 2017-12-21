#pragma once
#include <string>
#include <type_traits>
#include "buffer.h"
#include "ISerializableData.h"
using std::string;


template <typename T, typename U>
struct decay_equiv :
	std::is_same<typename std::decay<T>::type, U>::type
{};


inline string TToString(const char* sz)
{
	return string(sz);
}
inline string TToString(const char*& sz)
{
	return string(sz);
}
inline string TToString(char* const& sz)
{
	return string(sz);
}


template<typename T>
string TToString(const T& tValue)
{
	if (std::is_same<std::string, T>::value)
		return (string&)tValue;

	obuf obufData;
	if (std::is_base_of<ISerializableData, T>::value)
	{
		const ISerializableData& isd = (const ISerializableData&)tValue;
		isd.toBytes(obufData);
	}
	else
	{
		obufData << tValue;
	}

	return string(obufData.data(), obufData.size());// 编译器有返回值优化，所以这里效率不成问题
}


template<typename T>
void TFromString(const string& strValue, T& tValue)
{
	if (std::is_base_of<ISerializableData, T>::value)
	{
		ISerializableData& isd = (ISerializableData&)tValue;
		isd.fromBytes(strValue.c_str(), strValue.size());
	}
	else if (decay_equiv<T, std::string>::value)
	{
		string& str = (string&)tValue;
		str = strValue;
	}
	else
	{
		ibuffer ibufData(strValue.c_str(), strValue.size());
		ibufData >> tValue;
	}
}


struct IConverter
{
public:
	virtual	void	Convert() = 0;
};

template<typename T>
struct Converter : public IConverter
{
	T&		m_t;
	string	m_strValue;

	Converter(T& t) : m_t(t) {}

	virtual void Convert() override
	{
		TFromString(m_strValue, m_t);
	}
};