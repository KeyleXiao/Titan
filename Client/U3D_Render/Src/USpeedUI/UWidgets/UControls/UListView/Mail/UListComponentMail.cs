using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine.UI;
using USpeedUI;
using USpeedUI.UWidgets;
using UnityEngine;
using Mail_ManagerDef;
using U3D_Render.Common;
using UnityEngine.EventSystems;
using Data.ActorPrizeConfig;
using War;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListComponentMail : ListViewItem
    {
        public Image sourceImage;
        public Text title;
        public Text senderName;
        public Text recvTime;
        public Image readFlagImg;
        public Sprite markReadSprite;
        public Image SelectedImg;

        private Sprite defaultMarkReadSprite;

        private long m_nMailID;

        public virtual void SetData(UListItemMail item)
        {
            m_nMailID = item.nMailID;

            if (defaultMarkReadSprite == null)
                defaultMarkReadSprite = readFlagImg.sprite;

            if (defaultMarkReadSprite != null)
                readFlagImg.sprite = defaultMarkReadSprite;

            if (SelectedImg != null)
                SelectedImg.enabled = false;

            sourceImage.enabled = false;
            if (item.nSourceType > (int)EMMailSourceType.emMailSourceType_Normal && item.nSourceType <= (int)EMMailSourceType.emMailSourceType_PlayerTrade)
            {
                sourceImage.enabled = true;
                sourceImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MailSourceIcon, WndID.WND_ID_MAILBOX, item.nSourceType);
            }

            if (item.nReadFlag == 1)
                readFlagImg.sprite = markReadSprite;

            title.text = item.szTitle;
            senderName.text = String.Format(ULocalizationService.Instance.Get("UIView", "Mail", "FromNameDes"), item.szSenderName);

			DateTime dt = Api.convertTimeStamp2DateTime(item.nRecvTime);
            //DateTime dt = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(item.nRecvTime);
            recvTime.text = string.Format("{0:MM-dd HH:mm}", dt);
        }

        public virtual void SelectColoring()
        {
            // 选中
            if (SelectedImg != null)
            {
                readFlagImg.sprite = markReadSprite;
                SelectedImg.enabled = true;
            }
        }

        public virtual void DefaultColoring()
        {
            if (SelectedImg != null)
                SelectedImg.enabled = false;
        }
    }

    public class UListComponentMailGoods : ListViewItem
    {
        public Image goodsImage;
        public Text goodsName;
        public Text goodsNum;

        private UTooltipTrigger goodsTooltipTrigger;

        public virtual void SetData(UListItemMailGoods item)
        {
            if (goodsTooltipTrigger == null)
            {
                goodsTooltipTrigger = transform.GetComponentInChildren<UTooltipTrigger>(true);
            }

            SSchemeActorPrizeConfig config = new SSchemeActorPrizeConfig();
            config = ActorPrizeConfig.Instance.GetActorPrizeConfig(item.nGoodsID);
            if (config != null)
            {
                goodsName.text = config.strPrizeName;
                goodsNum.text = item.nCount.ToString();
                if (config.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HERO || config.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROCARD)
                {
                    goodsImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_MAILBOX, 1, 4, config.nPrizeParam[0]);
                }
                else
                {
                    goodsImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_MAILBOX, config.nPrizeType, config.nPrizeIcon);
                }             

                String szDesc = config.strPrizeDesc;
                UBB.toHtml(ref szDesc, UBB_FORMAT_TYPE.UGUI);
                goodsTooltipTrigger.SetText(UTooltipParamName.BodyText, szDesc);
            }
        }

        public virtual void SelectColoring()
        {

        }

        public virtual void DefaultColoring()
        {

        }
    }

    public class UListComponentMailPlusData : ListViewItem
    {
        public Text plusData;

        public virtual void SetData(UListItemMailPlusData item)
        {
            plusData.text = String.Format(item.szName, item.nCount);
        }

        public virtual void SelectColoring()
        {

        }

        public virtual void DefaultColoring()
        {

        }
    }
}
