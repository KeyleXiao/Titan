using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Effect;

// 光效工厂 
// 全局单一类，整个游戏只有一个实例
// 负责载入光效配置，创建光效

/// <summary>
/// 播放参数
/// </summary>
public class EffectResHandlePlayData
{
    public Transform parent
    {
        get
        {
            return m_parent;
        }
        set
        {
            m_parent = value;
            m_ParentDirty = true;
        }
    }
    private Transform m_parent = null;
    public bool ParentDirty
    {
        get
        {
            return m_ParentDirty;
        }
    }
    private bool m_ParentDirty = false;

    public float Speed
    {
        get
        {
            return m_Speed;
        }
        set
        {
            m_Speed = value;
            m_SpeedDirty = true;
        }
    }
    private float m_Speed = 1.0f;
    public bool SpeedDirty
    {
        get
        {
            return m_SpeedDirty;
        }
    }
    private bool m_SpeedDirty = false;
    

    public float SizeScale
    {
        get
        {
            return m_SizeScale;
        }
        set
        {
            m_SizeScale = value;
            m_SizeScaleDirty = true;
        }
    }
    private float m_SizeScale = 1.0f;
    public bool SizeScaleDirty
    {
        get
        {
            return m_SizeScaleDirty;
        }
    }
    private bool m_SizeScaleDirty = false;

    public bool isEnemy
    {
        get
        {
            return m_isEnemy;
        }
        set
        {
            m_isEnemy = value;
            m_isEnemyDirty = true;
        }
    }
    private bool m_isEnemy = false;
    public bool isEnemyDirty
    {
        get
        {
            return m_isEnemyDirty;
        }
    }
    private bool m_isEnemyDirty = false;

    public int entityID
    {
        get
        {
            return m_entityID;
        }
        set
        {
            m_entityID = value;
            m_entityIDDirty = true;
        }
    }
    private int m_entityID = -1;
    public bool entityIDDirty
    {
        get
        {
            return m_entityIDDirty;
        }
    }
    private bool m_entityIDDirty = false;

    public Transform SrcTransform
    {
        get
        {
            return m_SrcTransform;
        }
        set
        {
            m_SrcTransform = value;
            m_SrcTransformDirty = true;
        }
    }
    private Transform m_SrcTransform = null;
    public bool SrcTransformDirty
    {
        get
        {
            return m_SrcTransformDirty;
        }
    }
    private bool m_SrcTransformDirty = false;

    public Transform DstTransform
    {
        get
        {
            return m_DstTransform;
        }
        set
        {
            m_DstTransform = value;
            m_DstTransformDirty = true;
        }
    }
    private Transform m_DstTransform = null;
    public bool DstTransformDirty
    {
        get
        {
            return m_DstTransformDirty;
        }
    }
    private bool m_DstTransformDirty = false;

    public int effectColliderLayer
    {
        get
        {
            return m_effectColliderLayer;
        }
        set
        {
            m_effectColliderLayer = value;
            m_effectColliderLayerDirty = true;
        }
    }
    private int m_effectColliderLayer = -1;
    public bool effectColliderLayerDirty
    {
        get
        {
            return m_effectColliderLayerDirty;
        }
    }
    private bool m_effectColliderLayerDirty = false;

    public string broadcastfuncName
    {
        get
        {
            return m_broadcastfuncName;
        }
        set
        {
            m_broadcastfuncName = value;
            m_broadcastfuncNameDirty = true;
        }
    }
    private string m_broadcastfuncName = string.Empty;
    public bool broadcastfuncNameDirty
    {
        get
        {
            return m_broadcastfuncNameDirty;
        }
    }
    private bool m_broadcastfuncNameDirty = false;

    public int playMask
    {
        get
        {
            return m_playMask;
        }
        set
        {
            m_playMask = value;
            m_playMaskDirty = true;
        }
    }
    private int m_playMask = 0;
    public bool playMaskDirty
    {
        get
        {
            return m_playMaskDirty;
        }
    }
    private bool m_playMaskDirty = false;

