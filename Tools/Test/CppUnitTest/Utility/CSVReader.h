/*****************************************************************************
** 文件名:	CSVReader.h
** 版  权:	(C)
** 创建人:	彭璐
** 日  期:	2014/05/22
** 版  本:	1.0
** 描  述:	一个通用快速的CSV文件格式读取类
** 应  用:	
**************************** 修改记录 ****************************************/
/** 文件格式如下, 注意(第一列必须为key值)
*	key,	v1,		v2,			v3
*	int,	int,	string,		int
*	1,		1,		hello,		2009
*	2,		1,		hello,		2010
*/

#ifndef _CSVREADER_H
#define _CSVREADER_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cassert>

using namespace std;

// CSVReader只支持以下几种类型的读取
enum EMValueType
{
	EMValueType_Char,
	EMValueType_Int,
	EMValueType_Float,
	EMValueType_Double,
	EMValueType_String,
	EMValueType_Max,
};

template<typename T>
class CParamTraits
{
	typedef T			value_type;
	enum
	{
		type_num = EMValueType_Max
	};
	static const char*	type_str;
	static const char*	first_format;
	static const char*	second_format;
};

template<>
class CParamTraits<char>
{
public:
	typedef char		value_type;
	enum
	{
		type_num = EMValueType_Char
	};
	static const char*	type_str;
	static const char*	first_format;
	static const char*	second_format;
};

template<>
class CParamTraits<int>
{
public:
	typedef int			value_type;
	enum
	{
		type_num = EMValueType_Int
	};
	static const char*	type_str;
	static const char*	first_format;
	static const char*	second_format;
};

template<>
class CParamTraits<float>
{
public:
	typedef float		value_type;
	enum
	{
		type_num = EMValueType_Float
	};
	static const char*	type_str;
	static const char*	first_format;
	static const char*	second_format;
};

template<>
class CParamTraits<double>
{
public:
	typedef double		value_type;
	enum
	{
		type_num = EMValueType_Double
	};
	static const char*	type_str;
	static const char*	first_format;
	static const char*	second_format;
};

template<>
class CParamTraits<string>
{
public:
	typedef string		value_type;
	enum
	{
		type_num = EMValueType_String
	};
	static const char*	type_str;
	static const char*	first_format;
	static const char*	second_format;
};

inline int CSVReader_GetType(const string& strType)
{
	if (strType == CParamTraits<char>::type_str)
		return CParamTraits<char>::type_num;
	if (strType == CParamTraits<int>::type_str)
		return CParamTraits<int>::type_num;
	if (strType == CParamTraits<float>::type_str)
		return CParamTraits<float>::type_num;
	if (strType == CParamTraits<double>::type_str)
		return CParamTraits<double>::type_num;
	if (strType == CParamTraits<string>::type_str)
		return CParamTraits<string>::type_num;
	return EMValueType_Max;
}

template<typename T>
inline void CSVReader_ReadParam(const char* &strInput, char* &val, bool bFirstParam)
{
	typedef CParamTraits<T> traits_type;
	int nBytes = 0;
	if (bFirstParam)
		sscanf(strInput, traits_type::first_format, val, &nBytes);
	else
		sscanf(strInput, traits_type::second_format, val, &nBytes);
	strInput += nBytes;
	val += sizeof(traits_type::value_type);
}

template<>
inline void CSVReader_ReadParam<string>(const char* &strInput, char* &val, bool bFirstParam)
{
	typedef CParamTraits<string> traits_type;
	int nBytes = 0;
	string& strVal = (*(string*)val);
	char buf[128] = {0};
	if (bFirstParam)
		sscanf(strInput, traits_type::first_format, buf, &nBytes);
	else
		sscanf(strInput, traits_type::second_format, buf, &nBytes);
	strInput += nBytes;
	val += sizeof(traits_type::value_type);
	strVal = buf;
}

struct SProperty
{
	string		strName;		// 字段名
	string		strType;		// 类型串
	int			emType;			// 字段类型
};

template<typename KeyType, typename ItemType>
class CCSVReader
{
public:
	typedef map<KeyType, ItemType>		MapType;
	typedef typename MapType::iterator	iterator;

	iterator find(const KeyType& key)
	{
		return m_mapItems.find(key);
	}

	iterator begin()
	{
		return m_mapItems.begin();
	}

	iterator end()
	{
		return m_mapItems.end();
	}

	bool ReadFile(string strFilePath)
	{
		ifstream ifs(strFilePath.c_str());
		if (!ifs.bad())
		{
			size_t nLine = 0;
			size_t nCol = 1;
			while (!ifs.eof())
			{
				if (ifs.getline(m_szLine, sizeof(m_szLine)))
					++nLine;

				size_t len = strlen(m_szLine);
				if (len == 0)
					break;

				const char* szStr = m_szLine;
				switch (nLine)
				{
				case 1:		// 第一行是名称
					{
						for (size_t i = 0; i < len; ++i)
							if (m_szLine[i] == ',')
								nCol++;

						m_property.resize(nCol);

						for (size_t i = 0; i < nCol; ++i)
						{
							char *data = (char*)&m_property[i].strName;
							CSVReader_ReadParam<string>(szStr, data, i==0);
						}
					}
					break;

				case 2:		// 第二行是类型
					{
						for (size_t i = 0; i < m_property.size(); ++i)
						{
							char *data = (char*)&m_property[i].strType;
							CSVReader_ReadParam<string>(szStr, data, i==0);
							m_property[i].emType = CSVReader_GetType(m_property[i].strType);
						}
						assert(m_property[0].emType == CParamTraits<KeyType>::type_num);
					}
					break;

				default:	// 接下来的就具体数据
					{
						ItemType item;

						char *data = (char*)&item;

						for (size_t i = 0; i < m_property.size(); ++i)
						{
							bool first = i == 0;
							switch (m_property[i].emType)
							{
							case EMValueType_Char:	{ CSVReader_ReadParam<char>(szStr, data, first); }		break;
							case EMValueType_Int:	{ CSVReader_ReadParam<int>(szStr, data, first); }		break;
							case EMValueType_Float:	{ CSVReader_ReadParam<float>(szStr, data, first); }		break;
							case EMValueType_Double:{ CSVReader_ReadParam<double>(szStr, data, first); }	break;
							case EMValueType_String:{ CSVReader_ReadParam<string>(szStr, data, first); }	break;
							default: break;
							}	
						}

						KeyType &key = *((KeyType*)&item);
						m_mapItems[key] = item;
					}
					break;
				}
			}
			ifs.close();
			return true;
		}
		else
		{
			return false;
		}
	}

private:
	MapType				m_mapItems;
	vector<SProperty>	m_property;
	string				m_strFormat;
	char				m_szLine[4096];
};

#endif