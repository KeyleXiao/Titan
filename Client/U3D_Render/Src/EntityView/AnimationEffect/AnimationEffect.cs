using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;

// 动作播放类，可以播放比较复杂的自定义动作

public class AnimationParam
{
    //创建时需要的参数，有逻辑层提供
    public BaseStateMachine pFSM = null; //做动作的角色
    public float fSpeed = 1.0f; //动画播放速率
    public void ColoneTo(ref AnimationParam targetParam)
    {
        targetParam.pFSM = pFSM;
        targetParam.fSpeed = fSpeed;
    }
}

[System.Serializable]
//单个动作的配置
public class Single_Animation
{
    [Desc("在状态机里的动作名")]
    public string animName;

    [Desc("开始时间（单位：毫秒）")]
    public int startTick;

    [Desc("与上个动作的融合时间（单位：秒）")]
    public float durationTime;

    [Desc("最大浮空高度，超过这个高度则进入下一个动作，0则为无效")]
    public float maxHeight;
}



[System.Serializable]
public class AnimationEffect : ScriptableObject
{
    public int ID;
    public bool rigidHeight = false;
    public Single_Animation[] AnimationList;


    [HideInInspector, NonSerialized]
    public AnimationParam param;

    [HideInInspector, NonSerialized]
    public bool showLog = false;

    [HideInInspector, NonSerialized]
    public bool needClose = false;

    [HideInInspector, NonSerialized]
    private uint m_startTick = 0;

    [HideInInspector, NonSerialized]
    private uint m_nextAnimationIndex = 0;

    [HideInInspector, NonSerialized]
    public float currentHeight = 0.0f;

    public virtual int getID()
    {
        return ID;
    }

    public virtual string getDesc()
    {
        return "";
    }

    public virtual bool Begin() //创建代码，返回创建结果，创建失败则删除
    {
        m_startTick = GameLogicAPI.getTickCount();
        return true;
    }

    public virtual bool Update() //每帧更新，返回更新结果，更新失败则删除
    {
        if (m_nextAnimationIndex >= AnimationList.Length)
        {
            return false;
        }
        int currentTick = (int)(GameLogicAPI.getTickCount() - m_startTick);
        Single_Animation nextAnim=AnimationList[m_nextAnimationIndex];

        float maxHeight=0.0f;
        if (m_nextAnimationIndex > 1)
        {
            maxHeight = AnimationList[m_nextAnimationIndex - 1].maxHeight;
        }

        if (currentTick >= nextAnim.startTick || (maxHeight>0.0f && currentHeight>maxHeight))
        {
            if (param.pFSM != null)
            {
                param.pFSM.PlayAnim(nextAnim.animName, false, param.fSpeed, nextAnim.durationTime);
            }
            m_nextAnimationIndex++;
        }

        return true;
    }

    public virtual void Break()
    {
        if (param.pFSM != null)
        {
            param.pFSM.BreakAttackAnim();
        }
    }

    public virtual void Close()
    {
    }
}
