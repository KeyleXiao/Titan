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
    [CustomEditor(typeof(CharacterShadow))]
    public class CharacterShadowEditor : Editor
    {
        CharacterShadow pa;

        public void OnEnable()
        {
            pa = target as CharacterShadow;
        }

        public override void OnInspectorGUI()
        {

            pa.BodyRender = EditorGUILayout.ObjectField("身体:", pa.BodyRender, typeof(Renderer), true) as Renderer;
            if(pa.shadowFoci != null)
            {
                
                pa.shadowFoci[0].target = EditorGUILayout.ObjectField("参考点:", pa.shadowFoci[0].target, typeof(Transform), true) as Transform;
                pa.shadowFoci[0].offset = EditorGUILayout.Vector3Field("偏移:", pa.shadowFoci[0].offset);
                pa.shadowFoci[0].radius = EditorGUILayout.FloatField("半径:", pa.shadowFoci[0].radius);
                pa.shadowFoci[0].depthBias = EditorGUILayout.FloatField("深度偏移:", pa.shadowFoci[0].depthBias);
            }
            if (GUILayout.Button("Bake"))
            {
                pa.Bake(pa.transform, pa.BodyRender);
            }
        }
    }

}
