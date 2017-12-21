/*******************************************************************
** 文件名:	UMediatorTalentPage.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-3-24
** 版  本:	1.0
** 描  述:	登录界面视图模块中介类（接发消息类）
		
********************************************************************/
using System.Collections.Generic;
using GameLogic;
using System;
using System.Runtime.InteropServices;
using DataCenter;
using ASpeedGame.Data.TalentPage;
using USpeedUI.TooltipAndDialog;
using UnityEngine;
using USpeedUI.PlayerFrame;
namespace USpeedUI.TalentPagePart
{

    public class TalentPageWnd : UIPopupWnd<TalentPageWndView>
    {
        public bool bHasLoad = false;
        public bool bHasInitView = false;
        private const int nDefaultPageIndex = 1;
        public int nInitPageNum = 0;
        public bool bRoomState = false;
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_PLAYER_TALENTPAGE;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "TalentPage/TalentPage";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01; ;
        }

        public override int GetPermissionMask()
        {
            return (int)WndPermissionMaskDef.WND_PM_Normal;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TALENTPAGE_INITTALENTDATA, this);
            //UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERFRAME_ENTERPBUTTON, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERFRAME_CLOSE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TALENTPAGE_CHANGEPAGE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MAIN_OPEN, this);
            // 订阅进入战场事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TALENTPAGE_ROOMOPENTALENT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TALENTPAGE_INITTALENTDATA, this);
            //UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERFRAME_ENTERPBUTTON, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERFRAME_CLOSE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TALENTPAGE_CHANGEPAGE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MAIN_OPEN, this);
            // 退订进入战场事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TALENTPAGE_ROOMOPENTALENT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_TALENTPAGE_INITTALENTDATA:
                    {
                        UIMsgCmdData uiTalentPage = (UIMsgCmdData)msgData;
                        LogicDataCenter.talentPageDataManager.ReceiveEntityViewTalentInfo(uiTalentPage.ptrParam, uiTalentPage.nPtrLen, ref nInitPageNum);
                        LogicDataCenter.talentPageDataManager.SetCurPageIndex(nDefaultPageIndex);
                        InitTalentToShow();
                    }
                    break;
                //case WndMsgID.WND_MSG_PLAYERFRAME_ENTERPBUTTON:
                //    {
                //        UIButtonType uiPlayFrameType = (UIButtonType)msgData;
                //        onShowTalentPage(uiPlayFrameType.nType);
                //    }
                //    break;
                case WndMsgID.WND_MSG_PLAYERFRAME_CLOSE:
                    {
                        if (bRoomState)
                        {
                            SetSaveSingleTalentPage();
                            MsgHandlerToRoom();
                        }
                        else
                        {
                            onMsgBoxSaveInfo();
                        }

                        SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        this.UnloadView();
                        EntityFactory.RemoveEntityNumPropHandler(ResponseEntityNumPropEvent);
                    }
                    break;
                case WndMsgID.WND_MSG_TALENTPAGE_CHANGEPAGE:
                    {
                        onSendTalentPageIndexForEffect();
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MAIN_OPEN:
                    {
                        LoadView();
                        OnRequestTalentPagePoints();
                    }
                    break;
                case WndMsgID.WND_MSG_TALENTPAGE_ROOMOPENTALENT:
                    {
                        OnRoomOpenTalentView();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        EntityFactory.SetEntityNumPropHandler(ResponseEntityNumPropEvent);
                        bHasInitView = false;
                    }
                    break;
                default:
                    break;
            }
        }

        //数值属性回调函数
        public void ResponseEntityNumPropEvent(EntityNumPropEventArgs e)
        {
            switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_LEVEL:
                    {
                        if(EntityFactory.MainHeroID!=e.data.nEntityID)
                        {
                            return;
                        }

                        OnPlayerLevelChange(e.objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL));
                    }
                    break;
                default:
                    break;
            }
        }

        public void onShowTalentPage(int nType)
        {
            //if (nType != (int)PlayerSubWndType.TalentPage)
            //{
            //    onMsgBoxSaveInfo();
            //    SetVisible(false);
            //    return;
            //}

            SetVisible(true);

            if (m_wndView == null)
            {
                return;
            }

            m_wndView.gameObject.transform.SetAsLastSibling();
        }

        public void onMsgBoxSaveInfo()
        {
            if (m_isVisible && LogicDataCenter.talentPageDataManager.GetTalentPageChange())
            {
                UIDialogData data = new UIDialogData(title: "天赋保存询问",
                                            message: "是否保存当前天赋页",
                                            buttons: new UIWidgets.DialogActions() { { "确认", SetSaveHandler },
                                             { "取消", SetDisableSaveHandler}},
                                              closeButtonAction: new UnityEngine.Events.UnityAction(SetDisableSaveHandlerEx)
                                           );
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
            }
            else
            {
                MsgHandlerToRoom();
            }

        }

        public void SetDisableSaveHandlerEx()
        {
            SetDisableSaveHandler();
            return;
        }

        public bool SetDisableSaveHandler()
        {
            MsgHandlerToRoom();

            SetRescoverCurPage();
            return true;
        }

        public bool SetSaveHandler()
        {
            MsgHandlerToRoom();

            SetSaveSingleTalentPage();
            return true;
        }

        public void SetTalentIsChange(bool bChange)
        {
            if (m_wndView == null)
            {
                return;
            }

            m_wndView.SetButtonState(bChange);
        }

        public void onTalentPageLeftBtnClickSignal(int nTalentId)
        {
            if (m_wndView == null)
            {
                return;
            }

            int nTalentType = 0;
            int nOldPoints = LogicDataCenter.talentPageDataManager.GetTalentPagePointsInfo(nTalentId);
            int nAddPoint = LogicDataCenter.talentPageDataManager.CalculateAddPoint(nTalentId, ref nTalentType);
            int nTypeAllPoints = LogicDataCenter.talentPageDataManager.GetOneTypeOverLayPoints(nTalentType);
            int nLeftPoints = LogicDataCenter.talentPageDataManager.GetTalentPageLeftTotalNum();
            int nShowPoints = nOldPoints + nAddPoint;

            foreach (int nTalentOtherId in LogicDataCenter.talentPageDataManager.dicSameTypeLevelIdPoints.Keys)
            {
                m_wndView.showTalentButtonPoints(LogicDataCenter.talentPageDataManager.dicSameTypeLevelIdPoints[nTalentOtherId], nTalentOtherId, nTypeAllPoints, nLeftPoints);
            }

            m_wndView.showTalentButtonPoints(nShowPoints, nTalentId, nTypeAllPoints, nLeftPoints);
            m_wndView.SetTypeNumShow(nTalentType, nTypeAllPoints, nLeftPoints);
            m_wndView.SetTopPageDescShow(GetAllViewData());

            if (nLeftPoints == 0)
            {
                m_wndView.SetAllTalentLimitDescChange(LogicDataCenter.talentPageDataManager.GetTalentPageTempPoints(), GetAllViewData());
            }
            else
            {
                m_wndView.SetNextTalentLimitDescChange(nTalentId, nTypeAllPoints, nLeftPoints);
            }
        }

        public void onTalentPageRightBtnClickSignal(int nTalentId)
        {
            if (m_wndView == null)
            {
                return;
            }

            int nTalentType = 0;
            int nShowPoints = LogicDataCenter.talentPageDataManager.CalculateReducePoint(nTalentId, ref nTalentType);
            int nLeftPoints = LogicDataCenter.talentPageDataManager.GetTalentPageLeftTotalNum();
            int nTypeAllPoints = LogicDataCenter.talentPageDataManager.GetOneTypeOverLayPoints(nTalentType);
            if (nShowPoints == -1)
            {
                return;
            }

            m_wndView.showTalentButtonPoints(nShowPoints, nTalentId, nTypeAllPoints, nLeftPoints);
            m_wndView.SetTypeNumShow(nTalentType, nTypeAllPoints, nLeftPoints);
            m_wndView.SetTopPageDescShow(GetAllViewData());
            if (nLeftPoints == 1)
            {
                m_wndView.SetAllTalentLimitDescChange(LogicDataCenter.talentPageDataManager.GetTalentPageTempPoints(), GetAllViewData());
            }
            else
            {
                m_wndView.SetNextTalentLimitDescChange(nTalentId, nTypeAllPoints, nLeftPoints);
            }
        }

        public bool SetSaveSingleTalentPage()
        {
            if (m_wndView == null)
            {
                return true;
            }

            cmd_TalentPage_Points_Single cmdSinglePoint = new cmd_TalentPage_Points_Single();
            bool bSave = LogicDataCenter.talentPageDataManager.SetSaveSingleTalentPage(ref cmdSinglePoint);

            if (!bSave)
            {
                return true;
            }

            EntityEventHelper.Instance.SendCommand<cmd_TalentPage_Points_Single>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_SAVE_TALENT_PAGE_SINGLE, ref cmdSinglePoint);
            return true;
        }

        public void SetRescoverCurPage()
        {
            LogicDataCenter.talentPageDataManager.SetRescoverCurPage();

            GetViewDataAndShow();
        }

        public void SetDeleteCurPage()
        {
            cmd_Entity_Talent_Page_Detele cmdPointDelete = new cmd_Entity_Talent_Page_Detele();
            LogicDataCenter.talentPageDataManager.SetDeletePage(ref cmdPointDelete);
            EntityEventHelper.Instance.SendCommand<cmd_Entity_Talent_Page_Detele>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_BEGIN_TALENT_PAGE_DELETE, ref cmdPointDelete);

            GetViewDataAndShow();
        }

        public void SetResetCurPage()
        {
            LogicDataCenter.talentPageDataManager.SetResetCurPage();

            GetViewDataAndShow();
        }

        public void ChangeTalentPageSortIndex(int nPageSortIndex)
        {
            LogicDataCenter.talentPageDataManager.SetCurPageIndex(nPageSortIndex);

            GetViewDataAndShow();
        }

        public void SetNewPage(int nPageSortIndex)
        {
            LogicDataCenter.talentPageDataManager.SetNewPage(nPageSortIndex);

            GetViewDataAndShow();
        }

        public void SetPageName(string sName)
        {
            LogicDataCenter.talentPageDataManager.SetName(sName);

            LogicDataCenter.talentPageDataManager.SetTalentPageChange(true);

            m_wndView.SetTopPageDescShow(GetAllViewData());
        }

        public void OnRequestTalentPagePoints()
        {
            if (bHasLoad)
            {
                InitTalentToShow();
                return;
            }
            // 请求所有加点情况
            cmd_Entity_Request_Talent_Page cmdRequestPointAll = new cmd_Entity_Request_Talent_Page();
            cmdRequestPointAll.bIsRequestSelf = 1;
            cmdRequestPointAll.RequestFriendID = 0;

            EntityEventHelper.Instance.SendCommand<cmd_Entity_Request_Talent_Page>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQUEST_TALENT_PAGE_ALL, ref cmdRequestPointAll);
            bHasLoad = true;
        }

        public void InitTalentToShow()
        {
            if (m_wndView == null || !m_isVisible || bHasInitView)
            {
                return;
            }

            bHasInitView = true;
            m_wndView.nCurPageIndex = nDefaultPageIndex;
            m_wndView.CreateAllTopTalentButton(nInitPageNum);
            GetViewDataAndShow();
        }

        public void onSendTalentPageIndexForEffect()
        {
            cmd_Entity_Talent_Start_Effect cmdPointEffect = new cmd_Entity_Talent_Start_Effect();
            cmdPointEffect.nCurTalentPage = LogicDataCenter.talentPageDataManager.GetDataIndex();

            EntityEventHelper.Instance.SendCommand<cmd_Entity_Talent_Start_Effect>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_BEGIN_TALENT_PAGE_EFFECT, ref cmdPointEffect);
        }

        public void GetViewDataAndShow()
        {
            if (m_wndView == null)
            {
                return;
            }

            m_wndView.SetOnePagePointView(GetAllViewData(), LogicDataCenter.talentPageDataManager.GetTalentPageTempPoints());
        }

        public TalentChangePageModelInfo GetAllViewData(int nPageIndex = -1)
        {
            return LogicDataCenter.talentPageDataManager.GetAllTalentViewData(nPageIndex);
        }

        public void OnRoomOpenTalentView()
        {
            SetVisible(true);
            if (m_wndView == null)
            {
                return;
            }

            InitTalentToShow();
            m_wndView.gameObject.transform.SetAsLastSibling();
            m_wndView.EnterRoomTalentState();
            m_wndView.ChangeTalentPageSortIndex(LogicDataCenter.talentPageDataManager.nCurPageSortIndex);
            m_wndView.SetSelectTopBtnState();
            bRoomState = true;
        }

        public void LeaveRoomTalentState()
        {
            onMsgBoxSaveInfo();
        }

        public void MsgHandlerToRoom()
        {
            if (bRoomState)
            {
                SetViewTransform();
                bRoomState = false;
            }
        }

        public void SetViewTransform()
        {
            if (m_wndView)
            {
                m_wndView.SetTalentObjectsActive(false);
            }

            SetVisible(false);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMTALENTINROOMCLOSE, null);
        }

        public void OnPlayerLevelChange(int nLevel)
        {
            bool bChange = LogicDataCenter.talentPageDataManager.SetPlayerLevelChange(nLevel);
            if (!bChange)
            {
                return;
            }

            GetViewDataAndShow();

            int nLeftPoints = LogicDataCenter.talentPageDataManager.GetTalentPageLeftTotalNum();
            if (nLeftPoints == 1)
            {
                m_wndView.SetAllTalentLimitDescChange(LogicDataCenter.talentPageDataManager.GetTalentPageTempPoints(), GetAllViewData());
            }
        }
    }
}
