/*******************************************************************
** 文件名:	UMediatorWarBottonCenter.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-6-3
** 版  本:	1.0
** 描  述:	战场底部技能物品主界面
		
********************************************************************/
using System.Collections.Generic;
using GameLogic;
using System;
using System.Runtime.InteropServices;
using DataCenter;
using USpeedUI.TooltipAndDialog;
using UnityEngine;
using System.Collections;
using U3D_Render;
using ASpeedGame.Data.GameMobaSchemes;
using ASpeedGame.Data.PropertyPanelConfig;
namespace USpeedUI.WarBottonCenterPart
{
    public class WarBottonCenterWnd : UIBaseWnd<WarBottonCenterWndView>
    {

        /*特殊能力值*/
        private int m_currentStamina, m_maxStamina = 0;
        /*血量*/
        private int m_currentHp, m_maxHp = 0;
        /*魔法值*/
        private int m_currentMp, m_maxMp = 0;

        /*要显示的属性值*/
        private Dictionary<ENTITY_PROPERTY, float> m_dicPropertyValue = new Dictionary<ENTITY_PROPERTY, float>();
        private Dictionary<ENTITY_PROPERTY, int> m_dicPropertyIndex = new Dictionary<ENTITY_PROPERTY, int>();
        
        /// <summary>
        /// 英雄技能控制脚本
        /// </summary>
        private SkillCtrl MyHeroSkillCtrl = null;
        /// <summary>
        /// 英雄对象
        /// </summary>
        private GameObject MyHeroGO = null;

        /// <summary>
        /// 存一下当前可用的升级点，用于检测升级快捷键
        /// </summary>


        private const int nMainSkillSlotIdBegin = 0;

        private const int nMainSkillSlotIdEnd = 3;

        private const int nSummerSkillSlotIdBegin = 13;

        private const int nSummerSkillSlotIdEnd = 14;

        private const int nHeroTalentSkillSlotId1 = 16;
        private const int nHeroTalentSkillSlotId2 = 18;
        private const int nHeroTalentSkillSlotId3 = 19;

        private bool bMobaState = false;
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_BOTTOMCENTERVIEW;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "DynamicState_Moba/WarBottomCenterView/WarBottomCenter";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED ;
        }

        public override int GetPermissionMask()
        {
            return (int)WndPermissionMaskDef.WND_PM_Normal;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();
        
            // 订阅基础消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            //订阅战场结束事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            //退订战场结束事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
           
        }

        public void AddlistenerMessage()
        {
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLSETSLOT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLUPDATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLEFFECT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLOVEREFFECT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_BUFFINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SETSHORTCUT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLMPEFFECT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_UPDATEMAINPLAYERGOODS, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_SELF, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_LEARNHEROTALENT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_MAINHEROTALENTCLOSE, this);

