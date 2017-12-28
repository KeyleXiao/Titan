 using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using System.Collections;

namespace USpeedUI
{
    // 窗口逻辑基类
    public abstract class UIBaseWnd<T> : IUIWnd,IAsyncResLoadHandler
        where T : UIBaseWndView
    {
        // 窗口视图类
        protected T m_wndView;
        // 是否可见
        protected bool m_isVisible;
        // 是否需要异步加载
        protected bool m_isAsyncLoad;
        /// <summary>
        /// 是否可按esc关闭
        /// </summary>
        protected bool m_isKeyClose;

        // 设置当前窗体是否有Esc回调 
        protected bool m_bHaveEscCallBack = false;

        //设置当前窗体是否需要屏蔽技能音效，只播放UI音效
        protected bool m_bForbid3DSound = false;

        // 获取ID
        public abstract WndID GetID();
        // 获取资源路径
        public abstract String GetPrefabPath();
        // 获取所在层次
        public abstract WndLayerDef GetLayer();

        private ResNode ViewPrefabRes = null;
        private GameObject ViewInstance = null;

        private bool m_Init = false;

        // 缓存消息（当View还没创建好后，则先把消息缓存好来）
        protected List<UIMsgDataCache> m_CacheUIMsgList = new List<UIMsgDataCache>();

        /// <summary>
        /// 取是否有ESC回调
        /// </summary>
        /// <returns></returns>
        public virtual bool HaveEscCallBack()
        {
            return m_bHaveEscCallBack;
        }

        public virtual bool OnEscCallBack()
        {
            return true;
        }

        // 初始化
        public virtual bool Init()
        {
            m_Init = true;
            return true;
        }

        // 销毁
        public virtual void Destroy()
        {
            UnloadView();
            m_Init = false;
        }

        // 载入窗口资源
        public virtual bool LoadView()
        {
            if (m_wndView != null)
                return true;

            if (!m_isAsyncLoad)
            {
                return SyncLoadView();
            }
            else
            {
                // 危险动作，请勿模仿
                UISystem uiSys = (UISystem)UISystem.Instance;
                uiSys.StartCoroutine(AsyncLoadView());
            }

            return true;
        }

        private bool SyncLoadView()
        {
            bool result = false;

            Initialize.PrintTickCheck("SyncLoadView",_bStart:true);

            ViewPrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/" + GetPrefabPath());

            Initialize.PrintTickCheck(GetID() + " SyncLoadView GetAssets");

            if (null != ViewPrefabRes)
            {
                result = OnLoadView(ViewPrefabRes);
            }
            return result;
        }

        public void ResLoadCallBack(ResConfigData cof,ResNode res,System.Object userDataObj)
        {
            ViewPrefabRes = res;
            OnLoadView(ViewPrefabRes);
        }
        
        private IEnumerator AsyncLoadView()
        {
            AssetBundleManager.GetAssetsAsync(AssetType.Asset_Prefab, this, "UI/Prefabs/" + GetPrefabPath());
            yield return new WaitForEndOfFrame();
        }

        private bool OnLoadView(ResNode viewPrefab)
        {
            Initialize.PrintTickCheck("OnLoadView", _bStart:true);

            if (viewPrefab == null)
                return false;

            ViewInstance = viewPrefab.InstanceMainRes();

            Initialize.PrintTickCheck(GetID()+ ",OnLoadView - InstanceMainRes");

            if (!ViewInstance)
                return false;

            ViewInstance.name = viewPrefab.AssetName;
            m_wndView = ViewInstance.GetComponent<IUIWndView>() as T;
            if (m_wndView == null)
            {
                Debug.LogError("UISystem OnLoadView m_wndView == null  id=" + GetID());
                viewPrefab.DestroyInstanceRes(ref ViewInstance);
                return false;
            }

            if (m_wndView.Init(this) == false)
            {
                Debug.LogError("UISystem OnLoadView m_wndView.Init == false  id=" + GetID());
                viewPrefab.DestroyInstanceRes(ref ViewInstance);
                return false;
            }

            Initialize.PrintTickCheck(GetID() + ",OnLoadView - View.Init");


            UISystem.Instance.AddView(m_wndView, GetLayer());

            Initialize.PrintTickCheck(GetID() + ",OnLoadView - AddView");


            m_wndView.SetRendererVisible(m_isVisible);

            Initialize.PrintTickCheck(GetID() + ",OnLoadView - View.SetRendererVisible");

            return true;
        }

        // 卸载窗口资源
        public virtual void UnloadView()
        {
            if (m_wndView != null)
            {
                SetVisible(false);
                m_wndView.Destroy();
                m_wndView = null;
                AssetBundleManager.DeleteAssets(ref ViewPrefabRes, true);
            }
        }

        // 获取视图类
        public virtual IUIWndView GetWndView()
        {
            return m_wndView;
        }

        // 是否可见
        public virtual bool IsVisible()
        {
            if (m_wndView == null)
                return false;

            return m_wndView.IsVisible();
        }

        // 设置是否可见
        public virtual void SetVisible(bool visible)
        {
            //如果显示条件不成立，就return掉
            if (!CheckWndLayerState(visible))
                return;

            PreSetVisible(visible);

            _setVisible(visible);

            PostSetVisible(visible);

            //如果需要屏蔽技能音效
            if(m_bForbid3DSound)
                UISystem.Instance.Forbid3DSound(this, visible);

        }

        /// <summary>
        /// 关闭操作处理,有需要时才重载，如果只是按esc隐藏本窗体的操作的话，只需在Init方法里设置m_isEscClose = true即可
        /// </summary>
        /// <returns></returns>
        public virtual bool OnCloseKey()
        {
            //SetVisible(false);
            return m_isKeyClose;//true
        }

        public virtual bool IsCanCloseKey()
        {
            return m_isKeyClose;
        }

        /// <summary>
        /// 获取互斥等级
        /// </summary>
        /// <returns></returns>
        public virtual WndMutexLevelDef GetMutexLevel()
        {
            return WndMutexLevelDef.WND_Mutex_None;
        }

        //载入Mask,分地图类型加载
        public virtual int GetLoadMask()
        {
            return (int)WndLoadMaskDef.WND_LoadMask_All;
        }

        //权限Mask，分O玩家权限身份加载
        public virtual int GetPermissionMask()
        {
            return (int)WndPermissionMaskDef.WND_PM_All;
        }

        public bool InitFinish()
        {
            return m_Init;
        }

        // 接受消息
        public virtual void OnMessage(WndMsgID msgID, UIMsgData data)
        {
        }

        // 接受按键消息
        public virtual bool OnKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status)
        {
            return false;
        }

