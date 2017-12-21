using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Effect;



public partial class LightingEffectFactory : MonoBehaviour
{
    class PreWarmNode
    {
        /// <summary>
        /// 光效ID-需要缓存的数量,在场景加载阶段
        /// </summary>
        private Dictionary<int, int> NeedToCachingEffectTable = new Dictionary<int, int>();
        private Dictionary<int, int> CurrCachingEffectTable = new Dictionary<int, int>();
        private List<int> completeList = new List<int>(50);

        public void Clear()
        {
            NeedToCachingEffectTable.Clear();
            CurrCachingEffectTable.Clear();
        }

        public void PushPreWarm(Pre_Load_Light_Info lightinfo)
        {
            foreach(int effectID in lightinfo.effectIDList)
            {
                //目前只需要人物的光效缓存不需要去重,有多少个缓存多少.而其他的只需缓存一个即可
                if (lightinfo.nLightType == ASpeedGame.Data.Scheme.LightType.ActorSkin)
                {
                    if (NeedToCachingEffectTable.ContainsKey(effectID))
                    {
                        NeedToCachingEffectTable[effectID]++;
                    }
                    else
                    {
                        NeedToCachingEffectTable.Add(effectID, 1);
                    }
                }
                else
                {
                    if (!NeedToCachingEffectTable.ContainsKey(effectID))
                    {
                        NeedToCachingEffectTable.Add(effectID, 1);
                    }
                }
            }
        }

        public void RemovePreWarmID(int nID)
        {
            if (NeedToCachingEffectTable.ContainsKey(nID))
            {
                NeedToCachingEffectTable.Remove(nID);
            }

            if (CurrCachingEffectTable.ContainsKey(nID))
            {
                CurrCachingEffectTable.Remove(nID);
            }
        }

        public void UpdatePreWarmProgress()
        {
            if (CurrCachingEffectTable.Count <= 0)
            {
                return;
            }
            completeList.Clear();
            foreach (int ids in CurrCachingEffectTable.Keys)
            {
                LightingEffectResHandle halde = LightingEffectFactory.Instance.AllocResourcesHandle(ids);
                if (halde.stage == LightingEffectResHandle.loadingStage.Stage_Finish)
                {
                    completeList.Add(ids);
                }
            }

            foreach (int ids in completeList)
            {
                CurrCachingEffectTable.Remove(ids);
            }
        }

        public void CachingEffect()
        {
            if (NeedToCachingEffectTable.Count <= 0)
            {
                return;
            }
            completeList.Clear();
            foreach (KeyValuePair<int, int> voc in NeedToCachingEffectTable)
            {
                if (LightingEffectFactory.Instance.PreWarmEffect(voc.Key, voc.Value))
                {
                    if (!CurrCachingEffectTable.ContainsKey(voc.Key))
                    {
                        CurrCachingEffectTable.Add(voc.Key, voc.Value);
                    }
                }
            }
            NeedToCachingEffectTable.Clear();
        }

        public bool IsPreWarmFinish()
        {
            return CurrCachingEffectTable.Count <= 0;
        }

        /// <summary>
        /// 激活cache渲染
        /// </summary>
        public void ActiveCacheRendering()
        {
            if (CurrCachingEffectTable.Count <= 0)
            {
                return;
            }
            Vector3 effectPos = Initialize.mainCam.transform.position;
            Vector3 pos = Vector3.zero;
            pos.z = 20;
            pos = Initialize.mainCam.transform.rotation * pos;
            effectPos += pos;
           
            foreach (int ids in CurrCachingEffectTable.Keys)
            {
                LightingEffectResHandle halde = LightingEffectFactory.Instance.AllocResourcesHandle(ids);
                halde.ActiveCacheRendering(effectPos);
            }
        }

        /// <summary>
        /// 关闭cache渲染
        /// </summary>
        public void InvActiveCacheRendering()
        {
            if (CurrCachingEffectTable.Count <= 0)
            {
                return;
            }
            foreach (int ids in CurrCachingEffectTable.Keys)
            {
                LightingEffectResHandle halde = LightingEffectFactory.Instance.AllocResourcesHandle(ids);
                halde.InvActiveCacheRendering(LightingEffectFactory.Instance.CachePos);

            }

        }
        
    }

