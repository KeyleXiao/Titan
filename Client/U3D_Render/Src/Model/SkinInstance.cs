/*******************************************************************
** 文件名:	SkinInstance.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/3
** 版  本:	1.0
** 描  述:	皮肤实例，外部通过这个来使用这个皮肤
** 应  用:   用于管理皮肤的，简单的说，就是模型了
**************************** 修改记录 ******************************
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

/// <summary>
/// 这个是皮肤实例，外部通过这个来使用这个皮肤
/// </summary>
public partial class SkinInstance                        
{
    public ENTITY_TYPE SkinType
    {
        get
        {
            return m_SkinType;
        }
    }
    private ENTITY_TYPE m_SkinType = ENTITY_TYPE.TYPE_MONSTER;
    public bool isLoadFinish
    {
        get
        {
            return m_isLoadFinish;
        }
    }
    private bool m_isLoadFinish = false;
    /// <summary>
    /// 如果是异步加载，没有加载完成时，永远是false
    /// </summary>
    public bool isValid
    {
        get
        {
            if(!isLoadFinish)
            {
                return false;
            }
            return SkinObj && nSkinID > 0;
        }
    }

    public bool isBeCached = false;

    /// <summary>
    /// 皮肤ID
    /// </summary>
    public int nSkinID
    {
        get
        {
            return m_nSkinID;
        }
    }

    private int m_nSkinID = -1;

    /// <summary>
    /// 主皮肤
    /// </summary>
    public GameObject SkinObj
    {
        get
        {
            return m_SkinObj;
        }
    }

    private GameObject m_SkinObj = null;

    /// <summary>
    /// 皮肤数据节点
    /// </summary>
    public Transform SkinDataObj
    {
        get
        {
            return m_SkinDataObj;
        }
    }
    private Transform m_SkinDataObj  = null;

    /// <summary>
    /// 人物的上下半身分离A物体
    /// </summary>
    public GameObject SkinObj_A
    {
        get
        {
            return m_SkinObj_A;
        }
    }

    private GameObject m_SkinObj_A = null;

    /// <summary>
    /// 人物的上下半身分离B物体
    /// </summary>
    public GameObject SkinObj_B
    {
        get
        {
            return m_SkinObj_B;
        }
    }

    private GameObject m_SkinObj_B = null;

    /// <summary>
    /// 主Animator
    /// </summary>
    public Animator MainAnimator
    {
        get
        {
            return m_MainAnimator;
        }
    }

    private Animator m_MainAnimator = null;

    /// <summary>
    /// 人物上下半身分离AnimatorB
    /// </summary>
    public Animator AnimatorB
    {
        get
        {
            return m_AnimatorB;
        }
    }

    private Animator m_AnimatorB = null;

    /// <summary>
    /// 人物上下半身分离的BipedIKAnimator
    /// </summary>
    public BipedIKAnimator bipedIKAnimator
    {
        get
        {
            return m_bipedIKAnimator;
        }
    }

    private BipedIKAnimator m_bipedIKAnimator = null;

    /// <summary>
    /// 武器Animator1
    /// </summary>
    public Animator weaponAnimator
    {
        get
        {
            return m_weaponAnimator;
        }
    }
    private Animator m_weaponAnimator = null;

    /// <summary>
    /// 武器Animator1
    /// </summary>
    public Animator weaponAnimator2
    {
        get
        {
            return m_weaponAnimator2;
        }
    }

    private Animator m_weaponAnimator2 = null;
    /// <summary>
    /// creatureProperty
    /// </summary>
    public CreatureProperty creatureProperty
    {
        get
        {
            return m_creatureProperty;
        }
    }

    private CreatureProperty m_creatureProperty = null;
    /// <summary>
    /// 主皮肤上的skinnedMeshRender
    /// </summary>
    public List<SkinnedMeshRenderer> SkinnedMeshRenderList
    {
        get
        {
            return m_SkinnedMeshRenderList;
        }
    }
    private List<SkinnedMeshRenderer> m_SkinnedMeshRenderList = new List<SkinnedMeshRenderer>();

    /// <summary>
    /// 主皮肤上的MeshRender
    /// </summary>
    public List<MeshRenderer> MeshRenderList
    {
        get
        {
            return m_MeshRenderList;
        }
    }

    private List<MeshRenderer> m_MeshRenderList = new List<MeshRenderer>();
    /// <summary>
    /// 主皮肤上的所有Render，不包括SkinDataObj下的Render以及粒子系统的Render，Projector，LineRender
    /// </summary>
    public List<Renderer> AllRenders
    {
        get
        {
            return m_AllRenders;
        }
    }

    private List<Renderer> m_AllRenders = new List<Renderer>();
    /// <summary>
    /// 所有VisibleHelperList
    /// </summary>
    public List<RenderVisibleHelper> VisibleHelperList
    {
        get
        {
            return m_VisibleHelperList;
        }
    }

    private List<RenderVisibleHelper> m_VisibleHelperList = new List<RenderVisibleHelper>();

    private CharacterShadow characterShadow
    {
        get
        {
            return m_characterShadow;
        }
    }
    private CharacterShadow m_characterShadow = null;

    /// <summary>
    /// 配置文件中的皮肤缩放
    /// </summary>
    public float ConfigScale
    {
        get
        {
            return m_ConfigScale;
        }
    }

    private float m_ConfigScale = 1.0f;
    /// <summary>
    /// 预制体自己本身的缩放
    /// </summary>
    public Vector3 PrefabScale
    {
        get
        {
            return m_PrefabScale;
        }
    }


    private Vector3 m_PrefabScale = Vector3.one;

    /// <summary>
    /// cameraProperty
    /// </summary>
    public CameraProperty cameraProperty
    {
        get
        {
            return m_cameraProperty;
        }
    }

    private CameraProperty m_cameraProperty = null;
    /// <summary>
    /// cameraProperty
    /// </summary>
    public CharacterController characterController
    {
        get
        {
            return m_characterController;
        }
    }

    private CharacterController m_characterController = null;

    /// <summary>
    /// SkinConfig
    /// </summary>
    public EntitySkinConfig entitySkinConfig
    {
        get
        {
            return m_entitySkinConfig;
        }
    }

    private EntitySkinConfig m_entitySkinConfig = null;
    /// <summary>
    /// switchEffectWithHeroDistance
    /// </summary>
    public Effect.SwitchEffectWithHeroDistance switchEffectWithHeroDistance
    {
        get
        {
            return m_switchEffectWithHeroDistance;
        }
    }


    private Effect.SwitchEffectWithHeroDistance m_switchEffectWithHeroDistance = null;
    /// <summary>
    /// playEntityStaticEffect
    /// </summary>
    public Effect.Effect_PlayEntityStaticEffect playEntityStaticEffect
    {
        get
        {
            return m_playEntityStaticEffect;
        }
    }


    private Effect.Effect_PlayEntityStaticEffect m_playEntityStaticEffect = null;
    /// <summary>
    /// sidekickEntity
    /// </summary>
    public SidekickEntity sidekickEntity
    {
        get
        {
            return m_sidekickEntity;
        }
    }


    private SidekickEntity m_sidekickEntity = null;
    /// <summary>
    /// BindEffect_A
    /// </summary>
    public BindEffect bindEffect_A
    {
        get
        {
            return m_bindEffect_A;
        }
    }

    private BindEffect m_bindEffect_A = null;
    /// <summary>
    /// BindEffect_B
    /// </summary>
    public BindEffect bindEffect_B
    {
        get
        {
            return m_bindEffect_B;
        }
    }

    private BindEffect m_bindEffect_B = null;
    /// <summary>
    /// bindSound
    /// </summary>
    public BindSound bindSound
    {
        get
        {
            return m_bindSound;
        }
    }

    private BindSound m_bindSound = null;
    /// <summary>
    /// overrideController
    /// </summary>
    public AnimatorOverrideController overrideController
    {
        get
        {
            return m_overrideController;
        }
    }

    private AnimatorOverrideController m_overrideController = null;

    private RuntimeAnimatorController orginalAnimatorController = null;

    /// <summary>
    /// 设置被哪个skincontrol引用
    /// </summary>
    /// <param name="skinCtr"></param>
    public U3D_Render.SkinCtrl.EntitySkinControl ReferencedSkinCtrl
    {
        set
        {
            m_ReferencedSkinCtrl = value;
        }
        get
        {
            return m_ReferencedSkinCtrl;
        }
    }

    private U3D_Render.SkinCtrl.EntitySkinControl m_ReferencedSkinCtrl = null;

    public void Clear()
    {
        isBeCached = false;
        if (m_MainAnimator)
        {
            m_MainAnimator.runtimeAnimatorController = orginalAnimatorController;
        }

        m_nSkinID = -1;
        m_SkinObj = null;
        m_SkinObj_B = null;
        m_SkinObj_A = null;
        m_MainAnimator = null;
        m_AnimatorB = null;
        m_bipedIKAnimator = null;
        m_creatureProperty = null;
        m_ReferencedSkinCtrl = null;
        m_SkinnedMeshRenderList.Clear();
        m_MeshRenderList.Clear();
        m_AllRenders.Clear();
        m_cameraProperty = null;
        m_characterController = null;
        m_entitySkinConfig = null;

        if (m_playEntityStaticEffect)
        {
            m_playEntityStaticEffect.Clear();
        }

        if (m_switchEffectWithHeroDistance)
        {
            m_switchEffectWithHeroDistance.Clear();
        }

        m_playEntityStaticEffect = null;
        m_switchEffectWithHeroDistance = null;
        m_sidekickEntity = null;
        m_SkinDataObj = null;
        foreach (RenderVisibleHelper vis in m_VisibleHelperList)
        {
            vis.hostSkin = null;
            var temp1 = vis;
            ResNode.DestroyRes(ref temp1);
        }
        ResNode.DestroyRes(ref m_bindEffect_A);
        ResNode.DestroyRes(ref m_bindEffect_B);
        ResNode.DestroyRes(ref m_bindSound);
        ResNode.DestroyRes(ref m_overrideController);
        m_VisibleHelperList.Clear();

        ResNode.DestroyRes(ref m_characterShadow);
        ClearAlpha();
        ClearBonesMap();
        m_isLoadFinish = false;
    }

    public void Init()
    {
        InitAlpha(AssetBundleManager.EntityAlphaShader);
        BuildBonesMap();
        m_isLoadFinish = true;
    }

    public static bool isNullOrEmpry(SkinInstance instance)
    {
        if(null == instance)
        {
            return true;
        }

        return !instance.isValid;
    }
}
