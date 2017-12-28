/*******************************************************************
** 文件名:	PlayerInfoCompareWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	2016-8-31
** 版  本:	1.0
** 描  述:	角色信息比较界面逻辑模块(接发消息)+视图模块(显示数据)	
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using DataCenter;
using GameLogic;
using ASpeedGame.Data.SelectArrayTipPage;
using UIWidgets;
using USpeedUI.PlayerFrame;

namespace USpeedUI.PlayerInfoCompare
{
    public class PlayerInfoCmp : MonoBehaviour
    {
        public Image playerHead;
        public Text playerName;
        public Text playerLv;
        public Text fightCap;
        public Text rankName;
        public Image rankIcon;
        public Text winPro;
        public Text matchNum;
        public Slider matchNumIcon;

        public Text FightCap;

        public int nWinPro;
        public int nFightCap;
        private UPlayerInfoCompareWndView wndView;

        public void SetWndView(UPlayerInfoCompareWndView view)
        {
            wndView = view;
        }

        public void SetPlayerInfo(cmd_player_info_return sPlayerInfo)
        {
            FightCap.text = wndView.strFightCap;

            int nSex = sPlayerInfo.nSex;
            if (nSex < (int)PERSON_SEX.SEX_MAX && nSex >= (int)PERSON_SEX.SEX_MALE)
            {
                playerHead.gameObject.SetActive(true);
                playerHead.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, WndID.WND_ID_PLAYERINFOCOMPARE, 2, nSex + 1);
            }
            else
            {
                playerHead.gameObject.SetActive(false);
            }
            playerName.text = sPlayerInfo.chPlayerName;
            playerLv.text = "等级: " + sPlayerInfo.nLv.ToString();
            nFightCap = (int)sPlayerInfo.ulFighting;
            fightCap.text = nFightCap.ToString();
            rankName.text = sPlayerInfo.chRankName;
            if (sPlayerInfo.nRankIconID != 0)
            {
                rankIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, WndID.WND_ID_PLAYERINFOCOMPARE, 1, sPlayerInfo.nRankIconID);
            }

            int nMaxCount = sPlayerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_MATCHNUM];
            int nWinCount = sPlayerInfo.nGlory[(int)ENACHIEVE.ENACHIEVE_WINNUM];
            matchNum.text = "<color=#6fbe80ff>" + nWinCount + "胜/</color>" + "<color=#fb8406ff>" + (nMaxCount - nWinCount) + "负</color>";

            nMaxCount = nMaxCount == 0 ? 1 : nMaxCount;
            nWinPro = nWinCount * 100 / nMaxCount;
            winPro.text = nWinPro + "%";
            matchNumIcon.maxValue = 1;
            matchNumIcon.value = (float)nWinPro / 100;
        }
    }

    public class StatisticInfoCmp : MonoBehaviour
    {
        public Text mostUse;
        public GameObject[] useIcon;
        public Text kda;
        public Text detailInfo;
        private UPlayerInfoCompareWndView wndView;

        public void SetWndView(UPlayerInfoCompareWndView view)
        {
            wndView = view;
        }

        public void SetStatisticInfo(cmd_entity_update_warstatistic_total sStatisticInfo, Dictionary<HeroType, int> dicMatchCount)
        {
            int nMaxCount = 0;
            HeroType nMaxType = HeroType.HEROTYPE_ALL;
            int nMinCount = int.MaxValue;
            HeroType nMinType = HeroType.HEROTYPE_ALL;
            int nSumCount = 0;
            foreach (var item in dicMatchCount)
            {
                if (item.Value > nMaxCount)
                {
                    nMaxCount = item.Value;
                    nMaxType = item.Key;
                }

                if (item.Value < nMinCount && item.Value != 0)
                {
                    nMinCount = item.Value;
                    nMinType = item.Key;
                }

                nSumCount += item.Value;
            }

            // 设置常用英雄及分布图
            if (nMaxCount == 0)
            {
                mostUse.text = wndView.strHeroType[(int)HeroType.HEROTYPE_ALL];

                useIcon[(int)HeroType.HEROTYPE_SOLDIER].SetActive(false);
                useIcon[(int)HeroType.HEROTYPE_MAGIC].SetActive(false);
                useIcon[(int)HeroType.HEROTYPE_ASSASSIN].SetActive(false);
                useIcon[(int)HeroType.HEROTYPE_SUPPORT].SetActive(false);
                useIcon[(int)HeroType.HEROTYPE_ADC].SetActive(false);
            }
            else
            {
                mostUse.text = GetPercentStr((int)nMaxType, dicMatchCount[nMaxType], nSumCount);

                foreach (var itemView in dicMatchCount)
                {
                    if (itemView.Value != 0)
                    {
                        useIcon[(int)itemView.Key].SetActive(true);

                        LayoutElement layout = useIcon[(int)itemView.Key].GetComponent<LayoutElement>();
                        UTooltipTrigger tooltip = useIcon[(int)itemView.Key].GetComponent<UTooltipTrigger>();
                        if (!layout || !tooltip)
                            return;

                        layout.flexibleWidth = (float)itemView.Value / nMinCount;

                        tooltip.SetText(UTooltipParamName.BodyText, GetPercentStr((int)itemView.Key, itemView.Value, nSumCount));
                    }
                    else
                    {
                        useIcon[(int)itemView.Key].SetActive(false);
                    }
                }
            }

            kda.text = sStatisticInfo.nRecentKDAScore.ToString();
            string strKillCount = sStatisticInfo.nRecentKillCount.ToString();
            string strDieCount = sStatisticInfo.nRecentDieCount.ToString();
            string strAssistCount = sStatisticInfo.nRecentAssistCount.ToString();
            detailInfo.text = strKillCount + "/ " + strDieCount + "/ " + strAssistCount;
        }

        private string GetPercentStr(int nIndex, int nValue, int nSum)
        {
            float percent = (float)nValue * 100 / nSum;
            return wndView.strHeroType[(int)nIndex] + "(" + percent.ToString("F2") + "%)";
        }
    }

    public class UPlayerInfoCompareWnd : UIPopupWnd<UPlayerInfoCompareWndView>
    {
        int nSelfPDBID;
        int nReqPDBID; 

        public override WndID GetID()
        {
            return WndID.WND_ID_PLAYERINFOCOMPARE;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "PlayerInfoCompare/PlayerInfoCompareView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;

            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERINFOCOMPARE_OPEN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERINFO_INITINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_UPDATETOTAL, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERINFOCOMPARE_OPEN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERINFO_INITINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_UPDATETOTAL, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_PLAYERINFOCOMPARE_OPEN:
                    {
                        this.SetVisible(true);

                        if (m_wndView == null || EntityFactory.MainHeroView == null)
                            return;

                        nSelfPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                        UIMsgCmdData uiData = (UIMsgCmdData)data;
                        nReqPDBID = uiData.nParam;
                        m_wndView.SetComparePDBID(nSelfPDBID, nReqPDBID);

                        if (IsDataReady(nSelfPDBID, nReqPDBID))
                        {
                            m_wndView.SetData();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_PLAYERINFO_INITINFO:
                case WndMsgID.WND_MSG_PLAYERWARSTATISTIC_UPDATETOTAL:
                    {
                        if (m_wndView == null)
                            return;

                        if (IsDataReady(nSelfPDBID, nReqPDBID))
                        {
                            m_wndView.SetData();
                        }   
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        this.UnloadView();
                    }
                    break;
                default:
                    break;
            }
        }

        private bool IsDataReady(int nSelfID, int nReqID)
        {
            ActorPlayerInfo selfPlayerInfo = LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo(nSelfID);
            ActorPlayerInfo reqPlayerInfo = LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo(nReqID);
            ActorStatisticInfo selfStatisticInfo = LogicDataCenter.warStatisticDataManager.GetActorOverview(nSelfID);
            ActorStatisticInfo reqStatisticInfo = LogicDataCenter.warStatisticDataManager.GetActorOverview(nReqID);
            if (selfPlayerInfo.playerInfo.pDbid == 0 || reqPlayerInfo.playerInfo.pDbid == 0 || selfStatisticInfo.totalStatistic.nActorID == 0 || reqStatisticInfo.totalStatistic.nActorID == 0)
            {
                return false;
            }
            return true;
        }
    }

    public class UPlayerInfoCompareWndView : UIBaseWndView
    {
        public PlayerInfoCmp selfPlayerView;
        public PlayerInfoCmp reqPlayerView;
        public StatisticInfoCmp selfStatisticView;
        public StatisticInfoCmp reqStatisticView;
        public GameObject selfHeroGroup;
        public GameObject reqHeroGroup;
        public GameObject selfFightCap;
        public GameObject reqFightCap;
        public GameObject selfWinPro;
        public GameObject reqWinPro;
        public GameObject selfKDA;
        public GameObject reqKDA;
        public Button closeBtn;

        public Text Title;
        public Text WinPro;
        public Text Prefer;
        public Text Kda;
        public Text Expert;
        public string strFightCap;
        public string[] strHeroType;

        private ResNode selfHeroInsRes;
        private ResNode reqHeroInsRes;

        private int nSelfPDBID;
        private int nReqPDBID;
        private Dictionary<HeroType, int> selfDict;
        private Dictionary<HeroType, int> reqDict;
        
        public override void Awake()
        {
            selfHeroInsRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/PlayerInfoCompare/HeroInfoSelf");
            reqHeroInsRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/PlayerInfoCompare/HeroInfoReq");

            base.Awake();
        }

        public override void Destroy()
        {
            base.Destroy();
            AssetBundleManager.DeleteAssets(ref selfHeroInsRes, true);
            AssetBundleManager.DeleteAssets(ref reqHeroInsRes, true);
        }

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            selfDict = new Dictionary<HeroType, int>();
            reqDict = new Dictionary<HeroType, int>();

            strHeroType = new string[6];
            for (int i = 0; i < strHeroType.Length; i++)
            {
                strHeroType[i] = ULocalizationService.Instance.Get("UIView", "PlayerInfoCompare", "HeroType" + i.ToString());
            }
            strFightCap = ULocalizationService.Instance.Get("UIView", "PlayerInfoCompare", "FightCap");
            Title.text = ULocalizationService.Instance.Get("UIView", "PlayerInfoCompare", "Title");
            WinPro.text = ULocalizationService.Instance.Get("UIView", "PlayerInfoCompare", "WinPro");
            Prefer.text = ULocalizationService.Instance.Get("UIView", "PlayerInfoCompare", "Prefer");
            Kda.text = ULocalizationService.Instance.Get("UIView", "Common", "Kda");
            Expert.text = ULocalizationService.Instance.Get("UIView", "PlayerInfoCompare", "Expert");

            return true;
        }

        public void SetComparePDBID(int selfID, int reqID)
        {
            nSelfPDBID = selfID;
            nReqPDBID = reqID;
        }

        public void SetData()
        {
            ActorPlayerInfo selfPlayerInfo = LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo(nSelfPDBID);
            ActorPlayerInfo reqPlayerInfo = LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo(nReqPDBID);
            ActorStatisticInfo selfStatisticInfo = LogicDataCenter.warStatisticDataManager.GetActorOverview(nSelfPDBID);
            ActorStatisticInfo reqStatisticInfo = LogicDataCenter.warStatisticDataManager.GetActorOverview(nReqPDBID);
            if (selfPlayerInfo.playerInfo.pDbid == 0 || reqPlayerInfo.playerInfo.pDbid == 0 || selfStatisticInfo.totalStatistic.nActorID == 0 || reqStatisticInfo.totalStatistic.nActorID == 0)
            {
                return;
            }

            // 设置玩家数据
            selfPlayerView.SetWndView(this);
            reqPlayerView.SetWndView(this);
            selfPlayerView.SetPlayerInfo(selfPlayerInfo.playerInfo);
            reqPlayerView.SetPlayerInfo(reqPlayerInfo.playerInfo);

            // 设置战绩数据
            selfStatisticView.SetWndView(this);
            reqStatisticView.SetWndView(this);
            HeroTypeNumCount(selfStatisticInfo.totalStatisticWar, ref selfDict);
            selfStatisticView.SetStatisticInfo(selfStatisticInfo.totalStatistic, selfDict);
            HeroTypeNumCount(reqStatisticInfo.totalStatisticWar, ref reqDict);
            reqStatisticView.SetStatisticInfo(reqStatisticInfo.totalStatistic, reqDict);

            // 设置英雄数据
            SetOftenUseHeroInfo(selfPlayerInfo.strongHeroInfoList, selfHeroGroup, selfHeroInsRes, nSelfPDBID, true);
            SetOftenUseHeroInfo(reqPlayerInfo.strongHeroInfoList, reqHeroGroup, reqHeroInsRes, nReqPDBID, false);
        
            // 设置PK赢标志
            selfWinPro.SetActive(true);
            reqWinPro.SetActive(true);
            if (selfPlayerView.nFightCap < reqPlayerView.nFightCap)
                selfFightCap.SetActive(false);
            else if (selfPlayerView.nFightCap > reqPlayerView.nFightCap)
                reqFightCap.SetActive(false);

            selfWinPro.SetActive(true);
            reqWinPro.SetActive(true);
            if (selfPlayerView.nWinPro < reqPlayerView.nWinPro)
                selfWinPro.SetActive(false);
            else if (selfPlayerView.nWinPro > reqPlayerView.nWinPro)
                reqWinPro.SetActive(false);

            selfKDA.SetActive(true);
            reqKDA.SetActive(true);
            if (selfStatisticInfo.totalStatistic.nRecentKDAScore < reqStatisticInfo.totalStatistic.nRecentKDAScore)
                selfKDA.SetActive(false);
            else if (selfStatisticInfo.totalStatistic.nRecentKDAScore > reqStatisticInfo.totalStatistic.nRecentKDAScore)
                reqKDA.SetActive(false);
        }

        public void OnClickCloseBtn()
        {
            SetVisible(false);
        }

        // 设置经常使用英雄信息
        private void SetOftenUseHeroInfo(List<cmd_entity_hero_info> heroInfoList, GameObject heroGroup,ResNode heroIns, int nPDBID, bool bIsSelf)
        {
            if (heroInfoList.Count > 5 || heroIns == null)
                return;

            // 每次先清空HeroGroup内容
            int nChildCount = heroGroup.transform.childCount;
            for (int i = nChildCount - 1; i >= 0; --i)
            {
                GameObject go = heroGroup.transform.GetChild(i).gameObject;
                ResNode.DestroyRes(ref go);
            }

            bool bIsFirst = true;
            int nMaxUseNum = 0;
            // heroInfoList数据是按使用次数从大向小排序的
            foreach (var item in heroInfoList)
            {
                if (item.nHeroID == 0)
                    return;
                    
                // 确定最大使用次数
                if (bIsFirst)
                {
                    bIsFirst = false;
                    nMaxUseNum = item.nGameCount;
                }

                // 创建预制体并进行赋值显示
                GameObject inst = heroIns.InstanceMainRes();
                if (inst == null)
                    continue;
                inst.transform.SetParent(heroGroup.transform, false);

                HeroInfo heroInfo = inst.GetComponent<HeroInfo>();
                if (heroInfo == null)
                    continue;
                heroInfo.setRequestInfo(bIsSelf, nPDBID);
                heroInfo.setHeroInfo(item, nMaxUseNum);
            } 
        }
        
        // 计算英雄种类使用场次
        private void HeroTypeNumCount(List<cmd_entity_update_warstatistic_total_warinfo> warInfo, ref Dictionary<HeroType, int> dict)
        {
            dict.Clear();
            for (int i = 0; i < (int)HeroType.HEROTYPE_ALL; i++)
            {
                dict[(HeroType)i] = 0;
            }

            foreach (var item in warInfo)
            {
                int nHeroType = SelectArrayTipPage.Instance.GetHeroType(item.nHeroID);
                if (nHeroType < 0 || nHeroType > 4)
                    continue;
                    
                dict[(HeroType)nHeroType]++;
            }
        }
    }
}
