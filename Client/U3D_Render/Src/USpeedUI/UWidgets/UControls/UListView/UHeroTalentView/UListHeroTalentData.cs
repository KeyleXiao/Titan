using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using USpeedUI.UWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListHeroTalentData
    {
        public int nTalentIconId;
        public string sTalentDesc;
        public int nUseTypeIcon;
        public string sUseTypeDesc;
        public int nTalentLevel;
        public string sTalentName;
        //public bool bActive;
        public int nTalentId;
        //public bool bCanActive;
        public bool bOnlyActive;
        public bool bLock;
        public int nTalentTermId;
        public int nSortIndex;
		public bool bRecommand;
		//public bool bTalentTermIsOk;		// 前置天赋是否已经激活
        /// <summary>
        /// 新手引导添加控件标示
        /// </summary>
        public bool bUseGuideWidget;
        public int nGuideID;
        public int nGuideStepID;
        public int nGuideEffectID;
    }   
}

    
