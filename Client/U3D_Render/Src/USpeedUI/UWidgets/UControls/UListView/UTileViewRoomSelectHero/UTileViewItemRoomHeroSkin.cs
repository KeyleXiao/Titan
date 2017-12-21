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
    public class UTileViewItemRoomHeroSkin
    {
        [SerializeField]
        public int nSkinType;

        [SerializeField]
        public int nSkinId;

        [SerializeField]
        public bool bIsHaveSkin;

        [SerializeField]
        public bool bSelect;

        [SerializeField]
        public int nHeroID;

        [SerializeField]
        public int nSkinIconId;

        [SerializeField]
        public uint dwSkinLimitTime;

        [SerializeField]
        public string szSkinName;

        [SerializeField]
        public EMatchRoomState eRoomState = EMatchRoomState.MatchRoomState_Wait;
    }
}
