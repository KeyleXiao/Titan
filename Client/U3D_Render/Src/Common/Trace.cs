
//#define SYNC_DEBUG

using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System.IO;
using System.Text;

enum TraceLevel
{
    LOG = 1 << 1,           // 日志
    WARNING = 1 << 2,        // 警告
    ERROR = 1 << 3,        // 错误
    EMPHASIZE = 1 << 4,        // 强调
    ALL = 255,         // 所有全部打开
}


public class Trace : MonoBehaviour
{
    public static byte nTraceLevel = (byte)TraceLevel.ALL;
    public static bool bEditorLog = Application.isEditor;
    private static bool sbOutputConsole = false;
    public bool OutputConsole = false;
    public bool EditorLog = false;
#if SYNC_DEBUG
    static StreamWriter writer = null;
#endif


    public void Awake()
    {
#if SYNC_DEBUG
        string datestr = System.DateTime.Now.ToString("yyyy-MM-dd-hh-mm-ss");
        string outpath = Application.dataPath + "/outLog"+datestr+".txt";
        if (System.IO.File.Exists(outpath))
        {
            System.IO.File.Delete(outpath);
        }
        writer = new StreamWriter(outpath, true, Encoding.UTF8);
#endif

        Application.logMessageReceived += HandleLog;
        EditorLog = Application.isEditor;
    }

    public void Update()
    {
        sbOutputConsole = OutputConsole;
        bEditorLog = EditorLog;
    }

    public void OnDestroy()
    { 
#if SYNC_DEBUG
        if (writer != null)
        {
            writer.Close();
            writer = null;
        }
#endif
        Application.logMessageReceived -= HandleLog;
    }

    static void HandleLog(string logString, string stackTrace, LogType type)
    {
        if (Initialize.NeedToReStart)
        {
            return;
        }

#if SYNC_DEBUG
        if (writer != null)
        {
            writer.WriteLine(logString + "\r\n");
            writer.Flush();
        }
#endif

        if (System.Threading.Thread.CurrentThread.ManagedThreadId == 1)//主线程，非主线程在游戏没启动时也不应该抛出log给base.dll，但老报错不知道如何处理还
        {
            if (!Application.isPlaying)//在游戏没启动时也不应该抛出log给base.dll
            {
                return;
            }
        }

        if (logString.StartsWith("This custom render path shader"))
        {
            Initialize.NeedToReStart = true;
        }

        //设备丢失的字符串不输出,要不然log会爆炸
        if (logString.StartsWith("Skipped rendering"))
        {
            Initialize.isDeviceLost = true;
            Initialize.currLostDeviceCheckTimer = Initialize.LostDeviceCheckFrame;
            return;
        }

        if(Initialize.isDeviceLost)
        {
            //设备丢失这种字符串不需要输出，因为设备丢失unity相当的卡，不知道为什么，刷这个会导致log爆炸
            if (logString.StartsWith("fps is slow,"))
             {
                 return;
             }

           //设备丢失这种字符串不需要输出，因为设备丢失unity相当的卡，不知道为什么，刷这个会导致log爆炸
            if (logString.Contains("PingReplyer::checkDisConnect"))
             {
                 return;
             }
        }

        if (Initialize.isDeviceLost)
        {
            //设备丢失这种字符串不需要输出，因为设备丢失的时候，这些是必刷的
            if (logString.StartsWith("<I> Failed to get cursor"))
            {
                return;
            }
        }
        if (type == LogType.Error || type == LogType.Exception)
        {
            string msg = logString;
            if (stackTrace != null && stackTrace.Length > 0)
            {
                msg += "<details><summary>stack trace:</summary>";
                msg += stackTrace;
                msg += "</details>";
                msg += "<details><summary>memory usage:</summary>";
                msg += "MonoHeapSize:" + Profiler.GetMonoHeapSize();
                msg += "<br>MonoUsedSize:" + Profiler.GetMonoUsedSize();
                msg += "<br>TotalAllocatedMemory:" + Profiler.GetTotalAllocatedMemory();
                msg += "<br>TotalReservedMemory:" + Profiler.GetTotalReservedMemory();
                msg += "<br>TotalUnusedReservedMemory:" + Profiler.GetTotalUnusedReservedMemory();
                msg += "</details>";
            }
            GameLogicAPI.traceLog(msg, (int)TraceLevel.ERROR);
        }
        else if (type == LogType.Warning)
        {
            string msg = logString;
            //if (stackTrace != null && stackTrace.Length > 0)
            //{
            //    msg += "<details><summary>stack trace:</summary>";
            //    msg += stackTrace;
            //    msg += "</details>";
            //    msg += "<details><summary>memory usage:</summary>";
            //    msg += "MonoHeapSize:" + Profiler.GetMonoHeapSize();
            //    msg += "<br>MonoUsedSize:" + Profiler.GetMonoUsedSize();
            //    msg += "<br>TotalAllocatedMemory:" + Profiler.GetTotalAllocatedMemory();
            //    msg += "<br>TotalReservedMemory:" + Profiler.GetTotalReservedMemory();
            //    msg += "<br>TotalUnusedReservedMemory:" + Profiler.GetTotalUnusedReservedMemory();
            //    msg += "</details>";
            //}
            GameLogicAPI.traceLog(msg, (int)TraceLevel.WARNING);
        }
        else
        {
            GameLogicAPI.traceLog(logString, (int)TraceLevel.LOG);
        }

    }

    public static void Log(string info, Object context = null)
    {
        if ((nTraceLevel & (byte)TraceLevel.LOG) == 0)
        {
            return;
        }

        if (bEditorLog && sbOutputConsole)
        {
            Debug.Log(info, context);
        }
        else
        {
            HandleLog(info, System.Environment.StackTrace, LogType.Log);
        }
    }

    public static void LogFormat(string format, params object[] args)
    {
        Log(string.Format(format, args));
    }

    public static void Warning(string info, Object context = null)
    {
        if ((nTraceLevel & (byte)TraceLevel.WARNING) == 0)
        {
            return;
        }

        if (bEditorLog && sbOutputConsole)
        {
            Debug.LogWarning(info, context);
        }
        else
        {
            HandleLog(info, System.Environment.StackTrace, LogType.Warning);
        }
    }

    public static void LogWarning(string info, Object context = null)
    {
        Warning(info, context);
    }

    public static void Error(string info, Object context = null)
    {
        if ((nTraceLevel & (byte)TraceLevel.ERROR) == 0)
        {
            return;
        }

        if (bEditorLog && sbOutputConsole)
        {
            Debug.LogError(info, context);
        }
        else
        {
            HandleLog(info, System.Environment.StackTrace, LogType.Error);
        }
    }
    public static void LogError(string info, Object context = null)
    {
        Error(info, context);
    }
    public static void Emphasize(string info, Object context = null)
    {
        if ((nTraceLevel & (byte)TraceLevel.EMPHASIZE) == 0)
        {
            return;
        }

        if (bEditorLog && sbOutputConsole)
        {
            Debug.LogWarning(info, context);
        }
        else
        {
            HandleLog(info, System.Environment.StackTrace, LogType.Warning);
        }
    }

    //暂时打印下SFUI替换信息
    public static void TraceSFUIWaring(string SFViewType,string cmd,string func)
    {
        Trace.LogWarning("SF UI消息::界面 - " + SFViewType + "已移除,方法 - "+func+" 不会执行，暂时没有新UI处理消息ID - " + cmd);
    }
}
