using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Effect;


public class Pre_Load_Light_Info
{
    public ASpeedGame.Data.Scheme.LightType nLightType = ASpeedGame.Data.Scheme.LightType.Unknown;//光效类型
    public int nMask = 0;//标志位
    public List<int> effectIDList = new List<int>();
};

// 光效工厂 
// 全局单一类，整个游戏只有一个实例
// 负责载入光效配置，创建光效
public partial class LightingEffectFactory : MonoBehaviour
{
    /// <summary>
    /// 创建距离，如果超过这个距离，不创建光效了
    /// </summary>
    public static int CreateDistance
    {
        get
        {
            if(ClosedOptimiezed)
            {
                return 365;
            }

            else
            {
                return 65;
            }
        }
    }

    

    public static bool ClosedOptimiezed = false;
    [System.NonSerialized]
    public SelectEffectManager selectEffects = null;
    [System.NonSerialized]
    public SelectEffectManager selectTips = null;

    [System.NonSerialized]
    public GameObject StaticEffectRoot = null;

    [System.NonSerialized]
    public GameObject EffectCacheRoot = null;
     [System.NonSerialized]
    public Vector3 CachePos = Vector3.one * 99000;


    private bool isValid = false;

    /// <summary>
    /// 只会在程序初始化的时候调用
    /// </summary>
    public void InitWhenGameStart()
    {
        if (null == EffectCacheRoot)
        {
            EffectCacheRoot = new GameObject();
            EffectCacheRoot.name = "LightingEffectCache";
            GameObject.DontDestroyOnLoad(EffectCacheRoot);
        }
        selectEffects = new SelectEffectManager();
        selectEffects.initSelectEffectManager(transform, "selectEffects");
        selectTips = new SelectEffectManager();
        selectTips.initSelectEffectManager(transform, "selectTips");

        StaticEffectRoot = new GameObject();
        StaticEffectRoot.name = "StaticEffectRoot";
        Object.DontDestroyOnLoad(StaticEffectRoot);

        InitBehaviorTransformAgent();
        InitBehaviorDestroyTimerCache();



        InitAsyncWhenGameStart();
        isValid = true;
    }


    /// <summary>
    /// 在程序退出时候调用
    /// </summary>
    public void ClearWhenGameEnd()
    {
        ClearAllPreWarmData();
        ClearBehaviorTransformAgent();
        ClearAsyncWhenGameEnd();
        if (selectTips != null)
        {
            selectTips.clearEffects();

        }
        if (selectEffects != null)
        {
            selectEffects.clearEffects();
        }

        ClearBehaviorDestroyTimerCache();
        ClearCachingLightingBehavior();
        isValid = false;

    }

    /// <summary>
    /// 这个初始化可能被调用多次
    /// </summary>
    public void InitCache()
    {
        if(!isValid)
        {
            return;
        }
    }


    /// <summary>
    /// 这个清楚可能会被调用多次
    /// </summary>
    public void ClearCache()
    {
        if (!isValid)
        {
            return;
        }
        if (m_DeadEffect)
        {
            m_DeadEffect.StopEffect();
        }

        foreach( LinkedList<LightingEffectBehavior> list in m_allBehaviorTable.Values)
        {
            foreach (LightingEffectBehavior bev in list)
            {
                bev.SetInvalid();
                bev.ResetAllMembers();
                bev.ClearData();
            }
        }
        m_allBehaviorTable.Clear();
        m_behaviorCacheTable.Clear();
        ClearEntityBindEffectHostID();
        ClearAsyncData();
    }


    public void ClearCache(IEnumerable<int> IDList)
    {
        if (!isValid)
        {
            return;
        }
        foreach(int id in IDList)
        {
            ClearSingleCache(id);
        }
    }

