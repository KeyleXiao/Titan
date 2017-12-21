using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEditor.UI;
using USpeedUI.WarFloatFont;


[CustomEditor(typeof(WarFloatFontAnim), true)]
public class WarFloatFontAnimEditor :Editor
{
    private WarFloatFontAnim m_Target;
    private static int animLength = (int)WarFloatFontType.Max - 1;

    protected virtual void OnEnable()
    {
        m_Target = target as WarFloatFontAnim;
    }

    protected bool[] showAnim = new bool[animLength];

    public override void OnInspectorGUI()
    {
        //base.OnInspectorGUI();
        if (null != m_Target)
        {
            for (int i = 0; i < animLength; i++)
            {
                EditorGUILayout.BeginVertical();
                showAnim[i] = GUILayout.Toggle(showAnim[i], ((WarFloatFontType)(i + 1)).ToString(), "Foldout", GUILayout.ExpandWidth(true));
                if (showAnim[i])
                {
                    m_Target.m_FontAnimList[i].fontType = (WarFloatFontType)EditorGUILayout.EnumPopup("飘字类型", m_Target.m_FontAnimList[i].fontType);
                    //m_Target.m_FontAnimList[i].fontColor = EditorGUILayout.ColorField("飘字颜色:", m_Target.m_FontAnimList[i].fontColor);
                    m_Target.m_FontAnimList[i].fontSize = Mathf.Abs(EditorGUILayout.IntField("飘字起始大小:", m_Target.m_FontAnimList[i].fontSize));
                    m_Target.m_FontAnimList[i].expandTime = Mathf.Abs(EditorGUILayout.FloatField("膨胀时间(秒):", m_Target.m_FontAnimList[i].expandTime));
                    m_Target.m_FontAnimList[i].stagnationTime = Mathf.Abs(EditorGUILayout.FloatField("停滞时间(秒):", m_Target.m_FontAnimList[i].stagnationTime));
                    m_Target.m_FontAnimList[i].disappearTime = Mathf.Abs(EditorGUILayout.FloatField("消失时间(秒):", m_Target.m_FontAnimList[i].disappearTime));
                    m_Target.m_FontAnimList[i].topColor = EditorGUILayout.ColorField("渐变顶部颜色:", m_Target.m_FontAnimList[i].topColor);
                    m_Target.m_FontAnimList[i].bottomColor = EditorGUILayout.ColorField("渐变底部颜色:", m_Target.m_FontAnimList[i].bottomColor);
                    m_Target.m_FontAnimList[i].startXOffset = EditorGUILayout.FloatField("起始X轴偏移(像素):", m_Target.m_FontAnimList[i].startXOffset);
                    m_Target.m_FontAnimList[i].startYOffset = EditorGUILayout.FloatField("起始Y轴偏移(像素):", m_Target.m_FontAnimList[i].startYOffset);
                    m_Target.m_FontAnimList[i].xOffset = EditorGUILayout.FloatField("最终X偏移值(像素):", m_Target.m_FontAnimList[i].xOffset);
                    m_Target.m_FontAnimList[i].yOffset = EditorGUILayout.FloatField("最终Y偏移值(像素):", m_Target.m_FontAnimList[i].yOffset);
                    m_Target.m_FontAnimList[i].expandMinPercent = EditorGUILayout.FloatField("膨胀最小比例:", m_Target.m_FontAnimList[i].expandMinPercent);
                    m_Target.m_FontAnimList[i].expandMaxPercent = EditorGUILayout.FloatField("膨胀最大比例:", m_Target.m_FontAnimList[i].expandMaxPercent);
                    m_Target.m_FontAnimList[i].jumpPower = EditorGUILayout.FloatField("跳跃力度:",m_Target.m_FontAnimList[i].jumpPower);
                    m_Target.m_FontAnimList[i].bRandomStartPos = EditorGUILayout.Toggle("是否随机X轴初始位置:", m_Target.m_FontAnimList[i].bRandomStartPos);
                }
                EditorGUILayout.EndVertical();
            }
        }
        if (GUI.changed)
        {
            EditorUtility.SetDirty(m_Target);
        }
    }
}

