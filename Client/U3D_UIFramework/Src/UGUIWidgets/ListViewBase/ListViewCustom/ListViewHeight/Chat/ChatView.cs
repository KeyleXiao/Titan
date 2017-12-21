/*******************************************************************
** 文件名:	ChatView.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	聊天框列表控制类
		
********************************************************************/


using UIWidgetsSamples;

namespace UIWidgets
{
	/// <summary>
	/// ChatView.
	/// </summary>
	public class ChatView : ListViewCustomHeight<ChatLineComponent,ChatLine> {
		/// <summary>
		/// Sets component data with specified item.
        /// <para>WillNote:这里是这个类的重点从源数据获取并设置到ViewItem</para>
		/// </summary>
		/// <param name="component">Component.</param>
		/// <param name="item">Item.</param>
		protected override void SetData(ChatLineComponent component, ChatLine item)
		{
			component.SetData(item);
		}

		// leave coloring functions empty
		protected override void HighlightColoring(ChatLineComponent component)
		{
		}

		protected override void SelectColoring(ChatLineComponent component)
		{
		}
		
		protected override void DefaultColoring(ChatLineComponent component)
		{
		}
	}
}
