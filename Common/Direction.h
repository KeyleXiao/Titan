//==========================================================================
/**
* @file	  : Direction.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-1-29   17:05
* purpose : 方向相关计算
*/
//==========================================================================

#ifndef __DIRECTION_H__
#define __DIRECTION_H__

#include <math.h>

/// 考虑到跟3D坐标系兼容，方向从东南方向开始，逆时针方向递增
enum
{
	/// 东南
	eEastSouth,

	/// 东方
	eEast,

	/// 东北
	eEastNorth,

	/// 北方
	eNorth,

	/// 西北
	eWestNorth,

	/// 西方
	eWest,

	/// 西南
	eWestSouth,

	/// 南方
	eSouth,

	eDirectionCount
};

#if 0
//	+	|  -
//	3	|  4
// ------------> x
//	2	|  1
//	-	|  +
//		v  y
// 计算从一个点到另一个点的方向(使用屏幕坐标)
template<class T>
int calcDir8_ex(const T& ptFrom, const T& ptTo)
{
	int nDeltaX = ptTo.x - ptFrom.x;
	int nDeltaY = ptTo.y - ptFrom.y;
	if (nDeltaX > 0)
	{
		if (nDeltaY > 0)		return eEastSouth;
		else if (nDeltaY < 0)	return eEastNorth;
		else					return eEast;
	}
	else if (nDeltaX < 0)
	{
		if (nDeltaY > 0)		return eWestSouth;
		else if (nDeltaY < 0)	return eWestNorth;
		else					return eWest;
	}
	else
	{
		if (nDeltaY > 0)		return eSouth;
		else					return eNorth;
	}
	return eEastSouth;
}
#endif

// 计算从一个点到另一个点的方向(使用TILE坐标)，适合8方向走路等
template<class T>
ulong calcDir8_tile(const T& ptFrom, const T& ptTo)
{
	long nDeltaX = ptTo.x - ptFrom.x;
	long nDeltaY = ptTo.y - ptFrom.y;
	if (nDeltaX > 0)
	{
		if (nDeltaY > 0)		return eEast;
		else if (nDeltaY < 0)	return eSouth;
		else					return eEastSouth;
	}
	else if (nDeltaX < 0)
	{
		if (nDeltaY > 0)		return eNorth;
		else if (nDeltaY < 0)	return eWest;
		else					return eWestNorth;
	}
	else
	{
		if (nDeltaY > 0)		return eEastNorth;
		else					return eWestSouth;
	}
	return eEastSouth;
}
//      ^  y
//	-	|  +
//	2	|  1
// ------------> x
//	3	|  4
//	+	|  -
// 根据范围区间模糊确定(tile坐标)
template<class T>
int calcDir8_tile_ex(const T& ptFrom, const T& ptTo)
{
	int nDeltaX = ptTo.x - ptFrom.x;
	int nDeltaY = ptTo.y - ptFrom.y;
	float fSlope; // 斜率
	if (nDeltaX == 0)
		return (nDeltaY >= 0) ? eEastNorth : eWestSouth;
	else
		fSlope = (float)nDeltaY/nDeltaX;

	if (fSlope >= 0.0f) // 1,3区间
	{
		if (nDeltaX >= 0) // 1
		{
			if (fSlope >= 2.414f)	return  eEastNorth; // 90	(67.5～90.0) < tan(67.5) = 2.414 >
			if (fSlope >= 0.414f)	return  eEast; // 45	(22.5～67.5) < tan(22.5) = 0.414 >
			if (fSlope >=   0.0f)	return  eEastSouth; // 0		(00.0～22.5)
		}
		else // 3
		{
			if (fSlope >= 2.414f)	return eWestSouth;
			if (fSlope >= 0.414f)	return eWest;
			if (fSlope >=   0.0f)	return eWestNorth;
		}
	}
	else // 2,4
	{
		fSlope = -fSlope;
		if (nDeltaX >= 0) // 4
		{
			if (fSlope >= 2.414f)	return  eWestSouth;
			if (fSlope >= 0.414f)	return  eSouth;
			if (fSlope >=   0.0f)	return  eEastSouth;
		}
		else // 2
		{
			if (fSlope >= 2.414f)	return eEastNorth;
			if (fSlope >= 0.414f)	return eNorth;
			if (fSlope >=   0.0f)	return eWestNorth;		
		}
	}
	return eEastSouth;
}
// 计算从一个点到另一个点的方向所对应的角度(使用TILE坐标)，适合走路等
template<class T>
long calcAngle8_tile(const T& ptFrom, const T& ptTo)
{
	long nDeltaX = ptTo.x - ptFrom.x;
	long nDeltaY = ptTo.y - ptFrom.y;
	if (nDeltaX > 0)
	{
		if (nDeltaY > 0)		return 45;//eEast
		else if (nDeltaY < 0)	return 315;//eSouth
		else					return 0;//eEastSouth
	}
	else if (nDeltaX < 0)
	{
		if (nDeltaY > 0)		return 135;//eNorth
		else if (nDeltaY < 0)	return 225;//eWest
		else					return 180;//eWestNorth
	}
	else
	{
		if (nDeltaY > 0)		return 90;//eEastNorth
		else					return 270;//eWestSouth
	}
	return 0;//eEastSouth
}

