/*******************************************************************
** 文件名:	EventDef.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-3-5
** 版  本:	1.0
** 描  述:	事件ID和结构定义

		
********************************************************************/
using System;
using System.Collections.Generic;
using System.Text;
using UnityEngine;


namespace U3D_Render.Utility
{
    //////////////事件ID定义/////////////////////////////////////////
    public enum EVENT
    {
        ENTITY_SHOW_HERO,       // 显示或隐藏英雄
    }

    //////////////事件结构定义/////////////////////////////////////////
    // 显示或隐藏英雄
    public struct SEntityShowHero
    {
        public EntityView   ev;
        public bool         bIsHide;
    }
}
