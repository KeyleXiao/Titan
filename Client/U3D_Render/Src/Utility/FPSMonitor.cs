/*******************************************************************
** 文件名:	FPSMonitor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-1-11
** 版  本:	1.0
** 描  述:	FPS监控

		
********************************************************************/
using System;
using UnityEngine;
using System.Collections.Generic;

///// <summary>
///// FPS定时监控
///// </summary>
//public class FPSTimerMonitor : ITimerHanlder
//{
//    const int MAX_FPS = 1000;
//    int m_alertFps;         // 检测FPS输出阀值，0将不输出
//    float m_fps = MAX_FPS;
//    float m_outtime = 0.0f;

//    public FPSTimerMonitor()
//    {
//        m_alertFps = GameLogicAPI.getAlertFPS();

//        if (m_alertFps > 0)
//        {
//            TimerManager.SetTimer(this, 1, 1.0f);
//        }
//    }

//    public void Restore()
//    {
//        TimerManager.KillTimer(this, 0);
//    }

//    /// <summary>
//    /// 定时器回调函数
//    /// </summary>
//    /// <param name="nTimerID">定时器ID</param>
//    public void OnTimer(int nTimerID)
//    {
//        if (m_alertFps == 0)
//            return;

//        float fps = FPS.Value;
//        if (m_fps > fps)
//        {
//            m_fps = fps;
//        }

//        if (m_fps < m_alertFps)
//        {
//            if (Time.time > m_outtime + 3.0f)
//            {
//                m_outtime = Time.time;

//                Trace.LogWarning(string.Format("FPS lower, fps={0:N2}, FPS={0:N2}", m_fps, fps));

//                m_fps = MAX_FPS;
//            }
//        }
//    }
//}


// FPS均值回调事件
public delegate void FPS_MONITOR_REPORT_HANDLE(int duration, float avgFPS);

/// <summary>
/// FPS均值监控
/// </summary>
public class FPSMonitor
{
    float m_fps_total;
    int m_inc_count;
    float m_outtime;
    int m_config_duration;
    float m_duration;

    /// <summary>
    /// FPS均值回调
    /// </summary>
    public event FPS_MONITOR_REPORT_HANDLE ReportEvent;

    public FPSMonitor(int duration)
    {
        m_fps_total = 0.0f;
        m_inc_count = 0;
        m_outtime = 0.0f;

        m_config_duration = (duration == 0 ? 1000 : duration);
        m_duration = (m_config_duration > 0) ? ((float)m_config_duration / 1000.0f) : 1.0f;
    }

    void RestoreStat()
    {
        m_fps_total = 0.0f;
        m_inc_count = 0;
    }

    public void OnUpdate()
    {
        //场景在加载的时候，跳过均值fps计算
        if(SceneManager.isLoadingScene)
        {
            m_outtime = 0.0f;
            RestoreStat();
            return;
        }
        m_fps_total += FPS.Value;
        ++m_inc_count;

        // 每秒计算一次
        if (Time.time > m_outtime + m_duration)
        {
            m_outtime = Time.time;

            float avg_fps = m_fps_total / (float)m_inc_count;

            // 回调
            if (ReportEvent != null)
            {
                ReportEvent(m_config_duration, avg_fps);
            }
            
            // 重置
            RestoreStat();
        }
    }
}


/// <summary>
/// FPS监测报警
/// </summary>
public class FPSMonitorAlert
{
    int m_alertFps;  // 检测FPS输出阀值，0将不输出

    public FPSMonitorAlert()
    {
        m_alertFps = GameLogicAPI.getAlertFPS();
        GameFrameManager.FpsMonitor.SubScribeAlertEvent(this.OnReport);
    }
    ~FPSMonitorAlert()
    {
        GameFrameManager.FpsMonitor.UnSubScribeAlertEvent(this.OnReport);
    }

    public void OnReport(int duration, float avgFPS)
    {
        if (avgFPS < m_alertFps)
        {
            Trace.LogWarning(string.Format("fps is slow, ({0:G}ms)fps avg={1:N2}", duration, avgFPS));
        }        
    }
}



/// <summary>
/// FPS均值监控
/// </summary>
public class FPSAvgMonitor
{


    //FPSMonitor  m_fpsAlert;      // FPS均值监测报警
    //FPSMonitor  m_fpsProfile;    // FSP均值性能输出(新增长时间帧数均值，用于判断显示光效级别)

    //public FPSAvgMonitor()
    //{
    //    // FPS监测报警
    //    m_fpsAlert = new FPSMonitor(GameLogicAPI.getFPSInterVal());
    //    // FSP性能输出(新增长时间帧数均值，用于判断显示光效级别)
    //    m_fpsProfile = new FPSMonitor(10*1000);
    //}

