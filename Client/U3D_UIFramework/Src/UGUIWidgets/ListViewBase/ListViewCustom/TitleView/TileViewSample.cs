/*******************************************************************
** 文件名:	TileViewSample.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	TitleView例子
********************************************************************/

using UnityEngine;
using System.Collections.Generic;
using System;
using UIWidgets;

namespace UIWidgetsSamples {
	/// <summary>
	/// TileViewSample.
	/// </summary>
	public class TileViewSample : TileView<TileViewComponentSample,TileViewItemSample> {
		bool isStartedTileViewSample = false;

		//Comparison<TileViewItemSample> itemsComparison = (x, y) => x.Name.CompareTo(y.Name);

		/// <summary>
		/// Awake this instance.
		/// </summary>
        public new void Awake()
		{
			Start();
			//OnSelect.AddListener(ItemSelected);
			//OnSelectObject.AddListener(ItemSelected);
		}

		void ItemSelected(int index, ListViewItem component)
		{
			if (component!=null)
			{
				//(component as TileViewComponentSample).DoSomething();
			}
			Debug.Log(index);
			Debug.Log(DataSource[index].Name);
		}

		void ItemSelected(int index)
		{
			Debug.Log(index);
			Debug.Log(DataSource[index].Name);
		}

		/// <summary>
		/// Start this instance.
		/// </summary>
		public override void Start()
		{
			if (isStartedTileViewSample)
			{
				return ;
			}
			isStartedTileViewSample = true;
			
			base.Start();
			//DataSource.Comparison = itemsComparison;
		}

		/// <summary>
		/// Sets component data with specified item.
		/// </summary>
		/// <param name="component">Component.</param>
		/// <param name="item">Item.</param>
		protected override void SetData(TileViewComponentSample component, TileViewItemSample item)
		{
			component.SetData(item);
		}

		/// <summary>
		/// Set highlights colors of specified component.
		/// </summary>
		/// <param name="component">Component.</param>
		protected override void HighlightColoring(TileViewComponentSample component)
		{
			base.HighlightColoring(component);
		}

		/// <summary>
		/// Set select colors of specified component.
		/// </summary>
		/// <param name="component">Component.</param>
		protected override void SelectColoring(TileViewComponentSample component)
		{
			base.SelectColoring(component);
		}

		/// <summary>
		/// Set default colors of specified component.
		/// </summary>
		/// <param name="component">Component.</param>
		protected override void DefaultColoring(TileViewComponentSample component)
		{
			base.DefaultColoring(component);
		}
	}
}
