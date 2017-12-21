
#ifndef _spin_UTIL_RINGQUEUE_H_
#define _spin_UTIL_RINGQUEUE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//#include "msvc/stdint.h"
#include "port.h"
#include "testQ3.h"

#ifndef _MSC_VER
#include <pthread.h>
#include "msvc/pthread.h"
#else
#include "msvc/pthread.h"
#endif  // !_MSC_VER

#ifdef _MSC_VER
#include <intrin.h>     // For _ReadWriteBarrier(), InterlockedCompareExchange()
#endif  // _MSC_VER
#include <emmintrin.h>

#include <stdio.h>
#include <string.h>

namespace jimi {

#if 0
struct RingQueueHead
{
    volatile uint32_t head;
    volatile uint32_t tail;
};
#else
struct RingQueueHead
{
    volatile uint32_t head;
    char padding1[spin_CACHE_LINE_SIZE - sizeof(uint32_t)];

    volatile uint32_t tail;
    char padding2[spin_CACHE_LINE_SIZE - sizeof(uint32_t)];
};
#endif

typedef struct RingQueueHead RingQueueHead;

///////////////////////////////////////////////////////////////////
// class SmallRingQueueCore<Capacity>
///////////////////////////////////////////////////////////////////

template <typename T, uint32_t Capacity>
class SmallRingQueueCore
{
public:
    typedef uint32_t    size_type;
    typedef T *         item_type;

public:
    static const size_type  kCapacityCore    = (size_type)spin_MAX(spin_ROUND_TO_POW2(Capacity), 2);
    static const bool       kIsAllocOnHeap  = false;

public:
    RingQueueHead       info;
    volatile item_type  queue[kCapacityCore];
};

///////////////////////////////////////////////////////////////////
// class RingQueueCore<Capacity>
///////////////////////////////////////////////////////////////////

template <typename T, uint32_t Capacity>
class RingQueueCore
{
public:
    typedef T *         item_type;

public:
    static const bool kIsAllocOnHeap = true;

public:
    RingQueueHead       info;
    volatile item_type *queue;
};

///////////////////////////////////////////////////////////////////
// class RingQueueBase<T, Capacity, CoreTy>
///////////////////////////////////////////////////////////////////

template <typename T, uint32_t Capacity = 16U,
          typename CoreTy = RingQueueCore<T, Capacity> >
class RingQueueBase
{
public:
    typedef uint32_t                    size_type;
    typedef uint32_t                    index_type;
    typedef T *                         value_type;
    typedef typename CoreTy::item_type  item_type;
    typedef CoreTy                      core_type;
    typedef T *                         pointer;
    typedef const T *                   const_pointer;
    typedef T &                         reference;
    typedef const T &                   const_reference;

public:
    static const size_type  kCapacity = (size_type)spin_MAX(spin_ROUND_TO_POW2(Capacity), 2);
    static const index_type kMask     = (index_type)(kCapacity - 1);

public:
    RingQueueBase(bool bInitHead = false);
    ~RingQueueBase();

public:
    void dump_info();
    void dump_detail();

    index_type mask() const      { return kMask;     };
    size_type capacity() const   { return kCapacity; };
    size_type length() const     { return sizes();   };
    size_type sizes() const;

    void init(bool bInitHead = false);

    int push(T * item);
    T * pop();

    int push2(T * item);
    T * pop2();

    int spin_push(T * item);
    T * spin_pop();

    int spin1_push(T * item);
    T * spin1_pop();

    int spin2_push(T * item);
    T * spin2_pop();

    int spin2_push_(T * item);

    int spin3_push(T * item);
    T * spin3_pop();

    int spin8_push(T * item);
    T * spin8_pop();

