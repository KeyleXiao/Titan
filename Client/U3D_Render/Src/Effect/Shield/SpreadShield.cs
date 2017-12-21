//using UnityEngine;
//using System.Collections;
//using System.Collections.Generic;

//namespace Effect
//{
//    /// <summary>
//    /// 盾反馈等级
//    /// </summary>
//    public enum ShieldFeedBackQuality
//    {
//        /// <summary>
//        /// 同时只有一个反馈
//        /// </summary>
//        Single,
        
//        /// <summary>
//        /// 同时有三个反馈
//        /// </summary>
//        Three,

//        /// <summary>
//        /// 没有限制
//        /// </summary>
//        UnLitmed,
//    }

//    public class SpreadShield : ShieldBase
//    {
//        /// <summary>
//        /// 护盾反馈等级
//        /// </summary>
//        public ShieldFeedBackQuality ShieldQuality
//        {
//            get
//            {
//                return m_ShieldQuality;
//            }
//            set
//            {
//                m_ShieldQuality = value;
//            }
//        }

//        public Material ShieldMaterial;
//        public Texture ShiledMainTexture;

//        public float FieldDuartion = 0.0f;
//        public float FieldScale = 1.0f;

//        private Vector3 currentHitPoint;

//        [SerializeField,HideInInspector]
//        protected ShieldFeedBackQuality m_ShieldQuality;
//        //private bool bPlay = false;
//        private List<Renderer> m_Renders = new List<Renderer>();

//        private Dictionary<Decal, bool> FeedBackDecalPlayMap= new Dictionary<Decal, bool>();
//        private Dictionary<Decal, Effect_UVTextureAnimator> FeedBackDecalUVTextureAnimatorMap = new Dictionary<Decal, Effect_UVTextureAnimator>();
//        private Dictionary<Decal, Effect_UVScroll> FeedBackDecalUVScoll = new Dictionary<Decal, Effect_UVScroll>();
//        private Dictionary<Decal, Effect_FadeInOutMaterialColor> FeedBackFadeInOutMaterialColor = new Dictionary<Decal, Effect_FadeInOutMaterialColor>();
//        private Dictionary<Decal, Effect_FadeInOutMaterialFloat> FeedBackFadeInOutMaterialFloat = new Dictionary<Decal, Effect_FadeInOutMaterialFloat>();
//        private Dictionary<Decal, Effect_MaterialGradientColor> FeedBacMaterialGradColor = new Dictionary<Decal, Effect_MaterialGradientColor>();
//        private LinkedList<Decal> ActiveList = new LinkedList<Decal>();
//        private Queue<Decal> UnActiveList = new Queue<Decal>();

//        // Use this for initialization
//        public override void Start()
//        {
//            base.Start();
//            if (FieldDuartion <= 0.001f)
//            {
//                Debug.LogError("护盾持续时间为0,请填入持续时间!");
//                enabled = false;
//                return;
//            }
//            if (!ShieldMaterial || !ShiledMainTexture)
//            {
//                Debug.LogError("材质球为空或者贴图为空!");
//                enabled = false;
//                return;
//            }

//            DisableMaterialControl();
//            CheckMaterialControl();
            
//        }

//        public override void OnDestroy()
//        {
//            base.OnDestroy();
//            FeedBackDecalPlayMap.Clear();
//            FeedBackDecalUVTextureAnimatorMap.Clear();
//            FeedBackDecalUVScoll.Clear();
//            FeedBackFadeInOutMaterialColor.Clear();
//            FeedBackFadeInOutMaterialFloat.Clear();
//            FeedBacMaterialGradColor.Clear();

//            foreach(Decal de in ActiveList)
//            {
//                DestroyImmediate(de.gameObject);
//            }
//            foreach (Decal de in UnActiveList)
//            {
//                DestroyImmediate(de.gameObject);
//            }
//            ActiveList.Clear();
//            UnActiveList.Clear();
//        }

//        Decal CreateNewDeacl()
//        {
//            Decal DecalInstance = null;
//            GameObject DecalObject = null;
//            DecalObject = new GameObject("DecalObject");
//            DecalObject.transform.parent = transform;
//            DecalObject.hideFlags = HideFlags.HideAndDontSave;
//            DecalInstance = DecalObject.AddComponent<Decal>();
//            DecalInstance.OnlyAffectThisGameObject.Clear();
//            DecalInstance.OnlyAffectThisGameObject.Add(transform.gameObject);
//            DecalInstance.DecalMaterial = ShieldMaterial;
//            DecalInstance.DecalTexture = ShiledMainTexture;
//            DecalInstance.Init();

