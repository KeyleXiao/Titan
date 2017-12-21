using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using USpeedUI.SNS;
using DG.Tweening;
using USpeedUI.UExtensions;
using Game_ManagedDef;


namespace USpeedUI
{
    public class UISystem : MonoBehaviour, IUISystem, IResNodeHandle
    {
        /// <summary>
        /// 分辨率改变事件,参数一为新分辨率宽高
        /// </summary>
        public static Action OnResolutionChanged = null;
        static public UISystem Instance = null;
        static public GameObject EventInstance = null;
        static System.Reflection.MethodInfo ClearUIMesh = null;
        // 创建UI系统对象
        static public bool Init()
        {
            ResNode eventSystemPrefab = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/UIEventSystem");
            ResNode uiSystemPrefab = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/UISystem");

            if (eventSystemPrefab == null || null == eventSystemPrefab
                || uiSystemPrefab == null || null == uiSystemPrefab)
                return false;

            GameObject eventInstance = eventSystemPrefab.InstanceMainRes();
            GameObject instance = uiSystemPrefab.InstanceMainRes();

            GameObject uiCameraInstance = new GameObject(UDefines.UICamera);
            uiCameraInstance.layer = LayerMask.NameToLayer(Config.LayerUI);
            uiCameraInstance.AddComponent<Camera>();

            if (!eventInstance || !instance || !uiCameraInstance)
                return false;

            eventInstance.name = eventSystemPrefab.AssetName;
            DontDestroyOnLoad(eventInstance);
            instance.name = uiSystemPrefab.AssetName;
            DontDestroyOnLoad(instance);
            DontDestroyOnLoad(uiCameraInstance);

            Instance = instance.GetComponent<IUISystem>() as UISystem;
            EventInstance = eventInstance;
            ClearUIMesh = typeof(UnityEngine.UI.Graphic).GetMethod("ClearUIMesh", System.Reflection.BindingFlags.Static | System.Reflection.BindingFlags.Public);
            Initialize.OnDeviceStateChage += OnDeviceStageChange;
            return true;
        }

        public void OpenUI()
        {
            if (Instance)
            {
                if (null != ClearUIMesh)
                {
                    ClearUIMesh.Invoke(null, null);
                }
                EventInstance.SetActive(true);
                Instance.gameObject.SetActive(true);
                int cout = (int)WndLayerDef.WND_LAYER_MAX;
                for (int i = 0; i < cout;i++ )
                {
                    if (m_layerTransform[i])
                    {
                        m_layerTransform[i].gameObject.SetActive(true);
                    }
                    
                }

#if ENABLE_YUNYING_MODE
                UTopNameManager.Instance.renderCam.cullingMask = 1 << LayerMask.NameToLayer(Config.LayerUI);
                USpeedUI.Blood.UBloodManager.Instance.renderCam.cullingMask = 1 << LayerMask.NameToLayer(Config.LayerUI); 
#endif
            }
        }

        public void CloseUI()
        {
            if (Instance)
            {
                if (null != ClearUIMesh)
                {
                    ClearUIMesh.Invoke(null, null);
                }
                int cout = (int)WndLayerDef.WND_LAYER_MAX;
                for (int i = 0; i < cout; i++)
                {
                    if (m_layerTransform[i])
                    {
                        m_layerTransform[i].gameObject.SetActive(false);
                    }
                }
                Instance.gameObject.SetActive(false);
                EventInstance.SetActive(false);

#if ENABLE_YUNYING_MODE

                UTopNameManager.Instance.renderCam.cullingMask = 0;
                USpeedUI.Blood.UBloodManager.Instance.renderCam.cullingMask = 0;
#endif
            }
        }

        private static void OnDeviceStageChange(bool deviceState)
        {
            if (!Application.isEditor && Instance)
            {
                if (deviceState)
                {
                    Instance.CloseUI();
                }
                else
                {
                    Instance.OpenUI();
                }
            }

        }

        // canvas对象
        private Canvas m_canvas;
        // raycaster对象
        private GraphicRaycaster m_raycaster;
        // UI用到的摄像机
        private Camera m_uiCamera;
        // 窗口层次
        private RectTransform[] m_layerTransform;

        // 窗口管理
        private Dictionary<WndID, IUIWnd> m_wndList;
        // 窗口事件管理
        private Dictionary<WndMsgID, List<IUIWnd>> m_messageList;
        // 按键事件管理
        private Dictionary<FuntionShortCutSlotIndex, List<IUIWnd>>[] m_keyMessageList;
        //设计分辨率
        public const float ResolutionWidth = 1920f;
        public const float ResolutionHeight = 1080f;

        // 订阅消息
        public bool RegisterWndMessage(WndMsgID msgID, IUIWnd wnd)
        {
            if (m_messageList == null || wnd == null)
                return false;

            List<IUIWnd> list;
            if (m_messageList.TryGetValue(msgID, out list))
            {
                for (int i = 0; i < list.Count; ++i)
                {
                    if (list[i] == wnd)
                        return false;
                }

                list.Add(wnd);
            }
            else
            {
                list = new List<IUIWnd>();
                list.Add(wnd);

                m_messageList[msgID] = list;
            }
            return true;
        }

