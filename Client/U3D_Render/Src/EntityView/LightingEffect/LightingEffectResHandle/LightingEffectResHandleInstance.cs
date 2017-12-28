using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Effect;

// 光效工厂 
// 全局单一类，整个游戏只有一个实例
// 负责载入光效配置，创建光效

public class LightingEffectResHandleInstance
{
    /// <summary>
    /// 有的用的是替换动画,有的用的是EffectNode
    /// </summary>
    public bool Valid
    {
        get
        {
            return m_EffectID > 0 && (m_effectNode || m_effect_ReplaceAnimation || m_effect_CameraAnimation);
        }
    }

    public int EffectID
    {
        get
        {
            return m_EffectID;
        }
    }

    private int m_EffectID = -1;
    /// <summary>
    /// 光效配置
    /// </summary>
    public LightingEffect effectConfig
    {
        get
        {
            return m_effectConfig;
        }
    }
    private LightingEffect m_effectConfig = null;

    /// <summary>
    /// 光效预制体
    /// </summary>
    public GameObject effectPrefab
    {
        get
        {
            return m_effectPrefab;
        }
    }
    private GameObject m_effectPrefab = null;

    /// <summary>
    /// 光效预制体
    /// </summary>
    public Transform effectPrefabTransform
    {
        get
        {
            return m_effectPrefabTransform;
        }
    }
    private Transform m_effectPrefabTransform = null;

    /// <summary>
    /// EffectNode结点
    /// </summary>
    public Effect.EffectNode effectNode
    {
        get
        {
            return m_effectNode;
        }
    }
    private Effect.EffectNode m_effectNode= null;

    /// <summary>
    /// Effect_ReplaceAnimation结点
    /// </summary>
    public Effect.Effect_ReplaceAnimation ReplaceAnimation
    {
        get
        {
            return m_effect_ReplaceAnimation;
        }
    }
    private Effect_ReplaceAnimation m_effect_ReplaceAnimation = null;

    /// <summary>
    /// Effect_CameraAnimation结点
    /// </summary>
    public Effect.Effect_CameraAnimation CameraAnimation
    {
        get
        {
            return m_effect_CameraAnimation;
        }
    }
    private Effect_CameraAnimation m_effect_CameraAnimation = null;
    
    public bool isCached = false;

    private AttackCollisionGroup m_AttackGroup = null;

    public void OnCache(Transform cacheParent,Vector3 cachePos,Quaternion orginalrot)
    {
        isCached = true;
        if (Valid && m_effectNode)
        {
           
            m_effectNode.StopEffect();
            m_effectNode.SetParticleSpeed(1.0f);
            m_effectNode.ResetScale();
            m_effectPrefab.transform.parent = cacheParent;
            m_effectPrefab.transform.SetPosition(cachePos);
            m_effectNode.hitEvent = null;
            m_effectPrefab.transform.localRotation = orginalrot;

            //撤销延迟析构隐藏的inactive子部件，避免下次重用时出错                
            BroadcastMessage("renewActiveForDelayDestroy");
            m_effectPrefab.SetActive(false);
        }
    }

    public void BroadcastMessage(string funcName)
    {
        if (Valid && m_effectNode)
        {
            m_effectPrefab.BroadcastMessage(funcName, SendMessageOptions.DontRequireReceiver);
        }
    }

    public void ApplyPlayData(EffectResHandlePlayData data)
    {
        if(null == data)
        {
            return;
        }

        if(!Valid)
        {
            return;
        }

        if(isCached)
        {
            data.ClearDirtyData();
            return;
        }
        if (!m_effectNode)
        {
            return;
        }
        if(data.ParentDirty)
        {
            m_effectPrefab.SetActive(false);
            data.parent.gameObject.name = m_effectPrefab.name + "-Agent";
            m_effectPrefabTransform.SetParent(data.parent);
            m_effectPrefabTransform.localPosition = Vector3.zero;
            m_effectPrefabTransform.localRotation = Quaternion.identity;
            m_effectPrefab.SetActive(true);

        }
        //暂不支持
        //if (data.SpeedDirty)
        //{
        //    m_effectNode.SetParticleSpeed(data.Speed);
        //}
        if (data.SizeScaleDirty)
        {
            m_effectNode.SetScale(data.SizeScale);
        }

        if (data.effectColliderLayerDirty)
        {
            EffectUtily.SetEffectColliderLayer(data.effectColliderLayer, m_effectNode.effectColliders);
        }
        if (data.broadcastfuncNameDirty)
        {
            m_effectPrefab.BroadcastMessage(data.broadcastfuncName, SendMessageOptions.DontRequireReceiver);
        }
        if (data.AddhitEventDirty)
        {
            m_effectNode.hitEvent += data.AddhitEvent;
        }
        if (data.ReducehitEventDirty)
        {
            m_effectNode.hitEvent -= data.ReducehitEvent;
        }

        if (data.AddAttackCollisionHandleDirty)
        {
            if(m_AttackGroup)
            {
                m_AttackGroup.collisionEvent += data.AddAttackCollisionHandle;
            }
        }

        if (data.ReduceAttackCollisionHandleDirty)
        {
            if (m_AttackGroup)
            {
                m_AttackGroup.collisionEvent -= data.ReduceAttackCollisionHandle;
            }
        }
        if (data.Attack_nFeedBackIDDirty)
        {
            if (m_AttackGroup)
            {
                m_AttackGroup.nFeedBackID = data.Attack_nFeedBackID;
            }
        }
        if (data.Attack_nAttackEntityIDDirty)
        {
            if (m_AttackGroup)
            {
                m_AttackGroup.nAttackEntityID = data.Attack_nAttackEntityID;
            }
        }
        if (data.Attack_bEnableCollisionDirty)
        {
            if (m_AttackGroup)
            {
                m_AttackGroup.bEnableCollision = data.Attack_bEnableCollision;
            }
        }

        if (data.Attack_eventHandlerCountDirty)
        {
            if (m_AttackGroup)
            {
                m_AttackGroup.eventHandlerCount = data.Attack_eventHandlerCount;
            }
        }




        if (data.bPlayDirty || data.isEnemyDirty || data.entityIDDirty || data.SrcTransformDirty || data.DstTransformDirty || data.playMaskDirty || data.bRePlayDirty)
        {
            if (data.bRePlay)
            {
                m_effectNode.StopEffect();
            }

            if (data.bPlay || data.bRePlay)
            {
                m_effectNode.PlayEffect(data.isEnemy, data.entityID, data.SrcTransform, data.DstTransform, data.playMask);
            }
            else
            {
                m_effectNode.StopEffect();
            }
        }
        data.ClearDirtyData();
    }


