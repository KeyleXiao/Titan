using UnityEditor;
using UnityEngine;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System;

[CustomEditor(typeof(AdorningManager))]
public class BattleAdorningInspector : Editor
{
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        GUILayout.BeginVertical("Box");
        GUILayout.Label("Adornings Operations:");

        AdorningManager am = (AdorningManager)target;
        if (am == null)
        {
            return;
        }

        GUILayout.BeginVertical("Box");
        GUILayout.Label("Adornings Parameters:");
        GUILayout.BeginHorizontal("Box");
        am.VisFactor = EditorGUILayout.Slider("VisFactor", am.VisFactor, 0.0f, 1.0f);
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal("Box");
        am.FadeTime = EditorGUILayout.Slider("FadeTime", am.FadeTime, 0.0f, 1.0f);
        GUILayout.EndHorizontal();


        GUILayout.EndVertical();
    }


}


