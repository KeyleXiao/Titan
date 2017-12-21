
#ifndef _JIMIC_PORT_H_
#define _JIMIC_PORT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "msvc/stdint.h"
#include "msvc/stdbool.h"
#include <emmintrin.h>

#ifndef JIMI_CACHE_LINE_SIZE
#define JIMI_CACHE_LINE_SIZE    64
#endif

#ifndef JIMI_MIN
#define JIMI_MIN(a, b)          ((a) < (b) ? (a) : (b))
#endif

#ifndef JIMI_MAX
#define JIMI_MAX(a, b)          ((a) > (b) ? (a) : (b))
#endif

#if defined(_M_X64) || defined(_WIN64) || defined(_M_AMD64)
#define JIMI_SIZE_T_SIZEOF      8
#else
#define JIMI_SIZE_T_SIZEOF      4
#endif

/**
 * macro for round to power of 2
 */
#define jimi_b2(x)              (        (x) | (        (x) >>  1))
#define jimi_b4(x)              ( jimi_b2(x) | ( jimi_b2(x) >>  2))
#define jimi_b8(x)              ( jimi_b4(x) | ( jimi_b4(x) >>  4))
#define jimi_b16(x)             ( jimi_b8(x) | ( jimi_b8(x) >>  8))
#define jimi_b32(x)             (jimi_b16(x) | (jimi_b16(x) >> 16))
#define jimi_b64(x)             (jimi_b32(x) | (jimi_b32(x) >> 32))

#define jimi_next_power_of_2(x)     (jimi_b32((x) - 1) + 1)
#define jimi_next_power_of_2_64(x)  (jimi_b64((uint64_t)(x) - 1) + 1)

#if defined(JIMI_SIZE_T_SIZEOF) && (JIMI_SIZE_T_SIZEOF == 8)
#define JIMI_ROUND_TO_POW2(N)   jimi_next_power_of_2_64(N)
#else
#define JIMI_ROUND_TO_POW2(N)   jimi_next_power_of_2(N)
#endif

#ifndef jimi_mm_pause
#define jimi_mm_pause       _mm_pause
#endif

#if defined(_MSC_VER) || defined(__INTER_COMPILER) || defined(__ICC)

#ifndef jimi_likely
#define jimi_likely(x)      (x)
#endif

#ifndef jimi_unlikely
#define jimi_unlikely(x)    (x)
#endif

#ifndef JIMIC_INLINE
#define JIMIC_INLINE            __inline
#endif

#define ALIGN_PREFIX(N)         __declspec(align(N))
#define ALIGN_SUFFIX(N)

#define CACHE_ALIGN_PREFIX      __declspec(align(JIMI_CACHE_LINE_SIZE))
#define CACHE_ALIGN_SUFFIX

#if defined(__INTER_COMPILER) || defined(__ICC)

#define Jimi_ReadWriteBarrier() __memory_barrier()

#define Jimi_MemoryBarrier()    MemoryBarrier()

#else

///
/// _ReadWriteBarrier
///
/// See: http://msdn.microsoft.com/en-us/library/f20w0x5e%28VS.80%29.aspx
///
/// See: http://en.wikipedia.org/wiki/Memory_ordering
///
#define Jimi_ReadWriteBarrier() _ReadWriteBarrier()

#define Jimi_MemoryBarrier()    MemoryBarrier()

#endif  /* __INTER_COMPILER || __ICC */

#else  /* !_MSC_VER */

#ifndef jimi_likely
#define jimi_likely(x)          __builtin_expect((x), 1)
#endif

#ifndef jimi_unlikely
#define jimi_unlikely(x)        __builtin_expect((x), 0)
#endif

#ifndef JIMIC_INLINE
#define JIMIC_INLINE            inline
#endif

#define ALIGN_PREFIX(N)         __attribute__((__aligned__((N))))
#define ALIGN_SUFFIX(N)

#define CACHE_ALIGN_PREFIX      __attribute__((__aligned__((JIMI_CACHE_LINE_SIZE))))
#define CACHE_ALIGN_SUFFIX

#define PACKED_ALIGN_PREFIX(N)
#define PACKED_ALIGN_SUFFIX(N)  __attribute__((packed, aligned(N)))

///
/// See: http://en.wikipedia.org/wiki/Memory_ordering
///
/// See: http://bbs.csdn.net/topics/310025520
///

