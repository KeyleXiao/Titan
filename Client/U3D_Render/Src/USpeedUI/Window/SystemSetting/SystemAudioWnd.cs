using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using ASpeedGame.Data.GameSettingConfig;

namespace USpeedUI.SystemSetting
{
    public class SystemAudioWnd : UIBaseSubWnd<SystemAudioWndView>
    {
        public override int GetWndType()
        {
            return (int)SystemSettingSubWndType.WND_AUDIO;
        }

        public override string GetPrefabPath()
        {
            return "SystemSetting/AudioView";
        }

        public override bool Init()
        {
            base.Init();

            SetVisible(false);
            return true;
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch(msgID)
            {
                case WndMsgID.WND_MSG_SYSTEMSETTING_RESTORE:
                    if (m_wndView != null)
                        m_wndView.RestoreSettingData();
                    break;
                default:
                    break;
            }
        }
    }

    //声音类型
    [Serializable]
    public enum UIAudioType
    {
        MainAudio,
        BGMAudio,
        SkillAudio,
        SystemAudio,
        SpeechAudio,
    }

    [Serializable]
    public class AudioSlider
    {
        public UIAudioType audioType;
        public Slider audioSlider;
        public Text volumeTxt;
    }
    public class SystemAudioWndView : UIBaseSubWndView
    {
        public List<AudioSlider> m_AudioSlider = new List<AudioSlider>();
        public Toggle muteToggle;
        //private GameSettingsXml xmlNode;
        public Text musicOn;
        public Text mainMusic;
        public Text bgmMusic;
        public Text skillMusic;
        public Text systemMusic;
        public Text speechMusic;

        public override bool Init(IUISubWnd wnd)
        {
            base.Init(wnd);

            musicOn.text = ULocalizationService.Instance.Get("UIView", "SystemAudioView", "musicOn");
            mainMusic.text = ULocalizationService.Instance.Get("UIView", "SystemAudioView", "mainMusic");
            bgmMusic.text = ULocalizationService.Instance.Get("UIView", "SystemAudioView", "bgmMusic");
            skillMusic.text = ULocalizationService.Instance.Get("UIView", "SystemAudioView", "skillMusic");
            systemMusic.text = ULocalizationService.Instance.Get("UIView", "SystemAudioView", "systemMusic");
            speechMusic.text = ULocalizationService.Instance.Get("UIView", "SystemAudioView", "speechMusic");

            //xmlNode = GameSettingsXmlManager.Instance.GameSettingsModel;
            InitAudioData();

            return true;
        }

        public override void SetVisible(bool visible)
        {
            if (!visible)
                RestoreSettingData();
            
            base.SetVisible(visible);
        }

        public void RestoreSettingData()
        {
            InitAudioData();
            SoundManager.SetAudioMute(!muteToggle.isOn);
            foreach (AudioSlider temp in m_AudioSlider)
            {
                if (temp.audioType == UIAudioType.MainAudio)
                {
                    SoundManager.SetMainVolume(temp.audioSlider.value);
                }
                else if (temp.audioType == UIAudioType.BGMAudio)
                {
                    SoundManager.SetSoundVolume(SoundSystem.EMSoundType.Environment, temp.audioSlider.value);
                }
            }
        }

        private void InitAudioData()
        {
            //muteToggle.isOn = !xmlNode.muteAudio.AValue;
            muteToggle.isOn = !GameSettingConfig.Instance.GetGameSettingBoolValue(GameSettingIndex.MuteAudio);

            foreach (AudioSlider temp in m_AudioSlider)
            {
                switch (temp.audioType)
                {
                    case UIAudioType.MainAudio:
                        temp.audioSlider.value = (float)GameSettingConfig.Instance.GetGameSettingIntValue(GameSettingIndex.MainVolume) / 100;
                        temp.volumeTxt.text = GameSettingConfig.Instance.GetGameSettingIntValue(GameSettingIndex.MainVolume) + "%";
                        break;
                    case UIAudioType.BGMAudio:
                        temp.audioSlider.value = (float)GameSettingConfig.Instance.GetGameSettingIntValue(GameSettingIndex.BGMVolume) / 100;
                        temp.volumeTxt.text = GameSettingConfig.Instance.GetGameSettingIntValue(GameSettingIndex.BGMVolume) + "%";
                        break;
                    case UIAudioType.SkillAudio:
                        temp.audioSlider.value = (float)GameSettingConfig.Instance.GetGameSettingIntValue(GameSettingIndex.SkillSoundVolume) / 100;
                        temp.volumeTxt.text = GameSettingConfig.Instance.GetGameSettingIntValue(GameSettingIndex.SkillSoundVolume) + "%";
                        break;
                    case UIAudioType.SystemAudio:
                        temp.audioSlider.value = (float)GameSettingConfig.Instance.GetGameSettingIntValue(GameSettingIndex.SystemSoundVolume) / 100;
                        temp.volumeTxt.text = GameSettingConfig.Instance.GetGameSettingIntValue(GameSettingIndex.SystemSoundVolume) + "%";
                        break;
                    case UIAudioType.SpeechAudio:
                        temp.audioSlider.value = (float)GameSettingConfig.Instance.GetGameSettingIntValue(GameSettingIndex.SpeechSoundVolume) / 100;
                        temp.volumeTxt.text = GameSettingConfig.Instance.GetGameSettingIntValue(GameSettingIndex.SpeechSoundVolume) + "%";
                        break;
                    default:
                        break;
                }
            }
        }

