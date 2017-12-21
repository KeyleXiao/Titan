
#ifdef SUPPORT_STACKWALK

#include "IStackWalk.h"

class CrashReport : public IStackWalkCallback
{
	virtual void onDumpStart(IStackWalk*);
	virtual void onDump(IStackWalk*,PEXCEPTION_POINTERS pExceptionInfo);
	virtual long onDumpFinish(IStackWalk*,PEXCEPTION_POINTERS);
};

extern CrashReport g_CrashReport;

#endif // #ifdef SUPPORT_STACKWALK