    private HashSet<int> allCacheID = new HashSet<int>();

    /// <summary>
    /// 只会在场景加载阶段执行的part
    /// 第一优先级
    /// </summary>
    private PreWarmNode FirstPart = new PreWarmNode();

    /// <summary>
    /// 只会在场景加载阶段执行的part
    /// 第二优先级
    /// </summary>
    private PreWarmNode SecondPart = new PreWarmNode();

    /// <summary>
    /// 只会在场景加载阶段执行的part
    /// 第三优先级
    /// </summary>
    private PreWarmNode ThridPart = new PreWarmNode();

    /// <summary>
    /// 只会在场景加载阶段执行的part
    /// 第四优先级
    /// </summary>
    private PreWarmNode FourPart = new PreWarmNode();

    /// <summary>
    /// 正常的part，会实时加载的
    /// </summary>
    private PreWarmNode NormalPart = new PreWarmNode();

    /// <summary>
    /// 场景预加载队列是否激活
    /// </summary>
    private bool bPreWarmLoadingSceneQueueEnabled = false;


    /// <summary>
    /// 激活场景加载队列，这个队列是在场景加载的时候进行的，平时不会加载，速度比较快
    /// 尤其需要注意场景加载的时候会清楚缓存，如果预加载的时候，后面会去加载场景
    /// 应该调用此函数，否则预加载将会失败或者加载不完全，此函数的作用直到调用UnAcitvieLoadingSceneQueue为止
    /// </summary>
    public void AcitvieLoadingSceneQueue()
    {
        bPreWarmLoadingSceneQueueEnabled = true;
    }

    /// <summary>
    /// 反激活场景加载队列，后去的缓存将会移动到普通队列进行，普通队列加载的东西会
    /// 被场景加载时候清除缓存，导致缓存加载失败或者不全
    /// </summary>
    public void UnAcitvieLoadingSceneQueue()
    {
        bPreWarmLoadingSceneQueueEnabled = false;
    }


    private void PushPreWarmEffect(Pre_Load_Light_Info lightinfo)
    {
        foreach (int effectID in lightinfo.effectIDList)
        {
            if (lightinfo.nLightType == ASpeedGame.Data.Scheme.LightType.ActorSkin)
            {
                if (!allCacheID.Contains(effectID))
                {
                    allCacheID.Add(effectID);
                }
            }
        }

        if (bPreWarmLoadingSceneQueueEnabled)
        {
            //目前只需要人物的光效缓存不需要去重,有多少个缓存多少.而其他的只需缓存一个即可
            if (lightinfo.nLightType == ASpeedGame.Data.Scheme.LightType.ActorSkin)
            {
                //是自己的光效，在第一优先级
                if ((lightinfo.nMask & (int)CachePriority.PRIORITY_VERY_HIGH) != 0)
                {
                    FirstPart.PushPreWarm(lightinfo);
                }
                else if ((lightinfo.nMask & (int)CachePriority.PRIORITY_HIGH) != 0)//友方第二优先级
                {
                    SecondPart.PushPreWarm(lightinfo);
                }
                else if ((lightinfo.nMask & (int)CachePriority.PRIORITY_MEDIUM) != 0)//敌人第三优先级
                {
                    ThridPart.PushPreWarm(lightinfo);
                }
                else //其他的全部在第四优先级
                {
                    FourPart.PushPreWarm(lightinfo);
                }
            }
            else
            {
                //不是人物的，全部丢到优先级4队列
                FourPart.PushPreWarm(lightinfo);
            }

        }
        else
        {
            NormalPart.PushPreWarm(lightinfo);
        }
        

    }

