using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameLogic;


/*
 * 跟随实体：
可以为英雄配置跟随实体，
跟随实体可以有多个，
与英雄同时出现或者消失，采取渐进式的方式转向或者移动，
可以为实体配置动作，当英雄做某个攻击动作的时候，实体会调用状态机下的同名动作。
*/

[System.Serializable]
public class SingleSidekick
{
    [Desc("跟随的预制体")]
    public GameObject effectPrefb = null;

    [Desc("跟随的骨骼")]
    public Transform attachBone = null;

    [Desc("跟随的偏移值")]
    public Vector3 attachOffset = Vector3.zero;

    [Desc("出生的偏移值")]
    public Vector3 bornOffset = Vector3.zero;

    [Desc("跟随的渐进比例")]
    public float attachSpeed = 0.5f;

    [Desc("面向前进方向")]
    public bool faceForward = true;

    [Desc("非攻击动作名字")]
    public string idleAnimation = "IDLE";

    [Desc("出生动作名字")]
    public string bornAnimation = "IDLE";

    private GameObject m_entityObject = null;
    private Transform m_entityTransoform = null;
    private Animator m_Animator = null;
    private Transform m_masterTransoform = null;

    private bool m_active = true;
    public bool active
    {
        get
        {
            return m_active;
        }
        set
        {
            if (m_active == value) return;
            m_active = value;
            m_entityObject.SetActive(m_active);
        }
    }


    public void init(Transform masterTransform)
    {
        if (!masterTransform || !attachBone) return;
        if (effectPrefb == null)
        {
            Trace.Log("Sidekick effectPrefb is null!");
            return;
        }
        if (m_entityObject)
        {
            MonoBehaviour.DestroyObject(m_entityObject);
            m_entityObject = null;
        }
        m_entityObject = ResNode.InstantiateRes(effectPrefb) as GameObject;
        if (!m_entityObject) return;
        m_entityTransoform = m_entityObject.transform;
        m_entityTransoform.parent = EntityFactory.Instance.transform;
        m_masterTransoform = masterTransform;

        m_masterTransoform.SetPosition(attachBone.position + m_masterTransoform.rotation * bornOffset);
        m_entityTransoform.rotation = m_masterTransoform.rotation;

        m_Animator = m_entityObject.GetComponent<Animator>();
        playAnim(bornAnimation);
    }

    public void update()
    {
        if (!attachBone || !m_entityTransoform || !m_masterTransoform)
        {
            return;
        }

        Vector3 targetPos = attachBone.position + m_masterTransoform.rotation * attachOffset;
        Vector3 delta = targetPos - m_entityTransoform.position;

        Quaternion recordRotation = m_entityTransoform.rotation; //渐进转向
        if (delta.sqrMagnitude < 0.001f)
        {
            m_entityTransoform.rotation = m_masterTransoform.rotation;
        }
        else
        {
            m_entityTransoform.position += delta * attachSpeed;
            if (faceForward)
            {
                m_entityTransoform.LookAt(targetPos);
            }
            else
            {
                m_entityTransoform.rotation = m_masterTransoform.rotation;
            }
        }

        //渐进转向
        Quaternion finalRotation = Quaternion.Lerp(recordRotation, m_entityTransoform.rotation, attachSpeed);
        m_entityTransoform.rotation = finalRotation;
    }

    public void destroy()
    {
        MonoBehaviour.DestroyObject(m_entityObject);
        m_entityObject = null;
        m_entityTransoform = null;
        m_Animator = null;
        m_masterTransoform = null;
    }

    public void playAnim(string animStr)
    {
        if (!m_Animator) return;
        if (!m_active) return;
        if (ContainState(m_Animator, animStr))
        {
            m_Animator.Play(animStr);
        }
    }

    public void breakAnim()
    {
        playAnim(idleAnimation);
    }

    public bool ContainState(Animator animator, string name)
    {
        for (int i = 0; i < animator.layerCount; i++)
        {
            if (animator.HasState(i, Animator.StringToHash(name)))
            {
                return true;
            }
        }
        return false;
    }
}


#if USE_TIMMER_BEHAVIOUR
public class SidekickEntity : UpdateBehaviourOnTimer
#else
public class SidekickEntity : MonoBehaviour
#endif
{
    public SingleSidekick[] sidekicks;

    public void init()
    {
        foreach (SingleSidekick i in sidekicks)
        {
            i.init(transform);
        }
    }

    public void Update()
    {
        foreach (SingleSidekick i in sidekicks)
        {
            i.update();
        }
    }

    public void OnDestroy()
    {
        foreach (SingleSidekick i in sidekicks)
        {
            i.destroy();
        }
    }

    public void setActive(bool active)
    {
        foreach (SingleSidekick i in sidekicks)
        {
            i.active = active;
        }
    }

    public void playAnim(string animStr)
    {
        foreach (SingleSidekick i in sidekicks)
        {
            i.playAnim(animStr);
        }
    }

    public void breakAnim()
    {
        foreach (SingleSidekick i in sidekicks)
        {
            i.breakAnim();
        }
    }
}
