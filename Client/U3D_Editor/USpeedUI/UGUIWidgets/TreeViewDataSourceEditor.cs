/*******************************************************************
** 文件名:	TreeViewDataSourceEditor.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-16
** 版  本:	1.8.0
** 描  述:	
		
********************************************************************/


using UnityEngine;
using UnityEditor;
using System.Collections;

namespace UIWidgets {

	[CustomEditor(typeof(TreeViewDataSource))]
	public class TreeViewDataSourceEditor : Editor {

		public override void OnInspectorGUI()
		{
			//base.DrawDefaultInspector();
			if (GUILayout.Button("Edit"))
			{
				TreeViewDataSourceWindow.Init();
			}
		}
	}
}
