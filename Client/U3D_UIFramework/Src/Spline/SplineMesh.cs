using UnityEngine;
using System.Collections;
using System.Collections.Generic;


namespace Spline
{
    [RequireComponent(typeof(SplineComponent))]
    public class SplineMesh : MonoBehaviour
    {
        protected SplineComponent m_SplineComponent;
        public SplineComponent Spline
        {
            get
            {
                if (m_SplineComponent == null)
                {
                    m_SplineComponent = GetComponent<SplineComponent>();
                    m_fOldLength = m_SplineComponent.GetSplineLength();
                }

                return m_SplineComponent;
            }
        }

        /// <summary>
        /// 模型预制体
        /// </summary>
        public GameObject MeshPrefab;
        /// <summary>
        /// 根据前后长度判断是否要更新曲线模型
        /// </summary>
        protected float m_fOldLength;
        /// <summary>
        /// 两模型间距
        /// </summary>
        public float MeshDistanceDelta = 10;

        private Transform m_tUnuseContainer;

        private List<Transform> m_listUnuseContainer;

        private List<Transform> m_listUseContainer;

        private Transform m_tMineTransform;

        private bool m_bInit;

        public bool IsAutoUpdateMesh = true;

        private Vector3? m_vec3CurrentPosition = null;

#if UNITY_EDITOR
        protected virtual void OnValidate()
        {
            //UpdateInstanceMesh();
        }

        protected virtual void Reset()
        { }
#endif

        private void Init()
        {
            if (m_bInit)
                return;
            m_bInit = true;
            m_tMineTransform = transform;

            m_tUnuseContainer = new GameObject("UnUse").GetComponent<Transform>();
            m_tUnuseContainer.SetParent(m_tMineTransform);

            m_SplineComponent = GetComponent<SplineComponent>();
            m_fOldLength = m_SplineComponent.GetSplineLength();

            m_listUnuseContainer = new List<Transform>();
            m_listUseContainer = new List<Transform>();

            if(MeshPrefab == null)
            {
                MeshPrefab = GameObject.CreatePrimitive(PrimitiveType.Cube);
                BoxCollider collider = MeshPrefab.GetComponent<BoxCollider>();
                DestroyImmediate(collider);
            }
        }

        public void Update()
        {
            if(IsAutoUpdateMesh)
                UpdateMesh();
        }

        public void OnDestroy()
        {
            Destroy(m_tUnuseContainer);
            Destroy(MeshPrefab);

            ClearNode(ref m_listUseContainer);
            ClearNode(ref m_listUnuseContainer);

        }

        private void ClearNode(ref List<Transform> _list)
        {
            if (_list == null)
                return;
            Transform tmpNode = null;

            while (_list.Count > 0)
            {
                tmpNode = _list[0];
                Destroy(tmpNode.gameObject);
                _list.Remove(tmpNode);
            }
            _list.Clear();
            _list = null;
        }

        public void UpdateMesh()
        {
            Init();

            if (Spline != null && MeshPrefab != null )//&&  m_fOldLength != (int)Spline.GetSplineLength())
            {
                BeginNode();

                int count = (int)(Spline.GetSplineLength() / MeshDistanceDelta);

                for (float i = 0; i < count; i++)
                {
                    UseNode(i * MeshDistanceDelta, ESplineCoordinateSpace.Local);//AddInstance(i * MeshDistanceDelta, ESplineCoordinateSpace.Local);
                }

                m_fOldLength = (int)Spline.GetSplineLength();
            }
        }

        protected void UseNode(float _fDistance, ESplineCoordinateSpace _eCoorSpace = ESplineCoordinateSpace.World)
        {
            Vector3 pointLoc = Spline.GetLocationAtDistanceAlongSpline(_fDistance, _eCoorSpace);
            Quaternion pointTang = Spline.GetTangentAtDistanceAlongSpline(_fDistance, _eCoorSpace);

            Transform tmpTrans = null;
            if (!GetUnuseObject(out tmpTrans))
                return;

            if (_eCoorSpace == ESplineCoordinateSpace.Local)
            {
                tmpTrans.localPosition = pointLoc;
                tmpTrans.localRotation = pointTang;
            }

        }

        protected void BeginNode()
        {
            if (m_listUseContainer.Count <= 0)
                return;

            Transform node;
            while (m_listUseContainer.Count > 0)
            {
                node = m_listUseContainer[0];
                m_listUseContainer.Remove(node);
                m_listUnuseContainer.Add(node);

                node.SetParent(m_tUnuseContainer, false);

                node.gameObject.SetActive(false);
            }

        }
        protected bool GetUnuseObject(out Transform _tUnuse)
        {
            Transform node;
            //先从缓冲中取
            if (m_listUnuseContainer.Count > 0)
            {
                node = m_listUnuseContainer[m_listUnuseContainer.Count - 1];
                m_listUnuseContainer.Remove(node);

                m_listUseContainer.Add(node);
                node.SetParent(m_tMineTransform);
                node.gameObject.SetActive(true);
                _tUnuse = node;

                return true;
            }
            //没有的话实例化一个出来
            if (MeshPrefab == null)
            {
                Debug.LogError("没有节点预制体，不能实例化物品合成树节点");
                _tUnuse = null;
                return false;
            }

            InstantiateObject();

            node = m_listUnuseContainer[m_listUnuseContainer.Count - 1];
            m_listUnuseContainer.Remove(node);
            m_listUseContainer.Add(node);
            node.SetParent(m_tMineTransform);
            node.gameObject.SetActive(true);
            _tUnuse = node;

            return true;
        }
        protected void InstantiateObject()
        {
            Transform newNode = (Instantiate(MeshPrefab, m_tUnuseContainer) as GameObject).GetComponent<Transform>();

            m_listUnuseContainer.Add(newNode);

            newNode.gameObject.SetActive(false);
        }

        public void SetCurrentPosition(Vector3 _vec3)
        {
            m_vec3CurrentPosition = _vec3;
            UpdateMesh();
        }
    }
}

