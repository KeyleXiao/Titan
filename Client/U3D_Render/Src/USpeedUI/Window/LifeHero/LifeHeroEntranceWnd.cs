using ASpeedGame.Data.LifeHero;
using DataCenter;
using GameLogic;
using GUIDE;
using Stage_ManagerDef;
using System.Collections.Generic;
using U3D_Render.Common;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.TooltipAndDialog;
using USpeedUI.UEffect;
using USpeedUI.UExtensions;

namespace USpeedUI.LifeHero
{
    public class LifeHeroEntranceWnd : UIBaseWnd<LifeHeroEntranceView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LIFE_HERO_ENTRANCE;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override string GetPrefabPath()
        {
            return "LifeHero/LifeHeroEntrance";
        }

        public override int GetLoadMask()
        {
            return (int)WndLoadMaskDef.WND_LoadMask_RPG;
        }

        public override bool Init()
        {
            base.Init();

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_UPDATENOVICEGUIDEDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECV_ALL_LIFEHERO_INFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LIFEHERO_ENTRANCE_REFRESH_DATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LIFEHERO_RECV_HISTORY_RECORD, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_OPEN_LIFEHERO_CULTURE_HERO_HINT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERINFO_INITINFO, this);
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_UPDATENOVICEGUIDEDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECV_ALL_LIFEHERO_INFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LIFEHERO_ENTRANCE_REFRESH_DATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LIFEHERO_RECV_HISTORY_RECORD, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_OPEN_LIFEHERO_CULTURE_HERO_HINT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERINFO_INITINFO, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                #region 新手引导相关
                case WndMsgID.WND_MSG_LIFEHERO_GUIDE_ACTIONBEGIN:
                    {
                        SetVisible(true);
                        UGuideWidgetMsgData _widgetMsgData = data as UGuideWidgetMsgData;
                        if (_widgetMsgData != null)
                        {
                            m_wndView.GuideActionBegin(_widgetMsgData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_LIFEHERO_GUIDE_ACTIONEND:
                    {
                        UGuideWidgetMsgData _widgetMsgData = data as UGuideWidgetMsgData;
                        if (_widgetMsgData != null && m_wndView != null)
                        {
                            m_wndView.GuideActionEnd(_widgetMsgData);
                        }
                    }
                    break;
                #endregion 
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                case WndMsgID.WND_MSG_UPDATENOVICEGUIDEDATA:
                case WndMsgID.WND_MSG_PLAYERINFO_INITINFO:
                    {
                        if (!m_isVisible)
                        {
                            if (GuideManager.Instance.IsCurrentGuideGreaterEqual(ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUIDE_TWOSTEP)
                                || (LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo().playerInfo.nLv > 3))
                            {
                                SetVisible(true);
                                if (LogicDataCenter.lifeHeroDataManager.IsFromWarExit)
                                {
                                    OpenLifeHeroDialog();
                                }

                                UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERINFO_INITINFO, this);
                            }
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        UnloadView();
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG_HIDE, new UIDialogHide(EDialogType.LifeHeroEntrance));
                    }
                    break;
                case WndMsgID.WND_MSG_RECV_ALL_LIFEHERO_INFO:
                case WndMsgID.WND_MSG_LIFEHERO_ENTRANCE_REFRESH_DATA:
                case WndMsgID.WND_MSG_LIFEHERO_RECV_HISTORY_RECORD:
                case WndMsgID.WND_MSG_OPEN_LIFEHERO_CULTURE_HERO_HINT:
                    {
                        if (m_wndView != null)
                            m_wndView.RefreshView();
                    }
                    break;
                default:
                    break;
            }
        }

