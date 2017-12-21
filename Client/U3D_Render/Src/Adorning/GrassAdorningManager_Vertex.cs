using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using U3D_Render;

public partial class GrassAdorningManager : MonoBehaviour
{

    //List<GrassAdorning> grassadornings = new List<GrassAdorning>();
    void Awake_Vertex()
    {
        //MeshRenderer[] mrs = transform.GetComponentsInChildren<MeshRenderer>();
        //foreach (MeshRenderer mr in mrs)
        //{
        //    GrassAdorning ga = mr.gameObject.AddComponent<GrassAdorning>();
        //    grassadornings.Add(ga);
        //}
    }


    void PerformVisDetection_Vertex(GameObject go)
    {
        //foreach (GrassAdorning ga in grassadornings)
        //{
        //    ga.PerformVisDetection(go);
        //}
    }

}
