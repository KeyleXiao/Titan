using UnityEngine;
using System.Collections.Generic;
using System.Collections;
using System.IO;

public class CameraNavNode:MonoBehaviour
{
    public Vector3 pos;
    public Vector3 angle;
    public Vector3 dir;
    public Vector3 tran;
    public float distance;
    public float fov;
    public float range;
    public bool locked;

    [HideInInspector]
    public int count;

    public void Update()
    {

        Matrix4x4 basem = Matrix4x4.TRS(Vector3.zero, Quaternion.Euler(angle), Vector3.one);
        Vector3 invviewdir = (basem * dir).normalized;
        Vector3 transformedtran = basem * tran;
        Vector3 lookatpos = transform.position + transformedtran;
        Vector3 eyepos = lookatpos + (invviewdir * distance);
        Vector3 viewdir = -invviewdir;
        Vector3 centerpos = (lookatpos + eyepos) * 0.5f;

        Transform tdir = transform.GetChild(0);
        tdir.localScale = new Vector3(0.1f, distance * 0.5f, 0.1f);
        tdir.SetPosition( centerpos);
        tdir.forward = viewdir;
        tdir.eulerAngles = tdir.eulerAngles + new Vector3(90.0f, 0.0f, 0.0f);

        Transform teye = transform.GetChild(1);
        teye.SetPosition( eyepos);
        teye.forward = viewdir;

        Transform tlookat = transform.GetChild(2);
        tlookat.SetPosition(lookatpos);

        Transform ta = teye.GetChild(0);
        Transform tb = teye.GetChild(1);

        ta.localEulerAngles = new Vector3(0.0f, fov * 0.5f, 0.0f);
        ta.localScale = new Vector3(0.2f, 0.2f, distance * 1.1f);
        ta.localPosition = new Vector3(0.0f,0.0f,0.0f);

        tb.localEulerAngles = new Vector3(0.0f, -fov * 0.5f, 0.0f);
        tb.localScale = new Vector3(0.2f, 0.2f, distance * 1.1f);
        tb.localPosition = new Vector3(0.0f,0.0f,0.0f);

        Transform taa = ta.GetChild(0);
        taa.localPosition = new Vector3(0.0f, 0.0f, 0.5f);
        taa.localScale = new Vector3(1.0f, 0.5f, 1.0f);

        Transform tbb = tb.GetChild(0);
        tbb.localPosition = new Vector3(0.0f, 0.0f, 0.5f);
        tbb.localScale = new Vector3(1.0f, 0.5f, 1.0f);

        Transform trange = transform.GetChild(3);
        trange.localScale = new Vector3(range, range, range);

    }
}
