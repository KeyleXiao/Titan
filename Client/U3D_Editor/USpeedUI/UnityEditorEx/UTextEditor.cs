using System;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI;

namespace UnityEditor.UI
{
    [CustomEditor(typeof(Text), true)]
    [CanEditMultipleObjects]
    public class UTextEditor : TextEditor
    {
        protected override void OnEnable()
        {
            base.OnEnable();
            if(target is Text)
            {
                Font msyhFont = LoadAsset<Font>("Assets/Fonts/msyh.ttf");
                Font ArialFont = Resources.GetBuiltinResource<Font>("Arial.ttf");
                if((target as Text).font == ArialFont)
                    (target as Text).font = msyhFont;
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
    }
}
