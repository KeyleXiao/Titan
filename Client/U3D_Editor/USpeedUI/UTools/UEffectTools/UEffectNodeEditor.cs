using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEditor;
using UnityEditor.UI;
using UnityEngine;

namespace USpeedUI.UEffect
{
    [CustomEditor(typeof(UEffectNode))]
    class UEffectNodeEditor: SelectableEditor
    {
        UEffectNode targetEffectNode;
        GUIContent EffectNodeContent = new GUIContent();

        protected bool ShowRenderObjectList = true;


        protected override void OnEnable()
        {
            targetEffectNode = target as UEffectNode;
        }
        protected override void OnDisable()
        {

        }

        public override void OnInspectorGUI()
        {
            targetEffectNode.EffectType = (UEffectType)EditorGUILayout.EnumPopup(targetEffectNode.EffectType);
            if (targetEffectNode.EffectType == UEffectType.UET_None)
                return;
            switch (targetEffectNode.EffectType)
            {

                case UEffectType.UET_EffectMesh:
                    OnInspectorGUI_Mesh();
                    break;
                case UEffectType.UET_EffectPrefab:
                    OnInspectorGUI_Prefab();
                    break;
                case UEffectType.UET_EffectMaterial:
                    OnInspectorGUI_Material();
                    break;
                default:
                    break;
            }
        }

        private void OnInspectorGUI_Mesh()
        {
            var node = targetEffectNode.To<UEffectNodeData_Mesh>();
            if (targetEffectNode != null && node != null)
            {
                EffectNodeContent.text = "相机渲染图片尺寸";
                EffectNodeContent.tooltip = "要屎人咧超过 " + UDefines.USpeed_Screen_Width + "x" + UDefines.USpeed_Screen_Height + "的大小，尺寸越大消耗也会增加。RawTexture的尺寸也要跟渲染图片的尺寸成比例，否则会拉伸";
                node.MeshRTTSize = EditorGUILayout.Vector2Field(EffectNodeContent, node.MeshRTTSize);

                if (node.MeshRTTSize.x > UDefines.USpeed_Screen_Width || node.MeshRTTSize.y > UDefines.USpeed_Screen_Height)
                {
                    node.MeshRTTSize = new Vector2(UDefines.USpeed_Screen_Width, UDefines.USpeed_Screen_Height);
                }

                EffectNodeContent.text = "渲染图片尺寸Max";
                EffectNodeContent.tooltip = "渲染图片尺寸能小的，就别用大的，尺寸越大消耗也会增加。";
                if (GUILayout.Button(EffectNodeContent))
                {
                    node.MeshRTTSize = new Vector2(UDefines.USpeed_Screen_Width, UDefines.USpeed_Screen_Height);
                }

                node.EffectMeshCamera.FOV = EditorGUILayout.Slider("相机FOV", node.EffectMeshCamera.FOV, 1.0f, 179.0f);
                node.EffectMeshCamera.PositionOffset = EditorGUILayout.Vector3Field("相机坐标偏移", node.EffectMeshCamera.PositionOffset);
                node.EffectMeshCamera.Rotation = EditorGUILayout.Vector3Field("相机旋转角度", node.EffectMeshCamera.Rotation);
                node.EffectMeshCamera.IsAttachToObject = EditorGUILayout.Toggle("是否把相机绑定到观察对象上", node.EffectMeshCamera.IsAttachToObject);

                if (node.EffectMeshCamera.IsAttachToObject)
                {
                    GUI.color = Color.green;
                    node.EffectMeshCamera.AttachObject = (GameObject)EditorGUILayout.ObjectField("相机绑定的对象", node.EffectMeshCamera.AttachObject, typeof(GameObject), true);
                    GUI.color = Color.white;
                }
                else if (!node.EffectMeshCamera.IsAttachToObject && node.EffectMeshCamera.AttachObject != null)
                {
                    node.EffectMeshCamera.AttachObject = null;
                }

                if (GUILayout.Button("要增加渲染对象的话点我"))
                {
                    node.EffectMeshObjectList.Add(new UEffectMeshObject());
                }

                if (node.EffectMeshObjectList.Count > 0)
                {
                    ShowRenderObjectList = GUILayout.Toggle(ShowRenderObjectList, "渲染对象队列", "Foldout", GUILayout.ExpandWidth(true));
                    if (ShowRenderObjectList)
                    {
                        EditorGUILayout.BeginVertical();

                        RenderObjectGUI();

                        EditorGUILayout.EndVertical();
                    }
                }
            }

        }

