using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEditor;
using UnityEngine;

namespace USpeedUI.UEffect
{
    [CustomEditor(typeof(UPosEffectLayout))]
    class UPosEffectLayoutEditor : Editor
    {
        UPosEffectLayout selectPosEffectLayout;
        Rect MinVertical ;
        Rect MinHori;
        public virtual void OnEnable()
        {
            selectPosEffectLayout = (UPosEffectLayout)target;
        }
        public override void OnInspectorGUI()
        {
            selectPosEffectLayout.PosEffectType = (EPosEffectType)EditorGUILayout.EnumPopup("类型：", selectPosEffectLayout.PosEffectType);

            GUILayout.Space(10);
            GUILayout.BeginVertical("Box");
            MinVertical = EditorGUILayout.BeginVertical(GUILayout.MinHeight(60));

            if (selectPosEffectLayout.PosEffect != null)
            {
                switch (selectPosEffectLayout.PosEffect.GetPosEffectType())
                {
                    case EPosEffectType.PET_Curved:
                        OnInspectorGUI_Curved();
                        break;
                    case EPosEffectType.PET_Radial:
                        OnInspectorGUI_Radial();
                        break;
                    case EPosEffectType.PET_None:
                    case EPosEffectType.PET_Max:
                    default:
                        break;
                }
            }
            GUILayout.EndVertical();
            GUILayout.EndVertical();

            if (GUI.changed)
            {
                EditorUtility.SetDirty(target);
            }
        }
        protected void OnInspectorGUI_Curved()
        {
            UPosEffect_Curved curvedEffect = selectPosEffectLayout.PosEffect as UPosEffect_Curved;
            if(curvedEffect != null)
            {
                MinHori = GUILayoutUtility.GetRect(Mathf.Min(MinVertical.width, 60), 60);

                Rect curveRect = new Rect(MinHori.xMin, MinVertical.yMin, MinHori.width,50);
                
                curvedEffect.CurveForPosition = EditorGUI.CurveField(curveRect,"位置曲线", curvedEffect.CurveForPosition);
                curvedEffect.CurveHorizontalMultiplier = EditorGUILayout.FloatField("水平缩放", curvedEffect.CurveHorizontalMultiplier);//EditorGUI.FloatField(curveRect, "水平缩放", curvedEffect.CurveHorizontalMultiplier);             
                curvedEffect.CurveVerticalMultiplier = EditorGUILayout.FloatField("垂直缩放", curvedEffect.CurveVerticalMultiplier);
                //EditorGUI.FloatField(curveRect, "垂直缩放", curvedEffect.CurveVerticalMultiplier);
            }
        }
        protected void OnInspectorGUI_Radial()
        {
            UPosEffect_Radial radialEffect = selectPosEffectLayout.PosEffect as UPosEffect_Radial;
            if (radialEffect != null)
            {
                radialEffect.RadialRadius = EditorGUILayout.FloatField("辐射半径", radialEffect.RadialRadius);
                radialEffect.MinAngle = EditorGUILayout.FloatField("最小角度", radialEffect.MinAngle);
                radialEffect.MaxAngle = EditorGUILayout.FloatField("最大角度", radialEffect.MaxAngle);
                radialEffect.StartAngle = EditorGUILayout.FloatField("起始角度", radialEffect.StartAngle);
            }
        }
    }
}