//            if (!DecalInstance.IsVilad)
//            {
//                enabled = false;
//            }
//            else
//            {
//                BindDecalControlComponent(DecalInstance);
//                FeedBackDecalPlayMap.Add(DecalInstance, false);
//            }
//            return DecalInstance;
//        }
//        void BindDecalControlComponent(Decal decalInstance)
//        {
//            if (!decalInstance)
//            {
//                return;
//            }

//            //绑定UV动画组件
//            if (UVTextureAnimator)
//            {
//                if (UVTextureAnimator.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_UVTextureAnimator decalUVTextureAnimator = decalInstance.gameObject.AddComponent<Effect_UVTextureAnimator>();
//                    decalUVTextureAnimator.CopyInspectorParamsTo(UVTextureAnimator);
//                    decalUVTextureAnimator.SetMaterialInstance(decalInstance.DecalRender.material);
//                    decalUVTextureAnimator.SetRenderQuen(decalInstance.DecalRender.sharedMaterial.renderQueue + 1);
//                    FeedBackDecalUVTextureAnimatorMap.Add(decalInstance, decalUVTextureAnimator);
//                }
//            }

//            //绑定颜色材质球控制组件
//            if (FadeInOutMaterialColor)
//            {
//                if (FadeInOutMaterialColor.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_FadeInOutMaterialColor decalFadeInOutMaterialColor = decalInstance.gameObject.AddComponent<Effect_FadeInOutMaterialColor>();
//                    decalFadeInOutMaterialColor.CopyInspectorParamsTo(FadeInOutMaterialColor);
//                    decalFadeInOutMaterialColor.SetMaterialInstance(decalInstance.DecalRender.material);
//                    decalFadeInOutMaterialColor.SetRenderQuen(decalInstance.DecalRender.sharedMaterial.renderQueue + 1);
//                    FeedBackFadeInOutMaterialColor.Add(decalInstance, decalFadeInOutMaterialColor);
//                }
//            }

//            //绑定浮点数材质球控制组件
//            if (FadeInOutMaterialFloat)
//            {
//                if (FadeInOutMaterialFloat.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_FadeInOutMaterialFloat decalFadeInOutMaterialFloat = decalInstance.gameObject.AddComponent<Effect_FadeInOutMaterialFloat>();
//                    decalFadeInOutMaterialFloat.CopyInspectorParamsTo(FadeInOutMaterialFloat);
//                    decalFadeInOutMaterialFloat.SetMaterialInstance(decalInstance.DecalRender.material);
//                    decalFadeInOutMaterialFloat.SetRenderQuen(decalInstance.DecalRender.sharedMaterial.renderQueue + 1);
//                    FeedBackFadeInOutMaterialFloat.Add(decalInstance, decalFadeInOutMaterialFloat);
//                }
//            }

//            //绑定UV流动组件
//            if (UVScroll)
//            {
//                if (UVScroll.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_UVScroll decalUVScroll = decalInstance.gameObject.AddComponent<Effect_UVScroll>();
//                    decalUVScroll.CopyInspectorParamsTo(UVScroll);
//                    decalUVScroll.SetMaterialInstance(decalInstance.DecalRender.material);
//                    decalUVScroll.SetRenderQuen(decalInstance.DecalRender.sharedMaterial.renderQueue + 1);
//                    FeedBackDecalUVScoll.Add(decalInstance, decalUVScroll);
//                }
//            }

//            //绑定高级颜色组件
//            if (GradientColor)
//            {
//                if (GradientColor.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_MaterialGradientColor decalgrad = decalInstance.gameObject.AddComponent<Effect_MaterialGradientColor>();
//                    decalgrad.CopyInspectorParamsTo(decalgrad);
//                    decalgrad.SetMaterialInstance(decalInstance.DecalRender.material);
//                    decalgrad.SetRenderQuen(decalInstance.DecalRender.sharedMaterial.renderQueue + 1);
//                    FeedBacMaterialGradColor.Add(decalInstance, decalgrad);
//                }
//            }
//        }

