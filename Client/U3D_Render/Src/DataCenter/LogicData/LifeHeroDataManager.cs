using ASpeedGame.Data.LifeHero;
using GameLogic;
using Stage_ManagerDef;
using System;
using System.Collections.Generic;
using UnityEngine;
using USpeedUI;

namespace DataCenter
{
    // 匹配规则，随着注入次数的增加，规则也不断升级
    public class LifeHeroRules
    {
        public static bool IsMeetRule(int key, cmd_warstatistic_history_record record)
        {
            SSchemeLifeHero config;
            if (LifeHeroConfig.Instance.LifeHeroConfigDic.TryGetValue(key, out config))
            {
                return (record.wKillPersonNum + record.wAssistCount - record.wDieCount - config.rule) >= 0 ? true : false;
            }
            else
            {
                Debug.LogWarning("not find match rule in lifehero.csv. key=" + key);
                return false;
            }
        }
    }

    public enum LifeHeroActivateType
    {
        INVALID_DATA,           // 无效
        CAN_ACTIVATION,         // 有可以激活的本命英雄
        NO_ACTIVATION,          // 没有可激活的本命英雄
    }

    public enum ELifeHeroMatchType
    {
        INVALID_DATA,          // 无效
        HAVE_MatchType_Rank,   // 有排位赛
        NO_MatchType_Rank,     // 无排位赛
    }

    public enum ERecvDataFlag
    {
        NO_RECV_DATA = 0X00,
        RECV_LIFE_HERO = 0X01,
        RECV_HISTORY_RECORD = 0X02,
    }

    public class LifeHeroDataManager
    {
        private int m_prizeTaskData = 0;
        public int PrizeTaskData { get { return m_prizeTaskData; } }
        public delegate void PrizeTaskDataUpdate(int prizeTaskData);
        private event PrizeTaskDataUpdate PrizeTaskDataUpdateEvent;
        // 玩家的历史战绩，用来寻找是否有可激活的本命英雄
        private List<cmd_warstatistic_history_record> m_historyRecord = new List<cmd_warstatistic_history_record>();
        public List<cmd_warstatistic_history_record> HistoryRecord { get { return m_historyRecord; } }
        private ERecvDataFlag m_recvDataFlag = ERecvDataFlag.NO_RECV_DATA;
        private bool m_isFromWarExit = false; // 判断是否从战场返回，用于进入主城时本命英雄窗口的判断
        public bool IsFromWarExit
        {
            set
            {
                m_isFromWarExit = value;
                if (m_isFromWarExit)
                {
                    m_lifeHeroActivateType = LifeHeroActivateType.INVALID_DATA;
                    m_lifeheroMatchType = ELifeHeroMatchType.INVALID_DATA;

                    cmd_entity_end_player_data playerData = LastWarStat;
                    if (playerData.pDbid == 0)
                        return;

                    int[] allData = playerData.gzAllData;
                    cmd_war_info warInfo = LogicDataCenter.warInfoDataManager.getLastWarInfo();
                    cmd_warstatistic_history_record record;
                    record.nHeroID = playerData.nIconId;
                    record.wKillPersonNum = (ushort)allData[(int)War.ERecord_Data_Type.ERDT_KILLNUM];
                    record.wAssistCount = (ushort)allData[(int)War.ERecord_Data_Type.ERDT_ASSIST];
                    record.wDieCount = (ushort)allData[(int)War.ERecord_Data_Type.ERDT_DEADNUM];
                    record.bIsFull = warInfo.isFull;
                    record.wWarType = (ushort)warInfo.nMtachType;
                    m_historyRecord.Insert(0, record);
                    SetLifeHeroFlag();
                }
            }
            get
            {
                return m_isFromWarExit;
            }
        }
        // 本地玩家上一局战绩
        public cmd_entity_end_player_data LastWarStat = new cmd_entity_end_player_data();
        // 表示是否有可激活的本命英雄，在新手引导期间光效显示依据。CAN_ACTIVATION：引导光效在本命英雄入口按钮；NO_ACTIVATION：引导光效在开战按钮
        private LifeHeroActivateType m_lifeHeroActivateType = LifeHeroActivateType.INVALID_DATA;
        // 判断玩家是否有打过排位赛。用在新手引导期间，是否引导排位赛
        private ELifeHeroMatchType m_lifeheroMatchType = ELifeHeroMatchType.INVALID_DATA;
        public ELifeHeroMatchType LifeHeroMatchType { get { return m_lifeheroMatchType; } }
        // 存储的玩家拥有的本命英雄:key=heroID
        private Dictionary<int, cmd_entity_lifehero_info> m_LifeHeroDic = new Dictionary<int, cmd_entity_lifehero_info>();
        public Dictionary<int, cmd_entity_lifehero_info> AllLifeHero { get { return m_LifeHeroDic; } }

