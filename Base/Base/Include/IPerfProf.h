#ifndef __I_PerfProf_H__
#define __I_PerfProf_H__

#include "Config.h"

#ifdef SUPPORT_PROFILE

namespace rkt
{
	class RKT_EXPORT ppSample
	{
	public:
		ppSample(const char * name);
		ppSample(const char * name,int id);
		~ppSample();
	};

/// 是否打开性能测试
#if defined(OPEN_PROFILE) || defined(OPEN_PROFILEBYKEY)

	#define	PP_BY_NAME( name )		rkt::ppSample __profile(name);
	#define	PP_BY_ID( name,id )		rkt::ppSample __profile(name,id);
	#define PP_BY_NAME_START(name)	rkt::getProfile()->Start(name);
	#define PP_BY_NAMEEX_START(x)	{rkt::ostrbuf __o_s_b_;__o_s_b_<<x;rkt::getProfile()->Start(__o_s_b_.c_str());}
	#define PP_BY_NAME_STOP()		rkt::getProfile()->Stop();

#else

	#define	PP_BY_NAME(name)
	#define	PP_BY_ID(name,id)
	#define PP_BY_NAME_START(name)
	#define PP_BY_NAMEEX_START(x)
	#define PP_BY_NAME_STOP()

#endif

	struct IProfileNode
	{
		virtual IProfileNode * GetParent()	= 0;
		virtual IProfileNode * GetSibling()	= 0;
		virtual IProfileNode * GetChild()	= 0;

		virtual IProfileNode * GetSubNode( const char * name ) = 0;

		virtual void				Reset( void )	= 0;
		virtual void				Call( void )	= 0;
		virtual bool				Return( void )	= 0;

		virtual const char *	GetName()		= 0;
		virtual int				GetTotalCalls()	= 0;
		virtual float			GetTotalTime()	= 0;

		virtual float			GetAverageTime() = 0;
		virtual float			GetMaxTime() = 0;
		virtual float			GetMinTime() = 0;
		virtual float           GetNowTime() = 0;

		virtual void			Break(void) = 0;
	};

	struct IProfileIterator
	{
		virtual void	First()		= 0;
		virtual void	Next()		= 0;
		virtual bool	IsDone()	= 0;

		virtual void	EnterChild(int index) 					= 0;
		virtual void	EnterLargestChild()						= 0;
		virtual void	EnterParent()							= 0;

		virtual const char *	GetCurrentName()				= 0;
		virtual int				GetCurrentTotalCalls()			= 0;
		virtual float			GetCurrentTotalTime()			= 0;

		virtual const char *	GetCurrentParentName()			= 0;
		virtual int				GetCurrentParentTotalCalls()	= 0;
		virtual float			GetCurrentParentTotalTime()		= 0;
	};

	struct IProfile
	{
		virtual void Start(const char *name)				= 0;
		virtual void Stop()									= 0;

		virtual void	Reset()								= 0;
		virtual void	Profile()							= 0;
		virtual void	ShowUI()							= 0;
		virtual void	HideUI()							= 0;
		virtual void	WriteLog2File(char szFileName[])	= 0;
		virtual void	WriteLog2CsvFile(char szFileName[])	= 0;

		virtual void	IncrementFrameCounter()				= 0;
		virtual int		GetFrameCountSinceReset()			= 0;
		virtual float	GetTimeSinceReset()					= 0;

		virtual void CreateProfileWindow()					= 0;
		virtual void DestroyProfileWindow()					= 0;

	//	virtual IProfileIterator *	GetIterator()	= 0;
	//	virtual void				ReleaseIterator(IProfileIterator *) = 0;
		virtual IProfileNode *		GetRoot()				= 0;

		virtual void	release()							= 0;

		virtual void enable()								= 0;
		virtual void disable()								= 0;
		virtual void enableAutoBreak(float limitTicks)		= 0;
		virtual void disableAutoBreak()						= 0;
		virtual LONGLONG getRunTimes()						= 0;
	};

	RKT_API IProfile*	getProfile();
}


#else // #ifdef SUPPORT_PROFILE

#define	PP_BY_NAME(name)
#define	PP_BY_ID(name,id)
#define PP_BY_NAME_START(name)
#define PP_BY_NAMEEX_START(x)
#define PP_BY_NAME_STOP()

#endif // #ifndef SUPPORT_PROFILE

#endif