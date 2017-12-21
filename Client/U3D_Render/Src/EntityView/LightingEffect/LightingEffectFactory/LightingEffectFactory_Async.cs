using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Effect;


public partial class LightingEffectFactory : MonoBehaviour
{
    class EffectFactoryNodeAsyncLoadingObj
    {

        public List<System.Action<LightingEffectResHandleInstance>> CallBackList = new List<System.Action<LightingEffectResHandleInstance>>(10);
        public List<LightingEffectResHandleInstance> dataList = new List<LightingEffectResHandleInstance>(10);

        public int effectID = -1;

        public void Clear()
        {
            dataList.Clear();
            CallBackList.Clear();
        }
    }

    private Dictionary<int, EffectFactoryNodeAsyncLoadingObj> m_AllocedAsyncObj = new Dictionary<int, EffectFactoryNodeAsyncLoadingObj>();
    private Dictionary<int, LightingEffectResHandle> m_AllocLightEffectResHandleTable = new Dictionary<int, LightingEffectResHandle>();


    /// <summary>
    /// 还没加载完成的
    /// </summary>
    private HashSet<int> m_AsyncLoadingEffectResHandle = new HashSet<int>();

    /// <summary>
    /// 一定是加载完成的
    /// </summary>
    private HashSet<int> m_LightEffectHandleTable = new HashSet<int>();

    /// <summary>
    /// 一定是加载完成的
    /// </summary>
    private HashSet<int> m_CacheLightEffectResHandleTable = new HashSet<int>();
    private float MaxCacheTime = 15.0f;

    private List<int> tempList = new List<int>(50);

    private EffectFactoryNodeAsyncLoadingObj AllocAsyncLoadingObj(int effectID)
    {
        EffectFactoryNodeAsyncLoadingObj reslut = null;
        if(!m_AllocedAsyncObj.TryGetValue(effectID,out reslut))
        {
            reslut = new EffectFactoryNodeAsyncLoadingObj();
            reslut.Clear();
            reslut.effectID = effectID;
            m_AllocedAsyncObj.Add(effectID, reslut);
        }
        return reslut;
    }

    public LightingEffectResHandle AllocResourcesHandle(int effectID)
    {
        LightingEffectResHandle reslut = null;
        if (!m_AllocLightEffectResHandleTable.TryGetValue(effectID, out reslut))
        {
            reslut = new LightingEffectResHandle();
            reslut.Clear();

            reslut.EffectID = effectID;
            m_AllocLightEffectResHandleTable.Add(effectID, reslut);
        }
        return reslut;
    }

    private void InitAsyncWhenGameStart()
    {

    }

    private void ClearAsyncWhenGameEnd()
    {
        m_AllocedAsyncObj.Clear();
        m_AllocLightEffectResHandleTable.Clear();
    }

    private void ClearAsyncData()
    {
        foreach (LightingEffectResHandle id in m_AllocLightEffectResHandleTable.Values)
        {
            UnLoadResourceHandle(id);
        }
        m_AsyncLoadingEffectResHandle.Clear();
        m_LightEffectHandleTable.Clear();
        m_CacheLightEffectResHandleTable.Clear();
    }

    private void PushCallBack(int effectID, LightingEffectResHandleInstance data,System.Action<LightingEffectResHandleInstance> callBack)
    {
        EffectFactoryNodeAsyncLoadingObj obj = AllocAsyncLoadingObj(effectID);
        obj.CallBackList.Add(callBack);
        obj.dataList.Add(data);
        if(!m_AsyncLoadingEffectResHandle.Contains(effectID))
        {
            m_AsyncLoadingEffectResHandle.Add(effectID);
        }
    }

    private void RemoveAllCallBack(int effectID)
    {
        EffectFactoryNodeAsyncLoadingObj obj = AllocAsyncLoadingObj(effectID);
        if (m_AsyncLoadingEffectResHandle.Contains(effectID))
        {
            m_AsyncLoadingEffectResHandle.Remove(effectID);
        }
    }

    private void RemoveCallBack(int effectID,System.Action<LightingEffectResHandleInstance> callBack)
    {
        if (!m_AllocedAsyncObj.ContainsKey(effectID))
        {
            return;
        }

        EffectFactoryNodeAsyncLoadingObj obj = AllocAsyncLoadingObj(effectID);
        for(int i = 0;i < obj.CallBackList.Count;i++)
        {
            if(obj.CallBackList[i] == callBack)
            {
                obj.CallBackList.RemoveAt(i);
                obj.dataList.RemoveAt(i);
                break;
            }
        }
    }

