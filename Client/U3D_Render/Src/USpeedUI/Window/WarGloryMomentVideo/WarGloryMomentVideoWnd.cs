/*******************************************************************
** 文件名:	WarBroadcastMessageMgrWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	昔文博
** 日  期:	2017-6-24
** 版  本:	1.0
** 描  述:	结算界面精彩时刻弹窗
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using DG.Tweening;
using RenderHeads.Media.AVProVideo;

namespace USpeedUI.WarGloryMomentVideo
{
    public class UWarGloryMomentVideoWnd : UIBaseWnd<UWarGloryMomentVideoView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_WARGLORYMOMENTVIDEO;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "DynamicState_Moba/WarGloryMomentVideo/WarGloryMomentVideoView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_WAR_RECORDTABLE_END_SHOW:
                    {
                        if (m_wndView == null)
                        {
                            LoadView();
                            SetVisible(true);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        if (m_wndView != null)
                        {
                            UnloadView();
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class UWarGloryMomentVideoView : UIBaseWndView
    {
        public RectTransform WndRctTransform;
        public Button FrameBtn;
        public MediaPlayer Player;
        public DisplayUGUI DisPlayUGUIComponent;
        public Button PlayBtn;
        public Slider ProgressBar;
        public Text TimeLabel;

        private List<string> videoPathList;
        private int videoIndex;

        public override bool Init(IUIWnd wnd)
        {
            if(Player)
                Player.Events.AddListener(OnVideoEvent);

            if (FrameBtn)
                FrameBtn.onClick.AddListener(OnFrameClick);

            RefreshDirectory();
            LoadVideo();
            return base.Init(wnd);
        }

        public override void Destroy()
        {
            base.Destroy();

            if (Player != null && Player.Control != null)
                Player.Control.CloseVideo();

            if (DisPlayUGUIComponent != null && DisPlayUGUIComponent._mediaPlayer != null)
                DisPlayUGUIComponent._mediaPlayer = null;

            if (FrameBtn)
                FrameBtn.onClick.RemoveListener(OnFrameClick);
        }

        public override void Update()
        {
            base.Update();

            if (Player && Player.Info != null && Player.Info.GetDurationMs() > 0f)
            {
                float time = Player.Control.GetCurrentTimeMs();
                float d = time / Player.Info.GetDurationMs();
                ProgressBar.value = d;

                TimeLabel.text = MilliSeconds2Seconds(time) + "/" + MilliSeconds2Seconds(Player.Info.GetDurationMs());
            }
        }

        public void CloseView()
        {
            this.gameObject.SetActive(false);
        }

        public void PlayVideo()
        {
            if (null == Player || null == Player.Control)
                return;

            Player.Control.Stop();
            Player.Control.Play();

            if (null != PlayBtn)
            {
                PlayBtn.interactable = false;
                PlayBtn.GetComponent<Image>().DOFade(0, 0.5f);
            }
        }

        public void OnVideoEvent(MediaPlayer mp, MediaPlayerEvent.EventType et, ErrorCode errorCode)
        {
            switch (et)
            {
                case MediaPlayerEvent.EventType.ReadyToPlay:
                    break;
                case MediaPlayerEvent.EventType.Started:
                    break;
                case MediaPlayerEvent.EventType.FirstFrameReady:
                    {
                        TimeLabel.text = "00:00" + "/" + MilliSeconds2Seconds(Player.Info.GetDurationMs());

                        WndRctTransform.DOAnchorPosX(-20, 2.0f).OnComplete(() => { Player.Control.Play(); });
                    }
                    break;
                case MediaPlayerEvent.EventType.FinishedPlaying:
                    {
                        if (null != PlayBtn)
                        {
                            PlayBtn.GetComponent<Image>().DOFade(1, 0.5f);
                            PlayBtn.interactable = true;
                        }
                    }
                    break;
                case MediaPlayerEvent.EventType.Error:
                    {
                        if (errorCode == ErrorCode.LoadFailed)
                        {
                            if (Player != null && Player.Control != null)
                                Player.Control.CloseVideo();

                            PlayerNextVideo();
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public void OnFrameClick()
        {
            WndRctTransform.anchorMin = new Vector2(0.5f, 0.5f);
            WndRctTransform.anchorMax = new Vector2(0.5f, 0.5f);
            WndRctTransform.pivot = new Vector2(0.5f, 0.5f);
            WndRctTransform.anchoredPosition = Vector3.zero;
            Vector2 scale = WndRctTransform.sizeDelta;
            WndRctTransform.sizeDelta = new Vector2(scale.x * 2, scale.y * 2);

            //PlayBtn.interactable = false;
            //PlayBtn.GetComponent<Image>().DOFade(0, 0.5f);

            if (FrameBtn)
                FrameBtn.interactable = false;
        }

        private void RefreshDirectory()
        {
            string path = DataCenter.LogicDataCenter.screenCaptureDataManager.VideoFolderFullPath;
            if (string.IsNullOrEmpty(path))
                return;

            System.IO.DirectoryInfo directoryInfo = new System.IO.DirectoryInfo(path);
            if (directoryInfo != null)
            {
                directoryInfo.Refresh();
            }
        }

        private void LoadVideo()
        {
            if(null == Player || null == WndRctTransform)
                return;

            videoPathList = ScreenREC.GetInstance().GetWarGloryMomentVideoPathList();

            if (videoPathList == null || videoPathList.Count == 0)
                return;

            videoIndex = 0;
            string path = videoPathList[videoIndex];
            if(path != "")
            {
                if (System.IO.File.Exists(path))
                {
                    Player.OpenVideoFromFile(MediaPlayer.FileLocation.AbsolutePathOrURL, path, false);
                }
                else
                {
                    PlayerNextVideo();
                }
            }
        }

        private string MilliSeconds2Seconds(float milliSeconds)
        {
            int ms = (int)milliSeconds / 1000;

            int min = ms / 60;
            int sec = ms % 60;

            string szmin = min < 10 ? "0" + min : min.ToString();
            string szsec = sec < 10 ? "0" + sec : sec.ToString();

            return szmin + ":" + szsec;
        }

        private void PlayerNextVideo()
        {
            videoIndex++;

            if (videoIndex < 0 || videoPathList == null || videoPathList.Count <= videoIndex)
                return;

            if (videoPathList[videoIndex] != "")
            {
                if (System.IO.File.Exists(videoPathList[videoIndex]))
                {
                    Player.OpenVideoFromFile(MediaPlayer.FileLocation.AbsolutePathOrURL, videoPathList[videoIndex]);
                }
                else
                {
                    PlayerNextVideo();
                }
            }
        }

    }
}
