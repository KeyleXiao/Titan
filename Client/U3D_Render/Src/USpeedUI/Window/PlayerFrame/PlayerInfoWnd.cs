using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using USpeedUI.UWidgets;
using System.Runtime.InteropServices;
using UnityEngine.EventSystems;
using DataCenter;
using ASpeedGame.Data.PrizeUnlockConfig;
using USpeedUI.UExtensions;
using Data.ActorPrizeConfig;
using UIWidgets;
using USpeedUI.UEffect;
using USpeedUI.PointShop;
using ASpeedGame.Data.ShieldWorld;
using Data.PointShopConfig;
using U3D_Render.Common;

namespace USpeedUI.PlayerFrame
{
    public class UPlayerInfo : UIBaseSubWnd<UViewPlayerInfo>
    {
        public override int GetWndType()
        {
            return (int)PlayerSubWndType.WND_PLAYER;
        }

        public override String GetPrefabPath()
        {
            return "PlayerInfo/PlayerBaseInfo";
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if (m_wndView == null)
                return;

            if (visible)
            {
                int nActorID = LogicDataCenter.playerSystemDataManager.nRequestPDBID;
                ActorPlayerInfo info = LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo(nActorID);
                if (info.playerInfo.pDbid != 0)
                {
                    OnRecvPlayerInfo();
                }
            }
            else
            {
                m_wndView.closeChildrenView();
                UISystem.Instance.ShowWnd(WndID.WND_ID_PLAYER_HONORPAGE, false);
            }
        }

        public override bool Init()
        {
            SetVisible(false);
            return base.Init();
    
        }

        public override void Destroy()
        {
            base.Destroy();
            return;
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_PLAYERINFO_INITINFO:
                    {
                        if (m_wndView != null)
                            OnRecvPlayerInfo();
                    }
                    break;
                case WndMsgID.WND_MSG_PLAYERWARSTATISTIC_UPDATETOTAL:
                    {
                        if (m_wndView != null)
                            m_wndView.updatePlayerKillRecord();
                    }
                    break;
                case WndMsgID.WND_MSG_PLAYERINFO_REANMERESULT:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.onRenameResultMessage((msgData as UIMsgCmdData).nParam);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        private void OnRecvPlayerInfo()
        {
            int nActorID = LogicDataCenter.playerSystemDataManager.nRequestPDBID;
            ActorPlayerInfo info = LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo(nActorID);
            if (info == null)
            {
                Debug.LogError("Actorinfo is invalid, ActorId = " + nActorID);
                return;
            }

            m_wndView.showPlayerInfo(info.playerInfo);
            m_wndView.showOftenUseHeroInfo(info.strongHeroInfoList);
        }
    }

    public class HeroInfo : UCustomUIBehaviour
    {
        public Text heroStarLvDesc;
        public Text heroWinProDesc;
        public Text matchCountDesc;

        public Image heroIcon;
        public Text heroName;
        public Text heroStarLv;
        public Text heroWinPro;
        public Text matchCount;
        public Slider heroUseTimeSlider;
        public RectTransform heroStarFrame;

        private bool bIsReqSelf;
        private int nReqPDBID;
        private int nHeroID;
        private HeroStarItem heroStarItem;

