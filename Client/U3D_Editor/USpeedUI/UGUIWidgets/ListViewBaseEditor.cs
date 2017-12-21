/*******************************************************************
** 文件名:	ListViewBaseEditor.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	
		
********************************************************************/
using UnityEditor;
using System.Collections.Generic;
using System;

namespace UIWidgets
{
	[CanEditMultipleObjects]
	//[CustomEditor(typeof(ListViewBase), false)]
	public class ListViewBaseEditor : Editor
	{
		Dictionary<string,SerializedProperty> serializedProperties = new Dictionary<string,SerializedProperty>();
		
		string[] properties = new string[]{
			"items",
			"DestroyGameObjects",
			"Multiple",
			"selectedIndex",

			"Container",
		};

        public virtual void OnEnable()
		{
			Array.ForEach(properties, x => {
				serializedProperties.Add(x, serializedObject.FindProperty(x));
			});
		}
		
		public override void OnInspectorGUI()
		{
			serializedObject.Update();

			EditorGUILayout.PropertyField(serializedProperties["items"], true);
			EditorGUILayout.PropertyField(serializedProperties["DestroyGameObjects"]);
			EditorGUILayout.PropertyField(serializedProperties["Multiple"]);
			EditorGUILayout.PropertyField(serializedProperties["selectedIndex"]);
			EditorGUILayout.PropertyField(serializedProperties["Container"]);

			serializedObject.ApplyModifiedProperties();

			//Array.ForEach(targets, x => ((ListViewBase)x).UpdateItems());
		}
	}
}
