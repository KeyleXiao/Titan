using UnityEditor;
using UnityEngine;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System;

[CustomEditor(typeof(SoldierCamera))]
class SoldierCameraInspector : Editor
{
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();


        SoldierCamera sc = (SoldierCamera)target;
        if (sc == null)
        {
            return;
        }


        if (sc.curCameraModeParam != null)
        {
            GUILayout.BeginVertical("Box");
            GUILayout.Label("Camera Param:");

            sc.curCameraModeParam.curDistance = EditorGUILayout.FloatField("默认摄像机距离:", sc.curCameraModeParam.curDistance);
            sc.curCameraModeParam.distanceRange = EditorGUILayout.Vector2Field("摄像机距离范围:", sc.curCameraModeParam.distanceRange);
            sc.curCameraModeParam.curPitchAngle = EditorGUILayout.FloatField("默认摄像机俯角:", sc.curCameraModeParam.curPitchAngle);
            sc.curCameraModeParam.pitchAngleRange = EditorGUILayout.Vector2Field("摄像机俯角范围:", sc.curCameraModeParam.pitchAngleRange);
            sc.curCameraModeParam.targetOffset = EditorGUILayout.FloatField("屏幕中心（焦点）的Y轴偏移:", sc.curCameraModeParam.targetOffset);          

            if (GUILayout.Button("应用"))
            {
                sc.curCameraModeParam.curDistance = Mathf.Clamp(sc.curCameraModeParam.curDistance, sc.curCameraModeParam.distanceRange.x, sc.curCameraModeParam.distanceRange.y);
                sc.userDesireDistance = sc.curCameraModeParam.curDistance;
                Vector3 angle = sc.userDesireAngle;
                sc.curCameraModeParam.curPitchAngle = Mathf.Clamp(sc.curCameraModeParam.curPitchAngle, sc.curCameraModeParam.pitchAngleRange.x, sc.curCameraModeParam.pitchAngleRange.y);
                angle.x = sc.curCameraModeParam.curPitchAngle;
                sc.userDesireAngle = angle;
            }

            GUILayout.EndVertical();
        }

        GUILayout.BeginVertical("Box");
        GUILayout.Label("Nav Operation:");

        GUILayout.BeginVertical("Box");
        if (GUILayout.Button("Load From File"))
        {
            string path = EditorUtility.OpenFilePanel("load camera nav file", Application.persistentDataPath, "csv");
            if (path.Length > 0)
            {
                sc.LoadNavInfoFromFile(path);
            }
        }
        if (GUILayout.Button("Save To File"))
        {
            string path = EditorUtility.SaveFilePanel("load camera nav file", Application.persistentDataPath, "cameranav","csv");
            if (path.Length > 0)
            {
                sc.SaveNavInfosToFile(path);
            }
        }
        GUILayout.EndVertical();
    }


}
