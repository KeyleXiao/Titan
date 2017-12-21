using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using DataCenter;
using USpeedUI.UWidgets;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using ASpeedGame.Data.GameSettingConfig;
using ASpeedGame.Data.Scheme;
using RenderHeads.Media.AVProVideo;
using SocialSharing;

namespace USpeedUI.PlayerFrame
{
    public class PlayerGloryMomentWnd : UIBaseSubWnd<UViewPlayerGloryMoment>
    {
        public override int GetWndType()
        {
            return (int)PlayerSubWndType.WND_MOMENT;
        }

        public override String GetPrefabPath()
        {
            return "PlayerGloryMoment/PlayerGloryMomentView";
        }

        public override bool Init()
        {
            SetVisible(false);
            return base.Init();
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                    //打开图片分享界面
                case WndMsgID.WND_MSG_OPEN_SOCIALSHARE_FRAME:
                    {
                        if (m_wndView != null)
                            m_wndView.OpenImgShareFrame(msgData as USocialShareShowMsgData);
                    }
                    break;
                    //打开视频播放界面
                case WndMsgID.WND_MSG_OPEN_GLORYMOMENT_VIDEO_FRAME:
                    {
                        if(m_wndView != null)
                            m_wndView.OpenVedioPlayFrame(msgData as UGloryMomentVideoMsgData);
                    }
                    break;
                    //打开某场次精彩时刻数据
                case WndMsgID.WND_MSG_OPEN_GLORYMOMENT_WARDATA:
                    {
                        if (m_wndView != null)
                            m_wndView.OpenWarData(msgData as UGloryMomentReqMsgData);
                    }
                    break;
                default:
                    break;
            }
        }
    }

    //数据显示类型(全部、3、4、5杀、超神)
    public enum EMGloryMomentDataShowType
    {
        EMType_All = 2,
        EMType_TripleKill,  
        EMType_QuadraKill,
        EMType_PentaKill,
        EMSS_God,
    }

    //页面类型
    public enum EMGloryMomentPageType
    {
        EMPage_Image,       //图片
        EMPage_Video,       //视频
    }

    //时间轴item
    public class GloryMomentTimeAxisItem : MonoBehaviour, IPointerClickHandler
    {
        public Image TagLine;
        public Text SelectTxt;
        public Text UnSelectTxt;

        private int nIndex;
        private UViewPlayerGloryMoment m_WndView;

        public void Clear()
        {
            nIndex = -1;
            m_WndView = null;
            TagLine.gameObject.SetActive(false);
            SelectTxt.text = "";
            UnSelectTxt.text = "";
            SelectTxt.gameObject.SetActive(false);
            UnSelectTxt.gameObject.SetActive(false);
        }

        public void OnDestroy()
        {
            Clear();
        }

        public void Init(UViewPlayerGloryMoment wndView, int index, string time, bool bSelect)
        {
            Clear();
            m_WndView = wndView;
            nIndex = index;
            SetData(time, bSelect);
        }

        public void SetData(string time, bool bSelect)
        {
            SelectTxt.text = time;
            UnSelectTxt.text = time;

            SetState(bSelect);
        }

        //设置选中状态
        public void SetState(bool bSelect)
        {
            TagLine.gameObject.SetActive(bSelect ? true : false);
            SelectTxt.gameObject.SetActive(bSelect ? true : false);
            UnSelectTxt.gameObject.SetActive(bSelect ? false : true);
        }

        void IPointerClickHandler.OnPointerClick(PointerEventData eventData)
        {
            m_WndView.JumpToTime(nIndex);
        }
    }

    //时间轴操作
    public class GloryMomentTimeAxisHandle : MonoBehaviour, IScrollHandler
    {
        public int TimeJump = 5;

        private int timeDelta;
        private UViewPlayerGloryMoment m_wndView;

        public void Init(UViewPlayerGloryMoment wndView)
        {
            timeDelta = 0;
            m_wndView = wndView;
        }

        void IScrollHandler.OnScroll(PointerEventData eventData)
        {
            timeDelta++;
            if(timeDelta > TimeJump)
            {
                int dir = eventData.scrollDelta.y > 0 ? 1 : -1;
                m_wndView.MoveTimeAxis(dir);
            }
        }
    }

    //图片分享界面
    public class GloryMomentImageShareView : MonoBehaviour
    {
        public UShareBar shareBar;
        public RawImage ShareImg;
        public int ShareImgWidth;
        public int ShareImgHeight;

        private Texture2D shareTexture;

        private Int64 nDBWarID;
        private string m_fileUrl;
        private string m_fileName;
        private byte[] m_imgBytes;
        private UViewPlayerGloryMoment m_WndView;

        public void Init(UViewPlayerGloryMoment wndView)
        {
            m_WndView = wndView;
            this.gameObject.SetActive(false);
        }

        public void Clear()
        {
            if (shareTexture != null)
            {
                Destroy(shareTexture);
                shareTexture = null;
            }
        }

        public void RestData() 
        {
            nDBWarID = 0;
            m_fileUrl = "";
            m_fileName = "";
            m_imgBytes = null;
            shareBar.Clear();
        }

        public void CloseView()
        {
            RestData();
            this.gameObject.SetActive(false);
        }

        public void OpenView(USocialShareShowMsgData msgData)
        {
            if (msgData.imgData == null || msgData.imgPath == "")
                return;

            RestData();

            ShareImg.gameObject.SetActive(false);

            nDBWarID = msgData.nDBWarID;
            m_fileName = msgData.imgPath;
            m_imgBytes = msgData.imgData;

            if (m_imgBytes != null)
            {
                if (shareTexture == null)
                    shareTexture = new Texture2D(ShareImgWidth, ShareImgHeight);

                if (shareTexture != null)
                {
                    shareTexture.LoadImage(m_imgBytes);
                    if (ShareImg != null)
                    {
                        ShareImg.texture = shareTexture;
                        ShareImg.gameObject.SetActive(true);
                    }
                }
            }
            this.gameObject.SetActive(true);

            shareBar.SetData(SocialSharingType.WonderfulMoment, m_imgBytes);
        }

        public void OnPrevBtnClick()
        {
            m_WndView.PrevItem();
        }

        public void OnNextBtnClick()
        {
            m_WndView.NextItem();
        }

        public void ReqOpenWarData()
        {
            if (m_WndView)
                m_WndView.JumpToPlayerWarView(nDBWarID);
        }

        public void OpenLocalFile()
        {
            string filePath = LogicDataCenter.screenCaptureDataManager.ImgFolderFullPath + "/" + m_fileName;

            string[] path = filePath.Split('/');
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < path.Length; i++)
            {
                sb.Append(path[i]);
                if (i != path.Length - 1)
                    sb.Append("\\");
            }

            if ((System.IO.File.Exists(sb.ToString())))
            {
                System.Diagnostics.Process.Start("explorer.exe", "/select," + sb);
            }
        }
    }

    //视频查看
    public class GloryMomentVideoPlayView : MonoBehaviour
    {
        public MediaPlayer Player;
        public DisplayUGUI DisPlayUGUIComponent;
        public GameObject Controller;
        public Button PlayBtn;
        public Button PauseBtn;
        public Slider ProgressBar;
        public Text TimeLabel;

        private UViewPlayerGloryMoment m_WndView;
        private float m_SeekValue;
        private bool bPlayingOnScrub;

        private Int64 nDBWarID;

        public void Init(UViewPlayerGloryMoment wndView)
        {
            m_WndView = wndView;
            this.gameObject.SetActive(false);
        }

        public void Clear()
        {
            if (Player && Player.Control != null)
                Player.Control.CloseVideo();

            if (DisPlayUGUIComponent._mediaPlayer != null)
                DisPlayUGUIComponent._mediaPlayer = null;
        }

        public void Update()
        {
            if (Player && Player.Info != null && Player.Info.GetDurationMs() > 0f)
            {
                float time = Player.Control.GetCurrentTimeMs();
                float d = time / Player.Info.GetDurationMs();
                m_SeekValue = d;
                ProgressBar.value = d;
                if (ProgressBar.value > 0.9 && Player.Control.IsFinished())
                {
                    PlayBtn.gameObject.SetActive(true);
                    PauseBtn.gameObject.SetActive(false);
                }
                TimeLabel.text = MilliSeconds2Seconds(time) + "/" + MilliSeconds2Seconds(Player.Info.GetDurationMs());
            }
        }

        public void RestData()
        {
            nDBWarID = 0;
            m_SeekValue = 0;
            bPlayingOnScrub = false;
            if (Player != null)
                Player.CloseVideo();
        }

        public void CloseView()
        {
            RestData();
            this.gameObject.SetActive(false);
        }

        public void OpenView(UGloryMomentVideoMsgData msgData)
        {
            if (Player == null)
                return;

            this.gameObject.SetActive(false);
            RestData();

            if (msgData.videoPath == "")
                return;

            this.gameObject.SetActive(true);

            nDBWarID = msgData.nDBWarID;
            Player.m_VideoPath = msgData.videoPath;
            if (Player.OpenVideoFromFile(MediaPlayer.FileLocation.AbsolutePathOrURL, msgData.videoPath))
            {
                Controller.gameObject.SetActive(true);
                PlayBtn.gameObject.SetActive(false);
                PauseBtn.gameObject.SetActive(true);

                TimeLabel.text = "00:00" + "/" + MilliSeconds2Seconds(Player.Info.GetDurationMs());
            }
            else
            {
                Controller.gameObject.SetActive(false);
            }
        }

        public void OnPrevBtnClick()
        {
            m_WndView.PrevItem();
        }

        public void OnNextBtnClick()
        {
            m_WndView.NextItem();
        }

        public void OnVideoSeekSlider()
        {
            if (Player && ProgressBar && ProgressBar.value != m_SeekValue)
            {
                Player.Control.Seek(ProgressBar.value * Player.Info.GetDurationMs());
            }
        }

        public void OnVideoSliderDown()
        {
            if (Player)
            {
                bPlayingOnScrub = Player.Control.IsPlaying();
                if (bPlayingOnScrub)
                {
                    Player.Control.Pause();
                }
                OnVideoSeekSlider();
            }
        }

        public void OnVideoSliderUp()
        {
            if (Player && bPlayingOnScrub)
            {
                Player.Control.Play();
                bPlayingOnScrub = false;
            }
        }

        public void OnPlayBtnClick()
        {
            if (Player)
            {
                if (Player.Control.IsFinished())
                    Player.Control.Rewind();

                Player.Control.Play();
                PlayBtn.gameObject.SetActive(false);
                PauseBtn.gameObject.SetActive(true);
            }
        }

        public void OnPauseBtnClick()
        {
            if (Player)
            {
                Player.Control.Pause();
                PlayBtn.gameObject.SetActive(true);
                PauseBtn.gameObject.SetActive(false);
            }
        }

        public void ReqOpenWarData()
        {
            if (m_WndView)
                m_WndView.JumpToPlayerWarView(nDBWarID);
        }

        public void OpenLocalFile()
        {
            if (null == Player)
                return;

            if ((System.IO.File.Exists(Player.m_VideoPath)))
            {
                System.Diagnostics.Process.Start("explorer.exe", "/select," + Player.m_VideoPath);
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
    }

    public class UViewPlayerGloryMoment : UIBaseSubWndView
    {
        public UButtonGroup PageBtnGroup;
        public RectTransform[] PageBtnList;
        public UListGloryMoment GloryMomentList;

        public Transform TimeAxisContainer;
        public GameObject TimeAxisItem;
        public GloryMomentTimeAxisHandle TimeAxisHandle;

        public Toggle RecToggle;
        public GameObject SliderObj;

        public Toggle DefaultToggle;

        public Text ExplainLabel;
        public Button WarLinkBtn;

        public GloryMomentImageShareView ImgShareView;
        public GloryMomentVideoPlayView VideoPlayView;

        public int MaxTimeItemCount = 9;
        public float RedPointOffset_x = -100f;
        public float RedPointOffset_y = 20f;

        private URedPointTip imgRedPoint;
        private URedPointTip videoRedPoint;

        //当前页面类型
        private EMGloryMomentPageType m_currentPageType;
        private List<CaptureImgData> m_ImgDataList;
        private List<CaptureVideoData> m_videoDataList;

        private string m_currentPageTime
        {
            get
            {
                if (m_timeAxisMap != null && m_timeAxisMap.Count > 0 && m_timeAxisMap.Count > (m_topNode + m_currentTimeAxisIndex))
                    return m_timeAxisMap[m_topNode + m_currentTimeAxisIndex];
                return "";
            }
        }

        private int m_currentTimeAxisIndex = 0;
        private int m_topNode = 0;
        private int m_endNode = 0;
        private List<string> m_timeAxisMap;
        private List<GloryMomentTimeAxisItem> m_timeAxisItemList;

        private int m_fileIndex = 0;

        private bool bRecOpen;

        private EMGloryMomentDataShowType m_currentDataShowType;


        public override bool Init(IUISubWnd wnd)
        {
            m_timeAxisMap = new List<string>();
            m_timeAxisItemList = new List<GloryMomentTimeAxisItem>();
            m_videoDataList = new List<CaptureVideoData>();
            m_ImgDataList = new List<CaptureImgData>();

            m_currentPageType = EMGloryMomentPageType.EMPage_Image;
            m_currentDataShowType = EMGloryMomentDataShowType.EMType_All;

            //侦听列表滚动事件，不能用 OnEndScrolling 因为这个是在延迟一定时间后发出的，数据对不上
            GloryMomentList.OnStartScrolling.AddListener(OnGloryMomentListStartScrolling);

            TimeAxisHandle.Init(this);
            ImgShareView.Init(this);
            VideoPlayView.Init(this);

            LogicDataCenter.screenCaptureDataManager.ReLoad();
            bRecOpen = GameSettingConfig.Instance.GetGameSettingBoolValue(GameSettingIndex.OpenScrenRec);
            RecToggle.isOn = bRecOpen;
            SliderObj.SetActive(!bRecOpen);

            if (PageBtnList.Count() >= 2)
            {
                imgRedPoint = URedPointTip.CreateRedPointObject(PageBtnList[(int)EMGloryMomentPageType.EMPage_Image], RedPointOffset_x, RedPointOffset_y);
                if (imgRedPoint != null)
                    imgRedPoint.SetTipCount(CaptureScreen.GetInstance().WarImgCount);
                videoRedPoint = URedPointTip.CreateRedPointObject(PageBtnList[(int)EMGloryMomentPageType.EMPage_Video], RedPointOffset_x, RedPointOffset_y);
                if (videoRedPoint != null)
                    videoRedPoint.SetTipCount(ScreenREC.GetInstance().WarVideoCount);
            }

            RefreshTimeAxis();
            RefreshDataList();
            RefreshTipPart();

            return base.Init(wnd);
        }

        public override void OnDestroy()
        {
            base.OnDestroy();

            if(ImgShareView)
                ImgShareView.Clear();
            if(VideoPlayView)
                VideoPlayView.Clear();

            if (m_timeAxisMap != null)
            {
                m_timeAxisMap.Clear();
                m_timeAxisMap = null;
            }
            if (m_timeAxisItemList != null)
            {
                m_timeAxisItemList.Clear();
                m_timeAxisItemList = null;
            }
            if (m_ImgDataList != null)
            {
                m_ImgDataList.Clear();
                m_ImgDataList = null;
            }
            if (m_videoDataList != null)
            {
                m_videoDataList.Clear();
                m_videoDataList = null;
            }

            if (imgRedPoint != null)
                URedPointTip.DestroyRedPointTip(imgRedPoint);

            if (videoRedPoint != null)
                URedPointTip.DestroyRedPointTip(videoRedPoint);
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
            if (visible)
            {
                m_currentPageType = EMGloryMomentPageType.EMPage_Image;
                m_currentDataShowType = EMGloryMomentDataShowType.EMType_All;

                PageBtnGroup.SetItemSelectedItem(0);
                DefaultToggle.isOn = true;

                ImgShareView.CloseView();
                VideoPlayView.CloseView();

                RefreshTimeAxis();
                RefreshDataList();
                RefreshTipPart();
            }
            else
            {
                if (imgRedPoint != null)
                {
                    URedPointTip.DestroyRedPointTip(imgRedPoint);
                    imgRedPoint = null;
                }
                if (videoRedPoint != null)
                {
                    URedPointTip.DestroyRedPointTip(videoRedPoint);
                    videoRedPoint = null;
                }
            }
        }

        //请求查看图片
        public void OnImagePageBtnClick(bool bSelect)
        {
            if (!bSelect)
                return;

            if (m_currentPageType != EMGloryMomentPageType.EMPage_Image)
            {
                m_currentPageType = EMGloryMomentPageType.EMPage_Image;
                RefreshTimeAxis();
                RefreshDataList();
                RefreshTipPart();
                ImgShareView.CloseView();
                VideoPlayView.CloseView();

                if (imgRedPoint != null)
                {
                    URedPointTip.DestroyRedPointTip(imgRedPoint);
                    imgRedPoint = null;
                }
            }
        }

        //请求查看视频
        public void OnVideoPageBtnClick(bool bSelect)
        {
            if (!bSelect)
                return;

            if (m_currentPageType != EMGloryMomentPageType.EMPage_Video)
            {
                m_currentPageType = EMGloryMomentPageType.EMPage_Video;
                RefreshTimeAxis();
                RefreshDataList();
                RefreshTipPart();
                ImgShareView.CloseView();
                VideoPlayView.CloseView();

                if (videoRedPoint != null)
                {
                    URedPointTip.DestroyRedPointTip(videoRedPoint);
                    videoRedPoint = null;
                }
            }
        }

        //时间轴移动
        public void MoveTimeAxis(int direction)
        {
            switch (direction)
            {
                    //向下移动
                case -1:
                    {
                        if(m_timeAxisMap.Count <= MaxTimeItemCount)
                        {
                            if (m_currentTimeAxisIndex == m_timeAxisMap.Count - 1)
                                return;
                            else
                                JumpToTime(m_currentTimeAxisIndex + 1);
                        }
                        else
                        {
                            if(m_currentTimeAxisIndex < MaxTimeItemCount - 1)
                            {
                                JumpToTime(m_currentTimeAxisIndex + 1);
                            }
                            else
                            {
                                if (m_endNode == m_timeAxisMap.Count - 1)
                                    return;
                                else
                                {
                                    m_topNode++;
                                    m_endNode++;
                                    for (int i = 0; i < m_timeAxisItemList.Count; i++)
                                    {
                                        m_timeAxisItemList[i].SetData(m_timeAxisMap[m_topNode + i], i == m_currentTimeAxisIndex);
                                    }
                                    RefreshDataList();
                                }
                            }
                        }
                    }
                    break;
                    //向上移动
                case 1:
                    {
                        if (m_currentTimeAxisIndex > 0)
                        {
                            JumpToTime(m_currentTimeAxisIndex - 1);
                        }
                        else
                        {
                            if (m_topNode == 0)
                                return;
                            else
                            {
                                m_topNode--;
                                m_endNode--;
                                for (int i = 0; i < m_timeAxisItemList.Count; i++)
                                {
                                    m_timeAxisItemList[i].SetData(m_timeAxisMap[m_topNode + i], i == m_currentTimeAxisIndex);
                                }
                                RefreshDataList();
                            }
                        }
                    }
                    break;
            }
        }

        //时间轴跳转
        public void JumpToTime(int nIndex)
        {
            if (nIndex == m_currentTimeAxisIndex)
                return;

            m_timeAxisItemList[m_currentTimeAxisIndex].SetState(false);
            m_timeAxisItemList[nIndex].SetState(true);
            m_currentTimeAxisIndex = nIndex;

            RefreshDataList();
        }

        //打开图片分享
        public void OpenImgShareFrame(USocialShareShowMsgData msgData)
        {
            m_fileIndex = msgData.fileIndex;
            ImgShareView.OpenView(msgData);
        }

        //打开视频界面
        public void OpenVedioPlayFrame(UGloryMomentVideoMsgData msgData) 
        {
            m_fileIndex = msgData.fileIndex;
            VideoPlayView.OpenView(msgData);
        }

        //向前跳转
        public void PrevItem()
        {
            if (m_currentPageType == EMGloryMomentPageType.EMPage_Video)
            {
                if (m_fileIndex <= 0 || m_videoDataList.Count < m_fileIndex)
                    return;

                m_fileIndex--;
                if(m_videoDataList[m_fileIndex] == null)
                    return;

                UGloryMomentVideoMsgData msgData = new UGloryMomentVideoMsgData();
                msgData.nDBWarID = m_videoDataList[m_fileIndex].nDBWarID;
                msgData.videoPath = m_videoDataList[m_fileIndex].fileInfo.FullName;
                VideoPlayView.OpenView(msgData);
            }
            else if (m_currentPageType == EMGloryMomentPageType.EMPage_Image)
            {
                if (m_fileIndex <= 0 || m_ImgDataList.Count < m_fileIndex)
                    return;

                m_fileIndex--;
                if (m_ImgDataList[m_fileIndex] == null)
                    return;

                USocialShareShowMsgData msgData = new USocialShareShowMsgData();
                msgData.nDBWarID = m_ImgDataList[m_fileIndex].imgInfo.nDBWarID;
                msgData.imgPath = m_ImgDataList[m_fileIndex].fileInfo.Name;
                msgData.imgData = m_ImgDataList[m_fileIndex].imgByteData;
                ImgShareView.OpenView(msgData);
            }
        }

        public void NextItem()
        {
            if (m_currentPageType == EMGloryMomentPageType.EMPage_Video)
            {
                if (m_fileIndex < 0 || m_fileIndex >= m_videoDataList.Count - 1)
                    return;

                m_fileIndex++;
                if (m_videoDataList[m_fileIndex] == null)
                    return;

                UGloryMomentVideoMsgData msgData = new UGloryMomentVideoMsgData();
                msgData.nDBWarID = m_videoDataList[m_fileIndex].nDBWarID;
                msgData.videoPath = m_videoDataList[m_fileIndex].fileInfo.FullName;
                VideoPlayView.OpenView(msgData);
            }
            else if (m_currentPageType == EMGloryMomentPageType.EMPage_Image)
            {
                if (m_fileIndex < 0 || m_fileIndex >= m_ImgDataList.Count - 1)
                    return;

                m_fileIndex++;
                if (m_ImgDataList[m_fileIndex] == null)
                    return;

                USocialShareShowMsgData msgData = new USocialShareShowMsgData();
                msgData.nDBWarID = m_ImgDataList[m_fileIndex].imgInfo.nDBWarID;
                msgData.imgPath = m_ImgDataList[m_fileIndex].fileInfo.Name;
                msgData.imgData = m_ImgDataList[m_fileIndex].imgByteData;
                ImgShareView.OpenView(msgData);
            }
        }

        public void JumpToPlayerWarView(Int64 nDBWarID)
        {
            if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property == null)
                return;

            UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_WAR);
            int nActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            ButtonType.bIsRequestSelf = true;
            ButtonType.nRequestPDBID = nActorID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, ButtonType);

			WarStatisticViewActorWarData data = new WarStatisticViewActorWarData();
			data.msgID = (int)WndMsgID.WND_MSG_PLAYERWARSTATISTIC_VIEWACTORWAR;
			data.nActorID = nActorID;
			data.lWarID = nDBWarID;
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_VIEWACTORWAR, data);
		}

        //打开某一局的精彩时刻截图
        public void OpenWarData(UGloryMomentReqMsgData msgData) 
        {
            if (null == msgData)
                return;

            int index = m_timeAxisMap.FindIndex( x=> x == msgData.szDataTime);

            if (index < 0)
                return;

            if (m_timeAxisMap.Count <= MaxTimeItemCount)
                JumpToTime(index);
            else
            {
                //跳转时间不在最后一页中
                if (index + MaxTimeItemCount < m_timeAxisMap.Count)
                {
                    m_topNode = index;
                    m_currentTimeAxisIndex = 0;
                    m_endNode = m_topNode + MaxTimeItemCount - 1;
                    for (int i = 0; i < m_timeAxisItemList.Count; i++)
                    {
                        m_timeAxisItemList[i].SetData(m_timeAxisMap[m_topNode + i], i == m_currentTimeAxisIndex);
                    }
                }
                ///要跳转时间在最后一页中
                else
                {
                    m_topNode = m_timeAxisMap.Count - MaxTimeItemCount;
                    m_currentTimeAxisIndex = index - m_topNode;
                    m_endNode = m_timeAxisMap.Count - 1;
                    for (int i = 0; i < m_timeAxisItemList.Count; i++)
                    {
                        m_timeAxisItemList[i].SetData(m_timeAxisMap[m_topNode + i], i == m_currentTimeAxisIndex);
                    }
                }
                RefreshDataList();
            }

            CaptureImgData img = m_ImgDataList.Find(x => x.imgInfo.nDBWarID == msgData.nDBWarID);
            if(img != null)
            {
                USocialShareShowMsgData imgData = new USocialShareShowMsgData();
                imgData.fileIndex = index;
                imgData.nDBWarID = msgData.nDBWarID;
                imgData.imgPath = img.fileInfo.Name;
                imgData.imgData = img.imgByteData;
                OpenImgShareFrame(imgData);

                m_fileIndex = m_ImgDataList.IndexOf(img);
            }
        }

        //切换显示类型
        public void SwitchShowType(int type)
        {
            if(m_currentDataShowType != (EMGloryMomentDataShowType)type)
            {
                m_currentDataShowType = (EMGloryMomentDataShowType)type;
                RefreshDataList();
            }
        }

        //切换REC录制
        public void SwitchRecOpenState(bool bOpen)
        {
            bRecOpen = bOpen;
            SliderObj.SetActive(!bRecOpen);
            int nValue = bRecOpen ? 1 : 0;
            GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.OpenScrenRec, nValue.ToString());
            ScreenREC.GetInstance().OpenREC = bRecOpen;
            RefreshTipPart();
        }

        public void OpenLocalPath()
        {
            string dirPath = ""; 
            if (m_currentPageType == EMGloryMomentPageType.EMPage_Image)
                dirPath = LogicDataCenter.screenCaptureDataManager.ImgFolderFullPath;
            else if (m_currentPageType == EMGloryMomentPageType.EMPage_Video)
                dirPath = LogicDataCenter.screenCaptureDataManager.VideoFolderFullPath;

            if (System.IO.Directory.Exists(dirPath))
            {
                System.Diagnostics.Process.Start(dirPath);
            }
        }

        //刷新数据
        private void RefreshTimeAxis()
        {
            if(m_currentPageType == EMGloryMomentPageType.EMPage_Image)
                m_timeAxisMap = LogicDataCenter.screenCaptureDataManager.GetImgDateTimeList();
            else if(m_currentPageType == EMGloryMomentPageType.EMPage_Video)
                m_timeAxisMap = LogicDataCenter.screenCaptureDataManager.GetVideoDateTimeList();

            int nMaxCount = Mathf.Clamp(m_timeAxisMap.Count, 0, MaxTimeItemCount);

            m_topNode = 0;
            m_endNode = nMaxCount - 1;
            m_currentTimeAxisIndex = 0;

            for (int i = 0; i < m_timeAxisItemList.Count; i++)
            {
                m_timeAxisItemList[i].Clear();
                GameObject go = m_timeAxisItemList[i].gameObject;
                ResNode.DestroyRes(ref go);
            }
            m_timeAxisItemList.Clear();

            for (int i = 0; i < nMaxCount; i++)
            {
                GloryMomentTimeAxisItem item;
                item = ResNode.InstantiateRes<GloryMomentTimeAxisItem>(TimeAxisItem);
                if (item == null)
                    continue;

                item.transform.SetParent(TimeAxisContainer);
                item.transform.localScale = Vector3.one;

                item.Init(this, i, m_timeAxisMap[i], m_currentTimeAxisIndex == i);
                item.gameObject.SetActive(true);
                m_timeAxisItemList.Add(item);
            }
        }

        public void RefreshDataList()
        {
            m_fileIndex = 0;
            GloryMomentList.DataSource.Clear();
            GloryMomentList.DataSource.BeginUpdate();
            if (m_currentPageType == EMGloryMomentPageType.EMPage_Image)
            {
                m_ImgDataList = LogicDataCenter.screenCaptureDataManager.GetImgDataByDateTime(m_currentPageTime);
                if (m_ImgDataList != null)
                {
                    int index = 0;
                    foreach (CaptureImgData data in m_ImgDataList)
                    {
                        int imgType = (int)data.imgInfo.screenShotType;

                        if (imgType < (int)EMScreenShotType.EMSS_TripleKill || imgType > (int)EMScreenShotType.EMSS_Max)
                        {
                            continue;
                        }

                        if (m_currentDataShowType != EMGloryMomentDataShowType.EMType_All && (int)imgType != (int)m_currentDataShowType)
                        {
                            continue;
                        }

                        UListItemGloryMoment item = new UListItemGloryMoment();
                        item.Index = index;
                        item.TagIconID = imgType - 3;
                        item.heroID = data.imgInfo.nHeroID;
                        item.dbWarID = data.imgInfo.nDBWarID;
                        item.date = m_currentPageTime;
                        item.time = data.fileInfo.CreationTime.ToString("HH:mm:ss");
                        item.fileName = data.fileInfo.Name;
                        item.imgData = data.imgByteData;
                        item.pageType = EMGloryMomentPageType.EMPage_Image;
                        GloryMomentList.DataSource.Add(item);
                        index++;
                    }
                }
            }
            else if (m_currentPageType == EMGloryMomentPageType.EMPage_Video)
            {
                m_videoDataList = LogicDataCenter.screenCaptureDataManager.GetVideoDataByDateTime(m_currentPageTime);
                if (m_videoDataList != null)
                {
                    int index = 0;
                    foreach (CaptureVideoData data in m_videoDataList)
                    {
                        int imgType = (int)data.fileType;

                        if (imgType < (int)EMScreenRECType.EMSS_TripleKill || imgType > (int)EMScreenRECType.EMSS_Max)
                        {
                            continue;
                        }

                        if (m_currentDataShowType != EMGloryMomentDataShowType.EMType_All && (int)imgType != (int)m_currentDataShowType)
                        {
                            continue;
                        }

                        //string fileTime = data.fileInfo.CreationTime.ToString("yyyy-MM-dd");
                        CaptureImgData matchImg = LogicDataCenter.screenCaptureDataManager.GetMatchImg(data.fileInfo.CreationTime, imgType);

                        UListItemGloryMoment item = new UListItemGloryMoment();
                        item.Index = index;
                        item.TagIconID = imgType - 3;
                        item.heroID = data.nHeroID;
                        item.dbWarID = data.nDBWarID;
                        item.date = m_currentPageTime;
                        item.time = data.fileInfo.CreationTime.ToString("HH:mm:ss");
                        item.fileName = data.fileInfo.FullName;
                        item.imgData = matchImg == null ? null : matchImg.imgByteData;
                        item.pageType = EMGloryMomentPageType.EMPage_Video;
                        GloryMomentList.DataSource.Add(item);
                        index++;
                    }
                }
            }
            GloryMomentList.DataSource.EndUpdate();
        }

        public void RefreshTipPart()
        {
            if (null == m_timeAxisItemList || m_timeAxisItemList.Count > 0)
            {
                ExplainLabel.text = "";
                WarLinkBtn.gameObject.SetActive(false);
                return;
            }

            if (m_currentPageType == EMGloryMomentPageType.EMPage_Image)
            {
                string szExplain = ULocalizationService.Instance.Get("UIView", "GloryMoment", "ImgEmpty");
                U3D_Render.Common.UBB.toHtml(ref szExplain);
                ExplainLabel.text = szExplain;
                WarLinkBtn.GetComponentInChildren<Text>().text = ULocalizationService.Instance.Get("UIView", "GloryMoment", "TakePartIn");
                WarLinkBtn.onClick.RemoveAllListeners();
                WarLinkBtn.onClick.AddListener(JumpToMakeWar);
                WarLinkBtn.gameObject.SetActive(true);
            }
            else
            {
                if (bRecOpen)
                {
                    string szExplain = ULocalizationService.Instance.Get("UIView", "GloryMoment", "VideoEmpty_openRec");
                    U3D_Render.Common.UBB.toHtml(ref szExplain);
                    ExplainLabel.text = szExplain;
                    WarLinkBtn.GetComponentInChildren<Text>().text = ULocalizationService.Instance.Get("UIView", "GloryMoment", "TakePartIn");
                    WarLinkBtn.onClick.RemoveAllListeners();
                    WarLinkBtn.onClick.AddListener(JumpToMakeWar);
                    WarLinkBtn.gameObject.SetActive(true);
                }
                else
                {
                    string szExplain = ULocalizationService.Instance.Get("UIView", "GloryMoment", "ImgEmpt_CloseRec");
                    U3D_Render.Common.UBB.toHtml(ref szExplain);
                    ExplainLabel.text = szExplain;
                    WarLinkBtn.GetComponentInChildren<Text>().text = ULocalizationService.Instance.Get("UIView", "GloryMoment", "OpenRec");
                    WarLinkBtn.onClick.RemoveAllListeners();
                    WarLinkBtn.onClick.AddListener(OpenRec);
                    WarLinkBtn.gameObject.SetActive(true);
                }
            }
        }

        public void JumpToMakeWar()
        {
            PlayerButtonFrameSimulateOpenData uiData = new PlayerButtonFrameSimulateOpenData();
            uiData.eViewType = PlayerButtonFrame.ESimulateOpenViewType.ESOP_WAR_MAIN;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_SIMULATEVIEWOPEN, uiData);
        }

        public void OpenRec()
        {
            RecToggle.isOn = true;
        }

        /// <summary>
        /// 列表滚动事件
        /// </summary>
        protected void OnGloryMomentListStartScrolling()
        {
            float normalizedPos = GloryMomentList.ScrollRect.verticalNormalizedPosition;
            int dir = (int)EventSystem.current.CurrentPointerEventData().scrollDelta.y <= -1 ? -1 : 1;
            //Mathf.Clamp((int)EventSystem.current.CurrentPointerEventData().scrollDelta.y, -1, 1);

            if (EventSystem.current.CurrentPointerEventData() != null &&
                (normalizedPos <= 0.02f || normalizedPos >= (1 - 0.02f)))
            {
                MoveTimeAxis(dir);
            }
        }
    }
}
