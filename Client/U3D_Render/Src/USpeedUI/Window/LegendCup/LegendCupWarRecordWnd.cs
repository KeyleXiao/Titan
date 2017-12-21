using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using UnityEngine.EventSystems;
using GameLogic;
using DataCenter;
using UIWidgets;
using ASpeedGame.Data.GameMobaSchemes;
using U3D_Render.Common;
using War;
using LegendCup_ManagedDef;
using ASpeedGame.Data.WarStatisticMap;
using USpeedUI.UEffect;

namespace USpeedUI.LegendCup
{
    public enum EWarRecordEntryType
    {
        NONE,
        TOTAL_INFO,
        CUP_NODE_INFO,

        MAX
    };

    public class LegendCupWarRecordWnd : UIPopupWnd<LegendCupWarRecordWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LEGENDCUP_WARRECORD;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "LegendCup/WarRecordView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_TOTAL, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_SINGLE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_CUPNODE_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_CUPNODE_RECV, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_TOTAL, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_SINGLE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_CUPNODE_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_CUPNODE_RECV, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_SHOW:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                            m_wndView.OnShowTotalInfo();
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_TOTAL:
                    {
                        if (m_wndView != null)
                            m_wndView.OnUpdateTotalInfo();
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_SINGLE:
                    {
                        if (m_wndView != null)
                            m_wndView.OnUpdateWarInfo();
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_CUPNODE_SHOW:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                            m_wndView.OnShowCupNodeInfo(data as LegendCupNodeWarData);
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_CUPNODE_RECV:
                    {
                        if (m_wndView != null)
                            m_wndView.OnUpdateCupNodeInfo();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        if (m_wndView != null)
                            m_wndView.OnClickCloseBtn();
                    }
                    break;
                default:
                    break;
            }
        }

        public override bool OnCloseKey()
        {
            if (m_wndView != null)
            {
                m_wndView.OnClickCloseBtn();
            }
            return base.OnCloseKey();
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);

