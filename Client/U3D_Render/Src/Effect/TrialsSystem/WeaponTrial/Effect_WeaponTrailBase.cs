using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace Effect
{
    using EffectCore;
    public  class Effect_WeaponTrailBase : MonoBehaviour
    {
        public sealed class Element
        {
            public Vector3 PointStart;

            public Vector3 PointEnd;

            public Vector3 Pos
            {
                get
                {
                    return (PointStart + PointEnd) / 2f;
                }
            }


            public Element(Vector3 start, Vector3 end)
            {
                PointStart = start;
                PointEnd = end;
            }

            public Element()
            {

            }
        }


        #region 公有成员

        public bool Emit = false;

        //初始位置
        public Transform PointStart;

        //结束位置
        public Transform PointEnd;

        //存在多少帧
        public int LifeFrame = 14;


        public Color TintColor = Color.white;
        public Material TrailMaterial;

        public Effect_FadeInOutMaterialColor FadeInOutMaterialColor = null;
        public Effect_FadeInOutMaterialFloat FadeInOutMaterialFloat = null;
        public Effect_UVTextureAnimator UVTextureAnimator = null;
        public Effect_UVScroll UVScroll = null;
        public Effect_MaterialGradientColor GradientColor = null;

        #endregion

        #region 私有成员

        private float mTrailWidth = 0f;
        private Element mHeadElem = new Element();
        private List<Element> mSnapshotList = new List<Element>();
        private Spline mSpline = new Spline();
        private float mFadeT = 1f;
        private bool mIsFading = false;
        private float mFadeTime = 1f;
        private float mElapsedTime = 0f;
        private float mFadeElapsedime = 0f;
        private GameObject mMeshObj;
        private MeshRenderer m_Render = null;
        private VertexPool mVertexPool;
        private VertexPool.VertexSegment mVertexSegment;
        private bool mInited = false;

        private bool bActive = true;

        //更新速度(帧为单位)
        private float Fps = 60f;

        private Material InstanceMaterial = null;
        #endregion

        #region 属性

        public float UpdateInterval
        {
            get
            {
                return 1f / Fps;
            }
        }
        public Vector3 CurHeadPos
        {
            get { return (PointStart.position + PointEnd.position) / 2f; }
        }
        public float TrailWidth
        {
            get
            {
                return mTrailWidth;
            }
        }
        #endregion


        #region API

        /// <summary>
        /// 迭代密度
        /// </summary>
        /// <returns></returns>
        protected virtual int GetGranularity()
        {
            Debug.LogWarning("必须重写GetGranularity");
            return 1;
        }

        public void Init()
        {
            if (mInited)
                return;
            if (!PointStart || !PointEnd)
            {
                return;
            }
            if (!TrailMaterial)
            {
                Debug.LogWarning("材质球为空:" + name);
                return;
            }
            mTrailWidth = (PointStart.position - PointEnd.position).magnitude;
           // RelaryLifeFrame = LifeFrame;
            InitMeshObj();

            InitOriginalElements();

            InitSpline();

            CheckMaterialControl();
            OldEmit = Emit;
            mInited = true;
        }

        public void Play()
        {

            Init();


            if (!mInited)
            {
                return;
            }

            //场景切换，会删除Go，所以要检查
            if (mMeshObj == null)
            {
                InitMeshObj();
                return;
            }

            //gameObject.SetActive(true);
            if (mMeshObj != null)
                mMeshObj.SetActive(true);

            //如果开始的点太远，重置一遍，因为光效优化的位置是在99999
            //if (mSnapshotList[0].PointStart.sqrMagnitude > 100000000 ||
            //    mSnapshotList[0].PointEnd.sqrMagnitude > 100000000 ||
            //    RelaryLifeFrame != LifeFrame)
            //{
                //RelaryLifeFrame = LifeFrame;
                InitOriginalElements();
                InitSpline();
           // }

            mFadeT = 1f;
            mIsFading = false;
            mFadeTime = 1f;
            mFadeElapsedime = 0f;
            mElapsedTime = 0f;

            //重置
            for (int i = 0; i < mSnapshotList.Count; i++)
            {
                mSnapshotList[i].PointStart = PointStart.position;
                mSnapshotList[i].PointEnd = PointEnd.position;

                //mSpline.ControlPoints[i].Position = mSnapshotList[i].Pos;
                //mSpline.ControlPoints[i].Normal = mSnapshotList[i].PointEnd - mSnapshotList[i].PointStart;
            }

            //重置
            RefreshSpline();
            UpdateVertex();
            GenerateMaterialInstance();
            UpdateMaterial();
            PlayMaterialControl();
            EffectUtily.DispatchRender(m_Render, m_Render.sharedMaterial);

            mVertexPool.LateUpdate();
            bActive = true;
        }

        public void Stop()
        {

            if (!mInited)
            {
                return;
            }
            //InitOriginalElements();
            //InitSpline();

            //gameObject.SetActive(false);
            if (mMeshObj != null)
                mMeshObj.SetActive(false);
            bActive = false;

            EffectUtily.GabageRender(m_Render, m_Render.sharedMaterial);

            ClearMaterialInstance();
            StopMaterialControl();
        }

        public void StopSmoothly(float fadeTime)
        {
            mIsFading = true;
            mFadeTime = fadeTime;
        }

        /// <summary>
        /// 计算材质球实例
        /// </summary>
        /// <returns></returns>
        public Material GenerateMaterialInstance()
        {
            if (!InstanceMaterial)
            {
                if (TrailMaterial != null)
                {
                    InstanceMaterial = new Material(TrailMaterial);
                    InstanceMaterial.name = TrailMaterial.name + " (Instance)";
                }
            }
            return InstanceMaterial;
        }

        /// <summary>
        /// 清除材质球实例
        /// </summary>
        public void ClearMaterialInstance()
        {
            if (InstanceMaterial)
            {
                ResNode.DestroyRes(ref InstanceMaterial);
                InstanceMaterial = null;
            }
        }

        /// <summary>
        /// 更新材质球
        /// </summary>
        public void UpdateMaterial()
        {
            if (m_Render)
            {
                m_Render.sharedMaterial = GenerateMaterialInstance();
            }

            if (mVertexPool != null)
            {
                mVertexPool.Material = GenerateMaterialInstance();
            }

        }
        #endregion

        #region Unity函数

        private bool OldEmit = false;

        public void LateUpdate()
        {
            if (!mInited)
                return;

            //场景切换，会删除Go，所以要检查
            if (mMeshObj == null)
            {
                InitMeshObj();
                return;
            }



            UpdateHeadElem();


            mElapsedTime += Time.deltaTime;
            if (mElapsedTime < UpdateInterval)
            {
                return;
            }
            mElapsedTime -= UpdateInterval;

            if (Emit != OldEmit)
            {
                OldEmit = Emit;
                if (!Emit)
                {
                    Stop();
                }
                else
                {
                    Play();
                }
            }

            if (!bActive)
            {
                return;
            }

            if (Emit)
            {
                RecordCurElem();
                RefreshSpline();

            }

            UpdateFade();
            UpdateVertex();

            mVertexPool.LateUpdate();
        }


        public void Start()
        {
            Init();
            if (!Emit)
            {
                Stop();
            }
            OldEmit = Emit;
        }

        public void OnDrawGizmos()
        {
            if (PointEnd == null || PointStart == null)
            {
                return;
            }


            float dist = (PointStart.position - PointEnd.position).magnitude;

            if (dist < Mathf.Epsilon)
                return;


            Gizmos.color = Color.red;

            Gizmos.DrawSphere(PointStart.position, dist * 0.04f);


            Gizmos.color = Color.blue;
            Gizmos.DrawSphere(PointEnd.position, dist * 0.04f);

        }

        public void OnDestroy()
        {

            if (!mInited)
            {
                return;
            }
            Stop();
            if (Application.isEditor)
            {
                DestroyImmediate(mMeshObj);
            }
            else
            {
                ResNode.DestroyRes(ref mMeshObj);
            }

            mVertexPool = null;
            mVertexSegment = null;
        }
        #endregion

        #region 私有函数

        void InitSpline()
        {
            mSpline.Granularity = GetGranularity();

            mSpline.Clear();

            for (int i = 0; i < LifeFrame; i++)
            {
                mSpline.AddControlPoint(CurHeadPos, PointStart.position - PointEnd.position);
            }
        }

        void RefreshSpline()
        {
            for (int i = 0; i < mSnapshotList.Count; i++)
            {
                mSpline.ControlPoints[i].Position = mSnapshotList[i].Pos;
                mSpline.ControlPoints[i].Normal = mSnapshotList[i].PointEnd - mSnapshotList[i].PointStart;
            }

            mSpline.RefreshSpline();
        }

        void UpdateVertex()
        {
            VertexPool pool = mVertexSegment.Pool;


            for (int i = 0; i < GetGranularity(); i++)
            {
                int baseIdx = mVertexSegment.VertStart + i * 3;

                float uvSegment = (float)i / GetGranularity();


                float fadeT = uvSegment * mFadeT;

                Vector2 uvCoord = Vector2.zero;

                Vector3 pos = mSpline.InterpolateByLen(fadeT);

                //Debug.DrawRay(pos, Vector3.up, Color.red);

                Vector3 up = mSpline.InterpolateNormalByLen(fadeT);
                Vector3 pos0 = pos + (up.normalized * mTrailWidth * 0.5f);
                Vector3 pos1 = pos - (up.normalized * mTrailWidth * 0.5f);


                // pos0
                pool.Vertices[baseIdx] = pos0;
                pool.Colors[baseIdx] = TintColor;
                uvCoord.x = 0f;
                uvCoord.y = uvSegment;
                pool.UVs[baseIdx] = uvCoord;

                //pos
                pool.Vertices[baseIdx + 1] = pos;
                pool.Colors[baseIdx + 1] = TintColor;
                uvCoord.x = 0.5f;
                uvCoord.y = uvSegment;
                pool.UVs[baseIdx + 1] = uvCoord;

                //pos1
                pool.Vertices[baseIdx + 2] = pos1;
                pool.Colors[baseIdx + 2] = TintColor;
                uvCoord.x = 1f;
                uvCoord.y = uvSegment;
                pool.UVs[baseIdx + 2] = uvCoord;
            }

            mVertexSegment.Pool.UVChanged = true;
            mVertexSegment.Pool.VertChanged = true;
            mVertexSegment.Pool.ColorChanged = true;
        }

        void UpdateIndices()
        {

            VertexPool pool = mVertexSegment.Pool;

            for (int i = 0; i < GetGranularity() - 1; i++)
            {
                int baseIdx = mVertexSegment.VertStart + i * 3;
                int nextBaseIdx = mVertexSegment.VertStart + (i + 1) * 3;

                int iidx = mVertexSegment.IndexStart + i * 12;

                //triangle left
                pool.Indices[iidx + 0] = nextBaseIdx;
                pool.Indices[iidx + 1] = nextBaseIdx + 1;
                pool.Indices[iidx + 2] = baseIdx;
                pool.Indices[iidx + 3] = nextBaseIdx + 1;
                pool.Indices[iidx + 4] = baseIdx + 1;
                pool.Indices[iidx + 5] = baseIdx;


                //triangle right
                pool.Indices[iidx + 6] = nextBaseIdx + 1;
                pool.Indices[iidx + 7] = nextBaseIdx + 2;
                pool.Indices[iidx + 8] = baseIdx + 1;
                pool.Indices[iidx + 9] = nextBaseIdx + 2;
                pool.Indices[iidx + 10] = baseIdx + 2;
                pool.Indices[iidx + 11] = baseIdx + 1;

            }

            pool.IndiceChanged = true;
        }

        void UpdateHeadElem()
        {
            mSnapshotList[0].PointStart = PointStart.position;
            mSnapshotList[0].PointEnd = PointEnd.position;
        }


        void UpdateFade()
        {
            if (!mIsFading)
                return;

            mFadeElapsedime += Time.deltaTime;

            float t = mFadeElapsedime / mFadeTime;

            mFadeT = 1f - t;

            if (mFadeT < 0f)
            {
                Stop();
            }
        }

        void RecordCurElem()
        {
            //TODO: use element pool to avoid gc alloc.
            Element elem = new Element(PointStart.position, PointEnd.position);

            if (mSnapshotList.Count < LifeFrame)
            {
                mSnapshotList.Insert(1, elem);
            }
            else
            {
                mSnapshotList.RemoveAt(mSnapshotList.Count - 1);
                mSnapshotList.Insert(1, elem);
            }

        }

        void InitOriginalElements()
        {
            mSnapshotList.Clear();
            //at least add 2 original elements
            mSnapshotList.Add(new Element(PointStart.position, PointEnd.position));
            mSnapshotList.Add(new Element(PointStart.position, PointEnd.position));
        }



        void InitMeshObj()
        {
            //create a new mesh obj
            mMeshObj = new GameObject("WeaponTrialObje: " + gameObject.name);
            mMeshObj.layer = gameObject.layer;
            mMeshObj.hideFlags = HideFlags.HideAndDontSave;
            mMeshObj.SetActive(true);
            MeshFilter mf = mMeshObj.AddComponent<MeshFilter>();
            m_Render = mMeshObj.AddComponent<MeshRenderer>();
            m_Render.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
            m_Render.receiveShadows = false;
            mf.sharedMesh = new Mesh();
            mf.sharedMesh.name = "WeaponTrialMesh";

            //init vertexpool
            mVertexPool = new VertexPool(mf.sharedMesh, GenerateMaterialInstance());
            mVertexSegment = mVertexPool.GetVertices(GetGranularity() * 3, (GetGranularity() - 1) * 12);

            UpdateMaterial();
            UpdateIndices();

        }

        #region 材质控制器
        void StopMaterialControl()
        {
            //清空动画
            if (FadeInOutMaterialColor)
            {
                //如果运行模式不是外部指定的
                if (FadeInOutMaterialColor.RunMode == EffectRunMode.OutSiedePointed)
                {
                    FadeInOutMaterialColor.ClearMaterialInstance();
                    FadeInOutMaterialColor.Stop();
                }

            }

            if (FadeInOutMaterialFloat)
            {
                //如果运行模式不是外部指定的
                if (FadeInOutMaterialFloat.RunMode == EffectRunMode.OutSiedePointed)
                {
                    FadeInOutMaterialFloat.ClearMaterialInstance();
                    FadeInOutMaterialFloat.Stop();
                }

            }

            if (UVTextureAnimator)
            {
                //如果运行模式不是外部指定的
                if (UVTextureAnimator.RunMode == EffectRunMode.OutSiedePointed)
                {
                    UVTextureAnimator.ClearMaterialInstance();
                    UVTextureAnimator.Stop();
                }
            }

            if (UVScroll)
            {
                //如果运行模式不是外部指定的
                if (UVScroll.RunMode == EffectRunMode.OutSiedePointed)
                {
                    UVScroll.ClearMaterialInstance();
                    UVScroll.Stop();
                }
            }

            if (GradientColor)
            {
                //如果运行模式不是外部指定的
                if (GradientColor.RunMode == EffectRunMode.OutSiedePointed)
                {
                    GradientColor.ClearMaterialInstance();
                    GradientColor.Stop();
                }
            }

            
        }

        void PlayMaterialControl()
        {
            //把材质设置进动画里
            if (FadeInOutMaterialColor)
            {
                //如果运行模式不是外部指定的
                if (FadeInOutMaterialColor.RunMode == EffectRunMode.OutSiedePointed)
                {
                    FadeInOutMaterialColor.SetMaterialInstance(GenerateMaterialInstance());
                    FadeInOutMaterialColor.Play();
                }

            }

            if (FadeInOutMaterialFloat)
            {
                //如果运行模式不是外部指定的
                if (FadeInOutMaterialFloat.RunMode == EffectRunMode.OutSiedePointed)
                {
                    FadeInOutMaterialFloat.SetMaterialInstance(GenerateMaterialInstance());
                    FadeInOutMaterialFloat.Play();
                }

            }

            if (UVTextureAnimator)
            {
                //如果运行模式不是外部指定的
                if (UVTextureAnimator.RunMode == EffectRunMode.OutSiedePointed)
                {
                    UVTextureAnimator.SetMaterialInstance(GenerateMaterialInstance());
                    UVTextureAnimator.Play();
                }
            }

            if (UVScroll)
            {
                //如果运行模式不是外部指定的
                if (UVScroll.RunMode == EffectRunMode.OutSiedePointed)
                {
                    UVScroll.SetMaterialInstance(GenerateMaterialInstance());
                    UVScroll.Play();
                }
            }

            if (GradientColor)
            {
                //如果运行模式不是外部指定的
                if (GradientColor.RunMode == EffectRunMode.OutSiedePointed)
                {
                    GradientColor.SetMaterialInstance(GenerateMaterialInstance());
                    GradientColor.Play();
                }
            }
        }

        void CheckMaterialControl()
        {
            if (UVTextureAnimator)
            {
                if (UVTextureAnimator.RunMode != EffectRunMode.OutSiedePointed)
                {
                    Debug.LogError("UVTextureAnimator成员的运行模式不是OutSiedePointed!" + gameObject.name);
                }
            }

            if (FadeInOutMaterialColor)
            {
                if (FadeInOutMaterialColor.RunMode != EffectRunMode.OutSiedePointed)
                {
                    Debug.LogError("FadeInOutMaterialColor成员的运行模式不是OutSiedePointed!" + gameObject.name);
                }
            }

            if (FadeInOutMaterialFloat)
            {
                if (FadeInOutMaterialFloat.RunMode != EffectRunMode.OutSiedePointed)
                {
                    Debug.LogError("FadeInOutMaterialFloat成员的运行模式不是OutSiedePointed!" + gameObject.name);
                }
            }

            if (UVScroll)
            {
                if (UVScroll.RunMode != EffectRunMode.OutSiedePointed)
                {
                    Debug.LogError("UVScroll成员的运行模式不是OutSiedePointed!" + gameObject.name);
                }
            }

            if (GradientColor)
            {
                //如果运行模式不是外部指定的
                if (GradientColor.RunMode != EffectRunMode.OutSiedePointed)
                {
                    Debug.LogError("UVScroll成员的运行模式不是OutSiedePointed!" + gameObject.name);
                }
            }
        }
        #endregion

        #endregion


    }

}
