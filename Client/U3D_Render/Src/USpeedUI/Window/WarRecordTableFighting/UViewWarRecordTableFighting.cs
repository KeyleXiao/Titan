/*******************************************************************
** 文件名:	UViewWarRecordTableFighting.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016-3-31
** 版  本:	1.0
** 描  述:	战场中战绩表显示界面
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using War;
using UIWidgets;
using GameLogic;
using DataCenter;
using ASpeedGame.Data.GameMobaSchemes;
using U3D_Render.Common;
using ASpeedGame.Data.HeroTalentConfig;

namespace USpeedUI
{
    public class EquipFrame:MonoBehaviour,UnityEngine.EventSystems.IPointerEnterHandler
    {
        Image goodsImage;
        Text goodsCount;
        string m_sEquipDesc = null;
        private UTooltipTrigger playerEquipTipsTrigger;

        public void Awake()
        {
            goodsImage = transform.FindChild("GoodsImage").GetComponent<Image>();
            goodsCount = transform.FindChild("GoodsCount").GetComponent<Text>();
            playerEquipTipsTrigger = goodsImage.GetComponent<UTooltipTrigger>();
        }

        public void Destroy()
        {
			if(goodsImage != null)
				goodsImage.gameObject.SetActive(false);
			if(goodsCount != null)
				goodsCount.text = "0";
        }

        public void setEquipInfo(cmd_war_persongoodsinfo cmdGoodsInfo, SSchemeMobaGoods schemeMobaGoods)
        {
            // 物品图标
            int nEquipIcon = schemeMobaGoods.GoodsIconID;
            playerEquipTipsTrigger.enabled = nEquipIcon > 0;
            this.goodsImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MobaGoods, WndID.WND_ID_WAR_RECORDTABLE_FIGHTING, 1, nEquipIcon);
            if (this.goodsImage.sprite == null && this.goodsImage.gameObject.activeSelf)
            {
                this.goodsImage.gameObject.SetActive(false);
            }
            else if (this.goodsImage.sprite && this.goodsImage.gameObject.activeSelf == false)
            {
                this.goodsImage.gameObject.SetActive(true);
            }

            // 物品数量
            int nTimes = cmdGoodsInfo.nCanUseTimes;
            if (nTimes != 0)
            {
                goodsCount.gameObject.SetActive(true);
            }
            else
            {
                goodsCount.gameObject.SetActive(false);
            }
            this.goodsCount.text = nTimes.ToString();

            m_sEquipDesc = schemeMobaGoods.GoodsDes;

            UBB.toHtml(ref schemeMobaGoods.GoodsName, UBB_FORMAT_TYPE.UGUI);
            playerEquipTipsTrigger.SetText(UTooltipParamName.ItemName, schemeMobaGoods.GoodsName);
            playerEquipTipsTrigger.SetText(UTooltipParamName.ItemCost, schemeMobaGoods.GoodsSoldOutCost.ToString());
        }

        public void OnPointerEnter(UnityEngine.EventSystems.PointerEventData eventData)
        {
            if (string.IsNullOrEmpty(m_sEquipDesc) || playerEquipTipsTrigger == null)
            {
                return;
            }

            string sGoodsDes = m_sEquipDesc;
            UBBProperty.GetInstance().getRatio(sGoodsDes);
            UBB.toHtml(ref sGoodsDes, UBB_FORMAT_TYPE.UGUI);
            playerEquipTipsTrigger.SetText(UTooltipParamName.ItemDesc, sGoodsDes);
        }
    }

	public class TalentFrame : MonoBehaviour, UnityEngine.EventSystems.IPointerEnterHandler
	{
		public Image talentImage;

		private string m_sTalentDesc = "";
		private string m_sTalentName = "";
		private UTooltipTrigger tipsTrigger;

		public void Awake()
		{
			tipsTrigger = talentImage.GetComponent<UTooltipTrigger>();
		}

        public void Destroy()
		{
			if (talentImage != null)
				talentImage.gameObject.SetActive(false);
		}

		public void setTalentInfo(int nHeroID, int nTalentID)
		{
			SSchemeHeroTalentConfig talentConfig = HeroTalentConfigPage.Instance.GetHeroTalentConfig(nHeroID, nTalentID);
			if (talentConfig == null)
				return;

			m_sTalentDesc = talentConfig.sTalenDescInWar;
			m_sTalentName = talentConfig.sTalentName;

			int talentIconID = talentConfig.nTalentIconId;

			talentImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_WAR_RECORDTABLE_FIGHTING, 1, talentIconID);
			talentImage.gameObject.SetActive(true);
		}

		public void OnPointerEnter(UnityEngine.EventSystems.PointerEventData eventData)
		{
			if (string.IsNullOrEmpty(m_sTalentDesc) || tipsTrigger == null) {
				return;
			}

			string sTalentDesc = m_sTalentDesc;

			UBBProperty.GetInstance().getRatio(sTalentDesc);
			U3D_Render.Common.UBB.toHtml(ref sTalentDesc, UBB_FORMAT_TYPE.UGUI);
			tipsTrigger.SetText("TalentDesc", sTalentDesc);

			string sTalentName = m_sTalentName;
			U3D_Render.Common.UBB.toHtml(ref sTalentName, UBB_FORMAT_TYPE.UGUI);
			tipsTrigger.SetText("TalentName", sTalentName);
		}
	}

	public class WarRecordSummerSkillFrame : MonoBehaviour, UnityEngine.EventSystems.IPointerEnterHandler
    {
        public Image summerImage;
        string m_sSummerDesc = null;
        public UTooltipTrigger summerTipsTrigger;
        public void OnPointerEnter(UnityEngine.EventSystems.PointerEventData eventData)
        {
            if ( summerTipsTrigger == null)
            {
                return;
            }

            if (string.IsNullOrEmpty(m_sSummerDesc))
            {
                //summerTipsTrigger.SetText(UTooltipParamName.BodyText, "The SummerSkill Is Can't Find");
                return;
            }

            string sSummerDesc = m_sSummerDesc;
            UBBProperty.GetInstance().getRatio(sSummerDesc);
            UBB.toHtml(ref sSummerDesc, UBB_FORMAT_TYPE.UGUI);

            summerTipsTrigger.SetText(UTooltipParamName.BodyText, sSummerDesc);
        }

        public void SetSummerInfo(string sDesc, int nIconID)
        {
            if (summerImage == null)
            {
                return;
            }

            summerTipsTrigger.enabled = !string.IsNullOrEmpty(sDesc);
            summerImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_WAR_RECORDTABLE_FIGHTING, 1, nIconID);
            m_sSummerDesc = sDesc;
        }
    }

    public class PlayerRecordInfo : TimeCountDown
    {
        public Image            heroHead;
        public Text             playerName;
        public Text             heroLv;
        public Text             playerState;
        public Text             playerGold;
        public Text[]           goodsTime;
        public Image[]          EqupImage;
        public Image            playerBackGround;
        public Text             Countdown;
        public WarRecordSummerSkillFrame SummerFrame;
		public GameObject Frame;
		public GameObject goodsGroup;
		public GameObject talentGroup;
        public Image            SexImage;

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

        // 设置英雄死亡倒计时
        public void setHeroDieCountdown(int nReliveTime)
        {
            if (nReliveTime != 0 )
            {
                heroHead.color = Color.gray;
                Countdown.gameObject.SetActive(true);
            }
            Countdown.text = nReliveTime.ToString();
            this.StartCountDown(nReliveTime, 1.0f, 1.0f);
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
            if (SummerFrame == null || heroHead == null || heroLv == null || playerState == null || playerGold == null || goodsTime == null || EqupImage == null || playerBackGround == null || Countdown == null)
            {
                return;
            }

			if (personInfo.uPDBID > 0)
				gameObject.SetActive(true);
				

			// 初始化玩家数据
			SummerFrame.SetSummerInfo(personInfo.sSummerDesc,personInfo.nSummerIconID);
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

            if (!playerBackGround.gameObject.activeSelf && personInfo.bIsHero != 0 )
            {
                playerBackGround.gameObject.SetActive(true);
            }

            // 初始化装备格子
            goodsFrame = new EquipFrame[(int)SLOT_DEFINE.D_MAX_GOODS_SLOT_COUNT];
			for (int i = 0; i < UDefines.WAR_ITEM_GRID_COUNT; ++i)
            {
                goodsFrame[i] = goodsGroup.transform.FindChild("Equip" + i).GetComponent<EquipFrame>();
            }

			// 初始化天赋格子
			talentFrame = new TalentFrame[(int)UDefines.WAR_LEARN_TALENT_COUNT];
			for(int i = 0; i < UDefines.WAR_LEARN_TALENT_COUNT; ++i) 
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
            playerBackGround.gameObject.SetActive(false);
            m_isInit = false;
        }

        // 设置玩家信息
        public void setPlayerInfo(WarRecordTableDataManager.WarPersonInfo personInfo, int nMoneyTop, int nKillTop, int nDieTop, int nAssTop, bool isSelfCamp)
        {
            if(!m_isInit)
            {
                Debug.LogError("There is not init player info.");
                return;
            }

			// 设置玩家信息
            heroLv.text = personInfo.nLevel.ToString();

			if(isSelfCamp)
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
                if(playerInfo != null)
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
			if(LogicDataCenter.heroTalentDataManager.getActiveTalentList(uPDBID, out listTalent)) 
			{
				for(int i = 0; i < listTalent.nTalentList.Length; ++i) 
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
    }

    public enum EN_WarRecordTableTimeId
    {
        TIMEID_BOSSCDTIME = 0,      // 怪物倒计时时间
    }


    public class UViewWarRecordTableFighting:UIBaseWndView,ITimerHanlder
    {
		public Transform RecordGroup_Left;
		public Transform RecordGroup_Right;
		public Text BlueScore;
        public Text RedScore;
        public Text[] CDTime = new Text[6];
		public GameObject LvTitle;
		public Button ShowTalentBtn;
		public Button ShowEquipBtn;

        ResNode reqRes;
        PlayerRecordInfo[] playerRecordInfo;

		// Boss CD时间
		private DateTime[] nBossReliveTime = new DateTime[6];
		private bool[] isBossRefresh = new bool[6];

		public override void Start()
        {
            TimerManager.SetTimer(this, (int)EN_WarRecordTableTimeId.TIMEID_BOSSCDTIME, 1.0f);
            base.Start();
        }

        public override void Awake()
        {
            playerRecordInfo = new PlayerRecordInfo[UDefines.WAR_PERSON_MAX_COUNT];
            reqRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/DynamicState_Moba/WarRecordTable_Warning/PlayerRecordInfo");

			// 创建战绩表框
			if (RecordGroup_Left == null || RecordGroup_Right == null)
                return;

			GameUtil.DestroyChild(RecordGroup_Left);
			GameUtil.DestroyChild(RecordGroup_Right);

			for (int i = 0; i < UDefines.WAR_PERSON_MAX_COUNT; ++i)
            {
                GameObject instance = reqRes.InstanceMainRes();
                if (!instance)
                {
                    Trace.LogError("Awake instance = NULL");
                    return;
                }
                instance.name = reqRes.AssetName;

				if(i < (UDefines.WAR_PERSON_MAX_COUNT / 2))
				{
					instance.transform.SetParent(RecordGroup_Left, false);
				}
				else
				{
					instance.transform.SetParent(RecordGroup_Right, false);
				}

                playerRecordInfo[i] = instance.GetComponent<PlayerRecordInfo>();
            }

            base.Awake();
        }

        public override void Destroy()
        {
            TimerManager.KillTimer(this, (int)EN_WarRecordTableTimeId.TIMEID_BOSSCDTIME);

			if(playerRecordInfo != null)
			{
				foreach (var item in playerRecordInfo)
				{
					item.Destroy();
				}
			}

			GameUtil.DestroyChild(RecordGroup_Left);
			GameUtil.DestroyChild(RecordGroup_Right);

            AssetBundleManager.DeleteAssets(ref reqRes, true);

            base.Destroy();
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)EN_WarRecordTableTimeId.TIMEID_BOSSCDTIME:
                    {
                        onBossTimerComplete();
                    }
                    break;
                default:
                    break;
            }
        }

        protected void onBossTimerComplete()
        {
            for ( int i = 0; i < 6; ++i )
            {
				TimeSpan remainTime = nBossReliveTime[i] - DateTime.Now;
				int nSeconds = (int)remainTime.TotalSeconds;

				if (isBossRefresh[i])
					continue;

                if (nSeconds <= 0)
                {
					isBossRefresh[i] = true;
                    CDTime[i].text = "已刷新";
                }
                else
                {
                    CDTime[i].text = formatTime(nSeconds);
                }
            }
        }

        public void ShowTeamScore(int nBlueSumKill, int nRedSumKill)
        {
            BlueScore.text = nBlueSumKill.ToString();
            RedScore.text = nRedSumKill.ToString();
        }

        public void SetTeamBossReliveTime(int nBossIndex, int nReliveTime)
        {
            /*
             * index    BossName
             *   0      T1_BlueBoss
             *   1      T1_RedBoss
             *   2      T3_VioletBoss1
             *   3      T3_VioletBoss2
             *   4      T2_RedBoss
             *   5      T2_BlueBoss
             * 
             */
            nBossIndex = nBossIndex - 1;

            if (nBossIndex > 5 || nBossIndex < 0)
            {
                return;
            }

			//nCDTime[nBossIndex] = nReliveTime / 1000;
			//nReliveTime[nBossIndex] = Api.convertDateTime2TimeStamp(DateTime.Now);
			nBossReliveTime[nBossIndex] = DateTime.Now.AddMilliseconds(nReliveTime);

			CDTime[nBossIndex].text = formatTime(nReliveTime / 1000);
			isBossRefresh[nBossIndex] = false;
		}

        // 设置英雄死亡复活时间
        public void setHeroDieCountdown(ulong uPdbid, int nReliveTime)
        {
            int nIndex = LogicDataCenter.recordTableDataManager.getIndexAcordPDBID(uPdbid);
            if (nIndex < 0 || nIndex > playerRecordInfo.Length)
            {
                return;
            }

            playerRecordInfo[nIndex].setHeroDieCountdown(nReliveTime);
        }

        // 初始化战绩表
        public void initWarRecordTable()
        {
            // 对战绩表进行赋值
            BlueScore.text = "0";
            RedScore.text = "0";

            foreach (PlayerRecordInfo recInfo in playerRecordInfo)
            {
                recInfo.resetPlayerInfo();
            }

            Dictionary<ulong, WarRecordTableDataManager.WarPersonInfo> dicPersonInfo = LogicDataCenter.recordTableDataManager.recordInfoArry;
            foreach (KeyValuePair<ulong, WarRecordTableDataManager.WarPersonInfo> kvp in dicPersonInfo)
            {
                int nIndex = LogicDataCenter.recordTableDataManager.getIndexAcordPDBID(kvp.Value.uPDBID);
                playerRecordInfo[nIndex].initPlayerInfo(kvp.Value);
            }
        }


        // 显示战绩表s
        public void showRecordTable()
        {
            Dictionary<ulong, WarRecordTableDataManager.WarPersonInfo> dicPersonInfo = LogicDataCenter.recordTableDataManager.recordInfoArry;
            if ( dicPersonInfo == null )
                return;

            int nScoreTop = 0;
            int nMoneyTop = 0;
            int nKillTop = 0;
            int nDieTop = 0;
            int nAssistTop = 0;
            LogicDataCenter.recordTableDataManager.getDataTop(ref nScoreTop, ref nMoneyTop, ref nKillTop, ref nDieTop, ref nAssistTop);

            int nSelfTeamScore = 0;
            int nEnemyTeamScore = 0;
            int nMainCamp = LogicDataCenter.recordTableDataManager.getMainHeroCamp();
            foreach (KeyValuePair<ulong, WarRecordTableDataManager.WarPersonInfo> kvp in dicPersonInfo)
            {
				// 是否自己阵营
				bool isSelfCamp = (kvp.Value.nCamp == nMainCamp);

                int nIndex = LogicDataCenter.recordTableDataManager.getIndexAcordPDBID(kvp.Value.uPDBID);
                if (nIndex < 0 || nIndex >= playerRecordInfo.Length || playerRecordInfo[nIndex] == null)
                    continue;

                playerRecordInfo[nIndex].setPlayerInfo(kvp.Value, nMoneyTop, nKillTop, nDieTop, nAssistTop, isSelfCamp);

                if (isSelfCamp)
                    nSelfTeamScore += kvp.Value.nKillNum;
                else
                    nEnemyTeamScore += kvp.Value.nKillNum;
            }

            //设置总比分
            ShowTeamScore(nSelfTeamScore, nEnemyTeamScore);
        }

		// 显示天赋
		public void showTalent()
		{
			LvTitle.SetActive(true);
			ShowEquipBtn.gameObject.SetActive(true);
			ShowTalentBtn.gameObject.SetActive(false);

			foreach(var item in playerRecordInfo) 
			{
				item.showTalent();
			}
		}

		// 显示装备
		public void showEquip()
		{
			LvTitle.SetActive(false);
			ShowEquipBtn.gameObject.SetActive(false);
			ShowTalentBtn.gameObject.SetActive(true);

			foreach(var item in playerRecordInfo) 
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
    }
}
