/*******************************************************************
** 文件名:	UViewWarRecordTableFighting.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2016-3-31
** 版  本:	1.0
** 描  述:	XP技能显示界面
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using GameLogic;
using U3D_Render.Utility;
using ASpeedGame.Data.GameMobaSchemes;
using UnityEngine;
using DG.Tweening;
using UIWidgets;
using U3D_Render.Common;
using USpeedUI.UExtensions;
using USpeedUI;
using SPELL;
using ASpeedGame.Data.HeroXPskillConfig;
using USpeedUI.UEffect;

namespace USpeedUI.XPSkillWnd
{
    public class UXPSkill : UIBaseWnd<UViewXPSkill>
    {
        private int m_nSkillID = 0;
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_XPSKILL;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override string GetPrefabPath()
        {
            return "DynamicState_Moba/XPSkill/XpSkill";
        }

        public override int GetLoadMask()
        {
            return (int)WndLoadMaskDef.WND_LoadMask_Moba;
        }
        public override bool Init()
        {
            // 订阅基础消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_SELF, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_XPSKILL_XP_RANDOM_SELF_BEGINCOOL, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLSETSLOT, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_SELF, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_XPSKILL_XP_RANDOM_SELF_BEGINCOOL, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLSETSLOT, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_WAR_XPSKILL_SET_XP_SKILL_SELF:
                    {
                        SetVisible(true);

                        UIMsgCmdData uData = msgData as UIMsgCmdData;

                        cmd_creature_ShowPromptUI cmdData = IntPtrHelper.toData<cmd_creature_ShowPromptUI>(uData.ptrParam);

                        m_wndView.setXpSkill(cmdData);
                        m_wndView.SetXPSkillInfo(m_nSkillID);
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_XPSKILL_XP_RANDOM_SELF_BEGINCOOL:
                    {
                        SetVisible(true);
                        m_wndView.SetXPSkillInfo(m_nSkillID);
                        m_wndView.IncreaseProgressPart();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_WAREND:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        if ( m_wndView == null )
                        {
                            return;
                        }

                        UnloadView();
                        m_wndView = null;
                    }
                    break;
                case WndMsgID.WND_MSG_WARBUTTOMCENTER_SKILLSETSLOT:
                    {
                        UIMsgCmdData uiSkillSlot = (UIMsgCmdData)msgData;
                        cmd_creature_set_slot data = IntPtrHelper.toData<cmd_creature_set_slot>(uiSkillSlot.ptrParam);
                        if (data.nSlotIndex != 5)
                            return;

                       // SSchemeHeroXPskillConfig config = HeroXPskillConfig.Instance.GetHeroXPskillConfig(EntityFactory.MainHeroID, data.slotview.nId);
                       // if (config == null)
                       //     return;

                        m_nSkillID = data.slotview.nId;
                   
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class XPProgressPart : TimeCountDown
    {
        public enum ProgressInCreaseModel
        {
            ProgressInCreaseModel_Increase  = 1,
            ProgressInCreaseModel_DeIncrease ,
        }

        public Sprite[] progressSpriteArray;
        public Sprite[] frameSpriteArray;
        public Image FrameImage;
        public Image progressImage;
        public Text progressText;
        private int m_nChargeTime;
        private ProgressInCreaseModel m_progressModel = ProgressInCreaseModel.ProgressInCreaseModel_DeIncrease;

		// 当前充能百分比
		private float m_CurChargePercent;
		public float CurChargePercent { get { return m_CurChargePercent; } }

        public void StartChargeXpProgress()
        {
            m_nChargeTime = m_nChargeTime <= 0 ? GameLogicAPI.getXpSkillChargeTime()/1000 : m_nChargeTime;
            this.StartCountDown(m_nChargeTime, 1.0f, 1.0f);
        }

        protected override void CountDownRepeatCallBack()
        {
            base.CountDownRepeatCallBack();
            if (fOriginalTime >= -float.Epsilon && fOriginalTime <= float.Epsilon)
            {
                return;
            }

            float fProgress = 0;
            switch (m_progressModel)
            {
                case ProgressInCreaseModel.ProgressInCreaseModel_Increase:
                    {
                        fProgress = (fOriginalTime - fCurTime) / fOriginalTime;
                     
                    }
                    break;
                case ProgressInCreaseModel.ProgressInCreaseModel_DeIncrease:
                    {
                        fProgress = ( fCurTime) / fOriginalTime;
                    }
                    break;
                    default:
                        {

                        }
                    break;

            }

            progressImage.fillAmount = fProgress;

			m_CurChargePercent = fProgress * 100f;
			progressText.text = m_CurChargePercent.ToString("0") + "%";
        }

        protected override void CountDownCompleteCallBack()
        {
            base.CountDownCompleteCallBack();
            float fProgress = 0;
            switch (m_progressModel)
            {
                case ProgressInCreaseModel.ProgressInCreaseModel_Increase:
                    {
                        fProgress = 100;
                    }
                    break;
                case ProgressInCreaseModel.ProgressInCreaseModel_DeIncrease:
                    {
                        fProgress = 0;
                    }
                    break;
                default:
                    {

                    }
                    break;

            }

            progressImage.fillAmount = fProgress;
            progressText.text = fProgress.ToString() + "%";
        }
        
        public void IncreaseProgressPart()
        {
            CountDownCompleteCallBack();
            m_progressModel = ProgressInCreaseModel.ProgressInCreaseModel_Increase;
            StartChargeXpProgress();
            progressImage.sprite = progressSpriteArray[(int)m_progressModel - 1];
            FrameImage.sprite = frameSpriteArray[(int)m_progressModel - 1];
        }

        public void DecreaseProgressPart()
        {
            CountDownCompleteCallBack();
            m_progressModel = ProgressInCreaseModel.ProgressInCreaseModel_DeIncrease;
            StartChargeXpProgress();
            progressImage.sprite = progressSpriteArray[(int)m_progressModel - 1];
            FrameImage.sprite = frameSpriteArray[(int)m_progressModel - 1];
        }

        public void ResetProgressPart()
        {
            m_progressModel = ProgressInCreaseModel.ProgressInCreaseModel_DeIncrease;   
            CountDownCompleteCallBack();
        }

        public ProgressInCreaseModel ProgressModel
        {
            set {m_progressModel = value; }
            get { return m_progressModel; }
        }
    }
    public class UViewXPSkill : UIBaseWndView
    {
        Button xpSkillButton;
        bool bIsDisappear;
        Sequence mySequence;
        UTooltipTrigger slotTooltipTrigger;

        public Image skillImage;
        public XPProgressPart xpProgressPart;

		private string xpSkillName;

		// XP充能进度CD
		private int lastSendTick = 0;
		private const int sendInternalTimeMs = 5000;

		// XP充能完成光效
		private UEffectPrefabParam xpChargeOverEffect = null;

		public override void Awake()
        {
            xpSkillButton = transform.FindChild("Skill").GetComponent<Button>();
            slotTooltipTrigger = transform.FindChild("Skill").GetComponent<UTooltipTrigger>();
            skillImage.AddNewMaterial(EUIShaderType.EST_Gray);
            base.Awake();           
        }

        public void setXpSkill(cmd_creature_ShowPromptUI cmdData)
        {
			//SSchemeShowSpellPrompt SchemesSpellPrompt = GameMobaSchemesManager.Instance.GetSpellPromptInfo(cmdData.nPromptID);

			//if (SchemesSpellPrompt == null)
			//{
			//    return;
			//}

			//if (slotTooltipTrigger == null)
			//{
			//    slotTooltipTrigger = transform.FindChild("Skill").GetComponent<UTooltipTrigger>();
			//}

			//IntPtr strPtr = GameLogicAPI.getSpellStrData(SchemesSpellPrompt.nSpellID, (int)SPELL_DATA_ID.SPDATA_DESC);
			//if (strPtr != null)
			//{
			//    string strDesc = IntPtrHelper.Ptr2Str(strPtr);
			//    UBB.toHtml(ref strDesc, UBB_FORMAT_TYPE.UGUI);
			//    slotTooltipTrigger.SetText(UTooltipParamName.BodyText, strDesc);
			//}



			if ( cmdData.nType != 0 )
            {
                DecreaseProgressPart();

                mySequence = DOTween.Sequence();
                mySequence.Append(xpSkillButton.transform.DOScale(0.5f, 0.5f))
                            .SetDelay(1.0f);
                mySequence.SetLoops(-1,LoopType.Yoyo);
                mySequence.OnKill(() => mySequence = null);

            }
            else
            {
                if (xpProgressPart.ProgressModel != XPProgressPart.ProgressInCreaseModel.ProgressInCreaseModel_Increase)
                {
                    if (mySequence != null)
                        mySequence.Kill();

                    skillImage.AddNewMaterial(EUIShaderType.EST_Gray);
                    ResetProgressPart();
                    this.SetVisible(false);

                } 
            }
        }

        private void ResetProgressPart()
        {
            if (mySequence != null)
                mySequence.Kill();

            xpSkillButton.transform.localScale = new Vector3(1, 1, 1);

            xpProgressPart.ResetProgressPart();
        }

        private void DecreaseProgressPart()
        {
            if (mySequence != null)
                mySequence.Kill();

            xpSkillButton.transform.localScale = new Vector3(1, 1, 1);
            skillImage.ResetToDefaultMaterial();
            xpProgressPart.DecreaseProgressPart();

			// 开启XP特效
			openXPChargeOverEffect();
		}

        public void onChickXpButton()
        {
            // TODO释放技能
			
        }

        public void IncreaseProgressPart()
        {
            if (mySequence != null)
                mySequence.Kill();

            xpSkillButton.transform.localScale = new Vector3(1, 1, 1);
            skillImage.AddNewMaterial(EUIShaderType.EST_Gray);
            xpProgressPart.IncreaseProgressPart();

			// 关闭XP特效
			closeXPChargeOverEffect();
        }

        public void SetXPSkillInfo(int nSpellID)
        {
            if (nSpellID <= 0)
                return;

            if (slotTooltipTrigger == null)
            {
                slotTooltipTrigger = transform.FindChild("Skill").GetComponent<UTooltipTrigger>();
            }

            IntPtr Ptr = GameLogicAPI.getSpellData(nSpellID);
            if (Ptr == IntPtr.Zero)
                return;

            SPELL.SPELL_DATA spellData = IntPtrHelper.toData<SPELL.SPELL_DATA>(Ptr);
            IntPtr strPtr = GameLogicAPI.getSpellStrData(nSpellID, (int)SPELL_DATA_ID.SPDATA_DESC);
            String szDesc = IntPtrHelper.Ptr2Str(strPtr);

            UBB.toHtml(ref szDesc, UBB_FORMAT_TYPE.UGUI);
            slotTooltipTrigger.SetText(UTooltipParamName.BodyText, szDesc);
            skillImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_WAR_XPSKILL, 1, spellData.nIcon);

			xpSkillName = spellData.szName;
        }

		// 广播充能进度
		public void broadcastChargeProcess()
		{
			// 检查CD
			int curTick = Environment.TickCount;
			if(curTick - lastSendTick < sendInternalTimeMs)
			{
				UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CHAT_CLICK_RICHTEXT_ITEM_COOLDOWN);
				return;
			}
			lastSendTick = curTick;

			string msg = "";
			if (xpProgressPart.ProgressModel == XPProgressPart.ProgressInCreaseModel.ProgressInCreaseModel_Increase)
			{
				string tip1 = ULocalizationService.Instance.Get("UIView", "XPSkill", "ChargeTip1");
				msg = String.Format(tip1, xpSkillName, (int)xpProgressPart.CurChargePercent);
			}
			else if (xpProgressPart.ProgressModel == XPProgressPart.ProgressInCreaseModel.ProgressInCreaseModel_DeIncrease)
			{
				string tip2 = ULocalizationService.Instance.Get("UIView", "XPSkill", "ChargeTip2");
				msg = String.Format(tip2, xpSkillName, (int)xpProgressPart.CurChargePercent);
			}

			UIUtil.SendChatMessage(EMChatChannelType.CHAT_CHANNEL_CAMP, msg, false);
		}

		// 关闭XP充能完成特效
		private void closeXPChargeOverEffect()
		{
			if (xpChargeOverEffect != null)
				xpChargeOverEffect.EffectGameObject.SetActive(false);
		}

		// 开启XP充能完成特效
		private void openXPChargeOverEffect()
		{
			// 充能完成特效
			if (xpChargeOverEffect == null)
			{
				UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_UWarBottomCenter_XpSkillChargeOver, _tfAttachParent: transform);
				UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);
				xpChargeOverEffect = (UEffectPrefabParam)param;
			}
			else
			{
				xpChargeOverEffect.EffectGameObject.SetActive(true);
			}
		}
    }
}
