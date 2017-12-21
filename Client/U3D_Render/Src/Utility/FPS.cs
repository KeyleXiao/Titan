/*******************************************************************
** 文件名:	FPS.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-1-11
** 版  本:	1.0
** 描  述:	FPS

		
********************************************************************/
using System;
using UnityEngine;


public static class FPS
{
    static float m_deltaTime;

    static FPS()
    {
        m_deltaTime = 0.0f;
    }

    /// <summary>
    /// FPS数值
    /// </summary>
    public static float Value
    {
        get {
            if (m_deltaTime == 0.0f)
                return 1.0f;
            else
                return 1.0f / m_deltaTime;
        }
    }

    /// <summary>
    /// Update函数
    /// </summary>
    public static void OnUpdate()
    {
        m_deltaTime += (UnityEngine.Time.deltaTime - m_deltaTime) * 0.1f;
    }
}