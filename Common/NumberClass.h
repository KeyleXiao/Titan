/*******************************************************************
** 文件名:	NumberClass.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-04-29
** 版  本:	1.0
** 描  述:	

            由于内置类型经常在类中经常忘记初始化导致使用出问题，
			用这个类来封装达到自动初始化功能
********************************************************************/

#ifndef __COMMON_NUMBERCLASS_H__
#define __COMMON_NUMBERCLASS_H__

template<typename T>
class TNumberClass
{
public:
	TNumberClass(const T& data = 0) : data_(data)
	{
		
	}

	operator T() const 
	{
		return data_;
	}

	TNumberClass operator ++(int)
	{
		TNumberClass<T> temp(this);
		++data_;
		return temp;
	}
	TNumberClass& operator ++()
	{
		++data_;
		return *this;
	}
	
	TNumberClass& operator += (const T x)
	{
		data_ += x;
		return *this;
	}
	TNumberClass& operator -= (const T x)
	{
		data_ -= x;
		return *this;
	}
	TNumberClass& operator *= (const T x)
	{
		data_ *= x;
		return *this;
	}
	TNumberClass& operator /= (const T x)
	{
		Assert(x!=0);
		data_ /= x;
		return *this;
	}

	TNumberClass& operator = (const T x )
	{
		data_ = x;
		return *this;
	}
	T operator / (const T x)
	{
		Assert(x);
		return (data_ / x);
	}

private:
	T   data_;
};
// 内置类型
typedef unsigned int64			TUINT64; 
typedef TNumberClass<Char>		TChar;
typedef TNumberClass<int>		TInt;
typedef TNumberClass<int64>		TInt64;
typedef TNumberClass<unsigned>	TUInt32;
typedef TNumberClass<TUint64>	TUInt64;
typedef TNumberClass<float>		Float;
typedef TNumberClass<double>	Double;


class Boolean
{
public:
	Boolean(bool x) : data_(x)
	{
	}

	operator bool() { return data_; }

	Boolean & operator = (bool x)
	{
		data_ = x;
		return *this;
	}

	Boolean & operator &= (bool x)
	{
		data_ &= x;
		return *this;
	}

	Boolean & operator |= (bool x)
	{
		data_ |= x;
		return *this;
	}

private:
	bool data_;
};
typedef Boolean					Bool;


template<typename T>
class PointerClass
{
	PointerClass(T* p):data_(p) { }
	operator T*() { return data; }
	T* operator ->() { return data; }
private:
	T*  data_;
};
typedef	PointerClass			Pointer;



inline std::ostream & opeartor << (std::ostream& os, Bool b)
{
	if(b)
		os << "True";
	else
		os << "False";
	return os;
}


#endif