    public void ClearSingleCache(int ID)
    {
        if (!isValid)
        {
            return;
        }
        RemovePreWarmEffectQueue(ID);
        if(m_behaviorCacheTable.ContainsKey(ID))
        {
            m_behaviorCacheTable.Remove(ID);
        }

        LinkedList<LightingEffectBehavior> list;
        if(m_allBehaviorTable.TryGetValue(ID,out list))
        {
            foreach(LightingEffectBehavior bev in list)
            {
                LightingEffectBehavior temp = bev;
                DestroyBehavior(ref temp);
            }

            m_allBehaviorTable.Remove(ID);
        }
    }

    /// <summary>
    /// 缓存启用变化
    /// </summary>
    /// <param name="bEnable"></param>
    public void OnOptimizeEnabledChange(bool bEnable)
    {
        if (!isValid)
        {
            return;
        }
        ClearCache();
    }

    #region EffectBehaviorDestroyTimer相关

    //private Queue<EffectBehaviorDestroyTimer> m_UnUsedBehaviorDestroyTimer = new Queue<EffectBehaviorDestroyTimer>();
    //private LinkedList<EffectBehaviorDestroyTimer> m_AllBehaviorDestroyTimer = new LinkedList<EffectBehaviorDestroyTimer>();

    //private const int BehaviorDestroyTimerCacheCout = 60;
    //private GameObject BehaviorDestroyTimerCacheRoot = null;

    public void InitBehaviorDestroyTimerCache()
    {
        //if(!BehaviorDestroyTimerCacheRoot)
        //{
        //    BehaviorDestroyTimerCacheRoot = new GameObject();
        //    BehaviorDestroyTimerCacheRoot.name = "EffectScriptCacheRoot";
        //    GameObject.DontDestroyOnLoad(BehaviorDestroyTimerCacheRoot);

        //    for (int i = 0; i < BehaviorDestroyTimerCacheCout; i++)
        //    {
        //        EffectBehaviorDestroyTimer s = BehaviorDestroyTimerCacheRoot.AddComponent<EffectBehaviorDestroyTimer>();
        //        s.enabled = false;
        //        m_UnUsedBehaviorDestroyTimer.Enqueue(s);
        //        m_AllBehaviorDestroyTimer.AddLast(s);
        //    }
        //}

    }

    public void ClearBehaviorDestroyTimerCache()
    {

        //foreach (EffectBehaviorDestroyTimer s in m_AllBehaviorDestroyTimer)
        //{
        //    if(s)
        //    {
        //        EffectBehaviorDestroyTimer temp = s;
        //        ResNode.DestroyRes(ref temp);
        //    }
            
        //}
        //m_UnUsedBehaviorDestroyTimer.Clear();
        //m_AllBehaviorDestroyTimer.Clear();

    }

    public EffectBehaviorDestroyTimer GetBehaviorDestroyTimer()
    {
        return null;
        //EffectBehaviorDestroyTimer s = null;
        //if (m_UnUsedBehaviorDestroyTimer.Count > 0)
        //{
        //    s = m_UnUsedBehaviorDestroyTimer.Dequeue();
        //    s.enabled = true;
        //}
        //else
        //{
        //    s = BehaviorDestroyTimerCacheRoot.AddComponent<EffectBehaviorDestroyTimer>();
        //}

        //return s;
    }

    public void CacheBehaviorDestroyTimer(EffectBehaviorDestroyTimer script)
    {
        //if(!script)
        //{
        //    return;
        //}
        //script.destroyTick = 0;
        //script.hostEffect = null;
        //script.enabled = false;
        //m_UnUsedBehaviorDestroyTimer.Enqueue(script);
    }
    #endregion

    #region 光效相关

    private Dictionary<int, LinkedList<LightingEffectBehavior>> m_allBehaviorTable = new Dictionary<int, LinkedList<LightingEffectBehavior>>();
    private Dictionary<int, Queue<LightingEffectBehavior>> m_behaviorCacheTable = new Dictionary<int, Queue<LightingEffectBehavior>>();

