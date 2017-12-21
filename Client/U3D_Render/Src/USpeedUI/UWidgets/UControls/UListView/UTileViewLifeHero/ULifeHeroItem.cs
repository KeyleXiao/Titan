using UnityEngine;

namespace USpeedUI.UWidgets
{
    [System.Serializable]
    public class ULifeHeroItem
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


        /// <summary>
        /// 新手引导添加控件标示
        /// </summary>
        public bool bUseGuideWidget;
        public UGuideWidgetMsgData GuideWidgetData;
    }
}
