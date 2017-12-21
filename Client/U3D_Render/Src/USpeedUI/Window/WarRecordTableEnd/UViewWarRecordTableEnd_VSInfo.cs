/*******************************************************************
** 文件名:	UViewWarRecordTableEnd_VSInfo.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2017-9-12
** 版  本:	1.0
** 描  述:	战后战绩表显示界面-对战信息
		
********************************************************************/
using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using GameLogic;
using DataCenter;
using War;
using ASpeedGame.Data.GameMobaSchemes;
using U3D_Render.Common;
using UIWidgets;
using ASpeedGame.Data.WarStatisticMap;

namespace USpeedUI
{
    // 对战信息表
    public partial class UViewWarRecordTableEnd : UIBaseWndView, ITimerHanlder
    {
        public GameObject selfVSInfoPlayerGroup;
        public GameObject enemyVSInfoPlayerGroup;
        public Text ResultText;
        public Text MatchTimeText;
        public Text BlueKillNum;
        public Text RedKillNum;

        private ResNode VSInfoPlayerRes;

        private UVSInfoPlayerInfo[] VSInfoPlayerInfo;

        private Transform VSInfoTrans;

        private int nCurVSInfoBlueIndex = 0;
        private int nCurVSInfoRedIndex = 0;

        private GameObject RankScoreTip;
        private Text MatchTypeText;
        private Text StarUpDesc;
        private Text GradeDesc;
        private GameObject RankIconDefaultItem;

        private bool m_isSelfWin = false;

        private Image[] m_RankIconList;

        // 初始化所有玩家的对战信息表
        void initVSInfoPlayerFrame()
        {
            VSInfoPlayerInfo = new UVSInfoPlayerInfo[UDefines.WAR_PERSON_MAX_COUNT];

            VSInfoPlayerRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/DynamicState_Moba/WarRecordTable_End/VSInfoPlayerInfo");

            // 赋值预制体
            if (VSInfoPlayerRes == null)
            {
                Debug.LogError("VSInfoPlayerRes = null");
                return;
            }

            GameObject instance;
            for (int i = 0; i < UDefines.WAR_PERSON_MAX_COUNT; ++i)
            {
                if (i < 5)
                {
                    instance = VSInfoPlayerRes.InstanceMainRes();
                    if (!instance)
                    {
                        Debug.LogError("setRecordTable1PlayerInfo instance  = NULL");
                        return;
                    }
                    instance.name = VSInfoPlayerRes.AssetName;
                    instance.transform.SetParent(selfVSInfoPlayerGroup.transform, false);
                }
                else
                {
                    instance = VSInfoPlayerRes.InstanceMainRes();
                    if (!instance)
                    {
                        Debug.LogError("setRecordTable1PlayerInfo instance  = NULL");
                        return;
                    }
                    instance.name = VSInfoPlayerRes.AssetName;
                    instance.transform.SetParent(enemyVSInfoPlayerGroup.transform, false);
                }

                VSInfoPlayerInfo[i] = instance.GetComponent<UVSInfoPlayerInfo>();
                VSInfoPlayerInfo[i].initVSPlayerInfo();
                VSInfoPlayerInfo[i].gameObject.SetActive(true);
            }

            nCurVSInfoBlueIndex = 0;
            nCurVSInfoRedIndex = UDefines.WAR_PERSON_MAX_COUNT / 2;


            RankScoreTip = VSInfoTrans.Find("TitleFrame/RankScoreTip").gameObject;
            MatchTypeText = VSInfoTrans.transform.Find("TitleFrame/MatchTypeDesc").GetComponent<Text>();
            GradeDesc = RankScoreTip.transform.Find("GradeDesc").GetComponent<Text>();
            StarUpDesc = RankScoreTip.transform.Find("RankIconList/StarUpDesc").GetComponent<Text>();
            RankIconDefaultItem = RankScoreTip.transform.Find("RankIconList/DefaultItem").gameObject;
        }

