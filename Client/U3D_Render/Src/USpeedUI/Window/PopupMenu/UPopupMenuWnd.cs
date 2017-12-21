
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.EventSystems;
using USpeedUI.UWidgets;

namespace USpeedUI.PopupMenu
{
    public class UPopupMenuWnd : UIBaseWnd<UPopupMenuView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_POPUPMENUVIEW;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_OVERHEAD;
        }

        public override string GetPrefabPath()
        {
            return UPath.UCommomState + "/PopupMenu/PopupMenuView";
        }
        public override bool Init()
        {
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_POPUPMENU, this);

            SetVisible(true);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_POPUPMENU, this);
        }
        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            if (m_wndView == null)
                return;
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_POPUPMENU:
                    {
                        UIPopupMenuData popupData = data as UIPopupMenuData;
                        if(popupData != null)
                        {
                            m_wndView.ShowPopupMenu(popupData);
                        }
                    }
                    break;
            }
        }

        internal void ReleaseAllPopupList()
        {
            DataCenter.PopupMenuDataManager.Instance.onReleaseAllPopupList();
        }
    }

    public class UPopupMenuView : UIBaseWndView
    {
        private ResNode popUpMenuPrefabRes;

        protected List<UListPopup> m_ActivePopupList = new List<UListPopup>();
        protected List<UListPopup> m_PopupListCache = new List<UListPopup>();

        public override void Start()
        {
            base.Start();
            popUpMenuPrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,UPath.UPrefabsPath + "CommomState/PopupMenu/PopupMenu");

        }

        public override void Destroy()
        {
            AssetBundleManager.DeleteAssets(ref popUpMenuPrefabRes, true);
            base.Destroy();
        }

        public void ShowPopupMenu(UIPopupMenuData _actionData)
        {
            UListPopup popUpMenu = GetUnusePopupList();

            foreach (var action in _actionData.actionPopup)
            {
                popUpMenu.Add(action.Value);
            }

            popUpMenu.UpdatePopup(new UPopupMenuUpdateParam(_tfTarget: _actionData.actionTarget, _camUI: UISystem.Instance.GetCamera(),
                                                            _canvasRoot: UISystem.Instance.GetCanvas(), _eTipPostion: _actionData.actionTipPostion,
                                                            _tfPopupView: this.GetTransform(),_bUseModal:true,_actionModalCilck: ReleaseAllPopupList
                                                            ));
        }

        //protected UListComponentPopupBase m_lastEnterComponent = null;

        //protected void OnPopupListComponentEnterEvent(PointerEventData _eventData,UListComponentPopupBase _component)
        //{
        //    if (m_lastEnterComponent != null)
        //    {
        //        //判断当前进入的Com是不是在上次com的ChildPopupList里
        //        if (m_lastEnterComponent.ChildPopupList != null)
        //        {
        //            if (!m_lastEnterComponent.ChildPopupList.IsComponentExit(_component as UListComponentPopup))
        //                ReleaseChildPopupList(m_lastEnterComponent);
        //        }
        //    }
        //    m_lastEnterComponent = _component;


        //    if (_component.ItemData.ItemChildItems == null || _component.ItemData.ItemChildItems.Count <= 0)
        //        return;
        //    //有子列表数据且以显示子列表
        //    if (_component.ChildPopupList != null)
        //    {
        //        return;
        //    }
        //    Debug.Log("enter:" + _component.GetTransform.position);
           
        //    UListPopup popUpMenu = GetUnusePopupList();

        //    foreach (var action in _component.ItemData.ItemChildItems)
        //    {
        //        popUpMenu.Add(action as UListComponentItemDataPopup);
        //    }
            
        //    popUpMenu.UpdatePopup(new UPopupMenuUpdateParam(_tfTarget: _component.GetTransform, _camUI: UISystem.Instance.GetCamera(),
        //                                                    _canvasRoot: UISystem.Instance.GetCanvas(), _eTipPostion: UIWidgets.TipPosition.TopRightCorner,
        //                                                    _tfPopupView: this.GetTransform()
        //                                                    ));
        //    _component.ChildPopupList = popUpMenu;
        //}

        //protected void OnPopupListComponentExitEvent(PointerEventData _eventData, UListComponentPopupBase _component)
        //{
        //    UListComponentPopupBase hoverPopupCom = null;

        //    if(_eventData.pointerCurrentRaycast.gameObject != null)
        //    {
        //        hoverPopupCom = _eventData.pointerCurrentRaycast.gameObject.GetComponent<UListComponentPopupBase>();
        //        if(hoverPopupCom == null && _eventData.pointerCurrentRaycast.gameObject.transform.parent != null)
        //        {
        //            hoverPopupCom = _eventData.pointerCurrentRaycast.gameObject.transform.parent.GetComponent<UListComponentPopupBase>();
        //        }
        //    }

        //    //光标没有在任何UListComponentPopupBase上,释放吧
        //    if (hoverPopupCom == null)
        //    {
        //        ReleaseChildPopupList(_component);
        //        return;
        //    }
        //}
        //int iCount = 0;

        protected UListPopup GetUnusePopupList()
        {
            UListPopup popupList = null;

            if (m_PopupListCache.Count > 0)
            {
                popupList = m_PopupListCache[m_PopupListCache.Count - 1];
                m_PopupListCache.RemoveAt(m_PopupListCache.Count - 1);
            }
            else
            {
                popupList = popUpMenuPrefabRes.InstanceMainRes<UListPopup>();
                popupList.transform.SetParent(this.GetTransform(), false);
            }

            popupList.gameObject.SetActive(true);
            m_ActivePopupList.Add(popupList);
            popupList.OnEnterAction = OnPopupListComponentEnterEvents;
            popupList.OnExitAction = OnPopupListComponentExitEvents;
            popupList.OnClickAction = ReleaseAllPopupList;

            return popupList;
        }
        protected void ReleaseChildPopupList(UListComponentPopupBase _popupComponent)
        {
            //m_ActivePopupList.ForEach(DeactivateComponent);
            UListPopup releaseList = null;
            if (_popupComponent != null)
            {
                //有可能是二级菜单的组件
                releaseList = _popupComponent.ChildPopupList;
                if (releaseList == null)
                {
                    //0号是主列表
                    for(int i=1;i < m_ActivePopupList.Count;++i)
                    {
                        if(m_ActivePopupList[i].Contains(_popupComponent as UListComponentPopup))
                        {
                            releaseList = m_ActivePopupList[i];

                            releaseList.Clear();
                            if (!m_PopupListCache.Contains(releaseList))
                                m_PopupListCache.Add(releaseList);

                            m_ActivePopupList.RemoveAt(i);
                            return;
                        }
                    }
                    return;
                }                
            }
            else
                return;


            
            if (!m_PopupListCache.Contains(_popupComponent.ChildPopupList))
                m_PopupListCache.Add(_popupComponent.ChildPopupList);
            if(m_ActivePopupList.Contains(_popupComponent.ChildPopupList))
                m_ActivePopupList.Remove(_popupComponent.ChildPopupList);

            _popupComponent.ChildPopupList.Clear();
            _popupComponent.ChildPopupList = null;
        }
        protected void ReleaseChildPopupList(UListPopup _popupList)
        {
            if (!m_PopupListCache.Contains(_popupList))
                m_PopupListCache.Add(_popupList);
            if (m_ActivePopupList.Contains(_popupList))
                m_ActivePopupList.Remove(_popupList);

            _popupList.Clear();
        }
        protected void ReleaseAllPopupList()
        {
            (m_wnd as UPopupMenuWnd).ReleaseAllPopupList();

            if (m_ActivePopupList.Count <= 0)
                return;

            UListPopup list;
            while (m_ActivePopupList.Count > 0)
            {
                list = m_ActivePopupList[0];

                m_ActivePopupList.Remove(list);
                m_PopupListCache.Add(list);
                list.Clear();
            }
        }

        protected void OnPopupListComponentEnterEvents(PointerEventData _eventData,UListComponentPopupBase _component)
        {
            //没有下级的就不管了
            if (_component.ChildPopupList != null)
                return;
            if (_component.ItemData.ItemChildItems == null || _component.ItemData.ItemChildItems.Count <= 0)
                return;

            UListPopup childList = null;
            //判断当前是否已经显示超过两个列表,是则看最后一级列表能否复用
            if (m_ActivePopupList.Count - 1 > 0)
            {
                childList = m_ActivePopupList[m_ActivePopupList.Count - 1];
                //如果当前进入的对象在最后一级列表里，则不能复用
                if (childList.Contains(_component as UListComponentPopup))
                {
                    childList = null;
                }
                else
                {
                    //不在最后一级列表，则释放最后一级列表
                    ReleaseChildPopupList(childList.ParentConponent);
                    //如果当前进入的对象属于主列表，则释放所有次级列表
                    if (m_ActivePopupList[0].Contains(_component as UListComponentPopup))
                    {
                        for(int i=m_ActivePopupList.Count-1;i>0;--i)
                        {
                            ReleaseChildPopupList(m_ActivePopupList[i]);
                        }
                    }
                    childList = null;
                }

                    
            }
            //从回收的列表里取列表并赋值显示
            if (childList == null)
                childList = GetUnusePopupList();

            foreach (var action in _component.ItemData.ItemChildItems)
            {
                childList.Add(action as UListComponentItemDataPopup);
            }

            childList.UpdatePopup(new UPopupMenuUpdateParam(_tfTarget: _component.GetTransform, _camUI: UISystem.Instance.GetCamera(),
                                                            _canvasRoot: UISystem.Instance.GetCanvas(), _eTipPostion: UIWidgets.TipPosition.TopRightCorner,
                                                            _tfPopupView: this.GetTransform()
                                                            ));
            _component.ChildPopupList = childList;
            childList.ParentConponent = _component as UListComponentPopup;

        }

        protected void OnPopupListComponentExitEvents(PointerEventData _eventData,UListComponentPopupBase _component)
        {

        }
    }
}
