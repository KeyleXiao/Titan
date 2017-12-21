/*******************************************************************
** 文件名:	IEventHandler.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-3-5
** 版  本:	1.0
** 描  述:	事件接口定义

		
********************************************************************/
using System;
using System.Collections.Generic;

namespace U3D_Render.Utility
{
    /// <summary>
    /// 事件接口
    /// </summary>
    public interface IEventHandler
    {
        /// <summary>
        /// 事件回调函数
        /// </summary>
        /// <param name="eventID">事件ID</param>
        /// <param name="sourceID">事件来源ID</param>
        /// <param name="objEvent">事件数据</param>
        void OnEvent(U3D_Render.Utility.EVENT eventID, int sourceID, System.Object objEvent);
    }
}
