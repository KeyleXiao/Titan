/*******************************************************************
** 文件名:	SceneEffectManager.cs
** 修改人:  林建聪
** 日  期:  2017/01/10
** 描  述:  便于添加场景效果，要通过此类添加场景效果的话SceneEffectNodeData.csv表里需要有定义好的场景效果ID和资源路径
** 版  本:	1.0
** 应  用:  使用上建议场景效果ID都在配置表里配置，但鉴于有些情况需要程序里直接播放某种场景效果，所以增加了ESceneEffectID 类型，协助获取效果ID。
 * 使用方法：
            1、每次需要使用的时候都new 一个 SceneEffectParamBase 变量，并用于 SceneEffectManager.CreateSceneEffect
            2、一版每个场景效果都有配置持续时间，时间到了会自动释放。如果需要手动释放需自行保存创建时返回的SceneEffectParamBase 执行 SceneEffectManager.DestroySceneEffect。
            3、预制体路径可以通过选择预制体右键，拷贝预制体路径 功能获取。
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using DataCenter;
using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using Data.SceneEffectNodeData;
using Effect;

public enum ESceneEffectID
{
    SE_None,
    SE_HeroUpStar_Normal = 1,
    SE_HeroUpStar_Special,
    SE_PrizeChest_Click = 27,
    SE_PrizeChest_Cloud,
    SE_PrizeChest_DefaultBG = 32,
    /// <summary>   新手引导路径光效      </summary>
    SE_Guide_PathMesh = 130,
    SE_Common_AutoMoveTargetPoint = 132,
    SE_HeroDetailInfo_UpstarBase = 137,
    SE_SkinSelect_Begin = 1000,
    SE_SkinSelect_End = 5000,
    SE_CrownGemAppraisal_Main = 5001,
    SE_CrownGemAppraisal_Ass,
    SE_CrownGemAppraisal_Special,
    /// <summary>  本命英雄光效      </summary>
    SE_LifeHero_StandbyEffect = 5004,
    SE_LifeHero_VerticalDownBlink,
    SE_LifeHero_DNASpiralUp,
}

[Serializable]
public class SceneEffectParamBase
{
    //////////////////////////////////////////////////////////
    /// 初始化
    //////////////////////////////////////////////////////////
    /// <summary>   特效ID      </summary>
    public int AttachEffectID;
    /// <summary>   挂接对象    </summary>
    public Transform AttachTransform;
    /// <summary>   是否要自动播放    </summary>
    public bool IsAutoPlay;
    /// <summary>   延迟播放时间ms    </summary>
    public int DelaySpawnTime;
    /// <summary>   初始化局部坐标偏移    </summary>
    public Vector3 PositionOffset;

    //////////////////////////////////////////////////////////
    /// 返回值
    //////////////////////////////////////////////////////////
    /// <summary>   挂接返回的效果节点    </summary>
    public EffectNode ReturnComponent;

    //////////////////////////////////////////////////////////
    /// 逻辑数据
    //////////////////////////////////////////////////////////
    /// <summary>   记录起始播放时间ms    </summary>
    protected int m_nBeginCreateTime;
    /// <summary>   是否在播放中    </summary>
    protected bool m_bPlaying;


    /// <summary>
    /// 添加效果参数列表
    /// </summary>
    /// <param name="AttachParent">挂接对象</param>
    /// <param name="_strAttachChildName">挂接对象下子节点名字</param>
    /// <param name="_bAutoPlay">是否创建马上执行</param>
    public SceneEffectParamBase(int _eAttachEffectID, Transform _tfAttachParent,
        string _strAttachChildName = null, bool _bAutoPlay = true, int _nDelaySpawnTime = 0, Vector3 _vecPositionOffset = new Vector3()
        )
    {
        AttachEffectID = _eAttachEffectID;
        AttachTransform = _strAttachChildName == null ? _tfAttachParent : FindAttachChildTarget(_tfAttachParent, _strAttachChildName);
        IsAutoPlay = _bAutoPlay;
        DelaySpawnTime = _nDelaySpawnTime;
        PositionOffset = _vecPositionOffset;
    }

    /// <summary>
    /// 初始化方法
    /// </summary>
    public void Init()
    {
        m_nBeginCreateTime = (int)Time.time * 1000;
    }

    /// <summary>
    /// 此方法用户不允许执行，只允许创世神(我)来用
    /// 有需要用户自己控制播放时间的执行 PlayeSceneEffectByUser
    /// </summary>
    public void PlaySceneEffectByGod()
    {
        if (ReturnComponent == null || !IsAutoPlay || m_bPlaying)
            return;

        PlaySceneEffect();
    }

    public void PlaySceneEffectByUser()
    {
        if (ReturnComponent == null || IsAutoPlay || m_bPlaying)
            return;

        PlaySceneEffect();
    }

    private void PlaySceneEffect()
    {
        ReturnComponent.transform.SetParent(AttachTransform, false);

        ReturnComponent.StopEffect();
        ReturnComponent.PlayEffect();

        m_bPlaying = true;
    }

    public int GetSceneEffectDurationTime()
    {
        if (AttachEffectID == (int)ESceneEffectID.SE_None)
            return 0;

        return SceneEffectNodeData.Instance.GetData((int)AttachEffectID).nSceneEffectDuration;
    }

    public int GetSceneEffectDelaySpawnTime()
    {
        return DelaySpawnTime;
    }

    public Vector3 GetSceneEffectPositionOffset()
    {
        return PositionOffset;
    }

    public bool IsPlaying()
    {
        return m_bPlaying;
    }

    public bool IsValid()
    {
        return ReturnComponent != null;
    }

    public bool IsDurationTimeExit()
    {
        if (!IsPlaying())
            return true;

        int DurationTimeConfig = GetSceneEffectDurationTime();

        return DurationTimeConfig <= 0 ? false : DurationTimeConfig + m_nBeginCreateTime + GetSceneEffectDelaySpawnTime() <= (int)(Time.time * 1000);
    }

    public bool IsDelayTimeExit()
    {
        if (IsPlaying())
            return true;

        return GetSceneEffectDelaySpawnTime() + m_nBeginCreateTime <= (int)(Time.time * 1000);
    }

    protected Transform FindAttachChildTarget(Transform _tRoot, string _strChildName)
    {
        Transform resultTrans = null;

        resultTrans = (_tRoot.FindChild(_strChildName));

        resultTrans = (resultTrans == null ? _tRoot : resultTrans);

        return resultTrans;
    }
}

public class SceneEffectParamAsync : SceneEffectParamBase
{
    /// <summary>   资源加载回调，主要用于资源未加载，加载结束后第一次实例化执行用户操作    </summary>
    public Action<object> onResLoadFinished = null;
    /// <summary>   资源加载回调时用户需要的数据    </summary>
    public object ResLoadUserObject = null;
    /// <summary>   当前是否可实例化资源，主要用于当前加载资源时 前个资源 加载好，是否需要初始化实例对象    </summary>
    public bool IsCanBeInstance = false;

    public AssetLoadPriority priority = AssetLoadPriority.Priority_Normal;
    public SceneEffectParamAsync(int _eAttachEffectID, Transform _tfAttachParent, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal,
        string _strAttachChildName = null, bool _bAutoPlay = true, int _nDelaySpawnTime = 0, Vector3 _vecPositionOffset = new Vector3()
     , Action<object> _onResLoadFinished = null, object _resLoadUserObject = null) : base(_eAttachEffectID, _tfAttachParent, _strAttachChildName, _bAutoPlay, _nDelaySpawnTime, _vecPositionOffset)
    {
        onResLoadFinished = _onResLoadFinished;
        ResLoadUserObject = _resLoadUserObject;
        IsCanBeInstance = true;
        this.priority = priority;
        
    }
}

class SceneEffectManager : Singleton<SceneEffectManager>, IResNodeHandle, ITimerHanlder, IAsyncResLoadHandler
{
    protected static bool m_bInit = false;

    protected int m_nDurationTimer = 1000;

    /// <summary>
    /// 当前已加载的资源节点
    /// </summary>
    protected Dictionary<int, ResNode> m_dicEffectIDResNodeUsed = new Dictionary<int, ResNode>();

    /// <summary>
    /// 当前已执行CreateSceneEffect 的效果参数列表
    /// </summary>
    protected List<SceneEffectParamBase> m_listActiveEffectParam = new List<SceneEffectParamBase>();

    protected List<SceneEffectParamBase> m_listDelaySpawnEffectParam = new List<SceneEffectParamBase>();

    public static void Init()
    {
        if (m_bInit)
            return;
        m_bInit = true;
        TimerManager.SetTimer(Instance, Instance.m_nDurationTimer, 0.5f);
        StageManager.onStageStateChanged += Instance.OnStageStateChanged;
    }

    public static void Destroy()
    {
        m_bInit = false;
        AssetBundleManager.ReleaseResNodeHandle(Instance, true);
        Instance.m_dicEffectIDResNodeUsed.Clear();
        Instance.m_listActiveEffectParam.Clear();
        Instance.m_listDelaySpawnEffectParam.Clear();
        TimerManager.KillTimer(Instance, Instance.m_nDurationTimer);
        StageManager.onStageStateChanged -= Instance.OnStageStateChanged;
    }

    protected void OnStageStateChanged(Stage_ManagerDef.EStage_State _eOldState, Stage_ManagerDef.EStage_State _eNewState)
    {
        if ((_eNewState == Stage_ManagerDef.EStage_State.ESS_MATCHROOM && _eOldState == Stage_ManagerDef.EStage_State.ESS_GAME) ||
            (_eNewState == Stage_ManagerDef.EStage_State.ESS_GAME && _eOldState == Stage_ManagerDef.EStage_State.ESS_MATCHROOM))
            return;
        AssetBundleManager.ReleaseResNodeHandle(Instance, true);
        Instance.m_dicEffectIDResNodeUsed.Clear();
        Instance.m_listActiveEffectParam.Clear();
        Instance.m_listDelaySpawnEffectParam.Clear();

    }

    public bool CreateSceneEffect(ref SceneEffectParamBase _effectParam)
    {
        bool result = false;
        ResNode resNodeInfo = null;
        EffectNode effectNode = null;


        //先判断这个资源是否已经取了ResNode,没有的话需要取一次，仅此一次
        if (m_dicEffectIDResNodeUsed.ContainsKey(_effectParam.AttachEffectID))
        {
            resNodeInfo = m_dicEffectIDResNodeUsed[_effectParam.AttachEffectID];

            InitEffectParam(resNodeInfo, _effectParam);
            result = true;
        }
        else
        {
            SSchemeSceneEffectNodeData nodeData = SceneEffectNodeData.Instance.GetData((int)_effectParam.AttachEffectID);
            if (nodeData == null)
            {
                Trace.LogWarning("SceneEffectManager::配置表没有EffectID：" + _effectParam.AttachEffectID + "创建失败");
                return false;
            }

            if (_effectParam is SceneEffectParamAsync)
            {
                resNodeInfo = GetResNodeAsync(nodeData.strSceneEffectPath, _effectParam);
                if (resNodeInfo == null)
                {
                    Trace.LogWarning("SceneEffectManager::资源库没找到路径为" + nodeData.strSceneEffectPath + " 的资源");
                    return false;
                }

                m_dicEffectIDResNodeUsed.Add(_effectParam.AttachEffectID, resNodeInfo);
            }
            else
            {
                resNodeInfo = GetResNode(nodeData.strSceneEffectPath, _effectParam);
                if (resNodeInfo == null)
                {
                    Trace.LogWarning("SceneEffectManager::资源库没找到路径为" + nodeData.strSceneEffectPath + " 的资源");
                    return false;
                }
                
                m_dicEffectIDResNodeUsed.Add(_effectParam.AttachEffectID, resNodeInfo);

                InitEffectParam(resNodeInfo, _effectParam);
            }

           result = true;
        }


        // 根据参数初始化
        //if (resNodeInfo != null && resNodeInfo.IsValid())
        //{
        //    if (_effectParam.GetSceneEffectDelaySpawnTime() > 0)
        //    {
        //        DelaySceneEffect(_effectParam);
        //    }
        //    else
        //    {
        //        _CreateSceneEffect(_effectParam);
        //    }
        //    _effectParam.Init();
        //    result = true;
        //}
        return result;
    }

    protected void _CreateSceneEffect(SceneEffectParamBase _effectParam)
    {
        if (!m_dicEffectIDResNodeUsed.ContainsKey(_effectParam.AttachEffectID))
            return;

        EffectNode effectNode = null;

        effectNode = m_dicEffectIDResNodeUsed[_effectParam.AttachEffectID].InstanceMainRes<EffectNode>();//m_dicEffectIDResNodeUsed[_effectParam.AttachEffectID].InstanceRes<EffectNode>();
        
        m_listActiveEffectParam.Add(_effectParam);

        _effectParam.ReturnComponent = effectNode;
        _effectParam.ReturnComponent.transform.SetLocalPosition(_effectParam.ReturnComponent.transform.localPosition + _effectParam.GetSceneEffectPositionOffset());

        _effectParam.PlaySceneEffectByGod();
    }

    protected void DelaySceneEffect(SceneEffectParamBase _effectParam)
    {
        m_listDelaySpawnEffectParam.Add(_effectParam);
    }

    /// <summary>
    /// 删除场景光效
    /// </summary>
    /// <param name="_effectParam">场景光效参数</param>
    /// <param name="_bReleaseSource">是否释放光效资源</param>
    /// <returns></returns>
    public bool DestroySceneEffect(ref SceneEffectParamBase _effectParam, bool _bReleaseSource = true)
    {
        if(_effectParam == null)
        {
            return false;
        }

        int nEffectID = _effectParam.AttachEffectID;

        //不在activelist里面意味着走了异步，资源没加载完
        if (!m_listActiveEffectParam.Contains(_effectParam))
        {
            //删除时异步加载未完成
            if (m_dicEffectIDResNodeUsed.ContainsKey(nEffectID) && !m_dicEffectIDResNodeUsed[nEffectID].IsValid()) 
            {
                SceneEffectParamAsync paramAsync = _effectParam as SceneEffectParamAsync;
                if(paramAsync != null)
                {
                    paramAsync.IsCanBeInstance = true;
                }
            }
        }
        else
        {
            if (_effectParam.ReturnComponent == null)
            {
                m_listActiveEffectParam.Remove(_effectParam);
            }
            else
            {
                if (m_dicEffectIDResNodeUsed.ContainsKey(nEffectID))
                {
                    _effectParam.ReturnComponent.StopEffect();
                    var temp = _effectParam.ReturnComponent.gameObject;
                    m_dicEffectIDResNodeUsed[nEffectID].DestroyInstanceRes(ref temp);
                }
                    
                m_listActiveEffectParam.Remove(_effectParam);
            }
        }

        if (_effectParam is SceneEffectParamAsync)
            ReleaseResNodeAsync(nEffectID);
        else
        {
            if (_bReleaseSource)
                ReleaseResNode(nEffectID);
        }


        return true;
    }

    /// <summary>
    /// 检测ResNode在当前Handle里的实例引用数目是否达到可释放ResNode条件
    /// </summary>
    /// <param name="_nEffectID"></param>
    private void ReleaseResNode(int _nEffectID)
    {
        if (m_dicEffectIDResNodeUsed.ContainsKey(_nEffectID))
        {
                if (AssetBundleManager.ReleaseResNode(this, m_dicEffectIDResNodeUsed[_nEffectID], true))
                    m_dicEffectIDResNodeUsed.Remove(_nEffectID);
        }
    }

    private void ReleaseResNodeAsync(int _nEffectID)
    {
        if (m_dicEffectIDResNodeUsed.ContainsKey(_nEffectID))
        {
            if (AssetBundleManager.ReleaseResNodeAsync(this, m_dicEffectIDResNodeUsed[_nEffectID],this, true))
                m_dicEffectIDResNodeUsed.Remove(_nEffectID);
        }
    }

    private ResNode GetResNode(string _strPath, SceneEffectParamBase _param)
    {
        ResNodeHandleParamBase param = new ResNodeHandleParam_Path(AssetType.Asset_Prefab, _strPath);
        ResNode resultResNode = AssetBundleManager.GetResNode(this, param);

        if (resultResNode != null)
        {
            if (resultResNode.IsValid())
                return resultResNode;
        }
        return null;
    }

    private ResNode GetResNodeAsync(string _strPath, SceneEffectParamBase _param)
    {
        ResNodeHandleParamBase param = new ResNodeHandleParam_Path(AssetType.Asset_Prefab, _strPath);
        SceneEffectParamAsync _paramAsync = _param as SceneEffectParamAsync;
        ResNode resultResNode = AssetBundleManager.GetResNodeAsync(this, param, this, _param, _paramAsync == null ? 0 : _paramAsync.priority);//AssetBundleManager.GetResNode(this, param);

        if (resultResNode != null)
        {
            return resultResNode;
        }
        return null;
    }

    public void OnTimer(int nTimerID)
    {
        //延迟定时器
        if (nTimerID == m_nDurationTimer)
        {
            OnDelaySpawnTimer();
            OnDurationTimer();
        }
    }

    private void OnDurationTimer()
    {
        if (m_listActiveEffectParam.Count <= 0)
            return;
        SceneEffectParamBase param = null;
        for (int i = m_listActiveEffectParam.Count - 1; i >= 0; --i)
        {
            param = m_listActiveEffectParam[i];
            if (param == null)
                continue;

            int nEffectID = param.AttachEffectID;
            //如果在持续时间内EffectNode跟随父节点释放了的话，就直接移除吧
            if (param.ReturnComponent == null)
            {
                m_listActiveEffectParam.Remove(param);
                param = null;
            }
            else if (param.IsPlaying())
            {
                if (param.IsDurationTimeExit())
                {
                    //GameObject.DestroyObject(param.ReturnComponent.gameObject);
                    var temp = param.ReturnComponent.gameObject;
                    m_dicEffectIDResNodeUsed[nEffectID].DestroyInstanceRes(ref temp);
                    m_listActiveEffectParam.Remove(param);
                    param = null;
                }
            }
        }
    }

    private void OnDelaySpawnTimer()
    {
        if (m_listDelaySpawnEffectParam.Count <= 0)
            return;
        SceneEffectParamBase param = null;
        for (int i = m_listDelaySpawnEffectParam.Count - 1; i >= 0; --i)
        {
            param = m_listDelaySpawnEffectParam[i];
            if (param == null || param.IsPlaying())
            {
                m_listDelaySpawnEffectParam.Remove(param);
                continue;
            }

            if (param.IsDelayTimeExit())
            {
                _CreateSceneEffect(param);
                m_listDelaySpawnEffectParam.Remove(param);
            }

        }
    }

    public void ResLoadCallBack(ResConfigData cof, ResNode res, object UserDataObj)
    {
        SceneEffectParamAsync param = UserDataObj as SceneEffectParamAsync;
        ResNode resNodeInfo = null;
        if (param != null && m_dicEffectIDResNodeUsed.ContainsKey(param.AttachEffectID) && param.IsCanBeInstance)
        {
            resNodeInfo = m_dicEffectIDResNodeUsed[param.AttachEffectID];
            if (resNodeInfo != null)
                InitEffectParam(resNodeInfo, param);
        }

    }

    protected void InitEffectParam(ResNode resNodeInfo, SceneEffectParamBase _effectParam)
    {
        if (resNodeInfo != null && resNodeInfo.IsValid())
        {
            if (_effectParam.GetSceneEffectDelaySpawnTime() > 0)
            {
                DelaySceneEffect(_effectParam);
            }
            else
            {
                _CreateSceneEffect(_effectParam);
            }
            _effectParam.Init();

            if (_effectParam is SceneEffectParamAsync)
            {
                if ((_effectParam as SceneEffectParamAsync).onResLoadFinished != null)
                    (_effectParam as SceneEffectParamAsync).onResLoadFinished.Invoke((_effectParam as SceneEffectParamAsync).ResLoadUserObject);
            }
        }
    }
}


class SceneEffectHelper : MonoBehaviour
{
    public Transform AttachTarget;
    public int AttachEffect;
    public bool test;
    public bool testDelete;
    SceneEffectParamBase deleteParam;
    public string attachName = "Main/Root_A";
    public string szModelPath = "Gem_Lv2_Oval_1_NoDecal";
    public Transform parent;
    public void Update()
    {
        if (test)
        {
            test = false;

            ResNode itemNode = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "Effect/Scene/UI/StaticState/Baoshi/" + szModelPath);
            if (itemNode == null)
                return;

            Effect.EffectNode itemObject = itemNode.InstanceMainRes<Effect.EffectNode>();
            if (itemObject == null)
            {
                return;
            }

            //itemObject.StopEffect();
            itemObject.PlayEffect(false,-1, itemObject.SrcObj, itemObject.DstObj);
            itemObject.transform.SetParent(parent, false);
            itemObject.transform.localPosition = new UnityEngine.Vector3(0f, 0f, 0f);
        }
        if (testDelete)
        {
            testDelete = false;
            SceneEffectManager.Instance.DestroySceneEffect(ref deleteParam);
        }
    }
}
