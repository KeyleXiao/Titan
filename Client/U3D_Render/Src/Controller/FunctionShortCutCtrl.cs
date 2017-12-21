using UnityEngine;
using System.Collections.Generic;
using System;
using GameLogic;
using ASpeedGame.Data.SlotKeyCodeConfig;
public enum FuntionShortCutSlotIndex
{
    None = -1,
    //(注意：以下顺序不要随意更改，如需更改请联系策划修改配置表SlotMapedShortcut.csv对应位置)
    EnterLeft = 0,
    EnterRight,
    DragonBall,
    PropertyPannel,
    SystemPannel,
    WarRecordInfoPannel,
    ControlTraceOpen,
    ControlScanfUpdate,//这个目前没有用了
    ControlLightOpen,
    ControlFloatOpen,
    OpenGoodStore,
    KillSelf,
    MoveCameraPositionToFront,
    //组合键加个前缀
    Com_UpgradeSpellSlot0,
    Com_UpgradeSpellSlot1,
    Com_UpgradeSpellSlot2,
    Com_UpgradeSpellSlot3,
    Com_UpgradeSpellSlot4,
    AttackSign,
    DangerSign,
    AutoForward,
    SelectPKTarget,
    SelectNotAttack,
    Ctrl_1,
    Ctrl_2,
    Ctrl_3,
    Ctrl_4,
    Ctrl_5,
    Ctrl_6,
    UseWing,
    VoiceOpenMic,
    OpenFriend,
    OpenMailBox,
    SelectNPCTarget,
    LockView,
	OpenWarVSInfo,		// 打开战场段位信息窗口

    //以下为写死部分,不存在于配置表中,添加时需要同时在InitFixedShortCutCtrl()中配置对应键值
    Exit,                                   //退出
    EntityViewDebugOpen,      //实体调试
    ChatBoxActiveKey1,           //聊天框激活消失按键
    ChatBoxActiveKey2,        
}

public enum CombinationShortcutKeyIndex
{
    Combination_FirstCutKey = 0,
    Combination_SecondCutKey,
    Combination_ThirdCutKey,
}

public class FuntionShortCutCtrl
{
    private class CombinationShortcutKey
    {
        public List<KeyCode> KeyCodeList;
        public List<string> ModifierKeyCodeList;
        public List<EventModifiers> ModifierList;
        public CombinationShortcutKey()
        {
            ModifierList = new List<EventModifiers>();
            KeyCodeList = new List<KeyCode>();
            ModifierKeyCodeList = new List<string>();
        }
    }
    //　单实例
    private static FuntionShortCutCtrl instance = null;
    //存储按键列表
    private Dictionary<int, CombinationShortcutKey> dicCombinationShortcutKey = new Dictionary<int, CombinationShortcutKey>();
    private Dictionary<string, EventModifiers> dicModifiersCodeMap = new Dictionary<string, EventModifiers>();
    // 主英雄的技能控制键
    private SkillCtrl MyHeroSkillCtrl = null;
    //组合按键在配置里的符号
    private string strCombinType = ";";

    // 获得单实例
    public static FuntionShortCutCtrl GetInstance()
    {
        if (instance == null)
        {
            instance = new FuntionShortCutCtrl();
            instance.InitModifiersCodeMap();
        }
        return instance;
    }

    public static void Destroy()
    {
        FuntionShortCutCtrl.GetInstance().destroy();
    }