//#define Jimi_ReadWriteBarrier()     asm volatile ("":::"memory");
#define Jimi_ReadWriteBarrier()     __asm__ __volatile__ ("":::"memory");

#define Jimi_MemoryBarrier()        __sync_synchronize()

#endif  /* _MSC_VER */

#if defined(_MSC_VER) || defined(__INTER_COMPILER) || defined(__ICC)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "msvc/targetver.h"
#include <windows.h>
#include <intrin.h>

#define jimi_val_compare_and_swap32(destPtr, oldValue, newValue)        \
    InterlockedCompareExchange((volatile LONG *)(destPtr),              \
                            (LONG)(newValue), (LONG)(oldValue))

#define jimi_val_compare_and_swap64(destPtr, oldValue, newValue)        \
    InterlockedCompareExchange64((volatile LONG64 *)(destPtr),          \
                            (LONG64)(newValue), (LONG64)(oldValue))

#define jimi_bool_compare_and_swap32(destPtr, oldValue, newValue)       \
    (InterlockedCompareExchange((volatile LONG *)(destPtr),             \
                            (LONG)(newValue), (LONG)(oldValue))         \
                                == (LONG)(oldValue))

#define jimi_bool_compare_and_swap64(destPtr, oldValue, newValue)       \
    (InterlockedCompareExchange64((volatile LONG64 *)(destPtr),         \
                            (LONG64)(newValue), (LONG64)(oldValue))     \
                                == (LONG64)(oldValue))

#define jimi_lock_test_and_set32(destPtr, newValue)                     \
    InterlockedExchange((volatile LONG *)(destPtr), (LONG)(newValue))

#define jimi_fetch_and_add32(destPtr, addValue)                         \
    InterlockedExchangeAdd((volatile LONG *)(destPtr), (LONG)(addValue))

#define jimi_fetch_and_add64(destPtr, addValue)                         \
    InterlockedExchangeAdd64((volatile LONGLONG *)(destPtr), (LONGLONG)(addValue))

#elif defined(__GUNC__) || defined(__linux__) \
   || defined(__clang__) || defined(__APPLE__) || defined(__FreeBSD__) \
   || defined(__CYGWIN__) || defined(__MINGW32__)

#define jimi_val_compare_and_swap32(destPtr, oldValue, newValue)        \
    __sync_val_compare_and_swap((volatile uint32_t *)(destPtr),         \
                            (uint32_t)(oldValue), (uint32_t)(newValue))

#define jimi_val_compare_and_swap64(destPtr, oldValue, newValue)        \
    __sync_val_compare_and_swap((volatile uint64_t *)(destPtr),         \
                            (uint64_t)(oldValue), (uint64_t)(newValue))

#define jimi_val_compare_and_swap(destPtr, oldValue, newValue)          \
    __sync_val_compare_and_swap((destPtr), (oldValue), (newValue))

#define jimi_bool_compare_and_swap32(destPtr, oldValue, newValue)       \
    __sync_bool_compare_and_swap((volatile uint32_t *)(destPtr),        \
                            (uint32_t)(oldValue), (uint32_t)(newValue))

#define jimi_bool_compare_and_swap64(destPtr, oldValue, newValue)       \
    __sync_bool_compare_and_swap((volatile uint64_t *)(destPtr),        \
                            (uint64_t)(oldValue), (uint64_t)(newValue))

#define jimi_bool_compare_and_swap(destPtr, oldValue, newValue)         \
    __sync_bool_compare_and_swap((destPtr), (oldValue), (newValue))

#define jimi_lock_test_and_set32(destPtr, newValue)                     \
    __sync_lock_test_and_set((volatile uint32_t *)(destPtr),            \
                             (uint32_t)(newValue))

#define jimi_fetch_and_add32(destPtr, addValue)                         \
    __sync_fetch_and_add((volatile uint32_t *)(destPtr),                \
                         (uint32_t)(addValue))

#define jimi_fetch_and_add64(destPtr, addValue)                         \
    __sync_fetch_and_add((volatile uint64_t *)(destPtr),                \
                         (uint64_t)(addValue))

#else

#define jimi_val_compare_and_swap32(destPtr, oldValue, newValue)        \
    __internal_val_compare_and_swap32((volatile uint32_t *)(destPtr),   \
                                (uint32_t)(oldValue), (uint32_t)(newValue))

