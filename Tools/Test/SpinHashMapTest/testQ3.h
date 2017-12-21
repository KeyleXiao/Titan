
#ifndef _RINGQUEUE_TEST_H_
#define _RINGQUEUE_TEST_H_

//#include "msvc/stdint.h"

////////////////////////////////////////////////////////////////////////////////

/// RingQueue������(QSIZE, ���г���, ������2���ݴη�)��Maskֵ
#define QSIZE               (1 << 21)
/// ����һ���벻Ҫ�޸�, �м�!!! qmask = qsize - 1
#define QMASK               (QSIZE - 1)

/// �ֱ���push(����)��pop(����)���߳���
#define PUSH_CNT            4
#define POP_CNT             4

/// �ַ��������̵߳���Ϣ�ܳ���, �Ǹ����߳���Ϣ�������ܺ�
/// ���������������, ���Լ��޸�Ϊ�����Ǹ����� 8000
#if 1
#define MAX_MSG_COUNT           (8000000 * 1)
#else
#define MAX_MSG_COUNT           8000
#endif

/// ��ͬ��MAX_MSG_COUNT
#define MAX_MSG_CNT             (MAX_MSG_COUNT)

/// �ַ���ÿ��(push)�̵߳���Ϣ����, ��ͬ��MAX_PUSH_MSG_COUNT
#define MAX_PUSH_MSG_COUNT      (MAX_MSG_COUNT / PUSH_CNT)

/// �ַ���ÿ��(pop)�̵߳���Ϣ����
#define MAX_POP_MSG_COUNT       (MAX_MSG_COUNT / POP_CNT)

////////////////////////////////////////////////////////////////////////////////

/// �Ƿ���ݱ��뻷���Զ������Ƿ�ʹ�� 64bit �� sequence (���), Ĭ��Ϊ 0 (���Զ�)
#define AUTO_SCAN_64BIT_SEQUENCE    0

/// �Ƿ�ʹ�� 64bit �� sequence (���), Ĭ��ֵΪ 0 (��ʹ��)
#define USE_64BIT_SEQUENCE          0

/// ����ʵ�ʱ��뻷�������Ƿ�ʹ�� 64 bit sequence ?
#if defined(AUTO_SCAN_64BIT_SEQUENCE) && (AUTO_SCAN_64BIT_SEQUENCE != 0)
  #undef USE_64BIT_SEQUENCE
  #if defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64)
    #define USE_64BIT_SEQUENCE      1
  #else
    #define USE_64BIT_SEQUENCE      0
  #endif
#endif

////////////////////////////////////////////////////////////////////////////////

/// �Ƿ������̵߳�CPU��Ե��(0������, 1����, Ĭ�ϲ�����,
///       ��ѡ�������������ò�Ҫ����, VirtualBox�����ֻ����һ�� CPU����)
#ifndef USE_THREAD_AFFINITY
#define USE_THREAD_AFFINITY     0
#endif

/// �Ƿ�����ϵͳ��ʱ��Ƭ��С���ʱ��, ��Sleep()�ľ�����Ӱ��(0������, 1����, Ĭ�ϲ�����,
///       ��ѡ��ֻ��Windows�²���Ч)
#ifndef USE_TIME_PERIOD
#define USE_TIME_PERIOD         0
#endif

/// �Ƿ�����jimi:RingQueue�Ĳ��Դ���
#ifndef USE_spin_RINGQUEUE
#define USE_spin_RINGQUEUE      1
#endif

/// �Ƿ�����q3.h�Ĳ��Դ���
#ifndef USE_DOUBAN_QUEUE
#define USE_DOUBAN_QUEUE        1
#endif

////////////////////////////////////////////////////////////////////////////////