        /* 在刚进入主城时判断是否需要本命弹窗
         * 本命弹窗逻辑：
         * 1、从战场结算界面返回主城
         * 2、还没有收集完本命英雄
         * 3、本命记录里没有该英雄
         * 4、比赛必须满员
         * 5、比赛类型必须满足配置要求的比赛类型(lifehero.csv)
         */
        private void OpenLifeHeroDialog()
        {
            if (!LogicDataCenter.lifeHeroDataManager.IsFromWarExit)
                return;

            LogicDataCenter.lifeHeroDataManager.IsFromWarExit = false;
            if (StageManager.Instance.getCurrentState() != EStage_State.ESS_GAME && StageManager.Instance.getCurrentState() != EStage_State.ESS_LOADSCENE)
            {
                Debug.Log("recv all lifehero at gamestate=" + StageManager.Instance.getCurrentState() + ", so dont show dialog.");
                return;
            }

            if (!LogicDataCenter.lifeHeroDataManager.HasCollectedAllLifeHero()
                && LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count >= 1)
            {
                List<cmd_warstatistic_history_record> recordList = LogicDataCenter.lifeHeroDataManager.HistoryRecord;
                if (recordList == null || recordList.Count < 1)
                {
                    Debug.LogError("lifehero entance: history record is empty.");
                    return;
                }

                cmd_warstatistic_history_record record = recordList[0];
                if (!record.bIsFull)
                    return;

                Dictionary<int, cmd_entity_lifehero_info> ownedLifeHero = LogicDataCenter.lifeHeroDataManager.AllLifeHero;
                if (ownedLifeHero.ContainsKey(record.nHeroID))
                    return;

                if (!LifeHeroConfig.Instance.IsMeetWarType(ownedLifeHero.Count, record.wWarType))
                    return;

                string content;
                DialogActions buttons;
                if (LifeHeroRules.IsMeetRule(LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count, record))
                {
                    content = ULocalizationService.Instance.Get("UIView", "LifeHeroSelect", "TravelContent_Good");
                    buttons = new UIWidgets.DialogActions() { { ULocalizationService.Instance.Get("UIView", "LifeHeroSelect", "Confirm"), () => { UISystem.Instance.ShowWnd(WndID.WND_ID_LIFE_HERO_CULTURE, true); return true; } }, { ULocalizationService.Instance.Get("UIView", "LifeHeroSelect", "Cancel"), null } };
                }
                else
                {
                    SSchemeLifeHero config;
                    int key = LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count;
                    if (LifeHeroConfig.Instance.LifeHeroConfigDic.TryGetValue(key, out config))
                    {
                        content = string.Format(ULocalizationService.Instance.Get("UIView", "LifeHeroSelect", "TravelContent_NontDiscourage"), config.rule);
                        buttons = new UIWidgets.DialogActions() { { ULocalizationService.Instance.Get("UIView", "LifeHeroSelect", "Confirm"), () => { UISystem.Instance.ShowWnd(WndID.WND_ID_WAR_MAIN_FRAME, true); ; return true; } }, { ULocalizationService.Instance.Get("UIView", "LifeHeroSelect", "Cancel"), null } };
                    }
                    else
                    {
                        Debug.LogError("获取本命配置失败.key = " + key);
                        return;
                    }
                }

                UBB.toHtml(ref content);
                TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                            title: ULocalizationService.Instance.Get("UIView", "LifeHeroSelect", "TravelTitle"),
                            message: content,
                            buttons: buttons,
                            modal: false,
                            hideByModalClick: false,
                            modalColor: Color.clear,
                            mutex: true,
                            eDialogType: EDialogType.LifeHeroEntrance
                            );
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
            }
        }
    }

    public class LifeHeroEntranceView : UIBaseWndView
    {
        public Text processText;
        public Image processIcon;
        public Transform rotateIcon;
        private UEffectParamBase[] m_effectParamArray = new UEffectParamBase[2];
        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            GetComponent<Button>().onClick.AddListener(OnBackgroundClick);

            rotateIcon.gameObject.SetActive(false);

            if (!LoadEffect())
                return false;

            return true;
        }

        public override void Destroy()
        {
            for (int i = 0; i < m_effectParamArray.Length; ++i)
            {
                if (m_effectParamArray[i] != null)
                {
                    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_effectParamArray[i]);
                    m_effectParamArray[i] = null;
                }
            }
        }

        internal override void SetRendererVisible(bool _bVisible)
        {
            base.SetRendererVisible(_bVisible);

            if (_bVisible)
                RefreshView();
        }

        public void RefreshView()
        {
            if (!IsVisible())
                return;

            processText.text = string.Format("{0}/{1}", LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count, (int)ELifeHeroType.LIFEHERO_MAX_COUNT);
            processIcon.fillAmount = (float)LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count / (float)ELifeHeroType.LIFEHERO_MAX_COUNT;

            bool isShowEffect = IsShowEffect();
            (m_effectParamArray[0] as UEffectPrefabParam).EffectGameObject.SetActive(isShowEffect);
            (m_effectParamArray[1] as UEffectPrefabParam).EffectGameObject.SetActive(!isShowEffect);
        }

        private bool IsShowEffect()
        {
            return LogicDataCenter.lifeHeroDataManager.CanObtainPrize() ? true : LogicDataCenter.lifeHeroDataManager.CanAddLifeHero();
        }

        private bool HasPrize()
        {
            int taskID = LogicDataCenter.lifeHeroDataManager.PrizeTaskData;
            if (taskID < 0 || taskID >= LifeHeroConfig.Instance.LifeHeroPrizeList.Count)
            {
                return false;
            }

            int meetHeroCount = 0;
            foreach (int heroID in LogicDataCenter.lifeHeroDataManager.AllLifeHero.Keys)
            {
                ActorHeroDetailInfo heroInfo = LogicDataCenter.playerSystemDataManager.GetActorHeroDetail(heroID, eType: HeroDetailReqType.HDRT_LIFEHERO);
                if (heroInfo != null && heroInfo.heroDetailInfo.nHeroID > 0)
                {
                    if (heroInfo.heroDetailInfo.nStar >= LifeHeroConfig.Instance.LifeHeroPrizeList[taskID].starLv)
                    {
                        ++meetHeroCount;
                    }
                }
                else
                {
                    Debug.Log("英雄详情信息还未收到？");
                }
            }

            return meetHeroCount >= LifeHeroConfig.Instance.LifeHeroPrizeList[taskID].heroCount;
        }

        private bool LoadEffect()
        {
            m_effectParamArray[0] = new UEffectPrefabParam(
                _eType: UEffectPrefabType.UEPT_LifeHero_Entrance_01,
                _tfAttachParent: this.transform, _bAutoDestroy: true);
            if (!UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_effectParamArray[0]))
                return false;

            m_effectParamArray[1] = new UEffectPrefabParam(
                _eType: UEffectPrefabType.UEPT_LifeHero_Entrance_02,
                _tfAttachParent: this.transform, _bAutoDestroy: true);
            if (!UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_effectParamArray[1]))
                return false;

            (m_effectParamArray[0] as UEffectPrefabParam).EffectGameObject.SetActive(false);
            (m_effectParamArray[1] as UEffectPrefabParam).EffectGameObject.SetActive(false);

            return true;
        }

        public void OnBackgroundClick()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG_HIDE, new UIDialogHide(EDialogType.LifeHeroEntrance));
            UISystem.Instance.ShowWnd(WndID.WND_ID_LIFE_HERO_CULTURE, true);
        }
		
		public void GuideActionBegin(UGuideWidgetMsgData _msgData)
        {
            Guide.UGuideWidget guideWidget = gameObject.AddComponent<Guide.UGuideWidget>();
            if (guideWidget != null)
            {
                guideWidget.SetFromMsgData(_msgData);
            }
        }

        public void GuideActionEnd(UGuideWidgetMsgData _msgData)
        {
            Guide.UGuideWidget guideWidget = gameObject.GetComponent<Guide.UGuideWidget>();
            if (guideWidget != null)
            {
                gameObject.RemoveComponent<Guide.UGuideWidget>();
            }
        }
    }
}
