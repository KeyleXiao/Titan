using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEditor;
using UnityEditor.UI;
using UnityEngine;
using USpeedUI.LoginAndCreate;

namespace UWidgets.WebBrowser
{
    [CustomEditor(typeof(UWebBrowserToolbarWidget), true)]
    public class UWebBrowserToolbarWidgetEditor : Editor
    {
        private UWebBrowserToolbarWidget WebBrowserToolbar;
        public string[] ToolbarDes = new string[] { "地址栏", "后退", "前进", "关闭" };
        public virtual void OnEnable()
        {
            WebBrowserToolbar = target as UWebBrowserToolbarWidget;
        }

        public override void OnInspectorGUI()
        {
            base.OnInspectorGUI();
            WebBrowserToolbar.Toolbar = (UWebBrowserToolbarWidget.EToolbar)EditorGUILayout.MaskField("工具栏",(int)WebBrowserToolbar.Toolbar, ToolbarDes);
        }
    }

    //[CustomEditor(typeof(UAdvanceLoginBtn),true)]
    //public class UAdvanceLoginBtnEditor : SelectableEditor
    //{
    //    SerializedProperty m_IsUseWebBrowserLogin;
    //    SerializedProperty m_WebBrowserSize;
    //    SerializedProperty m_WebBrowserPosOffset;
    //    SerializedProperty m_AdvanceLoginType;

    //    GUIStyle m_VerticalGroupStyle;

    //    protected override void OnEnable()
    //    {
    //        base.OnEnable();
    //        m_IsUseWebBrowserLogin = serializedObject.FindProperty("IsUseWebBrowserLogin");
    //        m_WebBrowserSize = serializedObject.FindProperty("WebBrowserSize");
    //        m_WebBrowserPosOffset = serializedObject.FindProperty("WebBrowserPosOffset");
    //        m_AdvanceLoginType = serializedObject.FindProperty("AdvanceLoginType");
    //    }
    //    public override void OnInspectorGUI()
    //    {
    //        base.OnInspectorGUI();
    //        m_VerticalGroupStyle = new GUIStyle(GUI.skin.box);
    //        m_VerticalGroupStyle.margin = new RectOffset(20, 0, 0, 0);

    //        EditorGUILayout.Space();
    //        serializedObject.Update();
    //        EditorGUILayout.PropertyField(m_IsUseWebBrowserLogin);

    //        if (m_IsUseWebBrowserLogin != null && m_IsUseWebBrowserLogin.boolValue)
    //        {
    //            EditorGUILayout.BeginVertical(m_VerticalGroupStyle);

    //            EditorGUILayout.PropertyField(m_WebBrowserSize);
    //            EditorGUILayout.PropertyField(m_WebBrowserPosOffset);
    //            EditorGUILayout.PropertyField(m_AdvanceLoginType);

    //            EditorGUILayout.EndVertical();
    //        }


    //        EditorGUILayout.Space();
    //        serializedObject.ApplyModifiedProperties();

    //    }

    //}
}
