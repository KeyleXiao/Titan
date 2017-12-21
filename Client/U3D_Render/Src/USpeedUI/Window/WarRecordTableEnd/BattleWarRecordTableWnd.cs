/*******************************************************************
** 文件名:	BattleWarRecordTableWnd.cs
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	   李有红
** 日  期:	2017/11/10
** 版  本:	1.0
** 描  述:	大逃杀战场结算界面
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using ASpeedGame.Data.Scheme;
using ASpeedGame.Data.WarStatisticMap;
using DataCenter;
using GameLogic;
using System;
using System.Collections.Generic;
using U3D_Render;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UWidgets;
using War;

namespace USpeedUI
{
    public class BattleWarRecordTableWnd : UIFillWnd<BattleWarRecordTableView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_BATTLEWAR_RECORDTABLE;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FILL;
        }

        public override string GetPrefabPath()
        {
            return "DynamicState_Moba/WarRecordTable_End/BattleWarRecordTable";
        }

        public override int GetLoadMask()
        {
            return (int)WndLoadMaskDef.WND_LoadMask_MCAMP;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();
            m_bHaveEscCallBack = true;
            m_isKeyClose = true;

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);

            return true;
        }

        private void AddListener()
        {
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_BASEFRAME, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_INITAWAKEINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);

            if (_bVisible)
            {
                // 设置结算界面基本信息
                SetRecordTableBaseInfo();

                // 自动炫耀战绩
                CheckAutoSendWarResult();

                m_wndView.onVisible(_bVisible);
            }
        }

        // 销毁
        public override void Destroy()
        {
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            base.Destroy();
        }

        private void RemoveListener()
        {
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_BASEFRAME, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_INITAWAKEINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
        }

        // 接收消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        AddListener();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        RemoveListener();
                        if (m_wndView != null)
                        {
                            UnloadView();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_WAREND:
                    {
                        UCommonWarEndMsgData uiInfo = (UCommonWarEndMsgData)msgData;
                        if (m_wndView == null)
                        {
                            LoadView();
                        }

                        SetVisible(false);

                        m_wndView.setResult(uiInfo.isSelfWin, uiInfo.nUseTime);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        RemoveListener();
                        if (m_wndView != null)
                        {
                            UnloadView();
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        // 设置框架基本信息
        private void SetRecordTableBaseInfo()
        {
            if (m_wndView == null)
                return;

            // 计算最佳数据
            int nScoreTop = 0, nMoneyTop = 0, nKillTop = 0, nDieTop = 0, nAssistTop = 0;
            LogicDataCenter.recordTableDataManager.getDataTop(ref nScoreTop, ref nMoneyTop, ref nKillTop, ref nDieTop, ref nAssistTop);

            Dictionary<ulong, WarRecordTableDataManager.swar_flaunt_result> playerDataDic = LogicDataCenter.recordTableDataManager.getWarEndPlayerData();
            Dictionary<ulong, WarRecordTableDataManager.WarPersonInfo> dicPersonInfo = LogicDataCenter.recordTableDataManager.recordInfoArry;
            WarRecordTableDataManager.WarPersonInfo mainPlayerInfo = dicPersonInfo[(ulong)LogicDataCenter.playerSystemDataManager.SelfAcotrID];
            foreach (KeyValuePair<ulong, WarRecordTableDataManager.WarPersonInfo> kvp in dicPersonInfo)
            {
                WarRecordTableDataManager.WarPersonInfo personInfo = kvp.Value;
                if (mainPlayerInfo.nCamp == personInfo.nCamp)
                {
                    cmd_entity_end_player_data data = default(cmd_entity_end_player_data);
                    WarRecordTableDataManager.swar_flaunt_result flaunt;
                    if (playerDataDic.TryGetValue(personInfo.uPDBID, out flaunt))
                    {
                        data = flaunt.data;
                    }
                    else
                    {
                        data.bIsSelf = kvp.Value.uPDBID == mainPlayerInfo.uPDBID ? 1 : 0;
                        data.gzAllData = new int[(int)ERecord_Data_Type.ERDT_MAX];
                        data.gzAllData[(int)ERecord_Data_Type.ERDT_MONEY] = personInfo.nMobaWarMoney;
                        data.gzAllData[(int)ERecord_Data_Type.ERDT_KILLNUM] = personInfo.nKillNum;
                        data.gzAllData[(int)ERecord_Data_Type.ERDT_DEADNUM] = personInfo.nKillNum;
                        data.gzAllData[(int)ERecord_Data_Type.ERDT_ASSIST] = personInfo.nAssists;
                        data.nCamp = personInfo.nCamp;
                        data.nIconId = personInfo.nHeroId;
                        data.nPlayerType = personInfo.nPlayerType;
                        data.pDbid = (uint)personInfo.uPDBID;
                        data.szPlayerName = personInfo.playerName;
                    }

                    // 设置数据
                    m_wndView.setVSInfoPlayerInfo(data, nScoreTop, nMoneyTop, nKillTop, nDieTop, nAssistTop);
                }
            }

            // 设置奖励信息
            m_wndView.setAwardInfoData();

            // 设置排名信息
            m_wndView.SetRankSurvial();
        }

        // 检测是否自动发送战绩
        void CheckAutoSendWarResult()
        {
            if (EntityFactory.MainHeroView == null)
                return;

            int nClanID = EntityUtil.getMainHeroClanID();
            int nKinID = EntityUtil.getMainHeroKinID();
            if (nClanID == 0 && nKinID == 0)
                return;

            var playerDataDic = LogicDataCenter.recordTableDataManager.getWarEndPlayerData();

            bool bCanMilitaryExploits = CanAutoMilitaryExploits(playerDataDic);
            if (!bCanMilitaryExploits)
            {
                return;
            }

            foreach (var kvp in playerDataDic)
            {
                if (kvp.Value.data.bIsSelf > 0)
                {
                    int nMultKillMax = kvp.Value.data.gzAllData[(int)ERecord_Data_Type.ERDT_TIMEKILLMAX];
                    int nContinueKillMax = kvp.Value.data.gzAllData[(int)ERecord_Data_Type.ERDT_ALIVECONTKILLMAX];
                    if (nMultKillMax >= 4 || nContinueKillMax >= 8)
                    {
                        int nGlory = kvp.Value.data.nTitle;
                        String strWarResultName = LogicDataCenter.warStatisticDataManager.GetWarResultItemName(nGlory, nMultKillMax, nContinueKillMax);
                        long lWarID = LogicDataCenter.recordTableDataManager.getPlayerAwake().ulWarID;
                        uint nPDBID = kvp.Value.data.pDbid;
                        String strPrefix = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowDescPrefix");
                        String strMessage = String.Format("{0}, {1}[warresult color=235+80+30 id={2} war={3}]{4}[/warresult]", strPrefix, ChatMessageManager.ChatItemFlag, nPDBID, lWarID, strWarResultName);
                        if (nClanID > 0)
                            UIUtil.SendChatMessage(EMChatChannelType.CHAT_CHANNEL_CLAN, strMessage);
                        else if (nKinID > 0)
                            UIUtil.SendChatMessage(EMChatChannelType.CHAT_CHANNEL_KIN, strMessage);
                    }

                    return;
                }
            }
        }

        private bool CanAutoMilitaryExploits(Dictionary<ulong, WarRecordTableDataManager.swar_flaunt_result> actorInfo)
        {
            int nAIMilitaryExploitsFlag = GameLogicAPI.getAIMilitaryExploitsFlag();
            if (nAIMilitaryExploitsFlag > 0)
            {
                return true;
            }

            bool bHasMachine = false;
            foreach (WarRecordTableDataManager.swar_flaunt_result item in actorInfo.Values)
            {
                if (item.data.nPlayerType == (int)EPlayerType.EWPT_COMPUTER || item.data.nPlayerType == (int)EPlayerType.EWPT_VIRTUALPEOPLE)
                {
                    bHasMachine = true;
                    break;
                }
            }

            return !bHasMachine;
        }


    }

    public class BattleWarRecordTableView : UIBaseWndView, ITimerHanlder
    {
        public Text MatchTimeText;
        public UVSInfoPlayerInfo[] VSInfoPlayerInfo = new UVSInfoPlayerInfo[2];
        public GameObject showBtn;
        public Text showBtnLabel;
        public Text exitBtnLabel;
        public Text GetExp;
        public Text GetGold;
        public Text RankSurvial;

        public UShareBar ShareBar;
        public RectTransform ShareContentRt;

        public Text CountDownExitText;
        private int nCountDownExit = 0;

        private readonly string shareImgName = "battleWarRecordEndTemp.jpg";
        private string shareImgPath;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            showBtnLabel.text = ULocalizationService.Instance.Get("UIView", "WarRecordTableEnd", "ShowBtnText");
            exitBtnLabel.text = ULocalizationService.Instance.Get("UIView", "WarRecordTableEnd", "ExitBtnText");

            foreach (UVSInfoPlayerInfo item in VSInfoPlayerInfo)
            {
                item.initVSPlayerInfo();
                item.gameObject.SetActive(true);
            }

            initRecordShare();

            // 倒计时退出战场
            nCountDownExit = 60;

            return true;
        }

        internal override void SetRendererVisible(bool _bVisible)
        {
            base.SetRendererVisible(_bVisible);
            if (_bVisible)
            {
                #region recordShare
                CaptureScreen.GetInstance().CaptureAreaImage(new CaptureAreaImgData(shareImgPath, GetRecordRect()));
                #endregion
            }
        }

        // 设置奖励信息
        public void setAwardInfoData()
        {
            cmd_entity_return_player_awark cmdData = LogicDataCenter.recordTableDataManager.getPlayerAwake();
            if (cmdData.nActorGetExp > 0)
            {
                GetExp.gameObject.SetActive(true);
                GetExp.text = "+" + cmdData.nActorGetExp.ToString();
            }
            else
            {
                GetExp.gameObject.SetActive(false);
            }

            // 防沉迷衰减
            if (cmdData.nAntiReduceRate < 100)
            {
                cmdData.nGold = cmdData.nGold * cmdData.nAntiReduceRate / 100;
            }

            if (cmdData.nGold > 0)
            {
                GetGold.gameObject.SetActive(true);
                GetGold.text = String.Format("+{0}", cmdData.nGold);
            }
            else
            {
                GetGold.gameObject.SetActive(false);
            }
        }

        public void SetRankSurvial()
        {
            Dictionary<ulong, WarRecordTableDataManager.WarPersonInfo> recordInfoArry = LogicDataCenter.recordTableDataManager.recordInfoArry;
            int survial = 0;
            if (recordInfoArry != null)
            {
                survial = recordInfoArry.Count;
                foreach (var item in recordInfoArry.Values)
                {
                    survial -= (item.nDeadNum > 0 ? 1 : 0);
                }
            }

            RankSurvial.text = string.Format("队伍排名：第{0}名  剩余存活：{1}人", LogicDataCenter.recordTableDataManager.SelfCampACE.nRank, survial);
        }

        // 设置单个玩家的对战信息
        public void setVSInfoPlayerInfo(cmd_entity_end_player_data cmdData, int nScoreTop, int nMoneyTop, int nKillTop, int nDieTop, int nAssTop)
        {
            
            ulong uPDBID = cmdData.pDbid;

            int i = cmdData.bIsSelf > 0 ? 0 : 1;
            if (i < this.VSInfoPlayerInfo.Length && this.VSInfoPlayerInfo[i] != null)
            {
                this.VSInfoPlayerInfo[i].setPlayerInfo(cmdData, nScoreTop, nMoneyTop, nKillTop, nDieTop, nAssTop);
            }
        }


        // 设置最后的比赛结果
        public void setResult(bool isSelfWin, ulong uTime)
        {
            // 比赛时长
            int nMin = (int)(uTime / 60);
            int nMs = (int)(uTime % 60);
            string strMinDesc = ULocalizationService.Instance.Get("UIView", "Common", "Minute");
            string strSecDesc = ULocalizationService.Instance.Get("UIView", "Common", "SecondText");
            MatchTimeText.text = String.Format("{0}{1}{2}{3}", nMin, strMinDesc, nMs, strSecDesc);
        }

        // 当点击取消按钮时退出战场进入静态场景
        public void onChickReturnButton()
        {
            TimerManager.KillTimer(this, 0);

            if (StageManager.Instance.CurSubState == EStage_SubState.ESSS_WAREND)
            {
                // 战场结束返回主城
                onGameOverReturn();
            }
            else
            {
                // 战场中返回主城
                onReqExitWar();
            }
        }

        // 战场结束最后胜利队伍退出战场进入主城
        public void onGameOverReturn()
        {
            Debug.Log("onGameOverReturn");
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_USER_BACK_GAME);
            //EntityEventHelper.Instance.SendCommand(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQ_EXIT_WAR);

            SoldierCamera.MainInstance<SoldierCamera>().LeaveCurMode();
            // 隐藏聊天框
            ChatForceConditionHide ChatData = new ChatForceConditionHide();
            ChatData.bForceHide = true;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_CONDITIONHIDE, ChatData);
            SetVisible(false);
        }

        // 游戏中途退出战场进入主城
        public void onReqExitWar()
        {
            Debug.Log("onReqExitWar");
            SoundPlayHandler.StopSceneSound();
            //ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_USER_BACK_GAME);
            EntityEventHelper.Instance.SendCommand(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQ_EXIT_WAR);

            // 隐藏聊天框
            ChatForceConditionHide ChatData = new ChatForceConditionHide();
            ChatData.bForceHide = true;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_CONDITIONHIDE, ChatData);
        }

        // 当点击炫耀时将自己的对战信息发送到聊天框中
        public void onClickFlauntButton()
        {
            EntityView objMaster = EntityFactory.getMainPlayer();
            if (objMaster == null)
            {
                Debug.LogError(" objMaster == NULL ");
                return;
            }
            ulong pDBID = (ulong)objMaster.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            Dictionary<ulong, WarRecordTableDataManager.swar_flaunt_result> dicCmdData = LogicDataCenter.recordTableDataManager.getWarEndPlayerData();
            int nGlory = 0;
            int nMultKillMax = 0;
            int nContinueKillMax = 0;
            Int64 lWarID = 0;
            if (dicCmdData.ContainsKey(pDBID))
            {
                cmd_entity_end_player_data cmdData = dicCmdData[pDBID].data;
                nGlory = cmdData.nTitle;
                nMultKillMax = cmdData.gzAllData[(int)ERecord_Data_Type.ERDT_TIMEKILLMAX];
                nContinueKillMax = cmdData.gzAllData[(int)ERecord_Data_Type.ERDT_ALIVECONTKILLMAX];
                lWarID = dicCmdData[pDBID].dbWarID;
                LogicDataCenter.warStatisticDataManager.ShowWarResult((int)pDBID, lWarID, nGlory, nMultKillMax, nContinueKillMax);
            }
        }


        public override void Destroy()
        {
            // 销毁对战信息表
            destroyVSInfoPlayerFrame();

            // 销毁RecordShare
            destroyRecordShare();

            base.Destroy();
        }

        void destroyVSInfoPlayerFrame()
        {
            foreach (UVSInfoPlayerInfo item in VSInfoPlayerInfo)
            {
                item.ResDestroy();
            }
        }

        private void initRecordShare()
        {
            shareImgPath = "";

            int nActorID = 0;
            if (null != EntityFactory.MainHeroView && null != EntityFactory.MainHeroView.Property)
            {
                nActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            }

            shareImgPath = nActorID + shareImgName;

            ShareBar.SetData(SocialSharingType.WarRecord, CaptureScreen.GetInstance().CaptureTempDir + "/" + shareImgPath);
        }

        private void destroyRecordShare()
        {
            shareImgPath = "";
        }

        private Rect GetRecordRect()
        {
            Vector3[] worldCorners = new Vector3[4];

            ShareContentRt.GetWorldCorners(worldCorners);

            Vector3 corner0 = UISystem.Instance.GetCamera().WorldToScreenPoint(worldCorners[0]);
            Vector3 corner1 = UISystem.Instance.GetCamera().WorldToScreenPoint(worldCorners[1]);
            Vector3 corner3 = UISystem.Instance.GetCamera().WorldToScreenPoint(worldCorners[3]);

            return new Rect(corner0.x, corner0.y, corner3.x - corner0.x, corner1.y - corner0.y);
        }

        public void OnTimer(int nTimerID)
        {
            if(nTimerID == 0)
            {
                nCountDownExit--;
                if(nCountDownExit > 0)
                {
                    if(CountDownExitText != null)
                    {
                        CountDownExitText.text = nCountDownExit.ToString() + "秒后退出战场";
                    }
                }
                else
                {
                    onChickReturnButton();
                }
            }
        }

        public void onVisible(bool bVisible)
        {
            if(bVisible)
            {
                TimerManager.SetTimer(this, 0, 1f);
            }
        }
    }
}
