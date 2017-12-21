using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI.SystemSetting
{
    public class SystemVedioWnd : UIBaseSubWnd<SystemVedioWndView>
    {
        public override int GetWndType()
        {
            return (int)SystemSettingSubWndType.WND_VEDIO;
        }

        public override string GetPrefabPath()
        {
            return "SystemSetting/VedioView";
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
            if(visible && m_wndView)
            {
                m_wndView.SetVedioData();
            }
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_SYSTEMSETTING_APPLYVEDIOSETTING:
                    {
                        if (m_wndView != null)
                            m_wndView.SetVedioData();
                    }
                    break;
                case WndMsgID.WND_MSG_SYSTEMSETTING_RESTORE:
                    {
                        if (m_wndView != null)
                            m_wndView.RestoreSettingData();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    //视频等级
    [Serializable]
    public enum VedioLevel : int
    {
        Invaild,
        Fast,
        Good,
        High,
        Best,
        Max
    }

    //视频项目
    [Serializable]
    public enum VedioItem
    {
        ImageQuality,
        ShadowQuality,
        SceneQuality,
        EffectQuality,
        TextureQuality,
    }

    [Serializable]
    public class VedioItemToggle
    {
        public VedioItem item;
        public VedioLevel level;
        public Text txt;
    }

    public class SystemVedioWndView : UIBaseSubWndView
    {
        public List<VedioItemToggle> m_VedioItemList = new List<VedioItemToggle>();

        public Dropdown Resolution;
        public UButtonGroup ImageLevelBtnGroup;
        public Toggle AutoSetting;
        public Toggle FullScreen;
        public Toggle SSAO;
        public Toggle FXAA;
        public Toggle BloomAndFlares;
        public Toggle AdvanceImangeEffect;
        public float LvImgMargin;
        public float LvImgSpacing;
        public float LvImgWidth;
        public RectTransform LvBg;
        //public Image Shield;
        public Text adjustLabel;
        public Text adjustDesc;
        public Text fullScreenLabel;
        //public Text lowLevel;
        //public Text middleLevel;
        //public Text highLevel;
        //public Text bestLevel;
        public Text imageLabel;
        public Text shadowLabel;
        public Text sceneLabel;
        public Text effectLabel;
        public Text textureLabel;
       // public Text pixelLightLabel;
        //public Text waterLabel;
        public Text tips;
        public Text SSAODesc;
        public Text FXAADesc;
        public Text bloomAndFlaresDesc;
        public Text AdvanceImangeEffectDesc;
        public Text[] normalDesc;
        public Text shadowDesc;
        public Text textureDesc;
        public Text notSupportFullSceen;

        public Text ModifyGameBrightnessDesc;
        public Text ResetGameBrightnessBtnDesc;
        public Slider ModifyGameBrightness;

        private string[] levelDis = new string[5];
        private bool initingVedioData = false;

        public override bool Init(IUISubWnd wnd)
        {
            levelDis[0] = "";
            levelDis[1] = ULocalizationService.Instance.Get("UIView","SystemVedioView","LevelFast");
            levelDis[2] = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "LevelGood");
            levelDis[3] = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "LevelHigh");
            levelDis[4] = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "LevelBest");

            adjustLabel.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "adjustLabel");
            adjustDesc.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "adjustDesc");
            fullScreenLabel.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "fullScreenLabel");
            //lowLevel.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "lowLevel");
            //middleLevel.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "middleLevel");
            //highLevel.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "highLevel");
            //bestLevel.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "bestLevel");
            imageLabel.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "imageLabel");
            shadowLabel.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "shadowLabel");
            sceneLabel.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "sceneLabel");
            effectLabel.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "effectLabel");
            textureLabel.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "textureLabel");
            //pixelLightLabel.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "pixelLightLabel");
            //waterLabel.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "waterLabel");
            tips.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "tips");
            SSAODesc.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "SSAODesc");
            FXAADesc.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "FXAADesc");
            bloomAndFlaresDesc.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "bloomAndFlaresDesc");
            shadowDesc.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "shadowDesc");
            textureDesc.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "textureDesc");
            AdvanceImangeEffectDesc.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "AdvanceImangeEffectDesc");
            ModifyGameBrightnessDesc.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "ModifyGameBrightnessDesc");
            ResetGameBrightnessBtnDesc.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "ResetGameBrightnessBtnDesc");
            foreach (Text txt in normalDesc)
            {
                txt.text = ULocalizationService.Instance.Get("UIView", "SystemVedioView", "normalDesc");
            }

            //不需要自动调整了
            AutoSetting.gameObject.SetActive(false);
            SetResolutionData();
            SetVedioData();

            return base.Init(wnd);
        }

      

        public void RestoreSettingData()
        {
            ImageSetting.SetDefaultSetting();
            SetResolutionData();
            SetVedioData();
        }

        //自动调整按钮
        //public void OnAutoSettingBtnClick()
        //{
        //    ImageSetting.AutoAdjustQuality = AutoSetting.isOn ? 1 : 0;
        //    if (AutoSetting.isOn)
        //    {
        //        Shield.gameObject.SetActive(true);
        //        ImageSetting.AdjustLogic(GameFrameManager.FpsMonitor.AvgFps,true);
        //    }
        //    else
        //        Shield.gameObject.SetActive(false);
        //}
        public void ResetGameBrightessClick()
        {
            ModifyGameBrightness.interactable = false;
            ModifyGameBrightness.value = 1;
            ModifyGameBrightness.interactable = true;
            ImageSetting.GetData().currGameBrightness = 1;
            ImageEffects.ImageEffectManager.API_SetImagEffectParam<ImageEffects.ModifyGameBrightness>(ImageEffects.ModifyGameBrightness.ModifyGameBrightnessKey, ImageSetting.GetData().currGameBrightness);
        }

        public void OnModifyGameBrightessValueChanged(float value)
        {
            if (!ModifyGameBrightness.interactable)
            {
                return;
            }
            ImageSetting.GetData().currGameBrightness = value;
            ImageEffects.ImageEffectManager.API_SetImagEffectParam<ImageEffects.ModifyGameBrightness>(ImageEffects.ModifyGameBrightness.ModifyGameBrightnessKey, ImageSetting.GetData().currGameBrightness);
        }

        public void OnFullScreenBtnClick()
        {
            if (initingVedioData)
            {
                return;
            }
            if(FullScreen.isOn)
            {
                Resolution.interactable = false;

                int index = ImageSetting.GetResolutionIndex(ImageSetting.systemResolutions);
                if (index == Resolution.value)
                {
                    ImageSetting.SetResolution(ImageSetting.systemResolutions, true);
                }
                else
                {
                    Resolution.value = index;
                }

                if(!ImageSetting.isSupportFullScreen)
                {
                    if(notSupportFullSceen)
                    {
                        notSupportFullSceen.enabled = true;
                    }
                }
            }
            else
            {
                Resolution.interactable = true;

                int index = ImageSetting.GetResolutionIndex(ImageSetting.lastWindowResoultion);
                
                if (index == Resolution.value)
                {
                    ImageSetting.SetResolution(ImageSetting.lastWindowResoultion, false);
                }
                else
                {
                    Resolution.value = index;
                }

                if (!ImageSetting.isSupportFullScreen)
                {
                    if (notSupportFullSceen)
                    {
                        notSupportFullSceen.enabled = false;
                    }
                }
            }
            
        }

        public void OnSSAOClick()
        {
            if (!SSAO.interactable)
            {
                return;
            }

            ImageSetting.GetData().SSAOEnabled = SSAO.isOn ? 1 : 0;
            if(SSAO.isOn)
                ImageEffects.ImageEffectManager.API_EnableImageEffect<ImageEffects.SSAOPro>();
            else
                ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.SSAOPro>();
        }

        public void OnFXAAClick()
        {
            if (!FXAA.interactable)
            {
                return;
            }

            ImageSetting.GetData().FXAAEnabled = FXAA.isOn ? 1 : 0;

            if(FXAA.isOn)
                ImageEffects.ImageEffectManager.API_EnableImageEffect<ImageEffects.FXAA>();
            else
                ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.FXAA>();
        }

        public void OnBloomAndFlaresClick()
        {
            if (!BloomAndFlares.interactable)
            {
                return;
            }
            ImageSetting.GetData().BloomAndFlaresEnabled = BloomAndFlares.isOn ? 1 : 0;
            if(BloomAndFlares.isOn)
                ImageEffects.ImageEffectManager.API_EnableImageEffect<ImageEffects.BloomAndFlares>();
            else
                ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.BloomAndFlares>();
        }

        public void OnAdvanceImangeEffectClick()
        {
            if (!AdvanceImangeEffect.interactable)
            {
                return;
            }

            ImageSetting.GetData().AdvanceImageEffectEnabled = AdvanceImangeEffect.isOn ? 1 : 0;
            ImageEffects.ImageEffectManager.AdvanceImangeEffectEnabled = ImageSetting.GetData().AdvanceImageEffectEnabled == 1;
            SetOthersImageSetting();

        }

        //分辨率改变
        public void OnResolutionChange()
        {
            if (initingVedioData)
            {
                return;
            }
            //if(FullScreen.isOn)
            //{
            //    Resolution.interactable = false;
            //}
            ImageSetting.SetResolution(Resolution.options[Resolution.value].text, FullScreen.isOn);
        }

        #region 图像等级调整
        public void OnVolumeChange(UButtonGroupItem oldItem, UButtonGroupItem newItem)
        {
            if (oldItem == newItem)
                return;

            int index = ImageLevelBtnGroup.GetItemIndex(newItem);
            index++;
            if(index > (int)VedioLevel.Invaild && index < (int)VedioLevel.Max)
            {
                int lv = index - 1;
                LvBg.anchoredPosition = new Vector2(LvImgMargin + lv * (LvImgSpacing + LvImgWidth), LvBg.anchoredPosition.y);
                ApplySettingData(VedioItem.ImageQuality, index);
            }
        }

        public void OnLevelLeftBtnClick(int item)
        {
            foreach (VedioItemToggle toggle in m_VedioItemList)
            {
                if (toggle.item == (VedioItem)item)
                {
                    int currentLevel = (int)toggle.level;
                    if (currentLevel == 1)
                    {
                        toggle.level = VedioLevel.Best;
                        toggle.txt.text = levelDis[4];
                    }
                    else
                    {
                        toggle.level = (VedioLevel)(currentLevel - 1);
                        toggle.txt.text = levelDis[currentLevel - 1];
                    }
                    ApplySettingData((VedioItem)item, (int)toggle.level);
                }
            }
        }

        public void OnLevelRightBtnClick(int item)
        {
            foreach (VedioItemToggle toggle in m_VedioItemList)
            {
                if (toggle.item == (VedioItem)item)
                {
                    int currentLevel = (int)toggle.level;
                    if (currentLevel == 4)
                    {
                        toggle.level = VedioLevel.Fast;
                        toggle.txt.text = levelDis[1];
                    }
                    else
                    {
                        toggle.level = (VedioLevel)(currentLevel + 1);
                        toggle.txt.text = levelDis[currentLevel + 1];
                    }
                    ApplySettingData((VedioItem)item, (int)toggle.level);
                }
            }
        }
        #endregion

        #region 应用图像设置数据
        public void ApplySettingData(VedioItem item, int level)
        {
            switch(item)
            {
                case VedioItem.ImageQuality:
                    ImageSetting.SetImageQuality((ImageQuality)level);
                    SetVedioData();
                    break;
                case VedioItem.ShadowQuality:
                    ImageSetting.SetShadowQuality((ShadowQuality)level);
                    break;
                case VedioItem.SceneQuality:
                    ImageSetting.SetSceneRenderingQuality((SceneRenderingQuality)level);
                    break;
                case VedioItem.EffectQuality:
                    ImageSetting.SetEffectQuality((EffectQuality)level);
                    break;
                case VedioItem.TextureQuality:
                    ImageSetting.SetTextureQuality((TextureQuality)level);
                    break;
            }
        }

        //设置分辨率
        public void SetResolutionData()
        {
            List<Dropdown.OptionData> dataList = new List<Dropdown.OptionData>();
            List<string> supportList = ImageSetting.GetSupportResolutionList();

            foreach (string s in supportList)
            {
                Dropdown.OptionData data = new Dropdown.OptionData();
                data.text = s;
                dataList.Add(data);
            }
            if (notSupportFullSceen)
            {
                notSupportFullSceen.enabled = false;
            }
            initingVedioData = true;
            Resolution.options = dataList;
            Resolution.value = ImageSetting.GetResolutionIndex(ImageSetting.GetData().currResolution);
            FullScreen.isOn = ImageSetting.GetData().currWindowType == WindowType.FullScreen;
            Resolution.interactable = !FullScreen.isOn;
            initingVedioData = false;
        }

        private void SetOthersImageSetting()
        {
            SSAO.interactable = AdvanceImangeEffect.isOn;
            SSAO.isOn = ImageSetting.GetData().SSAOEnabled > 0 && AdvanceImangeEffect.isOn;

            BloomAndFlares.interactable = AdvanceImangeEffect.isOn;
            BloomAndFlares.isOn = ImageSetting.GetData().BloomAndFlaresEnabled > 0 && AdvanceImangeEffect.isOn;

            FXAA.interactable = AdvanceImangeEffect.isOn;
            FXAA.isOn = ImageSetting.GetData().FXAAEnabled > 0 && AdvanceImangeEffect.isOn;
        }

        public void SetVedioData()
        {
            //AutoSetting.isOn = ImageSetting.AutoAdjustQuality > 0;
            //if (AutoSetting.isOn)
            //    Shield.gameObject.SetActive(true);
            //else
            //    Shield.gameObject.SetActive(false);


            AdvanceImangeEffect.interactable = false;
            AdvanceImangeEffect.isOn = false;

            SSAO.interactable = false;
            SSAO.isOn = false;

            FXAA.interactable = false;
            FXAA.isOn = false;

            BloomAndFlares.interactable = false;
            BloomAndFlares.isOn = false;

            ModifyGameBrightness.interactable = false;
            ModifyGameBrightness.maxValue = ImageSetting.GetData().MaxGameBrightness;
            ModifyGameBrightness.minValue = ImageSetting.GetData().MinGameBrightness;
            ModifyGameBrightness.value = ImageSetting.GetData().currGameBrightness;
            ModifyGameBrightness.interactable = true;

            int lv = (int)ImageSetting.GetData().currImageQuality - 1;
            ImageLevelBtnGroup.SetItemSelectedItem(lv);

            LvBg.anchoredPosition = new Vector2(LvImgMargin + lv * (LvImgSpacing + LvImgWidth), LvBg.anchoredPosition.y);

            if ((int)ImageSetting.GetData().currImageQuality == (int)VedioLevel.Best || (int)ImageSetting.GetData().currImageQuality == (int)VedioLevel.High)
            {
                AdvanceImangeEffect.interactable = false;
                AdvanceImangeEffect.isOn = ImageSetting.GetData().AdvanceImageEffectEnabled > 0;
                AdvanceImangeEffect.interactable = true;

                SetOthersImageSetting();
            }
            foreach (VedioItemToggle toggle in m_VedioItemList)
            {
                switch (toggle.item)
                {
                    case VedioItem.ShadowQuality:
                        toggle.level = (VedioLevel)((int)ImageSetting.GetData().currShadowQuality);
                        toggle.txt.text = levelDis[(int)ImageSetting.GetData().currShadowQuality];
                        continue;
                    case VedioItem.SceneQuality:
                        toggle.level = (VedioLevel)((int)ImageSetting.GetData().currSceneRenderingQuality);
                        toggle.txt.text = levelDis[(int)ImageSetting.GetData().currSceneRenderingQuality];
                        continue;
                    case VedioItem.EffectQuality:
                        toggle.level = (VedioLevel)((int)ImageSetting.GetData().currEffectQuality);
                        toggle.txt.text = levelDis[(int)ImageSetting.GetData().currEffectQuality];
                        continue;
                    case VedioItem.TextureQuality:
                        toggle.level = (VedioLevel)((int)ImageSetting.GetData().currTextureQuality);
                        toggle.txt.text = levelDis[(int)ImageSetting.GetData().currTextureQuality];
                        continue;
                }
            }
        }
        #endregion

    }
}
