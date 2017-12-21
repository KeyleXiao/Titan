using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using SPELL;
using Data.PersonModel;
using USpeedUI;
// 生物属性基类
// 所有属性必须在Copy函数中拷贝一遍，才不会出BUG！！！
[System.Serializable]
public class CreatureProperty : MonoBehaviour
{
    //public string SkillOrderSetName = "orderset";
    public float runSpeed_Forward = 4.6f;
    public float runSpeed_Back = 4.6f;
    public float runSpeed_LeftRight = 3.07f;
    public float walkSpeed_ForawdBack = 1.2f;
    public float walkSpeed_LeftRight = 1.2f;
    public bool enableDodge = false;
    public float upGravity = 30.0f; //跳跃加速度
    public float downGravity = 50.0f; //死亡跌落加速度
    public float jumpBaseHeight = 5.0f; //跳跃高度
    public float jumpDodgeHeight = 5.0f;
    public float wallClimbSpeed = 10.0f; //飞檐走壁上升/下降速度
    public float wallWalkSpeed = 8.0f; //飞檐走壁行走速度
    public float floatingMinHeight=0.5f; //最低浮空高度
    //public float floatUpSpeed = 2.0f; //浮空自动上升速度
    public float aimDistance = 50.0f; //普通瞄准距离
    public float lockAimDistance = 10.0f; //锁定攻击对象的有效距离
    public float attackedAimDistance = 15.0f; //此距离内被其他英雄攻击后选中对方
    public float attackMoveRange = 3.0f; //此距离内边走边打动作变成原地动作
    public float maxAttackMoveSpeedScale = -1.0f; //攻击工作因为加速而设置状态机的speed时，位移的速度加倍有限，在此设置最大倍数，超过按比例滑步，小于0不起效
    public int commonAttackSkillID = 0; //普通攻击技能ID，用于处理连续普攻导致其他技能无法触发
    public float animatorCrossSpeed = 2.0f; //上下身分离动作的层级与骨骼动作过渡速度，即1/speed秒，默认为1/2秒


    //默认瞄准使用特殊设置
    public bool FPSMode = false; //FPS操作模式
    public int selectType = 0; //目标选择标识，0表示不开启
    public int targetFilter = 0; //目标过滤标识
    public int selectDrawType = 0; //绘制地面选择范围标识，0表示不开启
    public float selectDrawAttackRange = 0.0f; //地面选择范围
    public float selectDrawChooseRange = 0.0f; //地面选择宽度
    public int selectDrawTargetType = 0; //地面范围标识目标过滤

    public int cameraType = 0; //摄像机设置中的HeroType标识

    //生物受击效果参数
    public float shineScale = 0.92f; //scale倍数
    public float shineSeconds = 0.1f; //scale变化持续秒数
    public Color highLightColor = Color.white; //高亮颜色
    public float delightTime = 0.1f; //亮0.1秒后灭
    public float maxHighLightAlpha = 0.25f; //高亮alpha

    //起跳，招录，跑步，急停加光效
    public int effectIDForChangeSkin = 0;
    public int effectIDForJump = 0;
    public int effectIDForLand = 0;
    public int effectIDForRush = 0;
    public int effectIDForScream = 0;

    /// <summary>
    /// 头顶信息隐藏距离，最小值0
    /// </summary>
    public float TopInfoHideInDistance = 20.0f;

    /// <summary>
    /// 血条的Y偏移
    /// </summary>
    public float BloodYOffset = 15;

    /// <summary>
    /// 头顶名称的Y偏移
    /// </summary>
    public float TopNameYOffset = 30;

    /// <summary>
    /// 准心偏移，像素为单位
    /// </summary>
    public Vector2 CrossHairOffset=Vector2.zero;

    /// <summary>
    /// 准星是否显示
    /// </summary>
    public bool CrossHairVisible = false;
    public bool MouseAim = false;

    //同步位置校验球
    public bool checkPosition = false;

    //生物高度，米为单位
    public float CreatureHeightInMeters = 1.8f;

    public GameObject ScreenCastObject = null;

    //生物包围半径，米为单位，三个方向，X为平面投影半径，Y为Y轴半径，Z用于选中框Y方向上的偏移
    public Vector3 CreatureBoundingRad = new Vector3(1.0f, 1.0f, 1.0f);

    //惯性加速度
    public float InertiaAcceleration = 20.0f;
    //惯性加速度倍数
    public float InertiaScale = 1.0f;
    //惯性加速度倍数
    private ENTITY_PROPERTY m_PropertyMaxLimit = ENTITY_PROPERTY.PROPERTY_CLIENT_MAX;
    //////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// 实体名称
    /// </summary>
    public string CreatureName
    {
        get { return m_strCreatureName; }
        set { m_strCreatureName = value; }
    }
    private string m_strCreatureName = "unamed";

