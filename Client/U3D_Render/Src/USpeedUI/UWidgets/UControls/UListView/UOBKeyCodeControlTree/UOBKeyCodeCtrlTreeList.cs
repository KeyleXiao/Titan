/*******************************************************************
** 文件名:	UOBKeyCodeCtrlTreeList.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2017 - Speed
** 创建人:	昔文博
** 日  期:	2017/11/27
** 版  本:	1.0
** 描  述:	OB键位设置分组列表
** 应  用:  
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace USpeedUI.UWidgets.UOBKeyCodeCtrl
{
    public class UOBKeyCodeCtrlTreeList : TreeViewCustom<UOBKeyCodeCtrlTreeItemComponent, UOBKeyCodeCtrlTreeItemData>
    {
        protected override void SetData(UOBKeyCodeCtrlTreeItemComponent component, ListNode<UOBKeyCodeCtrlTreeItemData> item)
        {
            component.SetData(item.Node, item.Depth);
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(UOBKeyCodeCtrlTreeItemComponent component)
        {
            //if (component == null)
            //{
            //    return;
            //}
            //base.DefaultColoring(component);
            //if (component.Text != null)
            //{
            //    component.Text.color = DefaultColor;
            //}
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UOBKeyCodeCtrlTreeItemComponent component)
        {
            //base.HighlightColoring(component);
            //component.Text.color = HighlightedColor;
        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UOBKeyCodeCtrlTreeItemComponent component)
        {
            //base.SelectColoring(component);
            //component.Text.color = SelectedColor;
        }

        public override IObservableList<TreeNode<UOBKeyCodeCtrlTreeItemData>> Nodes
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
