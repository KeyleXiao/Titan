/*******************************************************************
** 文件名:	TreeView.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	树形列表显示对象 ,更新显示元素
		
********************************************************************/
using UnityEngine;
using UIWidgets;
using USpeedUI.UWidgets;

namespace USpeedUI.WarGoodsStore
{
	/// <summary>
	/// TreeView.
	/// </summary>
    public class UWarGoodsStoreTreeView : UCheckBoxTreeView<UWarGoodsStoreTreeComponent, UWarGoodsStoreTreeComponentData>
    {
        protected override void OnComponentCheckBoxValueChange(int _nIndex, bool _bCheck)
        {
            base.OnComponentCheckBoxValueChange(_nIndex, _bCheck);

            //由于勾选框和组件选取不是同一套逻辑，
            //组件的选中逻辑和状态不会清除
            DefaultColoring(GetItem(SelectedIndex));//DefaultCheckedColoring(GetItem(SelectedIndex));


            if (_bCheck)
            {
                SelectColoring(GetItem(_nIndex));//CheckedColoring(GetItem(_nIndex));                //处理勾选时候的组件表现
                if (SelectedIndex != -1 && _nIndex != SelectedIndex)
                {
                    var node = NodesList[SelectedIndex];
                    (node.Node.Item as UCheckBoxTreeViewItem).IsCheckBoxOn = false;
                    SilentDeselect(SelectedIndicies);
                }
            }
        }

        /// <summary>
        /// Sets component data with specified item.
        /// </summary>
        /// <param name="component">Component.</param>
        /// <param name="item">Item.</param>

        protected override void SetData(UWarGoodsStoreTreeComponent component, ListNode<UWarGoodsStoreTreeComponentData> item)
        {
            component.SetData(item.Node, item.Depth);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UWarGoodsStoreTreeComponent component)
		{
            //选中效果只在父节点处理
            if (!component.CheckBox.IsActive())
                base.HighlightColoring(component);
        }
		
		/// <summary>
		/// Set select colors of specified component.
		/// </summary>
		/// <param name="component">Component.</param>
        protected override void SelectColoring(UWarGoodsStoreTreeComponent component)
		{
            //选中效果只在父节点处理
            if (!component.CheckBox.IsActive())
                base.SelectColoring(component);
            
			//component.Text.color = SelectedColor;
		}
		
		/// <summary>
		/// Set default colors of specified component.
		/// </summary>
		/// <param name="component">Component.</param>
        protected override void DefaultColoring(UWarGoodsStoreTreeComponent component)
		{
			if (component==null)
			{
				return ;
			}
            //选中效果只在父节点处理
            if (!component.CheckBox.IsActive())
                base.DefaultColoring(component);

			if (component.Text!=null)
			{
				component.Text.color = DefaultColor;
			}
		}

    }
}
