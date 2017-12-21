//Unity的ChatacterController已经解决，不需要使用这个了
//using UnityEngine;
//using System.Collections;
//using UnityEngine.Events;

//public class CharacterControllerHitEvent : UnityEvent<ControllerColliderHit>
//{
//}

//public abstract class BaseCharacterController
//{
//    CharacterControllerConfig ControllerConfig
//    {
//        get
//        {
//            return m_Config;
//        }
//        set
//        {
//            m_Config = value;
//        }
//    }

//    public bool enabled
//    {
//        get
//        {
//            return m_Enabled;
//        }
//        set
//        {
//            m_Enabled = value;
//        }
//    }


//    private CharacterControllerConfig m_Config = null;

//    protected GameObject hostGameObject = null;
//    protected Transform hostTransform = null;

//    private bool bInit = false;
//    private bool m_Enabled = true;
//    private CharacterControllerHitEvent m_HitEvent = new CharacterControllerHitEvent();

//    public void RegisterCharacterHitEvent(UnityAction<ControllerColliderHit> ev)
//    {
//        if (null != m_HitEvent)
//        {
//            if (null != ev)
//            {
//                m_HitEvent.AddListener(ev);
//            }
//        }
//    }

//    public void UnRegisterCharacterHitEvent(UnityAction<ControllerColliderHit> ev)
//    {
//        if (null != m_HitEvent)
//        {
//            if (null != ev)
//            {
//                m_HitEvent.RemoveListener(ev);
//            }
//        }
//    }

//    public void ClearCharacterHitEvent()
//    {
//        if (null != m_HitEvent)
//        {
//           m_HitEvent.RemoveAllListeners();
//        }
//    }

//    public virtual void Init(GameObject host)
//    {
//        if (!bInit)
//        {
//            return ;
//        }
//        if (!host)
//        {
//            return;
//        }
//        bInit = true;
//        hostGameObject = host;
//        hostTransform = host.transform;
//    }

//    public virtual void Clear()
//    {
//        bInit = false;
//        m_Enabled = true;
//        m_Config = null;
//        hostGameObject = null;
//        hostTransform = null;
//        ClearCharacterHitEvent();
//    }

//    public virtual bool IsValid()
//    {
//        if(!bInit)
//        {
//            Debug.LogWarning("没有初始化");
//            return false;
//        }
//        if (!hostGameObject)
//        {
//            Debug.LogWarning("hostGameObject为null");
//            return false;
//        }
//        if (!hostGameObject.activeSelf)
//        {
//            Debug.LogWarning("Move函数不能用于非激活的实例上");
//            return false;
//        }

//        if (null == m_Config)
//        {
//            Debug.LogWarning("Config is null");
//            return false;
//        }
//        return true;
//    }

//    public abstract CollisionFlags Move(Vector3 dealt);

//    public abstract bool isGround();


//}
