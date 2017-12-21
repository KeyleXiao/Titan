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
    public class UBuddyGroupTreeItemData : TreeViewItem
    {
        // 好友信息
        //public DataCenter.SNSDataManager.BuddyInfo BuddyDetailInfo;
        public int nUserID;
        // 好友分组
        public int nBuddyGroupID;
        //public DataCenter.SNSDataManager.BuddyGroup BuddyGroupInfo;

        public UBuddyGroupTreeItemData(string strItemName, UnityEngine.Sprite sprIcon)
            : base(strItemName, sprIcon)
        {

        }
    }
}