//        /// <summary>
//        /// 激活
//        /// </summary>
//        /// <param name="decalInstance">贴花</param>
//        /// <param name="bUpdateDecalTransform">是否需要更新decal贴花的transform，如果是同一个点就不需要更新，节省性能</param>
//        void ActiveDecal(Decal decalInstance,bool bUpdateDecalTransform = true)
//        {
//            if (!decalInstance)
//            {
//                return;
//            }
//            decalInstance.gameObject.SetActive(true);
//            if (!FeedBackDecalPlayMap[decalInstance])
//            {
//                if (bUpdateDecalTransform)
//                {
//                    UpdateDecalTransform(decalInstance);
//                }
               

//                UpdateMaterialControl(decalInstance);
//                PlayMaterialControl(decalInstance);

//                GameUtil.StartCoroutineHelper(this,WaitForFinish(decalInstance));
//                FeedBackDecalPlayMap[decalInstance] = true;
//            }
//        }

//        void DisActiveDecal(Decal decalInstance)
//        {
//            if (!decalInstance)
//            {
//                return;
//            }
//            decalInstance.gameObject.SetActive(false);
//            StopMaterialControl(decalInstance);
//            FeedBackDecalPlayMap[decalInstance] = false;
//        }

//        void UpdateDecalTransform(Decal decalInstance)
//        {
//            if (!decalInstance)
//            {
//                return;
//            }

//            Vector3 old = transform.eulerAngles;
//            transform.LookAt(currentHitPoint);

//            Vector3 LookDirection;
//            LookDirection = transform.forward;
//            transform.eulerAngles = old;

//            decalInstance.transform.SetPosition( currentHitPoint);
//            decalInstance.transform.forward = -LookDirection;
//            decalInstance.transform.localScale = Vector3.one * FieldScale;

//            decalInstance.UpdateDecal();
//        }

//        //public void OnTriggerEnter(Collider other)
//        //{
//        //    OnHit(other.bounds.center);
//        //}


//        /// <summary>
//        /// 被击中
//        /// </summary>
//        /// <param name="hitPoint">位置</param>
//        /// <param name="bPrecisionPoint">是否是精确的点，如果为false，会调用Collider的点重新计算</param>
//        public override void OnHit(Vector3 hitPoint, bool bPrecisionPoint = true)
//        {
//            if (!bPrecisionPoint)
//            {
//                hitPoint = m_Collider.ClosestPointOnBounds(hitPoint);

//            }
//            currentHitPoint = hitPoint;
//            Play();
//        }

//        public override void Play()
//        {
//            if (FieldDuartion <= 0.001f)
//            {
//                Debug.LogError("护盾持续时间为0,请填入持续时间!");
//                enabled = false;
//            }
           
//            //遍历活动列表，看是不是有当前的受击点的decal
//            foreach (Decal de in ActiveList)
//            {
//                //找到了这个同一个受击点的decal,不做下面的处理了.
//                if (de.transform.position == currentHitPoint)
//                {
//                    ActiveDecal(de,false);
//                    return;
//                }
//            }

//            //质量等级限制
//            int MaxDecalNumber = 0;
//            int currentDecalNumber = ActiveList.Count;
//            if (ShieldQuality == ShieldFeedBackQuality.Single)
//            {
//                MaxDecalNumber = 1;
//            }
//            else if (ShieldQuality == ShieldFeedBackQuality.Three)
//            {
//                MaxDecalNumber = 3;
//            }
//            else if (ShieldQuality == ShieldFeedBackQuality.UnLitmed)
//            {
//                MaxDecalNumber = int.MaxValue;
//            }

//            //这是一个新的受击点
//            Decal decalInstance = null;
            
//            //非活动列表里是否有decal
//            if (UnActiveList.Count > 0)
//            {
//                //做数量限制，这个要小于等于，因为不是新建的
//                if (currentDecalNumber <= MaxDecalNumber)
//                {
//                    decalInstance = UnActiveList.Dequeue();
//                }
               
//            }
//            else
//            {
//                //做数量限制
//                if (currentDecalNumber < MaxDecalNumber)
//                {
//                    //没有，那就创建一个
//                    decalInstance = CreateNewDeacl();
//                }

