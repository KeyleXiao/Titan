/*******************************************************************
** 文件名:	Vector3.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	
** 修  改:	新增向量积和数量积等函数		彭璐		11/14/2014
**			注意！！！为保证Vector3可以作为原始的struct使用，此对象千万不能有虚函数
**			后续可以在程序启动的时候增加校验，如果有虚函数直接挂起
********************************************************************/

#pragma once
#include <math.h>
#include "Api.h"
#include <vector>

using std::vector;

// 常量PI
const float fPI=3.1415926f;

/* 检测点到线段距离
   x,y		检测点
   x1,y1	线段起点
   x2,y2	线段终点
*/
inline double PointToSegDist(double x, double y, double x1, double y1, double x2, double y2)
{
	double cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
	if (cross <= 0)
		return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));

	double d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	if (cross >= d2)
		return sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));

	double r = cross / d2;
	double px = x1 + (x2 - x1) * r;
	double py = y1 + (y2 - y1) * r;
	return sqrt((x-px)*(x-px)+(py-y)*(py-y));
}

/* 检测点是否在扇形里面
   cx,cy	扇形起点
   ux,uy	扇形正前面点
   squaredR 扇形半径平方值
   cosTheta 扇形一半角度cos值
   px,py	检测点
*/
inline bool IsPointInSector(
	float cx, float cy, float ux, float uy, float squaredR, float cosTheta,
	float px, float py)
{
	if (cosTheta <= -1 || cosTheta >= 1)
		return false;

	// 半径平方为0
	if (squaredR <= 0.0f)
		return false;

	// D = P - C
	float dx = px - cx;
	float dy = py - cy;

	// |D|^2 = (dx^2 + dy^2)
	float squaredLength = dx * dx + dy * dy;

	// |D|^2 > r^2
	if (squaredLength > squaredR)
		return false;

	// |D|
	float length = sqrt(squaredLength);

	// D dot U > |D| cos(theta)
	return dx * ux + dy * uy > length * cosTheta;
}

inline bool equalto0(float f)
{
	return f < 0.0001f && f > -0.0001f;
}

template <typename T>
void makesure_minmax(T& v, const T vmin, const T vmax)
{
	if (v < vmin) v = vmin;
	if (v > vmax) v = vmax;
}

// 三维向量类
class Vector3
{
public:
	Vector3(float v1,float v2,float v3 ) : x(v1),y(v2),z(v3){}
	Vector3() : x(0),y(0),z(0){} 
	Vector3(float* v) : x(v[0]),y(v[1]),z(v[2]){}
	Vector3(int* v) : x((float)v[0]),y((float)v[1]),z((float)v[2]){}

	bool checkDistance(const Vector3 & loc,float radii) const
	{
		Vector3 vDistance = loc - *this;
		return vDistance.getLengthSqr() < radii*radii;
	}

	bool checkDistance(const Vector3 & loc,float radii, float fHeight) const
	{
		// 大于高度
		if (abs(loc.y - y) > fHeight)
		{
			return false;
		}
		Vector3 vDistance = loc - *this;
		return vDistance.getLengthSqr() < radii*radii;
	}

	// 检测是否在直线上点
	bool checkLineDistance(const Vector3 & targetLoc, const Vector3 & dir, float radii, float fWith, float fHeight) const
	{
		// 起点位置
		Vector3 start = Vector3(x,y,z);
		// 终点向量
		Vector3 end = start + dir.normalize()*radii;

		float sx = start[0];
		float sz = start[2];
		float ex = end[0];
		float ez = end[2];
		float tx = targetLoc[0];
		float tz = targetLoc[2];

		// 排除首部半圆
		float fBeginValue = (targetLoc - start)*dir;
		//dir.x*(targetLoc.x - start.x) + dir.z*(targetLoc.z - start.z);
		if (fBeginValue < 0)
		{
			return false;
		}
		// 排除末尾半圆
		float fEndValue = (Vector3(0,0,0)-dir)*(targetLoc - end);
		//-dir.x*(targetLoc.x - end.x) + -dir.z*(targetLoc.z - end.z);
		if (fEndValue < 0)
		{
			return false;
		}

		// 检测点到平面高度
		if (start.PointToPlaneDistance(dir, targetLoc) > fHeight)
		{
			return false;
		}

		double dDis = PointToSegDist(tx, tz, sx, sz, ex, ez);
		if (dDis < fWith + 0.0001f)
		{
			return true;
		}

		return false;
	}

