using UnityEngine;
using UnityEditor;
using System.Reflection;
using System.Collections;
using System.Collections.Generic;
using UnityEditorInternal;
using System.IO;

//[CustomEditor(typeof(Effect_AnimatorBool))]

public class LightingEffect_Base_Editor<T> : CustomInspector<T> where T : ScriptableObject
{
    public override void OnMemberGUI(MemberInfo info)
    {

        //获取中文Name
        string itemDesc = info.Name;
        var nameObj = info.GetCustomAttributes(typeof(DescAttribute), true);
        if (nameObj.Length > 0)
        {
            itemDesc = ((DescAttribute)nameObj[0]).description;
        }

        ////获取中文ToolTip
        //string itemTooltip = "";
        //var tooltipObj = info.GetCustomAttributes(typeof(TooltipAttribute), true);
        //if (tooltipObj.Length > 0)
        //{
        //    itemTooltip = ((TooltipAttribute)tooltipObj[0]).tooltip;
        //}
        SerializedProperty prop = serializedObject.FindProperty(info.Name);
        if (prop == null)
        {
            return;
        }
        if (info.Name.Equals("NeedCache"))
        {
            SerializedProperty eVocationProp = serializedObject.FindProperty("eVocation");
            if (!eVocationProp.enumNames[eVocationProp.enumValueIndex].Equals(VOCATION.VOCATION_INVALID.ToString()))
            {
                return;
            }
        }
        // 显示名字
        EditorGUILayout.BeginVertical(editorInstance.rootGroupStyle);
        {
            if(itemDesc!="")
                EditorGUILayout.LabelField(itemDesc+ " : ");
            EditorGUILayout.BeginHorizontal();
            {
                // 根据info.Name和prop.type自己选择不同的编辑方式
                if (prop.type != "")
                {
                    //AssetBundleName和AssetName不可编辑
                    if (info.Name.Equals("AssetGUID"))
                    {
                        GUI.enabled = false;
                    }
                    GUIContent descContent = new GUIContent(info.Name + ":", itemDesc);
                    EditorGUIUtility.labelWidth= 500;
                    EditorGUILayout.PropertyField(prop, descContent,true);
                    //EditorGUILayout.LabelField(itemTooltip);
                    GUI.enabled = true;

                    //拖拽预制体的时候,同时赋值三个信息
                    if (info.Name.Equals("effectPrefb") && prop.objectReferenceValue)
                    {
                        AssetImporter asp = AssetImporter.GetAtPath(AssetDatabase.GetAssetPath(prop.objectReferenceValue));
                        SerializedProperty AssetGUID_prop = serializedObject.FindProperty("AssetGUID");
                        if (asp && null != AssetGUID_prop)
                        {
                            AssetGUID_prop.stringValue = AssetBundleManagerEditor.GetAssetGUIDFromMeta(asp.assetPath);
                            //prop.objectReferenceValue = null;
                        }
                        else
                        {
                            Debug.LogError("获取资源信息失败!");
                        }
                    }
                    serializedObject.ApplyModifiedProperties();
                }
            }
            EditorGUILayout.EndHorizontal();

        }
        EditorGUILayout.EndVertical();
    }
}

[CustomEditor(typeof(LightingEffect_Bind))]
public class LightingEffect_Bind_Editor : LightingEffect_Base_Editor<LightingEffect_Bind>
{
    // 自定义创建Effect_AttackEffect对象的菜单
    [MenuItem("Assets/Create/LightingEffect/LightingEffect_Bind")]
    public static void Create()
    {
        CreateAsset();
    }

    public override void DoSaveAction()
    {
        LightingEffect ta = target as LightingEffect;
        LightingEffectFactory.LoadByEditor(ta.nID);
    }
}

[CustomEditor(typeof(LightingEffect_Fly))]
public class LightingEffect_Fly_Editor : LightingEffect_Base_Editor<LightingEffect_Fly>
{
    // 自定义创建Effect_Animation对象的菜单
    [MenuItem("Assets/Create/LightingEffect/LightingEffect_Fly")]
    public static void Create()
    {
        CreateAsset();
    }

    public override void DoSaveAction()
    {
        Debug.Log("Applying changes on LightingEffect");
        LightingEffect ta = target as LightingEffect;
        LightingEffectFactory.LoadByEditor(ta.nID);
    }
}

[CustomEditor(typeof(LightingEffect_Area))]
public class LightingEffect_Area_Editor : LightingEffect_Base_Editor<LightingEffect_Area>
{
    // 自定义创建LightingEffect_Area对象的菜单
    [MenuItem("Assets/Create/LightingEffect/LightingEffect_Area")]
    public static void Create()
    {
        CreateAsset();
    }

