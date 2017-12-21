//using UnityEditor;
//using UnityEngine;

//namespace Effect
//{
//    namespace TrailsSystem
//    {
//        [CustomPropertyDrawer(typeof(Range))]
//        public class RangePropertyDrawer : PropertyDrawer
//        {
//            public override void OnGUI(UnityEngine.Rect position, SerializedProperty property, UnityEngine.GUIContent label)
//            {
//                SerializedProperty Min = property.FindPropertyRelative("Min");
//                SerializedProperty Max = property.FindPropertyRelative("Max");
//                Vector2 newMinMax = Vector2.zero;
//                position.height = 16;
//                EditorGUI.LabelField(position, label);
//                position.y += 20;
//                position.width = position.width / 2;
//                newMinMax.x = EditorGUI.FloatField(position, "Min", Min.floatValue);
//                position.x += position.width;
//                newMinMax.y = EditorGUI.FloatField(position, "Max", Max.floatValue);

//                if (newMinMax.x > newMinMax.y)
//                    newMinMax.x = newMinMax.y;

//                Min.floatValue = newMinMax.x;
//                Max.floatValue = newMinMax.y;
//            }

//            public override float GetPropertyHeight(SerializedProperty property, GUIContent label)
//            {
//                return base.GetPropertyHeight(property, label) + 20;
//            }
//        }
//    }
//}