    public bool bPlay
    {
        get
        {
            return m_bPlay;
        }
        set
        {
            m_bPlay = value;
            m_bPlayDirty = true;
        }
    }
    private bool m_bPlay = false;
    public bool bPlayDirty
    {
        get
        {
            return m_bPlayDirty;
        }
    }
    private bool m_bPlayDirty = false;

    public bool bRePlay
    {
        get
        {
            return m_bRePlay;
        }
        set
        {
            m_bRePlay = value;
            m_bRePlayDirty = true;
        }
    }
    private bool m_bRePlay = false;
    public bool bRePlayDirty
    {
        get
        {
            return m_bRePlayDirty;
        }
    }
    private bool m_bRePlayDirty = false;

    public System.Action<Collider> AddhitEvent
    {
        get
        {
            return m_AddhitEvent;
        }
        set
        {
            m_AddhitEvent = value;
            m_AddhitEventDirty = true;
        }
    }
    private System.Action<Collider> m_AddhitEvent = null;
    public bool AddhitEventDirty
    {
        get
        {
            return m_AddhitEventDirty;
        }
    }
    private bool m_AddhitEventDirty = false;

    public System.Action<Collider> ReducehitEvent
    {
        get
        {
            return m_ReducehitEvent;
        }
        set
        {
            m_ReducehitEvent = value;
            m_ReducehitEventDirty = true;
        }
    }
    private System.Action<Collider> m_ReducehitEvent = null;
    public bool ReducehitEventDirty
    {
        get
        {
            return m_ReducehitEventDirty;
        }
    }
    private bool m_ReducehitEventDirty = false;

    public CollisionEventHandler ReduceAttackCollisionHandle
    {
        get
        {
            return m_ReduceAttackCollisionHandle;
        }
        set
        {
            m_ReduceAttackCollisionHandle = value;
            m_ReduceAttackCollisionHandleDirty = true;
        }
    }
    private CollisionEventHandler m_ReduceAttackCollisionHandle = null;
    public bool ReduceAttackCollisionHandleDirty
    {
        get
        {
            return m_ReduceAttackCollisionHandleDirty;
        }
    }
    private bool m_ReduceAttackCollisionHandleDirty = false;

    public CollisionEventHandler AddAttackCollisionHandle
    {
        get
        {
            return m_AddAttackCollisionHandle;
        }
        set
        {
            m_AddAttackCollisionHandle = value;
            m_AddAttackCollisionHandleDirty = true;
        }
    }
    private CollisionEventHandler m_AddAttackCollisionHandle = null;
    public bool AddAttackCollisionHandleDirty
    {
        get
        {
            return m_AddAttackCollisionHandleDirty;
        }
    }
    private bool m_AddAttackCollisionHandleDirty = false;

    public int Attack_nFeedBackID
    {
        get
        {
            return m_Attack_nFeedBackID;
        }
        set
        {
            m_Attack_nFeedBackID = value;
            m_Attack_nFeedBackIDDirty = true;
        }
    }
    private int m_Attack_nFeedBackID = 0;
    public bool Attack_nFeedBackIDDirty
    {
        get
        {
            return m_Attack_nFeedBackIDDirty;
        }
    }
    private bool m_Attack_nFeedBackIDDirty = false;

    public int Attack_nAttackEntityID
    {
        get
        {
            return m_Attack_nAttackEntityID;
        }
        set
        {
            m_Attack_nAttackEntityID = value;
            m_Attack_nAttackEntityIDDirty = true;
        }
    }
    private int m_Attack_nAttackEntityID = 0;
    public bool Attack_nAttackEntityIDDirty
    {
        get
        {
            return m_Attack_nAttackEntityIDDirty;
        }
    }
    private bool m_Attack_nAttackEntityIDDirty = false;

    public bool Attack_bEnableCollision
    {
        get
        {
            return m_Attack_bEnableCollision;
        }
        set
        {
            m_Attack_bEnableCollision = value;
            m_Attack_bEnableCollisionDirty = true;
        }
    }
    private bool m_Attack_bEnableCollision = false;
    public bool Attack_bEnableCollisionDirty
    {
        get
        {
            return m_Attack_bEnableCollisionDirty;
        }
    }
    private bool m_Attack_bEnableCollisionDirty = false;