    ///// <summary>
    ///// 订阅FPS均值监测报警事件
    ///// </summary>
    ///// <param name="handler"></param>
    //public void SubScribeAlertEvent(FPS_MONITOR_REPORT_HANDLE handler)
    //{
    //    m_fpsAlert.ReportEvent += handler;
    //}
    ///// <summary>
    ///// 取消订阅FPS均值监测报警事件
    ///// </summary>
    ///// <param name="handler"></param>
    //public void UnSubScribeAlertEvent(FPS_MONITOR_REPORT_HANDLE handler)
    //{
    //    m_fpsAlert.ReportEvent -= handler;
    //}

    ///// <summary>
    ///// 订阅FSP均值性能输出
    ///// </summary>
    ///// <param name="handler"></param>
    //public void SubScribeProfileEvent(FPS_MONITOR_REPORT_HANDLE handler)
    //{
    //    m_fpsProfile.ReportEvent += handler;
    //}

    ///// <summary>
    ///// 取消订阅FSP均值性能输出
    ///// </summary>
    ///// <param name="handler"></param>
    //public void UnSubScribeProfileEvent(FPS_MONITOR_REPORT_HANDLE handler)
    //{
    //    m_fpsProfile.ReportEvent -= handler;
    //}


    ///// <summary>
    ///// 更新
    ///// </summary>
    //public void OnUpdate()
    //{
    //    m_fpsAlert.OnUpdate();
    //    m_fpsProfile.OnUpdate();
    //}
    FPSMonitor m_fpsAlert;      // FPS均值监测报警

    public float AvgFps
    {
        get
        {
            int fpsCout = FpsList.Count;
            //没有，返回0
            if (fpsCout <= 0)
            {
                return AvgFps;
            }

            //只有1帧
            if (fpsCout == 1)
            {
                return allFps;
            }

            //只有2帧
            if (fpsCout == 2)
            {
                return allFps * 0.5f;
            }

            float fps = allFps;
            //3帧以上，去掉一个最高，去掉一个最低
            fps -= MaxFps;
            fps -= MinFps;
            fpsCout -= 2;

            return allFps / fpsCout;
        }
    }

    /// <summary>
    /// 采样帧数
    /// </summary>
    const int FrameSampleCount = 40;

    private float MaxFps = float.MinValue;
    private float MinFps = float.MaxValue;
    private Queue<float> FpsList = null;
    private float allFps = 0;

    /// <summary>
    /// 均值回调间隔
    /// </summary>
    private const int CallBackInterVal = 20;
    private float CallBackTimer = 0;

    public FPSAvgMonitor()
    {
        // FPS监测报警
        m_fpsAlert = new FPSMonitor(GameLogicAPI.getFPSInterVal());
        FpsList = new Queue<float>();
    }

    public FPS_MONITOR_REPORT_HANDLE ReportEvent;

    /// <summary>
    /// 订阅FPS均值监测报警事件
    /// </summary>
    /// <param name="handler"></param>
    public void SubScribeAlertEvent(FPS_MONITOR_REPORT_HANDLE handler)
    {
        m_fpsAlert.ReportEvent += handler;
    }
    /// <summary>
    /// 取消订阅FPS均值监测报警事件
    /// </summary>
    /// <param name="handler"></param>
    public void UnSubScribeAlertEvent(FPS_MONITOR_REPORT_HANDLE handler)
    {
        m_fpsAlert.ReportEvent -= handler;
    }

    /// <summary>
    /// 订阅FSP均值性能输出
    /// </summary>
    /// <param name="handler"></param>
    public void SubScribeProfileEvent(FPS_MONITOR_REPORT_HANDLE handler)
    {
        ReportEvent += handler;
    }

    /// <summary>
    /// 取消订阅FSP均值性能输出
    /// </summary>
    /// <param name="handler"></param>
    public void UnSubScribeProfileEvent(FPS_MONITOR_REPORT_HANDLE handler)
    {
        ReportEvent -= handler;
    }


    /// <summary>
    /// 更新
    /// </summary>
    public void OnUpdate()
    {
        //在加载场景，不统计
        if(SceneManager.isLoadingScene)
        {
            CallBackTimer = 0.0f;
            return;
        }

        float curFps = FPS.Value;
        if(curFps < MinFps)
        {
            MinFps = curFps;
        }

        if(curFps > MaxFps)
        {
            MaxFps = curFps;
        }

        allFps += curFps;
        //列表里的数目已经达到上限，移除旧的
        if (FpsList.Count >= FrameSampleCount)
        {
            allFps -= FpsList.Dequeue();
        }
        FpsList.Enqueue(curFps);
        CallBackTimer += Time.deltaTime;
        if(CallBackTimer > CallBackInterVal)
        {
            CallBackTimer = 0.0f;
            if(null != ReportEvent)
            {
                ReportEvent(CallBackInterVal, AvgFps);
            }
        }

        m_fpsAlert.OnUpdate();
    }
}