            EntityFactory.SetEntityNumPropHandler(ResponseEntityNumPropEvent);
        }

        public void RemovelistenerMessage()
        {
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLSETSLOT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLUPDATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLEFFECT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLOVEREFFECT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_BUFFINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SETSHORTCUT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLMPEFFECT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_UPDATEMAINPLAYERGOODS, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_SELF, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_LEARNHEROTALENT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_MAINHEROTALENTCLOSE, this);

            EntityFactory.RemoveEntityNumPropHandler(ResponseEntityNumPropEvent);
        }
        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        this.SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_WAREND:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        if(m_wndView == null)
                        {
                            return;
                        }

                        m_wndView.SendChatVisibleMessage(false);

                        this.UnloadView();
                        ResetData();
                    }   
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        bMobaState = true;
                        StartData();
                        this.SetVisible(true);                    
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLSETSLOT:
                    {
                        if (m_wndView == null || !bMobaState)
                        {
                            return;
                        }

                        UIMsgCmdData uiSkillSlot = (UIMsgCmdData)msgData;
                        cmd_creature_set_slot data = IntPtrHelper.toData<cmd_creature_set_slot>(uiSkillSlot.ptrParam);
                        SetSpellSlot(data);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLUPDATE:
                    {
                        if (m_wndView == null||!bMobaState)
                        {
                            return;
                        }

                        UIMsgCmdData uiSkillUpdate = (UIMsgCmdData)msgData;
                        cmd_creature_spell_point data = IntPtrHelper.toData<cmd_creature_spell_point>(uiSkillUpdate.ptrParam);
                        m_wndView.SetSpellSlotUpgrade(data);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLEFFECT:
                    {
                        if (m_wndView == null || !bMobaState)
                        {
                            return;
                        }

                        UIMsgCmdData uiSpellEffect = (UIMsgCmdData)msgData;
                        cmd_creature_set_slot data = IntPtrHelper.toData<cmd_creature_set_slot>(uiSpellEffect.ptrParam);

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
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLOVEREFFECT:
                    {
                        if (m_wndView == null || !bMobaState)
                        {
                            return;
                        }

                        UIMsgCmdData uiSpellOverEffect = (UIMsgCmdData)msgData;
                        cmd_set_spell_overlay data = IntPtrHelper.toData<cmd_set_spell_overlay>(uiSpellOverEffect.ptrParam);

                        SetSpellSlotFlagEffect(data);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_BUFFINFO:
                    {
                        if (m_wndView == null || !bMobaState)
                        {
                            return;
                        }
                        
                        UIMsgCmdData uiBuffData = (UIMsgCmdData)msgData;
                        cmd_creature_setBuff data = IntPtrHelper.toData<cmd_creature_setBuff>(uiBuffData.ptrParam);

                        SetHeroBuff(data);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_SETSHORTCUT:
                    {
                        if (m_wndView == null || !bMobaState)
                        {
                            return;
                        }

                        UIMsgCmdData uiShortCutData = (UIMsgCmdData)msgData;

                        SetSlotShortcutString(uiShortCutData.nParam, uiShortCutData.nPtrLen, uiShortCutData.strParam);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLMPEFFECT:
                    {
                        if (m_wndView == null || !bMobaState)
                        {
                            return;
                        }

                        UIMsgCmdData uiMpEffectData = (UIMsgCmdData)msgData;
                        cmd_creature_mp_effect_spell data = IntPtrHelper.toData<cmd_creature_mp_effect_spell>(uiMpEffectData.ptrParam);
                        SetMpEffectSpell(data);
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_GOODSSTORE_UPDATEMAINPLAYERGOODS:
                    {
                        if (m_wndView == null || !bMobaState)
                        {
                            return;
                        }

                        SetPersonBuyGoodsInfo();
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_SELF:
                    {
                        if (m_wndView == null )
                        {
                            return;
                        }

                        UIMsgCmdData uiData = (UIMsgCmdData)msgData;
                        cmd_creature_ShowPromptUI data = IntPtrHelper.toData<cmd_creature_ShowPromptUI>(uiData.ptrParam);
                        SetXpSkillFireEffect(data);
                    }
                    break;
                case WndMsgID.WND_ID_HEROTALENTVIEW_LEARNHEROTALENT:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        UIMsgCmdData uiData = (UIMsgCmdData)msgData;
                        m_wndView.SetTalentLearnActive(uiData.nParam);
                    }
                    break;
                case WndMsgID.WND_ID_HEROTALENTVIEW_MAINHEROTALENTCLOSE:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        m_wndView.CloseWarTalentView();
                    }
                    break;

                case WndMsgID.WND_MSG_WARBUTTOMCENTER_WARTALENTBUTTONCLICK:
                    {
                        if(m_wndView != null)
                        {
                            m_wndView.OnWarTalentButtonClick();
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        private void StartData()
        {
             InitPropertyIndex();
             InitProperyDic();
             AddlistenerMessage();
        }

        private void ResetData()
        {
            m_currentHp = 0;
            m_currentMp = 0;
            m_currentStamina = 0;
            m_maxHp = 0;
            m_maxMp = 0;
            m_maxStamina = 0;

            bMobaState = false;
            DestroyProperyDic();
            RemovelistenerMessage();
        }

        private void SetXpSkillFireEffect(cmd_creature_ShowPromptUI data)
        {
            if(data.nType>0)
            {
                m_wndView.SetXpSkillFireEffect();
            }
            else
            {
                m_wndView.DestoryXpSkillFireEffect();
            }
        }

        //数值属性回调函数
        public void ResponseEntityNumPropEvent(EntityNumPropEventArgs e)
        {
            if (e.objev == null || m_wndView == null || !e.objev.IsHero || !bMobaState)
            {
                return;
            }

            entity_view_update_Numproperty data = e.data;
            int nUpdateValue = data.nValue;
            switch (data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_VOCATION:
                    {
                        SetHeroVocationProtrait(nUpdateValue);
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_EXP:
                    {
                        SetHeroExp(nUpdateValue, data.nOtherData);
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_LEVEL:
                    {
                        SetHeroLevel(nUpdateValue);
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_STAMINA:
                    {
                        m_currentStamina = nUpdateValue;
                        SetHeroStamina();
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_MAX_STAMINA:
                    {
                        m_maxStamina = nUpdateValue;
                        SetHeroStamina();
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_MP:
                case (int)ENTITY_PROPERTY.PROPERTY_ENERGY:
                    {
                        m_currentMp = nUpdateValue;
                        SetHeroMP();
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_MAX_MP:
                case (int)ENTITY_PROPERTY.PROPERTY_MAX_ENERGY:
                    {
                        m_maxMp = nUpdateValue;
                        SetHeroMP();
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_TALENT_POINT://金钱
                    {
                        SetGoodsStoreMoney(nUpdateValue);
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_HP:
                    {
                        m_currentHp = nUpdateValue;
                        SetHeroHP();
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_MAX_HP:
                    {
                        m_maxHp = nUpdateValue;
                        SetHeroHP();
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_SHIELD:
                    {
                        SetHeroHP();
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

                        ENTITY_PROPERTY index = (ENTITY_PROPERTY)e.data.nPropID;
                        float fValue = e.data.nValue;
                        if (!Mathf.Approximately(m_dicPropertyValue[index], fValue))
                        {
                            m_dicPropertyValue[index] = fValue;

                            if (e.data.nPropID == (int)ENTITY_PROPERTY.PROPERTY_CRC 
                                || e.data.nPropID == (int)ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE
                                || e.data.nPropID == (int)ENTITY_PROPERTY.PROPERTY_PCT_AF)
                            {
                                fValue /= 100;
                            }

                            // 取得配置区间值
                            if (EntityFactory.MainHeroView == null)
                                break;

                            int nHeroID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);

                            int nPropIndex = m_dicPropertyIndex[(ENTITY_PROPERTY)data.nPropID];

                            SSchemePropertyPanel propConfig = PropertyPanelConfig.Instance.GetPropertyPanelConfigList(nHeroID, fValue, nPropIndex);
                            if(propConfig == null)
                            {
                                Debug.LogError(String.Format("propConfig == null,{0},{1}, {2}", nHeroID, fValue, nPropIndex));
                                break;
                            }

                            string strValue = "";
                            if(index == ENTITY_PROPERTY.PROPERTY_PCT_AF)
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

                            m_wndView.SetPropertyValue(nPropIndex - 1, strNewText);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        private void InitProperyDic()
        {
            DestroyProperyDic();
            m_dicPropertyValue.Add(ENTITY_PROPERTY.PROPERTY_PA, 0); m_dicPropertyValue.Add(ENTITY_PROPERTY.PROPERTY_MA, 0);
            m_dicPropertyValue.Add(ENTITY_PROPERTY.PROPERTY_PD, 0); m_dicPropertyValue.Add(ENTITY_PROPERTY.PROPERTY_MD, 0);
            m_dicPropertyValue.Add(ENTITY_PROPERTY.PROPERTY_PCT_AF, 0); m_dicPropertyValue.Add(ENTITY_PROPERTY.PROPERTY_PCT_COOL_REDUCE, 0);
            m_dicPropertyValue.Add(ENTITY_PROPERTY.PROPERTY_CRC, 0); m_dicPropertyValue.Add(ENTITY_PROPERTY.PROPERTY_SD, 0);
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

        private void SetHeroVocationProtrait(int nVocationId)
        {
            int nIconID = nVocationId;
            m_wndView.SetHeroVocationProtrait(nIconID);
        }

        private void SetHeroExp(int nCurValue, int nMaxValue)
        {
            m_wndView.SetHeroExp(nCurValue , nMaxValue);
        }

        private void SetHeroLevel(int nLevel)
        {
            m_wndView.SetHeroLevel(nLevel);
        }

        private void SetHeroStamina()
        {
            m_wndView.SetHeroStamina(m_currentStamina , m_maxStamina);
        }

        private void SetHeroHP()
        {
            m_wndView.SetHeroHP(m_currentHp , m_maxHp);
        }

        private void SetHeroMP()
        {
            m_wndView.SetHeroMP(m_currentMp ,m_maxMp);
        }

        private void SetGoodsStoreMoney(int nMoney)
        {
            m_wndView.SetGoodsStoreMoney(nMoney);
        }

        //private void SetAllPropertyValue(string strATK, string strMA, string strDEF, string strMDEF, string strAF, string strCOOLREDUCE, string strCRC, string strSPD)
        //{
        //    m_wndView.SetAllPropertyValue(strATK, strMA, strDEF, strMDEF, strAF, strCOOLREDUCE, strCRC, strSPD);
        //}

        public SkillCtrl GetMyHeroCtrl()
        {
            if (MyHeroSkillCtrl == null)
            {
                MyHeroGO = EntityFactory.getEntityGameObjectByID(EntityFactory.MainHeroID);
                if (MyHeroGO == null)
                {
                    Trace.LogError("get Hero GameObject failed");
                    return null;
                }
                MyHeroSkillCtrl = MyHeroGO.GetComponent<SkillCtrl>();
            }
            return MyHeroSkillCtrl;
        }

        private void ReCalSlotIndex(ref int nSlotIndex)
        {
            if (nSlotIndex == nHeroTalentSkillSlotId1) // 天赋快捷键特殊处理下
            {
                nSlotIndex = 5;
            }
            else if(nSlotIndex == nHeroTalentSkillSlotId2)
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

        private bool CheckSlotIndexRange(int _nSlotIndex)
        {
            if (((_nSlotIndex >= nMainSkillSlotIdBegin) && (_nSlotIndex <= nMainSkillSlotIdEnd)) || ((_nSlotIndex == nSummerSkillSlotIdBegin)) 
                || _nSlotIndex == nHeroTalentSkillSlotId1|| _nSlotIndex == nHeroTalentSkillSlotId2|| _nSlotIndex == nHeroTalentSkillSlotId3)// && (_data.nSlotIndex <= nSummerSkillSlotIdEnd)))
            {
                return true;
            }
            else
            {
                return false;
            }       
        }

        public void SetSpellSlot(cmd_creature_set_slot _data)
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



            GetMyHeroCtrl().SetSkillIDThroughSlot(_data.nSlotIndex, _data.slotview.nId);

            m_wndView.SetSpellSlot(_data.nSlotIndex, _nType, _data.slotview.nIconID, _data.slotview.szSlotDesc, _data.nSlotLevel, bUseSlotFlag);
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
                m_wndView.SetSkillBtnEffectClear(_data.nSlotIndex);
            }
            else
            {
                m_wndView.SetSkillBtnEffect(_data.nSlotIndex, effectType, beginTime, endTime);
            }              
        }

        public void SetSpellSlotFlagEffect(cmd_set_spell_overlay _data)
        {
            //如果这里有报错，先看看逻辑层这个nID是否是槽位ID，以前是下发技能ID的

            ReCalSlotIndex(ref _data.nSlotIndex);

            if (_data.nType != 0)
            {
                m_wndView.SetSkillFlagEffect(_data.nSlotIndex, _data.nFlag, _data.nFreezeLeftTime, _data.nLeftTime, _data.nTotalTime);
            }
            else
            {
                m_wndView.SetSkillFlagEffectOver(_data.nSlotIndex, _data.nFlag);
            }
        }

        public void SetHeroBuff(cmd_creature_setBuff _data)
        {
            if (_data.nType == 1)
            {
                bool bShow = false;
                if (_data.nIsShow != 0)
                {
                    bShow = true;
                }
               m_wndView.AddBuff((int)_data.dwIndex, _data.nIconID, (int)_data.dwTime, _data.szName, _data.szTips, _data.nLevel, bShow);
            }
            else if (_data.nType == 2)
            {
                m_wndView.RemoveBuff((int)_data.dwIndex);
            }
        }

        public void SetPersonBuyGoodsInfo()
        {
            List< cmd_war_persongoodsinfo> listGoods = LogicDataCenter.warMobaGoodsDataManager.GetMainPlayerGoodsInfo();

            foreach(var data in listGoods)
            {                
                if (data.nGoodID != 0)
                {
                    SSchemeMobaGoods mobaGood;
                    LogicDataCenter.warMobaGoodsDataManager.GetMobaGoods_Out(data.nGoodID, out mobaGood);
                    
                    m_wndView.AddGoodsButtonToSlot(data,mobaGood);
                }
                else
                {
                    m_wndView.RemoveGoodsButtonFromSlot(data.nSlotIndex);
                }
            }

        }

        public void SetPurchasedGoodsUseEffect(cmd_creature_set_slot data)
        {
            if (data.nSlotIndex < 0)
            {
                return;
            }
               
            int endTime = data.slotview.nMaxData;
            int beginTime = data.slotview.nMaxData - data.slotview.nCurData < 0 ? 0 : data.slotview.nMaxData - data.slotview.nCurData;


            m_wndView.SetPurchasedGoodsUseEffect(data.nSlotIndex, beginTime, endTime);
        }

        public void SetPurchasedGoodsCanDragDrop(cmd_creature_set_slot data)
        {
            if (data.nSlotIndex < 0)
            {
                return;
            }

            bool bCanDrag = data.slotview.bCanDrag<=0?false:true;
            m_wndView.SetPurchasedGoodsCanDragDrop(data.nSlotIndex, bCanDrag);
        }

        //提供一个客户端自己更改槽位键值的方法
        public void SetSlotShortcutString(int nType, int nSlotId, string keyCode)
        {
            if (nType == (int)SLOT_TYPE.SLOT_TYPE_GOODS)
            {
               m_wndView.SetGoodsSlotShortcutKey(nSlotId, keyCode);
            }
            else if (nType == (int)SLOT_TYPE.SLOT_TYPE_SKILL)
            {
                if (CheckSlotIndexRange(nSlotId))//(((nSlotId >= 0) && (nSlotId <= 4)) || ((nSlotId >= 13) && (nSlotId <= 14)))
                {

                    ReCalSlotIndex(ref nSlotId);
                    m_wndView.SetSkillShortcutKey(nSlotId, keyCode);

                    //if (FuntionShortCutCtrl.GetInstance() != null)
                    //{
                    //    FuntionShortCutCtrl.GetInstance().ChangeKeyCodeToDic(nSlotId +(int) FuntionShortCutSlotIndex.Com_UpgradeSpellSlot0, keyCode, (int)CombinationShortcutKeyIndex.Combination_FirstCutKey);
                    //}
                }
            }
        }

        public void SetMpEffectSpell(cmd_creature_mp_effect_spell data)
        {
            if (data.nSlotType == SLOT_TYPE.SLOT_TYPE_SKILL)
            {
                bool bUse = data.nShowType<=0? true:false;
                m_wndView.SetMpEffectSpell(data.nSlotIndex, bUse);
            }
        }

        public void DynamicsSetTalentChild(GameObject gameObject)
        {
            if (m_wndView)
                m_wndView.DynamicsSetTalentChild(gameObject);
        }
    }
}