    public int Attack_eventHandlerCount
    {
        get
        {
            return m_Attack_eventHandlerCount;
        }
        set
        {
            m_Attack_eventHandlerCount = value;
            m_Attack_eventHandlerCountDirty = true;
        }
    }
    private int m_Attack_eventHandlerCount = 0;
    public bool Attack_eventHandlerCountDirty
    {
        get
        {
            return m_Attack_eventHandlerCountDirty;
        }
    }
    private bool m_Attack_eventHandlerCountDirty = false;

    public void ClearDirtyData()
    {
        m_Attack_eventHandlerCountDirty = false;
        m_Attack_bEnableCollisionDirty = false;
        m_Attack_nAttackEntityIDDirty = false;
        m_Attack_nFeedBackIDDirty = false;
        m_ParentDirty = false;
        m_SpeedDirty = false;
        m_SizeScaleDirty = false;
        m_isEnemyDirty = false;
        m_entityIDDirty = false;
        m_SrcTransformDirty = false;
        m_DstTransformDirty = false;
        m_effectColliderLayerDirty = false;
        m_broadcastfuncNameDirty = false;
        m_playMaskDirty = false;
        m_bPlayDirty = false;
        m_AddhitEventDirty = false;
        m_AddAttackCollisionHandleDirty = false;
        m_ReducehitEventDirty = false;
        m_ReduceAttackCollisionHandleDirty = false;
        m_bRePlayDirty = false;
    }

    public void Clear()
    {
        ClearDirtyData();
        m_parent = null;
        m_Speed = 1.0f;
        m_SizeScale = 1.0f;
        m_isEnemy = false;
        m_entityID = -1;
        m_SrcTransform = null;
        m_DstTransform = null;
        m_effectColliderLayer = -1;
        m_broadcastfuncName = string.Empty;
        m_playMask = 0;
        m_bPlay = false;
        m_AddhitEvent = null;
        m_AddAttackCollisionHandle = null;
        m_ReducehitEvent = null;
        m_ReduceAttackCollisionHandle = null;
        m_Attack_nFeedBackID = 0;
        m_Attack_nAttackEntityID = 0;
        m_Attack_bEnableCollision = false;
        m_Attack_eventHandlerCount = 0;
        m_bRePlay = false;
    }
}

/// <summary>
/// 
/// </summary>
public class LightingEffectResHandle
{
    public enum loadingStage
    {
        Stage_Invalid,
        Stage_Res,
        Stage_Finish,
    }

    public loadingStage stage = loadingStage.Stage_Invalid;

    public float currCacheTime = 0.0f;

    /// <summary>
    /// 是否有效
    /// </summary>
    public bool isValid = false;


    /// <summary>
    /// 光效ID
    /// </summary>
    public int EffectID = -1;

    /// <summary>
    /// 光效配置资源结点
    /// </summary>
    public LightingEffect effectConfig = null;

    /// <summary>
    /// 光效预制体资源节点
    /// </summary>
    public ResNode prefabResNode;

    public Queue<LightingEffectResHandleInstance> CacheList
    {
        get
        {
            return m_CacheList;
        }
    }
    private Queue<LightingEffectResHandleInstance> m_CacheList = new Queue<LightingEffectResHandleInstance>();

    public List<LightingEffectResHandleInstance> AllInstanceNode
    {
        get
        {
            return m_AllInstanceNode;
        }
    }
    private List<LightingEffectResHandleInstance> m_AllInstanceNode = new List<LightingEffectResHandleInstance>(10);

    private int ReferenceCount = 0;
    private Transform currCahceTransform = null;
    private Vector3 currCachePos = Vector3.zero;
    public int AddRef()
    {
        ReferenceCount++;
        return ReferenceCount;
    }

    public int Release()
    {
        ReferenceCount--;
        ReferenceCount = Mathf.Max(0, ReferenceCount);
        return ReferenceCount;
    }

