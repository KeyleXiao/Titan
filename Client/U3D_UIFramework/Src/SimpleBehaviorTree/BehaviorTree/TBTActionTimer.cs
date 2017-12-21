using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TsiU
{
    public class TBTActionTimer : TBTAction
    {
        public const float INFINITY = -1f;
        //--------------------------------------------------------
        protected class TBTActionTimerContext : TBTActionContext
        {
            internal float lastTime;

            public TBTActionTimerContext()
            {
                lastTime = INFINITY;
            }
        }
        //--------------------------------------------------------
        private float m_elapseTime;
        //--------------------------------------------------------
        public TBTActionTimer()
            : base(1)
        {
        }
        public TBTActionTimer SetElapseTime(float _nTime)
        {
            m_elapseTime = _nTime;
            return this;
        }
        //-------------------------------------------------------
        protected override bool onEvaluate(/*in*/TBTWorkingData wData)
        {
            TBTActionTimerContext thisContext = getContext<TBTActionTimerContext>(wData);

            if (thisContext.lastTime == INFINITY)
                thisContext.lastTime = UnityEngine.Time.time;

            bool checkTimerOver = (UnityEngine.Time.time >= thisContext.lastTime + m_elapseTime);

            if (checkTimerOver == false)
            {
                return false;
            }
            if (IsIndexValid(0))
            {
                TBTAction node = GetChild<TBTAction>(0);
                return node.Evaluate(wData);
            }
            return false;
        }
        protected override int onUpdate(TBTWorkingData wData)
        {
            TBTActionTimerContext thisContext = getContext<TBTActionTimerContext>(wData);
            int runningStatus = TBTRunningStatus.FINISHED;
            if (IsIndexValid(0))
            {
                TBTAction node = GetChild<TBTAction>(0);
                runningStatus = node.Update(wData);

                if (TBTRunningStatus.IsFinished(runningStatus))
                {
                    if (UnityEngine.Time.time > thisContext.lastTime + m_elapseTime)
                    {
                        runningStatus = TBTRunningStatus.FINISHED;
                    }
                }
                else if(TBTRunningStatus.IsExecuting(runningStatus))
                {
                    if (UnityEngine.Time.time > thisContext.lastTime + m_elapseTime)
                    {
                        thisContext.lastTime = UnityEngine.Time.time;
                    }
                }
            }
            return runningStatus;
        }
        protected override void onTransition(TBTWorkingData wData)
        {
            TBTActionTimerContext thisContext = getContext<TBTActionTimerContext>(wData);
            if (IsIndexValid(0))
            {
                TBTAction node = GetChild<TBTAction>(0);
                node.Transition(wData);
            }
            thisContext.lastTime = -1;
        }
    }
}