/// 根据角度[0,360)获取最接近的TILE方向(8方向)
inline ulong angle2Dir(ulong angle)
{
	float an = (float)angle;
	if (an > 337.5f) return eEastSouth;
	else if (an > 292.5f) return eSouth;
	else if (an > 247.5f) return eWestSouth;
	else if (an > 202.5f) return eWest;
	else if (an > 157.5f) return eWestNorth;
	else if (an > 112.5f) return eNorth;
	else if (an > 67.5f) return eEastNorth;
	else if (an > 22.5f) return eEast;
	return eEastSouth;
}

/// 根据方向活得角度(angle = dir * 45)
inline ulong dir2Angle(int dir)
{
	static ulong s_angle[8] = {0, 45, 90, 135, 180, 225, 270, 315};
	while (dir < 0) dir += eDirectionCount;
	while (dir >= eDirectionCount) dir -= eDirectionCount;
	return s_angle[dir];
}

// 根据笛卡尔坐标系计算角度，返回值范围为[0,360)
template<class T>
long calcAngle_descartes(const T& ptFrom, const T& ptTo)
{
	long angle = 0;
	long dx = ptTo.x - ptFrom.x;
	long dy = ptTo.y - ptFrom.y;
	if (dx > 0)			// 1,4象限
		angle = (long)Math::ATan((float)dy/dx).valueDegrees();
	else if (dx < 0)	// 2,3象限
		angle = 180 + (long)Math::ATan((float)dy/dx).valueDegrees();
	else if (dx == 0)
		angle= dy > 0 ? 90 : 270;
	else if (dy == 0)
		angle = dx > 0 ? 0 : 180;

	while (angle < 0) angle += 360;
	while (angle >= 360) angle -= 360;

	return angle;
}

#ifdef __Vector3_H__
inline long calcAngle_space(const Vector3& spaceFrom, const Vector3& spaceTo)
{
	POINT ptFrom	= { (LONG)spaceFrom.z, (LONG)spaceFrom.x};
	POINT ptTo		= { (LONG)spaceTo.z, (LONG)spaceTo.x};
	return calcAngle_descartes(ptFrom, ptTo);
}
#endif

#if 0
//	+	|  -
//	3	|  4
// ------------> x
//	2	|  1
//	-	|  +
//		v  y
// 根据范围区间模糊确定(笛卡儿视角，不受45度倾斜的影响)
template<class T>
int calcDir8(const T& ptFrom, const T& ptTo)
{
	int nDeltaX = ptTo.x - ptFrom.x;
	int nDeltaY = ptTo.y - ptFrom.y;
	float fSlope; // 斜率
	if (nDeltaX == 0)
		return (nDeltaY >= 0) ? eSouth : eNorth;
	else
		fSlope = (float)nDeltaY/nDeltaX;

	if (fSlope >= 0.0f) // 1,3区间
	{
		if (nDeltaX >= 0) // 1
		{
			if (fSlope >= 2.414f)	return  eSouth; // 90	(67.5～90.0) < tan(67.5) = 2.414 >
			if (fSlope >= 0.414f)	return  eEastSouth; // 45	(22.5～67.5) < tan(22.5) = 0.414 >
			if (fSlope >=   0.0f)	return  eEast; // 0		(00.0～22.5)
		}
		else // 3
		{
			if (fSlope >= 2.414f)	return eNorth;
			if (fSlope >= 0.414f)	return eWestNorth;
			if (fSlope >=   0.0f)	return eWest;
		}
	}
	else // 2,4
	{
		fSlope = -fSlope;
		if (nDeltaX >= 0) // 4
		{
			if (fSlope >= 2.414f)	return  eNorth;
			if (fSlope >= 0.414f)	return  eEastNorth;
			if (fSlope >=   0.0f)	return  eEast;
		}
		else // 2
		{
			if (fSlope >= 2.414f)	return eSouth;
			if (fSlope >= 0.414f)	return eWestSouth;
			if (fSlope >=   0.0f)	return eWest;		
		}
	}
	return eEastSouth;
}
#endif

