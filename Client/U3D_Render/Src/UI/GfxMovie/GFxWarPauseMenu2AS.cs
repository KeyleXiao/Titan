/*******************************************************************
** 文件名:	GFxWarPauseMenu.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	陈言蒙
** 日  期:	2015/09/28
** 版  本:	1.0
** 描  述:	游戏状态 GFx界面显示 团队信息, 拆分C#代码中与AS通讯的逻辑
** 应  用:  
 *          对应Flash Layout文件：WarPauseMenu.swf/gfx
********************************************************************
** 修改人: 昔文博
** 日  期: 2016/03/07
** 描  述: 增加主角血量显示、血量数值显示开关
********************************************************************
** 修改人: 昔文博
** 日  期: 2016/04/08
** 描  述: 增加视频、音频界面逻辑
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using Scaleform;
using GameLogic;
using ASpeedGame.Data.GameSettingsXml;

partial class GFxWarPauseMenu 
{
    #region C#->AS
    /// <summary>
    /// 设置Checkbox类型的值
    /// </summary>
    /// <param name="_value">类型实例</param>
    /// <param name="_select">是否选择</param>

    public string DefaultLab = "默认施法";
    public string NormalLab = "常规施法";
    public string SmartLab = "快捷施法";

    public void SetCheckBoxValue(Value _value, bool _select)
    {
        if (_value != null && _value.IsValid())
        {
            _value.SetMember("selected", _select);
        }
    }

    public void SetSliderValue(Value _value, int _number)
    {
        if (_value != null && _value.IsValid())
        {
            _value.SetMember("value", _number);
        }
    }

    public void SetDropdownMenuIndex(Value _value, int _index)
    {
        if (_value != null && _value.IsValid())
        {
            _value.SetMember("selectedIndex", _index);
        }
    }

    public void SetHotKeyBtnLabel(Value _value, string _label)
    {
        if (_value != null && _value.IsValid())
        {
            _value.SetMember("label", _label);
        }
    }

    public void SetHotKeyState(Value _value, int _number)
    {
        if (_value != null && _value.IsValid())
        {
            if (_number == 2)
            {
                _value.SetMember("selected", true);
            }
            else
                _value.SetMember("selected", false);
        }
    }

    /// <summary>
    /// 根据GameSettings.xml设置ControlAndCamera页面内的设置选项
    /// </summary>
    public void SettingHotKey()
    {
        if (GameSettingsXmlModel != null && Value.IsValueValid(vHeroKeyCode1))
        {
            SetHotKeyBtnLabel(vHeroKeyCode1, GameSettingsXmlModel.heroKeyCode1.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vHeroKeyCode2))
        {
            SetHotKeyBtnLabel(vHeroKeyCode2, GameSettingsXmlModel.heroKeyCode2.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vHeroKeyCode3))
        {
            SetHotKeyBtnLabel(vHeroKeyCode3, GameSettingsXmlModel.heroKeyCode3.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vSkillKeyCode1))
        {
            SetHotKeyBtnLabel(vSkillKeyCode1, GameSettingsXmlModel.skillKeyCode1.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vPropKeyCode1))
        {
            SetHotKeyBtnLabel(vPropKeyCode1, GameSettingsXmlModel.propKeyCode1.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vPropKeyCode2))
        {
            SetHotKeyBtnLabel(vPropKeyCode2, GameSettingsXmlModel.propKeyCode2.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vPropKeyCode3))
        {
            SetHotKeyBtnLabel(vPropKeyCode3, GameSettingsXmlModel.propKeyCode3.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vPropKeyCode4))
        {
            SetHotKeyBtnLabel(vPropKeyCode4, GameSettingsXmlModel.propKeyCode4.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vPropKeyCode5))
        {
            SetHotKeyBtnLabel(vPropKeyCode5, GameSettingsXmlModel.propKeyCode5.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vPropKeyCode6))
        {
            SetHotKeyBtnLabel(vPropKeyCode6, GameSettingsXmlModel.propKeyCode6.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vOrnamentKeyCode))
        {
            SetHotKeyBtnLabel(vOrnamentKeyCode, GameSettingsXmlModel.ornamentKeyCode.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vHeroKeyState1))
        {
            SetHotKeyState(vHeroKeyState1, GameSettingsXmlModel.heroKeyState1.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vHeroKeyState2))
        {
            SetHotKeyState(vHeroKeyState2, GameSettingsXmlModel.heroKeyState2.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vHeroKeyState3))
        {
            SetHotKeyState(vHeroKeyState3, GameSettingsXmlModel.heroKeyState3.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vSkillKeyState1))
        {
            SetHotKeyState(vSkillKeyState1, GameSettingsXmlModel.skillKeyState1.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vPropKeyState1))
        {
            SetHotKeyState(vPropKeyState1, GameSettingsXmlModel.propKeyState1.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vPropKeyState2))
        {
            SetHotKeyState(vPropKeyState2, GameSettingsXmlModel.propKeyState2.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vPropKeyState3))
        {
            SetHotKeyState(vPropKeyState3, GameSettingsXmlModel.propKeyState3.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vPropKeyState4))
        {
            SetHotKeyState(vPropKeyState4, GameSettingsXmlModel.propKeyState4.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vPropKeyState5))
        {
            SetHotKeyState(vPropKeyState5, GameSettingsXmlModel.propKeyState5.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vPropKeyState6))
        {
            SetHotKeyState(vPropKeyState6, GameSettingsXmlModel.propKeyState6.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vOrnamentKeyState))
        {
            SetHotKeyState(vOrnamentKeyState, GameSettingsXmlModel.ornamentKeyState.AValue);
        }
    }

    public void SettingAudio()
    {
        if (GameSettingsXmlModel != null && Value.IsValueValid(vCurrentBGMValue))
        {
            SetSliderValue(vCurrentBGMValue, GameSettingsXmlModel.currentBGMValue.AValue);
        }
    }

    public void SettingVideo()
    {

    }

    public void SettingInterface()
    {
        if (GameSettingsXmlModel != null && Value.IsValueValid(vShowPlayerBlood))
        {
            SetCheckBoxValue(vShowPlayerBlood, GameSettingsXmlModel.showPlayerBlood.AValue);
        }
        if (GameSettingsXmlModel != null && Value.IsValueValid(vShowBloodInfo))
        {
            SetCheckBoxValue(vShowBloodInfo, GameSettingsXmlModel.showBloodInfo.AValue);
        }
    }

    public void SettingLocalValue()
    {
        SettingHotKey();
        SettingAudio();
        SettingVideo();
        SettingInterface();
    }
    #endregion
}