	// 检测点是否在扇形内
	bool checkPointInSector(const Vector3 & targetLoc, const Vector3 & dir, float squaredR, float cosTheta, float fHeight) const
	{
		// 起点位置
		Vector3 start = Vector3(x,y,z);

		// 检测点到平面距离
		if (start.PointToPlaneDistance(dir, targetLoc) > fHeight)
		{
			return false;
		}

		float sx = start[0];
		float sz = start[2];
		float ex = dir[0];
		float ez = dir[2];
		float tx = targetLoc[0];
		float tz = targetLoc[2];
		if (IsPointInSector(sx, sz, ex, ez, squaredR, cosTheta, tx, tz))
		{
			return true;
		}

		return false;
	}


	// 向量单位化
	Vector3 normalize() const
	{
		float length = getLength();
		if ( equalto0(length) )
			return *this;
		return Vector3(x/length, y/length, z/length);
	}

    // 方向向量转化为欧拉角
    Vector3 toEulerAngles() const
    {
        float fAngle = atan2(x, z);  // atan2和atan 值域不一样
        if(fAngle < 0)
        {
            fAngle += (2 * fPI);
        }

//         if(fAngle > 0)
//         {
//             if(x < 0)
//             {
//                 fAngle += fPI;
//             }
//         }
//         else
//         {
//             if(x > 0)
//             {
//                 fAngle += fPI;
//             }
//             else
//             {
//                 fAngle += (2 * fPI);
//             }
//         }

        return Vector3(0, fAngle * 180 / fPI, 0);
    }

	float& operator[](int i)
	{
		static float f = 0.0f;
		switch (i)
		{
		case 0:		return x;
		case 1:		return y;
		case 2:		return z;
		default:	return f;
		}
	}

	float operator[](int i) const
	{
		return (*const_cast<Vector3*>(this))[i];
	}

	// 求向量的长度的平方
	inline float getLengthSqr() const
	{
		return x*x+y*y+z*z;
	}

	// 求向量的长度
	inline float getLength() const
	{
		return pow(x*x+y*y+z*z, 0.5f);
	}

	// 返回一个向量到另一个向量的距离
	inline float getDistance(const Vector3& v) const
	{
        return Vector3(x-v.x, y-v.y, z-v.z).getLength();
	}

	// 取得向量积
	inline Vector3 getCrossProduct(const Vector3& v) const
	{
		return Vector3(y*v.z-z*v.y,z*v.x-x*v.z, x*v.y-y*v.x);
	}

	// 求和
	Vector3 operator+ (const Vector3& v) const
	{
		return Vector3(x+v.x, y+v.y, z+v.z);
	}
	// 求差
	Vector3 operator- (const Vector3& v) const
	{
		return operator+(v*(-1.0f));
	}
	// 求倍数
	Vector3 operator* (float f) const
	{
		return Vector3(x*f, y*f, z*f);
	}

	Vector3& operator+= (const Vector3& v)
	{
		return (*this = operator+(v));
	}
	Vector3& operator-= (const Vector3& v)
	{
		return (*this = operator-(v));
	}
	Vector3& operator*= (float f)
	{
		return (*this = operator*(f));
	}
    bool operator == (const Vector3& v)
    {
        return equalto0(x-v.x) && equalto0(y-v.y) && equalto0(z-v.z); 
    }
    bool operator != (const Vector3& v)
    {
        return !equalto0(x-v.x) || !equalto0(y-v.y) || !equalto0(z-v.z);
    }

