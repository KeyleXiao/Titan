using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;


public class AnimationGrassAdorning : MonoBehaviour
{

 


    private bool m_init = false;
    private Animator m_Animator = null;
    private Vector3 center = new Vector3(0, 0, 0);
    private Vector3 boundmin;
    private Vector3 boundmax;
    bool lasttrigger = false;
    const float AnimatorOptimitzedDistance = 25.0f * 25.0f;
    bool bOptimitzedAnimator = false;
    SceneResInfo m_ResInfo;
    //const string animationName = "Move";
    public void Init()
    {
        if(m_init)
        {
            return;
        }
        m_Animator = GetComponent<Animator>();
        SkinnedMeshRenderer mr = GetComponentInChildren<SkinnedMeshRenderer>();
        if (mr != null)
        {
            Bounds b = mr.bounds;
            Vector3 boundcenter = b.center;
            Vector3 boundsize = b.size * 0.5f;
            boundmin = boundcenter - boundsize * 0.5f;
            boundmax = boundcenter + boundsize * 0.5f;
        }
        m_ResInfo = GetComponentInChildren<SceneResInfo>();
        if (m_ResInfo)
        {
            m_ResInfo.RegisterFadeInFinishEvent( OnFadeInFinsh);
            m_ResInfo.RegisterFadeOutFinishEvent( OnFadeOutFinsh);
        }
        GameUtil.DisabledAnimator(m_Animator);
        m_init = true;
    }

   
    void OnFadeInFinsh(SceneResInfo r)
    {
        if(bOptimitzedAnimator)
        {
            return;
        }
        GameUtil.EnabledAnimator(m_Animator);
    }

    void OnFadeOutFinsh(SceneResInfo r)
    {
        GameUtil.DisabledAnimator(m_Animator);
    }

    public void OnDestroy()
    {
        m_Animator = null;
    }

    public void PerformVisDetection(GameObject go)
    {
        Vector3 testpos = go.transform.position;
        bool curtrigger = false;
        
        float dis = GameUtil.GetSqrDistanceWithOutY(testpos, transform.position);
        
        if (dis >= AnimatorOptimitzedDistance)
        {
            if (!bOptimitzedAnimator)
            {
                GameUtil.DisabledAnimator(m_Animator);
                bOptimitzedAnimator = true;
            }
            
        }
        else
        {
            if (bOptimitzedAnimator)
            {
                GameUtil.EnabledAnimator(m_Animator);
                bOptimitzedAnimator = false;
            }
            
        }
        if (testpos.x >= boundmin.x && testpos.z >= boundmin.z
            && testpos.x <= boundmax.x && testpos.z <= boundmax.z)
        {
            curtrigger = true;
        }
        if (curtrigger != lasttrigger)
        {
            if (curtrigger)
            {
                OnEnter();
            }
            else
            {
                OnLeave();
            }
            lasttrigger = curtrigger;
        }
    }

    private void OnEnter()
    {
        
        m_Animator.SetTrigger("InTrigger");
        //m_Animator.Play(animationName);
    }

    private void OnLeave()
    {
        m_Animator.ResetTrigger("InTrigger");
    }
}