        // 取消订阅消息
        public void UnregisterWndMessage(WndMsgID msgID, IUIWnd wnd)
        {
            if (m_messageList == null || wnd == null)
                return;

            List<IUIWnd> list;
            if (m_messageList.TryGetValue(msgID, out list))
            {
                list.Remove(wnd);
            }
        }

        // 发送消息
        public void SendWndMessage(WndMsgID msgID, UIMsgData data = null)
        {
            if (m_messageList == null)
                return;

            Initialize.PrintTickCheck("SendWndMessage",_bStart:true);

            List<IUIWnd> list;
            if (m_messageList.TryGetValue(msgID, out list))
            {
                // 先复制一份以防目标在OnMessage中改变订阅list
                IUIWnd[] tempList = new IUIWnd[list.Count];

                list.CopyTo(tempList);

                Initialize.PrintTickCheck("SendWndMessage list.CopyTo");

                for (int i = 0; i < tempList.Length; ++i)
                {

                        tempList[i].OnMessage(msgID, data);
                        Initialize.PrintTickCheck("OnMessage::"+msgID+", wnd::"+ tempList[i].GetID());

                }
            }
        }

        // 给指定窗口发消息
        public void SendTargetWndMessage(WndID id, WndMsgID msgID, UIMsgData data)
        {
            IUIWnd wnd = GetWnd(id);
            if (wnd == null)
                return;
            wnd.OnMessage(msgID, data);
        }

        // 显示/隐藏窗口
        public bool ShowWnd(WndID id, bool bShow)
        {
            IUIWnd wnd = GetWnd(id);
            if (wnd == null)
                return false;

            wnd.SetVisible(bShow);
            return true;
        }

        // 获取窗口显示状态
        public bool IsWndVisible(WndID id)
        {
            IUIWnd wnd = GetWnd(id);
            if (wnd == null)
                return false;

            return wnd.IsVisible();
        }

        // 获取窗口
        public IUIWnd GetWnd(WndID id)
        {
            IUIWnd wnd;
            if (m_wndList.TryGetValue(id, out wnd) == false)
                return null;
            return wnd;
        }

        // 订阅按键消息
        public bool RegisterKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status, IUIWnd wnd)
        {
            if (m_keyMessageList == null || m_keyMessageList[(int)status] == null || wnd == null)
                return false;

            List<IUIWnd> list;
            if (m_keyMessageList[(int)status].TryGetValue(index, out list))
            {
                for (int i = 0; i < list.Count; ++i)
                {
                    if (list[i] == wnd)
                        return false;
                }

                list.Add(wnd);
            }
            else
            {
                list = new List<IUIWnd>();
                list.Add(wnd);

                m_keyMessageList[(int)status][index] = list;
            }
            return true;
        }

