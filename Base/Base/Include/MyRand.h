//==========================================================================
/**
* @file	  : MyRand.h
* @author : PeakGao <peakgao163@163.com>
* created : 2009-4-29   12:56
* purpose : 基于系统的随机数算法，只是拥有独立的种子
*/
//==========================================================================

#ifndef __MyRand_h__
#define __MyRand_h__

#ifdef SUPPORT_RANDGENERATOR

#include <math.h>

namespace rkt {

	/// 具有独立种子的随机数发生器(创建 0-0x7fff 之间的一个随机数)
	class MyRand
	{
		ulong	mSeed;
	public:
		MyRand() : mSeed(::rand())			{ }
		MyRand(ulong seed) : mSeed(seed)	{ }

		/// 初始化随机种子
		void srand(ulong seed)				{ mSeed = seed; }

		/// 获取一个随机数，可自动初始化随机种子
		int rand()
		{
			return (((mSeed = mSeed * 214013L + 2531011L) >> 16) & 0x7fff);
		}


		/// 在指定范围内随机[_min, _max]
		int rand(int _min, int _max)
		{
			if (_max > _min)
			{
				ulong rnd = this->rand();
				//double r = (double)rnd / (RAND_MAX+1); // 等比缩放
				double r = (double)rnd * 0.000030517578125;
				return (int)::floor(r * (_max - _min + 1)) + _min;
			}
			else if (_max < _min)
			{
				ulong rnd = this->rand();
				//double r = (double)rnd / (RAND_MAX+1); // 等比缩放
				double r = (double)rnd * 0.000030517578125;
				return (int)::floor(r * (_min - _max + 1)) + _max;
			}

			return _max;
		}
	};

} // namespace

#endif // #ifdef SUPPORT_RANDGENERATOR

#endif // __MyRand_h__