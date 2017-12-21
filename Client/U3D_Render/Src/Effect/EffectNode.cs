/*******************************************************************
** 文件名:	EffectNode.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	特效节点
** 应  用:  	特效节点，挂在根节点上面

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

//粒子时长计算有问题，全部循环和动画问题
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using ImageEffects;
namespace Effect
{
    /// <summary>
    /// 特效节点
    /// </summary>
    public partial class EffectNode : EffectNodeBase
    {
        #region 外部成员
        public static int AllEffectCount = 0;

        /// <summary>
        /// 美术调式模式
        /// </summary>
        [System.NonSerialized]
        public bool DebugMode = false;


        /// <summary>
        /// 特效长度,美术填写
        /// </summary>
        public float EffectLength = 3.0f;

        /// <summary>
        /// 实体ID,这个特效影响哪个实体ID，并不是所有的脚本都需要，某些脚本需要，如Play，UVAinmator等
        /// </summary>
        [System.NonSerialized]
        public int EntityID = -1;

        /// <summary>
        /// 源位置，这个特效的源物体是哪个，某些脚本需要，如LineRender等
        /// </summary>
        [System.NonSerialized]
        public Transform SrcObj = null;

        /// <summary>
        /// 目标位置，这个特效的目标物体是哪个，某些脚本需要，如LineRender等
        /// </summary>
        [System.NonSerialized]
        public Transform DstObj = null;

        [HideInInspector]
        public bool bBaked = false;

        /// <summary>
        /// 强制最高等级
        /// </summary>
        [System.NonSerialized]
        public bool ForceBestQuality = false;

        /// <summary>
        /// 使用测试模型，对于那些增加材质/替换材质的脚本在entityID的模式下，勾上这个，可使用测试模型，避免效果cache不了
        /// </summary>
        public static bool UseTestModel = false;


        /// <summary>
        /// 光效里面的碰撞，光效渲染相关的东西只能是linghtingeffectonly，碰撞可以是其他
        /// </summary>
        [HideInInspector]
        public Collider[] effectColliders = null;

        private GameObject currDstObj = null;
        private GameObject currSrcObj = null;

        #region 刀痕相关

        /// <summary>
        /// 是否需要刀痕
        /// </summary>
        public bool hasMarkDecal;

        /// <summary>
        /// 刀痕相对于人物正前方的偏移方向
        /// </summary>
        public Transform decalTran;

        /// <summary>
        /// 刀痕的倾斜角度
        /// </summary>
        public Vector3 decalScale;

        /// <summary>
        /// 刀痕的倾斜角度
        /// </summary>
        public float decalAngle;

        /// <summary>
        /// 刀痕相对于人物正前方的偏移方向
        /// </summary>
        public Vector3 decalOffsetDir;

        /// <summary>
        /// 是否使用图集
        /// </summary>
        public bool useAltas = false; 

        /// <summary>
        /// 刀痕纹理
        /// </summary>
        public Sprite decalSprite;

        /// <summary>
        /// 刀痕材质
        /// </summary>
        public Material decalMat;

        /// <summary>
        /// 刀痕延迟时间
        /// </summary>
        public float decalDelayTime;

        /// <summary>
        /// 刀痕是否需要淡入淡出
        /// </summary>
        public bool decalNeedFade = true;

        private float decalFadeInStartTime = 0;

        /// <summary>
        /// 刀痕淡入结束时间
        /// </summary>
        public float decalFadeInEndTime = 0.1f;
        /// <summary>
        /// 刀痕淡出开始时间
        /// </summary>
        public float decalFadeOutStartTime = 1.5f;
        /// <summary>
        /// 刀痕淡出结束时间
        /// </summary>
        public float decalFadeOutEndTime = 2.5f;

        #endregion

        #endregion

        #region Cache
        bool bInit = false;

        /// <summary>
        /// 是否掉用过Stop函数
        /// </summary>
        bool bCallStopFunc = false;

        /// <summary>
        /// 是否已经调用过play函数
        /// </summary>
        bool bCallPlayFunc = false;

        bool bPlay = false;

        bool isEnemy = false;

        /// <summary>
        /// 因为Unity的原因，有写特效默认是播放，有的默认是停止，所以，第一次调用stop的时候，全部调用一遍
        /// </summary>
        bool bFirstCallStop = false;
        #endregion


        /// <summary>
        /// 实际的等级
        /// </summary>
        private EffectQuality realyQuality = EffectQuality.Invalid;

        #region Speed
        public void SetParticleSpeed(float Speed)
        {
            SetParticleModluesSpeed(BestObj, BestModules, Speed);
            SetParticleModluesSpeed(HighObj, HighModlues, Speed);
            SetParticleModluesSpeed(GoodObj, GoodModlues, Speed);
            SetParticleModluesSpeed(FastestObj, FastestModlues, Speed);
        }

        private void SetParticleModluesSpeed(GameObject modluesObj, EffectModules moudlues, float Speed)
        {
            if (modluesObj)
            {
                if (!modluesObj.activeSelf)
                {
                    Debug.LogWarning("物体没有激活,失败:" + gameObject.name);
                    return;
                }
                if (null != moudlues && moudlues.isBaked)
                {
                    moudlues.SetParticleSpeed(Speed);
                }
                else
                {
                    Debug.LogWarning(name + "没有bake，请重新点击bake");
                    return;
                }
            }
        }
        #endregion

        #region Scale
        /// <summary>
        /// 设置缩放
        /// </summary>
        /// <param name="SizeScale">大小缩放</param>
        /// <param name="SpeedScale">速度缩放</param>
        /// /// <param name="AffectByParent">是否受父物体影响</param>
        public void SetScale(float SizeScale)
        {
            SetModluesScale(BestObj, BestModules, SizeScale);
            SetModluesScale(HighObj, HighModlues, SizeScale);
            SetModluesScale(GoodObj, GoodModlues, SizeScale);
            SetModluesScale(FastestObj, FastestModlues, SizeScale);
        }

        private void SetModluesScale(GameObject modluesObj,EffectModules moudlues,float sizeScale)
        {
            if (modluesObj)
            {
                if (!modluesObj.activeSelf)
                {
                    Debug.LogWarning("物体没有激活,:" + gameObject.name);
                    return;
                }
                if (null != moudlues && moudlues.isBaked)
                {
                    moudlues.SetScale(sizeScale);
                }
                else
                {
                    Debug.LogWarning(name + "没有bake，请重新点击bake");
                    return;
                }
            }
        }

        private void RestModluesScale(GameObject modluesObj,EffectModules moudlues)
        {
            if (modluesObj)
            {
                if (!modluesObj.activeSelf)
                {
                    Debug.LogWarning("物体没有激活,:" + gameObject.name);
                    return;
                }
                if (null != moudlues && moudlues.isBaked)
                {
                    moudlues.ResetScale();
                }
                else
                {
                    Debug.LogWarning(name + "没有bake，请重新点击bake");
                    return;
                }
            }
        }

        public void ResetScale()
        {
            RestModluesScale(BestObj, BestModules);
            RestModluesScale(HighObj, HighModlues);
            RestModluesScale(GoodObj, GoodModlues);
            RestModluesScale(FastestObj, FastestModlues);
        }


        #endregion

        public void Start()
        {
            //调用了play或者stop函数的话，这个就失效了
            if (PlayOnAwake && !bCallPlayFunc && !bCallStopFunc)
            {
                PlayEffect();
            }
            Initialize.OnDeviceStateChage += OnDeviceStateChanged;
        }

        public override void Init()
        {

            if (bInit)
            {
                return;
            }

            if (!bBaked)
            {
                Debug.LogWarning(name + ",没有bake过，请美术上传的时候点击bake按钮!");
                return;
            }

            if (isStaticEffect)
            {
                ResigterEffectQualityChange(OnStaticEffectQualityChange);
            }

            if (BestObj)
            {
                if (null != BestModules)
                {
                    BestModules.Init(this);
                }
                else
                {
                    Debug.LogWarning(name + "没有bake，请重新点击bake");
                }
            }

            if (HighObj)
            {
                if (null != HighModlues)
                {
                    HighModlues.Init(this);
                }
                else
                {
                    Debug.LogWarning(name + "没有bake，请重新点击bake");
                }
            }



            if (GoodObj)
            {
                if (null != GoodModlues)
                {
                    GoodModlues.Init(this);
                }
                else
                {
                    Debug.LogWarning(name + "没有bake，请重新点击bake");
                }
            }

            if (FastestObj)
            {
                if (null != FastestModlues)
                {
                    FastestModlues.Init(this);
                }
                else
                {
                    Debug.LogWarning(name + "没有bake，请重新点击bake");
                }
            }
            else
            {
                Debug.LogWarning(name + "--FastestObj 为空!");
            }

            bInit = true;
        }


        /// <summary>
        /// 播放特效
        /// </summary>
        public override void PlayEffect(bool bEnemy = false, int HostEntityID = - 1, Transform src = null, Transform dst = null, int mask = 0)
        {
            if (!PerformaceOk && !DebugMode)
            {
                Debug.LogWarning(StringHelper.BuildString("特效超标，不予以播放，请特效检查特效预制体:", gameObject.name));
                return;
            }

            //没有激活不播放
            if (bPlay || !gameObject.activeInHierarchy)
            {
                return;
            }
            bPlay = true;
            bCallPlayFunc = true;
            EntityID = HostEntityID;
            SrcObj = src;
            DstObj = dst;
            currMask = mask;
            isEnemy = bEnemy;
            if (!bInit)
            {
                Init();
            }

            BeginReplay();

            PlayImpl(mask);


        }


        /// <summary>
        /// 播放特效
        /// </summary>
        public override void PlayEffect(bool bEnemy, int HostEntityID, Vector3 srcPosition, Vector3 dstPosition, int mask = 0)
        {
            if (!PerformaceOk && !DebugMode)
            {
                Debug.LogError("特效超标，不予以播放，请特效检查特效预制体:" + gameObject.name);
                return;
            }

            //没有激活不播放
            if (bPlay || !gameObject.activeInHierarchy)
            {
                return;
            }

            GameObject go = getTargetCache();
            go.transform.SetParent(transform);
            go.transform.SetPosition(dstPosition);
            currDstObj = go;

            go = getTargetCache();
            go.transform.SetParent(transform);
            go.transform.SetPosition(srcPosition);
            currSrcObj = go;

            PlayEffect(bEnemy, HostEntityID, currSrcObj.transform, currDstObj.transform, mask);
            if (false && this.hasMarkDecal)
            {
                Vector3 decalDir = this.decalOffsetDir;
                decalDir.z = 0;
                Vector3 decalPosition = new Vector3();
                if (DecalSystem.Instance.CheckHitDecal(this.transform.position + decalDir, this.transform.forward, ref decalPosition))
                {
                    DecalBuildInfo info = new DecalBuildInfo();
                    info.decalPos = decalPosition;
                    info.decalEulerAngle = this.transform.eulerAngles;
                    info.decalEulerAngle.z = this.decalAngle;
                    info.decalScale = this.decalScale;
                    info.decalFadeInTime = this.decalFadeInEndTime - this.decalFadeInStartTime;
                    info.decalFadeTime = this.decalFadeOutStartTime - this.decalFadeInEndTime;
                    info.decalFadeOutTime = this.decalFadeOutEndTime - this.decalFadeOutStartTime;
                    info.decalMat = this.decalMat;
                    info.decalSprite = this.decalSprite;

                    if (this.decalDelayTime > 0)
                        DecalSystem.Instance.DelayBuildDecal(this, info, this.decalDelayTime);
                    else
                        DecalSystem.Instance.BuildDecal(info);
                }
            }
        }



        private void PlayImpl(int mask)
        {
            if (!bInit)
            {
                return;
            }
            AllEffectCount++;
            //if (!bWaitForComplete && !isLoop && DebugMode)
            //{
            //    GameUtil.StartCoroutineHelper(this,WaitForComplete());
            //    bWaitForComplete = true;
            //}

            //设备丢失了，不播放
            if(Initialize.isDeviceLost)
            {
                return;
            }

            if (/*DebugMode ||*/ ForceBestQuality)
            {
                PlayModlues(BestObj, BestModules, isEnemy, mask);
                PlayModlues(HighObj, HighModlues, isEnemy, mask);
                PlayModlues(GoodObj, GoodModlues, isEnemy, mask);
                PlayModlues(FastestObj, FastestModlues, isEnemy, mask);
                return;
            }
            realyQuality = ImageSetting.GetData().currEffectQuality;

            //等级无效，不播放
            if (realyQuality == EffectQuality.Invalid)
            {
                return;
            }

            ApplyMask(mask);

            if (realyQuality == EffectQuality.QUALITY_FASTEST)
            {
                PlayModlues(FastestObj, FastestModlues, isEnemy, mask);
            }
            else if (realyQuality == EffectQuality.QUALITY_MEDIUM)
            {
                PlayModlues(GoodObj, GoodModlues, isEnemy, mask);
                PlayModlues(FastestObj, FastestModlues, isEnemy, mask);
            }
            else if (realyQuality == EffectQuality.QUALITY_GOOD)
            {
                PlayModlues(HighObj, HighModlues, isEnemy, mask);
                PlayModlues(GoodObj, GoodModlues, isEnemy, mask);
                PlayModlues(FastestObj, FastestModlues, isEnemy, mask);
            }
            else if (realyQuality == EffectQuality.QUALITY_BEST)
            {
                PlayModlues(BestObj, BestModules, isEnemy, mask);
                PlayModlues(HighObj, HighModlues, isEnemy, mask);
                PlayModlues(GoodObj, GoodModlues, isEnemy, mask);
                PlayModlues(FastestObj, FastestModlues, isEnemy, mask);
            }


        }

        public override void StopEffect()
        {
            EntityID = -1;
            bCallStopFunc = true;

            //如果还没有调用初始化就调用stop函数

            if (!bInit)
            {
                Init();
            }

            if (!bPlay && bFirstCallStop)
            {
                return;
            }
            EndReplay();
            if (bPlay)
            {
                AllEffectCount--;
            }
            bPlay = false;
            bFirstCallStop = true;
            if (currSrcObj)
            {
                CacheTarget(currSrcObj);
                currSrcObj = null;
            }
            if (currDstObj)
            {
                CacheTarget(currDstObj);
                currDstObj = null;
            }
            StopEffectImpl();
        }

        public void ForceDeleteTestModel()
        {
            if (null != FastestModlues)
            {
                FastestModlues.ForceDeleteTestModel();
            }

            if (null != GoodModlues)
            {
                GoodModlues.ForceDeleteTestModel();
            }

            if (null != HighModlues)
            {
                HighModlues.ForceDeleteTestModel();
            }

            if (null != BestModules)
            {
                BestModules.ForceDeleteTestModel();
            }
        }

        private void StopEffectImpl()
        {
            if (!bInit)
            {
                return;
            }
            GabageRender();
            StopModlues(BestObj, BestModules);
            StopModlues(HighObj, HighModlues);
            StopModlues(GoodObj, GoodModlues);
            StopModlues(FastestObj, FastestModlues);
        }

        public void OnDestroy()
        {
            Initialize.OnDeviceStateChage -= OnDeviceStateChanged;

            if (bPlay)
            {
                AllEffectCount--;
            }
            if (null != BestModules)
            {
                BestModules.Clear();
                BestModules = null;
            }
            if (null != GoodModlues)
            {
                GoodModlues.Clear();
                GoodModlues = null;
            }
            if (null != HighModlues)
            {
                HighModlues.Clear();
                HighModlues = null;
            }
            if (null != FastestModlues)
            {
                FastestModlues.Clear();
                FastestModlues = null;
            }
            m_DispatchedRender.Clear();

            if (isStaticEffect)
            {
                UnResigterEffectQualityChange(OnStaticEffectQualityChange);
            }
        }

        void PlayModlues(GameObject ModluesObj,EffectModules moudles, bool bEnemy, int mask)
        {
            if (ModluesObj)
            {
                if (!ModluesObj.activeSelf)
                {
                    Debug.LogWarning("物体没有激活:" + ModluesObj.name);
                    return;
                }

                if (null != moudles && moudles.isBaked)
                {
                    DispatchRender(moudles);
                    moudles.Play(bEnemy, mask);
                }
                else
                {
                    Debug.LogWarning(name + "没有bake，请重新点击bake");
                    return;
                }
            }
        }
        
        void StopModlues(GameObject ModluesObj,EffectModules moudles)
        {
            if (ModluesObj)
            {
                if (null != moudles)
                {
                    moudles.Stop();
                }
            }
        }

        public void ReplayEffect()
        {
            StopEffect();
            PlayEffect(isEnemy,-1, SrcObj, DstObj);
        }
    }
}



