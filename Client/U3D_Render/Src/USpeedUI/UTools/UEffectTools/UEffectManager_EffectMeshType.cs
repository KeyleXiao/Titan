using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using USpeedUI.UExtensions;

namespace USpeedUI.UEffect
{
    public partial class UEffectManager
    {
        /// <summary>
        /// RT相机根节点
        /// </summary>
        protected GameObject m_MeshEffectCameraRoot;
        /// <summary>
        /// 当前RT相机列表
        /// </summary>
        protected List<Camera> m_ListMeshEffectCameraGO;
        /// <summary>
        /// 效果计数
        /// </summary>
        protected int m_MeshEffectCount;



        protected bool CreateMeshEffect(UEffectMeshParam _effectParam)
        {
            UEffectNode node = _effectParam.AttachParent.GetComponent<UEffectNode>();
            if (node == null)
            {
                node = _effectParam.AttachParent.gameObject.AddComponent<UEffectNode>();
                node.EffectType = UEffectType.UET_EffectMesh;
            }                

            var nodeMesh = node.To<UEffectNodeData_Mesh>();

            if (node && nodeMesh != null && _effectParam.MeshObjectList.Count > 0) 
            {
                node.UnRegisterEffectNode();
                
                nodeMesh.MeshRTTSize = _effectParam.RTTSize;
                //node.GetRectTransform.sizeDelta = _effectParam.DisplaySize;

                nodeMesh.EffectMeshCamera.AttachObject = _effectParam.MeshCamrea.AttachObject;
                nodeMesh.EffectMeshCamera.IsAttachToObject = _effectParam.MeshCamrea.IsAttachToObject;
                nodeMesh.EffectMeshCamera.FOV = _effectParam.MeshCamrea.FOV;
                nodeMesh.EffectMeshCamera.PositionOffset = _effectParam.MeshCamrea.PositionOffset;
                nodeMesh.EffectMeshCamera.Rotation = _effectParam.MeshCamrea.Rotation;

                nodeMesh.EffectMeshObjectList.AddRange(_effectParam.MeshObjectList);

                node.RegisterEffectNode();

                _effectParam.MeshCamrea.Camera = nodeMesh.EffectMeshCamera.Camera;
                _effectParam.MeshRenderNode = node;
            }
            return true;
        }
        protected bool DestroyMeshEffect(UEffectMeshParam _effectParam)
        {
            UEffectNode node;
            if (_effectParam.MeshRenderNode != null)
            {
                node = _effectParam.MeshRenderNode;
            }                
            else
            {
                node = _effectParam.AttachParent.GetComponent<UEffectNode>();
            }
            if (node == null)
                return true;

            node.UnRegisterEffectNode();
            _effectParam.AttachParent.gameObject.RemoveComponent<UEffectNode>();


            return true;
        }

