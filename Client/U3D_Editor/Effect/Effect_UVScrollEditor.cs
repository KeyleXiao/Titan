/*******************************************************************
** 文件名:	Effect_UVScrollEditor.cs
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
    [CustomEditor(typeof(Effect_UVScroll))]
    public class Effect_UVScrollEditor : Editor
    {

        Effect_UVScroll uvscroll;
        public void OnEnable()
        {
            uvscroll = target as Effect_UVScroll;
        }

        public override void OnInspectorGUI()
        {
            //uvscroll.isLoop = EditorGUILayout.Toggle("是否循环:", uvscroll.isLoop);
            //uvscroll.IncludeChildren = EditorGUILayout.Toggle("是否包括子节点:", uvscroll.IncludeChildren);
            EditorGUILayout.HelpBox("Effect_UVScroll脚本不能和Effect_UVTextureAnimator同时作用于同一张贴图", MessageType.Warning);

            uvscroll.RunMode = (EffectRunMode)EditorGUILayout.EnumPopup("运行模式:", uvscroll.RunMode);


            uvscroll.TexturePropertyName = EditorGUILayout.TextField("属性名:", uvscroll.TexturePropertyName);
            if(uvscroll.RunMode == EffectRunMode.Self)
            {
                uvscroll.UseSharedMaterial = EditorGUILayout.Toggle("使用共享材质:", uvscroll.UseSharedMaterial);
            }
            
            uvscroll.UseModelUV = EditorGUILayout.Toggle("使用模型UV:", uvscroll.UseModelUV);
            uvscroll.fStartDelay = EditorGUILayout.FloatField("延时开始(小于0.03无延时):", uvscroll.fStartDelay);
            uvscroll.BetweenTimes = EditorGUILayout.Vector2Field("两次流动间隔(小于0.03无间隔):", uvscroll.BetweenTimes);
            uvscroll.ScrollSpeed = EditorGUILayout.Vector2Field("流动速度:", uvscroll.ScrollSpeed);
            uvscroll.ScrollXTime = EditorGUILayout.FloatField("X流动时间(小于等于0不流动):", uvscroll.ScrollXTime);
            uvscroll.ScrollYTime = EditorGUILayout.FloatField("Y流动时间(小于等于0不流动):", uvscroll.ScrollYTime);

            uvscroll.RotateSpeed = EditorGUILayout.Vector3Field("旋转速度:", uvscroll.RotateSpeed);
            uvscroll.RotateXTime = EditorGUILayout.FloatField("X旋转时间(小于等于0不转):", uvscroll.RotateXTime);
            uvscroll.RotateYTime = EditorGUILayout.FloatField("Y旋转时间(小于等于0不转):", uvscroll.RotateYTime);
            uvscroll.RotateZTime = EditorGUILayout.FloatField("Z旋转时间(小于等于0不转):", uvscroll.RotateZTime);
            uvscroll.RoateInWorldSpace = EditorGUILayout.Toggle("是否在世界空间旋转:", uvscroll.RoateInWorldSpace);


            if (GUILayout.Button("Play"))
            {
                uvscroll.Play();
            }

            if (GUILayout.Button("Stop"))
            {
                uvscroll.Stop();
            }
            if (GUILayout.Button("Reset"))
            {
                uvscroll.ResetValue();
            }

            if (GUI.changed)
            {
                EditorUtility.SetDirty(uvscroll);
            }
        }
    }
}

