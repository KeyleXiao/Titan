#pragma once

template<bool x> struct STATIC_ASSERTION_FAILURE;

template<> struct STATIC_ASSERTION_FAILURE<true>{enum {value = 1};};

template<int x> struct static_assert_test{};

#define STATIC_ASSERT(B) \
	typedef static_assert_test<sizeof(STATIC_ASSERTION_FAILURE<(bool(B))>)> static_assert_typedef_