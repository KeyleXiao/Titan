/*******************************************************************
** 文件名:	UTileComponentRoomBanHero
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	
		
********************************************************************/
using UnityEngine;
using UnityEngine.UI;
using System;
using UIWidgets;
using System.Runtime.InteropServices;

namespace USpeedUI.UWidgets
{
    public class UTileComponentRoomBanHero : ListViewItem
    {
        [SerializeField]
        public Image HeadIcon;

        [SerializeField]
        public Text NameDesc;

        [SerializeField]
        public Image MaskIcon;

        [SerializeField]
        public Image BanIcon;

        [SerializeField]
        public Text BanCount;

        private bool bIsBan;

        public void SetData(UTileItemRoomBanHero item)
        {
            if (item == null || item.nHeroID <= 0)
                return;

            bIsBan = item.bIsBan;
                
            HeadIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, 3, item.nHeroID);
            NameDesc.text = item.strHeroName;

            BanCount.text = String.Format("+{0}", item.nBanCount);
            MaskIcon.gameObject.SetActive(bIsBan);
            BanIcon.gameObject.SetActive(bIsBan);
            BanCount.gameObject.SetActive(bIsBan);
        }

        public void SelectColoring()
        {
            if (bIsBan)
                return;

            MaskIcon.gameObject.SetActive(true);
        }

        public void DefaultColoring()
        {
            if (bIsBan)
                return;

            MaskIcon.gameObject.SetActive(false);
        }
    }
}
