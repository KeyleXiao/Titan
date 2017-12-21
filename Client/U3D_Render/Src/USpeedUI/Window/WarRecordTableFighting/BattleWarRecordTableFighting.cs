/*******************************************************************
** 文件名:	BattleWarRecordTableFighting.cs
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	王全申
** 日  期:	2017/11/17
** 版  本:	1.0
** 描  述:	大逃杀战场TAB界面
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


using ASpeedGame.Data.GameMobaSchemes;
using DataCenter;
using GameLogic;
using Stage_ManagerDef;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;
using War;

namespace USpeedUI.WarRecordTableWarning
{
    public class RecordTableFightingWnd : UIPopupWnd<RecordTableFightingWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_BATTLEWAR_RECORDTABLE_FIGHTING;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "DynamicState_MCAMP/WarRecordTable/MCAMP_RecordTableFighting";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        public override int GetLoadMask()
        {
            return (int)WndLoadMaskDef.WND_LoadMask_MCAMP;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;

            InputManager.Available = true;

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_SETPLAYERINFOINIT, this);

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_BOSSCDTIME, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_UPDATERECORDINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_SWITCH_WND, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_UPDATE_HEROICON, this);

            // 订阅按键消息
            UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.WarRecordInfoPannel, KeyStatus.Up, this);
            UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.WarRecordInfoPannel, KeyStatus.Down, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_SETPLAYERINFOINIT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_BOSSCDTIME, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_UPDATERECORDINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_SWITCH_WND, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_UPDATE_HEROICON, this);


            // 退订按键消息
            UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.WarRecordInfoPannel, KeyStatus.Up, this);
            UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.WarRecordInfoPannel, KeyStatus.Down, this);
            base.Destroy();
        }

        // 接收消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            saveCacheUIMsg(msgID, msgData);

            switch (msgID)
            {
                case WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_SETPLAYERINFOINIT:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }
                        m_wndView.initWarRecordTable();
                    }
                    break;

                //case WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_BOSSCDTIME:
                //    {
                //        BossCdTime uiInfo = (BossCdTime)msgData;
                //        if (m_wndView == null)
                //        {
                //            return;
                //        }
                //        m_wndView.SetTeamBossReliveTime(uiInfo.nIndex, uiInfo.nTime);
                //    }
                //    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                case WndMsgID.WND_MSG_COMMON_WAREND:
                    {
                        if (m_wndView != null)
                        {
                            this.UnloadView();
                        }
                        m_CacheUIMsgList.Clear();
                    }
                    break;

                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        if (m_wndView == null)
                        {
                            this.LoadView();
                        }

                        // 运行缓存的消息
                        runCacheUIMsg();
                    }
                    break;
                case WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_UPDATERECORDINFO:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }
                        m_wndView.showRecordTable();
                    }
                    break;
                case WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME:
                    {
                        if (m_wndView == null)
                            return;

                        UIWarShowReliveTimeData data = msgData as UIWarShowReliveTimeData;
                        m_wndView.setHeroDieCountdown((ulong)data.pdbid, data.nReliveTime);
                    }
                    break;
                case WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_SWITCH_WND:
                    {
                        switchTabWnd();
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_UPDATE_HEROICON:
                    {
                        if (m_wndView == null)
                            return;

                        UIWarUpdateHeroIcon data = msgData as UIWarUpdateHeroIcon;
                        m_wndView.updateHeroIcon(data);
                    }
                    break;
                default:
                    break;
            }
        }

        // 接受按键消息
        public override bool OnKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status)
        {
            if (m_wndView == null || StageManager.Instance.getCurrentState() != EStage_State.ESS_BATTLE)
            {
                return false;
            }

            if (index == FuntionShortCutSlotIndex.WarRecordInfoPannel)
            {
                if (status == KeyStatus.Down)
                {
                    openTabWnd();
                }
                else if (status == KeyStatus.Up)
                {
                    closeTabWnd();
                }
            }

            return base.OnKeyMessage(index, status);
        }

        private void switchTabWnd()
        {
            if (m_isVisible)
            {
                closeTabWnd();
            }
            else
            {
                openTabWnd();
            }
        }

        private void openTabWnd()
        {
            // 如果是第一次请求的话先请求一次战场数据
            if (LogicDataCenter.recordTableDataManager.isOneRequest)
                LogicDataCenter.recordTableDataManager.OnTabClick();
            LogicDataCenter.recordTableDataManager.IsWarRecordTableVisible = true;
            SetVisible(true);

            notifyTabWndStatus();
        }

        private void closeTabWnd()
        {
            LogicDataCenter.recordTableDataManager.IsWarRecordTableVisible = false;
            SetVisible(false);

            notifyTabWndStatus();
        }

        private void notifyTabWndStatus()
        {
            // 发向逻辑层是否判断战绩界面是否打开
            int nIsOpen = Convert.ToInt16(LogicDataCenter.recordTableDataManager.IsWarRecordTableVisible);
            IntPtrHelper helper = new IntPtrHelper();
            IntPtr ptr = helper.toPtr<int>(ref nIsOpen);
            int len = Marshal.SizeOf(nIsOpen);
            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_PALYER_TAB_ISOPEN, len, "", ptr, len);
            m_wndView.showRecordTable();
        }

    }

    public class RecordTableFightingWndView : UIBaseWndView
    {
        public Button ShowTalentBtn;
        public Button ShowEquipBtn;
        public BattleWarPlayerRecordInfo[] playerRecordInfo = new BattleWarPlayerRecordInfo[2];

        private Dictionary<ulong, BattleWarPlayerRecordInfo> playerRecordInfoList = new Dictionary<ulong, BattleWarPlayerRecordInfo>();

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            return true;
        }


        // 初始化战绩表
        public void initWarRecordTable()
        {
            foreach (BattleWarPlayerRecordInfo recInfo in playerRecordInfo)
            {
                recInfo.resetPlayerInfo();
            }

            Dictionary<ulong, WarRecordTableDataManager.WarPersonInfo> dicPersonInfo = LogicDataCenter.recordTableDataManager.recordInfoArry;
            WarRecordTableDataManager.WarPersonInfo mainPlayerInfo = dicPersonInfo[(ulong)LogicDataCenter.playerSystemDataManager.SelfAcotrID];

            int i = 0;
            foreach (KeyValuePair<ulong, WarRecordTableDataManager.WarPersonInfo> kvp in dicPersonInfo)
            {
                if (mainPlayerInfo.nCamp == kvp.Value.nCamp)
                {
                    playerRecordInfo[i].initPlayerInfo(kvp.Value);
                    playerRecordInfoList[kvp.Key] = playerRecordInfo[i];
                    i++;
                }
            }
        }

        // 设置英雄死亡复活时间
        public void setHeroDieCountdown(ulong uPdbid, int nReliveTime)
        {
            //int nIndex = LogicDataCenter.recordTableDataManager.getIndexAcordPDBID(uPdbid);
            //if (nIndex < 0 || nIndex > playerRecordInfo.Length)
            //{
            //    return;
            //}

            //playerRecordInfo[nIndex].setHeroDieCountdown(nReliveTime);
        }

        // 显示战绩表s
        public void showRecordTable()
        {
            Dictionary<ulong, WarRecordTableDataManager.WarPersonInfo> dicPersonInfo = LogicDataCenter.recordTableDataManager.recordInfoArry;
            if (dicPersonInfo == null)
                return;

            int nScoreTop = 0;
            int nMoneyTop = 0;
            int nKillTop = 0;
            int nDieTop = 0;
            int nAssistTop = 0;
            // 设置自己
            LogicDataCenter.recordTableDataManager.getDataTop(ref nScoreTop, ref nMoneyTop, ref nKillTop, ref nDieTop, ref nAssistTop);

            //WarRecordTableDataManager.WarPersonInfo mainPlayerInfo = dicPersonInfo[(ulong)LogicDataCenter.playerSystemDataManager.SelfAcotrID];
            //playerRecordInfo[0].setPlayerInfo(mainPlayerInfo, nMoneyTop, nKillTop, nDieTop, nAssistTop, true);

            // 设置队友
            //foreach (KeyValuePair<ulong, WarRecordTableDataManager.WarPersonInfo> kvp in dicPersonInfo)
            //{
            //    if (mainPlayerInfo.nID != kvp.Value.nID && mainPlayerInfo.nCamp == kvp.Value.nCamp)
            //    {
            //        playerRecordInfo[1].setPlayerInfo(kvp.Value, nMoneyTop, nKillTop, nDieTop, nAssistTop, true);
            //        break;
            //    }
            //}

            foreach(var item in playerRecordInfoList)
            {
                WarRecordTableDataManager.WarPersonInfo playerInfo = LogicDataCenter.recordTableDataManager.getWarPersonInfoByPDBID(item.Key);
                if(playerInfo != null)
                {
                    item.Value.setPlayerInfo(playerInfo, nMoneyTop, nKillTop, nDieTop, nAssistTop, true);
                }
            }
        }

        
        // 显示天赋
        public void showTalent()
        {
            ShowEquipBtn.gameObject.SetActive(true);
            ShowTalentBtn.gameObject.SetActive(false);

            foreach (var item in playerRecordInfo)
            {
                item.showTalent();
            }
        }

        // 显示装备
        public void showEquip()
        {
            ShowEquipBtn.gameObject.SetActive(false);
            ShowTalentBtn.gameObject.SetActive(true);

            foreach (var item in playerRecordInfo)
            {
                item.showEquip();
            }
        }

        // 格式化时间
        private string formatTime(int nSeconds)
        {
            if (nSeconds < 60)
                return "" + nSeconds + "秒";
            else
                return String.Format("{0}:{1}", (nSeconds / 60).ToString("D2"), (nSeconds % 60).ToString("D2"));
        }

        public void updateHeroIcon(UIWarUpdateHeroIcon data)
        {
            BattleWarPlayerRecordInfo playerRecordInfo = null;
            if(playerRecordInfoList.TryGetValue(data.nPdbid, out playerRecordInfo))
            {
                WarRecordTableDataManager.WarPersonInfo playerInfo = LogicDataCenter.recordTableDataManager.getWarPersonInfoByPDBID(data.nPdbid);
                if (playerInfo != null)
                {
                    playerRecordInfo.updateHeroIcon(data.nHeroID, data.nSkinID);
                }
            }
        }
    }

    public class BattleWarPlayerRecordInfo : TimeCountDown
    {
        public Image heroHead;
        public Text playerName;
        public Text heroLv;
        public Text playerState;
        public Text playerGold;
        public Image playerBackGround;
        public Text Countdown;
        public WarRecordSummerSkillFrame SummerFrame;
        public GameObject Frame;
        public GameObject goodsGroup;
        public GameObject talentGroup;
        public Image SexImage;

        private bool m_isInit = false;

        private UTooltipTrigger heroNameTipsTrigger;
        EquipFrame[] goodsFrame;
        TalentFrame[] talentFrame;

        public void Awake()
        {
            heroNameTipsTrigger = heroHead.GetComponent<UTooltipTrigger>();
        }

        public void Destroy()
        {

        }

        protected override void CountDownRepeatCallBack()
        {
            base.CountDownRepeatCallBack();
            Countdown.text = ((int)fCurTime).ToString();
        }

        protected override void CountDownCompleteCallBack()
        {
            base.CountDownCompleteCallBack();

            heroHead.color = Color.white;
            Countdown.gameObject.SetActive(false);
        }

        // 初始化玩家信息
        public void initPlayerInfo(WarRecordTableDataManager.WarPersonInfo personInfo)
        {
            if (SummerFrame == null || heroHead == null || heroLv == null || playerState == null || playerGold == null || playerBackGround == null || Countdown == null)
            {
                return;
            }

            if (personInfo.uPDBID > 0)
                gameObject.SetActive(true);


            // 初始化玩家数据
            SummerFrame.SetSummerInfo(personInfo.sSummerDesc, personInfo.nSummerIconID);
            heroHead.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_RECORDTABLE_FIGHTING, 1, 6, personInfo.nHeroId);

            playerName.text = "";
            playerState.text = "0/0/0";
            playerGold.text = "0";
            heroLv.text = "1";
            SexImage.gameObject.SetActive(false);



            if (heroNameTipsTrigger == null)
            {
                heroNameTipsTrigger = heroHead.GetComponent<UTooltipTrigger>();
            }
            heroNameTipsTrigger.SetText(UTooltipParamName.BodyText, personInfo.playerName);

            //if (!playerBackGround.gameObject.activeSelf && personInfo.bIsHero != 0)
            //{
            //    playerBackGround.gameObject.SetActive(true);
            //}

            // 初始化装备格子
            goodsFrame = new EquipFrame[(int)SLOT_DEFINE.D_MAX_GOODS_SLOT_COUNT];
            for (int i = 0; i < UDefines.WAR_ITEM_GRID_COUNT; ++i)
            {
                goodsFrame[i] = goodsGroup.transform.FindChild("Equip" + i).GetComponent<EquipFrame>();
            }

            // 初始化天赋格子
            talentFrame = new TalentFrame[(int)UDefines.WAR_LEARN_TALENT_COUNT];
            for (int i = 0; i < UDefines.WAR_LEARN_TALENT_COUNT; ++i)
            {
                talentFrame[i] = talentGroup.transform.FindChild("Talent" + i).GetComponent<TalentFrame>();
            }

            m_isInit = true;
        }

        public void resetPlayerInfo()
        {
            // 初始化玩家数据
            gameObject.SetActive(false);
            heroHead.sprite = null;
            playerState.text = "0/0/0";
            playerGold.text = "0";
            heroLv.text = "1";
            playerName.text = "";
            if (heroNameTipsTrigger == null)
            {
                heroNameTipsTrigger = heroHead.GetComponent<UTooltipTrigger>();
            }
            heroNameTipsTrigger.SetText(UTooltipParamName.BodyText, "");
            //playerBackGround.gameObject.SetActive(false);
            m_isInit = false;
        }

        // 设置玩家信息
        public void setPlayerInfo(WarRecordTableDataManager.WarPersonInfo personInfo, int nMoneyTop, int nKillTop, int nDieTop, int nAssTop, bool isSelfCamp)
        {
            if (!m_isInit)
            {
                Debug.LogError("There is not init player info.");
                return;
            }

            // 设置玩家信息
            heroLv.text = personInfo.nLevel.ToString();

            if (isSelfCamp)
            {
                playerName.text = "<color=#9CDBFF>" + personInfo.playerName + "</color>";
            }
            else
            {
                playerName.text = "<color=#E95A5A>" + personInfo.playerName + "</color>";
            }

            String strKillState = "";
            if (nKillTop != 0 && personInfo.nKillNum == nKillTop)
                strKillState += "<color=#FF7800>" + personInfo.nKillNum + "</color>";
            else
                strKillState += personInfo.nKillNum.ToString();
            strKillState += "/";

            if (nDieTop != 0 && personInfo.nDeadNum == nDieTop)
                strKillState += "<color=#FF6E2E>" + personInfo.nDeadNum + "</color>";
            else
                strKillState += personInfo.nDeadNum.ToString();
            strKillState += "/";

            if (nAssTop != 0 && personInfo.nAssists == nAssTop)
                strKillState += "<color=#FF6E2E>" + personInfo.nAssists + "</color>";
            else
                strKillState += personInfo.nAssists.ToString();

            playerState.text = strKillState;

            //playerGold.text = personInfo.nMobaWarMoney.ToString();

            if (personInfo.nMobaWarMoney >= 10000)
            {
                float fAllGold = (personInfo.nMobaWarMoney) / 10000.0f;
                playerGold.text = fAllGold.ToString("0.00") + ULocalizationService.Instance.Get("UIView", "Common", "TenThousand");
            }
            else
            {
                playerGold.text = personInfo.nMobaWarMoney.ToString();
            }

            Color clr;
            if (personInfo.nMobaWarMoney == nMoneyTop)
                ColorUtility.TryParseHtmlString("#FF6E2E", out clr);
            else
                ColorUtility.TryParseHtmlString("#FFd52E", out clr);
            this.playerGold.color = clr;


            // 性别显示
            if (personInfo.nPlayerType == (int)EPlayerType.EWPT_COMPUTER)
            {
                SexImage.gameObject.SetActive(false);
            }
            else //if(personInfo.nPlayerType == (int)EPlayerType.EWPT_REALPEOPLE)
            {
                SexImage.gameObject.SetActive(true);
                WarRecordTableDataManager.WarPlayerVsInfo playerInfo = LogicDataCenter.recordTableDataManager.GetWarPlayerVsInfo(personInfo.uPDBID);

                if (playerInfo != null)
                {
                    SexImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_SOCIALCONTACT, playerInfo.nSex + 1);
                }
            }

            // 设置装备信息
            SSchemeMobaGoods schemeMobaGoods = new SSchemeMobaGoods();
            ulong uPDBID = personInfo.uPDBID;
            List<cmd_war_persongoodsinfo> listEquipInfo = LogicDataCenter.warMobaGoodsDataManager.GetPlayerGoodsInfo(uPDBID);
            if (listEquipInfo != null && goodsFrame != null)
            {
                int nCount = listEquipInfo.Count;
                for (int i = 0; i < nCount; ++i)
                {
                    int nIndex = listEquipInfo[i].nSlotIndex;
                    if (nIndex >= goodsFrame.Length || goodsFrame[nIndex] == null)
                    {
                        Debug.LogWarning("nIndex >= goodsFrame.Length || goodsFrame[nIndex], nIndex=" + nIndex);
                        continue;
                    }
                    LogicDataCenter.warMobaGoodsDataManager.GetMobaGoods_Out(listEquipInfo[i].nGoodID, out schemeMobaGoods);
                    goodsFrame[nIndex].setEquipInfo(listEquipInfo[i], schemeMobaGoods);
                }
            }

            int nHeroID = personInfo.nHeroId;

            // 设置天赋信息
            cmd_war_active_talent_list listTalent;
            if (LogicDataCenter.heroTalentDataManager.getActiveTalentList(uPDBID, out listTalent))
            {
                for (int i = 0; i < listTalent.nTalentList.Length; ++i)
                {
                    int nTalentID = listTalent.nTalentList[i];
                    if (nTalentID <= 0)
                        continue;

                    if (i >= talentFrame.Length || talentFrame[i] == null)
                    {
                        Debug.LogWarning("i >= talentFrame.Length || talentFrame[i] == null, i=" + i + ",length=" + talentFrame.Length);
                        continue;
                    }

                    talentFrame[i].setTalentInfo(nHeroID, nTalentID);
                }
            }
        }

        // 显示装备
        public void showEquip()
        {
            goodsGroup.SetActive(true);
            talentGroup.SetActive(false);
        }

        // 显示天赋
        public void showTalent()
        {
            goodsGroup.SetActive(false);
            talentGroup.SetActive(true);
        }

        // 设置英雄死亡倒计时
        public void setHeroDieCountdown(int nReliveTime)
        {
            if (nReliveTime != 0)
            {
                heroHead.color = Color.gray;
                Countdown.gameObject.SetActive(true);
            }
            Countdown.text = nReliveTime.ToString();
            this.StartCountDown(nReliveTime, 1.0f, 1.0f);
        }

        public void updateHeroIcon(int nHeroID, int nSkinID)
        {
            heroHead.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_RECORDTABLE_FIGHTING, 1, 6, nHeroID);
        }
    }
}
