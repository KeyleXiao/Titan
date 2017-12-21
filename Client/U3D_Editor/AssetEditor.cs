using UnityEditor;
using UnityEngine;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System;
using UnityEditorInternal;


[InitializeOnLoad]
public class AssetEditor
{
    static AssetEditor()
    {
        AssetSystem.UnloadAssetEvent -= AssetEditor.UnloadAssetEditor;
        AssetSystem.UnloadAssetEvent += AssetEditor.UnloadAssetEditor;
    }

    private static void UnloadSingleAssetImp(UnityEngine.Object asset)
    {
        if (asset == null)
        {
            return;
        }

        System.Type assettype = asset.GetType();

        bool handled = false;
        for (int k = 0; k < 10; k++)
        {
            if (assettype == null)
            {
                break;
            }

            if (assettype == typeof(AnimationClip) ||
                assettype == typeof(Texture2D) ||
                assettype == typeof(Texture3D) ||
                assettype == typeof(UnityEditor.Animations.AnimatorController) ||
                assettype == typeof(UnityEditor.Animations.AnimatorStateMachine) ||
                assettype == typeof(UnityEditor.Animations.AnimatorState) ||
                assettype == typeof(Material) ||
                assettype == typeof(Shader) ||
                assettype == typeof(Avatar) ||
                assettype == typeof(ScriptableObject) ||
                assettype == typeof(Mesh))
            {
                Resources.UnloadAsset(asset);
                handled = true;
                break;
            }
            else if (assettype == typeof(GameObject)||
                assettype == typeof(Transform)||
                assettype == typeof(AssetBundle)||
                assettype == typeof(MonoScript) ||
                assettype == typeof(Component))
            {
                handled = true;
                break;
            }
            else
            {
            }

            assettype = assettype.BaseType;

        }

        if (handled == false)
        {
            Trace.Log("UnloadSingleAssetImp:" + asset);
            Resources.UnloadAsset(asset);
        }

    }

    //编辑器模式无法删除asset的临时解决办法
    public static void UnloadAssetEditor(UnityEngine.Object goasset)
    {
        UnityEngine.Object[] dependencies = EditorUtility.CollectDependencies(new UnityEngine.Object[] { goasset });
        foreach (UnityEngine.Object asset in dependencies)
        {
            UnloadSingleAssetImp(asset);
        }
        UnloadSingleAssetImp(goasset);
        Resources.UnloadUnusedAssets();
    }
}