//	+	|  -
//	3	|  4
// ------------> x
//	2	|  1
//	-	|  +
//		v  y
// 根据范围区间模糊确定(45度视角)
template<class T>
int calcDir8_45(const T& ptFrom, const T& ptTo)
{
	int nDeltaX = ptTo.x - ptFrom.x;
	int nDeltaY = ptTo.y - ptFrom.y;
	float fSlope; // 斜率
	if (nDeltaX == 0)
		return (nDeltaY >= 0) ? eSouth : eNorth;
	else
		fSlope = (float)nDeltaY/nDeltaX;

	if (fSlope >= 0.0f) // 1,3区间
	{
		if (nDeltaX >= 0) // 1
		{
			if (fSlope >= 1.618f)	return  eSouth; // 90
			if (fSlope >= 0.236f)	return  eEastSouth; // 45
			if (fSlope >=  0.0f)	return  eEast; // 0
		}
		else // 3
		{
			if (fSlope >= 1.618f)	return eNorth;
			if (fSlope >= 0.236f)	return eWestNorth;
			if (fSlope >=  0.0f)	return eWest;
		}
	}
	else // 2,4
	{
		fSlope = -fSlope;
		if (nDeltaX >= 0) // 4
		{
			if (fSlope >= 1.618f)	return  eNorth;
			if (fSlope >= 0.236f)	return  eEastNorth;
			if (fSlope >=  0.0f)	return  eEast;
		}
		else // 2
		{
			if (fSlope >= 1.618f)	return eSouth;
			if (fSlope >= 0.236f)	return eWestSouth;
			if (fSlope >=  0.0f)	return eWest;		
		}
	}
	return eEastSouth;
}

#if 0
//	+	|  -
//	3	|  4
// ------------> x
//	2	|  1
//	-	|  +
//		v  y
// 根据范围区间模糊确定(笛卡儿视角，不受45度倾斜的影响)
template<class T>
int calcDir32(const T& ptFrom, const T& ptTo)
{
	int nDeltaX = ptTo.x - ptFrom.x;
	int nDeltaY = ptTo.y - ptFrom.y;
	float fSlope;
	if (nDeltaX == 0)
		return (nDeltaY >= 0) ? 16 : 0;
	else
		fSlope = (float)nDeltaY/nDeltaX;

	if (fSlope >= 0.0f) // 1,3区间
	{
		if (nDeltaX >= 0) // 1
		{
			if (fSlope >= 10.153f)	return 16; // 90	(84.375 ～ 90.000) < tan(84.375) = 10.153 >
			if (fSlope >=  3.297f)	return 15; // 78.75	(73.125 ～ 84.375) < tan(73.125) =  3.297 >
			if (fSlope >=  1.871f)	return 14; // 67.50	(61.875 ～ 73.125) < tan(61.875) =  1.871 >
			if (fSlope >=  1.219f)	return 13; // 56.25	(50.625 ～ 61.875) < tan(50.625) =  1.219 >
			if (fSlope >=  0.821f)	return 12; // 45.00	(39.375 ～ 50.625) < tan(39.375) =  0.821 >
			if (fSlope >=  0.535f)	return 11; // 33.75	(28.125 ～ 39.375) < tan(28.125) =  0.535 >
			if (fSlope >=  0.303f)	return 10; // 22.50	(16.875 ～ 28.125) < tan(16.875) =  0.303 >
			if (fSlope >=  0.098f)	return  9; // 11.25	(05.625 ～ 16.875) < tan(05.625) =  0.098 >
			if (fSlope >=  0.000f)	return  8; // 0		(00.000 ～ 05.625) < tan(00.000) =  0.000 >
		}
		else // 3
		{
			if (fSlope >= 10.153f)	return 0;
			if (fSlope >=  3.297f)	return 31;
			if (fSlope >=  1.871f)	return 30;
			if (fSlope >=  1.219f)	return 29;
			if (fSlope >=  0.821f)	return 28;
			if (fSlope >=  0.535f)	return 27;
			if (fSlope >=  0.303f)	return 26;
			if (fSlope >=  0.098f)	return 25;
			if (fSlope >=  0.000f)	return 24;		
		}
	}
	else // 2,4
	{
		fSlope = -fSlope;
		if (nDeltaX >= 0) // 4
		{
			if (fSlope >= 10.153f)	return  0;
			if (fSlope >=  3.297f)	return  1;
			if (fSlope >=  1.871f)	return  2;
			if (fSlope >=  1.219f)	return  3;
			if (fSlope >=  0.821f)	return  4;
			if (fSlope >=  0.535f)	return  5;
			if (fSlope >=  0.303f)	return  6;
			if (fSlope >=  0.098f)	return  7;
			if (fSlope >=  0.000f)	return  8;
		}
		else // 2
		{
			if (fSlope >= 10.153f)	return 16;
			if (fSlope >=  3.297f)	return 17;
			if (fSlope >=  1.871f)	return 18;
			if (fSlope >=  1.219f)	return 19;
			if (fSlope >=  0.821f)	return 20;
			if (fSlope >=  0.535f)	return 21;
			if (fSlope >=  0.303f)	return 22;
			if (fSlope >=  0.098f)	return 23;
			if (fSlope >=  0.000f)	return 24;		
		}
	}
	return 0;
}


