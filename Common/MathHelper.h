/*******************************************************************
** 文件名:	MathHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/11/2
** 版  本:	1.0
** 描  述:	数学辅助算法类
********************************************************************/

#pragma once

#include <math.h>

#define PI 3.1415926

// 随机范围[0,1]
#define RAND0_1 ((float)mt_range_rand(0, 1000) / 1000.f)

// 圆圈
struct Cicle
{
	Vector3 center;
	float radius;

	Cicle()
	{
		center.x = center.y = center.z = 0;
		radius = 0.f;
	}

	Cicle(Vector3 c, float r)
	{
		center = c;
		radius = r;
	}

	Cicle& operator=(Cicle& c)
	{
		center = c.center;
		radius = c.radius;

		return *this;
	}
};

class CMathHelper
{
public:
	// 大圆内随机一个小圆
	static Vector3 getRandomCicleOfCicle(const Cicle & bigCicle, float radiusOfSmallCicle)
	{
		// 小圆需要在大圆内,且边缘也不能超过大圆
		// 当前大圆圆点cN
		// 当前大圆半径rN
		// 小圆半径rS
		// 小圆圆点只能在圈T中随机一个点，其中T的半径为rT = (rN-rS),T的圆心cT = cN
		// 小圆圆点cS
		// 以cT为原点坐标系，则cS到cT的距离 l = randrange(0, rT) 
		// 再求cS的角度 d = randrange(0, 360)
		// cS.x = CN.x + l * cos(d)
		// cS.y = CN.y + 0
		// cS.z = CN.z + l * sin(d)

		const Vector3& cN = bigCicle.center;
		float rN = bigCicle.radius;
		float rS = radiusOfSmallCicle;

		float rT = (rN - rS);
		float l = mt_range_rand(0, rT);
		float d = mt_range_rand(0, 360);
		float x = l * cos(d * PI / 180);
		float y = 0.f;
		float z = l * sin(d * PI / 180);

		return Vector3(cN.x + x, cN.y + y, cN.z + z);
	}

	// 圆内随机一个点
	static Vector3 getRandomPosOfCicle(const Cicle & cicle, float distance = 0.f)
	{
		const Vector3& cN = cicle.center;
		float rN = cicle.radius - distance;

		float rT = rN;
		float l = mt_range_rand(0, rT);
		float d = mt_range_rand(0, 360);
		float x = l * cos(d * PI / 180);
		float y = 0.f;
		float z = l * sin(d * PI / 180);

		return Vector3(cN.x + x, cN.y + y, cN.z + z);
	}

	// 圆边上随机一个点
	static std::vector<Vector3> getPosOfCicleEdge(const Cicle & cicle, int nPosNum)
	{
		vector<Vector3> result;

		if (nPosNum <= 0)
			return result;

		const Vector3& cT = cicle.center;
		float rT = cicle.radius;

		float fAvgDgree = 360.f / nPosNum;
		float fCurDgree = 0.f;
		for (int i = 0; i < nPosNum; ++i)
		{
			float l = rT;
			float d = fCurDgree;
			float x = l * cos(d * PI / 180);
			float y = 0.f;
			float z = l * sin(d * PI / 180);

			result.push_back(Vector3(cT.x + x, cT.y + y, cT.z + z));

			fCurDgree += fAvgDgree;
			if (fCurDgree > 360)
			{
				fCurDgree -= 360;
			}
		}

		return result;
	}

	// 圆内随机nPosNum个点,均匀分布
	static std::vector<Vector3> getRandomPosOfCicleOnAvg(const Cicle & cicle, int nPosNum)
	{
		/*
		本函数需要传入的参数：起始点坐标Po（Xo, Zo)、半径Do、刷新数量N
		输出结果为Ｎ个坐标点P(N, 2)

		1、禁刷半径Dx = Do*0.25
		2、根据数量将刷新区域分成Ｎ等份，起始射线角度为(0~360 / Ｎ)中随机得出，其余角度在这个基础上加n - 1个360 / Ｎ
		刷新射线角度　A(n) = RAND()*(360 / N) + (360 / N)*(n - 1)
		3、根据刷新射线的角度计算出各点坐标P(N, 2)：
		随机出第n点的刷新半径：D(n) = RAND()*(Do - Dx) + Dx
		X坐标：P(n, 1) = Xo + D(n)*COS(A(n))
		Z坐标: P(n, 2) = Zo + D(n)*SIN(A(n))

		注：RAND()生成一个0~1的随机数；PI()圆周率
		*/

		vector<Vector3> result;

		// 传入参数
		Vector3 P0(cicle.center.x, cicle.center.y, cicle.center.z);
		float D0 = cicle.radius;
		int N = nPosNum;

		// 禁刷半径
		float Dx = D0 * 0.25;

		// 刷新射线角度
		float RAND = 0.f;
		for (int n = 1; n <= N; ++n)
		{
			// 随机[0,1]
			RAND = RAND0_1;

			float An = RAND * (360 / N) + (360 / N) * (n - 1);

			// 随机[0,1]
			RAND = RAND0_1;

			float Dn = RAND * (D0 - Dx) + Dx;

			float x = P0.x + Dn*cos(An * PI / 180);
			float y = P0.y;
			float z = P0.z + Dn*sin(An * PI / 180);

			result.push_back(Vector3(x, y, z));
		}

		return result;
	}

