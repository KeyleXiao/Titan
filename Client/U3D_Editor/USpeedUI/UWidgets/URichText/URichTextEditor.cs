using UnityEngine;
using System.Collections;
using UnityEditor;

[CustomEditor(typeof(URichText))]
public class URichTextEditor : UnityEditor.UI.TextEditor
{
    protected SerializedProperty m_SpriteAsset;
    protected SerializedProperty m_TextSprite;
    protected SerializedProperty m_UnderLineColor;
    //public void OnEnable()
    //{
    //    inlineManager = (InlineSpriteManager)target;
    //    spriteAsset = inlineManager.m_spriteAsset;
    //}

    //private Vector2 ve2ScorllView;
    protected override void OnEnable()
    {
        base.OnEnable();
        if (target is URichText)
        {
            Font msyhFont = LoadAsset<Font>("Assets/Fonts/msyh.ttf");
            Font ArialFont = Resources.GetBuiltinResource<Font>("Arial.ttf");
            if ((target as URichText).font == ArialFont)
                (target as URichText).font = msyhFont;
        }
    }

    static public UnityEngine.Object LoadAsset(string path)
    {
        if (string.IsNullOrEmpty(path)) return null;
        return AssetDatabase.LoadMainAssetAtPath(path);
    }
    static public T LoadAsset<T>(string path) where T : UnityEngine.Object
    {
        UnityEngine.Object obj = LoadAsset(path);
        if (obj == null) return null;

        T val = obj as T;
        if (val != null) return val;

        if (typeof(T).IsSubclassOf(typeof(Component)))
        {
            if (obj.GetType() == typeof(GameObject))
            {
                GameObject go = obj as GameObject;
                return go.GetComponent(typeof(T)) as T;
            }
        }
        return null;
    }
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        serializedObject.Update();
        m_TextSprite = serializedObject.FindProperty("textSprite");
        m_UnderLineColor = serializedObject.FindProperty("UnderLineColor");

        EditorGUILayout.PropertyField(m_TextSprite);
        EditorGUILayout.PropertyField(m_UnderLineColor);

        serializedObject.ApplyModifiedProperties();
    }
}