//            }

//            if (decalInstance)
//            {
//                ActiveList.AddLast(decalInstance);
//                ActiveDecal(decalInstance);
//            }

//        }

//        /// <summary>
//        /// 停止单个的FeedBack-decal
//        /// </summary>
//        /// <param name="decalInstance"></param>
//        private void StopFeedBack(Decal decalInstance)
//        {
//            DisActiveDecal(decalInstance);
//            UnActiveList.Enqueue(decalInstance);
//            ActiveList.Remove(decalInstance);
//        }

//        public override void Stop()
//        {
//            foreach (Decal de in ActiveList)
//            {
//                DisActiveDecal(de);
//                UnActiveList.Enqueue(de);
//            }
//            ActiveList.Clear();
//            StopAllCoroutines();
//        }

//        IEnumerator WaitForFinish(Decal Instance)
//        {
//            yield return new WaitForSeconds(FieldDuartion);
//            StopFeedBack(Instance);
//        }


//        #region 材质控制器相关

//        void CheckMaterialControl()
//        {
//            if (UVTextureAnimator)
//            {
//                if (UVTextureAnimator.RunMode != EffectRunMode.OutSiedePointed)
//                {
//                    Debug.LogError("UVTextureAnimator成员的运行模式不是OutSiedePointed!" + gameObject.name);
//                }
//                else
//                {
//                    UVTextureAnimator.AniTimes = 1;
//                }
//            }

//            if (FadeInOutMaterialColor)
//            {
//                if (FadeInOutMaterialColor.RunMode != EffectRunMode.OutSiedePointed)
//                {
//                    Debug.LogError("FadeInOutMaterialColor成员的运行模式不是OutSiedePointed!" + gameObject.name);
//                }
//            }

//            if (FadeInOutMaterialFloat)
//            {
//                if (FadeInOutMaterialFloat.RunMode != EffectRunMode.OutSiedePointed)
//                {
//                    Debug.LogError("FadeInOutMaterialFloat成员的运行模式不是OutSiedePointed!" + gameObject.name);
//                }
//            }

//            if (UVScroll)
//            {
//                if (UVScroll.RunMode != EffectRunMode.OutSiedePointed)
//                {
//                    Debug.LogError("UVScroll成员的运行模式不是OutSiedePointed!" + gameObject.name);
//                }
//            }
//        }

//        void DisableMaterialControl()
//        {
//            if (UVTextureAnimator)
//            {
//                if (UVTextureAnimator.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    UVTextureAnimator.enabled = false;
//                }

//            }

//            if (FadeInOutMaterialColor)
//            {
//                if (FadeInOutMaterialColor.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    FadeInOutMaterialColor.enabled = false;
//                }
//            }

//            if (FadeInOutMaterialFloat)
//            {
//                if (FadeInOutMaterialFloat.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    FadeInOutMaterialFloat.enabled = false;
//                }
//            }

//            if (UVScroll)
//            {
//                if (UVScroll.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    UVScroll.enabled = false;
//                }
//            }
//        }

//        void UpdateMaterialControl(Decal decalInstance)
//        {
//            if (!decalInstance)
//            {
//                return;
//            }

//            if (UVTextureAnimator)
//            {
//                if (UVTextureAnimator.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_UVTextureAnimator decalUVTextureAnimator = FeedBackDecalUVTextureAnimatorMap[decalInstance];
//                    decalUVTextureAnimator.SetMaterialInstance(decalInstance.DecalRender.material);
//                    decalUVTextureAnimator.SetRenderQuen(decalInstance.DecalRender.sharedMaterial.renderQueue + 1);

//                }
//            }

//            if (FadeInOutMaterialColor)
//            {
//                if (FadeInOutMaterialColor.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_FadeInOutMaterialColor decalFadeInOutMaterialColor = FeedBackFadeInOutMaterialColor[decalInstance];
//                    decalFadeInOutMaterialColor.SetMaterialInstance(decalInstance.DecalRender.material);
//                    decalFadeInOutMaterialColor.SetRenderQuen(decalInstance.DecalRender.sharedMaterial.renderQueue + 1);
//                }
//            }

