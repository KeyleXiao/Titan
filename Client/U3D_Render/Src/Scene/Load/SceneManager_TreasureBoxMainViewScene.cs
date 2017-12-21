using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using UnityEngine.Rendering;

public partial class SceneManager : MonoBehaviour
{
    public const string TreasureBoxMainViewSceneName = "TreasureBoxRoomView01";

    private bool isLoadingTreasureBoxMainViewScene = false;
    private bool isElaryUnLoadTreasureBoxMainViewScene = false;

    private string backOldSceneBeforeTreasureBoxMainViewName = string.Empty;
    /// <summary>
    /// 总共需要三帧的时间才能加载完毕，如果不填回调的话，就自己外部等两帧也可以
    /// </summary>
    /// <param name="callBack"></param>
    public void LoadTreasureBoxMainViewScene(System.Action<SceneResInfoManager> callBack)
    {
        if (isLoadingTreasureBoxMainViewScene)
        {
            return;
        }
        isLoadingTreasureBoxMainViewScene = true;
        StartCoroutine(LoadTreasureBoxMainViewScene_Impl(callBack));
    }

    IEnumerator LoadTreasureBoxMainViewScene_Impl(System.Action<SceneResInfoManager> callBack)
    {
        if (AssetBundleManager.isCanLoadLevel(TreasureBoxMainViewSceneName))
        {
            if (SceneResInfoManager.CurrentActiveInstance)
            {
                backOldSceneBeforeTreasureBoxMainViewName = SceneResInfoManager.CurrentActiveInstance.sceneName;
            }

            AssetBundleManager.LoadLevelAdditive(TreasureBoxMainViewSceneName);
            DeActiveScene(backOldSceneBeforeTreasureBoxMainViewName);

            yield return new WaitForEndOfFrame();



            yield return new WaitForFixedUpdate();



            SceneResInfoManager manager = SceneResInfoManager.API_GetSceneResManager(TreasureBoxMainViewSceneName);
            ImageSetting.Apply(true);
            if (manager)
            {
                ActiveScene(TreasureBoxMainViewSceneName);
                manager.SceneRoot.transform.SetPosition(Vector3.one * -999);
            }
            InvokeSceneActiveChangeEvent(TreasureBoxMainViewSceneName);
            isLoadingTreasureBoxMainViewScene = false;
            if (isElaryUnLoadTreasureBoxMainViewScene)
            {
                isElaryUnLoadTreasureBoxMainViewScene = false;
                UnLoadTreasureBoxMainViewScene();
                yield break;
            }
            if (null != callBack)
            {
                callBack(manager);
            }
        }
        else
        {
            isLoadingTreasureBoxMainViewScene = false;
        }
    }

    public void UnLoadTreasureBoxMainViewScene()
    {
        //如果场景还没加载完，就卸载，先记录下来
        if (isLoadingTreasureBoxMainViewScene)
        {
            isElaryUnLoadTreasureBoxMainViewScene = true;
            return;
        }
        UnLoadLevel(TreasureBoxMainViewSceneName);
        ActiveScene(backOldSceneBeforeTreasureBoxMainViewName);
        InvokeSceneActiveChangeEvent(backOldSceneBeforeTreasureBoxMainViewName);
        backOldSceneBeforeTreasureBoxMainViewName = string.Empty;
    }
}