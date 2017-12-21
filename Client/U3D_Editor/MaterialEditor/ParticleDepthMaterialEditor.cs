using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEditor;
using UnityEngine;

namespace UnityEditor
{
    internal class ParticleDepthMaterialEditor : ShaderGUI
    {
        public override void OnGUI(MaterialEditor materialEditor, MaterialProperty[] properties)
        {
            // render the default gui
            base.OnGUI(materialEditor, properties);

            Material targetMat = materialEditor.target as Material;
            
            EditorGUI.BeginChangeCheck();
            int queueOffset = targetMat.renderQueue - targetMat.shader.renderQueue;
            queueOffset = EditorGUILayout.IntField("RenderQueue Offset: ",queueOffset);

            if (EditorGUI.EndChangeCheck())
            {
                targetMat.renderQueue = targetMat.shader.renderQueue + queueOffset;
            }
        }
    }
}