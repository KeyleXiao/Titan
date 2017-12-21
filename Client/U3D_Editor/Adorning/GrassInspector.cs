using UnityEditor;
using UnityEngine;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System;

//[CustomEditor(typeof(GrassAdorning))]
//public class GrassInspector : Editor
//{
//    float amplitude = 45.0f;
//    public override void OnInspectorGUI()
//    {
//        base.OnInspectorGUI();

//        GUILayout.BeginVertical("Box");
//        GUILayout.Label("Grass Operations:");

//        GrassAdorning grass = (GrassAdorning)target;
//        if (grass == null)
//        {
//            return;
//        }

//        GUILayout.BeginHorizontal("Box");
//        amplitude = EditorGUILayout.Slider("amplitude", amplitude, 0, 60.0f);
//        GUILayout.EndHorizontal();

//        GUILayout.BeginHorizontal("Box");
//        if (GUILayout.Button("Start"))
//        {
//            grass.StartAction(amplitude);
//        }
//        if (GUILayout.Button("Stop"))
//        {
//            grass.StopAction();
//        }
//        GUILayout.EndHorizontal();


//        GUILayout.EndVertical();
//    }


//}


