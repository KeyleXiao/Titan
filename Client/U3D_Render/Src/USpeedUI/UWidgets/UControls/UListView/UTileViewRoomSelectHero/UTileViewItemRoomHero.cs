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
using Stage_ManagerDef;

namespace USpeedUI.UWidgets
{
    [System.Serializable]
    public class UTileViewItemRoomHero
    {
        [SerializeField]
        public int     nSlotId;

        [SerializeField]
        public int nHeroType;

        [SerializeField]
        public int      nHeroIconId;

        [SerializeField]
        public bool bIsHaveHero;

        [SerializeField]
        public bool bIsLock;

        [SerializeField]
        public bool bSelect;

        [SerializeField]
        public string sHeroDesc;

        [SerializeField]
        public Sprite oldSprite;

        [SerializeField]
        public bool bIsLockByComputer;

        [SerializeField]
        public bool bIsLockByHero;

        [SerializeField]
        public int nPractisedLevel;

        [SerializeField]
        public string sHeroName;
         
        [SerializeField]
        public EMatchRoomState eRoomState;

        [SerializeField]
        public bool bWeekFree;

        [SerializeField]
        public int nSortIndex;

        [SerializeField]
        public int[]  nSkinIDList;

        [SerializeField]
        public string sPlayWaysTip;

        [SerializeField]
        public int nUnlockLevel;

        [SerializeField]
        public bool bFpsHero;

        [SerializeField]
        public bool bIsBan;

        [SerializeField]
        public int nUnlockTime;

        [SerializeField]
        public bool Lock
        {
            set 
            {
                if (!value)
                {
                    if(!bIsHaveHero&&!bWeekFree)
                    {
                        return;
                    }
                }

                bIsLock = value;
            }

            get { return bIsLock; }
        }

        [SerializeField]
        public int nIsNew;
    }
}
