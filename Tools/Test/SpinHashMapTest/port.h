
#ifndef _SPIN__PORT_H_
#define _SPIN__PORT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "msvc/stdint.h"
//#include "msvc/stdbool.h"
#include <emmintrin.h>

#ifndef spin_CACHE_LINE_SIZE
#define spin_CACHE_LINE_SIZE    64
#endif

#ifndef spin_MIN
#define spin_MIN(a, b)          ((a) < (b) ? (a) : (b))
#endif

#ifndef spin_MAX
#define spin_MAX(a, b)          ((a) > (b) ? (a) : (b))
#endif

#if defined(_M_X64) || defined(_WIN64) || defined(_M_AMD64)
#define spin_SIZE_T_SIZEOF      8
#else
#define spin_SIZE_T_SIZEOF      4
#endif

/**
 * macro for round to power of 2
 */
#define spin_b2(x)              (        (x) | (        (x) >>  1))
#define spin_b4(x)              ( spin_b2(x) | ( spin_b2(x) >>  2))
#define spin_b8(x)              ( spin_b4(x) | ( spin_b4(x) >>  4))
#define spin_b16(x)             ( spin_b8(x) | ( spin_b8(x) >>  8))
#define spin_b32(x)             (spin_b16(x) | (spin_b16(x) >> 16))
#define spin_b64(x)             (spin_b32(x) | (spin_b32(x) >> 32))

#define spin_next_power_of_2(x)     (spin_b32((x) - 1) + 1)
#define spin_next_power_of_2_64(x)  (spin_b64((uint64_t)(x) - 1) + 1)

#if defined(spin_SIZE_T_SIZEOF) && (spin_SIZE_T_SIZEOF == 8)
#define spin_ROUND_TO_POW2(N)   spin_next_power_of_2_64(N)
#else
#define spin_ROUND_TO_POW2(N)   spin_next_power_of_2(N)
#endif

#ifndef spin_mm_pause
#define spin_mm_pause       _mm_pause
#endif

#if defined(_MSC_VER) || defined(__INTER_COMPILER) || defined(__ICC)

#ifndef spin_likely
#define spin_likely(x)      (x)
#endif

#ifndef spin_unlikely
#define spin_unlikely(x)    (x)
#endif

#define inline            __inline

#define ALIGN_PREFIX(N)         __declspec(align(N))
#define ALIGN_SUFFIX(N)

#define CACHE_ALIGN_PREFIX      __declspec(align(spin_CACHE_LINE_SIZE))
#define CACHE_ALIGN_SUFFIX

#if defined(__INTER_COMPILER) || defined(__ICC)

#define Spin_ReadWriteBarrier() __memory_barrier()

#define Spin_MemoryBarrier()    MemoryBarrier()

#else

///
#define Spin_ReadWriteBarrier() _ReadWriteBarrier()

#define Spin_MemoryBarrier()    MemoryBarrier()

#endif  /* __INTER_COMPILER || __ICC */

#else  /* !_MSC_VER */

#ifndef spin_likely
#define spin_likely(x)          __builtin_expect((x), 1)
#endif

#ifndef spin_unlikely
#define spin_unlikely(x)        __builtin_expect((x), 0)
#endif

#ifndef inline
#define inline            inline
#endif

#define ALIGN_PREFIX(N)         __attribute__((__aligned__((N))))
#define ALIGN_SUFFIX(N)

#define CACHE_ALIGN_PREFIX      __attribute__((__aligned__((spin_CACHE_LINE_SIZE))))
#define CACHE_ALIGN_SUFFIX

#define PACKED_ALIGN_PREFIX(N)
#define PACKED_ALIGN_SUFFIX(N)  __attribute__((packed, aligned(N)))

///
/// See: http://en.wikipedia.org/wiki/Memory_ordering
///
/// See: http://bbs.csdn.net/topics/310025520
///

//#define Spin_ReadWriteBarrier()     asm volatile ("":::"memory");
#define Spin_ReadWriteBarrier()     __asm__ __volatile__ ("":::"memory");

#define Spin_MemoryBarrier()        __sync_synchronize()

#endif  /* _MSC_VER */

#if defined(_MSC_VER) || defined(__INTER_COMPILER) || defined(__ICC)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
//#include "msvc/targetver.h"
#include <windows.h>
#include <intrin.h>

#define spin_val_compare_and_swap32(destPtr, oldValue, newValue)        \
    InterlockedCompareExchange((volatile LONG *)(destPtr),              \
                            (LONG)(newValue), (LONG)(oldValue))

#define spin_val_compare_and_swap64(destPtr, oldValue, newValue)        \
    InterlockedCompareExchange64((volatile LONG64 *)(destPtr),          \
                            (LONG64)(newValue), (LONG64)(oldValue))

#define spin_bool_compare_and_swap32(destPtr, oldValue, newValue)       \
    (InterlockedCompareExchange((volatile LONG *)(destPtr),             \
                            (LONG)(newValue), (LONG)(oldValue))         \
                                == (LONG)(oldValue))

#define spin_bool_compare_and_swap64(destPtr, oldValue, newValue)       \
    (InterlockedCompareExchange64((volatile LONG64 *)(destPtr),         \
                            (LONG64)(newValue), (LONG64)(oldValue))     \
                                == (LONG64)(oldValue))