    private bool PreWarmEffect(int nID,int PreWarmCount)
    {
        if(m_allBehaviorTable.ContainsKey(nID))
        {
            //Debug.LogWarning("光效已经加载，不需要缓存，ID:" + nID);
            return false;
        }
        LightingEffect config = ASpeedGame.Data.Scheme.SchemeLightingEffect.Instance.GetLightingEffect(nID);
        if (!config)
        {
            Debug.LogWarning("光效加载失败，找不到配置，ID:" + nID);
            return false;
        }

        LightingEffectResHandle handle = AllocResourcesHandle(nID);
        Queue<LightingEffectBehavior> list = null;

        if (!m_behaviorCacheTable.TryGetValue(nID, out list))
        {
            list = new Queue<LightingEffectBehavior>();
            m_behaviorCacheTable.Add(nID, list);
        }

        for (int i = 0; i < PreWarmCount; i++)
        {
            LightingEffectBehavior reslut = CreateNewBehavior(nID, config.GetEffectType());
            LightingEffectResHandleInstance instance = GetResourceHandleAsync(nID, reslut.OnResourcesLoadFinish);
            handle.CacheInstance(instance,EffectCacheRoot.transform,CachePos);
            handle.Release();//缓存不需要保留引用
            reslut.FillData(config, null, instance, null);
            list.Enqueue(reslut);
        }

        return true;
    }

    /// <summary>
    /// 从预加载队列中移除他
    /// </summary>
    /// <param name="nID"></param>
    private void RemovePreWarmEffectQueue(int nID)
    {
        NormalPart.RemovePreWarmID(nID);
        FirstPart.RemovePreWarmID(nID);
        SecondPart.RemovePreWarmID(nID);
        ThridPart.RemovePreWarmID(nID);
        FourPart.RemovePreWarmID(nID);
    }

    private void UpdatePreWarmData()
    {
        UpdatePreWarmQueue();

        UpdatePreWarmProgress();
    }

    /// <summary>
    /// 激活cache渲染
    /// </summary>
    public void ActiveCacheRendering()
    {
        EffectNode.UseTestModel = true;
        FirstPart.ActiveCacheRendering();
        SecondPart.ActiveCacheRendering();
        ThridPart.ActiveCacheRendering();
        FourPart.ActiveCacheRendering();

    }

    /// <summary>
    /// 关闭cache渲染
    /// </summary>
    public void InvActiveCacheRendering()
    {
        FirstPart.InvActiveCacheRendering();
        SecondPart.InvActiveCacheRendering();
        ThridPart.InvActiveCacheRendering();
        FourPart.InvActiveCacheRendering();

        Effect.EffectUtily.ClearTestModel();
        EffectNode.UseTestModel = false;
    }

    private void UpdatePreWarmProgress()
    {
        FirstPart.UpdatePreWarmProgress();
        SecondPart.UpdatePreWarmProgress();
        ThridPart.UpdatePreWarmProgress();
        FourPart.UpdatePreWarmProgress();
        NormalPart.UpdatePreWarmProgress();
    }


    private void UpdatePreWarmQueue()
    {
        NormalPart.CachingEffect();
    }

    public void CacheEffectsWhenLoadingScene_FirstPart()
    {
        if(!SceneManager.isLoadingScene)
        {
            return;
        }
        FirstPart.CachingEffect();
    }

    public void CacheEffectsWhenLoadingScene_SecondPart()
    {
        if (!SceneManager.isLoadingScene)
        {
            return;
        }
        SecondPart.CachingEffect();
    }

    public void CacheEffectsWhenLoadingScene_ThridPart()
    {
        if (!SceneManager.isLoadingScene)
        {
            return;
        }
        ThridPart.CachingEffect();
    }

    public void CacheEffectsWhenLoadingScene_FourPart()
    {
        if (!SceneManager.isLoadingScene)
        {
            return;
        }
        FourPart.CachingEffect();
    }



    public bool isCachEffectsFinish_FirstPart()
    {
        return FirstPart.IsPreWarmFinish();
    }

    public bool isCachEffectsFinish_SecondPart()
    {
        return SecondPart.IsPreWarmFinish();
    }

    public bool isCachEffectsFinish_ThridPart()
    {
        return ThridPart.IsPreWarmFinish();
    }

    public bool isCachEffectsFinish_FourPart()
    {
        return FourPart.IsPreWarmFinish();
    }


    public void ClearAllPreWarmData()
    {
        allCacheID.Clear();
        FirstPart.Clear();
        SecondPart.Clear();
        ThridPart.Clear();
        FourPart.Clear();
        NormalPart.Clear();
    }



}
