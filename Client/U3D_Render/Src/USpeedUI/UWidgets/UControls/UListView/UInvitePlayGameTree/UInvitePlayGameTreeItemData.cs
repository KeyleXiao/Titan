/*******************************************************************
** 文件名:	UBuddyGroupTreeItemData.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2016/12/7
** 版  本:	1.0
** 描  述:	好友系统中好友分组
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace USpeedUI.UWidgets
{
    public class UInvitePlayGameTreeItemData : TreeViewItem
    {
        // 玩家id
        public int nUserID;     
        // 分组
        public int nGroupID;
        // 头像
        public int nHeadIconID;
        // 性别
        public int nSex;
        // 游戏状态
        public int nGameState;
        // 段位
        public int nRank;
        // 段位描述
        public string RankName;
        // 是否选中
        public bool isSelected;
        // 是否已经发送邀请
        public bool isInvited;

        public UInvitePlayGameTreeItemData(string strItemName, UnityEngine.Sprite sprIcon)
            : base(strItemName, sprIcon)
        {

        }
    }
}
