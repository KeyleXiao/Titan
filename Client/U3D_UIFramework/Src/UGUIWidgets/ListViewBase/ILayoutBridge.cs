/*******************************************************************
** 文件名:	ILayoutBridge.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	ILayoutBridge 控件使用哪种布局（Official or Easylayout)方式接口
		
********************************************************************/
using UnityEngine;

namespace UIWidgets {

	/// <summary>
	/// ILayoutBridge.
	/// </summary>
	public interface ILayoutBridge {
		/// <summary>
		/// Gets or sets a value indicating whether this instance is horizontal.
		/// </summary>
		/// <value><c>true</c> if this instance is horizontal; otherwise, <c>false</c>.</value>
		bool IsHorizontal {
			get;
			set;
		}

		/// <summary>
		/// Gets or sets a value indicating whether this <see cref="UIWidgets.ILayoutBridge"/> update content size fitter.
		/// </summary>
		/// <value><c>true</c> if update content size fitter; otherwise, <c>false</c>.</value>
		bool UpdateContentSizeFitter {
			get;
			set;
		}

		/// <summary>
		/// Updates the layout.
		/// </summary>
		void UpdateLayout();

		/// <summary>
		/// Sets the filler.
		/// </summary>
		/// <param name="first">First.</param>
		/// <param name="last">Last.</param>
		void SetFiller(float first, float last);

		/// <summary>
		/// Gets the size of the item.
		/// </summary>
		/// <returns>The item size.</returns>
		Vector2 GetItemSize();

		/// <summary>
		/// Gets the top or left margin.
		/// </summary>
		/// <returns>The margin.</returns>
		float GetMargin();

		/// <summary>
		/// Gets the full margin.
		/// </summary>
		/// <returns>The full margin.</returns>
		float GetFullMargin();

		/// <summary>
		/// Gets the spacing.
		/// </summary>
		/// <returns>The spacing.</returns>
		float GetSpacing();
	}
}