        void destroyVSInfoPlayerFrame()
        {
            for (int i = 0; i < UDefines.WAR_PERSON_MAX_COUNT; ++i)
            {
                UVSInfoPlayerInfo info = VSInfoPlayerInfo[i];
                if (info == null)
                {
                    continue;
                }
                VSInfoPlayerInfo[i].ResDestroy();

            }

            if(VSInfoPlayerRes != null)
            {
                AssetBundleManager.DeleteAssets(ref VSInfoPlayerRes, true);
            }

            GameUtil.DestroyChild(selfVSInfoPlayerGroup.transform);
            GameUtil.DestroyChild(enemyVSInfoPlayerGroup.transform);
        }

        // 设置单个玩家的对战信息
        public void setVSInfoPlayerInfo(cmd_entity_end_player_data cmdData, int nScoreTop, int nMoneyTop, int nKillTop, int nDieTop, int nAssTop)
        {

            ulong uPDBID = cmdData.pDbid;

            int nIndex = LogicDataCenter.recordTableDataManager.getIndexAcordPDBID(uPDBID);
            if (nIndex == -1)
            {
                return;
            }

            int i = 0;
            if(nIndex < UDefines.WAR_PERSON_MAX_COUNT / 2)
            {
                i = nCurVSInfoBlueIndex++;
            }
            else
            {
                i = nCurVSInfoRedIndex++;
            }

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
            MatchTimeText.text = String.Format("{0}{1}{2}{3}", nMin ,strMinDesc, nMs , strSecDesc);

            if (isSelfWin)
            {
                ResultText.text = ULocalizationService.Instance.Get("UIView", "Common", "Win");
            }
            else
            {
                ResultText.text = ULocalizationService.Instance.Get("UIView", "Common", "Failure");
            }

            m_isSelfWin = isSelfWin;
        }

        
        // 设置双方击杀
        public void setKillNum()
        {
            Dictionary<ulong, WarRecordTableDataManager.swar_flaunt_result> playerDataDic = LogicDataCenter.recordTableDataManager.getWarEndPlayerData();
            int nBlueKillNum = 0;
            int nRedKillNum = 0;
            foreach (var item in playerDataDic)
            {
                uint nPdbid = item.Value.data.pDbid;
                int index = LogicDataCenter.recordTableDataManager.getIndexAcordPDBID(nPdbid);
                int nKillNum = item.Value.data.gzAllData[(int)ERecord_Data_Type.ERDT_KILLNUM];
                if(index < UDefines.WAR_PERSON_MAX_COUNT / 2)
                {
                    nBlueKillNum += nKillNum;
                }
                else
                {
                    nRedKillNum += nKillNum;
                }
            }

            BlueKillNum.text = nBlueKillNum.ToString();
            RedKillNum.text = nRedKillNum.ToString();
        }

