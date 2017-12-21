/*******************************************************************
** 文件名:	MonsterStateMachine_LOD.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	怪物状态机
** 应  用:  	怪物状态机中的LOD部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameLogic;
using System;
using U3D_Render;


public sealed  partial class MonsterStateMachine : BaseStateMachine
{

    #region LOD相关

    private string currentAnimationName = string.Empty;
    public const string IdleAnimationName = "Idle";
    public const string RunAnimationName = "Run";

    private string boolParameName = string.Empty;
    private bool boolValue = false;

    private string intParameName = string.Empty;
    private int intValue = 0;

    private string floatParameName = string.Empty;
    private float floatValue = 0;


    private string triggerParameName = string.Empty;

    private int layerIndexParame = -10;
    private int layerIndexWeight = -1;

    /// <summary>
    /// 刷新状态机数据，如果采用异步加载的话，因为animator还没有加载出来，所有动作并没有设置
    /// </summary>
    public void RefershAnimatorDataWhenSkinChangedFinish()
    {
        if(!animator)
        {
            return;
        }
        if (!boolParameName.Equals(string.Empty))
        {
            animator.SetBool(boolParameName, boolValue);
            
            boolParameName = string.Empty;
        }
        if (!intParameName.Equals(string.Empty))
        {
            animator.SetInteger(intParameName, intValue);
            intParameName = string.Empty;
        }

        if (!floatParameName.Equals(string.Empty))
        {
            animator.SetFloat(floatParameName, floatValue);
            floatParameName = string.Empty;
        }

        if (!triggerParameName.Equals(string.Empty))
        {
            animator.SetTrigger(triggerParameName);
            triggerParameName = string.Empty;
        }

        if (layerIndexParame != -10)
        {
            animator.SetLayerWeight(layerIndexParame, layerIndexWeight);
            layerIndexParame = -10;
        }

        if(!currentAnimationName.Equals(string.Empty))
        {
            animator.Play(currentAnimationName);
        }
    }

    private void ResetAnimatorDataWhenSkinChangedFinish()
    {
        currentAnimationName = string.Empty;

        boolParameName = string.Empty;
        boolValue = false;

        intParameName = string.Empty;
        intValue = 0;

        floatParameName = string.Empty;
        floatValue = 0;


        triggerParameName = string.Empty;

        layerIndexParame = -10;
        layerIndexWeight = -1;
    }

    public override bool GetAnimatorBool(string name)
    {
        if (animator)
        {
            return animator.GetBool(name);
        }

        return false;
    }

    public override void SetAnimatorBool(string name, bool value)
    {
        boolParameName = name;
        boolValue = value;
        if (animator)
        {
 
            animator.SetBool(name, value);
        }

        //if (Initialize.Instance.EntityEnabledLODSystem)
        //{
        //    foreach (Animator a in CurrentLODAnimators)
        //    {
        //        if (a)
        //        {
        //            a.SetBool(name, value);
        //        }
        //    }
        //}
    }

    public override int GetAnimatorInteger(string name)
    {
        if (animator)
        {
            return animator.GetInteger(name);
        }

        return 0;
    }

    public override void SetAnimatorInteger(string name, int value)
    {
        intParameName = name;
        intValue = value;
        if (animator)
        {
            animator.SetInteger(name, value);
        }

        //if (Initialize.Instance.EntityEnabledLODSystem)
        //{
        //    foreach (Animator a in CurrentLODAnimators)
        //    {
        //        if (a)
        //        {
        //            a.SetInteger(name, value);
        //        }
        //    }
        //}
    }

    public override void SetAnimatorTrigger(string name)
    {
        triggerParameName = name;
        if (animator)
        {
            animator.SetTrigger(name);
        }

        //if (Initialize.Instance.EntityEnabledLODSystem)
        //{
        //    foreach (Animator a in CurrentLODAnimators)
        //    {
        //        if (a)
        //        {
        //            a.SetTrigger(name);
        //        }
        //    }
        //}
    }

    public override float GetAnimatorFloat(string name)
    {
        if (animator)
        {
            return animator.GetFloat(name);
        }

        return 0.0f;
    }

    public override void SetAnimatorFloat(string name, float value)
    {
        floatParameName = name;
        floatValue = value;
        if (animator)
        {
            animator.SetFloat(name, value);
        }

        //if (Initialize.Instance.EntityEnabledLODSystem)
        //{
        //    foreach (Animator a in CurrentLODAnimators)
        //    {
        //        if (a)
        //        {
        //            a.SetFloat(name, value);
        //        }
        //    }
        //}
    }

    public override void SetAnimatorLayerWeight(int LayerIndex, int value)
    {
        layerIndexParame = LayerIndex;
        layerIndexWeight = value;
        if (animator)
        {
            animator.SetLayerWeight(LayerIndex, value);
        }

        //if (Initialize.Instance.EntityEnabledLODSystem)
        //{
        //    foreach (Animator a in CurrentLODAnimators)
        //    {
        //        if (a)
        //        {
        //            a.SetLayerWeight(LayerIndex, value);
        //        }
        //    }
        //}
    }

    public override void AnimatorPlay(string name, float durationTime = 0.1f)
    {
        if(string.IsNullOrEmpty(name))
        {
            return;
        }
        if (animator)
        {
            if (currentAnimationName.Equals(name) && currentAnimationName.Equals(RunAnimationName))
            {
                return;
            }
            currentAnimationName = name;

            if (!ContainState(animator, name))
            {
                //Trace.LogWarning("PlayAnim:" + name + " fail,AnimationController:" + animator.name + " doesn't contain state:" + name);
                return;
            }
            //怪物不过度，直接强切动画
            animator.Play(name);
            
            //animator.CrossFade(name, durationTime, -1, 0.0f);
        }

        //if (Initialize.Instance.EntityEnabledLODSystem)
        //{
        //    foreach (Animator a in CurrentLODAnimators)
        //    {
        //        if (a)
        //        {
        //            a.CrossFade(name, durationTime, -1, 0.0f);;
        //        }
        //    }
        //}
    }

    #endregion
}
