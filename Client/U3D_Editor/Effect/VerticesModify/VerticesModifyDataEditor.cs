using UnityEngine;
using System.Collections;
using UnityEditor;
[CustomEditor(typeof(VerticesModifyData))]
public class VerticesModifyDataEditor : Editor
{


    private VerticesModifyData tar;
    public void OnEnable()
    {
        tar = target as VerticesModifyData;
    }

    //public override void OnInspectorGUI()
    //{
    //    EditorGUILayout.TextField("标示名:", tar.MeshKeyName);
    //    EditorGUILayout.TextField("数据路径:", tar.DataPatch);
    //}

}