    public bool CheckGolbalEffect(int nID)
    {
        if (!isValid)
        {
            return false;
        }
        LightingEffect reslut = ASpeedGame.Data.Scheme.SchemeLightingEffect.Instance.GetLightingEffect(nID);
        if (!reslut)
        {
             return false;
        }
        return reslut.bGlobal;
    }

    /// <summary>
    /// 创建光效
    /// </summary>
    /// <param name="nID"></param>
    /// <param name="param"></param>
    /// <returns></returns>
    public LightingEffectBehavior CreateEffect(int nID, LightEffectParam param,bool isFromHero = false)
    {
        if (!isValid)
        {
            return null;
        }
        if (nID <= 0)
        {
            return null;
        }
        LightingEffectBehavior reslut = null;
        LightingEffect config = ASpeedGame.Data.Scheme.SchemeLightingEffect.Instance.GetLightingEffect(nID);
        if (!config)
        {
            Trace.Warning("LightingEffect creates failed. Id " + nID.ToString() + "has not loaded.");
            return reslut;

        }

        //if(!SceneManager.Instance.isMainCity())
        //{
        //    if (!FirstCachedEffectID.Contains(nID))
        //    {
        //        if (!currentCachedEffectID.Contains(nID))
        //        {
        //            FirstCachedEffectID.Add(nID);
        //            Debug.LogWarning("创建了没有缓存的光效，ID:" + nID);
        //        }
        //    }
        //}

        if (!isCanCreateEffect(config, param))
        {
            return reslut;
        }
        RemovePreWarmEffectQueue(nID);
        Queue<LightingEffectBehavior> cacheList = null;

        if (!m_behaviorCacheTable.TryGetValue(nID,out cacheList))
        {
            cacheList = new Queue<LightingEffectBehavior>();
            m_behaviorCacheTable.Add(nID, new Queue<LightingEffectBehavior>());
        }

        if (cacheList.Count > 0)
        {
            reslut = cacheList.Dequeue();
            ResetBehavior(reslut);
        }
        else
        {
            reslut = CreateNewBehavior(nID, config.GetEffectType());
        }
        reslut.FillData(config, param, GetResourceHandleAsync(nID, reslut.OnResourcesLoadFinish),AllocBehaviorTransformAgent());
        reslut.SetValid();
        PushToEntityEffectTable(reslut);
        return reslut;
    }

    private LightingEffectBehavior CreateNewBehavior(int effectID,LightingEffectConfigType t)
    {
        if (!isValid)
        {
            return null;
        }

        LightingEffectBehavior reslut = null;
        switch (t)
        {
            case LightingEffectConfigType.AniReplace:
                reslut = new LightingEffectBehavior_AnimationReplace();
                break;
            case LightingEffectConfigType.Area:
                reslut = new LightingEffectBehavior_Area();
                break;
            case LightingEffectConfigType.Bind:
                reslut = new LightingEffectBehavior_Bind();
                break;
            case LightingEffectConfigType.Camera:
                reslut = new LightingEffectBehavior_Camera();
                break;
            case LightingEffectConfigType.Fly:
                reslut = new LightingEffectBehavior_Fly();
                break;
            case LightingEffectConfigType.Invalid:
                break;
            //case LightingEffectConfigType.MatReplace:
            //    reslut = new LightingEffectBehavior_MaterialReplace();
            //    break;
            case LightingEffectConfigType.Shadow:
                reslut = new LightingEffectBehavior_Shadow();
                break;
            case LightingEffectConfigType.Sound:
                reslut = new LightingEffectBehavior_Sound();
                break;
            case LightingEffectConfigType.Speed:
                reslut = new LightingEffectBehavior_Speed();
                break;
            case LightingEffectConfigType.Ward:
                reslut = new LightingEffectBehavior_Ward();
                break;
        }
        if(null == reslut)
        {
            Debug.LogError("CreateNewBehavior faild,找不到配置类型:" + t);
        }
        else
        {
            LinkedList<LightingEffectBehavior> list = null;
            if(!m_allBehaviorTable.TryGetValue(effectID,out list))
            {
                list = new LinkedList<LightingEffectBehavior>();
                m_allBehaviorTable.Add(effectID, list);
            }
            list.AddLast(reslut);
        }
        return reslut;
    }

