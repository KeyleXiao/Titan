using System;
using System.Collections.Generic;
using GameLogic;
using USpeedUI;
using Data.ActorPrizeConfig;
using War;

namespace DataCenter
{
    ////////////// 先不删除SRandCardResult部分,策划有可能改回原方案 //////////////
    //public struct SRandCardResult
    //{
    //    public int nPrizeId;
    //    public List<cmd_entity_hero_card> cardResult;
    //};
    public class PrizeChestDataManager
    {
        private Dictionary<int, int> m_dicChestInfo;
        public Dictionary<int, int> ChestInfo { get { return m_dicChestInfo; } }

        private cmd_entity_open_chest_result m_openChestResult;
        public cmd_entity_open_chest_result OpenChestResult { get { return m_openChestResult; } }

        const int RAND_CARD_FIRST_INDEX = 901;
        private int m_randCardIndex = RAND_CARD_FIRST_INDEX;
        public int RandCardIndex { get { return m_randCardIndex; } }
        private Dictionary<int, SSchemeActorPrizeConfig> m_randCardResult;
        public Dictionary<int, SSchemeActorPrizeConfig> RandCardResult { get { return m_randCardResult; } }
        private List<int> m_PrizeIdList;
        public List<int> PrizeIdList { get { return m_PrizeIdList; } }
        private int m_OpenChestID = 0;
        public int OpenChestID { get { return m_OpenChestID; } }
        //private List<SRandCardResult> m_randCardResult;
        //public List<SRandCardResult> RandCardResult { get { return m_randCardResult; } }
        private bool m_bInitChestData;

        public void Init()
        {
            //m_randCardResult = new List<SRandCardResult>();
            m_randCardResult = new Dictionary<int, SSchemeActorPrizeConfig>();
            m_PrizeIdList = new List<int>();

            m_dicChestInfo = new Dictionary<int, int>();
            m_bInitChestData = true;
        }

        public void Clear()
        {
            m_randCardResult.Clear();
            m_PrizeIdList.Clear();
        }

        public void OnUpdateChestInfo(IntPtr ptrParam, int nPtrLen)
        {
            if (m_bInitChestData)
            {
                IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
                int nCount = helper.get<int>();

                for (int i = 0; i < nCount; i++)
                {
                    int nChestID = helper.get<int>();
                    int nChestCount = helper.get<int>();
                    m_dicChestInfo[nChestID] = nChestCount;
                }

                m_bInitChestData = false;
            }
            else
            {
                Dictionary<int, int> newChestInfo = new Dictionary<int, int>();
                IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
                int nCount = helper.get<int>();
                for (int i = 0; i < nCount; i++)
                {
                    int nChestID = helper.get<int>();
                    int nChestCount = helper.get<int>();
                    newChestInfo[nChestID] = nChestCount;
                }

                int nOldCount = 0;
                foreach (var item in m_dicChestInfo)
                {
                    nOldCount += item.Value;
                }
                int nNewCount = 0;
                foreach (var item in newChestInfo)
                {
                    nNewCount += item.Value;
                }

                //if (nNewCount > nOldCount)
                //{
                //    PrizeTipData tipData = new PrizeTipData();
                //    tipData.eTipType = EMPrizeTipType.EMPrizeTipType_Chest;
                //    tipData.nPrizeID = 0;

                //    LogicDataCenter.prizeTipDataManager.AddItem(tipData);
                //}

                m_dicChestInfo = newChestInfo;
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PRIZECHEST_UPDATECHESTINFO, null);

            // 计算宝箱红点提示
            int nTotalCount = 0;
            Dictionary<int, SSchemePrizeChestConfig> chestConfig = ActorPrizeConfig.Instance.PrizeChestConfig;
            foreach (var config in chestConfig)
            {
                if (!m_dicChestInfo.ContainsKey(config.Key))
                    continue;

                int nHaveNum = m_dicChestInfo[config.Key];
                if (config.Value.nAlwaysShow == 0 && nHaveNum <= 0)
                    continue;

                //只显示宝箱，不显示矿石
                if (config.Value.nChestFlag > 1)
                    continue;

                nTotalCount += nHaveNum;
            }
            RedPointSetCount uiData = new RedPointSetCount();
            uiData.nSubType = 2;
            uiData.nCount = nTotalCount;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_SETREDPOINTTIP, uiData);
        }

        //public void OnOpenChestResult(IntPtr ptrParam, int nPtrLen)
        //{
        //    m_openChestResult = IntPtrHelper.toData<cmd_entity_open_chest_result>(ptrParam);

        //    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PRIZECHEST_OPENCHESTRESULT, null);
        //}

        //public void OnReturnRandCardInfo(IntPtr ptrParam, int nPtrLen)
        //{
        //    IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        //    int nPrizeID = helper.get<int>();
        //    int nCount = helper.get<int>();