//            if (FadeInOutMaterialFloat)
//            {
//                if (FadeInOutMaterialFloat.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_FadeInOutMaterialFloat decalFadeInOutMaterialFloat = FeedBackFadeInOutMaterialFloat[decalInstance];
//                    decalFadeInOutMaterialFloat.SetMaterialInstance(decalInstance.DecalRender.material);
//                    decalFadeInOutMaterialFloat.SetRenderQuen(decalInstance.DecalRender.sharedMaterial.renderQueue + 1);
//                }
//            }

//            if (UVScroll)
//            {
//                if (UVScroll.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_UVScroll decalUVScroll = FeedBackDecalUVScoll[decalInstance];
//                    decalUVScroll.SetMaterialInstance(decalInstance.DecalRender.material);
//                    decalUVScroll.SetRenderQuen(decalInstance.DecalRender.sharedMaterial.renderQueue + 1);
//                }
//            }
//            if (GradientColor)
//            {
//                if (GradientColor.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_MaterialGradientColor decalgrad = FeedBacMaterialGradColor [decalInstance];
//                    decalgrad.SetMaterialInstance(decalInstance.DecalRender.material);
//                    decalgrad.SetRenderQuen(decalInstance.DecalRender.sharedMaterial.renderQueue + 1);
//                }
//            }
            
//        }

//        void PlayMaterialControl(Decal decalInstance)
//        {
//            if (!decalInstance)
//            {
//                return;
//            }

//            if (UVTextureAnimator)
//            {
//                if (UVTextureAnimator.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_UVTextureAnimator decalUVTextureAnimator = FeedBackDecalUVTextureAnimatorMap[decalInstance];
//                    decalUVTextureAnimator.Play();
//                }

//            }

//            if (FadeInOutMaterialColor)
//            {
//                if (FadeInOutMaterialColor.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_FadeInOutMaterialColor decalFadeInOutMaterialColor = FeedBackFadeInOutMaterialColor[decalInstance];
//                    decalFadeInOutMaterialColor.Play();
//                }
//            }

//            if (FadeInOutMaterialFloat)
//            {
//                if (FadeInOutMaterialFloat.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_FadeInOutMaterialFloat decalFadeInOutMaterialFloat = FeedBackFadeInOutMaterialFloat[decalInstance];
//                    decalFadeInOutMaterialFloat.Play();
//                }
//            }

//            if (UVScroll)
//            {
//                if (UVScroll.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_UVScroll decalUVScroll = FeedBackDecalUVScoll[decalInstance];
//                    decalUVScroll.Play();
//                }
//            }

//            if (GradientColor)
//            {
//                if (GradientColor.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_MaterialGradientColor decalgrad = FeedBacMaterialGradColor[decalInstance];
//                    decalgrad.Play();
//                }
//            }
//        }

//        void StopMaterialControl(Decal decalInstance)
//        {
//            if (!decalInstance)
//            {
//                return;
//            }

//            if (UVTextureAnimator)
//            {
//                if (UVTextureAnimator.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_UVTextureAnimator decalUVTextureAnimator = FeedBackDecalUVTextureAnimatorMap[decalInstance];
//                    decalUVTextureAnimator.Stop();
//                }

//            }

//            if (FadeInOutMaterialColor)
//            {
//                if (FadeInOutMaterialColor.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_FadeInOutMaterialColor decalFadeInOutMaterialColor = FeedBackFadeInOutMaterialColor[decalInstance];
//                    decalFadeInOutMaterialColor.Stop();
//                }
//            }

//            if (FadeInOutMaterialFloat)
//            {
//                if (FadeInOutMaterialFloat.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_FadeInOutMaterialFloat decalFadeInOutMaterialFloat = FeedBackFadeInOutMaterialFloat[decalInstance];
//                    decalFadeInOutMaterialFloat.Stop();
//                }
//            }

//            if (UVScroll)
//            {
//                if (UVScroll.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_UVScroll decalUVScroll = FeedBackDecalUVScoll[decalInstance];
//                    decalUVScroll.Stop();
//                }
//            }

//            if (GradientColor)
//            {
//                if (GradientColor.RunMode == EffectRunMode.OutSiedePointed)
//                {
//                    Effect_MaterialGradientColor decalgrad = FeedBacMaterialGradColor[decalInstance];
//                    decalgrad.Stop();
//                }
//            }
//        }

//        #endregion
//    }

//}

