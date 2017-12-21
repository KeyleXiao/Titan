/*******************************************************************
** 文件名:	Effect_Random3DEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/6/9
** 版  本:	1.0
** 描  述:	Effect_UVScroll编辑器脚本
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
namespace Effect
{
    [CustomEditor(typeof(Effect_CameraAnimation))]
    public class Effect_CameraAnimationEditor : Editor
    {

        Effect_CameraAnimation instance;

        SerializedProperty CameraAxisProp = null;
        public void OnEnable()
        {
            instance = target as Effect_CameraAnimation;
            CameraAxisProp = serializedObject.FindProperty("cameraAxis");
        }

        Vector2 scrollPos = Vector2.zero;
        public override void OnInspectorGUI()
        {
            // 显示名字
            scrollPos = EditorGUILayout.BeginScrollView(scrollPos);

            EditorGUILayout.BeginVertical();
            EditorGUILayout.LabelField("以动画形式制作的镜头动画效果");
            instance.animationClip = EditorGUILayout.ObjectField(instance.animationClip, typeof(AnimationClip), false) as AnimationClip;
            EditorGUILayout.EndVertical();

            EditorGUILayout.BeginVertical();
            EditorGUI.BeginChangeCheck();
            EditorGUILayout.LabelField("摄像机移动关键帧节点，可以有多个，展开后配置EffectCameraNode");
            EditorGUILayout.PropertyField(CameraAxisProp, true);
            if(EditorGUI.EndChangeCheck())
            {
                serializedObject.ApplyModifiedProperties();
            }
            EditorGUILayout.EndVertical();
            EditorGUILayout.EndScrollView();
        }
    }
}

