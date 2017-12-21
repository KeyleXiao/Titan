using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UExtensions;

namespace USpeedUI.UEffect
{
    /// <summary>
    /// 要渲染的对象参数
    /// </summary>
    [Serializable]
    public class UEffectMeshObject
    {
        [SerializeField]
        protected GameObject m_RenderObject;
        /// <summary>
        /// 渲染对象
        /// </summary>
        public GameObject RenderObject
        {
            get { return m_RenderObject; }
            set { m_RenderObject = value; }
        }
        [SerializeField]
        protected Vector3 m_RenderObjectPostion;
        /// <summary>
        /// 渲染对象相对父节点位置
        /// </summary>
        public Vector3 RenderObjectPostion
        {
            get { return m_RenderObjectPostion; }
            set
            {
                m_RenderObjectPostion = value;
                if (m_RenderObject != null)
                {
                    m_RenderObject.transform.localPosition = m_RenderObjectPostion;
                }
            }
        }
        [SerializeField]
        protected Vector3 m_RenderObjectRotation;
        /// <summary>
        /// 渲染对象相对父节点旋转角度
        /// </summary>
        public Vector3 RenderObjectRotation
        {
            get { return m_RenderObjectRotation; }
            set
            {
                m_RenderObjectRotation = value;
                if (m_RenderObject != null)
                {
                    m_RenderObject.transform.localEulerAngles = m_RenderObjectRotation;
                }
            }
        }
        [SerializeField]
        protected Vector3 m_RenderObjectScale = Vector3.one;
        /// <summary>
        /// 渲染对象相对父节点缩放
        /// </summary>
        public Vector3 RenderObjectScale
        {
            get { return m_RenderObjectScale; }
            set
            {
                m_RenderObjectScale = value;
                if (m_RenderObject != null)
                {
                    m_RenderObject.transform.localScale = m_RenderObjectScale;
                }
            }
        }
            [SerializeField]
            protected bool m_RenderObjectIsSceneObject;
        /// <summary>
        /// 渲染对象是否是场景上的对象
        /// true:直接拿场景对象是用
        /// false：根据渲染对象实例化一个新的实例是用
        /// </summary>
        public bool RenderObjectIsSceneObject
        {
            get { return m_RenderObjectIsSceneObject; }
            set { m_RenderObjectIsSceneObject = value; }
        }
        /// <summary>
        /// 渲染对象非场景对象，实例的一个引用
        /// </summary>
        [NonSerialized]
        public GameObject RenderObjectInstantiate;

        public UEffectMeshObject(GameObject _nRenderObj = null, Vector3? _vRenderPos = null, Vector3? _vRenderRot = null, Vector3? _vRenderScale = null, bool _bIsSceneObj = false)
        {
            RenderObject = _nRenderObj;
            RenderObjectPostion = _vRenderPos ?? Vector3.zero;
            RenderObjectRotation = _vRenderRot ?? Vector3.zero;
            RenderObjectScale = _vRenderScale ?? Vector3.one;
            RenderObjectIsSceneObject = _bIsSceneObj;
        }
    }
    /// <summary>
    /// 渲染相机参数
    /// </summary>
    [Serializable]
    public class UEffectMeshCamera
    {
        protected Camera m_RenderCamera;
        /// <summary>
        /// 渲染相机
        /// </summary>
        public Camera Camera
        {
            get { return m_RenderCamera; }
            set
            {
                m_RenderCamera = value;
            }
        }

        [SerializeField]
        protected float m_FOV = 60.0f;
        /// <summary>
        /// 渲染相机的FOV值
        /// </summary>
        public float FOV
        {
            get { return m_FOV; }
            set
            {
                if (m_FOV != value)
                {
                    m_FOV = value;
                    if (Camera != null)
                        Camera.fieldOfView = m_FOV;
                }
            }
        }

        [SerializeField]
        protected Vector3 m_PositionOffset;
        /// <summary>
        /// 渲染相机偏移值
        /// </summary>
        public Vector3 PositionOffset
        {
            get { return m_PositionOffset; }
            set
            {
                if (!Vector3.Equals(m_PositionOffset, value))
                {
                    m_PositionOffset = value;
                    if (Camera != null)
                    {
                        Camera.transform.SetPosition((Vector3)PositionDefault + m_PositionOffset);
                    }
                }
            }

        }