    /// <summary>
    /// 检测功能按键是否有按下
    /// </summary>
    /// <param name="keyCtrlIndex">功能按键槽位序号</param>
    /// <returns></returns>
    public static bool CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex keyCtrlIndex)
    {
        if (!FuntionShortCutCtrl.GetInstance().dicCombinationShortcutKey.ContainsKey((int)keyCtrlIndex))
            return false;

        //获取此槽位权限Flag， 如果为0代表全局开启，不限制
        //不为0则与场景保姆中的快捷键方案标识做&运算，判断当前是否有权限使用该槽位
        //如果无权限使用，则直接return掉
        int flag = SlotKeyCodeConfig.Instance.GetSlotFlag((int)SlotType.Function, (int)keyCtrlIndex);
        if (flag != 0 && (flag & GameLogicAPI.getHotkeyIndex()) == 0)
            return false;

        if (FuntionShortCutCtrl.GetInstance().dicCombinationShortcutKey[(int)keyCtrlIndex].ModifierKeyCodeList.Count <= 0)//((int)keyCtrlIndex < (int)FuntionShortCutSlotIndex.Com_UpgradeSpellSlot0)
        {
            return FuntionShortCutCtrl.GetInstance().CheckKeyCodeKeyStatus(keyCtrlIndex, KeyStatus.Down);
        }
        else
        {
            return FuntionShortCutCtrl.GetInstance().CheckCombinationKeyStatus(keyCtrlIndex, KeyStatus.Down);
        }
    }

    /// <summary>
    /// 检测功能按键是否抬起
    /// </summary>
    /// <param name="keyCtrlIndex">功能按键槽位序号</param>
    /// <returns></returns>
    public static bool CheckFunctionShortcutKeyUp(FuntionShortCutSlotIndex keyCtrlIndex)
    {
        if (!FuntionShortCutCtrl.GetInstance().dicCombinationShortcutKey.ContainsKey((int)keyCtrlIndex))
            return false;

        int flag = SlotKeyCodeConfig.Instance.GetSlotFlag((int)SlotType.Function, (int)keyCtrlIndex);
        if (flag != 0 && (flag & GameLogicAPI.getHotkeyIndex()) == 0)
            return false;


        if (FuntionShortCutCtrl.GetInstance().dicCombinationShortcutKey[(int)keyCtrlIndex].ModifierKeyCodeList.Count <= 0)//((int)keyCtrlIndex < (int)FuntionShortCutSlotIndex.Com_UpgradeSpellSlot0)
        {
            return FuntionShortCutCtrl.GetInstance().CheckKeyCodeKeyStatus(keyCtrlIndex, KeyStatus.Up);
        }
        else
        {
            return FuntionShortCutCtrl.GetInstance().CheckCombinationKeyStatus(keyCtrlIndex, KeyStatus.Up);
        }
    }

    /// <summary>
    /// 检测功能按键是否按住
    /// </summary>
    /// <param name="keyCtrlIndex">功能按键槽位序号</param>
    /// <returns></returns>
    public static bool CheckFunctionShortcutKeyPressing(FuntionShortCutSlotIndex keyCtrlIndex)
    {
        if (!FuntionShortCutCtrl.GetInstance().dicCombinationShortcutKey.ContainsKey((int)keyCtrlIndex))
            return false;

        int flag = SlotKeyCodeConfig.Instance.GetSlotFlag((int)SlotType.Function, (int)keyCtrlIndex);
        if (flag != 0 && (flag & GameLogicAPI.getHotkeyIndex()) == 0)
            return false;


        if (FuntionShortCutCtrl.GetInstance().dicCombinationShortcutKey[(int)keyCtrlIndex].ModifierKeyCodeList.Count <= 0)//((int)keyCtrlIndex < (int)FuntionShortCutSlotIndex.Com_UpgradeSpellSlot0)
        {
            return FuntionShortCutCtrl.GetInstance().CheckKeyCodeKeyStatus(keyCtrlIndex, KeyStatus.Pressing);
        }
        else
        {
            return FuntionShortCutCtrl.GetInstance().CheckCombinationKeyStatus(keyCtrlIndex, KeyStatus.Pressing);
        }
    }

    public void ClearAndRest()
    {
        Clear();
        InitFixedShortCutCtrl();
    }

    private void InitFixedShortCutCtrl()
    {
        SetCombinationShortcutKey((int)FuntionShortCutSlotIndex.Exit, KeyCode.Escape);
        SetCombinationShortcutKey((int)FuntionShortCutSlotIndex.EntityViewDebugOpen, KeyCode.F9);
        SetCombinationShortcutKey((int)FuntionShortCutSlotIndex.ChatBoxActiveKey1, KeyCode.KeypadEnter);
        SetCombinationShortcutKey((int)FuntionShortCutSlotIndex.ChatBoxActiveKey2, KeyCode.Return);
    }

    private void destroy()
    {
        dicModifiersCodeMap.Clear();
        dicModifiersCodeMap = null;

        Clear();
        dicCombinationShortcutKey = null;
    }

    private void Clear()
    {
        foreach (var combinationKey in dicCombinationShortcutKey)
        {
            combinationKey.Value.KeyCodeList.Clear();
            combinationKey.Value.KeyCodeList = null;
            combinationKey.Value.ModifierList.Clear();
            combinationKey.Value.ModifierList = null;
            combinationKey.Value.ModifierKeyCodeList.Clear();
            combinationKey.Value.ModifierKeyCodeList = null;
        }
        dicCombinationShortcutKey.Clear();
    }

    // 功能型快捷键消息储存
    public void SaveFuntionShortCutCtrl(IntPtr nShortCutInfo, int nPtrLen)
    {
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(nShortCutInfo, nPtrLen);
        cmd_count nCountdata = helper.get<cmd_count>();
        for (int i = 0; i < nCountdata.nCount; ++i)
        {
            cmd_Slot_Shortcut data = helper.get<cmd_Slot_Shortcut>();
            SaveKeyCodeToDic(data.nSlotId, data.szShortcut);

        }
    }

    public void InitFunctionShortCutCtrl()
    {
        foreach (KeyValuePair<int, SlotKeyCodeData> kv in SlotKeyCodeConfig.Instance.CurrentSlotMap)
        {
            if (kv.Value.nSlotType == (int)SlotType.Function)
                SaveKeyCodeToDic(kv.Value.nSlotIndex, kv.Value.szKeySequence);
        }
    }

    // 储存到容器
    public void SaveKeyCodeToDic(int slotIndex, string keyStr)
    {

        string[] keyList = keyStr.Split(strCombinType.ToCharArray());
        foreach (var key in keyList)
        {

                SetCombinationShortcutKey(slotIndex, key);
            
            //if (dicModifiersCodeMap.ContainsKey(key.ToLower()) && keyStr.Contains(strCombinType))
            //{
            //    SetCombinationShortcutKey(slotIndex, dicModifiersCodeMap[key.ToLower()]);
            //}
            //else
            //{
            //    tmpKeyCode = GetKeyCodeByKeyString(ref tmpKey);
            //    if (tmpKeyCode != KeyCode.None)
            //        SetCombinationShortcutKey(slotIndex, EventModifiers.None, tmpKeyCode);
            //}
        }
    }

       // 改变容器中的键位值
    public void ChangeKeyCodeToDic( int slotIndex,  string keyStr,int nComListIndex)
    {
        if(!dicCombinationShortcutKey.ContainsKey(slotIndex))
        {
            return;
        }

        if(dicCombinationShortcutKey[slotIndex].KeyCodeList == null)
        {
            return;
        }

        if (nComListIndex < 0 || nComListIndex >= dicCombinationShortcutKey[slotIndex].KeyCodeList.Count)
        {
            return;
        }

        keyStr = keyStr.ToLower();
        KeyCode keyCode = GetKeyCodeByKeyString(ref keyStr);
        if (keyCode != KeyCode.None)
        {
            if(!dicCombinationShortcutKey[slotIndex].KeyCodeList.Contains(keyCode))
            {
                dicCombinationShortcutKey[slotIndex].KeyCodeList[nComListIndex] = keyCode;
            }
        }
    }

    // 获得主英雄的技能控制键
    private SkillCtrl GetMyHeroCtrl()
    {
        if (MyHeroSkillCtrl == null)
        {
            GameObject MyHeroGO = EntityFactory.getEntityGameObjectByID(EntityFactory.MainHeroID);
            if (MyHeroGO == null)
            {
                Trace.LogError("get Hero GameObject failed");
                return null;
            }
            MyHeroSkillCtrl = MyHeroGO.GetComponent<SkillCtrl>();
        }
        return MyHeroSkillCtrl;
    }

    // 统一在技能控制键中获取
    public KeyCode GetKeyCodeByKeyString(ref string keyStr)
    {
        KeyCode resultKey = GetMyHeroCtrl().GetKeyCodeByKeyString(ref keyStr);
        if (Application.isEditor)
        {
            if (resultKey >= KeyCode.Alpha0 && resultKey <= KeyCode.Alpha9)
            {
                resultKey += KeyCode.Keypad0 - KeyCode.Alpha0;
            }
        }

        return resultKey;
    }

    // 外界通过单实例，通过脚本中的槽位id获取键值
    public KeyCode GetKeyCodeByIndex(FuntionShortCutSlotIndex keyCtrIndex)
    {

        if (dicCombinationShortcutKey.ContainsKey((int)keyCtrIndex) && dicCombinationShortcutKey[(int)keyCtrIndex].KeyCodeList.Count > 0)
        {

            return dicCombinationShortcutKey[(int)keyCtrIndex].KeyCodeList[0];
        }
        else
        {
            return KeyCode.None;
        }

    }

    /// <summary>
    /// 初始化组合按键功能键信息
    /// </summary>
    private void InitModifiersCodeMap()
    {
        SetModifiersCode("shift", EventModifiers.Shift);
        SetModifiersCode("ctrl", EventModifiers.Control);
        SetModifiersCode("alt", EventModifiers.Alt);
        SetModifiersCode("command", EventModifiers.Command);
        SetModifiersCode("numeric", EventModifiers.Numeric);
        SetModifiersCode("capslock", EventModifiers.CapsLock);
        SetModifiersCode("functionkey", EventModifiers.FunctionKey);
        InitFixedShortCutCtrl();
    }

    /// <summary>
    /// 设置功能键
    /// </summary>
    /// <param name="key">功能键索引</param>
    /// <param name="keyModifier">功能键</param>
    private void SetModifiersCode(string key, EventModifiers keyModifier)
    {
        if (dicModifiersCodeMap.ContainsKey(key))
            dicModifiersCodeMap.Remove(key);
        dicModifiersCodeMap.Add(key, keyModifier);
    }

    /// <summary>
    /// 设置组合键
    /// </summary>
    /// <param name="slotIndex">槽位ID</param>
    /// <param name="keyModifier">功能键</param>
    /// <param name="keyCode">非功能键</param>
    private void SetCombinationShortcutKey(int slotIndex, KeyCode keyCode = KeyCode.None, EventModifiers keyModifier = EventModifiers.None)
    {
        if (!dicCombinationShortcutKey.ContainsKey(slotIndex))
        {
            dicCombinationShortcutKey.Add(slotIndex, new CombinationShortcutKey());
        }
        
        if (keyCode != KeyCode.None && !dicCombinationShortcutKey[slotIndex].KeyCodeList.Contains(keyCode))
            dicCombinationShortcutKey[slotIndex].KeyCodeList.Add(keyCode);

        foreach (KeyValuePair<string, EventModifiers> kvp in dicModifiersCodeMap)
        {
            if (kvp.Value == keyModifier)
            {
                if (!dicCombinationShortcutKey[slotIndex].ModifierKeyCodeList.Contains(kvp.Key))
                    dicCombinationShortcutKey[slotIndex].ModifierKeyCodeList.Add(kvp.Key);
            }
        }
    }

    private void SetCombinationShortcutKey(int _nSlotIndex,string _strKeyCode)
    {
        if (!dicCombinationShortcutKey.ContainsKey(_nSlotIndex))
        {
            dicCombinationShortcutKey.Add(_nSlotIndex, new CombinationShortcutKey());
        }
        KeyCode tmpKeyCode = GetKeyCodeByKeyString(ref _strKeyCode);
        //判断是功能键还是普通按键，分别存
        if (dicModifiersCodeMap.ContainsKey(_strKeyCode.ToLower()))
        {
            if(!dicCombinationShortcutKey[_nSlotIndex].ModifierKeyCodeList.Contains(_strKeyCode.ToLower()))
                dicCombinationShortcutKey[_nSlotIndex].ModifierKeyCodeList.Add(_strKeyCode.ToLower());
        }            
        else
        {
            if (tmpKeyCode != KeyCode.None && !dicCombinationShortcutKey[_nSlotIndex].KeyCodeList.Contains(tmpKeyCode))
                dicCombinationShortcutKey[_nSlotIndex].KeyCodeList.Add(tmpKeyCode);
        }
    }

    private bool CheckKeyCodeKeyStatus(FuntionShortCutSlotIndex keyCtrlIndex, KeyStatus status)
    {
        if (dicCombinationShortcutKey.Count <= 0)
            return false;

        if (!dicCombinationShortcutKey.ContainsKey((int)keyCtrlIndex))
            return false;

        //这个只能是配一个按键
        if (dicCombinationShortcutKey[(int)keyCtrlIndex].KeyCodeList.Count > 1 || dicCombinationShortcutKey[(int)keyCtrlIndex].ModifierKeyCodeList.Count > 0)
            Trace.LogWarning("功能按键槽位为：" + keyCtrlIndex + " 配置上是否有问题？这里只能配一个按键，不能是组合键");

        if (!dicCombinationShortcutKey.ContainsKey((int)keyCtrlIndex) || dicCombinationShortcutKey[(int)keyCtrlIndex].KeyCodeList.Count < 1)
            return false;

        KeyCode keycode = dicCombinationShortcutKey[(int)keyCtrlIndex].KeyCodeList[0];

        if (status == KeyStatus.Down)
            return InputManager.GetKeyDown(keycode);
        else if (status == KeyStatus.Up)
            return InputManager.GetKeyUp(keycode);
        else if (status == KeyStatus.Pressing)
            return InputManager.GetKey(keycode);
        else
            return false;
    }

    private bool CheckCombinationKeyStatus(FuntionShortCutSlotIndex keyCtrlIndex, KeyStatus status)
    {
        if (dicCombinationShortcutKey.Count <= 0)
        {
            return false;
        }

        if (!dicCombinationShortcutKey.ContainsKey((int)keyCtrlIndex))
            return false;

        foreach (var keycode in dicCombinationShortcutKey[(int)keyCtrlIndex].KeyCodeList)
        {
            if (status == KeyStatus.Down && !InputManager.GetKeyDown(keycode))
            {
                return false;
            }
            else if (status == KeyStatus.Up && !InputManager.GetKeyUp(keycode))
            {
                return false;
            }
            else if (status == KeyStatus.Pressing && !InputManager.GetKey(keycode))
            {
                return false;
            }
        }

        foreach(var combKey in dicCombinationShortcutKey[(int)keyCtrlIndex].ModifierKeyCodeList)
        {
            if (GetMyHeroCtrl() == null || !GetMyHeroCtrl().GetKeyStatus(combKey, status))
            {
                return false;
            }
        }

        return true;
    }
}