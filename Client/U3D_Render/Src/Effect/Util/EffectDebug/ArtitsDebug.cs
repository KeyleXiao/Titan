/*******************************************************************
** 文件名:	ArtitsDebug.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/6/9
** 版  本:	1.0
** 描  述:	美术调试脚本
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class ArtitsDebug : MonoBehaviour
{
    public int FrameLimit = 60;

    void Awake()
    {
        ImageSetting.GetData().currEffectQuality = EffectQuality.QUALITY_BEST;
        Initialize.mainCam = Camera.main;

        Initialize.mainCam.nearClipPlane = 0.1f;
        Initialize.mainCam.farClipPlane = Mathf.Min(Initialize.mainCam.farClipPlane, 2000);
    }

	// Use this for initialization
	void Start () {
        QualitySettings.vSyncCount = 0;
        Application.targetFrameRate = FrameLimit;
        //ImageEffects.ImageEffectManager.mainCam = Camera.main;
        //ImageEffects.ImageEffectManager.Init();
	    //Camera.main.hdr = false;
        Shader.SetGlobalFloat("_EffectSceneBrightnessScale", 1.0f);
        ImageEffects.ImageEffectManager.API_ResetSceneBrightness();
        Effect.EffectNode.InitTargetCache();
	}
    //string guid = string.Empty;
    //UnityEngine.Object obja = null;
    //ResNode res;
    //void Update()
    //{
    //    if (Input.GetKeyUp(KeyCode.A))
    //    {
    //        res = AssetBundleManager.GetPrefab("Skin/Hero/Didi");
    //        Instantiate(res.ResObj);
    //    }

    //    if (Input.GetKeyDown(KeyCode.D))
    //    {
    //        //Destroy(obja);
    //        AssetBundleManager.DeletePrefab(ref res);
    //    }
    //}


    void OnDestroy()
    {
        Effect.EffectNode.ClearTargetCache();
    }
}