	// 求向量的数量积
	inline float operator* (const Vector3& v) const
	{
		return x*v.x + y*v.y + z*v.z;
	}

	// 到v3End的直线 最近距离fMinDis放入stCount个半径fRadius的obj 的坐标
	void getLinePos(const Vector3& v3End, size_t stCount, float fRadius, float fMinDist, vector<Vector3>& vecRet)
	{
		if (0 == stCount)
		{
			vecRet.clear();
			return;
		}
		vecRet.resize( stCount );

		float fDiameter = 2 * fRadius;
		Vector3 v3BackLine = (v3End - *this).normalize();
		vecRet[0] = v3BackLine * fMinDist + *this;

		//TraceLn("v3Start x=" << this->x << " y=" << this->y << " z=" << this->z);
		//TraceLn("v3End x=" << v3End.x << " y=" << v3End.y << " z=" << v3End.z);
		//TraceLn("v3BackLine x=" << v3BackLine.x << " y=" << v3BackLine.y << " z=" << v3BackLine.z);
		//TraceLn("vecRet[0] x=" << vecRet[0].x << " y=" << vecRet[0].y << " z=" << vecRet[0].z);

		v3BackLine *= fDiameter;
		for (size_t i = 1; i < stCount; ++i)
		{
			vecRet[i] = vecRet[i - 1] + v3BackLine;
		}
	}
	// 获取自己为圆心到v3End的扇形可放置点 忽略y（用自己的Y）
	void getSectorPosXZ(Vector3 v3End, size_t stCount, float fRadius, float fMinDist, vector<Vector3>& vecRet)
	{
		vecRet.clear();
		if (0 == stCount)
		{
			return;
		}
		vecRet.reserve( stCount + 1 );

		float fDiameter = 2 * fRadius;
		float fDist = fMinDist < fDiameter ? fDiameter : fMinDist;

		Vector3 v3TempLine(1, 0, 0);
		while (vecRet.size() < stCount)
		{
			// 切线长度
			float fQieLineLen = sqrt(fDist * fDist - fRadius * fRadius);
			float fCos = fQieLineLen / fDist;
			float fSin = fRadius / fDist;
			float fSin2 = 2.0f * fSin * fCos;
			float fCos2 = 1.0f - (2.0f * (fSin * fSin));

			Vector3 v3Center = v3TempLine * fDist;
			vecRet.push_back( v3Center );
			//TraceLn("vecRet[" << vecRet.size() - 1 << "] x=" << vecRet.back().x << " z=" << vecRet.back().z );
			Vector3* pv3Last = &v3Center;

			while(vecRet.size() < stCount)
			{
				vecRet.push_back( Vector3(pv3Last->x * fCos2 - pv3Last->z * fSin2, 0, pv3Last->x * fSin2 + pv3Last->z * fCos2) );
				//TraceLn("vecRet[" << vecRet.size() - 1 << "] x=" << vecRet.back().x << " z=" << vecRet.back().z );
				pv3Last = &vecRet.back();

				vecRet.push_back( Vector3(pv3Last->x, 0, -pv3Last->z) );
				//TraceLn("vecRet[" << vecRet.size() - 1 << "] x=" << vecRet.back().x << " z=" << vecRet.back().z );

				if (pv3Last->x < fRadius)
				{
					break;
				}
			}

			// 往外找
			fDist += fDiameter;
		}

		{
			Vector3 v3Start = *this;
			v3Start.y = 0;
			v3End.y = 0;
			//TraceLn("v3Start x=" << v3Start.x << " z=" << v3Start.z );
			//TraceLn("v3End x=" << v3End.x << " z=" << v3End.z );
			Vector3 v3BackLine = (v3End - v3Start);
			//TraceLn("v3BackLine x=" << v3BackLine.x << " z=" << v3BackLine.z );
			float fCos = v3BackLine.x / v3BackLine.getLength();
			float fSin = v3BackLine.z / v3BackLine.getLength();
			//TraceLn("fCos =" << fCos);
			//TraceLn("fSin =" << fSin);
			//TraceLn("v3BackLine.getLength() =" << v3BackLine.getLength());
			v3Start.y = this->y; // 临时 这要通过地图找
			for (size_t i = 0; i < stCount; ++i)
			{
				Vector3 v3Temp(vecRet[i].x * fCos - vecRet[i].z * fSin, 0, vecRet[i].x * fSin + vecRet[i].z * fCos);
				//TraceLn("v3Temp x=" << v3Temp.x << " z=" << v3Temp.z );
				vecRet[i] = v3Temp + v3Start;
				//TraceLn("vecRet[" << i << "] x=" << vecRet[i].x << " z=" << vecRet[i].z );
			}
		}
	}

