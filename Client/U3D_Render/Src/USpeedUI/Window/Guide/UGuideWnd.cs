/*******************************************************************
** 文件名:	UGuideWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-12-12
** 版  本:	1.0
** 描  述:	引导UI显示文件
		    主要处理引导文字显示动画部分，目前引导只显示一条，多余的延迟显示
********************************************************************/
using DG.Tweening;
using GUIDE;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using U3D_Render.Common;
using UIWidgets;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using USpeedUI.UEffect;
using USpeedUI.UExtensions;
using USpeedUI.UWidgets;

namespace USpeedUI.Guide
{
    public class UGuideWnd :UIPopupWnd<UGuideWndView>//,ITimerHanlder
    {
        /// <summary>
        /// 确保移除消息时是同一个引导ID触发的
        /// </summary>
        private EGuideNodeID m_eAddMessageGuideNodeID;

        private List<UGuideMsgData> m_messageQueue = new List<UGuideMsgData>();
        private bool m_bAnimPlaying = false;

        private const int m_nPlayTimerID_0 = 0;
        private bool m_bRunningTimerID_0 = false;

        private int m_nDelayPlayAnimDuration = 10;

        /// <summary>
        /// 目前主要用于战场统计战绩时会有提示弹出
        /// </summary>
        private bool m_bEnableGuideUI = true;



        public override WndID GetID()
        {
            return WndID.WND_ID_GAMEGUIDE;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
           return UPath.UCommomState + "/Guide/GuideView";
        }
        public override bool Init()
        {
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_GAMEGUIDE_ADDNODEMESSAGE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_GAMEGUIDE_REMOVENODEMESSAGE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_GAMEGUIDE_REMOVEALLMESSAGE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_GAMEGUIDE_ADDNODEREWARDMESSAGE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_GAMEGUIDE_REMOVENODEREWARDMESSAGE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);

            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_GAMEGUIDE_ADDNODEMESSAGE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_GAMEGUIDE_REMOVENODEMESSAGE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_GAMEGUIDE_REMOVEALLMESSAGE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_GAMEGUIDE_ADDNODEREWARDMESSAGE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_GAMEGUIDE_REMOVENODEREWARDMESSAGE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
        }

