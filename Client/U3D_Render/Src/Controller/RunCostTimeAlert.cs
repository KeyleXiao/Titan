/*******************************************************************
** 文件名:	RunCostTimeAlert.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-1-11
** 版  本:	1.0
** 描  述:	程序运行耗时报警

		
********************************************************************/
using System;


// 运行耗时回调事件
public delegate void CostTimeAlertEventHanlder(string text);


/// <summary>
/// 程序运行耗时报警
/// </summary>
public class RunCostTimeAlert
{
    /// <summary>
    /// Update函数
    /// </summary>
    public event CostTimeAlertEventHanlder CostTimeAlertEvent;

    public void Do(string alert_text)
    {
        if (CostTimeAlertEvent != null)
        {
            CostTimeAlertEvent(alert_text);
        }
    }

}