    /// <summary>
    /// 渲染这个光效
    /// </summary>
    /// <param name="pos"></param>
    public void ActiveCacheRendering(Vector3 pos)
    {
        if (Valid && m_effectNode)
        {
            effectPrefab.SetActive(true);
            m_effectNode.transform.SetPosition(pos);
            m_effectNode.PlayEffect();
        }
    }

    /// <summary>
    /// 不渲染这个光效
    /// </summary>
    /// <param name="pos"></param>
    public void InvActiveCacheRendering(Vector3 pos)
    {
        if (Valid && m_effectNode)
        {
            m_effectNode.StopEffect();
            m_effectNode.ForceDeleteTestModel();
            effectPrefab.SetActive(false);
            m_effectNode.transform.SetPosition(pos);
            
        }
    }

    public void Clear()
    {
        m_effect_ReplaceAnimation = null;
        m_effectNode = null;
        m_effectPrefab = null;
        m_effectConfig = null;
        m_effectPrefabTransform = null;
        m_EffectID = -1;
    }

    /// <summary>
    /// 是否是空的，注意一下，有的用的是替换动画,有的用的是EffectNode
    /// </summary>
    /// <param name="instance"></param>
    /// <returns></returns>
    public static bool isNullOrEmpry(LightingEffectResHandleInstance instance)
    {
        if(null == instance)
        {
            return true;
        }

        return !instance.Valid;
    }

    public static void BuildInstance(LightingEffectResHandle node,ref LightingEffectResHandleInstance instance)
    {
        if (null == instance)
        {
            instance = new LightingEffectResHandleInstance();
        }
        instance.m_EffectID = node.EffectID;
        instance.m_effectConfig = node.effectConfig;
        return ;
    }

    public static void FillInstance(Transform parent,LightingEffectResHandle node,LightingEffectResHandleInstance instance)
    {
        if (LightingEffectResHandle.isNullOrEmpry(node))
        {
            return;
        }

        if(null == instance)
        {
            return;
        }
        if(instance.Valid)
        {
            return;
        }

        instance.m_EffectID = node.EffectID;
        instance.m_effectConfig = node.effectConfig;


        instance.m_effectPrefab = node.prefabResNode.InstanceMainRes(); 
        instance.m_AttackGroup = instance.m_effectPrefab.GetComponent<AttackCollisionGroup>();
        instance.m_effect_ReplaceAnimation = instance.m_effectPrefab.GetComponent<Effect_ReplaceAnimation>();
        instance.m_effect_CameraAnimation = instance.m_effectPrefab.GetComponent<Effect_CameraAnimation>();
        instance.m_effectNode = instance.m_effectPrefab.GetComponent<EffectNode>();
        if(instance.m_effectNode)
        {
            instance.m_effectNode.isStaticEffect = false;
            instance.m_effectNode.PlayOnAwake = false;
            instance.m_effectNode.Init();
            instance.m_effectNode.StopEffect();
        }
        
        instance.m_effectPrefabTransform = instance.m_effectPrefab.transform;
        instance.m_effectPrefabTransform.SetParent(parent);
        instance.m_effectPrefabTransform.position = Vector3.one * 999.0f;
        instance.m_effectPrefab.name = node.prefabResNode.AssetName + "(effect" + node.EffectID + ")";
        
    
       
        
    }
}