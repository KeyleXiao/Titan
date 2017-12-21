/*******************************************************************
** 文件名:	MonsterStateMachine_Skin.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	怪物状态机
** 应  用:  	怪物状态机中的皮肤相关部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameLogic;
using System;
using U3D_Render;


public sealed  partial class MonsterStateMachine : BaseStateMachine
{

    public MonsterSkinConfig SkinConfig
    {
        get { return (MonsterSkinConfig)entitySkinConfig; }
    }


    /// <summary>
    /// 实体显示，超过这个距离，不显示,小于0不参与优化
    /// </summary>

    public const int EntityVisibleDistance = 40;

    #region 皮肤切换

    /// <summary>
    /// 当皮肤已经被改变-消息通知
    /// </summary>
    /// <param name="nSkinID">皮肤ID</param>
    /// <param name="nPart">哪个部位</param>
    protected override void OnSkinChanged(SkinInstance sk, int nPart)
    {
        if(null == sk || !sk.isValid)
        {
            return;
        }
        m_DistanceVisible = true;
        EntityView ev = EntityFactory.getMonsterViewByID(this.entityID);
        ChangeStateMachineParams();
        ChangeCreaturePropety();
        ChangeMask(ev);
        gameObject.name = SkinManager.GetSkinName(sk.nSkinID) + "(entityID:" + entityID + ")";
        base.OnSkinChanged(sk, nPart);

        //关于ward结界类型的特殊处理
        //结界类型的物理盒子放在LayerWard层，用于阻挡指定类型的玩家
        //但是结界有可以选中和不可以选中，
        //这里只将配置的ScreenCastObject改为其他层，那么不可选中的结界不配置ScreenCastObject，就不影响结界后面实体的技能选中，
        //除了ScreenCastObject之外，实体的其他部分仍然为LayerWard，
        //那么hit到了之后LayerWard在判断实体有效性的时候，也将LayerWard加入有效层的范围内，这样就结界的选中就是合法的了。
        if (creature.gameObject.layer == LayerMask.NameToLayer(Config.LayerWard_Enemy))
        {
            if (creaturePropety.ScreenCastObject)
            {
                GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerMonsterEnemy), creaturePropety.ScreenCastObject, false);
            }
        }
        else if (creature.gameObject.layer == LayerMask.NameToLayer(Config.LayerWard_Friend))
        {
            if (creaturePropety.ScreenCastObject)
            {
                GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerMonsterFriend), creaturePropety.ScreenCastObject, false);
            }
        }

        goRootTransform = FindTransformEx("Main");

        currentGravity = creaturePropety.downGravity;
       
        SkinInstance si = SkinControl.CurrentSkinInstance;
        if (si.playEntityStaticEffect)
        {
            si.playEntityStaticEffect.ResetLayer();
            si.playEntityStaticEffect.Stop();
            si.playEntityStaticEffect.Play(ev);
        }
        if (si.switchEffectWithHeroDistance)
        {
            si.switchEffectWithHeroDistance.ResetLayer();
            si.switchEffectWithHeroDistance.SetEntitiyView(ev);
        }
#if !ENABLE_YUNYING_MODE
        if (SkinConfig.OcclusCollider)
        {
            if (!startCheckOcclusCoroutine)
            {
                startCheckOcclusCoroutine = true;
                cor_CheckOcclus = StartCoroutine(CheckOcclus());
            }
        }
#endif


    }

    private void ChangeStateMachineParams()
    {
        if (null == SkinConfig)
        {
            return;
        }
        //castLayer.CastlayerObject = SkinConfig.ScreenCastObject;
        if (SkinConfig.ColliderShape)
        {
            SkinConfig.ColliderShape.enabled = false;
        }
        //castLayer.Start();
    }

    private void ChangeCreaturePropety()
    {
        CreatureProperty srccp = SkinControl.CurrentSkinInstance.creatureProperty;
        if (!srccp)
        {
            return;
        }

        checkPosition = srccp.checkPosition;

        creaturePropety.Copy(srccp);
    }

    public override void SetVisible(bool bVisible)
    {
        if (SkinControl)
        {
            SkinControl.SetVisible(bVisible);
        }

        if (bVisible)
        {
            //可见的时候重设为默认角度
            creature.eulerAngles = defaultAngle;
        }
        base.SetVisible(bVisible);
    }

    protected override void UpdateVisible()
    {
        m_bVisible = m_OnViewVisible && m_MaskVisible && m_DistanceVisible;
        SetVisible(m_bVisible);
    }
    #endregion

    #region 皮肤淡入淡出相关

    /// <summary>
    /// 是否在淡入
    /// </summary>
    private bool bFadeInSkin = false;

    /// <summary>
    /// 是否在淡出皮肤
    /// </summary>
    private bool bFadeOutSkin = false;

    /// <summary>
    /// 皮肤渐变计时器
    /// </summary>
    private float FadeTimer = 0.0f;

    private float FadeDuation = 0.0f;
    /// <summary>
    /// 淡入皮肤
    /// </summary>
    public void FadeInSkin(float duationTime = 0.2f)
    {
        if (!bFadeInSkin)
        {
            bFadeInSkin = true;
            bFadeOutSkin = false;
            FadeTimer = 0.0f;
            FadeDuation = duationTime * (1 - SkinControl.SkinAlpha);
        }
        
    }

    /// <summary>
    /// 更新皮肤渐变
    /// </summary>
    public void UpdateFadeSkin()
    {
        if (!bFadeOutSkin && !bFadeInSkin)
        {
            return;
        }

        if (bFadeInSkin)
        {
            FadeTimer += Time.deltaTime;
            //SkinControl.setAlpha(Mathf.Lerp(0.0f, occlusCamera ? occlusCameraAlpha : 1.0f, FadeTimer / FadeDuation));
            if (FadeTimer > FadeDuation)
            {
                bFadeInSkin = false;
            }
        }
        else
        {
            FadeTimer += Time.deltaTime;
            //SkinControl.setAlpha(Mathf.Lerp(occlusCamera ? occlusCameraAlpha : 1.0f, 0.0f, FadeTimer / FadeDuation));

            //淡出时间到了，更新可见性
            if (FadeTimer > FadeDuation)
            {
                bFadeOutSkin = false;
                UpdateVisible();
            }
        }
    }

    /// <summary>
    /// 淡出皮肤
    /// </summary>
    public void FadeOutSkin(float duationTime = 0.2f)
    {
        if (!bFadeOutSkin)
        {
            bFadeOutSkin = true;
            bFadeInSkin = false;
            FadeTimer = 0.0f;
            FadeDuation = duationTime * SkinControl.SkinAlpha;
        }
        
    }
    #endregion

    #region 遮挡检测

    /// <summary>
    /// 是否被遮挡
    /// </summary>
    private bool occlusCamera = false;
    private bool startCheckOcclusCoroutine = false;
    private float occlusCameraAlpha = 0.5f;
    private WaitForSeconds checkOccInterval = new WaitForSeconds(0.1f);

    private Ray CheckRay = new Ray();
    private bool lastocclusCamera = false;
    private float PlayerCameraDistance = 0.0f;

    IEnumerator CheckOcclus()
    {
        while(true)
        {
            yield return checkOccInterval;

            Vector3 pos = Vector3.zero;
            if (null != Initialize.mainCam)
                pos = Initialize.mainCam.transform.position;
            occlusCamera = false;
            if (SkinControl && SkinConfig && EntityFactory.MainHeroView != null && EntityFactory.MainHero)
            {
                CheckRay.origin = pos;
                Vector3 mainPlayPos = EntityFactory.MainHero.transform.position;
                mainPlayPos.y += EntityFactory.MainHeroView.Property.CreatureHeightInMeters;
                Vector3 dir = mainPlayPos - pos;
                PlayerCameraDistance = Vector3.Distance(mainPlayPos, pos);

                CheckRay.direction = dir.normalized;
                RaycastHit hitInfo;
                if (SkinConfig.OcclusCollider.Raycast(CheckRay, out hitInfo, PlayerCameraDistance))
                {
                    occlusCamera = true;
                    if (lastocclusCamera != occlusCamera)
                    {
                        lastocclusCamera = occlusCamera;
                        SkinControl.setAlpha(occlusCameraAlpha);
                    }
                }
                else
                {
                    if (lastocclusCamera != occlusCamera)
                    {
                        lastocclusCamera = occlusCamera;
                        SkinControl.setAlpha(1.0f);
                    }
                }
            }
        }
    }
    #endregion
}
