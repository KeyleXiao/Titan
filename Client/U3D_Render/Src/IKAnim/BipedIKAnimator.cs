using System;
using System.Collections.Generic;
using System.Text;
using UnityEngine;
using RootMotion;
using RootMotion.FinalIK;
using System.IO;

#if USE_TIMMER_BEHAVIOUR
public class BipedIKAnimator : UpdateBehaviourOnScreen
#else
public class BipedIKAnimator : MonoBehaviour
#endif

{
    public float a1 = 1.0f;
    public float a2 = 0.0f;

    Dictionary<string, CombinedBone> combinedbones = new Dictionary<string, CombinedBone>();
    private List<CombinedBone> BlendOrders = new List<CombinedBone>();

    void BuildBoneMapRecursive(Transform t, string key, ref Dictionary<string, Transform> bones)
    {
        bones[key] = t;

        int n = t.childCount;
        for (int i = 0; i < n; i++)
        {
            Transform tchild = t.GetChild(i);
            if (tchild != null)
            {
                BuildBoneMapRecursive(tchild,key+"/"+tchild.name, ref bones);
            }
        }
    }

    Dictionary<string, Transform> BuildBoneMap(GameObject go)
    {
        Dictionary<string, Transform> bones = new Dictionary<string, Transform>();

        Transform troot = go.transform.Find("Main");
        if (troot == null)
        {
            SkinnedMeshRenderer[] smrs = go.GetComponentsInChildren<SkinnedMeshRenderer>();
            foreach (SkinnedMeshRenderer smr in smrs)
            {
                foreach (Transform t in smr.bones)
                {
                    bones[t.name] = t;
                }
            }
        }
        else
        {
            BuildBoneMapRecursive(troot,troot.name, ref bones);
        }

        return bones;
    }

    void BuildOrderRecursive(Transform t, ref List<CombinedBone> bones)
    {
        CombinedBone cb = t.GetComponent<CombinedBone>();
        if (cb != null)
        {
            bones.Add(cb);
        }

        int n = t.childCount;
        for (int i = 0; i < n; i++)
        {
            Transform tchild = t.GetChild(i);
            if (tchild != null)
            {
                BuildOrderRecursive(tchild, ref bones);
            }
        }
    }

    void BuildBoneOrder()
    {
        BlendOrders.Clear();
        Transform troot = this.transform.Find("Main");
        if (troot == null)
        {
            troot = this.transform;
            BuildOrderRecursive(troot, ref BlendOrders);
        }
        else
        {
            BuildOrderRecursive(troot, ref BlendOrders);
        }
    }


    public GameObject goA = null;
    public GameObject goB = null;

    public delegate void ResetHanlder(BipedIKAnimator bipedikanimator);
    public static event ResetHanlder ResetEvent;


    public void Reset()
    {
        if (Application.isEditor && !Application.isPlaying)
        {
            if (ResetEvent != null)
            {
                ResetEvent(this);
            }
        }
    }

    public void Awake()
    {
        if (Application.isEditor && !Application.isPlaying)
        {
            Build();
        }
    }

    public void Build()
    {
        GameObject goC = this.gameObject;

        if (goA == null || goB == null || goC == null)
        {
            return;
        }

        Dictionary<string, Transform> bonesA = BuildBoneMap(goA);
        Dictionary<string, Transform> bonesB = BuildBoneMap(goB);
        Dictionary<string, Transform> bonesC = BuildBoneMap(goC);


        foreach (KeyValuePair<string, Transform> keypair in bonesC)
        {
            string bonename = keypair.Key;
            Transform t = keypair.Value;
            float y = t.position.y;

            if (bonesA.ContainsKey(bonename) && bonesB.ContainsKey(bonename))
            {
                CombinedBone cb = t.gameObject.AddComponent<CombinedBone>();
                cb.destc = t;
                cb.srca = bonesA[bonename];
                cb.srcb = bonesB[bonename];
                combinedbones[bonename] = cb;
            }
            else
            {
                Debug.Log("error bone=" + bonename);
            }

        }

        BuildBoneOrder();

        SetDefaultWeightsSetting1(combinedbones.ContainsKey("Main/RootCopy/Root_M/BackA_M"));

        BipedIK bipedik = GetComponent<BipedIK>();
        if (bipedik != null)
        {
            Transform troot = gameObject.transform.Find("Main");
            if (troot != null)
            {
                bipedik.references.root = troot;
            }
        }
    }

    public bool containBones(string boneName)
    {
        return combinedbones.ContainsKey("boneName");
    }

    private void CalcWeights()
    {
        //set default weights
        float miny = 99999999.0f;
        float maxy = -99999999.0f;
        foreach (KeyValuePair<string, CombinedBone> keypair in combinedbones)
        {
            CombinedBone cb = keypair.Value;
            Transform t = cb.transform;
            float y = t.position.y;
            if (y < miny) miny = y;
            if (y > maxy) maxy = y;
        }
        foreach (KeyValuePair<string, CombinedBone> keypair in combinedbones)
        {
            CombinedBone cb = keypair.Value;
            Transform t = cb.transform;
            float y = t.position.y;
            cb.posweight = 1.0f;
            if (Mathf.Abs(maxy - miny) > 0.0f)
            {
                cb.posweight = (y - miny) / (maxy - miny);
            }
            cb.posweight = (cb.posweight - 0.5f) * a1 + 0.5f + a2;
            cb.posweight = Mathf.Clamp01(cb.posweight);
            cb.rotationweight = cb.posweight;
            cb.scaleweight = cb.posweight;
        }
    }

    public void SetDefaultWeightsSetting1(bool hasRootCopy)
    {
        //set default weights
        CalcWeights();

        //apply weights if biped
        if (combinedbones.ContainsKey("Main"))
        {
            CombinedBone cb = combinedbones["Main"];
            cb.SetPositionWeightRecursive(0.0f);
            cb.SetPositionLockRecursive(false);
            cb.SetRotationWeightRecursive(0.0f);
            cb.SetRotationLockRecursive(false);
            cb.SetScaleWeightRecursive(0.0f);
            cb.SetScaleLockRecursive(false);
        }
        if (combinedbones.ContainsKey("Main/Root_M"))
        {
            CombinedBone cb = combinedbones["Main/Root_M"];
            cb.SetPositionWeight(0.0f);
            cb.SetPositionLockRecursive(false);
            cb.SetRotationWeight(0.8f);
            cb.SetRotationLockRecursive(false);
            cb.SetScaleWeight(0.0f);
            cb.SetScaleLockRecursive(false);
        }
        string bonesUp = "Main/Root_M/BackA_M";
        if (hasRootCopy)
        {
            bonesUp = "Main/RootCopy/Root_M/BackA_M";
        }        

        if (combinedbones.ContainsKey(bonesUp))
        {
            CombinedBone cb = combinedbones[bonesUp];
            cb.SetPositionWeightRecursive(1.0f);
            cb.SetPositionLockRecursive(true);
            cb.SetRotationWeightRecursive(1.0f);
            cb.SetRotationLockRecursive(true);
            cb.SetScaleWeightRecursive(1.0f);
            cb.SetScaleLockRecursive(true);
        }
 
    }



    public void SetDefaultWeightsSetting2(bool hasRootCopy)
    {
        //set default weights
        CalcWeights();

        //apply weights if biped
        if (combinedbones.ContainsKey("Main"))
        {
            CombinedBone cb = combinedbones["Main"];
            cb.SetPositionWeightRecursive(0.0f);
            cb.SetPositionLockRecursive(true);
            cb.SetRotationWeightRecursive(0.0f);
            cb.SetRotationLockRecursive(true);
            cb.SetScaleWeightRecursive(0.0f);
            cb.SetScaleLockRecursive(true); ;
        }
        string bonesUp = "Main/Root_M/BackA_M";
        if (hasRootCopy)
        {
            bonesUp = "Main/RootCopy/Root_M/BackA_M";
        }
        if (combinedbones.ContainsKey(bonesUp))
        {
            CombinedBone cb = combinedbones[bonesUp];
            cb.SetPositionWeightRecursive(1.0f);
            cb.SetPositionLockRecursive(true);
            cb.SetRotationWeightRecursive(1.0f);
            cb.SetRotationLockRecursive(true);
            cb.SetScaleWeightRecursive(1.0f);
            cb.SetScaleLockRecursive(true);
        }

    }


    public void SetDefaultWeightsSetting3(bool hasRootCopy)
    {
        //set default weights
        CalcWeights();

        //apply weights if biped
        if (combinedbones.ContainsKey("Main"))
        {
            CombinedBone cb = combinedbones["Main"];
            cb.SetPositionWeightRecursive(0.0f);
            cb.SetPositionLockRecursive(false);
            cb.SetRotationWeightRecursive(0.0f);
            cb.SetRotationLockRecursive(false);
            cb.SetScaleWeightRecursive(0.0f);
            cb.SetScaleLockRecursive(false);
        }
        string bonesUp = "Main/Root_M/BackA_M";
        if (hasRootCopy)
        {
            bonesUp = "Main/RootCopy/Root_M/BackA_M";
        }
        if (combinedbones.ContainsKey(bonesUp))
        {
            CombinedBone cb = combinedbones[bonesUp];
            cb.SetPositionWeightRecursive(1.0f);
            cb.SetPositionLockRecursive(true);
            cb.SetRotationWeightRecursive(1.0f);
            cb.SetRotationLockRecursive(false);
            cb.SetScaleWeightRecursive(1.0f);
            cb.SetScaleLockRecursive(false);
        }

    }

    public void SetAll(bool rotationlock,float rotationweight,bool poslock,float posweight)
    {
        if (combinedbones.ContainsKey("Main"))
        {
            CombinedBone cb = combinedbones["Main"];
            cb.SetPositionWeightRecursive(rotationweight);
            cb.SetPositionLockRecursive(rotationlock);
            cb.SetRotationWeightRecursive(posweight);
            cb.SetRotationLockRecursive(poslock);
            cb.SetScaleWeightRecursive(posweight);
            cb.SetScaleLockRecursive(poslock);
        }
    }

    //public bool blocal = false;
    public bool ikenable = true;

    void BlendBoneSelf(CombinedBone cb, Transform roota, Transform rootb, Transform rootc)
    {

        if (cb != null)
        {
            if (cb.rotationlock)//local
            {
                cb.RotationA = cb.srca.localRotation;
                cb.RotationB = cb.srcb.localRotation;
                cb.RotationC = Quaternion.Slerp(cb.RotationA, cb.RotationB, cb.rotationweight);
                cb.destc.localRotation = cb.RotationC;
            }
            else
            {
                cb.RotationA = Quaternion.Inverse(roota.rotation)*cb.srca.rotation ;
                cb.RotationB = Quaternion.Inverse(rootb.rotation)*cb.srcb.rotation;
                cb.RotationC = Quaternion.Slerp(cb.RotationA, cb.RotationB, cb.rotationweight);
                cb.destc.rotation = rootc.rotation * cb.RotationC;
            }

            if (cb.poslock)//local
            {
                cb.posA = cb.srca.localPosition;
                cb.posB = cb.srcb.localPosition;
                cb.posC=Vector3.Lerp(cb.posA, cb.posB, cb.posweight);
                cb.destc.localPosition = cb.posC;
            }
            else
            {
                cb.posA = roota.InverseTransformPoint(cb.srca.position);
                cb.posB = rootb.InverseTransformPoint(cb.srcb.position);
                cb.posC=Vector3.Lerp(cb.posA, cb.posB, cb.posweight);
                cb.destc.SetPosition(rootc.TransformPoint(cb.posC));
            }

            if (cb.scalelock)//local
            {
                cb.scaleA = cb.srca.localScale;
                cb.scaleB = cb.srcb.localScale;
                cb.scaleC=Vector3.Lerp(cb.scaleA, cb.scaleB, cb.scaleweight);
                cb.destc.localScale = cb.scaleC;
            }
            else
            {
                cb.scaleA = cb.srca.localScale;
                cb.scaleB = cb.srcb.localScale;
                cb.scaleC = Vector3.Lerp(cb.scaleA, cb.scaleB, cb.scaleweight);
                cb.destc.localScale = cb.scaleC;
            }
        }
    }

    void BlendBone(Transform tbone,Transform roota,Transform rootb,Transform rootc)
    {

        CombinedBone cb = tbone.GetComponent<CombinedBone>();
        BlendBoneSelf(cb, roota, rootb, rootc);

        int n = tbone.childCount;
        for (int i = 0; i < n; i++)
        {
            Transform tchild = tbone.GetChild(i);
            if (tchild != null)
            {
                BlendBone(tchild, roota,rootb,rootc);
            }
        }
    }
#if USE_TIMMER_BEHAVIOUR
    public override void OnUpdate()
#else
    public void Update()
#endif
    {
        GameObject goC = this.gameObject;
        if (!(goA == null || goB == null || goC == null))
        {
            //Transform troot = gameObject.transform.Find("Main");
            //if (troot == null)
            //{
            //    SkinnedMeshRenderer[] smrs = gameObject.GetComponentsInChildren<SkinnedMeshRenderer>();
            //    foreach (SkinnedMeshRenderer smr in smrs)
            //    {
            //        if (smr.rootBone != null)
            //        {
            //            BlendBone(smr.rootBone, goA.transform, goB.transform, goC.transform);
            //        }
            //    }
            //}
            //else
            //{
            //    BlendBone(troot, goA.transform, goB.transform, goC.transform);
            //}

            for (int i = 0; i < BlendOrders.Count; i++)
            {
                BlendBoneSelf(BlendOrders[i], goA.transform, goB.transform, goC.transform);
            }
        }


        if (!ikenable)
        {
            return;
        }
        //BipedIK一直都是null，注释掉，浪费性能
        //BipedIK bipedik= GetComponent<BipedIK>();
        //if (bipedik == null) return;

        
        //if (combinedbones.ContainsKey("LeftToes"))
        //{
        //    CombinedBone cb = combinedbones["LeftToes"];
        //    float weight = cb.posweight;
        //    Vector3 posA = cb.srca.position - cb.srca.root.position;
        //    Vector3 posB = cb.srcb.position - cb.srcb.root.position;
        //    Vector3 posC;
        //    posC.x = Mathf.Lerp(posA.x, posB.x, weight);
        //    posC.y = Mathf.Lerp(posA.y, posB.y, weight);
        //    posC.z = Mathf.Lerp(posA.z, posB.z, weight);
        //    Vector3 combinepos = posC + cb.destc.root.position;
        //    bipedik.solvers.leftFoot.IKPosition = combinepos;
        //    bipedik.solvers.leftFoot.IKPositionWeight = 1.0f;
        //    //bipedik.solvers.leftFoot.IKRotation = cb.transform.rotation;
        //    //bipedik.solvers.leftFoot.IKRotationWeight = 1.0f;
        //}
        
        //if (combinedbones.ContainsKey("RightToes"))
        //{
        //    CombinedBone cb = combinedbones["RightToes"];
        //    float weight = cb.posweight;
        //    Vector3 posA = cb.srca.position - cb.srca.root.position;
        //    Vector3 posB = cb.srcb.position - cb.srcb.root.position;
        //    Vector3 posC;
        //    posC.x = Mathf.Lerp(posA.x, posB.x, weight);
        //    posC.y = Mathf.Lerp(posA.y, posB.y, weight);
        //    posC.z = Mathf.Lerp(posA.z, posB.z, weight);
        //    Vector3 combinepos = posC + cb.destc.root.position;
        //    bipedik.solvers.rightFoot.IKPosition = combinepos;
        //    bipedik.solvers.rightFoot.IKPositionWeight = 1.0f;
        //    //bipedik.solvers.rightFoot.IKRotation = cb.transform.rotation;
        //    //bipedik.solvers.rightFoot.IKRotationWeight = 1.0f;
        //}

        //if (combinedbones.ContainsKey("LeftHand"))
        //{
        //    CombinedBone cb = combinedbones["LeftHand"];
        //    float weight = cb.posweight;
        //    Vector3 posA = cb.srca.position - cb.srca.root.position;
        //    Vector3 posB = cb.srcb.position - cb.srcb.root.position;
        //    Vector3 posC;
        //    posC.x = Mathf.Lerp(posA.x, posB.x, weight);
        //    posC.y = Mathf.Lerp(posA.y, posB.y, weight);
        //    posC.z = Mathf.Lerp(posA.z, posB.z, weight);
        //    Vector3 combinepos = posC + cb.destc.root.position;
        //    bipedik.solvers.leftHand.IKPosition = combinepos;
        //    bipedik.solvers.leftHand.IKPositionWeight = 1.0f;
        //}

        //if (combinedbones.ContainsKey("RightHand"))
        //{
        //    CombinedBone cb = combinedbones["RightHand"];
        //    float weight = cb.posweight;
        //    Vector3 posA = cb.srca.position - cb.srca.root.position;
        //    Vector3 posB = cb.srcb.position - cb.srcb.root.position;
        //    Vector3 posC;
        //    posC.x = Mathf.Lerp(posA.x, posB.x, weight);
        //    posC.y = Mathf.Lerp(posA.y, posB.y, weight);
        //    posC.z = Mathf.Lerp(posA.z, posB.z, weight);
        //    Vector3 combinepos = posC + cb.destc.root.position;
        //    bipedik.solvers.rightHand.IKPosition = combinepos;
        //    bipedik.solvers.rightHand.IKPositionWeight = 1.0f;
        //}

        ////if (combinedbones.ContainsKey("Left_Eye"))
        ////{
        ////    CombinedBone cb = combinedbones["Left_Eye"];
        ////    float weight = cb.posweight;
        ////    Vector3 posA = cb.srca.position - cb.srca.root.position;
        ////    Vector3 posB = cb.srcb.position - cb.srcb.root.position;
        ////    Vector3 posC;
        ////    posC.x = Mathf.Lerp(posA.x, posB.x, weight);
        ////    posC.y = Mathf.Lerp(posA.y, posB.y, weight);
        ////    posC.z = Mathf.Lerp(posA.z, posB.z, weight);
        ////    Vector3 combinepos = posC + cb.destc.root.position;
        ////    bipedik.solvers.lookAt.IKPosition = combinepos + cb.transform.right.normalized * 0.5f;
        ////    bipedik.solvers.lookAt.IKPositionWeight = 1.0f;
        ////}

        ////if (combinedbones.ContainsKey("Root_M"))
        ////{
        ////    CombinedBone cb = combinedbones["Root_M"];
        ////    float weight = cb.posweight;
        ////    Vector3 posA = cb.srca.position - cb.srca.root.position;
        ////    Vector3 posB = cb.srcb.position - cb.srcb.root.position;
        ////    Vector3 posC;
        ////    posC.x = Mathf.Lerp(posA.x, posB.x, weight);
        ////    posC.y = Mathf.Lerp(posA.y, posB.y, weight);
        ////    posC.z = Mathf.Lerp(posA.z, posB.z, weight);
        ////    Vector3 combinepos = posC + cb.destc.root.position;
        ////    bipedik.solvers.pelvis.SetPosition( combinepos;
        ////    bipedik.solvers.pelvis.positionWeight = 1.0f;
        ////}

        //if (combinedbones.ContainsKey("BackA_M"))
        //{
        //    CombinedBone cb = combinedbones["BackA_M"];
        //    float weight = cb.posweight;
        //    Vector3 posA = cb.srca.position - cb.srca.root.position;
        //    Vector3 posB = cb.srcb.position - cb.srcb.root.position;
        //    Vector3 posC;
        //    posC.x = Mathf.Lerp(posA.x, posB.x, weight);
        //    posC.y = Mathf.Lerp(posA.y, posB.y, weight);
        //    posC.z = Mathf.Lerp(posA.z, posB.z, weight);
        //    Vector3 combinepos = posC + cb.destc.root.position;
        //    //bipedik.solvers.spine.IKPosition = combinepos;
        //    //bipedik.solvers.spine.IKPositionWeight = 1.0f;

        //}
    }

    //BipedIK一直都是null，注释掉，浪费性能
//#if USE_TIMMER_BEHAVIOUR
//    public override void OnLateUpdate()
//#else
//    public void LateUpdate()
//#endif
//    {
//        if (!ikenable)
//        {
//            return;
//        }

//        BipedIK bipedik = GetComponent<BipedIK>();
//        if (bipedik == null) return;

//        bipedik.LateUpdateEx();

//    }

    public bool LoadSetting(string path)
    {
        if (path.Length <= 0)
        {
            Debug.Log("Load Bone Setting Fail, path is empty");
            return false;
        }

        if (!Application.isPlaying)
        {
            Debug.Log("Load Bone Setting Fail, please do it in playing mode!");
            return false;
        }

        if (File.Exists(path) == false)
        {
            return false;
        }

        CombinedBone[] cbs = this.GetComponentsInChildren<CombinedBone>();
        Dictionary<string, CombinedBone> cbmap = new Dictionary<string, CombinedBone>();
        foreach (CombinedBone cb in cbs)
        {
            cbmap[cb.name] = cb;
        }

        string text = File.ReadAllText(path);
        ScpReader sr = new ScpReader(text, path);
        int n = sr.GetRecordCount();
        for (int i = 0; i < n; i++)
        {
            string cmd = sr.GetString(i, 0, "");
            if (cmd != "bone")
            {
                continue;
            }

            string bonename = sr.GetString(i, 1, "");
            int rotationlock = sr.GetInt(i, 2, 0);
            float rotationweight = sr.GetFloat(i, 3, 0);
            int poslock = sr.GetInt(i, 4, 0);
            float posweight = sr.GetFloat(i, 5, 0);
            int scalelock = sr.GetInt(i, 6, 0);
            float scaleweight = sr.GetFloat(i, 7, 0);

            if (cbmap.ContainsKey(bonename))
            {
                CombinedBone cb = cbmap[bonename];
                cb.rotationlock = (rotationlock != 0);
                cb.rotationweight = rotationweight;
                cb.poslock = (poslock != 0);
                cb.posweight = posweight;
                cb.scalelock = (scalelock != 0);
                cb.scaleweight = scaleweight;
            }
        }

        return true;
    }

    public bool SaveSetting(string path)
    {
        if (path.Length <= 0)
        {
            Debug.Log("Save Bone Setting Fail, path is empty");
            return false;
        }

        if (!Application.isPlaying)
        {
            Debug.Log("Save Bone Setting Fail, please do it in playing mode!");
            return false;
        }

        CombinedBone[] cbs = this.GetComponentsInChildren<CombinedBone>();

        StreamWriter sw = new StreamWriter(path, false);
        sw.WriteLine("#,bonename,rotationlock,rotationweight,poslock,posweight");
        for (int i = 0; i < cbs.Length; i++)
        {
            CombinedBone cb = cbs[i];
            string sline = "bone" + ",";

            string bonename = cb.name;
            sline += bonename + ",";

            int rotationlock = cb.rotationlock ? 1 : 0;
            sline += rotationlock.ToString() + ",";

            float rotationweight = cb.rotationweight;
            sline += rotationweight.ToString() + ",";

            int poslock = cb.poslock ? 1 : 0;
            sline += poslock.ToString() + ",";

            float posweight = cb.posweight;
            sline += posweight.ToString() + ",";

            int scalelock = cb.scalelock ? 1 : 0;
            sline += scalelock.ToString() + ",";

            float scaleweight = cb.scaleweight;
            sline += scaleweight.ToString() + ",";

            sw.WriteLine(sline);
            sw.Flush();
        }

        sw.Close();

        return true;

    }

}


