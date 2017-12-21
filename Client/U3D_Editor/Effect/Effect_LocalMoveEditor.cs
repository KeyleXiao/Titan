using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

namespace Effect
{
    [CustomEditor(typeof(Effect_LocalMove))]
    public class Effect_LocalMoveEditor : Editor
    {
        Effect_LocalMove localMove;

        public void OnEnable()
        {
            localMove = target as Effect_LocalMove;
        }

        public override void OnInspectorGUI()
        {
            localMove.startDelay = EditorGUILayout.FloatField("延迟时间:", localMove.startDelay);
            localMove.moveTime = EditorGUILayout.FloatField("持续时间:", localMove.moveTime);

            Rect Vertical = EditorGUILayout.BeginVertical(GUILayout.MinHeight(35));
            localMove.bEnableX = EditorGUILayout.Toggle("启用X轴:", localMove.bEnableX);
            if (localMove.bEnableX)
            {
                Rect curveRect = new Rect(Vertical.xMin + 5, Vertical.yMin + 18, Vertical.width * 0.9f, 15);
                localMove.xPath = EditorGUI.CurveField(curveRect, "X轴轨迹", localMove.xPath);
            }
            GUILayout.EndVertical();

            Rect Vertical2 = EditorGUILayout.BeginVertical(GUILayout.MinHeight(35));
            localMove.bEnableY = EditorGUILayout.Toggle("启用Y轴:", localMove.bEnableY);
            if (localMove.bEnableY)
            {
                Rect curveRect = new Rect(Vertical2.xMin + 5, Vertical2.yMin + 18, Vertical2.width * 0.9f, 15);
                localMove.yPath = EditorGUI.CurveField(curveRect, "Y轴轨迹", localMove.yPath);
            }
            GUILayout.EndVertical();

            Rect Vertical3 = EditorGUILayout.BeginVertical(GUILayout.MinHeight(35));
            localMove.bEnableZ = EditorGUILayout.Toggle("启用Z轴:", localMove.bEnableZ);
            if (localMove.bEnableZ)
            {
                Rect curveRect = new Rect(Vertical3.xMin + 5, Vertical3.yMin + 18, Vertical3.width * 0.9f, 15);
                localMove.zPath = EditorGUI.CurveField(curveRect, "Z轴轨迹", localMove.zPath);
            }
            GUILayout.EndVertical();

            if (GUILayout.Button("Play"))
            {
                localMove.Play();
            }
            if (GUILayout.Button("Stop"))
            {
                localMove.Stop();
            }

            if (GUI.changed)
            {
                EditorUtility.SetDirty(localMove);
            }
        }
    }
}
