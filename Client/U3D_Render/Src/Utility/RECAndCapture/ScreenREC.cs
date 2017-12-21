using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using UnityEngine;
using ASpeedGame.Data.GameSettingConfig;
using GameLogic;
using DataCenter;

public class ScreenREC : MonoBehaviour
{
    public enum EMRECState
    {
        EMREC_STOP,
        EMREC_RECORD,
        EMREC_DELAY,
    }

    public struct SWarGloryMomentVideoData
    {
        public EMScreenRECType videoType;
        public string szVideoPath;

        public SWarGloryMomentVideoData(EMScreenRECType type, string path)
        {
            videoType = type;
            szVideoPath = path;
        }
    }

    public const string szTempPathName = "temp";

    private int nHeroID = 0;
    private UInt32 checkPointTime = 0;
    private UInt32 killIntervalTime = 15000;
    private UInt32 delayIntervalTime = 2000;
    private bool bRECValid = false;
    private EMRECState recState = EMRECState.EMREC_STOP;
    private EMScreenRECType recType = EMScreenRECType.EMSS_Invalid;
    //private EMScreenRECType videoType = EMScreenRECType.EMSS_Invalid;
    //private string warGloryMomentvideoPath = "";
    private List<SWarGloryMomentVideoData> warGloryMomentvideoList;

    public int WarVideoCount 
    { 
        get { return warVideoCount; }
        set { warVideoCount = value; }
    }
    private int warVideoCount;

    public bool OpenREC { get { return bOpenREC; } set { bOpenREC = value; } }
    private bool bOpenREC;

    private static ScreenREC instance;
    public static ScreenREC GetInstance()
    {
        if (instance == null)
        {
            //挂载在输入管理控件上
            if (InputManager.InstanceGo != null)
                instance = InputManager.InstanceGo.AddComponent<ScreenREC>();
            else
            {
                GameObject go = new GameObject();
                go.name = "ScreenRec";
                DontDestroyOnLoad(go);
                instance = go.AddComponent<ScreenREC>();
            }
        }
        return instance;
    }

    public void Start()
    {
        warVideoCount = 0;
        bOpenREC = GameSettingConfig.Instance.GetGameSettingBoolValue(GameSettingIndex.OpenScrenRec);
        StageManager.onStageStateChanged += OnStageStateChanged;
    }

    public void OnDestroy()
    {
        SwitchRECState(EMRECState.EMREC_STOP);
        string tempPath = ViewConfigManager.DataUserPath + "/" + szTempPathName;
        if (Directory.Exists(tempPath))
        {
            try
            {
                Directory.Delete(tempPath, true);
            }
            catch (System.Exception ex)
            {
                Trace.LogWarning("Delete tempPath failed!" + ex);
            }
        }
        StageManager.onStageStateChanged -= OnStageStateChanged;

        if (warGloryMomentvideoList != null)
        {
            warGloryMomentvideoList.Clear();
            warGloryMomentvideoList = null;
        }
    }

    public void Update()
    {
        if (recState != EMRECState.EMREC_STOP)
        {
            if (StageManager.Instance.getCurrentState() != Stage_ManagerDef.EStage_State.ESS_BATTLE)
            {
                StopRecord();
                return;
            }

            UInt32 timeNow = GameLogicAPI.getSyncTickCount();
            if(recState == EMRECState.EMREC_RECORD)
            {
                //如果当前时间能比检查点超过（killintervalTime + 1000）毫秒，说明已经不可能继续击杀了
                if (timeNow > checkPointTime + killIntervalTime + delayIntervalTime)
                {
                    SwitchRECState(EMRECState.EMREC_STOP);
                }
            }
            else if(recState == EMRECState.EMREC_DELAY)
            {
                if (timeNow > checkPointTime + delayIntervalTime)
                {
                    SwitchRECState(EMRECState.EMREC_STOP);
                }
            }
        }
    }

    public void SwitchRECState(EMRECState state)
    {
        switch(state)
        {
            case EMRECState.EMREC_RECORD:
            {
                if (recState == EMRECState.EMREC_STOP)
                {
                    StartRecord();
                }
            }
            break;
            case EMRECState.EMREC_DELAY:
            {
                if (recState == EMRECState.EMREC_RECORD)
                {
                    recState = EMRECState.EMREC_DELAY;
                }
            }
            break;
            case EMRECState.EMREC_STOP:
            {
                if(recState == EMRECState.EMREC_RECORD || recState == EMRECState.EMREC_DELAY)
                {
                    StopRecord();
                }
            }
            break;
            default:
            break;
        }
    }

    //标记视频，有效则编码，无效抛弃
    public void SignValid(int killNum, int heroID)
    {
        if (killNum < 1)
            return;

        if (killNum == 1)
            SwitchRECState(EMRECState.EMREC_RECORD);
        else
        {
            if(killNum < 5 && recState == EMRECState.EMREC_STOP)
                SwitchRECState(EMRECState.EMREC_RECORD);

            if (heroID > 0)
            {
                recType = (EMScreenRECType)killNum;
                nHeroID = heroID;
                bRECValid = killNum > 2 ? true : false;

                if (killNum == (int)EMScreenRECType.EMSS_PentaKill)
                    SwitchRECState(EMRECState.EMREC_DELAY);
            }
        }
        checkPointTime = GameLogicAPI.getSyncTickCount();
    }

