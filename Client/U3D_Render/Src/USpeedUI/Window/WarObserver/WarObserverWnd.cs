using ASpeedGame.Data.GameMobaSchemes;
using ASpeedGame.Data.PropertyPanelConfig;
using DataCenter;
using GameLogic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using U3D_Render;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UExtensions;
using War;

namespace USpeedUI.WarObserver
{
    public enum WarOBUIPartType
    {
        SKILL_PART = 1,
        BUFF_PART = SKILL_PART << 1,
        BUTTEN_LEFT_PART = SKILL_PART << 2,
        TALENT_PART = SKILL_PART << 3,
    }

    public class WarObserverWnd : UIBaseWnd<WarObserverWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_OBSERVER;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override string GetPrefabPath()
        {
            return "DynamicState_Moba/WarObserver/WarObserverView";
        }

        public override int GetLoadMask()
        {
            return (int)WndLoadMaskDef.WND_LoadMask_Moba;
        }

        public override int GetPermissionMask()
        {
            return (int)WndPermissionMaskDef.WND_PM_OB;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_UPDATEPLAYERINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_WARHEROMAXSKILLSTATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_XP_SKILL_UPDATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_UPDATEMAINPLAYERGOODS, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLSETSLOT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_OTHER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLEFFECT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLOVEREFFECT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_BUFFINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLMPEFFECT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_OBSERVER_SWITCH_HERO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_OBSERVER_HIDE_FRAME_PART, this);

            EntityFactory.SetEntityNumPropHandler(ResponseEntityNumPropEvent);

