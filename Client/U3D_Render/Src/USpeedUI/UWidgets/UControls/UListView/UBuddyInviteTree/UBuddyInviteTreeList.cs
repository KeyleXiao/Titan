/*******************************************************************
** 文件名:	UBuddyGroupTreeList.cs
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
    public class UBuddyInviteTreeList : TreeViewCustom<UBuddyInviteTreeItemComponent, UBuddyInviteTreeItemData>
    {
        protected override void SetData(UBuddyInviteTreeItemComponent component, ListNode<UBuddyInviteTreeItemData> item)
        {
			component.SetData(item.Node, item.Depth);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UBuddyInviteTreeItemComponent component)
        {
            base.HighlightColoring(component);
            component.Text.color = HighlightedColor;
        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UBuddyInviteTreeItemComponent component)
        {
            base.SelectColoring(component);
            component.Text.color = SelectedColor;
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(UBuddyInviteTreeItemComponent component)
        {
            if (component == null)
            {
                return;
            }
            base.DefaultColoring(component);
            if (component.Text != null)
            {
                component.Text.color = DefaultColor;
            }
        }

        public override IObservableList<TreeNode<UBuddyInviteTreeItemData>> Nodes
        {
            get
            {
                return nodes;
            }
            set
            {
                if (nodes != null)
                {
                    nodes.OnChange -= NodesChanged;
                    nodes.OnChange -= CollectionChanged;
                }
                nodes = value;
                RootNode.Nodes = value;
                Refresh();
                if (nodes != null)
                {
                    nodes.OnChange += NodesChanged;
                    nodes.OnChange += CollectionChanged;
                    CollectionChanged();
                }
            }
        }
    }
}