    /// <summary>
    /// 缓存光效配置
    /// </summary>
    /// <param name="effect"></param>
    public void CacheEffectBehavior(ref LightingEffectBehavior behaviorInstance)
    {
        if (!isValid)
        {
            return ;
        }
        if (behaviorInstance == null)
        {
            return;
        }
        
        int ID = behaviorInstance.getID();
        if(ID <= 0)
        {
            return;
        }
        //对于没有在缓存表配置的ID来说，全部采用删除处理，不缓存，因为缓存了之后，主城切换皮肤的时候是不会被删除的，这样会导致越来越多光效遗留
        if(GameLogicAPI.isMainMap(SceneManager.Instance.getCurrentMapId()))
        {
            if (!allCacheID.Contains(ID))
            {
                
                DestroyBehavior(ref behaviorInstance);
                return;
            }
        }

        Queue<LightingEffectBehavior> list = null;
        if (!m_behaviorCacheTable.TryGetValue(ID, out list))
        {
            list = new Queue<LightingEffectBehavior>();
            m_behaviorCacheTable.Add(ID, list);
        }
        RemoveFromEntityEffectTable(behaviorInstance);
        list.Enqueue(behaviorInstance);
        ResetBehavior(behaviorInstance);
    }

    void DestroyBehavior(ref LightingEffectBehavior instance)
    {
        if (!isValid)
        {
            return;
        }
        RemoveFromEntityEffectTable(instance);

        LightingEffectResHandleInstance temp = instance.ResourceHandle;
        DeleteResourceHandleAsync(ref temp, instance.OnResourcesLoadFinish);
        DeleteBehaviorTransformAgent(instance.TransformAgent);
        instance.ClearData();
        instance.ResetAllMembers();
        instance.SetInvalid();
        instance = null;
    }

    public void ResetBehavior(LightingEffectBehavior instance)
    {
        if (!isValid)
        {
            return;
        }
        if(null == instance)
        {
            return;
        }
        int effectID = instance.getID();
        if(effectID > 0)
        {
            LightingEffectResHandle handle = AllocResourcesHandle(instance.getID());
            handle.CacheInstance(instance.ResourceHandle, EffectCacheRoot.transform, CachePos);
            handle.Release();
        }

        DeleteBehaviorTransformAgent(instance.TransformAgent);
        //CacheBehaviorDestroyTimer(instance.BehaviorDestroyTimer);
        instance.ResetAllMembers();
        instance.SetInvalid();
    }

    void ClearCachingLightingBehavior()
    {
        if (!isValid)
        {
            return;
        }
        m_behaviorCacheTable.Clear();
    }


    #endregion

    #region 单一光效相关

    /// <summary>
    /// 实体上的绑定光效 [实体ID,[光效ID-对应的光效实例列表]]
    /// </summary>
    private Dictionary<int, Dictionary<int, List<LightingEffectBehavior>>> m_EntityBindEffectHostID = new Dictionary<int, Dictionary<int, List<LightingEffectBehavior>>>();


    /// <summary>
    /// 实体和他身上的受击光效
    /// </summary>
    private Dictionary<int, int> m_HitEffectTable = new Dictionary<int, int>();

    /// <summary>
    /// 这个光效是否可以创建
    /// </summary>
    /// <param name="effect"></param>
    /// <param name="param"></param>
    private bool isCanCreateEffect(LightingEffect effect,LightEffectParam param)
    {
        if (!isValid)
        {
            return false;
        }
        if (ClosedOptimiezed)
        {
            return true;
        }
        //是single类型的，梁成说必须要创建
        if(effect.bSingle)
        {
            return true;
        }
        //只对绑定光效作这个效果
        if (effect.GetEffectType() != LightingEffectConfigType.Bind)
        {
            return true;
        }

        LightingEffect_Bind bindeffect = effect as LightingEffect_Bind;

        bool Reslut = isCanCreateEffect_Single(effect.nID, param.nSrcEntityID, bindeffect.bIgnoreSingleEffectOptimtized, param) /*&&
                      isCanCreateEffect_OnView(param.nSrcEntityID)*/;
        return Reslut;
        
    }

