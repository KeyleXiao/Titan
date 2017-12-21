using UnityEngine;
using UnityEditor;
using System.Reflection;
using System.Collections;
using System.Collections.Generic;
using UnityEditorInternal;
using System.IO;

//[CustomEditor(typeof(Effect_AnimatorBool))]

[CustomEditor(typeof(AnimationEffect))]
public class AnimationEffect_Editor : CustomInspector<AnimationEffect>
{
    [MenuItem("Assets/Create/AnimationEffect")]
    public static void Create()
    {
        CreateAsset();
    }

    public override void DoSaveAction()
    {
        AnimationEffectFactory.LoadByEditor(target.name);
        Debug.Log("Applying changes on AnimationEffect");
    }

}

