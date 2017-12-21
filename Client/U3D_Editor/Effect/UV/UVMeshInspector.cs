using UnityEditor;
using UnityEngine;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System;

namespace Effect
{
    [CustomEditor(typeof(UVMesh))]
    public class UVMeshInspector : Editor
    {
        public override void OnInspectorGUI()
        {
            base.OnInspectorGUI();

            UVMesh uvcomponent = (UVMesh)target;
            if (uvcomponent.IsPlay())
            {
                if (GUILayout.Button("Stop"))
                {
                    uvcomponent.Stop();
                }
            }
            else
            {
                if (GUILayout.Button("Play"))
                {
                    uvcomponent.Play();
                }
            }
        }
    }
}

