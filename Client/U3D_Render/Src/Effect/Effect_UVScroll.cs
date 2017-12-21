/*******************************************************************
** 文件名:	Effect_UVScroll.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/6/9
** 版  本:	1.0
** 描  述:	UV流动
** 应  用:  	UV流动会和UV动画冲突！

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace Effect
{
    /// <summary>
    /// UV流动
    /// </summary>
#if USE_TIMMER_BEHAVIOUR
    public sealed class Effect_UVScroll : UpdateBehaviourOnTimer
#else
    public sealed class Effect_UVScroll : MonoBehaviour
#endif
    {
        public EffectRunMode RunMode = EffectRunMode.Self;

        public string TexturePropertyName =  "_MainTex" ;

        public float fStartDelay = 0.0f;

        /// <summary>
        /// Self模式,使用共享材质球，复制就不会丢失材质球，但是两个相同的会影响到，这个选项只是调试的时候开
        /// </summary>
        public bool UseSharedMaterial = false;

        /// <summary>
        /// 是否使用模型UV
        /// </summary>
        public bool UseModelUV = true;

        /// <summary>
        ///是否使用上级指派的offset作为初始值，如果是的话，resetValue时offset不清0
        /// <summary>
        public bool UseRendererOffset = false; 

        /// <summary>
        /// 流动时间
        /// </summary>
        public float ScrollXTime = 9999999;
        public float ScrollYTime = 9999999;

        public Vector2 ScrollSpeed = Vector2.one;
        public Vector3 RotateSpeed = Vector3.zero;

        /// <summary>
        /// 两次流动的间隔，小于0.03无间隔
        /// </summary>
        public Vector2 BetweenTimes = Vector3.zero;

        /// <summary>
        /// X旋转时间
        /// </summary>
        public float RotateXTime = 9999999;

        /// <summary>
        /// Y旋转时间
        /// </summary>
        public float RotateYTime = 9999999;

        /// <summary>
        /// Z旋转时间
        /// </summary>
        public float RotateZTime = 9999999;
        public bool RoateInWorldSpace = false;

        private bool IncludeChildren = false;

        private List<Renderer> RenderList = new List<Renderer>();

        private List<Material> matInstances = new List<Material>();

        /// <summary>
        ///专门开辟一个数组记录每个子纹理的起始偏移值，如果每个纹理自顾自流动的话，时间长了会错位
        /// <summary>
        private List<Vector2> startOffset = new List<Vector2>();

        private bool bInit = false;
        private bool bPlay = false;

        private bool OldUseModelUV = true;

        private bool bStartPlayYield = false;
        private float currRotateXTime = 0.0f;
        private float currRotateYTime = 0.0f;
        private float currRotateZTime = 0.0f;
        //流动一次所需要的时间
        private Vector2 ScrollOnceTime = Vector2.zero;

        /// <summary>
        /// 是否可以流动
        /// </summary>
        private Vector2 canScroll = Vector2.one ;

        /// <summary>
        /// 当前的等待时间
        /// </summary>
        private Vector2 currentScrollWaitingTime = Vector2.zero;

        private Vector2 currentScrollDealtTime = Vector2.zero;


        /// <summary>
        /// 只有使用共享材质才需要
        /// </summary>
        private Dictionary<Material, Vector2> orginalUVScroll = new Dictionary<Material, Vector2>();

        private Quaternion OrginalLocalRoate = Quaternion.identity;
        private Quaternion OrginalWorldRoate = Quaternion.identity;

        /// <summary>
        /// 总的流动时间
        /// </summary>
        private Vector2 DuringHoldScrollTime = Vector2.zero;


        public Renderer[] GetRendersOnBakeTime()
        {
            if (!Application.isPlaying && Application.isEditor)
            {
                RenderList.Clear();
                if (RunMode == EffectRunMode.Self)
                {
                    if (IncludeChildren)
                    {
                        RenderList.AddRange(GetComponentsInChildren<Renderer>());
                    }
                    else
                    {
                        RenderList.AddRange(GetComponents<Renderer>());
                    }
                }
                return RenderList.ToArray();
            }
            else
            {
                Debug.LogWarning("GetRendersOnBakeTime只能在编辑器下的非播放模式下调用!");
                return new Renderer[0];
            }

        }

        /// <summary>
        /// 拷贝编辑器变量
        /// </summary>
        /// <param name="FromInstance">从哪一个拷贝</param>
        public void CopyInspectorParamsTo(Effect_UVScroll FromInstance)
        {
            RunMode = FromInstance.RunMode;
            TexturePropertyName = FromInstance.TexturePropertyName;

            ScrollSpeed = FromInstance.ScrollSpeed;
            ScrollXTime = FromInstance.ScrollXTime;
            ScrollYTime = FromInstance.ScrollYTime;
            RotateXTime = FromInstance.RotateXTime;
            RotateYTime = FromInstance.RotateYTime;
            RotateZTime = FromInstance.RotateZTime;
            RotateSpeed = FromInstance.RotateSpeed;
            fStartDelay = FromInstance.fStartDelay;
            RoateInWorldSpace = FromInstance.RoateInWorldSpace;

        }

#if USE_TIMMER_BEHAVIOUR
    public override void OnFrameDestroy()
#else
        public void OnDestroy()
#endif
        {
            if(UseSharedMaterial)
            {
                if (RunMode == EffectRunMode.Self)
                {
                    foreach (Renderer r in RenderList)
                    {
                        if (!r.sharedMaterial)
                        {
                            continue;
                        }
                        EffectUtily.GabageRender(r, r.sharedMaterial);
                        if (!r.sharedMaterial.HasProperty(TexturePropertyName))
                        {
                            continue;
                        }
                        Vector2 uv;
                        orginalUVScroll.TryGetValue(r.sharedMaterial, out uv);
                        r.sharedMaterial.SetTextureOffset(TexturePropertyName, uv);
                    }

                }
            }


            matInstances.Clear();
            matInstances = null;
            RenderList.Clear();
            RenderList = null;

        }

        public void Play()
        {
            if (!bInit || bPlay)
            {
                return;
            }

            if (!bStartPlayYield)
            {
                if (fStartDelay > 0.03f)
                {
                    GameUtil.StartCoroutineHelper(this,DelayPlay());
                    bStartPlayYield = true;
                }
                else
                {
                    PlayImpl();
                }
            }

        }

        IEnumerator DelayPlay()
        {
            yield return new WaitForSeconds(fStartDelay);
            bStartPlayYield = false;
            PlayImpl();
            
        }

        private void PlayImpl()
        {
            DuringHoldScrollTime = Vector2.zero;
            currRotateXTime = 0.0f;
            currRotateYTime = 0.0f;
            currRotateZTime = 0.0f;

            bPlay = true;
        }

        public void Stop()
        {
            if (!bInit)
            {
                return;
            }


            StopAllCoroutines();
            ResetValue();
            bStartPlayYield = false;
            bPlay = false;
        }

        public void Init()
        {
            if (bInit)
            {
                return;
            }
            RenderList.Clear();
            if (RunMode == EffectRunMode.Self)
            {
                GenerateSelfMaterial();
            }

            OrginalLocalRoate = transform.localRotation;
            OrginalWorldRoate = transform.rotation;
            currRotateXTime = 0.0f;
            currRotateYTime = 0.0f;
            currRotateZTime = 0.0f;
            bInit = true;
        }


        public void SetMaterialInstance(List<Material> MatInstances)
        {
            if (RunMode == EffectRunMode.Self)
            {
                return;
            }
            matInstances.Clear();
            foreach (Material mat in MatInstances)
            {
                if (!mat.HasProperty(TexturePropertyName))
                {
                    Debug.LogWarning("物体:" + gameObject.name + "的材质球:" + mat + "没有属性:" + TexturePropertyName);
                    continue;
                }
                matInstances.Add(mat);
            }

            ResetValue();
        }


        public void SetMaterialInstance(HashSet<Material> MatInstances)
        {
            if (RunMode == EffectRunMode.Self)
            {
                return;
            }
            matInstances.Clear();
            foreach (Material mat in MatInstances)
            {
                if (!mat.HasProperty(TexturePropertyName))
                {
                    Debug.LogWarning("物体:" + gameObject.name + "的材质球:" + mat + "没有属性:" + TexturePropertyName);
                    continue;
                }
                matInstances.Add(mat);
            }

            ResetValue();
        }

        public void SetMaterialInstance(Material[] MatInstances)
        {
            if (RunMode == EffectRunMode.Self)
            {
                return;
            }
            matInstances.Clear();
            foreach (Material mat in MatInstances)
            {
                if (!mat.HasProperty(TexturePropertyName))
                {
                    Debug.LogWarning("物体:" + gameObject.name + "的材质球:" + mat + "没有属性:" + TexturePropertyName);
                    continue;
                }
                matInstances.Add(mat);
            }

            ResetValue();
        }

        public void SetMaterialInstance(Material MatInstance)
        {
            if (RunMode == EffectRunMode.Self)
            {
                return;
            }
            matInstances.Clear();
            if (!MatInstance.HasProperty(TexturePropertyName))
            {
                Debug.LogWarning("物体:" + gameObject.name + "的材质球:" + MatInstance + "没有属性:" + TexturePropertyName);
                return;
            }
            matInstances.Add(MatInstance);
            ResetValue();
        }

        public void ClearMaterialInstance()
        {
            matInstances.Clear();
        }

        public void GenerateSelfMaterial()
        {
            matInstances.Clear();

            if (IncludeChildren)
            {
                RenderList.AddRange(GetComponentsInChildren<Renderer>());
            }
            else
            {
                RenderList.AddRange(GetComponents<Renderer>());
            }
            for (int i = 0; i < RenderList.Count;i++ )
            {
                Renderer r = RenderList[i];
                if (!r.sharedMaterial)
                {
                    continue;
                }
                if (!r.sharedMaterial.HasProperty(TexturePropertyName))
                {
                    Debug.LogWarning("物体:" + gameObject.name + "的材质球:" + r.sharedMaterial.name + "没有属性:" + TexturePropertyName);
                    continue;
                }
                if (RunMode == EffectRunMode.Self)
                {
                    if (UseSharedMaterial)
                    {
                        matInstances.Add(r.sharedMaterial);
                        EffectUtily.DispatchRender(r, r.sharedMaterial);
                        orginalUVScroll.Add(r.sharedMaterial, r.sharedMaterial.GetTextureOffset(TexturePropertyName));
                    }
                    else
                    {
                        matInstances.Add(r.material);
                        EffectUtily.DispatchRender(r, r.material);
                    }
                }


            }
            ChangeUseModeUV();
        }

        public void SetRenderQuen(int RenderQuen)
        {
            if (matInstances.Count > 0)
            {
                foreach (Material mat in matInstances)
                {
                    mat.renderQueue = RenderQuen;
                }
            }
        }

        public void ResetValue()
        {
            DuringHoldScrollTime = Vector2.zero;
            currentScrollWaitingTime = Vector2.zero;
            canScroll = Vector2.one;
            currentScrollDealtTime = Vector2.zero;
            transform.rotation = OrginalWorldRoate;
            transform.localRotation = OrginalLocalRoate;
            currRotateXTime = 0.0f;
            currRotateYTime = 0.0f;
            currRotateZTime = 0.0f;

            if (matInstances.Count > 0 && matInstances[0])
            {
                Vector2 orgOffset = matInstances[0].GetTextureOffset(TexturePropertyName);
                startOffset.Clear();
                foreach (Material mat in matInstances)
                {
                    if (!UseRendererOffset)
                    {
                        mat.SetTextureOffset(TexturePropertyName, Vector3.zero);
                        startOffset.Add(Vector2.zero);
                    }
                    else
                    {
                        startOffset.Add(mat.GetTextureOffset(TexturePropertyName)-orgOffset);
                    }
                    if (UseModelUV)
                    {
                        mat.EnableKeyword("USE_MODE_UV");
                    }
                    else
                    {
                        mat.DisableKeyword("USE_MODE_UV");
                    }
                    OldUseModelUV = UseModelUV;
                }
            }
            ChangeUseModeUV();
            //RoateSpeed = Vector3.zero;
        }

        private void ChangeUseModeUV()
        {
            
            if (matInstances.Count > 0)
            {
                foreach (Material mat in matInstances)
                {
                    if(!mat)
                    {
                        continue;
                    }
                    if (UseModelUV)
                    {
                        mat.EnableKeyword("USE_MODE_UV_ON");
                        mat.DisableKeyword("USE_MODE_UV_OFF");
                        EffectUtily.UpdateKeyWorld(mat, "USE_MODE_UV_ON", true);
                        EffectUtily.UpdateKeyWorld(mat, "USE_MODE_UV_OFF", false);
                    }
                    else
                    {
                        mat.EnableKeyword("USE_MODE_UV_OFF");
                        mat.DisableKeyword("USE_MODE_UV_ON");
                        EffectUtily.UpdateKeyWorld(mat, "USE_MODE_UV_ON", false);
                        EffectUtily.UpdateKeyWorld(mat, "USE_MODE_UV_OFF", true);
                    }
                }
            }
            OldUseModelUV = UseModelUV;
        }

        public void Start()
        {
            Init();
        }

        public void Awake()
        {
#if USE_TIMMER_BEHAVIOUR
            startTimmer(3, UpdateBehaviourOnTimer.TIMER_THREAD_MID);
#endif
        }

#if USE_TIMMER_BEHAVIOUR
        public override void OnUpdate()
#else
        public void Update()
#endif  
        {
            if (bPlay)
            {
                UVScroll();
                UVRotate();
            }

            if (OldUseModelUV != UseModelUV)
            {
                ChangeUseModeUV();
            }

        }


        private void UVRotate()
        {
            if (RotateSpeed != Vector3.zero)
            {
                if (RoateInWorldSpace)
                {
                    Vector3 realyRotateSpeed = RotateSpeed;

                    if (RotateXTime > 0 && currRotateXTime < RotateXTime)
                    {
                        currRotateXTime += Time.deltaTime;
                    }
                    else
                    {
                        realyRotateSpeed.x = 0.0f;
                    }

                    if (RotateYTime > 0 && currRotateYTime < RotateYTime)
                    {
                        currRotateYTime += Time.deltaTime;
                    }
                    else
                    {
                        realyRotateSpeed.y = 0.0f;
                    }

                    if (RotateZTime > 0 && currRotateZTime < RotateZTime)
                    {
                        currRotateZTime += Time.deltaTime;
                    }
                    else
                    {
                        realyRotateSpeed.z = 0.0f;
                    }

                    if (realyRotateSpeed != Vector3.zero)
                    {
                        transform.Rotate(realyRotateSpeed * Time.deltaTime, Space.World);
                    }
                        
                }
                else
                {
                    Vector3 realyRotateSpeed = RotateSpeed;

                    
                    if (RotateXTime > 0 && currRotateXTime < RotateXTime)
                    {
                        currRotateXTime += Time.deltaTime;
                    }
                    else
                    {
                        realyRotateSpeed.x = 0.0f;
                    }

                    if (RotateYTime > 0 && currRotateYTime < RotateYTime)
                    {
                        currRotateYTime += Time.deltaTime;
                    }
                    else
                    {
                        realyRotateSpeed.y = 0.0f;
                    }

                    if (RotateZTime > 0 && currRotateZTime < RotateZTime)
                    {
                        currRotateZTime += Time.deltaTime;
                    }
                    else
                    {
                        realyRotateSpeed.z = 0.0f;
                    }

                    if (realyRotateSpeed != Vector3.zero)
                    {
                        transform.Rotate(realyRotateSpeed * Time.deltaTime, Space.Self);
                    }
                    
                }

            }
        }

        private void UVScroll()
        {
            if (matInstances.Count > 0)
            {
                Vector2 offset = matInstances[0].GetTextureOffset(TexturePropertyName);
                Vector2 realyScrollSpeed = ScrollSpeed;
                float deltaTime = Time.deltaTime;
                DuringHoldScrollTime +=  Vector2.one * deltaTime;

                if (canScroll.x < 0)
                {
                    currentScrollWaitingTime.x -= deltaTime;
                    if (currentScrollWaitingTime.x < 0.0f)
                    {
                        currentScrollWaitingTime.x = 0.0f;
                        canScroll.x = 1.0f;
                    }
                }

                if (canScroll.y < 0)
                {
                    currentScrollWaitingTime.y -= deltaTime;
                    if(currentScrollWaitingTime.y < 0.0f)
                    {
                        currentScrollWaitingTime.y = 0.0f;
                        canScroll.y = 1.0f;
                    }
                }

                if (ScrollXTime <= 0 || Mathf.Abs(DuringHoldScrollTime.x) > ScrollXTime ||
                    canScroll.x < 0.0f)
                {
                    realyScrollSpeed.x = 0.0f;
                }
                if (ScrollYTime <= 0 || Mathf.Abs(DuringHoldScrollTime.y) > ScrollYTime ||
                    canScroll.y < 0.0f)
                {
                    realyScrollSpeed.y = 0.0f;
                }

                if (realyScrollSpeed == Vector2.zero)
                {
                    return;
                }

                if (!GameUtil.FloatEquls(realyScrollSpeed.x, 0.0f))
                {
                    currentScrollDealtTime.x += deltaTime;
                }

                if (!GameUtil.FloatEquls(realyScrollSpeed.y, 0.0f))
                {
                    currentScrollDealtTime.y += deltaTime;
                }

                //计算流动一次所需要的时间
                ScrollOnceTime.x = -1.0f;
                if (!GameUtil.FloatEquls(ScrollSpeed.x, 0.0f))
                {
                    ScrollOnceTime.x = 1.0f / Mathf.Abs(ScrollSpeed.x);
                }

                ScrollOnceTime.y = -1.0f;
                if (!GameUtil.FloatEquls(ScrollSpeed.y, 0.0f))
                {
                    ScrollOnceTime.y = 1.0f / Mathf.Abs(ScrollSpeed.y);
                }

                //看看流动的时间是否到了,如果到了，要等待时间完毕
                if(ScrollOnceTime.x > 0.0f)
                {
                    if(currentScrollDealtTime.x > ScrollOnceTime.x)
                    {
                        currentScrollDealtTime.x = 0.0f;
                        currentScrollWaitingTime.x = BetweenTimes.x;
                        canScroll.x = -1.0f;
                    }
                }

                if(ScrollOnceTime.y > 0.0f)
                {
                    if (currentScrollDealtTime.y > ScrollOnceTime.y)
                    {
                        currentScrollDealtTime.y = 0.0f;
                        currentScrollWaitingTime.y = BetweenTimes.y;
                        canScroll.y = -1.0f;
                    }
                }
                offset += realyScrollSpeed * deltaTime;
                if (offset.x > 1)
                {
                    offset.x = 0;
                }

                if (offset.x < -1)
                {
                    offset.x = 0;
                }

                if (offset.y > 1)
                {
                    offset.y = 0;
                }

                if (offset.y < -1)
                {
                    offset.y = 0;
                }

                for (int i = 0; i < matInstances.Count; i++)
                {
                    Material mat = matInstances[i];
                    Vector2 offseti = offset;
                    if (matInstances.Count == startOffset.Count)
                    {
                        offseti += startOffset[i];
                    }
                    mat.SetTextureOffset(TexturePropertyName, offseti);
                    EffectUtily.UpdateUVScroll(mat, TexturePropertyName, offseti);
                }
            }
        }
    }

}

