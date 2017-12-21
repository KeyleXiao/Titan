/*******************************************************************
** 文件名:	UViewWarRecordTableEnd_DataChart.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2017-9-12
** 版  本:	1.0
** 描  述:	战后战绩表显示界面-数据图表
		
********************************************************************/
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using GameLogic;
using DataCenter;
using War;
using ASpeedGame.Data.GameMobaSchemes;
using UnityEngine.EventSystems;
using USpeedUI.UExtensions;
using DG.Tweening;

namespace USpeedUI
{
    // 数据图表
    public partial class UViewWarRecordTableEnd : UIBaseWndView, ITimerHanlder
    {
        public GameObject selfDataChartPlayerGroup;
        public GameObject enemyDataChartPlayerGroup;
        public GameObject warDataArea1;
        public GameObject warDataArea2;
        public GameObject warDataArea3;

        public Toggle ToggleHead;
        public Toggle ToggleHead2;
        public Toggle ToggleHead3;

        Transform DataChartTrans;
        ResNode DataChartPlayerReqRes;
        ResNode toggleDataRes;
        UDataChartPlayer[] DataChartPlayer;

        List<UDataChartPlayer> DataChartPlayers = new List<UDataChartPlayer>();
        Dictionary<int, Toggle> dicToggle = new Dictionary<int, Toggle>();
        Toggle tmpToggle;


        private int nCurDataChartBlueIndex = 0;
        private int nCurDataChartRedIndex = 0;

        // 初始化數據圖表的所有玩家的預製體
        private void initDataChartFrame()
        {
            DataChartPlayer = new UDataChartPlayer[UDefines.WAR_PERSON_MAX_COUNT];
            ToggleHead.onValueChanged.AddListener(onChickToggleHead);
            ToggleHead2.onValueChanged.AddListener(onChickToggleHead2);
            ToggleHead3.onValueChanged.AddListener(onChickToggleHead3);

            toggleDataRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/DynamicState_Moba/WarRecordTable_End/TogglePrefab");
            if (toggleDataRes == null)
            {
                Debug.LogError("toggleDataRes = null");
                return;
            }

            DataChartPlayerReqRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/DynamicState_Moba/WarRecordTable_End/DataChartPlayerInfo");
            // 赋值预制体
            if (DataChartPlayerReqRes == null)
            {
                Debug.LogError("DataChartPlayerReqRes end3PlayerReq = null");
                return;
            }

            for (int i = 0; i < UDefines.WAR_PERSON_MAX_COUNT; ++i)
            {
                GameObject instance = DataChartPlayerReqRes.InstanceMainRes();
                if (!instance)
                {
                    Debug.LogError("DataChartPlayerReqRes instance = NULL");
                    return;
                }
                instance.name = DataChartPlayerReqRes.AssetName;
                // 以5個玩家為分界點，將單獨玩家數據圖表創建到group中
                if (i < 5)
                    instance.transform.SetParent(selfDataChartPlayerGroup.transform, false);
                else
                    instance.transform.SetParent(enemyDataChartPlayerGroup.transform, false);
                // 將每個玩家的對象保存起來
                DataChartPlayer[i] = instance.GetComponent<UDataChartPlayer>();
            }

            // 初始化數據圖表右側的數據描述
            initToggle();

            nCurDataChartBlueIndex = 0;
            nCurDataChartRedIndex = UDefines.WAR_PERSON_MAX_COUNT / 2;
        }

        private void destroyDataChartFrame()
        {
            GameUtil.DestroyChild(selfDataChartPlayerGroup.transform);
            GameUtil.DestroyChild(enemyDataChartPlayerGroup.transform);

            ToggleHead.onValueChanged.RemoveAllListeners();
            ToggleHead2.onValueChanged.RemoveAllListeners();
            ToggleHead3.onValueChanged.RemoveAllListeners();

            AssetBundleManager.DeleteAssets(ref DataChartPlayerReqRes, true);
            AssetBundleManager.DeleteAssets(ref toggleDataRes, true);

            ToggleHead.onValueChanged.RemoveAllListeners();
            ToggleHead2.onValueChanged.RemoveAllListeners();
            ToggleHead3.onValueChanged.RemoveAllListeners();
        }