        protected override void Awake()
        {
            base.Awake();

            heroStarLvDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "heroStarLvDesc");
            heroWinProDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "heroWinProDesc");
            matchCountDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "matchCountDesc");
            EventTriggerEx.Get(this.gameObject).AddListener(EventTriggerType.PointerClick, OnPointerClick);
            if (heroStarItem == null)
            {
                heroStarItem = LogicDataCenter.pointShopDataManager.CreateHeroStarItem(heroStarFrame);
            }
        }
   
        public void setRequestInfo(bool _bIsReqSelf, int _nReqPDBID)
        {
            bIsReqSelf = _bIsReqSelf;
            nReqPDBID = _nReqPDBID;
        }

        public void setHeroInfo(cmd_entity_hero_info cmdInfo, int nMaxExp)
        {
            nHeroID = cmdInfo.nHeroID;
            // 此处由于可能找不到对应的头像，策划特别提出随便加一个先
            heroIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 6, cmdInfo.nHeroID);            
            if (heroIcon.sprite == null)
                heroIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 6, 10);

            heroName.text = cmdInfo.chHeroName;
            heroStarLv.text = cmdInfo.byStarLv.ToString();
            heroWinPro.text = cmdInfo.usWinPro.ToString() + "%";
            matchCount.text = cmdInfo.nGameCount.ToString();
            heroUseTimeSlider.maxValue = nMaxExp;
            heroUseTimeSlider.value = cmdInfo.nGameCount;

            int nStarLvLimit = GameLogicAPI.getHeroStarLvLimit(nHeroID);
            int nHeroStar = cmdInfo.byStarLv;
            nHeroStar = nHeroStar > nStarLvLimit ? nStarLvLimit : nHeroStar;
            heroStarItem.SetData(nHeroID, nHeroStar, false, false);

            setVisible(true);
        }

        public void setVisible(bool bIsShow)
        {
            this.gameObject.SetActive(bIsShow);
        }

        public void OnPointerClick(BaseEventData eventData, EventTriggerEx.TriggerParam param)
        {
            ActorHeroDetailInfo info = LogicDataCenter.playerSystemDataManager.GetActorHeroDetail(nHeroID, nReqPDBID);
            if (info.heroDetailInfo.nHeroID != 0)
            {
                cmd_Return_Hero_Detail_Info returnHeroInfo;
                returnHeroInfo.nPDBID = nReqPDBID;
                returnHeroInfo.nHeroID = nHeroID;

                UIMsgCmdData uiReturnHeroInfo = UIMsgCmdDataHelper.Instance.create<cmd_Return_Hero_Detail_Info>(WndMsgID.WND_MSG_HERODETAIL_HERODETAILINFO, ref returnHeroInfo);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HERODETAIL_HERODETAILINFO, uiReturnHeroInfo);
            }
        }
    }

    public class PlayerInfo : UCustomUIBehaviour
    {
        public Text gloryTitle;
        public Text heroTitle;
        public Text kinDesc;
        public Text clanDesc;
        public Text levelDesc;
        public Text kdaDesc;
        public Text killDesc;
        public Text warWinProDesc;
        public Text pkWinProDesc;
        public Text totalMatchNumDesc;
        public Text winMatchNumDesc;
        public Text winMatchProDesc;
        public Text mvpDesc;
        public Text godLikeDesc;
        public Text fiveKillDesc;
        public Text fourKillDesc;
        public Text threeKillDesc;

        public Image playerHead;
        public Text playerName;
        public Text kinName;
        public Text clanName;
        public Text playerLvNow;
        public Text playerLvNext;
        public Slider playerExp;
        public UTooltipTrigger expTip;
        public Text kda;
        public Text kill;
        public Text warWinPro;
        public Text pkWinPro;

        public GameObject RankFrame;
        public Button RankBtn;
        public Image rankImage;
        public Text rankName;
        public Text rankScore;
        public UTooltipTrigger warDataTip;
        public Transform RankStarFrame;
        public Text serverOrder;

        public GameObject InvalidRankFrame;
        public Button InvalidRankBtn;
        public Text InvalidRankDes;
        public Text InvalidDetailDes;

        public GameObject EditNameGo;

        public Text totalMatchNum;
        public Text winMatchNum;
        public Text winMatchPro;
        public Text mvp;
        public Text mvpTip;
        public Text godLike;
        public Text godLikeTip;
        public Text fiveKill;
        public Text fourKill;
        public Text threeKill;

        private UEffectParamBase param;

        protected override void Awake()
        {
            base.Awake();

            gloryTitle.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "gloryTitle");
            heroTitle.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "heroTitle");
            kinDesc.text = ULocalizationService.Instance.Get("UIview", "Common", "Team") + ":";
            clanDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "clanDesc");
            levelDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "levelDesc");
            kdaDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "kdaDesc");
            killDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "killDesc");
            warWinProDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "warWinProDesc");
            pkWinProDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "pkWinProDesc");

            totalMatchNumDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "totalMatchNumDesc");
            winMatchNumDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "winMatchNumDesc");
            winMatchProDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "winMatchProDesc");
            mvpDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "mvpDesc");
            godLikeDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "godLikeDesc");
            fiveKillDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "fiveKillDesc");
            fourKillDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "fourKillDesc");
            threeKillDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "threeKillDesc");


            string strTip = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "InvalidRankDes");
            UBB.toHtml(ref strTip);
            InvalidRankDes.text = strTip;

            string strDetail = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "InvalidDetailDes");
            UBB.toHtml(ref strDetail);
            InvalidDetailDes.text = strDetail;

            string strWarDetail = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "warDataTip");
            UBB.toHtml(ref strWarDetail);
            warDataTip.SetText(UTooltipParamName.BodyText, strWarDetail);
        }

        public void setPlayerInfo(cmd_player_info_return cmdPlayerInfo)
        {
            playerHead.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, WndID.WND_ID_PLAYERFRAME, 2, 1);
            playerName.text = cmdPlayerInfo.chPlayerName;
            kinName.text = cmdPlayerInfo.chKinName;
            clanName.text = cmdPlayerInfo.chClanName;

            playerLvNow.text = cmdPlayerInfo.nLv.ToString();
            playerLvNext.text = (cmdPlayerInfo.nLv + 1).ToString();
            playerExp.maxValue = cmdPlayerInfo.ulNeedExp;
            playerExp.value = cmdPlayerInfo.ulCurrentExp;
            expTip.SetText(UTooltipParamName.BodyText, 
                String.Format("{0}:{1}/{2}", ULocalizationService.Instance.Get("UIView", "Common", "ExpValue"), cmdPlayerInfo.ulCurrentExp, cmdPlayerInfo.ulNeedExp));
            int nMatchCount = cmdPlayerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_MATCHNUM] == 0 ? 1 : cmdPlayerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_MATCHNUM];
            int nWinPro = cmdPlayerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_WINNUM] * 100 / nMatchCount;
            //warWinPro.text = String.Format("{0}%", nWinPro);
            int nPKWinPro = cmdPlayerInfo.nPKTotalNum > 0 ? (cmdPlayerInfo.nPKWinNum * 100 / cmdPlayerInfo.nPKTotalNum) : 0;
            pkWinPro.text = String.Format("{0}%", nPKWinPro);
            updatePlayerKillRecord((int)cmdPlayerInfo.pDbid);
            if (param != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
            }

            rankShow(cmdPlayerInfo);
            
            EditNameGo.SetActive(LogicDataCenter.playerSystemDataManager.bRequestSelf);

            totalMatchNum.text = cmdPlayerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_MATCHNUM].ToString();
            winMatchNum.text = cmdPlayerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_WINNUM].ToString();
            winMatchPro.text = String.Format("{0}%", nWinPro);
            mvp.text = cmdPlayerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_MVPNUM].ToString();
            float nMvpPro = cmdPlayerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_MVPNUM] > 0 ? ((float)nMatchCount / cmdPlayerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_MVPNUM]) : 0.0f;
            mvpTip.text = String.Format(ULocalizationService.Instance.Get("UIview", "PlayerFrame", "mvpTip"), nMvpPro);
            godLike.text = cmdPlayerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_SUPERGODNUM].ToString();
            float nGodPro = cmdPlayerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_SUPERGODNUM] > 0 ? ((float)nMatchCount / cmdPlayerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_SUPERGODNUM]) : 0.0f;
            godLikeTip.text = String.Format(ULocalizationService.Instance.Get("UIview", "PlayerFrame", "godTip"), nGodPro);
            fiveKill.text = cmdPlayerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_5KILLNUM].ToString();
            fourKill.text = cmdPlayerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_4KILLNUM].ToString();
            threeKill.text = cmdPlayerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_3KILLNUM].ToString();

            setVisible(true);
        }

        public void rankShow(cmd_player_info_return cmdPlayerInfo)
        {
            // 没有参加过比赛
            if (cmdPlayerInfo.nRankScore == 0)
            {
                // 显示无效的段位frame
                RankFrame.SetActive(false);
                InvalidRankFrame.SetActive(true);
            }
            else
            {
                // 显示有效的段位frame
                RankFrame.SetActive(true);
                InvalidRankFrame.SetActive(false);
            }

            if (cmdPlayerInfo.nRankScore == 0)
            {
                // todo 没参加过排位赛界面处理
            }
            else
            {
                if (cmdPlayerInfo.nRankIconID != 0)
                {
                    rankImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, WndID.WND_ID_PLAYERFRAME, 1, cmdPlayerInfo.nRankIconID);
                    rankName.text = cmdPlayerInfo.chRankName;
                    rankScore.text = String.Format("{0}/{1}" + ULocalizationService.Instance.Get("UIview", "Common", "Score"), cmdPlayerInfo.nRankScore, cmdPlayerInfo.nNextRankScore);

                    param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_PlayerBaseInfo_RankIcon1 + cmdPlayerInfo.nRankIconID - 1, _tfAttachParent: rankImage.gameObject.transform, _bAutoDestroy: false);
                    UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);

                    RankStarFrame.gameObject.SetActive(true);
                    int nMatchType = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MATCHTYPE);
                    int nRankScore = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_RANKSCORE);
                    int nRankGrade = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_RANKGRADE);
                    int nServerOrderID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_KINGRANK_ORDER);
                    int nBaseKingRankScore = GameLogicAPI.getMatchtTypeBaseKingRankScore(nMatchType);
                    if (nRankScore >= nBaseKingRankScore)
                    {
                        // 王者段位排名
                        if (nServerOrderID == 0)
                        {
                            serverOrder.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "serverOrderDefalutDes");
                        }
                        else
                        {
                            serverOrder.text = String.Format(ULocalizationService.Instance.Get("UIview", "PlayerFrame", "serverOrderDes"), nServerOrderID);
                        }

                        // 不是自己不显示
                        serverOrder.gameObject.SetActive(LogicDataCenter.playerSystemDataManager.bRequestSelf);
                    }
                    else
                    {
                        serverOrder.gameObject.SetActive(false);
                    }
                    int nRankIcon = GameLogicAPI.getRankIconIDByGrade(nMatchType, nRankGrade);
                    int nRankStar = GameLogicAPI.getActorRankStar(nMatchType, nRankScore);
                    int nMaxRankStar = GameLogicAPI.getActorCurRankMaxStar(nMatchType, nRankScore);
                    for (int index = 0; index < RankStarFrame.childCount; ++index)
                    {
                        if (nRankStar > 0)
                        {
                            RankStarFrame.GetChild(index).GetComponent<Image>().sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankStarImage, WndID.WND_ID_WAR_MAIN_FRAME, 2);
                            nRankStar--;
                            nMaxRankStar--;
                        }
                        else if (nMaxRankStar > 0)
                        {
                            RankStarFrame.GetChild(index).GetComponent<Image>().sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankStarImage, WndID.WND_ID_WAR_MAIN_FRAME, 1);
                            nMaxRankStar--;
                        }
                        else
                        {
                            RankStarFrame.GetChild(index).GetComponent<Image>().gameObject.SetActive(false);
                        }
                    }
                }
            }
            
        }

        public void rankBtnClick()
        {
            // 显示点击排位赛引导界面
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RANK_CLICK_SHOW, null);
        }
	
        public void updatePlayerKillRecord(int nReqPDBID)
        {
            int nActorID = 0;
            if (EntityFactory.MainHeroView != null)
            {
                nActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            }
            ActorStatisticInfo info = LogicDataCenter.warStatisticDataManager.GetActorOverview(nReqPDBID);
            int nMatchCount = info.totalStatistic.nRecentWinCount + info.totalStatistic.nRecentLoseCount;
            nMatchCount = nMatchCount == 0 ? 1 : nMatchCount;
            //int nKDA = info.totalStatistic.nRecentKDAScore / nMatchCount;
            //kda.text = nKDA.ToString("F1");
            //kda.text = info.totalStatistic.nRecentKDAScore.ToString();
            kda.text = info.totalStatistic.nRecentScore.ToString();

            float fKill = (float)info.totalStatistic.nRecentKillCount / nMatchCount;
            float fDead = (float)info.totalStatistic.nRecentDieCount / nMatchCount;
            float fAssist = (float)info.totalStatistic.nRecentAssistCount / nMatchCount;
            kill.text = String.Format("{0:F1}/{1:F1}/{2:F1}", fKill, fDead, fAssist);

            warWinPro.text = String.Format("{0}%", info.totalStatistic.nRecentWinCount * 100 / nMatchCount);
        }

        public void onActorNameChange()
        {
            if (LogicDataCenter.playerSystemDataManager.bRequestSelf)
            {
                playerName.text = EntityFactory.MainHeroView.Property.CreatureName;
            }
        }

        public void onRankTipBtnClick()
        {
            string strTitle = ULocalizationService.Instance.Get("UIView", "PlayerFrame", "rankTipTitle");
            string strContent = ULocalizationService.Instance.Get("UIView", "PlayerFrame", "rankTipContent");
            UPopupTipMsgData data = new UPopupTipMsgData(strTitle, strContent);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW_STR, data);
        }

        public void setVisible(bool bIsShow)
        {
            this.gameObject.SetActive(bIsShow);

            if (!bIsShow && param != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
            }
        }
    }

    public class ActorNameChangeView : MonoBehaviour
    {
        public InputField UserNameField;
        public Button ConfirmButton;
        public GameObject ConfirmFrame;
        public Text MoneyText;
        public Text[] WarningText;
        public GameObject[] WarningFlagObj;
        public Text CostTipText;
        public Button ConfirmSubButton;
        public Text NameSubText;
        private SSchemePointShopGoodsConfig m_config;
        private int m_nCostType = -1;// 点券0金币1
        public Toggle MoneyToggle;
        public Toggle PointToggle;
        public void Awake()
        {
            ConfirmFrame.SetActive(false);

            if (m_config == null)
            {
                List<SSchemePointShopGoodsConfig> configList = PointShopConfig.Instance.GetGoodsListByType(PointShopGoodsType.Goods_Special);
                if (configList != null && configList.Count > 0)
                {
                    foreach (var item in configList)
                    {
                        if (item.nConsumpApproach == (int)PointShopConsumpApproach.Approach_Actor_Rename)
                        {
                            m_config = item;
                            break;
                        }
                    }
                }

                if (m_config != null && MoneyToggle != null && PointToggle != null)
                {
                    MoneyToggle.gameObject.SetActive(false);
                    PointToggle.gameObject.SetActive(false);
                    if (m_config.nPointPrice > 0 || m_config.nMoneyPrice > 0)
                    {
                        MoneyToggle.gameObject.SetActive(true);
                        PointToggle.gameObject.SetActive(true);
                        MoneyToggle.isOn = true;
                    }
                    if (m_config.nPointPrice > 0)
                    {
                        PointToggle.gameObject.SetActive(true);
                        PointToggle.isOn = true;
                    }
                    if (m_config.nMoneyPrice > 0)
                    {
                        MoneyToggle.gameObject.SetActive(true);
                        MoneyToggle.isOn = true;
                    }
                }
            }
        }

        public void OpenActorNameChangeView()
        {
            this.gameObject.SetActive(! this.gameObject.activeSelf);
            if (!this.gameObject.activeSelf)
                return;

            UserNameField.text = "";
            ConfirmFrame.SetActive(false);
            ConfirmButton.interactable = false;
            NameSubText.text = "";
            CostTipText.text = "";
            for (int i = 0; i < 3; i++)
            {
                WarningText[i].text = ULocalizationService.Instance.Get("UIView", "CreateHeroView", "NameDes" + (i + 1));
                WarningFlagObj[i].gameObject.SetActive(false);
            }

            DoCostNameChangeInType();
            OnInputPlayerTextChange("");
        }

        public void OnInputPlayerTextChange(string sText)
        {
            if (UserNameField == null || ConfirmButton == null)
            {
                return;
            }

            if (ShieldWord.Instance.IsStringMaxLenRight(sText))
            {
                LogicDataCenter.playerSystemDataManager.sFixActorName = sText;
            }
            else
            {
                if (!string.IsNullOrEmpty(LogicDataCenter.playerSystemDataManager.sFixActorName))
                    UserNameField.text = LogicDataCenter.playerSystemDataManager.sFixActorName;
            }

            if (m_config == null)
            {
                ConfirmButton.interactable = false;
                return;
            }

            DoConfirmButtonEnable();
        }

        private void DoConfirmButtonEnable()
        {
            if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property == null )
            {
                return;
            }

            bool bCostEnough = m_nCostType == 1 ? EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_GOLD) >= m_config.nMoneyPrice && m_config.nMoneyPrice != 0
            : EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TICKET) >= m_config.nPointPrice && m_config.nPointPrice != 0;
            bool bLenRight = false;
            bool bSensitive = false;
            bool bNumHead = false;
            if (string.IsNullOrEmpty(LogicDataCenter.playerSystemDataManager.sFixActorName))
            {
                 bLenRight = false;
                 bSensitive = false;
                 bNumHead = false;          
            }
            else
            {
                 bLenRight = ShieldWord.Instance.IsStringLenRight(LogicDataCenter.playerSystemDataManager.sFixActorName);
                 bSensitive = ShieldWord.Instance.IsSensitiveAndSpecialString(LogicDataCenter.playerSystemDataManager.sFixActorName);
                 bNumHead = ShieldWord.Instance.IsStringHeadCharIsNum(LogicDataCenter.playerSystemDataManager.sFixActorName);
            }

            ConfirmButton.interactable = bLenRight && !bSensitive && !bNumHead&&bCostEnough;
            WarningText[0].color = bLenRight?Color.white:Color.red;
            WarningText[1].color = !bSensitive ? Color.white : Color.red;
            WarningText[2].color = !bNumHead ? Color.white : Color.red;
            WarningFlagObj[0].SetActive(!bLenRight);
            WarningFlagObj[1].SetActive(bSensitive);
            WarningFlagObj[2].SetActive(bNumHead);

            CostTipText.text = bCostEnough ? "" : (m_nCostType == 1 ? "金币不足" : "点券不足"); ;
        }

        public void OnConfirmButtonClick()
        {
            ConfirmFrame.SetActive(true);
            NameSubText.text = "确定角色名修改成" + "<color=#ffd16d>" + LogicDataCenter.playerSystemDataManager.sFixActorName + "</color>" + "么？";
        }

        public void OnConfirmSubButtonClick()
        {
            cmd_Actor_Name_Fix cmdInfo = new cmd_Actor_Name_Fix();
            cmdInfo.szNewName = LogicDataCenter.playerSystemDataManager.sFixActorName;
            cmdInfo.nCostType = m_nCostType;
            IntPtrHelper helper = new IntPtrHelper();
            IntPtr ptr = helper.toPtr<cmd_Actor_Name_Fix>(ref cmdInfo);
            int len = Marshal.SizeOf(cmdInfo);
            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_ACTOR_NAME_FIX, len, "", ptr, len);
        }

        public void onRenameResultMessage(int nResult)
        {
            if(!this.gameObject.activeSelf)
            {
                return;
            }

            bool bResult = nResult > 0;
            this.gameObject.SetActive(!bResult);
        }

        public void OnMoneyButtonSelect(bool bIsOn)
        {
            if (!bIsOn)
            {
                return;
            }

            m_nCostType = 1;
            DoCostNameChangeInType();
            DoConfirmButtonEnable();
        }

        public void OnTicketButtonSelect(bool bIsOn)
        {
            if (!bIsOn)
            {
                return;
            }

            m_nCostType = 0;
            DoCostNameChangeInType();
            DoConfirmButtonEnable();
        }

        public void DoCostNameChangeInType()
        {
            if (m_config != null)
            {
                if (m_nCostType == 0)
                {
                    MoneyText.text = "点击确定" + "<color=#ffed57>消耗" + m_config.nPointPrice + "点券</color>" + "完成改名";
                }
                else if (m_nCostType == 1)
                {
                    MoneyText.text = "点击确定" + "<color=#ffed57>消耗" + m_config.nMoneyPrice + "金币</color>" + "完成改名";
                }
            }     
        }
    }

    public class UViewPlayerInfo : UIBaseSubWndView
    {
        public Text CompareBtnDesc;
        public URichText HonorBtnDesc;

        public PlayerInfo playerInfo;
        public GameObject heroGroup;
        public GameObject CompareBtn;

        ResNode heroReqRes;
        ActorNameChangeView actorNameChangeView;
        ResNode actorNameChangeNode;
        public override void Awake()
        {
            base.Awake();
            CompareBtnDesc.text = ULocalizationService.Instance.Get("UIview", "PlayerFrame", "CompareBtnDesc");
            HonorBtnDesc.text = "<a href=HonorBtn color=bbbbbc>" + ULocalizationService.Instance.Get("UIview", "PlayerFrame", "HonorBtnDesc") + "</a>";
            HonorBtnDesc.onHrefClick.AddListener(OnClickHonorBtnRT);

            heroReqRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/PlayerInfo/HeroInfo");
            if (null == heroReqRes)
            {
                return;
            }
        }

        public override void OnDestroy()
        {
            heroReqRes = null;
            AssetBundleManager.DeleteAssets(ref heroReqRes, true);
            AssetBundleManager.DeleteAssets(ref actorNameChangeNode, true);
            base.OnDestroy();
        }

        public void showPlayerInfo(cmd_player_info_return cmdPlayerInfo)
        {
            // 替换名字
            if (cmdPlayerInfo.bFake == 1)
            {
                cmdPlayerInfo.chPlayerName = LogicDataCenter.playerSystemDataManager.strCurFakeName;
            }

            playerInfo.setPlayerInfo(cmdPlayerInfo);

            //比一比按钮的显示与否
            if (LogicDataCenter.playerSystemDataManager.bRequestSelf)
                CompareBtn.SetActive(false);
            else
                CompareBtn.SetActive(true);
        }

        public void updatePlayerKillRecord()
        {
            playerInfo.updatePlayerKillRecord(LogicDataCenter.playerSystemDataManager.nRequestPDBID);
        }

        public void showOftenUseHeroInfo(List<cmd_entity_hero_info> heroInfoList)
        {
            if (heroInfoList.Count > 5 || heroReqRes == null)
                return;

            // 在创建预制体之前，先把group内的东西清空
            int nChildCount = heroGroup.transform.childCount;
            for (int i = 0; i < nChildCount; i++)
            {
                GameObject go = heroGroup.transform.GetChild(0).gameObject;
                ResNode.DestroyRes(ref go);
            }

            bool bIsFirst = true;
            int nMaxUseTime = 0;
            // dicHeroInfo接收的经常使用的英雄是按照使用英雄的次数进行排序的
            foreach (cmd_entity_hero_info item in heroInfoList)
            {
                if (item.nHeroID == 0)
                    continue;

                // 可以先打开游戏看一下像经验条一样，我要先确定最大经验， 即英雄使用做多的次数
                if (bIsFirst)
                {
                    nMaxUseTime = item.nGameCount;
                    bIsFirst = false;
                }

                // 创建预制体并进行赋值显示
                GameObject instance = heroReqRes.InstanceMainRes();
                instance.name = heroReqRes.AssetName;
                if (instance == null)
                    continue;
                instance.transform.SetParent(heroGroup.transform, false);
                HeroInfo heroInfo = instance.GetComponent<HeroInfo>();
                if (heroInfo == null)
                    continue;
                bool bReqSelf = LogicDataCenter.playerSystemDataManager.bRequestSelf;
                int nReqID = LogicDataCenter.playerSystemDataManager.nRequestPDBID;
                heroInfo.setRequestInfo(bReqSelf, nReqID);
                heroInfo.setHeroInfo(item, nMaxUseTime);
            }
        }

        public void OnClickPlayerCompare()
        {
            int nActorID = LogicDataCenter.playerSystemDataManager.nRequestPDBID;
            if (nActorID == 0)
                return;

            UIMsgCmdData uiData = UIMsgCmdDataHelper.Instance.create(WndMsgID.WND_MSG_PLAYERINFOCOMPARE_OPEN, nActorID);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERINFOCOMPARE_OPEN, uiData);
        }

        public void OnNameChangeBtnClick()
        {
            if (actorNameChangeView == null)
            {
                actorNameChangeNode = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,UPath.UPrefabsPath + "PlayerInfo/" + "ActorNameChangeView");
                if (actorNameChangeNode == null)
                {
                    Debug.Log("actorNameChangePrefab Is Null");
                    return;
                }

                GameObject actorNameChangeObject = actorNameChangeNode.InstanceMainRes();
                if (actorNameChangeObject == null)
                {
                    return;
                }

                actorNameChangeView = actorNameChangeObject.GetComponent<ActorNameChangeView>();
                actorNameChangeObject.transform.SetParent(this.transform, false);
                actorNameChangeObject.SetActive(false);
            }

            if (actorNameChangeView == null)
            {
                return;
            }

            actorNameChangeView.OpenActorNameChangeView();
        }

        public void OnClickHonorBtnRT(string eventName)
        {
            if (eventName == "HonorBtn")
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HONORPAGE_OPEN, null);
            }
        }

        public void closeChildrenView()
        {
            if (actorNameChangeView)
                actorNameChangeView.gameObject.SetActive(false);
        }

        public void onRenameResultMessage(int nResult)
        {
            if (playerInfo != null)
            {
                playerInfo.onActorNameChange();
            }

            if (actorNameChangeView != null)
            {
                actorNameChangeView.onRenameResultMessage(nResult);
            }
        }
    }
}
