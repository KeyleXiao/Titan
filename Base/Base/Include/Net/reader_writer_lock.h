#include "..//thread.h"

/** A simple Reader/Writer Lock.
This RWL has no events - we rely solely on spinlocks and sleep() to yield control to other threads.
I don't know what the exact penalty is for using sleep vs events, but at least when there is no contention, we are basically
as fast as a critical section. This code is written for Windows, but it should be trivial to find the appropriate
equivalents on another OS.
**/
class TinyReaderWriterLock
{
public:
	 volatile uint32 Main;
	 static const uint32 WriteDesireBit = 0x80000000;

	 void Noop( uint32 tick )
	 {
		if ( ((tick + 1) & 0xfff) == 0 )  // Sleep after 4k cycles. Crude, but usually better than spinning indefinitely.
			Sleep(0);
	 }

	 TinyReaderWriterLock()     { Main = 0; }
	~TinyReaderWriterLock()    { ASSERT( Main == 0 ); }

	 void EnterRead()
	 {
	  for ( uint32 tick = 0 ;; tick++ )
	  {
		   uint32 oldVal = Main;
		   if ( (oldVal & WriteDesireBit) == 0 )
		   {
				if ( InterlockedCompareExchange( (LONG*) &Main, oldVal + 1, oldVal ) == oldVal )
					break;
		   }

	       Noop(tick);
	  }
	 }

	 void EnterWrite()
	 {
		  for ( uint32 tick = 0 ;; tick++ )
		  {
			   if ( (tick & 0xfff) == 0 )          // Set the write-desire bit every 4k cycles (including cycle 0).
				_InterlockedOr( (LONG*) &Main, WriteDesireBit );
			   uint32 oldVal = Main;

			   if ( oldVal == WriteDesireBit )
			   {
				if ( InterlockedCompareExchange( (LONG*) &Main, -1, WriteDesireBit ) == WriteDesireBit )
				 break;
			   }
			   Noop(tick);
		  }
	 }

	 void LeaveRead()
	 {
		  ASSERT( Main != -1 );
		  InterlockedDecrement( (LONG*) &Main );
	 }

	 void LeaveWrite()
	 {
		  ASSERT( Main == -1 );
		  InterlockedIncrement( (LONG*) &Main );
	 }
};