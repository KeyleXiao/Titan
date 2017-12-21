/**
* Created by PeakGao 2008.01.07
*/

#ifndef RKT_STRINGHASH_H
#define RKT_STRINGHASH_H

#include "Common.h"

#ifdef SUPPORT_STRINGHASH

#include <hash_map>
#include <functional>

#ifdef RKT_COMPILER_MSVC
#	pragma warning(push)
#	pragma warning(disable:4996)
#endif

namespace rkt {

	/// 字符串映射成整数
	#define STR2ID(str)	hashString((str), 0)

	/// 哈希一个字符串
	RKT_API ulong hashString(const char * str, ulong type = 0);

	/// 哈希一个二进制
	RKT_API ulong hashBinary(const char * bin, ulong len);

	/// 加密一段内存
	RKT_API bool makeMap(uchar* buffer, ulong size, ulong key);


	struct StrHashKeyType
	{
		ulong name;
		ulong name1;
		ulong name2;
	};

	struct my_less
	{
		bool operator()(const rkt::StrHashKeyType& l, const rkt::StrHashKeyType& r) const
		{
			return (l.name1 == r.name1 && l.name2 == r.name2) ? false : true;
		}
	};

	/**
		采用的wow的字符串hash算法，速度不知道
		自动忽略字符串的大小写
	*/

	template<class T>
	class StrHashMap: public HashMap<StrHashKeyType, T, stdext::hash_compare<StrHashKeyType, my_less> >
	{
	public:
		typedef typename HashMap<StrHashKeyType, T, stdext::hash_compare<StrHashKeyType, my_less> >  _base;
		typedef typename _base::iterator iterator;
		typedef typename _base::const_iterator const_iterator;
		typedef typename _base::size_type size_type;

		mapped_type& operator[](const char* str)
		{
			StrHashKeyType _Keyval;
			calcHashValue(_Keyval, str);
			iterator _Where = this->lower_bound(_Keyval);
			if (_Where == this->end())
				_Where = this->insert(value_type(_Keyval, mapped_type())).first;
			return ((*_Where).second);
		}
		mapped_type& operator[](StrHashKeyType key)
		{
			iterator _Where = this->lower_bound(key);
			if (_Where == this->end())
				_Where = this->insert(value_type(key, mapped_type())).first;
			return ((*_Where).second);
		}
		iterator find(const char* str)
		{
			StrHashKeyType _Keyval;
			calcHashValue(_Keyval, str);
			return lower_bound(_Keyval);
		}
		const_iterator find(const char* str) const
		{
			StrHashKeyType _Keyval;
			calcHashValue(_Keyval, str);
			return lower_bound(_Keyval);
		}

		size_type erase(const char* str)
		{
			StrHashKeyType _Keyval;
			calcHashValue(_Keyval, str);
			return _base::erase(_Keyval);
		}

		iterator erase(iterator it)
		{
			return _base::erase(it);
		}

		StrHashKeyType strToKeyType(const char* str)
		{
			StrHashKeyType _Keyval;
			calcHashValue(_Keyval, str);
			return _Keyval;
		}
	private:
		inline void calcHashValue(StrHashKeyType& key, const char* str) const
		{
			key.name = hashString(str, 0);
			key.name1 = hashString(str, 1);
			key.name2 = hashString(str, 2);
		}
	};
} // namespace

#ifdef RKT_COMPILER_MSVC
#	pragma warning(pop)
#endif

#if defined(RKT_COMPILER_MSVC)
#if RKT_COMPILER_VER > 1300
namespace stdext
#else
namespace std
#endif
{
	inline size_t hash_value(const rkt::StrHashKeyType& key)
	{
		return key.name;
	};
}
#endif // RKT_COMPILER_MSVC

#endif // #ifdef SUPPORT_STRINGHASH

#endif // RKT_STRINGHASH_H