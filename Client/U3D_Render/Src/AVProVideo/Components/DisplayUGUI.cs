#if UNITY_4_6 || UNITY_4_7 || UNITY_4_8 || UNITY_5
#define UNITY_FEATURE_UGUI
#endif

#if UNITY_EDITOR || UNITY_STANDALONE_OSX || UNITY_STANDALONE_WIN
#define UNITY_PLATFORM_SUPPORTS_LINEAR
#endif

// Some older versions of Unity don't set the _TexelSize variable from uGUI so we need to set this manually
#if (!UNITY_5 || UNITY_5_0 || UNITY_5_1 || UNITY_5_2 || UNITY_5_3_0 || UNITY_5_3_1 || UNITY_5_3_2 || UNITY_5_3_3)
#define UNITY_UGUI_NOSET_TEXELSIZE
#endif

using System.Collections.Generic;
#if UNITY_FEATURE_UGUI
using UnityEngine;
using UnityEngine.UI;

//-----------------------------------------------------------------------------
// Copyright 2015-2016 RenderHeads Ltd.  All rights reserverd.
//-----------------------------------------------------------------------------

namespace RenderHeads.Media.AVProVideo
{
    [ExecuteInEditMode]
    [AddComponentMenu("AVPro Video/Display uGUI", 200)]
    public class DisplayUGUI : UnityEngine.UI.MaskableGraphic
    {
        [SerializeField]
        public MediaPlayer _mediaPlayer;

        [SerializeField]
        public Rect m_UVRect = new Rect(0f, 0f, 1f, 1f);

        [SerializeField]
        public bool _setNativeSize = false;

        [SerializeField]
        public ScaleMode _scaleMode = ScaleMode.ScaleToFit;

        [SerializeField]
        public bool _noDefaultDisplay = true;

        [SerializeField]
        public bool _displayInEditor = true;

        [SerializeField]
        public Texture _defaultTexture;

        private int _lastWidth;
        private int _lastHeight;
        private bool _flipY;
        private Texture _lastTexture;
        private static Shader _shaderStereoPacking;
        private static Shader _shaderAlphaPacking;
        private static int _propAlphaPack;
        private static int _propVertScale;
        private static int _propStereo;
        private static int _propApplyGamma;
#if UNITY_UGUI_NOSET_TEXELSIZE
        private static int _propMainTextureTexelSize;
#endif
        private bool _userMaterial = true;
        private Material _material;

        protected override void Awake()
        {
            if (_propAlphaPack == 0)
            {
                _propStereo = Shader.PropertyToID("Stereo");
                _propAlphaPack = Shader.PropertyToID("AlphaPack");
                _propVertScale = Shader.PropertyToID("_VertScale");
                _propApplyGamma = Shader.PropertyToID("_ApplyGamma");
#if UNITY_UGUI_NOSET_TEXELSIZE
                _propMainTextureTexelSize = Shader.PropertyToID("_MainTex_TexelSize");
#endif
            }

            if (_shaderAlphaPacking == null)
            {
                _shaderAlphaPacking = AssetBundleManager.GetShader("AVProVideo/UI/Transparent Packed");
            }
            if (_shaderStereoPacking == null)
            {
                _shaderStereoPacking = AssetBundleManager.GetShader("AVProVideo/UI/Stereo");
            }

            base.Awake();
        }

        protected override void Start()
        {
            _userMaterial = (this.m_Material != null);

            base.Start();
        }


        protected override void OnDestroy()
        {
            // Destroy existing material
            if (_material != null)
            {
                this.material = null;

#if UNITY_EDITOR
                Material.DestroyImmediate(_material);
#else
                Material.Destroy(_material);
#endif
                _material = null;
            }
            base.OnDestroy();
        }

