/*******************************************************************
** 文件名:	EntitySkinConfigEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	皮肤配置脚本
** 应  用:  	皮肤配置脚本编辑器，怪物

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
    [CustomEditor(typeof(EntitySkinConfig))]
    public class EntitySkinConfigEditor : Editor
    {
        protected EntitySkinConfig Instance;
        public void OnEnable()
        {
            Instance = target as EntitySkinConfig;
        }

        public override void OnInspectorGUI()
        {
            EditorGUILayout.HelpBox("BakeMesh只适合那些只有出生或者死亡动画的物体，如防御塔等建筑物", MessageType.Info);

            EditorGUILayout.BeginVertical("box");
            Instance.RightWeaponRender = (Renderer)EditorGUILayout.ObjectField("右手武器:", Instance.RightWeaponRender, typeof(Renderer), true);
            if (Instance.RightWeaponRender && Instance.RightWeaponRender.GetType() == typeof(SkinnedMeshRenderer))
            {
                Instance.RightWeaponNeedToBakeMesh = EditorGUILayout.Toggle("BakeMesh:", Instance.RightWeaponNeedToBakeMesh);
            }
            else
            {
                Instance.RightWeaponNeedToBakeMesh = false;
            }
            EditorGUILayout.EndVertical();

            EditorGUILayout.BeginVertical("box");
            Instance.LeftWeaponRender = (Renderer)EditorGUILayout.ObjectField("左手武器:", Instance.LeftWeaponRender, typeof(Renderer), true);
            if (Instance.LeftWeaponRender && Instance.LeftWeaponRender.GetType() == typeof(SkinnedMeshRenderer))
            {
                Instance.LeftWeaponNeedToBakeMesh = EditorGUILayout.Toggle("BakeMesh:", Instance.LeftWeaponNeedToBakeMesh);
            }
            else
            {
                Instance.LeftWeaponNeedToBakeMesh = false;
            }
            EditorGUILayout.EndVertical();

            EditorGUILayout.BeginVertical("box");
            Instance.BodyRender = (Renderer)EditorGUILayout.ObjectField("身体:", Instance.BodyRender, typeof(Renderer), true);
            if (Instance.BodyRender && Instance.BodyRender.GetType() == typeof(SkinnedMeshRenderer))
            {
                Instance.BodyNeedToBakeMesh = EditorGUILayout.Toggle("BakeMesh:", Instance.BodyNeedToBakeMesh);
            }
            else
            {
                Instance.BodyNeedToBakeMesh = false;
            }
            EditorGUILayout.EndVertical();

            EditorGUILayout.BeginVertical("box");
            Instance.BackBodyRender = (Renderer)EditorGUILayout.ObjectField("背部:", Instance.BackBodyRender, typeof(Renderer), true);
            if (Instance.BackBodyRender && Instance.BackBodyRender.GetType() == typeof(SkinnedMeshRenderer))
            {
                Instance.BackBodyNeedToBakeMesh = EditorGUILayout.Toggle("BakeMesh:", Instance.BackBodyNeedToBakeMesh);
            }
            else
            {
                Instance.BackBodyNeedToBakeMesh = false;
            }
            EditorGUILayout.EndVertical();


            Instance.chibangguadian = (Transform)EditorGUILayout.ObjectField("翅膀挂点:", Instance.chibangguadian, typeof(Transform), true);
            Instance.OptimitzedDistance = EditorGUILayout.FloatField("优化距离:", Instance.OptimitzedDistance);
            Instance.CacheCout = EditorGUILayout.IntField("缓存数目:", Instance.CacheCout);
            Instance.OptimizedNearMainHero = EditorGUILayout.Toggle("靠近主角时半透明显示", Instance.OptimizedNearMainHero);

        }
    }

}