        protected virtual bool CheckWndLayerState(bool _bVisible)
        {
            return true;
        }

        /// <summary>
        /// 在 view.setVisible 前执行
        /// </summary>
        protected virtual void PreSetVisible(bool _bVisible) { }
        
        /// <summary>
        /// 在View.setVisible 后执行
        /// </summary>
        protected virtual void PostSetVisible(bool _bVisible) { }

        /// <summary>
        /// 实际的SetVisible，没加载就加载，有加载就控制显示隐藏
        /// </summary>
        protected virtual void _setVisible(bool _bVisible)
        {
            m_isVisible = _bVisible;

            if (m_wndView != null)
            {
                m_wndView.SetRendererVisible(_bVisible);//m_wndView.SetVisible(visible);
            }
            else
            {
                if (_bVisible)
                    LoadView();
            }
        }

        /// <summary>
        /// 缓存窗口消息
        /// </summary>
        /// <param name="msgID"></param>
        /// <param name="msgData"></param>
        protected void saveCacheUIMsg(WndMsgID msgID, UIMsgData msgData)
        {
            if (m_wndView == null)
            {
                UIMsgDataCache cacheMsg = new UIMsgDataCache();
                cacheMsg.msgID = (int)msgID;
                cacheMsg.msgData = msgData;

                Debug.Log("增加缓存消息," + GetID() + "," + msgID);
                m_CacheUIMsgList.Add(cacheMsg);
            }
        }

