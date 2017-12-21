/*******************************************************************
** 文件名:	TimerManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-1-11
** 版  本:	1.0
** 描  述:	定时器处理

**************************** 修改记录 ******************************
** 修改人: 林建聪
** 日  期: 2017.8.4
** 描  述: 更改定时器的存储方式，每次新增移除时都将时间最低的排到队头。
1、当前使用的游戏运行时间驱动，如要更改为时间服，直接修改 GetCurrentTickCount()方法。

********************************************************************/
using System;
using UnityEngine;
using U3D_Render.STL;

using TimerID = System.Int32;
using System.Collections.Generic;

/// <summary>
/// 定时器管理
/// </summary>
public class TimerManager
{
    static TimerManager()
    {
        init();
    }

    static public void OnUpdate()
    {
        Update();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private struct TimerIdPair
    {
        public ITimerHanlder Handler;
        public int Id;
        public uint KeyTime;
    }

    private static PriorityQueue<uint, TimerObject> timers;
    private static List<TimerIdPair> activeTimers;
    /// <summary>
    /// 是否在执行OnTimer回调,主要用于防止在OnTimer内移除循环Timer操作而导致的异常。
    /// </summary>
    private static bool m_bIsOnTimer;

    private static void init()
    {
        timers = new PriorityQueue<uint, TimerObject>();
        activeTimers = new List<TimerIdPair>(20);
    }

    public static bool SetTimer(ITimerHanlder handler,int nTimerID,float fElapseTime,int nCallCount = -1)
    {
        return _addTimer(new TimerObject(handler, nTimerID, ToMilliSecond(fElapseTime), nCallCount));
    }

    public static void KillTimer(ITimerHanlder handler,int nTimerID)
    {
        int index = GetActiveTimerIndex(handler, nTimerID);

        //防止在OnTimer内移除循环Timer操作而导致的异常。
        if (m_bIsOnTimer)
        {
            var timer = timers.Top.Value;
            int topTimerIndex = GetActiveTimerIndex(timer.m_handler, timer.m_timerID);
            if (index == topTimerIndex)
            {//执行次数设为1在OnTimer执行后移除
                timer.m_timerCallCount = 1;
                return;
            }
        }

        if (index > -1)
        {

            uint priority = activeTimers[index].KeyTime;
            activeTimers.RemoveAt(index);

            timers.RemoveAtPriority(priority, 
                (t => t.m_handler == handler && t.m_timerID == nTimerID));
        }
    }

    /// <summary>
    /// 获取当前时间(ms)
    /// </summary>
    /// <returns></returns>
    public static uint GetCurrentTickCount()
    {
        return GameLogicAPI.GetUnityTickCount();
    }

    /// <summary>
    /// 添加定时器
    /// </summary>
    /// <param name="_timerObj"></param>
    /// <returns></returns>
    private static bool _addTimer(TimerObject _timerObj)
    {
       // Initialize.PrintTickCheck("new Timer AddTimer begin", _bStart: true);

        int index = GetActiveTimerIndex(_timerObj.m_handler, _timerObj.m_timerID);

        if (index > -1)
            return false;

        //转毫秒
        uint keyTime = GetCurrentTickCount() + _timerObj.m_elapseTime;
        //添加到队列
        timers.Push(keyTime, _timerObj);

        //添加激活列表
        activeTimers.Add(new TimerIdPair() { Handler = _timerObj.m_handler,Id=_timerObj.m_timerID,KeyTime=keyTime});
        //Initialize.PrintTickCheck("new Timer AddTimer end", 0);

        return true;
    }

    private static uint ToMilliSecond(float time)
    {
        return (uint)(time * 1000);
    }

    /// <summary>
    /// 从活动定时器列表中获取指定定时器的序号
    /// </summary>
    /// <param name="handler"></param>
    /// <param name="timerId"></param>
    /// <returns>-1：没在活动列表里</returns>
    private static int GetActiveTimerIndex(ITimerHanlder handler ,int timerId)
    {
        for (int i = 0; i < activeTimers.Count; ++i)
        {
            if (activeTimers[i].Handler == handler && activeTimers[i].Id == timerId)
            {
                return i;
            }
        }

        return -1;
    }

    /// <summary>
    /// 当前时间是否>=定时器时间
    /// </summary>
    /// <param name="key"></param>
    /// <returns></returns>
    private static bool HasKeyTimePassed(uint key)
    {
        return key <= GetCurrentTickCount();
    }

    /// <summary>
    /// 移除指定的定时器
    /// </summary>
    /// <param name="handler"></param>
    /// <param name="timerId"></param>
    /// <returns></returns>
    private static bool RemoveActiveTimer(ITimerHanlder handler,int timerId)
    {
        int index = GetActiveTimerIndex(handler,timerId);
        if (index > -1)
        {
            activeTimers.RemoveAt(index);
            return true;
        }

        return false;
    }

    private static void Update()
    {
        while (timers.Count > 0 && HasKeyTimePassed(timers.Top.Key))
        {
            var timer = timers.Top.Value;

            //避免在OnTimer里面关闭定时器出问题。
            m_bIsOnTimer = true;
            timer.m_handler.OnTimer(timer.m_timerID);
            m_bIsOnTimer = false;

            RemoveActiveTimer(timer.m_handler, timer.m_timerID);

            timers.Pop();

            //处理执行次数的定时器
            if (timer.m_timerCallCount > 0)
            {
                --timer.m_timerCallCount;
                if (timer.m_timerCallCount != 0)
                {
                    _addTimer(timer);
                }
            }
            //处理循环定时器
            else if (timer.m_timerCallCount == -1)
            {
                _addTimer(timer);
            }
        }
    }
}
