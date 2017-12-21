/*******************************************************************
** 文件名:	ResizableEditor.cs 
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
	[CustomEditor(typeof(Resizable), true)]
	public class ResizableEditor : CursorsEditor {
		public ResizableEditor()
		{
			Cursors = new List<string>(){
				"CurrentCamera",
				"CursorEWTexture",
				"CursorEWHotSpot",
				"CursorNSTexture",
				"CursorNSHotSpot",
				"CursorNESWTexture",
				"CursorNESWHotSpot",
				"CursorNWSETexture",
				"CursorNWSEHotSpot",
				"DefaultCursorTexture",
				"DefaultCursorHotSpot",
			};
		}
	}
}