//	+	|  -
//	3	|  4
// ------------> x
//	2	|  1
//	-	|  +
//		v  y
// 根据范围区间模糊确定(传入TILE中心点坐标,受45度倾斜的影响)
template<class T>
int calcDir32_45(const T& ptFrom, const T& ptTo)
{
	int nDeltaX = ptTo.x - ptFrom.x;
	int nDeltaY = ptTo.y - ptFrom.y;
	float fSlope;
	if (nDeltaX == 0)
		return (nDeltaY >= 0) ? 16 : 0;
	else
		fSlope = (float)nDeltaY*2/nDeltaX;

	if (fSlope >= 0.0f) // 1,3区间
	{
		if (nDeltaX >= 0) // 1
		{
			if (fSlope >= 10.153f)	return 16; // 90	(84.375 ～ 90.000) < tan(84.375) = 10.153 >
			if (fSlope >=  3.297f)	return 15; // 78.75	(73.125 ～ 84.375) < tan(73.125) =  3.297 >
			if (fSlope >=  1.871f)	return 14; // 67.50	(61.875 ～ 73.125) < tan(61.875) =  1.871 >
			if (fSlope >=  1.219f)	return 13; // 56.25	(50.625 ～ 61.875) < tan(50.625) =  1.219 >
			if (fSlope >=  0.821f)	return 12; // 45.00	(39.375 ～ 50.625) < tan(39.375) =  0.821 >
			if (fSlope >=  0.535f)	return 11; // 33.75	(28.125 ～ 39.375) < tan(28.125) =  0.535 >
			if (fSlope >=  0.303f)	return 10; // 22.50	(16.875 ～ 28.125) < tan(16.875) =  0.303 >
			if (fSlope >=  0.098f)	return  9; // 11.25	(05.625 ～ 16.875) < tan(05.625) =  0.098 >
			if (fSlope >=  0.000f)	return  8; // 0		(00.000 ～ 05.625) < tan(00.000) =  0.000 >
		}
		else // 3
		{
			if (fSlope >= 10.153f)	return 0;
			if (fSlope >=  3.297f)	return 31;
			if (fSlope >=  1.871f)	return 30;
			if (fSlope >=  1.219f)	return 29;
			if (fSlope >=  0.821f)	return 28;
			if (fSlope >=  0.535f)	return 27;
			if (fSlope >=  0.303f)	return 26;
			if (fSlope >=  0.098f)	return 25;
			if (fSlope >=  0.000f)	return 24;		
		}
	}
	else // 2,4
	{
		fSlope = -fSlope;
		if (nDeltaX >= 0) // 4
		{
			if (fSlope >= 10.153f)	return  0;
			if (fSlope >=  3.297f)	return  1;
			if (fSlope >=  1.871f)	return  2;
			if (fSlope >=  1.219f)	return  3;
			if (fSlope >=  0.821f)	return  4;
			if (fSlope >=  0.535f)	return  5;
			if (fSlope >=  0.303f)	return  6;
			if (fSlope >=  0.098f)	return  7;
			if (fSlope >=  0.000f)	return  8;
		}
		else // 2
		{
			if (fSlope >= 10.153f)	return 16;
			if (fSlope >=  3.297f)	return 17;
			if (fSlope >=  1.871f)	return 18;
			if (fSlope >=  1.219f)	return 19;
			if (fSlope >=  0.821f)	return 20;
			if (fSlope >=  0.535f)	return 21;
			if (fSlope >=  0.303f)	return 22;
			if (fSlope >=  0.098f)	return 23;
			if (fSlope >=  0.000f)	return 24;		
		}
	}
	return 0;
}
#endif

#endif // __DIRECTION_H__