///
/// RingQueue�������Ͷ���: (����ú�TEST_FUNC_TYPEδ����, ���ͬ�ڶ���Ϊ0)
///
/// ����Ϊ0, ��ʾʹ�ö�����q3.h��lock-free�Ľ���,    ����RingQueue.push(), RingQueue.pop();
/// ����Ϊ1, ��ʾʹ��ϸ���ȵı�׼spin_mutex������,   ����RingQueue.spin_push(),  RingQueue.spin_pop();
/// ����Ϊ2, ��ʾʹ��ϸ���ȵĸĽ���spin_mutex������, ����RingQueue.spin1_push(), RingQueue.spin1_pop();
/// ����Ϊ3, ��ʾʹ��ϸ���ȵ�ͨ����spin_mutex������, ����RingQueue.spin2_push(), RingQueue.spin2_pop();
/// ����Ϊ4, ��ʾʹ�ô����ȵ�pthread_mutex_t��(Windows��Ϊ�ٽ���, Linux��Ϊpthread_mutex_t),
///          ����RingQueue.mutex_push(), RingQueue.mutex_pop();
/// ����Ϊ9, ��ʾʹ��ϸ���ȵķ���spin_mutex������(������), ����RingQueue.spin3_push(), RingQueue.spin3_pop();
///
/// ���� 0 ���ܻᵼ���߼�����, �������, ���ҵ�(PUSH_CNT + POP_CNT) > CPU���������ʱ,
///     �п��ܲ�����ɲ��Ի�����ʱ��ܾ�(��ʮ��򼸷��Ӳ���, ���ҽ�����Ǵ����), ��������֤.
///
/// ����ֻ��1, 2, 3, 4�����Եõ���ȷ���, 2���ٶȿ������, 3���ȶ�(�Ƽ�);
///
/// 9 ���ܻ�������ţ(��Ϣ�����е����ߵú�������, ��������);
///

/// ȡֵ��Χ�� 0-9
/// ����Ϊ 0 �� ������ú�, ��ʾ����main()��ָ����ĳ���� RingQueue ����;
/// ����Ϊ 1-9, ���ʾֻ���� TEST_FUNC_TYPE ָ���Ĳ�������, ���Ͷ���ֵ����.
/// �����㶨��Ϊ 0(����) ���� 3(ֻ����ȫ���һ��)
#ifndef TEST_FUNC_TYPE
#define TEST_FUNC_TYPE          0
#endif

/// �Ƿ���ʾ push ����, pop ���� �� rdtsc���� �ȶ���Ĳ�����Ϣ
#define DISPLAY_EXTRA_RESULT    0

///
/// ��spin_mutex���Ƿ�ʹ��spin_counter����, 0Ϊ��ʹ��(����!������Ϊ��ֵ), 1Ϊʹ��
///
#define USE_SPIN_MUTEX_COUNTER  0

///
/// spin_mutex�����spin_countֵ, Ĭ��ֵΪ16, ������Ϊ0��1,2, ����! ��Ϊ0���USE_SPIN_MUTEX_COUNTER��Ϊ0�ȼ�
///
#define MUTEX_MAX_SPIN_COUNT    1

#define SPIN_YIELD_THRESHOLD    1

////////////////////////////////////////////////////////////////////////////////

/// �����CacheLineSize(x86����64�ֽ�)
#define CACHE_LINE_SIZE         64

#ifdef __cplusplus
extern "C" {
#endif

struct msg_t
{
    uint64_t dummy;
};

typedef struct msg_t msg_t;

struct spin_mutex_t
{
    volatile char padding1[CACHE_LINE_SIZE];
    volatile uint32_t locked;
    volatile char padding2[CACHE_LINE_SIZE - 1 * sizeof(uint32_t)];
    volatile uint32_t spin_counter;
    volatile uint32_t recurse_counter;
    volatile uint32_t thread_id;
    volatile uint32_t reserve;
    volatile char padding3[CACHE_LINE_SIZE - 4 * sizeof(uint32_t)];
};


#ifdef __cplusplus
}
#endif

#endif  /* _RINGQUEUE_TEST_H_ */