    /// <summary>
    /// 模型路径
    /// </summary>
    public string ModelPath
    {
        get { return m_strModelPath; }
        set { m_strModelPath = value; }
    }
    private string m_strModelPath = "";

    // 属性数值对应逻辑层数值，控制不在属性面板上显示
    private int[] m_numprops = null;

    /// <summary>
    /// 所属主体对象
    /// </summary>
    public U3D_Render.EntityView Master
    {
        get { return m_objMaster; }
        set { m_objMaster = value; }
    }

    public ENTITY_PROPERTY PropertyMaxLimit
    {
        get { return m_PropertyMaxLimit; }
        set { m_PropertyMaxLimit = value; }
    }

    private U3D_Render.EntityView m_objMaster = null;

    //////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// 重置对象
    /// </summary>
    public virtual void Restore()
    {
        m_objMaster = null;
        m_strCreatureName = "unamed";
        m_strModelPath = "";
        m_numprops = null;

        runSpeed_Forward = 4.6f;
        runSpeed_Back = 4.6f;
        runSpeed_LeftRight = 3.07f;
        walkSpeed_ForawdBack = 1.2f;
        walkSpeed_LeftRight = 1.2f;
        enableDodge = false;
        upGravity = 30.0f;
        downGravity = 50.0f;
        jumpBaseHeight = 5.0f;
        jumpDodgeHeight = 5.0f;
        floatingMinHeight=0.5f;
        wallClimbSpeed = 10.0f; //飞檐走壁上升/下降速度
        wallWalkSpeed = 8.0f; //飞檐走壁行走速度

        FPSMode = false;
        aimDistance = 50.0f;
        lockAimDistance = 10.0f;
        attackedAimDistance = 15.0f;
        attackMoveRange = 3.0f;
        maxAttackMoveSpeedScale = -1.0f;
        commonAttackSkillID = 0;

        shineScale = 0.92f;
        shineSeconds = 0.1f;
        highLightColor = Color.white;
        delightTime = 0.1f;
        maxHighLightAlpha = 0.25f;

        effectIDForChangeSkin = 0;
        effectIDForJump = 0;
        effectIDForLand = 0;
        effectIDForRush = 0;
        effectIDForScream = 0;

        TopInfoHideInDistance = 20.0f;
        CrossHairOffset=Vector2.zero;
        CrossHairVisible = false;
        MouseAim = false;
        checkPosition = false;
       // BloodYOffset = 15;
        //TopNameYOffset = 30;
        //CreatureHeightInMeters = 1.8f;
        CreatureBoundingRad = new Vector3(1.0f, 1.0f, 1.0f);
        InertiaAcceleration = 20.0f;
        InertiaScale = 1.0f;
    }

    public bool Create()
    {
        m_numprops = new int[(int)m_PropertyMaxLimit];
        return true;
    }

    // 所有属性必须在Copy函数中拷贝一遍，才不会出BUG！！！
    public void Copy(CreatureProperty src)
    {
        runSpeed_Forward = src.runSpeed_Forward;
        runSpeed_Back = src.runSpeed_Back;
        runSpeed_LeftRight = src.runSpeed_LeftRight;
        walkSpeed_ForawdBack = src.walkSpeed_ForawdBack;
        walkSpeed_LeftRight = src.walkSpeed_LeftRight;
        enableDodge = src.enableDodge;
        upGravity = src.upGravity;
        downGravity = src.downGravity;
        jumpBaseHeight = src.jumpBaseHeight;
        jumpDodgeHeight = src.jumpDodgeHeight;        
        floatingMinHeight=src.floatingMinHeight; //最低浮空高度
        wallClimbSpeed=src.wallClimbSpeed; //飞檐走壁上升/下降速度
        wallWalkSpeed = src.wallWalkSpeed; //飞檐走壁行走速度

        FPSMode = src.FPSMode;
        cameraType = src.cameraType;
        aimDistance = src.aimDistance; //普通瞄准距离
        lockAimDistance = src.lockAimDistance; //锁定攻击对象的有效距离
        attackedAimDistance = src.attackedAimDistance; //此距离内被其他英雄攻击后选中对方
        attackMoveRange = src.attackMoveRange; //此距离内边走边打动作变成原地动作
        maxAttackMoveSpeedScale = src.maxAttackMoveSpeedScale; //攻击工作因为加速而设置状态机的speed时，位移的速度加倍有限，在此设置最大倍数，超过按比例滑步，小于0不起效
        commonAttackSkillID = src.commonAttackSkillID; //普通攻击技能ID，用于处理连续普攻导致其他技能无法触发
        animatorCrossSpeed = src.animatorCrossSpeed;

        selectType = src.selectType; //目标选择标识，0表示不开启
        targetFilter = src.targetFilter; //目标过滤标识
        selectDrawType = src.selectDrawType; //绘制地面选择范围标识，0表示不开启
        selectDrawAttackRange = src.selectDrawAttackRange; //地面选择范围
        selectDrawChooseRange = src.selectDrawChooseRange; //地面选择宽度
        selectDrawTargetType = src.selectDrawTargetType; //地面范围标识目标过滤


        shineScale = src.shineScale; //scale倍数
        shineSeconds = src.shineSeconds; //scale变化持续秒数
        highLightColor = src.highLightColor; //高亮颜色
        delightTime = src.delightTime; //亮0.1秒后灭
        maxHighLightAlpha = src.maxHighLightAlpha; //高亮alpha

        //起跳，招录，跑步，急停加光效
        effectIDForChangeSkin = src.effectIDForChangeSkin;
        effectIDForJump = src.effectIDForJump;
        effectIDForLand = src.effectIDForLand;
        effectIDForRush = src.effectIDForRush;
        effectIDForScream = src.effectIDForScream;

        TopInfoHideInDistance = src.TopInfoHideInDistance;

        BloodYOffset = src.BloodYOffset;
        TopNameYOffset = src.TopNameYOffset;
        CrossHairOffset=src.CrossHairOffset;

        CrossHairVisible = src.CrossHairVisible;
        MouseAim = src.MouseAim;

        checkPosition = src.checkPosition;
        CreatureHeightInMeters = src.CreatureHeightInMeters;
        ScreenCastObject = src.ScreenCastObject;
        CreatureBoundingRad = src.CreatureBoundingRad;

        InertiaAcceleration = src.InertiaAcceleration;
        InertiaScale = src.InertiaScale;
    }

