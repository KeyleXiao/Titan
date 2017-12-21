/*******************************************************************
** 文件名:	ListViewPaginatorEditor.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-7-1
** 版  本:	1.8.0
** 描  述:	
		
********************************************************************/
using UnityEngine;
using UnityEditor;
using System.Collections.Generic;
using System;

namespace UIWidgets {
	[CustomEditor(typeof(ListViewPaginator), true)]
	[CanEditMultipleObjects]
	public class ListViewPaginatorEditor : Editor {

		Dictionary<string,SerializedProperty> Properties = new Dictionary<string,SerializedProperty>();
		string[] properties = new string[]{
			"ListView",
			"perPage",

			"DefaultPage",
			"ActivePage",
			"PrevPage",
			"NextPage",
			
			"currentPage",
			"ForceScrollOnPage",
			
			"Animation",
			"Movement",
			"UnscaledTime",

			"OnPageSelect",
		};

        public virtual void OnEnable()
		{
			Properties.Clear();

			Array.ForEach(properties, x => Properties.Add(x, serializedObject.FindProperty(x)));
		}
		
		public override void OnInspectorGUI()
		{
			serializedObject.Update();

			EditorGUILayout.PropertyField(Properties["ListView"], true);
			EditorGUILayout.PropertyField(Properties["perPage"], true);

			EditorGUILayout.PropertyField(Properties["DefaultPage"], true);
			EditorGUILayout.PropertyField(Properties["ActivePage"], true);
			EditorGUILayout.PropertyField(Properties["PrevPage"], true);
			EditorGUILayout.PropertyField(Properties["NextPage"], true);

			EditorGUILayout.PropertyField(Properties["currentPage"], true);
			EditorGUILayout.PropertyField(Properties["ForceScrollOnPage"], true);

			EditorGUILayout.PropertyField(Properties["Animation"], true);
			EditorGUI.indentLevel++;
			if (Properties["Animation"].boolValue)
			{
				EditorGUILayout.PropertyField(Properties["Movement"], true);
				EditorGUILayout.PropertyField(Properties["UnscaledTime"], true);
			}
			EditorGUI.indentLevel--;

			EditorGUILayout.PropertyField(Properties["OnPageSelect"], true);

			serializedObject.ApplyModifiedProperties();
		}
	}
}
