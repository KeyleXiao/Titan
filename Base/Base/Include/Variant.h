//==========================================================================
/**
* @file	  : Variant.h
* @author : PeakGao <peakgao163@163.com>
* created : 2010-1-6   11:36
* purpose : 用于统一抽象的变量、值等概念，不需要太强大，游戏够用即可
*/
//==========================================================================

#ifndef __Variant_h__
#define __Variant_h__

#include "Common.h"

namespace rkt {

#pragma pack(push)
#pragma pack(1)

	// 变量类型(注：如果增加了新的类型，一定要修改cpp里面的静态数组s_typesize)
	enum VariantType
	{
		vt_null = 0,

		vt_bool,

		vt_i08,
		vt_i16,
		vt_i32,
		vt_i64,

		vt_f32,
		vt_f64,

		vt_mstring,
		vt_wstring,

		vt_maxcount, // 类型最大数目

		vt_value = 100, /// 单值
		vt_array		/// 数组，字符串暂时不支持数组
	};

	/// 布尔数组
	struct BoolArray
	{
		bool* data;
		size_t count;
	};

	/// 有符号整型数组
	struct Int08Array
	{
		int08* data;
		size_t count;
	};
	struct Int16Array
	{
		int16* data;
		size_t count;
	};
	struct Int32Array
	{
		int32* data;
		size_t count;
	};
	struct Int64Array
	{
		int64* data;
		size_t count;
	};


	/// 浮点数组
	struct F32Array
	{
		f32* data;
		size_t count;
	};
	struct F64Array
	{
		f64* data;
		size_t count;
	};



	/// 变量对象
	class RKT_EXPORT Variant
	{
		VariantType	isarr:16;
		VariantType	type:16;

		struct ValueArray
		{
			void*	arr;
			size_t	n;
		};

		union
		{
			bool	b;

			int08	i08;
			int16	i16;
			int32	i32;
			int64	i64;

			f32		f;
			f64		d;

			const mstring* ms; // 本来可以通过int08数组实现，这里保留，便于直接使用字符串类
			const wstring* ws; // 本来可以通过int16数组实现，这里保留，便于直接使用字符串类

			ValueArray va;
		};

		static ValueArray sEmptyValueArray;

	public:
		Variant();

		Variant(bool val);

		Variant(int08 val);
		Variant(int16 val);
		Variant(int32 val);
		Variant(int64 val);

		Variant(f32 val);
		Variant(f64 val);

		Variant(const char* val);
		Variant(const mstring& val);

		Variant(const wchar* val);
		Variant(const wstring& val);

		Variant(bool* vals, size_t count);

		Variant(int08* vals, size_t count);
		Variant(int16* vals, size_t count);
		Variant(int32* vals, size_t count);
		Variant(int64* vals, size_t count);

		Variant(f32* vals, size_t count);
		Variant(f64* vals, size_t count);

		Variant(const Variant& rhs);

		~Variant();

		void clear();

		Variant& operator = (const Variant& rhs);

		bool isArray() const		{ return (isarr == vt_array); }
		VariantType getType() const	{ return type; }

		inline operator bool()		{ return getBool(); }

		inline operator int08()		{ return getInt08(); }
		inline operator int16()		{ return getInt16(); }
		inline operator int32()		{ return getInt32(); }
		inline operator int64()		{ return getInt64(); }

		inline operator uint08()	{ return (uint08)getInt08(); }
		inline operator uint16()	{ return (uint16)getInt16(); }
		inline operator uint32()	{ return (uint32)getInt32(); }
		inline operator uint64()	{ return (uint64)getInt64(); }

		inline operator f32()		{ return getF32(); }
		inline operator f64()		{ return getF64(); }

		bool operator == (const Variant& rhs);
		bool operator != (const Variant& rhs)	{ return !(operator==(rhs)); }
		bool operator < (const Variant& rhs);
		bool operator >= (const Variant& rhs)	{ return !(operator<(rhs)); }
		bool operator > (const Variant& rhs)	{ return !(operator<(rhs)) || operator==(rhs); }
		bool operator <= (const Variant& rhs)	{ return !(operator>(rhs)); }

		inline bool getBool() const;
		inline void setBool(bool val);

		inline int08 getInt08() const;
		inline void setInt08(int08 val);

		inline int16 getInt16() const;
		inline void setInt16(int16 val);

		inline int32 getInt32() const;
		inline void setInt32(int32 val);

		inline int64 getInt64() const;
		inline void setInt64(int64 val);

		inline f32 getF32() const;
		inline void setF32(f32 val);

		inline f64 getF64() const;
		inline void setF64(f64 val);

		// mbcs
		inline const mstring& getMString() const;
		inline void setMString(const char* val);
		inline void setMString(const mstring& val);

		// unicode
		inline const wstring& getWString() const;
		inline void setWString(const wchar* val);
		inline void setWString(const wstring& val);

		inline const std::string& getString() const;
		inline void setString(const char* val);
		inline void setString(const std::string& val);

		inline const BoolArray& getBoolArray() const;
		inline void setBoolArray(bool* vals, size_t count);

		inline const Int08Array& getInt08Array() const;
		inline void setInt08Array(int08* vals, size_t count);

		inline const Int16Array& getInt16Array() const;
		inline void setInt16Array(int16* vals, size_t count);

		inline const Int32Array& getInt32Array() const;
		inline void setInt32Array(int32* vals, size_t count);

		inline const Int64Array& getInt64Array() const;
		inline void setInt64Array(int64* vals, size_t count);

		inline const F32Array& getF32Array() const;
		inline void setF32Array(f32* vals, size_t count);

		inline const F64Array& getF64Array() const;
		inline void setF64Array(f64* vals, size_t count);
	};

#pragma pack(pop)

} // rkt

#endif // __Variant_h__