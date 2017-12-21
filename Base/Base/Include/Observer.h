//==========================================================================
/**
* @file	  : Observer.h
* @author : PeakGao <peakgao163@163.com>
* created : 2009-12-30   15:24
* purpose : 观察者模式
*/
//==========================================================================

#ifndef __Observer_h__
#define __Observer_h__

#include "Common.h"
#include "safelist.h"


namespace rkt {


	struct ObserverArgs
	{
	};

	struct BufferArgs : public ObserverArgs
	{
		void* data;
		size_t len;
	};



	/// 观察者
	class Observable;
	class RKT_EXPORT Observer
	{
	public:
		virtual void update(Observable* o, ObserverArgs& args) {}
	};


	/// 可观察的对象
	class RKT_EXPORT Observable
	{
	protected:
		typedef safelist<Observer*> ObserverList;
		ObserverList  mObserverList;

	public:
		void addObserver(Observer* o);
		void removeObserver(Observer* o);
		inline bool hasObserver(Observer* o) const;
		size_t getObservers() const;

		virtual void notifyObservers(ObserverArgs& args);
	};

} // rkt

#endif // __Observer_h__