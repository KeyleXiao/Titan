using UnityEditor;
using UnityEngine;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System;

[InitializeOnLoad]
[CustomEditor(typeof(BipedIKAnimator))]
public class CombinedSkinAnimatorInspector : Editor
{
    public static void OnBipedReset(BipedIKAnimator bipedikanimator)
    {
        string myname = bipedikanimator.gameObject.name;
        string subname = myname.Substring(0, myname.Length - 1);
        string nameA = subname + "A";
        string nameB = subname + "B";
        string nameC = subname + "C";

        bipedikanimator.goA = GameObject.Find(nameA);
        bipedikanimator.goB = GameObject.Find(nameB);

        if (bipedikanimator.goA == null)
        {
            GameObject prefab = (GameObject)PrefabUtility.GetPrefabParent(bipedikanimator.gameObject);
            if (prefab != null)
            {
                bipedikanimator.goA = (GameObject)GameObject.Instantiate(prefab);
                bipedikanimator.goA.name = nameA;
                bipedikanimator.goA.transform.position = bipedikanimator.gameObject.transform.position + new Vector3(-1, 0, 0);
            }
        }

        if (bipedikanimator.goB == null)
        {
            GameObject prefab = (GameObject)PrefabUtility.GetPrefabParent(bipedikanimator.gameObject);
            if (prefab != null)
            {
                bipedikanimator.goB = (GameObject)GameObject.Instantiate(prefab);
                bipedikanimator.goB.name = nameB;
                bipedikanimator.goB.transform.position = bipedikanimator.gameObject.transform.position + new Vector3(1, 0, 0);
            }
        }

        bipedikanimator.gameObject.name = nameC;
    }

    static CombinedSkinAnimatorInspector()
    {
        BipedIKAnimator.ResetEvent -= CombinedSkinAnimatorInspector.OnBipedReset;
        BipedIKAnimator.ResetEvent += CombinedSkinAnimatorInspector.OnBipedReset;
    }

    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        BipedIKAnimator csa = (BipedIKAnimator)target;

        GUILayout.BeginVertical("Box");

        if (GUILayout.Button("Set Default Weights Setting1"))
        {
            csa.SetDefaultWeightsSetting1(csa.containBones("Main/RootCopy/Root_M/BackA_M"));
        }
        if (GUILayout.Button("Set Default Weights Setting2"))
        {
            csa.SetDefaultWeightsSetting2(csa.containBones("Main/RootCopy/Root_M/BackA_M"));
        }
        if (GUILayout.Button("Set Default Weights Setting3"))
        {
            csa.SetDefaultWeightsSetting3(csa.containBones("Main/RootCopy/Root_M/BackA_M"));
        }
        if (GUILayout.Button("Load Bone Blend Config"))
        {
            string path = EditorUtility.OpenFilePanel("Load Bone Blend Config", Application.dataPath, "bfg");
            csa.LoadSetting(path);
        }

        if (GUILayout.Button("Save Bone Blend Config"))
        {
            string path = EditorUtility.SaveFilePanel("Save Bone Blend Config", Application.dataPath, "" ,"bfg");
            csa.SaveSetting(path);
        }

        GUILayout.EndVertical();
    }


}