/*******************************************************************
** 文件名:	Effect_FullScreen.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/10
** 版  本:	1.0
** 描  述:	全屏特效脚本
** 应  用:  	全屏特效脚本

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using ImageEffects;

namespace Effect
{
    public sealed class Effect_FullScreen : MonoBehaviour
    {

        public Effect_FadeInOutMaterialColor FadeInOutMaterialColor = null;
        public Effect_FadeInOutMaterialFloat FadeInOutMaterialFloat = null;
        public Effect_UVTextureAnimator UVTextureAnimator = null;
        public Effect_UVScroll UVScroll = null;

        [Range(-1.0f,1.0f)]
        public float sortOrder = 0.0f;

        /// <summary>
        /// 随机位置
        /// </summary>
        public bool RandomPos = false;


        public Vector2 CorrectScale = Vector2.one;
        public Vector2 CorrectPos = Vector2.zero;

        public Material mat;

        public Material InstanceMat
        {
            get
            {
                if (Application.isPlaying)
                {
                    if (OldMat != mat)
                    {
                        if (m_InstanceMat)
                        {
                            ResNode.DestroyRes(ref m_InstanceMat);
                            m_InstanceMat = null;
                        }
                        OldMat = mat;
                        m_InstanceMat = new Material(mat);
                        //m_InstanceMat.hideFlags = HideFlags.HideAndDontSave;
                    }
                   
                    return m_InstanceMat;
                }
                return mat;
            }
        }

        private Material m_InstanceMat;
        private Material OldMat = null;
        private bool bInit = false;
        private GameObject HostGo = null;
        private Transform HostTransform = null;
        private MeshRenderer hostRender = null;
        private Vector3 RelayScale = Vector3.one;

        private int lastScreenHeight = 0;
        private int lastScreenWidth = 0;
        private float lastFieldOfView = 0f;
        private static float unScaleflags_fov = 0.0f;
        private static float unScaleflags_srceen = 0.0f;
        private float m_PixelToMeterScale = 1.0f;
        private Vector2 FullScreenScale = Vector2.one;
        private Camera HostCamera = null;
        private float m_zDistance = 0.0f;
        private Vector3 posMax = Vector3.zero;
        private Vector3 posMin = Vector3.zero;
        public void OnDestroy()
        {
            FadeInOutMaterialColor = null;
            FadeInOutMaterialFloat = null;
            UVTextureAnimator = null;
            UVScroll = null;
            HostTransform = null;
            hostRender = null;
            HostCamera = null;
            if (m_InstanceMat)
            {
                ResNode.DestroyRes(ref m_InstanceMat);
                if (HostGo)
                {
                    ResNode.DestroyRes(ref HostGo);
                }
                
            }
        }



        public void Play(int entityID)
        {
            if (!bInit)
            {
                return;
            }
            if (HostCamera)
            {
                if(Initialize.isOnGameRuning && SceneManager.Instance)
                {
                    if(GameLogicAPI.isMainMap(SceneManager.Instance.m_curMapID))
                    {
                        Vector3 refPos = Vector3.zero;
                        U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(entityID);
                        if (ev != null && ev.StateMachine)
                        {
                            refPos = ev.StateMachine.transform.position;
                            //距离太远，不播放
                            if (GameUtil.GetSqrDistanceWithOutY(refPos, HostCamera.transform.position) > 200)
                            {
                                return;
                            }
                        }
                    }
                }
            }
            LateUpdate();

            if (HostGo)
            {
                HostGo.SetActive(true);
            }

            if (RandomPos)
            {
                UpdateScreenPos();

                CorrectPos.x = Random.Range(posMin.x, posMax.x);
                CorrectPos.y = Random.Range(posMin.y, posMax.y);
            }

            EffectUtily.DispatchRender(hostRender, hostRender.sharedMaterial);
            PlayMaterialControl();
        }

        public void Stop()
        {
            if (!bInit)
            {
                return;
            }

            if (HostGo)
            {
                HostGo.SetActive(false);
            }
            EffectUtily.GabageRender(hostRender, hostRender.sharedMaterial);
            StopMaterialControl();
        }

        public void Init()
        {
            if (bInit || !Initialize.mainCam)
            {
                return;
            }
            CheckMaterialControl();
            HostGo = GameObject.CreatePrimitive(PrimitiveType.Quad);
            DontDestroyOnLoad(HostGo);
            Collider c = HostGo.GetComponent<Collider>();
            if(c)
            {
                ResNode.DestroyRes(ref c);
            }
            //HostGo.hideFlags = HideFlags.HideAndDontSave;
            HostGo.name = "FullScreenEfectObj";
            hostRender = HostGo.GetComponent<MeshRenderer>();
            hostRender.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
            hostRender.receiveShadows = false;
            HostTransform = HostGo.transform;
            HostCamera = Initialize.mainCam;
            HostTransform.SetParent(HostCamera.transform);
            Vector3 pos = Vector3.zero;
            m_zDistance = HostCamera.nearClipPlane + 0.01f - (sortOrder * 0.01f);
            pos.z = m_zDistance;
            HostTransform.localPosition = pos;
            HostTransform.rotation = Quaternion.identity;
            HostTransform.localRotation = Quaternion.identity;

            hostRender.sharedMaterial = InstanceMat;
            HostGo.SetActive(false);
            bInit = true;
        }


        public void Start()
        {
            Init();
            
        }

        public void LateUpdate()
        {
            if(!HostCamera)
            {
                HostCamera = Initialize.mainCam;
            }
            if(!HostCamera)
            {
                return;
            }
            if(!HostGo)
            {
                bInit = false;
                Init();
            }
            //不产生近大远小的效果
            m_zDistance = HostCamera.nearClipPlane + 0.01f - (sortOrder * 0.01f);

            if (!lastFieldOfView.Equals(HostCamera.fieldOfView) || lastScreenHeight != Screen.height ||
                lastScreenWidth != Screen.width || (Time.frameCount % 5 == 0))//每隔5帧，强制刷一遍
            {
                unScaleflags_srceen = 2f / Screen.height;
                unScaleflags_fov = Mathf.Tan(HostCamera.fieldOfView * 0.5f * Mathf.Deg2Rad);
                lastScreenHeight = Screen.height;
                lastScreenWidth = Screen.width;
                lastFieldOfView = HostCamera.fieldOfView;
                UpdateScreenPos();
            }

            m_PixelToMeterScale = unScaleflags_srceen * m_zDistance * unScaleflags_fov;

            

            FullScreenScale.x = Screen.width * m_PixelToMeterScale;
            FullScreenScale.y = Screen.height * m_PixelToMeterScale;

            RelayScale.x = FullScreenScale.x * CorrectScale.x;
            RelayScale.y = FullScreenScale.y * CorrectScale.y;
            RelayScale.z = 1.0f;

            if (HostTransform)
            {
                Vector3 pos = HostTransform.localPosition;
                pos.x = CorrectPos.x;
                pos.y = CorrectPos.y;
                pos.z = m_zDistance;
                HostTransform.localScale = RelayScale;
                
                HostTransform.localPosition = pos;
            }
        }

        /// <summary>
        /// 更新屏幕坐标点信息
        /// </summary>
        void UpdateScreenPos()
        {
            posMax.z = m_zDistance;
            posMax.x = 1.0f;
            posMax.y = 1.0f;

            posMin.z = m_zDistance;
            posMin.x = 0.0f;
            posMin.y = 0.0f;

            posMax = HostCamera.ViewportToWorldPoint(posMax);
            posMin = HostCamera.ViewportToWorldPoint(posMin);

            posMax = HostCamera.transform.worldToLocalMatrix * posMax;
            posMin = HostCamera.transform.worldToLocalMatrix * posMin;
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
        }

        void PlayMaterialControl()
        {
            //把材质设置进动画里
            if (FadeInOutMaterialColor)
            {
                //如果运行模式不是外部指定的
                if (FadeInOutMaterialColor.RunMode == EffectRunMode.OutSiedePointed)
                {
                    FadeInOutMaterialColor.SetMaterialInstance(InstanceMat);
                    FadeInOutMaterialColor.Play();
                }

            }

            if (FadeInOutMaterialFloat)
            {
                //如果运行模式不是外部指定的
                if (FadeInOutMaterialFloat.RunMode == EffectRunMode.OutSiedePointed)
                {
                    FadeInOutMaterialFloat.SetMaterialInstance(InstanceMat);
                    FadeInOutMaterialFloat.Play();
                }

            }

            if (UVTextureAnimator)
            {
                //如果运行模式不是外部指定的
                if (UVTextureAnimator.RunMode == EffectRunMode.OutSiedePointed)
                {
                    UVTextureAnimator.SetMaterialInstance(InstanceMat);
                    UVTextureAnimator.Play();
                }
            }

            if (UVScroll)
            {
                //如果运行模式不是外部指定的
                if (UVScroll.RunMode == EffectRunMode.OutSiedePointed)
                {
                    UVScroll.SetMaterialInstance(InstanceMat);
                    UVScroll.Play();
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
        }

        #endregion
    }



}

