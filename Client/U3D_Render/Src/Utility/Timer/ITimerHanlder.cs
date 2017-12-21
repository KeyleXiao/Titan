/*******************************************************************
** 文件名:	ITimerHanlder.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-1-11
** 版  本:	1.0
** 描  述:	定时器接口

		
********************************************************************/
using System;


/// <summary>
/// 定时器接口
/// </summary>
public interface ITimerHanlder
{
    void OnTimer(int nTimerID);
}
