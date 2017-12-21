using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

public class CombinedBone : MonoBehaviour
{
    public Transform srca;
    public Transform srcb;
    public Transform destc;

    public Quaternion RotationA;
    public Quaternion RotationB;
    public Quaternion RotationC;

    public Vector3 posA;
    public Vector3 posB;
    public Vector3 posC;

    public Vector3 scaleA;
    public Vector3 scaleB;
    public Vector3 scaleC;

    [HideInInspector]
    public float posweight;
    [HideInInspector]
    public bool poslock = true;

    [HideInInspector]
    public float rotationweight;
    [HideInInspector]
    public bool rotationlock = true;

    [HideInInspector]
    public float scaleweight;
    [HideInInspector]
    public bool scalelock = true;

    public void SetPositionWeightRecursive(float w)
    {
        posweight = w;
        CombinedBone[] childcbs = gameObject.GetComponentsInChildren<CombinedBone>();
        foreach (CombinedBone childcb in childcbs)
        {
            childcb.posweight = w;
        }
    }
    public void SetPositionWeight(float w)
    {
        posweight = w;
    }

    public void SetPositionLockRecursive(bool l)
    {
        poslock = l;
        CombinedBone[] childcbs = gameObject.GetComponentsInChildren<CombinedBone>();
        foreach (CombinedBone childcb in childcbs)
        {
            childcb.poslock = l;
        }
    }
    public void SetPositionLock(bool l)
    {
        poslock = l;
    }

    public void SetRotationWeightRecursive(float w)
    {
        rotationweight = w;
        CombinedBone[] childcbs = gameObject.GetComponentsInChildren<CombinedBone>();
        foreach (CombinedBone childcb in childcbs)
        {
            childcb.rotationweight = w;
        }
    }
    public void SetRotationWeight(float w)
    {
        rotationweight = w;
    }

    public void SetRotationLockRecursive(bool l)
    {
        rotationlock = l;
        CombinedBone[] childcbs = gameObject.GetComponentsInChildren<CombinedBone>();
        foreach (CombinedBone childcb in childcbs)
        {
            childcb.rotationlock = l;
        }
    }
    public void SetRotationLock(bool l)
    {
        rotationlock = l;
    }

    public void SetScaleWeightRecursive(float w)
    {
        scaleweight = w;
        CombinedBone[] childcbs = gameObject.GetComponentsInChildren<CombinedBone>();
        foreach (CombinedBone childcb in childcbs)
        {
            childcb.scaleweight = w;
        }
    }
    public void SetScaleWeight(float w)
    {
        scaleweight = w;
    }

    public void SetScaleLockRecursive(bool l)
    {
        scalelock = l;
        CombinedBone[] childcbs = gameObject.GetComponentsInChildren<CombinedBone>();
        foreach (CombinedBone childcb in childcbs)
        {
            childcb.scalelock = l;
        }
    }
    public void SetScaleLock(bool l)
    {
        scalelock = l;
    }
}