        //slider响应函数
        public void OnVolumeChange(int type)
        {
            switch (type)
            {
                case (int)UIAudioType.MainAudio:
                    foreach (AudioSlider temp in m_AudioSlider)
                    {
                        if (temp.audioType == UIAudioType.MainAudio)
                        {
                            //xmlNode.mainAudioValue.AValue = (int)(temp.audioSlider.value * 100);
                            GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.MainVolume, ((int)(temp.audioSlider.value * 100)).ToString());
                            temp.volumeTxt.text = (int)(temp.audioSlider.value * 100) + "%";
                            SoundManager.SetMainVolume(temp.audioSlider.value);
                        }
                    }
                    break;
                case (int)UIAudioType.BGMAudio:
                    foreach (AudioSlider temp in m_AudioSlider)
                    {
                        if (temp.audioType == UIAudioType.BGMAudio)
                        {
                            //xmlNode.currentBGMValue.AValue = (int)(temp.audioSlider.value * 100);
                            GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.BGMVolume, ((int)(temp.audioSlider.value * 100)).ToString());
                            temp.volumeTxt.text = (int)(temp.audioSlider.value * 100) + "%";
                            SoundManager.SetSoundVolume(SoundSystem.EMSoundType.Environment, temp.audioSlider.value);
                        }
                    }
                    break;
                case (int)UIAudioType.SkillAudio:
                    foreach (AudioSlider temp in m_AudioSlider)
                    {
                        if (temp.audioType == UIAudioType.SkillAudio)
                        {
                            //xmlNode.currentSkillAudioValue.AValue = (int)(temp.audioSlider.value * 100);
                            GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.SkillSoundVolume, ((int)(temp.audioSlider.value * 100)).ToString());
                            temp.volumeTxt.text = (int)(temp.audioSlider.value * 100) + "%";
                            SoundManager.SetSoundVolume(SoundSystem.EMSoundType.Skill, temp.audioSlider.value);
                        }
                    }
                    break;
                case (int)UIAudioType.SystemAudio:
                    foreach (AudioSlider temp in m_AudioSlider)
                    {
                        if (temp.audioType == UIAudioType.SystemAudio)
                        {
                            //xmlNode.currentSystemAudioValue.AValue = (int)(temp.audioSlider.value * 100);
                            GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.SystemSoundVolume, ((int)(temp.audioSlider.value * 100)).ToString());
                            temp.volumeTxt.text = (int)(temp.audioSlider.value * 100) + "%";
                            SoundManager.SetSoundVolume(SoundSystem.EMSoundType.System, temp.audioSlider.value);
                        }
                    }
                    break;
                case (int)UIAudioType.SpeechAudio:
                    foreach (AudioSlider temp in m_AudioSlider)
                    {
                        if (temp.audioType == UIAudioType.SpeechAudio)
                        {
                            //xmlNode.currentSpeechAudioValue.AValue = (int)(temp.audioSlider.value * 100);
                            GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.SpeechSoundVolume, ((int)(temp.audioSlider.value * 100)).ToString());
                            temp.volumeTxt.text = (int)(temp.audioSlider.value * 100) + "%";
                            SoundManager.SetSoundVolume(SoundSystem.EMSoundType.Speech, temp.audioSlider.value);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        //开启声音按钮
        public void OnMuteToggleClick(bool bmute)
        {
            if (bmute)
                SoundManager.SetAudioMute(false);
            else
                SoundManager.SetAudioMute(true);

            //xmlNode.muteAudio.AValue = !bmute;
            int result = !bmute ? 1 : 0;
            GameSettingConfig.Instance.ModifyGameSetting(GameSettingIndex.MuteAudio, result.ToString());
        }
    }
}