    int mutex_push(T * item);
    T * mutex_pop();

protected:
    core_type       core;
    spin_mutex_t    spin_mutex;
    pthread_mutex_t queue_mutex;
};

template <typename T, uint32_t Capacity, typename CoreTy>
RingQueueBase<T, Capacity, CoreTy>::RingQueueBase(bool bInitHead  /* = false */)
{
    //printf("RingQueueBase::RingQueueBase();\n\n");

    init(bInitHead);
}

template <typename T, uint32_t Capacity, typename CoreTy>
RingQueueBase<T, Capacity, CoreTy>::~RingQueueBase()
{
    // Do nothing!
    Spin_ReadWriteBarrier();

    spin_mutex.locked = 0;

    pthread_mutex_destroy(&queue_mutex);
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
void RingQueueBase<T, Capacity, CoreTy>::init(bool bInitHead /* = false */)
{
    //printf("RingQueueBase::init();\n\n");

    if (!bInitHead) {
        core.info.head = 0;
        core.info.tail = 0;
    }
    else {
        memset((void *)&core.info, 0, sizeof(core.info));
    }

    Spin_ReadWriteBarrier();

    // Initilized spin mutex
    spin_mutex.locked = 0;
    spin_mutex.spin_counter = MUTEX_MAX_SPIN_COUNT;
    spin_mutex.recurse_counter = 0;
    spin_mutex.thread_id = 0;
    spin_mutex.reserve = 0;

    // Initilized mutex
    pthread_mutex_init(&queue_mutex, NULL);
}

template <typename T, uint32_t Capacity, typename CoreTy>
void RingQueueBase<T, Capacity, CoreTy>::dump_info()
{
    //ReleaseUtils::dump(&core.info, sizeof(core.info));
    dump_mem(&core.info, sizeof(core.info), false, 16, 0, 0);
}

template <typename T, uint32_t Capacity, typename CoreTy>
void RingQueueBase<T, Capacity, CoreTy>::dump_detail()
{
#if 0
    printf("---------------------------------------------------------\n");
    printf("RingQueueBase.p.head = %u\nRingQueueBase.p.tail = %u\n\n", core.info.p.head, core.info.p.tail);
    printf("RingQueueBase.c.head = %u\nRingQueueBase.c.tail = %u\n",   core.info.c.head, core.info.c.tail);
    printf("---------------------------------------------------------\n\n");
#else
    printf("RingQueueBase: (head = %u, tail = %u)\n",
           core.info.head, core.info.tail);
#endif
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
typename RingQueueBase<T, Capacity, CoreTy>::size_type
RingQueueBase<T, Capacity, CoreTy>::sizes() const
{
    index_type head, tail;

    Spin_ReadWriteBarrier();

    head = core.info.head;

    tail = core.info.tail;

    return (size_type)((head - tail) <= kMask) ? (head - tail) : (size_type)-1;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
int RingQueueBase<T, Capacity, CoreTy>::push(T * item)
{
    index_type head, tail, next;
    bool ok = false;

    Spin_ReadWriteBarrier();

	int nFlag = 0;
    do {
		if (nFlag > 0 && nFlag % 8000 == 0)
		{
			Sleep(0);
		}
		++nFlag;
		
        head = core.info.head;
        tail = core.info.tail;
        if ((head - tail) > kMask)
            return -1;
        next = head + 1;
        ok = spin_bool_compare_and_swap32(&core.info.head, head, next);		
    } while (!ok);

    core.queue[head & kMask] = item;

    Spin_ReadWriteBarrier();

    return 0;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
T * RingQueueBase<T, Capacity, CoreTy>::pop()
{
    index_type head, tail, next;
    value_type item;
    bool ok = false;

    Spin_ReadWriteBarrier();

	int nFlag = 0;
    do {
		if (nFlag > 0 && nFlag % 8000 == 0)
		{
			Sleep(0);
		}
		++nFlag;
        head = core.info.head;
        tail = core.info.tail;
        if ((tail == head) || (tail > head && (head - tail) > kMask))
            return (value_type)NULL;
        next = tail + 1;
        ok = spin_bool_compare_and_swap32(&core.info.tail, tail, next);
    } while (!ok);

    item = core.queue[tail & kMask];

    Spin_ReadWriteBarrier();

    return item;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
int RingQueueBase<T, Capacity, CoreTy>::push2(T * item)
{
    index_type head, tail, next;
    bool ok = false;

    Spin_ReadWriteBarrier();

#if 1
    do {
        head = core.info.head;
        tail = core.info.tail;
        if ((head - tail) > kMask)
            return -1;
        next = head + 1;
        ok = spin_bool_compare_and_swap32(&core.info.head, head, next);
    } while (!ok);
#else
    do {
        head = core.info.head;
        tail = core.info.tail;
        if ((head - tail) > kMask)
            return -1;
        next = head + 1;
    } while (spin_compare_and_swap32(&core.info.head, head, next) != head);
#endif
	core.queue[head & kMask] = item;
    Spin_ReadWriteBarrier();       

    return 0;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
T * RingQueueBase<T, Capacity, CoreTy>::pop2()
{
    index_type head, tail, next;
    value_type item;
    bool ok = false;

    Spin_ReadWriteBarrier();

#if 1
    do {
        head = core.info.head;
        tail = core.info.tail;
        //if (tail >= head && (head - tail) <= kMask)
        if ((tail == head) || (tail > head && (head - tail) > kMask))
            return (value_type)NULL;
        next = tail + 1;
        ok = spin_bool_compare_and_swap32(&core.info.tail, tail, next);
    } while (!ok);
#else
    do {
        head = core.info.head;
        tail = core.info.tail;
        //if (tail >= head && (head - tail) <= kMask)
        if ((tail == head) || (tail > head && (head - tail) > kMask))
            return (value_type)NULL;
        next = tail + 1;
    } while (spin_compare_and_swap32(&core.info.tail, tail, next) != tail);
#endif

    item = core.queue[tail & kMask];

    Spin_ReadWriteBarrier();

    return item;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
int RingQueueBase<T, Capacity, CoreTy>::spin_push(T * item)
{
    index_type head, tail, next;
#if defined(USE_SPIN_MUTEX_COUNTER) && (USE_SPIN_MUTEX_COUNTER != 0)
    uint32_t pause_cnt, spin_count, max_spin_cnt;
#endif

#if defined(USE_SPIN_MUTEX_COUNTER) && (USE_SPIN_MUTEX_COUNTER != 0)
    max_spin_cnt = MUTEX_MAX_SPIN_COUNT;
    spin_count = 1;

    while (spin_val_compare_and_swap32(&spin_mutex.locked, 0U, 1U) != 0U) {
        if (spin_count <= max_spin_cnt) {
            for (pause_cnt = spin_count; pause_cnt > 0; --pause_cnt) {
                spin_mm_pause();
                //spin_mm_pause();
            }
            spin_count *= 2;
        }
        else {
            //SwitchToThread();
            Sleep(0);
            //spin_counter = 1;
        }
    }
#else   /* !USE_SPIN_MUTEX_COUNTER */
    while (spin_val_compare_and_swap32(&spin_mutex.locked, 0U, 1U) != 0U) {
        //SwitchToThread();
        Sleep(0);
    }
#endif   /* USE_SPIN_MUTEX_COUNTER */

    head = core.info.head;
    tail = core.info.tail;
    if ((head - tail) > kMask) {
        Spin_ReadWriteBarrier();
        //spin_lock_test_and_set32(&spin_mutex.locked, 0U);
        spin_mutex.locked = 0;
        return -1;
    }
    next = head + 1;
    core.info.head = next;

    core.queue[head & kMask] = item;

    Spin_ReadWriteBarrier();

    //spin_lock_test_and_set32(&spin_mutex.locked, 0U);
    spin_mutex.locked = 0;

    return 0;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
T * RingQueueBase<T, Capacity, CoreTy>::spin_pop()
{
    index_type head, tail, next;
    value_type item;
#if defined(USE_SPIN_MUTEX_COUNTER) && (USE_SPIN_MUTEX_COUNTER != 0)
    uint32_t pause_cnt, spin_count, max_spin_cnt;
#endif

#if defined(USE_SPIN_MUTEX_COUNTER) && (USE_SPIN_MUTEX_COUNTER != 0)
    max_spin_cnt = MUTEX_MAX_SPIN_COUNT;
    spin_count = 1;

    while (spin_val_compare_and_swap32(&spin_mutex.locked, 0U, 1U) != 0U) {
        if (spin_count <= max_spin_cnt) {
            for (pause_cnt = spin_count; pause_cnt > 0; --pause_cnt) {
                spin_mm_pause();
                //spin_mm_pause();
            }
            spin_count *= 2;
        }
        else {
            //SwitchToThread();
            Sleep(0);
            //spin_counter = 1;
        }
    }
#else   /* !USE_SPIN_MUTEX_COUNTER */
    while (spin_val_compare_and_swap32(&spin_mutex.locked, 0U, 1U) != 0U) {
        //SwitchToThread();
        Sleep(0);
    }
#endif   /* USE_SPIN_MUTEX_COUNTER */

    head = core.info.head;
    tail = core.info.tail;
    if ((tail == head) || (tail > head && (head - tail) > kMask)) {
        Spin_ReadWriteBarrier();
        //spin_lock_test_and_set32(&spin_mutex.locked, 0U);
        spin_mutex.locked = 0;
        return (value_type)NULL;
    }
    next = tail + 1;
    core.info.tail = next;

    item = core.queue[tail & kMask];

    Spin_ReadWriteBarrier();

    //spin_lock_test_and_set32(&spin_mutex.locked, 0U);
    spin_mutex.locked = 0;

    return item;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
int RingQueueBase<T, Capacity, CoreTy>::spin1_push(T * item)
{
    index_type head, tail, next;
    uint32_t pause_cnt, spin_counter;
    static const uint32_t max_spin_cnt = MUTEX_MAX_SPIN_COUNT;

    Spin_ReadWriteBarrier();

    /* atomic_exchange usually takes less instructions than
       atomic_compare_and_exchange.  On the other hand,
       atomic_compare_and_exchange potentially generates less bus traffic
       when the lock is locked.
       We assume that the first try mostly will be successful, and we use
       atomic_exchange.  For the subsequent tries we use
       atomic_compare_and_exchange.  */
    if (spin_lock_test_and_set32(&spin_mutex.locked, 1U) != 0U) {
        spin_counter = 1;
        do {
            if (spin_counter <= max_spin_cnt) {
                for (pause_cnt = spin_counter; pause_cnt > 0; --pause_cnt) {
                    spin_mm_pause();
                }
                spin_counter *= 2;
            }
            else {
                //SwitchToThread();
                Sleep(0);
                //spin_counter = 1;
            }
        } while (spin_val_compare_and_swap32(&spin_mutex.locked, 0U, 1U) != 0U);
    }

    head = core.info.head;
    tail = core.info.tail;
    if ((head - tail) > kMask) {
        Spin_ReadWriteBarrier();
        //spin_lock_test_and_set32(&spin_mutex.locked, 0U);
        spin_mutex.locked = 0;
        return -1;
    }
    next = head + 1;
    core.info.head = next;

    core.queue[head & kMask] = item;

    Spin_ReadWriteBarrier();

    //spin_lock_test_and_set32(&spin_mutex.locked, 0U);
    spin_mutex.locked = 0;

    return 0;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
T * RingQueueBase<T, Capacity, CoreTy>::spin1_pop()
{
    index_type head, tail, next;
    value_type item;
    uint32_t pause_cnt, spin_counter;
    static const uint32_t max_spin_cnt = MUTEX_MAX_SPIN_COUNT;

    Spin_ReadWriteBarrier();

    /* atomic_exchange usually takes less instructions than
       atomic_compare_and_exchange.  On the other hand,
       atomic_compare_and_exchange potentially generates less bus traffic
       when the lock is locked.
       We assume that the first try mostly will be successful, and we use
       atomic_exchange.  For the subsequent tries we use
       atomic_compare_and_exchange.  */
    if (spin_lock_test_and_set32(&spin_mutex.locked, 1U) != 0U) {
        spin_counter = 1;
        do {
            if (spin_counter <= max_spin_cnt) {
                for (pause_cnt = spin_counter; pause_cnt > 0; --pause_cnt) {
                    spin_mm_pause();
                }
                spin_counter *= 2;
            }
            else {
                //SwitchToThread();
                Sleep(0);
                //spin_counter = 1;
            }
        } while (spin_val_compare_and_swap32(&spin_mutex.locked, 0U, 1U) != 0U);
    }

    head = core.info.head;
    tail = core.info.tail;
    if ((tail == head) || (tail > head && (head - tail) > kMask)) {
        Spin_ReadWriteBarrier();
        //spin_lock_test_and_set32(&spin_mutex.locked, 0U);
        spin_mutex.locked = 0;
        return (value_type)NULL;
    }
    next = tail + 1;
    core.info.tail = next;

    item = core.queue[tail & kMask];

    Spin_ReadWriteBarrier();

    //spin_lock_test_and_set32(&spin_mutex.locked, 0U);
    spin_mutex.locked = 0;

    return item;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
int RingQueueBase<T, Capacity, CoreTy>::spin2_push_(T * item)
{
    index_type head, tail, next;
    int32_t pause_cnt;
    uint32_t loop_count, yield_cnt, spin_count;
    static const uint32_t YIELD_THRESHOLD = 1;  // ����������ֵ

    Spin_ReadWriteBarrier();    // ��������д����

    // ������һ����һ��С����, �ο��� pthread_spin_lock(), ������ʼ.
    if (spin_lock_test_and_set32(&spin_mutex.locked, 1U) != 0U) {
        loop_count = 0;
        spin_count = 1;
        do {
            if (loop_count < YIELD_THRESHOLD) {
                for (pause_cnt = spin_count; pause_cnt > 0; --pause_cnt) {
                    spin_mm_pause();        // ����Ϊ֧�ֳ��̵߳� CPU ׼�����л���ʾ
                }
                spin_count *= 2;
            }
            else {
                yield_cnt = loop_count - YIELD_THRESHOLD;
                if ((yield_cnt & 63) == 63) {
                    Sleep(1);          // ����������, ת���ں�̬
                }
                else if ((yield_cnt & 3) == 3) {
                    Sleep(0);          // �л������ȼ����Լ�һ������ߵ��߳�, ���Ի������CPU������
                }
                else {
                    if (!SwitchToThread()) {    // �ò������߳����ڵ�CPU�����ϵı���߳�,
                                            // �����л������CPU�����ϵȴ����߳�
                        Sleep(0);      // ���ͬ������û�п��л����߳�,
                                            // ���е���ĺ�������(ֻ�������ȼ����Լ���ͬ����õ�)
                    }
                }
            }
            loop_count++;
        } while (spin_val_compare_and_swap32(&spin_mutex.locked, 0U, 1U) != 0U);
    }

    // ����������
    head = core.info.head;
    tail = core.info.tail;
    if ((head - tail) > kMask) {
        Spin_ReadWriteBarrier();
        // ��������, �ͷ���
        spin_mutex.locked = 0;
        return -1;
    }
    next = head + 1;
    core.info.head = next;

    core.queue[head & kMask] = item;    // ������д�����

    Spin_ReadWriteBarrier();        // ��������д����

    spin_mutex.locked = 0;          // �ͷ���
    return 0;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
int RingQueueBase<T, Capacity, CoreTy>::spin2_push(T * item)
{
    index_type head, tail, next;
    int32_t pause_cnt;
    uint32_t loop_count, yield_cnt, spin_count;
    static const uint32_t YIELD_THRESHOLD = SPIN_YIELD_THRESHOLD;

    Spin_ReadWriteBarrier();

    /* atomic_exchange usually takes less instructions than
       atomic_compare_and_exchange.  On the other hand,
       atomic_compare_and_exchange potentially generates less bus traffic
       when the lock is locked.
       We assume that the first try mostly will be successful, and we use
       atomic_exchange.  For the subsequent tries we use
       atomic_compare_and_exchange.  */
    if (spin_lock_test_and_set32(&spin_mutex.locked, 1U) != 0U) {
        loop_count = 0;
        spin_count = 1;
        do {
            if (loop_count < YIELD_THRESHOLD) {
                for (pause_cnt = spin_count; pause_cnt > 0; --pause_cnt) {
                    spin_mm_pause();
                }
                spin_count *= 2;
            }
            else {
                yield_cnt = loop_count - YIELD_THRESHOLD;
#if defined(__MINGW32__) || defined(__CYGWIN__)
                if ((yield_cnt & 3) == 3) {
                    Sleep(0);
                }
                else {
                    if (!SwitchToThread()) {
                        Sleep(0);
                        //spin_mm_pause();
                    }
                }
#else
                if ((yield_cnt & 63) == 63) {
                    Sleep(1);
                }
                else if ((yield_cnt & 3) == 3) {
                    Sleep(0);
                }
                else {
                    if (!SwitchToThread()) {
                        Sleep(0);
                        //spin_mm_pause();
                    }
                }
#endif
            }
            loop_count++;
            //spin_mm_pause();
        } while (spin_val_compare_and_swap32(&spin_mutex.locked, 0U, 1U) != 0U);
    }

    head = core.info.head;
    tail = core.info.tail;
    if ((head - tail) > kMask) {
        Spin_ReadWriteBarrier();
        spin_mutex.locked = 0;
        return -1;
    }
    next = head + 1;
    core.info.head = next;

    core.queue[head & kMask] = item;

    Spin_ReadWriteBarrier();

    spin_mutex.locked = 0;

    return 0;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
T * RingQueueBase<T, Capacity, CoreTy>::spin2_pop()
{
    index_type head, tail, next;
    value_type item;
    int32_t pause_cnt;
    uint32_t loop_count, yield_cnt, spin_count;
    static const uint32_t YIELD_THRESHOLD = SPIN_YIELD_THRESHOLD;

    Spin_ReadWriteBarrier();

    /* atomic_exchange usually takes less instructions than
       atomic_compare_and_exchange.  On the other hand,
       atomic_compare_and_exchange potentially generates less bus traffic
       when the lock is locked.
       We assume that the first try mostly will be successful, and we use
       atomic_exchange.  For the subsequent tries we use
       atomic_compare_and_exchange.  */
    if (spin_lock_test_and_set32(&spin_mutex.locked, 1U) != 0U) {
        loop_count = 0;
        spin_count = 1;
        do {
            if (loop_count < YIELD_THRESHOLD) {
                for (pause_cnt = spin_count; pause_cnt > 0; --pause_cnt) {
                    spin_mm_pause();
                }
                spin_count *= 2;
            }
            else {
                yield_cnt = loop_count - YIELD_THRESHOLD;
#if defined(__MINGW32__) || defined(__CYGWIN__)
                if ((yield_cnt & 3) == 3) {
                    Sleep(0);
                }
                else {
                    if (!SwitchToThread()) {
                        Sleep(0);
                        //spin_mm_pause();
                    }
                }
#else
                if ((yield_cnt & 63) == 63) {
                    Sleep(1);
                }
                else if ((yield_cnt & 3) == 3) {
                    Sleep(0);
                }
                else {
                    if (!SwitchToThread()) {
                        Sleep(0);
                        //spin_mm_pause();
                    }
                }
#endif
            }
            loop_count++;
            //spin_mm_pause();
        } while (spin_val_compare_and_swap32(&spin_mutex.locked, 0U, 1U) != 0U);
    }

    head = core.info.head;
    tail = core.info.tail;
    if ((tail == head) || (tail > head && (head - tail) > kMask)) {
        Spin_ReadWriteBarrier();
        //spin_lock_test_and_set32(&spin_mutex.locked, 0U);
        spin_mutex.locked = 0;
        return (value_type)NULL;
    }
    next = tail + 1;
    core.info.tail = next;

    item = core.queue[tail & kMask];

    Spin_ReadWriteBarrier();

    //spin_lock_test_and_set32(&spin_mutex.locked, 0U);
    spin_mutex.locked = 0;

    return item;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
int RingQueueBase<T, Capacity, CoreTy>::spin3_push(T * item)
{
    index_type head, tail, next;
    int32_t pause_cnt;
    uint32_t loop_count, yield_cnt, spin_count;
    static const uint32_t YIELD_THRESHOLD = SPIN_YIELD_THRESHOLD;

    Spin_ReadWriteBarrier();

    /* atomic_exchange usually takes less instructions than
       atomic_compare_and_exchange.  On the other hand,
       atomic_compare_and_exchange potentially generates less bus traffic
       when the lock is locked.
       We assume that the first try mostly will be successful, and we use
       atomic_exchange.  For the subsequent tries we use
       atomic_compare_and_exchange.  */
    if (spin_lock_test_and_set32(&spin_mutex.locked, 1U) != 0U) {
        loop_count = 0;
        spin_count = 1;
        do {
            do {
                if (loop_count < YIELD_THRESHOLD) {
                    for (pause_cnt = spin_count; pause_cnt > 0; --pause_cnt) {
                        spin_mm_pause();
                    }
                    spin_count *= 2;
                }
                else {
                    yield_cnt = loop_count - YIELD_THRESHOLD;
#if defined(__MINGW32__) || defined(__CYGWIN__)
                    if ((yield_cnt & 3) == 3) {
                        Sleep(0);
                    }
                    else {
                        if (!SwitchToThread()) {
                            Sleep(0);
                            //spin_mm_pause();
                        }
                    }
#else
                    if ((yield_cnt & 63) == 63) {
  #if !(defined(_M_X64) || defined(_WIN64))
                        Sleep(1);
  #else
                        Sleep(1);
  #endif  /* !(_M_X64 || _WIN64) */
                    }
                    else if ((yield_cnt & 3) == 3) {
                        Sleep(0);
                    }
                    else {
                        if (!SwitchToThread()) {
                            Sleep(0);
                            //spin_mm_pause();
                        }
                    }
#endif
                }
                loop_count++;
                //spin_mm_pause();
            } while (spin_mutex.locked != 0U);
        } while (spin_val_compare_and_swap32(&spin_mutex.locked, 0U, 1U) != 0U);
    }

    head = core.info.head;
    tail = core.info.tail;
    if ((head - tail) > kMask) {
        Spin_ReadWriteBarrier();
        spin_mutex.locked = 0;
        return -1;
    }
    next = head + 1;
    core.info.head = next;

    core.queue[head & kMask] = item;

    Spin_ReadWriteBarrier();

    spin_mutex.locked = 0;

    return 0;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
T * RingQueueBase<T, Capacity, CoreTy>::spin3_pop()
{
    index_type head, tail, next;
    value_type item;
    int32_t pause_cnt;
    uint32_t loop_count, yield_cnt, spin_count;
    static const uint32_t YIELD_THRESHOLD = SPIN_YIELD_THRESHOLD;

    Spin_ReadWriteBarrier();

    /* atomic_exchange usually takes less instructions than
       atomic_compare_and_exchange.  On the other hand,
       atomic_compare_and_exchange potentially generates less bus traffic
       when the lock is locked.
       We assume that the first try mostly will be successful, and we use
       atomic_exchange.  For the subsequent tries we use
       atomic_compare_and_exchange.  */
    if (spin_lock_test_and_set32(&spin_mutex.locked, 1U) != 0U) {
        loop_count = 0;
        spin_count = 1;
        do {
            do {
                if (loop_count < YIELD_THRESHOLD) {
                    for (pause_cnt = spin_count; pause_cnt > 0; --pause_cnt) {
                        spin_mm_pause();
                    }
                    spin_count *= 2;
                }
                else {
                    yield_cnt = loop_count - YIELD_THRESHOLD;
#if defined(__MINGW32__) || defined(__CYGWIN__)
                    if ((yield_cnt & 3) == 3) {
                        Sleep(0);
                    }
                    else {
                        if (!SwitchToThread()) {
                            Sleep(0);
                            //spin_mm_pause();
                        }
                    }
#else
                    if ((yield_cnt & 63) == 63) {
  #if !(defined(_M_X64) || defined(_WIN64))
                        Sleep(1);
  #else
                        Sleep(1);
  #endif  /* !(_M_X64 || _WIN64) */
                    }
                    else if ((yield_cnt & 3) == 3) {
                        Sleep(0);
                    }
                    else {
                        if (!SwitchToThread()) {
                            Sleep(0);
                            //spin_mm_pause();
                        }
                    }
#endif
                }
                loop_count++;
                //spin_mm_pause();
            } while (spin_mutex.locked != 0U);
        } while (spin_val_compare_and_swap32(&spin_mutex.locked, 0U, 1U) != 0U);
    }

    head = core.info.head;
    tail = core.info.tail;
    if ((tail == head) || (tail > head && (head - tail) > kMask)) {
        Spin_ReadWriteBarrier();
        //spin_lock_test_and_set32(&spin_mutex.locked, 0U);
        spin_mutex.locked = 0;
        return (value_type)NULL;
    }
    next = tail + 1;
    core.info.tail = next;

    item = core.queue[tail & kMask];

    Spin_ReadWriteBarrier();

    //spin_lock_test_and_set32(&spin_mutex.locked, 0U);
    spin_mutex.locked = 0;

    return item;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
int RingQueueBase<T, Capacity, CoreTy>::spin8_push(T * item)
{
    index_type head, tail, next;

    Spin_ReadWriteBarrier();
	while (spin_val_compare_and_swap32(&spin_mutex.locked, 0U, 1U) != 0U) {		
		Sleep(0);
	} 
	//*/
/*	int cnt = 0;		// mush slow
	Spin_ReadWriteBarrier();
	while (spin_val_compare_and_swap32(&spin_mutex.locked, 0U, 1U) != 0U) {		
		//	spin_mm_pause();
		cnt++;
		if (cnt > 8000) {
			cnt = 0;
			Sleep(0);
		}
	} 
	//*/

    head = core.info.head;
    tail = core.info.tail;
    if ((head - tail) > kMask) {
        Spin_ReadWriteBarrier();
        spin_mutex.locked = 0;
        return -1;
    }
    next = head + 1;
    core.info.head = next;	

    core.queue[head & kMask] = item;

    Spin_ReadWriteBarrier();
    spin_mutex.locked = 0;

    return 0;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
T * RingQueueBase<T, Capacity, CoreTy>::spin8_pop()
{
    index_type head, tail, next;
    value_type item;
   
    Spin_ReadWriteBarrier();
	while (spin_val_compare_and_swap32(&spin_mutex.locked, 0U, 1U) != 0U) {		
		Sleep(0);
	}  
	//*/
/*	int cnt = 0;		// mush slow
	Spin_ReadWriteBarrier();
	while (spin_val_compare_and_swap32(&spin_mutex.locked, 0U, 1U) != 0U) {		
		//	spin_mm_pause();
		cnt++;
		if (cnt > 8000) {
			cnt = 0;
			Sleep(0);
		}
	} 
	//*/     

    head = core.info.head;
    tail = core.info.tail;
    if ((tail == head) || (tail > head && (head - tail) > kMask)) {
        Spin_ReadWriteBarrier();
        spin_mutex.locked = 0;
        return (value_type)NULL;
    }
    next = tail + 1;
    core.info.tail = next;

    item = core.queue[tail & kMask];

    Spin_ReadWriteBarrier();
    spin_mutex.locked = 0;

    return item;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
int RingQueueBase<T, Capacity, CoreTy>::mutex_push(T * item)
{
    index_type head, tail, next;

    Spin_ReadWriteBarrier();

    pthread_mutex_lock(&queue_mutex);

    head = core.info.head;
    tail = core.info.tail;
    if ((head - tail) > kMask) {
        pthread_mutex_unlock(&queue_mutex);
        return -1;
    }
    next = head + 1;
    core.info.head = next;

    core.queue[head & kMask] = item;

    Spin_ReadWriteBarrier();

    pthread_mutex_unlock(&queue_mutex);

    return 0;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline
T * RingQueueBase<T, Capacity, CoreTy>::mutex_pop()
{
    index_type head, tail, next;
    value_type item;

    Spin_ReadWriteBarrier();

    pthread_mutex_lock(&queue_mutex);

    head = core.info.head;
    tail = core.info.tail;
    //if (tail >= head && (head - tail) <= kMask)
    if ((tail == head) || (tail > head && (head - tail) > kMask)) {
        pthread_mutex_unlock(&queue_mutex);
        return (value_type)NULL;
    }
    next = tail + 1;
    core.info.tail = next;

    item = core.queue[tail & kMask];

    Spin_ReadWriteBarrier();

    pthread_mutex_unlock(&queue_mutex);

    return item;
}

///////////////////////////////////////////////////////////////////
// class SmallRingQueue<T, Capacity>
///////////////////////////////////////////////////////////////////

template <typename T, uint32_t Capacity = 1024U>
class SmallRingQueue : public RingQueueBase<T, Capacity, SmallRingQueueCore<T, Capacity> >
{
public:
    typedef uint32_t                    size_type;
    typedef uint32_t                    index_type;
    typedef T *                         value_type;
    typedef T *                         pointer;
    typedef const T *                   const_pointer;
    typedef T &                         reference;
    typedef const T &                   const_reference;

    static const size_type kCapacity = RingQueueBase<T, Capacity, SmallRingQueueCore<T, Capacity> >::kCapacity;

public:
    SmallRingQueue(bool bFillQueue = true, bool bInitHead = false);
    ~SmallRingQueue();

public:
    void dump_detail();

protected:
    void init_queue(bool bFillQueue = true);
};

template <typename T, uint32_t Capacity>
SmallRingQueue<T, Capacity>::SmallRingQueue(bool bFillQueue /* = true */,
                                             bool bInitHead  /* = false */)
: RingQueueBase<T, Capacity, SmallRingQueueCore<T, Capacity> >(bInitHead)
{
    //printf("SmallRingQueue::SmallRingQueue();\n\n");

    init_queue(bFillQueue);
}

template <typename T, uint32_t Capacity>
SmallRingQueue<T, Capacity>::~SmallRingQueue()
{
    // Do nothing!
}

template <typename T, uint32_t Capacity>
inline
void SmallRingQueue<T, Capacity>::init_queue(bool bFillQueue /* = true */)
{
    //printf("SmallRingQueue::init_queue();\n\n");

    if (bFillQueue) {
        memset((void *)this->core.queue, 0, sizeof(value_type) * kCapacity);
    }
}

template <typename T, uint32_t Capacity>
void SmallRingQueue<T, Capacity>::dump_detail()
{
    printf("SmallRingQueue: (head = %u, tail = %u)\n",
           this->core.info.head, this->core.info.tail);
}

///////////////////////////////////////////////////////////////////
// class RingQueue<T, Capacity>
///////////////////////////////////////////////////////////////////

template <typename T, uint32_t Capacity = 1024U>
class RingQueue : public RingQueueBase<T, Capacity, RingQueueCore<T, Capacity> >
{
public:
    typedef uint32_t                    size_type;
    typedef uint32_t                    index_type;
    typedef T *                         value_type;
    typedef T *                         pointer;
    typedef const T *                   const_pointer;
    typedef T &                         reference;
    typedef const T &                   const_reference;

    typedef RingQueueCore<T, Capacity>   core_type;

    static const size_type kCapacity = RingQueueBase<T, Capacity, RingQueueCore<T, Capacity> >::kCapacity;

public:
    RingQueue(bool bFillQueue = true, bool bInitHead = false);
    ~RingQueue();

public:
    void dump_detail();

protected:
    void init_queue(bool bFillQueue = true);
};

template <typename T, uint32_t Capacity>
RingQueue<T, Capacity>::RingQueue(bool bFillQueue /* = true */,
                                   bool bInitHead  /* = false */)
: RingQueueBase<T, Capacity, RingQueueCore<T, Capacity> >(bInitHead)
{
    //printf("RingQueue::RingQueue();\n\n");

    init_queue(bFillQueue);
}

template <typename T, uint32_t Capacity>
RingQueue<T, Capacity>::~RingQueue()
{
    // If the queue is allocated on system heap, release them.
    if (RingQueueCore<T, Capacity>::kIsAllocOnHeap) {
        delete [] this->core.queue;
        this->core.queue = NULL;
    }
}

template <typename T, uint32_t Capacity>
inline
void RingQueue<T, Capacity>::init_queue(bool bFillQueue /* = true */)
{
    //printf("RingQueue::init_queue();\n\n");

    value_type *newData = new T *[kCapacity];
    if (newData != NULL) {
        this->core.queue = newData;
        if (bFillQueue) {
            memset((void *)this->core.queue, 0, sizeof(value_type) * kCapacity);
        }
    }
}

template <typename T, uint32_t Capacity>
void RingQueue<T, Capacity>::dump_detail()
{
    printf("RingQueue: (head = %u, tail = %u)\n",
           this->core.info.head, this->core.info.tail);
}

typedef RingQueue<Product, QSIZE> RingQueue_t;

}  /* namespace jimi */

#endif  /* _spin_UTIL_RINGQUEUE_H_ */