        protected Vector3? m_PositionDefault;
        /// <summary>
        /// 渲染相机默认坐标
        /// </summary>
        public Vector3? PositionDefault
        {
            get
            {
                return m_PositionDefault == null ? Vector3.zero : m_PositionDefault;
            }
            set
            {
                if (m_PositionDefault == null || value == null)
                {
                    m_PositionDefault = value;
                }
            }
        }
        [SerializeField]
        protected Vector3 m_Rotation;
        /// <summary>
        /// 渲染相机旋转值
        /// </summary>
        public Vector3 Rotation
        {
            get { return m_Rotation; }
            set
            {
                if (!Vector3.Equals(m_Rotation, value))
                {
                    m_Rotation = value;
                    if (Camera != null)
                    {

                        Camera.transform.localRotation = Quaternion.Euler(Rotation.x, Rotation.y, Rotation.z);
                    }
                }
            }
        }

        [SerializeField]
        protected bool m_IsAttachToObject;
        /// <summary>
        /// 渲染相机是否挂在某个对象上
        /// </summary>
        public bool IsAttachToObject
        {
            get { return m_IsAttachToObject; }
            set { m_IsAttachToObject = value; }
        }
        [SerializeField]
        protected GameObject m_AttachObject;
        /// <summary>
        /// 渲染相机挂接的对象
        /// </summary>
        public GameObject AttachObject
        {
            get { return m_AttachObject; }
            set { m_AttachObject = value; }
        }

        public UEffectMeshCamera(float _fFOV = 60f, Vector3? _vCamOffset = null, Vector3? _vCamRot = null, bool _bIsAttachToObj = false, GameObject _nAttachObj = null)
        {
            FOV = _fFOV;
            PositionOffset = _vCamOffset ?? Vector3.zero;
            Rotation = _vCamRot ?? Vector3.zero;
            IsAttachToObject = _bIsAttachToObj;
            AttachObject = _nAttachObj;
        }
    }


    [Serializable]
    public class UEffectNodeData_Mesh : UEffectNodeData
    {
        public UEffectNodeData_Mesh(UEffectNode _node) : base(_node)
        {
        }

        public override void DoAwake()
        {

        }

        public override void DoOnDestroy()
        {
            DoUnRegisterEffectNode();
        }

        public override void DoOnDisable()
        {

        }

        public override void DoOnEnable()
        {
            CacheMeshImage();
        }

        public override void DoRegisterEffectNode()
        {
            if (m_bIsRegister)
                return;

            UEffectManager.Instance.RegisterEffectMeshRenderNode(GetNode());

            AttachToObject();

            SetUpCamera();

            SetUpMeshDisplayGameObject();

            m_bIsRegister = true;
        }

        public override void DoStart()
        {
            CacheMeshImage();

            DoRegisterEffectNode();
        }

        public override void DoUnRegisterEffectNode()
        {
            if (!m_bIsRegister)
                return;

            UEffectManager.Instance.UnRegisterEffectMeshRenderNode(GetNode());
            ReleaseMeshDisplayGameObject();

            m_bIsRegister = false;
        }

        public override void DoUpdate()
        {

        }


        #region 属性
        [SerializeField]
        protected RawImage m_MeshDisplayImage;
        public RawImage MeshDisplayImage
        {
            get { return m_MeshDisplayImage; }
            set
            {
                m_MeshDisplayImage = value;
            }
        }
        public Material MeshDisplayImageMaterial
        {
            get
            { return MeshDisplayImage == null ? null : MeshDisplayImage.material; }
            set
            {
                if (MeshDisplayImage != null)
                    MeshDisplayImage.material = value;
            }
        }
        public RenderTexture MeshDisplayImageRTT
        {
            get { return MeshDisplayImage == null ? null : MeshDisplayImage.mainTexture as RenderTexture; }
            set
            {
                if (MeshDisplayImage != null)
                    MeshDisplayImage.texture = value;
            }
        }
        [SerializeField]
        protected Vector2 m_MeshRTTSize;
        public Vector2 MeshRTTSize
        {
            get
            {
                if (m_MeshRTTSize.magnitude == 0)
                    m_MeshRTTSize = GetNode().GetRectTransform.sizeDelta;
                return m_MeshRTTSize;
            }
            set
            {
                if (!float.Equals(m_MeshRTTSize, value))
                {
                    m_MeshRTTSize = value;
                    //GetRectTransform.sizeDelta = m_MeshRTTSize;
                    if (MeshDisplayImageRTT != null)
                    {
                        MeshDisplayImageRTT.width = (int)m_MeshRTTSize.x;
                        MeshDisplayImageRTT.height = (int)m_MeshRTTSize.y;
                    }
                }
            }
        }