    private void UpdateCallBack()
    {
        if (m_AsyncLoadingEffectResHandle.Count <= 0)
        {
            return;
        }
        tempList.Clear();

        foreach (int id in m_AsyncLoadingEffectResHandle)
        {
            LightingEffectResHandle reslut = AllocResourcesHandle(id);

            if(reslut.stage == LightingEffectResHandle.loadingStage.Stage_Finish)
            {
                tempList.Add(id);
                continue;
            }

            if(reslut.stage == LightingEffectResHandle.loadingStage.Stage_Invalid)
            {
                reslut.effectConfig = ASpeedGame.Data.Scheme.SchemeLightingEffect.Instance.GetLightingEffect(reslut.EffectID);
                if(!reslut.effectConfig)
                {
                    OnHandleResLoadFinish(null, null, reslut);
                    tempList.Add(id);
                    continue;
                }
                reslut.prefabResNode = AssetBundleManager.GetAssetsAsync(AssetType.Asset_Prefab, OnHandleResLoadFinish, reslut.effectConfig.GetAssetGUID(), true, reslut);
                if (reslut.prefabResNode == null)
                {
                    OnHandleResLoadFinish(null, null, reslut);
                    tempList.Add(id);
                    continue;
                }

                reslut.stage = LightingEffectResHandle.loadingStage.Stage_Res;
            }
        }

        foreach(int id in tempList)
        {
            EffectFactoryNodeAsyncLoadingObj obj = AllocAsyncLoadingObj(id);
            RemoveAllCallBack(id);

            for(int i = 0;i < obj.CallBackList.Count;i++)
            {
                if(obj.CallBackList[i] != null)
                {
                    obj.CallBackList[i](obj.dataList[i]);
                }
            }

            obj.Clear();


        }
    }

    private void UpdateAsync()
    {
        UpdatePreWarmData();
        UpdateCache();
        UpdateCallBack();
    }



    private void OnHandleResLoadFinish(ResConfigData cof, ResNode res, System.Object UserDataObj)
    {
        LightingEffectResHandle node = UserDataObj as LightingEffectResHandle;
        if(null == node)
        {
            Debug.LogError("光效异步加载致命bug---UserDataObj not match");
            return;
        }
        m_LightEffectHandleTable.Add(node.EffectID);
        node.OnLoadFinish(EffectCacheRoot.transform, res);
    }

    private void UpdateCache()
    {
        if(m_CacheLightEffectResHandleTable.Count <= 0)
        {
            return;
        }
        tempList.Clear();
        foreach(int id in m_CacheLightEffectResHandleTable)
        {
            LightingEffectResHandle reslut = AllocResourcesHandle(id);
            reslut.currCacheTime += Time.deltaTime;
            if(reslut.currCacheTime > MaxCacheTime)
            {
                tempList.Add(id);
                UnLoadResourceHandle(reslut);
            }
        }

        foreach(int id in tempList)
        {
            m_CacheLightEffectResHandleTable.Remove(id);
        }
    }

    private LightingEffectResHandle GetFromCache(int effectID)
    {
        LightingEffectResHandle reslut = null;
        if(m_CacheLightEffectResHandleTable.Contains(effectID))
        {
            m_CacheLightEffectResHandleTable.Remove(effectID);
            m_LightEffectHandleTable.Add(effectID);
            reslut = AllocResourcesHandle(effectID);
            reslut.currCacheTime = 0.0f;
        }
        return reslut;
    }

    private void PushToCache(LightingEffectResHandle node)
    {
        //if(node.stage == LightingEffectResHandle.loadingStage.Stage_Finish)
        {
            node.currCacheTime = 0;
            m_CacheLightEffectResHandleTable.Add(node.EffectID);
        }


    }

    /// <summary>
    /// 返回值是一个标示，必须在回调函数中判断是否有效
    /// </summary>
    /// <param name="effectID"></param>
    /// <param name="param"></param>
    /// <param name="callBack"></param>
    /// <returns></returns>
    private LightingEffectResHandleInstance GetResourceHandleAsync(int effectID, System.Action<LightingEffectResHandleInstance> callBack)
    {
        LightingEffectResHandleInstance Reslut = null;
        LightingEffectResHandle node = GetFromCache(effectID);
        if(null != node)
        {
            Reslut = node.Instantiate();
        }
        else
        {
            node = AllocResourcesHandle(effectID);
            Reslut = node.Instantiate();

        }
        node.AddRef();
        PushCallBack(effectID, Reslut,callBack);

        return Reslut;
    }

    private void DeleteResourceHandleAsync(ref LightingEffectResHandleInstance instance,System.Action<LightingEffectResHandleInstance> callBack)
    {
        if(null == instance)
        {
            return;
        }
        int effectID = instance.EffectID;
        LightingEffectResHandle node = AllocResourcesHandle(effectID);
        node.CacheInstance(instance, EffectCacheRoot.transform, CachePos);

        int refCount = node.Release();
        if(refCount <= 0)
        {
            //移除回调
            RemoveAllCallBack(effectID);
            //看看是否加载完成
            if(m_LightEffectHandleTable.Contains(effectID))
            {
                m_LightEffectHandleTable.Remove(effectID);
            }
            PushToCache(node);
            //else
            //{
            //    //没有加载完成的直接卸载了
            //    UnLoadResourceHandle(node);
            //}
        }
        else
        {
            RemoveCallBack(effectID, callBack);
        }
        
    }

    private void UnLoadResourceHandle(LightingEffectResHandle instance)
    {
        if (null == instance)
        {
            return;
        }
        ResNode temp2 = instance.prefabResNode;
        instance.Clear();

        AssetBundleManager.DeleteAssetsAsync(ref temp2, OnHandleResLoadFinish, true);
    }
}
