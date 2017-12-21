
using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI.UEffect
{
    [AddComponentMenu("UI/Effects/UGradient")]
    public class UGradient : BaseMeshEffect
    {
        [SerializeField]
        protected UGradientDir _gradientType;

        [SerializeField]
        protected UGradientBlendType _blendMode = UGradientBlendType.Multiply;

        [SerializeField]
        [Range(-1, 1)]
        protected float _offset = 0f;

        [SerializeField]
        protected UnityEngine.Gradient _effectGradient = new UnityEngine.Gradient() { colorKeys = new GradientColorKey[] { new GradientColorKey(Color.black, 0), new GradientColorKey(Color.white, 1) } };

        #region Properties
        public UGradientBlendType BlendMode
        {
            get { return _blendMode; }
            set { _blendMode = value; }
        }

        public UnityEngine.Gradient EffectGradient
        {
            get { return _effectGradient; }
            set { _effectGradient = value; }
        }

        public UGradientDir GradientType
        {
            get { return _gradientType; }
            set { _gradientType = value; }
        }

        public float Offset
        {
            get { return _offset; }
            set { _offset = value; }
        }
        #endregion

        
        private bool CompareCarefully(Color col1, Color col2)
        {
            if (Mathf.Abs(col1.r - col2.r) < 0.003f && Mathf.Abs(col1.g - col2.g) < 0.003f && Mathf.Abs(col1.b - col2.b) < 0.003f && Mathf.Abs(col1.a - col2.a) < 0.003f)
                return true;
            return false;
        }


        public override void ModifyMesh(VertexHelper helper)
        {
            if (!IsActive() || helper.currentVertCount == 0)
                return;

            List<UIVertex> _vertexList = new List<UIVertex>();

            helper.GetUIVertexStream(_vertexList);

            int nCount = _vertexList.Count;
            switch (GradientType)
            {
                case UGradientDir.UGD_Horizontal:
                    {
                        float left = _vertexList[0].position.x;
                        float right = _vertexList[0].position.x;
                        float x = 0f;

                        for (int i = nCount - 1; i >= 1; --i)
                        {
                            x = _vertexList[i].position.x;

                            if (x > right) right = x;
                            else if (x < left) left = x;
                        }

                        float width = 1f / (right - left);
                        UIVertex vertex = new UIVertex();

                        for (int i = 0; i < helper.currentVertCount; i++)
                        {
                            helper.PopulateUIVertex(ref vertex, i);

                            vertex.color = BlendColor(vertex.color, EffectGradient.Evaluate((vertex.position.x - left) * width - Offset));

                            helper.SetUIVertex(vertex, i);
                        }
                    }
                    break;

                case UGradientDir.UGD_Vertical:
                    {
                        float bottom = _vertexList[0].position.y;
                        float top = _vertexList[0].position.y;
                        float y = 0f;

                        for (int i = nCount - 1; i >= 1; --i)
                        {
                            y = _vertexList[i].position.y;

                            if (y > top) top = y;
                            else if (y < bottom) bottom = y;
                        }

                        float height = 1f / (top - bottom);
                        UIVertex vertex = new UIVertex();

                        for (int i = 0; i < helper.currentVertCount; i++)
                        {
                            helper.PopulateUIVertex(ref vertex, i);

                            vertex.color = BlendColor(vertex.color, EffectGradient.Evaluate((vertex.position.y - bottom) * height - Offset));

                            helper.SetUIVertex(vertex, i);
                        }
                    }
                    break;
            }
        }

        Color BlendColor(Color colorA, Color colorB)
        {
            switch (BlendMode)
            {
                default: return colorB;
                case UGradientBlendType.Add: return colorA + colorB;
                case UGradientBlendType.Multiply: return colorA * colorB;
            }
        }

        public void SetGradientColorKey(params Color[] _colors)
        {
            GradientColorKey[] newColorKeys = new GradientColorKey[_colors.Length];
            GradientAlphaKey[] newAlphaKeys = new GradientAlphaKey[_colors.Length];

            for (int i=0;i<_colors.Length;++i)
            {
                float time = i / (_colors.Length-1);
                newColorKeys[i].color = _colors[i];
                newColorKeys[i].time = time;

                newAlphaKeys[i].alpha = 1f;
                newAlphaKeys[i].time = time;
            }
            EffectGradient.SetKeys(newColorKeys, newAlphaKeys);

        }

        public void SetGradientAlphaKey(params float[] _alpha)
        {
            GradientColorKey[] newColorKeys = new GradientColorKey[_alpha.Length];
            GradientAlphaKey[] newAlphaKeys = new GradientAlphaKey[_alpha.Length];

            for (int i = 0; i < _alpha.Length; ++i)
            {
                float time = i / (_alpha.Length - 1);
                newColorKeys[i].color = i < EffectGradient.colorKeys.Length ? EffectGradient.colorKeys[i].color : Color.white;
                newColorKeys[i].time = time;

                newAlphaKeys[i].alpha = _alpha[i];
                newAlphaKeys[i].time = time;
            }
            EffectGradient.SetKeys(newColorKeys, newAlphaKeys);

        }

        public void SetGradientKey(Color[] _colors, float[] _alpha)
        {
            GradientColorKey[] newColorKeys = new GradientColorKey[_colors.Length];
            GradientAlphaKey[] newAlphaKeys = new GradientAlphaKey[_colors.Length];

            for (int i = 0; i < _colors.Length; ++i)
            {
                float time = i / (_colors.Length - 1);
                newColorKeys[i].color = _colors[i];
                newColorKeys[i].time = time;

                newAlphaKeys[i].alpha = _alpha[i];
                newAlphaKeys[i].time = time;
            }
            EffectGradient.SetKeys(newColorKeys, newAlphaKeys);
        }
    }

    /// <summary>
    /// 渐变方向
    /// </summary>
    public enum UGradientDir
    {
        UGD_Vertical,
        UGD_Horizontal,

        ///// <summary>
        ///// 对角方向左至右渐变
        ///// </summary>
        //UGD_DiagonalLeftToRight,
        ///// <summary>
        ///// 对角方向右至左渐变
        ///// </summary>
        //UGD_DiagonalRightToLeft
        //Free
    }

    public enum UGradientBlendType
    {
        Override,
        Add,
        Multiply
    }
}