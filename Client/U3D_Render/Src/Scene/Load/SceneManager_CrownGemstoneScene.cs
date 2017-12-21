using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using UnityEngine.Rendering;

public partial class SceneManager : MonoBehaviour
{
    public const string CrownGemstoneViewSceneName = "JewelRoomView";

    private bool isLoadingCrownGemstoneViewScene = false;
    private bool isElaryUnLoadCrownGemstoneScene = false;

    private string backOldCrownGemstoneSceneName = string.Empty;
    /// <summary>
    /// 总共需要三帧的时间才能加载完毕，如果不填回调的话，就自己外部等两帧也可以
    /// </summary>
    /// <param name="callBack"></param>
    public void LoadCrownGemstoneViewScene(System.Action<SceneResInfoManager> callBack)
    {
        if(isLoadingCrownGemstoneViewScene)
        {
            return;
        }
        isLoadingCrownGemstoneViewScene = true;
        StartCoroutine(LoadCrownGemstoneViewScene_Impl(callBack));
    }

    IEnumerator LoadCrownGemstoneViewScene_Impl(System.Action<SceneResInfoManager> callBack)
    {
        if (AssetBundleManager.isCanLoadLevel(CrownGemstoneViewSceneName))
        {
            if (SceneResInfoManager.CurrentActiveInstance)
            {
                backOldCrownGemstoneSceneName = SceneResInfoManager.CurrentActiveInstance.sceneName;
            }
            
            AssetBundleManager.LoadLevelAdditive(CrownGemstoneViewSceneName);
            DeActiveScene(backOldCrownGemstoneSceneName);

            /*为什么要两帧？？因为一个是本帧，另一个才是下一帧..*/
            yield return new WaitForEndOfFrame();
            yield return new WaitForFixedUpdate();



            SceneResInfoManager manager = SceneResInfoManager.API_GetSceneResManager(CrownGemstoneViewSceneName);
            ImageSetting.Apply(true);
            if (manager)
            {
                ActiveScene(CrownGemstoneViewSceneName);
                manager.SceneRoot.transform.SetPosition(Vector3.one * -999);
            }
            InvokeSceneActiveChangeEvent(CrownGemstoneViewSceneName);
            isLoadingCrownGemstoneViewScene = false;
            if (isElaryUnLoadCrownGemstoneScene)
            {
                isElaryUnLoadCrownGemstoneScene = false;
                UnLoadCrownGemstoneViewScene();
                yield break;
            }
            if(null != callBack)
            {
                callBack(manager);
            }
        }
        else
        {
            isLoadingCrownGemstoneViewScene = false;
        }
    }

    public void UnLoadCrownGemstoneViewScene()
    {
        //如果场景还没加载完，就卸载，先记录下来
        if(isLoadingCrownGemstoneViewScene)
        {
            isElaryUnLoadCrownGemstoneScene = true;
            return;
        }
        UnLoadLevel(CrownGemstoneViewSceneName);
        ActiveScene(backOldCrownGemstoneSceneName);
        InvokeSceneActiveChangeEvent(backOldCrownGemstoneSceneName);
        backOldCrownGemstoneSceneName = string.Empty;
    }
}