        public void Init()
        {
            EntityFactory.SetEntityNumPropHandler(OnCommand_UpdateNumProp);
        }

        public void AddPrizeTaskDataHandler(PrizeTaskDataUpdate handler)
        {
            PrizeTaskDataUpdateEvent -= handler;
            PrizeTaskDataUpdateEvent += handler;
        }

        public void RemovePrizeTaskDataHandler(PrizeTaskDataUpdate handler)
        {
            PrizeTaskDataUpdateEvent -= handler;
        }

        public void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
        {
            if (!e.objev.IsHero || e.objev == null)
                return;

            switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_ID:
                    {
                        int nActorID = e.data.nValue;
                        if (StageManager.Instance.getCurrentState() == EStage_State.ESS_GAME || StageManager.Instance.getCurrentState() == EStage_State.ESS_LOADSCENE)
                        {
                            if (!m_isFromWarExit)
                            {
                                m_recvDataFlag = ERecvDataFlag.NO_RECV_DATA;
                                RequestHistoryRecord((uint)nActorID);
                                RequestAllLifeHero(nActorID);
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public void Clear()
        {
            m_lifeHeroActivateType = LifeHeroActivateType.INVALID_DATA;
            m_lifeheroMatchType = ELifeHeroMatchType.INVALID_DATA;
            EntityFactory.RemoveEntityNumPropHandler(OnCommand_UpdateNumProp);
        }

        public void onEnterLoginState()
        {
            LastWarStat = new cmd_entity_end_player_data();
        }

        public void RequestAllLifeHero(int actorID)
        {
            if (actorID <= 0)
                return;

            cmd_entity_lifehero_query param;
            param.actorID = actorID;
            EntityEventHelper.Instance.SendCommand<cmd_entity_lifehero_query>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_LIFEHERO_QUERY, ref param);
        }

        public void OnRecvAllLifeHero(IntPtr ptr, int nLen)
        {
            m_LifeHeroDic.Clear();
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_count cmd = helper.get<cmd_count>();
            for (int i = 0; i < cmd.nCount; ++i)
            {
                cmd_entity_lifehero_info item = helper.get<cmd_entity_lifehero_info>();
                m_LifeHeroDic.Add(item.nHeroID, item);
            }

            m_recvDataFlag |= ERecvDataFlag.RECV_LIFE_HERO;
            if ((m_recvDataFlag & ERecvDataFlag.RECV_HISTORY_RECORD) == ERecvDataFlag.RECV_HISTORY_RECORD)
            {
                SetLifeHeroFlag();
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECV_ALL_LIFEHERO_INFO, null);
        }

        public void OnUpdatePrizeTaskData(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            m_prizeTaskData = helper.get<int>();
            if (PrizeTaskDataUpdateEvent != null)
            {
                PrizeTaskDataUpdateEvent(m_prizeTaskData);
            }
        }

        // 向服务器请求玩家最近历史战绩
        public void RequestHistoryRecord(uint actorID)
        {
            cmd_warstatistic_req_history_record msgInfo;
            msgInfo.dwActorID = actorID;

            EntityEventHelper.Instance.SendCommand<cmd_warstatistic_req_history_record>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQ_HISTORY_RECORD, ref msgInfo);
        }

        // 收到玩家的历史战绩
        public void RecvHistoryRecord(IntPtr ptrParam, int nPtrLen)
        {
            m_historyRecord.Clear();
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            int count = helper.get<int>();
            for (int i = 0; i < count; ++i)
            {
                cmd_warstatistic_history_record item = helper.get<cmd_warstatistic_history_record>();
                m_historyRecord.Add(item);
            }

            m_recvDataFlag |= ERecvDataFlag.RECV_HISTORY_RECORD;
            if ((m_recvDataFlag & ERecvDataFlag.RECV_LIFE_HERO) == ERecvDataFlag.RECV_LIFE_HERO)
            {
                SetLifeHeroFlag();
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LIFEHERO_RECV_HISTORY_RECORD);
        }

        public void RequestAddLifeHero(cmd_entity_lifehero_info lifeHero)
        {
            if (lifeHero.nHeroID <= 0 || lifeHero.nCultureID < 0 || lifeHero.nCultureID >= (int)ELifeHeroType.LIFEHERO_MAX_COUNT)
                return;

            if (m_LifeHeroDic.ContainsKey(lifeHero.nHeroID))
                return;

            EntityEventHelper.Instance.SendCommand<cmd_entity_lifehero_info>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_LIFEHERO_ADD, ref lifeHero);
        }

        public void OnUpdateLifeHero(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_entity_lifehero_info lifeHero = helper.get<cmd_entity_lifehero_info>();
            if (!m_LifeHeroDic.ContainsKey(lifeHero.nHeroID))
            {
                m_LifeHeroDic.Add(lifeHero.nHeroID, lifeHero);

                SetLifeHeroFlag();
            }
            else
            {
                m_LifeHeroDic[lifeHero.nHeroID] = lifeHero;
            }

            ULifeHeroInjectHeroData msg = new ULifeHeroInjectHeroData();
            msg.msgID = (int)WndMsgID.WND_MSG_LIFEHERO_INJECT;
            msg.lifeHero = lifeHero;
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_LIFE_HERO_CULTURE, WndMsgID.WND_MSG_LIFEHERO_INJECT, msg);
        }

