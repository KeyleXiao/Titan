using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;
using UnityEngine;
using GameLogic;
using USpeedUI;
using ASpeedGame.Data.PlayerSettingLocalConfig;

namespace ASpeedGame.Data.SlotKeyCodeConfig
{
    //默认槽位快捷键配置表列
    public enum DefaultShortcutsTableCol : int
    {
        SlotID = 0,
        SlotType,
        ShortCutKey,
        ReleaseCondition,
        SkillType,
        SmartCast,
        Flag,
        FuncDesc,
    }

    public enum SlotType : int
    {
        Invalid,

        Equip = 1,         //装备
        Skill,             //技能
        Function,          //功能

        Max,
    }

    public enum SpellMode : int
    {
        ST_Default = 0, //默认施法
        ST_Smart, //智能施法
        ST_Normal, //常规施法
        ST_Speed,  //快捷施法
    }

    public class SlotKeyCodeData
    {
        public int nSlotIndex;        //槽位索引
        public int nSlotType;         //槽位类型
        public int nReleaseCondition; //释放条件
        public int nAutoCasting;      //智能施法状态
        public int nFlag;             //标识
        public string szKeySequence;  //快捷键键值
        public string szSlotFunction; //快捷键作用

        public SlotKeyCodeData(){}
        public SlotKeyCodeData(int nIndex, int nType, int nCondition, int nAuto, int flag, string szKeyCode, string szFunc)
        {
            nSlotIndex = nIndex;
            nSlotType = nType;
            nReleaseCondition = nCondition;
            nAutoCasting = nAuto;
            nFlag = flag;
            szKeySequence = szKeyCode;
            szSlotFunction = szFunc;
        }
    }

    /// <summary>
    /// 槽位对应快捷键相关配置
    /// </summary>
    public class SlotKeyCodeConfig
    {
        //当前槽位表
        public Dictionary<int, SlotKeyCodeData> CurrentSlotMap = new Dictionary<int,SlotKeyCodeData>();
        //默认槽位对应表
        private Dictionary<int, SlotKeyCodeData> defaultSlotMap = new Dictionary<int,SlotKeyCodeData>();
        public SpellMode CurrentSpellMode = SpellMode.ST_Default;
        private SpellMode defaultSpellMode = SpellMode.ST_Default;
        private string spellTypeNameInCsv = "SpellType";
        public int MaxSlotPerType { get { return m_maxSlotPerType; } }
        //每类槽位最大索引值，用于合并编码
        private const int m_maxSlotPerType = 1000;
        private static SlotKeyCodeConfig singleton = new SlotKeyCodeConfig();

