/**
* Created by PeakGao 2008.01.08
*/

#ifndef RKT_OBJECT_H
#define RKT_OBJECT_H

#include "Common.h"

#ifdef SUPPORT_OBJECT

#include "Trace.h"
#include <string>
#include <vector>
#include <hash_map>

/**
	对象序列化例子
	
	@code

	// Foo.h
	#pragma once

	#include "Object.h"

	namespace rkt {

		class Foo : public Object
		{
		public:
			DECL_SERIAL(Foo)

			int k;

			virtual void serialize(Archive& ar);
		};

	}

	// Foo.cpp
	#include "stdafx.h"
	#include "Foo.h"

	namespace rkt {

		IMPL_SERIAL(Foo, Object)

		void Foo::serialize(Archive& ar)
		{
			Object::serialize(ar);

			if (ar.isStoring())
				ar<<k;
			else
				ar>>k;
		}
	}

	@endcode
*/



namespace rkt {

	class Object;
	class Archive;

	/// 运行时类
	struct RKT_EXPORT RuntimeClass
	{
		std::string		className;		/// 类名
		int				objSize;		/// 类大小
		Object* (RKT_STDCALL* procCreateObject)(); /// 创建函数，抽象类时为NULL

		/// 动态创建对象
		Object* createObject();

		/// 从类名获取运行时类
		static RuntimeClass* RKT_STDCALL fromName(const std::string& className);

		/// 通过类名动态创建对象
		static Object* RKT_STDCALL createObject(const std::string& className);

		/// 存储运行时类
		void store(Archive& ar) const;

		/// 加载运行时类
		static RuntimeClass* RKT_STDCALL load(Archive& ar);
	};

	/// 运行时类的自动注册机制
	RKT_API void regClass(RuntimeClass* newClass);
	struct SRegClass
	{ SRegClass(RuntimeClass* newClass) { regClass(newClass); } };


	/// 基对象
	class RKT_EXPORT Object
	{
	public:
		static const RuntimeClass classObject;
	public:
		virtual RuntimeClass* getRuntimeClass() const
		{ return (RuntimeClass*)(&Object::classObject); }
		virtual ~Object() {}
		virtual void serialize(Archive& ar) {}

#pragma push_macro("new")
#undef new

		void* RKT_STDCALL operator new(size_t nSize)
		{ return ::operator new(nSize); }
		void* RKT_STDCALL operator new(size_t, void* p)
		{ return p; }
		void RKT_STDCALL operator delete(void* p)
		{ ::operator delete(p); };
		void RKT_STDCALL operator delete(void* p, void* pPlace)
		{ ::operator delete(p);	}
#pragma pop_macro("new")

	protected:
		Object() {}
	private:
		Object(const Object& objectSrc) {}
		void operator=(const Object& objectSrc) {}
	};


	// 一些辅助宏
	#define GET_RUNTIME_CLASS(class_name) ((RuntimeClass*)(&class_name::class##class_name))

	#define DECL_DYNAMIC(class_name) \
	public: \
		static const RuntimeClass class##class_name; \
		virtual RuntimeClass* getRuntimeClass() const; \

	#define DECL_DYNCREATE(class_name) \
		DECL_DYNAMIC(class_name) \
		static Object* RKT_STDCALL createObject();

	#define DECL_SERIAL(class_name) \
		DECL_DYNCREATE(class_name) \
		friend Archive& operator<<(Archive& ar, const class_name*& obj);\
		friend Archive& operator>>(Archive& ar, class_name*& obj);

