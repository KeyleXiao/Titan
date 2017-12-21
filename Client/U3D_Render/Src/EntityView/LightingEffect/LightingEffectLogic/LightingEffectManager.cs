using System;
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

// 光效管理器
// 每个人物挂一个脚本
// 负责管理此人身上的光效

// 等到某状态时再挂光效的效果暂时无效了，因为animator放在了子节点上

#if USE_TIMMER_BEHAVIOUR
public partial class LightingEffectManager : UpdateBehaviourOnTimer
#else
public partial class LightingEffectManager : MonoBehaviour
#endif
{
    public static int HitEffectCount = 0;

    private List<LightingEffectBehavior> mEffectList; //记录当前挂在人物上的光效
    public bool showLog = false;
    public bool isGlobal = false;

    /// <summary>
    /// 升级光效ID，写死，99999
    /// </summary>
    public const int LevlUpEffectID = 99999;
    public BaseStateMachine stateMachine = null;
    public void Init()
    {
        mEffectList = new List<LightingEffectBehavior>();
        //m_materialReplaceEffectList = new List<LightingEffectBehavior_MaterialReplace>();
        // m_showingMaterialReplaceEffect = new Dictionary<int, LightingEffectBehavior_MaterialReplace>();
        //mDelayEffectList= new List<LightingEffect>();
        //animator = GetComponent<Animator>();
#if USE_TIMMER_BEHAVIOUR
        if(!isAddToTimer)
        {
            startTimmer(1, UpdateBehaviourOnTimer.TIMER_THREAD_MID);
            isAddToTimer = true;
        }
#endif
    }

    /// <summary>
    /// 重置对象
    /// </summary>
    public virtual void Restore()
    {
        showLog = false;
        isGlobal = false;
        //if (m_materialReplaceEffectList != null)
        //{
        //    m_materialReplaceEffectList.Clear();
        //}

        //if (m_showingMaterialReplaceEffect != null)
        //{
        //    m_showingMaterialReplaceEffect.Clear();
        //}

        if (mEffectList != null)
        {
            mEffectList.Clear();
        }

        //if (m_materialReplaceEffectList != null)
        //{
        //    m_materialReplaceEffectList.Clear();
        //}

        //if (m_showingMaterialReplaceEffect != null)
        //{
        //    m_showingMaterialReplaceEffect.Clear();
        //}

        stateMachine = null;
    }

    ////-----------------------------------------------------------------------------------------------------
    ////以下部分主要处理光效跟动作配合的时候，需要检查状态机切换再开始光效的功能
    ////-----------------------------------------------------------------------------------------------------
    //private List<LightingEffect> mDelayEffectList; //延迟挂接的光效，等到状态切换后再开始挂接
    //private int lastestStateNameHash = 0;
    //private void dealWithStateEffect()
    //{
    //    if (animator == null)
    //    {
    //        animator = transform.GetComponent<Animator>();
    //    }

    //    if (animator == null)
    //    {
    //        return;
    //    }
    //    if (animator.layerCount < 2)
    //    {
    //        return;
    //    }
    //    AnimatorStateInfo sta = animator.GetCurrentAnimatorStateInfo(1);
    //    int currentStateNameHash = sta.nameHash;
    //    if (currentStateNameHash == lastestStateNameHash)
    //    {
    //        return;
    //    }

    //    //切换动作了！这里nameHash只做切换时间点的判断，用nameHash和effect.waitState对比老是失败
    //    lastestStateNameHash=currentStateNameHash;
    //    foreach (LightingEffect effect in mDelayEffectList)
    //    {
    //        if (sta.IsName(effect.getWaitState()))
    //        {
    //            if (effect.Begin())
    //            {
    //                mEffectList.Add(effect);
    //            }
    //        }
    //    }

    //    //目前暂定切一次动作就清空一次延迟光效的记录，也就是说光效只能延迟一个动作挂接
    //    mDelayEffectList.Clear();
    //}





    // 添加一个光效
    // @param nID : 光效ID
    // @param nSrcEntityID : 光效源对象ID
    // @param targetObject : 目标对象，可以为空
    // @param targetPos : 目标位置，可以为空
    // @param isHero : 是否是游戏玩家控制的主角
    // @param nFeedbackID : 逻辑层传的ID标识，用于标记、删除和碰撞是返回标识。

    public LightingEffectBehavior AddLightingImpl(int nID, LightEffectParam param)
    {
        //远距离不创建分情况考虑，具体在LightingEffect实现
        //if (LightingEffectFactory.Instance.IsEnabledOptimize())
        //{
        //    //距离超过创建距离，不创建光效了。
        //    Vector3 dis = Initialize.mainCam.transform.position - param.srcObject.transform.position;
        //    if (dis.sqrMagnitude > LightingEffectFactory.CreateDistance * LightingEffectFactory.CreateDistance)
        //    {
        //        return null;
        //    }
        //}

        //处理单一实例的光效
        if (LightingEffectFactory.Instance.IsSingleEffect(nID))
        {
            foreach (LightingEffectBehavior effect in mEffectList)
            {
                if (effect.getID() == nID && effect.EffectConfig.bSingle)
                {
                    if (effect.EffectConfig.bRecoverSingleEffect)
                    {
                        effect.recoverEffectLift(param);
                        return effect;
                    }
                    else if (effect.EffectParam.nTargetEntityID == param.nTargetEntityID)
                    {
                        effect.extendEffectLift();
                        return effect;
                    }
                }
            }
        }

        if (LightingEffectFactory.Instance.CheckGolbalEffect(nID) && isGlobal == false)
        {
            LightingEffectManager leManager = EntityFactory.GloabaleManager;
            if (leManager != null && leManager.isGlobal == true)
            {
                leManager.AddLighting(nID, param);
            }
            return null;
        }

        LightingEffectBehavior newEffect = LightingEffectFactory.Instance.CreateEffect(nID, param);
        if (newEffect == null)
        {
            return null;
        }

        newEffect.showLog = showLog;
        //string needStateName = newEffect.getWaitState();

        //if (needStateName.Length != 0)
        //{
        //    AnimatorStateInfo sta = animator.GetCurrentAnimatorStateInfo(1);
        //    if (!sta.IsName(needStateName))
        //    {
        //        //等下一个切换状态时再检测
        //        mDelayEffectList.Add(newEffect);
        //        return newEffect;
        //    }
        //}

        //开始失败了，返回null
        if (newEffect.Begin())
        {
            mEffectList.Add(newEffect);

            return newEffect;
        }
        return null;

    }

    // 移除光效
    // @param nID : 光效ID
    // @param nFeedbackID : 创建时填充的nFeedbackID，只删除此ID相同的光效，如果是0则无视
    public bool RemoveLighting(int nID, int nFeedbackID = 0)
    {
        if (showLog)
        {
            Trace.Log("RemoveLighting" + nID);
        }
        foreach (LightingEffectBehavior effect in mEffectList)
        {
            if (effect.getID() == nID && (nFeedbackID == 0 || effect.EffectParam.nFeedbackID == nFeedbackID))
            {
                effect.needClose = true;
            }
        }
        //Update();
        return true;
    }

    public bool RemoveLightingImmediate(int nID, int nFeedbackID = 0)
    {
        //遍历光效列表，对光效进行更新或关闭操作
        for (int i = 0; i < mEffectList.Count;)
        {
            LightingEffectBehavior effect = mEffectList[i];
            bool bRemove = effect.getID() == nID;
            bRemove = bRemove && (nFeedbackID == 0 || effect.EffectParam.nFeedbackID == nFeedbackID);
            bRemove = bRemove || !effect.Valid;

            if (bRemove)
            {
                effect.Close();
                mEffectList.RemoveAt(i);
                LightingEffectFactory.Instance.CacheEffectBehavior(ref effect);
            }
            else
            {
                i++;
            }
        }
        return true;
    }

    // 开关光效
    // @param enable : 开或者关
    public bool lightingEffectEnable = true;
    public bool setEnable(bool enable)
    {
        lightingEffectEnable = enable;
        foreach (LightingEffectBehavior effect in mEffectList)
        {
            effect.setEnable(enable);
        }
        return true;
    }




    private void clearEffectImpl()
    {
        if (null == mEffectList)
        {
            return;
        }

        foreach (LightingEffectBehavior effect in mEffectList)
        {
            effect.Close();
            //创建失败,要进行回收，要不然缓存无法更新
            LightingEffectBehavior temp = effect;
            LightingEffectFactory.Instance.CacheEffectBehavior(ref temp);
        }
        mEffectList.Clear();
    }

    // 死亡时移除光效
    public bool CloseLightingWhenDead()
    {
        if (null == mEffectList)
        {
            return true;
        }
        foreach (LightingEffectBehavior effect in mEffectList)
        {
            if (!effect.Valid)
            {
                continue;
            }
            if (effect.EffectConfig.CloseWhenDead)
            {
                effect.needClose = true;
            }
        }
        //Update();
        return true;
    }


    public void Clear()
    {
        clearEffect();
    }

    public void OnDisable()
    {
        clearEffect();
    }

#if USE_TIMMER_BEHAVIOUR   
    public override void OnUpdate()
#else
    public void Update() //Update阶段更新，在人物动作计算之前，不支持失败删除光效
#endif
    {
        if (mEffectList == null)
        {
            return;
        }
        //dealWithStateEffect();

        //遍历光效列表，对光效进行更新
        for (int i = 0; i < mEffectList.Count; i++)
        {
            LightingEffectBehavior effect = mEffectList[i];

            if (!effect.Valid)
            {
                continue;
            }
            if (effect.needClose != true)
            {
                effect.Update();
            }
        }
    }

#if USE_TIMMER_BEHAVIOUR
    public override void OnLateUpdate()
#else
    public void LateUpdate() //光效更新改在了LateUpdate阶段，在人物动作计算之后，以便获得绑点准确的transoform信息
#endif
    {
        if (mEffectList == null)
        {
            return;
        }
        //dealWithStateEffect();

        //遍历光效列表，对光效进行更新或关闭操作
        for (int i = 0; i < mEffectList.Count;)
        {
            LightingEffectBehavior effect = mEffectList[i];
            int effectid = effect.getID();


            bool bRemove = false;
            if (!effect.Valid)
            {
                bRemove = true;
            }
            else
            {
                if (effect.needClose == true) //光效需要关闭并移除
                {
                    effect.Close();
                    if (effect.needClose)
                    {
                        //如果有类似原路返回的效果，则不能立刻删除光效，在Close()函数中会把needClose改回false，此时就不移除光效
                        bRemove = true;
                    }
                }
                else if (effect.LateUpdate() == false) //否则，对光效进行更新，更新失败的关闭并移除
                {
                    effect.needClose = true;
                    effect.Close();
                    if (effect.needClose)
                    {
                        //如果有类似原路返回的效果，则不能立刻删除光效，在Close()函数中会把needClose改回false，此时就不移除光效
                        bRemove = true;
                    }
                }


                effect.ApplyPlayDataToResHandler();
            }

            if (bRemove)
            {
                mEffectList.RemoveAt(i);
                DoCompleteCallBack(effectid);

                LightingEffectFactory.Instance.CacheEffectBehavior(ref effect);
            }
            else
            {
                DoStartCallBack(effectid);
                i++;
            }
        }
    }
    IEnumerator InvokeStartEffect(Action EffectStartAction)
    {
        yield return null;
        EffectStartAction();
    }

    //动作触发，添加绑定光效
    public void BindLighting(int LightingID)
    {
        if (showLog)
        {
            Trace.Log("Animation Add Bind Light" + LightingID);
        }
        if (!lightingEffectEnable) //挂动作上的光效，如果物体已经被隐藏，那直接不挂了
        {
            return;
        }
        LightEffectParam param = new LightEffectParam();
        param.srcObject = gameObject;
        param.sourcePos = gameObject.transform.position;
        param.targetPos = gameObject.transform.position;
        if (stateMachine == null)
        {
            stateMachine = GetComponent<BaseStateMachine>();
        }
        if (stateMachine)
        {
            param.nSrcEntityID = stateMachine.entityID;
        }

        AddLighting(LightingID, param);
    }


    //动作触发，添加绑定空光效
    public void BindLightingEmpty(int LightingID)
    {
        if (showLog)
        {
            Trace.Log("Animation Add Bind Light anb" + LightingID);
        }
    }

    //添加默认光效，默认光效的预制体已经在LightingEffectFactory里用代码写好生成

    //public void AddDefaultInstance(string instanceName, Vector3 pos, bool projectToFloor, float destroyTime)
    //{
    //    if (projectToFloor)
    //    {
    //        int LayerMaskOnPos = (1 << LayerMask.NameToLayer(Config.LayerDefault) | 1 << LayerMask.NameToLayer(Config.LayerBuilding));
    //        Ray PointToGroundRay = new Ray(pos + new Vector3(0, 0.5f, 0), new Vector3(0, -1, 0));
    //        RaycastHit projectInfo;
    //        Physics.Raycast(PointToGroundRay, out projectInfo, 100, LayerMaskOnPos);
    //        pos = projectInfo.point;
    //    }

    //    GameObject entity = LightingEffectFactory.Instance.createDefaultInstance(instanceName, pos);       
    //    if (entity != null)
    //    {
    //        entity.transform.SetPosition( pos;
    //        StartCoroutine(DelayDestroy(entity, destroyTime));
    //    }

    //}

    #region 替换材质效果管理
    //private List<LightingEffectBehavior_MaterialReplace> m_materialReplaceEffectList; //记录正在生效的效
    //private Dictionary<int, LightingEffectBehavior_MaterialReplace> m_showingMaterialReplaceEffect; //记录第int号材质正在被某个材质替换
    //public void applyMaterialReplace(LightingEffectBehavior_MaterialReplace effect)
    //{
    //    m_materialReplaceEffectList.Add(effect);
    //    updateMaterialReplace(effect.matRepConfig.replaceMaterialIndex);
    //}

    //public void removeMaterialReplace(LightingEffectBehavior_MaterialReplace effect)
    //{
    //    if (!m_materialReplaceEffectList.Contains(effect))
    //    {
    //        return;
    //    }
    //    m_materialReplaceEffectList.Remove(effect);
    //    if ((m_showingMaterialReplaceEffect.ContainsKey(effect.matRepConfig.replaceMaterialIndex)) && (m_showingMaterialReplaceEffect[effect.matRepConfig.replaceMaterialIndex] == effect))
    //    {
    //        updateMaterialReplace(effect.matRepConfig.replaceMaterialIndex);
    //    }
    //    effect.Close();
    //}

    //private void updateMaterialReplace(int materialIndex)
    //{

    //    if (stateMachine == null)
    //    {
    //        stateMachine = GetComponent<BaseStateMachine>();
    //    }


    //    LightingEffectBehavior_MaterialReplace bestEffect = null;
    //    foreach (LightingEffectBehavior_MaterialReplace effecti in m_materialReplaceEffectList)
    //    {
    //        if (effecti.matRepConfig.replaceMaterialIndex == materialIndex && (bestEffect == null || bestEffect.matRepConfig.replacePriority < effecti.matRepConfig.replacePriority))
    //        {
    //            bestEffect = effecti;
    //        }
    //    }
    //    if (!m_showingMaterialReplaceEffect.ContainsKey(materialIndex) || bestEffect != m_showingMaterialReplaceEffect[materialIndex])
    //    {
    //        if (bestEffect == null)
    //        {                
    //            if (stateMachine)
    //            {
    //                stateMachine.recoverMaterial(materialIndex); //恢复成原本材质
    //                m_showingMaterialReplaceEffect.Remove(materialIndex);
    //            }
    //        }
    //        else
    //        {
    //            if (stateMachine)
    //            {
    //                stateMachine.changeMaterial(materialIndex, bestEffect.matRepConfig.effectPrefb);
    //                m_showingMaterialReplaceEffect[materialIndex] = bestEffect;
    //            }
    //        }            
    //    }
    //}    


    #endregion

    #region CallBack-对外提供事件

    //注册光效已开始播放事件
    public void RegisterEffectStart(int EffectID, Action StartCallBack)
    {
        if (!EffectStartCallBackDic.ContainsKey(EffectID))
        {
            EffectStartCallBackDic.Add(EffectID, EffectCallBackAc);
        }
        EffectStartCallBackDic[EffectID] += StartCallBack;
    }
    //注册光效已播放完成事件
    public void RegisterEffectComplete(int EffectID, Action CompleteCallBack)
    {
        if (!EffectCompleteCallBackDic.ContainsKey(EffectID))
        {
            EffectCompleteCallBackDic.Add(EffectID, EffectCallBackAc);
        }
        EffectCompleteCallBackDic[EffectID] += CompleteCallBack;
    }
    //CallBack事件原型
    Action EffectCallBackAc = null;
    //光效ID对应开始事件
    Dictionary<int, Action> EffectStartCallBackDic = new Dictionary<int, Action>();
    //光效ID对应完成事件
    Dictionary<int, Action> EffectCompleteCallBackDic = new Dictionary<int, Action>();

    //光效开始,遍历注册事件
    void DoStartCallBack(int effectid)
    {
        if (EffectStartCallBackDic.ContainsKey(effectid))
        {
            if (EffectStartCallBackDic[effectid] != null)
            {
                //EffectStartCallBackDic[effectid]();
                StartCoroutine(InvokeStartEffect(EffectStartCallBackDic[effectid]));
            }
            EffectStartCallBackDic.Remove(effectid);
        }
    }
    //光效完成,遍历注册事件
    void DoCompleteCallBack(int effectid)
    {
        if (EffectCompleteCallBackDic.ContainsKey(effectid))
        {
            if (EffectCompleteCallBackDic[effectid] != null)
            {
                EffectCompleteCallBackDic[effectid]();
            }
            EffectCompleteCallBackDic.Remove(effectid);
        }
 
    }
    #endregion

}
