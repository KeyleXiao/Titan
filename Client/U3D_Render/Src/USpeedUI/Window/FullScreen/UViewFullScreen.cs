using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using ASpeedGame.Data.LoadingWarTipConfig;
using U3D_Render.Common;

namespace USpeedUI.FullScreen
{
    public class UMediatorFullScreen : UIFillWnd<UViewFullScreen>
    {
        private bool m_bForceConditionHide = false;
        public override WndID GetID()
        {
            return WndID.WND_ID_FULLSCREEN;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FILL;
        }

        public override string GetPrefabPath()
        {
            return "FullScreen/FullScreenView";
        }

        public override bool Init()
        {
            LoadView();

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_CHOOSESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_FULLSCREEN_CHANGEIMAGE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_LOADSCENESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_LOADSCENESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_GAMEWAITINGROOMVISILBLE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_FALLBACK, this);

            return base.Init();
        }
        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_CHOOSESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_FULLSCREEN_CHANGEIMAGE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_LOADSCENESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_LOADSCENESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_GAMEWAITINGROOMVISILBLE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CREATEHERO_FALLBACK, this);

        }
        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            if (m_wndView == null)
                return;
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_LOADSCENESTATE_ENTER:

                    {
                        if (m_bForceConditionHide)
                        {
                            return;
                        }

                        SetVisible(true);

                        if (m_wndView != null)
                            m_wndView.OnWndOpen();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
                    {
                        SetVisible(true);
                        //m_wndView.ChangeFullScreenSprite(1);
                        m_wndView.OpenFullScreenMovie();

                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_LEAVE:
                    {
                        m_wndView.CloseFullScreenMovie();
                        SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_CREATEHERO_FALLBACK:
                    {
                        m_wndView.OpenFullScreenMovie();
                        //m_wndView.ChangeFullScreenSprite(1);

                        SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_CHOOSESTATE_ENTER:
                    {
                        m_wndView.ChangeFullScreenSprite(0);
                        SetVisible(true);

                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                case WndMsgID.WND_MSG_COMMOM_LOADSCENESTATE_LEAVE:
                    {
                        SetVisible(false);

                        if (m_wndView != null)
                            m_wndView.OnWndClose();
                    }
                    break;
                case WndMsgID.WND_MSG_FULLSCREEN_CHANGEIMAGE:
                    {
                        UFullScreenStateData ViewData = data as UFullScreenStateData;
                        m_wndView.ChangeFullScreenSprite(ViewData.FullScreenImageID);
                        SetVisible(ViewData.bFullScreenShow);
                    }
                    break;
                case WndMsgID.WND_MSG_GAMEWAITINGROOMVISILBLE:
                    {
                        SetForceConditionHide(data as FullViewForceConditionHide);
                    }
                    break;
            }
        }

        public void SetForceConditionHide(FullViewForceConditionHide data)
        {
            m_bForceConditionHide = data.bForceHide;
        }
    }
    public class UViewFullScreen : UIBaseWndView
    {
        protected Image FullScreenImage;
        public Sprite TempleFullScreen;
        public Sprite FullScreenSprite2;

        public RawImage FullScreenMovieImage;
        public AudioSource FullScreenMovieAudio;
        public MovieTexture FullScreenMovieTexture;
        public string FullScreenMovieUrl;

        private LoginVideo logvideo = null;

        public int[] RandomSpriteIndex;

        public Text TipDesc;
        public Text TipTitle;
        public GameObject TipGo;

        public override bool Init(IUIWnd wnd)
        {
            FullScreenImage = this.GetComponent<Image>();
            logvideo = this.GetComponent<LoginVideo>();
            if (!logvideo)
            {
                logvideo = gameObject.AddComponent<LoginVideo>();
            }
            logvideo.Init();
            TipGo.SetActive(false);
            return base.Init(wnd);
        }

        public void OnWndOpen()
        {
            SSchemeLoadingWarTip tip = LoadingWarTipConfig.Instance.GetRandTipConfig(ELoadingTipType.ELTT_SECOND);
            if (tip != null)
            {
                string strContent = tip.sTipContent;
                UBB.toHtml(ref strContent);
                TipDesc.text = strContent;
                TipTitle.text = tip.sTipTitle;
            }

            TipGo.SetActive(false);
            //TipGo.SetActive(true);
        }

        public void OnWndClose()
        {
            TipGo.SetActive(false);
        }

        public override void Destroy()
        {
            if (FullScreenMovieAudio != null)
                FullScreenMovieAudio.clip = null;
            if (FullScreenMovieImage != null)
                FullScreenMovieImage.texture = null;
            if (FullScreenMovieTexture != null)
                FullScreenMovieTexture = null;

            base.Destroy();
        }

        internal void ChangeFullScreenSprite(int _nID)
        {
            Sprite tmpSprite;
            switch (_nID)
            {
                case -1:
                    FullScreenImage.enabled = false;
                    return;
                case 1:
                    tmpSprite = FullScreenSprite2;
                    break;
                default:
                    {
                        int randomIndex = UnityEngine.Random.Range(0, RandomSpriteIndex.Length);
                        Sprite sp = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_FullScreen, m_wnd.GetID(), RandomSpriteIndex[randomIndex]);
                        if (sp == USpriteManager.Instance.DefaultSprite)
                            Debug.LogWarning("无效全屏图：randomIndex:" + randomIndex + ",sp:" + sp + ",spriteID:" + RandomSpriteIndex[randomIndex]);
                        tmpSprite = sp;

                    }
                    break;
            }

            FullScreenImage.enabled = true;
            FullScreenImage.sprite = tmpSprite;
        }

        internal void OpenFullScreenMovie()
        {

            ////Debug.Log("path:" + FullScreenMovieUrl);
            ////StartCoroutine(fooCoroutineThatStartsTheVideo(FullScreenMovieUrl));
            ////return;
            //if (FullScreenMovieImage == null || FullScreenMovieTexture == null )//|| FullScreenMovieAudio == null)
            //    return;

            //FullScreenMovieImage.texture = FullScreenMovieTexture;
            ////FullScreenMovieAudio.clip = FullScreenMovieTexture.audioClip;
            //FullScreenMovieTexture.loop = true;

            //FullScreenMovieImage.gameObject.SetActive(true);

            //StartCoroutine(DelayCheckRenderingQuality());
            //FullScreenMovieTexture.Play();

            //ImageSetting.OnSceneRenderingQualityChanged += onImageSettingSceneRenderingQualityChanged;
            //Initialize.OnDeviceStateChage += onDeviceStateChange;
            ////FullScreenMovieAudio.Play();
            //SceneSoundHelper.PlayUISceneSound((int)SoundIDIndex.MapLogin);
            logvideo.Init();
            logvideo.Play();
        }

        internal void CloseFullScreenMovie()
        {
            //if (!FullScreenMovieTexture.isPlaying)
            //    return;
            //SceneSoundHelper.StopUISceneSound(false);
            //ImageSetting.OnSceneRenderingQualityChanged -= onImageSettingSceneRenderingQualityChanged;
            //Initialize.OnDeviceStateChage -= onDeviceStateChange;

            //FullScreenMovieTexture.Stop();
            ////FullScreenMovieAudio.Stop();
            //FullScreenMovieImage.gameObject.SetActive(false);
            logvideo.Stop();
            logvideo.Clear();
        }

        IEnumerator fooCoroutineThatStartsTheVideo(string url)
        {
            WWW videoStreamer = new WWW(url);
            while (!videoStreamer.isDone)
                yield return 0;

            FullScreenMovieTexture = videoStreamer.movie;

            while (!FullScreenMovieTexture.isReadyToPlay)
            {
                yield return 0;
            }

            FullScreenMovieTexture.Play();
            FullScreenMovieTexture.loop = true;
        }

        IEnumerator DelayCheckRenderingQuality()
        {
            {
                yield return null;
                if (ImageSetting.GetData().currSceneRenderingQuality <= SceneRenderingQuality.Fastest)
                {
                    FullScreenMovieTexture.Stop();
                }
            }
        }

        void onImageSettingSceneRenderingQualityChanged(SceneRenderingQuality _newQuality, SceneRenderingQuality _oldQuality)
        {
            if (FullScreenMovieImage.gameObject.activeSelf)
            {
                if (_newQuality <= SceneRenderingQuality.Fastest)
                {
                    FullScreenMovieTexture.Stop();
                    FullScreenMovieTexture.loop = false;
                }
                else
                {
                    FullScreenMovieTexture.Play();
                    FullScreenMovieTexture.loop = true;

                }
            }
        }

        void onDeviceStateChange(bool isDeviceLost)
        {
            if (!isDeviceLost)
            {
                if (FullScreenMovieTexture != null)
                {
                    FullScreenMovieTexture.Stop();
                    FullScreenMovieTexture.Play();
                }
            }
        }
    }
}