	#define IMPL_RUNTIMECLASS(class_name, base_class_name, pfnNew, class_init) \
		const RuntimeClass class_name::class##class_name = \
		{ #class_name, sizeof(class class_name), pfnNew/*, NULL */}; \
		RuntimeClass* class_name::getRuntimeClass() const \
		{ return GET_RUNTIME_CLASS(class_name); } \

	#define IMPL_DYNAMIC(class_name, base_class_name) \
		IMPL_RUNTIMECLASS(class_name, base_class_name, NULL, NULL)

	#define IMPL_DYNCREATE(class_name, base_class_name) \
		Object* RKT_STDCALL class_name::createObject() { return new class_name; } \
		IMPL_RUNTIMECLASS(class_name, base_class_name, class_name::createObject, NULL)

	#define IMPL_SERIAL(class_name, base_class_name) \
		IMPL_DYNCREATE(class_name, base_class_name)\
		SRegClass _reg_##class_name(GET_RUNTIME_CLASS(class_name)); \
		Archive& operator<<(Archive& ar, const class_name*& obj)\
		{ ar.writeObject(obj); return ar; }\
		Archive& operator>>(Archive& ar, class_name*& obj) \
		{ obj = (class_name*)ar.readObject(); return ar; } \


	/// 文档
	/// 这里我们只需要内存文档，不需要文件文档，内存文档可以保存为文件文档
	class RKT_EXPORT Archive
	{
	public:
		enum EMode {modeStore=0, modeLoad=1};	/// 文档模式
	private:
		enum LoadArrayObjType{ typeUndefined = 0, typeRuntimeClass = 1, typeObject = 2 };
		enum { NewClassTag = (int)0xffffffff };

		struct SLoadInfo
		{
			void* ptr;
			LoadArrayObjType type;

			SLoadInfo(void* p, LoadArrayObjType t) : ptr(p), type(t) {}
		};
		typedef HashMap<void*, int>					StoreMap;
		typedef std::vector<SLoadInfo>				LoadArray;

	protected:
		int			mMode;		/// 文档模式（存储还是加载？）
		uchar*		mBegin;		/// 缓冲区开始位置
		uchar*		mCur;		/// 缓冲区当前位置
		uchar*		mEnd;		/// 缓冲区结束位置

		union
		{
			LoadArray*	m_pLoadArray;	/// 用于保存加载后的对象<对象或运行时类>
			StoreMap*	m_pStoreMap;	/// 存储的对象映射<对象或运行时类, 索引>
		};

		
	public:
		Archive();
		Archive(uchar* buf, size_t bytes);
		~Archive();

		bool isLoading() const		{ return  (mMode & Archive::modeLoad) != 0; }
		bool isStoring() const		{ return  (mMode & Archive::modeLoad) == 0; }

		/// 文档的缓冲
		uchar* data() const			{ return mBegin; }

		/// 文档的有效大小
		size_t size() const			{ return mCur - mBegin; }

		/// 文档的最大容量
		size_t capacity() const		{ return mEnd - mBegin; }


		/// 清除文档内容
		void clear();

		/// 预置文档大小
		void reserve(size_t bytes);

		/// 读缓冲
		size_t read(void* buf, size_t bytes);

		/// 写缓冲
		void write(const void* buf, size_t bytes);

		/// 定位当前游标
		void seek(int offset);

		
		/// 读取对象
		Object* readObject();

		/// 写入对象
		void writeObject(const Object* obj);


		/// 读取运行时类
		RuntimeClass* readClass(int* objIndex = NULL);

		/// 写入运行时类
		void writeClass(const RuntimeClass* runtimeClass);
		//void serializeClass(const RuntimeClass* runtimeClass);

		friend Archive& RKT_STDCALL operator<<(Archive& ar, const Object* pOb);
		friend Archive& RKT_STDCALL operator>>(Archive& ar, Object*& pOb);
		friend Archive& RKT_STDCALL operator>>(Archive& ar, const Object*& pOb);

		// 简单类型的输入输出操作
		Archive& operator<<(bool v)
		{ return Archive::operator<<((uchar)(v ? 1 : 0)); }
		Archive& operator<<(char v)
		{ return Archive::operator<<((uchar)v); }
#ifdef _NATIVE_WCHAR_T_DEFINED
		Archive& operator<<(wchar_t v)
		{ return Archive::operator<<((ushort)v); }
#endif
		Archive& operator<<(short v)
		{ return Archive::operator<<((ushort)v); }
		Archive& operator<<(long v)
		{ return Archive::operator<<((ulong)v); }
		Archive& operator<<(int v)
		{ return Archive::operator<<((uint)v); }
		Archive& operator<<(int64 v)
		{ return Archive::operator<<((uint64)v); }
		Archive& operator<<(float v)
		{ Assert(isStoring()); grow(sizeof(float)); *(float*)mCur = v; mCur += sizeof(float); return *this; }
		Archive& operator<<(double v)
		{ Assert(isStoring()); grow(sizeof(double)); *(double*)mCur = v; mCur += sizeof(double); return *this; }
		Archive& operator<<(uchar v)
		{ Assert(isStoring()); grow(sizeof(uchar)); *(uchar*)mCur = v; mCur += sizeof(uchar); return *this; }
		Archive& operator<<(ushort v)
		{ Assert(isStoring()); grow(sizeof(ushort)); *(ushort*)mCur = v; mCur += sizeof(ushort); return *this; }
		Archive& operator<<(ulong v)
		{ Assert(isStoring()); grow(sizeof(ulong)); *(ulong*)mCur = v; mCur += sizeof(ulong); return *this; }
		Archive& operator<<(uint _w64 v)
		{ Assert(isStoring()); grow(sizeof(uint)); *(uint _w64*)mCur = v; mCur += sizeof(uint); return *this; }
		Archive& operator<<(uint64 v)
		{ Assert(isStoring()); grow(sizeof(int64)); *(int64*)mCur = v; mCur += sizeof(int64); return *this; }
		Archive& operator<<(const std::string& v)
		{ Assert(isStoring()); Archive::operator<<(v.length()); write(v.c_str(), v.length()); return *this; }

		Archive& operator>>(bool& v)
		{ uchar by; Archive& ar = Archive::operator>>(by); v = (by ? true : false); return ar; }
		Archive& operator>>(char& v)
		{ return Archive::operator>>((uchar&)v); }
#ifdef _NATIVE_WCHAR_T_DEFINED
		Archive& operator>>(wchar_t& v)
		{ return Archive::operator>>((ushort&)v); }
#endif
		Archive& operator>>(short& v)
		{ return Archive::operator>>((ushort&)v); }
		Archive& operator>>(long& v)
		{ return Archive::operator>>((ulong&)v); }
		Archive& operator>>(int& v)
		{ return Archive::operator>>((uint&)v); }
		Archive& operator>>(int64& v)
		{ return Archive::operator>>((uint64&)v); }
		Archive& operator>>(float& v)
		{ Assert(isLoading()); if (mCur+sizeof(float) <= mEnd) {v = *(float*)mCur; mCur += sizeof(float);} return *this; }
		Archive& operator>>(double& v)
		{ Assert(isLoading()); if (mCur+sizeof(double) <= mEnd) {v = *(double*)mCur; mCur += sizeof(double);} return *this; }
		Archive& operator>>(uchar& v)
		{ Assert(isLoading()); if (mCur+sizeof(uchar) <= mEnd) {v = *(uchar*)mCur; mCur += sizeof(uchar);} return *this; }
		Archive& operator>>(ushort& v)
		{ Assert(isLoading()); if (mCur+sizeof(ushort) <= mEnd) {v = *(ushort*)mCur; mCur += sizeof(ushort);} return *this; }
		Archive& operator>>(ulong& v)
		{ Assert(isLoading()); if (mCur+sizeof(ulong) <= mEnd) {v = *(ulong*)mCur; mCur += sizeof(ulong);} return *this; }
		Archive& operator>>(uint& v)
		{ Assert(isLoading()); if (mCur+sizeof(uint) <= mEnd) {v = *(uint*)mCur; mCur += sizeof(uint);} return *this; }
		Archive& operator>>(uint64& v)
		{ Assert(isLoading()); if (mCur+sizeof(int64) <= mEnd) {v = *(int64*)mCur; mCur += sizeof(int64);} return *this; }
		Archive& operator>>(std::string& v)
		{
			Assert(isLoading()); 
			size_t len;
			*this >> len;
			v.reserve(len);
			if (mCur + len > mEnd)
				len = mEnd - mCur;
			v.assign((const char*)mCur, len);
			mCur += len;
			return *this;
		}

	private:
		void grow(size_t n);
		void initContainer();
	};
}

#endif // #ifdef SUPPORT_OBJECT

#endif // RKT_OBJECT_H