    public override void DoSaveAction()
    {
        LightingEffect ta = target as LightingEffect;
        LightingEffectFactory.LoadByEditor(ta.nID);
        Debug.Log("Applying changes on LightingEffect");
    }
}


[CustomEditor(typeof(LightingEffect_Shadow))]
public class LightingEffect_Shadow_Editor : LightingEffect_Base_Editor<LightingEffect_Shadow>
{
    // 自定义创建LightingEffect_Shadow对象的菜单
    [MenuItem("Assets/Create/LightingEffect/LightingEffect_Shadow")]
    public static void Create()
    {
        CreateAsset();
    }

    public override void DoSaveAction()
    {
        LightingEffect ta = target as LightingEffect;
        LightingEffectFactory.LoadByEditor(ta.nID);
        Debug.Log("Applying changes on LightingEffect");
    }
}

[CustomEditor(typeof(LightingEffect_Camera))]
public class LightingEffect_Camera_Editor : LightingEffect_Base_Editor<LightingEffect_Camera>
{
    // 自定义创建LightingEffect_Camera对象的菜单
    [MenuItem("Assets/Create/LightingEffect/LightingEffect_Camera")]
    public static void Create()
    {
        CreateAsset();
    }

    public override void DoSaveAction()
    {
        LightingEffect ta = target as LightingEffect;
        LightingEffectFactory.LoadByEditor(ta.nID);
        Debug.Log("Applying changes on LightingEffect");
    }
}


[CustomEditor(typeof(LightingEffect_Ward))]
public class LightingEffect_Ward_Editor : LightingEffect_Base_Editor<LightingEffect_Ward>
{
    // 自定义创建LightingEffect_Ward对象的菜单
    [MenuItem("Assets/Create/LightingEffect/LightingEffect_Ward")]
    public static void Create()
    {
        CreateAsset();
    }

    public override void DoSaveAction()
    {
        LightingEffect ta = target as LightingEffect;
        LightingEffectFactory.LoadByEditor(ta.nID);
        Debug.Log("Applying changes on LightingEffect");
    }
}

[CustomEditor(typeof(LightingEffect_AnimationReplace))]
public class LightingEffect_AnimationReplace_Editor : LightingEffect_Base_Editor<LightingEffect_AnimationReplace>
{
    // 自定义创建LightingEffect_AnimationReplace对象的菜单
    [MenuItem("Assets/Create/LightingEffect/LightingEffect_AnimationReplace")]
    public static void Create()
    {
        CreateAsset();
    }

    public override void DoSaveAction()
    {
        LightingEffect ta = target as LightingEffect;
        LightingEffectFactory.LoadByEditor(ta.nID);
        Debug.Log("Applying changes on LightingEffect");
    }
}

[CustomEditor(typeof(LightingEffect_Speed))]
public class LightingEffect_Speed_Editor : LightingEffect_Base_Editor<LightingEffect_Speed>
{
    // 自定义创建LightingEffect_Speed对象的菜单
    [MenuItem("Assets/Create/LightingEffect/LightingEffect_Speed")]
    public static void Create()
    {
        CreateAsset();
    }

    public override void DoSaveAction()
    {
        LightingEffect ta = target as LightingEffect;
        LightingEffectFactory.LoadByEditor(ta.nID);
        Debug.Log("Applying changes on LightingEffect");
    }
}


//[CustomEditor(typeof(LightingEffect_MaterialReplace))]
//public class LightingEffect_MaterialReplace_Editor : LightingEffect_Base_Editor<LightingEffect_MaterialReplace>
//{
//    // 自定义创建LightingEffect_Speed对象的菜单
//    [MenuItem("Assets/Create/LightingEffect/LightingEffect_MaterialReplace")]
//    public static void Create()
//    {
//        CreateAsset();
//    }

//    public override void DoSaveAction()
//    {
//        LightingEffect ta = target as LightingEffect;
//        LightingEffectFactory.LoadByEditor(ta.nID);
//        Debug.Log("Applying changes on LightingEffect");
//    }
//}

[CustomEditor(typeof(LightingEffect_Sound))]
public class LightingEffect_Sound_Editor : LightingEffect_Base_Editor<LightingEffect_Sound>
{
    // 自定义创建LightingEffect_Speed对象的菜单
    [MenuItem("Assets/Create/LightingEffect/LightingEffect_Sound")]
    public static void Create()
    {
        CreateAsset();
    }

    public override void DoSaveAction()
    {
        LightingEffect ta = target as LightingEffect;
        LightingEffectFactory.LoadByEditor(ta.nID);
        Debug.Log("Applying changes on LightingEffect");
    }
}


