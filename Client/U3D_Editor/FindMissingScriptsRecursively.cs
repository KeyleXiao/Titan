// --------------------------------------
//  Unity3d Mvvm Toolkit and Lobby
//  FindMissingScriptsRecursively.cs
//  copyright (c) 2014 Nicholas Ventimiglia, http://avariceonline.com
//  All rights reserved.
//  -------------------------------------
// 

using System;
using System.Collections.Generic;
using System.Reflection;
using UnityEditor;
using UnityEngine;

namespace Foundation.Editor
{
    public class FindMissingScriptsRecursively : EditorWindow
    {
        private static int go_count, components_count, missing_count;
        public static FindMissingScriptsRecursively instance;

        [MenuItem("Window/FindMissingScriptsRecursively")]
        public static void ShowWindow()
        {
            instance = GetWindow(typeof(FindMissingScriptsRecursively)) as FindMissingScriptsRecursively;
        }

        public void OnGUI()
        {
            if (GUILayout.Button("Find Missing Scripts in selected GameObjects"))
            {
                FindInSelected();
            }

            curComponent = EditorGUILayout.ObjectField(curComponent, typeof(MonoScript), false) as MonoScript;
            if (GUILayout.Button("�����������"))
            {
                FindComponentObject();
            }

        }

        private static void FindInSelected()
        {
            var go = Selection.gameObjects;
            go_count = 0;
            components_count = 0;
            missing_count = 0;
            foreach (var g in go)
            {
                FindInGO(g);
            }
            Debug.Log(string.Format("Searched {0} GameObjects, {1} components, found {2} missing", go_count,
                components_count, missing_count));
        }

        private static void FindInGO(GameObject g)
        {
            go_count++;
            var components = g.GetComponents<Component>();
            for (var i = 0; i < components.Length; i++)
            {
                components_count++;
                if (components[i] == null)
                {
                    missing_count++;
                    var s = g.name;
                    var t = g.transform;
                    while (t.parent != null)
                    {
                        s = t.parent.name + "/" + s;
                        t = t.parent;
                    }
                    Debug.Log(s + " has an empty script attached in position: " + i, g);
                }
            }
            // Now recurse through each child GO (if there are any):
            foreach (Transform childT in g.transform)
            {
                //Debug.Log("Searching " + childT.name  + " " );
                FindInGO(childT.gameObject);
            }
        }

        private static void FindComponentObject()
        {
            Debug.Log("��ʼ���ҡ�");
            ComponentCount = 0;
            var t = curComponent.GetClass();
            if (t == null)
            {
                Debug.LogWarning("û��ѡ����ҵ����");
                return;
            }

            string activeObjectPath = AssetDatabase.GetAssetPath(Selection.activeObject);
            if (string.IsNullOrEmpty(activeObjectPath))
            {
                Debug.LogWarning("û��ѡ����ҵ�·��");
                return;
            }
            //ѡ���ļ����µ���Դ
            foreach (var assets in System.IO.Directory.GetFiles(activeObjectPath))
            {
                GetObject(assets);
            }
            //ѡ���ļ��������ļ�����Դ
            foreach (var path in System.IO.Directory.GetDirectories(activeObjectPath))
            {
                foreach (var assets in System.IO.Directory.GetFiles(path))
                {
                    GetObject(assets);
                }
            }

            Debug.Log("������ϡ��ܹ���"+ ComponentCount);
        }
        protected static MonoScript curComponent;
        protected static int ComponentCount;

        private static void GetObject(string path)
        {
            var objs = AssetDatabase.LoadAssetAtPath<GameObject>(path);
            if (objs != null)
            {
                IteGetGameObject(path, objs);
            }
        }
        private static void IteGetGameObject(string file,GameObject Obj)
        {
            GetComponent(file, Obj);

            for (int i = 0; i < Obj.transform.childCount; i++)
            {
                IteGetGameObject(file, Obj.transform.GetChild(i).gameObject);
            }
        }
        private static void GetComponent(string file, GameObject obj)
        {
            var com = obj.GetComponent(curComponent.GetClass());
            if (com != null)
            {
                ComponentCount++;
                Debug.Log("path:" + file + ",name:" + obj.name);
            }
        }




        [MenuItem("Assets/����Ԥ����·��")]
        public static void GetPerfabPath()
        {
            UnityEngine.Object[] ObjToPackage = Selection.GetFiltered(typeof(UnityEngine.Object), SelectionMode.DeepAssets);

            System.Text.StringBuilder sb = new System.Text.StringBuilder();

            foreach (UnityEngine.Object o in ObjToPackage)
            {
                string path = AssetDatabase.GetAssetPath(o);
                if(path.Contains("Assets/Prefabs/"))
                {
                    string RelativePathToPerfab = ConverRelativePatchToPerfabPatch(path);
                    if (System.IO.Directory.Exists(RelativePathToPerfab))
                    {
                        continue;
                    }

                    sb.AppendLine(RelativePathToPerfab);
                }
                else
                {
                    Debug.LogWarning("Ŀǰ�޷���ȡ�� Assets/Prefabs/�ڵ�·��");
                }
            }
            if(sb.Length > 0)
            {
                GUIUtility.systemCopyBuffer = sb.ToString();
                Debug.Log("·�������ɹ�");
            }
            
        }
        public static string ConverRelativePatchToPerfabPatch(string ObslotePatch)
        {
            string result = string.Empty;

            result = ObslotePatch;
            result = result.Replace("Assets/Prefabs/", "");
            result = result.Split('.')[0];
            return result;
        }
    }

    

}