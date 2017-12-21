/*******************************************************************
** 文件名:	ResizeListener.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
            RectTransform 大小改变事件
		
********************************************************************/

using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using System.Collections;

namespace UIWidgets
{
	/// <summary>
	/// Resize listener.
	/// </summary>
	public class ResizeListener : UIBehaviour
	{
		RectTransform rectTransform;

		/// <summary>
		/// Gets the RectTransform.
		/// </summary>
		/// <value>The RectTransform.</value>
		public RectTransform RectTransform {
			get {
				if (rectTransform==null)
				{
					rectTransform = transform as RectTransform;
				}
				return rectTransform;
			}
		}

		/// <summary>
		/// The OnResize event.
		/// </summary>
		public UnityEvent OnResize = new UnityEvent();

		Rect oldRect;

		/// <summary>
		/// Raises the rect transform dimensions change event.
		/// </summary>
		protected override void OnRectTransformDimensionsChange()
		{
			if (oldRect.Equals(RectTransform.rect))
			{
				return ;
			}
			oldRect = RectTransform.rect;
			OnResize.Invoke();
		}
	}
}