        public static SlotKeyCodeConfig Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new SlotKeyCodeConfig();
            }
        }

        public void Load(string stDefaultPath)
        {
            //读默认配置表
            LoadDefaultConfig(stDefaultPath);
            //读取玩家本地配置表并覆盖相关项目
            LoadUserAndCoverConfig();          
        }

        public void UnLoad()
        {
            SaveConfig();   

            if (CurrentSlotMap != null)
            {
                CurrentSlotMap.Clear();
                CurrentSlotMap = null;
            }
            if (defaultSlotMap != null)
            {
                defaultSlotMap.Clear();
                defaultSlotMap = null;
            }
            singleton = null;
        }

        /// <summary>
        /// 设置槽位信息
        /// </summary>
        public void SetSlotData(bool isForceDefault = false)
        {
            if (!EntityFactory.MainHero)
                return;
            SkillCtrl sc = EntityFactory.MainHero.GetComponent<SkillCtrl>();
            if (!sc)
                return;
            int vocationID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
            foreach (KeyValuePair<int, SlotKeyCodeData> kv in CurrentSlotMap)
            {
                switch (kv.Value.nSlotType)
                {
                    case (int)SlotType.Equip:
                    case (int)SlotType.Skill:
                        {
                            sc.SetSlotKeyCode(kv.Value.nSlotType, kv.Value.nSlotIndex, kv.Value.nReleaseCondition, kv.Value.szKeySequence);
                            UIMsgCmdData uiPlayerProgress = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBUTTOMCENTER_SETSHORTCUT, kv.Value.nSlotType, kv.Value.szKeySequence.ToUpper(), IntPtr.Zero, kv.Value.nSlotIndex);
                            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SETSHORTCUT, uiPlayerProgress);
                            cmd_entity_set_smart_cast data = new cmd_entity_set_smart_cast();
                            List<PlayerSlotItem> slotList = null;
                            int maxSlotIndexForEachType = 1000; //一类slot最多有多少个index，用于将index和type合并编码
                            int nSlotID = kv.Value.nSlotType * maxSlotIndexForEachType + kv.Value.nSlotIndex;
                            SpellMode useType = SpellMode.ST_Normal;
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
                                if (sc != null)
                                {
                                    OrderData odata = sc.GetCurOrderData(nSlotID);
                                    if (odata == null)
                                    {
                                        useType = SpellMode.ST_Normal;
                                    }
                                    else
                                    {
                                        useType = (SpellMode)Enum.Parse(typeof(SpellMode), (odata.nAdvanceUseType).ToString());
                                    }
                                }
                            }
                            kv.Value.nAutoCasting = (int)useType;
                            data.nSlotIndex = kv.Value.nSlotIndex;
                            data.nSlotType = kv.Value.nSlotType;
                            data.nSmartCast = kv.Value.nAutoCasting;
                            slotSmartDatas[0] = data;
                            SendSmartDataToLogic();
                            slotSmartDatas.Clear();
                        }
                        break;
                    case (int)SlotType.Function:
                        FuntionShortCutCtrl.GetInstance().SaveKeyCodeToDic(kv.Value.nSlotIndex, kv.Value.szKeySequence);
                        break;
                    default:
                        break;
                }
            }
        }

        /// <summary>
        /// 根据槽位获取快捷键键值
        /// </summary>
        /// <param name="nType">槽位类型</param>
        /// <param name="nIndex">槽位索引</param>
        /// <returns></returns>
        public string GetSlotKeyCode(SlotType nType, int nIndex)
        {
            string szCode = "";

            int key = (int)nType * m_maxSlotPerType + nIndex;
            if (CurrentSlotMap.ContainsKey(key))
            {
                szCode = CurrentSlotMap[key].szKeySequence;
            }

            return szCode;
        }

        public int GetSlotFlag(int slotType, int slotIndex)
        {
            int index = slotType * m_maxSlotPerType + slotIndex;
            if (defaultSlotMap.ContainsKey(index))
                return defaultSlotMap[index].nFlag;
            return 0;
        }

        public void MatchSlotData(int slotType, int slotIndex)
        {
            if (EntityFactory.MainHero == null)
                return;

            SkillCtrl sc = EntityFactory.MainHero.GetComponent<SkillCtrl>();
            int index = slotType * m_maxSlotPerType + slotIndex;
            if (!sc || !CurrentSlotMap.ContainsKey(index))
                return;

            sc.SetSlotKeyCode(CurrentSlotMap[index].nSlotType, CurrentSlotMap[index].nSlotIndex, CurrentSlotMap[index].nReleaseCondition, CurrentSlotMap[index].szKeySequence);

            cmd_entity_set_smart_cast data = new cmd_entity_set_smart_cast();
            data.nSlotIndex = CurrentSlotMap[index].nSlotIndex;
            data.nSlotType = CurrentSlotMap[index].nSlotType;
            data.nSmartCast = CurrentSlotMap[index].nAutoCasting;
            slotSmartDatas[0] = data;
            SendSmartDataToLogic();
            slotSmartDatas.Clear();
        }

        /// <summary>
        /// 还原默认槽位配置
        /// </summary>
        public void RestSlotData()
        {
            CurrentSlotMap.Clear();
            CurrentSpellMode = defaultSpellMode;
            foreach (KeyValuePair<int, SlotKeyCodeData> kv in defaultSlotMap)
            {
                SlotKeyCodeData data = new SlotKeyCodeData(kv.Value.nSlotIndex, kv.Value.nSlotType, kv.Value.nReleaseCondition, kv.Value.nAutoCasting, kv.Value.nFlag, kv.Value.szKeySequence, kv.Value.szSlotFunction);
                CurrentSlotMap.Add(kv.Key, data);
            }
            FuntionShortCutCtrl.GetInstance().ClearAndRest();
            SetSlotData(true);
        }

        /// <summary>
        /// 修改槽位方法
        /// </summary>
        /// <param name="nIndex">槽位索引 槽位类型(slotType) * 每类槽位最大数量(1000)+当前槽位索引(slotIndex)</param>
        /// <param name="nAuto">智能施法状态(0:默认；1:快捷 ；2:常规；3:推荐)</param>
        /// <param name="szKeyCode">槽位键值</param>
        public void ChangeSlotData(int nIndex, int nAuto, string szKeyCode)
        {
            if (CurrentSlotMap.ContainsKey(nIndex))
            {
                int index = CurrentSlotMap[nIndex].nSlotIndex;
                int type = CurrentSlotMap[nIndex].nSlotType;

                if (szKeyCode != CurrentSlotMap[nIndex].szKeySequence)
                {
                    CurrentSlotMap[nIndex].szKeySequence = szKeyCode;

                    if (type == (int)SlotType.Function)
                    {
                        FuntionShortCutCtrl.GetInstance().ChangeKeyCodeToDic(index, szKeyCode, 0);
                    }
                    else
                    {
                        if (!EntityFactory.MainHero)
                            return;
                        SkillCtrl sc = EntityFactory.MainHero.GetComponent<SkillCtrl>();
                        if (sc != null)
                        {
                            sc.SetSlotKeyCode(type, index, CurrentSlotMap[nIndex].nReleaseCondition, szKeyCode);
                            UIMsgCmdData uiPlayerProgress = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBUTTOMCENTER_SETSHORTCUT, type, szKeyCode.ToUpper(), IntPtr.Zero, index);
                            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SETSHORTCUT, uiPlayerProgress);
                        }
                    }
                }

                if (nAuto != CurrentSlotMap[nIndex].nAutoCasting)
                {
                    if (type == (int)SlotType.Function)
                        return;

                    CurrentSlotMap[nIndex].nAutoCasting = nAuto;
                    cmd_entity_set_smart_cast data = new cmd_entity_set_smart_cast();
                    data.nSlotIndex = index;
                    data.nSlotType = type;
                    data.nSmartCast = nAuto;
                    slotSmartDatas[0] = data;
                    SendSmartDataToLogic();
                    slotSmartDatas.Clear();
                }
            }
        }

        private void LoadDefaultConfig(string stPath)
        {
            //走mpk包暂不判断文件
            //if (!File.Exists(stPath))
            //{
            //    Trace.Error("配置表：SlotMapedShortcut.csv 不存在！");
            //    return;
            //}

            ScpReader reader = new ScpReader(stPath, true, 2);

            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SlotKeyCodeData sConfig = new SlotKeyCodeData();
                sConfig.nSlotIndex = reader.GetInt(i, (int)DefaultShortcutsTableCol.SlotID, 0);
                sConfig.nSlotType = reader.GetInt(i, (int)DefaultShortcutsTableCol.SlotType, 0);
                sConfig.szKeySequence = reader.GetString(i, (int)DefaultShortcutsTableCol.ShortCutKey, "");
                sConfig.nReleaseCondition = reader.GetInt(i, (int)DefaultShortcutsTableCol.ReleaseCondition, 0);
                sConfig.nAutoCasting = reader.GetInt(i, (int)DefaultShortcutsTableCol.SmartCast, 0);
                sConfig.nFlag = reader.GetInt(i, (int)DefaultShortcutsTableCol.Flag, 0);
                sConfig.szSlotFunction = reader.GetString(i, (int)DefaultShortcutsTableCol.FuncDesc, "");
                int index = sConfig.nSlotType * m_maxSlotPerType + sConfig.nSlotIndex;
                if (defaultSlotMap.ContainsKey(index))
                    defaultSlotMap[index] = sConfig;
                else
                    defaultSlotMap.Add(index, sConfig);
            }
            foreach (KeyValuePair<int, SlotKeyCodeData> kv in defaultSlotMap)
            {
                SlotKeyCodeData data = new SlotKeyCodeData(kv.Value.nSlotIndex, kv.Value.nSlotType, kv.Value.nReleaseCondition, kv.Value.nAutoCasting, kv.Value.nFlag, kv.Value.szKeySequence, kv.Value.szSlotFunction);
                CurrentSlotMap.Add(kv.Key, data);
            }
        }

        private void LoadUserAndCoverConfig()
        {
            List<PlayerSettingData> tempList;
            if (PlayerSettingLocalConfig.PlayerSettingLocalConfig.Instance.GetLocalData(PlayerSettingDataType.shortcutsSetting, out tempList))
            {
                foreach (PlayerSettingData data in tempList)
                {
                    if (data.m_DataName == spellTypeNameInCsv)
                    {
                        int type = Int32.Parse(data.m_DataValueList[0]);
                        if (type < 0 || type > 2)
                            type = 0;
                        CurrentSpellMode = (SpellMode)type;
                    }

                    if(CurrentSlotMap.ContainsKey(Int32.Parse(data.m_DataValueList[0])))
                    {
                        CurrentSlotMap[Int32.Parse(data.m_DataValueList[0])].szKeySequence = data.m_DataValueList[1];
                        CurrentSlotMap[Int32.Parse(data.m_DataValueList[0])].nAutoCasting = Int32.Parse(data.m_DataValueList[2]);
                    }
                }
            }
        }

        private void SaveConfig()
        {
            List<PlayerSettingData> dataList = new List<PlayerSettingData>();

            PlayerSettingData spellType = new PlayerSettingData(PlayerSettingDataType.shortcutsSetting, spellTypeNameInCsv, ((int)CurrentSpellMode).ToString());
            dataList.Add(spellType);

            foreach (KeyValuePair<int, SlotKeyCodeData> kvp in CurrentSlotMap)
            {
                PlayerSettingData data = new PlayerSettingData(PlayerSettingDataType.shortcutsSetting, kvp.Value.szSlotFunction, kvp.Key.ToString(), kvp.Value.szKeySequence, kvp.Value.nAutoCasting.ToString());
                dataList.Add(data);
            }
            if (dataList.Count > 0)
                PlayerSettingLocalConfig.PlayerSettingLocalConfig.Instance.ModifyLocalData(PlayerSettingDataType.shortcutsSetting, dataList);
        }

        private static Hashtable slotSmartDatas = new Hashtable();
        /// <summary>
        /// 向逻辑层发送智能施法状态数据
        /// </summary>
        private void SendSmartDataToLogic()
        {
            cmd_entity_set_smart_cast tempSmartData = new cmd_entity_set_smart_cast();
            int smartDataSize = Marshal.SizeOf(tempSmartData);
            cmd_entity_set_smart_cast_head tempSmartDataHead = new cmd_entity_set_smart_cast_head();
            int smartDateHeadSize = Marshal.SizeOf(tempSmartDataHead);
            tempSmartDataHead.nCount = slotSmartDatas.Count;
            int len = slotSmartDatas.Count * smartDataSize + smartDateHeadSize;
            IntPtr ptrBytes = Marshal.AllocHGlobal(len);
            Int64 pBytes = ptrBytes.ToInt64();
            IntPtr ptr = new IntPtr(pBytes);
            if (ptr == null || ptr == IntPtr.Zero)
                return;
            Marshal.StructureToPtr(tempSmartDataHead, ptr, false);
            pBytes += smartDateHeadSize;
            foreach (DictionaryEntry keypair in slotSmartDatas)
            {
                int id = (int)keypair.Key;
                cmd_entity_set_smart_cast data = (cmd_entity_set_smart_cast)keypair.Value;
                IntPtr ptrInfo = new IntPtr(pBytes);
                Marshal.StructureToPtr(data, ptrInfo, false);
                pBytes += smartDataSize;
            }
            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_SET_SMART_CAST, len, "", ptrBytes, len);
            Marshal.FreeHGlobal(ptrBytes);
        }
    }
}

