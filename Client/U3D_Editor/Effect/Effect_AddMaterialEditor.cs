/*******************************************************************
** 文件名:	Effect_AddMaterialEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	特效功能脚本
** 应  用:  	播放特效编辑器

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
    [CustomEditor(typeof(Effect_AddMaterial))]
    public class Effect_AddMaterialEditor : Editor
    {
        Effect_AddMaterial pa;
        public void OnEnable()
        {
            pa = target as Effect_AddMaterial;
        }
        public override void OnInspectorGUI()
        {
            EditorGUILayout.HelpBox("Effect_AddMaterial脚本不能和Effect_ReplaceMaterial同时作用于同一个物体", MessageType.Warning);

            //pa.RunMode = (EffectRunMode)EditorGUILayout.EnumPopup("运行模式:", pa.RunMode);
            //string runmodeTips = "";
            //if (pa.RunMode == EffectRunMode.Self)
            //{
            //    runmodeTips = "自己运行自己";
            //}
            //else if (pa.RunMode == EffectRunMode.OutSiedePointed)
            //{
            //    runmodeTips = "通过外部来运行,比如护盾、Effect_AddMaterial系列，请用这个，表示这个脚本由他们来管理";
            //}

            //EditorGUILayout.TextField(runmodeTips);

            pa.TargetType = (MaterialOpRunType)EditorGUILayout.EnumPopup("作用模式:", pa.TargetType);

            string dotips = "";

            if (pa.TargetType == MaterialOpRunType.EntityID)
            {
                dotips = "通过指定实体ID，来让这个脚本起效，这种模式只能用在游戏中";
            }
            else if (pa.TargetType == MaterialOpRunType.PointedModel)
            {
                dotips = "通过外部指定模型来让这个脚本起效";
            }
            else if (pa.TargetType == MaterialOpRunType.Self)
            {
                dotips = "作用于自己身上";
            }
            EditorGUILayout.TextField(dotips);

            if (pa.TargetType == MaterialOpRunType.PointedModel)
            {
                pa.TargetModel = EditorGUILayout.ObjectField("目标物体:",pa.TargetModel, typeof(GameObject), true) as GameObject;
            }

            if (pa.TargetType == MaterialOpRunType.EntityID)
            {
                pa.AffectBody = EditorGUILayout.Toggle("影响身体", pa.AffectBody);
                pa.AffectRightWeapon = EditorGUILayout.Toggle("影响右手", pa.AffectRightWeapon);
                pa.AffectLeftWeapon = EditorGUILayout.Toggle("影响左手", pa.AffectLeftWeapon);
                pa.AffectBackPart = EditorGUILayout.Toggle("影响背部", pa.AffectBackPart);
                pa.AffectWings = EditorGUILayout.Toggle("影响翅膀", pa.AffectWings);
            }


            pa.TargetMaterial = EditorGUILayout.ObjectField("要增加的材质球:",pa.TargetMaterial, typeof(Material), false) as Material;
            pa.UVScroll = EditorGUILayout.ObjectField("UV流动控制器:", pa.UVScroll, typeof(Effect_UVScroll), true) as Effect_UVScroll;
            pa.UVTextureAnimator = EditorGUILayout.ObjectField("UV动画控制器:",pa.UVTextureAnimator, typeof(Effect_UVTextureAnimator), true) as Effect_UVTextureAnimator;
            pa.FadeInOutMaterialFloat = EditorGUILayout.ObjectField("浮点数控制器:",pa.FadeInOutMaterialFloat, typeof(Effect_FadeInOutMaterialFloat), true) as Effect_FadeInOutMaterialFloat;
            pa.FadeInOutMaterialColor = EditorGUILayout.ObjectField("颜色控制器:",pa.FadeInOutMaterialColor, typeof(Effect_FadeInOutMaterialColor), true) as Effect_FadeInOutMaterialColor;
            pa.GradientColor = EditorGUILayout.ObjectField("颜色控制器(高级):", pa.GradientColor, typeof(Effect_MaterialGradientColor), true) as Effect_MaterialGradientColor;
            pa.Duartion = EditorGUILayout.FloatField("持续时间(小于0.03表示一直存在):", pa.Duartion);
            pa.DelayTime = EditorGUILayout.FloatField("延迟时间(小于0.03表示无延迟):", pa.DelayTime);

            if (GUILayout.Button("Init"))
            {
                pa.Init();
            }

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
