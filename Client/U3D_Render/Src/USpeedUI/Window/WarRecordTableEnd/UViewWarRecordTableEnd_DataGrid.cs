/*******************************************************************
** 文件名:	UViewWarRecordTableEnd_DataGrid.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2017-9-12
** 版  本:	1.0
** 描  述:	战后战绩表显示界面-数据网格
		
********************************************************************/
using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using GameLogic;
using DataCenter;
using War;


namespace USpeedUI
{
    // 数据网格信息
    public partial class UViewWarRecordTableEnd : UIBaseWndView, ITimerHanlder
    {
        private UDataGrid DataGrid;

        private Transform DataGridTrans;

        private void initDataGridFrame()
        {
            DataGrid = DataGridTrans.GetComponent<UDataGrid>();
            if(DataGrid == null)
            {
                Debug.LogError("DataGrid == null");
                return;
            }

            DataGrid.Init();
        }

        private void destroyDataGridFrame()
        {
            DataGrid.Destroy();
        }

        // 设置单个玩家的数据网络信息
        public void setDataGridPlayerInfo(cmd_entity_end_player_data cmdData, int[] gzAllDataTop)
        {
            DataGrid.setDataGridPlayerInfo(cmdData, gzAllDataTop);
        }

    }

    public class UDataGrid : MonoBehaviour
    {
        // 战绩,数据表格
        public Transform BlueTeamTrans;
        public Transform RedTeamTrans;
        public GameObject PlayerDefaultItem;


        public Transform BlueTeamHead;
        public Transform RedTeamHead;
        public GameObject HeadDefaultItem;


        public Text[] TitleLabel;
        public Transform DataArrayLabel1;
        public Transform DataArrayLabel2;
        public Transform DataArrayLabel3;

        private UDataGridPlayer[] DataGridPlayer;
        private GameObject[] PlayerHead;


        private int nCurDataGridBlueIndex = 0;
        private int nCurDataGridRedIndex = 0;

