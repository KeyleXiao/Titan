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
    [CustomEditor(typeof(Effect_Random3D))]
    public class Effect_Random3DEditor : Editor
    {

        Effect_Random3D random3D;
        public void OnEnable()
        {
            random3D = target as Effect_Random3D;
        }

        public override void OnInspectorGUI()
        {
            EditorGUILayout.HelpBox("需要注意与其他脚本的冲突，比如动画K了位置，Effect_FllowObj脚本等等", MessageType.Info);

            random3D.RandomAngle = EditorGUILayout.Toggle("随机角度:", random3D.RandomAngle);
            if(random3D.RandomAngle)
            {
                random3D.MinAngle = EditorGUILayout.Vector3Field("起始范围:", random3D.MinAngle);
                random3D.MaxAngle = EditorGUILayout.Vector3Field("结束范围:", random3D.MaxAngle);
            }

            random3D.RandomScale = EditorGUILayout.Toggle("随机缩放:", random3D.RandomScale);
            if (random3D.RandomAngle)
            {
                random3D.MinScale = EditorGUILayout.Vector3Field("起始范围:", random3D.MinScale);
                random3D.MaxScale = EditorGUILayout.Vector3Field("结束范围:", random3D.MaxScale);
            }
            random3D.RandomPos = EditorGUILayout.Toggle("随机位置:", random3D.RandomPos);
            if (random3D.RandomPos)
            {
                random3D.MinPos = EditorGUILayout.Vector3Field("起始范围:", random3D.MinPos);
                random3D.MaxPos = EditorGUILayout.Vector3Field("结束范围:", random3D.MaxPos);
            }
            if (GUILayout.Button("Play"))
            {
                random3D.Play();
            }

            if (GUILayout.Button("Stop"))
            {
                random3D.Stop();
            }

            if (GUI.changed)
            {
                EditorUtility.SetDirty(random3D);
            }
        }
    }
}

