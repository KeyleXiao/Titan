#pragma once

namespace speed{ namespace aux{

template<bool C>
struct if_impl
{
	template<typename T1, typename T2> struct result_
	{
		typedef T1 type;
	};
};

template<>
struct if_impl<false>
{
	template<typename T1, typename T2> struct result_
	{
		typedef T2 type;
	};
};

template<bool C,
		typename T1,
		typename T2>
struct if_c
{
	typedef typename if_impl<C>::template result_<T1, T2>::type type;
};

}
}