/*******************************************************************
** 文件名:	AssetBundleManager_Thread.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/12/12
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器,线程部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using System.IO;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;
using System.Threading;

public sealed partial class AssetBundleManager : MonoBehaviour
{

    public class BaseMultThreadWorkData
    {
        
    }



    /// <summary>
    /// 是否已经释放线程
    /// </summary>
    private static bool m_bDisposedThread = true;

    /// <summary>
    /// 是否跑单线程
    /// </summary>
    private static bool isSingleThread = false;

    /// <summary>
    /// 允许的最大线程数量
    /// </summary>
    public static int MaxActiveThread = 0;

    private static int OldWolkerThread = 0;
    private static int OldCPThread = 0;

    private static System.Object m_Lock = null;
    /// <summary>
    /// 初始化AssetBundle线程
    /// </summary>
    public static void InitAssetBundleThread()
    {
        if (!m_bDisposedThread)
        {
            return;
        }

        if(!isSingleThread)
        {
            //最大的可运行线程CPU*2
            MaxActiveThread = SystemInfo.processorCount * 2;
            ThreadPool.GetMaxThreads(out OldWolkerThread, out OldCPThread);
            ThreadPool.SetMaxThreads(MaxActiveThread, MaxActiveThread / 2);
        }
        m_Lock = new System.Object();
        m_bDisposedThread = false;
    }

    public static bool PushWork(System.Threading.WaitCallback workfunc, BaseMultThreadWorkData data)
    {
        if(m_bDisposedThread || null == workfunc)
        {
            return false;
        }

        if (isSingleThread)
        {
            workfunc(data);
            return true;
        }

        return ThreadPool.QueueUserWorkItem(workfunc, data);

    }

    /// <summary>
    /// 结束AssetBundle线程
    /// </summary>
    public static void ClearAssetBundleThread()
    {
        if (isSingleThread || m_bDisposedThread)
        {
            return;
        }
        m_bDisposedThread = true;
        ThreadPool.SetMaxThreads(OldWolkerThread, OldCPThread);
        m_Lock = null;
    }


    /// <summary>
    /// 送入请求
    /// </summary>
    /// <param name="filePatch"></param>
    /// <param name="isCompress"></param>
    /// <returns></returns>
    public static ResourcePackage PostAssetFileEvent(string filePatch, bool isCompress)
    {
        if(m_bDisposedThread)
        {
            return null;
        }

        return null;
    }

    
    /// <summary>
    /// 工作线程函数
    /// </summary>
    private static void DeCompressAssetFileWorkFunc(System.Object ThreadDataObj)
    {
    }

}
