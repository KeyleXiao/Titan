using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using UnityEditor;
using UnityEditor.UI;
using UnityEditorInternal;

[CustomEditor(typeof(URendererDepth), true)]
[CanEditMultipleObjects]
public class URendererDepthEditor : SelectableEditor
{
    URendererDepth targerURendererDepth;
    string[] SortingLayerName;
    bool m_bShowSortingLayer = false;

    SerializedProperty m_orderInLayerProperty;
    SerializedProperty m_sortingLayerNameProperty;
    SerializedProperty m_bIsUIElememtProperty;
    SerializedProperty m_bIsRequirRaycasterProperty;


    protected override void OnEnable()
    {
        //base.OnEnable();

        m_orderInLayerProperty = serializedObject.FindProperty("m_orderInLayer");
        m_sortingLayerNameProperty = serializedObject.FindProperty("m_sortingLayerName");
        m_bIsUIElememtProperty = serializedObject.FindProperty("m_bIsUIElememt");
        m_bIsRequirRaycasterProperty = serializedObject.FindProperty("m_bIsRequirRaycaster");


        targerURendererDepth = target as URendererDepth;
        SortingLayerName = GetSortingLayerNames();
    }

    public override void OnInspectorGUI()
    {
        //base.OnInspectorGUI();
        EditorGUILayout.Space();
        targerURendererDepth.IsUIElememt = EditorGUILayout.Toggle("是否是UI控件", targerURendererDepth.IsUIElememt);
        if(targerURendererDepth.IsUIElememt)
        {
            targerURendererDepth.IsRequirRaycaster = EditorGUILayout.Toggle("是否需要UI射线检测", targerURendererDepth.IsRequirRaycaster);
        }
        EditorGUILayout.Space();
        targerURendererDepth.OrderInLayer = EditorGUILayout.IntField("在指定显示层的显示顺序", targerURendererDepth.OrderInLayer);
        var LayerIndex = Array.IndexOf(SortingLayerName, targerURendererDepth.SortingLayerName);
        if(m_bShowSortingLayer = EditorGUILayout.Toggle("是否是要更改显示层级", m_bShowSortingLayer))
            targerURendererDepth.SortingLayerName = SortingLayerName[ EditorGUILayout.Popup("显示层", LayerIndex, SortingLayerName)];
    }

    public string[] GetSortingLayerNames()
    {
        Type internalEditorUtilityType = typeof(InternalEditorUtility);
        PropertyInfo sortingLayersProperty = internalEditorUtilityType.GetProperty("sortingLayerNames", BindingFlags.Static | BindingFlags.NonPublic);
        return (string[])sortingLayersProperty.GetValue(null, new object[0]);
    }
}