    // 缩放
    public void SetScale(Vector3 scal)
    {
        //这些不用缩

        //runSpeed_Forawd *= scal;
        //runSpeed_Back *= scal;
       // runSpeed_LeftRight *= scal;
       // walkSpeed_ForawdBack *= scal;
        //walkSpeed_LeftRight *= scal;
        //gravity *= scal;
        //jumpBaseHeight *= scal;
        //TopInfoHideInDistance *= scal;
        //CrossHairOffset *= scal;
        //aimDistance *= scal;

        //float temp = BloodYOffset;
        //temp *= scal;
        //BloodYOffset = (int)temp;

        //temp = TopNameYOffset;
        //temp *= scal;
        //TopNameYOffset = (int)temp;

        CreatureHeightInMeters *= scal.y;
    }

    /// <summary>
    /// 获取指定属性数值
    /// </summary>
    /// <param name="prop">属性</param>
    /// <returns></returns>
    public int GetNumProp(ENTITY_PROPERTY prop)
    {
        if (prop >= m_PropertyMaxLimit || m_objMaster == null)
        {
            Debug.LogWarning("prop >= m_PropertyMaxLimit || m_objMaster == null");
            return 0;
        }

        return m_numprops[(int)prop];
    }

    /// <summary>
    /// 设置指定属性数值
    /// </summary>
    /// <param name="prop">属性</param>
    /// <param name="nValue">属性数值</param>
    /// <returns></returns>
    public bool SetNumProp(ENTITY_PROPERTY prop, int nValue,int nOtherValue = 0)
    {
        if (prop >= m_PropertyMaxLimit || m_objMaster == null)
        {
            return false;
        }
        int propID = (int)prop;
        // 取得更新前数值
        int nOldValue = m_numprops[propID];
        m_numprops[propID] = nValue;
        OnAfterUpdateNumProp(propID, nValue, nOldValue, nOtherValue);
        return true;
    }

    /// <summary>
    /// 数值属性变化处理
    /// </summary>
    /// <param name="data"></param>
    public bool UpdateNumProp(entity_update_Numproperty data)
    {
        if (data.nPropID >= (int)m_PropertyMaxLimit || m_objMaster == null)
        {
            return false;
        }
        // 取得更新前数值
        int nOldValue = m_numprops[data.nPropID];
        // 更新数值
        m_numprops[data.nPropID] = data.nValue;    

        if (data.isRaiseIprEvent == 1)
        {
            EntityNumPropEventArgs e = new EntityNumPropEventArgs();
            e.data.nEntityID = data.uID;	        // 显示层的实体ID
            e.data.nPBUID = data.nPBUID;
            e.data.nPropID = data.nPropID;			// 属性ID
            e.data.nValue = data.nValue;			// 属性数值
            e.data.nBeforeValue = nOldValue;        // 更新前数据
            e.data.nOtherData = data.nOtherData;	// 状态数据
            e.objev = m_objMaster;                  // 实体对象 
   
            EntityFactory.RaiseIprEvent( e );
        }

        OnAfterUpdateNumProp(data.nPropID, data.nValue, nOldValue, data.nOtherData);
        return true;
    }

