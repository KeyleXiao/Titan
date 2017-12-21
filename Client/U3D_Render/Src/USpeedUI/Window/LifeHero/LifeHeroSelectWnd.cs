using ASpeedGame.Data.LifeHero;
using Data.PersonModel;
using DataCenter;
using GameLogic;
using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UExtensions;
using USpeedUI.UWidgets;

namespace USpeedUI.LifeHero
{
    public enum ESelectViewState
    {
        INVALID,
        INJECT,
        REPLACE,
    }

    public class LifeHeroSelectWnd : UIPopupWnd<LifeHeroSelectView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LIFE_HERO_SELECT;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        public override string GetPrefabPath()
        {
            return "LifeHero/LifeHeroSelect";
        }

        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        UnloadView();
                    }
                    break;

                case WndMsgID.WND_MSG_OPEN_LIFEHERO_SELECT:
                    {
                        if (data == null)
                            return;

                        ULifeHeroCandidateData msgData = data as ULifeHeroCandidateData;
                        if (msgData.candidates == null || msgData.candidates.Count == 0)
                            return;

                        SetVisible(true);
                        if (m_wndView != null)
                        {
                            m_wndView.SetData(msgData);
                        }
                    }
                    break;
                #region 新手引导相关
                case WndMsgID.WND_MSG_LIFEHERO_SELECT_GUIDE_ACTIONBEGIN:
                    {
                        UGuideWidgetMsgData _widgetMsgData = data as UGuideWidgetMsgData;

                        if (m_wndView != null && _widgetMsgData != null)
                        {
                            m_wndView.GuideActionBegin(_widgetMsgData);
                        }

                    }
                    break;
                case WndMsgID.WND_MSG_LIFEHERO_SELECT_GUIDE_ACTIONEND:
                    {
                        UGuideWidgetMsgData _widgetMsgData = data as UGuideWidgetMsgData;

                        if (m_wndView != null && _widgetMsgData != null)
                        {
                            m_wndView.GuideActionEnd(_widgetMsgData);
                        }
                    }
                    break;
                #endregion

                default:
                    break;
            }
        }
    }

    public class LifeHeroSelectView : UIBaseWndView
    {
        public ULifeHeroTileView HeroList;
        public Button sureBtn;
        public Button cancelBtn;
        public Text sureBtnDesc;
        public Text cancelBtnDesc;
        public Text InjectionRequirementDesc;
        public ConsumeFrame consumeFrame;
        private int m_cultureID;
        private ESelectViewState m_currentState;
        private int m_heroID;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            sureBtn.onClick.AddListener(OnSureBtnClicked);
            cancelBtn.onClick.AddListener(OnCancelBtnClicked);

            sureBtnDesc.text = ULocalizationService.Instance.Get("UIView", "LifeHeroSelect", "InjectBtn");
            cancelBtnDesc.text = ULocalizationService.Instance.Get("UIView", "LifeHeroSelect", "ThinkAgainBtn");

            return true;
        }

        private void SwitchState(ESelectViewState state)
        {
            if (state == m_currentState)
                return;

            EnterState(state);
            m_currentState = state;
        }

        private void EnterState(ESelectViewState state)
        {
            if (m_currentState == state)
                return;

            switch (state)
            {
                case ESelectViewState.INJECT:
                    {
                        consumeFrame.gameObject.SetActive(false);
                        RectTransform rf = this.gameObject.GetComponent<RectTransform>();
                            rf.sizeDelta = new Vector2(670, rf.sizeDelta.y);
                    }
                    break;
                case ESelectViewState.REPLACE:
                    {
                        int lifeHeroCount = LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count;
                        if (lifeHeroCount != (int)ELifeHeroType.LIFEHERO_MAX_COUNT)
                        {
                            Debug.LogFormat("本命英雄未收集满，不能替换");
                            return;
                        }
                        consumeFrame.gameObject.SetActive(true);
                        RectTransform rf = this.gameObject.GetComponent<RectTransform>();
                        rf.sizeDelta = new Vector2(832, rf.sizeDelta.y);
                        consumeFrame.SetData(-1);
                    }
                    break;
                default:
                    break;
            }
        }

        internal override void SetRendererVisible(bool _bVisible)
        {
            if (!_bVisible)
            {
                HeroList.SetAllDeselect();
            }

            base.SetRendererVisible(_bVisible);
        }

        public void SetData(ULifeHeroCandidateData msgData)
        {
            SwitchState(msgData.state);
            m_heroID = msgData.heroID;
            m_cultureID = msgData.cultureID;
            ShowHeadPortrait(msgData.candidates);
            SetInjectRequirementDesc();
        }

        private void ShowHeadPortrait(List<cmd_entity_hero_info> entityHeros)
        {
            if (entityHeros == null)
                return;

            HeroList.DataSource.Clear();
            UIWidgets.ObservableList<ULifeHeroItem> dataSource = new UIWidgets.ObservableList<ULifeHeroItem>();

            // 遍历已拥有的英雄加到表中
            foreach (cmd_entity_hero_info cmdInfo in entityHeros)
            {
                int nHeroID = cmdInfo.nHeroID;

                if (cmdInfo.nNeedCardCount == 0)
                    continue;

                SSchemePersonModelData sPersonModel = null;
                if (!PersonModelConfig.Instance.GetData_Out(nHeroID, out sPersonModel))
                    continue;

                ULifeHeroItem item = new ULifeHeroItem();
                item.nHeroID = nHeroID;
                item.headIcon = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 9, nHeroID);
                if (item.headIcon == null)
                    item.headIcon = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 9, 10);
                item.stName = cmdInfo.chHeroName;
                item.stTitle = sPersonModel.szHeroTitle;
                item.nStarLv = cmdInfo.byStarLv;
                item.bHaveHero = (cmdInfo.bIsBuy > 0);
                item.bWeekFree = (cmdInfo.bIsWeekFree > 0);

                int nStarLvLimit = GameLogicAPI.getHeroStarLvLimit(nHeroID);
                item.nStarLvLimit = nStarLvLimit;
                item.fExpValue = (float)cmdInfo.nCardCount / (float)cmdInfo.nNeedCardCount;
                item.stStarExp = cmdInfo.nCardCount.ToString() + (cmdInfo.byStarLv >= nStarLvLimit ? "" : "/" + cmdInfo.nNeedCardCount.ToString());
                int nGold = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_GOLD);
                int nNeedGold = GameLogicAPI.getHeroUpgradeNeedGold(nHeroID, cmdInfo.byStarLv);
                //已购买、未满级、卡够、钱够
                item.bUpGrade = (cmdInfo.bIsBuy > 0 && cmdInfo.byStarLv < nStarLvLimit && item.fExpValue >= 1 && nGold >= nNeedGold);

                item.nUnlockLevel = GameLogicAPI.getUnlockLevel(nHeroID);
                item.bFpsHero = GameLogicAPI.checkIsFpsHero(nHeroID);

                dataSource.Add(item);
            }

            HeroList.DataSource = dataSource;

            if (HeroList.DataSource.Count == 1)
            {
                HeroList.Select(0);
            }

            //增加监听点击头像的事件
            HeroList.OnSelect.RemoveAllListeners();
            HeroList.OnSelect.AddListener(ItemSelected);
        }

        private void SetInjectRequirementDesc()
        {
            int key;
            int heroCount = LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count;
            if (m_currentState == ESelectViewState.INJECT)
            {
                key = (heroCount == (int)ELifeHeroType.LIFEHERO_MAX_COUNT) ? (heroCount - 1) : heroCount;
            }
            else if (m_currentState == ESelectViewState.REPLACE)
            {
                key = heroCount - 1;
            }
            else
            {
                Debug.LogError("LifeHeroSelct : not support state!");
                return;
            }

            key = key < 0 ? 0 : key;
            SSchemeLifeHero config;
            if (LifeHeroConfig.Instance.LifeHeroConfigDic.TryGetValue(key, out config))
            {
                string keyFilter = m_currentState == ESelectViewState.INJECT ? "Bottom_Injection_Hint" : "Bottom_Replace_Hint";
                string content = String.Format(ULocalizationService.Instance.Get("UIView", "LifeHeroSelect", keyFilter), config.rule);
                U3D_Render.Common.UBB.toHtml(ref content);
                InjectionRequirementDesc.text = content;
            }
            else
            {
                Debug.LogError("获取本命配置失败.key = " + key);
            }
        }

        // 当玩家点击头像时
        private void ItemSelected(int index, UIWidgets.ListViewItem component)
        {
            if (m_currentState != ESelectViewState.REPLACE)
                return;

            int key = LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count - 1;
            SSchemeLifeHero lifeHeroScheme;
            if (LifeHeroConfig.Instance.LifeHeroConfigDic.TryGetValue(key, out lifeHeroScheme))
            {
                consumeFrame.SetData(HeroList.DataSource[index].nHeroID);
            }
            else
            {
                Debug.LogErrorFormat("Read LifeHeroConfig.csv failed.key={0}", key);
            }
        }

        private void OnSureBtnClicked()
        {
            if (HeroList.SelectedIndex >= 0 && HeroList.SelectedIndex < HeroList.DataSource.Count)
            {
                cmd_entity_lifehero_info cmd;
                cmd.nHeroID = HeroList.DataSource[HeroList.SelectedIndex].nHeroID;
                cmd.nCultureID = m_cultureID;

                if (m_currentState == ESelectViewState.INJECT)
                {
                    LogicDataCenter.lifeHeroDataManager.RequestAddLifeHero(cmd);
                }
                else
                {
                    int key = LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count - 1;
                    SSchemeLifeHero cfg;
                    if (LifeHeroConfig.Instance.LifeHeroConfigDic.TryGetValue(key, out cfg))
                    {
                        if (GameLogicAPI.getCardCount(cmd.nHeroID) < cfg.cardCount)
                        {
                            UConsumptionMsgData cmdData = new UConsumptionMsgData((int)EConsumptionRequestType.Request_Upstar, (int)EConsumptionGuideType.Guide_HeroCard);
                            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CONSUMPTIONGUIDE_SHOW, cmdData);
                            return;
                        }

                        if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_GOLD) < cfg.goldCount)
                        {
                            UConsumptionMsgData cmdData = new UConsumptionMsgData((int)EConsumptionRequestType.Request_Upstar, (int)EConsumptionGuideType.Guide_Gold);
                            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CONSUMPTIONGUIDE_SHOW, cmdData);
                            return;
                        }

                        LogicDataCenter.lifeHeroDataManager.RequestReplaceLifeHero(m_heroID, cmd);
                    }
                    else
                    {
                        Debug.LogErrorFormat("Read LifeHeroConfig.csv failed.key={0}", key);
                    }
                }
                
                CloseView();
            }
            else
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LIFEHERO_CHOICE_ONE);
            }
        }

        public void OnCancelBtnClicked()
        {
            CloseView();
        }

        private void CloseView()
        {
            m_wnd.SetVisible(false);
        }


        #region 新手引导相关
        public void GuideActionBegin(UGuideWidgetMsgData _msgData)
        {
            if (_msgData.nGuideStepID == 1)
            {
                HeroList.DataSource.BeginUpdate();

                if (HeroList.DataSource.Count == 0)
                    Trace.LogWarning("没有可用的本命英雄？");

                {
                    HeroList.DataSource[0].bUseGuideWidget = true;
                    HeroList.DataSource[0].GuideWidgetData = new UGuideWidgetMsgData(_msgData);
                }
                HeroList.DataSource.EndUpdate();
                HeroList.UpdateItems();
            }
            else if(_msgData.nGuideStepID == 2 && sureBtn != null)
            {
                var widget = sureBtn.gameObject.AddComponent<Guide.UGuideWidget>();
                widget.SetFromMsgData(_msgData);
            }

        }
        public void GuideActionEnd(UGuideWidgetMsgData _msgData)
        {
            if (_msgData.nGuideStepID == 1)
            {
                if (HeroList.DataSource.Count == 0)
                    Trace.LogWarning("没有可用的本命英雄？");

                HeroList.DataSource[0].bUseGuideWidget = false;
                HeroList.DataSource[0].GuideWidgetData = new UGuideWidgetMsgData(_msgData);
            }
            else if(_msgData.nGuideStepID == 2 && sureBtn != null)
            {
                sureBtn.gameObject.RemoveComponent<Guide.UGuideWidget>();
            }
        }
        #endregion
    }

    public class ConsumeFrame : MonoBehaviour
    {
        public Text goldHint;
        public Text consumeAndOwnGold;
        public Text cardHint;
        public Text consumeAndOwnCard;

        public void Start()
        {
            goldHint.text = ULocalizationService.Instance.Get("UIView", "Common", "Gold");
            cardHint.text = ULocalizationService.Instance.Get("UIView", "Common", "Card");
        }

        public void SetData(int heroID)
        {
            if (LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count != (int)ELifeHeroType.LIFEHERO_MAX_COUNT)
            {
                Debug.LogError("本命英雄未满不允许替换");
                return;
            }

            int key = LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count - 1;
            SSchemeLifeHero lifeHeroScheme;
            if (!LifeHeroConfig.Instance.LifeHeroConfigDic.TryGetValue(key, out lifeHeroScheme))
            {
                Debug.LogError("获取本命配置失败.key = " + key);
                return;
            }

            int consummeGold = lifeHeroScheme.goldCount;
            int oweGold = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_GOLD);
            int consumeCard = lifeHeroScheme.cardCount;
            int oweCard = GameLogicAPI.getCardCount(heroID);
            if (oweCard < 0)
            {
                Debug.LogWarningFormat("获取卡牌数量失败,heroID={0}", heroID);
                oweCard = 0;
            }
            consumeAndOwnGold.text = String.Format("{0}({1}{2})", consummeGold, ULocalizationService.Instance.Get("UIView", "Common", "CurrentHave"), oweGold);
            consumeAndOwnCard.text = String.Format("{0}({1}{2})", consumeCard, ULocalizationService.Instance.Get("UIView", "Common", "CurrentHave"), oweCard);
            // 颜色
            if (oweGold < consummeGold)
            {
                goldHint.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
                consumeAndOwnGold.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
            }
            else
            {
                goldHint.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
                consumeAndOwnGold.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
            }

            if (oweCard < consumeCard)
            {
                cardHint.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
                consumeAndOwnCard.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
            }
            else
            {
                cardHint.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
                consumeAndOwnCard.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
            }
        }
    }
}
