/*******************************************************************
** 文件名:	DragSupportEditor.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
            
		
********************************************************************/

using UnityEditor;
using System.Collections.Generic;

namespace UIWidgets
{
	[CanEditMultipleObjects]
	[CustomEditor(typeof(BaseDragSupport), true)]
	public class DragSupportEditor : CursorsEditor {
		public DragSupportEditor()
		{
			Cursors = new List<string>(){
				"AllowDropCursor",
				"AllowDropCursorHotSpot",
				"DeniedDropCursor",
				"DeniedDropCursorHotSpot",
				"DefaultCursorTexture",
				"DefaultCursorHotSpot",
			};
		}
	}
}