        protected override bool CheckWndLayerState(bool _bVisible)
        {
            return true;
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch(msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    this.SetVisible(false);
                    LoadView();
                    
                    break;
                case WndMsgID.WND_MSG_GAMEGUIDE_ADDNODEMESSAGE:
                    {
                        //this.SetVisible(true);
                        UGuideMsgData gData = data as UGuideMsgData;

                        if (m_wndView == null || gData == null || string.IsNullOrEmpty(gData.strMsgText))
                            break;
                        if(gData != null)
                        {
                            #region 旧版处理
                            //m_messageQueue.Add(gData);
                            //if(m_messageQueue.Count >=1 && m_bAnimPlaying && !m_bRunningTimerID_0)
                            //{
                            //    m_bRunningTimerID_0 = true;
                            //    TimerManager.SetTimer(this, m_nPlayTimerID_0, m_nDelayPlayAnimDuration, 1);
                            //}
                            //PlayAnim();
                            #endregion
                            UBB.toHtml(ref gData.strMsgText);
                            SetVisible(true);
                            m_wndView.Show(gData);

                            ToggleModal(gData);


                        }                        
                    }                    
                    break;
                case WndMsgID.WND_MSG_GAMEGUIDE_REMOVENODEMESSAGE:
                    {
                        if (m_wndView == null)
                            break;
                        UGuideMsgRemoveData gData = data as UGuideMsgRemoveData;
                        if (gData != null )//&& gData.nGuideNodeID == m_eAddMessageGuideNodeID)
                        {
                            #region 旧版处理
                            //if (gData.nGuideNodeID == m_eAddMessageGuideNodeID && !m_bRunningTimerID_0)
                            //    m_wndView.RemoveNode();
                            //else
                            //{
                            //    int removeIndex = m_messageQueue.FindIndex((queueData) => { if (queueData.nGuideNodeID == gData.nGuideNodeID) return true; else return false; });
                            //    if(removeIndex != -1)
                            //        m_messageQueue.RemoveAt(removeIndex);
                            //}
                            #endregion
                            if(gData.IsRemoveMessage)
                                m_wndView.Hide();
                            if(gData.IsRemoveModal)
                                m_wndView.HideModal();
                            if(gData.IsHideView)
                                SetVisible(false);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
                    {
                        if (m_wndView != null)
                        {
                            #region 旧版处理
                            //m_wndView.ClearNode();
                            //TimerManager.KillTimer(this, m_nPlayTimerID_0);
                            #endregion
                            m_wndView.Clear();
                            m_wndView.HideModal();

                            SetVisible(false);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                case WndMsgID.WND_MSG_COMMON_WAREND:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                case WndMsgID.WND_MSG_GAMEGUIDE_REMOVEALLMESSAGE:
                    {

                        if (m_wndView == null)
                            break;
                        #region 旧版处理
                        //m_messageQueue.Clear();
                        //m_wndView.RemoveNode();
                        //TimerManager.KillTimer(this, m_nPlayTimerID_0);
                        #endregion

                        //WND_MSG_COMMON_WAREND 关闭引导提示
                        //WND_MSG_COMMON_BATTLESTATE_LEAVE 启用引导提示
                        m_bEnableGuideUI = msgID == WndMsgID.WND_MSG_COMMON_WAREND ? false : msgID == WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE ? true : true;

                            
                        m_wndView.Clear();
                        m_wndView.HideModal();

                        SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_GAMEGUIDE_ADDAUTOMESSAGE:
                    {
                        if (m_wndView == null || !m_bEnableGuideUI)
                            break;
                        UGuideMsgData gData = data as UGuideMsgData;
                        if (gData != null)
                        {
                            UBB.toHtml(ref gData.strMsgText);
                            m_wndView.ShowAutoMessage(gData.strMsgText,gData.nGuideSignSpriteID);
                            SetVisible(true);
                        }                           

                    }
                    break;
                case WndMsgID.WND_MSG_GAMEGUIDE_ALWAYMESSAGE:
                    {
                        if (m_wndView == null)
                            break;

                        UGuideMsgData gData = data as UGuideMsgData;
                        if (gData != null)
                        {
                            UBB.toHtml(ref gData.strMsgText);
                            m_wndView.ShowAlwaysDisplayMessage(gData.strMsgText);
                            SetVisible(true);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GAMEGUIDE_ADDGUIDEEFFECT:
                    {
                        if (m_wndView == null)
                            break;
                        UGuideWidgetMsgData gData = data as UGuideWidgetMsgData;
                        if(gData != null)
                        {
                            m_wndView.AddGuideEffect(gData.nGuideEffectID);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GAMEGUIDE_REMOVEGUIDEEFFECT:
                    {
                        if (m_wndView == null)
                            break;
                        UGuideWidgetMsgData gData = data as UGuideWidgetMsgData;
                        if (gData != null)
                        {
                            m_wndView.RemoveGuideEffect(gData.nGuideEffectID);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GAMEGUIDE_ADDNODEREWARDMESSAGE:
                    {
                        UGuideMsgData gData = data as UGuideMsgData;

                        if (m_wndView == null || gData == null || string.IsNullOrEmpty(gData.strMsgText))
                            break;
                        if (gData != null)
                        {
                            UBB.toHtml(ref gData.strMsgText);
                            SetVisible(true);
                            m_wndView.ShowReward(gData);

                            ToggleModal(gData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_GAMEGUIDE_REMOVENODEREWARDMESSAGE:
                    {
                        if (m_wndView == null)
                            break;
                        UGuideMsgRemoveData gData = data as UGuideMsgRemoveData;
                        if (gData != null)
                        {
                            if (gData.IsRemoveMessage)
                                m_wndView.HideReward();
                            if (gData.IsRemoveModal)
                                m_wndView.HideModal();
                            if (gData.IsHideView)
                                SetVisible(false);
                        }
                    }
                    break;
            }

        }

        protected void ToggleModal(UGuideMsgData _gData)
        {
            if (_gData.bModal)
                m_wndView.ShowModal();
            else
                m_wndView.HideModal();
        }

        #region 旧版处理
        //private void PlayAnim()
        //{

        //    if (m_bAnimPlaying || m_messageQueue.Count <= 0)
        //        return;

        //    if(m_messageQueue.Count > 0)
        //    {
        //        m_bAnimPlaying = true;

        //        UGuideMsgData gData = m_messageQueue[0];
        //        m_messageQueue.RemoveAt(0);

        //        UBB.toHtml(ref gData.strMsgText);
        //        m_eAddMessageGuideNodeID = gData.nGuideNodeID;
        //        m_wndView.AddNode(gData);
        //    }
        //}
        //internal void ResetAnim()
        //{
        //    m_bAnimPlaying = false;
        //    PlayAnim();
        //}

        //public void OnTimer(int nTimerID)
        //{
        //    switch(nTimerID)
        //    {
        //        case m_nPlayTimerID_0:
        //            {
        //                m_bRunningTimerID_0 = false;
        //                m_wndView.RemoveNode();

        //                if(m_messageQueue.Count >= 2)
        //                {
        //                    m_bRunningTimerID_0 = true;
        //                    TimerManager.ChangeTimer(this, m_nPlayTimerID_0, m_nDelayPlayAnimDuration, 2);
        //                }
        //            }
        //            break;
        //    }
        //}
        #endregion
    }

    /// <summary>
    /// 2017.3.29新版UI的View
    /// </summary>
    public class UGuideWndView : UIBaseWndView
    {
        [SerializeField]
        public EGuideDebugType GuideManagerDebugType = EGuideDebugType.None;

        /// <summary>
        /// 当前主引导模板对象
        /// </summary>
        protected UGuideNodeDisplayComponent DefaultComponent;
        /// <summary>
        /// 主引导模板对象类型0
        /// </summary>
        public UGuideNodeDisplayComponent DefaultComponent_Type0;
        /// <summary>
        /// 主引导模板对象类型1
        /// </summary>
        public UGuideNodeDisplayComponent DefaultComponent_Type1;

        /// <summary>
        /// 自动移除的引导模板对象
        /// </summary>
        public UGuideNodeDisplayComponent AutoRemoveComponent;
        /// <summary>
        /// 一直显示的引导模板对象
        /// </summary>
        public UGuideNodeDisplayComponent AlwaysDisplayComponent;
        /// <summary>
        /// 奖励引导模版对象
        /// </summary>
        public UGuideNodeRewardComponent RewardComponent;

        /// <summary>
        /// 自动移除时间
        /// </summary>
        public int AutoRemoveDelayTime = 5000;
        /// <summary>
        /// 自动移除渐变持续时间
        /// </summary>
        public float AutoMessageFadeTime = 1f;
        /// <summary>
        /// 自动移除渐变动画对象
        /// </summary>
        protected Tweener m_AutoMessageTweener = null;
        /// <summary>
        /// 当前使用中的引导模板对象列表
        /// </summary>
        protected List<UGuideNodeDisplayComponent> m_listUsingComponent;
        /// <summary>
        /// 当前使用中的奖励模板对象列表
        /// </summary>
        protected List<UGuideNodeRewardComponent> m_listUsingRewardComponent;
        /// <summary>
        /// 是否需要更新模板对象的层级
        /// </summary>
        protected bool m_bNeedUpdateSibling = false;

        protected int? m_nModalKey = null;
        protected Color? ModalColor = null;


        protected List<UEffect.UEffectParamBase> m_dicGuideEffect;
        protected UEffectParamBase m_RewardEffect;

        /// <summary>
        /// 默认的用户自定义数据
        /// </summary>
        protected UGuideMsgData.CustomUserData m_defaultCustomUserData;

        public override bool Init(IUIWnd wnd)
        {
            if(DefaultComponent_Type0 != null)
                DefaultComponent_Type0.gameObject.SetActive(false);
            if (DefaultComponent_Type1 != null)
                DefaultComponent_Type1.gameObject.SetActive(false);
            DefaultComponent = DefaultComponent_Type0;

            if (AutoRemoveComponent != null)
                AutoRemoveComponent.gameObject.SetActive(false);
            if (AlwaysDisplayComponent != null)
                AlwaysDisplayComponent.gameObject.SetActive(false);

            if (RewardComponent != null)
                RewardComponent.gameObject.SetActive(false);

            m_listUsingComponent = new List<UGuideNodeDisplayComponent>();
            m_listUsingRewardComponent = new List<UGuideNodeRewardComponent>();
            m_dicGuideEffect = new List<UEffectParamBase>();


            m_defaultCustomUserData = new UGuideMsgData.CustomUserData(0f,250f,0);
            //ModalColor = new Color(0, 0, 0, 233f / 255f);
            return base.Init(wnd);
        }

        public override void Destroy()
        {
            base.Destroy();
            if(m_listUsingComponent != null)
            {
                m_listUsingComponent.Clear();
                m_listUsingComponent = null;
            }
            if(m_listUsingRewardComponent != null)
            {
                m_listUsingRewardComponent.Clear();
                m_listUsingRewardComponent = null;
            }
            if (m_dicGuideEffect != null)
            {
                for (int i = 0; i < m_dicGuideEffect.Count; ++i)
                {
                    UEffectParamBase param = m_dicGuideEffect[i];
                    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
                }
                m_dicGuideEffect.Clear();
                m_dicGuideEffect = null;
            }
            if (m_RewardEffect != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_RewardEffect);
                m_RewardEffect = null;
            }
        }

        internal void Clear()
        {
            Hide();
            HideAutoMessage();
            HideAlwaysDisplayMessage();
        }

        internal void Show(UGuideMsgData _gData)
        {
            if (DefaultComponent == null)
                return;
            //必须先执行 SetUserDataDefaultComponent，因为里面会设置DefaultComponent 用哪个类型
            SetUserDataDefaultComponent(_gData.UserData);
            SetUserData(_gData.UserData,ref DefaultComponent);

            DefaultComponent.GuideTextField.text = string.Empty;
            DefaultComponent.GuideTextField.DOText(_gData.strMsgText, 1.0f).SetAutoKill(true);

            DefaultComponent.gameObject.SetActive(true);
            if (!m_listUsingComponent.Contains(DefaultComponent))
                m_listUsingComponent.Add(DefaultComponent);
            m_bNeedUpdateSibling = true;
        }

        internal void Hide()
        {
            if (DefaultComponent == null)
                return;
            DefaultComponent.GetRectTransform.SetParent(GetTransform(), true);
            DefaultComponent.gameObject.SetActive(false);
            m_listUsingComponent.Remove(DefaultComponent);
        }

        internal void ShowReward(UGuideMsgData _gData)
        {
            if (RewardComponent == null)
                return;

            RewardComponent.GuideTextField.text = string.Empty;
            RewardComponent.GuideTextField.DOText(_gData.strMsgText, 1.0f).SetAutoKill(true);

            RewardComponent.gameObject.SetActive(true);
            if (!m_listUsingRewardComponent.Contains(RewardComponent))
                m_listUsingRewardComponent.Add(RewardComponent);
            m_bNeedUpdateSibling = true;

            UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_GuideWidget_Reward_heroUnlock, _tfAttachParent: RewardComponent.transform, _bAutoDestroy: true);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);
            m_RewardEffect = param;
        }

        internal void HideReward()
        {
            if (RewardComponent == null)
                return;
            RewardComponent.GetRectTransform.SetParent(GetTransform(), true);
            RewardComponent.gameObject.SetActive(false);
            m_listUsingRewardComponent.Remove(RewardComponent);

            if(m_RewardEffect != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_RewardEffect);
                m_RewardEffect = null;
            }
        }

        public void LateUpdate()
        {
            if (m_listUsingComponent != null && m_listUsingComponent.Count > 0 && ModalHelper.IsModalExit())
            {
                for (int i = 0; i < m_listUsingComponent.Count; ++i)
                {
                    {
                        m_listUsingComponent[i].GetTransform.SetParent(UISystem.Instance.transform);
                        m_listUsingComponent[i].GetTransform.SetAsLastSibling();
                    }
                }
                m_bNeedUpdateSibling = false;
            }
            else if (m_listUsingComponent != null && m_listUsingComponent.Count > 0 && m_bNeedUpdateSibling && !ModalHelper.IsModalExit())
            {
                for (int i = 0; i < m_listUsingComponent.Count; ++i)
                {
                    {
                        m_listUsingComponent[i].GetTransform.SetParent(GetTransform());
                        m_listUsingComponent[i].GetTransform.SetAsLastSibling();
                    }
                }
            }

            if(m_listUsingRewardComponent != null && m_listUsingRewardComponent.Count > 0 && ModalHelper.IsModalExit())
            {
                for (int i = 0; i < m_listUsingRewardComponent.Count; ++i)
                {
                    {
                        m_listUsingRewardComponent[i].GetTransform.SetParent(UISystem.Instance.transform);
                        m_listUsingRewardComponent[i].GetTransform.SetAsLastSibling();
                    }
                }
                m_bNeedUpdateSibling = false;
            }
            else if(m_listUsingRewardComponent != null && m_listUsingRewardComponent.Count > 0 && m_bNeedUpdateSibling && !ModalHelper.IsModalExit())
            {
                for (int i = 0; i < m_listUsingRewardComponent.Count; ++i)
                {
                    {
                        m_listUsingRewardComponent[i].GetTransform.SetParent(GetTransform());
                        m_listUsingRewardComponent[i].GetTransform.SetAsLastSibling();
                    }
                }
            }
        }
        
        public override void Update()
        {
            base.Update();

            if(m_AutoMessageTweener != null && m_AutoMessageTweener.IsPlaying() && EntityFactory.MainHeroView != null)
            {
                if (EntityFactory.MainHeroView.StateMachine != null && EntityFactory.MainHeroView.StateMachine.GetState() == GameLogic.EntityState.Dead)
                    HideAutoMessage();
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="_strMessage"></param>
        /// <param name="_fDelayTime">单位:毫秒</param>
        internal void ShowAutoMessage(string _strMessage,int _fDelayTime)
        {
            if (AutoRemoveComponent == null)
                return;

            
            AutoRemoveComponent.GuideTextField.text = _strMessage;
            AutoRemoveComponent.gameObject.SetActive(true);
            m_bNeedUpdateSibling = true;
            // 自动显示的先不做层级提前处理
            //if (!m_listUsingComponent.Contains(AutoRemoveComponent))
            //    m_listUsingComponent.Add(AutoRemoveComponent);


            AutoRemoveComponent.GuideCanvasGroup.alpha = 1;

            float delayTime = _fDelayTime / 1000;

            if(m_AutoMessageTweener == null)
            {
                m_AutoMessageTweener = AutoRemoveComponent.GuideCanvasGroup
                    .DOFade(0, AutoMessageFadeTime)
                    .SetDelay(delayTime)
                    .SetAutoKill(true)
                    .OnKill(() => 
                            {
                                m_AutoMessageTweener = null;
                                AutoRemoveComponent.gameObject.SetActive(false);
                                GuideManager.Instance.SetBBValue(EGuideBBKey.IsAutoGuideMessagePlaying, false);
                               // GuideManager.Instance.SetBBValue(EGuideBBKey.AutoGuideMessageID, -1);
                            });
            }
            else
            {
                m_AutoMessageTweener.SetDelay(delayTime);
                m_AutoMessageTweener.Restart();                
            }            
        }

        internal void HideAutoMessage()
        {
            if (AutoRemoveComponent == null)
                return;
            AutoRemoveComponent.GetRectTransform.SetParent(GetTransform(), true);
            AutoRemoveComponent.gameObject.SetActive(false);
            //m_listUsingComponent.Remove(AutoRemoveComponent);
            AutoRemoveComponent.GuideCanvasGroup.alpha = 1;
            if(m_AutoMessageTweener != null)
            {
                if (m_AutoMessageTweener.IsPlaying())
                    m_AutoMessageTweener.Complete();
                m_AutoMessageTweener = null;
            }
        }

        internal void ShowAlwaysDisplayMessage(string _strMessage)
        {
            if (AlwaysDisplayComponent == null)
                return;

            AlwaysDisplayComponent.GuideTextField.text = _strMessage;
            AlwaysDisplayComponent.gameObject.SetActive(true);
            m_bNeedUpdateSibling = true;
            if (!m_listUsingComponent.Contains(AlwaysDisplayComponent))
                m_listUsingComponent.Add(AlwaysDisplayComponent);
        }
        internal void HideAlwaysDisplayMessage()
        {
            if (AlwaysDisplayComponent == null)
                return;
            AlwaysDisplayComponent.GetRectTransform.SetParent(GetTransform(), true);
            AlwaysDisplayComponent.gameObject.SetActive(false);
            m_listUsingComponent.Remove(AlwaysDisplayComponent);
        }

        internal void ShowModal()
        {
            if (UIWidgets.ModalHelper.IsModalExit())
                return;
            if (m_nModalKey != null)
                HideModal();

            m_nModalKey = UIWidgets.ModalHelper.Open(this, null, ModalColor, null, null);
        }

        internal void HideModal()
        {
            if (m_nModalKey != null)
                UIWidgets.ModalHelper.Close((int)m_nModalKey);
        }

        internal void AddGuideEffect(int _nEffectID)
        {
            UEffectParamBase param = new UEffectPrefabParam((UEffectPrefabType)_nEffectID, GetTransform());
            if(UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param))
            {
                m_dicGuideEffect.Add(param);
            }
        }

        internal void RemoveGuideEffect(int _nEffectID)
        {
            UEffectParamBase tmpParam = null;
            for (int i = 0; i < m_dicGuideEffect.Count; ++i)
            {
                if (m_dicGuideEffect[i] != null && m_dicGuideEffect[i] is UEffectPrefabParam 
                    && (int)(m_dicGuideEffect[i] as UEffectPrefabParam).AttachEffectType == _nEffectID)
                {
                    tmpParam = m_dicGuideEffect[i];
                    if (UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab,ref tmpParam))
                    {
                        m_dicGuideEffect.RemoveAt(i);
                        break;
                    }
                }
            }
        }

        protected void SetUserData(UGuideMsgData.CustomUserData _userData,ref UGuideNodeDisplayComponent _targetComponent)
        {
            UGuideMsgData.CustomUserData tmpUserData = _userData == null ? m_defaultCustomUserData : _userData;

            if (tmpUserData.CustomXPos != null)
            {
                Vector2 newPos = DefaultComponent.GetRectTransform.anchoredPosition;
                newPos.x = (float)tmpUserData.CustomXPos;
                DefaultComponent.GetRectTransform.anchoredPosition = newPos;
            }
            if (tmpUserData.CustomYPos != null)
            {
                Vector2 newPos = DefaultComponent.GetRectTransform.anchoredPosition;
                newPos.y = (float)tmpUserData.CustomYPos;
                DefaultComponent.GetRectTransform.anchoredPosition = newPos;
            }
        }

        /// <summary>
        /// 更新当前主引导对象
        /// </summary>
        /// <param name="_userData"></param>
        protected void SetUserDataDefaultComponent(UGuideMsgData.CustomUserData _userData)
        {
            UGuideMsgData.CustomUserData tmpUserData = _userData == null ? m_defaultCustomUserData : _userData;

            if (tmpUserData.CustomType != null)
            {
                if (tmpUserData.CustomType == 1)
                {
                    if (DefaultComponent != DefaultComponent_Type1)
                    {
                        Hide();
                        DefaultComponent = DefaultComponent_Type1;
                    }
                }
                else
                {
                    if (DefaultComponent != DefaultComponent_Type0)
                    {
                        Hide();
                        DefaultComponent = DefaultComponent_Type0;
                    }
                }
            }
        }
    }

    [Serializable]
    public class UGuideNodeDisplayComponent : UCustomUIBehaviour
    {
        public Text GuideTextField;
        public CanvasGroup GuideCanvasGroup;
        public EasyLayout.EasyLayout Layout;
    }

    [Serializable]
    public class UGuideNodeRewardComponent : UCustomUIBehaviour
    {
        public Text GuideTextField;
    }

    #region 旧版UI View 暂时保留
    //public class UGuideWndView : UIBaseWndView
    //{
    //    [SerializeField]
    //    public EGuideDebugType GuideManagerDebugType = EGuideDebugType.None;


    //    /// <summary>
    //    /// 挂接激活状态的引导
    //    /// </summary>
    //    public RectTransform NodeContainer;
    //    /// <summary>
    //    /// 引导模板预制体
    //    /// </summary>
    //    public UGuideNodeDisplayComponent DefaultComponent;
    //    /// <summary>
    //    /// 激活状态的引导引用列表
    //    /// </summary>
    //    protected List<UGuideNodeDisplayComponent> m_UseDisplayComponentList = new List<UGuideNodeDisplayComponent>();
    //    /// <summary>
    //    /// 非激活状态的引导引用列表
    //    /// </summary>
    //    protected List<UGuideNodeDisplayComponent> m_UnUseDisplayComponentList = new List<UGuideNodeDisplayComponent>();
    //    /// <summary>
    //    /// 准备要释放的引导引用列表
    //    /// </summary>
    //    protected List<UGuideNodeDisplayComponent> m_ReadyToUnUseDisplayComponentList = new List<UGuideNodeDisplayComponent>();

    //    /// <summary>
    //    /// 确认是否准备将要释放的引导引用设为非激活状态
    //    /// </summary>
    //    protected bool m_bConfirmToUnUseDisplayComponent = false;
    //    /// <summary>
    //    /// 挂接非激活状态的引导
    //    /// </summary>
    //    private Transform m_UnuseDisplayComponentContainner;
    //    /// <summary>
    //    /// 引导要显示的文本内容(考虑是否用list存储)
    //    /// </summary>
    //    private UGuideMsgData m_dGuideMsgData = new UGuideMsgData();

    //    /// <summary>
    //    /// 显示一个引导的同时有新引导需要显示，两引导间的等待时间
    //    /// </summary>
    //    public float ChangeNodeMessageDelay = 3.0f;

    //    public override bool Init(IUIWnd wnd)
    //    {
    //        InitNode();

    //        return base.Init(wnd);
    //    }

    //    public override void Update()
    //    {
    //        base.Update();
    //        if (GuideManagerDebugType != GuideManager.GuideDebugType)
    //        {
    //            GuideManager.GuideDebugType = GuideManagerDebugType;
    //        }
    //        //if (InputManager.GetKeyDown(KeyCode.V))
    //        //{
    //        //    count = (count + 1) % test.Length;
    //        //    AddNode(test[count]);
    //        //}
    //        //if (InputManager.GetKeyDown(KeyCode.C))
    //        //    RemoveNode();
    //    }

    //    public override void Destroy()
    //    {
    //        ClearDisplayComponent(m_UnUseDisplayComponentList);
    //        ClearDisplayComponent(m_UseDisplayComponentList);
    //        base.Destroy();
    //    }
    //    #region 临时测试用的数据
    //    public int count;
    //    public UGuideMsgData[] test = new UGuideMsgData[2]
    //        {
    //            new UGuideMsgData("文本测试，我只是想多要几行而已\n 那这里来个换行吧。",EGuideNodeID.GuideNodeID_1,1),
    //            new UGuideMsgData("然而好像并没有什么用。",EGuideNodeID.GuideNodeID_1,1)
    //        };

    //    #endregion

    //    private void ClearDisplayComponent(List<UGuideNodeDisplayComponent> _componentList)
    //    {
    //        _componentList.ForEach(x => DestroyObject(x));
    //        _componentList.Clear();
    //        _componentList = null;
    //    }

    //    private void InitNode()
    //    {
    //        if (NodeContainer == null)
    //        {
    //            Transform tmpNotify = this.GetTransform().FindChild("NodeContainer");
    //            if (tmpNotify == null)
    //            {
    //                GameObject tmpObj = new GameObject("NotifyContainer");
    //                EasyLayout.EasyLayout layout = tmpObj.AddComponent<EasyLayout.EasyLayout>();
    //                layout.GroupPosition = EasyLayout.Anchors.UpperRight;
    //                layout.Stacking = EasyLayout.Stackings.Vertical;
    //                layout.LayoutType = EasyLayout.LayoutTypes.Compact;
    //                layout.RowAlign = EasyLayout.HorizontalAligns.Right;
    //                layout.InnerAlign = EasyLayout.InnerAligns.Top;
    //                layout.RightToLeft = true;

    //                NodeContainer = tmpObj.AddComponent<RectTransform>();
    //                NodeContainer.anchoredPosition = new Vector2(0, -5);
    //                NodeContainer.sizeDelta = new Vector2(100, 0);
    //                NodeContainer.anchorMin = new Vector2(1, 0);
    //                NodeContainer.anchorMax = new Vector2(1, 1);
    //                NodeContainer.pivot = new Vector2(1, 0.5f);
    //                NodeContainer.localPosition = new Vector3(960, -5, 0);
    //            }
    //        }

    //        InitNodeTemplete();
    //    }

    //    private void InitNodeTemplete()
    //    {
    //        if(NodeContainer.childCount <= 0)
    //        {
    //            Trace.LogError("引导UI没有模板组件...");
    //            return;
    //        }
    //        if(DefaultComponent == null)
    //        {
    //            DefaultComponent = NodeContainer.GetChild(0).GetComponent<UGuideNodeDisplayComponent>();
    //            if(DefaultComponent == null)
    //                Trace.LogError("引导UI没有模板组件...");
    //            return;
    //        }

    //        DefaultComponent.ResetGuideNodeSequence();
    //        DefaultComponent.gameObject.SetActive(false);

    //        m_UnuseDisplayComponentContainner = new GameObject("UnuseContainner").AddComponent<RectTransform>();

    //        m_UnuseDisplayComponentContainner.SetParent(this.GetTransform(), false);

    //    }

    //    protected bool GetUnuseDisplayComponent(out UGuideNodeDisplayComponent _nUnuseNode)
    //    {
    //        UGuideNodeDisplayComponent node;
    //        //先看准备无用的列表有没有
    //        if(m_ReadyToUnUseDisplayComponentList.Count > 0)
    //        {
    //            node = m_ReadyToUnUseDisplayComponentList[m_ReadyToUnUseDisplayComponentList.Count - 1];
    //            m_ReadyToUnUseDisplayComponentList.Remove(node);
    //            m_UseDisplayComponentList.Add(node);
    //            _nUnuseNode = node;

    //            return true;
    //        }


    //        //再从缓冲中取
    //        if (m_UnUseDisplayComponentList.Count > 0)
    //        {
    //            node = m_UnUseDisplayComponentList[m_UnUseDisplayComponentList.Count - 1];
    //            m_UnUseDisplayComponentList.Remove(node);
    //            m_UseDisplayComponentList.Add(node);
    //            _nUnuseNode = node;

    //            return true;
    //        }
    //        //没有的话实例化一个出来
    //        if (DefaultComponent == null)
    //        {
    //            Trace.LogError("没有节点预制体，不能实例化物品合成树节点");
    //            _nUnuseNode = null;
    //            return false;
    //        }

    //        {
    //            UGuideNodeDisplayComponent newNode = MonoBehaviour.Instantiate<UGuideNodeDisplayComponent>(DefaultComponent);
    //            newNode.transform.SetParent(m_UnuseDisplayComponentContainner, false);
    //            m_UnUseDisplayComponentList.Add(newNode);
    //            newNode.gameObject.SetActive(false);
    //        }

    //        node = m_UnUseDisplayComponentList[m_UnUseDisplayComponentList.Count - 1];
    //        m_UnUseDisplayComponentList.Remove(node);
    //        m_UseDisplayComponentList.Add(node);
    //        _nUnuseNode = node;

    //        return true;
    //    }


    //    internal void AddNode(UGuideMsgData _msgData)
    //    {
    //        if(m_bConfirmToUnUseDisplayComponent)
    //        {
    //            m_dGuideMsgData.CloneTo(_msgData);
    //            StartCoroutine(AddNode_WaitRemoveNodeEnd());
    //            return;
    //        }

    //        if (m_UseDisplayComponentList.Count > 0)
    //        {
    //            ChangeNodeMessage(_msgData);
    //            return;
    //        }

    //        m_dGuideMsgData.CloneTo(_msgData);


    //        if (m_ReadyToUnUseDisplayComponentList.Count > 0)
    //        {
    //            StopCoroutine(ReadyToUnused());
    //            GetNodeFromReadyToUnUseList();
    //            StartCoroutine(PlayTextAnim());
    //            return;
    //        }

    //        GetNodeFromUnUseList();
    //    }
    //    internal void RemoveNode()
    //    {
    //        if (m_UseDisplayComponentList.Count <= 0)
    //            return;
    //        StopCoroutine(AddNode_WaitRemoveNodeEnd());
    //        StartCoroutine(ReadyToUnused());
    //    }

    //    /// <summary>
    //    /// 从准备释放的引用列表中取可用的引用
    //    /// </summary>
    //    void GetNodeFromReadyToUnUseList()
    //    {
    //        UGuideNodeDisplayComponent node = null;

    //        node = m_ReadyToUnUseDisplayComponentList[0];
    //        node.SetGuideNodeInfo(null);

    //        m_ReadyToUnUseDisplayComponentList.Remove(node);
    //        m_UseDisplayComponentList.Add(node);

    //    }

    //    /// <summary>
    //    /// 从非激活列表中取引导
    //    /// </summary>
    //    void GetNodeFromUnUseList()
    //    {
    //        UGuideNodeDisplayComponent node;
    //        GetUnuseDisplayComponent(out node);
    //        node.transform.SetParent(NodeContainer, false);

    //        node.ResetGuideNodeSequence();
    //        node.SetGuideNodeInfo(m_dGuideMsgData);
    //        node.gameObject.SetActive(true);
    //        node.PlayGuideNodeSequence();
    //    }

    //    /// <summary>
    //    /// 如果要显示下一个引导，但当前引导已经进入确认释放状态，则等待释放完成
    //    /// </summary>
    //    /// <returns></returns>
    //    IEnumerator AddNode_WaitRemoveNodeEnd()
    //    {
    //        while(m_bConfirmToUnUseDisplayComponent)
    //        {
    //            yield return null;
    //        }

    //        GetNodeFromUnUseList();
    //    }        

    //    /// <summary>
    //    /// 接收释放消息时，先将引导存入准备释放列表备用，一定时间没有新引导要显示时才将他转入非激活状态列表
    //    /// </summary>
    //    /// <returns></returns>
    //    IEnumerator ReadyToUnused()
    //    {
    //        UGuideNodeDisplayComponent node = m_UseDisplayComponentList[0];
    //        m_UseDisplayComponentList.Remove(node);
    //        m_ReadyToUnUseDisplayComponentList.Add(node);
    //        node.SetGuideNodeInfo(null);

    //        yield return new WaitForSeconds(1);

    //        m_bConfirmToUnUseDisplayComponent = true;
    //        if (m_ReadyToUnUseDisplayComponentList.Count > 0)
    //        {
    //            node = m_ReadyToUnUseDisplayComponentList[0];
    //            m_ReadyToUnUseDisplayComponentList.Remove(node);
    //            m_UseDisplayComponentList.Add(node);
    //            node.ReplayGuideNodeSequence(ResetNode);
    //        }
    //        //StopCoroutine(PlayAnim());
    //    }

    //    internal void ResetNode()
    //    {
    //        if (m_UseDisplayComponentList.Count <= 0)
    //            return;

    //        UGuideNodeDisplayComponent node;
    //        while (m_UseDisplayComponentList.Count > 0)
    //        {
    //            node = m_UseDisplayComponentList[0];
    //            node.transform.SetParent(m_UnuseDisplayComponentContainner, false);
    //            node.gameObject.SetActive(false);
    //            m_UseDisplayComponentList.Remove(node);
    //            m_UnUseDisplayComponentList.Add(node);
    //        }
    //        m_bConfirmToUnUseDisplayComponent = false;

    //        (m_wnd as UGuideWnd).ResetAnim();
    //    }

    //    /// <summary>
    //    /// 以显示引导有新引导需要显示时直接替换显示内容
    //    /// </summary>
    //    /// <param name="_strNewText"></param>
    //    private void ChangeNodeMessage(UGuideMsgData _msgData)
    //    {
    //        m_dGuideMsgData.CloneTo(_msgData);

    //        StartCoroutine(PlayTextAnim());
    //    }

    //    /// <summary>
    //    /// 延迟替换显示内容
    //    /// </summary>
    //    /// <returns></returns>
    //    IEnumerator PlayTextAnim()
    //    {
    //        yield return new WaitForSeconds(ChangeNodeMessageDelay);
    //        m_UseDisplayComponentList[0].SetGuideNodeInfo(null);
    //        yield return new WaitForSeconds(0.3f);

    //        if (m_UseDisplayComponentList.Count > 0)
    //            m_UseDisplayComponentList[0].PlayGuideNodeTextAnim(m_dGuideMsgData, ResetTextAnim);
    //    }
    //    private void ResetTextAnim()
    //    {
    //        StopCoroutine(PlayTextAnim());
    //    }

    //    internal void ClearNode()
    //    {
    //        foreach(var node in m_UseDisplayComponentList)
    //        {
    //            node.ResetGuideNodeSequence();
    //        }
    //        ResetNode();
    //    }
    //}

    //[Serializable]
    //public class UGuideNodeDisplayComponent : UCustomUIBehaviour
    //{
    //    public Text GuideNodeText = null;
    //    public Image GuideNodeMarkBG = null;
    //    public Image GuideNodeImageBG = null;
    //    public Image GuideNodeSign = null;
    //    public ContentSizeFitter GuideNodeContentSizeFitter = null;
    //    public HorizontalLayoutGroup GuideNodeHorizontalLayoutGroup = null;

    //    protected Sequence GuideNodeSequence = null;


    //    public int CurrentRotateTarget = 0;
    //    public int CurrentScaleTarget = 1;
    //    public float AnimDuration = 1f; 

    //    protected override void Start()
    //    {
    //        base.Start();
    //    }

    //    /// <summary>
    //    /// 高度和图标大小都是这个值，与子节点的LayoutElement一致
    //    /// </summary>
    //    protected const int GuideNodeHeight = 60;
    //    protected const float GuideNodeDefaultWidth = 23.78f;
    //    protected const int GuideNodeImageDefaultAngle = 180;
    //    protected const int GuideNodeImageDefaultScale = 0;

    //    internal void ReplayGuideNodeSequence(Action _onRewind = null)
    //    {
    //        GuideNodeSequence = DOTween.Sequence();
    //        if(GuideNodeSequence != null)
    //        {
    //            GuideNodeSequence.Append(this.GetRectTransform.DOSizeDelta(new Vector2(GuideNodeDefaultWidth, GuideNodeHeight), AnimDuration)
    //                                                          .OnPlay(()=> { GuideNodeImageBG.enabled = true; })
    //                                                          .OnComplete(()=> { GuideNodeImageBG.enabled = false; }))
    //                             .Append(GuideNodeMarkBG.rectTransform.DORotate(new Vector3(0, 0, GuideNodeImageDefaultAngle), AnimDuration))
    //                             .Insert(AnimDuration, GuideNodeMarkBG.rectTransform.DOScale(new Vector3(GuideNodeImageDefaultScale, GuideNodeImageDefaultScale, 1), AnimDuration - 0.5f))
    //                             .SetAutoKill(true)
    //                             .OnPlay(() => { GuideNodeText.enabled = GuideNodeContentSizeFitter.enabled = GuideNodeHorizontalLayoutGroup.enabled = false; })
    //                             .OnComplete(()=> { ResetGuideNodeSequence(); if (_onRewind != null) _onRewind.Invoke(); })
    //                             .OnKill(()=> { GuideNodeSequence = null; })
    //                             .Play();
    //        }
    //        //if(GuideNodeSequence != null)
    //        //{
    //        //    GuideNodeSequence
    //        //        .OnPlay(() =>
    //        //        {
    //        //            GuideNodeText.enabled = GuideNodeContentSizeFitter.enabled = GuideNodeHorizontalLayoutGroup.enabled = false;
    //        //        })
    //        //        .OnRewind(() => { ResetGuideNodeSequence();if (_onRewind != null) _onRewind.Invoke(); })
    //        //        .PlayBackwards();
    //        //}
    //        //else
    //        //{
    //        //    ResetGuideNodeSequence();
    //        //}
    //    }

    //    internal void PlayGuideNodeSequence()
    //    {
    //        GuideNodeSequence = DOTween.Sequence();
    //        GuideNodeSequence.Append(GuideNodeMarkBG.rectTransform.DORotate(new Vector3(0, 0, CurrentRotateTarget), AnimDuration))
    //                         .Insert(0, GuideNodeMarkBG.rectTransform.DOScale(new Vector3(CurrentScaleTarget, CurrentScaleTarget, 1), AnimDuration - 0.5f))
    //                         .Append(this.GetRectTransform.DOSizeDelta(new Vector2(GetTargetWidth(), GuideNodeHeight), AnimDuration+0.5f)
    //                                    .OnPlay(() => { GuideNodeImageBG.enabled = true; })
    //                                    .OnRewind(() => { if (GuideNodeSequence.IsBackwards()) GuideNodeImageBG.enabled = false; }))
    //                         .SetAutoKill(true)
    //                         .OnComplete(() => { GuideNodeText.enabled = GuideNodeContentSizeFitter.enabled = GuideNodeHorizontalLayoutGroup.enabled = true; })
    //                         .OnKill(()=> { GuideNodeSequence = null; })
    //                         .PlayForward();

    //    }

    //    internal void ResetGuideNodeSequence()
    //    {
    //        GuideNodeMarkBG.rectTransform.localRotation = Quaternion.Euler(0, 0, GuideNodeImageDefaultAngle);
    //        GuideNodeMarkBG.rectTransform.localScale = new Vector3(GuideNodeImageDefaultScale, GuideNodeImageDefaultScale, 1);
    //        this.GetRectTransform.sizeDelta = new Vector2(GuideNodeDefaultWidth, GuideNodeHeight);
    //        GuideNodeImageBG.enabled = GuideNodeText.enabled = GuideNodeContentSizeFitter.enabled = GuideNodeHorizontalLayoutGroup.enabled = false;

    //    }

    //    internal void PlayGuideNodeTextAnim(UGuideMsgData _msgData, Action _onComplete = null)
    //    {

    //        SetGuideNodeSignSprite(_msgData);
    //        GuideNodeText.DOText(_msgData.strMsgText, 2)
    //            .SetAutoKill(true)
    //            .OnComplete(()=> { if (_onComplete != null) _onComplete.Invoke(); });
    //    }

    //    internal void SetGuideNodeInfo(UGuideMsgData _msgData)
    //    {
    //        SetGuideNodeText(_msgData);
    //        SetGuideNodeSignSprite(_msgData);
    //    }

    //    internal void SetGuideNodeText(UGuideMsgData _msgData)
    //    {
    //        GuideNodeText.text = _msgData == null ? "" : _msgData.strMsgText;
    //    }

    //    internal void SetGuideNodeSignSprite(UGuideMsgData _msgData)
    //    {
    //        int nID = _msgData == null ? 3 : _msgData.nGuideSignSpriteID == -1 ? 3 : _msgData.nGuideSignSpriteID;
    //        GuideNodeSign.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_GuideSignSprite, WndID.WND_ID_GAMEGUIDE, nID);
    //    }

    //    private float GetTargetWidth()
    //    {
    //        return GuideNodeText.GetComponent<LayoutElement>().preferredWidth;//.rectTransform.sizeDelta.x;
    //        //目前不需要根据字符长度来控制显示长度
    //        return GuideNodeHorizontalLayoutGroup.padding.horizontal+ GuideNodeHorizontalLayoutGroup.spacing + GuideNodeText.cachedTextGenerator.GetPreferredWidth(GuideNodeText.text, GuideNodeText.GetGenerationSettings(Vector2.zero)) + GuideNodeHeight; 
    //    }
    //}
    #endregion
}