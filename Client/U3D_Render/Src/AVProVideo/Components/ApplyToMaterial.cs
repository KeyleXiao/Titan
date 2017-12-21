//#if UNITY_EDITOR || UNITY_STANDALONE_OSX || UNITY_STANDALONE_WIN
//    #define UNITY_PLATFORM_SUPPORTS_LINEAR
//#endif

//using UnityEngine;

////-----------------------------------------------------------------------------
//// Copyright 2015-2016 RenderHeads Ltd.  All rights reserverd.
////-----------------------------------------------------------------------------

//namespace RenderHeads.Media.AVProVideo
//{
//    [AddComponentMenu("AVPro Video/Apply To Material", 300)]
//    public class ApplyToMaterial : MonoBehaviour 
//    {
//        public Vector2 _offset = Vector2.zero;
//        public Vector2 _scale = Vector2.one;
//        public Material _material;
//        public string _texturePropertyName;
//        public MediaPlayer _media;
//        public Texture2D _defaultTexture;

//        private Texture _originalTexture;
//        private Vector2 _originalScale = Vector2.one;
//        private Vector2 _originalOffset = Vector2.zero;

//        private static int _propStereo;
//        private static int _propAlphaPack;
//        private static int _propApplyGamma;

//        void Awake()
//        {
//            if (_propStereo == 0 || _propAlphaPack == 0)
//            {
//                _propStereo = Shader.PropertyToID("Stereo");
//                _propAlphaPack = Shader.PropertyToID("AlphaPack");
//                _propApplyGamma = Shader.PropertyToID("_ApplyGamma");
//            }
//        }

//        // We do a LateUpdate() to allow for any changes in the texture that may have happened in Update()
//        void LateUpdate()
//        {
//            bool applied = false;
//            if (_media != null && _media.TextureProducer != null)
//            {
//                Texture texture = _media.TextureProducer.GetTexture();
//                if (texture != null)
//                {
//                    ApplyMapping(texture, _media.TextureProducer.RequiresVerticalFlip());
//                    applied = true;
//                }
//            }

//            if (!applied)
//            {
//                ApplyMapping(_defaultTexture, false);
//            }
//        }
		
//        private void ApplyMapping(Texture texture, bool requiresYFlip)
//        {
//            if (_material != null)
//            {
//                if (string.IsNullOrEmpty(_texturePropertyName))
//                {
//                    _material.mainTexture = texture;

//                    if (texture != null)
//                    {
//                        if (requiresYFlip)
//                        {
//                            _material.mainTextureScale = new Vector2(_scale.x, -_scale.y);
//                            _material.mainTextureOffset = Vector2.up + _offset;
//                        }
//                        else
//                        {
//                            _material.mainTextureScale = _scale;
//                            _material.mainTextureOffset = _offset;
//                        }
//                    }
//                }
//                else
//                {
//                    _material.SetTexture(_texturePropertyName, texture);

//                    if (texture != null)
//                    {
//                        if (requiresYFlip)
//                        {
//                            _material.SetTextureScale(_texturePropertyName, new Vector2(_scale.x, -_scale.y));
//                            _material.SetTextureOffset(_texturePropertyName, Vector2.up + _offset);
//                        }
//                        else
//                        {
//                            _material.SetTextureScale(_texturePropertyName, _scale);
//                            _material.SetTextureOffset(_texturePropertyName, _offset);
//                        }
//                    }
//                }

//                if (_media != null)
//                {
//                    // Apply changes for stereo videos
//                    if (_material.HasProperty(_propStereo))
//                    {
//                        Helper.SetupStereoMaterial(_material, _media.m_StereoPacking, _media.m_DisplayDebugStereoColorTint);
//                    }
//                    // Apply changes for alpha videos
//                    if (_material.HasProperty(_propAlphaPack))
//                    {
//                        Helper.SetupAlphaPackedMaterial(_material, _media.m_AlphaPacking);
//                    }
//#if UNITY_PLATFORM_SUPPORTS_LINEAR
//                    // Apply gamma
//                    if (_material.HasProperty(_propApplyGamma) && _media.Info != null)
//                    {
//                        Helper.SetupGammaMaterial(_material, _media.Info.PlayerSupportsLinearColorSpace());
//                    }
//#else
//                    _propApplyGamma |= 0;
//#endif
//                }
//            }
//        }

//        void OnEnable()
//        {
//            if (string.IsNullOrEmpty(_texturePropertyName))
//            {
//                _originalTexture = _material.mainTexture;
//                _originalScale = _material.mainTextureScale;
//                _originalOffset = _material.mainTextureOffset;
//            }
//            else
//            {
//                _originalTexture = _material.GetTexture(_texturePropertyName);
//                _originalScale = _material.GetTextureScale(_texturePropertyName);
//                _originalOffset = _material.GetTextureOffset(_texturePropertyName);
//            }

//            LateUpdate();
//        }
		
//        void OnDisable()
//        {
//            if (string.IsNullOrEmpty(_texturePropertyName))
//            {
//                _material.mainTexture = _originalTexture;
//                _material.mainTextureScale = _originalScale;
//                _material.mainTextureOffset = _originalOffset;
//            }
//            else
//            {
//                _material.SetTexture(_texturePropertyName, _originalTexture);
//                _material.SetTextureScale(_texturePropertyName, _originalScale);
//                _material.SetTextureOffset(_texturePropertyName, _originalOffset);
//            }
//        }
//    }
//}