    /// <summary>
    /// 属性更新后处理
    /// </summary>
    /// <param name="nPropID"></param>
    /// <param name="nNewValue"></param>
    /// <param name="nOldValue"></param>
    private void OnAfterUpdateNumProp(int nPropID, int nNewValue, int nOldValue, int nOtherValue)
    {
        switch (nPropID)
        {
        case (int)ENTITY_PROPERTY.PROPERTY_SKIN:
            {
                if (nNewValue != nOldValue)
                {
                    if (Master.SkinControl.ChangeSkin(nNewValue, nOtherValue, Master.IsHero? AssetLoadPriority.Priority_Exclusive : AssetLoadPriority.Priority_Normal) == U3D_Render.SkinCtrl.SkinChangedReslut.Succes)
					//if (Master.SkinControl.ChangeSkin(nNewValue, nOtherValue) == U3D_Render.SkinCtrl.SkinChangedReslut.Succes)
                    {
#if ENABLE_YUNYING_MODE
                        if (nOldValue > 0)
                        {
                            EffectHelper.UnloadPreLight(ASpeedGame.Data.Scheme.LightType.ActorSkin, nOldValue);
                        }

                        EffectHelper.LoadPreLight(ASpeedGame.Data.Scheme.LightType.ActorSkin, nNewValue, CachePriority.PRIORITY_VERY_HIGH);
#else
                        //主城才需要这个
                        if (GameLogicAPI.isMainMap(SceneManager.Instance.m_curMapID))
                        {
                            // 主角切英雄或皮肤时加载下光效
                            if (Master.IsHero)
                            {
                                if (nOldValue > 0)
                                {
                                    EffectHelper.UnloadPreLight(ASpeedGame.Data.Scheme.LightType.ActorSkin, nOldValue);
                                }

                                EffectHelper.LoadPreLight(ASpeedGame.Data.Scheme.LightType.ActorSkin, nNewValue, CachePriority.PRIORITY_VERY_HIGH);
                            }
                        }
#endif
                    }
                }
            }
            break;

        case (int)ENTITY_PROPERTY.PROPERTY_SD:
            {
                BaseStateMachine fsm = Master.StateMachine;
                if (fsm)
                {
                    fsm.maxRunForwardSpeed = nNewValue / 100.0f;
                    fsm.maxRunSidewaySpeed = nNewValue / 100.0f * GameLogicAPI.getHeroSidewaySpeedFactor();
                    fsm.maxRunBackwardSpeed = fsm.maxRunForwardSpeed * GameLogicAPI.getHeroBackwardSpeedFactor();
                    fsm.updateMovingAnimSpeed();
                }
            }
            break;

        case (int)ENTITY_PROPERTY.PROPERTY_MASK:
            {
                BaseStateMachine fsm = Master.StateMachine;
                if (fsm)
                {
                    fsm.ChangeMask(Master);
                }
            }
            break;
        case (int)ENTITY_PROPERTY.PROPERTY_VOCATION:
            {
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property == this)
                {
                    foreach (SSchemePersonModelData data in PersonModelConfig.Instance.PersonModelConfigs)
                    {
                        if (data.nHeroID == nNewValue)
                        {
                            OperationType fpsMode = new OperationType();
                            fpsMode.operationType = data.nOperationType;
                            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_UPDATE_MAIN_HERO_VOCATION, fpsMode);
                            break;
                        }
                    }
                }
            }
            break;
            case (int)ENTITY_PROPERTY.PROPERTY_OBSERVER:
                {
                    if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property == this)
                    {
                        if(nNewValue > 0)
                            StageManager.Instance.CurPermissionState = EStage_PermissionState.ESPS_OB;
                        else
                            StageManager.Instance.CurPermissionState = EStage_PermissionState.ESPS_NORMAL;
                    }
                }
                break;

        default:
            break;
        }
    }

    public void SetLightEffect()
    {
        Master.LightingEffectManager.BindLighting(LightingEffectManager.LevlUpEffectID);
        //FloatFontManager.CreateFloatFont(Master.gameObject, "111111", FloatFontType.TYPE_LEVELUP,true);
        if (Master == EntityFactory.MainHeroView)
        {
            USpeedUI.UWarFloatFontData fontData = new USpeedUI.UWarFloatFontData((int)USpeedUI.WndMsgID.WND_MSG_WAR_FLOATFONT_CREATE, Master.gameObject, "111111", USpeedUI.WarFloatFont.WarFloatFontType.LevelUp);
            USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_WAR_FLOATFONT_CREATE, fontData);
        }
    }
}
