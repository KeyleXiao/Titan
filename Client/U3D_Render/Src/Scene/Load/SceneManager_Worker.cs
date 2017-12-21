using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;

public partial class SceneManager : MonoBehaviour
{

    /// <summary>
    /// 准备阶段
    /// </summary>
    /// <returns></returns>
    private IEnumerator Worker_Ready()
    {
        Double loadSceneReadyTine = DateTime.Now.Ticks;

        //清除缓存
        LightingEffectFactory.Instance.ClearCache();
        SkinManager.UnLoadAllSkin();


        //卸载所有场景
        UnLoadAllLevel();

        AdvanceProgress(10);
        yield return new WaitForSeconds(1.0f);
        currentLoadingTime++;
        //卸载无用内存
        Resources.UnloadUnusedAssets();
        //GC.Collect();
        if(SoldierCamera.MainInstance())
        {
            SoldierCamera.MainInstance().ClearCameraModeChangeEvent();
        }
        yield return new WaitForSeconds(0.5f);
        currentLoadingTime += 0.5f;

        isLoadingScene = true;
        loadSceneReadyTine = DateTime.Now.Ticks - loadSceneReadyTine;
        loadSceneReadyTine *= 0.0000001f;
        Debug.Log("加载场景准备工作，耗时:" + loadSceneReadyTine);
        currentLoadingTime += loadSceneReadyTine;
    }

    /// <summary>
    /// 场景
    /// </summary>
    /// <returns></returns>
    private IEnumerator Worker_Scene()
    {
        if (AssetBundleManager.isCanLoadLevel(m_SceneName))
        {
            double loadsceneFileTime = DateTime.Now.Ticks;

            m_AsyncOperation = AssetBundleManager.LoadLevelAsync(m_SceneName);
            while (!m_AsyncOperation.isDone)
            {
                yield return endofframe;
            }

            yield return endofframe;
            ImageSetting.ApplySceneImageSettingWhenAfterLoadingScene();

            InvokeSceneActiveChangeEvent(m_SceneName);
            //将摄像机的位置移到目标点.不需要移动了，现在优化了加载速度，可以了
           // Initialize.mainCam.transform.SetPosition(m_vecBornPos);
            AdvanceProgress(30);

            loadsceneFileTime = DateTime.Now.Ticks - loadsceneFileTime;
            loadsceneFileTime *= 0.0000001f;
            Debug.Log("场景文件加载完成，耗时:" + loadsceneFileTime);
            currentLoadingTime += loadsceneFileTime;
            yield return new WaitForSeconds(1.0f);
            currentLoadingTime ++;
        }
        else
        {
            Trace.Error("scene:" + m_SceneName + " can not be loaded due to that is not in level setting");
        }
    }

    /// <summary>
    /// 场景贴图
    /// </summary>
    /// <returns></returns>
    private IEnumerator Worker_SceneTexture()
    {
        double loadTextureTime = DateTime.Now.Ticks;

        /*********************等待场景里的主贴图加载完成****************************************/
        while (true && AssetBundleManager.isVaild)
        {
            //先等两帧，场景里的sceneresinfo调用一遍Awake;
            yield return endofframe;
            yield return endofframe;
            //看看maintex加载完成了或者超过了半分钟
            if (!SceneResInfo.isPreLoadFinish())
            {
                double dealtloadTextureTime = DateTime.Now.Ticks - loadTextureTime;
                dealtloadTextureTime *= 0.0000001f;
                if (dealtloadTextureTime > 30)
                {
                    Debug.LogWarning("场景贴图预加载超时!");
                    SceneResInfo.ClearPreLoadedRes();
                    break;
                }
                yield return endofframe;
            }
            else
            {
                SceneResInfo.ClearPreLoadedRes();
                break;
            }

        }

        Water.Init();

        AdvanceProgress(40);

        loadTextureTime = DateTime.Now.Ticks - loadTextureTime;
        loadTextureTime *= 0.0000001f;
        Debug.Log("加载场景贴图完成:" + loadTextureTime);
        currentLoadingTime += loadTextureTime;
        //加载完成之后,等1秒，等帧数稳定,并且更新加载进度
        yield return new WaitForSeconds(1.0f);
        currentLoadingTime++;
        /*********************开始加载场景里的主贴图****************************************/
    }