        public void DelLifeHero(int heroID)
        {
            if (heroID <= 0)
                return;

            if (!m_LifeHeroDic.ContainsKey(heroID))
                return;

            m_LifeHeroDic.Remove(heroID);
            SetLifeHeroFlag();
            cmd_entity_lifehero_del param;
            param.heroID = heroID;
            EntityEventHelper.Instance.SendCommand<cmd_entity_lifehero_del>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_LIFEHERO_DEL, ref param);
        }

        public void RequestReplaceLifeHero(int heroID, cmd_entity_lifehero_info lifeHero)
        {
            if (heroID <= 0 || lifeHero.nHeroID <= 0)
                return;

            if (!m_LifeHeroDic.ContainsKey(heroID))
                return;

            if (m_LifeHeroDic.ContainsKey(lifeHero.nHeroID))
                return;

            lifeHero.nCultureID = m_LifeHeroDic[heroID].nCultureID;
            m_LifeHeroDic.Add(lifeHero.nHeroID, lifeHero);
            m_LifeHeroDic.Remove(heroID);

            SetLifeHeroFlag();

            cmd_entity_lifehero_replace param;
            param.heroID = heroID;
            param.hero = lifeHero;
            EntityEventHelper.Instance.SendCommand<cmd_entity_lifehero_replace>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_LIFEHERO_REPLACE, ref param);
        }

        public void ExchangeLifeHero(int heroID1, int heroID2)
        {
            if (heroID1 <= 0 || heroID2 <= 0)
                return;

            if (!m_LifeHeroDic.ContainsKey(heroID1))
                return;

            if (!m_LifeHeroDic.ContainsKey(heroID2))
                return;


            cmd_entity_lifehero_info item1 = m_LifeHeroDic[heroID1];
            cmd_entity_lifehero_info item2 = m_LifeHeroDic[heroID2];
            int cultureID = item1.nCultureID;
            item1.nCultureID = item2.nCultureID;
            item2.nCultureID = cultureID;
            m_LifeHeroDic[heroID1] = item1;
            m_LifeHeroDic[heroID2] = item2;
            cmd_entity_lifehero_exchange param;
            param.heroID1 = heroID1;
            param.heroID2 = heroID2;
            EntityEventHelper.Instance.SendCommand<cmd_entity_lifehero_exchange>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_LIFEHERO_EXCHANGE, ref param);
        }

        public bool HasCollectedAllLifeHero()
        {
            int data = GameLogicAPI.getLifeHeroTaskData();
            return data != 0 ? true : false;
        }

        public LifeHeroActivateType GetLifeHeroActivateType()
        {
            return m_lifeHeroActivateType;
        }

        public bool CanObtainPrize()
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

        public bool CanAddLifeHero()
        {
            if (LogicDataCenter.lifeHeroDataManager.AllLifeHero.Count == (int)ELifeHeroType.LIFEHERO_MAX_COUNT)
                return false;

            List<int> candidates = LogicDataCenter.lifeHeroDataManager.FindCandidatesFromHistoryRecord();
            if (candidates == null || candidates.Count <= 0)
                return false;

            return true;
        }

        // 从历史战绩中和玩家当前拥有的本命英雄中，找出满足当前条件的候选本命英雄
        public List<int> FindCandidatesFromHistoryRecord()
        {
            List<int> candidateList = new List<int>();
            foreach (cmd_warstatistic_history_record record in m_historyRecord)
            {
                if (IsMeetLifeHeroRule(record) && !candidateList.Contains(record.nHeroID))
                {
                    candidateList.Add(record.nHeroID);
                }
            }

            return candidateList;
        }

        // 用来在新手引导过程中，本命入口UI是否展示引导光效。
        private void SetLifeHeroFlag()
        {
            bool hasRank = false;
            bool hasActivatedLifeHero = false;
            foreach (cmd_warstatistic_history_record record in m_historyRecord)
            {
                if (!hasRank && (record.wWarType == (int)Match_ManagerDef.EMMatchType.MatchType_Rank))
                {
                    hasRank = true;
                }

                if (!hasActivatedLifeHero && IsMeetLifeHeroRule(record))
                {
                    hasActivatedLifeHero = true;
                }

                if (hasRank && hasActivatedLifeHero)
                    break;
            }

            m_lifeheroMatchType = hasRank ? ELifeHeroMatchType.HAVE_MatchType_Rank : ELifeHeroMatchType.NO_MatchType_Rank;
            m_lifeHeroActivateType = hasActivatedLifeHero ? LifeHeroActivateType.CAN_ACTIVATION : LifeHeroActivateType.NO_ACTIVATION;
        }

        private int GetMeetRuleKey()
        {
            return m_LifeHeroDic.Count == (int)ELifeHeroType.LIFEHERO_MAX_COUNT ? m_LifeHeroDic.Count - 1 : m_LifeHeroDic.Count;
        }

        // 根据当前本命英雄的个数和历史战绩匹配是否有可激活的本命英雄
        private bool IsMeetLifeHeroRule(cmd_warstatistic_history_record record)
        {
            if (record.nHeroID <= 0)
            {
                Debug.LogWarning("返回的历史记录中错误(heroID=0).warType=" + record.wWarType + "k/d/a=" + record.wKillPersonNum + "/" + record.wDieCount + "/" + record.wAssistCount);
                return false;
            }

            int meetRuleKey = GetMeetRuleKey();

            return (record.bIsFull
                && LifeHeroRules.IsMeetRule(meetRuleKey, record)
                && LifeHeroConfig.Instance.IsMeetWarType(meetRuleKey, record.wWarType)
                && !m_LifeHeroDic.ContainsKey(record.nHeroID));
        }
    }
}
