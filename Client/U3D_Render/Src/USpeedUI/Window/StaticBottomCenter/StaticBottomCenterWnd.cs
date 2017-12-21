/*******************************************************************
** 文件名:	StaticBottomCenterWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-10-20
** 版  本:	1.0
** 描  述:	主城底部ui
		
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
using UIWidgets;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using ASpeedGame.Data.HeroTalentConfig;
using U3D_Render.Common;
using USpeedUI.UEffect;
using USpeedUI.UExtensions;
namespace USpeedUI.StaticBottomCenterWnd
{
    public class StaticBottomCenterWnd : UIBaseWnd<StaticBottomCenterView>
    {   
        private SkillCtrl MyHeroSkillCtrl = null;

        private const int nMainSkillSlotIdBegin = 0;

        private const int nMainSkillSlotIdEnd = 3;

        private const int nSummerSkillSlotIdBegin = 13;

        private const int nSummerSkillSlotIdEnd = 14;

        private const int nHeroTalentSkillSlotId = 16;
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_STATICBOTTOMCENTER;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "StaticBottomCenter/StaticBottomCenter";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED; ;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            
            this.LoadView();
            AddlistenerMessage();
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();
            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
     
        }

        public void AddlistenerMessage()
        {
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLSETSLOT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLEFFECT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLOVEREFFECT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SETSHORTCUT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLMPEFFECT, this);
        }

        public void RemovelistenerMessage()
        {
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLSETSLOT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLEFFECT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLOVEREFFECT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SETSHORTCUT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLMPEFFECT, this);
           
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        this.SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        this.SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        if(m_wndView == null)
                        {
                            this.LoadView();
                            AddlistenerMessage();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        this.UnloadView();
                        RemovelistenerMessage();
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLSETSLOT:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        UIMsgCmdData uiSkillSlot = (UIMsgCmdData)msgData;
                        cmd_creature_set_slot data = IntPtrHelper.toData<cmd_creature_set_slot>(uiSkillSlot.ptrParam);
                        SetSpellSlot(data);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLEFFECT:
                    {
                        if (m_wndView == null)
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
                          //  SetPurchasedGoodsUseEffect(data);
                          //  SetPurchasedGoodsCanDragDrop(data);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLOVEREFFECT:
                    {
                        if (m_wndView == null )
                        {
                            return;
                        }

                        UIMsgCmdData uiSpellOverEffect = (UIMsgCmdData)msgData;
                        cmd_set_spell_overlay data = IntPtrHelper.toData<cmd_set_spell_overlay>(uiSpellOverEffect.ptrParam);

                        SetSpellSlotFlagEffect(data);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_SETSHORTCUT:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        UIMsgCmdData uiShortCutData = (UIMsgCmdData)msgData;

                        SetSlotShortcutString(uiShortCutData.nParam, uiShortCutData.nPtrLen, uiShortCutData.strParam);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLMPEFFECT:
                    {
                        if (m_wndView == null )
                        {
                            return;
                        }

                        UIMsgCmdData uiMpEffectData = (UIMsgCmdData)msgData;
                        cmd_creature_mp_effect_spell data = IntPtrHelper.toData<cmd_creature_mp_effect_spell>(uiMpEffectData.ptrParam);
                        SetMpEffectSpell(data);
                    }
                    break;
                default:
                    break;
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


         private void ReCalSlotIndex(ref int nSlotIndex)
         {
             if (nSlotIndex == nHeroTalentSkillSlotId) // 天赋快捷键特殊处理下
             {
                 nSlotIndex = 5;
             }
             else if (nSlotIndex > 3)
             {
                 nSlotIndex -= 9;
             }
         }

         public SkillCtrl GetMyHeroCtrl()
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

         private bool CheckSlotIndexRange(int _nSlotIndex)
         {
             if (((_nSlotIndex >= nMainSkillSlotIdBegin) && (_nSlotIndex <= nMainSkillSlotIdEnd)) || ((_nSlotIndex == nSummerSkillSlotIdBegin)) || _nSlotIndex == nHeroTalentSkillSlotId)// && (_data.nSlotIndex <= nSummerSkillSlotIdEnd)))
             {
                 return true;
             }
             else
             {
                 return false;
             }
         }

         public void SetMpEffectSpell(cmd_creature_mp_effect_spell data)
         {
             if (data.nSlotType == SLOT_TYPE.SLOT_TYPE_SKILL)
             {
                 bool bUse = data.nShowType <= 0 ? true : false;
                 m_wndView.SetMpEffectSpell(data.nSlotIndex, bUse);
             }
         }

         //提供一个客户端自己更改槽位键值的方法
         public void SetSlotShortcutString(int nType, int nSlotId, string keyCode)
         {
             if (nType == (int)SLOT_TYPE.SLOT_TYPE_GOODS)
             {
                // m_wndView.SetGoodsSlotShortcutKey(nSlotId, keyCode);
             }
             else if (nType == (int)SLOT_TYPE.SLOT_TYPE_SKILL)
             {
                 if (CheckSlotIndexRange(nSlotId))//(((nSlotId >= 0) && (nSlotId <= 4)) || ((nSlotId >= 13) && (nSlotId <= 14)))
                 {

                     ReCalSlotIndex(ref nSlotId);
                     m_wndView.SetSkillShortcutKey(nSlotId, keyCode);
                 }
             }
         }
    }

    public class StaticBottomCenterView : UIBaseWndView
    {
        private StaticBottomCenterWnd m_PlayerVsWnd;
        public StaticCenterSkillPart staticCenterSkillPart;
        public override void Start()
        {
            base.Start();
            return;
        }

        public override void Update()
        {
            base.Update();
            if(!this.IsVisible())
            {
                return;
            }

            bool bOtherWndVisible = UISystem.Instance.IsWndVisible(WndID.WND_ID_CHART) || UISystem.Instance.IsWndVisible(WndID.WND_ID_PLAYERFRAME)
                || UISystem.Instance.IsWndVisible(WndID.WND_ID_SOCIALCONTACT) || UISystem.Instance.IsWndVisible(WndID.WND_ID_HALLROOMINFOVIEW)
                || UISystem.Instance.IsWndVisible(WndID.WND_ID_WAR_PROGRESSBAR);
            staticCenterSkillPart.gameObject.SetActive(!bOtherWndVisible);
        }

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            m_PlayerVsWnd = wnd as StaticBottomCenterWnd;
            return true;
        }

        public void SetSpellSlot(int nSlotID, int nSlotType, int nSlotIconID, string strSlotTipsDesc, int nSlotLevel, bool bSpellUseFlag)
        {
            staticCenterSkillPart.SetSpellSlot(nSlotID, nSlotType, nSlotIconID, strSlotTipsDesc, nSlotLevel, bSpellUseFlag);
        }

        public void SetSkillBtnEffectClear(int nSlotID)
        {
            staticCenterSkillPart.SetSkillBtnEffectClear(nSlotID);
        }

        public void SetSkillBtnEffect(int nSlotID, int effectType, int beginTime, int endTime)
        {
            staticCenterSkillPart.SetSkillBtnEffect(nSlotID, effectType, beginTime, endTime);
        }

        public void SetSkillFlagEffect(int nSlotID, int nFlag, int nFreezeLeftTime, int nLeftTime, int nTotalTime)
        {
            staticCenterSkillPart.SetSkillFlagEffect(nSlotID, nFlag, nFreezeLeftTime, nLeftTime, nTotalTime);
        }

        public void SetSkillShortcutKey(int nSlotId, string keyCode)
        {
            staticCenterSkillPart.SetSkillShortcutKey(nSlotId, keyCode);
        }

        public void SetSkillFlagEffectOver(int nSlotID, int nFlag)
        {
            staticCenterSkillPart.SetSkillFlagEffectOver(nSlotID, nFlag);
        }

        public void SetMpEffectSpell(int nSlotID, bool bUse)
        {
            staticCenterSkillPart.SetSlotSkillUse(nSlotID, bUse);
        }
    }

    #region 技能part
    public class StaticCenterSkillPart : MonoBehaviour
    {
        public StaticCenterSkillButtonItem[] skillItemArray;

        public void SetSpellSlot(int nSlotID, int nSlotType, int nSlotIconID, string strSlotTipsDesc, int nSlotLevel, bool bSpellUseFlag)
        {
            if (IsOutOfSkillArray(nSlotID))
            {
                return;
            }

            if (nSlotID == 4 || nSlotID == 3 || nSlotID == 5 || nSlotID == 6 || nSlotID == 7)
            {
                nSlotLevel = 1;
            }

            this.skillItemArray[nSlotID].SetSpellSlot(nSlotID, nSlotType, nSlotIconID, strSlotTipsDesc, nSlotLevel, bSpellUseFlag);
        }

        public void SetSkillBtnEffectClear(int nSlotID)
        {
            if (IsOutOfSkillArray(nSlotID))
            {
                return;
            }

            this.skillItemArray[nSlotID].SetSkillBtnEffectClear();
        }

        public void SetSkillBtnEffect(int nSlotID, int nEffectType, int beginTime, int endTime)
        {
            if (IsOutOfSkillArray(nSlotID))
            {
                return;
            }

            this.skillItemArray[nSlotID].SetSkillBtnEffect(nEffectType, beginTime, endTime);
        }

        public void SetSkillFlagEffect(int nSlotID, int nFlag, int nFreezeLeftTime, int nLeftTime, int nTotalTime)
        {
            if (IsOutOfSkillArray(nSlotID))
            {
                return;
            }

            this.skillItemArray[nSlotID].SetSkillFlagEffect(nFlag, nFreezeLeftTime, nLeftTime, nTotalTime);
        }

        public void SetSkillShortcutKey(int nSlotID, string keyCode)
        {
            if (IsOutOfSkillArray(nSlotID))
            {
                return;
            }

            this.skillItemArray[nSlotID].SetSkillShortcutKey(keyCode);
        }

        public void SetSkillFlagEffectOver(int nSlotID, int nFlag)
        {
            if (IsOutOfSkillArray(nSlotID))
            {
                return;
            }

            this.skillItemArray[nSlotID].SetSkillFlagEffectOver(nFlag);
        }

        public bool IsOutOfSkillArray(int nSlotID)
        {
            if (nSlotID < 0 || nSlotID >= skillItemArray.Length || skillItemArray[nSlotID] == null)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public void SetSlotSkillUse(int nSlotID, bool bUse)
        {
            if (IsOutOfSkillArray(nSlotID))
            {
                return;
            }

            this.skillItemArray[nSlotID].SetSlotSkillUse(bUse);
        }

        public bool GetSlotSkillUse(int nSlotID)
        {
            if (IsOutOfSkillArray(nSlotID))
            {
                return false;
            }

            return this.skillItemArray[nSlotID].GetSlotSkillUse();
        }

        public void SetTalentLearnActive(int nTalentId, int nHeroId)
        {
            SSchemeHeroTalentConfig config = HeroTalentConfigPage.Instance.GetHeroTalentConfig(nHeroId, nTalentId);
            if (config == null)
            {
                return;
            }

            int nSlotId = config.nSlotId;
            if (IsOutOfSkillArray(nSlotId))
            {
                return;
            }

            this.skillItemArray[nSlotId].SetTalentLearnActive(config.sTalentName, config.sTalenDescInWar);
        }
    }

    public class StaticCenterSkillButtonItem : TimeCountDown, IPointerEnterHandler, IPointerExitHandler
    {
        public class ActiveTalentDescItemInfo
        {
            public int nTalentIndex = 0;
            public string sTalentDesc = null;
            public string sTalentName = null;
        }

        public Image skilImage;
        public bool bCanUse;
        public Image timeCount;
        public Text KeyText;
        private int nLeftFlagTime = 0;
        private bool bEffectFlagState = false;
        private int nSkillFlag = 0;

        private int nMaxFlagTime = 0;//-充能最大时间
        private bool bEffectNomalState = false;   //-是否处于使用CD--

        private int m_nActiveTalentCount = 0;
        private const int m_MaxActiveTalentCount = 5;
        private String SkillTooltip_TalentParamName = "TalentName";
        private String SkillTooltip_SkillParamDesc = "TalentDesc";
        private String SkillTooltip_SkillParamItem = "HeroTalentItem";
        public UTooltipTrigger SkilltipTrigger;
        private string m_sOriginalDesc = null;
        private List<ActiveTalentDescItemInfo> m_listActiveTalent = new List<ActiveTalentDescItemInfo>();
        private int m_nSlotID = 0;
        private Text m_SkillFlagText;
        private Color m_SkinFlagBGColor;
        private GameObject m_TimeCountWordObj;
        private Color m_SkinBGColor;

        public virtual void Awake()
        {
            InitSkilltipTrigger();
            if (skilImage)
                skilImage.gameObject.SetActive(false);

            if (KeyText)
                KeyText.gameObject.SetActive(false);
            //{
            m_SkillFlagText = this.gameObject.transform.FindChild("UseCount").GetComponent<Text>();
            m_SkillFlagText.gameObject.SetActive(false);
            m_SkillFlagText.text = "0";
            m_SkinFlagBGColor = timeCount.gameObject.GetComponent<Image>().color;
            m_SkinBGColor = skilImage.gameObject.GetComponent<Image>().color;
            m_TimeCountWordObj = timeCount.transform.FindChild("Text").gameObject;
            //}
        }

        private void InitSkilltipTrigger()
        {
            for (int i = 1; i <= m_MaxActiveTalentCount; i++)
            {
                SkilltipTrigger.TurnSectionOff(SkillTooltip_SkillParamItem + i);
            }

            SkilltipTrigger.enabled = false;
        }

        public void SetSpellSlot(int nSlotID, int nSlotType, int nSlotIconID, string strSlotTipsDesc, int nSlotLevel, bool bSpellUseFlag)
        {
            if (skilImage == null)
            {
                return;
            }

            if (nSlotIconID <= 0)
                return;

            GameObject useCount = this.gameObject.transform.FindChild("UseCount").gameObject;
            useCount.SetActive(false);

            m_nSlotID = nSlotID;
            skilImage.gameObject.SetActive(true);
            Sprite skillSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_STATICBOTTOMCENTER, 1, nSlotIconID);
            if (skillSprite != null)
            {
                skilImage.sprite = skillSprite;
                this.gameObject.SetActive(true);
                KeyText.gameObject.SetActive(true);
            }

            bSpellUseFlag = nSlotLevel > 0 ? true : false;//目前并不知道这个flag的定义
            SetSlotSkillUse(bSpellUseFlag);
            m_sOriginalDesc = strSlotTipsDesc;
            if (nSlotLevel > 0 && nSlotLevel < 6)
            {
                var groupSkillLevel = gameObject.transform.FindChild("SkillLevel");
                if (groupSkillLevel)
                {
                    groupSkillLevel.GetComponent<SkillLevelGroup>().skillLightArray[nSlotLevel - 1].gameObject.SetActive(true);
                }
            }

            SkilltipTrigger.enabled = true;
        }

        public void SetTalentLearnActive(string sTalentName, string sTalentDesc)
        {
            m_nActiveTalentCount++;
            if (m_nActiveTalentCount > m_MaxActiveTalentCount)
            {
                return;
            }

            ActiveTalentDescItemInfo info = new ActiveTalentDescItemInfo();
            info.nTalentIndex = m_nActiveTalentCount;
            info.sTalentName = sTalentName;
            info.sTalentDesc = sTalentDesc;
            m_listActiveTalent.Add(info);
        }

        public void SetSkillBtnEffectClear()
        {
            CountDownCompleteCallBack();
        }

        public void SetSkillBtnEffect(int effectType, int nBeginTime, int nEndTime)
        {
            int nEffectTime = nEndTime - nBeginTime;
            if (/*nEffectTime == 0 ||*/ timeCount == null)
            {
                return;
            }

            if (bEffectFlagState)
            {
                SetSkillBtnFlagEffect(nEffectTime);
            }
            else
            {
                SetSkillBtnNormalEffect(nEffectTime);
            }
        }

        public void SetSkillBtnFlagEffect(int nTimeShow)
        {
            if (nSkillFlag </*=*/ 0)
            {
                return;
            }

            if (nTimeShow <= 0)
            {
                return;
            }

            int nLastFlagTime = nLeftFlagTime;
            nLeftFlagTime = 0;
            CountDownCompleteCallBack();
            bEffectNomalState = true;//-
            nLeftFlagTime = nLastFlagTime - nTimeShow;

            this.StartCountDown(((float)nTimeShow / 1000), 0.1f, 0.1f);
            this.timeCount.gameObject.SetActive(true);
            //{
            m_SkillFlagText.gameObject.SetActive(false);
            int nTmpFlag = nSkillFlag;
            nSkillFlag = 0;
            DoFlagChange();
            nSkillFlag = nTmpFlag;
            //}
        }

        public void SetSkillBtnNormalEffect(int nTimeShow)
        {
            CountDownCompleteCallBack();
            if (nTimeShow <= 0)
            {
                return;
            }

            this.StartCountDown(((float)nTimeShow / 1000), 0.1f, 0.1f);
            this.timeCount.gameObject.SetActive(true);
        }

        protected override void CountDownRepeatCallBack()
        {
            base.CountDownRepeatCallBack();
            float MaxfillAmount = bEffectNomalState ? this.fOriginalTime : (float)nMaxFlagTime / 1000;//-
            timeCount.fillAmount = (fCurTime / MaxfillAmount);//-
           // timeCount.fillAmount = (fCurTime / this.fOriginalTime);
            if (fCurTime > 10f)
            {
                timeCount.gameObject.transform.FindChild("Text").GetComponent<Text>().text = fCurTime.ToString("0");
            }
            else
            {
                timeCount.gameObject.transform.FindChild("Text").GetComponent<Text>().text = fCurTime.ToString("0.0");
            }

        }

        protected override void CountDownCompleteCallBack()
        {
            base.CountDownCompleteCallBack();

            this.timeCount.fillAmount = 1.0f;
            timeCount.gameObject.transform.FindChild("Text").GetComponent<Text>().text = "";
            this.timeCount.gameObject.SetActive(false);

            //-----
            if (bEffectNomalState)
                bEffectNomalState = false;
            if (bEffectFlagState && nLeftFlagTime > 0)
            {
                StartNextFlagLeftCountDown();
                this.nLeftFlagTime = 0;
            }
        }

        private void StartNextFlagLeftCountDown()
        {
            this.StartCountDown((float)nLeftFlagTime / 1000, 0.1f, 0.1f);
            this.timeCount.gameObject.SetActive(true);
            //-
            m_SkillFlagText.gameObject.SetActive(true && nSkillFlag > 0);
            DoFlagChange();
            //-
        }

        public void SetSkillFlagEffect(int nFlag, int nLeftTime, int nFreezeLeftTime, int nTotalTime)// 这边只能做自然冷却
        {
            bEffectFlagState = true;
            SetSkillFlagCount(nFlag);
            if (nMaxFlagTime != nTotalTime) nMaxFlagTime = nTotalTime;
            //1.在充能中"使用"技能 
            if (nLeftTime != nTotalTime)
            {
                nLeftFlagTime = nLeftTime;
                return;
            }
            // 2.恢复能量时 技能处于CD                                  
            if (bEffectNomalState)
            {
                if (nLeftFlagTime <= 0)//正常状态为负数 
                {
                    nLeftFlagTime += nTotalTime;
                }
                else  //补充能量时处于CD理论上LeftTime 为负  再此进行补偿（本地线程和服务器计时出现误差）
                {
                    nLeftFlagTime = nTotalTime;
                }
                return;
            }
            //3.恢复非最高能量且不处于CD
            //4.使用能量时处于最大能量
            int nLastFlagTime = nLeftFlagTime;
            nLeftFlagTime = 0;
            CountDownCompleteCallBack();
            nLeftFlagTime = nLastFlagTime;
            if (nLastFlagTime < 0)
            {
                nLeftTime += nLastFlagTime;
            }

            int nTimeShow = nLeftTime;
            if (nTimeShow > 0)
            {
                this.StartCountDown((float)nTimeShow / 1000, 0.1f, 0.1f);
                this.timeCount.gameObject.SetActive(true);
            }

            nLeftFlagTime = nLeftTime;
            m_SkillFlagText.gameObject.SetActive(true && nSkillFlag > 0);
            DoFlagChange();
        }

        //public void SetSkillFlagCount(int nFlag)
        //{
        //    nSkillFlag = nFlag;
        //    GameObject useCount = this.gameObject.transform.FindChild("UseCount").gameObject;
        //    useCount.SetActive(true);
        //    useCount.GetComponent<Text>().text = nFlag.ToString();

        //    if (nFlag <= 0)
        //    {
        //        useCount.SetActive(false);
        //    }
        //}
        public void SetSkillFlagCount(int nFlag)//{}
        {
            if (m_SkillFlagText == null)
            {
                Debug.LogError("m_SkillFlagText == null");
                return;
            }

            nSkillFlag = nFlag;
            m_SkillFlagText.gameObject.SetActive(true);
            m_SkillFlagText.text = nFlag.ToString();
            m_SkillFlagText.gameObject.SetActive(nFlag > 0);
        }

        public void SetSkillFlagEffectOver(int nFlag)
        {
            bEffectFlagState = false;
            nLeftFlagTime = 0;
            SetSkillFlagCount(nFlag);
        }

        public void SetSkillShortcutKey(string keyCode)
        {
            if (string.IsNullOrEmpty(keyCode))
            {
                return;
            }

            if (KeyText == null)
            {
                return;
            }

            KeyText.text = keyCode;

            UTooltipTrigger skillTip = gameObject.GetComponent<UTooltipTrigger>();
            if (skillTip == null)
            {
                skillTip = gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
                skillTip.backgroundTint = Color.grey;
            }

            string sDesc = skillTip.parameterizedTextFields[0].value;
            if (string.IsNullOrEmpty(sDesc))
            {
                return;
            }

            UBBProperty.GetInstance().getRatio(sDesc);
            UBB.toHtml(ref sDesc, UBB_FORMAT_TYPE.UGUI);
            skillTip.SetText(UTooltipParamName.BodyText, sDesc);
        }

        public void SetSlotSkillUse(bool bUse)
        {
            if (skilImage == null)
            {
                return;
            }


            if (bUse)
            {
                skilImage.ResetToDefaultMaterial();
            }
            else
            {
                skilImage.AddNewMaterial(EUIShaderType.EST_Gray);
            }

            bCanUse = bUse;
        }

        public bool GetSlotSkillUse()
        {
            return bCanUse;
        }

        public void UpdateSpellSlotDescInPropertyChange()
        {
            if (string.IsNullOrEmpty(m_sOriginalDesc))
            {
                return;
            }

            if (SkilltipTrigger == null)
            {
                return;
            }

            SkilltipTrigger.enabled = true;
            string strSlotTipsDesc = m_sOriginalDesc;
            UBBProperty.GetInstance().getRatio(m_sOriginalDesc);
            UBB.toHtml(ref strSlotTipsDesc, UBB_FORMAT_TYPE.UGUI);
            SkilltipTrigger.SetText(UTooltipParamName.BodyText, strSlotTipsDesc);

            if (m_listActiveTalent == null || m_listActiveTalent.Count <= 0)
            {
                return;
            }

            int nTalentIndex = 0;
            foreach (ActiveTalentDescItemInfo itemInfo in m_listActiveTalent)
            {
                string sTalentName = itemInfo.sTalentName;
                UBBProperty.GetInstance().getRatio(sTalentName);
                UBB.toHtml(ref sTalentName, UBB_FORMAT_TYPE.UGUI);

                string sTalentDesc = itemInfo.sTalentDesc;
                UBBProperty.GetInstance().getRatio(sTalentDesc);
                UBB.toHtml(ref sTalentDesc, UBB_FORMAT_TYPE.UGUI);

                nTalentIndex = itemInfo.nTalentIndex;
                //   SkilltipTrigger.SetText(SkillTooltip_TalentParamName + nTalentIndex, sTalentName);
                SkilltipTrigger.SetText(SkillTooltip_SkillParamDesc + nTalentIndex, "[" + sTalentName + "]" + sTalentDesc);
            }

            SkilltipTrigger.TurnSectionOn(SkillTooltip_SkillParamItem + nTalentIndex);
        }

        public void OnPointerEnter(PointerEventData eventData)
        {
            UpdateSpellSlotDescInPropertyChange();

            int nSlotType = m_nSlotID == 4 ? (int)SLOT_TYPE.SLOT_TYPE_SUMMONER : (int)SLOT_TYPE.SLOT_TYPE_SKILL;
            onAllSlotMouseEvent(m_nSlotID, nSlotType, 1);
        }

        public void OnPointerExit(PointerEventData eventData)
        {
            int nSlotType = m_nSlotID == 4 ? (int)SLOT_TYPE.SLOT_TYPE_SUMMONER : (int)SLOT_TYPE.SLOT_TYPE_SKILL;
            onAllSlotMouseEvent(m_nSlotID, nSlotType, 0);
        }

        public void onAllSlotMouseEvent(int nSlotId, int nSlotType, int nMouseEventType)
        {
            cmd_entity_mouse_enter_slot data = new cmd_entity_mouse_enter_slot();

            data.nEnterSlot = nMouseEventType;
            data.nSlotType = nSlotType;
            data.nSlotIndex = nSlotId;

            IntPtrHelper helper = new IntPtrHelper();
            IntPtr ptr = helper.toPtr<cmd_entity_mouse_enter_slot>(ref data);
            int len = Marshal.SizeOf(data);

            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_MOUSE_ENTER_SLOT, len, "", ptr, len);
        }

        public void DoFlagChange()//-
        {
            m_SkinFlagBGColor.a = nSkillFlag <= 0 ? 0.9f : 0.55f;                              //TimeCount上图片Color值
            timeCount.gameObject.GetComponent<Image>().color = m_SkinFlagBGColor;              //TimeCount上图片Color值设置
            m_TimeCountWordObj.gameObject.SetActive(nSkillFlag <= 0);                          //TimeCount子Text

            m_SkinBGColor = nSkillFlag <= 0 ? new Color(0.6f, 0.6f, 0.6f, 1f) : new Color(1f, 1f, 1f, 1f);//SkillImage..Color值
            skilImage.gameObject.GetComponent<Image>().color = m_SkinBGColor;                            //SkillImage.Color值设置
        }
    }
    #endregion
}
