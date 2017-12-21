/*******************************************************************
** 文件名:	SwitchEffectWithHeroDistanceEditor.cs
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
    [CustomEditor(typeof(SwitchEffectWithHeroDistance))]
    public class SwitchEffectWithHeroDistanceEditor : Editor
    {


        SwitchEffectWithHeroDistance pa;
        Vector2 scrollviewPos = Vector2.zero;
        public void OnEnable()
        {
            pa = target as SwitchEffectWithHeroDistance;
        }

        public override void OnInspectorGUI()
        {

            if (pa.SwitchList.Count > 0)
            {
                GUILayout.Label("距离从大到小,物体只能在Project视图中拖进来");
                pa.HideInEmeny = EditorGUILayout.Toggle("对敌方隐藏:", pa.HideInEmeny);
                pa.HideInFriend = EditorGUILayout.Toggle("对友方隐藏:", pa.HideInFriend);
                pa.MaxShowDistahce = EditorGUILayout.FloatField("最大显示距离:", pa.MaxShowDistahce);
                scrollviewPos = EditorGUILayout.BeginScrollView(scrollviewPos);
                for (int i = 0; i < pa.SwitchList.Count; i++)
                {
                    EditorGUILayout.BeginVertical("Box");

                    SwitchEffectWithHeroDistance.SwitchEffectNode nod = pa.SwitchList[i];
                    if (!nod.effect)
                    {
                        if (!string.IsNullOrEmpty(nod.guid))
                        {
                            nod.effect = AssetDatabase.LoadAssetAtPath<EffectNode>(AssetDatabase.GUIDToAssetPath(nod.guid));
                        }
                    }

                    nod.ShowDistance = EditorGUILayout.FloatField("距离:", nod.ShowDistance);
                    if (nod.ShowDistance > pa.MaxShowDistahce)
                    {
                        pa.MaxShowDistahce = nod.ShowDistance;
                    }
                    nod.effect = (EffectNode)EditorGUILayout.ObjectField("物体:", nod.effect, typeof(EffectNode), true);
                    EditorGUILayout.TextField(nod.guid);
                    if (nod.effect)
                    {
                        string path = AssetDatabase.GetAssetPath(nod.effect);
                        nod.guid = AssetDatabase.AssetPathToGUID(path);
                    }

                    pa.SwitchList[i] = nod;
                    if (GUILayout.Button("删除"))
                    {
                        pa.SwitchList.RemoveAt(i);
                    }
                    EditorGUILayout.EndVertical();
                }
                EditorGUILayout.EndScrollView();
            }

            if (GUILayout.Button("增加配置"))
            {
                pa.SwitchList.Add(default(SwitchEffectWithHeroDistance.SwitchEffectNode));
            }
        }
    }
}