        private Shader GetRequiredShader()
        {
            Shader result = null;

            switch (_mediaPlayer.m_StereoPacking)
            {
                case StereoPacking.None:
                    break;
                case StereoPacking.LeftRight:
                case StereoPacking.TopBottom:
                    result = _shaderStereoPacking;
                    break;
            }

            switch (_mediaPlayer.m_AlphaPacking)
            {
                case AlphaPacking.None:
                    break;
                case AlphaPacking.LeftRight:
                case AlphaPacking.TopBottom:
                    result = _shaderAlphaPacking;
                    break;
            }

#if UNITY_PLATFORM_SUPPORTS_LINEAR
            if (result == null && _mediaPlayer.Info != null)
            {
                if (QualitySettings.activeColorSpace == ColorSpace.Linear && !_mediaPlayer.Info.PlayerSupportsLinearColorSpace())
                {
                    result = _shaderAlphaPacking;
                }
            }
#endif

            return result;
        }

        /// <summary>
        /// Returns the texture used to draw this Graphic.
        /// </summary>
        public override Texture mainTexture
        {
            get
            {
                Texture result = Texture2D.whiteTexture;
                if (HasValidTexture())
                {
                    result = _mediaPlayer.TextureProducer.GetTexture();
                }
                else
                {
                    if (_noDefaultDisplay)
                    {
                        result = null;
                    }
                    else if (_defaultTexture != null)
                    {
                        result = _defaultTexture;
                    }

                    if (result == null && _displayInEditor)
                    {
#if UNITY_EDITOR
                        result = Resources.Load<Texture2D>("AVProVideoIcon");
#endif
                    }
                }
                return result;
            }
        }

        public bool HasValidTexture()
        {
            return (_mediaPlayer != null && _mediaPlayer.TextureProducer != null && _mediaPlayer.TextureProducer.GetTexture() != null);
        }

        private void UpdateInternalMaterial()
        {
            if (_mediaPlayer != null)
            {
                // Get required shader
                Shader currentShader = null;
                if (_material != null)
                {
                    currentShader = _material.shader;
                }
                Shader nextShader = GetRequiredShader();

                // If the shader requirement has changed
                if (currentShader != nextShader)
                {
                    // Destroy existing material
                    if (_material != null)
                    {
                        this.material = null;
#if UNITY_EDITOR
                        Material.DestroyImmediate(_material);
#else
                        Material.Destroy(_material);
#endif
                        _material = null;
                    }

                    // Create new material
                    if (nextShader != null)
                    {
                        _material = new Material(nextShader);
                    }
                }

                this.material = _material;
            }
        }

        // We do a LateUpdate() to allow for any changes in the texture that may have happened in Update()
        public void LateUpdate()
        {
            if (_setNativeSize)
            {
                SetNativeSize();
            }

            if (_lastTexture != mainTexture)
            {
                _lastTexture = mainTexture;
                SetVerticesDirty();
            }

            if (HasValidTexture())
            {
                if (mainTexture != null)
                {
                    if (mainTexture.width != _lastWidth || mainTexture.height != _lastHeight)
                    {
                        _lastWidth = mainTexture.width;
                        _lastHeight = mainTexture.height;
                        SetVerticesDirty();
                    }
                }
            }

            if (!_userMaterial && Application.isPlaying)
            {
                UpdateInternalMaterial();
            }

            if (material != null && _mediaPlayer != null)
            {
                // Apply changes for alpha videos
                if (material.HasProperty(_propAlphaPack))
                {
                    Helper.SetupAlphaPackedMaterial(material, _mediaPlayer.m_AlphaPacking);

                    if (_flipY && _mediaPlayer.m_AlphaPacking != AlphaPacking.None)
                    {
                        material.SetFloat(_propVertScale, -1f);
                    }
                    else
                    {
                        material.SetFloat(_propVertScale, 1f);
                    }

#if UNITY_UGUI_NOSET_TEXELSIZE
                    if (mainTexture != null)
                    {
                        material.SetVector(_propMainTextureTexelSize, new Vector4(1.0f / mainTexture.width, 1.0f / mainTexture.height, mainTexture.width, mainTexture.height));
                    }
#endif
                }

                // Apply changes for stereo videos
                if (material.HasProperty(_propStereo))
                {
                    Helper.SetupStereoMaterial(material, _mediaPlayer.m_StereoPacking, _mediaPlayer.m_DisplayDebugStereoColorTint);
                }
#if UNITY_PLATFORM_SUPPORTS_LINEAR
                if (material.HasProperty(_propApplyGamma) && _mediaPlayer.Info != null)
                {
                    Helper.SetupGammaMaterial(material, _mediaPlayer.Info.PlayerSupportsLinearColorSpace());
                }
#else
                _propApplyGamma |= 0;
#endif
            }

            SetMaterialDirty();
        }