        public void Init()
        {

            DataGridPlayer = new UDataGridPlayer[UDefines.WAR_PERSON_MAX_COUNT];
            PlayerHead = new GameObject[UDefines.WAR_PERSON_MAX_COUNT];

            for (int i = 0; i < UDefines.WAR_PERSON_MAX_COUNT; ++i)
            {
                // 玩家战绩信息
                GameObject instance = ResNode.InstantiateRes(PlayerDefaultItem);
                if (instance == null)
                {
                    Debug.LogError("URecordtableEnd2Player instance  = NULL");
                    //return;
                }

                GameObject headGo = ResNode.InstantiateRes(HeadDefaultItem);
                if (headGo == null)
                {
                    Debug.LogError("HeadDefaultItem instance  = NULL");
                    return;
                }

                if (i < 5)
                {
                    instance.transform.SetParent(BlueTeamTrans.transform, false);
                    headGo.transform.SetParent(BlueTeamHead.transform, false);
                }
                else
                {
                    instance.transform.SetParent(RedTeamTrans.transform, false);
                    headGo.transform.SetParent(RedTeamHead.transform, false);
                }

                instance.gameObject.SetActive(true);
                headGo.SetActive(true);

                DataGridPlayer[i] = instance.GetComponent<UDataGridPlayer>();
                DataGridPlayer[i].Init();

                PlayerHead[i] = headGo;
            }

            // 初始化Label
            TitleLabel[0].text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "BattleStat");
            TitleLabel[1].text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "TeamContribution");
            TitleLabel[2].text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "GoodBehaviour");

            DataArrayLabel1.FindChild("Text1").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "KillNumDesc");
            DataArrayLabel1.FindChild("Text2").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "DieNumDesc");
            DataArrayLabel1.FindChild("Text3").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "AssistNumDesc");
            DataArrayLabel1.FindChild("Text4").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ScoreDesc");
            DataArrayLabel1.FindChild("Text5").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "GPM");
            DataArrayLabel1.FindChild("Text6").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "FirstBlood");
            DataArrayLabel1.FindChild("Text7").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "DeadlyControl");

            DataArrayLabel2.FindChild("Text1").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "MoneyRateRate");
            DataArrayLabel2.FindChild("Text2").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "OutputRate");
            DataArrayLabel2.FindChild("Text3").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "JoinBattleRate");
            DataArrayLabel2.FindChild("Text4").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "OutDamageTotal");
            DataArrayLabel2.FindChild("Text5").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "OutDamagePhysics");
            DataArrayLabel2.FindChild("Text6").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "OutDamageMagic");
            DataArrayLabel2.FindChild("Text7").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "OutDamageTrue");
            DataArrayLabel2.FindChild("Text8").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "BearDamageTotal");
            DataArrayLabel2.FindChild("Text9").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "BearDamagePhysics");
            DataArrayLabel2.FindChild("Text10").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "BearDamageMagic");
            DataArrayLabel2.FindChild("Text11").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "BearDamageTrue");
            DataArrayLabel2.FindChild("Text12").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "GetMoney");
            DataArrayLabel2.FindChild("Text13").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "CureTotal");
            DataArrayLabel2.FindChild("Text14").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "RecoveryHP");

            DataArrayLabel3.FindChild("Text1").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "MultiKillMax");
            DataArrayLabel3.FindChild("Text2").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ContinueKillMax");
            DataArrayLabel3.FindChild("Text3").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "AliveAssistMax");
            DataArrayLabel3.FindChild("Text4").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ThrillSave");
            DataArrayLabel3.FindChild("Text5").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "CrildDamageMax");
            DataArrayLabel3.FindChild("Text6").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "KillTowerNum");
            DataArrayLabel3.FindChild("Text7").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "KillSmallDragen");

            nCurDataGridBlueIndex = 0;
            nCurDataGridRedIndex = UDefines.WAR_PERSON_MAX_COUNT / 2;
        }

        public void Destroy()
        {
            GameUtil.DestroyChild(BlueTeamTrans.transform);
            GameUtil.DestroyChild(RedTeamTrans.transform);

            GameUtil.DestroyChild(BlueTeamHead.transform);
            GameUtil.DestroyChild(RedTeamHead.transform);
        }

        public void setDataGridPlayerInfo(cmd_entity_end_player_data cmdData, int[] gzAllDataTop)
        {
            int nMainHeroID = EntityUtil.getMainHeroHeroID();
            ulong uPDBID = cmdData.pDbid;

            int nIndex = LogicDataCenter.recordTableDataManager.getIndexAcordPDBID(uPDBID);
            if (nIndex == -1)
            {
                return;
            }

            int i = 0;
            if (nIndex < UDefines.WAR_PERSON_MAX_COUNT / 2)
            {
                i = nCurDataGridBlueIndex++;
            }
            else
            {
                i = nCurDataGridRedIndex++;
            }

            // 玩家战绩
            if (DataGridPlayer[i] != null)
            {
                DataGridPlayer[i].setPlayerInfo(cmdData, gzAllDataTop);
            }

            // 玩家头像
            if (PlayerHead[i] != null)
            {
                Sprite sp = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_RECORDTABLE_END, 1, 7, cmdData.nIconId);
                Transform headTrans = PlayerHead[i].transform.FindChild("HeroHead");
                if (headTrans != null)
                {
                    headTrans.GetComponent<Image>().sprite = sp;
                    headTrans.gameObject.SetActive(true);
                }

                if(cmdData.bIsSelf > 0)
                {
                    Transform selfImageTrans = PlayerHead[i].transform.FindChild("SelfImage");
                    if (selfImageTrans != null)
                    {
                        selfImageTrans.gameObject.SetActive(true);
                    }
                }
            }
        }

    }
    public class UDataGridPlayer : MonoBehaviour
    {
        public Transform DataArray1;    // 战斗统计
        public Transform DataArray2;    // 团队贡献
        public Transform DataArray3;    // 精彩表现
        public Image selfImage;         // 当前玩家高亮背景

        private List<Text> DataArrayText;  // 所有的战绩


        public void Start()
        {

        }

        public void Init()
        {

            DataArrayText = new List<Text>();
            for (int i = 0; i < UDefines.DataArrayNum[0]; ++i)
            {
                string name = String.Format("Text{0}", i + 1);
                DataArrayText.Add(DataArray1.FindChild(name).GetComponent<Text>());
            }

            for (int i = 0; i < UDefines.DataArrayNum[1]; ++i)
            {
                string name = String.Format("Text{0}", i + 1);
                DataArrayText.Add(DataArray2.FindChild(name).GetComponent<Text>());
            }

            for (int i = 0; i < UDefines.DataArrayNum[2]; ++i)
            {
                string name = String.Format("Text{0}", i + 1);
                DataArrayText.Add(DataArray3.FindChild(name).GetComponent<Text>());
            }

        }

        // 设置玩家的信息
        public void setPlayerInfo(cmd_entity_end_player_data cmdData, int[] gzAllDataTop)
        {
            for (int i = 0; i < (int)ERecord_Data_Type.ERDT_MAX1; ++i)
            {
                int data = cmdData.gzAllData[i];
                if (i == (int)ERecord_Data_Type.ERDT_FIRSTBLOOD)
                {
                    if (data > 0)
                        DataArrayText[i].text = String.Format("<color=#FFCE34FF>{0}</color>", ULocalizationService.Instance.Get("UIView", "WarStatistic", "IsFirstBlood"));
                    else
                        DataArrayText[i].text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "IsntFirstBlood");
                }
                else if (i == (int)ERecord_Data_Type.ERDT_MONEYRATE
                   || i == (int)ERecord_Data_Type.ERDT_JOINBATTLERATE
                   || i == (int)ERecord_Data_Type.ERDT_OUTPUTRATE)
                {
                    if (data > 0 && data == gzAllDataTop[i])
                        DataArrayText[i].text = String.Format("<color=#FFCE34FF>{0}%</color>", data);
                    else
                        DataArrayText[i].text = String.Format("{0}%", data);
                }
                else
                {
                    if (data > 0 && data == gzAllDataTop[i])
                        DataArrayText[i].text = String.Format("<color=#FFCE34FF>{0}</color>", data);
                    else
                        DataArrayText[i].text = data.ToString();
                }
            }
        }

        public void Clear()
        {

        }
    }
}
