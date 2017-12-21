/*******************************************************************
** 文件名:	UTileRoomBanHero
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	
		
********************************************************************/
using UnityEngine;
using System.Collections.Generic;
using System;
using UIWidgets;

namespace USpeedUI.UWidgets
{
    public class UTileRoomBanHero : TileView<UTileComponentRoomBanHero, UTileItemRoomBanHero>
    {
        protected override void SetData(UTileComponentRoomBanHero component, UTileItemRoomBanHero item)
        {
            component.SetData(item);
        }
    }
}
