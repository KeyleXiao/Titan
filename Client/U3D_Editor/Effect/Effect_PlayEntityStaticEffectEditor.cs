/*******************************************************************
** 文件名:	Effect_PlayEntityStaticEffectEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/6/9
** 版  本:	1.0
** 描  述:	SwitchEffectWithHeroDistance编辑器脚本
** 应  用:  	编辑器

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
    [CustomEditor(typeof(Effect_PlayEntityStaticEffect))]
    public class Effect_PlayEntityStaticEffectEditor : Editor
    {


        Effect_PlayEntityStaticEffect pa;
        Vector2 scrollviewPos = Vector2.zero;
        public void OnEnable()
        {
            pa = target as Effect_PlayEntityStaticEffect;
        }

        public override void OnInspectorGUI()
        {
            if (pa.EffectList.Count > 0)
            {
                GUILayout.Label("物体只能在Project视图中拖进来");
                scrollviewPos = EditorGUILayout.BeginScrollView(scrollviewPos);
                for (int i = 0; i < pa.EffectList.Count; i++)
                {
                    EditorGUILayout.BeginVertical("Box");
                    Effect_PlayEntityStaticEffect.StaticEffectNode nod = pa.EffectList[i];
                    if (!nod.effect)
                    {
                        if (!string.IsNullOrEmpty(nod.guid))
                        {
                            nod.effect = AssetDatabase.LoadAssetAtPath<EffectNode>(AssetDatabase.GUIDToAssetPath(nod.guid));
                        }
                    }
                    nod.effect = (EffectNode)EditorGUILayout.ObjectField("特效:", nod.effect, typeof(EffectNode), false);
                    EditorGUILayout.TextField(nod.guid);
                    nod.fllowBone = EditorGUILayout.ObjectField("跟随的骨骼:", nod.fllowBone, typeof(Transform), true) as Transform;
                    nod.PosOffset = EditorGUILayout.Vector3Field("位置偏移:", nod.PosOffset);
                    nod.AngleOffset = EditorGUILayout.Vector3Field("角度偏移:", nod.AngleOffset);
                    if (nod.effect)
                    {
                        string path = AssetDatabase.GetAssetPath(nod.effect);
                        nod.guid = AssetDatabase.AssetPathToGUID(path);
                    }
                    pa.EffectList[i] = nod;
                    if (GUILayout.Button("删除"))
                    {
                        pa.EffectList.RemoveAt(i);
                    }

                    EditorGUILayout.EndVertical();
                }
                EditorGUILayout.EndScrollView();
            }


            if (GUILayout.Button("增加配置"))
            {
                pa.EffectList.Add(default(Effect_PlayEntityStaticEffect.StaticEffectNode));
            }
        }
    }
}

