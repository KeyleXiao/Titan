using System;
using UnityEngine;
using System.Collections;
using MessageLibrary;
using UnityEngine.EventSystems;
using UnityEngine.UI;

namespace WebBrowser
{
    public class WebBrowser2D : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler, IPointerDownHandler,
        IPointerUpHandler
    {

        #region General

        [Header("General settings")]
        public int Width = 1024;

        public int Height = 768;

        public string MemoryFile = "MainSharedMem";

        public bool RandomMemoryFile = true;

        public string InitialURL = "http://www.q1.com";

        public bool EnableWebRTC = false;

        [Header("Testing")]
        public bool EnableGPU = false;

        [Multiline]
        public string JSInitializationCode = "";

        #endregion


        [Header("2D setup")]
        [SerializeField]
        public RawImage Browser2D = null;
               
        private BrowserEngine _mainEngine;

        #region Initialization

        //why Unity does not store the links in package?
        protected virtual void InitPrefabLinks()
        {
            if (Browser2D == null)
                Browser2D = gameObject.GetComponent<RawImage>();
        }

        public virtual void Awake()
        {

        }

        public virtual void Start()
        {
            _mainEngine = new BrowserEngine();

            if (RandomMemoryFile)
            {
                Guid memid = Guid.NewGuid();
                MemoryFile = memid.ToString();
            }

            StartCoroutine(_mainEngine.InitPlugin(Width, Height, MemoryFile, InitialURL, EnableWebRTC, EnableGPU));
            //run initialization
            if (JSInitializationCode.Trim() != "")
                _mainEngine.RunJSOnce(JSInitializationCode);

            InitPrefabLinks();

            Browser2D.texture = _mainEngine.BrowserTexture;
            Browser2D.uvRect = new Rect(0f, 0f, 1f, -1f);


            //attach dialogs and querys
            _mainEngine.OnJavaScriptDialog += _mainEngine_OnJavaScriptDialog;
            _mainEngine.OnJavaScriptQuery += _mainEngine_OnJavaScriptQuery;
            _mainEngine.OnPageLoaded += _mainEngine_OnPageLoaded;
            _mainEngine.OnBrowserEngineConnected += _mainEngine_OnBrowserEngineConnected;
        }

        // Update is called once per frame
        public virtual void Update()
        {
            if (_mainEngine == null)
                return;

            _mainEngine.UpdateTexture();
            _mainEngine.CheckMessage();

            #region 2D mouse

            if (Browser2D != null)
            {
                //GetScreenCoords(true);
            }


            #endregion

            //Connected
            if(_bConnected)
            {
                if (OnEngineConnected != null)
                    OnEngineConnected(_strConnectedInfo);
                _bConnected = false;
            }

            //Dialog
            if (_showDialog)
            {
                if (OnDialogShow != null)
                    OnDialogShow((int)_dialogEventType, _dialogMessage, _dialogPrompt);
                _showDialog = false;
            }

            //Query
            if (_startQuery)
            {                
                if (OnJSQuery != null)
                    OnJSQuery(_jsQueryString);
                _startQuery = false;
            }

            //Status
            if (_setUrl)
            {                
                if(OnPageLoaded != null)
                {
                    OnPageLoaded(_setUrlString);
                }
                _setUrl = false;
            }



            if (_focused && !IsURLFieldFocus) //keys
            {
                foreach (char c in Input.inputString)
                {
                    _mainEngine.SendCharEvent((int)c, KeyboardEventType.CharKey);
                }
                ProcessKeyEvents();
            }

        }

        public virtual void OnDestroy()
        {
            if (_mainEngine == null)
                return;

            _mainEngine.Shutdown();
            _mainEngine.OnJavaScriptDialog -= _mainEngine_OnJavaScriptDialog;
            _mainEngine.OnJavaScriptQuery -= _mainEngine_OnJavaScriptQuery;
            _mainEngine.OnPageLoaded -= _mainEngine_OnPageLoaded;
            _mainEngine.OnBrowserEngineConnected -= _mainEngine_OnBrowserEngineConnected;

            if(_mainEngine.BrowserTexture != null)
            {
                Destroy(_mainEngine.BrowserTexture);
                _mainEngine.BrowserTexture = null;
            }
        }

        private bool _bConnected = false;
        private string _strConnectedInfo = string.Empty;
        public delegate void BrowserEngineConnected(string query);
        /// <summary>
        /// 连接成功时会发出
        /// </summary>
        public event BrowserEngineConnected OnEngineConnected;
            
        private void _mainEngine_OnBrowserEngineConnected(string _strMessage)
        {
            _bConnected = true;
            _strConnectedInfo = _strMessage;
        }
        #endregion

        #region PageLoaded
        //status - threading 
        private bool _setUrl = false;
        private string _setUrlString = "";
        private void _mainEngine_OnPageLoaded(string url)
        {
            _setUrl = true;
            _setUrlString = url;
        }
        #endregion

        #region JS Query events
        //query - threading
        private bool _startQuery = false;
        private string _jsQueryString = "";


        public delegate void JSQuery(string query);
        /// <summary>
        /// 接收网页查询到的JS方法调用事件
        /// </summary>
        public event JSQuery OnJSQuery;


        /// <summary>
        /// 接收查询到的JS方法参数
        /// </summary>
        /// <param name="message"></param>
        private void _mainEngine_OnJavaScriptQuery(string message)
        {
            _jsQueryString = message;
            _startQuery = true;
        }
        /// <summary>
        /// 发送查询到的JS方法回调
        /// </summary>
        /// <param name="response"></param>
        public void RespondToJSQuery(string response)
        {
            if (_mainEngine == null)
                return;

            _mainEngine.SendQueryResponse(response);
        }

        #endregion                

        #region JS Dialogs
        //dialog states - threading
        private bool _showDialog = false;
        private string _dialogMessage = "";
        private string _dialogPrompt = "";
        private DialogEventType _dialogEventType;


        //make it thread-safe

        /// <summary>
        /// 接收对话框参数
        /// </summary>
        /// <param name="message"></param>
        /// <param name="prompt"></param>
        /// <param name="type"></param>
        private void _mainEngine_OnJavaScriptDialog(string message, string prompt, DialogEventType type)
        {
            _showDialog = true;
            _dialogEventType = type;
            _dialogMessage = message;
            _dialogPrompt = prompt;
        }

        public delegate void DialogShow(int _nDialogType,string _strDialogMessage,string _strDialogPrompt);
        /// <summary>
        /// 接收网页查询到的JS方法调用事件
        /// </summary>
        public event DialogShow OnDialogShow;

        /// <summary>
        /// 对话框类型按键响应
        /// </summary>
        /// <param name="result"></param>
        public void DialogResult(bool result)
        {
            if (_mainEngine == null)
                return;

            //DialogPanel.SetActive(false);
            _mainEngine.SendDialogResponse(result, DialogPromptResult);
        }
        /// <summary>
        /// 对话框提示返回给浏览器的
        /// </summary>
        public virtual string DialogPromptResult { get; set; }


        /// <summary>
        /// 执行JS语句
        /// </summary>
        /// <param name="js"></param>
        public void RunJavaScript(string js)
        {
            _mainEngine.SendExecuteJSEvent(js);
        }

        #endregion

        #region UI

        public delegate void PageLoaded(string _strLoadedURL);
        /// <summary>
        /// 网页加载完成事件
        /// </summary>
        public event PageLoaded OnPageLoaded;

        /// <summary>
        /// 焦点是否在地址栏输入区
        /// </summary>
        public virtual bool IsURLFieldFocus { get; set; }

        /// <summary>
        /// 发送地址栏输入完毕事件给浏览器
        /// </summary>
        public void OnNavigate(string _strUrl)
        {
            if (_mainEngine == null)
                return;

            // MainUrlInput.isFocused
            _mainEngine.SendNavigateEvent(_strUrl, false, false);
        }

        /// <summary>
        /// 发送前进/后退事件给浏览器
        /// </summary>
        /// <param name="forward"></param>
        public void GoBackForward(bool forward)
        {
            if (_mainEngine == null)
                return;

            if (forward)
                _mainEngine.SendNavigateEvent(string.Empty, false, true);
            else
                _mainEngine.SendNavigateEvent(string.Empty, true, false);
        }

        #endregion

        #region Mouse Control 

        private bool _focused = false;


        private int posX = 0;
        private int posY = 0;


        public virtual void OnPointerEnter(PointerEventData data)
        {
            _focused = true;
            //mainUIPanel.Show();
            StartCoroutine("TrackPointer");
        }

        public virtual void OnPointerExit(PointerEventData data)
        {
            _focused = false;
            //mainUIPanel.Hide();
            StopCoroutine("TrackPointer");
        }

        /// <summary>
        /// 追踪鼠标位置
        /// </summary>
        /// <returns></returns>
        IEnumerator TrackPointer()
        {
            var _raycaster = GetComponentInParent<GraphicRaycaster>();
            var _input = FindObjectOfType<StandaloneInputModule>();

            if (_raycaster != null && _input != null && _mainEngine.Initialized)
            {
                while (Application.isPlaying)
                {
                    Vector2 localPos = GetScreenCoords(_raycaster, _input);

                    int px = (int)localPos.x;
                    int py = (int)localPos.y;

                    ProcessScrollInput(px, py);

                    if (posX != px || posY != py)
                    {
                        MouseMessage msg = new MouseMessage
                        {
                            Type = MouseEventType.Move,
                            X = px,
                            Y = py,
                            GenericType = MessageLibrary.BrowserEventType.Mouse,
                            // Delta = e.Delta,
                            Button = MouseButton.None
                        };

                        if (Input.GetMouseButton(0))
                            msg.Button = MouseButton.Left;
                        if (Input.GetMouseButton(1))
                            msg.Button = MouseButton.Right;
                        if (Input.GetMouseButton(1))
                            msg.Button = MouseButton.Middle;

                        posX = px;
                        posY = py;
                        _mainEngine.SendMouseEvent(msg);
                    }

                    yield return 0;
                }
            }
            //  else
            //      UnityEngine.Debug.LogWarning("Could not find GraphicRaycaster and/or StandaloneInputModule");
        }

        public void OnPointerDown(PointerEventData data)
        {

            if (_mainEngine != null && _mainEngine.Initialized)
            {
                var _raycaster = GetComponentInParent<GraphicRaycaster>();
                var _input = FindObjectOfType<StandaloneInputModule>();
                Vector2 pixelUV = GetScreenCoords(_raycaster, _input);

                switch (data.button)
                {
                    case PointerEventData.InputButton.Left:
                        {
                            SendMouseButtonEvent((int)pixelUV.x, (int)pixelUV.y, MouseButton.Left,
                                MouseEventType.ButtonDown);
                            break;
                        }
                    case PointerEventData.InputButton.Right:
                        {
                            SendMouseButtonEvent((int)pixelUV.x, (int)pixelUV.y, MouseButton.Right,
                                MouseEventType.ButtonDown);
                            break;
                        }
                    case PointerEventData.InputButton.Middle:
                        {
                            SendMouseButtonEvent((int)pixelUV.x, (int)pixelUV.y, MouseButton.Middle,
                                MouseEventType.ButtonDown);
                            break;
                        }
                }
            }

        }

        public void OnPointerUp(PointerEventData data)
        {

            if (_mainEngine != null && _mainEngine.Initialized)
            {
                var _raycaster = GetComponentInParent<GraphicRaycaster>();
                var _input = FindObjectOfType<StandaloneInputModule>();

                Vector2 pixelUV = GetScreenCoords(_raycaster, _input);

                switch (data.button)
                {
                    case PointerEventData.InputButton.Left:
                        {
                            SendMouseButtonEvent((int)pixelUV.x, (int)pixelUV.y, MouseButton.Left, MouseEventType.ButtonUp);
                            break;
                        }
                    case PointerEventData.InputButton.Right:
                        {
                            SendMouseButtonEvent((int)pixelUV.x, (int)pixelUV.y, MouseButton.Right,
                                MouseEventType.ButtonUp);
                            break;
                        }
                    case PointerEventData.InputButton.Middle:
                        {
                            SendMouseButtonEvent((int)pixelUV.x, (int)pixelUV.y, MouseButton.Middle,
                                MouseEventType.ButtonUp);
                            break;
                        }
                }


            }

        }

        /// <summary>
        /// 屏幕坐标转为网页显示范围坐标
        /// </summary>
        /// <param name="ray"></param>
        /// <param name="input"></param>
        /// <returns></returns>
        private Vector2 GetScreenCoords(GraphicRaycaster ray, StandaloneInputModule input)
        {

            Vector2 localPos; // Mouse position  
            RectTransformUtility.ScreenPointToLocalPointInRectangle(transform as RectTransform, Input.mousePosition,
                ray.eventCamera, out localPos);

            // local pos is the mouse position.
            RectTransform trns = transform as RectTransform;
            localPos.y = trns.rect.height - localPos.y;
            //Debug.Log("x:"+localPos.x+",y:"+localPos.y);

            //now recalculate to texture
            localPos.x = (localPos.x * Width) / trns.rect.width;
            localPos.y = (localPos.y * Height) / trns.rect.height;

            return localPos;

        }

        private void ProcessScrollInput(int px, int py)
        {
            if (_mainEngine == null)
                return;

            float scroll = Input.GetAxis("Mouse ScrollWheel");

            scroll = scroll * _mainEngine.BrowserTexture.height;

            int scInt = (int)scroll;

            if (scInt != 0)
            {
                MouseMessage msg = new MouseMessage
                {
                    Type = MouseEventType.Wheel,
                    X = px,
                    Y = py,
                    GenericType = MessageLibrary.BrowserEventType.Mouse,
                    Delta = scInt,
                    Button = MouseButton.None
                };

                if (Input.GetMouseButton(0))
                    msg.Button = MouseButton.Left;
                if (Input.GetMouseButton(1))
                    msg.Button = MouseButton.Right;
                if (Input.GetMouseButton(1))
                    msg.Button = MouseButton.Middle;

                _mainEngine.SendMouseEvent(msg);
            }
        }

        /// <summary>
        /// 发送鼠标数据给浏览器
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="btn"></param>
        /// <param name="type"></param>
        private void SendMouseButtonEvent(int x, int y, MouseButton btn, MouseEventType type)
        {
            if (_mainEngine == null)
                return;

            MouseMessage msg = new MouseMessage
            {
                Type = type,
                X = x,
                Y = y,
                GenericType = MessageLibrary.BrowserEventType.Mouse,
                // Delta = e.Delta,
                Button = btn
            };
            _mainEngine.SendMouseEvent(msg);
        }
        #endregion

        #region Keys

        private void ProcessKeyEvents()
        {
            foreach (KeyCode k in Enum.GetValues(typeof(KeyCode)))
            {
                CheckKey(k);
            }

        }

        private void CheckKey(KeyCode code)
        {
            if (_mainEngine == null)
                return;

            if (Input.GetKeyDown(code))
                _mainEngine.SendCharEvent((int)code, KeyboardEventType.Down);
            if (Input.GetKeyUp(KeyCode.Backspace))
                _mainEngine.SendCharEvent((int)code, KeyboardEventType.Up);
        }
        
        #endregion
    }

}
