/*******************************************************************
** 文件名:	StageManager.cs
** 修改人: 林建聪
** 日  期: 2015/07/23
** 描  述:  与逻辑层状态对应的场景状态管理
** 版  本:	1.0
** 应  用:  
 * 
*******************************************************************/
using UnityEngine;
using System;
using System.Collections;
using Stage_ManagerDef;
using GameLogic;


/// <summary>
/// 与逻辑层状态对应的场景状态管理,游戏场景加载接口在这里
/// </summary>
public abstract class BaseStageManager 
{

    public static BaseStageManager Instance
    {
        get
        {
            return m_Instance;
        }
    }

    protected static BaseStageManager m_Instance = null;

    ///////////////////////////////////////////////////////////////////
    //场景状态部分
    /// <summary>
    /// 当前状态
    /// </summary>
    protected EStage_State CurrentStageState;

    /// <summary>
    /// 状态改变进入新状态时发出的事件（新状态还没开始进入）
    /// 参数1是旧状态，参数2是新状态
    /// </summary>
    public static Action<EStage_State, EStage_State> onStageStateChanged = null;

    // 当前子状态
    public EStage_SubState CurSubState
    {
        set { CurStageSubState = value; }
        get { return CurStageSubState; }
    }
    protected EStage_SubState CurStageSubState = EStage_SubState.ESSS_NONE;

    //当前权限状态
    public EStage_PermissionState CurPermissionState
    {
        set { CurStagePermissionState = value; }
        get { return CurStagePermissionState; }
    }
    protected EStage_PermissionState CurStagePermissionState = EStage_PermissionState.ESPS_NORMAL;

    public virtual EStage_State getCurrentState()
    {
        return CurrentStageState;
    }

    public abstract bool SetStageState(EStage_State _NewState);

    public abstract int getSoundID();

    public abstract int getMapID();

    public abstract void onReadyToLoadMobaScene();

    public abstract void LoadSceneEnd();

    public abstract void LoadSceneBegin(game_LoadSceneInfo _data);
}