    private void RemoveFromEntityEffectTable(LightingEffectBehavior instance)
    {
        if (!isValid)
        {
            return;
        }
        if (ClosedOptimiezed)
        {
            return;
        }
        if(null == instance)
        {
            return;
        }
        if(null == instance.EffectConfig || null == instance.EffectParam)
        {
            return;
        }

        int effectID = instance.getID();
        if (effectID <= 0)
        {
            return;
        }
        LightEffectParam param = instance.EffectParam;
        //是single类型的，梁成说必须要创建
        if (instance.EffectConfig.bSingle)
        {
            return;
        }
        //只对绑定光效作这个效果
        if (instance.EffectConfig.GetEffectType() != LightingEffectConfigType.Bind)
        {
            return;
        }

        int entityID = param.nSrcEntityID;
        Dictionary<int, List<LightingEffectBehavior>> tables = null;
        List<LightingEffectBehavior> list = null;
        //已经有这个ID了
        if (m_EntityBindEffectHostID.TryGetValue(entityID, out tables))
        {
            if (tables.TryGetValue(effectID, out list))
            {
                list.Remove(instance);
                if(list.Count <= 0)
                {
                    tables.Remove(effectID);
                }
            }

            if(tables.Count <= 0)
            {
                m_EntityBindEffectHostID.Remove(entityID);
            }
        }
    }

    private void PushToEntityEffectTable(LightingEffectBehavior instance)
    {
        if (!isValid)
        {
            return;
        }
        if (ClosedOptimiezed)
        {
            return ;
        }
        if(null == instance)
        {
            return;
        }
        //是single类型的，梁成说必须要创建
        if (instance.EffectConfig.bSingle)
        {
            return;
        }
        //只对绑定光效作这个效果
        if (instance.EffectConfig.GetEffectType() != LightingEffectConfigType.Bind)
        {
            return;
        }

        int entityID = instance.EffectParam.nSrcEntityID;

        int effectID = instance.getID();
        if (effectID <= 0)
        {
            return;
        }
        Dictionary<int, List<LightingEffectBehavior>> tables = null;
        List<LightingEffectBehavior> list = null;
        //已经有这个ID了
        if (m_EntityBindEffectHostID.TryGetValue(entityID, out tables))
        {
            if(!tables.TryGetValue(effectID,out list))
            {
                list = new List<LightingEffectBehavior>();
                tables.Add(effectID, list);
            }
            list.Add(instance);
        }
        else
        {
            tables = new Dictionary<int, List<LightingEffectBehavior>>();
            list = new List<LightingEffectBehavior>();
            list.Add(instance);
            tables.Add(effectID, list);
            m_EntityBindEffectHostID.Add(entityID, tables);
        }
    }

    private bool isCanCreateEffect_OnView(int entityID)
    {
        U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(entityID);
        if(U3D_Render.EntityView.isNullOrEmpty(ev))
        {
            return false;
        }

        return ev.StateMachine.m_OnViewVisible;
    }