        // 設置單獨玩家數據圖表的信息
        public void setDataChartPlayerInfo(cmd_entity_end_player_data cmdData)
        {
            ulong uPDBID = cmdData.pDbid;
            // 根据pdbid找到玩家对应的位置
            int nIndex = LogicDataCenter.recordTableDataManager.getIndexAcordPDBID(uPDBID);
            if (nIndex < 0)
                return;

            int i = 0;
            if (nIndex < UDefines.WAR_PERSON_MAX_COUNT / 2)
            {
                i = nCurDataChartBlueIndex++;
            }
            else
            {
                i = nCurDataChartRedIndex++;
            }

            if (DataChartPlayer[i] != null)
            {
                DataChartPlayer[i].setPlayerInfo(cmdData, dicToggle);
                DataChartPlayers.Add(DataChartPlayer[i]);
            }
        }

        // 當點擊區域1的toggle組件
        protected void onChickToggleHead(bool bActive)
        {
            for (int i = 0; i <= (int)ERecord_Data_Type.ERDT_DEADLYCONTROL; ++i)
            {
                if (!dicToggle.ContainsKey(i))
                {
                    continue;
                }
                dicToggle[i].isOn = bActive;

                updateWarDataPic(bActive, i);
            }
        }

        // 當點擊區域2的toggle組件
        protected void onChickToggleHead2(bool bActive)
        {
            for (int i = ((int)ERecord_Data_Type.ERDT_DEADLYCONTROL + 1); i <= (int)ERecord_Data_Type.ERDT_RECOVERYHP; ++i)
            {
                if (!dicToggle.ContainsKey(i))
                {
                    continue;
                }

                dicToggle[i].isOn = bActive;
                updateWarDataPic(bActive, i);
            }
        }

        // 當點擊區域3的toggle組件
        protected void onChickToggleHead3(bool bActive)
        {
            for (int i = ((int)ERecord_Data_Type.ERDT_RECOVERYHP + 1); i <= (int)ERecord_Data_Type.ERDT_DRAGEN; ++i)
            {
                if (!dicToggle.ContainsKey(i))
                {
                    continue;
                }

                dicToggle[i].isOn = bActive;
                updateWarDataPic(bActive, i);
            }
        }

        // 初始化數據圖表右側的數據描述
        public void initToggle()
        {
            for (int i = 0; i < (int)ERecord_Data_Type.ERDT_MAX1; ++i)
            {
                GameObject instance = toggleDataRes.InstanceMainRes();
                if (!instance)
                {
                    Debug.LogError("initToggle instance = NULL");
                    return;
                }
                instance.name = toggleDataRes.AssetName + "_" + i;
                Text tName = instance.transform.FindChild("Label").GetComponent<Text>();
                SSchemeRecordConfig colorConfig = GameMobaSchemesManager.Instance.getRecordConfigAccordID(i + 1);
                if (tName != null && colorConfig != null)
                {
                    tName.text = colorConfig.stDesc;
                }
                // 根據不同的區域將預製體放在不同的group里
                if (i <= (int)ERecord_Data_Type.ERDT_DEADLYCONTROL)
                    instance.transform.SetParent(warDataArea1.transform, false);
                else if (i > (int)ERecord_Data_Type.ERDT_DEADLYCONTROL && i <= (int)ERecord_Data_Type.ERDT_RECOVERYHP)
                    instance.transform.SetParent(warDataArea2.transform, false);
                else if (i > (int)ERecord_Data_Type.ERDT_RECOVERYHP && i < (int)ERecord_Data_Type.ERDT_MAX1)
                    instance.transform.SetParent(warDataArea3.transform, false);

                Toggle toggleChild = instance.GetComponent<Toggle>();
                if (toggleChild)
                {
                    // 增加監聽
                    toggleChild.onValueChanged.AddListener(onChickChildToggle);
                    // 將toggle對象存儲起來
                    dicToggle[i] = toggleChild;
                }
            }
        }

