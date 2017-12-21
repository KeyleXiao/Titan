/*******************************************************************
** 文件名:	ImageEffectManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/13
** 版  本:	1.0
** 描  述:	图像特效管理
** 应  用:  	管理所有程序写的各种特效

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using System.Text;
namespace ImageEffects
{
    //[ExecuteInEditMode]
    [RequireComponent(typeof(Camera))]
    public class ImageEffectOpaqueQueue : MonoBehaviour
    {
        public ImageEffectManager hostImageEffectManager = null;
        //OnRenderImage需要回读GPU消耗太高，可以选择用OnPre/PostRender代替，或者合并成一个（暂时方案）
        //[ImageEffectOpaque]
        //public void OnRenderImage(RenderTexture source, RenderTexture destination)
        //{
        //    if (hostImageEffectManager)
        //    {
        //        hostImageEffectManager.depthtexture = source;
        //        hostImageEffectManager.OnRenderImageOpaque(source, destination);
        //    }
        //    else
        //    {
        //        Graphics.Blit(source, destination);
        //    }
        //}
        //[ImageEffectOpaque]
        //public void OnPreRender()
        //{
        //    srcRenderTexture = RenderTexture.GetTemporary(Screen.width, Screen.height);
        //    Camera.main.targetTexture = srcRenderTexture;
        //}
        public RenderTexture srcRenderTexture;
        public void OnRender(RenderTexture srcRenderTexture,out RenderTexture reslut,out bool NeedToReleaseReslut)
        {
            if (hostImageEffectManager)
            {
                hostImageEffectManager.depthtexture = srcRenderTexture;
                hostImageEffectManager.OnRenderImageOpaque(srcRenderTexture, out reslut,out NeedToReleaseReslut);
            }
            else
            {
                reslut = srcRenderTexture;
                NeedToReleaseReslut = false;
            }
        }

        public void OnDestroy()
        {
            hostImageEffectManager = null;
        }
    }


    /// <summary>
    /// 图像特效管理
    /// </summary>
    /// 
    //[ExecuteInEditMode]
    [RequireComponent(typeof(Camera))]
    public  partial class ImageEffectManager :MonoBehaviour
    {
        public static int ImageEffectShaderLOD = int.MaxValue;

        [System.NonSerialized]
        public Camera HostCam = null;

        [System.NonSerialized]
        public GameObject hero = null;   //用于做后处理的区域控制

        public class ImageEffectNodeList
        {
            public List<ImageEffectNode> list = new List<ImageEffectNode>();
            public bool enable=true;


            public void Clear()
            {
                list.Clear();
            }

            public bool CorrectEnabled()
            {
                if(list.Count <= 0)
                {
                    return false;
                }
                foreach(ImageEffectNode nod in list)
                {
                    if(nod.enabled)
                    {
                        return true;
                    }
                }
                return false;
            }
        }

        private ImageEffectOpaqueQueue OpaqueQueue=null;

        public  System.Action<bool> OnFoucusChange = null;

        private Dictionary<int, CameraRenderFlagsNode> CameraRenderFlagsTable = new Dictionary<int, CameraRenderFlagsNode>();

        public List<ImageEffectNode> AllImageEffeNodeList
        {
            get
            {
                return m_AllImageEffeNodeList;
            }
        }
        private List<ImageEffectNode> m_AllImageEffeNodeList = new List<ImageEffectNode>();

        public void ApplicationFocusChanged(bool focusStatus)
        {
            if (null != OnFoucusChange)
            {
                OnFoucusChange(focusStatus);
            }
        }


        private  Dictionary<Type, ImageEffectNodeList> BuildOpaqueQueueTable()
        {
            Dictionary<Type, ImageEffectNodeList> table = new Dictionary<Type, ImageEffectNodeList>();
            table.Clear();
            table[typeof(EdgeDetectEffectNormals)] = new ImageEffectNodeList();
            table[typeof(SSAOPro)] = new ImageEffectNodeList();
            //table[typeof(ScreenSpaceAmbientOcclusion)] = new ImageEffectNodeList();
            //table[typeof(CandelaSSRREx)] = new ImageEffectNodeList();
            table[typeof(CandelaSSRR)] = new ImageEffectNodeList();
            table[typeof(GlobalFog)] = new ImageEffectNodeList();


            foreach(Type t in table.Keys)
            {
                if(AllEffectEnabledTable.ContainsKey(t))
                {
                    AllEffectEnabledTable[t] = false;
                }
                else
                {
                    AllEffectEnabledTable.Add(t, false);
                }
            }
            return table;
        }

        private  Dictionary<Type, ImageEffectNodeList> BuildNormalQueueTable()
        {
            Dictionary<Type, ImageEffectNodeList> table = new Dictionary<Type, ImageEffectNodeList>();
            table.Clear();

            table[typeof(DepthOfFieldScatter)] = new ImageEffectNodeList();
            table[typeof(FXAA)] = new ImageEffectNodeList();
            table[typeof(Tonemapping)] = new ImageEffectNodeList();
            table[typeof(ColorCorrectionCurves)] = new ImageEffectNodeList();
            table[typeof(VignetteAndChromaticAberration)] = new ImageEffectNodeList();

            table[typeof(VolumetricLightScattering)] = new ImageEffectNodeList();
            table[typeof(SunShafts)] = new ImageEffectNodeList();
            table[typeof(BloomAndFlares)] = new ImageEffectNodeList();
            table[typeof(GlowManager)] = new ImageEffectNodeList();
           
            
            table[typeof(DissolveGlowManager)] = new ImageEffectNodeList();
            table[typeof(GlowOutLineManager)] = new ImageEffectNodeList();
            //table[typeof(GlowOutLineManagerEx)] = new ImageEffectNodeList();
            //table[typeof(DynamicRayScattering)] = new ImageEffectNodeList();
            
          
            table[typeof(RadialBlur)] = new ImageEffectNodeList();
            table[typeof(FullScreenSaturation)] = new ImageEffectNodeList();
            

            table[typeof(BlurAndDownScreen)] = new ImageEffectNodeList();
            table[typeof(WarFog)] = new ImageEffectNodeList();
            table[typeof(ModifyGameBrightness)] = new ImageEffectNodeList();

            foreach (Type t in table.Keys)
            {
                if (AllEffectEnabledTable.ContainsKey(t))
                {
                    AllEffectEnabledTable[t] = false;
                }
                else
                {
                    AllEffectEnabledTable.Add(t, false);
                }
            }
            return table;
        }

        
        public void Awake()
        {
            InitSceneCamera();
        }

        public void OnDestroy()
        {
            ClearSceneCamera();
        }

        public void Update()
        {
            if (!HostCam)
            {
                HostCam = GetComponent<Camera>();
            }

            

            //force update in editor mode
            if (!Application.isPlaying)
            {
                if (OpaqueTypeEffectTable == null || OpaqueTypeEffectTable.Count <= 0)
                    OpaqueTypeEffectTable = BuildOpaqueQueueTable();
                if (NormalTypeEffectTable == null || NormalTypeEffectTable.Count <= 0)
                    NormalTypeEffectTable = BuildNormalQueueTable();
                dirty = true;
            }

            if (dirty)
            {
                Refresh();
                dirty = false;
            }

        }

        public  bool dirty = false;

        private  T Add<T>() where T : ImageEffectNode
        {
            T node = null;
            Type t = typeof(T);
            if (NormalTypeEffectTable.ContainsKey(t))
            {
                node = HostCam.gameObject.GetComponent<T>();
                if (node == null)
                {
                    node = HostCam.gameObject.AddComponent<T>();
                    ImageEffectNodeList effectnodelist = NormalTypeEffectTable[t];
                    effectnodelist.list.Add(node);
                    effectnodelist.enable = false;
                    AllEffectEnabledTable[t] = false;
                } 

            }
            else if (OpaqueTypeEffectTable.ContainsKey(t))
            {
                node = HostCam.gameObject.GetComponent<T>();
                if (node == null)
                {
                    node = HostCam.gameObject.AddComponent<T>();
                    ImageEffectNodeList effectnodelist = OpaqueTypeEffectTable[t];
                    effectnodelist.list.Add(node);
                    effectnodelist.enable = false;
                    AllEffectEnabledTable[t] = false;
                } 
            }
            return node;
        }

        private void Refresh()
        {
            m_AllImageEffeNodeList.Clear();
            foreach (ImageEffectNodeList effectnodelist in NormalTypeEffectTable.Values)
            {
                foreach(ImageEffectNode nod in effectnodelist.list)
                {
                    nod.OnDeActive();
                }
                effectnodelist.Clear();
            }
            foreach (ImageEffectNodeList effectnodelist in OpaqueTypeEffectTable.Values)
            {
                foreach (ImageEffectNode nod in effectnodelist.list)
                {
                    nod.OnDeActive();
                }
                effectnodelist.Clear();
            }
            ResetEffectEnabledTable();
            ImageEffectNode[] effts = HostCam.GetComponents<ImageEffectNode>();
            m_AllImageEffeNodeList.AddRange(effts);
            foreach (ImageEffectNode effectnode in effts)
            {
                Type t = effectnode.GetType();
                if (NormalTypeEffectTable.ContainsKey(t))
                {
                    ImageEffectNodeList lis = NormalTypeEffectTable[t];
                    lis.list.Add(effectnode);
                }
                else if (OpaqueTypeEffectTable.ContainsKey(t))
                {
                    ImageEffectNodeList lis = OpaqueTypeEffectTable[t];
                    lis.list.Add(effectnode);
                }
            }

            foreach (ImageEffectNodeList effectnodelist in NormalTypeEffectTable.Values)
            {
                if ( effectnodelist.CorrectEnabled() && effectnodelist.enable)
                {
                    if (effectnodelist.list[0].IsOnValidLOD())
                    {
                        AllEffectEnabledTable[effectnodelist.list[0].GetType()] = true;
                        foreach (ImageEffectNode nod in effectnodelist.list)
                        {
                            nod.OnActive();
                        }
                    }
                }
            }
            foreach (ImageEffectNodeList effectnodelist in OpaqueTypeEffectTable.Values)
            {
                if (effectnodelist.CorrectEnabled() && effectnodelist.enable)
                {
                    if (effectnodelist.list[0].IsOnValidLOD())
                    {
                        AllEffectEnabledTable[effectnodelist.list[0].GetType()] = true;
                        foreach (ImageEffectNode nod in effectnodelist.list)
                        {
                            nod.OnActive();
                        }


                    }
                }
            }
            UpdateEnabled();
        }

        private void SetImagEffectParam<T>(string paramName, float Val) where T : ImageEffectNode
        {
            ImageEffectNodeList list = null;
            Type t = typeof(T);
            if(NormalTypeEffectTable.TryGetValue(t,out list))
            {
                if(list.list.Count > 0)
                {
                    foreach(ImageEffectNode nod in list.list)
                    {
                        nod.SetParam(paramName, Val);
                    }
                }
            }

            if (OpaqueTypeEffectTable.TryGetValue(t, out list))
            {
                if (list.list.Count > 0)
                {
                    foreach (ImageEffectNode nod in list.list)
                    {
                        nod.SetParam(paramName, Val);
                    }
                }
            }
        }

        private void Remove<T>() where T : ImageEffectNode
        {
            T node = HostCam.gameObject.GetComponent<T>();
            if (node != null)
            {
                Type t = typeof(T);
                if (NormalTypeEffectTable.ContainsKey(t))
                {
                    ImageEffectNodeList effectnodelist = NormalTypeEffectTable[t];
                    effectnodelist.list.Remove(node);
                    effectnodelist.enable = effectnodelist.CorrectEnabled();
                    AllEffectEnabledTable[t] = effectnodelist.enable;

                }
                else if (OpaqueTypeEffectTable.ContainsKey(t))
                {
                    ImageEffectNodeList effectnodelist = NormalTypeEffectTable[t];
                    effectnodelist.list.Remove(node);
                    effectnodelist.enable = effectnodelist.CorrectEnabled();
                    AllEffectEnabledTable[t] = effectnodelist.enable;
                }
            } 
        }


        public  void DisableAllImageEffect()
        {
            foreach (ImageEffectNodeList effectnodelist in NormalTypeEffectTable.Values)
            {
                effectnodelist.enable = false;
                foreach (ImageEffectNode nod in effectnodelist.list)
                {
                    nod.OnDeActive();
                }
            }
            foreach (ImageEffectNodeList effectnodelist in OpaqueTypeEffectTable.Values)
            {
                effectnodelist.enable = false;
                foreach (ImageEffectNode nod in effectnodelist.list)
                {
                    nod.OnDeActive();
                }
            }
            ResetEffectEnabledTable();
            UpdateEnabled();
        }

        /// <summary>
        /// 禁用特效
        /// </summary>
        /// <param name="cam"></param>
        private  void DisableImageEffect(Type t)
        {
            if (NormalTypeEffectTable.ContainsKey(t))
            {
                ImageEffectNodeList effectnodelist = NormalTypeEffectTable[t];

                AllEffectEnabledTable[t] = false;
                effectnodelist.enable = false;
                foreach (ImageEffectNode nod in effectnodelist.list)
                {
                    nod.OnDeActive();
                }
                UpdateEnabled();
            }
            else if (OpaqueTypeEffectTable.ContainsKey(t))
            {
                ImageEffectNodeList effectnodelist = OpaqueTypeEffectTable[t];

                AllEffectEnabledTable[t] = false;
                effectnodelist.enable = false;
                foreach (ImageEffectNode nod in effectnodelist.list)
                {
                    nod.OnDeActive();
                }
                UpdateEnabled();
            }
            else
            {
                Debug.LogWarning("在Table中找不到特效类型:" + t.ToString() + ",是否忘记在Init中添加进去？");
            }

           
        }

        private  void DisableImageEffect<T>() where T : ImageEffectNode
        {
            DisableImageEffect(typeof(T));
        }

        /// <summary>
        /// 启用特效
        /// </summary>
        /// <param name="cam"></param>
        private  void EnableImageEffect(Type t)
        {
            if (NormalTypeEffectTable.ContainsKey(t))
            {
                ImageEffectNodeList effectnodelist = NormalTypeEffectTable[t];
                if(effectnodelist.CorrectEnabled())
                {
                    effectnodelist.enable = true;
                    if (effectnodelist.list[0].IsOnValidLOD())
                    {
                        foreach (ImageEffectNode nod in effectnodelist.list)
                        {
                            nod.OnActive();
                        }

                        AllEffectEnabledTable[t] = true;
                        UpdateEnabled();
                    }
                    else
                    {

                        AllEffectEnabledTable[t] = false;
                        UpdateEnabled();
                    }
                }
                else
                {
                    AllEffectEnabledTable[t] = false;
                    UpdateEnabled();
                }
            }
            else if (OpaqueTypeEffectTable.ContainsKey(t))
            {
                ImageEffectNodeList effectnodelist = OpaqueTypeEffectTable[t];
                if(effectnodelist.CorrectEnabled())
                {
                    effectnodelist.enable = true;
                    if (effectnodelist.list[0].IsOnValidLOD())
                    {
                        foreach (ImageEffectNode nod in effectnodelist.list)
                        {
                            nod.OnActive();
                        }

                        AllEffectEnabledTable[t] = true;
                        UpdateEnabled();
                    }
                    else
                    {
                        AllEffectEnabledTable[t] = false;
                        UpdateEnabled();
                    }
                }
                else
                {
                    AllEffectEnabledTable[t] = false;
                    UpdateEnabled();
                }
            }
            else
            {
                Debug.LogWarning("在Table中找不到特效类型:" + t.ToString() + ",是否忘记在Init中添加进去？");
            }
        }

        private  void EnableImageEffect<T>() where T:ImageEffectNode
        {
            EnableImageEffect(typeof(T));
        }

        public void OnRenderImageOpaque(RenderTexture source, out RenderTexture reslut,out bool NeedToReleaseReslut)
        {
            OnRenderImageQueue(source, true, out reslut, out NeedToReleaseReslut);
        }

        //统一管理，全部走着一个Image，避免深度缓冲混乱
        protected void OnRenderImageQueue(RenderTexture source, bool opaque, out RenderTexture reslut, out bool NeedToReleaseReslut)
        {
            RenderBuffer depthbuffer = depthtexture.depthBuffer;// source.depthBuffer;
            RenderTexture[] ret = new RenderTexture[2];
            ret[0] = source;
            ret[1] = RenderTexture.GetTemporary(source.width, source.height,0,source.format);
            RenderTexture releaseMe = ret[1];

            int index = 0;
            //是否处理过
            bool bDealth = false;

            if (opaque)
            {
                if (OpaqueTypeEffectTable != null)
                {
                    foreach (ImageEffectNodeList nodelist in OpaqueTypeEffectTable.Values)
                    {
                        if (nodelist.enable)
                        {
                            foreach (ImageEffectNode node in nodelist.list)
                            {
                                if (node)
                                {
                                    if (node.enabled)
                                    {
                                        if(node.ImageEffect_RenderImage(ret[index], ret[1 - index], depthbuffer))
                                        {
                                            index = 1 - index;
                                            bDealth = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else 
            {
                if (NormalTypeEffectTable != null)
                {
                    foreach (ImageEffectNodeList nodelist in NormalTypeEffectTable.Values)
                    {
                        if (nodelist.enable)
                        {
                            foreach (ImageEffectNode node in nodelist.list)
                            {
                                if (node)
                                {
                                    if (node.enabled)
                                    {
                                        if(node.ImageEffect_RenderImage(ret[index], ret[1 - index], depthbuffer))
                                        {
                                            index = 1 - index;
                                            bDealth = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (bDealth)
            {
                if(ret[index] == source)
                {
                    reslut = source;
                    NeedToReleaseReslut = false;
                }
                else
                {
                    reslut = ret[index];
                    NeedToReleaseReslut = true;
                }
            }
            else
            {
                reslut = source;
                NeedToReleaseReslut = false;
            }

            if (!NeedToReleaseReslut)
            {
                RenderTexture.ReleaseTemporary(releaseMe);
            }

        }

        //public RenderTexture srcRenderTexture;
        //public RenderTexture srcRenderTexture2;
        //public RenderTexture destRenderTexture;
        //void OnPreRender()
        //{
        //    srcRenderTexture = RenderTexture.GetTemporary(Screen.width, Screen.height,16);
        //    Camera.main.targetTexture = srcRenderTexture;
        //}

        //void OnPostRender()
        //{
        //    //先渲染Opaque
        //    srcRenderTexture2 = RenderTexture.GetTemporary(Screen.width, Screen.height,16);

        //    this.OpaqueQueue.OnRender(srcRenderTexture, srcRenderTexture2);

        //    //渲染其他的
        //    destRenderTexture = RenderTexture.GetTemporary(Screen.width, Screen.height,16);
        //    Camera.main.targetTexture = destRenderTexture;   

        //    OnRenderOtherQueue(srcRenderTexture2, destRenderTexture);

        //    Camera.main.targetTexture = null as RenderTexture;   //null意味着输出到frame buffer
        //    Graphics.Blit(destRenderTexture, null as RenderTexture);

        //    RenderTexture.ReleaseTemporary(srcRenderTexture);
        //    RenderTexture.ReleaseTemporary(srcRenderTexture2);
        //    RenderTexture.ReleaseTemporary(destRenderTexture);
        //}

        //void OnRenderImage(RenderTexture source, RenderTexture destination)
        //{
        //    OnRenderOtherQueue(source, destination);
        //}
        //OnRenderImage需要回读GPU消耗太高，可以选择用OnPre/PostRender代替，或者合并成一个（暂时方案）
        public RenderTexture tempRenderTexture;
        public void OnRenderImage(RenderTexture source, RenderTexture destination)
        {
            RenderTexture reslutOpaque = null;
            bool NeedToReleaseReslutOpaque = false;


            this.OpaqueQueue.OnRender(source, out reslutOpaque, out NeedToReleaseReslutOpaque);

            RenderTexture reslutNormal = null;
            bool NeedToReleaseReslutNormal = false;

            //渲染其他的
            //this.depthtexture = source;
            OnRenderImageQueue(reslutOpaque, false, out reslutNormal, out NeedToReleaseReslutNormal);

            tempRenderTexture = reslutNormal;
            Graphics.Blit(reslutNormal, destination);

            if(NeedToReleaseReslutOpaque)
            {
                RenderTexture.ReleaseTemporary(reslutOpaque);
            }

            if (NeedToReleaseReslutNormal)
            {
                //有可能是同一张
                if(reslutOpaque != reslutNormal)
                {
                    RenderTexture.ReleaseTemporary(reslutNormal);
                }
               
            }

        }

        private void ResetEffectEnabledTable()
       {
           foreach (Type effectnodelist in OpaqueTypeEffectTable.Keys)
           {
               AllEffectEnabledTable[effectnodelist] = false;
           }
           foreach (Type effectnodelist in NormalTypeEffectTable.Keys)
           {
               AllEffectEnabledTable[effectnodelist] = false;
           }
       }

        private bool bInitSceneCamera = false;

        public void InitSceneCamera()
        {
            if (bInitSceneCamera)
            {
                return;
            }
            bInitSceneCamera = true;

            HostCam = GetComponent<Camera>();
            OpaqueQueue = GetComponent<ImageEffectOpaqueQueue>();
            if (OpaqueQueue == null)
            {
                OpaqueQueue = gameObject.AddComponent<ImageEffectOpaqueQueue>();
            }


            if (OpaqueTypeEffectTable == null || OpaqueTypeEffectTable.Count <= 0)
                OpaqueTypeEffectTable = BuildOpaqueQueueTable();
            if (NormalTypeEffectTable == null || NormalTypeEffectTable.Count <= 0)
                NormalTypeEffectTable = BuildNormalQueueTable();

            Add<RadialBlur>();
            DisableImageEffect<RadialBlur>();

            Add<BlurAndDownScreen>();
            DisableImageEffect<BlurAndDownScreen>();

            Add<FullScreenSaturation>();
            DisableImageEffect<FullScreenSaturation>();

            Add<ModifyGameBrightness>();
            DisableImageEffect<ModifyGameBrightness>();

            
            HostCam.depthTextureMode = DepthTextureMode.None;

            Refresh();

            OpaqueQueue.hostImageEffectManager = this;
            ImageEffectManagerTable.Add(HostCam, this);

            foreach (ImageEffectNodeList nodelist in OpaqueTypeEffectTable.Values)
            {
                foreach (ImageEffectNode node in nodelist.list)
                {
                    if (node)
                    {
                        node.OnSceneCameraInit();
                    }
                }
            }

            foreach (ImageEffectNodeList nodelist in NormalTypeEffectTable.Values)
            {
                foreach (ImageEffectNode node in nodelist.list)
                {
                    if (node)
                    {
                        node.OnSceneCameraInit();
                    }
                }
            }
        }

        public void ClearSceneCamera()
        {
            if (!bInitSceneCamera)
            {
                return;
            }
            bInitSceneCamera = false;
            if (ImageEffectManager.IsStandAlone)
            {
                CameraRenderFlagsTable.Clear();
                ImageEffectManagerTable.Clear();
            }
            else
            {
                CameraRenderFlagsTable.Clear();
                ImageEffectManagerTable.Remove(HostCam);
            }

        }

        public CameraRenderFlagsNode AttachCameraRenderFlags(DepthTextureMode mod)
        {
            CameraRenderFlagsNode reslut = null;
            if(!bInitSceneCamera)
            {
                return reslut;
            }
            int key = (int)mod;

            if (!CameraRenderFlagsTable.TryGetValue(key, out reslut))
            {
                HostCam.depthTextureMode |= mod;
                reslut = new CameraRenderFlagsNode();
                reslut.hostCamera = HostCam;
                reslut.flags = mod;
                reslut.guid = key;
                CameraRenderFlagsTable.Add(key, reslut);
            }
            reslut.RefCount++;

            return reslut;
        }

        public void DeAttachCameraRenderFlags(ref CameraRenderFlagsNode flags)
        {
            if (null == flags)
            {
                return;
            }
            if (!flags.hostCamera || flags.guid < 0)
            {
                return;
            }

            CameraRenderFlagsNode result;
            if (!CameraRenderFlagsTable.TryGetValue(flags.guid, out result))
            {
                return;
            }
            flags.RefCount--;

            if (flags.RefCount <= 0)
            {
                CameraRenderFlagsTable.Remove(flags.guid);
                flags.hostCamera.depthTextureMode &= ~flags.flags;
            }
            flags = null;

        }

       public RenderTexture depthtexture;


       private void UpdateEnabled()
       {
           enabled = false;
           OpaqueQueue.enabled = false;
           foreach(bool b in  AllEffectEnabledTable.Values)
           {
               if(b)
               {
                   enabled = true;
                   OpaqueQueue.enabled = true;
                   return;
               }
           }

       }

        Dictionary<Type, bool> AllEffectEnabledTable = new Dictionary<Type,bool>();

        //bool bInit = false;
        Dictionary<Type, ImageEffectNodeList> OpaqueTypeEffectTable = null;
        Dictionary<Type, ImageEffectNodeList> NormalTypeEffectTable = null;
    }
}

