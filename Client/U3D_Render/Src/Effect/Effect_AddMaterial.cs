/*******************************************************************
** 文件名:	Effect_AddMaterial.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/08/04
** 版  本:	1.0
** 描  述:	特效功能脚本
** 应  用:  	增加材质球

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


    /// <summary>
    /// 增加材质球，只支持一个材质和MeshRener、SkinnedMeshRender
    /// </summary>
    public class Effect_AddMaterial : MonoBehaviour, ISaveSerializedData
    {

        /// <summary>
        /// 持续时间,小于0.03f，表示一直存在
        /// </summary>
        public float Duartion = 5.0f;

        /// <summary>
        /// 延迟多少秒，小于0.03，无视
        /// </summary>
        public float DelayTime = 0.0f;
        public Material TargetMaterial = null;
        //public EffectRunMode RunMode = EffectRunMode.Self;
        public MaterialOpRunType TargetType = MaterialOpRunType.Self;

        /// <summary>
        /// 影响右手武器，仅仅在EntityID下
        /// </summary>
        public bool AffectRightWeapon = true;

        /// <summary>
        /// 影响左手武器，仅仅在EntityID下
        /// </summary>
        public bool AffectLeftWeapon = true;

        /// <summary>
        /// 影响翅膀
        /// </summary>
        public bool AffectWings = false;

        /// <summary>
        /// 影响背部，仅仅在EntityID下
        /// </summary>
        public bool AffectBackPart = false;

        /// <summary>
        /// 影响身体，仅仅在EntityID下
        /// </summary>
        public bool AffectBody = true;
        
        public Effect_FadeInOutMaterialColor FadeInOutMaterialColor = null;
        public Effect_FadeInOutMaterialFloat FadeInOutMaterialFloat = null;
        public Effect_UVTextureAnimator UVTextureAnimator = null;
        public Effect_UVScroll UVScroll = null;
        public Effect_MaterialGradientColor GradientColor = null;

        protected int HostEntityID = -1;

        public GameObject TargetModel = null;

        /// <summary>
        /// 添加的材质球实例
        /// </summary>
        private HashSet<Material> matInstances = new HashSet<Material>();

        /// <summary>
        /// 添加的材质球和对应的Render表
        /// </summary>
        private Dictionary<Renderer, Material> matInstancesMap = new Dictionary<Renderer, Material>();
        private List<Renderer> m_Renders = new List<Renderer>();
        private GameObject TestModel = null;

        private bool bPlay = false;
        private bool bInit = false;

        public System.Object SaveSerializedData()
        {
            if (TargetType == MaterialOpRunType.EntityID)
            {
                return null;
            }

            m_Renders.Clear();

            if(TargetType == MaterialOpRunType.Self)
            {
                m_Renders.AddRange(GetComponents<MeshRenderer>());
                m_Renders.AddRange(GetComponents<SkinnedMeshRenderer>());
            }
            else if(TargetType == MaterialOpRunType.PointedModel)
            {
                if (TargetModel)
                {
                    m_Renders.AddRange(TargetModel.GetComponentsInChildren<MeshRenderer>());
                    m_Renders.AddRange(TargetModel.GetComponentsInChildren<SkinnedMeshRenderer>());
                }
            }

            List<Material[]> list = new List<Material[]>();
            for (int i = 0; i < m_Renders.Count; i++)
            {
                list.Add(m_Renders[i].sharedMaterials);
            }
            m_Renders.Clear();
            return list;
        }

        public void SetSerializedData(System.Object data)
        {
            if (null == data)
            {
                return;
            }
            if (TargetType == MaterialOpRunType.EntityID)
            {
                return ;
            }

            List<Material[]> list = data as List<Material[]>;
            if (null == data)
            {
                Debug.LogError("Effect_AddMaterial-- ConverSerializedData faild");
                return;
            }
            m_Renders.Clear();

            if (TargetType == MaterialOpRunType.Self)
            {
                m_Renders.AddRange(GetComponents<MeshRenderer>());
                m_Renders.AddRange(GetComponents<SkinnedMeshRenderer>());
            }
            else if (TargetType == MaterialOpRunType.PointedModel)
            {
                if (TargetModel)
                {
                    m_Renders.AddRange(TargetModel.GetComponentsInChildren<MeshRenderer>());
                    m_Renders.AddRange(TargetModel.GetComponentsInChildren<SkinnedMeshRenderer>());
                }
            }

            if (list.Count != m_Renders.Count)
            {
                Debug.LogError("Effect_AddMaterial-- orginalMaterialMap not match");
                return;
            }

            for (int i = 0; i < m_Renders.Count; i++)
            {
                m_Renders[i].sharedMaterials = list[i];
            }
            m_Renders.Clear();
        }

        public void OnDestroy()
        {
            matInstances.Clear();
            matInstances = null;
            matInstancesMap.Clear();
            matInstancesMap = null;
            m_Renders = null;
            HostEntityID = -1;
        }

        public Renderer[] GetRendersOnBakeTime()
        {
            if (!Application.isPlaying && Application.isEditor)
            {
                m_Renders.Clear();
                if (TargetType == MaterialOpRunType.PointedModel)
                {
                    if (TargetModel)
                    {
                        m_Renders.AddRange(TargetModel.GetComponentsInChildren<MeshRenderer>());
                        m_Renders.AddRange(TargetModel.GetComponentsInChildren<SkinnedMeshRenderer>());
                    }
                }
                else if (TargetType == MaterialOpRunType.Self)
                {
                    m_Renders.AddRange(GetComponents<MeshRenderer>());
                    m_Renders.AddRange(GetComponents<SkinnedMeshRenderer>());
                }
                return m_Renders.ToArray();
            }
            else
            {
                Debug.LogWarning("GetRendersOnBakeTime只能在编辑器下的非播放模式下调用!");
                return new Renderer[0];
            }

        }

        public void Init()
        {
            if (!TargetMaterial)
            {
                Debug.LogWarning(StringHelper.BuildString("Effect_AddMaterial --TargetMaterial为空:", name));
                return;
            }
            m_Renders.Clear();
            //如果是自身
            if (TargetType == MaterialOpRunType.Self)
            {
                m_Renders.AddRange(GetComponents<MeshRenderer>());
                m_Renders.AddRange(GetComponents<SkinnedMeshRenderer>());

            }
            //是别人的话，要取所有的Render
            else if (TargetType == MaterialOpRunType.PointedModel)
            {
                if (TargetModel)
                {
                    m_Renders.AddRange(TargetModel.GetComponentsInChildren<MeshRenderer>());
                    m_Renders.AddRange(TargetModel.GetComponentsInChildren<SkinnedMeshRenderer>());
                }

            }
            else if (TargetType == MaterialOpRunType.EntityID)
            {
                bInit = true;
                CheckMaterialControl();
                return;
            }

            if ((
                (null != m_Renders && m_Renders.Count > 0) ) && TargetMaterial)
            {
                bInit = true;
            }
            CheckMaterialControl();
        }


        void UpdateRendersFromEntityID(int entityID)
        {
            if (TargetType != MaterialOpRunType.EntityID || !TargetMaterial || !bInit)
            {
                return;
            }

            
            if (EffectNode.UseTestModel)
            {
                m_Renders.Clear();
                if (!TestModel)
                {
                    TestModel = GameObject.CreatePrimitive(PrimitiveType.Sphere);
                    TestModel.name += "-Add-" + gameObject.name;
                    EffectUtily.SaveTestModel(TestModel);
                }

                m_Renders.Add(TestModel.GetComponent<Renderer>());
                return;
            }

            if (entityID == HostEntityID)
            {
                return;
            }

            Finish();
            m_Renders.Clear();
            HostEntityID = entityID;
            if (HostEntityID > 0)
            {
                U3D_Render.EntityView view = EntityFactory.getEntityViewByID(HostEntityID);
                if (null != view)
                {
                    if (!view.SkinControl)
                    {
                        return;
                    }

                    if (null == view.SkinControl.CurrentSkinInstance)
                    {
                        return;

                    }
                    EntitySkinConfig sc = view.SkinControl.CurrentSkinInstance.entitySkinConfig;
                    if (AffectBody)
                    {
                        if (sc && sc.BodyRender)
                        {
                            m_Renders.Add(sc.BodyRender);
                        }
                    }

                    if (AffectLeftWeapon)
                    {
                        if (sc && sc.LeftWeaponRender)
                        {
                            m_Renders.Add(sc.LeftWeaponRender);
                        }
                    }

                    if (AffectRightWeapon)
                    {
                        if (sc && sc.RightWeaponRender)
                        {
                            m_Renders.Add(sc.RightWeaponRender);
                        }
                    }

                    if (AffectWings)
                    {
                        if (sc && sc.chibangguadian)
                        {
                            m_Renders.AddRange(sc.chibangguadian.GetComponentsInChildren<SkinnedMeshRenderer>());
                        }
                    }

                    if (AffectBackPart)
                    {
                        if (sc && sc.BackBodyRender)
                        {
                            m_Renders.Add(sc.BackBodyRender);
                        }
                    }
                }
            }
        }

        public void Play(int EntityID = -1)
        {
            if (!bInit)
            {
                return;
            }

            if (TargetType == MaterialOpRunType.EntityID)
            {
                //增加材质球只在高配以上起效，人物的skinned太耗了.
                if(ImageSetting.GetData().currEffectQuality < EffectQuality.QUALITY_GOOD)
                {
                    return;
                }
            }

            UpdateRendersFromEntityID(EntityID);

            if (!bPlay)
            {
                bPlay = true;
                if (DelayTime > 0.03f)
                {
                    GameUtil.StartCoroutineHelper(this,WaitForPlay());
                }
                else
                {
                    PlayerImpl();
                }
            }
        }

        public void Stop()
        {
            if (!bInit)
            {
                return;
            }

            StopAllCoroutines();
            Finish();
            bPlay = false;
        }

        public void ForceDeleteTestModel()
        {
            if (EffectNode.UseTestModel)
            {
                matInstances.Clear();
                m_Renders.Clear();
                if (TestModel)
                {
                    ResNode.DestroyRes(ref TestModel);
                }
                return;
            }

        }

        void Finish()
        {
            if (!bInit)
            {
                return;
            }

            if (EffectNode.UseTestModel)
            {
                return;
            }
            if (matInstances.Count <= 0)
            {
                return;
            }

            List<Material> TempList = new List<Material>();
            //结束的时候，清楚新增的材质球
            foreach (Renderer r in m_Renders)
            {
                if (!r)
                {
                    continue;
                }
                if (matInstancesMap.ContainsKey(r))
                {
                    TempList.Clear();
                    Material[] materials = r.sharedMaterials;
                    TempList.AddRange(r.sharedMaterials);
                    TempList.Remove(matInstancesMap[r]);
                    r.sharedMaterials = TempList.ToArray();

                    EffectUtily.GabageRender(r, matInstancesMap[r]);
                    var temp1 = matInstancesMap[r];
                    ResNode.DestroyRes(ref temp1);
                }

            }

            matInstances.Clear();
            matInstancesMap.Clear();

            //清空动画
            if (FadeInOutMaterialColor)
            {
                //如果运行模式不是外部指定的
                if (FadeInOutMaterialColor.RunMode == EffectRunMode.OutSiedePointed)
                {
                    FadeInOutMaterialColor.SetMaterialInstance(matInstances);
                    FadeInOutMaterialColor.Stop();
                }

            }

            if (FadeInOutMaterialFloat)
            {
                //如果运行模式不是外部指定的
                if (FadeInOutMaterialFloat.RunMode == EffectRunMode.OutSiedePointed)
                {
                    FadeInOutMaterialFloat.SetMaterialInstance(matInstances);
                    FadeInOutMaterialFloat.Stop();
                }

            }

            if (UVTextureAnimator)
            {
                //如果运行模式不是外部指定的
                if (UVTextureAnimator.RunMode == EffectRunMode.OutSiedePointed)
                {
                    UVTextureAnimator.SetMaterialInstance(matInstances);
                    UVTextureAnimator.Stop();
                }
            }

            if (UVScroll)
            {
                //如果运行模式不是外部指定的
                if (UVScroll.RunMode == EffectRunMode.OutSiedePointed)
                {
                    UVScroll.SetMaterialInstance(matInstances);
                    UVScroll.Stop();
                }
            }

            if (GradientColor)
            {
                //如果运行模式不是外部指定的
                if (GradientColor.RunMode == EffectRunMode.OutSiedePointed)
                {
                    GradientColor.SetMaterialInstance(matInstances);
                    GradientColor.Stop();
                }
            }
        }

        /// <summary>
        /// 计算材质球实例，如果有多个render，会返回多个材质球实例
        /// </summary>
        /// <returns></returns>
        private void GenrateMaterialInstance()
        {
            if (!bInit)
            {
                return ;
            }
            if (matInstances.Count <= 0)
            {
                Material instanceMat;

                //在Render上增加一个材质球
                foreach (Renderer r in m_Renders)
                {
                    if (!r)
                    {
                        continue;
                    }
                    if (!matInstancesMap.ContainsKey(r))
                    {
                        Material[] materials = r.sharedMaterials;
                        r.material = TargetMaterial;
                        instanceMat = r.material;
                        int length = materials.Length + 1;
                        Material[] newMaterials = new Material[length];

                        materials.CopyTo(newMaterials, 0);
                        newMaterials[length - 1] = instanceMat;
                        r.sharedMaterials = newMaterials;

                        matInstancesMap.Add(r,instanceMat);
                        matInstances.Add(instanceMat);
   
                        EffectUtily.DispatchRender(r, instanceMat);
                    }


                  
                }
            }
        }

        private void PlayerImpl()
        {
            GenrateMaterialInstance();

            //把材质设置进动画里
            if (FadeInOutMaterialColor)
            {
                //如果运行模式不是外部指定的
                if (FadeInOutMaterialColor.RunMode == EffectRunMode.OutSiedePointed)
                {
                    FadeInOutMaterialColor.SetMaterialInstance(GetMaterialInstance());
                    FadeInOutMaterialColor.Play();
                }

            }

            if (FadeInOutMaterialFloat)
            {
                //如果运行模式不是外部指定的
                if (FadeInOutMaterialFloat.RunMode == EffectRunMode.OutSiedePointed)
                {
                    FadeInOutMaterialFloat.SetMaterialInstance(GetMaterialInstance());
                    FadeInOutMaterialFloat.Play();
                }

            }

            if (UVTextureAnimator)
            {
                //如果运行模式不是外部指定的
                if (UVTextureAnimator.RunMode == EffectRunMode.OutSiedePointed)
                {
                    UVTextureAnimator.SetMaterialInstance(GetMaterialInstance());
                    UVTextureAnimator.Play();
                }
            }

            if (UVScroll)
            {
                //如果运行模式不是外部指定的
                if (UVScroll.RunMode == EffectRunMode.OutSiedePointed)
                {
                    UVScroll.SetMaterialInstance(GetMaterialInstance());
                    UVScroll.Play();
                }
            }

            if (GradientColor)
            {
                //如果运行模式不是外部指定的
                if (GradientColor.RunMode == EffectRunMode.OutSiedePointed)
                {
                    GradientColor.SetMaterialInstance(GetMaterialInstance());
                    GradientColor.Play();
                }
            }
            if (Duartion > 0.03f)
            {
                GameUtil.StartCoroutineHelper(this,WaitForFinish());
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
                    Debug.LogError("UVScroll成员的运行模式不是OutSiedePointed!-" + gameObject.name );
                }
            }

            if (GradientColor)
            {
                //如果运行模式不是外部指定的
                if (GradientColor.RunMode != EffectRunMode.OutSiedePointed)
                {
                    Debug.LogError("UVScroll成员的运行模式不是OutSiedePointed!-" + gameObject.name);
                }
            }
        }

        public HashSet<Material> GetMaterialInstance()
        {
            return matInstances;
        }

        IEnumerator WaitForPlay()
        {
            yield return new WaitForSeconds(DelayTime);
            PlayerImpl();
        }

        IEnumerator WaitForFinish()
        {
            yield return new WaitForSeconds(Duartion);
            Stop();
        }

    }
}

