using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace USpeedUI.UWidgets
{
    public class URuneTypeTreeList : TreeViewCustom<URuneTypeTreeItemComponent, URuneTypeTreeItemData>
    {
        protected override void SetData(URuneTypeTreeItemComponent component, ListNode<URuneTypeTreeItemData> item)
        {
            component.SetData(item.Node, item.Depth);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(URuneTypeTreeItemComponent component)
        {
            base.HighlightColoring(component);
            component.Text.color = HighlightedColor;
        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(URuneTypeTreeItemComponent component)
        {
            base.SelectColoring(component);
            component.Text.color = SelectedColor;
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(URuneTypeTreeItemComponent component)
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

        public override IObservableList<TreeNode<URuneTypeTreeItemData>> Nodes
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
