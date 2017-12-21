//==========================================================================
/**
* @file	  : XlsValue.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-3-13   15:53
* purpose : 用于记录Excel数据脚本中的一个值
*/
//==========================================================================

#ifndef __XLSVALUE_H__
#define __XLSVALUE_H__

#include "Common.h"

#ifdef SUPPORT_SCHEMEENGINE

//#include "autostring.h"
#include "Trace.h"
#include "strconv.h"

namespace rkt {

	/// 整型数组
	struct IntArray
	{
		int count;
		int* data;
	};
	
	/// 浮点数组
	struct FloatArray
	{
		int count;
		float* data;
	};

	/// Excel表格中的一个值对象
	struct XlsValue
	{
		enum {_unknow=-1, _int, _float, _string, _int_array, _float_array};
		int	type;
		union{
			int		i;
			float	f;
			const std::string* as;
			IntArray* ia;
			FloatArray* fa;
		};
		static RKT_EXPORT std::string sEmptyAutoString;
		static RKT_EXPORT IntArray sEmptyIntArray;
		static RKT_EXPORT FloatArray sEmptyFloatArray;

		XlsValue() : type(_unknow), i(0)			{}
		XlsValue(int val) : type(_int), i(val)		{}
		XlsValue(float val) : type(_float), f(val)	{}
		XlsValue(const char* s) : type(_string) { as = (s && s[0]) ? new std::string(s) : 0; }
		XlsValue(int* vals, int count) : type(_int_array)
		{
			if (count > 0 && vals)
			{
				ia = new IntArray;
				ia->data = new int[count];
				ia->count = count;
				memcpy(ia->data, vals, count*sizeof(int));
			}
			else
				ia = 0;
		}
		XlsValue(float* vals, int count) : type(_float_array)
		{
			if (count > 0 && vals)
			{
				fa = new FloatArray;
				fa->data = new float[count];
				fa->count = count;
				memcpy(fa->data, vals, count*sizeof(float));
			}
			else
				fa = 0;
		}
		XlsValue(const XlsValue& rhs)
		{
			(*this) = rhs;
		}
		~XlsValue()
		{
			if (type == _string && as != 0)
			{
				safeDelete(as);
			}
			else if (type == _int_array && ia != 0)
			{
				safeDeleteArray(ia->data);
				safeDelete(ia);
			}
			else if (type == _float_array && fa != 0)
			{
				safeDeleteArray(fa->data);
				safeDelete(fa);
			}
		}

		XlsValue& operator = (const XlsValue& rhs)
		{
			type = rhs.type;
			if (type == _int) i = rhs.i;
			else if (type == _float) f = rhs.f;
			else if (type == _string) as = rhs.as ? new std::string(*rhs.as) : 0;
			else if (type == _int_array)
			{
				if (rhs.ia)
				{
					ia = new IntArray;
					ia->data = new int[rhs.ia->count];
					ia->count = rhs.ia->count;
					memcpy(ia->data, rhs.ia->data, ia->count*sizeof(int));
				}
				else ia = rhs.ia;
			}
			else if (type == _float_array)
			{
				if (rhs.fa)
				{
					fa = new FloatArray;
					fa->data = new float[rhs.fa->count];
					fa->count = rhs.fa->count;
					memcpy(fa->data, rhs.fa->data, fa->count*sizeof(float));
				}
				else fa = rhs.fa;
			}
			return (*this);
		}

		inline int getInt() const
		{
			if (type == _int)
				return i;

			Assert(type == _int);
			return 0;
		}
		inline void setInt(int val)
		{
			type = _int;
			i = val;
		}
		inline float getFloat() const
		{
			if (type == _float)
				return f;

			Assert(type == _float);
			return 0;
		}
		inline void setFloat(float val)
		{
			type = _float;
			f = val;
		}
		inline const std::string& getString() const
		{
			if (type == _string)
				return as ? *as : sEmptyAutoString;

			Assert(type == _string);
			return sEmptyAutoString;
		}
		inline void setString(const char* val)
		{
			type = _string;
			as = (val && val[0]) ? new std::string(val) : 0;
		}
		inline const IntArray& getIntArray() const
		{
			if (type == _int_array)
				return ia ? *ia : sEmptyIntArray;

			Assert(type == _int_array);
			return sEmptyIntArray;
		}
		inline void setIntArray(int* vals, int count)
		{
			type = _int_array;
			ia = 0;
			if (count > 0 && vals)
			{
				ia = new IntArray;
				ia->data = new int[count];
				ia->count = count;
				memcpy(ia->data, vals, count*sizeof(int));
			}
		}
		inline const FloatArray& getFloatArray() const
		{
			if (type == _float_array)
				return fa ? *fa : sEmptyFloatArray;

			Assert(type == _float_array);
			return sEmptyFloatArray;
		}
		inline void setFloatArray(float* vals, int count)
		{
			type = _float_array;
			fa = 0;
			if (count > 0 && vals)
			{
				fa = new FloatArray;
				fa->data = new float[count];
				fa->count = count;
				memcpy(fa->data, vals, count*sizeof(float));
			}
		}
	};


