using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using USpeedUI;
using USpeedUI.TooltipAndDialog;
using Team_ManagedDef;
using Match_ManagerDef;
using USpeedUI.SideButtonList;
using Data.SideButtonConfig;
using ASpeedGame.Data.MatchRankLimitConfig;

namespace DataCenter
{
    public class MatchDataManager : ITimerHanlder
    {
        private int m_MatchType;
        private Dictionary<byte, int> m_SelfPunishInfo;
        private Dictionary<int, bool> m_MatchTypeOpen;

        public void Init()
        {
            m_MatchType = (int)EMMatchType.MatchType_None;
            m_SelfPunishInfo = new Dictionary<byte, int>();
            m_MatchTypeOpen = new Dictionary<int,bool>();
            TimerManager.SetTimer(this, 1, 1.0f);
        }
        public void Clear()
        {
            TimerManager.KillTimer(this, 1);
        }
        public void OnTimer(int nTimerID)
        {
            // 定时器触发
            SideButtonOnTimer();
        }

        public void OnMatchModeBtnClick()
        {
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_BEGIN_MATCH, (int)EMMatchType.MatchType_Match);
        }

        public void OnRankModeBtnClick()
        {
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_BEGIN_MATCH, (int)EMMatchType.MatchType_Rank);
        }

        public void OnMachineVSModeBtnClick()
        {
            if (GameLogicAPI.getPlayerNoviceGuideTypeData() < (int)ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUIDE_TWOSTEP)
            {
                // 发送到逻辑层 引导相关记录
                cmd_guide_info guideInfo = new cmd_guide_info();
                guideInfo.nGuideID = (int)EMMatchGuideStep.GuideStep_MachineGameModel_Select;

                ViewEventHelper.Instance.SendCommand<cmd_guide_info>(GameLogicDef.GVIEWCMD_MATCH_GUIDE_INFO, ref guideInfo);

                ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_BEGIN_MATCH, (int)EMMatchType.MatchType_NewPlayerGuide2);
            }
            else
            {
                ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_BEGIN_MATCH, (int)EMMatchType.MatchType_MachineMatch);
            }
            
        }

        public void OnSurviveVSModeBtnClick()
        {
            if (GameLogicAPI.getPlayerNoviceGuideTypeData() < (int)ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUIDE_TWOSTEP)
            {
                // 发送到逻辑层 引导相关记录
                cmd_guide_info guideInfo = new cmd_guide_info();
                guideInfo.nGuideID = (int)EMMatchGuideStep.GuideStep_MachineGameModel_Select;

                ViewEventHelper.Instance.SendCommand<cmd_guide_info>(GameLogicDef.GVIEWCMD_MATCH_GUIDE_INFO, ref guideInfo);

                ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_BEGIN_MATCH, (int)EMMatchType.MatchType_NewPlayerGuide2);
            }
            else
            {
                ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_BEGIN_MATCH, (int)EMMatchType.MatchType_MultiCampMatch);
            }
            
        }
        

        public void OnCancelMatch()
        {
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_CANCEL_MATCH);
        }

        public void OnMatchConfirmCheck(int nCheckID)
        {
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_CONFIRM_ACCEPTCHECK, (int)nCheckID);
        }

        public void OnRefuseMatch(int nCheckID)
        {
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_REFUSE_ACCEPTCHECK, (int)nCheckID);
        }

        public void OnRecvSelfPunishInfo(IntPtr ptr, int nLen)
        {
            // 显示玩家正在接受惩罚
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_self_punish_head head = helper.get<cmd_self_punish_head>();
            
            for (int i = 0; i < head.nCount; i++)
            {
                cmd_self_punish_info data = helper.get<cmd_self_punish_info>();
                // 记录到本地
                m_SelfPunishInfo[data.byPunishType] = data.nLeftTime;
            }

            if (m_SelfPunishInfo.Count > 0)
            {
                ShowSelfPunishSideButton();
            }
            

        }

        public void OnRecvDeletePunishInfo(int nDelPunishType)
        {
            // 显示玩家正在接受惩罚
            m_SelfPunishInfo.Remove((byte)nDelPunishType);

            // 先移除
            SideButtonListRemoveData removData = new SideButtonListRemoveData();
            removData.nItemID = (int)ESideButtonItemType.ESBT_Punish;
            LogicDataCenter.sideButtonDataManager.RemoveItem(removData);

            if (m_SelfPunishInfo.Count > 0)
            {
                ShowSelfPunishSideButton();
            }

        }

        public void OnRecvMatchTypeOpenOrClose(IntPtr ptr, int nLen)
        {
            bool bMatchOpenChanged = false;
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_match_close_or_open data = helper.get<cmd_match_close_or_open>();
            int nMatchType = data.nMatchType;
            bool bOpen = data.bOpen == 1? true : false;
            bool bLogin = data.bActorLogin == 1? true : false;
            if (m_MatchTypeOpen.ContainsKey(data.nMatchType))
            {
                if (m_MatchTypeOpen[data.nMatchType] != bOpen)
                {
                    // 更新下显示层信息
                    m_MatchTypeOpen[data.nMatchType] = bOpen;
                    bMatchOpenChanged = true;
                }
                
            }
            else
            {
                bMatchOpenChanged = true;
                m_MatchTypeOpen[nMatchType] = bOpen;
            }

            if (bMatchOpenChanged)
            {
                UIMsgCmdData UIData = new UIMsgCmdData(0, 0, "", IntPtr.Zero, 0);
                UIData.msgID = (int)WndMsgID.WND_MSG_WAR_MAIN_MATCHTYPE_OPEN_OR_CLOSE;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MAIN_MATCHTYPE_OPEN_OR_CLOSE, UIData);
                
            }
        }

        public bool getMatchTypeShowEffect(EMMatchType eMatchType)
        {
            int nMatchType = (int)eMatchType;
            if (m_MatchTypeOpen.ContainsKey(nMatchType))
            {
                return m_MatchTypeOpen[nMatchType];
            }
            return false;
        }

        public void GetSelfSidePunishInfo(ref byte refPunishType, ref int refLeftTime)
        {
            refLeftTime = 0;
            foreach (KeyValuePair<byte, int> kvp in m_SelfPunishInfo)
            {
                if (refLeftTime < kvp.Value)
                {
                    refPunishType = kvp.Key;
                    refLeftTime = kvp.Value;
                }
            }

        }
        public void ShowSelfPunishSideButton()
        {
            int playerLv = 0;
            if (EntityFactory.MainHeroView != null)
                playerLv = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            SSchemeSideButtonInfo config = SideButtonConfig.Instance.GetSideButtonConfig((int)ESideButtonItemType.ESBT_Punish);
            if (playerLv >= config.nLowLv && playerLv <= config.nHighLv)
            {
                SideButtonListAddData msg = new SideButtonListAddData();
                msg.nItemID = (int)ESideButtonItemType.ESBT_Punish;
                msg.bAllowRepeatItem = false;
                msg.nButtonIconID = config.nIconID;
                msg.strButtonText = config.strName;
                msg.strTooltip = config.strHint;
                msg.callBack = (customData) =>
                {
                    // 显示界面
                    UIMsgCmdData UIData = new UIMsgCmdData(0, 0, "", IntPtr.Zero, 0);
                    UIData.msgID = (int)WndMsgID.WND_MSG_SIDEBTN_PUNISH_SHOW;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SIDEBTN_PUNISH_SHOW, UIData);
                };

                LogicDataCenter.sideButtonDataManager.AddItem(msg);
            }
        }

        void SideButtonOnTimer()
        {
            if (m_SelfPunishInfo.Count <= 0)
            {
                return;
            }
            List<byte> updateList = new List<byte>();
            List<byte> removeList = new List<byte>();
            foreach (KeyValuePair<byte, int> kvp in m_SelfPunishInfo)
            {
                updateList.Add(kvp.Key);
            }

            for (int i = 0; i < updateList.Count; i++)
            {
                byte key = updateList[i];
                m_SelfPunishInfo[key]--;
                if (m_SelfPunishInfo[key] <= 0)
                {
                    // 记录起来 要删除
                    removeList.Add(key);

                }
            }

            for(int i=0;i<removeList.Count;i++)
            {
                m_SelfPunishInfo.Remove(removeList[i]);
            }

            if (m_SelfPunishInfo.Count <= 0)
            {
                // 显示更新
                SideButtonListRemoveData removData = new SideButtonListRemoveData();
                removData.nItemID = (int)ESideButtonItemType.ESBT_Punish;
                LogicDataCenter.sideButtonDataManager.RemoveItem(removData);
            }
        }

        public string GetLeftTimeStr(int nLeftSeconds)
        {
            if (nLeftSeconds <= 0)
            {
                return "";
            }

            int nHours = nLeftSeconds / 3600;
            int nMinute = (nLeftSeconds / 60) % 60;
            int nSeconds = nLeftSeconds  % 60;
            string szProssesText = String.Format("{0:D2}:{1:D2}:{2:D2}", nHours, nMinute, nSeconds);

            return szProssesText;
        }

        // 收到推荐排位请求
        public void OnRecvRecommendRank()
        {
            // 社会服取不到英雄星级数据，客户端判断一次排位要求(int)EMMatchType.MatchType_None
            SMatchRankLimitHeroSchemeInfo info = MatchRankLimitConfig.Instance.getRankLimitHeroShemeInfo((int)EMMatchType.MatchType_Rank);
            if (info == null)
                return;
            ActorHeroInfo heroInfo = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
            int nCount = 0;
            foreach (var item in heroInfo.dicEntityHeroInfo)
            {
                if (item.Value.bIsBuy > 0 && item.Value.byStarLv >= info.nLimitHeroStarLevel)
                    nCount++;

                if (nCount >= info.nLimitHeroCount)
                    break;
            }
            if (nCount < info.nLimitHeroCount)
                return;
            
            // DIDA提醒
            DIDAAddData msg = new DIDAAddData();
            msg.eLayout = EDIDALayout.EDT_Left;
            //msg.nDIDATime = 60;
            msg.nItemID = (int)EDIDAType.EDIDA_RANK;
            msg.bAllowRepeatItem = false;
            msg.nButtonIconID = (int)EDIDAType.EDIDA_RANK;
            msg.strTooltip = ULocalizationService.Instance.Get("UIView", "MatchTimeInfo", "RecommendTooltip");
            msg.sCustomData = new SideButtonCustomData();
            msg.callBack = (CustomerInfo) =>
            {
                UIDialogData toolTipData = new UIDialogData(
                    title: ULocalizationService.Instance.Get("UIView", "MatchTimeInfo", "RecommendTitle"),
                    message: ULocalizationService.Instance.Get("UIView", "MatchTimeInfo", "RecommendContent"),
                    buttons: new UIWidgets.DialogActions() 
                    {
						{ULocalizationService.Instance.Get("UIView", "Common", "Agree"),onClickAgreeBtn},
						{ULocalizationService.Instance.Get("UIView", "Common", "Refuse"),onClickRefuseBtn}
                    },
                    closeButtonAction : new UnityEngine.Events.UnityAction(onClickCloseBtn)
				);

                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
            };

            LogicDataCenter.didaDataManager.AddItem(msg);
        }

        // 接受
        private bool onClickAgreeBtn()
        {
            OnRankModeBtnClick();

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_RECOMMEND_RANK_RESULT, 1);

            return true;
        }

        // 拒绝
        private bool onClickRefuseBtn()
        {
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_RECOMMEND_RANK_RESULT, 0);

            return true;
        }

        private void onClickCloseBtn()
        {
            onClickRefuseBtn();
        }

    }
}