        /// <summary>
        /// Texture to be used.
        /// </summary>
        public MediaPlayer CurrentMediaPlayer
        {
            get
            {
                return _mediaPlayer;
            }
            set
            {
                if (_mediaPlayer != value)
                {
                    _mediaPlayer = value;
                    //SetVerticesDirty();
                    SetMaterialDirty();
                }
            }
        }

        /// <summary>
        /// UV rectangle used by the texture.
        /// </summary>
        public Rect uvRect
        {
            get
            {
                return m_UVRect;
            }
            set
            {
                if (m_UVRect == value)
                {
                    return;
                }
                m_UVRect = value;
                SetVerticesDirty();
            }
        }

        /// <summary>
        /// Adjust the scale of the Graphic to make it pixel-perfect.
        /// </summary>

        [ContextMenu("Set Native Size")]
        public override void SetNativeSize()
        {
            Texture tex = mainTexture;
            if (tex != null)
            {
                int w = Mathf.RoundToInt(tex.width * uvRect.width);
                int h = Mathf.RoundToInt(tex.height * uvRect.height);

                if (_mediaPlayer != null)
                {
                    if (_mediaPlayer.m_AlphaPacking == AlphaPacking.LeftRight || _mediaPlayer.m_StereoPacking == StereoPacking.LeftRight)
                    {
                        w /= 2;
                    }
                    else if (_mediaPlayer.m_AlphaPacking == AlphaPacking.TopBottom || _mediaPlayer.m_StereoPacking == StereoPacking.TopBottom)
                    {
                        h /= 2;
                    }
                }

                rectTransform.anchorMax = rectTransform.anchorMin;
                rectTransform.sizeDelta = new Vector2(w, h);
            }
        }

        /// <summary>
        /// Update all renderer data.
        /// </summary>
        // OnFillVBO deprecated by 5.2
        // OnPopulateMesh(Mesh mesh) deprecated by 5.2 patch 1
#if UNITY_5 && !UNITY_5_0 && !UNITY_5_1 && !UNITY_5_2_0
        /*		protected override void OnPopulateMesh(Mesh mesh)
        {			
            List<UIVertex> verts = new List<UIVertex>();
            _OnFillVBO( verts );

            var quad = new UIVertex[4];
            for (int i = 0; i < vbo.Count; i += 4)
            {
                vbo.CopyTo(i, quad, 0, 4);
                vh.AddUIVertexQuad(quad);
            }
            vh.FillMesh( toFill );
        }*/

#if !UNITY_5_2_1
        protected override void OnPopulateMesh(VertexHelper vh)
        {
            vh.Clear();

            List<UIVertex> aVerts = new List<UIVertex>();
            _OnFillVBO(aVerts);

            // TODO: cache these indices since they never change?
            List<int> aIndicies = new List<int>(new int[] { 0, 1, 2, 2, 3, 0 });
            vh.AddUIVertexStream(aVerts, aIndicies);
        }
#endif
        [System.Obsolete("This method is not called from Unity 5.2 and above")]
#endif
        protected override void OnFillVBO(List<UIVertex> vbo)
        {
            _OnFillVBO(vbo);
        }