	/// Excel记录，基于索引的值数组(类似一个简化的vector)
	class XlsRecord
	{
		XlsValue*		mValues;	/// 值数组
		size_t			mCount;		/// 数组节点数目
		size_t			mCapacity;	/// 数组容量

	public:
		static RKT_EXPORT XlsValue	sEmptyValue;

	public:
		XlsRecord() : mValues(0), mCount(0), mCapacity(0) {}
		~XlsRecord()
		{
			clear(); 
			for (size_t i=0; i<mCapacity; i++)
				mValues[i].~XlsValue();
			safeDeleteArray(mValues); 
		}

		XlsValue& operator [](uint index) const
		{
			return (index < mCapacity) ? mValues[index] : sEmptyValue;
		}

		void reserve(size_t count)
		{
			if (count > mCapacity)
			{
				mCapacity = count;
				XlsValue* newValues = new XlsValue[mCapacity];
				if (mValues && mCount)
					memcpy(newValues, mValues, mCount * sizeof(XlsValue));
				safeDeleteArray(mValues);
				mValues = newValues;
			}
		}
		inline void clear()				{ mCount = 0; }
		inline size_t size() const		{ return mCount; }
		inline size_t capacity() const	{ return mCapacity; }

		int getInt(uint index) const						{ return (*this)[index].getInt(); }
		float getFloat(uint index) const					{ return (*this)[index].getFloat(); }
		const std::string& getString(uint index) const		{ return (*this)[index].getString(); }
		const IntArray& getIntArray(uint index) const		{ return (*this)[index].getIntArray(); }
		const FloatArray& getFloatArray(uint index) const	{ return (*this)[index].getFloatArray(); }
	};


	/// 记录集，适合分大类小类的情况下，用于某些读取Excel数据脚本的应用中
	template<int _MaxSubRecordCount>
	class RecordSet
	{
		XlsRecord*	mRecords[_MaxSubRecordCount];	/// 索引为0的记录为大类记录，其他为子类记录

	public:
		RecordSet()		{ memset(mRecords, 0, sizeof(mRecords)); }
		~RecordSet()	{ clear(); }

		inline void clear()
		{
			for (int i=0; i<_MaxSubRecordCount; i++)
				safeDelete(mRecords[i]);
		}

		void setRecord(XlsRecord* record)			{ mRecords[0] = record; }
		const XlsRecord* getRecord() const			{ return mRecords[0]; }

		void setSubRecord(ulong index, XlsRecord* record)
		{
			if (index >= 0 && index < _MaxSubRecordCount)
			{
				mRecords[index] = record;
				return;
			}

			Assert(index >= 0 && index < _MaxSubRecordCount);
		}
		const XlsRecord* getSubRecord(ulong index) const
		{
			if (index >= 0 && index < _MaxSubRecordCount)
				return mRecords[index];

			Assert(index >= 0 && index < _MaxSubRecordCount);
			return NULL;
		}
	};




	/// 将一个字符串分解为整数列表
	template<class T>
	inline int parseIntArray(const char* str, /*in out*/T* arr, int count, const char sep = ';')
	{
		Assert(arr != NULL && count > 0);
		if (!str) return 0;

		char buf[32];
		int i = 0;
		int j = 0;
		const char* p = str;
		while (*p)
		{
			if (*p == sep)
			{
				p++;
				buf[i] = 0;
				i = 0;
				arr[j++] = (T)strtoul(buf, 0, 10);
				if (j >= count)
					return j;
			}
			else
				buf[i++] = *p++;
		}

		if (i > 0)
		{
			buf[i] = 0;
			arr[j++] = (T)strtoul(buf, 0, 10);
		}

		return j;
	}

	/// 将一个字符串分解为浮点列表
	inline int parseFloatArray(const char* str, /*in out*/float* arr, int count, const char sep = ';')
	{
		Assert(arr != NULL && count > 0);
		if (!str) return 0;

		char buf[32];
		int i = 0;
		int j = 0;
		const char* p = str;
		while (*p)
		{
			if (*p == sep)
			{
				p++;
				buf[i] = 0;
				i = 0;
				arr[j++] = (float)atof(buf);
				if (j >= count)
					return j;
			}
			else
				buf[i++] = *p++;
		}

		if (i > 0)
		{
			buf[i] = 0;
			arr[j++] = (float)atof(buf);
		}

		return j;
	}

} // namespace

#endif // #ifdef SUPPORT_SCHEMEENGINE

#endif // __XLSVALUE_H__