        public void RegisterEffectMeshRenderNode(UEffectNode _nodeRegister)
        {
            var _node = _nodeRegister.To<UEffectNodeData_Mesh>();

            if (_nodeRegister.EffectType == UEffectType.UET_EffectMesh && _node != null)
            {
                m_MeshEffectCount++;
                var tmpRT = GetRT((int)_node.MeshRTTSize.x, (int)_node.MeshRTTSize.y);
                var tmpRTCamera = GetRTCamera(_nodeRegister);
                _node.MeshDisplayImageRTT = tmpRT;
                _node.EffectMeshCamera.Camera = tmpRTCamera;
            }            
        }
        public void UnRegisterEffectMeshRenderNode(UEffectNode _nodeRegister)
        {
            var _node = _nodeRegister.To<UEffectNodeData_Mesh>();

            if (_nodeRegister.EffectType == UEffectType.UET_EffectMesh && _node != null)
            {
                if (!_node.EffectMeshCamera.IsAttachToObject)
                    RemoveAndSortCamera(_node.EffectMeshCamera.Camera);

                if (_node.MeshDisplayImageRTT != null)
                    ReleaseRT(_nodeRegister);
                if (_node.EffectMeshCamera.Camera != null)
                    ReleaseRTCamera(_nodeRegister);
            }
        }
        /// <summary>
        /// 移除相机的时候会重新整理相机位置
        /// </summary>
        /// <param name="_nRemoveCamera"></param>
        public void RemoveAndSortCamera(Camera _nRemoveCamera)
        {
            if (_nRemoveCamera == null || !m_ListMeshEffectCameraGO.Contains(_nRemoveCamera))
                return;
            int index = m_ListMeshEffectCameraGO.IndexOf(_nRemoveCamera);
            m_ListMeshEffectCameraGO.Remove(_nRemoveCamera);
            for (int i = index; i < m_ListMeshEffectCameraGO.Count; i++)
            {
                var preCam = m_ListMeshEffectCameraGO[i - 1 > 0 ? i - 1 : 0];
                var curCam = m_ListMeshEffectCameraGO[i];
                var newPos = preCam.transform.localPosition + new Vector3(0, 0, preCam.farClipPlane);
                curCam.transform.localPosition = newPos;
            }
        }
        public void AddCamera(Camera _nAddCamera)
        {
            if(_nAddCamera != null)
            {
                _nAddCamera.gameObject.transform.SetParent(m_MeshEffectCameraRoot.transform, false);

                if (m_ListMeshEffectCameraGO.Count > 0)
                {
                    var lastCamera = m_ListMeshEffectCameraGO[m_ListMeshEffectCameraGO.Count - 1];
                    var newPos = lastCamera.transform.localPosition + new Vector3(0, 0, lastCamera.farClipPlane);
                    _nAddCamera.gameObject.transform.localPosition = newPos;
                }

                m_ListMeshEffectCameraGO.Add(_nAddCamera);
            }
        }
        protected Camera GetRTCamera(UEffectNode _nodeRegister)
        {
            if (m_MeshEffectCameraRoot == null)
            {
                m_MeshEffectCameraRoot = new GameObject("UMeshEffectCameraRoot");

                m_MeshEffectCameraRoot.transform.SetPosition(new Vector3(0, 0, 0));
                m_ListMeshEffectCameraGO = new List<Camera>();
            }

            var newCameraGO = new GameObject("UMeshEffectCamera_" + m_MeshEffectCount);
            var newCamera = newCameraGO.AddComponent<Camera>();

            newCamera.clearFlags = CameraClearFlags.SolidColor;
            newCamera.backgroundColor = new Color();
            newCamera.cullingMask = 1 << LayerMask.NameToLayer(Config.LayerUI);
            newCamera.farClipPlane = 5;

            AddCamera(newCamera);

            return newCamera;
        }
        protected void ReleaseRTCamera(UEffectNode _nodeRegister)
        {
            var _node = _nodeRegister.To<UEffectNodeData_Mesh>();

            if (_nodeRegister.EffectType == UEffectType.UET_EffectMesh && _node != null)
            {
                RemoveAndSortCamera(_node.EffectMeshCamera.Camera);
                m_MeshEffectCount--;
                var temp = _node.EffectMeshCamera.Camera.gameObject;
                ResNode.DestroyRes(ref temp);
                _node.EffectMeshCamera.Camera = null;
            }
        }
        protected RenderTexture GetRT(int _nSizeX, int _nSizeY, int _nDepth = 24)
        {
            var tmpRT = new RenderTexture(_nSizeX, _nSizeY, _nDepth);//RenderTexture.GetTemporary(_nSizeX, _nSizeY, _nDepth);
            //tmpRT.antiAliasing = 4;
            return tmpRT;
        }
        protected void ReleaseRT(UEffectNode _nodeRegister)
        {
            //RenderTexture.ReleaseTemporary(_nodeRegister.MeshDisplayImageRTT);
            var _node = _nodeRegister.To<UEffectNodeData_Mesh>();

            if (_nodeRegister.EffectType == UEffectType.UET_EffectMesh && _node != null)
            {
                _node.MeshDisplayImageRTT.Release();
                _node.MeshDisplayImageRTT = null;

            }
        }

    }
}