            return true;
        }

        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_UPDATEPLAYERINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_WARHEROMAXSKILLSTATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_XP_SKILL_UPDATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_UPDATEMAINPLAYERGOODS, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLSETSLOT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_OTHER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLEFFECT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLOVEREFFECT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_BUFFINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLMPEFFECT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_OBSERVER_SWITCH_HERO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_OBSERVER_HIDE_FRAME_PART, this);

            EntityFactory.RemoveEntityNumPropHandler(ResponseEntityNumPropEvent);

            return;
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);

            if (_bVisible && m_wndView != null)
            {
                m_wndView.SetData();
            }
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        this.UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_UPDATEPLAYERINFO:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetData();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.ShowReliveTime(msgData as UIWarShowReliveTimeData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_WARHEROMAXSKILLSTATE:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetMaxSkill(msgData as UIMsgCmdData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_XP_SKILL_UPDATE:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetXPSkill(msgData as UIXPSkillCD);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_GOODSSTORE_UPDATEMAINPLAYERGOODS:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetPersonBuyGoodsInfo();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLSETSLOT:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetSpellData();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_OTHER:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetXPEffect(msgData as UIMsgCmdData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLEFFECT:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetCurrentPlayerSlotSpellCD(msgData as UIMsgCmdData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLOVEREFFECT:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetSKillOverEffect(msgData as UIMsgCmdData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_BUFFINFO:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetBuffInfo(msgData as UIMsgCmdData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLMPEFFECT:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetSkillMPEffect(msgData as UIMsgCmdData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_OBSERVER_SWITCH_HERO:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SwitchHero(msgData as UISWitchHero);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_OBSERVER_HIDE_FRAME_PART:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.HideFramePart(msgData as UIOBHideFramePart);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        //数值属性回调函数
        public void ResponseEntityNumPropEvent(EntityNumPropEventArgs e)
        {
            if (e.objev == null || m_wndView == null)
            {
                return;
            }

            EntityView ev = e.objev;
            int nUID = e.data.nEntityID;
            switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_HP:
                case (int)ENTITY_PROPERTY.PROPERTY_MAX_HP:
                    {
                        m_wndView.SetHPBar(nUID, ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP), ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP));
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_MP:
                case (int)ENTITY_PROPERTY.PROPERTY_MAX_MP:
                    {
                        m_wndView.SetMPBar(nUID, ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_MP), ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MP));
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_LEVEL:
                    {
                        m_wndView.SetLevel(nUID, ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL));
                    }
                    break;
                default:
                    break;
            }

            if (m_wndView.CurrentPlayer != null && m_wndView.CurrentPlayer.nEntityID == nUID)
            {
                entity_view_update_Numproperty data = e.data;
                int nUpdateValue = data.nValue;
                switch (e.data.nPropID)
                {
                    case (int)ENTITY_PROPERTY.PROPERTY_HP:
                    case (int)ENTITY_PROPERTY.PROPERTY_MAX_HP:
                        {
                            m_wndView.BottomView.SetHeroHP(ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP), ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP));
                        }
                        break;
                    case (int)ENTITY_PROPERTY.PROPERTY_MP:
                    case (int)ENTITY_PROPERTY.PROPERTY_MAX_MP:
                        {
                            m_wndView.BottomView.SetHeroMP(ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MP), ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_MP));
                        }
                        break;
                    case (int)ENTITY_PROPERTY.PROPERTY_LEVEL:
                        {
                            m_wndView.BottomView.SetHeroLevel(ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL));
                        }
                        break;
                    case (int)ENTITY_PROPERTY.PROPERTY_EXP:
                        {
                            m_wndView.BottomView.SetHeroExp(nUpdateValue, data.nOtherData);
                        }
                        break;
                    case (int)ENTITY_PROPERTY.PROPERTY_STAMINA:
                        {
                            m_wndView.BottomView.SetHeroStamina(ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_STAMINA), ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_STAMINA));
                        }
                        break;
                    case (int)ENTITY_PROPERTY.PROPERTY_MAX_STAMINA:
                        {
                            m_wndView.BottomView.SetHeroStamina(ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_STAMINA), ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_STAMINA));
                        }
                        break;
                    case (int)ENTITY_PROPERTY.PROPERTY_TALENT_POINT://金钱
                        {
                            m_wndView.BottomView.SetGoodsStoreMoney(nUpdateValue);
                        }
                        break;
                    case (int)ENTITY_PROPERTY.PROPERTY_PA:
                    case (int)ENTITY_PROPERTY.PROPERTY_MA:
                    case (int)ENTITY_PROPERTY.PROPERTY_PD:
                    case (int)ENTITY_PROPERTY.PROPERTY_MD:
                    case (int)ENTITY_PROPERTY.PROPERTY_SD:
                    case (int)ENTITY_PROPERTY.PROPERTY_PCT_AF:
                    case (int)ENTITY_PROPERTY.PROPERTY_CRC:
                    case (int)ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE:
                        {
                            m_wndView.SetPropertyValue((ENTITY_PROPERTY)e.data.nPropID);
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }

    enum ETimerID
    {
        COMPETITION_TIME = 0,     // 游戏时间定时器id
    }

    [Serializable]
    public class TeamCombatData
    {
        public Text towerText;
        public Text moneyText;
        public Text killNumText;
    }

    [Serializable]
    public class PlayerCombatData : TimeCountDown
    {
        public Text playerNameText;
        public Text levelText;
        public Image headPortraitImage;
        public Image summonerSkill1Image;
        public Image summonerSkill2Image;
        public Image summonerSkill3Image;
        public Slider healthBar;
        public Slider magicBar;
        public Text TimeCountText;
        public GameObject DieMask;
        [HideInInspector]
        public int nEntityID;
        [HideInInspector]
        public int nPDBID;

        public void Init()
        {
            DieMask.gameObject.SetActive(false);
            TimeCountText.gameObject.SetActive(false);

            summonerSkill3Image.enabled = false;
            this.gameObject.AddComponent<MaxSkillCountDown>();
            this.gameObject.AddComponent<XPSkillCountDown>();
        }

        public override void StartCountDown(float nReliveTime, float time, float repeatRate)
        {
            TimeCountText.gameObject.SetActive(true);
            DieMask.SetActive(true);
            healthBar.gameObject.SetActive(false);
            magicBar.gameObject.SetActive(false);
            TimeCountText.text = nReliveTime.ToString();

            base.StartCountDown(nReliveTime, time, repeatRate);
        }

        protected override void CountDownRepeatCallBack()
        {
            base.CountDownRepeatCallBack();

            TimeCountText.text = ((int)fCurTime).ToString();
        }

        protected override void CountDownCompleteCallBack()
        {
            base.CountDownCompleteCallBack();

            TimeCountText.gameObject.SetActive(false);
            DieMask.SetActive(false);
            healthBar.gameObject.SetActive(true);
            magicBar.gameObject.SetActive(true);
        }
    }

    [Serializable]
    public class CampData : MonoBehaviour
    {
        public TeamCombatData teamData;
        public PlayerCombatData templateItem;
        private List<PlayerCombatData> listPlayerData = new List<PlayerCombatData>();
        private int m_camp;
        private WarObserverWndView m_masterView;

        public void Init(WarObserverWndView masterView)
        {
            m_masterView = masterView;
            teamData.killNumText.text = "0";
            teamData.moneyText.text = "0";
            teamData.towerText.text = "0";

            templateItem.gameObject.SetActive(false);
            for (int j = 0; j < LogicDataCenter.recordTableDataManager.GetHalfPlayerCount(); ++j)
            {
                PlayerCombatData item = Instantiate<PlayerCombatData>(templateItem);
                item.gameObject.GetComponent<Button>().onClick.AddListener(() => m_masterView.OnPlayerItemClick(item));
                item.gameObject.SetActive(true);
                item.transform.SetParent(templateItem.transform.parent, false);
                item.Init();
                listPlayerData.Add(item);
            }
        }

        public void SetData(int camp, ref Dictionary<int, PlayerCombatData> dicPlayerData)
        {
            m_camp = camp;
            teamData.killNumText.text = LogicDataCenter.recordTableDataManager.getWarSameCampPersonKillNum(camp).ToString();
            teamData.moneyText.text = string.Format("${0}", LogicDataCenter.recordTableDataManager.GetTeamTotalMoney(camp).ToString());
            teamData.towerText.text = LogicDataCenter.recordTableDataManager.GetTeamTotalDestroyTower(camp).ToString();
            Dictionary<ulong, WarRecordTableDataManager.WarPersonInfo> dicPersonInfo = LogicDataCenter.recordTableDataManager.recordInfoArry;

            foreach (PlayerCombatData playerData in listPlayerData)
            {
                playerData.gameObject.SetActive(false);
            }

            int nHalfPlayerNum = LogicDataCenter.recordTableDataManager.GetHalfPlayerCount();
            foreach (KeyValuePair<ulong, WarRecordTableDataManager.WarPersonInfo> kvp in dicPersonInfo)
            {
                WarRecordTableDataManager.WarPersonInfo personInfo = kvp.Value;
                if (personInfo.nCamp == camp)
                {
                    int nSlot = LogicDataCenter.recordTableDataManager.getIndexAcordPDBID(personInfo.uPDBID);
                    int index = nSlot < listPlayerData.Count ? nSlot : nHalfPlayerNum * 2 - nSlot - 1;
                    if (index < listPlayerData.Count)
                    {
                        PlayerCombatData playerData = listPlayerData[index];
                        playerData.playerNameText.text = personInfo.playerName;
                        playerData.levelText.text = personInfo.nLevel.ToString();
                        playerData.headPortraitImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_OBSERVER, 1, 3, personInfo.nHeroId);
                        playerData.summonerSkill2Image.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_WAR_OBSERVER, 1, personInfo.nSummerIconID);
                        playerData.gameObject.SetActive(true);
                        playerData.nEntityID = personInfo.nID;
                        playerData.nPDBID = (int)personInfo.uPDBID;

                        if (!dicPlayerData.ContainsKey((int)kvp.Value.nID))
                        {
                            dicPlayerData[(int)kvp.Value.nID] = playerData;
                        }
                    }
                    else
                    {
                        Debug.LogErrorFormat("warhero info slot is error.({0})", index);
                        break;
                    }
                }
            }
        }
    }

    public class MaxSkillCountDown : TimeCountDown
    {
        protected override void CountDownRepeatCallBack()
        {
            base.CountDownRepeatCallBack();

            PlayerCombatData item = this.gameObject.GetComponent<PlayerCombatData>();
            item.summonerSkill1Image.fillAmount = (fOriginalTime - fCurTime) / (fOriginalTime);
        }
    }

    public class XPSkillCountDown : TimeCountDown
    {
        //protected override void CountDownRepeatCallBack()
        //{
        //    base.CountDownRepeatCallBack();

        //    PlayerCombatData item = this.gameObject.GetComponent<PlayerCombatData>();
        //    item.summonerSkill3Image.fillAmount = (fOriginalTime - fCurTime) / (fOriginalTime);
        //}

        protected override void CountDownCompleteCallBack()
        {
            base.CountDownCompleteCallBack();

            PlayerCombatData item = this.gameObject.GetComponent<PlayerCombatData>();
            item.summonerSkill3Image.ResetToDefaultMaterial();
        }
    }

    public class WarObserverWndView : UIBaseWndView, ITimerHanlder
    {
        private const int CAMP_COUNT = 2;
        public Text competitionTimeText;
        public CampData[] campData = new CampData[CAMP_COUNT];
        private Dictionary<int, PlayerCombatData> m_dicPlayerWarInfo = new Dictionary<int, PlayerCombatData>();
        private Dictionary<int, CampData> m_dicCampData = new Dictionary<int, CampData>();
        [HideInInspector]
        public WarBottonCenterWndView BottomView;
        [HideInInspector]
        public PlayerCombatData CurrentPlayer;

        /*要显示的属性值*/
        private Dictionary<ENTITY_PROPERTY, float> m_dicPropertyValue = new Dictionary<ENTITY_PROPERTY, float>();
        private Dictionary<ENTITY_PROPERTY, int> m_dicPropertyIndex = new Dictionary<ENTITY_PROPERTY, int>();

        private const int nMainSkillSlotIdBegin = 0;
        private const int nMainSkillSlotIdEnd = 3;
        private const int nSummerSkillSlotIdBegin = 13;
        private const int nSummerSkillSlotIdEnd = 14;
        private const int nHeroTalentSkillSlotId1 = 16;
        private const int nHeroTalentSkillSlotId2 = 18;
        private const int nHeroTalentSkillSlotId3 = 19;

        private Dictionary<WarOBUIPartType, GameObject> m_dicPart = new Dictionary<WarOBUIPartType, GameObject>();

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            InitPropertyIndex();
            InitProperyDic();

            ResNode res = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/DynamicState_Moba/WarBottomCenterView/WarBottomCenter");
            if (res == null)
            {
                Debug.LogFormat("load prefab 'WarBottomCenter' failed.");
                return false;
            }

            GameObject instance = res.InstanceMainRes();
            instance.transform.SetParent(this.transform, false);
            BottomView = instance.GetComponent<WarBottonCenterWndView>();
            BottomView.ResetView();
            competitionTimeText.text = "00:00";

            m_dicCampData[(int)CAMP_TYPE.CAMP_BLUE] = campData[0];
            m_dicCampData[(int)CAMP_TYPE.CAMP_RED] = campData[1];

            foreach (CampData item in m_dicCampData.Values)
            {
                item.Init(this);
            }

            TimerManager.SetTimer(this, (int)ETimerID.COMPETITION_TIME, 1.0f);

            InitDicPartFrame();

            return true;
        }

        private void InitDicPartFrame()
        {
            m_dicPart[WarOBUIPartType.BUFF_PART] = BottomView.warCenterBuffPart.gameObject;
            m_dicPart[WarOBUIPartType.BUTTEN_LEFT_PART] = BottomView.leftPartObj;
            m_dicPart[WarOBUIPartType.SKILL_PART] = BottomView.warCenterSkillPart.gameObject;
            m_dicPart[WarOBUIPartType.TALENT_PART] = BottomView.warTalentBtn;
        }

        public override void Destroy()
        {
            TimerManager.KillTimer(this, (int)ETimerID.COMPETITION_TIME);
            DestroyProperyDic();

            base.Destroy();
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)ETimerID.COMPETITION_TIME:
                    {
                        if (LogicDataCenter.warInfoDataManager.WarStartTime > 0)
                        {
                            uint nWarGameTimeMs = GameLogicAPI.getSyncTickCount() - LogicDataCenter.warInfoDataManager.WarStartTime;
                            LogicDataCenter.warInfoDataManager.WarPlayTimeSec = nWarGameTimeMs / 1000;

                            uint nMin, nSec;
                            string strMin, strSec;
                            nMin = LogicDataCenter.warInfoDataManager.WarPlayTimeSec / 60;
                            nSec = LogicDataCenter.warInfoDataManager.WarPlayTimeSec % 60;
                            strMin = nMin.ToString("00");
                            strSec = nSec.ToString("00");

                            //更新游戏时间
                            competitionTimeText.text = strMin + ":" + strSec;
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public void SwitchHero(UISWitchHero msg)
        {
            if (msg == null)
                return;

            PlayerCombatData data;
            if (m_dicPlayerWarInfo.TryGetValue(msg.nEntityID, out data))
            {
                OnPlayerItemClick(data);
            }
        }

        public void HideFramePart(UIOBHideFramePart msg)
        {
            if (msg == null)
                return;

            foreach (var item in m_dicPart)
            {
                item.Value.SetActive(((int)item.Key & msg.frame) == 0);
            }
        }

        public void SetCurrentPlayerSlotSpellCD(UIMsgCmdData uiSpellEffect)
        {
            if (CurrentPlayer == null || CurrentPlayer.nEntityID != LogicDataCenter.warOBUIDataManager.CurrentUID)
                return;

            SetSpellCD(IntPtrHelper.toData<cmd_creature_set_slot>(uiSpellEffect.ptrParam));
        }

        private void SetSpellCD(cmd_creature_set_slot data)
        {
            if (data.nSlotType == SLOT_TYPE.SLOT_TYPE_SKILL)
            {
                SetSpellSlotEffect(data);
            }
            else if (data.nSlotType == SLOT_TYPE.SLOT_TYPE_GOODS)
            {
                SetPurchasedGoodsUseEffect(data);
                SetPurchasedGoodsCanDragDrop(data);
            }
        }

        public void SetSKillOverEffect(UIMsgCmdData uiSpellOverEffect)
        {
            Trace.Log("SetSKillOverEffect WND_MSG_WARBUTTOMCENTER_SKILLOVEREFFECT");
            if (CurrentPlayer == null || CurrentPlayer.nEntityID != LogicDataCenter.warOBUIDataManager.CurrentUID)
                return;

            cmd_set_spell_overlay data = IntPtrHelper.toData<cmd_set_spell_overlay>(uiSpellOverEffect.ptrParam);
            SetSpellSlotFlagEffect(data);
        }

        public void SetBuffInfo(UIMsgCmdData uiBuffData)
        {
            cmd_creature_setBuff data = IntPtrHelper.toData<cmd_creature_setBuff>(uiBuffData.ptrParam);
            SetHeroBuff(data);
        }

        public void SetSkillMPEffect(UIMsgCmdData uiMpEffectData)
        {
            cmd_creature_mp_effect_spell data = IntPtrHelper.toData<cmd_creature_mp_effect_spell>(uiMpEffectData.ptrParam);
            SetMpEffectSpell(data);
        }

        private void SetMpEffectSpell(cmd_creature_mp_effect_spell data)
        {
            if (data.nSlotType == SLOT_TYPE.SLOT_TYPE_SKILL)
            {
                bool bUse = data.nShowType <= 0 ? true : false;
                BottomView.SetMpEffectSpell(data.nSlotIndex, bUse);
            }
        }

        private void SetHeroBuff(cmd_creature_setBuff _data)
        {
            if (_data.nType == 1)
            {
                bool bShow = false;
                if (_data.nIsShow != 0)
                {
                    bShow = true;
                }
                BottomView.AddBuff((int)_data.dwIndex, _data.nIconID, (int)_data.dwTime, _data.szName, _data.szTips, _data.nLevel, bShow);
            }
            else if (_data.nType == 2)
            {
                BottomView.RemoveBuff((int)_data.dwIndex);
            }
        }

        private void SetSpellSlotFlagEffect(cmd_set_spell_overlay _data)
        {
            //如果这里有报错，先看看逻辑层这个nID是否是槽位ID，以前是下发技能ID的

            ReCalSlotIndex(ref _data.nSlotIndex);

            if (_data.nType != 0)
            {
                BottomView.SetSkillFlagEffect(_data.nSlotIndex, _data.nFlag, _data.nFreezeLeftTime, _data.nLeftTime, _data.nTotalTime);
            }
            else
            {
                BottomView.SetSkillFlagEffectOver(_data.nSlotIndex, _data.nFlag);
            }
        }

        private void SetPurchasedGoodsUseEffect(cmd_creature_set_slot data)
        {
            if (data.nSlotIndex < 0)
            {
                return;
            }

            int endTime = data.slotview.nMaxData;
            int beginTime = data.slotview.nMaxData - data.slotview.nCurData < 0 ? 0 : data.slotview.nMaxData - data.slotview.nCurData;


            BottomView.SetPurchasedGoodsUseEffect(data.nSlotIndex, beginTime, endTime);
        }

        private void SetPurchasedGoodsCanDragDrop(cmd_creature_set_slot data)
        {
            if (data.nSlotIndex < 0)
            {
                return;
            }

            bool bCanDrag = data.slotview.bCanDrag <= 0 ? false : true;
            BottomView.SetPurchasedGoodsCanDragDrop(data.nSlotIndex, bCanDrag);
        }


        public void OnPlayerItemClick(PlayerCombatData item)
        {
            U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(item.nEntityID);
            if (ev == null)
                return;

            CurrentPlayer = item;
            BottomView.ResetView();

            SetNumPromps(ev);
            SetSpellData();
            SetPersonBuyGoodsInfo();
            RestoreAllSpellCD();

            // 切换镜头
            SoldierCamera.MainInstance<SoldierCamera>().SwitchOBSoliderTarget(ev);

#if ENABLE_YUNYING_MODE
            SoldierCamera.MainInstance<SoldierCamera>().EnterOB_yunying(CameraMode.YUNYING_OBHeroLock);
#else
            SoldierCamera.MainInstance<SoldierCamera>().EnableOBMode(true);
#endif
        }


        public void SetSpellData()
        {
            if (CurrentPlayer == null)
                return;

            int nEntityID = CurrentPlayer.nEntityID;
            Dictionary<int, OB_PersonSpellData> dicSlot;
            if (LogicDataCenter.warOBUIDataManager.OBPersonData.TryGetValue(nEntityID, out dicSlot))
            {
                foreach (OB_PersonSpellData slot in dicSlot.Values)
                {
                    SetSpellSlot(slot.slotSpell);
                }
            }
        }

        public void SetPersonBuyGoodsInfo()
        {
            if (CurrentPlayer == null)
                return;

            List<cmd_war_persongoodsinfo> listGoods = LogicDataCenter.warMobaGoodsDataManager.GetPlayerGoodsInfo((ulong)CurrentPlayer.nPDBID);
            if (listGoods != null)
            {
                foreach (var data in listGoods)
                {
                    if (data.nGoodID != 0)
                    {
                        SSchemeMobaGoods mobaGood;
                        LogicDataCenter.warMobaGoodsDataManager.GetMobaGoods_Out(data.nGoodID, out mobaGood);

                        BottomView.AddGoodsButtonToSlot(data, mobaGood);
                    }
                    else
                    {
                        BottomView.RemoveGoodsButtonFromSlot(data.nSlotIndex);
                    }
                }
            }
        }

        private void RestoreAllSpellCD()
        {
            if (CurrentPlayer == null)
                return;

            int nEntityID = CurrentPlayer.nEntityID;
            Dictionary<int, OB_PersonSpellData> dicSlot;
            if (LogicDataCenter.warOBUIDataManager.OBPersonData.TryGetValue(nEntityID, out dicSlot))
            {
                foreach (OB_PersonSpellData slot in dicSlot.Values)
                {
                    int currentTime = (int)GameLogicAPI.getTickCount();
                    int elapsTime = currentTime - slot.initCDTime;
                    if (elapsTime < slot.changeSlot.slotview.nMaxData)
                    {
                        slot.changeSlot.slotview.nCurData = slot.changeSlot.slotview.nMaxData - elapsTime;
                        SetSpellCD(slot.changeSlot);
                    }
                }
            }
        }

        private bool CheckSlotIndexRange(int _nSlotIndex)
        {
            if (((_nSlotIndex >= nMainSkillSlotIdBegin) && (_nSlotIndex <= nMainSkillSlotIdEnd)) || ((_nSlotIndex == nSummerSkillSlotIdBegin))
                || _nSlotIndex == nHeroTalentSkillSlotId1 || _nSlotIndex == nHeroTalentSkillSlotId2 || _nSlotIndex == nHeroTalentSkillSlotId3)// && (_data.nSlotIndex <= nSummerSkillSlotIdEnd)))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        private void ReCalSlotIndex(ref int nSlotIndex)
        {
            if (nSlotIndex == nHeroTalentSkillSlotId1) // 天赋快捷键特殊处理下
            {
                nSlotIndex = 5;
            }
            else if (nSlotIndex == nHeroTalentSkillSlotId2)
            {
                nSlotIndex = 6;
            }
            else if (nSlotIndex == nHeroTalentSkillSlotId3)
            {
                nSlotIndex = 7;
            }
            else if (nSlotIndex > 3)
            {
                nSlotIndex -= 9;
            }
        }

        private void SetSpellSlot(cmd_creature_set_slot _data)
        {
            //召唤师技能(5 - 6)目前还没有，不接受消息
            if (!CheckSlotIndexRange(_data.nSlotIndex))
            {
                return;
            }
            ReCalSlotIndex(ref _data.nSlotIndex);

            //主动技能(0-2)
            //天赋技能(3)
            //召唤师技能(AS中序号是4)(策划配置是13)
            //SetSkillSlotID(_data.nSlotIndex);
            //SetSkillSlotType(_data.nSlotIndex, );
            //SetSkillIcon(_data.nSlotIndex, _data.slotview.nIconID);
            //SetSkillTooltip(_data.nSlotIndex, _data);
            //SetSkillLevel(_data.nSlotIndex, );
            int _nType = Convert.ToInt32(_data.nSlotType);
            bool bUseSlotFlag = Convert.ToBoolean(_data.nOverlaySpell);

            BottomView.SetSpellSlot(_data.nSlotIndex, _nType, _data.slotview.nIconID, _data.slotview.szSlotDesc, _data.nSlotLevel, bUseSlotFlag);
        }

        public void SetSpellSlotEffect(cmd_creature_set_slot _data)
        {
            //主动技能(0-2)
            //天赋技能(3)
            //召唤师技能(AS中序号是4)(策划配置是13)
            if (!CheckSlotIndexRange(_data.nSlotIndex))
            {
                return;
            }

            ReCalSlotIndex(ref _data.nSlotIndex);

            int effectType = (int)(_data.slotview.nViewType | SoltViewType.ViewType_RightCircle);
            if (_data.slotview.nViewType == SoltViewType.ViewType_NONE)
            {
                return;
            }
            int endTime = _data.slotview.nMaxData;
            int beginTime = _data.slotview.nMaxData - _data.slotview.nCurData < 0 ? 0 : _data.slotview.nMaxData - _data.slotview.nCurData;

            if (_data.slotview.nCurData == 0)
            {
                BottomView.SetSkillBtnEffectClear(_data.nSlotIndex);
            }
            else
            {
                BottomView.SetSkillBtnEffect(_data.nSlotIndex, effectType, beginTime, endTime);
            }
        }

        private void InitProperyDic()
        {
            DestroyProperyDic();
            m_dicPropertyValue.Add(ENTITY_PROPERTY.PROPERTY_PA, 0);
            m_dicPropertyValue.Add(ENTITY_PROPERTY.PROPERTY_MA, 0);
            m_dicPropertyValue.Add(ENTITY_PROPERTY.PROPERTY_PD, 0);
            m_dicPropertyValue.Add(ENTITY_PROPERTY.PROPERTY_MD, 0);
            m_dicPropertyValue.Add(ENTITY_PROPERTY.PROPERTY_PCT_AF, 0);
            m_dicPropertyValue.Add(ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE, 0);
            m_dicPropertyValue.Add(ENTITY_PROPERTY.PROPERTY_CRC, 0);
            m_dicPropertyValue.Add(ENTITY_PROPERTY.PROPERTY_SD, 0);
        }

        private void DestroyProperyDic()
        {
            m_dicPropertyValue.Clear();
        }

        private void InitPropertyIndex()
        {
            m_dicPropertyIndex.Clear();
            m_dicPropertyIndex.Add(ENTITY_PROPERTY.PROPERTY_PA, 1);
            m_dicPropertyIndex.Add(ENTITY_PROPERTY.PROPERTY_PD, 2);
            m_dicPropertyIndex.Add(ENTITY_PROPERTY.PROPERTY_AF, 3);
            m_dicPropertyIndex.Add(ENTITY_PROPERTY.PROPERTY_CRC, 4);
            m_dicPropertyIndex.Add(ENTITY_PROPERTY.PROPERTY_MA, 5);
            m_dicPropertyIndex.Add(ENTITY_PROPERTY.PROPERTY_MD, 6);
            m_dicPropertyIndex.Add(ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE, 7);
            m_dicPropertyIndex.Add(ENTITY_PROPERTY.PROPERTY_SD, 8);
            m_dicPropertyIndex.Add(ENTITY_PROPERTY.PROPERTY_PCT_AF, 9);
        }

        public void SetData()
        {
            foreach (var item in m_dicCampData)
            {
                item.Value.SetData(item.Key, ref m_dicPlayerWarInfo);
            }
        }

        public void SetNumPromps(EntityView ev)
        {
            if (ev == null || ev.Property == null)
                return;

            BottomView.SetHeroHP(ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP), ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP));
            BottomView.SetHeroMP(ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MP), ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_MP));
            BottomView.SetHeroLevel(ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL));
            BottomView.SetHeroExp(ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_EXP), GameLogicAPI.GetWarHeroNextLevelExp(ev.ID));
            BottomView.SetHeroStamina(ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_STAMINA), ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_STAMINA));
            BottomView.SetGoodsStoreMoney(ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TALENT_POINT));
            SetAllPropertyValue();
        }

        private void SetAllPropertyValue()
        {
            ENTITY_PROPERTY[] arrayProperty = {
                ENTITY_PROPERTY.PROPERTY_PA, ENTITY_PROPERTY.PROPERTY_MA, ENTITY_PROPERTY.PROPERTY_PD,
                ENTITY_PROPERTY.PROPERTY_MD, ENTITY_PROPERTY.PROPERTY_SD, ENTITY_PROPERTY.PROPERTY_PCT_AF,
                ENTITY_PROPERTY.PROPERTY_CRC, ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE
            };

            foreach (ENTITY_PROPERTY index in arrayProperty)
            {
                SetPropertyValue(index);
            }
        }

        public void SetPropertyValue(ENTITY_PROPERTY index)
        {
            if (CurrentPlayer == null)
                return;

            EntityView ev = EntityFactory.getEntityViewByID(CurrentPlayer.nEntityID);
            float fValue = ev.Property.GetNumProp(index);
            if (!Mathf.Approximately(m_dicPropertyValue[index], fValue))
            {
                m_dicPropertyValue[index] = fValue;

                if (index == ENTITY_PROPERTY.PROPERTY_CRC
                    || index == ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE
                    || index == ENTITY_PROPERTY.PROPERTY_PCT_AF)
                {
                    fValue /= 100;
                }

                int nHeroID = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);

                int nPropIndex = m_dicPropertyIndex[index];

                SSchemePropertyPanel propConfig = PropertyPanelConfig.Instance.GetPropertyPanelConfigList(nHeroID, fValue, nPropIndex);
                if (propConfig == null)
                {
                    Debug.LogError(String.Format("propConfig == null,{0},{1}, {2}", nHeroID, fValue, nPropIndex));
                    return;
                }

                string strValue = "";
                if (index == ENTITY_PROPERTY.PROPERTY_PCT_AF)
                {
                    strValue = String.Format("{0}{1}", (int)fValue, "%");
                }
                else if (index == ENTITY_PROPERTY.PROPERTY_CRC || index == ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE)
                {
                    strValue = fValue.ToString("0") + "%";
                }
                else
                {
                    strValue = fValue.ToString();
                }

                string strNewText = String.Format("<color='#{0}'>{1}</color>", propConfig.strColor, strValue);

                BottomView.SetPropertyValue(nPropIndex - 1, strNewText);
            }
        }

        public void SetHPBar(int nEntityID, int nHPMax, int nHPValue)
        {
            PlayerCombatData combat;
            if (m_dicPlayerWarInfo.TryGetValue(nEntityID, out combat))
            {
                combat.healthBar.value =  (float)nHPValue / nHPMax;
            }
        }

        public void SetMPBar(int nEntityID, int nMPMax, int nMPValue)
        {
            PlayerCombatData combat;
            if (m_dicPlayerWarInfo.TryGetValue(nEntityID, out combat))
            {
                combat.magicBar.value = (float)nMPValue / nMPMax;
            }
        }

        public void SetLevel(int nEntityID, int nLevel)
        {
            PlayerCombatData combat;
            if (m_dicPlayerWarInfo.TryGetValue(nEntityID, out combat))
            {
                combat.levelText.text = nLevel.ToString();
            }
        }

        public void ShowReliveTime(UIWarShowReliveTimeData msg)
        {
            if (msg == null)
                return;

            PlayerCombatData playerWarData;
            if (m_dicPlayerWarInfo.TryGetValue(msg.uid, out playerWarData))
            {
                playerWarData.StartCountDown(msg.nReliveTime, 1.0f, 1.0f);
            }
        }

        public void SetMaxSkill(UIMsgCmdData msg)
        {
            if (msg == null)
                return;

            cmd_player_skill_state data = IntPtrHelper.toData<cmd_player_skill_state>(msg.ptrParam);
            PlayerCombatData playerWarData;
            if (m_dicPlayerWarInfo.TryGetValue((int)data.uOperator, out playerWarData))
            {
                int nCoolTime = data.nCoolTime / 1000;
                if (nCoolTime > 0)
                {
                    playerWarData.gameObject.GetComponent<MaxSkillCountDown>().StartCountDown(nCoolTime, 0.5f, 0.5f);
                }
            }
        }

        public void SetXPSkill(UIXPSkillCD msg)
        {
            if (msg == null)
                return;

            SWar2ViewMsgXPBeginCD data = msg.data;
            PlayerCombatData playerWarData;
            if (m_dicPlayerWarInfo.TryGetValue((int)data.uID, out playerWarData))
            {
                IntPtr Ptr = GameLogicAPI.getSpellData(data.nSkillID);
                if (Ptr == IntPtr.Zero)
                {
                    Debug.LogWarning("SetXPSkill:Ptr is null");
                    return;
                }

                SPELL.SPELL_DATA spellData = IntPtrHelper.toData<SPELL.SPELL_DATA>(Ptr);
                Sprite sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_WAR_XPSKILL, 1, spellData.nIcon);
                if (sprite == null)
                {
                    Debug.LogFormat("sprite is null.{0},{1}", data.nSkillID, spellData.nIcon);
                }
                playerWarData.summonerSkill3Image.enabled = true;
                playerWarData.summonerSkill3Image.sprite = sprite;
                int elapseTime = (int)((double)GameLogicAPI.getSyncTickCount() - (double)data.nBeginCDTick / 1000);
                int remainTime = data.nCDTime / 1000 - elapseTime;

                playerWarData.summonerSkill3Image.AddNewMaterial(EUIShaderType.EST_Gray);
            }
        }

        public void SetXPEffect(UIMsgCmdData uData)
        {
            cmd_creature_ShowPromptUI cmdData = IntPtrHelper.toData<cmd_creature_ShowPromptUI>(uData.ptrParam);
            PlayerCombatData playerWarData;
            if (m_dicPlayerWarInfo.TryGetValue((int)cmdData.uid, out playerWarData))
            {
                if (cmdData.nType != 0)
                {
                    playerWarData.summonerSkill3Image.ResetToDefaultMaterial();
                }
                else
                {
                    playerWarData.summonerSkill3Image.AddNewMaterial(EUIShaderType.EST_Gray);
                }
            }

            Trace.Log("WND_MSG_WAR_XPSKILL_SET_XP_SKILL_OTHER come end.");
        }
    }
}