	/* 检测点到平面距离
	   vDir	    方向
	   vPoint   检测点
	   返回点到平面距离
	*/
	float PointToPlaneDistance(Vector3 vDir, Vector3 vPoint)
	{
		// 起点
		Vector3 vStart(x,y,z);
		vDir = vDir.normalize();
		Vector3 vPlaneNormal(0,1,0);
		// 如果是朝上打的时候，特殊处理下
		if (equalto0(vPlaneNormal.x-vDir.x) && equalto0(vPlaneNormal.y-vDir.y) && equalto0(vPlaneNormal.z-vDir.z))
		{
			// 设置前向量
			vPlaneNormal = Vector3(0,0,1);
		}
		// a × b= [a2b3-a3b2,a3b1-a1b3, a1b2-a2b1]
		Vector3 vPlaneDir(vDir.y*vPlaneNormal.z-vDir.z*vPlaneNormal.y, vDir.z*vPlaneNormal.x-vDir.x*vPlaneNormal.z, vDir.x*vPlaneNormal.y-vDir.y*vPlaneNormal.x);

		// 法向量
		Vector3 vNormal(vDir.y*vPlaneDir.z-vDir.z*vPlaneDir.y, vDir.z*vPlaneDir.x-vDir.x*vPlaneDir.z, vDir.x*vPlaneDir.y-vDir.y*vPlaneDir.x);

		vNormal = vNormal.normalize();
		//// AX+BY+CZ+D=0,求出D
		//float fValue = vNormal.x*vStart.x + vNormal.y*vStart.y + vNormal.z*vStart.z;
		//// 求出点到平面距离
		//float fDistance = vNormal.x*vPoint.x + vNormal.y*vPoint.y + vNormal.z*vPoint.z - fValue;
		float fDistance = abs(vNormal.x*(vPoint.x-vStart.x) + vNormal.y*(vPoint.y-vStart.y) + vNormal.z*(vPoint.z-vStart.z)); 

		return fDistance;
	}

public:
	float x,y,z;
};

 


// 实体移动三元组
struct Move3
{
	Vector3	vPosition;		// 位置
	Vector3	vRotation;		// 角度
	Vector3	vVelocity;		// 角色速度(目前暂时用像素/1000ms来表示, 后续可能改成像素/帧)
};

// 记录实体的运动状态
struct MoveContext
{
	unsigned long	lTicks;// 此状态更新时的tick数
	Move3			move;

	MoveContext()
	{
		lTicks = 0;
	}

	MoveContext(const Move3& m, unsigned long t)
		:move(m), lTicks(t) {}

	inline bool isValid()
	{
		return (lTicks != 0);
	}

	void update(Move3& pos)
	{
		lTicks = rkt::getTickCountEx();
		move = pos;
	}

	// 到指定时间点时，根据速度等属性更新位置
	void timeElapse(unsigned long lCurTicks = 0)
	{
		if ( lCurTicks==0 )
		{
			lCurTicks = rkt::getTickCountEx();
		}
		if ( lCurTicks>lTicks && lTicks!=0 )
		{
			float fElapse = (lCurTicks - lTicks)/1000.0f;
			move.vPosition += move.vVelocity*fElapse;
			lTicks = lCurTicks;
		}
	}
};

#define INVALID_LOC (Vector3(2222, 2222, 2222))

