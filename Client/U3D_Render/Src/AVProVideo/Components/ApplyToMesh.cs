//#if UNITY_EDITOR || UNITY_STANDALONE_OSX || UNITY_STANDALONE_WIN
//    #define UNITY_PLATFORM_SUPPORTS_LINEAR
//#endif

//using UnityEngine;

////-----------------------------------------------------------------------------
//// Copyright 2015-2016 RenderHeads Ltd.  All rights reserverd.
////-----------------------------------------------------------------------------

//namespace RenderHeads.Media.AVProVideo
//{
//    [AddComponentMenu("AVPro Video/Apply To Mesh", 300)]
//    public class ApplyToMesh : MonoBehaviour 
//    {
//        // TODO: add specific material / material index to target in the mesh if there are multiple materials
//        public Vector2 _offset = Vector2.zero;
//        public Vector2 _scale = Vector2.one;
//        public Renderer _mesh;
//        public MediaPlayer _media;
//        public Texture2D _defaultTexture;
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
//            if (_mesh != null)
//            {
//                Material[] meshMaterials = _mesh.materials;
//                if (meshMaterials != null)
//                {
//                    for (int i = 0; i < meshMaterials.Length; i++)
//                    {
//                        Material mat = meshMaterials[i];
//                        if (mat != null)
//                        {
//                            mat.mainTexture = texture;

//                            if (texture != null)
//                            {
//                                if (requiresYFlip)
//                                {
//                                    mat.mainTextureScale = new Vector2(_scale.x, -_scale.y);
//                                    mat.mainTextureOffset = Vector2.up + _offset;
//                                }
//                                else
//                                {
//                                    mat.mainTextureScale = _scale;
//                                    mat.mainTextureOffset = _offset;
//                                }
//                            }


//                            if (_media != null)
//                            {
//                                // Apply changes for stereo videos
//                                if (mat.HasProperty(_propStereo))
//                                {
//                                    Helper.SetupStereoMaterial(mat, _media.m_StereoPacking, _media.m_DisplayDebugStereoColorTint);
//                                }
//                                // Apply changes for alpha videos
//                                if (mat.HasProperty(_propAlphaPack))
//                                {
//                                    Helper.SetupAlphaPackedMaterial(mat, _media.m_AlphaPacking);
//                                }
//#if UNITY_PLATFORM_SUPPORTS_LINEAR
//                                // Apply gamma
//                                if (mat.HasProperty(_propApplyGamma) && _media.Info != null)
//                                {
//                                    Helper.SetupGammaMaterial(mat, _media.Info.PlayerSupportsLinearColorSpace());
//                                }
//#else
//                                _propApplyGamma |= 0;
//#endif
//                            }
//                        }
//                    }
//                }
//            }
//        }

//        void OnEnable()
//        {
//            if (_mesh == null)
//            {
//                _mesh = this.GetComponent<MeshRenderer>();
//                if (_mesh == null)
//                {
//                    Debug.LogWarning("[AVProVideo] No mesh renderer set or found in gameobject");
//                }
//            }
			
//            if (_mesh != null)
//            {
//                LateUpdate();
//            }
//        }
		
//        void OnDisable()
//        {
//            ApplyMapping(_defaultTexture, false);
//        }
//    }
//}