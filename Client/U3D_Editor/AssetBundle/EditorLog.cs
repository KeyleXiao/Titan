/*******************************************************************
** 文件名:	EditorLog.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源系统
** 应  用:  	ab资源管理器编辑器

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using System.IO;
using System;
using LZ4ps;
using System.Text;
using System.Linq;

public class EditorLog
{
    public enum LogLevel
    {
        FatleError = 1,        // 致命错误
        ERROR = 1 << 1,           // 日志
        WARNING = 1 << 2,        // 警告
        INFO = 1 << 3,        // 错误
        
        ALL = 1 << 5
    }

    static bool isCanWrite = false;
    static string ErrorKey = "ERROR:";
    static string FatalErrorKey = "Fatal ERROR:";
    static string WarringKey = "WARING:";
    static string InfoKey = "INFO:";

    static StreamWriter logWriter = null;
    static LogLevel CurrentLevel = LogLevel.ERROR;

    public static bool isHaveErroLog = false;
    public static bool isHaveFatleErrorLog = false;
    public static string LogPath = string.Empty;

    static System.Object lockObj = new System.Object();
    public static void StartLog(string LogName = "EditorLog",LogLevel loglvel = LogLevel.ALL)
    {
        lock(lockObj)
        {
            if (isCanWrite)
            {
                EndLog();
            }
            CurrentLevel = loglvel;
            string path = Application.dataPath;
            path = path.Replace("Game/Assets", "");
            LogPath = path + LogName + ".csv";
            logWriter = new StreamWriter(LogPath, false, new System.Text.UTF8Encoding(true));
            isCanWrite = true;
            isHaveErroLog = false;

            Application.logMessageReceived += HandleLog;
        }

    }

    static void HandleLog(string logString, string stackTrace, LogType type)
    {
        if(!isCanWrite)
        {
            return;
        }
        switch(type)
        {
            case LogType.Error:
                LogError(logString);
                break;
            case LogType.Exception:
                LogError(logString);
                break;
            case LogType.Warning:
                LogWarring(logString);
                break;
            case LogType.Log:
                LogInfo(logString);
                break;
        }
    }

    public static void LogFatleError(string msg, bool force = false)
    {

        isHaveFatleErrorLog = true;
        if ((CurrentLevel < LogLevel.FatleError  && !force) || null == logWriter)
        {
            return;
        }
        lock (lockObj)
        {
            logWriter.WriteLine(FatalErrorKey + msg);
        }
        
    }

    public static void LogError(string msg, bool force = false)
    {
        //删除了默认资源，打包会报这些错
        if (msg.StartsWith("The resource "))
        {
            return;
        }
        isHaveErroLog = true;
        if ((CurrentLevel < LogLevel.ERROR  && !force) || null == logWriter)
        {
            return;
        }
        lock (lockObj)
        {
            logWriter.WriteLine(ErrorKey + msg);
        }
        
    }

    public static void LogWarring(string msg, bool force = false)
    {
        if ((CurrentLevel < LogLevel.WARNING && !force)|| null == logWriter)
        {
            return;
        }

        lock (lockObj)
        {
            logWriter.WriteLine(WarringKey + msg);
        }
       
    }

    public static void LogInfo(string msg,bool force = false)
    {
        if ((CurrentLevel < LogLevel.INFO && !force) || null == logWriter)
        {
            return;
        }

        lock (lockObj)
        {
            logWriter.WriteLine(InfoKey + msg);
        }
        
    }

    public static void EndLog()
    {
        if(!isCanWrite)
        {
            return;
        }

        lock (lockObj)
        {
            isHaveErroLog = false;
            isCanWrite = false;
            logWriter.Flush();
            logWriter.Close();
            Application.logMessageReceived -= HandleLog;
            logWriter = null;
        }

    }

}
