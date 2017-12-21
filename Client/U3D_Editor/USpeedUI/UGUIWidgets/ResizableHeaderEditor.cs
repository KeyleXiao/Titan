/*******************************************************************
** 文件名:	ResizableHeaderEditor.cs 
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
	[CustomEditor(typeof(ResizableHeader), true)]
	public class ResizableHeaderEditor : CursorsEditor {
		public ResizableHeaderEditor()
		{
			Cursors = new List<string>(){
				"CurrentCamera",

				"CursorTexture",
				"CursorHotSpot",

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
