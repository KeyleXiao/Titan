using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using UIWidgets;
using ASpeedGame.Data.GameMobaSchemes;
using U3D_Render.Common;

namespace USpeedUI.WarHeroDetailedInfoPanel
{
    enum PropertyChangeFlag
    {
        PCF_INCREASE,
        PCF_DECREASE,

        PCT_MAX
    }

    public class UWarHeroDetailedInfoPanelWnd : UIPopupWnd<UWarHeroDetailedInfoPanelWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_HERODETAILEDINFOPANEL;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "DynamicState_Moba/WarHeroDetailedInfoPanelView/WarHeroDetailedInfoPanelView";
        }

        public override int GetPermissionMask()
        {
            return (int)WndPermissionMaskDef.WND_PM_Normal;
        }

        public override bool Init()
        {
            m_isKeyClose = true;

            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);

            UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.PropertyPannel, KeyStatus.Down, this);


            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);

            UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.PropertyPannel, KeyStatus.Down, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        this.LoadView();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_WAREND:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        this.UnloadView();
                    }
                    break;
                default:
                    break;
            }
        }

        // 接受按键消息
        public override bool OnKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status)
        {
            if (m_wndView == null)
                return false;
            if (index == FuntionShortCutSlotIndex.PropertyPannel && status == KeyStatus.Down)
            {
                if (!m_wndView.IsVisible())
                {
                    m_wndView.SetVisible(true);
                    m_wndView.SetHeroProperty();
                    m_wndView.SaveHeroPrperty();
                }
                else
                {
                    m_wndView.SetVisible(false);
                }
            }

            return true;
        }
    }

    public class UWarHeroDetailedInfoPanelWndView : UIBaseWndView
    {
        public Text Title;
        public Text SubTitle1;
        public Text SubTitle2;
        public Text[] propertyName;
        public Sprite[] changeIcons;

        private Dictionary<int, int> DicMainHeroProperty = new Dictionary<int, int>();

        private Dictionary<int, int> m_dicLastMainHeroProperty = new Dictionary<int, int>();
        // 攻击、法强穿透特殊存储
        private int m_nOldPCTPDP, m_nPCTPDP = 0;
        private int m_nOldPCTMDP, m_nPCTMDP = 0;

        public override bool Init(IUIWnd wnd)
        {
            Title.text = ULocalizationService.Instance.Get("UIView", "WarHeroDetailedInfoPanel", "Title");
            SubTitle1.text = ULocalizationService.Instance.Get("UIView", "WarHeroDetailedInfoPanel", "SubTitle1");
            SubTitle2.text = ULocalizationService.Instance.Get("UIView", "WarHeroDetailedInfoPanel", "SubTitle2");
            InitPropertyInfo();

            EntityFactory.SetEntityNumPropHandler(OnCommand_UpdateNumProp);
            return base.Init(wnd);
        }

        private void InitPropertyInfo()
        {
            for (int i = 0; i < (int)EMobaDetailedPropertyInfoID.EMDPI_Max; i++)
            {
                if (i >= propertyName.Length || propertyName[i] == null)
                {
                    continue;
                }

                propertyName[i].text = ULocalizationService.Instance.Get("UIView", "WarHeroDetailedInfoPanel", "Title_" + i.ToString());
                string sDesc = ULocalizationService.Instance.Get("ToolTips", "WarHeroDetailedInfoPanel", "PropertyName_" + i);
                PropertyItem propertyItem = propertyName[i].gameObject.AddComponent<PropertyItem>();
                propertyItem.SetToolTipDesc(sDesc);
                propertyItem.triger = propertyItem.gameObject.GetComponent<UTooltipTrigger>();
                propertyName[i].transform.FindChild("PValue").gameObject.GetComponent<Text>().text = "0";
                //初始化主英雄属性表
                DicMainHeroProperty.Add(i, 0);
            }
        }

        public override void Destroy()
        {
            EntityFactory.RemoveEntityNumPropHandler(OnCommand_UpdateNumProp);

            if (DicMainHeroProperty != null)
            {
                DicMainHeroProperty.Clear();
                DicMainHeroProperty = null;
            }

            base.Destroy();
        }



        public void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
        {
            if (!e.objev.IsHero || e.objev == null)
                return;

            bool bIsViewItemChange = true;
            int nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_Max;//在配置表里的属性序号
            int strPropertyValue = e.data.nValue;
            switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_PA://攻击力
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_PA;
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_PCT_AF://百分比普攻系数
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_PCT_AF;
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_PDP://护甲穿透
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_PDP;
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_APPEND_PCT_PDP://百分比物理防御穿透
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_Max;
                        m_nPCTPDP = e.data.nValue / 100;
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_PCT_PDRAINS://百分比物理吸血
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_PCTPDrains;
                        strPropertyValue = e.data.nValue / 100;
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_MA://法术强度
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_MA;
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_CRC://暴击几率
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_CRC;
                        strPropertyValue = e.data.nValue / 100;
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_MDP://法术防御穿透
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_MDP;
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_APPEND_PCT_MDP://百分比法术防御穿透
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_Max;
                        m_nPCTMDP = e.data.nValue / 100;
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_PCT_MDRAINS://法术吸血
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_PCTMDrains;
                        strPropertyValue = e.data.nValue / 100;
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_MAX_HP://总生命值
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_Hp;
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_PD://物理防御
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_PD;
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_VOCATION://职业
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_AttackRange;
                        int nHeroId = e.data.nValue;
                        strPropertyValue = GameLogicAPI.getPersonPropAttackRange(nHeroId);
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_HP_REPLY://生命回复
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_HpReply;
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_MAX_MP://总法力值
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_Mp;
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_MD://魔抗
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_MD;
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE://冷却缩减
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_PCTCoolReduce;
                        strPropertyValue = e.data.nValue / 100;
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_MP_REPLY://法力回复
                    {
                        nIndex = (int)EMobaDetailedPropertyInfoID.EMDPI_MpReply;
                    }
                    break;
                default:
                    {
                        bIsViewItemChange = false;
                    }
                    break;
            }

            if (bIsViewItemChange)
            {
                SetMainHeroPropertyValue(nIndex, strPropertyValue);
                if (IsVisible())
                {
                    SetHeroProperty();
                    SaveHeroPrperty();
                }
            }
        }

        private void SetMainHeroPropertyValue(int _nIndex, int _strPropertyValue)
        {
            if (!DicMainHeroProperty.ContainsKey(_nIndex))
                return;

            DicMainHeroProperty[_nIndex] = _strPropertyValue;
        }

        public void SetHeroProperty()
        {
            SetPropertyValueToPropertyLabel();
            SetPropertyChange();
        }

        public void SaveHeroPrperty()
        {
            foreach (KeyValuePair<int, int> dicPair in DicMainHeroProperty)
            {
                m_dicLastMainHeroProperty[dicPair.Key] = dicPair.Value; 
            }
            m_nOldPCTPDP = m_nPCTPDP;
            m_nOldPCTMDP = m_nPCTMDP;
        }

        private void SetPropertyValueToPropertyLabel()
        {
            foreach (KeyValuePair<int, int> dicPair in DicMainHeroProperty)
            {
                string strPropertyValue = "";
                switch (dicPair.Key)
                {
                    case (int)EMobaDetailedPropertyInfoID.EMDPI_PA://攻击力
                    case (int)EMobaDetailedPropertyInfoID.EMDPI_MA://法术强度
                    case (int)EMobaDetailedPropertyInfoID.EMDPI_PD://物理防御
                    case (int)EMobaDetailedPropertyInfoID.EMDPI_MD://魔抗
                    case (int)EMobaDetailedPropertyInfoID.EMDPI_Hp://总生命值
                    case (int)EMobaDetailedPropertyInfoID.EMDPI_Mp://总法力值
                        {
                            strPropertyValue = dicPair.Value.ToString();
                        }
                        break;
                    case (int)EMobaDetailedPropertyInfoID.EMDPI_PCT_AF://百分比普攻系数
                        {
                            //strPropertyValue = ((dicPair.Value) / 100.00).ToString("f2");
                            strPropertyValue = String.Format("{0}%", (int)(dicPair.Value / 100));
                        }
                        break;
                    case (int)EMobaDetailedPropertyInfoID.EMDPI_PDP://护甲穿透
                        {
                            strPropertyValue = String.Format("{0} | {1}%", dicPair.Value, m_nPCTPDP);
                        }
                        break;
                    case (int)EMobaDetailedPropertyInfoID.EMDPI_PCTPDrains://百分比物理吸血
                    case (int)EMobaDetailedPropertyInfoID.EMDPI_CRC://暴击几率
                    case (int)EMobaDetailedPropertyInfoID.EMDPI_PCTMDrains://法术吸血
                    case (int)EMobaDetailedPropertyInfoID.EMDPI_PCTCoolReduce://冷却缩减
                        {
                            strPropertyValue = String.Format("{0}%", dicPair.Value);
                        }
                        break;
                    case (int)EMobaDetailedPropertyInfoID.EMDPI_MDP://法术防御穿透
                        {
                            strPropertyValue = String.Format("{0} | {1}%", dicPair.Value, m_nPCTMDP);
                        }
                        break;
                    case (int)EMobaDetailedPropertyInfoID.EMDPI_AttackRange://职业
                        {
                            strPropertyValue = String.Format("{0}米", dicPair.Value);
                        }
                        break;
                    case (int)EMobaDetailedPropertyInfoID.EMDPI_HpReply://生命回复
                    case (int)EMobaDetailedPropertyInfoID.EMDPI_MpReply://法力回复
                        {
                            strPropertyValue = String.Format("{0}/5秒", dicPair.Value);
                        }
                        break;
                    default:
                        break;
                }

                propertyName[dicPair.Key].transform.FindChild("PValue").gameObject.GetComponent<Text>().text = strPropertyValue;
            }
        }

        private void SetPropertyChange()
        {
            foreach (KeyValuePair<int, int> dicPair in DicMainHeroProperty)
            {
                Image changeIcon = propertyName[dicPair.Key].transform.FindChild("ChangeIcon").gameObject.GetComponent<Image>();
                Text changeValue = propertyName[dicPair.Key].transform.FindChild("ChangeValue").gameObject.GetComponent<Text>();
                int nOldValue = 0;
                if (m_dicLastMainHeroProperty.TryGetValue(dicPair.Key, out nOldValue))
                {
                    if (dicPair.Value > nOldValue)
                    {
                        changeIcon.sprite = changeIcons[(int)PropertyChangeFlag.PCF_INCREASE];
                        if (dicPair.Key == (int)EMobaDetailedPropertyInfoID.EMDPI_PCT_AF)
                            changeValue.text = ((dicPair.Value - nOldValue) / 10000.00).ToString("f2");
                        else
                            changeValue.text = (dicPair.Value - nOldValue).ToString();
                        changeValue.color = UDefines.CommomColor(ECommonColor.ECC_Green1);
                        changeIcon.gameObject.SetActive(true);
                        changeValue.gameObject.SetActive(true);
                    }
                    else if (dicPair.Value < nOldValue)
                    {
                        changeIcon.sprite = changeIcons[(int)PropertyChangeFlag.PCF_DECREASE];
                        if (dicPair.Key == (int)EMobaDetailedPropertyInfoID.EMDPI_PCT_AF)
                            changeValue.text = ((nOldValue - dicPair.Value) / 10000.00).ToString("f2");
                        else
                            changeValue.text = (nOldValue - dicPair.Value).ToString();
                        changeValue.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
                        changeIcon.gameObject.SetActive(true);
                        changeValue.gameObject.SetActive(true);
                    }
                    else
                    {
                        changeIcon.gameObject.SetActive(false);
                        changeValue.gameObject.SetActive(false);
                        if (m_nPCTPDP != m_nOldPCTPDP)
                            SetPDPPropertyChange();
                        if (m_nPCTMDP != m_nOldPCTMDP)
                            SetMDPPropertyChange();
                    }
                }
                else
                {
                    changeIcon.gameObject.SetActive(false);
                    changeValue.gameObject.SetActive(false);
                }
            }
        }

        // 数值未改变时，比较百分比
        // 物理穿透
        private void SetPDPPropertyChange()
        {
            Image changeIcon = propertyName[(int)EMobaDetailedPropertyInfoID.EMDPI_PDP].transform.FindChild("ChangeIcon").gameObject.GetComponent<Image>();
            Text changeValue = propertyName[(int)EMobaDetailedPropertyInfoID.EMDPI_PDP].transform.FindChild("ChangeValue").gameObject.GetComponent<Text>();
            changeIcon.gameObject.SetActive(true);
            changeValue.gameObject.SetActive(true);
            changeValue.text = (m_nPCTPDP - m_nOldPCTPDP).ToString();
            if (m_nPCTPDP > m_nOldPCTPDP)
            {
                changeIcon.sprite = changeIcons[(int)PropertyChangeFlag.PCF_INCREASE];
                changeValue.color = UDefines.CommomColor(ECommonColor.ECC_Green1);
            }
            else if (m_nPCTPDP < m_nOldPCTPDP)
            {
                changeIcon.sprite = changeIcons[(int)PropertyChangeFlag.PCF_DECREASE];
                changeValue.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
            }
        }
        // 法术穿透
        private void SetMDPPropertyChange()
        {
            Image changeIcon = propertyName[(int)EMobaDetailedPropertyInfoID.EMDPI_MDP].transform.FindChild("ChangeIcon").gameObject.GetComponent<Image>();
            Text changeValue = propertyName[(int)EMobaDetailedPropertyInfoID.EMDPI_MDP].transform.FindChild("ChangeValue").gameObject.GetComponent<Text>();
            changeIcon.gameObject.SetActive(true);
            changeValue.gameObject.SetActive(true);
            changeValue.text = (m_nPCTMDP - m_nOldPCTMDP).ToString();
            if (m_nPCTMDP > m_nOldPCTMDP)
            {
                changeIcon.sprite = changeIcons[(int)PropertyChangeFlag.PCF_INCREASE];
                changeValue.color = UDefines.CommomColor(ECommonColor.ECC_Green1);
            }
            else if (m_nPCTMDP < m_nOldPCTMDP)
            {
                changeIcon.sprite = changeIcons[(int)PropertyChangeFlag.PCF_DECREASE];
                changeValue.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
            }
        }

        public void onCloseButton()
        {
            SetVisible(false);
        }
    }
}
