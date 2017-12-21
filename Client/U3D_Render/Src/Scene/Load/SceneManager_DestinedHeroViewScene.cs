using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using UnityEngine.Rendering;

public partial class SceneManager : MonoBehaviour
{
    public const string DestinedHeroViewSceneName = "DestinedHero_View";

    private bool isLoadingDestinedHeroViewScene = false;
    private bool isElaryUnLoadDestinedHeroScene = false;

    private string backOldDestinedHeroSceneName = string.Empty;
    /// <summary>
    /// 总共需要三帧的时间才能加载完毕，如果不填回调的话，就自己外部等两帧也可以
    /// </summary>
    /// <param name="callBack"></param>
    public void LoadDestinedHeroViewScene(System.Action<SceneResInfoManager> callBack)
    {
        if(isLoadingDestinedHeroViewScene)
        {
            return;
        }
        isLoadingDestinedHeroViewScene = true;
        StartCoroutine(LoadDestinedHeroViewScene_Impl(callBack));
    }

    IEnumerator LoadDestinedHeroViewScene_Impl(System.Action<SceneResInfoManager> callBack)
    {
        if (AssetBundleManager.isCanLoadLevel(DestinedHeroViewSceneName))
        {
            if (SceneResInfoManager.CurrentActiveInstance)
            {
                backOldDestinedHeroSceneName = SceneResInfoManager.CurrentActiveInstance.sceneName;
            }
            
            AssetBundleManager.LoadLevelAdditive(DestinedHeroViewSceneName);
            DeActiveScene(backOldDestinedHeroSceneName);

            /*为什么要两帧？？因为一个是本帧，另一个才是下一帧..*/
            yield return new WaitForEndOfFrame();
            yield return new WaitForFixedUpdate();


            
            SceneResInfoManager manager = SceneResInfoManager.API_GetSceneResManager(DestinedHeroViewSceneName);
            ImageSetting.Apply(true);
            if (manager)
            {
                ActiveScene(DestinedHeroViewSceneName);
                manager.SceneRoot.transform.SetPosition(Vector3.one * -9999);
            }
            InvokeSceneActiveChangeEvent(DestinedHeroViewSceneName);
            isLoadingDestinedHeroViewScene = false;
            if (isElaryUnLoadDestinedHeroScene)
            {
                isElaryUnLoadDestinedHeroScene = false;
                UnLoadDestinedHeroViewScene();
                yield break;
            }
            if(null != callBack)
            {
                callBack(manager);
            }
        }
        else
        {
            isLoadingDestinedHeroViewScene = false;
        }
    }

    public void UnLoadDestinedHeroViewScene()
    {
        //如果场景还没加载完，就卸载，先记录下来
        if(isLoadingDestinedHeroViewScene)
        {
            isElaryUnLoadDestinedHeroScene = true;
            return;
        }
        UnLoadLevel(DestinedHeroViewSceneName);
        ActiveScene(backOldDestinedHeroSceneName);
        InvokeSceneActiveChangeEvent(backOldDestinedHeroSceneName);
        backOldDestinedHeroSceneName = string.Empty;
    }
}