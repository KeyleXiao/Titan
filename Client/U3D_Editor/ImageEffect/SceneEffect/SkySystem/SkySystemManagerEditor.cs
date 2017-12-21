/*******************************************************************
** 文件名:	SkySystemManagerEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/11/3
** 版  本:	1.0
** 描  述:	天空系统
** 应  用:  	天空系统--管理器编辑器脚本

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Reflection;
using UnityEditor;

namespace SkySystem
{
    [CustomEditor(typeof(SkySystemManager))]
    public class SkySystemManagerEditor : Editor
    {

        SkySystemManager m_Manager = null;
        SerializedProperty NightZenithColorProperty = null;
        public void OnEnable()
        {
            m_Manager = target as SkySystemManager;
            NightZenithColorProperty = serializedObject.FindProperty("NightZenithColor");
        }

       // bool baseSettings = false;
        //bool sunSettings = false;
       // bool moonSettigns = false;
        public override void OnInspectorGUI()
        {
            //baseSettings = EditorGUILayout.Foldout(baseSettings,"基础设置");
            EditorGUILayout.LabelField("基础设置:");
            //if(baseSettings)
            {
                m_Manager.SkyboxMaterial = EditorGUILayout.ObjectField("天空盒材质(必须用指定的shader)",m_Manager.SkyboxMaterial,typeof(Material),false) as Material;
                m_Manager.SkyUpdate = EditorGUILayout.Toggle("是否更新:",m_Manager.SkyUpdate);
                m_Manager.Timeline = EditorGUILayout.Slider("当前时间(24小时):",m_Manager.Timeline,0.0f,24.0f);
                m_Manager.Exposure = EditorGUILayout.Slider("整体亮度缩放(提高所有光的亮度):", m_Manager.Exposure, 0.0f, 5.0f);
                m_Manager.Longitude = EditorGUILayout.Slider("水平角度:", m_Manager.Longitude, -180.0f, 180.0f);
                m_Manager.SkyTint = EditorGUILayout.ColorField("Sky Tint(与波长结合，对最后的可见光颜色进行微调，会同时影响天空和物体的颜色)", m_Manager.SkyTint);
                m_Manager.Wavelengths = EditorGUILayout.Vector3Field("可见光波长(不同的可见光会有不同的颜色，与SkyTint一起,对最后的可见光进行微调，会同时影响天空和物体的颜色):", m_Manager.Wavelengths);
                m_Manager.m_GroundColor = EditorGUILayout.ColorField("天空底部颜色，如无特殊需求，默认",m_Manager.m_GroundColor);
                m_Manager.LinearSpace = EditorGUILayout.Toggle("是否是线性色彩空间:", m_Manager.LinearSpace);
            }

            EditorGUILayout.Space();

           // sunSettings = EditorGUILayout.Foldout(sunSettings,"太阳设置");
            EditorGUILayout.LabelField("太阳设置:");
           // if(sunSettings)
            {
                m_Manager.m_sunLight = EditorGUILayout.ObjectField("太阳(一个平行光):",m_Manager.m_sunLight,typeof(GameObject),true) as GameObject;
                m_Manager.RayleighScattering = EditorGUILayout.Slider("瑞利散射(指8km高空微粒散射现象,可以模拟出真实的地球天空，日落时地平面上的红/黄色,增强晴天，旁晚的天空效果):",m_Manager.RayleighScattering,0.0f,5.0f);
                m_Manager.MieScattering = EditorGUILayout.Slider("米氏散射(指1.2km低空微粒的散射现象,在雾气比较大的情况下，会产生围绕太阳的光环，配合fog使用,增强雾天天气效果)",m_Manager.MieScattering,0.0f,5.0f);
                m_Manager.SunAnisotropyFactor = EditorGUILayout.Slider("太阳各项异性(控制太阳光斑的大小):",m_Manager.SunAnisotropyFactor,0.0f,0.9995f);
                m_Manager.SunSize = EditorGUILayout.FloatField("太阳尺寸:",m_Manager.SunSize);
                
            }

            EditorGUILayout.Space();

            //moonSettigns = EditorGUILayout.Foldout(moonSettigns,"月亮设置");
            EditorGUILayout.LabelField("月亮设置:");
            //if(moonSettigns)
            {
                m_Manager.m_moonLight = EditorGUILayout.ObjectField("月亮(一盏平行光):",m_Manager.m_moonLight,typeof(GameObject),true) as GameObject;
                m_Manager.NightSky = (SkySystemManager.NightModes)EditorGUILayout.EnumPopup("夜晚类型:", m_Manager.NightSky);
                EditorGUI.BeginChangeCheck();
                EditorGUILayout.PropertyField(NightZenithColorProperty,new GUIContent("天空中部颜色:","夜晚天空中部的颜色"),false);
                if(EditorGUI.EndChangeCheck())
                {
                    serializedObject.ApplyModifiedProperties();
                }

                m_Manager.NightTintColor = EditorGUILayout.ColorField("夜晚TintColor:", m_Manager.NightTintColor);
                m_Manager.StarIntensity = EditorGUILayout.Slider("星星密度:",m_Manager.StarIntensity,0.0f,5.0f);
                m_Manager.OuterSpaceIntensity = EditorGUILayout.Slider("外空间强度(OuterSpaceCubeMap):",m_Manager.OuterSpaceIntensity,0.0f,2.0f);
                m_Manager.MoonInnerCorona = EditorGUILayout.ColorField("月亮内部月晕颜色(alpha控制晕的大小):",m_Manager.MoonInnerCorona);
                m_Manager.MoonOuterCorona = EditorGUILayout.ColorField("月亮外部月晕颜色(alpha控制晕的大小):",m_Manager.MoonOuterCorona);
                m_Manager.MoonSize = EditorGUILayout.FloatField("月亮尺寸:",m_Manager.MoonSize);
            }
        }
    }
}