        //    SRandCardResult cardInfo = new SRandCardResult();
        //    cardInfo.nPrizeId = nPrizeID;
        //    cardInfo.cardResult = new List<cmd_entity_hero_card>();
        //    for (int i = 0; i < nCount; i++)
        //    {
        //        cmd_entity_hero_card data = helper.get<cmd_entity_hero_card>();
        //        cardInfo.cardResult.Add(data);
        //    }

        //    cardInfo.cardResult.Sort(CompareMember);
        //    m_randCardResult.Add(cardInfo);
        //}

        public void OnOpenChestResult(IntPtr ptrParam, int nPtrLen)
        {
            m_openChestResult = IntPtrHelper.toData<cmd_entity_open_chest_result>(ptrParam);

            // 卡包要拆成具体卡牌显示，所以去除卡包奖励，加入已赋值过的m_randCardResult(type为6的为策划卡包ID，901为策划预留卡牌ID)
            for (int i = 0; i < m_openChestResult.nPrizeID.Length; i++)
                m_PrizeIdList.Add(m_openChestResult.nPrizeID[i]);
            //m_PrizeIdList.RemoveAll(it => (it == 0 || (it >= 1511 && it <= 1514)));
            for (int i = m_PrizeIdList.Count - 1; i >= 0; --i)
            {
                if (m_PrizeIdList[i] == 0)
                {
                    m_PrizeIdList.Remove(m_PrizeIdList[i]);
                    continue;
                }

                SSchemeActorPrizeConfig config = ActorPrizeConfig.Instance.GetActorPrizeConfig(m_PrizeIdList[i]);
                if (config != null && config.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROCARD_RANDOM)
                {
                    m_PrizeIdList.Remove(m_PrizeIdList[i]);
                }
            }

            // 按数量从大到小排序
            List<SSchemeActorPrizeConfig> temp = new List<SSchemeActorPrizeConfig>();
            foreach (var item in m_randCardResult)
                temp.Add(item.Value);
            temp.Sort(CompareMember);

            foreach (var config in temp)
                m_PrizeIdList.Add(config.nPrizeID);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PRIZECHEST_OPENCHESTRESULT, null);
        }

        public void OnReturnRandCardInfo(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            int nPrizeID = helper.get<int>();
            int nCount = helper.get<int>();
            SSchemeActorPrizeConfig config = ActorPrizeConfig.Instance.GetActorPrizeConfig(nPrizeID);

            for (int i = 0; i < nCount; i++)
            {
                cmd_entity_hero_card data = helper.get<cmd_entity_hero_card>();

                // 已经有则合并数量
                bool bInResult = false;
                foreach (var item in m_randCardResult)
                {
                    if (item.Value.nPrizeParam[0] == data.nHeroID)
                    {
                        item.Value.nPrizeParam[1] += data.nCount;
                        item.Value.strPrizeName = data.chHeroCardName + "*" + item.Value.nPrizeParam[1].ToString();
                        bInResult = true;
                    }
                }
                if (bInResult)
                    continue;

                // 没有则加入
                SSchemeActorPrizeConfig cardInfo = new SSchemeActorPrizeConfig();
                cardInfo.nPrizeID = m_randCardIndex;
                cardInfo.strPrizeName = data.chHeroCardName + "*" + data.nCount.ToString();
                cardInfo.nPrizeGrade = config.nPrizeGrade;
                cardInfo.nPrizeType = 5;
                cardInfo.nPrizeParam = new int[3];
                cardInfo.nCakeTrailEffectID = 9;
                cardInfo.nPedestalEffectID = 15;
                cardInfo.nPedestalGlowEffectID = 21;
                cardInfo.nPrizeParam[0] = data.nHeroID;
                cardInfo.nPrizeParam[1] = data.nCount;
                cardInfo.strPrizeDesc = data.chHeroCardDesc;
                m_randCardResult[cardInfo.nPrizeID] = cardInfo;
                m_randCardIndex++;
            }
        }

        public void RequestOpenChest(int nChestType)
        {
            m_randCardResult.Clear();
            m_PrizeIdList.Clear();
            m_randCardIndex = RAND_CARD_FIRST_INDEX;
            m_OpenChestID = nChestType;

            cmd_entity_open_chest cmdInfo;
            cmdInfo.nChestType = nChestType;
            EntityEventHelper.Instance.SendCommand<cmd_entity_open_chest>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQ_OPENCHEST, ref cmdInfo);
        }

        private int CompareMember(SSchemeActorPrizeConfig A, SSchemeActorPrizeConfig B)
        {
            if (A.nPrizeParam[1] != B.nPrizeParam[1])
                return A.nPrizeParam[1] > B.nPrizeParam[1] ? -1 : 1;
            else
                return A.nPrizeParam[0] < B.nPrizeParam[0] ? -1 : 1;
        }
    }
}
