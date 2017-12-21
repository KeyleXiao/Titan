using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;  


// Adds a mesh collider to each game object that contains collider in its name		
class ColliderOnName : Editor 
{
   // [MenuItem("Tools/CreateCollider")]
    static void CreateCollider()
    {
        GameObject SceneRoot = GameObject.Find("ScenesMeshs");
        if (SceneRoot == null)
        {
            Debug.Log("can not find ScenesMeshs.");
            return;
        }

        Material zise = AssetDatabase.LoadAssetAtPath("Assets/UnityScene/zise.mat", typeof(Material)) as Material;
        Material baise = AssetDatabase.LoadAssetAtPath("Assets/UnityScene/baise.mat", typeof(Material)) as Material;

        Debug.Log(zise.ToString());

        GameObject ColliderRoot = new GameObject();
        ColliderRoot.name="ScenesColliders";

        ColliderRoot.transform.position = SceneRoot.transform.position;
        ColliderRoot.transform.rotation = SceneRoot.transform.rotation;
        ColliderRoot.transform.localScale = SceneRoot.transform.localScale;

        for (int i = 0; i < SceneRoot.transform.childCount; i++)
        {
            Transform child = SceneRoot.transform.GetChild(i);


            string name = child.name;
            if (name.Length > 6 && name.Substring(0, 6) == "caodui")
            {
                continue;
            }

            BoxCollider mc = child.gameObject.GetComponent<BoxCollider>();
            if (mc == null)
            {
                BoxCollider[] bcs = child.gameObject.GetComponentsInChildren<BoxCollider>();

                if (bcs.Length > 1)
                {
                    string log = child.name + " has more than 1 boxCollider";
                    foreach (BoxCollider box in bcs)
                    {
                        log += '"' + box.transform.parent.name + '"';
                        Debug.LogWarning(log);
                    }
                }
                else if (bcs.Length == 1)
                {
                    mc = bcs[0];
                }
            }


            Vector3 size=Vector3.zero;
            Vector3 center=Vector3.zero;
            Transform boxTransform;

            if (mc == null)
            {
                //Debug.Log(child.name + " need Collider");

                Renderer[] renderers = child.GetComponentsInChildren<Renderer>();

                if (renderers.Length == 1)
                {
                    Renderer render = renderers[0];
                    center = render.bounds.center;
                    size = render.bounds.size;
                }
                else
                {
                    Vector3 minbound = Vector3.zero;
                    Vector3 maxbound = Vector3.zero;

                    foreach (Renderer renderer in renderers)
                    {
                        if ((minbound.sqrMagnitude + maxbound.sqrMagnitude) > 0.00001f)
                        {
                            minbound.x = Mathf.Min(minbound.x, renderer.bounds.min.x);
                            minbound.y = Mathf.Min(minbound.y, renderer.bounds.min.y);
                            minbound.z = Mathf.Min(minbound.z, renderer.bounds.min.z);

                            maxbound.x = Mathf.Max(maxbound.x, renderer.bounds.max.x);
                            maxbound.y = Mathf.Max(maxbound.y, renderer.bounds.max.y);
                            maxbound.z = Mathf.Max(maxbound.z, renderer.bounds.max.z);
                        }
                        else
                        {
                            minbound = renderer.bounds.min;
                            maxbound = renderer.bounds.max;
                        }
                    }

                    center = (minbound + maxbound) / 2.0f;
                    size = maxbound - minbound;
                }
                boxTransform = child.transform;
            }
            else
            {
                size = mc.size;
                center = mc.center;
                boxTransform = mc.transform;
            }

            Transform cTransform = coloneSubTree(child, boxTransform, ColliderRoot.transform);
            GameObject cube = GameObject.CreatePrimitive(PrimitiveType.Cube);
            Shader s = AssetBundleManager.GetShader("Transparent/Diffuse");


            if (mc != null)
            {
                cube.GetComponent<Renderer>().material = zise;
                cube.transform.parent = cTransform;
                cube.name = "colliderBox";
                cube.transform.localScale = size;
                cube.transform.localPosition = center;
                cube.transform.localRotation = Quaternion.identity;
                DestroyImmediate(child.gameObject);
                i--;
            }
            else
            {
                cube.GetComponent<Renderer>().material = baise;
                cube.name = "colliderBox";
                cube.transform.localScale = size;
                cube.transform.position = center;
                cube.transform.parent = cTransform;
                cube.transform.localRotation = Quaternion.identity;
            }

        }
    }

    static Transform coloneSubTree(Transform fromRoot, Transform fromEnd, Transform toRoot)
    {
        Stack<Transform> copyTreeStack=new Stack<Transform> ();
        while (fromRoot != fromEnd)
        {
            copyTreeStack.Push(fromEnd);
            fromEnd=fromEnd.parent;
        }
        copyTreeStack.Push(fromRoot);      

        while (copyTreeStack.Count>0)
        {
            Transform sorceNode = copyTreeStack.Pop();
            GameObject newNode = new GameObject();
            newNode.transform.parent = toRoot;
            newNode.name = sorceNode.name;
            newNode.transform.localPosition = sorceNode.localPosition;
            newNode.transform.localScale = sorceNode.localScale;
            newNode.transform.localRotation = sorceNode.localRotation;
            toRoot = newNode.transform;
        }

        return toRoot;
    }


    //[MenuItem("Tools/ClearCollider")]
    static void ClearCollider()
    {
        GameObject SceneRoot = GameObject.Find("ScenesMeshs");
        if (SceneRoot == null)
        {
            Debug.Log("can not find ScenesMeshs.");
            return;
        }

        GameObject ColliderRoot = GameObject.Find("ScenesColliders"); ;
        if (ColliderRoot == null)
        {
            return;
        }


        List<string> uneraseObjectNames=new List<string>();
        for (int i = 0; i < ColliderRoot.transform.childCount; i++)
        {
            Transform child = ColliderRoot.transform.GetChild(i);
            string name=child.name;
            if (!uneraseObjectNames.Contains(name))
            {
                if (child.childCount <= 0)
                {
                    uneraseObjectNames.Add(name);
                }
            }
        }

        foreach (string delname in uneraseObjectNames)
        {
            Debug.Log("delete..." + delname);
            for (int i = 0; i < ColliderRoot.transform.childCount; i++)
            {
                Transform child = ColliderRoot.transform.GetChild(i);
                if (child.name == delname)
                {
                    DestroyImmediate(child.gameObject);
                    i--;
                }
            }

        }

    }


    //[MenuItem("Tools/ClearMesh")]
    static void ClearMesh()
    {
        GameObject SceneRoot = GameObject.Find("ScenesMeshs");
        if (SceneRoot == null)
        {
            Debug.Log("can not find ScenesMeshs.");
            return;
        }

        GameObject ColliderRoot = GameObject.Find("ScenesColliders"); ;
        if (ColliderRoot == null)
        {
            return;
        }


        List<string> uneraseObjectNames = new List<string>();
        for (int i = 0; i < ColliderRoot.transform.childCount; i++)
        {
            Transform child = ColliderRoot.transform.GetChild(i);
            string name = child.name;
            if (!uneraseObjectNames.Contains(name))
            {
                uneraseObjectNames.Add(name);                
            }
        }

        foreach (string delname in uneraseObjectNames)
        {
            Debug.Log("delete..." + delname);
            for (int i = 0; i < SceneRoot.transform.childCount; i++)
            {
                Transform child = SceneRoot.transform.GetChild(i);
                if (child.name == delname)
                {
                    DestroyImmediate(child.gameObject);
                    i--;
                }
            }

        }

    }
}