        [SerializeField]
        protected UEffectMeshCamera m_EffectMeshCamera = new UEffectMeshCamera();
        public UEffectMeshCamera EffectMeshCamera
        {
            get { return m_EffectMeshCamera; }
        }

        [SerializeField]
        protected List<UEffectMeshObject> m_EffectMeshObjectList = new List<UEffectMeshObject>();
        public List<UEffectMeshObject> EffectMeshObjectList
        {
            get { return m_EffectMeshObjectList; }
        }

        #endregion
        private void CacheMeshImage()
        {
            if (MeshDisplayImage == null)
            {
                MeshDisplayImage = GetNode().GetComponent<RawImage>();
                if (MeshDisplayImage == null)
                    MeshDisplayImage = GetNode().gameObject.AddComponent<RawImage>();
            }
        }
        /// <summary>
        /// 初始化渲染对象列表
        /// </summary>
        public void SetUpMeshDisplayGameObject()
        {
            foreach (var renderObj in m_EffectMeshObjectList)
            {
                //不能将相机要绑定到的对象绑定给相机
                if (renderObj.RenderObject == EffectMeshCamera.AttachObject)
                    continue;

                if (renderObj.RenderObject != null)
                {
                    var tmpGameObject = renderObj.RenderObject;
                    if (!renderObj.RenderObjectIsSceneObject)
                    {
                        tmpGameObject = ResNode.InstantiateRes(tmpGameObject) as GameObject;
                        renderObj.RenderObjectInstantiate = tmpGameObject;
                    }


                    tmpGameObject.transform.SetParent(EffectMeshCamera.Camera.transform, false);
                    tmpGameObject.transform.localPosition = renderObj.RenderObjectPostion;
                    tmpGameObject.transform.localEulerAngles = renderObj.RenderObjectRotation;
                    tmpGameObject.transform.localScale = renderObj.RenderObjectScale;

                    tmpGameObject.ChangeLayersRecursively(Config.LayerUI);
                }

            }
        }
        protected void ReleaseMeshDisplayGameObject()
        {
            foreach (var renderObj in m_EffectMeshObjectList)
            {
                //MeshDisplayGameObject 指向的是场景里的对象
                if (renderObj.RenderObject != null && renderObj.RenderObjectIsSceneObject)
                {
                    renderObj.RenderObject = null;
                }
                else if (!renderObj.RenderObjectIsSceneObject && renderObj.RenderObjectInstantiate != null)
                {
                    ResNode.DestroyRes(ref renderObj.RenderObjectInstantiate);
                    renderObj.RenderObjectInstantiate = null;
                }
            }
            m_EffectMeshObjectList.Clear();
        }

        /// <summary>
        /// 相机绑定到_newAttachObject对象上
        /// </summary>
        /// <param name="_newAttachObject"></param>
        public void AttachToObject(GameObject _newAttachObject = null)
        {
            if (EffectMeshCamera.IsAttachToObject && (_newAttachObject != null || EffectMeshCamera.AttachObject != null))
            {
                var tmpAttachObject = _newAttachObject == null ? EffectMeshCamera.AttachObject : _newAttachObject;

                UEffectManager.Instance.RemoveAndSortCamera(EffectMeshCamera.Camera);

                EffectMeshCamera.Camera.transform.SetParent(tmpAttachObject.transform, false);

                SetUpCamera(true);
            }
        }
        /// <summary>
        /// 相机移除绑定
        /// </summary>
        public void DisattachToObject()
        {
            UEffectManager.Instance.AddCamera(EffectMeshCamera.Camera);

            SetUpCamera(true);
        }
        /// <summary>
        /// 初始化相机属性
        /// </summary>
        /// <param name="_bNewParent"></param>
        public void SetUpCamera(bool _bNewParent = false)
        {
            if (EffectMeshCamera.Camera != null)
            {
                if (_bNewParent)
                    EffectMeshCamera.PositionDefault = null;

                EffectMeshCamera.Camera.targetTexture = MeshDisplayImageRTT;
                EffectMeshCamera.Camera.fieldOfView = EffectMeshCamera.FOV;
                EffectMeshCamera.PositionDefault = EffectMeshCamera.Camera.transform.position;
                EffectMeshCamera.Camera.transform.SetPosition((Vector3)EffectMeshCamera.PositionDefault + EffectMeshCamera.PositionOffset);
                EffectMeshCamera.Camera.transform.localRotation = Quaternion.Euler(EffectMeshCamera.Rotation.x, EffectMeshCamera.Rotation.y, EffectMeshCamera.Rotation.z);
            }
        }
    }
    partial class UEffectNode
    {

    }
}
