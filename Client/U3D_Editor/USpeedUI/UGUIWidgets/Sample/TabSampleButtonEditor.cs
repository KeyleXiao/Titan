using UnityEngine;
using UnityEditor;
using UnityEditor.UI;
using System;
using System.Collections.Generic;
using UIWidgets;

namespace UIWidgetsSamples
{
    [CustomEditor(typeof(TabSampleButton),true)]
    [CanEditMultipleObjects]
    public class TabSampleButtonEditor : ButtonEditor
    {
        Dictionary<string, SerializedProperty> serializedProperties = new Dictionary<string, SerializedProperty>();

        string[] properties = new string[]{
            "Name",
        };
        protected override void OnEnable()
        {
            Array.ForEach(properties, x => serializedProperties.Add(x, serializedObject.FindProperty(x)));

            base.OnEnable();
        }

        public override void OnInspectorGUI()
        {
            serializedProperties.ForEach(x => EditorGUILayout.PropertyField(x.Value));
            serializedObject.ApplyModifiedProperties();

            base.OnInspectorGUI();
        }
    }
}