    private bool isCanCreateEffect_Single(int effectID,int entityID,bool bIgnore,LightEffectParam param)
    {
        if (!isValid)
        {
            return false;
        }
        /*主要是看同一个物体上是不是会有同一个光效，如果有同一个光效，就重新播放一次*/
        Dictionary<int, List<LightingEffectBehavior>> tables = null;

        //已经有这个ID了
        if (m_EntityBindEffectHostID.TryGetValue(entityID, out tables))
        {
            List<LightingEffectBehavior> effect = null;
            //这个光效已经有了
            if (tables.TryGetValue(effectID, out effect))
            {
                //如果这个配置,忽略这个单一光效优化
                if (bIgnore)
                {
                    U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(entityID);
                    if (null != ev)
                    {
                        //如果是玩家，则优化有效，如果是怪物，则继续跳过
                        if (ev.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
                        {
                            return true;
                        }
                    }
                }

                if(effect != null)
                {
                    while (true)
                    {
                        if (effect.Count <= 0)
                        {
                            break;
                        }

                        if (effect[0] != null)
                        {
                            if(effect[0].RePlay(param))
                            {
                                return false;
                            }
                           
                        }
                        effect.RemoveAt(0);
                    }
                }
                return true;
            }
            return true;
        }
        return true;
    }

    //private bool isCanCreateEffect_Hit(LightingEffect effect, LightEffectParam param)
    //{
    //    /*同一个实体，同一时间只能有MaxHitEffectCount个受击光效*/

    //    //不是受击光效
    //    if(effect.effectType !=  LightingEffect.LightingEffectType.Hit ||
    //       effect.bIgnoreSingleEffectOptimtized)
    //    {
    //        return true;
    //    }
        
    //    int maxCount = GetMaxHitCount(param.nSrcEntityID);
    //    if(maxCount <= 0)
    //    {
    //        return false;
    //    }
    //    int count = 0;
    //    if (m_HitEffectTable.TryGetValue(param.nSrcEntityID, out count))
    //    {
    //        count++;
    //        if (count >= maxCount)
    //        {
    //            return false;
    //        }
    //        m_HitEffectTable[param.nSrcEntityID] = count;
    //    }
    //    else
    //    {
    //        m_HitEffectTable.Add(param.nSrcEntityID, 1);
    //    }
    //    return true;
    //}

    //private static int GetMaxHitCount(int entityID)
    //{
    //    int reslut = 0;
    //    U3D_Render.EntityView ev = EntityFactory.getPlayerViewByID(entityID);
    //    bool isHero = entityID == EntityFactory.MainHeroID;
    //    bool isPlayer = ev != null;


    //    if (ImageSetting.currEffectQuality == ImageSetting.EffectQuality.QUALITY_FASTEST)
    //    {
    //        if(isPlayer)
    //        {
    //            reslut = 2;
    //            if(isHero)
    //            {
    //                reslut = 3;
    //            }
    //        }
    //    }

    //    if (ImageSetting.currEffectQuality == ImageSetting.EffectQuality.QUALITY_MEDIUM)
    //    {
    //        reslut = 1;
    //        if (isPlayer)
    //        {
    //            reslut = 3;
    //            if (isHero)
    //            {
    //                reslut = 4;
    //            }
    //        }
    //    }

    //    else if(ImageSetting.currEffectQuality == ImageSetting.EffectQuality.QUALITY_GOOD)
    //    {
    //        reslut = 2;
    //        if (isPlayer)
    //        {
    //            reslut = 4;
    //            if (isHero)
    //            {
    //                reslut = 5;
    //            }
    //        }
    //    }

    //    else if(ImageSetting.currEffectQuality == ImageSetting.EffectQuality.QUALITY_BEST)
    //    {
    //        reslut = 2;
    //        if (isPlayer)
    //        {
    //            reslut = 4;
    //            if (isHero)
    //            {
    //                reslut = 5;
    //            }
    //        }
    //    }
    //    return reslut;
    //}

    //private void RemoveEffectHostIDs(LightingEffect effect, LightEffectParam param)
    //{
    //    //是single类型的，梁成说不需要改
    //    if (effect.bSingle)
    //    {
    //        return;
    //    }

    //    //只对绑定光效作这个效果
    //    if (effect.GetType() != BindType)
    //    {
    //        return ;
    //    }

    //    HashSet<int> hostIDs = null;

    //    //已经有这个ID了
    //    if (m_EntityBindEffectHostID.TryGetValue(effect.getID(), out hostIDs))
    //    {
    //        int entityID = 0;
    //        if (effect.GetType() == BindType)
    //        {
    //            entityID = param.nSrcEntityID;
    //        }

    //        if(hostIDs.Contains(entityID))
    //        {
    //            hostIDs.Remove(entityID);
    //        }
    //    }

    //    int count = 0;
    //    if (m_HitEffectTable.TryGetValue(param.nSrcEntityID, out count))
    //    {
    //        count--;
    //        if(count <= 0)
    //        {
    //            m_HitEffectTable.Remove(param.nSrcEntityID);
    //        }
    //        else
    //        {
    //            m_HitEffectTable[param.nSrcEntityID] = count;
    //        }
            
    //    }
    //}
    private void ClearEntityBindEffectHostID()
    {
        if (!isValid)
        {
            return;
        }
        //m_HitEffectTable.Clear();
        m_EntityBindEffectHostID.Clear();
    }
    #endregion

    #region BehaviorTransformAgent

    LinkedList<Transform> m_AllBehaviorTransformAgent = new LinkedList<Transform>();
    Queue<Transform> m_UnUsedBehaviorTransformAgent = new Queue<Transform>();

    private void InitBehaviorTransformAgent()
    {
        for(int i = 0;i < 200;i++)
        {
            Transform agent = CreateNewBehaviorTransformAgent();
           
            m_UnUsedBehaviorTransformAgent.Enqueue(agent);
        }
    }

    public Transform AllocBehaviorTransformAgent()
    {
        
        if(m_UnUsedBehaviorTransformAgent.Count > 0)
        {
            return m_UnUsedBehaviorTransformAgent.Dequeue();
        }
        else
        {
            return CreateNewBehaviorTransformAgent();
        }
    }

    public void DeleteBehaviorTransformAgent(Transform agent)
    {
        if (!isValid)
        {
            return;
        }
        if(!agent)
        {
            return;
        }

        agent.SetParent(EffectCacheRoot.transform);
        agent.localPosition = Vector3.zero;
        agent.localRotation = Quaternion.identity;
        m_UnUsedBehaviorTransformAgent.Enqueue(agent);
    }

    private Transform CreateNewBehaviorTransformAgent()
    {
        GameObject agent = new GameObject();
        agent.name = "BehaviorTransformAgent";
        Object.DontDestroyOnLoad(agent);
        agent.transform.SetParent(EffectCacheRoot.transform);
        m_AllBehaviorTransformAgent.AddLast(agent.transform);
        return agent.transform;
    }

    private void ClearBehaviorTransformAgent()
    {

        if (!isValid)
        {
            return;
        }
        foreach(Transform tr in m_AllBehaviorTransformAgent)
        {
            GameObject.Destroy(tr);
        }

        m_AllBehaviorTransformAgent.Clear();
        m_UnUsedBehaviorTransformAgent.Clear();
    }

    #endregion

    #region 死亡特效

    private static ResNode DeadEffectRes = null;
    private static Effect.EffectNode m_DeadEffect = null;

    public static void DisableDeadEffect()
    {
        if (m_DeadEffect)
        {
            m_DeadEffect.StopEffect();
        }
    }

    public static void EnableDeadEffect()
    {
        if (DeadEffectRes == null)
        {
            DeadEffectRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "CommonPrefab/Effect/selectEffect/DeadEffect");
            if (null != DeadEffectRes)
            {
                m_DeadEffect = DeadEffectRes.InstanceMainRes<Effect.EffectNode>();
                m_DeadEffect.isStaticEffect = false;
                m_DeadEffect.PlayOnAwake = false;
                m_DeadEffect.Init();
                GameObject.DontDestroyOnLoad(m_DeadEffect.gameObject);
            }
        }
        if(m_DeadEffect)
        {
            m_DeadEffect.PlayEffect(false,-1,null,null);
        }
    }
    #endregion
}
