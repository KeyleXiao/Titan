#pragma once

#include "windows.h"
#include "intrin.h"
#include "if.h"

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)


typedef unsigned __int64 uint64_t; 
typedef unsigned int uint32_t; 

#ifndef _WIN64
// 64位cas
inline static bool cas64(void* pdst, uint64_t comp, uint64_t xchgn)
{
    __asm
    {
        lea esi, comp
            mov eax, [esi]
        mov edx, 4[esi]   
        lea esi, xchgn
            mov ebx, [esi]
        mov ecx, 4[esi]   
        mov esi, pdst
            lock cmpxchg8b [esi]
        setz al
    }
}
#endif


template<typename T>
struct select_storage_type
{
	typedef typename speed::aux::if_c<sizeof(T) == 8, __int64, int>::type type;
};


// 封装了cas等原子操作
template<typename T>
class atomic
{
public:
	typedef typename select_storage_type<T>::type storage_type;

	typedef T value_type;

	explicit atomic(value_type v) : v_(0)
	{
		if(sizeof(v) > sizeof(storage_type))
		{
			throw std::runtime_error("construct atomic failed");
			return;
		}
		
		memcpy(&v_, &v, sizeof(value_type));
		//v_ = *((storage_type*)&v);
	}
	atomic(void){}

	void store(value_type v)
	{
		 _ReadWriteBarrier(); 
		// v_ = static_cast<storage_type>(v);

		memcpy(&v_, &v, sizeof(value_type));
		//v_ = *((storage_type*)&v);
	}

	value_type load() const
	{
		value_type v;
		memcpy(&v, &v_, sizeof(value_type));	

		//value_type v = *((value_type*)&v_);
		//value_type v = static_cast<value_type>(v_);
		_ReadWriteBarrier();
		return v;
	}

	bool compare_exchange(value_type& expected, value_type desired)
	{
		value_type previous = expected;
		
		if(sizeof(value_type) == sizeof(long))
		{	
			_ReadWriteBarrier();
			long ret = InterlockedCompareExchange((long*)&v_, *((long*)&desired), *((long*)&expected));
			value_type old_value = *((value_type*)&ret);
			_ReadWriteBarrier();
			return old_value == previous;  
		}
		else if(sizeof(value_type) == sizeof(__int64))
		{
#ifndef _WIN64
			_ReadWriteBarrier();
			bool ret = cas64(&v_, *((__int64*)&expected), *((__int64*)&desired));
			_ReadWriteBarrier();

			return ret;

#else
			
			// win32貌似不支持64位 cas。 这个操作总是失败
			LONGLONG ret = _InterlockedCompareExchange64((__int64*)&v_, *((__int64*)&desired), *((__int64*)&expected));
			value_type old_value = *((value_type*)&ret);
			return old_value == previous; 
#endif
		}
		else
		{
			return false;
		}
		
	}

private:
	storage_type v_;
};