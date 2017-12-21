using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Common.FSMSample
{
    /// <summary>
    /// 状态参数
    /// </summary>
    public interface IFSMParam
    {

    }

    /// <summary>
    /// 状态接口
    /// </summary>
    public interface IFSM_Base
    {
        uint GetStateID();
        int Enter(IFSMParam _data);
        int Execute(IFSMParam _data);
        int Exit(IFSMParam _data);
    }

    public class RunningStatus
    {        
        //默认运行状态
        public const int EXECUTING = 0;
        public const int FINISHED = 1;
        public const int TRANSITION = 2;
        //-------------------------------------------------------
        //用户自定义的运行状态
        //100-999, 预定给用户的执行状态
        public const int USER_EXECUTING = 100;
        //>=1000, 预定给用户的结束状态
        public const int USER_FINISHED = 1000;
        //-------------------------------------------------------
        static public bool IsOK(int runningStatus)
        {
            return runningStatus == RunningStatus.FINISHED ||
                   runningStatus >= RunningStatus.USER_FINISHED;
        }
        static public bool IsError(int runningStatus)
        {
            return runningStatus < 0;
        }
        static public bool IsFinished(int runningStatus)
        {
            return IsOK(runningStatus) || IsError(runningStatus);
        }
        static public bool IsExecuting(int runningStatus)
        {
            return !IsFinished(runningStatus);
        }
    }

}
