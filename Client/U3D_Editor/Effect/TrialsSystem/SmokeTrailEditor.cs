using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

namespace Effect
{
    namespace TrailsSystem
    {
        [CustomEditor(typeof(Effect_SmokeTrail))]
        [CanEditMultipleObjects]
        public class SmokeTrailEditor : TrailEditor_Base
        {
            protected override void DrawTrailSpecificGUI()
            {
                EditorGUILayout.PropertyField(serializedObject.FindProperty("RandomForceScale"));
                EditorGUILayout.PropertyField(serializedObject.FindProperty("MinVertexDistance"));
                EditorGUILayout.PropertyField(serializedObject.FindProperty("MaxNumberOfPoints"));
            }
        }
    }

}

