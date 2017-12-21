/*******************************************************************
** 文件名:	UPlayerFrame.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016-3-31
** 版  本:	1.0
** 描  述:	角色系统主界面外框
********************************************************************/
/*******************************************************************
** 创建人:	周坤
** 日  期:	2016-10-31
** 版  本:	2.0
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DataCenter;
using UnityEngine;
using USpeedUI.PlayerCrownWndPart;
using UnityEngine.UI;
using USpeedUI.UWidgets;
using SocialSharing;

namespace USpeedUI.PlayerFrame
{
    // 子页面分类
    public enum PlayerSubWndType
    {
        WND_PLAYER,
        WND_WAR,
        WND_HERO,
        WND_RUNE,
        WND_CROWN,
        WND_MOMENT,
    }

    // button组分类
    public enum PlayerFrameBtnType
    {
        BTN_PLAYER,
        BTN_WAR,
        BTN_HERO,
        BTN_RUNE,
        WND_CROWN,
        BTN_MOMENT,
    }

    public class UPlayerFrame : UIPopupWnd<UViewPlayerFrame>
    {
        public List<IUISubWnd> m_SubWndList = new List<IUISubWnd>();
        internal Dictionary<PlayerSubWndType, IUIWnd> m_dicFillWnd = new Dictionary<PlayerSubWndType, IUIWnd>();

        public override WndID GetID()
        {
            return WndID.WND_ID_PLAYERFRAME;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override String GetPrefabPath()
        {
            return "PlayerFrame/PlayerFrame";
        }

        public override WndMutexLevelDef GetMutexLevel()
        {
            return WndMutexLevelDef.WND_Mutex_Normal;
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);
            if (!_bVisible && m_wndView != null)
            {
                //目前只有宝石界面单独了出来，非UIBaseSubWnd,所以特殊处理下
                PlayerSubWndType currentSubWndType = m_wndView.GetCurSubWnd();
                if (currentSubWndType == PlayerSubWndType.WND_CROWN)
                {
                    //IUIWnd subWnd = GetSubUIWnd(currentSubWndType);
                    //subWnd.SetVisible(false);
                }
                else
                {
                    IUISubWnd subWnd = GetSubWnd(currentSubWndType);
                    subWnd.SetVisible(false);
                }


                //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERFRAME_CLOSE, null);
            }

            UCommonPopupWndVisible msgData = new UCommonPopupWndVisible();
            msgData.msgID = (int)WndMsgID.WND_MSG_COMMON_POPUPWND_VISIBLE;
            msgData.nWndID = (int)GetID();
            msgData.isVisible = _bVisible;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_POPUPWND_VISIBLE, msgData);
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            m_bForbid3DSound = true;

            // 添加子窗口
            m_SubWndList.Add(new UPlayerInfo());
            m_SubWndList.Add(new PlayerWarStatisticWnd());
            m_SubWndList.Add(new UHeroPreviewFrame());
            m_SubWndList.Add(new PlayerGloryMomentWnd());
            
            //目前只有宝石界面单独了出来，非UIBaseSubWnd
            m_dicFillWnd.Add(PlayerSubWndType.WND_CROWN, UISystem.Instance.GetWnd(WndID.WND_ID_CROWNPAGE));

            // m_SubWndList.Add(new RunePageWnd());
            //m_SubWndList.Add(new PlayerCrownWnd());
            // 子窗口初始化
            foreach (IUISubWnd wnd in m_SubWndList)
            {
                wnd.Init();
            }
            //目前只有宝石界面单独了出来，非UIBaseSubWnd
            foreach (IUIWnd wnd in m_dicFillWnd.Values)
            {
                wnd.Init();
            }

            // 注册消息事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);

            // PlayerInfoWnd
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERINFO_INITINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_UPDATETOTAL, this);

            // PlayerWarWnd
            //UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_UPDATETOTAL, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_UPDATEWAR, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_VIEWSINGLEWAR, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_VIEWACTORWAR, this);

            // PlayerHeroWnd
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HERODETAIL_UPDATEGOLD, this);

            // PlayerRuneWnd
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RUNEPAGE_INITRUNEDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RUNEPAGE_INITOTHERRUNEDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RUNEPAGE_PUTRUNE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RUNEPAGE_CHANGEPAGE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MAIN_OPEN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERINFO_REANMERESULT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CROWNPAGE_UPDATEVIEW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CROWNPAGE_OTHERUPDATEVIEW, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CROWNPAGE_OTHERPAGESINGLESHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CROWNPAGE_SELFPAGESINGLESHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CROWNPAGE_APPRAISALRET, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CROWNPAGE_ACTIVEGEMMODEL, this);

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_OPEN_SOCIALSHARE_FRAME, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_OPEN_GLORYMOMENT_VIDEO_FRAME, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_OPEN_GLORYMOMENT_WARDATA, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();

            // 注销消息事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this); 
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);

            // PlayerInfoWnd
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERINFO_INITINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_UPDATETOTAL, this);

            // PlayerWarWnd
            //UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_UPDATETOTAL, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_UPDATEWAR, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_VIEWSINGLEWAR, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_VIEWACTORWAR, this);

            // PlayerHeroWnd
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_HERODETAIL_UPDATEGOLD, this);

            // PlayerRuneWnd
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RUNEPAGE_INITRUNEDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RUNEPAGE_INITOTHERRUNEDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RUNEPAGE_PUTRUNE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RUNEPAGE_CHANGEPAGE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MAIN_OPEN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERINFO_REANMERESULT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CROWNPAGE_UPDATEVIEW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CROWNPAGE_OTHERUPDATEVIEW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CROWNPAGE_OTHERPAGESINGLESHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CROWNPAGE_SELFPAGESINGLESHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CROWNPAGE_APPRAISALRET, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CROWNPAGE_ACTIVEGEMMODEL, this);

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_OPEN_SOCIALSHARE_FRAME, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_OPEN_GLORYMOMENT_VIDEO_FRAME, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_OPEN_GLORYMOMENT_WARDATA, this);
            foreach (IUISubWnd wnd in m_SubWndList)
            {
                wnd.Destroy();
            }
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME:
                    {
                        //UISystem.Instance.ShowWnd(WndID.WND_ID_SOCIALCONTACT, false);
                        //UISystem.Instance.ShowWnd(WndID.WND_ID_CHART, false);
                        SetVisible(true);

                        if (m_wndView != null)
                            OnMsgOpenFrame(data);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        LoadView();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        //SetVisible(false);
                        UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_PLAYERINFO_INITINFO:
                    {
                        IUISubWnd playerInfoWnd = GetSubWnd(PlayerSubWndType.WND_PLAYER);
                        if (playerInfoWnd != null)
                            playerInfoWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_PLAYERWARSTATISTIC_UPDATEWAR:
                case WndMsgID.WND_MSG_PLAYERWARSTATISTIC_VIEWSINGLEWAR:
                    {
                        IUISubWnd playerWarWnd = GetSubWnd(PlayerSubWndType.WND_WAR);
                        if (playerWarWnd != null)
                            playerWarWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO:
                case WndMsgID.WND_MSG_HERODETAIL_UPDATEGOLD:
                    {
                        IUISubWnd playerHeroWnd = GetSubWnd(PlayerSubWndType.WND_HERO);
                        if (playerHeroWnd != null)
                            playerHeroWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_RUNEPAGE_INITRUNEDATA:
                case WndMsgID.WND_MSG_RUNEPAGE_INITOTHERRUNEDATA:
                case WndMsgID.WND_MSG_RUNEPAGE_PUTRUNE:
                case WndMsgID.WND_MSG_RUNEPAGE_CHANGEPAGE:
                case WndMsgID.WND_MSG_WAR_MAIN_OPEN:
                    {
                        IUISubWnd playerRuneWnd = GetSubWnd(PlayerSubWndType.WND_RUNE);
                        if (playerRuneWnd != null)
                            playerRuneWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_PLAYERWARSTATISTIC_UPDATETOTAL:
                    {
                        IUISubWnd playerInfoWnd = GetSubWnd(PlayerSubWndType.WND_PLAYER);
                        if (playerInfoWnd != null)
                            playerInfoWnd.OnMessage(msgID, data);

                        IUISubWnd playerWarWnd = GetSubWnd(PlayerSubWndType.WND_WAR);
                        if (playerWarWnd != null)
                            playerWarWnd.OnMessage(msgID, data);
                    }
                    break;
				case WndMsgID.WND_MSG_PLAYERWARSTATISTIC_VIEWACTORWAR:
					{
						IUISubWnd playerWarWnd = GetSubWnd(PlayerSubWndType.WND_WAR);
						if (playerWarWnd != null)
							playerWarWnd.OnMessage(msgID, data);
					}
					break;
                case WndMsgID.WND_MSG_PLAYERINFO_REANMERESULT:
                    {
                        IUISubWnd playerInfoWnd = GetSubWnd(PlayerSubWndType.WND_PLAYER);
                        if (playerInfoWnd != null)
                            playerInfoWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_CROWNPAGE_UPDATEVIEW:
                case WndMsgID.WND_MSG_CROWNPAGE_OTHERUPDATEVIEW:
                case WndMsgID.WND_MSG_CROWNPAGE_OTHERPAGESINGLESHOW:
                case WndMsgID.WND_MSG_CROWNPAGE_SELFPAGESINGLESHOW:
                case WndMsgID.WND_MSG_CROWNPAGE_APPRAISALRET:
                case WndMsgID.WND_MSG_CROWNPAGE_ACTIVEGEMMODEL:
                    {
                        //IUISubWnd crownInfoWnd = GetSubWnd(PlayerSubWndType.WND_CROWN);
                        IUIWnd crownInfoWnd = GetSubUIWnd(PlayerSubWndType.WND_CROWN);
                        if (crownInfoWnd != null)
                            crownInfoWnd.OnMessage(msgID, data);
                    }
                    break;
                case WndMsgID.WND_MSG_OPEN_SOCIALSHARE_FRAME:
                case WndMsgID.WND_MSG_OPEN_GLORYMOMENT_VIDEO_FRAME:
                case WndMsgID.WND_MSG_OPEN_GLORYMOMENT_WARDATA:
                    {
                        if (m_wndView != null && m_wndView.GetCurSubWnd() != PlayerSubWndType.WND_MOMENT)
                        {
                            if (EntityFactory.MainHeroView != null)
                            {
                                UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_MOMENT);
                                ButtonType.bIsRequestSelf = true;
                                ButtonType.nRequestPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                                OnMsgOpenFrame(ButtonType);
                            }
                        }

                        IUISubWnd playerMomentWnd = GetSubWnd(PlayerSubWndType.WND_MOMENT);
                        if (playerMomentWnd != null)
                            playerMomentWnd.OnMessage(msgID, data);
                    }
                    break;
                default:
                    break;
            }
        }

        private void OnMsgOpenFrame(UIMsgData data)
        {
            UIButtonType buttonType = (UIButtonType)data;

            LogicDataCenter.playerSystemDataManager.bRequestSelf = buttonType.bIsRequestSelf;
            LogicDataCenter.playerSystemDataManager.nRequestPDBID = buttonType.nRequestPDBID;
            LogicDataCenter.playerSystemDataManager.bFake = buttonType.bFake;
            // 造假数据
            if (buttonType.bFake)
            {
                LogicDataCenter.playerSystemDataManager.strCurFakeName = buttonType.strFakeName;
            }
            
            m_wndView.SetRuneBtnState();

            if (buttonType.nType == (int)PlayerSubWndType.WND_CROWN)
            {
                m_wndView.OnRuneBtnClick(true);
            }
            else
            {
                m_wndView.SwitchWndPage((PlayerFrameBtnType)buttonType.nType);
            }            
        }

        public IUISubWnd GetSubWnd(PlayerSubWndType wndType)
        {
            IUISubWnd subWnd = null;
            foreach (IUISubWnd wnd in m_SubWndList)
            {
                if (wnd.GetWndType() == (int)wndType)
                {
                    subWnd = wnd;
                    break;
                }
            }

            return subWnd;
        }
        public IUIWnd GetSubUIWnd(PlayerSubWndType _eWndType)
        {
            IUIWnd subWnd = null;

            if (m_dicFillWnd.ContainsKey(_eWndType))
                subWnd = m_dicFillWnd[_eWndType];

            return subWnd;
        }
    }

    public class UViewPlayerFrame : UIBaseWndView
    {
        public Text PlayerInfoTitle;
        public Text PlayerWarTitle;
        public Text PlayerHeroTitle;

        public UButtonGroup PlayerButtonGroup;
        public Transform ContentFrame;
        public GameObject RunBtnObj;
        public GameObject GoodsDispostionBtnObj;
        public GameObject MomentBtnObj;

        private UPlayerFrame m_PlayerFrameWnd;
        private PlayerSubWndType m_CurWndType;
        private PlayerFrameBtnType m_CurBtnType;

        private URedPointTip gloryMomentRedPoint;

        public override bool Init(IUIWnd wnd)
        {
            m_UseImageEffect = true;

            PlayerInfoTitle.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "PlayerInfoTitle");
            PlayerWarTitle.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "PlayerWarTitle");
            PlayerHeroTitle.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "PlayerHeroTitle");

            m_PlayerFrameWnd = wnd as UPlayerFrame;
            InitSubWndContainer(ContentFrame);
            
            #region 2017.6.2 大boss说目前不开放这功能
            if (GoodsDispostionBtnObj != null)
                GoodsDispostionBtnObj.SetActive(false);
            #endregion

            return base.Init(wnd);
        }

        public override void Start()
        {
            base.Start();

            int redPointCount = CaptureScreen.GetInstance().WarImgCount + ScreenREC.GetInstance().WarVideoCount;
            if (null == gloryMomentRedPoint)
            {
                gloryMomentRedPoint = URedPointTip.CreateRedPointObject(MomentBtnObj.GetComponent<RectTransform>(), 100.0f, 20.0f);
                if (gloryMomentRedPoint != null)
                    gloryMomentRedPoint.SetTipCount(redPointCount);
            }
            //m_PlayerFrameWnd.GetSubWnd(PlayerSubWndType.WND_CROWN).SetContainer(UISystem.Instance.GetWndLayerTrans(WndLayerDef.WND_LAYER_FILL));//(this.transform.parent);
        }

        public override void Destroy()
        {
            if (null != gloryMomentRedPoint)
            {
                URedPointTip.DestroyRedPointTip(gloryMomentRedPoint);
            }
        }

        public void OnCloseBtnClick()
        {
            m_PlayerFrameWnd.SetVisible(false);
        }

        public PlayerSubWndType GetCurSubWnd()
        {
            return m_CurWndType;
        }

        public void SetRuneBtnState()
        {
            RunBtnObj.SetActive(!LogicDataCenter.playerSystemDataManager.bRequestSelf);
            //2017.6.2 大boss说目前不开放这功能
            //GoodsDispostionBtnObj.SetActive(LogicDataCenter.playerSystemDataManager.bRequestSelf);
            MomentBtnObj.SetActive(LogicDataCenter.playerSystemDataManager.bRequestSelf);
        }

        // 按钮切页事件
        public void SwitchWndPage(PlayerFrameBtnType btnType)
        {
            m_CurBtnType = btnType;

            PlayerButtonGroup.SetAllItemDeSelected();
            int nIndex = (int)btnType;
            //BTN_RUNE和WND_CROWN没有了
            if(btnType == PlayerFrameBtnType.BTN_MOMENT)
                nIndex = 3;
            PlayerButtonGroup.SetItemSelectedItem(nIndex, true);
        }
        public void OnGoodsDispositionBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            UISystem.Instance.ShowWnd(WndID.WND_ID_GOODSDISPOSITION, true);
            m_PlayerFrameWnd.SetVisible(false);

        }

        public void OnPlayerBtnClick(bool bActive)
        {
            if (!bActive)
                return;
                
            ShowSubWnd(PlayerSubWndType.WND_PLAYER);
        }

        public void OnWarBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            ShowSubWnd(PlayerSubWndType.WND_WAR);
        }

        public void OnHeroBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            ShowSubWnd(PlayerSubWndType.WND_HERO);
        }

        public void OnMomentBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            if (gloryMomentRedPoint != null)
            {
                URedPointTip.DestroyRedPointTip(gloryMomentRedPoint);
                gloryMomentRedPoint = null;
            }

            ShowSubWnd(PlayerSubWndType.WND_MOMENT);
        }

        public void OnRuneBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            LogicDataCenter.crownPageDataManager.RequestOtherPlayerCrownPageData();
            ShowSubUIWnd(PlayerSubWndType.WND_CROWN);  
        }

        private void ShowSubWnd(PlayerSubWndType wndType)
        {
            foreach (IUISubWnd wnd in m_PlayerFrameWnd.m_SubWndList)
            {
                if (wnd.GetWndType() == (int)wndType)
                    wnd.SetVisible(true);
                else
                    wnd.SetVisible(false);
            }
            m_CurWndType = wndType;
        }

        private void ShowSubUIWnd(PlayerSubWndType _eWndType)
        {
            foreach(KeyValuePair<PlayerSubWndType,IUIWnd>keyValue in m_PlayerFrameWnd.m_dicFillWnd)
            {
                if (keyValue.Key == _eWndType)
                    keyValue.Value.SetVisible(true);
                else
                    keyValue.Value.SetVisible(false);
            }
            m_CurWndType = _eWndType;
        }

        private void ShowSubWnd(WndID _eWndID)
        {
            
        }

        private void InitSubWndContainer(Transform container)
        {
            if (m_PlayerFrameWnd == null)
                return;

            foreach (IUISubWnd wnd in m_PlayerFrameWnd.m_SubWndList)
            {
                if (wnd.GetWndType() == (int)PlayerSubWndType.WND_CROWN)
                    continue;
                else
                    wnd.SetContainer(container);
            }
        }
    }
}
