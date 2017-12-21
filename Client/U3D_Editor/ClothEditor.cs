using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using System.IO;



public class ClothEditor : Editor
{

    //[MenuItem("Tools/Cloth/Build")]
    static void Build()
    {
        Debug.Log("cloth build begin");

        GameObject go = Selection.activeGameObject;
        if (go == null)
        {
            Debug.LogWarning("Selection.activeGameObject is null");
        }

        Cloth cloth = go.GetComponent<Cloth>();
        if (cloth == null)
        {
            Debug.LogWarning("cloth component is null");
            return;
        }

        Vector3[] positions = cloth.vertices;
        int n = positions.Length;
        float miny = 99999999.0f;
        float maxy = -99999999.0f;
        for (int i = 0; i < n; i++)
        {
            Vector3 pos = positions[i];
            if (pos.y < miny) miny = pos.y;
            if (pos.y > maxy) maxy = pos.y;
        }

        ClothSkinningCoefficient[] coefficients=cloth.coefficients;
        if (coefficients.Length!=positions.Length)
        {
            Debug.LogWarning("coefficients.Length!=positions.Length");
            return;
        }

        if (maxy-miny<0.01f)
        {
            Debug.LogWarning("maxy-miny<0.01f");
            return;
        }

        for (int i = 0; i < n; i++)
        {
            Vector3 pos = positions[i];
            float alpha = (pos.y - miny) / (maxy - miny);
            if (alpha > 0.7f) alpha = 1.0f;

            coefficients[i].maxDistance = 1.0f-alpha;
            coefficients[i].collisionSphereDistance = 0.0f;
        }
        cloth.coefficients = coefficients;

        Debug.Log(n.ToString());
    }
}