        // 當點擊某一個toggle時
        protected void onChickChildToggle(bool bActive)
        {
            Toggle target = tmpToggle ? tmpToggle : EventSystem.current.currentSelectedGameObject.GetComponent<Toggle>();
            int nCount = DataChartPlayers.Count;
            for (int i = 0; i < nCount; ++i)
            {
                UDataChartPlayer uPlayer = DataChartPlayers[i];
                if (uPlayer == null)
                {
                    continue;
                }
                uPlayer.setWarData(target, bActive);
            }
            //setWarData
        }

        // 更新某個區域的toggle是否顯示
        protected void updateWarDataPic(bool bIsActive, int nIndex)
        {
            int nCount = DataChartPlayers.Count;
            for (int i = 0; i < nCount; ++i)
            {
                UDataChartPlayer uPlayer = DataChartPlayers[i];
                if (uPlayer == null)
                {
                    continue;
                }
                uPlayer.setWarData(dicToggle[nIndex], bIsActive);
            }
        }
    }

    public class UWarData : MonoBehaviour
    {
        Slider sliderData;
        Image fillImage;
        int nValue;
        Tweener tweener;

        public int Camp{get{ return m_nCamp; } }
        int m_nCamp;

        public void Start()
        {

        }

        public void Awake()
        {
            sliderData = this.GetComponent<Slider>();
            fillImage = this.transform.FindChild("Background").FindChild("Fill").GetComponent<Image>();
            nValue = 0;
        }

        public void Destroy()
        {
            if (tweener != null)
                tweener.Kill();
        }

        // 初始化单个数据条的信息
        public void initDataImage(int nIndex, int nValue, int nCamp)
        {
            m_nCamp = nCamp;

            SSchemeRecordConfig colorConfig = GameMobaSchemesManager.Instance.getRecordConfigAccordID(nIndex + 1);
            if (colorConfig == null)
            {
                return;
            }

            sliderData.maxValue = colorConfig.nDataNumMax;

            this.nValue = nValue;
            sliderData.value = nValue;
        }

        public void setFillColor(string strClr)
        {
            Color32 color = ColorExtension.FromHex(strClr);
            fillImage.color = color;
        }

        // 播放动画
        public void playAnimation()
        {
            if (tweener != null)
            {
                tweener.Restart();
            }
            else
            {
                tweener = sliderData.DOValue(this.nValue, 2.0f).SetAutoKill(false).OnKill(() => tweener = null);
            }
        }
    }

    public class UDataChartPlayer : MonoBehaviour
    {
        GameObject selfImage;
        Image heroIcon;
        GameObject DataGroup;
        ResNode reqRes;

        Vector2 dataGroupSize;
        Vector2 initCellSize;
        GridLayoutGroup dataGroupLayout;

        Dictionary<Toggle, UWarData> dicWarData = new Dictionary<Toggle, UWarData>();

        // 显示个数最大值
        const int ShowCountMax = 6;

        // 颜色
        string[] ColorsBlueTeam;
        string[] ColorsRedTeam;

        public void Start()
        {
        }

        public void OnDestroy()
        {
            AssetBundleManager.DeleteAssets(ref reqRes, true);
        }

        public void Awake()
        {
            reqRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/DynamicState_Moba/WarRecordTable_End/WarData");
            selfImage = this.transform.FindChild("SelfImage").gameObject;
            heroIcon = this.transform.FindChild("HeroFrame").FindChild("HeroIcon").GetComponent<Image>();
            DataGroup = this.transform.FindChild("DataGroup").gameObject;

            RectTransform rectInfo = DataGroup.GetComponent<RectTransform>();
            dataGroupSize = rectInfo.sizeDelta;

            dataGroupLayout = DataGroup.GetComponent<GridLayoutGroup>();
            initCellSize = dataGroupLayout.cellSize;

            ColorsBlueTeam = new string[ShowCountMax] {"276591FF", "0C9EB8FF", "4F81BCFF", "27918EFF", "40699DFF", "46B491FF" };
            ColorsRedTeam = new string[ShowCountMax] { "912774FF", "B80CB7FF", "BC4F91FF", "7E2791FF", "9D4076FF", "8046B4FF" };
        }

