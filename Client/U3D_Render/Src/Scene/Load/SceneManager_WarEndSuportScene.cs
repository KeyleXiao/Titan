using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using UnityEngine.Rendering;

public partial class SceneManager : MonoBehaviour
{
    public const string WarSupportPlayerViewSceneName = "Thumbs_up_RoomView";

    private bool isLoadingWarSupportPlayerViewScene = false;
    private bool isElaryUnLoadSupportScene = false;

    private string backOldSupportSceneName = string.Empty;
    /// <summary>
    /// 总共需要三帧的时间才能加载完毕，如果不填回调的话，就自己外部等两帧也可以
    /// </summary>
    /// <param name="callBack"></param>
    public void LoadWarSupportPlayerViewScene(System.Action<SceneResInfoManager> callBack)
    {
        if(isLoadingWarSupportPlayerViewScene)
        {
            return;
        }
        isLoadingWarSupportPlayerViewScene = true;
        StartCoroutine(LoadWarSupportPlayerViewScene_Impl(callBack));
    }

    IEnumerator LoadWarSupportPlayerViewScene_Impl(System.Action<SceneResInfoManager> callBack)
    {
        if (AssetBundleManager.isCanLoadLevel(WarSupportPlayerViewSceneName))
        {
            if (SceneResInfoManager.CurrentActiveInstance)
            {
                backOldSupportSceneName = SceneResInfoManager.CurrentActiveInstance.sceneName;
            }
            
            AssetBundleManager.LoadLevelAdditive(WarSupportPlayerViewSceneName);

            DeActiveScene(backOldSupportSceneName);

            /*为什么要两帧？？因为一个是本帧，另一个才是下一帧..*/
            yield return new WaitForEndOfFrame();
            yield return new WaitForFixedUpdate();



            SceneResInfoManager manager = SceneResInfoManager.API_GetSceneResManager(WarSupportPlayerViewSceneName);
            ImageSetting.Apply(true);
            if (manager)
            {
                ActiveScene(WarSupportPlayerViewSceneName);
                manager.SceneRoot.transform.SetPosition(Vector3.one * -999);
            }
            InvokeSceneActiveChangeEvent(WarSupportPlayerViewSceneName);
            isLoadingWarSupportPlayerViewScene = false;
            if (isElaryUnLoadSupportScene)
            {
                isElaryUnLoadSupportScene = false;
                UnLoadWarSupportPlayerViewScene();
                yield break;
            }
            if(null != callBack)
            {
                callBack(manager);
            }
        }
        else
        {
            isLoadingWarSupportPlayerViewScene = false;
        }
    }

    public void UnLoadWarSupportPlayerViewScene()
    {
        //如果场景还没加载完，就卸载，先记录下来
        if(isLoadingWarSupportPlayerViewScene)
        {
            isElaryUnLoadSupportScene = true;
            return;
        }
        UnLoadLevel(WarSupportPlayerViewSceneName);
        ActiveScene(backOldSupportSceneName);
        InvokeSceneActiveChangeEvent(backOldSupportSceneName);
        backOldSupportSceneName = string.Empty;
    }
}