/*******************************************************************
** 文件名:	PersonSkinConfigEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	皮肤配置脚本
** 应  用:  	皮肤配置脚本编辑器,人物

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
    [CustomEditor(typeof(PersonSkinConfig))]
    public class PersonSkinConfigEditor : EntitySkinConfigEditor
    {

        public override void OnInspectorGUI()
        {
            base.OnInspectorGUI();
            PersonSkinConfig pa = target as PersonSkinConfig;
            pa.Main = (Transform)EditorGUILayout.ObjectField("Main:", pa.Main, typeof(Transform), true);
            pa.Root_M = (Transform)EditorGUILayout.ObjectField("Root_M:", pa.Root_M, typeof(Transform), true);
            pa.BackA_M = (Transform)EditorGUILayout.ObjectField("BackA_M:", pa.BackA_M, typeof(Transform), true);
            pa.Root_MCopy = (Transform)EditorGUILayout.ObjectField("Root_MCopy:", pa.Root_MCopy, typeof(Transform), true);
            pa.weaponObject = (Transform)EditorGUILayout.ObjectField("weaponObject:", pa.weaponObject, typeof(Transform), true);
            pa.weaponObject2 = (Transform)EditorGUILayout.ObjectField("weaponObject2:", pa.weaponObject2, typeof(Transform), true);
            pa.adjustSpine =EditorGUILayout.Toggle("adjustSpine:", pa.adjustSpine);
            pa.RootAReplace = EditorGUILayout.Toggle("RootAReplace:", pa.RootAReplace);
            pa.AttackAnimatorLayer = EditorGUILayout.IntField("上下半身分离的攻击层位置，0表示用旧版IK做上下半身分离:", pa.AttackAnimatorLayer);
            if(pa.AttackAnimatorLayer >= 0)
            {
                pa.controllerA = null; 
                pa.controllerB = null;
            }
            else
            {
                pa.controllerA = (RuntimeAnimatorController)EditorGUILayout.ObjectField("controllerA:", pa.controllerA, typeof(RuntimeAnimatorController), false);
                pa.controllerB = (RuntimeAnimatorController)EditorGUILayout.ObjectField("controllerB:", pa.controllerB, typeof(RuntimeAnimatorController), false);
            }
            pa.rootAngleWithAim = EditorGUILayout.Vector3Field("rootAngleWithAim:",pa.rootAngleWithAim);
            pa.progressWeight = EditorGUILayout.Toggle("progressWeight:", pa.progressWeight);
            pa.lookAngleScale = EditorGUILayout.FloatField("弯腰瞄准程度(0-1):", pa.lookAngleScale);
            pa.lookRefTransform = (Transform)EditorGUILayout.ObjectField("弯腰瞄准参考骨骼:", pa.lookRefTransform, typeof(Transform), true);
            pa.ScreenCastObject = (GameObject)EditorGUILayout.ObjectField("ScreenCastObject:", pa.ScreenCastObject, typeof(GameObject), true);
            pa.jumpDeltyTick = EditorGUILayout.IntField("jumpDeltyTick:", pa.jumpDeltyTick);
            pa.maxFallSpeed = EditorGUILayout.FloatField("maxFallSpeed:", pa.maxFallSpeed);
            pa.MorphHip = EditorGUILayout.Toggle("MorphHip:", pa.MorphHip);
            EditorGUILayout.HelpBox("必须根据实际情况拖拽RightWeaponRender、" +
                                    "LeftWeraponRender、BodyRnder!" +
                                    "必须根据实际情况填写CacheCout，比如弓兵，同时存在" +
                                    "的数目大概会达到30个，这个值要填30！", MessageType.Info);
        }
    }

}