            UCommonFullWndVisible msgData = new UCommonFullWndVisible();
            msgData.msgID = (int)WndMsgID.WND_MSG_COMMON_FULLWND_VISIBLE;
            msgData.nWndID = (int)GetID();
            msgData.isVisible = _bVisible;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_FULLWND_VISIBLE, msgData);
        }
    }

    public class LegendCupWarHistoryItem : MonoBehaviour, IPointerDownHandler
    {
        public Text kin1Name;
        public Text kin2Name;
        public Image kin1WinIcon;
        public Image kin2WinIcon;
        public Text matchTime;
        public Image judgeIcon;
        public UTooltipTrigger judgeTooltip;

        private long nWarID;
        public long WarID { get { return nWarID; } }
        private int nBoCount;
        public int BoCount { get { return nBoCount; } }
        private cmd_kin_warrecord_total_warnode nodeInfo;
        public cmd_kin_warrecord_total_warnode NodeInfo { get { return nodeInfo; } }
        private LegendCupWarRecordWndView wndView;
        public void SetWndView(LegendCupWarRecordWndView view)
        {
            wndView = view;
        }

        public void Clear()
        {
            this.gameObject.SetActive(false);
        }

        public void SetData(cmd_kin_warrecord_total_warnode data)
        {
            nWarID = data.nWarID;
            nBoCount = data.nBoCount;
            nodeInfo = data;
            string strMonth = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "Month");
            string strDay = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "Day");

            kin1Name.text = data.szKin1Name;
            kin2Name.text = data.szKin2Name;
            judgeTooltip.SetText(UTooltipParamName.BodyText, ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "WaitJudge"));
            kin1WinIcon.gameObject.SetActive(data.nWinKinID == data.nKin1ID);
            kin2WinIcon.gameObject.SetActive(data.nWinKinID == data.nKin2ID);
            judgeIcon.gameObject.SetActive(data.nWarID > 0 && data.byWinFlag == (byte)EJudgeNodeWar.JUDGE_NONE);// 已打、未裁决

            //if (data.nBeginTime > 0)
            //{
            //    DateTime calcTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(data.nBeginTime);
            //    calcTime = calcTime.ToLocalTime();
            //    matchTime.text = String.Format("{0:D2}" + strMonth + " {1:D2}" + strDay + " {2:D2} : {3:D2}", calcTime.Month, calcTime.Day, calcTime.Hour, calcTime.Minute);
            //}
            //else
            //{
            //    matchTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "NotMatch");
            //}

            matchTime.text = "";
            EJudgeNodeWar JudgeWar = (EJudgeNodeWar)data.byWinFlag;
            switch (JudgeWar)
            {
                case EJudgeNodeWar.JUDGE_NONE:
                    {
                        if (data.nWarID > 0)
                        {
                            matchTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "WaitJudgeHaveMatch");
                            matchTime.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
                        }
                        else
                        {
                            matchTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "WaitJudgeNoMatch");
                            matchTime.color = UDefines.CommomColor(ECommonColor.ECC_Gray);
                        }
                    }
                    break;
                case EJudgeNodeWar.JUDGE_Cancle:
                    {
                        //记录被移除不用处理
                    }
                    break;
                case EJudgeNodeWar.JUDGE_SelectWinner:
                    {
                        matchTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "SelectWinner");
                        matchTime.color = UDefines.CommomColor(ECommonColor.ECC_Green1);
                    }
                    break;
                case EJudgeNodeWar.JUDGE_AllKnockOut:
                    {
                        matchTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "KnockOutMatch");
                        matchTime.color = UDefines.CommomColor(ECommonColor.ECC_Green1);
                    }
                    break;
                case EJudgeNodeWar.JUDGE_Reset:
                    {
                        matchTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "ResetMatch");
                        matchTime.color = UDefines.CommomColor(ECommonColor.ECC_Green1);
                    }
                    break;
                case EJudgeNodeWar.JUDGE_ConfirmNode:
                    {
                        matchTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "ConfirmMatch");
                        matchTime.color = UDefines.CommomColor(ECommonColor.ECC_Green1);
                    }
                    break;
                default:
                    break;
            }

            if (wndView.WarRecordEntryType == (int)EWarRecordEntryType.TOTAL_INFO)
            {
                matchTime.gameObject.SetActive(false);
                judgeIcon.gameObject.SetActive(false);
            }

            gameObject.SetActive(true);
        }

        public virtual void OnPointerDown(PointerEventData eventData)
        {
            wndView.OnSelectWarHistory(nWarID, nBoCount);
        }
    }

    public class LegendCupSingleWarActorItem : MonoBehaviour, IPointerDownHandler
    {
        public Image heroIcon;
        public GameObject achiIconContainer;
        public Text actorName;
        public Text score;
        public Text money;
        public Text kdaValue;
        public RectTransform equipFrame;
        public GameObject equipItemTemplate;

        private const int equipItemCount = 6;
        private Image[] itemIcon;
        private UTooltipTrigger[] tooltipTrigger;
        private LegendCupWarRecordWndView wndView;

        public enum AchiIconType
        {
            Icon_MVP,
            Icon_TopDamage,
            Icon_TopKill,
            Icon_TopAssist,
            Icon_TopTank,
            Icon_TopTowerKill,
            Icon_TopRich,
            Icon_TopFarm,
            Icon_Godlike,
            Icon_TripleKill,
            Icon_FourKill,
            Icon_FiveKill,
            Icon_AliveAssist,
            Icon_DeadlyControl,
            Icon_ThrilSave,
            Icon_TeamControl,
        }
        
        public void SetWndView(LegendCupWarRecordWndView view)
        {
            wndView = view;

            // 创建equipItem
            //float fEquipItemOffsetX = -100f;
            itemIcon = new Image[equipItemCount];
            tooltipTrigger = new UTooltipTrigger[equipItemCount];
            for (int i = 0; i < equipItemCount; ++i)
            {
                GameObject equipItemObj = ResNode.InstantiateRes(equipItemTemplate);
                if (!equipItemObj)
                    return;

                itemIcon[i] = equipItemObj.GetComponent<Image>();
                tooltipTrigger[i] = equipItemObj.GetComponent<UTooltipTrigger>();

                equipItemObj.name = "EquipItem" + (i + 1);
                RectTransform rectTrans = equipItemObj.transform as RectTransform;
                rectTrans.SetParent(equipFrame, false);
                //rectTrans.anchoredPosition = new Vector2(fEquipItemOffsetX, 0f);
                //fEquipItemOffsetX += 40f;
            }

            equipItemTemplate.SetActive(false);
        }

        public void Clear()
        {
            gameObject.SetActive(true);

            heroIcon.sprite = wndView.emptyHeroIcon;
            actorName.text = "-";
            score.text = "-";
            money.text = "-";
            kdaValue.text = "-";
            for (int i = 0; i < itemIcon.Length; ++i)
            {
                itemIcon[i].sprite = wndView.emptyHeroIcon;
            }
            for (int i = 0; i < tooltipTrigger.Length; ++i)
            {
                tooltipTrigger[i].enabled = false;
            }

            ClearAchiIcon();
        }

        private void ClearAchiIcon()
        {
            Transform containerTrans = achiIconContainer.transform;
            int nChildCount = containerTrans.childCount;
            for (int i = nChildCount - 1; i >= 0; --i)
            {
                var temp = containerTrans.GetChild(i).gameObject;
                ResNode.DestroyRes(ref temp);
            }
        }

        public void SetData(cmd_entity_update_warstatistic_war_actorinfo data, int nScoreTop, int nMoneyTop, int nKillTop, int nDieTop, int nAssistTop)
        {
            heroIcon.sprite = wndView.GetHeroIcon(data.nHeroID);
            actorName.text = data.szActorName;
            score.text = data.nKdaScore.ToString();
            Color clr;
            if (data.nKdaScore == nScoreTop)
                ColorUtility.TryParseHtmlString("#FB8C3A", out clr);
            else
                clr = Color.white;
            score.color = clr;
            money.text = String.Format("{0:N1}k", data.nGetMoney / 1000.0f);
            if (data.nGetMoney == nMoneyTop)
                ColorUtility.TryParseHtmlString("#FFCE34", out clr);
            else
                clr = Color.white;
            money.color = clr;
            String strKDAValue = "";
            if (data.nKillPersonNum == nKillTop)
                strKDAValue += "<color=#FFCE34FF>" + data.nKillPersonNum + "</color>";
            else
                strKDAValue += data.nKillPersonNum.ToString();
            strKDAValue += "/";
            if (data.nDieCount == nDieTop)
                strKDAValue += "<color=#FFCE34FF>" + data.nDieCount + "</color>";
            else
                strKDAValue += data.nDieCount.ToString();
            strKDAValue += "/";
            if (data.nAssistCount == nAssistTop)
                strKDAValue += "<color=#FFCE34FF>" + data.nAssistCount + "</color>";
            else
                strKDAValue += data.nAssistCount.ToString();
            kdaValue.text = strKDAValue;

            for (int i = 0; i < itemIcon.Length; ++i)
            {
                Image iconImage = itemIcon[i];
                UTooltipTrigger tooltip = tooltipTrigger[i];
                SSchemeMobaGoods goodsInfo;
                if (data.nEquipID[i] <= 0 || !LogicDataCenter.goodsSchemesDataManager.GetMobaGoods_Out(data.nEquipID[i], out goodsInfo))
                {
                    iconImage.sprite = wndView.emptyHeroIcon;
                    tooltip.enabled = false;
                    continue;
                }

                int nEquipIcon = goodsInfo.GoodsIconID;
                if (nEquipIcon > 0)
                {
                    Sprite equipSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MobaGoods, WndID.WND_ID_PLAYERFRAME, 1, nEquipIcon);
                    if (equipSprite == null)
                        Debug.LogWarning("can't find mobagoods icon " + nEquipIcon);
                    iconImage.sprite = equipSprite;
                }
                else
                {
                    iconImage.sprite = wndView.emptyHeroIcon;
                }

                // tooltip
                UBB.toHtml(ref goodsInfo.GoodsName, UBB_FORMAT_TYPE.UGUI);
                tooltip.SetText(UTooltipParamName.ItemName, goodsInfo.GoodsName);
                tooltip.SetText(UTooltipParamName.ItemCost, goodsInfo.GoodsCost.ToString());
                UBB.toHtml(ref goodsInfo.GoodsDes, UBB_FORMAT_TYPE.UGUI);
                tooltip.SetText(UTooltipParamName.ItemDesc, goodsInfo.GoodsDes);
                tooltip.enabled = true;
            }

            ClearAchiIcon();
            if ((data.nGlory & (int)EWarMedal.EWM_SCORE) > 0)
                AddAchiIcon((int)AchiIconType.Icon_MVP);
            if ((data.nGlory & (int)EWarMedal.EWM_OUTPUT) > 0)
                AddAchiIcon((int)AchiIconType.Icon_TopDamage);
            if ((data.nGlory & (int)EWarMedal.EWM_KILLCOUNT) > 0)
                AddAchiIcon((int)AchiIconType.Icon_TopKill);
            if ((data.nGlory & (int)EWarMedal.EWM_ASSCOUNT) > 0)
                AddAchiIcon((int)AchiIconType.Icon_TopAssist);
            if ((data.nGlory & (int)EWarMedal.EWM_DAMAGE) > 0)
                AddAchiIcon((int)AchiIconType.Icon_TopTank);
            if ((data.nGlory & (int)EWarMedal.EWM_DESTORYTOWER) > 0)
                AddAchiIcon((int)AchiIconType.Icon_TopTowerKill);
            if ((data.nGlory & (int)EWarMedal.EWM_MONEY) > 0)
                AddAchiIcon((int)AchiIconType.Icon_TopRich);
            if ((data.nGlory & (int)EWarMedal.EWM_KILLMONSTER) > 0)
                AddAchiIcon((int)AchiIconType.Icon_TopFarm);
            if ((data.nGlory & (int)EWarMedal.EWM_DEADLYCONTROL) > 0)
                AddAchiIcon((int)AchiIconType.Icon_DeadlyControl);
            if ((data.nGlory & (int)EWarMedal.EWM_THRILLSAVE) > 0)
                AddAchiIcon((int)AchiIconType.Icon_ThrilSave);
            if ((data.nGlory & (int)EWarMedal.EWM_TEAMCONTROL) > 0)
                AddAchiIcon((int)AchiIconType.Icon_TeamControl);
            if ((data.nAliveAssistMax) >= 15)
                AddAchiIcon((int)AchiIconType.Icon_AliveAssist);
            if (data.nContinueKillMax >= 8)
                AddAchiIcon((int)AchiIconType.Icon_Godlike);
            if (data.nMultiKillMax == 3)
                AddAchiIcon((int)AchiIconType.Icon_TripleKill);
            if (data.nMultiKillMax == 4)
                AddAchiIcon((int)AchiIconType.Icon_FourKill);
            if (data.nMultiKillMax == 5)
                AddAchiIcon((int)AchiIconType.Icon_FiveKill);

            gameObject.SetActive(true);
        }

        private void AddAchiIcon(int nIconIndex)
        {
            if (nIconIndex < 0 || nIconIndex >= wndView.achievementIconImage.Length)
                return;

            GameObject achiObj = wndView.achiIconObj.InstanceMainRes();
            if (achiObj)
            {
                Image icon = achiObj.GetComponent<Image>();
                icon.sprite = wndView.achievementIconImage[nIconIndex];
                achiObj.transform.SetParent(achiIconContainer.transform, false);

                string stTitleName = "Title_" + nIconIndex;
                UTooltipTrigger titleTooltip = achiObj.GetComponent<UTooltipTrigger>();
                if (titleTooltip)
                {
                    string stTitleDesc = ULocalizationService.Instance.Get("ToolTips", "RecordTableEnd", stTitleName);
                    titleTooltip.SetText(UTooltipParamName.BodyText, "<color=#FFFFFFFF>" + stTitleDesc + "</color>");
                }
            }
        }

        public virtual void OnPointerDown(PointerEventData eventData)
        {
        }
    }

    public class LegendCupWarRecordWndView : UIBaseWndView
    {
        // Border
        public Text sysTitleDes;
        public Text titleDes;
        public Text subTitleDes1;
        //public Text subTitleDes2;
        // HistoryWar
        public RectTransform histroyItemFrame;
        public GameObject histroyItemTemplate;
        private const int histroyItemCount = 6;
        private LegendCupWarHistoryItem[] historyItem;
        public Image historySelectImage;
        public Text historyPageText;
        public Button historyPrevPageBtn;
        public Button historyNextPageBtn;
        public Text historyNoDataText;
        // SingleWar
        public ResNode achiIconObj
        {
            get
            {
                return achiIconPrefabRes;
            }
        }
        private ResNode achiIconPrefabRes;
        public Sprite[] achievementIconImage;
        public Text historyDate;
        public Text historyBeginTime;
        public Text historyLastTime;
        public Text historyWarType;
        public Text selfKillCount;
        public Text selfMoney;
        public Text winTeamLabel;
        public Text scoreLabel1;
        public Text moneyLabel1;
        public Text kdaLabel1;
        public Text itemLabel1;
        public Text loseTeamLabel;
        public Text scoreLabel2;
        public Text moneyLabel2;
        public Text kdaLabel2;
        public Text itemLabel2;
        public Sprite emptyHeroIcon;
        public RectTransform warActorItemFrame;
        public GameObject warActorItemTemplate;
        private const int warActorItemCount = 10;
        private LegendCupSingleWarActorItem[] warActorItem;
        public Image historySelfIcon;
        public Text resetBtnDes;
        public Text JudgeBtnDes;
        public Text ConfirmBtnDes;
        public GameObject onlineBtnFrame;
        public Text TotalScoreKin1Des;
        public Text TotalScoreKin2Des;
        public Text TotalVSScoreDes;
        public GameObject TotalScoreGo;

        public Text offlineResetBtnDes;
        //public Text offlineJudgeBtnDes;
        public GameObject offlineBtnFrame;

        public Text returnBtnDes;

        // Data
        private long curWarID;
        private int curWarHistoryPage;
        private int curWarHistorySel;
        private bool m_viewCupNode; // 标识(1.战绩入口2.比赛节点入口)
        private long m_legendCupID;
        private int m_roundID;
        private int m_searchID;

        // 标题光效
        public GameObject AttachPoint1;
        public GameObject AttachPoint2;
        private UEffectParamBase param1;
        private UEffectParamBase param2;

        public EasyLayout.EasyLayout historyLayout;
        public EasyLayout.EasyLayout actorLayout;

        private int m_nWarRecordEntryType;
        public int WarRecordEntryType { get { return m_nWarRecordEntryType; } }

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);
            achiIconPrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, UPath.UPrefabsPath + "PlayerWarStatistic/AchiIconItem");

            // 创建historyItem
            historyItem = new LegendCupWarHistoryItem[histroyItemCount];
            //float fHistroyItemOffsetY = -46.35f;
            for (int i = 0; i < histroyItemCount; ++i)
            {
                GameObject histroyItemObj = ResNode.InstantiateRes(histroyItemTemplate);
                if (!histroyItemObj)
                    return false;

                historyItem[i] = histroyItemObj.GetComponent<LegendCupWarHistoryItem>();
                if (!historyItem[i])
                    return false;

                histroyItemObj.name = "HistroyItem" + (i + 1);
                historyItem[i].SetWndView(this);
                RectTransform rectTrans = histroyItemObj.transform as RectTransform;
                rectTrans.SetParent(histroyItemFrame, false);
                //rectTrans.anchoredPosition = new Vector2(143f, fHistroyItemOffsetY);
                //fHistroyItemOffsetY -= 95.7f;
            }
            histroyItemTemplate.SetActive(false);

            // 创建warActorItem
            warActorItem = new LegendCupSingleWarActorItem[warActorItemCount];
            //float fWarActorItemOffsetX = 322f;
            //float fWarActorItemOffsetY = -43f;
            for (int i = 0; i < warActorItemCount; ++i)
            {
                GameObject warActorItemObj = ResNode.InstantiateRes(warActorItemTemplate);
                if (!warActorItemObj)
                    return false;

                warActorItem[i] = warActorItemObj.GetComponent<LegendCupSingleWarActorItem>();
                if (!warActorItem[i])
                    return false;

                warActorItemObj.name = "MemberItem" + (i + 1);
                warActorItem[i].SetWndView(this);
                RectTransform rectTrans = warActorItemObj.transform as RectTransform;
                rectTrans.SetParent(warActorItemFrame, false);
                //rectTrans.anchoredPosition = new Vector2(fWarActorItemOffsetX, fWarActorItemOffsetY);
                //if (i == warActorItemCount / 2 - 1)
                //{
                //    fWarActorItemOffsetX = 970f;
                //    fWarActorItemOffsetY = -43f;
                //}
                //else
                //    fWarActorItemOffsetY -= 90f;
            }
            warActorItemTemplate.SetActive(false);

            sysTitleDes.text = ULocalizationService.Instance.Get("UIView", "Common", "OpenMatchSystem");
            titleDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "Title");
            subTitleDes1.text = ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "SubTitle1");

            historyNoDataText.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "HistoryNoData");

            winTeamLabel.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarWinTeam");
            scoreLabel1.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarScore");
            moneyLabel1.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMoney");
            kdaLabel1.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarKDA");
            itemLabel1.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarGoods");
            loseTeamLabel.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarLoseTeam");
            scoreLabel2.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarScore");
            moneyLabel2.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMoney");
            kdaLabel2.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarKDA");
            itemLabel2.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarGoods");
            resetBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "ResetBtn");
            JudgeBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "JudgeBtn");
            ConfirmBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "ConfirmBtn");
            returnBtnDes.text = ULocalizationService.Instance.Get("UIView", "Common", "Return");

            TotalScoreKin1Des.text = "";
            TotalScoreKin2Des.text = "";
            TotalVSScoreDes.text = "";
            TotalVSScoreDes.color = UDefines.CommomColor(ECommonColor.ECC_Blue2);
            TotalScoreGo.SetActive(false);

            if (param1 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param1);
            }
            param1 = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_LegendCupListView_ConfineFrame_beisai01, _tfAttachParent: AttachPoint1.transform, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param1);
            if (param2 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param2);
            }
            param2 = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_LegendCupListView_ConfineFrame_beisai02, _tfAttachParent: AttachPoint2.transform, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param2);

            m_nWarRecordEntryType = (int)EWarRecordEntryType.NONE;

            return true;
        }

        public override void Destroy()
        {
            AssetBundleManager.DeleteAssets(ref achiIconPrefabRes, true);
            base.Destroy();
        }

        public void OnShowTotalInfo()
        {
            m_nWarRecordEntryType = (int)EWarRecordEntryType.TOTAL_INFO;
            ClearHistoryPage();
            m_viewCupNode = false;
            onlineBtnFrame.SetActive(false);
            offlineBtnFrame.SetActive(false);

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_KIN_REQ_WARRECORD_TOTAL);
        }

        public void OnShowCupNodeInfo(LegendCupNodeWarData nodeData)
        {
            m_nWarRecordEntryType = (int)EWarRecordEntryType.CUP_NODE_INFO;
            ClearHistoryPage();
            m_viewCupNode = true;
            m_legendCupID = nodeData.nLegendCupID;
            m_roundID = nodeData.nRoundID;
            m_searchID = nodeData.nSearchID;
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(m_legendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return;
            int nSelfPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            if (cupBaseData.nIsOnline == 1)
            {
                onlineBtnFrame.SetActive(nSelfPDBID == cupBaseData.nCreaterPDBID);
                offlineBtnFrame.SetActive(false);
            }
            else
            {
                offlineBtnFrame.SetActive(nSelfPDBID == cupBaseData.nCreaterPDBID);
                onlineBtnFrame.SetActive(false);
            }

            cmd_legendcup_view_competition_node cmdData = new cmd_legendcup_view_competition_node();
            cmdData.nLegendCupID = m_legendCupID;
            cmdData.nRoundID = m_roundID;
            cmdData.nSearchID = m_searchID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_view_competition_node>(GameLogicDef.GVIEWCMD_LEGENDCUP_VIEW_COMPETITION_NODE, ref cmdData);
        }

        public void OnShowWarInfo()
        {
            WarStatisticInfo info = LogicDataCenter.warStatisticDataManager.GetSingleWarInfo(curWarID, EGetSingleWarType.EGSWT_KinRecord);
            if (info.mainInfo.nWarID != 0)
            {
                OnUpdateWarInfo();
            }
        }

        public void OnUpdateTotalInfo()
        {
            List<cmd_kin_warrecord_total_warnode> info = LogicDataCenter.legendCupDataManager.GetTotalWarRecord();
            if (info != null)
            {
                SetWarHistroyPage(curWarHistoryPage);
                SetWarHistorySelect(0);
            }
            else
            {
                // 清空数据
                ClearHistoryPage();
            }
        }

        public void OnUpdateCupNodeInfo()
        {
            SingleNodeRecordInfo info = LogicDataCenter.legendCupDataManager.GetCupNodeWarRecord();
            if (info != null)
            {
                SetWarHistroyPage(curWarHistoryPage);
                SetWarHistorySelect(0);
            }
            else
            {
                // 清空数据
                ClearHistoryPage();
            }

            SetTotalScoreInfo();
        }

        public void SetTotalScoreInfo()
        {
            cmd_legendcup_recv_competitionnode_head totalHeadInfo = LogicDataCenter.legendCupDataManager.GetTotalWarRecordHeadInfo();
            TotalScoreKin1Des.text = totalHeadInfo.szKin1Name;
            TotalScoreKin2Des.text = totalHeadInfo.szKin2Name;
            TotalVSScoreDes.text = Convert.ToString(totalHeadInfo.nKin1Score) + " : " + Convert.ToString(totalHeadInfo.nKin2Score);
            TotalScoreGo.SetActive(true);
        }

        public void OnUpdateWarInfo()
        {
            WarStatisticInfo info = LogicDataCenter.warStatisticDataManager.GetSingleWarInfo(curWarID, EGetSingleWarType.EGSWT_KinRecord);
            if (info.mainInfo.nWarID != curWarID)
            {
                ClearWarInfoPage();
                return;
            }

            // 计算一次布局，放置自身图标
            actorLayout.NeedUpdateLayout();

            DateTime calcTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(info.mainInfo.nBeginTime);
            calcTime = calcTime.ToLocalTime();
            historyDate.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarDate") + ": " + String.Format("{0:D2}-{1:D2}", calcTime.Month, calcTime.Day);
            historyBeginTime.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarStartTime") + ": " + String.Format("{0:D2}:{1:D2}", calcTime.Hour, calcTime.Minute);
            int lastTime = info.mainInfo.nEndTime - info.mainInfo.nBeginTime;
            lastTime /= 60;
            int nLastHour = lastTime / 60;
            int nLastMin = lastTime % 60;
            String lastTimeStr = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarLastTime") + ":";
            if (nLastHour > 0)
                lastTimeStr += nLastHour + ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarHour");
            lastTimeStr += nLastMin + ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMinute");
            historyLastTime.text = lastTimeStr;
            // 目前只有匹配赛
            string strMatchDes = "";
            if (WarStatisticMapConfig.Instance.MapConfig.ContainsKey(info.mainInfo.nWarType))
            {
                strMatchDes = WarStatisticMapConfig.Instance.MapConfig[info.mainInfo.nWarType].strName;
            }
            historyWarType.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMatchType") + ":" + strMatchDes;

            int winIndex = 0;
            int loseIndex = 5;
            bool bCamp1Win = false;
            if (info.mainInfo.nEndBecause == 1 || info.mainInfo.nEndBecause == 4)
                bCamp1Win = true;

            int actorCount = info.actorInfo.Count;
            for (int i = 0; i < warActorItem.Length; ++i)
            {
                warActorItem[i].Clear();
            }
            historySelfIcon.gameObject.SetActive(false);
            int nCamp1KillCount = 0;
            int nCamp1GetMoney = 0;
            int nCamp2KillCount = 0;
            int nCamp2GetMoney = 0;
            int nScoreTop = 0;
            int nMoneyTop = 0;
            int nKillTop = 0;
            int nDieTop = 0;
            int nAssistTop = 0;
            for (int i = 0; i < actorCount; ++i)
            {
                cmd_entity_update_warstatistic_war_actorinfo actorInfo = info.actorInfo[i];
                if (nScoreTop < actorInfo.nKdaScore)
                    nScoreTop = actorInfo.nKdaScore;
                if (nMoneyTop < actorInfo.nGetMoney)
                    nMoneyTop = actorInfo.nGetMoney;
                if (nKillTop < actorInfo.nKillPersonNum)
                    nKillTop = actorInfo.nKillPersonNum;
                if (nDieTop < actorInfo.nDieCount)
                    nDieTop = actorInfo.nDieCount;
                if (nAssistTop < actorInfo.nAssistCount)
                    nAssistTop = actorInfo.nAssistCount;
            }

            cmd_entity_update_warstatistic_war_actorinfo curActorInfo = new cmd_entity_update_warstatistic_war_actorinfo();
            bool bActorWin = false;
            for (int i = 0; i < actorCount; ++i)
            {
                cmd_entity_update_warstatistic_war_actorinfo actorInfo = info.actorInfo[i];
                if (actorInfo.nCamp == 1)
                {
                    nCamp1KillCount += actorInfo.nKillPersonNum;
                    nCamp1GetMoney += actorInfo.nGetMoney;
                }
                else
                {
                    nCamp2KillCount += actorInfo.nKillPersonNum;
                    nCamp2GetMoney += actorInfo.nGetMoney;
                }


                int nActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                bool bWin = false;
                if ((actorInfo.nCamp == 1 && bCamp1Win) || (actorInfo.nCamp != 1 && !bCamp1Win))
                    bWin = true;
                if (bWin)
                {
                    if (winIndex < warActorItem.Length)
                    {
                        warActorItem[winIndex].SetData(actorInfo, nScoreTop, nMoneyTop, nKillTop, nDieTop, nAssistTop);
                        if (actorInfo.nActorID == nActorID)
                        {  
                            historySelfIcon.gameObject.SetActive(true);
                            historySelfIcon.transform.SetPosition(warActorItem[winIndex].transform.position);
                            historySelfIcon.rectTransform.sizeDelta = (warActorItem[winIndex].transform as RectTransform).sizeDelta;

                            curActorInfo = actorInfo;
                            bActorWin = bWin;
                        }
                        ++winIndex;
                    }
                    else
                    {
                        Debug.Log("PlayerWarStatisticWndView OnUpdateWarInfo winIndex > length");
                    }
                }
                else
                {
                    if (loseIndex < warActorItem.Length)
                    {
                        warActorItem[loseIndex].SetData(actorInfo, nScoreTop, nMoneyTop, nKillTop, nDieTop, nAssistTop);
                        if (actorInfo.nActorID == nActorID)
                        {
                            historySelfIcon.transform.SetPosition(warActorItem[loseIndex].transform.position);
                            historySelfIcon.gameObject.SetActive(true);
                            curActorInfo = actorInfo;
                            bActorWin = !bWin;
                        }
                        ++loseIndex;
                    }
                    else
                    {
                        Debug.Log("PlayerWarStatisticWndView OnUpdateWarInfo loseIndex > length");
                    }
                }
            }

            if (bCamp1Win)
            {
                selfKillCount.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarKillNum") + ": " + nCamp1KillCount + "/" + nCamp2KillCount;
                selfMoney.text = String.Format("{0}: {1:N1}k/{2:N1}k", ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMoney"), nCamp1GetMoney / 1000.0f, nCamp2GetMoney / 1000.0f);
            }
            else
            {
                selfKillCount.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarKillNum") + ": " + nCamp2KillCount + "/" + nCamp1KillCount;
                selfMoney.text = String.Format("{0}: {1:N1}k/{2:N1}k", ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMoney"), nCamp2GetMoney / 1000.0f, nCamp1GetMoney / 1000.0f);
            }
        }

        private void SetWarHistroyPage(int nPageIndex)
        {
            List<cmd_kin_warrecord_total_warnode> info = null;
            if (m_viewCupNode)
	        {
                SingleNodeRecordInfo recordInfo = LogicDataCenter.legendCupDataManager.GetCupNodeWarRecord();
                info = recordInfo.baseInfo;
	        }
            else
            {
                info = LogicDataCenter.legendCupDataManager.GetTotalWarRecord();
            }
                
            if (info != null)
            {
                int totalCount = info.Count;
                int itemInPage = historyItem.Length;
                if (totalCount <= 0)
                {
                    ClearHistoryPage();
                    return;
                }
                historyNoDataText.gameObject.SetActive(false);
                historyPrevPageBtn.gameObject.SetActive(true);
                historyNextPageBtn.gameObject.SetActive(true);

                int maxPage = totalCount / itemInPage;
                if (totalCount % itemInPage > 0)
                    ++maxPage;
                nPageIndex = Mathf.Clamp(nPageIndex, 1, maxPage);
                int baseOffset = (nPageIndex - 1) * itemInPage;
                for (int i = 0; i < itemInPage; ++i)
                {
                    int index = baseOffset + i;
                    if (index < totalCount)
                        historyItem[i].SetData(info[index]);
                    else
                        historyItem[i].Clear();
                }

                historyPageText.text = nPageIndex + "/" + maxPage;
                curWarHistoryPage = nPageIndex;
            }
        }
        
        private void SetWarHistorySelect(int nSel)
        {
            ClearWarInfoPage();
            if (nSel < 0 || nSel >= historyItem.Length || historyItem[nSel].gameObject.activeSelf == false)
            {
                curWarHistorySel = -1;
                historySelectImage.gameObject.SetActive(false);
                return;
            }

            // 计算一次布局，放置选中图标位置
            historyLayout.NeedUpdateLayout();

            curWarHistorySel = nSel;
            Vector3 curSelPos = historySelectImage.gameObject.transform.position;
            curSelPos.y = historyItem[nSel].transform.position.y;

            historySelectImage.gameObject.SetActive(true);
            historySelectImage.gameObject.transform.SetPosition(curSelPos);
            historySelectImage.rectTransform.sizeDelta = (historyItem[nSel].transform as RectTransform).sizeDelta;

            curWarID = historyItem[nSel].WarID;
            OnShowWarInfo();
        }

        private void ClearHistoryPage()
        {
            for (int i = 0; i < historyItem.Length; ++i)
            {
                historyItem[i].Clear();
            }
            historyPageText.text = "";
            historyPrevPageBtn.gameObject.SetActive(false);
            historyNextPageBtn.gameObject.SetActive(false);
            historyNoDataText.gameObject.SetActive(true);
            curWarHistoryPage = 0;

            SetWarHistorySelect(-1);
        }

        private void ClearWarInfoPage()
        {
            historyDate.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarDate") + ":";
            historyBeginTime.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarStartTime") + ":";
            historyLastTime.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarLastTime") + ":";
            historyWarType.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMatchType") + ":";
            selfKillCount.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarKillNum") + ":";
            selfMoney.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMoney") + ":";
            for (int i = 0; i < warActorItem.Length; ++i)
            {
                warActorItem[i].Clear();
            }
            historySelfIcon.gameObject.SetActive(false);
        }

        public Sprite GetHeroIcon(int nHeroID)
        {
            Sprite sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 3, nHeroID);
            if (sprite == null)
            {
                sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 3, 10);
            }

            return sprite;
        }

        // 入口不同，判断标志不同，因为杯赛可能WarID都为0
        public void OnSelectWarHistory(long nWarID, int nBoCount)
        {
            if (m_viewCupNode)
            {
                for (int i = 0; i < historyItem.Length; ++i)
                {
                    if (historyItem[i].BoCount == nBoCount)
                    {
                        SetWarHistorySelect(i);
                        break;
                    }
                }
            }
            else
            {
                for (int j = 0; j < historyItem.Length; ++j)
                {
                    if (historyItem[j].WarID == nWarID)
                    {
                        SetWarHistorySelect(j);
                        break;
                    }
                }
            }
        }

        public void OnClickHistoryPrevBtn()
        {
            if (curWarHistoryPage > 1)
            {
                SetWarHistroyPage(curWarHistoryPage - 1);
                SetWarHistorySelect(-1);
            }
        }

        public void OnClickHistoryNextBtn()
        {
            List<cmd_kin_warrecord_total_warnode> info = null;
            if (m_viewCupNode)
            {
                SingleNodeRecordInfo recordInfo = LogicDataCenter.legendCupDataManager.GetCupNodeWarRecord();
                info = recordInfo.baseInfo;
            }
            else
            {
                info = LogicDataCenter.legendCupDataManager.GetTotalWarRecord();
            }

            if (info != null)
            {
                int totalCount = info.Count;
                int itemInPage = historyItem.Length;
                if (totalCount <= 0)
                {
                    return;
                }

                int maxPage = totalCount / itemInPage;
                if (totalCount % itemInPage > 0)
                    ++maxPage;

                if (curWarHistoryPage < maxPage)
                {
                    SetWarHistroyPage(curWarHistoryPage + 1);
                    SetWarHistorySelect(-1);
                }
            }
        }

        public void OnClickOnlineResetBtn()
        {
            LegendCupOnlineResetData uiData = new LegendCupOnlineResetData();
            uiData.nLegendCupID = m_legendCupID;
            uiData.nRoundID = m_roundID;
            uiData.nSearchID = m_searchID;

            cmd_legendcup_recv_competitionnode_head headInfo = LogicDataCenter.legendCupDataManager.GetTotalWarRecordHeadInfo();
            uiData.nKin1ID = headInfo.nKin1ID;
            uiData.nKin2ID = headInfo.nKin2ID;
            uiData.szKin1Name = headInfo.szKin1Name;
            uiData.szKin2Name = headInfo.szKin2Name;
            uiData.nResetEndTime = headInfo.nResetEndTime;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_ONLINE_RESET_SHOW, uiData);

            
        }

        

        public void OnClickOnlineJudgeBtn()
        {
            if (curWarHistorySel < 0 || curWarHistorySel >= historyItem.Length || historyItem[curWarHistorySel].gameObject.activeSelf == false)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NOT_SELECT_ANY_WAR_RECORD);
                return;
            }

            LegendCupJudgeWarData uiData = new LegendCupJudgeWarData();
            uiData.nLegendCupID = m_legendCupID;
            uiData.nRoundID = m_roundID;
            uiData.nSearchID = m_searchID;
            uiData.nWarID = curWarID;
            uiData.nKin1ID = historyItem[curWarHistorySel].NodeInfo.nKin1ID;
            uiData.nKin2ID = historyItem[curWarHistorySel].NodeInfo.nKin2ID;
            uiData.szKin1Name = historyItem[curWarHistorySel].NodeInfo.szKin1Name;
            uiData.szKin2Name = historyItem[curWarHistorySel].NodeInfo.szKin2Name;
            uiData.nWinKinID = historyItem[curWarHistorySel].NodeInfo.nWinKinID;
            uiData.nBoCount = historyItem[curWarHistorySel].NodeInfo.nBoCount;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_JUDGE_SHOW, uiData);
        }

        public void OnClickResetBtn()
        {
            // 未选中
            if (curWarHistorySel < 0 || curWarHistorySel >= historyItem.Length || historyItem[curWarHistorySel].gameObject.activeSelf == false)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NOT_SELECT_ANY_WAR_RECORD);
                return;
            }
            // 已裁决
            if (historyItem[curWarHistorySel].NodeInfo.byWinFlag > (byte)EJudgeNodeWar.JUDGE_NONE)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_CANT_REPEATE_JUDGED);
                return;
            }
            // 未比赛
            if (historyItem[curWarHistorySel].WarID <= 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NOT_MATCH_CANT_RESET_WAR);
                return;
            }

            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                    title: ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "ResetTitle"),
                                    message: ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "ResetContent"),
                                    buttons: new UIWidgets.DialogActions() { { ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "Confirm"), DoResetCup }, { ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "Cancel"), null } }
                                    );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        public void OnClickJudgeBtn()
        {
            if (curWarHistorySel < 0 || curWarHistorySel >= historyItem.Length || historyItem[curWarHistorySel].gameObject.activeSelf == false)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NOT_SELECT_ANY_WAR_RECORD);
                return;
            }
            if (historyItem[curWarHistorySel].NodeInfo.byWinFlag > (byte)EJudgeNodeWar.JUDGE_NONE)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_CANT_REPEATE_JUDGED);
                return;
            }

            LegendCupJudgeWarData uiData = new LegendCupJudgeWarData();
            uiData.nLegendCupID = m_legendCupID;
            uiData.nRoundID = m_roundID;
            uiData.nSearchID = m_searchID;
            uiData.nWarID = curWarID;
            uiData.nKin1ID = historyItem[curWarHistorySel].NodeInfo.nKin1ID;
            uiData.nKin2ID = historyItem[curWarHistorySel].NodeInfo.nKin2ID;
            uiData.szKin1Name = historyItem[curWarHistorySel].NodeInfo.szKin1Name;
            uiData.szKin2Name = historyItem[curWarHistorySel].NodeInfo.szKin2Name;
            uiData.nWinKinID = historyItem[curWarHistorySel].NodeInfo.nWinKinID;
            uiData.nBoCount = historyItem[curWarHistorySel].NodeInfo.nBoCount;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_JUDGE_SHOW, uiData);
        }

        public void OnClickConfirmBtn()
        {
            // 未选中
            if (curWarHistorySel < 0 || curWarHistorySel >= historyItem.Length || historyItem[curWarHistorySel].gameObject.activeSelf == false)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NOT_SELECT_ANY_WAR_RECORD);
                return;
            }
            // 已裁决
            if (historyItem[curWarHistorySel].NodeInfo.byWinFlag > (byte)EJudgeNodeWar.JUDGE_NONE)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_CANT_REPEATE_JUDGED);
                return;
            }
            // 未比赛
            if (historyItem[curWarHistorySel].WarID <= 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NOT_MATCH_CANT_CONFIRM_WAR);
                return;
            }

            string strResult;
            if (historyItem[curWarHistorySel].NodeInfo.nWinKinID == historyItem[curWarHistorySel].NodeInfo.nKin1ID)
                strResult = historyItem[curWarHistorySel].NodeInfo.szKin1Name + ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "Win");
            else if (historyItem[curWarHistorySel].NodeInfo.nWinKinID == historyItem[curWarHistorySel].NodeInfo.nKin2ID)
                strResult = historyItem[curWarHistorySel].NodeInfo.szKin2Name + ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "Win");
            else
                strResult = ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "Abstain");

            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                    title: ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "ConfirmTitle"),
                                    message: string.Format(ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "ConfirmContent"), historyItem[curWarHistorySel].NodeInfo.szKin1Name, historyItem[curWarHistorySel].NodeInfo.szKin2Name, strResult),
                                    //message: ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "ConfirmContent"),
                                    buttons: new UIWidgets.DialogActions() { { ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "Confirm"), DoConfirmCup }, { ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "Cancel"), null } }
                                    );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        private bool DoResetCup()
        {
            cmd_legendcup_judge_competition_node node = new cmd_legendcup_judge_competition_node();
            node.nLegendCupID = m_legendCupID;								// 杯赛ID
            node.nRoundID = m_roundID;										// 轮次ID
            node.nSearchID = m_searchID;									// 节点ID
            node.nBoCount = historyItem[curWarHistorySel].NodeInfo.nBoCount;// 第几场
            node.llWarID = curWarID;										// 比赛ID
            node.byJudgeFlag = (byte)EJudgeNodeWar.JUDGE_Cancle;			// EJudgeNodeWar 
            node.dwWinnerKinID = 0;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_judge_competition_node>(GameLogicDef.GVIEWCMD_LEGENDCUP_JUDGE_COMPETITION_NODE, ref node);

            return true;
        }

        private bool DoConfirmCup()
        {
            cmd_legendcup_judge_competition_node node = new cmd_legendcup_judge_competition_node();
            node.nLegendCupID = m_legendCupID;								// 杯赛ID
            node.nRoundID = m_roundID;										// 轮次ID
            node.nSearchID = m_searchID;									// 节点ID
            node.nBoCount = historyItem[curWarHistorySel].NodeInfo.nBoCount;// 第几场
            node.llWarID = curWarID;										// 比赛ID
            node.byJudgeFlag = (byte)EJudgeNodeWar.JUDGE_ConfirmNode;		// EJudgeNodeWar
            node.dwWinnerKinID = historyItem[curWarHistorySel].NodeInfo.nWinKinID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_judge_competition_node>(GameLogicDef.GVIEWCMD_LEGENDCUP_JUDGE_COMPETITION_NODE, ref node);

            return true;
        }

        public void OnClickCloseBtn()
        {
            if (param1 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param1);
            }
            if (param2 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param2);
            }

            m_wnd.UnloadView();
        }
    }
}
