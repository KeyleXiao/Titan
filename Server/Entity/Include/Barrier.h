// 描述：结界对象

#pragma once

#include "Vector3.h"
#include "GameDef.h"
#include "EffectCommon_ManagedDef.h"

struct Barrier
{
    // 阻挡类型
    BarrierBlockType   m_eBlockType;

    // 创建者
    UID                 m_uidCreater;

    // 序列号
    unsigned long       m_dwSerial; 

    // 检测碰撞
    virtual bool hitTest(Vector3 locFrom, Vector3 locTo) = 0;

    // 获取结界形状
    virtual int getShape() = 0;

    // 释放
    virtual void release() = 0;
};

// 圆环结界
class Barrier_Circle : public Barrier
{
    friend class AttackObject_Barrier;
protected:
    // 圆心
    Vector3             m_ptCenter;

    // 内径
    float               m_fInnerRadius;

    // 外径
    float               m_fOutterRadius;

public:
    static Barrier* create(BarrierBlockType eBlockType, UID uidCreater, unsigned long dwSerial, Vector3 ptCenter, float fInnerRadius, float fOutterRadius)
    {
        return new Barrier_Circle(eBlockType, uidCreater, dwSerial, ptCenter, fInnerRadius, fOutterRadius);
    }

    virtual bool hitTest(Vector3 locFrom, Vector3 locTo)
    {
        float fDistanceOld = (locFrom - m_ptCenter).getLength(); 
        float fDistanceNew = (locTo - m_ptCenter).getLength();

        float fFar = max(fDistanceOld, fDistanceNew);
        float fNear = min(fDistanceOld, fDistanceNew);

        // 都在圆环外
        if(fNear > m_fOutterRadius)
        {
            return false;
        }

        // 都在圆环内
        if(fFar < m_fInnerRadius)
        {
            return false;
        }

        // 撞到
        return true;
    }

    virtual int getShape()
    {
        return BARRIER_SHAPE_CIRCLE; 
    }

    virtual void release()
    {
        delete this;
    }

private:
    Barrier_Circle(BarrierBlockType eBlockType, UID uidCreater, unsigned long dwSerial, Vector3 ptCenter, float fInnerRadius, float fOutterRadius)
        : m_ptCenter(ptCenter), m_fInnerRadius(fInnerRadius), m_fOutterRadius(fOutterRadius)
    {
        m_eBlockType = eBlockType;
        m_uidCreater = uidCreater;
        m_dwSerial = dwSerial;
    }
};

// 圆柱体障碍
class Barrier_Cylinder : public Barrier
{
    friend class AttackObject_Barrier;

protected:
    // 圆心
    Vector3             m_ptCenter;

    // 半径
    float               m_fRadius;

    // 高度
    float               m_fHeight;

public:
    static Barrier* create(BarrierBlockType eBlockType, UID uidCreater, unsigned long dwSerial, Vector3 ptCenter, float fRadius, float fHeight)
    {
        return new Barrier_Cylinder(eBlockType, uidCreater, dwSerial, ptCenter, fRadius, fHeight);
    }

    virtual bool hitTest(Vector3 locFrom, Vector3 locTo)
    {
        return isInCylinder(locTo) && !isInCylinder(locFrom);
    }

    virtual int getShape()
    {
        return BARRIER_SHAPE_CIRCLE; 
    }

    virtual void release()
    {
        delete this;
    }

private:
    Barrier_Cylinder(BarrierBlockType eBlockType, UID uidCreater, unsigned long dwSerial, Vector3 ptCenter, float fRadius, float fHeight)
        : m_ptCenter(ptCenter), m_fRadius(fRadius), m_fHeight(fHeight)
    {
        m_eBlockType = eBlockType;
        m_uidCreater = uidCreater;
        m_dwSerial = dwSerial;
    }

    bool isInCylinder(Vector3 loc)
    {
        Vector3 loc1 = loc;
        loc1.y = 0;
        Vector3 locCenter = m_ptCenter;
        locCenter.y = 0;

        return loc1.checkDistance(locCenter, m_fRadius) && (loc.y + 0.5 >= m_ptCenter.y && loc.y + 0.5 <= m_ptCenter.y + m_fHeight); 
    }
};

// 直线结界
class Barrier_Line : public Barrier
{
    friend class AttackObject_Barrier;
protected:
    Vector3         m_ptStart; // 起点
    Vector3         m_ptEnd;   // 终点
    float           m_fWidth;  // 宽度
    float           m_fHeight; // 高度

public:
    static Barrier* create(BarrierBlockType eBlockType, UID uidCreater, unsigned long dwSerial, Vector3 ptStart, Vector3 ptEnd, float fWidth, float fHeight)
    {
        return new Barrier_Line(eBlockType, uidCreater, dwSerial, ptStart, ptEnd, fWidth, fHeight);
    }

    virtual bool hitTest(Vector3 locFrom, Vector3 locTo)
    {
        Vector3 dir = m_ptEnd - m_ptStart;
        float fLength = dir.getLength();
       

        // 线段和长方体是否相交 todo.

        // 暂时逐步检测是否在长方体内
        float fLen = (locTo - locFrom).getLength();
        Vector3 step = (locTo - locFrom) * (1 / fLen);
        for(int i = 0; i < fLen; ++i)
        {
            Vector3 ptCheck = locFrom + step * i;
            if(m_ptStart.checkLineDistance(ptCheck, dir, fLength, m_fWidth/2, m_fHeight))
            {
                return true;
            }
        }

        return false;
    }

    virtual int getShape()
    {
        return BARRIER_SHAPE_LINE; 
    }

    virtual void release()
    {
        delete this;
    }

private:
    Barrier_Line(BarrierBlockType eBlcokType, UID uidCreater, unsigned long dwSerial, Vector3 ptStart, Vector3 ptEnd, float fWidth, float fHeight)
        : m_ptStart(ptStart), m_ptEnd(ptEnd), m_fWidth(fWidth), m_fHeight(fHeight)
    {
        m_eBlockType = eBlcokType;
        m_uidCreater = uidCreater;
        m_dwSerial = dwSerial;
    }
};