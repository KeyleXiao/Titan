/*******************************************************************
** 文件名:	ListViewCustomHeightEditor.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	
		
********************************************************************/

using UnityEditor;
using UIWidgets;

namespace UIWidgetsSamples
{
	public class ListViewCustomHeightEditor : ListViewCustomEditor
	{
		public ListViewCustomHeightEditor()
		{
			Properties.Add("ForceAutoHeightCalculation");
		}
	}
}
