using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using UnityEngine.Rendering;

public partial class SceneManager : MonoBehaviour
{
    public const string SelectHeroSceneName = "GameWaitingRoomView";

    private bool isLoadingSelectHeroScene = false;
    private bool isElaryUnLoadSelectScene = false;

    private string backOldSceneName = string.Empty;
    /// <summary>
    /// 总共需要三帧的时间才能加载完毕，如果不填回调的话，就自己外部等两帧也可以
    /// </summary>
    /// <param name="callBack"></param>
    public void LoadSelectHeroScene(System.Action<SceneResInfoManager> callBack)
    {
        if(isLoadingSelectHeroScene)
        {
            return;
        }
        isLoadingSelectHeroScene = true;
        StartCoroutine(LoadSelectHeroScene_Impl(callBack));
    }

    IEnumerator LoadSelectHeroScene_Impl(System.Action<SceneResInfoManager> callBack)
    {
        if (AssetBundleManager.isCanLoadLevel(SelectHeroSceneName))
        {
            if (SceneResInfoManager.CurrentActiveInstance)
            {
                backOldSceneName = SceneResInfoManager.CurrentActiveInstance.sceneName;
            }

            DeActiveScene(backOldSceneName);
            AssetBundleManager.LoadLevelAdditive(SelectHeroSceneName);

            /*为什么要两帧？？因为一个是本帧，另一个才是下一帧..*/
            yield return new WaitForEndOfFrame();
            yield return new WaitForFixedUpdate();



            SceneResInfoManager manager = SceneResInfoManager.API_GetSceneResManager(SelectHeroSceneName);
            ImageSetting.Apply(true);
            if (manager)
            {
                ActiveScene(SelectHeroSceneName);
                manager.SceneRoot.transform.SetPosition(Vector3.one * -999);
            }
            InvokeSceneActiveChangeEvent(SelectHeroSceneName);
            isLoadingSelectHeroScene = false;
            if (isElaryUnLoadSelectScene)
            {
                isElaryUnLoadSelectScene = false;
                UnLoadSelectHeroScene();
                yield break;
            }
            if(null != callBack)
            {
                callBack(manager);
            }
        }
        else
        {
            isLoadingSelectHeroScene = false;
        }
    }

    public void UnLoadSelectHeroScene()
    {
        //如果场景还没加载完，就卸载，先记录下来
        if(isLoadingSelectHeroScene)
        {
            isElaryUnLoadSelectScene = true;
            return;
        }
        UnLoadLevel(SelectHeroSceneName);
        ActiveScene(backOldSceneName);
        InvokeSceneActiveChangeEvent(backOldSceneName);
        backOldSceneName = string.Empty;
    }
}