using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEditor;
using USpeedUI.WarGoodsStore;

[CustomEditor(typeof(UWarGoodsStoreRecommendComponent), true)]
[CanEditMultipleObjects]
public class UWarGoodsStoreRecommendComponentEditor : UButtonItemEditor
{
    SerializedProperty m_GoodsIconProperty;
    SerializedProperty m_GoodsCostProperty;
    SerializedProperty m_GoodsSelectedFrameProperty;
    SerializedProperty m_GoodsTooltipsProperty;

    SerializedProperty m_GoodsTooltip_IconParamNameProperty;
    SerializedProperty m_GoodsTooltip_CostParamNameProperty;
    SerializedProperty m_GoodsTooltip_NameParamNameProperty;
    SerializedProperty m_GoodsTooltip_InfoParamNameProperty;


    public override void OnInspectorGUI()
    {
        EditorGUILayout.Space();
        serializedObject.Update();
        EditorGUILayout.PropertyField(m_GoodsIconProperty);
        EditorGUILayout.PropertyField(m_GoodsCostProperty);
        EditorGUILayout.PropertyField(m_GoodsSelectedFrameProperty);
        EditorGUILayout.PropertyField(m_GoodsTooltipsProperty);
        EditorGUILayout.Space();
        EditorGUILayout.PropertyField(m_GoodsTooltip_IconParamNameProperty);
        EditorGUILayout.PropertyField(m_GoodsTooltip_CostParamNameProperty);
        EditorGUILayout.PropertyField(m_GoodsTooltip_NameParamNameProperty);
        EditorGUILayout.PropertyField(m_GoodsTooltip_InfoParamNameProperty);
        serializedObject.ApplyModifiedProperties();

        base.OnInspectorGUI();

    }
    protected override void OnEnable()
    {
        m_GoodsIconProperty = serializedObject.FindProperty("GoodsIcon");
        m_GoodsCostProperty = serializedObject.FindProperty("GoodsCost");
        m_GoodsSelectedFrameProperty = serializedObject.FindProperty("GoodsSelectedFrame");
        m_GoodsTooltipsProperty = serializedObject.FindProperty("GoodsTooltips");

        m_GoodsTooltip_IconParamNameProperty = serializedObject.FindProperty("GoodsTooltip_IconParamName");
        m_GoodsTooltip_CostParamNameProperty = serializedObject.FindProperty("GoodsTooltip_CostParamName");
        m_GoodsTooltip_NameParamNameProperty = serializedObject.FindProperty("GoodsTooltip_NameParamName");
        m_GoodsTooltip_InfoParamNameProperty = serializedObject.FindProperty("GoodsTooltip_InfoParamName");

        base.OnEnable();
    }
}