#define jimi_val_compare_and_swap64(destPtr, oldValue, newValue)        \
    __internal_val_compare_and_swap64((volatile uint64_t *)(destPtr),   \
                                (uint64_t)(oldValue), (uint64_t)(newValue))

#define jimi_bool_compare_and_swap32(destPtr, oldValue, newValue)       \
    __internal_bool_compare_and_swap32((volatile uint32_t *)(destPtr),  \
                                (uint32_t)(oldValue), (uint32_t)(newValue))

#define jimi_bool_compare_and_swap64(destPtr, oldValue, newValue)       \
    __internal_bool_compare_and_swap64((volatile uint64_t *)(destPtr),  \
                                (uint64_t)(oldValue), (uint64_t)(newValue))

#define jimi_lock_test_and_set32(destPtr, newValue)                     \
    __internal_lock_test_and_set32((volatile uint32_t *)(destPtr),      \
                                (uint32_t)(newValue))

#define jimi_fetch_and_add32(destPtr, addValue)                         \
    __internal_fetch_and_add32((volatile uint32_t *)(destPtr),          \
                                (uint32_t)(addValue))

#define jimi_fetch_and_add64(destPtr, addValue)                         \
    __internal_fetch_and_add64((volatile uint64_t *)(destPtr),          \
                                (uint64_t)(addValue))

#endif  /* defined(_MSC_VER) || defined(__INTER_COMPILER) */

#if defined(_MSC_VER) || defined(__INTEL_COMPILER)  || defined(__ICC) \
 || defined(__MINGW32__) || defined(__CYGWIN__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "msvc/targetver.h"
#include <windows.h>
#elif defined(__linux__) || defined(__GUNC__) \
   || defined(__clang__) || defined(__APPLE__) || defined(__FreeBSD__)
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

static JIMIC_INLINE
int get_num_of_processors(void)
{
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)  || defined(__ICC) \
 || defined(__MINGW32__) || defined(__CYGWIN__)

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;

#elif defined(__linux__) || defined(__GUNC__) \
   || defined(__clang__) || defined(__APPLE__) || defined(__FreeBSD__)

    int nprocs = -1;
  #ifdef _SC_NPROCESSORS_ONLN
    nprocs = sysconf(_SC_NPROCESSORS_ONLN);
  #endif
    return nprocs;

#else
    return 1;
#endif
}

static JIMIC_INLINE
uint32_t __internal_val_compare_and_swap32(volatile uint32_t *destPtr,
                                           uint32_t oldValue,
                                           uint32_t newValue)
{
    uint32_t origValue = *destPtr;
    if (*destPtr == oldValue) {
        *destPtr = newValue;
    }
    return origValue;
}

static JIMIC_INLINE
uint64_t __internal_val_compare_and_swap64(volatile uint64_t *destPtr,
                                           uint64_t oldValue,
                                           uint64_t newValue)
{
    uint64_t origValue = *destPtr;
    if (*destPtr == oldValue) {
        *destPtr = newValue;
    }
    return origValue;
}

static JIMIC_INLINE
bool __internal_bool_compare_and_swap32(volatile uint32_t *destPtr,
                                        uint32_t oldValue,
                                        uint32_t newValue)
{
    if (*destPtr == oldValue) {
        *destPtr = newValue;
        return 1;
    }
    else
        return 0;
}

static JIMIC_INLINE
bool __internal_bool_compare_and_swap64(volatile uint64_t *destPtr,
                                        uint64_t oldValue,
                                        uint64_t newValue)
{
    if (*destPtr == oldValue) {
        *destPtr = newValue;
        return 1;
    }
    else
        return 0;
}

static JIMIC_INLINE
uint32_t __internal_lock_test_and_set32(volatile uint32_t *destPtr,
                                        uint32_t newValue)
{
    uint32_t origValue = *destPtr;
    *destPtr = newValue;
    return origValue;
}

static JIMIC_INLINE
uint32_t __internal_fetch_and_add32(volatile uint32_t *destPtr,
                                    uint32_t addValue)
{
    uint32_t origValue = *destPtr;
    *destPtr += addValue;
    return origValue;
}

static JIMIC_INLINE
uint64_t __internal_fetch_and_add64(volatile uint64_t *destPtr,
                                    uint64_t addValue)
{
    uint64_t origValue = *destPtr;
    *destPtr += addValue;
    return origValue;
}

#ifdef __cplusplus
}
#endif

#endif  /* _JIMIC_PORT_H_ */
