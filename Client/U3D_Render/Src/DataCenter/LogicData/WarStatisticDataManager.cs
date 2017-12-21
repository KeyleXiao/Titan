using GameLogic;
using System;
using System.Collections.Generic;
using UnityEngine;
using USpeedUI;
using USpeedUI.Chat;
using War;
using Data.SideButtonConfig;
using System.Linq;

namespace DataCenter
{
    enum ETIMER
    {
        ETIMER_FIRSTWINAWARK = 0,	// 首胜奖励倒计时
    };

    public class ActorStatisticInfo
    {
        public float lastRequestTime;
        public cmd_entity_update_warstatistic_total totalStatistic;
        public List<cmd_entity_update_warstatistic_total_warinfo> totalStatisticWar;
    }

    public class WarStatisticInfo
    {
        public cmd_entity_update_warstatistic_war mainInfo;
        public List<cmd_entity_update_warstatistic_war_actorinfo> actorInfo;
    }

    public class WarStatisticDataManager : ITimerHanlder
    {
        private Dictionary<int, ActorStatisticInfo> actorStatistic;
        private Dictionary<long, WarStatisticInfo> warStatistic;

        private int m_firstwinLeftTime;
        public int FirstWinLeftTime
        {
            get { return m_firstwinLeftTime; }
        }
        private float m_lastUpdateTime;

        private EGetSingleWarType m_getSingleWarType;

		private ResNode achiIconPrefabRes;
		public ResNode achiIconObj{ get{return achiIconPrefabRes;} }

		public void Init()
        {
            actorStatistic = new Dictionary<int, ActorStatisticInfo>();
            warStatistic = new Dictionary<long, WarStatisticInfo>();
			achiIconPrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, UPath.UPrefabsPath + "PlayerWarStatistic/AchiIconItem");
		}

		public void Clear()
		{
			AssetBundleManager.DeleteAssets(ref achiIconPrefabRes, true);
        }

		// 获取某个玩家的历史战绩总览
		public ActorStatisticInfo GetActorOverview(int actorID, List<int> nMatchTypeIDList = null)
        {
            ActorStatisticInfo info = null;
            if (actorStatistic.TryGetValue(actorID, out info))
            {
                // 1分钟内直接返回
                if (info.lastRequestTime + 60f > Time.unscaledTime)
                {
                    if(nMatchTypeIDList == null)
                    {
                        return info;
                    }
                    else if(nMatchTypeIDList.Count == 1 && nMatchTypeIDList[0] == 0)
                    {
                        return info;
                    }
                    else
                    {
                        ActorStatisticInfo result = new ActorStatisticInfo();
                        result.totalStatistic = info.totalStatistic;
                        result.totalStatisticWar = info.totalStatisticWar.Where(v => (nMatchTypeIDList.Contains(v.nWarType))).Select(v => v).ToList();
                        return result;
                    }
                }
            }
            else
            {
                info = new ActorStatisticInfo();
                info.totalStatistic = new cmd_entity_update_warstatistic_total();
                info.totalStatisticWar = new List<cmd_entity_update_warstatistic_total_warinfo>();
                actorStatistic[actorID] = info;
            }

			// 向服务器请求最新的战绩数据
            info.lastRequestTime = Time.unscaledTime;
            RequestTotalStatistic(actorID);

            return info;
        }

		// 获取某场的详细战绩
        public WarStatisticInfo GetSingleWarInfo(long nWarID, EGetSingleWarType eWndType)
        {
            m_getSingleWarType = eWndType;

            WarStatisticInfo info = null;
            if (warStatistic.TryGetValue(nWarID, out info))
            {
                return info;
            }
            else
            {
                info = new WarStatisticInfo();
                info.mainInfo = new cmd_entity_update_warstatistic_war();
                info.actorInfo = new List<cmd_entity_update_warstatistic_war_actorinfo>();
                warStatistic[nWarID] = info;
            }

            RequestWarStatistic(nWarID, eWndType);
            return info;
        }