    /// <summary>
    /// 缓存
    /// </summary>
    /// <returns></returns>
    private IEnumerator Worker_Caching()
    {

        double cachingTime = DateTime.Now.Ticks;

        double linghtingEffectPartTime = DateTime.Now.Ticks;
        double SkinPartTime = DateTime.Now.Ticks;

        int cachingLoadingPart = 0;

        //是否超过了服务器设定的时间
        bool isOverServerTime = false;
        //第一优先级

        SkinManager.StartPreWarmSkin_FirstPart();
        while (!SkinManager.IsSkinPreWarmFinish_FirstPart())
        {
            yield return endofframe;
        }

        SkinPartTime = DateTime.Now.Ticks - SkinPartTime;
        Debug.Log("皮肤缓存第一阶段耗时:" + GameUtil.TicksToSecond(SkinPartTime));
        currentLoadingTime += GameUtil.TicksToSecond(SkinPartTime);
        isOverServerTime = currentLoadingTime >= serverWaitngTime;

        linghtingEffectPartTime = DateTime.Now.Ticks;
        LightingEffectFactory.Instance.CacheEffectsWhenLoadingScene_FirstPart();
        while (!LightingEffectFactory.Instance.isCachEffectsFinish_FirstPart())
        {
            yield return endofframe;
        }

        linghtingEffectPartTime = DateTime.Now.Ticks - linghtingEffectPartTime;
        Debug.Log("光效缓存第一阶段耗时:" + GameUtil.TicksToSecond(linghtingEffectPartTime));
        currentLoadingTime += GameUtil.TicksToSecond(linghtingEffectPartTime);

        AdvanceProgress(55);
        if (GameUtil.TicksToSecond(linghtingEffectPartTime + SkinPartTime) > 5.0f)
        {
            //加载完成之后,等0.5秒，等帧数稳定,并且更新加载进度
            yield return new WaitForSeconds(0.5f);
            currentLoadingTime += 0.5f;
        }
        isOverServerTime = currentLoadingTime >= serverWaitngTime;

        if ((serverWaitngTime - currentLoadingTime) > 40)
        {
            cachingLoadingPart = 1;

            SkinPartTime = DateTime.Now.Ticks;
            SkinManager.StartPreWarmSkin_SecondPart();
            while (!SkinManager.IsSkinPreWarmFinish_SecondPart())
            {
                yield return endofframe;
            }

            SkinPartTime = DateTime.Now.Ticks - SkinPartTime;
            Debug.Log("皮肤缓存第二阶段耗时:" + GameUtil.TicksToSecond(SkinPartTime));
            currentLoadingTime += GameUtil.TicksToSecond(SkinPartTime);

            AdvanceProgress(60);
            if (GameUtil.TicksToSecond(SkinPartTime) > 5.0f)
            {
                //加载完成之后,等0.5秒，等帧数稳定,并且更新加载进度
                yield return new WaitForSeconds(0.5f);
                currentLoadingTime += 0.5f;
            }
            isOverServerTime = currentLoadingTime >= serverWaitngTime;

            linghtingEffectPartTime = DateTime.Now.Ticks;
            LightingEffectFactory.Instance.CacheEffectsWhenLoadingScene_SecondPart();
            while (!LightingEffectFactory.Instance.isCachEffectsFinish_SecondPart())
            {
                yield return endofframe;
            }

            linghtingEffectPartTime = DateTime.Now.Ticks - linghtingEffectPartTime;
            Debug.Log("光效缓存第二阶段耗时:" + GameUtil.TicksToSecond(linghtingEffectPartTime));
            currentLoadingTime += GameUtil.TicksToSecond(linghtingEffectPartTime);

            AdvanceProgress(65);
            if (GameUtil.TicksToSecond(linghtingEffectPartTime) > 5.0f)
            {
                //加载完成之后,等0.5秒，等帧数稳定,并且更新加载进度
                yield return new WaitForSeconds(0.5f);
                currentLoadingTime += 0.5f;
            }
            isOverServerTime = currentLoadingTime >= serverWaitngTime;
        }

        if (cachingLoadingPart >= 1)
        {
            /*看下时间够,耗时小于30，可以进行第三优先级操作*/
            if ((serverWaitngTime - currentLoadingTime) > 25)
            {
                cachingLoadingPart = 2;

                SkinPartTime = DateTime.Now.Ticks;
                SkinManager.StartPreWarmSkin_ThirdPart();
                while (!SkinManager.IsSkinPreWarmFinish_ThirdPart())
                {
                    yield return endofframe;
                }

                SkinPartTime = DateTime.Now.Ticks - SkinPartTime;
                Debug.Log("皮肤缓存第三阶段耗时:" + GameUtil.TicksToSecond(SkinPartTime));
                currentLoadingTime += GameUtil.TicksToSecond(SkinPartTime);

                AdvanceProgress(70);
                if (GameUtil.TicksToSecond(SkinPartTime) > 5.0f)
                {
                    //加载完成之后,等0.5秒，等帧数稳定,并且更新加载进度
                    yield return new WaitForSeconds(0.5f);
                    currentLoadingTime += 0.5f;
                }
                isOverServerTime = currentLoadingTime >= serverWaitngTime;

                linghtingEffectPartTime = DateTime.Now.Ticks;
                LightingEffectFactory.Instance.CacheEffectsWhenLoadingScene_ThridPart();
                while (!LightingEffectFactory.Instance.isCachEffectsFinish_ThridPart())
                {
                    yield return endofframe;
                }

                linghtingEffectPartTime = DateTime.Now.Ticks - linghtingEffectPartTime;
                currentLoadingTime += GameUtil.TicksToSecond(linghtingEffectPartTime);
                Debug.Log("光效缓存第三阶段耗时:" + GameUtil.TicksToSecond(linghtingEffectPartTime));

                AdvanceProgress(75);
                if (GameUtil.TicksToSecond(linghtingEffectPartTime) > 5.0f)
                {
                    //加载完成之后,等0.5秒，等帧数稳定,并且更新加载进度
                    yield return new WaitForSeconds(0.5f);
                    currentLoadingTime += 0.5f;
                }
                isOverServerTime = currentLoadingTime >= serverWaitngTime;
            }
        }


        if (cachingLoadingPart >= 2)
        {
            /*看下时间够，可以进行第四优先级操作*/
            if ((serverWaitngTime - currentLoadingTime) > 10)
            {
                cachingLoadingPart = 3;

                SkinPartTime = DateTime.Now.Ticks;
                SkinManager.StartPreWarmSkin_FourPart();
                while (!SkinManager.IsSkinPreWarmFinish_FourPart())
                {
                    yield return endofframe;
                }

                SkinPartTime = DateTime.Now.Ticks - SkinPartTime;
                Debug.Log("皮肤缓存第四阶段耗时:" + GameUtil.TicksToSecond(SkinPartTime));
                currentLoadingTime += GameUtil.TicksToSecond(SkinPartTime);

                AdvanceProgress(80);
                if (GameUtil.TicksToSecond(SkinPartTime) > 5.0f)
                {
                    //加载完成之后,等0.5秒，等帧数稳定,并且更新加载进度
                    yield return new WaitForSeconds(0.5f);
                    currentLoadingTime += 0.5f;
                }
                isOverServerTime = currentLoadingTime >= serverWaitngTime;

                linghtingEffectPartTime = DateTime.Now.Ticks;
                LightingEffectFactory.Instance.CacheEffectsWhenLoadingScene_FourPart();
                while (!LightingEffectFactory.Instance.isCachEffectsFinish_FourPart())
                {
                    yield return endofframe;
                }

                linghtingEffectPartTime = DateTime.Now.Ticks - linghtingEffectPartTime;
                Debug.Log("光效缓存第四阶段耗时:" + GameUtil.TicksToSecond(linghtingEffectPartTime));
                currentLoadingTime += GameUtil.TicksToSecond(linghtingEffectPartTime);

                AdvanceProgress(85);

                if ( GameUtil.TicksToSecond(linghtingEffectPartTime) > 5.0f)
                {
                    //加载完成之后,等0.5秒，等帧数稳定,并且更新加载进度
                    yield return new WaitForSeconds(0.5f);
                    currentLoadingTime+= 0.5f;
                }

                isOverServerTime = currentLoadingTime >= serverWaitngTime;
            }
        }
        linghtingEffectPartTime = DateTime.Now.Ticks;
        LightingEffectFactory.Instance.ActiveCacheRendering();

        //渲染1.0s
        yield return new WaitForSeconds(1.0f);
        currentLoadingTime++;
        isOverServerTime = currentLoadingTime >= serverWaitngTime;
        LightingEffectFactory.Instance.InvActiveCacheRendering();

        SkinManager.EndPreWarmSkin();
    
        SkinManager.ClearCacheSkinID();
        LightingEffectFactory.Instance.ClearAllPreWarmData();

        cachingTime = DateTime.Now.Ticks - cachingTime;
        cachingTime *= 0.0000001f;
        Debug.Log("缓存完毕，耗时:" + cachingTime + ",总耗时:" + currentLoadingTime);

        
    }

    private IEnumerator Worker_Finish()
    {
        Double loadSceneWillFinishTime = DateTime.Now.Ticks;

        AdvanceProgress(80);
        //休息1.5s，帧数稳定
        yield return new WaitForSeconds(1.0f);
        //SceneResInfoManager.RefershAllCollider();
        currentLoadingTime += 1.0f;
        isLoadingScene = false;

        Resources.UnloadUnusedAssets();
        //GC.Collect();

        loadSceneWillFinishTime = DateTime.Now.Ticks - loadSceneWillFinishTime;
        loadSceneWillFinishTime *= 0.0000001f;
        currentLoadingTime += loadSceneWillFinishTime;
        Debug.Log("清理场景加载临时文件，耗时:" + loadSceneWillFinishTime);
    }
}



