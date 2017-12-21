/*******************************************************************
** 文件名:	UTileViewItemHero
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	
		
********************************************************************/
using UnityEngine;
using UIWidgets;

namespace USpeedUI.UWidgets
{
    [System.Serializable]
    public class UListWarMainModeItem
    {

        [SerializeField]
        public int      nModeID;

        [SerializeField]
        public string   strMode;

        [SerializeField]
        public string   strModeDesc;

        [SerializeField]
        public string   strModeTooltip;

        [SerializeField]
        public bool     bOpenEffect;

        /// <summary>
        /// 新手引导添加控件标示
        /// </summary>
        public bool bUseGuideWidget;
        public UGuideWidgetMsgData GuideWidgetData;
    }
}
