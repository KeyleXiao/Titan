using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

namespace Effect
{
    namespace TrailsSystem
    {
        [CustomEditor(typeof(Effect_SmokePlume))]
        [CanEditMultipleObjects]
        public class SmokePlumeEditor : TrailEditor_Base
        {
            protected override void DrawTrailSpecificGUI()
            {
                EditorGUILayout.PropertyField(serializedObject.FindProperty("ConstantForce"));
                EditorGUILayout.PropertyField(serializedObject.FindProperty("RandomForceScale"));
                EditorGUILayout.PropertyField(serializedObject.FindProperty("TimeBetweenPoints"));
                EditorGUILayout.PropertyField(serializedObject.FindProperty("MaxNumberOfPoints"));
            }
        }
    }

}