        // 设置排位信息
        public void setRankInfo(cmd_entity_end_player_data selfData)
        {
            Debug.Log(String.Format("{0}({1}),{2},当前分：{3},上次分：{4}本局分：{5}", selfData.nMatchType,selfData.szMatchTypeName, selfData.nUpgradeRank, selfData.nPlayerMatchScore, 
                selfData.nOldPlayerMatchScore, selfData.nMatchScore));

            // 胜利且是排位赛才显示
            if(selfData.nMatchType == 8)
            {
                // todo 战场的要单独处理下 王者段位以上的 只加分了 段位是单独换算的
                int nMatchType = selfData.nMatchType;
                int nCurRankScore = selfData.nPlayerMatchScore;
                int nRankIconID = GameLogicAPI.getActorRankIconID(nMatchType, nCurRankScore);
                int nRankStar = GameLogicAPI.getActorRankStar(nMatchType, nCurRankScore);
                int nMaxRankStar = GameLogicAPI.getActorCurRankMaxStar(nMatchType, nCurRankScore);

                int nOldRankScore = selfData.nOldPlayerMatchScore;
                int nOldRankStar = GameLogicAPI.getActorRankStar(nMatchType, nOldRankScore);

                // 段位名称
                IntPtr ptr = GameLogicAPI.getActorRankName(nMatchType, nCurRankScore);
                string strRankName = "";
                if (ptr != IntPtr.Zero)
                    strRankName = IntPtrHelper.Ptr2Str(ptr);
                GradeDesc.text = strRankName;

                // 当前段位所有星级
                m_RankIconList = new Image[nMaxRankStar];
                for (int i = 0; i < nMaxRankStar; ++i)
                {
                    GameObject RankIconItem = ResNode.InstantiateRes(RankIconDefaultItem);
                    RankIconItem.transform.SetParent(RankIconDefaultItem.transform.parent, false);
                    RankIconItem.SetActive(true);

                    m_RankIconList[i] = RankIconItem.GetComponent<Image>();
                }

                // 当前拥有星级
                for(int j = 0; j < nRankStar; ++j)
                {
                    m_RankIconList[j].sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankStarImage, WndID.WND_ID_WAR_RECORDTABLE_END, 2);
                }

                if(selfData.nUpgradeRank == 0) // 段位未发生改变
                {
                    // 升星级个数
                    int nDiff = nRankStar - nOldRankStar;
                    if (nDiff < 0)
                    {
                        StarUpDesc.gameObject.SetActive(true);
                        StarUpDesc.text = String.Format("星级{0}", nDiff);
                    }
                    else if (nDiff > 0)
                    {
                        StarUpDesc.gameObject.SetActive(true);
                        StarUpDesc.text = String.Format("星级+{0}", nDiff);
                    }
                    else
                    {
                        StarUpDesc.gameObject.SetActive(false);
                    }
                }
                else if(selfData.nUpgradeRank < 0)
                {
                    StarUpDesc.gameObject.SetActive(true);
                    StarUpDesc.text = String.Format("星级-{0}", 1);
                }
                else if (selfData.nUpgradeRank > 0)
                {
                    StarUpDesc.gameObject.SetActive(true);
                    StarUpDesc.text = String.Format("星级+{0}", 1);
                }
            }
            else
            {
                RankScoreTip.gameObject.SetActive(false);
            }

