/*******************************************************************
** 文件名:	UTileViewItemHero
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	
		
********************************************************************/
using UnityEngine;

namespace USpeedUI.UWidgets
{
    [System.Serializable]
    public class UTileViewItemHero
    {
        [SerializeField]
        public int nHeroID;

        [SerializeField]
        public Sprite headIcon;

        [SerializeField]
        public string stName;

        [SerializeField]
        public string stTitle;

        [SerializeField]
        public int nStarLv;

        [SerializeField]
        public int nStarLvLimit;

        [SerializeField]
        public float fExpValue;

        [SerializeField]
        public string stStarExp;

        [SerializeField]
        public bool bUpGrade;

        [SerializeField]
        public bool bHaveHero;

        [SerializeField]
        public bool bWeekFree;

        [SerializeField]
        public int nUnlockLevel;

        [SerializeField]
        public bool bFpsHero;
    }
}
