/*******************************************************************
** 文件名:	UWarRecordTableEnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016-3-31
** 版  本:	1.0
** 描  述:	战后战绩表显示控制界面
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using DataCenter;
using War;

namespace USpeedUI.WarRecordTableEnd
{
    public class UWarRecordTableEnd : UIFillWnd<UViewWarRecordTableEnd>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_RECORDTABLE_END;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "DynamicState_Moba/WarRecordTable_End/WarRecordTable_End";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FILL;
        }

        public override int GetLoadMask()
        {
            return (int)WndLoadMaskDef.WND_LoadMask_Moba;
        }

        // 初始化
        public override bool Init()
        {
            m_bHaveEscCallBack = true;
            m_isKeyClose = true;

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            base.Init();
            return true;
        }

        private void AddListener()
        {
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_BASEFRAME, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_INITAWAKEINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
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
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_SHOW, this);
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
                case WndMsgID.WND_MSG_WAR_RECORDTABLE_END_SHOW:
                    {
                        if (m_wndView == null)
                        {
                            LoadView();
                        }
                        SetVisible(true);

                        // 设置结算界面基本信息
                        SetRecordTableBaseInfo();

                        // 显示杯赛信息
                        ShowWarEndLegendCupInfo();

                        // 自动炫耀战绩
                        CheckAutoSendWarResult();
                    }
                    break;

                //case WndMsgID.WND_MSG_WAR_RECORDTABLE_END_BASEFRAME:
                //    {
                //        if (m_wndView == null)
                //        {
                //            LoadView();
                //        }
                //        SetRecordTableBaseInfo();
                //    }
                //    break;
                //case WndMsgID.WND_MSG_WAR_RECORDTABLE_END_INITAWAKEINFO:
                //    {
                //        if (m_wndView == null)
                //        {
                //            LoadView();
                //        }
                //        m_wndView.initAwakeInfo();
                //        CheckAutoSendWarResult();
                //    }
                //    break;

                case WndMsgID.WND_MSG_RECORDTABLE_END_EXITBTN_ADDGUIDEWIDGET:
                    {
                        if (m_wndView != null)
                        {
                            UGuideWidgetMsgData cmdData = (UGuideWidgetMsgData)msgData;
                            if (cmdData != null)
                            {
                                m_wndView.AddExitBtnGuideUIWidget(cmdData);
                            }
                        }

                    }
                    break;
                case WndMsgID.WND_MSG_RECORDTABLE_END_EXITBTN_REMOVEGUIDEWIDGET:
                    {
                        if (m_wndView != null)
                        {
                            UGuideWidgetMsgData cmdData = (UGuideWidgetMsgData)msgData;
                            if (cmdData != null)
                            {
                                m_wndView.RemoveExitBtnGuideUIWidget(cmdData);
                            }
                        }
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

        public void ShowWarEndLegendCupInfo()
        {
            LogicDataCenter.warInfoDataManager.showLegendCupMsgInfo();
            
        }

        // 设置框架基本信息
        protected void SetRecordTableBaseInfo()
        {
            // 计算最佳数据
            int nScoreTop = 0, nMoneyTop = 0, nKillTop = 0, nDieTop = 0, nAssistTop = 0;
            LogicDataCenter.recordTableDataManager.getDataTop(ref nScoreTop, ref nMoneyTop, ref nKillTop, ref nDieTop, ref nAssistTop);
            int[] gzAllDataTop = LogicDataCenter.recordTableDataManager.getDataTopEx();

            // 设置玩家数据
            cmd_entity_end_player_data selfData = new cmd_entity_end_player_data();
            Dictionary<ulong, WarRecordTableDataManager.swar_flaunt_result> playerDataDic = LogicDataCenter.recordTableDataManager.getWarEndPlayerData();
            foreach (var item in playerDataDic)
            {
                if (item.Value.data.bIsSelf > 0)
                {
                    selfData = item.Value.data;
                }

                // 设置数据
                m_wndView.setVSInfoPlayerInfo(item.Value.data, nScoreTop, nMoneyTop, nKillTop, nDieTop, nAssistTop);
                m_wndView.setDataGridPlayerInfo(item.Value.data, gzAllDataTop);
                m_wndView.setDataChartPlayerInfo(item.Value.data);
            }

            if(StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_NORMAL)
            {
                // 设置奖励信息
                m_wndView.setAwardInfoData();
            }

            // 设置数据网格默认选项
            m_wndView.setRecordTable3DefualtToggle();

            // 显示玩家增益卡信息
            m_wndView.displayBenefitCardInfo();

            // 设置击杀个数
            m_wndView.setKillNum();

            // 设置排位信息
            m_wndView.setRankInfo(selfData);
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
            if(!bCanMilitaryExploits)
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

        public override bool OnEscCallBack()
        {
            if (m_wndView != null)
                m_wndView.onChickReturnButton();

            return base.OnEscCallBack();
        }
    }
}
