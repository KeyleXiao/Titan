using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace USpeedUI.UEffect
{
    public enum UEffectMaterialType
    {
        EMT_Gray,
        EMT_ToLinearColorSpace,
        EMT_Blur,
        EMT_Contrast
    }

    public partial class UEffectManager
    {
        protected Dictionary<UEffectMaterialType, UEffectMaterialDataParam> m_dicMaterialDataTable = new Dictionary<UEffectMaterialType, UEffectMaterialDataParam>()
        {
            {UEffectMaterialType.EMT_Gray,new UEffectMaterialDataParam("Gray") },
            {UEffectMaterialType.EMT_ToLinearColorSpace,new UEffectMaterialDataParam("ToLinearColorSpace") },
            {UEffectMaterialType.EMT_Blur,new UEffectMaterialDataParam("Blur") },
            {UEffectMaterialType.EMT_Contrast,new UEffectMaterialDataParam("Contrast") },
        };


        [Serializable]
        protected class UEffectMaterialDataParam
        {
            public string ShaderPath;

            /// <summary>
            /// 
            /// </summary>
            /// <param name="_strShaderPath">着色器资源声明的路径（以 UPath.UEffectPath为基准）</param>
            public UEffectMaterialDataParam(string _strShaderPath = "")
            {
                ShaderPath = UPath.UEffectPath + _strShaderPath;
            }
        }

        protected Dictionary<UEffectMaterialType, Shader> m_dicShaderTable = new Dictionary<UEffectMaterialType, Shader>();

        protected Dictionary<UEffectMaterialType, List<Material>> m_dicMaterialsTable = new Dictionary<UEffectMaterialType, List<Material>>();


        protected bool CreateMaterialEffect(UEffectMaterialParam _param)
        {
            if(!m_dicShaderTable.ContainsKey(_param.MaterialType))
            {
                Shader TypeShader = AssetBundleManager.GetShader(m_dicMaterialDataTable[_param.MaterialType].ShaderPath);

                if (TypeShader == null)
                {
                    Trace.LogWarning("不能找到名为:" + m_dicMaterialDataTable[_param.MaterialType].ShaderPath + "的shader");
                    return false;
                }

                m_dicShaderTable.Add(_param.MaterialType,TypeShader);
            }
            Material tmpMaterial;
            if(!_param.IsNewMaterial && m_dicMaterialsTable.ContainsKey(_param.MaterialType) && m_dicMaterialsTable[_param.MaterialType] != null && m_dicMaterialsTable[_param.MaterialType].Count > 0)
            {
                tmpMaterial = _param.ReturnEffectMaterial = m_dicMaterialsTable[_param.MaterialType][0];
                if(_param.GraphicsTarget != null)
                    _param.GraphicsTarget.material = tmpMaterial;
                return true;
            }

            _param.ReturnEffectMaterial = tmpMaterial = new Material(m_dicShaderTable[_param.MaterialType]);
            //tmpMaterial.hideFlags = HideFlags.do;
            if(m_dicMaterialsTable.ContainsKey(_param.MaterialType))
            {
                m_dicMaterialsTable[_param.MaterialType].Add(tmpMaterial);
            }
            else
                m_dicMaterialsTable.Add(_param.MaterialType, new List<Material>() { tmpMaterial });

            if (_param.GraphicsTarget != null)
                _param.GraphicsTarget.material = tmpMaterial;


            return true;
        }

        protected bool DestroyMaterialEffect(UEffectMaterialParam _param)
        {
            _param.GraphicsTarget.material = _param.GraphicsTarget.defaultMaterial;
            return false;
        }


    }
}
