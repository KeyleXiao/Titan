using UnityEngine;
using System.Collections;
using UnityEditor.UI;
using UnityEditor;

[CustomEditor(typeof(UButtonGroupItem), true)]
[CanEditMultipleObjects]

public class UButtonItemEditor : SelectableEditor
{
    SerializedProperty m_SelectedProperty;
    SerializedProperty m_ToggleProperty;
    SerializedProperty m_GroupProperty;
    SerializedProperty m_OnButtonItemSelectedProperty;
    SerializedProperty m_OnButtonItemSelectedExProperty;
    SerializedProperty m_GroupIndexProperty;

    protected override void OnEnable()
    {
        base.OnEnable();
        m_SelectedProperty = serializedObject.FindProperty("m_Selected");
        m_ToggleProperty = serializedObject.FindProperty("m_Toggle");
        m_GroupProperty = serializedObject.FindProperty("m_Group");
        m_OnButtonItemSelectedProperty = serializedObject.FindProperty("onSelectedChanged");
        m_OnButtonItemSelectedExProperty = serializedObject.FindProperty("onSelectedChangedEx");
        m_GroupIndexProperty = serializedObject.FindProperty("GroupIndex");
    }
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        EditorGUILayout.Space();
        serializedObject.Update();
        EditorGUILayout.PropertyField(m_SelectedProperty);
        EditorGUILayout.PropertyField(m_ToggleProperty);
        EditorGUILayout.PropertyField(m_GroupProperty);
        EditorGUILayout.PropertyField(m_GroupIndexProperty);
        EditorGUILayout.Space();

        EditorGUILayout.PropertyField(m_OnButtonItemSelectedProperty);
        EditorGUILayout.PropertyField(m_OnButtonItemSelectedExProperty);
        
        serializedObject.ApplyModifiedProperties();

    }

}
