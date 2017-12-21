/*******************************************************************
** 文件名:	queued_spin_lock.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	6/7/2016
** 版  本:	1.0
** 描  述:	摘自

http://www.codeproject.com/Tips/100195/Queued-spinlocks

			
********************************************************************/

#pragma once

// 队列原子锁,避免某个线程总是抢不到
// represents processor in wait queue of the spinlock
struct qsl_entry
{
 
	// next processor in the queue that is waiting to enter section
	qsl_entry* next;
 
	// indicates whether the access to section has been granted to processor
	int state;
 
	qsl_entry() : next(0),state(0){}
};
 
// queued spinlock
struct qsl
{
 
	// the first processor in the queue that is waiting to enter section
	qsl_entry* head;
 
	qsl() : head(0){}
};
 
// requests access to critical section guarded by the spinlock,
// if the section is already taken it puts processor to wait
// and insert it into queue
// lck - queued lock that used to guard section
// ent - entry that represent processor in queue of the spinlock
void lock_qsl(qsl* lck, qsl_entry* ent)
{
	__asm
	{
		mov eax, ent;
		mov ebx, lck;

		// prepare queue entry
		mov [eax], 0;
		mov edx, eax;
		mov [eax]qsl_entry.state, 1;

		// store it as the last entry of the queue
		lock xchg [ebx],eax;

		// if the section available grant access to processor?
		test eax, eax;
		jz enter_section;
			// link new entry with the rest of queue
			mov [eax],edx
 
			// wait for processor's turn
			wait1:
				pause;
				cmp [edx]qsl_entry.state, 1;
				je wait1;

		enter_section:
	}
}
 
// release access to critical section guarded by the spinlock
// it also grants access to next processor in the queue if there is one
// lck - queued lock that used to guard section
// ent - entry that represent processor in queue of the spinlock
void unlock_qsl(qsl* lck, qsl_entry* ent)
{
	__asm
	{
		mov eax, ent;
		mov ebx, lck;
		xor ecx, ecx;
		mov edx, eax;

		// release section and get next processor in the queue
		// which is waiting for the section
		lock cmpxchg [ebx], ecx;

		// is this the last processor waiting for the queue?
		je exit_section;

			// wait for processor that just has started entering into section
			wait2:
				pause;
				cmp [edx], ecx;
				je wait2;

			// grant access to next processor in the queue
			mov eax, [edx];
			mov [eax]qsl_entry.state, ecx;

		exit_section:
	}
}

// 队列原子锁，防止某个线程一直抢不到，等待过久
class queued_spin_lock
{
	qsl_entry  mEntry;
	qsl     *  mLock;
public:

	queued_spin_lock(qsl * lock)
	{
		mLock = lock;
		lock_qsl( mLock,&mEntry );
	}

	~queued_spin_lock()
	{
		unlock_qsl( mLock,&mEntry );
	}
};
