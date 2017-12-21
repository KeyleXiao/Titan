using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using UnityEngine.Rendering;

public partial class SceneManager : MonoBehaviour
{
    public const string TreasureBoxAwardViewSceneName = "TreasureBoxRoomView02";

    private bool isLoadingTreasureBoxAwardViewScene = false;
    private bool isElaryUnLoadTreasureBoxAwardViewScene = false;

    private string backOldSceneBeforeTreasureBoxAwardViewName = string.Empty;
    /// <summary>
    /// 总共需要三帧的时间才能加载完毕，如果不填回调的话，就自己外部等两帧也可以
    /// </summary>
    /// <param name="callBack"></param>
    public void LoadTreasureBoxAwardViewScene(System.Action<SceneResInfoManager> callBack)
    {
        if (isLoadingTreasureBoxAwardViewScene)
        {
            return;
        }
        isLoadingTreasureBoxAwardViewScene = true;
        StartCoroutine(LoadTreasureBoxAwardViewScene_Impl(callBack));
    }

    IEnumerator LoadTreasureBoxAwardViewScene_Impl(System.Action<SceneResInfoManager> callBack)
    {
        if (AssetBundleManager.isCanLoadLevel(TreasureBoxAwardViewSceneName))
        {
            if (SceneResInfoManager.CurrentActiveInstance)
            {
                backOldSceneBeforeTreasureBoxAwardViewName = SceneResInfoManager.CurrentActiveInstance.sceneName;
            }

            AssetBundleManager.LoadLevelAdditive(TreasureBoxAwardViewSceneName);
            DeActiveScene(backOldSceneBeforeTreasureBoxAwardViewName);

            yield return new WaitForEndOfFrame();

            yield return new WaitForFixedUpdate();



            SceneResInfoManager manager = SceneResInfoManager.API_GetSceneResManager(TreasureBoxAwardViewSceneName);
            ImageSetting.Apply(true);
            if (manager)
            {
                ActiveScene(TreasureBoxAwardViewSceneName);
                manager.SceneRoot.transform.SetPosition(Vector3.one * -9999);
            }
            InvokeSceneActiveChangeEvent(TreasureBoxAwardViewSceneName);
            isLoadingTreasureBoxAwardViewScene = false;
            if (isElaryUnLoadTreasureBoxAwardViewScene)
            {
                isElaryUnLoadTreasureBoxAwardViewScene = false;
                UnLoadTreasureBoxAwardViewScene();
                yield break;
            }
            if (null != callBack)
            {
                callBack(manager);
            }
        }
        else
        {
            isLoadingTreasureBoxAwardViewScene = false;
        }
    }

    public void UnLoadTreasureBoxAwardViewScene()
    {
        //如果场景还没加载完，就卸载，先记录下来
        if (isLoadingTreasureBoxAwardViewScene)
        {
            isElaryUnLoadTreasureBoxAwardViewScene = true;
            return;
        }
        UnLoadLevel(TreasureBoxAwardViewSceneName);
        ActiveScene(backOldSceneBeforeTreasureBoxAwardViewName);
        InvokeSceneActiveChangeEvent(backOldSceneBeforeTreasureBoxAwardViewName);
        backOldSceneBeforeTreasureBoxAwardViewName = string.Empty;
    }
}