        protected void RenderObjectGUI()
        {
            int nRemoveIndex = -1;
            var node = targetEffectNode.To<UEffectNodeData_Mesh>();
            if (node == null)
                return;
            foreach (var renderObj in node.EffectMeshObjectList)
            {
                EditorGUILayout.BeginVertical("box");

                GUI.color = (node.EffectMeshCamera.AttachObject != null && (renderObj.RenderObject == node.EffectMeshCamera.AttachObject)) ? Color.green : Color.white;

                renderObj.RenderObject = (GameObject)EditorGUILayout.ObjectField("相机观察的模型对象", renderObj.RenderObject, typeof(GameObject), true);
                if (GUILayout.Button("要移除渲染对象的话点我"))
                    nRemoveIndex = node.EffectMeshObjectList.IndexOf(renderObj);
                GUI.color = Color.white;

                if (renderObj.RenderObject != null)
                {
                    EditorGUILayout.BeginHorizontal();
                    EffectNodeContent.text = "模型对象是否是场景对象";
                    EffectNodeContent.tooltip = "模型对象是不是需要实例化，场景对象不需要实例化，我会直接拿来用。";
                    renderObj.RenderObjectIsSceneObject = EditorGUILayout.Toggle(EffectNodeContent, renderObj.RenderObjectIsSceneObject);

                    EditorGUILayout.EndHorizontal();
                    renderObj.RenderObjectPostion = EditorGUILayout.Vector3Field("模型对象相对相机偏移值", renderObj.RenderObjectPostion);
                    renderObj.RenderObjectRotation = EditorGUILayout.Vector3Field("模型对象的旋转角度", renderObj.RenderObjectRotation);
                    renderObj.RenderObjectScale = EditorGUILayout.Vector3Field("模型对象缩放值", renderObj.RenderObjectScale);
                }
                EditorGUILayout.EndVertical();
                EditorGUILayout.Space();
            }
            if (nRemoveIndex != -1)
            {
                node.EffectMeshObjectList.RemoveAt(nRemoveIndex);
            }
        }

        private void OnInspectorGUI_Prefab()
        {
            var node = targetEffectNode.To<UEffectNodeData_Prefab>();
            if(node != null)
            {
                node.EffectType = (UEffectPrefabType)EditorGUILayout.EnumPopup(node.EffectType);
            }
        }

        private void OnInspectorGUI_Material()
        {
            var node = targetEffectNode.To<UEffectNodeData_Material>();
            if (node != null)
            {
                node.MateralType = (UEffectMaterialType)EditorGUILayout.EnumPopup(node.MateralType);
            }
            Material m = ((UEffectMaterialParam) node.MaterialParam).ReturnEffectMaterial;
            ShowShaderPropertys(m.shader, m);
        }

        private void ShowShaderPropertys(Shader s,Material m)
        {
            string propertyName = string.Empty;

            EditorGUI.BeginChangeCheck();

            for (int i = 0; i < ShaderUtil.GetPropertyCount(s); i++)
            {                
                if(!ShaderUtil.IsShaderPropertyHidden(s,i))
                {
                    EditorGUILayout.BeginHorizontal();

                    propertyName = ShaderUtil.GetPropertyName(s, i);
                    MaterialProperty materialProperty = MaterialEditor.GetMaterialProperty(new UnityEngine.Object[] { m }, propertyName);

                    EditorGUILayout.LabelField(ShaderUtil.GetPropertyDescription(s, i),GUILayout.Width(150));


                    switch (ShaderUtil.GetPropertyType(s, i))
                    {
                        case ShaderUtil.ShaderPropertyType.Color:
                            
                            Color c = EditorGUILayout.ColorField(m.GetColor(propertyName));
                            m.SetColor(propertyName, c);
                            break;
                        case ShaderUtil.ShaderPropertyType.Float:
                            float f = EditorGUILayout.FloatField(m.GetFloat(propertyName));
                            m.SetFloat(propertyName, f);
                            break;
                        case ShaderUtil.ShaderPropertyType.Range:                           
                            float min = ShaderUtil.GetRangeLimits(s, i, 1);
                            float max = ShaderUtil.GetRangeLimits(s, i, 2);
                            float r = EditorGUILayout.Slider(m.GetFloat(propertyName), min, max);
                            m.SetFloat(propertyName, r);
                            break;
                        case ShaderUtil.ShaderPropertyType.TexEnv:                          
                            EditorGUILayout.BeginVertical();
                            Texture2D tex = (Texture2D)EditorGUILayout.ObjectField(m.GetTexture(propertyName), typeof(Texture2D), false);
                            Vector2 texOffset = EditorGUILayout.Vector2Field("TextureOffset", m.GetTextureOffset(propertyName));
                            Vector2 texScale = EditorGUILayout.Vector2Field("TextureOffset", m.GetTextureScale(propertyName));
                            EditorGUILayout.EndVertical();
                            m.SetTexture(propertyName, tex);
                            m.SetTextureOffset(propertyName,texOffset);
                            m.SetTextureScale(propertyName, texScale);
                            break;
                        case ShaderUtil.ShaderPropertyType.Vector:
                            Vector4 v = EditorGUILayout.Vector4Field("", m.GetVector(propertyName));
                            m.SetVector(propertyName, v);
                            break;
                    }
                    EditorGUILayout.EndHorizontal();
                }
            }
            if (EditorGUI.EndChangeCheck())
            {
                serializedObject.ApplyModifiedProperties();
            }
                
        }
    }
}
