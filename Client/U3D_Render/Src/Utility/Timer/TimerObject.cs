/*******************************************************************
** 文件名:	TimerManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-1-11
** 版  本:	1.0
** 描  述:	定时器对象

		
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

using TimerID = System.Int32;

public class TimerObject
{
    public int m_timerID;
    public uint m_elapseTime;
    public float m_lasttime;
    public int m_timerCallCount;//定时器执行次数
    public ITimerHanlder m_handler;

    public TimerObject(ITimerHanlder handler, int nTimerID, uint nElapseTime, int nCallCount = -1)
    {
        m_handler = handler;
        m_timerID = nTimerID;
        m_elapseTime = nElapseTime;
        m_lasttime = Time.time;
        m_timerCallCount = nCallCount;
    }

    /// <summary>
    /// 执行定时器回调操作
    /// </summary>
    //public void OnUpdate()
    //{
        //if (Time.time > m_lasttime + m_elapseTime)
        //{
        //    m_lasttime = Time.time;

        //    // 执行定时器回调
        //    m_handler.OnTimer(m_timerID);
        //    // 不等于0的意味着这个定时器非永久性，一定次数后将自行移除
        //    if (m_timerCallCount > 0)
        //    {
        //        --m_timerCallCount;
        //        if (m_timerCallCount == 0)
        //        {
        //            TimerManager.KillTimer(m_handler, m_timerID);
        //        }
        //    }
        //}
   // }
}


//public class TimerObjectCollect
//{
//    public List<TimerObject> collections;

//    public TimerObjectCollect()
//    {
//        collections = new List<TimerObject>();
//    }

//    public void Clear()
//    {
//        collections.Clear();
//    }

//    public bool Register(ITimerHanlder handler, int nTimerID, float fElapseTime, int nCallCount = 0)
//    {
//        for (int i = 0; i < collections.Count; ++i)
//        {
//            if (collections[i].m_timerID == nTimerID)
//                return false;
//        }
//        collections.Add(new TimerObject(handler, nTimerID, fElapseTime, nCallCount));
//        return true;
//    }

//    public void unRegister(ITimerHanlder handler, int nTimerID)
//    {
//        for (int i = 0; i < collections.Count; ++i)
//        {
//            if (collections[i].m_timerID == nTimerID)
//            {
//                collections.RemoveAt(i);
//                return;
//            }
//        }
//    }

//    public void OnUpdate()
//    {
//        if (collections.Count == 0)
//            return;

//        // 先复制一份再遍历
//        TimerObject[] tempList = new TimerObject[collections.Count];
//        collections.CopyTo(tempList);
//        //Initialize.PrintTickCheck("Timer Object Updata", _bStart: true);
//        for (int i = 0; i < tempList.Length; ++i)
//        {
//            tempList[i].OnUpdate();

//            //Initialize.PrintTickCheck("Timer handler:"+tempList[i].m_handler+",timerID:"+tempList[i].m_timerID, Initialize.CostThreshold_Lv0);
//        }
//    }
//}