	// 圆内随机nPosNum个点,且点之间保持至少distance的间隔
	// 动态半径为盲区
	static std::vector<Vector3> getRandomPosOfCicleFixNum(const Cicle & cicle, float distance, int nPosNum)
	{
		vector<Vector3> result;

		if (nPosNum <= 0)
			return result;

		float d1 = 360 / nPosNum;
		float l1 = (distance / 2) / sin((d1 / 2.f) * PI / 180);

		const Vector3& cT = cicle.center;
		float rT = cicle.radius;

		if (l1 > rT)
		{
			ErrorLn(__FUNCTION__":monster num or radius is invalid. l1 > rT,l1=" << l1);
			l1 = rT;
		}

		float fCurDgree = mt_range_rand(0, 360);
		for (int i = 0; i < nPosNum; ++i)
		{
			float l = mt_range_rand(l1, rT);
			float d = fCurDgree;
			float x = l * cos(d * PI / 180);
			float y = 0.f;
			float z = l * sin(d * PI / 180);

			result.push_back(Vector3(cT.x + x, cT.y + y, cT.z + z));

			WarningLn(__FUNCTION__":" << d << "," << l);

			fCurDgree += d1;
			if (fCurDgree > 360)
			{
				fCurDgree -= 360;
			}
		}

		return result;
	}

	// 圆内随机nPosNum个点,且点之间保持至少distance的间隔
	// 在圆内画小圆，几乎没有盲区
	static std::vector<Vector3> getRandomPosOfCicleFixNumEx(const Cicle & cicle, float distance, int nPosNum)
	{
		// 核心原理：
		// 1.把大圆切割成N个半径为distance/2的小圆
		// 2.把生成的小圆圆心绕大圆旋转d度，d为随机角度
		// 3.在小圆内随机取一个点p
		// 4.在N个小圆中随机选取nPosNum个p点返回结果

		vector<Vector3> smallCenterList;

		// 小圆心半径
		float l = distance / 2.f;

		// 生成4个象限的单位向量
		vector<Vector3> vList;
		vList.push_back(Vector3(1.f, 0, 1.f));
		vList.push_back(Vector3(-1.f, 0, 1.f));
		vList.push_back(Vector3(-1.f, 0, -1.f));
		vList.push_back(Vector3(1.f, 0, -1.f));

		// 角度
		float deg = RAND0_1 * 360.f;
		float rad = deg * PI / 180.f;

		const Vector3& center = cicle.center;
		const float& radius = cicle.radius;

		// 生成所有的小圆
		for(vector<Vector3>::iterator vIt = vList.begin(); vIt != vList.end(); ++vIt)
		{
			Vector3& v = *vIt;
			float x = center.x + l * v.x;
			while (true)
			{
				Vector3 tmpLoc1(x, center.y, center.z);
				if(tmpLoc1.checkDistance(center, radius - l) == false)
				{
					break;
				}

				float z = center.z + l * v.z;
				while (true)
				{
					Vector3 tmpLoc2(x, center.y, z);
					if (tmpLoc2.checkDistance(center, radius - l) == false)
					{
						break;
					}

					// 饶某一点旋转a度
					// x0 = (x - rx0) * cos(a) - (y - ry0) * sin(a) + rx0
					// y0 = (x - rx0) * sin(a) + (y - ry0) * cos(a) + ry0;
					float xx = (tmpLoc2.x - center.x) * (float)cos(rad) - (tmpLoc2.z - center.z) * sin(rad) + center.x;
					float zz = (tmpLoc2.x - center.x) * (float)sin(rad) + (tmpLoc2.z - center.z) * cos(rad) + center.z;

					smallCenterList.push_back(Vector3(xx, center.y, zz));

					z += 2 * l * v.z;
				}

				x += 2 * l * v.x;
			}
		}

		// 生成N个圆后，随机取圆内某个点
		double d2 = RAND0_1 * 360.f;
		double l2 = RAND0_1 * l;

		// 随机选取nPosNum个点
		vector<Vector3> result;
		for (int i = 0; i < nPosNum; ++i)
		{
			if (smallCenterList.empty())
			{
				break;
			}

			int index = mt_range_rand(0, smallCenterList.size() - 1);
			Vector3 tmpLoc = smallCenterList[index];

			float x = l2 * cos(PI * ((float)d2 / 180));
			float y = 0.f;
			float z = l2 * sin(PI * ((float)d2 / 180));

			result.push_back(Vector3(tmpLoc.x + x, tmpLoc.y + y, tmpLoc.z + z));

			smallCenterList.erase(smallCenterList.begin() + index);
		}

		return result;
	}
};