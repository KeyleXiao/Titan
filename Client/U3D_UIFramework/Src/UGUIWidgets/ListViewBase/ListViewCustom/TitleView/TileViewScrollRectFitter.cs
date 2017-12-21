/*******************************************************************
** 文件名:	TileViewScrollRectFitter.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	让ListView支持拉伸宽高
********************************************************************/

using UnityEngine;

namespace UIWidgets {
	/// <summary>
	/// TileView ScrollRect fitter. Resize ScrollRect to fit whole number of items.
	/// </summary>
	[RequireComponent(typeof(ListViewBase))]
	public class TileViewScrollRectFitter : MonoBehaviour {
		ListViewBase tileView;

		/// <summary>
		/// Gets the TileView.
		/// </summary>
		/// <value>The TileView.</value>
		public ListViewBase TileView {
			get {
				if (tileView==null)
				{
					tileView = GetComponent<ListViewBase>();
				}
				return tileView;
			}
		}

		bool isStarted;

		/// <summary>
		/// The base size delta.
		/// </summary>
		protected Vector2 BaseSizeDelta;

		/// <summary>
		/// The resize listener.
		/// </summary>
		protected ResizeListener ResizeListener;

		/// <summary>
		/// Start this instance.
		/// </summary>
		public virtual void Start()
		{
			if (isStarted)
			{
				return ;
			}
			isStarted = true;

			var scrollRect = TileView.GetScrollRect();

			if (scrollRect!=null)
			{
				ResizeListener = scrollRect.GetComponent<ResizeListener>();
				if (ResizeListener==null)
				{
					ResizeListener = scrollRect.gameObject.AddComponent<ResizeListener>();
				}

				BaseSizeDelta = (scrollRect.transform as RectTransform).sizeDelta;

				ApplyFitter();
			}
		}

		/// <summary>
		/// Add resize listener.
		/// </summary>
		protected void ResizeListenerOn()
		{
			ResizeListener.OnResize.AddListener(ApplyFitter);
		}

		/// <summary>
		/// Remove resize listener.
		/// </summary>
		protected void ResizeListenerOff()
		{
			ResizeListener.OnResize.RemoveListener(ApplyFitter);
		}

		/// <summary>
		/// Applies the fitter.
        /// <para>WillNote:该类的重点是根据当前拉伸的大小和限制ListView的最小宽高而改变ListView的宽高</para>
		/// </summary>
		public virtual void ApplyFitter()
		{
			ResizeListenerOff();

			var scrollRect = TileView.GetScrollRect();
			var scrollRectTransform = scrollRect.transform as RectTransform;
			var size = scrollRectTransform.rect.size;

			size += BaseSizeDelta - scrollRectTransform.sizeDelta;

			var item_width = TileView.GetDefaultItemWidth();
			var items_in_row = Mathf.Max(1, Mathf.FloorToInt(size.x / item_width));
			var required_width = items_in_row * (item_width + TileView.GetItemSpacing());
			if (size.x!=required_width)
			{
				scrollRectTransform.SetSizeWithCurrentAnchors(RectTransform.Axis.Horizontal, required_width);
			}

			var item_height = TileView.GetDefaultItemHeight();
			var items_in_column = Mathf.Max(1, Mathf.FloorToInt(size.y / item_height));
			var required_height = items_in_column * (item_height + TileView.GetItemSpacing());
			if (size.y!=required_height)
			{
				scrollRectTransform.SetSizeWithCurrentAnchors(RectTransform.Axis.Vertical, required_height);
			}

			ResizeListenerOn();
		}

		/// <summary>
		/// This function is called when the MonoBehaviour will be destroyed.
		/// </summary>
		public virtual void OnDestroy()
		{
			if (ResizeListener!=null)
			{
				ResizeListenerOff();
			}
		}
	}
}
