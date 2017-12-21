using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using U3D_Render;
using ASpeedGame.Data.GameSettingConfig;
using ASpeedGame.Data.SlotKeyCodeConfig;
using System.IO;
using ASpeedGame.Data.PlayerSettingLocalConfig;

namespace USpeedUI.SystemSetting
{
    public class SystemControlWnd : UIBaseSubWnd<SystemControlWndView>
    {
        public override int GetWndType()
        {
            return (int)SystemSettingSubWndType.WND_CONTROL_DEFAULT;
        }

        public override string GetPrefabPath()
        {
            return "SystemSetting/ControlView";
        }

        public override bool Init()
        {
            base.Init();

            SetVisible(false);
            return true;
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
            if (visible)
            {
                if (m_wndView != null)
                {
                    m_wndView.SetToggleDefault();
                    m_wndView.SettingKeyCodeInfo(EntityFactory.MainHeroView);
                }
            }
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_SYSTEMSETTING_SAVE:
                    if (m_wndView != null)
                        m_wndView.SaveSettingData();
                    break;
                case WndMsgID.WND_MSG_SYSTEMSETTING_RESTORE:
                    if (m_wndView != null)
                        m_wndView.RestoreSettingData(EntityFactory.MainHeroView);
                    break;
                case WndMsgID.WND_MSG_SYSTEMCONTROLSETTING_RESTORE:
                    if (m_wndView != null)
                        m_wndView.RestoreSettingData(EntityFactory.MainHeroView);
                    break;
                case WndMsgID.WND_MSG_SYSTEMCONTROLSETTING_REFRESH:
                    if (m_wndView != null)
                        m_wndView.RefreshSettingData(EntityFactory.MainHeroView);
                    break;
                default:
                    break;
            }
        }
    }

    [Serializable]
    public class KeyCodeItem
    {
        public int keyIndex;
        public Text keyCodeText;
        public Button keyCodeToggleBtn;
        public Text toggleText;
        public int useType = 1;
    }

    public class HeroSlotItem
    {
        public int slotNum;
        public List<int> skillIDList;
    }

    public class SystemControlWndView : UIBaseSubWndView
    {
        public List<KeyCodeItem> m_KeyCodeMap = new List<KeyCodeItem>();
        public Dictionary<int, HeroSlotItem> slotDict = new Dictionary<int, HeroSlotItem>();
        public ToggleGroup spellState;
        public Toggle[] spellStateToggleGroup;
        public Sprite smartSelected;
        public Sprite normalSelected;

        private int nSpellTypeState = 0;
        private bool bFirstLoad = true;
        private string[] SKILL_USE_TYPE_TEXT = new string[] { "默认", "智能", "常规", "快捷" };
        public override bool Init(IUISubWnd wnd)
        {
            base.Init(wnd);

            EntityView curHeroView = EntityFactory.MainHeroView;
            InitSpellSlotConfigCsv(ViewConfigManager.GetConfigFullPath("SpellSlot.csv", true));
            //SettingKeyCodeInfo(curHeroView);

            return true;
        }

        private void InitSpellSlotConfigCsv(string stPath)
        {
            if (slotDict != null || slotDict.Count > 0)
                return;
            if (!File.Exists(stPath))
                return;

            ScpReader reader = new ScpReader(stPath, true, 2);
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int nCol = 0;
                //读取表中记录的数据类型(HeroGemPageSettingConfig)
                int nHeroID = reader.GetInt(i, nCol, 0);
                int nGemPageIndex = reader.GetInt(i, nCol++, 0);
                HeroSlotItem item = new HeroSlotItem();
                const int slotNumCol = 13;
                item.slotNum = reader.GetInt(i, slotNumCol, 0);
                item.skillIDList = new List<int>();
                for (int n = 1; n <= item.slotNum; n++)
                {
                    int skillID = reader.GetInt(i, slotNumCol + n, 0);
                    item.skillIDList.Add(skillID);
                }
                slotDict.Add(nHeroID, item);
            }
        }

        public override void OnDestroy()
        {
            StopAllCoroutines();
        }

        //玩家鼠标点击任意区域，代表改键完成，终止掉携程
        public void Update()
        {
            if (InputManager.Raw_GetMouseButtonDown(0) || InputManager.Raw_GetMouseButtonDown(1) || InputManager.Raw_GetMouseButtonDown(2))
                StopAllCoroutines();
        }

        //隐藏界面也需要终止掉携程
        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if (!visible)
                StopAllCoroutines();
        }


        //public void OnDefaultKeyCode(bool bActive)
        //{
        //    if (!bActive || nSpellTypeState == 0)
        //        return;

        //    foreach (KeyCodeItem temp in m_KeyCodeMap)
        //    {
        //        if (temp.keyCodeToggleBtn != null)
        //        {
        //            if(EntityFactory.MainHeroView!=null)
        //                SettingKeyCodeInfo(EntityFactory.MainHeroView, true);
        //        }
        //    }
        //    nSpellTypeState = -1;
        //}

        //快捷施法Toggle点击事件
        public void OnSmartKeyCode(bool bActive)
        {
            if (!bActive || nSpellTypeState == (int)SpellMode.ST_Speed)
                return;

            foreach (KeyCodeItem temp in m_KeyCodeMap)
            {
                if (temp.toggleText != null && temp.keyIndex != PlayerSlotSettingConfig.UNUSEABLE_INDEX)
                {
                    temp.useType = (int)SpellMode.ST_Speed;
                    temp.toggleText.text = SKILL_USE_TYPE_TEXT[temp.useType];
                }
            }
            nSpellTypeState = (int)SpellMode.ST_Speed;
        }

        //常规施法Toggle点击事件
        public void OnNormalKeyCode(bool bActive)
        {
            if (!bActive || nSpellTypeState == (int)SpellMode.ST_Normal)
                return;

            foreach (KeyCodeItem temp in m_KeyCodeMap)
            {
                if (temp.toggleText != null && temp.keyIndex != PlayerSlotSettingConfig.UNUSEABLE_INDEX)
                {
                    temp.useType = (int)SpellMode.ST_Normal;
                    temp.toggleText.text = SKILL_USE_TYPE_TEXT[temp.useType];
                }
            }
            nSpellTypeState = (int)SpellMode.ST_Normal;
        }

        public void OnAdvanceKeyCode(bool bActive)
        {
            if (!bActive || nSpellTypeState == (int)SpellMode.ST_Smart)
                return;

            foreach (KeyCodeItem temp in m_KeyCodeMap)
            {
                if (temp.toggleText != null && temp.keyIndex != PlayerSlotSettingConfig.UNUSEABLE_INDEX)
                {
                    temp.useType = (int)SpellMode.ST_Smart;
                    temp.toggleText.text = SKILL_USE_TYPE_TEXT[temp.useType];
                }
            }
            nSpellTypeState = (int)SpellMode.ST_Smart;
        }

        //改键按钮事件
        public void OnKeyCodeBtnClick(int slotIndex)
        {
            StopAllCoroutines();
            StartCoroutine(ChangeKey(slotIndex));
        }

        //功能键改键按钮点击事件
        public void OnFunKeyCodeBtnClick(int type)
        {

        }

        //智能施法状态切换事件
        public void OnKeyStateBtnClick(int slotIndex)
        {
            foreach (KeyCodeItem temp in m_KeyCodeMap)
            {
                if (temp.keyIndex == slotIndex && temp.toggleText != null &&  temp.keyIndex != PlayerSlotSettingConfig.UNUSEABLE_INDEX)
                {
                    temp.useType = (temp.useType % 3) + 1;
                    temp.toggleText.text = SKILL_USE_TYPE_TEXT[temp.useType];
                    SetToggleDefault();
                }
            }
        }

        public void SaveSettingData()
        {
            int nSpellMode = 0;
            for (int i = 0; i < spellStateToggleGroup.Length; i++)
            {
                if (spellStateToggleGroup[i].isOn)
                {
                    nSpellMode = i+1;
                    break;
                }
            }

            List<PlayerSlotItem> slotList = new List<PlayerSlotItem>();
            foreach (KeyCodeItem temp in m_KeyCodeMap)
            {
                int autoCasting = temp.useType;
                SlotKeyCodeConfig.Instance.ChangeSlotData(temp.keyIndex, autoCasting, temp.keyCodeText.text.ToLower());
                int slotID = GetSlotIDByIndex(temp.keyIndex);
                if (slotID > 0) {
                    slotList.Add(new PlayerSlotItem(temp.keyIndex, autoCasting));
                }
                else if(temp.keyIndex!= PlayerSlotSettingConfig.UNUSEABLE_INDEX)
                {
                    Debug.LogError("保存失败 : " + temp.keyIndex);
                }
            }
            SlotKeyCodeConfig.Instance.CurrentSpellMode = (SpellMode)nSpellMode;
            if(!U3D_Render.EntityView.isNullOrEmpty(EntityFactory.MainHeroView) && EntityFactory.MainHeroView.Property != null)
            {
                PlayerSlotSettingConfig.Instance.ModifyLocalData(EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION), slotList);
            }
        }

        public void RestoreSettingData(EntityView view)
        {
            SetToggleDefault();
            SettingKeyCodeInfo(view,true);
        }
        public void RefreshSettingData(EntityView view)
        {
            SettingKeyCodeInfo(view);
        }

        private int GetSlotIDByIndex(int index)
        {
            foreach (KeyValuePair<int, SlotKeyCodeData> kv in SlotKeyCodeConfig.Instance.CurrentSlotMap)
            {
                if (kv.Key == index)
                {
                    int maxSlotIndexForEachType = 1000; //一类slot最多有多少个index，用于将index和type合并编码
                    return kv.Value.nSlotType * maxSlotIndexForEachType + kv.Value.nSlotIndex;
                }
            }
            return -1;
        }

        //将toggle恢复到默认全部不勾选状态，将当前spellMode设为-1
        public void SetToggleDefault()     
        {
            SlotKeyCodeConfig.Instance.CurrentSpellMode = SpellMode.ST_Default;
            spellState.SetAllTogglesOff();
        }


        public void SettingKeyCodeInfo(EntityView view,bool isForceDefault = false)
        {
            nSpellTypeState = (int)SlotKeyCodeConfig.Instance.CurrentSpellMode;
            if (this.gameObject.activeInHierarchy && nSpellTypeState >= 1)
                spellState.NotifyToggleOn(spellStateToggleGroup[nSpellTypeState - 1]);
            if (EntityFactory.MainHero == null)
                return;
            SkillCtrl skillCtrl = EntityFactory.MainHero.GetComponent<SkillCtrl>();

            foreach (KeyValuePair<int, SlotKeyCodeData> kv in SlotKeyCodeConfig.Instance.CurrentSlotMap)
            {
                foreach (var temp in m_KeyCodeMap)
                {
                    if (kv.Key == temp.keyIndex && temp.keyCodeText != null)
                    {
                        temp.keyCodeText.text = kv.Value.szKeySequence.ToUpper();
                    }
                    if (kv.Key == temp.keyIndex && temp.keyCodeToggleBtn != null && temp.toggleText != null)
                    {
                        int maxSlotIndexForEachType = 1000; //一类slot最多有多少个index，用于将index和type合并编码
                        int nSlotID = kv.Value.nSlotType * maxSlotIndexForEachType + kv.Value.nSlotIndex;

                        SpellMode useType = SpellMode.ST_Normal;

                        List<PlayerSlotItem> slotList = null;
                        bool hasUserConfig = PlayerSlotSettingConfig.Instance.GetLocalData(EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION), out slotList);
                        if (hasUserConfig && !isForceDefault) //如果用户配置了就使用用户的配置
                        {
                            for (int i = 0; i < slotList.Count; i++)
                            {
                                if (nSlotID == slotList[i].slotID)
                                {
                                    useType = (SpellMode)Enum.Parse(typeof(SpellMode), slotList[i].useType.ToString());
                                }
                            }
                        }
                        else   //默认施法：使用策划在技能编辑器的配置
                        {
                            if (skillCtrl != null)
                            {
                                OrderData odata = skillCtrl.GetCurOrderData(nSlotID);
                                if (odata == null || odata.nAdvanceUseType==0)
                                {
                                    useType = SpellMode.ST_Normal;
                                }
                                else
                                {
                                    useType = (SpellMode)Enum.Parse(typeof(SpellMode), (odata.nAdvanceUseType).ToString());
                                }
                            }
                        }
                        temp.useType = (int)useType;
                        switch (useType)
                        {
                            case SpellMode.ST_Normal:      //常规施法
                                temp.toggleText.text = SKILL_USE_TYPE_TEXT[temp.useType]; 
                                break;
                            case SpellMode.ST_Speed:      //快捷施法
                                temp.toggleText.text = SKILL_USE_TYPE_TEXT[temp.useType];
                                break;
                            case SpellMode.ST_Smart:       //推荐施法
                                temp.toggleText.text = SKILL_USE_TYPE_TEXT[temp.useType];
                                break;
                            default:      //默认施法（不会是这个）
                                temp.toggleText.text = "";
                                break;
                        }

                    }
                }
            }
            bFirstLoad = false;
        }

        #region 监听玩家输入的新键
        /// <summary>
        /// 监听玩家输入的新键
        /// </summary>
        /// <param name="type">要改的键</param>
        /// <param name="FunKeyCode">是否是功能键，因为技能键有改建限制，所以需要区分一下</param>
        /// <returns></returns>
        //IEnumerator ChangeKey(KeyCodeType type, bool FunKeyCode)
        IEnumerator ChangeKey(int slotIndex)
        {
            while (true)
            {
                KeyCode code = KeyCode.None;
                if (InputManager.Raw_AnyKeyDown)
                {
                    foreach (KeyCode keyCode in Enum.GetValues(typeof(KeyCode)))
                    {
                        if (InputManager.Raw_GetKeyDown(keyCode))
                            code = keyCode;
                    }
                }
                if (code != KeyCode.None)
                {
                    string s = code.ToString();
                    if (!((code <= KeyCode.Alpha9 && code >= KeyCode.Alpha0) || (code <= KeyCode.Z && code >= KeyCode.A) || (code <= KeyCode.F8 && code >= KeyCode.F1) || code == KeyCode.Space))
                        s = "";

                    if (code == KeyCode.LeftShift || code == KeyCode.RightShift)
                        s = "SHIFT";

                    if (code == KeyCode.LeftControl || code == KeyCode.RightControl)
                        s = "CTRL";

                    if (code == KeyCode.LeftAlt || code == KeyCode.RightAlt)
                        s = "ALT";

                    if (code <= KeyCode.Alpha9 && code >= KeyCode.Alpha0)
                    {
                        s = ((int)code - (int)KeyCode.Alpha0).ToString();
                    }
                    foreach (var temp in m_KeyCodeMap)
                    {
                        if (temp.keyIndex == slotIndex)
                        {
                            temp.keyCodeText.text = s;
                        }
                        else
                        {
                            if (temp.keyCodeText.text == s)
                                temp.keyCodeText.text = "";
                        }
                    }
                }
                yield return null;
            }
        }
    }
    #endregion
}