#define spin_lock_test_and_set32(destPtr, newValue)                     \
    InterlockedExchange((volatile LONG *)(destPtr), (LONG)(newValue))

#define spin_fetch_and_add32(destPtr, addValue)                         \
    InterlockedExchangeAdd((volatile LONG *)(destPtr), (LONG)(addValue))

#define spin_fetch_and_add64(destPtr, addValue)                         \
    InterlockedExchangeAdd64((volatile LONGLONG *)(destPtr), (LONGLONG)(addValue))

#elif defined(__GUNC__) || defined(__linux__) \
   || defined(__clang__) || defined(__APPLE__) || defined(__FreeBSD__) \
   || defined(__CYGWIN__) || defined(__MINGW32__)

#define spin_val_compare_and_swap32(destPtr, oldValue, newValue)        \
    __sync_val_compare_and_swap((volatile uint32_t *)(destPtr),         \
                            (uint32_t)(oldValue), (uint32_t)(newValue))

#define spin_val_compare_and_swap64(destPtr, oldValue, newValue)        \
    __sync_val_compare_and_swap((volatile uint64_t *)(destPtr),         \
                            (uint64_t)(oldValue), (uint64_t)(newValue))

#define spin_val_compare_and_swap(destPtr, oldValue, newValue)          \
    __sync_val_compare_and_swap((destPtr), (oldValue), (newValue))

#define spin_bool_compare_and_swap32(destPtr, oldValue, newValue)       \
    __sync_bool_compare_and_swap((volatile uint32_t *)(destPtr),        \
                            (uint32_t)(oldValue), (uint32_t)(newValue))

#define spin_bool_compare_and_swap64(destPtr, oldValue, newValue)       \
    __sync_bool_compare_and_swap((volatile uint64_t *)(destPtr),        \
                            (uint64_t)(oldValue), (uint64_t)(newValue))

#define spin_bool_compare_and_swap(destPtr, oldValue, newValue)         \
    __sync_bool_compare_and_swap((destPtr), (oldValue), (newValue))

#define spin_lock_test_and_set32(destPtr, newValue)                     \
    __sync_lock_test_and_set((volatile uint32_t *)(destPtr),            \
                             (uint32_t)(newValue))

#define spin_fetch_and_add32(destPtr, addValue)                         \
    __sync_fetch_and_add((volatile uint32_t *)(destPtr),                \
                         (uint32_t)(addValue))

#define spin_fetch_and_add64(destPtr, addValue)                         \
    __sync_fetch_and_add((volatile uint64_t *)(destPtr),                \
                         (uint64_t)(addValue))

#else

#define spin_val_compare_and_swap32(destPtr, oldValue, newValue)        \
    __internal_val_compare_and_swap32((volatile uint32_t *)(destPtr),   \
                                (uint32_t)(oldValue), (uint32_t)(newValue))

#define spin_val_compare_and_swap64(destPtr, oldValue, newValue)        \
    __internal_val_compare_and_swap64((volatile uint64_t *)(destPtr),   \
                                (uint64_t)(oldValue), (uint64_t)(newValue))

#define spin_bool_compare_and_swap32(destPtr, oldValue, newValue)       \
    __internal_bool_compare_and_swap32((volatile uint32_t *)(destPtr),  \
                                (uint32_t)(oldValue), (uint32_t)(newValue))

#define spin_bool_compare_and_swap64(destPtr, oldValue, newValue)       \
    __internal_bool_compare_and_swap64((volatile uint64_t *)(destPtr),  \
                                (uint64_t)(oldValue), (uint64_t)(newValue))

#define spin_lock_test_and_set32(destPtr, newValue)                     \
    __internal_lock_test_and_set32((volatile uint32_t *)(destPtr),      \
                                (uint32_t)(newValue))

#define spin_fetch_and_add32(destPtr, addValue)                         \
    __internal_fetch_and_add32((volatile uint32_t *)(destPtr),          \
                                (uint32_t)(addValue))

#define spin_fetch_and_add64(destPtr, addValue)                         \
    __internal_fetch_and_add64((volatile uint64_t *)(destPtr),          \
                                (uint64_t)(addValue))

#endif  /* defined(_MSC_VER) || defined(__INTER_COMPILER) */

#if defined(_MSC_VER) || defined(__INTEL_COMPILER)  || defined(__ICC) \
 || defined(__MINGW32__) || defined(__CYGWIN__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
//#include "msvc/targetver.h"
#include <windows.h>
#elif defined(__linux__) || defined(__GUNC__) \
   || defined(__clang__) || defined(__APPLE__) || defined(__FreeBSD__)
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

static inline
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

static inline
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

static inline
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

static inline
BOOL __internal_bool_compare_and_swap32(volatile uint32_t *destPtr,
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

static inline
BOOL __internal_bool_compare_and_swap64(volatile uint64_t *destPtr,
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

static inline
uint32_t __internal_lock_test_and_set32(volatile uint32_t *destPtr,
                                        uint32_t newValue)
{
    uint32_t origValue = *destPtr;
    *destPtr = newValue;
    return origValue;
}

static inline
uint32_t __internal_fetch_and_add32(volatile uint32_t *destPtr,
                                    uint32_t addValue)
{
    uint32_t origValue = *destPtr;
    *destPtr += addValue;
    return origValue;
}

static inline
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

#endif  /* _SPIN__PORT_H_ */