        // 设置玩家信息
        public void setPlayerInfo(cmd_entity_end_player_data cmdData, Dictionary<int, Toggle> dicToggle)
        {
            if(cmdData.bIsSelf > 0)
                selfImage.SetActive(true);
            else 
                selfImage.SetActive(false);

            heroIcon.gameObject.SetActive(true);
            heroIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_RECORDTABLE_END, 1, 7, cmdData.nIconId);

            for (int i = 0; i < (int)ERecord_Data_Type.ERDT_MAX1; ++i)
            {
                GameObject instance = reqRes.InstanceMainRes();
                if (!instance)
                {
                    Debug.LogError("setPlayerInfo instance = NULL");
                    return;
                }
                instance.name = reqRes.AssetName + "_" + cmdData.szPlayerName;
                instance.transform.SetParent(DataGroup.transform, false);

                UWarData warData = instance.GetComponent<UWarData>();
                if (warData != null)
                {
                    warData.initDataImage(i, cmdData.gzAllData[i], cmdData.nCamp);
                    warData.gameObject.SetActive(false);
                    if (dicToggle.ContainsKey(i))
                    {
                        dicWarData[dicToggle[i]] = warData;
                    }
                }
            }
        }

        // 设置数据条
        public void setWarData(Toggle toggle, bool bActive)
        {
            if (toggle == null)
            {
                return;
            }

            if (getActiveCount() >= ShowCountMax)
            {
                if (bActive)
                {
                    toggle.isOn = false;
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CUSTOMER_TIP, "已达战斗统计选中最大上限");
                    return;
                }
            }

            if (dicWarData.ContainsKey(toggle))
            {
                //nCurShowCount++;
                dicWarData[toggle].gameObject.SetActive(bActive);
                //if(nCurShowCount <= ShowCountMax)
                //{
                //    dicWarData[toggle].setFillColor(Colors[nCurShowCount - 1]);
                //}
            }

            int nActiveCount = getActiveCount();
            float fSpaceY = 2f;
            float tmpHeight = dataGroupSize.y - fSpaceY * nActiveCount;
            if(nActiveCount <= 2)
            {
                tmpHeight /= 3;
            }
            else
            {
                tmpHeight /= 6;
            }

            Vector2 originSize = dataGroupLayout.cellSize;
            dataGroupLayout.cellSize = new Vector2(dataGroupSize.x, tmpHeight);

            if (bActive)
            {
                // 播放动画
                playAllToogleAnomation();

                // 设置颜色
                setAllToogleColor();
            }
        }

        // 获取激活数量
        int getActiveCount()
        {
            int nAcitveCount = 0;
            foreach (KeyValuePair<Toggle, UWarData> kvp in dicWarData)
            {
                if (kvp.Value.gameObject.activeSelf)
                {
                    nAcitveCount++;
                }
            }
            return nAcitveCount;
        }

        // 播放所有toogle的动画
        void playAllToogleAnomation()
        {
            foreach (KeyValuePair<Toggle, UWarData> kvp in dicWarData)
            {
                if (kvp.Value.gameObject.activeSelf)
                {
                    kvp.Value.playAnimation();
                }
            }
        }

        // 设置所有toogle的颜色
        void setAllToogleColor()
        {
            int i = 0;
            foreach (KeyValuePair<Toggle, UWarData> kvp in dicWarData)
            {
                if (kvp.Value.gameObject.activeSelf)
                {
                    if(i < ShowCountMax)
                    {
                        if(kvp.Value.Camp == 1)
                        {
                            kvp.Value.setFillColor(ColorsBlueTeam[i]);
                        }
                        else
                        {
                            kvp.Value.setFillColor(ColorsRedTeam[i]);
                        }

                    }
                    else
                    {
                        // 超过颜色数组范围，默认为白色
                        kvp.Value.setFillColor("FFFFFF");
                    }

                    i++;
                }
            }
        }
    }
}
