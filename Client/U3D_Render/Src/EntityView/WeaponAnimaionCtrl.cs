using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameLogic;


/*
 * 武器控制脚本
 * 此脚本可以控制武器的动画
 * 将武器虚拟目标骨骼按时间配置拉伸到绑定位置
*/


#if USE_TIMMER_BEHAVIOUR
public class WeaponAnimaionCtrl : UpdateBehaviourOnTimer
#else
public class WeaponAnimaionCtrl : MonoBehaviour
#endif
{
    public Transform targetTransform;
    public Transform weaponTargetBone;
    public bool enlargeOnly = true;

    private Animator m_animator;

    public void Start()
    {
        m_animator = GetComponent<Animator>();
    }

    public void Update()
    {
        if (m_animator == null || targetTransform == null || weaponTargetBone == null) return;

        Vector3 fromPos = weaponTargetBone.position;
        Vector3 toPos = targetTransform.position;

        Vector3 fromVec = fromPos - transform.position;
        Vector3 toVec = toPos - transform.position;

        float scale = toVec.magnitude / fromVec.magnitude;
        if (fromVec.magnitude<0.01f || (enlargeOnly && scale < 1.0f))
        {
            scale = 1.0f; //只放大不缩小
        }

        scale=1.0f+(scale-1.0f)*scaleFactor;
        scaleFactor = scaleFactor + scaleSpeed * Time.deltaTime;
        if (scaleFactor >= 1.0f)
        {
            scaleFactor = 1.0f;
            scaleSpeed = 0.0f;
        }
        if (scaleFactor <= 0.0f)
        {
            scaleFactor = 0.0f;
            scaleSpeed = 0.0f;
        }
        transform.localScale = new Vector3(scale, scale, scale);

    }

    //开始动作时对各种拉伸参数赋值
    public void startAnim()
    {
        scaleSpeed = 0.0f;
        scaleFactor = 0.0f;
        transform.localRotation = Quaternion.identity;
        transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
    }

    //动作触发，开始/结束缩放
    float scaleSpeed;
    float scaleFactor;
    public void beginScale(float deltaTime)
    {
        scaleSpeed = (1.0f - scaleFactor) / deltaTime;
    }

    public void endScale(float deltaTime)
    {
        scaleSpeed = -scaleFactor / deltaTime;
    }

}
