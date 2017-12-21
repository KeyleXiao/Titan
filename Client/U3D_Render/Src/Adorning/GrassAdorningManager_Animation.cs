using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using U3D_Render;

public partial class GrassAdorningManager : MonoBehaviour
{

    List<AnimationGrassAdorning> ani_grassadornings = new List<AnimationGrassAdorning>();
    void Awake_Animation()
    {
        Animator[] Animations = transform.GetComponentsInChildren<Animator>();
        foreach (Animator mr in Animations)
        {
            AnimationGrassAdorning ga = mr.gameObject.AddComponent<AnimationGrassAdorning>();
            ani_grassadornings.Add(ga);
            ga.Init();
        }

    }


    void PerformVisDetection_Ani(GameObject go)
    {
        foreach (AnimationGrassAdorning ga in ani_grassadornings)
        {
            ga.PerformVisDetection(go);
        }
    }


}
