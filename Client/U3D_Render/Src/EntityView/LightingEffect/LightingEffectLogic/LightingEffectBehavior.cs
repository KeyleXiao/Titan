using UnityEngine;
using System.Collections;
using UnityEngine.Events;


/// <summary>
/// 光效行为
/// </summary>
public class LightingEffectBehavior 
{
    //标记光效是否需要删除    
    public bool needClose = false;
    public bool showLog = false;

    public LightingEffect EffectConfig
    {
        get
        {
            return effectConfig;
        }
    }
    public LightEffectParam EffectParam
    {
        get
        {
            return effectParam;
        }
    }
    public LightingEffectResHandleInstance ResourceHandle
    {
        get
        {
            return ResHandleInstance;
        }
    }
    public Transform TransformAgent
    {
        get
        {
            return behaviorTransformAgent;
        }
    }
    /// <summary>
    /// 这个变量的用处是，有些情况下，是直接通知删除光效ID的，但是
    /// LightingEffectManager里没有删除，还保留这个引用。
    /// 这个时候就需要这个变量来判断这个是否被删除了。
    /// </summary>
    public bool Valid
    {
        get
        {
            return isInvalid;
        }
    }

    //public EffectBehaviorDestroyTimer BehaviorDestroyTimer
    //{
    //    get
    //    {
    //        return behaviorDestroyTimer;
    //    }
    //}

    protected Transform behaviorTransformAgent = null;
    protected LightingEffect effectConfig = null;
    protected LightEffectParam effectParam = null;
    protected LightingEffectResHandleInstance ResHandleInstance = null;
    //protected EffectBehaviorDestroyTimer behaviorDestroyTimer = null;
    protected EffectResHandlePlayData ResHandlePlayData = new EffectResHandlePlayData();

    protected int effectID = 0;

    protected bool isInvalid = false;

    public virtual void SetValid()
    {
        isInvalid = true;
    }

    public virtual void SetInvalid()
    {
        isInvalid = false;
    }

    public int getID()
    {
       return effectID;
    }

    public bool isGlobalEffect() //返回是否为全局光效
    {
        if (null != effectConfig)
        {
            return effectConfig.bGlobal;
        }
        return false;
    }

    public virtual void OnResourcesLoadFinish(LightingEffectResHandleInstance resHandle)
    {

    }

    public virtual void FillData(LightingEffect effectConfig, LightEffectParam param,LightingEffectResHandleInstance instance,Transform transformAgent)
    {
        this.effectConfig = effectConfig;
        effectID = effectConfig.nID;
        this.effectParam = param;
        this.ResHandleInstance = instance;
        this.behaviorTransformAgent = transformAgent;
        ResHandlePlayData.Clear();
    }

    public virtual void ClearData()
    {
        effectConfig = null;
     
        effectID = 0;
        ResHandleInstance = null;
    }


    public virtual bool Begin() //创建光效的代码，返回创建结果，创建失败则删除光效
    {
        return true;
    }

    public virtual void Update() //每帧更新，无返回结果，不支持失败删除光效
    {
    }

    public virtual bool LateUpdate() //每帧更新，返回更新结果，更新失败则删除光效
    {
        return true;
    }

    public virtual void ApplyPlayDataToResHandler()
    {
        if(LightingEffectResHandleInstance.isNullOrEmpry(ResourceHandle))
        {
            return;
        }

        ResHandleInstance.ApplyPlayData(ResHandlePlayData);
        
    }

    public virtual void Close()
    {
    }

    public virtual void setEnable(bool enable)
    {
    }

    public virtual void extendEffectLift() //和bSingle一起使用，延长光效生命时间
    {
    }

    public virtual void recoverEffectLift(LightEffectParam newParam) //和bSingle一起使用，为光效更新参数
    {
    }

    public virtual bool RePlay(LightEffectParam newParam)
    {
        return false;
    }

    public virtual void ResetAllMembers()
    {
        needClose = false;
        effectParam = null;
        behaviorTransformAgent = null;
        //behaviorDestroyTimer = null;
        ResHandlePlayData.Clear();


    }

    public static BaseStateMachine GetStateMachine(int entityID)
    {
        U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(entityID);
        if (U3D_Render.EntityView.isNullOrEmpty(ev))
        {
            return null;
        }

        BaseStateMachine machine = ev.StateMachine;
        return machine;
    }

    public static Transform FindTransformRoot(Transform root, string boneName)
    {
        if (string.IsNullOrEmpty(boneName))
        {
            return null;
        }
        if(!root)
        {
            return null;
        }
        BaseStateMachine machine = root.GetComponent<BaseStateMachine>();
        if (machine)
        {
            return machine.FindTransformEx(boneName);
        }
        return null;
    }

    /// <summary>
    /// 是否可以寻找骨骼,因为皮肤属于异步加载逻辑，有可能皮肤没有加载完成，光效已经出来了。对于绑定光效是找不到骨骼的
    /// </summary>
    /// <param name="entityID"></param>
    /// <returns></returns>
    public static bool IsCanFindTransform(int entityID)
    {
        U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(entityID);
        if (U3D_Render.EntityView.isNullOrEmpty(ev))
        {
            return true;
        }
        BaseStateMachine machine = GetStateMachine(entityID);
        if (!machine)
        {
            return true;
        }

        return !SkinInstance.isNullOrEmpry(machine.SkinControl.CurrentSkinInstance);
    }

    public static void RegsiterSkinChangedEvent(int entityID, UnityAction<SkinInstance, int> action)
    {
        U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(entityID);
        if (U3D_Render.EntityView.isNullOrEmpty(ev))
        {
            return;
        }
        BaseStateMachine machine = GetStateMachine(entityID);
        if (!machine)
        {
            return;
        }

        if(SkinInstance.isNullOrEmpry(machine.SkinControl.CurrentSkinInstance))
        {
            return;
        }
        machine.SkinControl.RegsiterSkinChangedEvent(action);
    }

    public static void UnRegsiterSkinChangedEvent(int entityID,UnityAction<SkinInstance,int> action)
    {
        U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(entityID);
        if (U3D_Render.EntityView.isNullOrEmpty(ev))
        {
            return;
        }
        BaseStateMachine machine = GetStateMachine(entityID);
        if (!machine)
        {
            return;
        }

        if (SkinInstance.isNullOrEmpry(machine.SkinControl.CurrentSkinInstance))
        {
            return;
        }
        machine.SkinControl.UnRegsiterSkinChangedEvent(action);
    }        

    public static Transform FindTransformEx(int entityID, string boneName)
    {
        if(string.IsNullOrEmpty(boneName))
        {
            return null;
        }
        BaseStateMachine machine = GetStateMachine(entityID);
        if(machine)
        {
            return machine.FindTransformEx(boneName);
        }
        return null;
    }

    public static Transform FindTransformWound(int entityID, string woundName)
    {
        if (string.IsNullOrEmpty(woundName))
        {
            return null;
        }

        U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(entityID);
        if (null!= ev && null != ev.SkinControl)
        {
            SkinInstance si = ev.SkinControl.CurrentSkinInstance;
            if (null != si && null != si.SkinDataObj)
            {
                Transform wound = si.SkinDataObj.FindChild(woundName);
                if (wound != null)
                {
                    return wound;
                }
            }
        }
        return null;
    }
}