		// 向服务器请求某个玩家的历史战绩总览
        private void RequestTotalStatistic(int actorID)
        {
            cmd_warstatistic_req_total msgInfo = new cmd_warstatistic_req_total();
            msgInfo.nActorID = actorID;

            EntityEventHelper.Instance.SendCommand<cmd_warstatistic_req_total>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQ_WARSTATISTIC_TOTAL, ref msgInfo);
        }

		// 向服务器请求某场的详细战绩
        private void RequestWarStatistic(long warID, EGetSingleWarType eWndType)
        {
            cmd_warstatistic_req_war msgInfo = new cmd_warstatistic_req_war();
            msgInfo.nWarID = warID;
            msgInfo.byWnd = (byte)eWndType;

            EntityEventHelper.Instance.SendCommand<cmd_warstatistic_req_war>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQ_WARSTATISTIC_WAR, ref msgInfo);
        }

		// 收到服务器返回的历史战绩总览
        public void RecvTotalStatistic(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);

            cmd_entity_update_warstatistic_total totalStatistic = helper.get<cmd_entity_update_warstatistic_total>();
            ActorStatisticInfo info = null;
            if (actorStatistic.TryGetValue(totalStatistic.nActorID, out info) == false)
            {
                return;
            }
            info.totalStatistic = totalStatistic;

            info.totalStatisticWar.Clear();
            for (int i = 0; i < info.totalStatistic.nWarInfoCount; ++i)
            {
                cmd_entity_update_warstatistic_total_warinfo data = helper.get<cmd_entity_update_warstatistic_total_warinfo>();
                info.totalStatisticWar.Add(data);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_UPDATETOTAL, null);
        }

		// 收到服务器返回的某场战绩详情
        public void RecvWarStatistic(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);

            cmd_entity_update_warstatistic_war mainInfo = helper.get<cmd_entity_update_warstatistic_war>();
            WarStatisticInfo info = null;
            if (warStatistic.TryGetValue(mainInfo.nWarID, out info) == false)
            {
                return;
            }
            info.mainInfo = mainInfo;

            info.actorInfo.Clear();
            for (int i = 0; i < info.mainInfo.nActorCount; ++i)
            {
                cmd_entity_update_warstatistic_war_actorinfo data = helper.get<cmd_entity_update_warstatistic_war_actorinfo>();
                // 假人数据
                if (LogicDataCenter.playerSystemDataManager.bFake)
                {
                    //替换下名字
                    if (LogicDataCenter.playerSystemDataManager.nRequestPDBID == data.nActorID)
                    {
                        data.szActorName = LogicDataCenter.playerSystemDataManager.strCurFakeName;
                    }
                }

                info.actorInfo.Add(data);
            }

            if (m_getSingleWarType == EGetSingleWarType.EGSWT_PlayerRecord)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_UPDATEWAR, null);
            }
            else if (m_getSingleWarType == EGetSingleWarType.EGSWT_KinRecord)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_SINGLE, null);
            } 
        }
        
		// 更新首胜时间
        public void UpdateFirstwinTime(IntPtr ptrParam, int nPtrLen)
        {
            cmd_entity_firstwin_notice data = IntPtrHelper.toData<cmd_entity_firstwin_notice>(ptrParam);
            m_firstwinLeftTime = data.nLeftTime;
            m_lastUpdateTime = Time.unscaledTime;
        }

		// 获取对战结果描述
        public String GetWarResultItemName(int nGlory, int nMultiKillMax, int nContinueKillMax)
        {
            String strFeature = "";
            if (EntityFactory.MainHeroView == null)
                return strFeature;

            if (nMultiKillMax == 5)
                strFeature = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowDesc5Kill");
            else if (nMultiKillMax == 4)
                strFeature = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowDesc4Kill");
            else if (nContinueKillMax >= 8)
                strFeature = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowDescOver8Kill");
            else if ((nGlory & (int)War.EWarMedal.EWM_SCORE) > 0)
                strFeature = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowDescMVP");
            else if (nMultiKillMax == 3)
                strFeature = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowDesc3Kill");
            else if ((nGlory & (int)War.EWarMedal.EWM_KILLCOUNT) > 0)
                strFeature = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowDescTopKill");
            else if ((nGlory & (int)War.EWarMedal.EWM_ASSCOUNT) > 0)
                strFeature = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowDescTopAssist");

            return String.Format("[" + ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowDescText") + "]", EntityFactory.MainHeroView.Property.CreatureName, strFeature);
        }

		// 炫耀战绩到聊听框
        public void ShowWarResult(int nActorID, long nWarID, int nGlory, int nMultiKillMax, int nContinueKillMax)
        {
            if (nWarID == 0 || nActorID == 0)
			{
				Debug.LogError("nWarID=" + nWarID + ", nActorID =" + nActorID);
                return;
			}

            SChatMessageObjectInfo channelObj;
            channelObj.type = ChatItemType.ObjType_Text;
            channelObj.subType = ChatItemSubType.ObjSubType_Text_ActorWarResult;
            channelObj.text = GetWarResultItemName(nGlory, nMultiKillMax, nContinueKillMax);
            channelObj.param = new Dictionary<String, String>();
            channelObj.param.Add("color", LogicDataCenter.chatMessageManager.CreateColorParam(235, 80, 30));
            channelObj.param.Add("id", "" + nActorID);
            channelObj.param.Add("war", "" + nWarID);
            ChatBoxAddInputFieldItemData uiData = new ChatBoxAddInputFieldItemData();
            uiData.msgID = (int)WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM;
            uiData.objInfo = channelObj;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM, uiData);
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)ETIMER.ETIMER_FIRSTWINAWARK:
                    {
                        float nElapseTime = Time.unscaledTime - m_lastUpdateTime;
                        m_firstwinLeftTime -= (int)nElapseTime;
                        m_lastUpdateTime = Time.unscaledTime;

                        // 倒计时以后再加
                        if (m_firstwinLeftTime <= 0)
                        {
                            TimerManager.KillTimer(this, (int)ETIMER.ETIMER_FIRSTWINAWARK);

                            int playerLv = 0;
                            if (EntityFactory.MainHeroView != null)
                                playerLv = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
                            SSchemeSideButtonInfo config = SideButtonConfig.Instance.GetSideButtonConfig((int)ESideButtonItemType.ESBT_FirstWin);
                            if (playerLv >= config.nLowLv && playerLv <= config.nHighLv)
                            {
                                SideButtonListAddData msg = new SideButtonListAddData();
                                msg.nItemID = (int)ESideButtonItemType.ESBT_FirstWin;
                                msg.nButtonIconID = config.nIconID;
                                msg.strButtonText = config.strName;
                                msg.strTooltip = config.strHint;
                                LogicDataCenter.sideButtonDataManager.AddItem(msg);
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
}
