/*******************************************************************
** 文件名:	RunTimeCounter.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-1-8
** 版  本:	1.0
** 描  述:	运行时计时

		
********************************************************************/
using System;


// 运行时计时
public class RunTimeCounter
{
    private static System.DateTime startTime;
    private static System.DateTime endTime;

    /// <summary>
    /// 开始计时
    /// </summary>
    public static System.DateTime Start()
    {
        startTime = System.DateTime.Now;
        return startTime;
    }

    /// <summary>
    /// 获取当前时间
    /// </summary>
    /// <returns></returns>
    public static System.DateTime Current()
    {
        return System.DateTime.Now;
    }

    /// <summary>
    /// 获取运行时间 ms
    /// </summary>
    /// <returns></returns>
    public static int GetRunTime()
    {
        endTime = System.DateTime.Now;

        System.TimeSpan time = endTime - startTime;
        // 以毫秒为单位
        return time.Milliseconds;
    }

    /// <summary>
    /// 获取两个时间差 ms
    /// </summary>
    /// <param name="startTime">开始时间</param>
    /// <param name="endTime">结束时间</param>
    /// <returns></returns>
    public static int GetDiffTime(System.DateTime startTime, System.DateTime endTime)
    {
        return (endTime - startTime).Milliseconds;
    }
}