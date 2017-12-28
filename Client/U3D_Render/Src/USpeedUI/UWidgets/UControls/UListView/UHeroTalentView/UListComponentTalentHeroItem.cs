using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine.UI;
using USpeedUI;
using USpeedUI.UWidgets;
using USpeedUI.Guide;
using UnityEngine;
using USpeedUI.UExtensions;
using DataCenter;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListComponentTalentHeroItem : ListViewItem
    {
        private UListHeroTalentData ItemData;

        public Image RecommandImage;
        public Image TalentIconImage;
        public Text TalentDescText;
        public Image UseTypeImage;
        public Text UseTypeText;
        public GameObject ActiveObject;
        public Text TalentNameText;
        public Text TalentKeyText;
        public GameObject NoActiveLockObject;

        private ObservableList<UTileViewItemRoomHero> SelectListHeroData = new ObservableList<UTileViewItemRoomHero>();

        // 是否激活天赋
        private bool bActive = false;
        // 是否激活前置天赋
        private bool bTermActive = false;
        // 是否可以激活天赋
        private bool bCanActive = false;

        protected override void Start()
        {
            base.Start();
        }

        protected override void OnDestroy()
        {
            base.OnDestroy();
        }

        public virtual void SetData(UListHeroTalentData item)
        {
            TalentIconImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, item.nTalentIconId);
            TalentIconImage.ResetToDefaultMaterial();
            TalentNameText.ResetToDefaultMaterial();
            TalentDescText.ResetToDefaultMaterial();
            TalentKeyText.ResetToDefaultMaterial();
            if (TalentIconImage.sprite == null) // 空的话给个默认的
            {
                TalentIconImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, 3, 4);
            }      

            string sTalentDesc = item.sTalentDesc;
            string sTalentName = item.sTalentName;
            UBBProperty.GetInstance().getRatio(sTalentDesc);
            U3D_Render.Common.UBB.toHtml(ref sTalentDesc, UBB_FORMAT_TYPE.UGUI);
            // 此处需要再转换一下，不然PASD标签不能正确解析
            U3D_Render.Common.UBB.toHtml(ref sTalentDesc, UBB_FORMAT_TYPE.UGUI);

            U3D_Render.Common.UBB.toHtml(ref sTalentName, UBB_FORMAT_TYPE.UGUI);

            TalentDescText.text = sTalentDesc;
            UseTypeText.text = item.sUseTypeDesc;
            TalentNameText.text = sTalentName;
            TalentKeyText.text = "Ctrl+" + item.nSortIndex;
            Item = item;

            bActive = LogicDataCenter.heroTalentDataManager.isActivedThisTalent(item.nTalentId);
            ActiveObject.SetActive(bActive);
            
            if (NoActiveLockObject!=null)
            {
                NoActiveLockObject.SetActive(item.bLock); 
            }        
   
			// 前置天赋未激活
            bTermActive = LogicDataCenter.heroTalentDataManager.isActivedThisTalent(item.nTalentTermId);
            ActiveObject.SetActive(bActive);

            if( item.nTalentTermId > 0 && !bTermActive)
            {
                TalentDescText.text = sTalentDesc + "(前置天赋未激活)";
                TalentIconImage.AddNewMaterial(EUIShaderType.EST_Gray);
                TalentNameText.AddNewMaterial(EUIShaderType.EST_Gray);
                TalentDescText.AddNewMaterial(EUIShaderType.EST_Gray);
                TalentKeyText.AddNewMaterial(EUIShaderType.EST_Gray);
            }

            bCanActive = LogicDataCenter.heroTalentDataManager.isCanActiveTalent(item.nTalentId, item.nTalentLevel);

            // 推荐图标
            if (RecommandImage != null)
			{
				if (Item.bRecommand)
					RecommandImage.gameObject.SetActive(true);
				else
					RecommandImage.gameObject.SetActive(false);
			}

            #region 新手引导部分
            if(item.bUseGuideWidget)
            //{
            //    gameObject.RemoveComponent<UGuideWidget>();
            //}
            //else
            {
                UGuideWidget guideWidget = gameObject.GetComponent<UGuideWidget>();
                if (guideWidget == null)
                    guideWidget = this.gameObject.AddComponent<UGuideWidget>();
                guideWidget.GuideID = (GUIDE.EGuideNodeID)item.nGuideID;
                guideWidget.GuideStepID = item.nGuideStepID;
                guideWidget.GuideEffectID = item.nGuideEffectID;
                guideWidget.IsForceGuide = true;
            }
            #endregion
        }

        public UListHeroTalentData Item
        {
            set
            {
                ItemData = value;
            }
            get
            {
                return ItemData;
            }
        }

        public ObservableList<UTileViewItemRoomHero> SelectHeroData
        {
            set
            {
                SelectListHeroData = value;
            }
            get
            {
                return SelectListHeroData;
            }
        }

        public override void OnPointerClick(UnityEngine.EventSystems.PointerEventData eventData)
        {
            base.OnPointerClick(eventData);
        }

        public override void OnPointerEnter(UnityEngine.EventSystems.PointerEventData eventData)
        {
            if (bActive)
            {
                return;        
            }

            if (Item.nTalentTermId > 0 && !bTermActive)
            {
                return;
            }

			ActiveObject.SetActive(true);

            base.OnPointerEnter(eventData);
        }

		public override void OnPointerExit(UnityEngine.EventSystems.PointerEventData eventData)
		{
            if (bActive)
			{
				return;
			}

			if (Item.nTalentTermId > 0 && !bTermActive)
			{
				return;
			}

			ActiveObject.SetActive(false);

			base.OnPointerEnter(eventData);
		}

		private void onClickTalentItemInWar()
		{
			int nCurLv = EntityUtil.getMainHeroLv();
			string tips = "";
			if (nCurLv < 18)
			{
				tips = ULocalizationService.Instance.Get("UIView", "TalentView", "NextLvActiveTalentTips");
				int nNextLv = DataCenter.LogicDataCenter.heroTalentDataManager.getNextCanActiveLv(nCurLv);
				tips = String.Format(tips, nNextLv);
			}
			else
			{
				tips = ULocalizationService.Instance.Get("UIView", "TalentView", "AllTalentActivedTips");
			}

			UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CUSTOMER_TIP, tips);

			// 关闭天赋窗口
			UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_MAINHEROTALENTCLOSE, null);
		}
	}

}