        // 取消订阅按键消息
        public void UnregisterKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status, IUIWnd wnd)
        {
            if (m_keyMessageList == null || m_keyMessageList[(int)status] == null || wnd == null)
                return;

            List<IUIWnd> list;
            if (m_keyMessageList[(int)status].TryGetValue(index, out list))
            {
                list.Remove(wnd);
            }
        }

        // 按键消息
        public bool OnKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status)
        {
            if (m_keyMessageList == null)
                return false;

            if (index == FuntionShortCutSlotIndex.Exit && status == KeyStatus.Down)
            {
                OnPressCloseKey();
                return true;
            }

            List<IUIWnd> list;

            if (m_keyMessageList[(int)status].TryGetValue(index, out list))
            {
                // 先复制一份以防目标在OnKeyMessage中改变订阅list
                IUIWnd[] tempList = new IUIWnd[list.Count];
                list.CopyTo(tempList);
                for (int i = 0; i < tempList.Length; ++i)
                {
                    if (tempList[i].OnKeyMessage(index, status))
                        return true;
                }
            }

            return false;
        }

        private void CheckUIKeyMessage()
        {
            if (FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.Exit) && StageManager.Instance.getCurrentState() != Stage_ManagerDef.EStage_State.ESS_LOADSCENE)
            {
                OnKeyMessage(FuntionShortCutSlotIndex.Exit, KeyStatus.Down);
            }

            for (int i = 1; i < (int)KeyStatus.Max; i++)
            {
                foreach (FuntionShortCutSlotIndex index in m_keyMessageList[i].Keys)
                {
                    if (i == (int)KeyStatus.Down)
                    {
                        if (FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(index))
                            OnKeyMessage(index, KeyStatus.Down);
                    }
                    else if (i == (int)KeyStatus.Up)
                    {
                        if (FuntionShortCutCtrl.CheckFunctionShortcutKeyUp(index))
                            OnKeyMessage(index, KeyStatus.Up);
                    }
                    else if (i == (int)KeyStatus.Pressing)
                    {
                        if (FuntionShortCutCtrl.CheckFunctionShortcutKeyPressing(index))
                            OnKeyMessage(index, KeyStatus.Pressing);
                    }
                }
            }
        }

        // 鼠标是否在UI上
        public bool IsMouseOverUI()
        {
            if (!m_canvas || !m_raycaster)
                return false;
            if(!EventSystem.current)
            {
                return false;
            }
            return EventSystem.current.IsPointerOverGameObject();

            PointerEventData eventData = new PointerEventData(EventSystem.current);
            eventData.position = Input.mousePosition;
            eventData.pressPosition = Input.mousePosition;

            List<RaycastResult> list = new List<RaycastResult>();
            m_raycaster.Raycast(eventData, list);
            return list.Count > 0;
        }

        // 添加窗口物件
        public void AddView(IUIWndView view, WndLayerDef layer)
        {
            RectTransform trans = GetWndLayerTrans(layer);
            if (view == null || trans == null)
                return;

            view.GetTransform().SetParent(trans, false);
        }

        // 获取Canvas组件
        public Canvas GetCanvas()
        {
            return m_canvas;
        }

        // 处理按下关闭窗口键
        private void OnPressCloseKey()
        {
            int layer = (int)WndLayerDef.WND_LAYER_MAX - 1;
            for (; layer >= 0; --layer)
            {
                RectTransform layerTrans = m_layerTransform[layer];
                if (layerTrans == null)
                    continue;

                for (int i = layerTrans.childCount - 1; i >= 0; --i)
                {
                    Transform wndViewTrans = layerTrans.GetChild(i);
                    if (wndViewTrans.gameObject.activeSelf)
                    {
                        IUIWndView view = wndViewTrans.GetComponent<IUIWndView>();
                        if (view != null)
                        {
                            IUIWnd wnd = view.GetUIWnd();
                            if(wnd!=null)
                            {
                                if (wnd.HaveEscCallBack())
                                {
                                    wnd.OnEscCallBack();
                                    return;
                                }
                                if (wnd.OnCloseKey())
                                {
                                    wnd.SetVisible(false);
                                    return;
                                }   
                            }                                 
                        }
                    }
                }
            }
            //如果目前没有窗口需要关闭，则打开系统设置界面
            SendWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_SHOW, null);
        }

        // 初始化
        public bool Start()
        {
            m_canvas = GetComponent<Canvas>();
            InitUICamera();
            InitLayer();

            m_raycaster = GetComponent<GraphicRaycaster>();

            m_wndList = new Dictionary<WndID, IUIWnd>();
            m_messageList = new Dictionary<WndMsgID, List<IUIWnd>>();
            m_keyMessageList = new Dictionary<FuntionShortCutSlotIndex, List<IUIWnd>>[(int)KeyStatus.Max];
            for (int i = 0; i < m_keyMessageList.Length; ++i)
                m_keyMessageList[i] = new Dictionary<FuntionShortCutSlotIndex, List<IUIWnd>>();

            if (Instance == null)
            {
                // 单独调试场景,不创建其他窗口
                Instance = this;
                return true;
            }

            //删除载入等待UI
            GameObject waitingloadUI = GameObject.Find("Canvas");
            if (waitingloadUI)
            {
                DestroyImmediate(waitingloadUI, true);
            }
            foreach (IUIWnd wnd in UWindowWnd.CreateWindowWnd())//CreateAllWindow())
            {
                WndID id = wnd.GetID();
                if (m_wndList.ContainsKey(id))
                {
                    Debug.LogError("UISystem 重复初始化窗口, id=" + id);
                    return false;
                }



                if ((wnd.GetLoadMask() & (int)WndLoadMaskDef.WND_LoadMask_All) != 0 && (wnd.GetPermissionMask() & (int)WndPermissionMaskDef.WND_PM_All) != 0)
                {
                    if(!wnd.InitFinish() && wnd.Init() == false)
                    {
                        Debug.LogError("UISystem 窗口初始化失败, id=" + id);
                        return false;
                    }
                }

                m_wndList[id] = wnd;
            }

            #region 字体花屏处理
            Font.textureRebuilt += OnFontTextureRebuilt;

            System.Type fontUpdateTrackerType = typeof(UnityEngine.UI.FontUpdateTracker);
            m_RebuildForFont = fontUpdateTrackerType.GetMethod("RebuildForFont", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Static);
            #endregion


            return true;
        }

        bool isMatchWnd(IUIWnd wnd)
        {
            int loadMask;
            EGameMode gameMode = GameLogicAPI.getCurGameMode();
            switch (gameMode)
            {
                case EGameMode.EGM_Moba:
                    loadMask = (int)WndLoadMaskDef.WND_LoadMask_Moba;
                    break;
                case EGameMode.EGM_MCAMP:
                    loadMask = (int)WndLoadMaskDef.WND_LoadMask_MCAMP;
                    break;
                case EGameMode.EGM_RPG:
                default:
                    loadMask = (int)WndLoadMaskDef.WND_LoadMask_RPG;
                    break;
            }

            int permissionMask = (int)WndPermissionMaskDef.WND_PM_Normal;
            if (StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB)
            {
                permissionMask = (int)WndPermissionMaskDef.WND_PM_OB;
            }

            return (wnd.GetLoadMask() & loadMask) != 0 && (wnd.GetPermissionMask() & permissionMask) != 0;
        }

        public void MatchWnd()
        {
            int loadMask;
            EGameMode gameMode = GameLogicAPI.getCurGameMode();
            switch (gameMode)
            {
                case EGameMode.EGM_Moba:
                    loadMask = (int)WndLoadMaskDef.WND_LoadMask_Moba;
                    break;
                case EGameMode.EGM_MCAMP:
                    loadMask = (int)WndLoadMaskDef.WND_LoadMask_MCAMP;
                    break;
                case EGameMode.EGM_RPG:
                default:
                    loadMask = (int)WndLoadMaskDef.WND_LoadMask_RPG;
                    break;
            }

            int permissionMask = (int)WndPermissionMaskDef.WND_PM_Normal;
            if(StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB)
            {
                permissionMask = (int)WndPermissionMaskDef.WND_PM_OB;
            }

            foreach (IUIWnd wnd in m_wndList.Values)
            {
                if ((wnd.GetLoadMask() & loadMask) != 0 && (wnd.GetPermissionMask() & permissionMask) != 0)
                {
                    if (!wnd.InitFinish())
                        wnd.Init();
                }
                else
                {
                    if (wnd.InitFinish())
                        wnd.Destroy();
                }
            }
        }

        public RectTransform GetWndLayerTrans(WndLayerDef layer)
        {
            int index = (int)layer;
            if (null == m_layerTransform || index < 0 || index >= m_layerTransform.Length)
                return null;

            return m_layerTransform[index];
        }

        private void InitUICamera()
        {
            GameObject uiCameraInstance = GameObject.Find(UDefines.UICamera);
            if (uiCameraInstance == null)
            {
                uiCameraInstance = new GameObject(UDefines.UICamera);
                uiCameraInstance.layer = LayerMask.NameToLayer(Config.LayerUI);
                uiCameraInstance.AddComponent<Camera>();
                DontDestroyOnLoad(uiCameraInstance);
            }

            m_uiCamera = uiCameraInstance.GetComponent<Camera>();
            //这里确保相机用的值没被改变
            m_uiCamera.transform.SetPosition(new Vector3(10000, 10000, 10000));
            m_uiCamera.clearFlags = CameraClearFlags.Depth;
            m_uiCamera.cullingMask = 1 << LayerMask.NameToLayer(Config.LayerUI);
            m_uiCamera.orthographic = true;
            m_uiCamera.orthographicSize = 5;// ResolutionWidth / ResolutionHeight;
            m_uiCamera.depth = 1;//这个相机要在其他相机上面
            m_uiCamera.nearClipPlane = 0;

            m_canvas.renderMode = RenderMode.WorldSpace;//RenderMode.ScreenSpaceCamera;
            m_canvas.worldCamera = m_uiCamera;

            if (m_canvas.renderMode == RenderMode.WorldSpace)
            {
                m_canvas.transform.localScale = new Vector3(0.009259259f, 0.009259259f, 0.009259259f);
                m_canvas.transform.SetPosition(new Vector3(10000, 10000, 10002));
                //m_canvas.transform.SetPosition(m_uiCamera.transform.position);
                (m_canvas.transform as RectTransform).pivot = new Vector2(0.5f, 0.5f);
                SetResolution(ImageSetting.GetCurrentResolution().width, ImageSetting.GetCurrentResolution().height);
                //m_uiCamera.orthographicSize = 1080 / 2;
                //m_uiCamera.farClipPlane = 20000;
            }

        }

        private void InitLayer()
        {
            m_layerTransform = new RectTransform[(int)WndLayerDef.WND_LAYER_MAX];
            m_layerTransform[(int)WndLayerDef.WND_LAYER_FIXED] = (RectTransform)transform.GetChild(0);
            m_layerTransform[(int)WndLayerDef.WND_LAYER_POPUP01] = (RectTransform)transform.GetChild(1);
            m_layerTransform[(int)WndLayerDef.WND_LAYER_FILL] = (RectTransform)transform.GetChild(2);
            m_layerTransform[(int)WndLayerDef.WND_LAYER_POPUP02] = (RectTransform)transform.GetChild(3);
            m_layerTransform[(int)WndLayerDef.WND_LAYER_OVERHEAD] = (RectTransform)transform.GetChild(4);


            m_canvas.sortingLayerName = USpeedUILayerManager.CalculateLayerName(WndLayerDef.WND_LAYER_MAX);
            m_canvas.sortingOrder = USpeedUILayerManager.CalculateLayerOrder(WndLayerDef.WND_LAYER_MAX);

            SetupUILayer(WndLayerDef.WND_LAYER_FIXED);
            SetupUILayer(WndLayerDef.WND_LAYER_POPUP01);
            SetupUILayer(WndLayerDef.WND_LAYER_FILL);
            SetupUILayer(WndLayerDef.WND_LAYER_POPUP02);
            SetupUILayer(WndLayerDef.WND_LAYER_OVERHEAD);

        }
        private void SetupUILayer(WndLayerDef _nLayerDef)
        {
            USpeedUILayer tmpUILayer = null;
            tmpUILayer = m_layerTransform[(int)_nLayerDef].GetComponent<USpeedUILayer>();
            if (tmpUILayer == null)
                tmpUILayer = m_layerTransform[(int)_nLayerDef].gameObject.AddComponent<USpeedUILayer>();

            tmpUILayer.UILayerDef = _nLayerDef;
            tmpUILayer.RaycasterLayerMask = 1 << LayerMask.NameToLayer(Config.LayerUI);

        }

        //设置屏蔽技能音效，根据当前是否有需要屏蔽的界面打开判断
        public void Forbid3DSound(IUIWnd wnd, bool bForbid)
        {
            int index = m_forbid3dSoundWndList.IndexOf(wnd);
            if (bForbid)
            {
                if (index < 0)
                    m_forbid3dSoundWndList.Add(wnd);
            }
            else
            {
                if (index > -1)
                    m_forbid3dSoundWndList.RemoveAt(index);

                //如果表里面还有，那就renturn，不能解除屏蔽
                if (m_forbid3dSoundWndList.Count > 0)
                    return;
            }
            SoundManager.Forbid3DSound(bForbid);
        }

        //当前打开的需要屏蔽技能音效的界面列表
        private List<IUIWnd> m_forbid3dSoundWndList = new List<IUIWnd>();

        // 获取UI相机
        public Camera GetCamera()
        {
            return m_uiCamera;
        }

        public Vector3 GetMousePos(bool _bWordPos = false)
        {
            Vector3 res = InputManager.MousePosition;

            if (_bWordPos)
            {
                if (GetCanvas().renderMode == RenderMode.ScreenSpaceOverlay)
                    res = Camera.main.ScreenToWorldPoint(res);
                else if (GetCanvas().renderMode == RenderMode.ScreenSpaceCamera)
                    res = GetCamera().ScreenToWorldPoint(res);
                else
                    Trace.LogWarning("目前没有计算非Canvas渲染模式为ScreenSpaceOverlay或ScreenSpaceCamera 模式下的鼠标坐标");
            }

            return res;
        }

        public string GetMouseUIName(bool _bShowViewName = true)
        {
            string strRes = string.Empty;

            UPointerEventDataEx eventData = EventSystem.current.CurrentPointerEventData();
            if (eventData != null && eventData.pointerCurrentRaycast.isValid)
            {
                strRes = eventData.pointerCurrentRaycast.gameObject.name;
                Transform tmpTrans = eventData.pointerCurrentRaycast.gameObject.transform;
                if (_bShowViewName)
                {
                    List<Transform> tmpTransList = new List<Transform>();
                    while (tmpTrans != null)
                    {
                        tmpTransList.Add(tmpTrans);
                        tmpTrans = tmpTrans.parent;
                    }
                    strRes = tmpTransList[tmpTransList.Count - 3].name;
                }
            }

            return strRes;
        }
        int OldResWidth;
        int OldResHeight;
        public void Update()
        {
            //编辑器下侦听窗口大小改变
            if (Application.isEditor)
            {
                if ((Screen.width != OldResWidth) || (Screen.height != OldResHeight))
                {
                    OldResWidth = Screen.width;
                    OldResHeight = Screen.height;
                    SetResolution(OldResWidth, OldResHeight);
                }
            }

            if (SoldierCamera.MainInstance() != null && SoldierCamera.MainInstance().cameraMode == CameraMode.FPSMode)
            {
                CheckShowCursor();
                CursorLockMode newMode = SoldierCamera.MainInstance<SoldierCamera>().bRotateWithMouse ? CursorLockMode.Locked : CursorLockMode.None;

                if(newMode != Cursor.lockState)
                {
                    Cursor.lockState = newMode;

                    //if(SoldierCamera.MainInstance<SoldierCamera>().bFPSRotate)
                    //{
                    //    EntityFactory.MainHero.GetComponent<ScreenRaycast>().SetDefaultSelection(); 
                    //}
                    //else
                    //{
                    //    EntityFactory.MainHero.GetComponent<ScreenRaycast>().SetNPCTargetSelection();
                    //}
                }
            }

            CheckUIKeyMessage();
        }


        public void LateUpdate()
        {
            #region 字体花屏处理
            if (null == m_RebuildForFont)
            {
                return;
            }

            for (int i = 0; i < m_FontUpdateList.Count; i++)
            {
                FontUpdateNode node = m_FontUpdateList[i];
                if (node.NeedUpdate)
                {
                    Font font = node.font;
                    m_RebuildForFont.Invoke(null, new object[] { font });

                    // Log rebuild.
                    Texture fontTexture = font.material.mainTexture;
                    Trace.Log(string.Format("动态字体： \"{0}\" 的纹理大小更新为 {1}x{2}.", font.name, fontTexture.width, fontTexture.height));

                    node.Invalidate();
                }
            }
            #endregion
        }

        public void OnDestroy()
        {
            Font.textureRebuilt -= OnFontTextureRebuilt;
        }

        public void SetResolution(int _nNewWidth, int _nNewHeight)
        {
            if (m_canvas != null && m_canvas.renderMode == RenderMode.WorldSpace)
            {
                int width = _nNewWidth * UDefines.USpeed_Screen_Height / _nNewHeight;

                (m_canvas.transform as RectTransform).sizeDelta = new Vector2(width, UDefines.USpeed_Screen_Height);//new Vector2(Screen.width*1080/Screen.height, 1080);
            }

            GameUtil.StartCoroutineHelper(this,DelayOnResolutionChanged());

        }


        public bool ToggleCheckShowCursor = false;
        public void CheckShowCursor()
        {
            //有泉水buff时一直显示
            {
                if(GameLogicAPI.IsExistBuff(1044))
                {
                    SoldierCamera.MainInstance<SoldierCamera>().bFPSShowCursor = true;
                    return;
                }
            }

            //死亡下一直显示
            {
                if(EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.StateMachine != null && EntityFactory.MainHeroView.StateMachine.GetState() == GameLogic.EntityState.Dead)
                {
                    SoldierCamera.MainInstance<SoldierCamera>().bFPSShowCursor = true;
                    return;
                }
            }

            //没死亡
            {
                bool containUI = false;

                for (WndLayerDef i = WndLayerDef.WND_LAYER_POPUP02; i >= WndLayerDef.WND_LAYER_POPUP01; --i)
                {
                    containUI = CheckShowCursorByWNDLayer(i);
                    if (containUI)
                        break;
                }


                if (containUI && !SoldierCamera.MainInstance<SoldierCamera>().bFPSShowCursor)
                {
                    SoldierCamera.MainInstance<SoldierCamera>().bFPSShowCursor = true;
                    return;
                }
                else if (!containUI && SoldierCamera.MainInstance<SoldierCamera>().bFPSShowCursor)
                {
                    SoldierCamera.MainInstance<SoldierCamera>().bFPSShowCursor = false;
                    return;
                }
            }
            //Cursor.lockState = (SoldierCamera.MainInstance<SoldierCamera>().bFPSShowCursor ? CursorLockMode.None : CursorLockMode.Locked);

        }

        private List<IUIWnd> m_CommonBlurEffectWndList = new List<IUIWnd>();

        /// <summary>
        /// 激活通用模糊背景效果
        /// </summary>
        /// <param name="wnd"></param>
        /// <param name="bActive"></param>
        public void ActiveCommonBlurEffect(IUIWnd wnd, bool bActive)
        {
            int index = m_CommonBlurEffectWndList.IndexOf(wnd);
            if (bActive)
            {
                if (index < 0)
                    m_CommonBlurEffectWndList.Add(wnd);
                UISystem.Instance.ShowWnd(WndID.WMD_ID_COMMONBLURBG, true);
            }
            else
            {
                if (index > -1)
                    m_CommonBlurEffectWndList.RemoveAt(index);

                if (m_CommonBlurEffectWndList.Count > 0)
                {
                    int result = m_CommonBlurEffectWndList.FindIndex(x => x != null);
                    if (result > -1)
                        return;
                }
                UISystem.Instance.ShowWnd(WndID.WMD_ID_COMMONBLURBG, false);
            }
        }

        /// <summary>
        /// 高级别互斥界面是否有显示
        /// </summary>
        /// <param name="_eJudgeWndLayer">需要判断的层</param>
        /// <returns></returns>
        public bool IsWndLayerHasHighMutexWndShow(WndLayerDef _eJudgeWndLayer)
        {
            IUIWnd tmpUIWnd = null;

            foreach (var wndID in Enum.GetValues(typeof(WndID)))
            {
                tmpUIWnd = GetWnd((WndID)wndID);
                if (tmpUIWnd != null && tmpUIWnd.GetLayer() == _eJudgeWndLayer && tmpUIWnd.IsVisible() && tmpUIWnd.GetMutexLevel() > WndMutexLevelDef.WND_Mutex_Normal)
                {
                    return true;
                }
            }
            return false;
        }

        protected bool CheckShowCursorByWNDLayer(WndLayerDef _eLayer)
        {

            RectTransform layerTrans = m_layerTransform[(int)_eLayer];
            if (layerTrans == null)
                return false;

            for (int i = layerTrans.childCount - 1; i >= 0; --i)
            {
                Transform wndViewTrans = layerTrans.GetChild(i);
                if (wndViewTrans.gameObject.activeSelf)
                {
                    IUIWndView view = wndViewTrans.GetComponent<IUIWndView>();
                    if (view != null)
                    {
                        IUIWnd wnd = view.GetUIWnd();
                        if (wnd.IsCanCloseKey() || _eLayer == WndLayerDef.WND_LAYER_FILL)
                        {
                            return true;
                        }
                    }
                }
            }
            return false;
        }

        bool IUISystem.ToggleCheckShowCursor(bool? _bShow)
        {

            ToggleCheckShowCursor = (bool)_bShow;
             
           return ToggleCheckShowCursor;

        }

        /// <summary>
        /// 设置某互斥界面单独显示
        /// </summary>
        /// <param name="_wndView">界面</param>
        /// <param name="_eWndLayerNotVisible">处理层</param>
        internal void SetMutexWndVisbleAlone(IUIWndView _wndView, params WndLayerDef[] _eWndLayerNotVisible)
        {
            foreach(WndLayerDef layerDef in _eWndLayerNotVisible)
            {
                RectTransform LayerTrans = GetWndLayerTrans(layerDef);
                if (LayerTrans != null)
                {
                    int count = LayerTrans.childCount;
                    for (int i = 0; i < count; ++i)
                    {
                        IUIWndView wndView = LayerTrans.GetChild(i).GetComponent<IUIWndView>();
                        if (wndView != null && _wndView != null && wndView != _wndView && wndView.GetUIWnd().GetMutexLevel() > WndMutexLevelDef.WND_Mutex_None)
                        {
                            if(_wndView.GetUIWnd().GetMutexLevel() >= wndView.GetUIWnd().GetMutexLevel())
                            {
                                if (wndView.IsVisible())
                                    wndView.GetUIWnd().SetVisible(false);
                            }
                        }
                    }
                }
            }
        }

        /// <summary>
        /// 某一层设为显示，其余层则设为隐藏
        /// </summary>
        /// <param name="_eWndLayerVisible">需要显示的层</param>
        /// <param name="_eWndLayerNotVisible">需要隐藏的层</param>
        internal void SetWndLayerVisible_Not(WndLayerDef _eWndLayerVisible,params WndLayerDef[] _eWndLayerNotVisible)
        {
            bool bHasFillViewVisible = false;

            bHasFillViewVisible = IsWndLayerHasVisibleWnd(_eWndLayerVisible);

            foreach (WndLayerDef e in _eWndLayerNotVisible)
            {
                if(e != WndLayerDef.WND_LAYER_NONE && e != WndLayerDef.WND_LAYER_MAX)
                    GetWndLayerTrans(e).gameObject.SetActive(!bHasFillViewVisible);
            }
        }
        /// <summary>
        /// _wnd界面当WND_LAYER_FILL有显示中的界面时，将更改_wnd的层为_eNewWndLayer
        /// </summary>
        /// <param name="_wnd">需要更改的界面</param>
        /// <param name="_eNewWndLayer">更改的层</param>
        internal void SetWndRenderAfterTheFillLayer(IUIWndView _wnd,WndLayerDef _eNewWndLayer)
        {
            //当Fill层有界面显示着的时候
            if (UISystem.Instance != null)
            {
                bool bHasFillView = false;

                bHasFillView = IsWndLayerHasVisibleWnd(WndLayerDef.WND_LAYER_FILL);

                //将view提升至Popup02层
                if (bHasFillView)
                {
                    AddView(_wnd, _eNewWndLayer);
                }
                else
                {
                    ;//m_WndRenderAfterFillLayerVisible.Add(_wnd.GetUIWnd());
                }
                
            }
        }
        /// <summary>
        /// 当全屏界面有显示时，将那些比全屏界面先设为显示的界面重新显示出来，因为全屏界面会将比他低的层隐藏，当这个界面在显示全屏界面是需要显示
        /// </summary>
        internal void SetWndRenderAfterFillLayerVisible()
        {
            if(m_WndRenderAfterFillLayerVisible.Count > 0)
            {
                for (int i = m_WndRenderAfterFillLayerVisible.Count - 1; i >= 0; --i) 
                {
                    m_WndRenderAfterFillLayerVisible[i].SetVisible(true);
                    m_WndRenderAfterFillLayerVisible.RemoveAt(i);
                }
            }
        }
        /// <summary>
        /// 将该界面不需要在全屏界面显示时显示出来
        /// </summary>
        /// <param name="_wnd"></param>
        internal void RemoveWndRenderAfterFillLayerVisible(IUIWnd _wnd)
        {
            if(m_WndRenderAfterFillLayerVisible.Count > 0)
            {
                foreach(IUIWnd wnd in m_WndRenderAfterFillLayerVisible)
                {
                    if(wnd.GetID() == _wnd.GetID())
                    {
                        m_WndRenderAfterFillLayerVisible.Remove(_wnd);
                        return;
                    }
                }
            }
        }
        /// <summary>
        /// 存储比全屏界面先处理显示，而无法提升层级的界面，待全屏界面显示后再执行他们的显示
        /// </summary>
        protected List<IUIWnd> m_WndRenderAfterFillLayerVisible = new List<IUIWnd>();

        private bool IsWndLayerHasVisibleWnd(WndLayerDef _eJudgeWndLayer)
        {
            IUIWnd tmpUIWnd = null;

            foreach (var wndID in Enum.GetValues(typeof(WndID)))
            {
                tmpUIWnd = GetWnd((WndID)wndID);
                if (tmpUIWnd != null && tmpUIWnd.GetLayer() == _eJudgeWndLayer && tmpUIWnd.IsVisible())
                {
                    return true;
                }
            }
            return false;
        }

        IEnumerator DelayOnResolutionChanged()
        {
            //延迟一针执行，因为部分属性不在当前帧完成
            yield return null;

            if (OnResolutionChanged != null)
            {
                OnResolutionChanged();//(new Vector2(_nNewWidth, _nNewHeight));
            }
            //yield return new WaitForSeconds(1);
        }


        #region 字体花屏处理
        private class FontUpdateNode
        {
            private bool m_FontTextureRebuilt = false;
            private Font m_FontRebuilt = null;

            public FontUpdateNode(Font font)
            {
                m_FontRebuilt = font;
                Validate();
            }

            public void Validate()
            {
                if (null == m_FontRebuilt)
                {
                    m_FontTextureRebuilt = false;
                    return;
                }

                m_FontTextureRebuilt = true;
            }

            public void Invalidate()
            {
                m_FontTextureRebuilt = false;
            }

            public bool NeedUpdate
            {
                get { return m_FontTextureRebuilt && (null != m_FontRebuilt); }
            }

            public Font font
            {
                get { return m_FontRebuilt; }
            }
        }

        private System.Reflection.MethodInfo m_RebuildForFont = null;
        private List<FontUpdateNode> m_FontUpdateList = new List<FontUpdateNode>();

        /// <summary>
        /// 字体纹理重绘事件
        /// </summary>
        /// <param name="font"></param>
        private void OnFontTextureRebuilt(Font font)
        {
            bool findThisFont = false;
            for (int i = 0; i < m_FontUpdateList.Count; i++)
            {
                FontUpdateNode node = m_FontUpdateList[i];
                if (node.font == font)
                {
                    node.Validate();
                    findThisFont = true;
                    break;
                }
            }

            if (!findThisFont)
            {
                m_FontUpdateList.Add(new FontUpdateNode(font));
            }
        }
        #endregion

        #region UI系统内加载资源

        public ResNode GetResNode(string _strPath)
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

        public bool ReleaseResNode(ResNode _resNode,bool _bImmediate)
        {
           return AssetBundleManager.ReleaseResNode(this, _resNode, _bImmediate);
        }
        #endregion
    }

    public  class UCanvasHelper: UWidgets.UCustomUIBehaviour
    {
        public int AbsoluteDepth;
        public int RelativeDepth;
        public int RenderOrder;
        public int CanvasRenderOrder;
        public int SortingOrder;
        public int SiblingIndex;

        protected override void OnEnable()
        {
            base.OnEnable();
            if (GetComponent<Graphic>() != null)
            {
                AbsoluteDepth = GetComponent<Graphic>().canvasRenderer.absoluteDepth;
                RelativeDepth = GetComponent<Graphic>().canvasRenderer.relativeDepth;
                RenderOrder = GetComponent<Graphic>().canvas.renderOrder;
                SortingOrder = GetComponent<Graphic>().canvas.sortingOrder;
            }
            SiblingIndex = transform.GetSiblingIndex();
            if(GetComponent<Canvas>() != null)
            {
                CanvasRenderOrder = GetComponent<Canvas>().renderOrder;
            }           
        }

        protected override void OnDisable()
        {
            base.OnDisable();
            if (GetComponent<Graphic>() != null)
            {
                AbsoluteDepth = GetComponent<Graphic>().canvasRenderer.absoluteDepth;
                RelativeDepth = GetComponent<Graphic>().canvasRenderer.relativeDepth;
                RenderOrder = GetComponent<Graphic>().canvas.renderOrder;
                SortingOrder = GetComponent<Graphic>().canvas.sortingOrder;
            }
            SiblingIndex = transform.GetSiblingIndex();
            if (GetComponent<Canvas>() != null)
            {
                CanvasRenderOrder = GetComponent<Canvas>().renderOrder;
            }
        }

        protected override void OnTransformParentChanged()
        {
            base.OnTransformParentChanged();
            if (GetComponent<Graphic>() != null)
            {
                AbsoluteDepth = GetComponent<Graphic>().canvasRenderer.absoluteDepth;
                RelativeDepth = GetComponent<Graphic>().canvasRenderer.relativeDepth;
                RenderOrder = GetComponent<Graphic>().canvas.renderOrder;
                SortingOrder = GetComponent<Graphic>().canvas.sortingOrder;
            }
            SiblingIndex = transform.GetSiblingIndex();
            if (GetComponent<Canvas>() != null)
            {
                CanvasRenderOrder = GetComponent<Canvas>().renderOrder;
            }
        }
    }
   public class UTestView : UIBaseWndView
    {

        public override void Start()
        {
            base.Start();

        }
        public override void Update()
        {

        }
    }
}
