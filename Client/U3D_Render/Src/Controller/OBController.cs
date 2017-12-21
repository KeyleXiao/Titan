/*******************************************************************
** 文件名:	OBController.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2017 - Speed
** 创建人:	昔文博
** 日  期:	2017/11/27
** 版  本:	1.0
** 描  述:	OB操作控制
** 应  用:  
********************************************************************/
using System;
using System.Collections.Generic;
using UnityEngine;
using ASpeedGame.Data.Scheme;

public class OBController : MonoBehaviour
{ 
    public void Start()
    {
        DataCenter.LogicDataCenter.warOBDataManager.SwitchOBViewCampEvent += SwitchViewCamp;
    }

    public void OnDestroy()
    {
        DataCenter.LogicDataCenter.warOBDataManager.SwitchOBViewCampEvent -= SwitchViewCamp;
    }

    public void Update()
    {
        //非OB战场不执行
        if (StageManager.Instance.CurPermissionState != EStage_PermissionState.ESPS_OB || StageManager.Instance.CurSubState != EStage_SubState.ESSS_WAR)
            return;

        UpdateViewCamp();      //更新观察阵营
        UpdateViewHero();      //更新观察英雄
        UpdateLockHero();      //更新锁定英雄
    }

    /// <summary>
    /// 观察指定英雄
    /// </summary>
    /// <param name="entityID">实体ID</param>
    public void ViewTargetHero(int entityID)
    {
        U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(entityID);
        if (!DataCenter.LogicDataCenter.warOBDataManager.CheckEntityViewVisible(ev))
            return;

        ViewTargetHero(ev);
    }

    /// <summary>
    /// 观察指定英雄
    /// </summary>
    /// <param name="nCamp">英雄阵营</param>
    /// <param name="nIndex">英雄索引</param>
    public void ViewTargetHero(int nCamp, int nIndex)
    {
        int curMainCamp = DataCenter.LogicDataCenter.warOBDataManager.CurMainCamp;
        if (curMainCamp != 0 && nCamp != curMainCamp)
            return;

        U3D_Render.EntityView ev = DataCenter.LogicDataCenter.warOBDataManager.GetEntityIDByCampAndIndex(nCamp, nIndex);
        ViewTargetHero(ev);
    }

    /// <summary>
    /// 观察指定英雄
    /// </summary>
    /// <param name="ev"></param>
    public void ViewTargetHero(U3D_Render.EntityView ev)
    {
        if (U3D_Render.EntityView.isNullOrEmpty(ev))
            return;

#if ENABLE_YUNYING_MODE
        SoldierCamera.MainInstance<SoldierCamera>().SwitchOBSoliderTarget(ev);
        SoldierCamera.MainInstance<SoldierCamera>().EnterOB_yunying(CameraMode.YUNYING_OBHeroLock);
#else
        SoldierCamera.MainInstance<SoldierCamera>().SwitchOBSoliderTarget(ev);
        SoldierCamera.MainInstance<SoldierCamera>().EnableOBMode(true);
#endif

        USpeedUI.UISWitchHero heroData = new USpeedUI.UISWitchHero();
        heroData.nEntityID = ev.ID;
        USpeedUI.UISystem.Instance.SendWndMessage(USpeedUI.WndMsgID.WND_MSG_WAR_OBSERVER_SWITCH_HERO, heroData);
    } 

    private void UpdateViewCamp()
    {
#if ENABLE_YUNYING_MODE
        return;
#endif

        int nStartIndex = (int)SchemeOBControl.EOBCtrlKeyCodeType.EOBCtrl_view_blue;
        for (int i = 0; i < 2; i++)
        {
            if(CheckOBCtrlKeyCodeDown((SchemeOBControl.EOBCtrlKeyCodeType)(nStartIndex + i)))
            {
                DataCenter.LogicDataCenter.warOBDataManager.SwitchMainCamp(i+1);
            }
        }

        if (CheckOBCtrlKeyCodeDown(SchemeOBControl.EOBCtrlKeyCodeType.EOBCtrl_view_all))
        {
            DataCenter.LogicDataCenter.warOBDataManager.SwitchMainCamp(0);
        }
    }