        private void _OnFillVBO(List<UIVertex> vbo)
        {
            _flipY = false;
            if (HasValidTexture())
            {
                _flipY = _mediaPlayer.TextureProducer.RequiresVerticalFlip();
            }

            Rect uvRect = m_UVRect;
            Vector4 v = GetDrawingDimensions(_scaleMode, ref uvRect);

            vbo.Clear();

            var vert = UIVertex.simpleVert;

            vert.position = new Vector2(v.x, v.y);
            vert.uv0 = new Vector2(uvRect.xMin, uvRect.yMin);
            if (_flipY)
            {
                vert.uv0 = new Vector2(uvRect.xMin, 1.0f - uvRect.yMin);
            }
            vert.color = color;
            vbo.Add(vert);

            vert.position = new Vector2(v.x, v.w);
            vert.uv0 = new Vector2(uvRect.xMin, uvRect.yMax);
            if (_flipY)
            {
                vert.uv0 = new Vector2(uvRect.xMin, 1.0f - uvRect.yMax);
            }
            vert.color = color;
            vbo.Add(vert);

            vert.position = new Vector2(v.z, v.w);
            vert.uv0 = new Vector2(uvRect.xMax, uvRect.yMax);
            if (_flipY)
            {
                vert.uv0 = new Vector2(uvRect.xMax, 1.0f - uvRect.yMax);
            }
            vert.color = color;
            vbo.Add(vert);

            vert.position = new Vector2(v.z, v.y);
            vert.uv0 = new Vector2(uvRect.xMax, uvRect.yMin);
            if (_flipY)
            {
                vert.uv0 = new Vector2(uvRect.xMax, 1.0f - uvRect.yMin);
            }
            vert.color = color;
            vbo.Add(vert);
        }

        private Vector4 GetDrawingDimensions(ScaleMode scaleMode, ref Rect uvRect)
        {
            Vector4 returnSize = Vector4.zero;

            if (mainTexture != null)
            {
                var padding = Vector4.zero;
                var textureSize = new Vector2(mainTexture.width, mainTexture.height);

                if (_mediaPlayer != null)
                {
                    if (_mediaPlayer.m_AlphaPacking == AlphaPacking.LeftRight || _mediaPlayer.m_StereoPacking == StereoPacking.LeftRight)
                    {
                        textureSize.x /= 2f;
                    }
                    else if (_mediaPlayer.m_AlphaPacking == AlphaPacking.TopBottom || _mediaPlayer.m_StereoPacking == StereoPacking.TopBottom)
                    {
                        textureSize.y /= 2f;
                    }
                }

                Rect r = GetPixelAdjustedRect();
                //				Debug.Log(string.Format("r:{2}, textureSize:{0}, padding:{1}", textureSize, padding, r));

                int spriteW = Mathf.RoundToInt(textureSize.x);
                int spriteH = Mathf.RoundToInt(textureSize.y);

                var size = new Vector4(padding.x / spriteW,
                                        padding.y / spriteH,
                                        (spriteW - padding.z) / spriteW,
                                        (spriteH - padding.w) / spriteH);


                {
                    if (textureSize.sqrMagnitude > 0.0f)
                    {
                        if (scaleMode == ScaleMode.ScaleToFit)
                        {
                            float spriteRatio = textureSize.x / textureSize.y;
                            float rectRatio = r.width / r.height;

                            if (spriteRatio > rectRatio)
                            {
                                float oldHeight = r.height;
                                r.height = r.width * (1.0f / spriteRatio);
                                r.y += (oldHeight - r.height) * rectTransform.pivot.y;
                            }
                            else
                            {
                                float oldWidth = r.width;
                                r.width = r.height * spriteRatio;
                                r.x += (oldWidth - r.width) * rectTransform.pivot.x;
                            }
                        }
                        else if (scaleMode == ScaleMode.ScaleAndCrop)
                        {
                            float aspectRatio = textureSize.x / textureSize.y;
                            float screenRatio = r.width / r.height;
                            if (screenRatio > aspectRatio)
                            {
                                float adjust = aspectRatio / screenRatio;
                                uvRect = new Rect(0f, (1f - adjust) * 0.5f, 1f, adjust);
                            }
                            else
                            {
                                float adjust = screenRatio / aspectRatio;
                                uvRect = new Rect(0.5f - adjust * 0.5f, 0f, adjust, 1f);
                            }
                        }
                    }
                }

                returnSize = new Vector4(r.x + r.width * size.x,
                                          r.y + r.height * size.y,
                                          r.x + r.width * size.z,
                                          r.y + r.height * size.w);

            }

            return returnSize;
        }
    }
}

#endif