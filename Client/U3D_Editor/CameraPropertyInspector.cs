using UnityEditor;
using UnityEngine;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System;

[CustomEditor(typeof(CameraProperty))]
class CameraPropertyInspector : Editor
{
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();
        if (GUILayout.Button("Reset"))
        {
            if (SoldierCamera.MainInstance<SoldierCamera>() != null)
            {
                SoldierCamera.MainInstance<SoldierCamera>().Reset(true);
                Debug.Log("Camera reseted");
            }
        }
    }


}
