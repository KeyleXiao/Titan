using UnityEditor;
using UnityEngine;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System;

[CustomEditor(typeof(CombinedBone))]
public class CombinedBoneInspector : Editor
{
    public override void OnInspectorGUI()
    {
        //base.OnInspectorGUI();

        CombinedBone cb = (CombinedBone)target;


        GUILayout.BeginVertical("Box");

        GUILayout.BeginHorizontal("Box");
        cb.poslock = EditorGUILayout.Toggle("position lock", cb.poslock);
        GUILayout.EndHorizontal();
        if (GUILayout.Button("Apply Position Lock to Childrens"))
        {
            cb.SetPositionLockRecursive(cb.poslock);
        }

        GUILayout.BeginHorizontal("Box");
        cb.posweight = EditorGUILayout.Slider("position weight", cb.posweight, 0.0f, 1.0f);
        GUILayout.EndHorizontal();
        if (GUILayout.Button("Apply Position Weight to Childrens"))
        {
            cb.SetPositionWeightRecursive(cb.posweight);
        }

        GUILayout.BeginHorizontal("Box");
        cb.rotationlock = EditorGUILayout.Toggle("rotation lock", cb.rotationlock);
        GUILayout.EndHorizontal();
        if (GUILayout.Button("Apply Position Lock to Childrens"))
        {
            cb.SetRotationLockRecursive(cb.rotationlock);
        }

        GUILayout.BeginHorizontal("Box");
        cb.rotationweight = EditorGUILayout.Slider("rotation weight", cb.rotationweight, 0.0f, 1.0f);
        GUILayout.EndHorizontal();
        if (GUILayout.Button("Apply Rotation Weight to Childrens"))
        {
            cb.SetRotationWeightRecursive(cb.rotationweight);
        }

        GUILayout.BeginHorizontal("Box");
        cb.scalelock = EditorGUILayout.Toggle("scale lock", cb.scalelock);
        GUILayout.EndHorizontal();
        if (GUILayout.Button("Apply Scale Lock to Childrens"))
        {
            cb.SetScaleLockRecursive(cb.scalelock);
        }

        GUILayout.BeginHorizontal("Box");
        cb.scaleweight = EditorGUILayout.Slider("scale weight", cb.scaleweight, 0.0f, 1.0f);
        GUILayout.EndHorizontal();
        if (GUILayout.Button("Apply Rotation Weight to Childrens"))
        {
            cb.SetScaleWeightRecursive(cb.scaleweight);
        }

        GUILayout.EndVertical();
    }
}