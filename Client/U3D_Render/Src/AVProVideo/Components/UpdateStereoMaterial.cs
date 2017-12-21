//using UnityEngine;

////-----------------------------------------------------------------------------
//// Copyright 2015-2016 RenderHeads Ltd.  All rights reserverd.
////-----------------------------------------------------------------------------

//namespace RenderHeads.Media.AVProVideo
//{
//    /// <summary>
//    /// In Unity 5.3.x and below there is no support for single pass VR stereo renering,
//    /// so this script is needed to send the camera position to the stereo shader so that
//    /// it can determine which eye it is rendering.  This script isn't needed for Unity 5.4
//    /// and above.
//    /// </summary>
//    [AddComponentMenu("AVPro Video/Update Stereo Material", 400)]
//    public class UpdateStereoMaterial : MonoBehaviour
//    {
//        public Camera _camera;
//        public MeshRenderer _renderer;
//        public Material _material;
//        private int _cameraPositionId;
//        private int _viewMatrixId;

//        void Awake()
//        {
//            _cameraPositionId = Shader.PropertyToID("_cameraPosition");
//            _viewMatrixId = Shader.PropertyToID("_ViewMatrix");
//            if (_camera == null)
//            {
//                Debug.LogWarning("[AVProVideo] No camera set for UpdateStereoMaterial component. If you are rendering in stereo then it is recommended to set this.");
//            }
//        }

//        // We do a LateUpdate() to allow for any changes in the camera position that may have happened in Update()
//        void LateUpdate()
//        {
//            Camera camera = _camera;
//            if (camera == null)
//            {
//                camera = Camera.main;
//            }
//            if (_renderer == null && _material == null)
//            {
//                _renderer = this.gameObject.GetComponent<MeshRenderer>();
//            }

//            if (camera != null)
//            {
//                if (_renderer != null)
//                {
//                    _renderer.material.SetVector(_cameraPositionId, camera.transform.position);
//                    _renderer.material.SetMatrix(_viewMatrixId, camera.worldToCameraMatrix.transpose);
//                }
//                if (_material != null)
//                {
//                    _material.SetVector(_cameraPositionId, camera.transform.position);
//                    _material.SetMatrix(_viewMatrixId, camera.worldToCameraMatrix.transpose);
//                }
//            }
//        }
//    }
//}