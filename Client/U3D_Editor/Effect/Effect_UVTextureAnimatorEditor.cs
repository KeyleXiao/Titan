using UnityEngine;
using UnityEditor;

namespace Effect
{
    [CustomEditor(typeof(Effect_UVTextureAnimator))]
    public class Effect_UVTextureAnimatorEditor : Editor
    {

        Effect_UVTextureAnimator pa;
        public void OnEnable()
        {
            pa = target as Effect_UVTextureAnimator;
        }


        public override void OnInspectorGUI()
        {
            EditorGUILayout.HelpBox("Effect_UVScroll脚本不能和Effect_UVTextureAnimator同时作用于同一张贴图", MessageType.Warning);

            pa.RunMode = (EffectRunMode)EditorGUILayout.EnumPopup("运行模式:", pa.RunMode);

            serializedObject.Update();

            EditorGUI.BeginChangeCheck();
            pa.TexturePropertyName = EditorGUILayout.TextField("属性名:", pa.TexturePropertyName);
            if (pa.RunMode == EffectRunMode.Self)
            {
                pa.UseSharedMaterial = EditorGUILayout.Toggle("使用共享材质:", pa.UseSharedMaterial);
            }
            pa.fStartDelay = EditorGUILayout.FloatField("延迟开始(小于0.03无延迟):", pa.fStartDelay);
            pa.Rows = EditorGUILayout.IntField("行:",pa.Rows);
            pa.Columns = EditorGUILayout.IntField("列:",pa.Columns);
            pa.Fps = EditorGUILayout.FloatField("帧数:",pa.Fps);
            pa.SelfTiling = EditorGUILayout.Vector2Field("密度:",pa.SelfTiling);
            pa.AniTimes = EditorGUILayout.IntField("播放次数(小于等于0无限次):", pa.AniTimes);
            pa.IsReverse = EditorGUILayout.Toggle("逆向:",pa.IsReverse);

            pa.UseModelUV = EditorGUILayout.Toggle("使用模型UV:", pa.UseModelUV);

            if (GUILayout.Button("Play"))
            {
                pa.Play();
            }
            if (GUILayout.Button("Stop"))
            {
                pa.Stop();
            }
            if (GUI.changed)
            {
                EditorUtility.SetDirty(pa);
            }
        }
    }
}
