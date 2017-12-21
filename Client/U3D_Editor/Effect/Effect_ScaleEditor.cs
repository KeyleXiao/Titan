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
    [CustomEditor(typeof(Effect_Scale))]
    public class Effect_ScaleEditor : Editor
    {

        Effect_Scale effectScale;
        public void OnEnable()
        {
            effectScale = target as Effect_Scale;
        }

        public override void OnInspectorGUI()
        {
            GUI.enabled = !Application.isPlaying;
            effectScale.scaleType = (ScaleType)EditorGUILayout.EnumPopup("缩放类型:", effectScale.scaleType);
            GUI.enabled = true;
            if(effectScale.scaleType == ScaleType.ParitcleSystem)
            {
                string lableText = "此缩放类型会为对粒子系统做缩放，具体的缩放方式对以下做选择";
                EditorGUILayout.LabelField(lableText);

                GUI.enabled = !Application.isPlaying;
                effectScale.IncludeChildren = EditorGUILayout.Toggle("影响子节点:", effectScale.IncludeChildren);
                GUI.enabled = true;

                effectScale.particleSystemScaleType = (ParticleSystemScaleType)EditorGUILayout.EnumPopup("缩放方式:", effectScale.particleSystemScaleType);
                lableText = "无效的缩放方式";
                if(effectScale.particleSystemScaleType == ParticleSystemScaleType.WoldSpaceScale)
                {
                    lableText = "此缩放方式会被父节点影响";
                }
                else if(effectScale.particleSystemScaleType == ParticleSystemScaleType.LocalSpaceScale)
                {
                    lableText = "此缩放方式不会被父节点影响";
                }
                else if (effectScale.particleSystemScaleType == ParticleSystemScaleType.ShapeScale)
                {
                    lableText = "此缩放方式不会被父节点影响,缩放的是Shape选项卡";
                }

                EditorGUILayout.LabelField(lableText);
            }
            else if(effectScale.scaleType == ScaleType.Model)
            {
                string lableText = "此缩放类型会为对模型的缩放，模型的缩放总是会影响子节点以及父节点";
                EditorGUILayout.LabelField(lableText);
            }

           
        }
    }
}

