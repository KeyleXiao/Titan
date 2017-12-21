using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using DG.Tweening;
using USpeedUI.UEffect;

namespace USpeedUI.LoadingBar
{
    public class UMediatorLoadingBar : UIPopupWnd<UViewLoadingBar>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LOADINGBAR;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        public override string GetPrefabPath()
        {
            return "LoadingBar/LoadingBarView";
        }

        public override bool Init()
        {
            //订阅登录界面离开
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_LEAVE, this);
            //订阅进入选择英雄界面
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_CHOOSESTATE_ENTER, this);
            //订阅离开选择英雄界面
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_CHOOSESTATE_LEAVE, this);
            //订阅进入静态场景状态
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);

            //订阅加载进度
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_LOADING_VALUE, this);

            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_LOADING_VALUE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_CHOOSESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_CHOOSESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);

        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_LEAVE:
                case WndMsgID.WND_MSG_COMMON_CHOOSESTATE_LEAVE:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        //if (WndMsgID.WND_MSG_COMMON_CHOOSESTATE_LEAVE != msgID)
                        //{
                        //    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LOGIN_SERVER_MESSAGE,
                        //    ULocalizationService.Instance.Get("UIView", "LoadingBar", "GameStateChange"));
                        //}

                        LoadingValue = 0f;
                        m_bIsEnterStaticGameState = false;
                        m_nSceneLoadingValue = 0f;

                        SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_CHOOSESTATE_ENTER:
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
                    {
                        SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        if(m_LoadingTweener != null)
                            m_bIsEnterStaticGameState = true;
                        else
                            UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_LOADING_VALUE:
                    {
                        if (m_wndView!=null)
                        {
                            UIMsgCmdData ViewData = data as UIMsgCmdData;
                            SceneLoadingValue = (float)ViewData.nParam / 100f;
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        #region 加载动画处理逻辑
        private Tweener m_LoadingTweener;
        protected float m_fTweenDuration = 10.0f;
        /// <summary>
        /// 当前加载值
        /// </summary>
        public float LoadingValue;
        protected bool m_bIsEnterStaticGameState;

        /// <summary>
        /// 当前逻辑下发的加载值
        /// </summary>
        protected float m_nSceneLoadingValue;
        public float SceneLoadingValue
        {
            get { return m_nSceneLoadingValue; }
            set
            {
                m_nSceneLoadingValue = value ;
                if (m_LoadingTweener == null)
                {
                    m_LoadingTweener = DOTween.To(() => LoadingValue, v => LoadingValue = v, m_nSceneLoadingValue, m_fTweenDuration);
                    m_LoadingTweener.OnComplete(OnLoadingTweenerComplete).OnUpdate(OnLoadingTweenerUpdate).SetAutoKill(true).OnKill(() => m_LoadingTweener = null);
                }
                else
                {
                    //逻辑加载完成时，改变显示加载持续时间
                    if (m_nSceneLoadingValue == 1f)
                        //m_LoadingTweener.Complete(true);
                        //这是为了让进度条填满而改的，如果进度条消失和背景消失严重不同步的话，改用上面一句
                        m_LoadingTweener.ChangeEndValue(m_nSceneLoadingValue, 0.2f, true);
                    else
                        m_LoadingTweener.ChangeEndValue(m_nSceneLoadingValue, true);
                }
            }
        }

        protected void OnLoadingTweenerComplete()
        {
            m_wndView.OnLoadingTweenerComplete();
            if (m_bIsEnterStaticGameState)
            {
                UnloadView();
            }
                
        }
        protected void OnLoadingTweenerUpdate()
        {
            m_wndView.OnLoadingTweenerUpdate(LoadingValue );
        }
        #endregion
    }
    public class UViewLoadingBar : UIBaseWndView
    {
        public RectTransform StartTrans;
        public Image LightImage;

        /// <summary>
        /// 图片边缘偏移
        /// </summary>
        public float LightImageOffsetValue;
        private float LightImageWidth;

        private UEffectParamBase tmpEffectParamL;
        private UEffectParamBase tmpEffectParamR;
        
        //挂接特效的对象
        private GameObject StartImage;
        //特效对象
        private GameObject tmpEffectL;

        public override bool Init(IUIWnd wnd)
        {
            if (StartTrans == null)
                StartTrans = transform.FindChild("Start").GetComponent<RectTransform>();
            if (LightImage == null)
                LightImage = transform.FindChild("Light").GetComponent<Image>();

            StartImage = StartTrans.FindChild("Image").gameObject;
            StartImage.SetActive(false);
            LightImageWidth = LightImage.rectTransform.sizeDelta.x;

            return base.Init(wnd);
        }

        /// <summary>
        /// 这个是真实显示的
        /// </summary>
        public float TweenerLoadingValue
        {
            set
            {
                Debug.LogWarning("现在不用这个方法了。");
                UpdateValue(value);
            }
            get
            {
                Debug.LogWarning("现在不用这个方法了。");
                return LightImage.fillAmount;
            }
        }

        private void UpdateValue(float _nPercent)
        {
            LightImage.fillAmount = _nPercent;
            Vector3 localPos = StartTrans.anchoredPosition;
            if (LightImageWidth <= 0)
                LightImageWidth = LightImage.rectTransform.sizeDelta.x;

            float offset = 0;
            if (_nPercent > 0.5f)
                offset = LightImageOffsetValue * (-1);

            localPos.x = LightImageWidth * LightImage.fillAmount + offset;
            StartTrans.anchoredPosition = localPos;

        }

        public override void Destroy()
        {
            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref tmpEffectParamL);
            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref tmpEffectParamR);
            base.Destroy();
        }

        private void UpdateEffect(float _nPercent)
        {
            
            if (_nPercent > 0.01f && tmpEffectL == null)
            {
                tmpEffectParamL = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_LoadingBarView__Star_L, _tfAttachParent: StartTrans);
                UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref tmpEffectParamL);
                tmpEffectL = (tmpEffectParamL as UEffect.UEffectPrefabParam).EffectGameObject;
            }
            else if (_nPercent > 0.0f)
            {
                StartImage.SetActive(true);
            }

        }
        
        /// ////////////////////////////////////////////////////////////////
        
        public void OnLoadingTweenerComplete()
        {

        }

        public void OnLoadingTweenerUpdate(float _fUpdateValue)
        {
            UpdateValue(_fUpdateValue);

            if (Application.isPlaying)
                UpdateEffect(_fUpdateValue);
        }
    }

}
