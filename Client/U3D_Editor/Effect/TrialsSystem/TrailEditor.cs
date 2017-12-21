using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

namespace Effect
{
    namespace TrailsSystem
    {
        [CustomEditor(typeof(Effect_Trail))]
        [CanEditMultipleObjects]
        public class TrailEditor : TrailEditor_Base
        {
            protected override void DrawTrailSpecificGUI()
            {
                EditorGUILayout.PropertyField(serializedObject.FindProperty("MinVertexDistance"));
                EditorGUILayout.PropertyField(serializedObject.FindProperty("MaxNumberOfPoints"));
            }
        }
    }

}

