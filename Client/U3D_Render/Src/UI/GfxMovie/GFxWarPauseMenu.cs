/*******************************************************************
** 文件名:	GFxWarPauseMenu.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/5/21
** 版  本:	1.0
** 描  述:	游戏战场里面按ESC出现的游戏设置界面
** 应  用:  
 *          对应Flash Layout文件：WarPauseMenu.swf/gfx
**************************** 修改记录 ******************************
** 修改人: 陈言蒙
** 日  期: 2015/09/28
** 描  述: 拆分C#代码中与AS通讯的逻辑
**************************** 修改记录 ******************************
** 修改人: 昔文博
** 日  期: 2016/03/07
** 描  述: 增加主角血量显示、血量数值显示开关
********************************************************************
** 修改人: 昔文博
** 日  期: 2016/04/08
** 描  述: 增加数据项
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using Scaleform;
using GameLogic;
using ASpeedGame.Data.GameSettingsXml;

public class GameSettingIntEventArgs : EventArgs
{
    public string GameSettingEventName;
    public int GameSettingIntValue;
}

public class GameSettingBoolEventArgs : EventArgs
{
    public string GameSettingEventName;
    public bool GameSettingBoolValue;
}

public class GameSettingStringEventArgs : EventArgs
{
    public string GameSettingEventName;
    public string GameSettingStringValue;
}

partial class GFxWarPauseMenu : SFMovieEx
{
    protected Value vSettingWindow;

    protected Value vHotKeyPage;

    protected Value vAudioPage;

    protected Value vVideoPage;

    protected Value vInterfacePge;

    //自动施法checkbox对象
    protected Value vAutoSpellCasting;
    //显示主角血量checkbox对象
    protected Value vShowPlayerBlood;
    //显示血量数值checkbox对象
    protected Value vShowBloodInfo;
    //背景音乐音量
    protected Value vCurrentBGMValue;
    //英雄技能键
    protected Value vHeroKeyCode1;
    protected Value vHeroKeyCode2;
    protected Value vHeroKeyCode3;
    //召唤师技能键
    protected Value vSkillKeyCode1;
    //装备键
    protected Value vPropKeyCode1;
    protected Value vPropKeyCode2;
    protected Value vPropKeyCode3;
    protected Value vPropKeyCode4;
    protected Value vPropKeyCode5;
    protected Value vPropKeyCode6;
    //饰品键
    protected Value vOrnamentKeyCode;
    //英雄技能快捷施法状态
    protected Value vHeroKeyState1;
    protected Value vHeroKeyState2;
    protected Value vHeroKeyState3;
    //召唤师技能快捷施法
    protected Value vSkillKeyState1;
    //装备快捷施法状态
    protected Value vPropKeyState1;
    protected Value vPropKeyState2;
    protected Value vPropKeyState3;
    protected Value vPropKeyState4;
    protected Value vPropKeyState5;
    protected Value vPropKeyState6;
    //饰品快捷施法状态
    protected Value vOrnamentKeyState;

    //修改后是否执行保存或取消操作的标志位
    private bool bSaveHandle;

    private GameSettingsXml GameSettingsXmlModel = null;

    private Event e;

    private Dictionary<string,Value> m_ValueMap = new Dictionary<string,Value>();
    private Dictionary<string, GameSettingAttributeBase<int>> m_AttrIntMap = new Dictionary<string, GameSettingAttributeBase<int>>();
    private Dictionary<string, GameSettingAttributeBase<bool>> m_AttrBoolMap = new Dictionary<string, GameSettingAttributeBase<bool>>();
    private Dictionary<string, GameSettingAttributeBase<string>> m_HotKeyAttrStringMap = new Dictionary<string, GameSettingAttributeBase<string>>();
    private Dictionary<string, GameSettingAttributeBase<int>> m_HotKeyAttrIntMap = new Dictionary<string, GameSettingAttributeBase<int>>();
    
    //int型游戏设置项事件
    public static event EventHandler<GameSettingIntEventArgs> IntEvent;
    public static event EventHandler<GameSettingBoolEventArgs> BoolEvent;
    public static event EventHandler<GameSettingStringEventArgs> StringEvent;

    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);
        GameSettingsXmlModel = GameSettingsXmlManager.Instance.GameSettingsModel;
        bSaveHandle = true;
        e = Event.current;
    }

    override public void Update()
    {
        //if (InputManager.GetKeyUp(KeyCode.Escape) && MyRoot.IsValid())
        //{
        //    if (IsWarPauseMenuVisible)
        //    {
        //        SFGFxMovieManager.Instance.UseHeroMouseCtrl();
        //        LeaveMovie();
        //    }
        //    else
        //    {
        //        SFGFxMovieManager.Instance.UseUIMouseCtrl();
        //        EnterMovie();
        //    }
        //}
    }

    public override void EnterMovie()
    {
        base.EnterMovie();
        this.SetMovieVisible(bIsVisible);
        //如果玩家上次关闭的时候，点击了保存或取消，则根据本地配置设置一遍参数，如果玩家没操作直接关闭了界面，则还原上一次的设置
        if (bSaveHandle)
        {
            SettingLocalValue();
        }
        else
        {
            onGameSettingsCancel();
        }
        bSaveHandle = false;
    }

    public override void LeaveMovie()
    {
        StopAllCoroutines();
        base.LeaveMovie();
        this.SetMovieVisible(bIsVisible);
    }
    public override void OnDestroy()
    {
        PopupMovieManager.Instance.UnBindKeyCodeForMovie(this);
        StopAllCoroutines();
        m_ValueMap.Clear();
        m_ValueMap = null;
        m_AttrIntMap.Clear();
        m_AttrIntMap = null;
        m_AttrBoolMap.Clear();
        m_AttrBoolMap = null;
        m_HotKeyAttrStringMap.Clear();
        m_HotKeyAttrStringMap = null;
        m_HotKeyAttrIntMap.Clear();
        m_HotKeyAttrIntMap = null;
        base.OnDestroy();
    }

    public override bool SetMovieVisible(bool _visible)
    {
        if (_visible)
        {
            SFGFxMovieManager.Instance.UseUIMouseCtrl();
        }
        else
        {
            SFGFxMovieManager.Instance.UseHeroMouseCtrl();
        }
        return base.SetMovieVisible(_visible);
    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
        GetMemberWithCheck(MyRoot, "SettingWindow", out vSettingWindow);
        //this.SetDepth((int)SFGFxMovieManager.EGFxMovieLayer.EGFxLayer_PopupLayer);
        m_ValueMap.Add("HeroKeyCode1", null);
        m_ValueMap.Add("HeroKeyCode2", null);
        m_ValueMap.Add("HeroKeyCode3", null);
        m_ValueMap.Add("SkillKeyCode1", null);
        m_ValueMap.Add("PropKeyCode1", null);
        m_ValueMap.Add("PropKeyCode2", null);
        m_ValueMap.Add("PropKeyCode3", null);
        m_ValueMap.Add("PropKeyCode4", null);
        m_ValueMap.Add("PropKeyCode5", null);
        m_ValueMap.Add("PropKeyCode6", null);
        m_ValueMap.Add("OrnamentKeyCode", null);
        m_HotKeyAttrStringMap.Add("HeroKeyCode1", GameSettingsXmlModel.heroKeyCode1);
        m_HotKeyAttrStringMap.Add("HeroKeyCode2", GameSettingsXmlModel.heroKeyCode2);
        m_HotKeyAttrStringMap.Add("HeroKeyCode3", GameSettingsXmlModel.heroKeyCode3);
        m_HotKeyAttrStringMap.Add("SkillKeyCode1", GameSettingsXmlModel.skillKeyCode1);
        m_HotKeyAttrStringMap.Add("PropKeyCode1", GameSettingsXmlModel.propKeyCode1);
        m_HotKeyAttrStringMap.Add("PropKeyCode2", GameSettingsXmlModel.propKeyCode2);
        m_HotKeyAttrStringMap.Add("PropKeyCode3", GameSettingsXmlModel.propKeyCode3);
        m_HotKeyAttrStringMap.Add("PropKeyCode4", GameSettingsXmlModel.propKeyCode4);
        m_HotKeyAttrStringMap.Add("PropKeyCode5", GameSettingsXmlModel.propKeyCode5);
        m_HotKeyAttrStringMap.Add("PropKeyCode6", GameSettingsXmlModel.propKeyCode6);
        m_HotKeyAttrStringMap.Add("OrnamentKeyCode", GameSettingsXmlModel.ornamentKeyCode);

        m_HotKeyAttrIntMap.Add("HeroKeyState1", GameSettingsXmlModel.heroKeyState1);
        m_HotKeyAttrIntMap.Add("HeroKeyState2", GameSettingsXmlModel.heroKeyState2);
        m_HotKeyAttrIntMap.Add("HeroKeyState3", GameSettingsXmlModel.heroKeyState3);
        m_HotKeyAttrIntMap.Add("SkillKeyState1", GameSettingsXmlModel.skillKeyState1);
        m_HotKeyAttrIntMap.Add("PropKeyState1", GameSettingsXmlModel.propKeyState1);
        m_HotKeyAttrIntMap.Add("PropKeyState2", GameSettingsXmlModel.propKeyState2);
        m_HotKeyAttrIntMap.Add("PropKeyState3", GameSettingsXmlModel.propKeyState3);
        m_HotKeyAttrIntMap.Add("PropKeyState4", GameSettingsXmlModel.propKeyState4);
        m_HotKeyAttrIntMap.Add("PropKeyState5", GameSettingsXmlModel.propKeyState5);
        m_HotKeyAttrIntMap.Add("PropKeyState6", GameSettingsXmlModel.propKeyState6);
        m_HotKeyAttrIntMap.Add("OrnamentKeyState", GameSettingsXmlModel.ornamentKeyState);
        m_AttrBoolMap.Add("ShowPlayerBlood", GameSettingsXmlModel.showPlayerBlood);
        m_AttrBoolMap.Add("ShowBloodInfo", GameSettingsXmlModel.showBloodInfo);
        
        m_AttrIntMap.Add("MusicVolume", GameSettingsXmlModel.currentBGMValue);

        PopupMovieManager.Instance.BindKeyCodeForMovie(FuntionShortCutCtrl.GetInstance().GetKeyCodeByIndex(FuntionShortCutSlotIndex.SystemPannel), this);
        SetMovieVisibleOnCreate();
    }



    //如果玩家点击了确认，则遍历配置表的修改列表，根据对应的类型执行订阅的事件
    public void onGameSettingsConfirm()
    {
        if (GameSettingsXmlModel.CheckSettingsAttIsDirty())
        {
            if (GameSettingsXmlModel != null)
            {
                if (GameSettingsXmlModel.intTypeAttributeDirtyList != null)
                {
                    foreach (GameSettingAttributeBase<int> intAtt in GameSettingsXmlModel.intTypeAttributeDirtyList)
                    {
                        if (IntEvent != null)
                        {
                            GameSettingIntEventArgs e = new GameSettingIntEventArgs();
                            e.GameSettingEventName = intAtt.AType;
                            e.GameSettingIntValue = intAtt.AValue;
                            IntEvent(this, e);
                        }
                    }
                }

                if (GameSettingsXmlModel.boolTypeAttributeDirtyList != null)
                {
                    foreach (GameSettingAttributeBase<bool> boolAtt in GameSettingsXmlModel.boolTypeAttributeDirtyList)
                    {
                        if (BoolEvent != null)
                        {
                            GameSettingBoolEventArgs e = new GameSettingBoolEventArgs();
                            e.GameSettingEventName = boolAtt.AType;
                            e.GameSettingBoolValue = boolAtt.AValue;
                            BoolEvent(this, e);
                        }
                    }
                }

                if (GameSettingsXmlModel.stringTypeAttributeDirtyList != null)
                {
                    foreach (GameSettingAttributeBase<string> strAtt in GameSettingsXmlModel.stringTypeAttributeDirtyList)
                    {
                        if (StringEvent != null)
                        {
                            GameSettingStringEventArgs e = new GameSettingStringEventArgs();
                            e.GameSettingEventName = strAtt.AType;
                            e.GameSettingStringValue = strAtt.AValue;
                            StringEvent(this, e);
                        }
                    }
                }
                GameSettingsXmlModel.SaveGameSettingsDirty();
            }
        }
        //PopupMovieManager.Instance.PopMovie(this);
      // LeaveMovie();
    }

    public void onGameSettingsCancel()
    {
        if (GameSettingsXmlModel.CheckSettingsAttIsDirty())
        {
            if (GameSettingsXmlModel != null)
            {
                GameSettingsXmlModel.RestoreLastGameSettings();
            }
        }
        SettingLocalValue();
        //PopupMovieManager.Instance.PopMovie(this);
        //LeaveMovie();
    }

    #region AS->C#

    /// <summary>
    /// 获取AS GameSubWindowCAC 界面实例
    /// </summary>
    /// <param name="_root"></param>
    public void OnRegisterHotKey(Value _root)
    {
        vHotKeyPage = _root;
        GetMemberWithCheck(vHotKeyPage, "HeroKeyCode1", out vHeroKeyCode1);
        GetMemberWithCheck(vHotKeyPage, "HeroKeyCode2", out vHeroKeyCode2);
        GetMemberWithCheck(vHotKeyPage, "HeroKeyCode3", out vHeroKeyCode3);
        GetMemberWithCheck(vHotKeyPage, "SkillKeyCode1", out vSkillKeyCode1);
        GetMemberWithCheck(vHotKeyPage, "PropKeyCode1", out vPropKeyCode1);
        GetMemberWithCheck(vHotKeyPage, "PropKeyCode2", out vPropKeyCode2);
        GetMemberWithCheck(vHotKeyPage, "PropKeyCode3", out vPropKeyCode3);
        GetMemberWithCheck(vHotKeyPage, "PropKeyCode4", out vPropKeyCode4);
        GetMemberWithCheck(vHotKeyPage, "PropKeyCode5", out vPropKeyCode5);
        GetMemberWithCheck(vHotKeyPage, "PropKeyCode6", out vPropKeyCode6);
        GetMemberWithCheck(vHotKeyPage, "OrnamentKeyCode", out vOrnamentKeyCode);
        GetMemberWithCheck(vHotKeyPage, "HeroKeyState1", out vHeroKeyState1);
        GetMemberWithCheck(vHotKeyPage, "HeroKeyState2", out vHeroKeyState2);
        GetMemberWithCheck(vHotKeyPage, "HeroKeyState3", out vHeroKeyState3);
        GetMemberWithCheck(vHotKeyPage, "SkillKeyState1", out vSkillKeyState1);
        GetMemberWithCheck(vHotKeyPage, "PropKeyState1", out vPropKeyState1);
        GetMemberWithCheck(vHotKeyPage, "PropKeyState2", out vPropKeyState2);
        GetMemberWithCheck(vHotKeyPage, "PropKeyState3", out vPropKeyState3);
        GetMemberWithCheck(vHotKeyPage, "PropKeyState4", out vPropKeyState4);
        GetMemberWithCheck(vHotKeyPage, "PropKeyState5", out vPropKeyState5);
        GetMemberWithCheck(vHotKeyPage, "PropKeyState6", out vPropKeyState6);
        GetMemberWithCheck(vHotKeyPage, "OrnamentKeyState", out vOrnamentKeyState);
        m_ValueMap["HeroKeyCode1"] = vHeroKeyCode1;
        m_ValueMap["HeroKeyCode2"] = vHeroKeyCode2;
        m_ValueMap["HeroKeyCode3"] = vHeroKeyCode3;
        m_ValueMap["SkillKeyCode1"] = vSkillKeyCode1;
        m_ValueMap["PropKeyCode1"] = vPropKeyCode1;
        m_ValueMap["PropKeyCode2"] = vPropKeyCode2;
        m_ValueMap["PropKeyCode3"] = vPropKeyCode3;
        m_ValueMap["PropKeyCode4"] = vPropKeyCode4;
        m_ValueMap["PropKeyCode5"] = vPropKeyCode5;
        m_ValueMap["PropKeyCode6"] = vPropKeyCode6;
        m_ValueMap["OrnamentKeyCode"] = vOrnamentKeyCode;
        SettingHotKey();
    }

    public void OnRegisterAudio(Value _root)
    {
        vAudioPage = _root;
        GetMemberWithCheck(vAudioPage, "MusicVolume", out vCurrentBGMValue);
        SettingAudio();
    }

    public void OnRegisterVideo(Value _root)
    {
        vVideoPage = _root;
        SettingVideo();
    }

    public void OnRegisterInterface(Value _root)
    {
        vInterfacePge = _root;
        GetMemberWithCheck(vInterfacePge, "ShowPlayerBlood", out vShowPlayerBlood);
        GetMemberWithCheck(vInterfacePge, "ShowBloodInfo", out vShowBloodInfo);
        SettingInterface();
    }

    //退出游戏按键
    public void onExitBtnClick()
    {
        //暂时设置为结束当前场景
      //  (SFGFxMovieManager.GetWindow<("GFxDragonBall") as GFxDragonBall).OnLinkClick("SupperStone_EndtheWar(0)");
      ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_USER_EXIT_WAR);

    }

    //认输按钮按键
    public void onAdmitBtnClick()
    {
        //暂时设置为结束当前场景
        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_USER_EXIT_WAR);
    }

    public void onConfirmBtnClick()
    {
        onGameSettingsConfirm();
        bSaveHandle = true;
    }

    public void onCancelBtnClick()
    {
        onGameSettingsCancel();
        bSaveHandle = true;
    }

    public void onCloseButtonClick()
    {
        base.LeaveMovie();
        this.SetMovieVisible(bIsVisible);
    }
    
    //接收设置界面CheckBox消息
    public void onCheckBoxSelect(string _name, bool _state)
    {
        if(GameSettingsXmlModel != null)
        {     
            //switch (_name)
            //{
            //    case "HeroKeyState1":
            //        {
            //            GameSettingsXmlModel.autoSpellCasting.AValue = _state;
            //        }
            //        break;
            //    case "HeroKeyState2":
            //        {
            //            GameSettingsXmlModel.showPlayerBlood.AValue = _state;                  
            //        }
            //        break;
            //}
            if (m_AttrBoolMap.ContainsKey(_name))
            {
                m_AttrBoolMap[_name].AValue = _state;
            }
        }
    }

    public void onSliderValueChange(string _name,double _value)
    {
        if (GameSettingsXmlModel != null)
        {
            if (m_AttrIntMap.ContainsKey(_name))
            {
                m_AttrIntMap[_name].AValue = (int)_value;
            }
        }
    }

    public void onDropdownMenuChange(string _name, int _index)
    {
        if (GameSettingsXmlModel != null)
        {
            if (m_AttrIntMap.ContainsKey(_name))
            {
                m_AttrIntMap[_name].AValue = _index;
            }
        }
    }

    public void onKeyStateSelect(string _name, bool _state)
    {
        if (GameSettingsXmlModel != null)
        {
            if (m_HotKeyAttrIntMap.ContainsKey(_name))
            {
                m_HotKeyAttrIntMap[_name].AValue = _state ? 2 : 1;
            }
        }
    }

    //public void onHotKeycodeFocusIn(string _name)
    //{
    //    if (GameSettingsXmlModel != null)
    //    {
    //        StartCoroutine("Listen",_name);
    //    }
    //}

    //public void onHotKeycodeFocusOut()
    //{
    //    StopAllCoroutines();
    //}

    public void onHotKeycodeClick(string _name)
    {
        StopAllCoroutines();
        StartCoroutine("ChangeKey", _name);
    }

    public void onDefaultStateBtnClick()
    {
        foreach (KeyValuePair<string, GameSettingAttributeBase<int>> temp in m_HotKeyAttrIntMap)
        {
            temp.Value.AValue = 0;
        }
    }

    public void onNormalStateBtnClick()
    {
        foreach (KeyValuePair<string, GameSettingAttributeBase<int>> temp in m_HotKeyAttrIntMap)
        {
            temp.Value.AValue = 1;
        }
    }

    public void onSmartStateBtnClick()
    {
        foreach (KeyValuePair<string, GameSettingAttributeBase<int>> temp in m_HotKeyAttrIntMap)
        {
            temp.Value.AValue = 2;
        }
    }
    #endregion

    //监听键盘输入，实现改键，策划要求，只能改为F1-F8,数字和字母
    private IEnumerator ChangeKey(string _name)
    {
        while (true)
        {
            if (e.isKey && e.keyCode != KeyCode.None)
            {
                string s = e.keyCode.ToString();
                if (!((e.keyCode <= KeyCode.Alpha9 && e.keyCode >= KeyCode.Alpha0) || (e.keyCode <= KeyCode.Z && e.keyCode >= KeyCode.A) || (e.keyCode <= KeyCode.F8 && e.keyCode >= KeyCode.F1)))
                {
                    s = "";
                }
                if (e.keyCode <= KeyCode.Alpha9 && e.keyCode >= KeyCode.Alpha0)
                {
                    s = ((int)e.keyCode - (int)KeyCode.Alpha0).ToString();
                }
                if (m_ValueMap.ContainsKey(_name) && m_HotKeyAttrStringMap.ContainsKey(_name))
                {
                    SetHotKeyBtnLabel(m_ValueMap[_name],s);
                    m_HotKeyAttrStringMap[_name].AValue = s;
                }
                foreach (KeyValuePair<string, GameSettingAttributeBase<string>> keypair in m_HotKeyAttrStringMap)
                {
                    if (keypair.Key != _name && keypair.Value.AValue == s)
                    {
                        keypair.Value.AValue = "";

                        SetHotKeyBtnLabel(m_ValueMap[keypair.Key], keypair.Value.AValue);
                    }
                }
                break;
            }
            yield return null;
        }
    }
}