    private void UpdateViewHero()
    {
#if ENABLE_YUNYING_MODE
        return;
#endif

        //观察当前阵营英雄
        int curMainCamp = DataCenter.LogicDataCenter.warOBDataManager.CurMainCamp;
        int nCamp1StartIndex = (int)SchemeOBControl.EOBCtrlKeyCodeType.EOBCtrl_player_camp1_1;
        for (int i = 0; i < 5; i++)
        {
            if (CheckOBCtrlKeyCodeDown((SchemeOBControl.EOBCtrlKeyCodeType)(nCamp1StartIndex + i)))
            {
                ViewTargetHero(curMainCamp == 0 ? 1 : curMainCamp, i);
            }
        }

        //如果正在观察双阵营
        if (curMainCamp == 0)
        {
            int nCamp2StartIndex = (int)SchemeOBControl.EOBCtrlKeyCodeType.EOBCtrl_player_camp2_1;
            for (int i = 0; i < 5; i++)
            {
                if (CheckOBCtrlKeyCodeDown((SchemeOBControl.EOBCtrlKeyCodeType)(nCamp2StartIndex + i)))
                {
                    ViewTargetHero(2, i);
                }
            }
        }
    }

    private void UpdateLockHero()
    {
        if (CheckOBCtrlKeyCodeDown(SchemeOBControl.EOBCtrlKeyCodeType.EOBCtrl_lock_nearbyHero))
        {
            SwitchNearbyViewHero();
        }

        if (InputManager.GetMouseButtonDown(0))
        {
            SwitchRandomViewHero();
        }
    }

    /// <summary>
    /// 切换观察阵营时的处理，如果锁定的英雄不可见了，则切换锁定目标
    /// </summary>
    /// <param name="nNewCamp"></param>
    private void SwitchViewCamp(int nNewCamp)
    {
        if (SoldierCamera.MainInstance<SoldierCamera>().cameraMode != CameraMode.OBHeroLock)
            return;

        U3D_Render.EntityView ev = SoldierCamera.MainInstance<SoldierCamera>().soldier;

        if (!DataCenter.LogicDataCenter.warOBDataManager.CheckEntityViewVisible(ev))
        {
            SwitchNearbyViewHero();
        }
    }

    /// <summary>
    /// 切换观察随机英雄
    /// </summary>
    private void SwitchRandomViewHero()
    {
        U3D_Render.EntityView ev = DataCenter.LogicDataCenter.warOBDataManager.GetRondomEntityView();
        ViewTargetHero(ev);
    }

    /// <summary>
    /// 切换观察当前距离最近的可见英雄
    /// </summary>
    private void SwitchNearbyViewHero()
    {
        if (SoldierCamera.MainInstance().CurrentCamera == null)
            return;

        ICollection<int> entityList = DataCenter.LogicDataCenter.warOBDataManager.GetCurMainCampPlayerList();
        Transform camTrans = SoldierCamera.MainInstance().CurrentCamera.transform;

        float nearbyDis = float.MaxValue;
        U3D_Render.EntityView nearbyEv = null;
        foreach (int evID in entityList)
        {
            U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(evID);
            if (U3D_Render.EntityView.isNullOrEmpty(ev) || ev.gameObject == null)
                continue;

            float dis = Vector3.SqrMagnitude(ev.gameObject.transform.position - camTrans.position);
            if (Mathf.Abs(dis) < nearbyDis)
            {
                nearbyDis = dis;
                nearbyEv = ev;
            }
        }

        ViewTargetHero(nearbyEv);
    }

    /// <summary>
    /// 检测OB控制键是否按下
    /// </summary>
    /// <param name="codeType"></param>
    /// <returns></returns>
    private bool CheckOBCtrlKeyCodeDown(SchemeOBControl.EOBCtrlKeyCodeType codeType)
    {
        List<KeyCode[]> codeList;
        if (!SchemeOBControl.Instance.GetKeyCodeList(codeType, out codeList))
            return false;

        if (codeList == null || codeList.Count < 1)
            return false;

        for(int i = 0; i < codeList.Count; i++)
        {
            if(i == codeList.Count - 1)
            {
                bool result = false;
                for(int j = 0; j < codeList[i].Length; j++)
                {
                    if (InputManager.GetKeyDown(codeList[i][j]))
                        result = true;
                }

                if (!result)
                    return false;
            }
            else
            {
                bool result = false;
                for (int j = 0; j < codeList[i].Length; j++)
                {
                    if (InputManager.GetKey(codeList[i][j]))
                        result = true;
                }

                if (!result)
                    return false;
            }
        }
        return true;
    }
}

