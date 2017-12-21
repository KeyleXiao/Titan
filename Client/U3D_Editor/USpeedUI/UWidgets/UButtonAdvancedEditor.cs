using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEditor;
using UnityEditor.UI;
using UnityEngine;

namespace USpeedUI.UWidgets
{
    [CustomEditor(typeof(UButtonAdvanced), true)]
    [CanEditMultipleObjects]
    public class UButtonAdvancedEditor : SelectableEditor
    {
        protected Dictionary<string, SerializedProperty> SerializedProperties = new Dictionary<string, SerializedProperty>();
        protected Dictionary<string, SerializedProperty> SerializedEvents = new Dictionary<string, SerializedProperty>();
        public bool ShowEvents;

        protected List<string> Properties = new List<string>{
            "IsCanBubbleEvent",
        };

        protected List<string> Events = new List<string>{
            "onPointerDown",
            "onPointerUp",
            "onPointerEnter",
            "onPointerExit",
            "m_OnClick",
        };

        protected override void OnEnable()
        {
            base.OnEnable();

            Properties.ForEach(x =>
            {
                var property = serializedObject.FindProperty(x);
                if (property != null)
                {
                    SerializedProperties.Add(x, property);
                }
            });
            Events.ForEach(x =>
            {
                var property = serializedObject.FindProperty(x);
                if (property != null)
                {
                    SerializedEvents.Add(x, property);
                }
            });
        }
        public override void OnInspectorGUI()
        {
            base.OnInspectorGUI();

            serializedObject.Update();

            SerializedProperties.ForEach(x => EditorGUILayout.PropertyField(x.Value, true));

            EditorGUILayout.BeginVertical();
            ShowEvents = GUILayout.Toggle(ShowEvents, "Events", "Foldout", GUILayout.ExpandWidth(true));
            if (ShowEvents)
            {
                SerializedEvents.ForEach(x => EditorGUILayout.PropertyField(x.Value, true));
            }
            EditorGUILayout.EndVertical();

            serializedObject.ApplyModifiedProperties();
        }


    }
}
