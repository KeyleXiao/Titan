/*******************************************************************
** 文件名:	EntityPosUpdate.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/3/6
** 版  本:	1.0
** 描  述:	实体位置更新了

		
********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI;
using UnityEngine;

namespace U3D_Render
{
	class EntityLocUpdate
	{
		public static void SendActorLocationChange(Vector3 vNewPos)
		{
			UActorLocationChangeMsgData msgData = new UActorLocationChangeMsgData();
			msgData.msgID = (int)WndMsgID.WND_MSG_COMMON_LOCATION_CHANGE;
			msgData.vNewPos = vNewPos;
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_LOCATION_CHANGE, msgData);
		}
	}
}
