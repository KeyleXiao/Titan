using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEditor.UI;
using UnityEditor;
using USpeedUI.LoadingBar;

[CustomEditor(typeof(UViewLoadingBar), true)]
[CanEditMultipleObjects]
public class UViewLoadingBarEditor : Editor
{
    private UViewLoadingBar m_LoadingBarView;
    public virtual void OnEnable()
    {
        m_LoadingBarView = target as UViewLoadingBar;
    }
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();
        if (m_LoadingBarView == null)
            return;
        m_LoadingBarView.TweenerLoadingValue = EditorGUILayout.Slider(m_LoadingBarView.TweenerLoadingValue, 0.0f, 1.0f);
    }
}