    public void OnLoadFinish(Transform CacheParent,ResNode effectPrefabRes)
    {
        AssetBundleManager.SaveMemory(effectPrefabRes);
        prefabResNode = effectPrefabRes;
        stage = loadingStage.Stage_Finish;
        Init(CacheParent);
        foreach (LightingEffectResHandleInstance instance in m_AllInstanceNode)
        {
            LightingEffectResHandleInstance.FillInstance(currCahceTransform,this, instance);
            if(instance.isCached)
            {
                Quaternion qtn = Quaternion.identity;
                if(prefabResNode != null)
                {
                    qtn = prefabResNode.GetResRotation();
                }
                instance.OnCache(currCahceTransform, currCachePos, qtn);
            }
        }
    }

    public void CacheInstance(LightingEffectResHandleInstance instance, Transform cacheParent, Vector3 cachePos)
    {
        if(null == instance)
        {
            return;
        }
        if(instance.isCached)
        {
            return;
        }
        currCahceTransform = cacheParent;
        currCachePos = cachePos;
        Quaternion qtn = Quaternion.identity;
        if(prefabResNode != null)
        {
            qtn = prefabResNode.GetResRotation();
        }
        instance.OnCache(cacheParent, cachePos, qtn);
        m_CacheList.Enqueue(instance);
    }

    public void ActiveCacheRendering(Vector3 pos)
    {
        if (!isValid)
        {
            return;
        }
        foreach(LightingEffectResHandleInstance instance in AllInstanceNode)
        {
            if(!LightingEffectResHandleInstance.isNullOrEmpry(instance))
            {
                instance.ActiveCacheRendering(pos);
            }
        }
    }

    public void InvActiveCacheRendering(Vector3 pos)
    {
        if(!isValid)
        {
            return;
        }
        foreach (LightingEffectResHandleInstance instance in AllInstanceNode)
        {
            if (!LightingEffectResHandleInstance.isNullOrEmpry(instance))
            {
                instance.InvActiveCacheRendering(pos);
            }
        }

    }

    public LightingEffectResHandleInstance Instantiate()
    {
        LightingEffectResHandleInstance instance = null;
        if(m_CacheList.Count > 0)
        {
            instance = m_CacheList.Dequeue();
            //先看看是不是加载完成了。加载完成之后，再看看是否有效
            if (stage == loadingStage.Stage_Finish)
            {
                if (isValid)
                {
                    if(!instance.Valid)
                    {
                        instance.Clear();
                        LightingEffectResHandleInstance.FillInstance(currCahceTransform,this, instance);
                    }
                }
            }
        }
        else
        {
            LightingEffectResHandleInstance.BuildInstance(this, ref instance);
            m_AllInstanceNode.Add(instance);
            //先看看是不是加载完成了。加载完成之后，再看看是否有效
            if (stage == loadingStage.Stage_Finish)
            {
                if (isValid)
                {
                    LightingEffectResHandleInstance.FillInstance(currCahceTransform,this, instance);
                }
            }
        }
        instance.isCached = false;
        return instance;
    }

    public void Clear()
    {
        foreach (LightingEffectResHandleInstance instance in m_AllInstanceNode)
        {
            GameObject temp = instance.effectPrefab;
            if (!ResNode.isNullOrEmpty(prefabResNode))
            {
                prefabResNode.DestroyInstanceRes(ref temp);
            }
            instance.Clear();
        }
        m_CacheList.Clear();
        m_AllInstanceNode.Clear();
        effectConfig = null;
        prefabResNode = null;
        ReferenceCount = 0;
        isValid = false;
        stage = loadingStage.Stage_Invalid;
        currCacheTime = 0.0f;
    }

    public void Init(Transform CacheParent)
    {
        if (stage != loadingStage.Stage_Finish)
        {
            return;
        }

        if(ResNode.isNullOrEmpty(prefabResNode) || !effectConfig)
        {
            return;
        }
        currCahceTransform = CacheParent;
        isValid = true;
    }

    public static bool isNullOrEmpry(LightingEffectResHandle node)
    {
        if(null == node)
        {
            return true;
        }

        return !node.isValid;
    }
}