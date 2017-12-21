using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using USpeedUI;
using DataCenter;

public class CaptureAreaImgData
{
    public string fileName;
    public Rect imgArea;

    public CaptureAreaImgData(string name, Rect area)
    {
        fileName = name;
        imgArea = area;
    }
}

public class CaptureScreen : MonoBehaviour
{
    private bool bCaptureFinish = true;
    public int WarImgCount 
    { 
        get { return warImgCount; }
        set { warImgCount = value; }
    }
    private int warImgCount = 0;

    public string CaptureTempDir
    {
        get
        {
            string path = ViewConfigManager.DataUserPath + "/" + "Temp";
            if (!System.IO.Directory.Exists(path))
            {
                System.IO.Directory.CreateDirectory(path);
            }
            return path;
        }
    }

    private static CaptureScreen instance;
    public static CaptureScreen GetInstance()
    {
        if (instance == null)
        {
            //挂载在输入管理控件上
            if (InputManager.InstanceGo != null)
                instance = InputManager.InstanceGo.AddComponent<CaptureScreen>();
            else
            {
                GameObject go = new GameObject();
                go.name = "CaptureScreen";
                DontDestroyOnLoad(go);
                instance = go.AddComponent<CaptureScreen>();
            }
        }
        return instance;
    }

    public void Start()
    {
        warImgCount = 0;
        StageManager.onStageStateChanged += OnStageStateChanged;
        bCaptureFinish = true;
    }

    public void OnDestroy()
    {
        StopAllCoroutines();
        StageManager.onStageStateChanged -= OnStageStateChanged;
    }

    protected void OnStageStateChanged(Stage_ManagerDef.EStage_State _eOldState, Stage_ManagerDef.EStage_State _eNewState)
    {
        if (_eNewState == Stage_ManagerDef.EStage_State.ESS_BATTLE)
        {
            warImgCount = 0;
        }
    }

    /// <summary>
    /// 截取屏幕图像
    /// </summary>
    /// <param name="_type">截图类型</param>
    public void CaptureScreenImage(SScreenShotImgInfo imgInfo)
    {
        if (!bCaptureFinish)
            return;

        if (!Directory.Exists(LogicDataCenter.screenCaptureDataManager.ImgFolderFullPath))
            return;

        StopAllCoroutines();

        if(imgInfo.screenShotType == EMScreenShotType.EMSS_RemoveUI)
            USpeedUI.UISystem.Instance.GetCamera().gameObject.SetActive(false);

        StartCoroutine(CaptureScreenImg(imgInfo));
    }

    /// <summary>
    /// 截取区域图像
    /// </summary>
    /// <param name="captureData"></param>
    public void CaptureAreaImage(CaptureAreaImgData captureData) 
    {
        if (!bCaptureFinish)
            return;

        if (null == captureData || null == captureData.imgArea || string.IsNullOrEmpty(captureData.fileName))
            return;

        if (!Directory.Exists(CaptureTempDir))
            return;

        string fullPath = CaptureTempDir + "/" + captureData.fileName;

        StopAllCoroutines();
        StartCoroutine(CaptureAreaImg(captureData, fullPath));
    }

    IEnumerator CaptureScreenImg(SScreenShotImgInfo imgInfo)
    {
        bCaptureFinish = false;

        yield return new WaitForEndOfFrame();

        Texture2D screenShot = new Texture2D(Screen.width, Screen.height, TextureFormat.RGB24, false);
        screenShot.ReadPixels(new Rect(0, 0, Screen.width, Screen.height), 0, 0);
        screenShot.Apply();

        if (imgInfo.screenShotType == EMScreenShotType.EMSS_RemoveUI)
            USpeedUI.UISystem.Instance.GetCamera().gameObject.SetActive(true);

        string filePath = GetFilePath();

        WriteDataToJPG(screenShot, imgInfo, filePath);
        warImgCount++;
        ResNode.DestroyRes(ref screenShot);
        bCaptureFinish = true;
    }

    IEnumerator CaptureAreaImg(CaptureAreaImgData captureData, string fullPath)
    {
        bCaptureFinish = false;

        yield return new WaitForEndOfFrame();

        Texture2D screenShot = new Texture2D((int)captureData.imgArea.width, (int)captureData.imgArea.height, TextureFormat.RGB24, false);
        screenShot.ReadPixels(captureData.imgArea, 0, 0);
        screenShot.Apply();

        byte[] imgBytes = screenShot.EncodeToJPG();
        if (fullPath != "")
            File.WriteAllBytes(fullPath, imgBytes);
        ResNode.DestroyRes(ref screenShot);
        bCaptureFinish = true;
    }

    /// <summary>
    /// 获取文件路径，此路径为文件最终完整路径
    /// </summary>
    /// <param name="_type"></param>
    /// <returns></returns>
    private string GetFilePath()
    {
        //System.DateTime now = new System.DateTime();
        //now = System.DateTime.Now;
        //string fileName = string.Format("{0}{1}{2}{3}{4}{5}", now.Year, now.Month, now.Day, now.Hour, now.Minute, now.Second);

        string time = DateTime.Now.ToString("yyyyMMddHHmmss");

        string filePath = "";

        filePath = LogicDataCenter.screenCaptureDataManager.ImgFolderFullPath + "/" + ScreenShotAndVedioDefines.ImageDef + "_" + time + ".jpg";

        return filePath;
    }

    private bool WriteDataToJPG(Texture2D texture, SScreenShotImgInfo imgInfo, string filePath)
    {
        if (filePath == "")
            return false;

        //类型byte数组
        byte[] typeBytes = ImageExtensions.StructToBytes(imgInfo);

        //压缩Jpg文件Byte数组
        byte[] imgBytes = texture.EncodeToJPG();

        ImageExtensions.AppendDataToJPG(filePath, imgBytes, typeBytes);

        return true;
    }
}