        protected void runCacheUIMsg(WndMsgID curWndMsgID = WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER)
        {
            if(m_wndView == null)
            {
                return;
            }

            // 执行缓存消息
            foreach (var item in m_CacheUIMsgList)
            {
                WndMsgID msgid = (WndMsgID)item.msgID;

                Debug.Log("执行缓存消息," + GetID() + "," + msgid);

                if (curWndMsgID == msgid)
                {
                    continue;
                }

                OnMessage(msgid, item.msgData);
            }

            m_CacheUIMsgList.Clear();
        }

    }

    public abstract class UIPopupWnd<T>:UIBaseWnd<T>
        where T : UIBaseWndView
    {
        /// <summary>
        /// 对于Pop1层界面，如果Fill层正在拥有最高级别的互斥界面显示，则没有权限显示以及提升至Pop2
        /// </summary>
        /// <param name="_bVisible"></param>
        /// <returns></returns>
        protected override bool CheckWndLayerState(bool _bVisible)
        {
            if (_bVisible && GetLayer() == WndLayerDef.WND_LAYER_POPUP01)
            {
                bool bHasMutexWnd = UISystem.Instance.IsWndLayerHasHighMutexWndShow(WndLayerDef.WND_LAYER_FILL);
                return !bHasMutexWnd;
            }
            return true;
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            if (m_wndView == null)
                return;

            if (_bVisible)
            {
                //有全屏窗口显示中并当前WND_LAYER_POPUP01类型窗口需要显示时，将其提升到WND_LAYER_POPUP02层进行显示
                if (GetLayer() == WndLayerDef.WND_LAYER_POPUP01)
                {
                    UISystem.Instance.SetWndRenderAfterTheFillLayer(m_wndView,WndLayerDef.WND_LAYER_POPUP02);
                }
                //Popup类型显示时顶置
                if ((GetLayer() == WndLayerDef.WND_LAYER_POPUP01 || GetLayer() == WndLayerDef.WND_LAYER_POPUP02))
                {
                    UISystem.Instance.SetMutexWndVisbleAlone(this.m_wndView, WndLayerDef.WND_LAYER_POPUP01, WndLayerDef.WND_LAYER_FILL, WndLayerDef.WND_LAYER_POPUP02);

                    if (m_wndView.GetTransform() != null)
                    {
                        m_wndView.GetTransform().SetAsLastSibling();

                        //System.Diagnostics.Stopwatch watch = new System.Diagnostics.Stopwatch();
                        //StringBuilder sb = new StringBuilder();
                        //watch.Start();

                        //重新算该层级下所有界面的层级数
                        USpeedUILayer layer = USpeedUILayerManager.GetUILayer(GetLayer());
                        if (layer != null)
                        {
                            RectTransform LayerTrans = layer.GetRectTransform;
                            if (LayerTrans != null)
                            {
                                int count = LayerTrans.childCount;
                                for (int i = 0; i < count; ++i)
                                {
                                    USpeedUILayerElement element = LayerTrans.GetChild(i).GetComponent<USpeedUILayerElement>();
                                    if (element != null)
                                        LayerTrans.GetChild(i).GetComponent<USpeedUILayerElement>().SetSortingOrder();
                                }
                            }
                        }
 
                    }

                    //watch.Stop();
                    //sb.Append(m_wndView.gameObject.name + " time:" + watch.Elapsed.TotalMilliseconds);
                    //Debug.LogWarning(sb.ToString());
                }


            }
            else
            {
                UISystem.Instance.RemoveWndRenderAfterFillLayerVisible(this);
                //隐藏的时候还原回原来的层
                UISystem.Instance.AddView(m_wndView, GetLayer());
            }
        }

    }

    public abstract class UIFillWnd<T>:UIBaseWnd<T>
        where T : UIBaseWndView
    {
        //Fill层默认为普通互斥级别
        public override WndMutexLevelDef GetMutexLevel()
        {
            return WndMutexLevelDef.WND_Mutex_Normal;
        }

        //对于Fill层，不允许同时显示多个全屏界面，同时按照互斥级别处理显示
        protected override bool CheckWndLayerState(bool visible)
        {
            //不允许同时显示多个全屏界面
            if(GetLayer() == WndLayerDef.WND_LAYER_FILL && visible)
            {
                RectTransform rect = UISystem.Instance.GetWndLayerTrans(WndLayerDef.WND_LAYER_FILL);
                for (int i = 0; i < rect.childCount; ++i)
                {
                    Transform r = rect.GetChild(i);
                    if (r != null)
                    {
                        IUIWndView wnd = r.GetComponent<IUIWndView>();
                        if (wnd != null && wnd.GetUIWnd().IsVisible())
                        {
                            //如果希望显示的界面互斥级别高于正在显示的，则隐藏正在显示的Fill层界面
                            if (GetMutexLevel() >= wnd.GetUIWnd().GetMutexLevel())
                            {
                                wnd.GetUIWnd().SetVisible(false);
                                return true;
                            }//否则不允许显示
                            else
                                return false;

                            #region 暂不用
                            //Func<bool> resetCurrentFillWnd = () =>
                            //{
                            //    wnd.GetUIWnd().SetVisible(false);
                            //    SetVisibleEx(visible);
                            //    return true;
                            //};

                            //TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                            //                       title: "警告",
                            //                       message: "该操作将会离开房间界面",
                            //                       buttons: new UIWidgets.DialogActions() { { "确认", resetCurrentFillWnd } ,
                            //                                       {"取消",null } },
                            //                       modal: true
                            //                       );
                            //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);

                            //return;
                            #endregion

                        }
                    }
                }
            }
            return true;
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);

            if (GetLayer() == WndLayerDef.WND_LAYER_FILL)
            {
                UISystem.Instance.SetWndLayerVisible_Not(WndLayerDef.WND_LAYER_FILL,WndLayerDef.WND_LAYER_FIXED,WndLayerDef.WND_LAYER_POPUP01);
                UISystem.Instance.SetWndRenderAfterFillLayerVisible();
            }
        }

    }
}