            SSchemeWarStatisticMapConfig warStatConfig = ASpeedGame.Data.WarStatisticMap.WarStatisticMapConfig.Instance.getWarStatMapConfig(selfData.nMatchType);
            if(warStatConfig != null)
            {
                MatchTypeText.text = warStatConfig.strName;
            }
            else
            {
                MatchTypeText.text = "";
            }
        }
    }

    // 装备
    public class UEquipInfo : MonoBehaviour
    {
        public Image EquipImage;

        public void Awake()
        {
        }

        public void setData(int nEquipID)
        {
            if (nEquipID <= 0)
                return;

            gameObject.SetActive(true);

            SSchemeMobaGoods schemeMobaGoods;
            LogicDataCenter.warMobaGoodsDataManager.GetMobaGoods_Out(nEquipID, out schemeMobaGoods);

            int nEquipIcon = schemeMobaGoods.GoodsIconID;

            setEquipPic(nEquipIcon);
            setEquipTips(schemeMobaGoods);
        }
        private void setEquipPic(int nEquipID)
        {
            if (EquipImage == null)
            {
                Debug.LogError(" setEquipPic SkillImage = null");
                return;
            }
            EquipImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MobaGoods, WndID.WND_ID_WAR_RECORDTABLE_END, 1, nEquipID);
            if (EquipImage.sprite != null)
            {
                EquipImage.gameObject.SetActive(true);
            }
        }

        private void setEquipTips(SSchemeMobaGoods goods)
        {
            UTooltipTrigger equipTipsTrigger = EquipImage.GetComponent<UTooltipTrigger>();
            if (equipTipsTrigger == null)
                return;

            int nEquipID = goods.GoodsID;
            string equipDesc = goods.GoodsDes;

            if (nEquipID <= 0)
            {
                equipTipsTrigger.enabled = false;
                return;
            }

            equipTipsTrigger.enabled = true;

            string sGoodsDes = equipDesc;
            UBBProperty.GetInstance().getRatio(sGoodsDes);
            UBB.toHtml(ref sGoodsDes, UBB_FORMAT_TYPE.UGUI);
            equipTipsTrigger.SetText(UTooltipParamName.ItemDesc, sGoodsDes);

            string sGoodsName = goods.GoodsName;
            UBB.toHtml(ref sGoodsName, UBB_FORMAT_TYPE.UGUI);
            equipTipsTrigger.SetText(UTooltipParamName.ItemName, sGoodsName);
            equipTipsTrigger.SetText(UTooltipParamName.ItemCost, goods.GoodsCost.ToString());
        }
    }

    // 召唤师技能
    public class USummonerInfo : MonoBehaviour
    {
        Image SkillImage;
        public void Start()
        {

        }

        public void Awake()
        {
            SkillImage = this.transform.FindChild("SkillImage").GetComponent<Image>();
        }

        public void setSummonerPic(int nSummonerID)
        {
            if (SkillImage == null)
            {
                Debug.LogError(" setSummonerPic SkillImage = null");
                return;
            }

            SkillImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_WAR_RECORDTABLE_END, 1, nSummonerID);
            if (SkillImage.sprite != null)
            {
                SkillImage.gameObject.SetActive(true);
            }
        }
    }

    public class UVSInfoPlayerInfo : MonoBehaviour
    {
        public GameObject SummonerSkillGroup;
        public GameObject TitleGroup;
        public GameObject EquipTemplate;
        public Sprite UIDefaultSprite;

        Image heroHead;
        //Image normalFrame;
        Text playerName;
        Text KDA;
        Text gameMoney;
        Text ScoreText;
        GameObject selfImage;
        //Text heroName;
        //Text heroLv;
        //Image mvp;

        ResNode SummonerReqRes;
        //ResNode EquipReqRes;
        ResNode TitleObjRes;

        private const int EquipCount = 6;
        private UEquipInfo[] EquipInfos;

        public void Start()
        {

        }

        public void ResDestroy()
        {
            AssetBundleManager.DeleteAssets(ref SummonerReqRes, true);
            AssetBundleManager.DeleteAssets(ref TitleObjRes, true);
        }

        public void SetInitData()
        {
            heroHead.sprite = UIDefaultSprite;
            playerName.text = "-";
            KDA.text = "-";
            gameMoney.text = "-";
            ScoreText.text = "-";
        }

        public void initVSPlayerInfo()
        {
            SummonerReqRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/DynamicState_Moba/WarRecordTable_End/SummonerSkill");
            //EquipReqRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/DynamicState_Moba/WarRecordTable_End/EquipInfo");
            TitleObjRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/DynamicState_Moba/WarRecordTable_End/Title");

            heroHead = this.transform.FindChild("HeroIcon").GetComponent<Image>();
            //normalFrame = this.transform.FindChild("HeadFrame").FindChild("Image").GetComponent<Image>();
            playerName = this.transform.FindChild("ActorName").GetComponent<Text>();
            KDA = this.transform.FindChild("KDA").GetComponent<Text>();
            gameMoney = this.transform.FindChild("Money").GetComponent<Text>();
            ScoreText = this.transform.FindChild("Score").GetComponent<Text>();
            //heroLv = this.transform.FindChild("HeroLv").FindChild("Text").GetComponent<Text>();
            selfImage = this.transform.FindChild("SelfImage").gameObject;

            EquipInfos = new UEquipInfo[EquipCount];
            for (int i = 0; i < EquipCount; ++i)
            {
                GameObject equipItemObj = ResNode.InstantiateRes(EquipTemplate);
                if (!equipItemObj)
                    return;

                equipItemObj.SetActive(true);
                equipItemObj.name = "EquipItem" + (i + 1);
                equipItemObj.transform.SetParent(EquipTemplate.transform.parent, false);
                EquipInfos[i] = equipItemObj.GetComponent<UEquipInfo>();
            }


            // 设置初始状态,如果预制体是当从缓存中取出的，则还保留上次的数据
            SetInitData();
        }

        public void setPlayerInfo(cmd_entity_end_player_data cmdData, int nScoreTop, int nMoneyTop, int nKillTop, int nDieTop, int nAssTop)
        {
            //normalFrame.gameObject.SetActive(false);
            heroHead.gameObject.SetActive(true);
            heroHead.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_RECORDTABLE_END, 1, 7, cmdData.nIconId);

            if(cmdData.bIsSelf > 0)
            {
                selfImage.SetActive(true);
                playerName.text = String.Format("<color=#55ea91>{0}</color>", cmdData.szPlayerName);
            }
            else
            {
                selfImage.SetActive(false);
                playerName.text = cmdData.szPlayerName;
            }

            // 击杀比
            int nKillNum = cmdData.gzAllData[(int)ERecord_Data_Type.ERDT_KILLNUM];
            int nDeadNum = cmdData.gzAllData[(int)ERecord_Data_Type.ERDT_DEADNUM];
            int nAssNum = cmdData.gzAllData[(int)ERecord_Data_Type.ERDT_ASSIST];
            string strKillNum , strDeadNum, strAssistNum;
            if (nKillTop != 0 && nKillNum == nKillTop)
                strKillNum = "<color=#FF6E2E>" + nKillNum + "</color>";
            else
                strKillNum = nKillNum.ToString();

            if (nDieTop != 0 && nDeadNum == nDieTop)
                strDeadNum = "<color=#FF6E2E>" + nDeadNum + "</color>";
            else
                strDeadNum = nDeadNum.ToString();

            if (nAssTop != 0 && nAssNum == nAssTop)
                strAssistNum = "<color=#FF6E2E>" + nAssNum + "</color>";
            else
                strAssistNum = nAssNum.ToString();
            KDA.text = String.Format("{0}/{1}/{2}", strKillNum, strDeadNum, strAssistNum);

            int nMoney = cmdData.gzAllData[(int)ERecord_Data_Type.ERDT_MONEY];
            if (nMoney == nMoneyTop)
                gameMoney.text = "<color=#FF6E2E>" + nMoney + "</color>";
            else
                gameMoney.text = nMoney.ToString();

            int nScore = cmdData.nScore;
            if (nScore == nScoreTop)
                ScoreText.text = "<color=#FF6E2E>" + nScore + "</color>";
            else
                ScoreText.text = nScore.ToString();

            int nLv = LogicDataCenter.recordTableDataManager.getHeroLvAcordPDBID(cmdData.pDbid);
            //heroLv.text = nLv.ToString();

            // 加载召唤师技能预制体
            if (SummonerReqRes == null)
            {
                Debug.LogError("setPlayerInfo req = null");
                return;
            }
            // 现在只有一个召唤师技能
            GameObject instance = SummonerReqRes.InstanceMainRes();
            if (!instance)
            {
                Debug.LogError("setPlayerInfo instance = NULL");
                return;
            }
            instance.name = SummonerReqRes.AssetName;
            instance.transform.SetParent(SummonerSkillGroup.transform, false);
            USummonerInfo summonerInfo = instance.GetComponent<USummonerInfo>();
            if (summonerInfo != null)
                summonerInfo.setSummonerPic(cmdData.nTalentSkillId1);

            // 装备
            List<cmd_war_persongoodsinfo> listEquipInfo = LogicDataCenter.warMobaGoodsDataManager.GetPlayerGoodsInfo(cmdData.pDbid);
            for (int i = 0; i < EquipCount; ++i)
            {
                UEquipInfo EquipInfo = EquipInfos[i];
                if (EquipInfo != null && listEquipInfo != null && i < listEquipInfo.Count)
                {
                    EquipInfo.setData(listEquipInfo[i].nGoodID);
                }
            }

            // 设置荣誉信息
            int nTitle = cmdData.nTitle;
            if ((nTitle & (int)EWarMedal.EWM_SCORE) > 0)
                AddTitleIcon((int)AchiIconType.Icon_MVP);
            if ((nTitle & (int)EWarMedal.EWM_OUTPUT) > 0)
                AddTitleIcon((int)AchiIconType.Icon_TopDamage);
            if ((nTitle & (int)EWarMedal.EWM_KILLCOUNT) > 0)
                AddTitleIcon((int)AchiIconType.Icon_TopKill);
            if ((nTitle & (int)EWarMedal.EWM_ASSCOUNT) > 0)
                AddTitleIcon((int)AchiIconType.Icon_TopAssist);
            if ((nTitle & (int)EWarMedal.EWM_DAMAGE) > 0)
                AddTitleIcon((int)AchiIconType.Icon_TopTank);
            if ((nTitle & (int)EWarMedal.EWM_DESTORYTOWER) > 0)
                AddTitleIcon((int)AchiIconType.Icon_TopTowerKill);
            if ((nTitle & (int)EWarMedal.EWM_MONEY) > 0)
                AddTitleIcon((int)AchiIconType.Icon_TopRich);
            // 不显示补兵最多图标
            //if ((nMedal & (int)EWarMedal.EWM_KILLMONSTER) > 0)
            //    AddTitleIcon((int)AchiIconType.Icon_TopFarm);
            if ((nTitle & (int)EWarMedal.EWM_DEADLYCONTROL) > 0)
                AddTitleIcon((int)AchiIconType.Icon_DeadlyControl);
            if ((nTitle & (int)EWarMedal.EWM_THRILLSAVE) > 0)
                AddTitleIcon((int)AchiIconType.Icon_ThrilSave);
            if ((nTitle & (int)EWarMedal.EWM_TEAMCONTROL) > 0)
                AddTitleIcon((int)AchiIconType.Icon_TeamControl);
            if (cmdData.gzAllData[(int)ERecord_Data_Type.ERDT_ALIVECOUTASSISTMAX] >= 15)
                AddTitleIcon((int)AchiIconType.Icon_AliveAssist);
            if (cmdData.gzAllData[(int)ERecord_Data_Type.ERDT_ALIVECONTKILLMAX] >= 8)
                AddTitleIcon((int)AchiIconType.Icon_Godlike);
            if (cmdData.gzAllData[(int)ERecord_Data_Type.ERDT_TIMEKILLMAX] == 3)
                AddTitleIcon((int)AchiIconType.Icon_TripleKill);
            if (cmdData.gzAllData[(int)ERecord_Data_Type.ERDT_TIMEKILLMAX] == 4)
                AddTitleIcon((int)AchiIconType.Icon_FourKill);
            if (cmdData.gzAllData[(int)ERecord_Data_Type.ERDT_TIMEKILLMAX] >= 5)
                AddTitleIcon((int)AchiIconType.Icon_FiveKill);
        }

        // 创建荣耀预制体，并设置信息
        private void AddTitleIcon(int nIconIndex)
        {
            if (nIconIndex < 0)
                return;
            GameObject instance = TitleObjRes.InstanceMainRes();
            if (instance)
            {
                Image icon = instance.GetComponent<Image>();
                icon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_WarResultShow, "WarAchiType_" + nIconIndex, WndID.WND_ID_WAR_RECORDTABLE_END);
                instance.transform.SetParent(TitleGroup.transform, false);
                string stTitleName = "Title_" + nIconIndex;
                UTooltipTrigger heroTipsTrigger = instance.GetComponent<UTooltipTrigger>();
                string stTitleDesc = ULocalizationService.Instance.Get("ToolTips", "RecordTableEnd", stTitleName);
                heroTipsTrigger.SetText(UTooltipParamName.BodyText, stTitleDesc);
            }
        }
    }
}
