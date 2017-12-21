/*******************************************************************
** 文件名:	SkyCloudEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/11/3
** 版  本:	1.0
** 描  述:	天空系统
** 应  用:  	天空系统--云编辑器脚本

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
    [CustomEditor(typeof(SkyCloud))]
    public class SkyCloudEditor : Editor
    {

        SkyCloud m_skyCloud = null;
        Editor currentMaterialEditor;
        Editor m_tmpEditor;

        SerializedObject serObj;
        SerializedProperty mat;

        public void OnEnable()
        {
            serObj = new SerializedObject(target);
            mat = serObj.FindProperty("CloudMaterial");
            m_skyCloud = target as SkyCloud;
        }

        public override void OnInspectorGUI()
        {

            serObj.Update();
            EditorGUILayout.PropertyField(mat, new GUIContent("Cloud Material", "云材质球."));
            serObj.ApplyModifiedProperties();

            Material m_Mat = m_skyCloud.CloudMaterial;

            EditorGUI.BeginChangeCheck();

            InitEditor(m_Mat);

            currentMaterialEditor = m_tmpEditor;

            if (EditorGUI.EndChangeCheck())
            {
                InitEditor(m_Mat);
            }

            if (currentMaterialEditor != null && m_Mat != null)
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
