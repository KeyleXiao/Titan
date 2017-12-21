using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace USpeedUI
{
    public interface IUIWndView
    {
        // 初始化
        bool Init(IUIWnd wnd);
        // 销毁
        void Destroy();

        // 获取对应的IUIWnd
        IUIWnd GetUIWnd();
        // 获取RectTransform
        RectTransform GetTransform();

        // 是否可见
        bool IsVisible();
        // 设置是否可见
        void SetVisible(bool visible);
    }

    public interface IUIWnd
    {
        // 获取ID
        WndID GetID();
        // 获取资源路径
        String GetPrefabPath();
        // 获取所在层次
        WndLayerDef GetLayer();
        //获取互斥优先级
        WndMutexLevelDef GetMutexLevel();

        // 初始化
        bool Init();
        // 销毁
        void Destroy();

        // 载入窗口资源
        bool LoadView();
        // 卸载窗口资源
        void UnloadView();
        // 获取视图类
        IUIWndView GetWndView();

        int GetLoadMask();
        int GetPermissionMask();
        // 是否可见
        bool IsVisible();
        // 设置是否可见
        void SetVisible(bool visible);
        // 关闭操作处理
        bool OnCloseKey();
        // 是否可以按关闭按钮关闭界面
        bool IsCanCloseKey();
        bool InitFinish();

        // 是否有ESC回调
        bool HaveEscCallBack();

        bool OnEscCallBack();

        // 接受消息
        void OnMessage(WndMsgID msgID, UIMsgData data);

        // 接受按键消息
        bool OnKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status);
    }

    public interface IUISystem
    {
        // 订阅消息
        bool RegisterWndMessage(WndMsgID msgID, IUIWnd wnd);
        // 取消订阅消息
        void UnregisterWndMessage(WndMsgID msgID, IUIWnd wnd);
        // 发送消息
        void SendWndMessage(WndMsgID msgID, UIMsgData data = null);

        // 给指定窗口发消息
        void SendTargetWndMessage(WndID id, WndMsgID msgID, UIMsgData data);

        // 显示/隐藏窗口
        bool ShowWnd(WndID id, bool bShow);
        // 获取窗口显示状态
        bool IsWndVisible(WndID id);
        // 获取窗口接口
        // 珍爱生命，远离毒品，如需强转，后果自负
        IUIWnd GetWnd(WndID id);

        // 订阅按键消息
        bool RegisterKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status, IUIWnd wnd);
        // 取消订阅按键消息
        void UnregisterKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status, IUIWnd wnd);
        // 按键消息
        bool OnKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status);

        // 鼠标是否在UI上
        bool IsMouseOverUI();

        // 取鼠标坐标
        Vector3 GetMousePos(bool _bWordPos = false);

        // 取鼠标所在的UI元素名字
        string GetMouseUIName(bool _bShowViewName = true);

        // 添加窗口物件
        void AddView(IUIWndView view, WndLayerDef layer);

        // 获取Canvas组件
        Canvas GetCanvas();
        // 获取UI摄像机
        Camera GetCamera();
        //更改分辨率
        void SetResolution(int _nNewWidth, int _nNewHeight);
        //取得层级
        RectTransform GetWndLayerTrans(WndLayerDef layer);
        // 是否开启光标显示检测
        bool ToggleCheckShowCursor(bool? _bShow = null);
    }
}
