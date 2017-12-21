// ������������

#pragma once

#include "Vector3.h"
#include "GameDef.h"
#include "EffectCommon_ManagedDef.h"

struct Barrier
{
    // �赲����
    BarrierBlockType   m_eBlockType;

    // ������
    UID                 m_uidCreater;

    // ���к�
    unsigned long       m_dwSerial; 

    // �����ײ
    virtual bool hitTest(Vector3 locFrom, Vector3 locTo) = 0;

    // ��ȡ�����״
    virtual int getShape() = 0;

    // �ͷ�
    virtual void release() = 0;
};

// Բ�����
class Barrier_Circle : public Barrier
{
    friend class AttackObject_Barrier;
protected:
    // Բ��
    Vector3             m_ptCenter;

    // �ھ�
    float               m_fInnerRadius;

    // �⾶
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

        // ����Բ����
        if(fNear > m_fOutterRadius)
        {
            return false;
        }

        // ����Բ����
        if(fFar < m_fInnerRadius)
        {
            return false;
        }

        // ײ��
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

// Բ�����ϰ�
class Barrier_Cylinder : public Barrier
{
    friend class AttackObject_Barrier;

protected:
    // Բ��
    Vector3             m_ptCenter;

    // �뾶
    float               m_fRadius;

    // �߶�
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

// ֱ�߽��
class Barrier_Line : public Barrier
{
    friend class AttackObject_Barrier;
protected:
    Vector3         m_ptStart; // ���
    Vector3         m_ptEnd;   // �յ�
    float           m_fWidth;  // ���
    float           m_fHeight; // �߶�

public:
    static Barrier* create(BarrierBlockType eBlockType, UID uidCreater, unsigned long dwSerial, Vector3 ptStart, Vector3 ptEnd, float fWidth, float fHeight)
    {
        return new Barrier_Line(eBlockType, uidCreater, dwSerial, ptStart, ptEnd, fWidth, fHeight);
    }

    virtual bool hitTest(Vector3 locFrom, Vector3 locTo)
    {
        Vector3 dir = m_ptEnd - m_ptStart;
        float fLength = dir.getLength();
       

        // �߶κͳ������Ƿ��ཻ todo.

        // ��ʱ�𲽼���Ƿ��ڳ�������
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