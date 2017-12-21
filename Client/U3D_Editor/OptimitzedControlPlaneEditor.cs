/*******************************************************************
** 文件名:	OptimitzedControlPlaneEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2016/2/24
** 版  本:	1.0
** 描  述:	
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

[CustomEditor(typeof(OptimitzedControlPlane))]
public class OptimitzedControlPlaneEditor : Editor
{
    private OptimitzedControlPlane m_Instance;
    public void OnEnable()
    {
        m_Instance = this.target as OptimitzedControlPlane;
    }
    public override void OnInspectorGUI()
    {
        if(Application.isPlaying)
        {
            GUI.enabled = false;
        }

        //GUILayout.BeginVertical("Box");
        //m_Instance.OptimitzedShader = EditorGUILayout.Toggle("Shader优化:", m_Instance.OptimitzedShader);
        //GUILayout.EndVertical();

        //GUILayout.BeginVertical("Box");
        //m_Instance.EnityCreateOptimize = EditorGUILayout.Toggle("实体创建优化:", m_Instance.EnityCreateOptimize);
        //GUILayout.EndVertical();

        GUI.enabled = true;

        EditorUtility.SetDirty(m_Instance);
    }
}
