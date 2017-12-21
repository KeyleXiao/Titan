/*******************************************************************
** 文件名:	Effect_LineRenderEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/10
** 版  本:	1.0
** 描  述:	Effect_LineRender编辑器脚本
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
    [CustomEditor(typeof(Effect_LineRender))]
    public class Effect_LineRenderEditor : Editor
    {

        Effect_LineRender m_Instance;
        SerializedProperty ThickNessTexture = null;
        public void OnEnable()
        {
            m_Instance = target as Effect_LineRender;
            ThickNessTexture = serializedObject.FindProperty("ThickNessTexture");
        }

        Transform src;
        Transform dst;
        public override void OnInspectorGUI()
        {
            m_Instance.RunMode = (EffectRunMode)EditorGUILayout.EnumPopup("运行模式:", m_Instance.RunMode);
            m_Instance.targetType = (LineRenderTargetType)EditorGUILayout.EnumPopup("目标类型:", m_Instance.targetType);
            if(m_Instance.RunMode == EffectRunMode.Self)
            {
                src = EditorGUILayout.ObjectField("源物体:", src, typeof(Transform), true) as Transform;
                dst = EditorGUILayout.ObjectField("目标物体:", dst, typeof(Transform), true) as Transform;
                m_Instance.LocalSrcObj = src;
                m_Instance.LocalDstObj = dst;

            }



            m_Instance.UseThickNess = EditorGUILayout.Toggle("使用贴图密度:",m_Instance.UseThickNess);
            if(m_Instance.UseThickNess)
            {
                EditorGUI.BeginChangeCheck();
                EditorGUILayout.PropertyField(ThickNessTexture, true);
                if (EditorGUI.EndChangeCheck())
                    serializedObject.ApplyModifiedProperties();

                m_Instance.RefDistance = EditorGUILayout.FloatField("贴图密度距离:", m_Instance.RefDistance);
            }


            GUI.enabled = !EditorApplication.isPlaying;
            ValueMode valueMode = (ValueMode)EditorGUILayout.EnumPopup("运算方式:", m_Instance.valueMode);
            if (valueMode != m_Instance.valueMode)
            {
                m_Instance.valueMode = valueMode;
            }
            GUI.enabled = true;

            if (m_Instance.valueMode == ValueMode.Constant)
            {
                int vertexCount = EditorGUILayout.IntSlider("顶点数:", m_Instance.VertexCount, 2, 150);
                if (vertexCount!=m_Instance.VertexCount)
                {
                    m_Instance.VertexCount = vertexCount;
                    m_Instance.useMultiColor = (vertexCount > 2);
                }

                if (m_Instance.useMultiColor)
                {
                    EditorGUILayout.TextField("使用多个顶点时");
                    EditorGUILayout.TextField("每增加一个顶点");
                    EditorGUILayout.TextField("渲染量会多一倍");
                    EditorGUILayout.TextField("仅在需要流动纹理并且两端模糊时使用");
                    m_Instance.vecStartPoint = EditorGUILayout.Vector3Field("StartPoint:", m_Instance.vecStartPoint);
                    m_Instance.vecEndPoint = EditorGUILayout.Vector3Field("EndPoint:", m_Instance.vecEndPoint);
                    m_Instance.lineWidth = EditorGUILayout.Vector2Field("LineWidth", m_Instance.lineWidth);

                    if (m_Instance.subVectorColor==null || m_Instance.subVectorColor.Length != m_Instance.VertexCount)
                    {
                        m_Instance.subVectorColor = null;
                        m_Instance.subVectorColor = new Effect_LineRender.ColorVertex[m_Instance.VertexCount];
                        for (int i = 0; i < m_Instance.subVectorColor.Length; i++)
                        {
                            m_Instance.subVectorColor[i].pert = (float)i / (float)(m_Instance.subVectorColor.Length - 1);
                            m_Instance.subVectorColor[i].color = Color.white;
                        }
                    }

                    for (int i = 0; i < m_Instance.subVectorColor.Length; i++)
                    {
                        m_Instance.subVectorColor[i].pert = EditorGUILayout.FloatField("顶点比例(0-1,头0尾1)" + i, m_Instance.subVectorColor[i].pert);
                        m_Instance.subVectorColor[i].color = EditorGUILayout.ColorField("顶点色" + i, m_Instance.subVectorColor[i].color);
                    }

                    if (m_Instance.subVectorColor.Length>2)
                    {
                        m_Instance.subVectorColor[0].pert=0.0f;
                        m_Instance.subVectorColor[m_Instance.subVectorColor.Length-1].pert=1.0f;
                    }
                }
                
            }

            if(m_Instance.valueMode == ValueMode.Curve)
            {
                m_Instance.VertexCount = EditorGUILayout.IntSlider("顶点数:", m_Instance.VertexCount, 2, 150);
                EditorGUILayout.BeginVertical("box");
                m_Instance.VertexCurve = EditorGUILayout.CurveField("顶点曲线:", m_Instance.VertexCurve);
                m_Instance.CurveScale = EditorGUILayout.FloatField("曲线缩放:", m_Instance.CurveScale);
                m_Instance.VertexCurve_X = EditorGUILayout.Toggle("X:", m_Instance.VertexCurve_X);
                m_Instance.VertexCurve_Y = EditorGUILayout.Toggle("Y:", m_Instance.VertexCurve_Y);
                m_Instance.VertexCurve_Z = EditorGUILayout.Toggle("Z:", m_Instance.VertexCurve_Z);
                EditorGUILayout.EndVertical();
            }

            m_Instance.UVScroll = EditorGUILayout.ObjectField("UV流动控制器:", m_Instance.UVScroll, typeof(Effect_UVScroll), true) as Effect_UVScroll;
            m_Instance.UVTextureAnimator = EditorGUILayout.ObjectField("UV动画控制器:", m_Instance.UVTextureAnimator, typeof(Effect_UVTextureAnimator), true) as Effect_UVTextureAnimator;
            m_Instance.FadeInOutMaterialFloat = EditorGUILayout.ObjectField("浮点数控制器:", m_Instance.FadeInOutMaterialFloat, typeof(Effect_FadeInOutMaterialFloat), true) as Effect_FadeInOutMaterialFloat;
            m_Instance.FadeInOutMaterialColor = EditorGUILayout.ObjectField("颜色控制器:", m_Instance.FadeInOutMaterialColor, typeof(Effect_FadeInOutMaterialColor), true) as Effect_FadeInOutMaterialColor;

            //if (GUILayout.Button("Play"))
            //{
            //    m_Instance.Play();
            //}

            //if (GUILayout.Button("Stop"))
            //{
            //    m_Instance.Stop();
            //}
            //if (GUILayout.Button("Init"))
            //{
            //    m_Instance.Init();
            //}

            if (GUI.changed)
            {
                EditorUtility.SetDirty(m_Instance);
            }
        }
    }
}

