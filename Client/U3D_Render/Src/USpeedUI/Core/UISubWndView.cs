using System;
using System.Collections.Generic;
using System.Text;
using UnityEngine;

// 这里的类尚未最终定型，有需要的可以等正式化后再用
namespace USpeedUI
{
    public interface IUISubWnd
    {
        // 获取子类型
        int GetWndType();
        // 获取资源路径
        String GetPrefabPath();

        // 初始化
        bool Init();
        // 设置主窗口容器
        void SetContainer(Transform container);

        // 载入窗口资源
        bool LoadView();
        // 获取视图类
        IUISubWndView GetWndView();

        // 是否可见
        bool IsVisible();
        // 设置是否可见
        void SetVisible(bool visible);

        // 接受消息
        void OnMessage(WndMsgID msgID, UIMsgData data);
        // 销毁
        void Destroy();
    }

    public interface IUISubWndView
    {
        // 初始化
        bool Init(IUISubWnd wnd);

        // 获取对应的IUIWnd
        IUISubWnd GetUIWnd();
        // 获取RectTransform
        RectTransform GetTransform();

        // 是否可见
        bool IsVisible();
        // 设置是否可见
        void SetVisible(bool visible);
    }

    public abstract class UIBaseSubWnd<T> : IUISubWnd
        where T : UIBaseSubWndView
    {
        protected Transform m_parentContainer;
        protected T m_wndView;
        protected bool m_isVisible;

        public abstract int GetWndType();
        public abstract String GetPrefabPath();

        public virtual bool Init()
        {
            return true;
        }

        public virtual void Destroy()
        {
            return ;
        }

        public virtual void SetContainer(Transform container)
        {
            m_parentContainer = container;
        }

        // 载入窗口资源
        public virtual bool LoadView()
        {
            if (m_wndView != null)
                return true;
            ResNode viewPrefab = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/" + GetPrefabPath());
            if (null == viewPrefab)
                return false;

            GameObject instance = viewPrefab.InstanceMainRes();
            if (!instance)
                return false;

            instance.name = viewPrefab.AssetName;
            m_wndView = instance.GetComponent<IUISubWndView>() as T;
            if (m_wndView == null)
            {
                Debug.LogError("UISystem SubWnd LoadView m_wndView == null  GetPrefabPath=" + GetPrefabPath());
                viewPrefab.DestroyInstanceRes(ref instance);
                return false;
            }

            if (m_wndView.Init(this) == false)
            {
                Debug.LogError("UISystem SubWnd LoadView m_wndView.Init == false  GetPrefabPath=" + GetPrefabPath());
                viewPrefab.DestroyInstanceRes(ref instance);
                return false;
            }

            m_wndView.GetTransform().SetParent(m_parentContainer, false);
            m_wndView.SetVisible(m_isVisible);

            return true;
        }

        // 获取视图类
        public virtual IUISubWndView GetWndView()
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
            m_isVisible = visible;

            if (m_wndView != null)
            {
                m_wndView.SetVisible(visible);
            }
            else
            {
                if (visible)
                    LoadView();
            }
        }

        public virtual void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
        }
    }

    public abstract class UIBaseSubWndView : MonoBehaviour, IUISubWndView
    {
        // 子窗口对象
        protected IUISubWnd m_wnd;
        // transform组件
        protected RectTransform m_transform;

        // 初始化
        public virtual bool Init(IUISubWnd wnd)
        {
            m_wnd = wnd;
            return true;
        }

        // 获取对应的IUIWnd
        public virtual IUISubWnd GetUIWnd()
        {
            return m_wnd;
        }

        // 获取RectTransform
        public virtual RectTransform GetTransform()
        {
            if (m_transform == null)
            {
                m_transform = GetComponent<RectTransform>();
            }
            return m_transform;
        }

        // 是否可见
        public virtual bool IsVisible()
        {
            return gameObject.activeSelf;
        }

        // 设置是否可见
        public virtual void SetVisible(bool visible)
        {
            gameObject.SetActive(visible);
        }

        // MonoBehaviour
        public virtual void Awake()
        {
            m_transform = GetComponent<RectTransform>();
        }

        public virtual void OnDestroy()
        { }
    }
}
