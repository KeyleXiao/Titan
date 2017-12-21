/*******************************************************************
** 文件名:	Effect_FullScreenEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	特效功能脚本
** 应  用:  	全屏特效编辑器脚本

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using UnityEngine;
using System.Collections;
using UnityEditor;
namespace Effect
{
    [CustomEditor(typeof(Effect_FullScreen))]
    public class Effect_FullScreenEditor : Editor
    {
        Effect_FullScreen pa;
        Editor currentMaterialEditor;
        Editor m_tmpEditor;
        public void OnEnable()
        {
            pa = target as Effect_FullScreen;
        }

        public void OnDisable()
        {
            if (m_tmpEditor != null)
            {
                DestroyImmediate(m_tmpEditor);
            }

            if (currentMaterialEditor != null)
            {
                DestroyImmediate(currentMaterialEditor);
            }
        }

        public override void OnInspectorGUI()
        {
            pa.FadeInOutMaterialColor = EditorGUILayout.ObjectField("颜色控制器:", pa.FadeInOutMaterialColor, typeof(Effect_FadeInOutMaterialColor), true) as Effect_FadeInOutMaterialColor;
            pa.FadeInOutMaterialFloat = EditorGUILayout.ObjectField("浮点数控制器:", pa.FadeInOutMaterialFloat, typeof(Effect_FadeInOutMaterialFloat), true) as Effect_FadeInOutMaterialFloat;
            pa.UVTextureAnimator = EditorGUILayout.ObjectField("UV动画控制器:", pa.UVTextureAnimator, typeof(Effect_UVTextureAnimator), true) as Effect_UVTextureAnimator;
            pa.UVScroll = EditorGUILayout.ObjectField("UV流动控制器:", pa.UVScroll, typeof(Effect_UVScroll), true) as Effect_UVScroll;

            pa.sortOrder = EditorGUILayout.Slider("排序(越大越前)", pa.sortOrder, -1.0f, 1.0f);
            //pa.RandomPos = EditorGUILayout.Toggle("随机位置:", pa.RandomPos);
            if (!pa.RandomPos)
            {
                pa.CorrectPos = EditorGUILayout.Vector3Field("位置:", pa.CorrectPos);
            }
            pa.CorrectScale = EditorGUILayout.Vector3Field("缩放:", pa.CorrectScale);
            pa.mat = EditorGUILayout.ObjectField("材质:", pa.mat, typeof(Material), true) as Material;
            if (GUILayout.Button("Init"))
            {
                pa.Init();
            }

            if (GUILayout.Button("Play"))
            {
                pa.Play(0);
            }

            if (GUILayout.Button("Stop"))
            {
                pa.Stop();
            }

            if (GUI.changed)
            {
                EditorUtility.SetDirty(pa);
            }


           // EditorGUI.BeginChangeCheck();
            InitEditor(pa.InstanceMat);

            currentMaterialEditor = m_tmpEditor;
            //if (EditorGUI.EndChangeCheck())
            //{
               // InitEditor(pa.InstanceMat);
           // }
            if (currentMaterialEditor != null && pa != null)
            {
                currentMaterialEditor.DrawHeader();
                currentMaterialEditor.OnInspectorGUI();
            }
        }

        void InitEditor(Material m_Mat)
        {
            if (m_Mat != null)
            {
                m_tmpEditor = CreateEditor(m_Mat);
            }

            if (currentMaterialEditor != null)
            {
                DestroyImmediate(currentMaterialEditor);
            }
        }
    }

}
