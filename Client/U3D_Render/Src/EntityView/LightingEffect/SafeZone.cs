using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using SPELL;

public sealed class SafeZone
{
    public static float StartedSpreadTimeInSecond
    {
        set
        {
            m_StartedSpreadTimeInSecond = value;
            m_currTimes = m_StartedSpreadTimeInSecond;
            m_currTicks = GameLogicAPI.getSyncTickCount();
        }
        get
        {
            return m_StartedSpreadTimeInSecond;
        }
    }

    public static Vector3 currPos
    {
        get
        {
            return m_currPos;
        }
    }

    public static float currRadius
    {
        get
        {
            return m_currRadius;
        }
    }

    public static Vector3 dstCenter
    {
        set
        {
            m_DstCenter = MonsterStateMachine.CorrectPosFromPhysic(value, 500, 500);
        }
        get
        {
            return m_DstCenter;
        }
    }

    public static Vector3 srcCenter
    {
        set
        {
            m_SrcCenter = MonsterStateMachine.CorrectPosFromPhysic(value, 500, 500);
            m_currPos = m_SrcCenter;
        }
        get
        {
            return m_SrcCenter;
        }
    }

    public static bool isSpreadStarted = false;


    public static float dstRadius
    {
        get
        {
            return m_dstRadius;
        }
        set
        {
            m_dstRadius = value;
        }
    }

    public static float srcRadius
    {
        get
        {
            return m_srcRadius;
        }
        set
        {
            m_srcRadius = value;
            m_currRadius = value;
        }
    }

    public static float SpreadTimeInSecond = 0;

    private static float m_dstRadius = 0;
    private static float m_srcRadius = 0;
    private static Vector3 m_DstCenter = Vector3.zero;
    private static Vector3 m_SrcCenter = Vector3.zero;

    private static float m_StartedSpreadTimeInSecond = 0.0f;

    private static Vector3 m_currPos = Vector3.zero;
    private static float m_currRadius = 0.0f;
    private static float m_currTimes = 0.0f;
    private static float m_currTicks = 0.0f;
    public static void Update()
    {
        if(!isSpreadStarted)
        {
            return;
        }

        if(SpreadTimeInSecond < 0.03f)
        {
            m_currPos = dstCenter;
            m_currRadius = dstRadius;
            isSpreadStarted = false;
            return;
        }
        uint tick = GameLogicAPI.getSyncTickCount();
        m_currTimes += (tick - m_currTicks) * 0.001f;
        m_currTicks = tick;
        float t = m_currTimes / SpreadTimeInSecond; ;
        m_currPos = Vector3.Lerp(srcCenter, dstCenter, t);
        m_currRadius = Mathf.Lerp(srcRadius, dstRadius, t);
        if(m_currTimes >= SpreadTimeInSecond)
        {
            m_currPos = dstCenter;
            m_currRadius = dstRadius;
            isSpreadStarted = false;
        }
    }

    public static bool IsPointInSafeZone(Vector3 point)
    {
        float dis = GameUtil.GetSqrDistanceWithOutY(point, SafeZone.dstCenter);
        return dis <= SafeZone.dstRadius * SafeZone.dstRadius;
    }
}
