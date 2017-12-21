/*******************************************************************
** 文件名:	UCheckBoxTreeViewItem.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2017-9-11
** 版  本:	1.0
** 描  述:	带勾选功能的树状控件，数据对象
            1、未支持勾选不同显示类型
********************************************************************/
using System;
using UIWidgets;
using UnityEngine;


namespace USpeedUI.UWidgets
{
    [Serializable]
    public class UCheckBoxTreeViewItem : TreeViewItem
    {
        [SerializeField]
        public enum ECheckType
        {
            /// <summary>
            /// 所有子节点已勾选
            /// </summary>
            ECT_Default,
            /// <summary>
            /// 子节点有没勾选
            /// </summary>
            ECT_Half
        }

        [SerializeField]
        protected bool m_bIsCheckBoxOn;
        public bool IsCheckBoxOn
        {
            get { return m_bIsCheckBoxOn; }
            set
            {
                m_bIsCheckBoxOn = value;
                Changed("IsCheckBoxOn");
            }
        }

        protected ECheckType m_eCheckType;
        public ECheckType CheckType
        {
            get { return m_eCheckType; }
            set
            {
                m_eCheckType = value;
                Changed("CheckType");
            }
        }

        public UCheckBoxTreeViewItem(string strItemName, UnityEngine.Sprite sprIcon)
            : base(strItemName, sprIcon)
        {

        }
    }
}