    public List<string> GetWarGloryMomentVideoPathList()
    {
        List<string> pathList = new List<string>();

        if (null != warGloryMomentvideoList)
        {
            foreach (SWarGloryMomentVideoData videoItem in warGloryMomentvideoList)
            {
                pathList.Add(videoItem.szVideoPath);
            }
        }

        return pathList;
    }

    protected void OnStageStateChanged(Stage_ManagerDef.EStage_State _eOldState, Stage_ManagerDef.EStage_State _eNewState)
    {
        //进入战场
        if (_eNewState == Stage_ManagerDef.EStage_State.ESS_BATTLE)
        {
            int warId = GameLogicAPI.getConfigWarID();
            int time = GameLogicAPI.getTimeCountKillInterval(warId);
            killIntervalTime = 15000;
            if(time > 0)
                killIntervalTime = (UInt32)time;

            //warGloryMomentvideoPath = "";
            //videoType = EMScreenRECType.EMSS_Invalid;
            recType = EMScreenRECType.EMSS_Invalid;
            bRECValid = false;

            if (warGloryMomentvideoList != null)
                warGloryMomentvideoList.Clear();

            warVideoCount = 0;
        }

        if (_eOldState == Stage_ManagerDef.EStage_State.ESS_BATTLE)
        {
            SwitchRECState(EMRECState.EMREC_STOP);
        }
    }

    private void StartRecord()
    {
        if (!bOpenREC)
            return;

        if (GameLogicAPI.isMainMap(SceneManager.Instance.getCurrentMapId()))
            return;

        string rootPath = ViewConfigManager.DataUserPath;
        if (rootPath == "")
            return;

        string tempPath = rootPath + "/" + szTempPathName;

        if (!Directory.Exists(tempPath))
            Directory.CreateDirectory(tempPath);

        gameview_start_screenCapture data = new gameview_start_screenCapture();

        if (Application.isEditor)
        {
            data.point_x = 0;
            data.point_y = 0;
            int width, height;
            ImageSetting.GetMainMonitorSize(out width, out height);
            data.width = width;
            data.height = height;
        }
        else
        {
            RECTAPI rect = new RECTAPI();
            GameLogicAPI.GetWindowRect(Initialize.m_WindowHwnd, ref rect);

            data.point_x = rect.left;
            data.point_y = rect.top;

            int titleHeight = GameLogicAPI.GetSystemMetrics((int)ESystemMetrics.SM_CYCAPTION);
            //int frameBorder = GameLogicAPI.GetSystemMetrics((int)ESystemMetrics.SM_CXFRAME);

            data.width = rect.right - rect.left;
            data.height = rect.bottom - rect.top;

            if(rect.top > 0)
            {
                data.point_y += titleHeight;
                data.height -= titleHeight;
            }
        }
        data.width = Mathf.Abs(data.width % 2 == 0 ? data.width : data.width - 1);
        data.height = Mathf.Abs(data.height % 2 == 0 ? data.height : data.height - 1);

        data.szTempFilePath = tempPath;
        ViewEventHelper.Instance.SendCommand<gameview_start_screenCapture>(GameLogicDef.GVIEWCMD_SCREENOPEN_OPEN, ref data);

        bRECValid = false;
        nHeroID = 0;
        checkPointTime = 0;
        recType = EMScreenRECType.EMSS_Invalid;
        recState = EMRECState.EMREC_RECORD;
    }

    private void StopRecord()
    {
        string outFilePath = "";

        if (bRECValid)
        {
            outFilePath = GetAppendFilePath();
            RecordWarVideo(new SWarGloryMomentVideoData(recType, outFilePath));
        }

        gameview_stop_screenCapture data = new gameview_stop_screenCapture();
        data.nValid = bRECValid ? 1 : 0;
        data.szOutFilePath = outFilePath;
        ViewEventHelper.Instance.SendCommand<gameview_stop_screenCapture>(GameLogicDef.GVIEWCMD_SCREENOPEN_STOP, ref data);

        bRECValid = false;
        recType = EMScreenRECType.EMSS_Invalid;
        checkPointTime = 0;
        nHeroID = 0;

        recState = EMRECState.EMREC_STOP;
    }

    //获取合并路径
    private string GetAppendFilePath()
    {
        string filePath = LogicDataCenter.screenCaptureDataManager.VideoFolderFullPath;

        StringBuilder sbr = new StringBuilder();

        Int64 nDBWarID = LogicDataCenter.warInfoDataManager.DBWarID;

        sbr.Append(filePath);
        sbr.Append('/');
        sbr.Append(ScreenShotAndVedioDefines.VideoDef);
        sbr.Append('_');
        sbr.Append((int)recType);
        sbr.Append('_');
        sbr.Append(nHeroID);
        sbr.Append('_');
        sbr.Append(nDBWarID);
        sbr.Append('_');
        sbr.Append(DateTime.Now.ToString("yyyyMMddHHmmss"));
        sbr.Append(".mp4");
        return sbr.ToString();
    }

    /// <summary>
    /// 记录战场内打到的精彩视频
    /// </summary>
    /// <param name="videoData"></param>
    private void RecordWarVideo(SWarGloryMomentVideoData videoData)
    {
        if (warGloryMomentvideoList == null)
            warGloryMomentvideoList = new List<SWarGloryMomentVideoData>();

        int index = warGloryMomentvideoList.FindIndex(x => x.videoType < videoData.videoType);

        if (index > -1)
            warGloryMomentvideoList.Insert(index, videoData);
        else
            warGloryMomentvideoList.Add(videoData);

        warVideoCount++;
    }
}

