///*******************************************************************
//** 文件名:	SpreadFieldEditor.cs
//** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
//** 创建人:	谭强均
//** 日  期:	2015/8/6
//** 版  本:	1.0
//** 描  述:	
//** 应  用:  	

//**************************** 修改记录 ******************************
//** 修改人: 
//** 日  期: 
//** 描  述: 
//********************************************************************/
//using UnityEngine;
//using UnityEditor;

//namespace Effect
//{
//    [CustomEditor(typeof(SpreadShield))]
//    public class SpreadShieldEditor : Editor
//    {
//        SpreadShield pa;
//        public void OnEnable()
//        {
//            pa = target as SpreadShield;
//        }

//        public override void OnInspectorGUI()
//        {
//            EditorGUILayout.HelpBox("护盾的顶点数要少!，要少!，要少!", MessageType.Warning);
//            EditorGUILayout.Separator();
//            pa.ShieldQuality = (ShieldFeedBackQuality)EditorGUILayout.EnumPopup("护盾质量等级:", pa.ShieldQuality);
//            pa.UVScroll = EditorGUILayout.ObjectField("UV流动控制器:", pa.UVScroll, typeof(Effect_UVScroll), true) as Effect_UVScroll;
//            pa.UVTextureAnimator = EditorGUILayout.ObjectField("UV动画控制器:", pa.UVTextureAnimator, typeof(Effect_UVTextureAnimator), true) as Effect_UVTextureAnimator;
//            pa.FadeInOutMaterialFloat = EditorGUILayout.ObjectField("浮点数控制器:", pa.FadeInOutMaterialFloat, typeof(Effect_FadeInOutMaterialFloat), true) as Effect_FadeInOutMaterialFloat;
//            pa.FadeInOutMaterialColor = EditorGUILayout.ObjectField("颜色控制器:", pa.FadeInOutMaterialColor, typeof(Effect_FadeInOutMaterialColor), true) as Effect_FadeInOutMaterialColor;
//            pa.GradientColor = EditorGUILayout.ObjectField("颜色控制器(高级):", pa.GradientColor, typeof(Effect_MaterialGradientColor), true) as Effect_MaterialGradientColor;
//            pa.ShieldMaterial = EditorGUILayout.ObjectField("护盾材质球:", pa.ShieldMaterial, typeof(Material), false) as Material;
//            pa.ShiledMainTexture = EditorGUILayout.ObjectField("护盾贴图:", pa.ShiledMainTexture, typeof(Texture), false) as Texture;
//            pa.FieldDuartion = EditorGUILayout.FloatField("持续时间(建议根据控制器的时间来填写):", pa.FieldDuartion);
//            pa.FieldScale = EditorGUILayout.FloatField("缩放:", pa.FieldScale);
//            if (GUI.changed)
//            {
//                EditorUtility.SetDirty(pa);
//            }
//        }
//    }
//}
