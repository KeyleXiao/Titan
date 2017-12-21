using System;
using System.Diagnostics;
using System.Collections;
using MessageLibrary;
using SharedMemory;

using UnityEngine;
using Debug = UnityEngine.Debug;
using Object = System.Object;

namespace WebBrowser
{
    public class BrowserEngine
    {
        /// <summary>
        /// 接收浏览器纹理数据
        /// </summary>
        private SharedArray<byte> _mainTexArray;
        /// <summary>
        /// 接收浏览器发来的数据
        /// </summary>
        private SharedCommServer _inCommServer;
        /// <summary>
        /// 发送给浏览器的数据
        /// </summary>
        private SharedCommServer _outCommServer;
        /// <summary>
        /// 浏览器进程
        /// </summary>
        private Process _pluginProcess;

        private static Object sPixelLock;
        /// <summary>
        /// 网页纹理
        /// </summary>
        public Texture2D BrowserTexture = null;
        public bool Initialized = false;

        /// <summary>
        /// 网页初始化后是否需要执行JS初始化语句
        /// </summary>
        private bool _needToRunOnce = false;
        private string _runOnceJS = "";

        /// <summary>
        /// 网页纹理数据
        /// </summary>
        private byte[] _bufferBytes = null;
        /// <summary>
        /// 是否已连接浏览器
        /// </summary>
        private bool _connected = false;

        #region Settings
        /// <summary>
        /// 浏览器窗口宽度
        /// </summary>
        public int kWidth = 512;
        /// <summary>
        /// 浏览器窗口高度
        /// </summary>
        public int kHeight = 512;
        /// <summary>
        /// 网页纹理数据 内存区域名字
        /// </summary>
        private string _sharedFileName;

        /// <summary>
        /// 接收浏览器发来的数据 内存区域名字
        /// </summary>
        private string _inCommFile;
        /// <summary>
        /// 发送给浏览器的数据 内存区域名字
        /// </summary>
        private string _outCommFile;
        /// <summary>
        /// 网页初始地址
        /// </summary>
        private string _initialURL;
        /// <summary>
        /// 是否使用网页实时通讯功能
        /// </summary>
        private bool _enableWebRTC;
        /// <summary>
        /// 是否使用网页GPU加速
        /// </summary>
        private bool _enableGPU;

        #endregion

        #region PageLoaded
        public delegate void PageLoaded(string url);
        /// <summary>
        /// 网页加载完成事件
        /// </summary>
        public event PageLoaded OnPageLoaded;
        #endregion

        #region Dialogs
        public delegate void JavaScriptDialog(string message, string prompt, DialogEventType type);
        /// <summary>
        /// 网页显示对话框事件
        /// </summary>
        public event JavaScriptDialog OnJavaScriptDialog;
        #endregion

        #region JSQuery

        public delegate void JavaScriptQuery(string message);
        /// <summary>
        /// 接收到网页返回数据事件
        /// </summary>
        public event JavaScriptQuery OnJavaScriptQuery;

        #endregion

        #region Init

        public IEnumerator InitPlugin(int width, int height, string sharedfilename, string initialURL, bool enableWebRTC, bool enableGPU)
        {
            ///浏览器exe位置，目前没分32/64位
#if UNITY_EDITOR_64
                        string PluginServerPath = Application.dataPath + @"\WebBrowser\PluginServer\x64";
#else
#if UNITY_EDITOR_32
            string indexStr = "Game";
            string PluginServerPath = string.Empty;
            if (Application.isEditor)
            {
                PluginServerPath = Application.dataPath.Remove(Application.dataPath.LastIndexOf(indexStr)) + @"Bin/PluginServer";
            }
            else
            {
                PluginServerPath = Application.streamingAssetsPath;
                PluginServerPath = PluginServerPath.Replace("/StreamingAssets", string.Empty);
                string[] temp = PluginServerPath.Split('/');
                PluginServerPath = PluginServerPath.Replace(temp[temp.Length - 1], string.Empty);
                PluginServerPath = PluginServerPath.Remove(PluginServerPath.Length - 1, 1);
                PluginServerPath += "/PluginServer";
            }


#else


                    //HACK
                    string AssemblyPath=System.Reflection.Assembly.GetExecutingAssembly().Location;
                    //log this for error handling
                    Debug.Log("Assembly path:"+AssemblyPath);

                    AssemblyPath = Path.GetDirectoryName(AssemblyPath); //Managed
      
                    AssemblyPath = Directory.GetParent(AssemblyPath).FullName; //<project>_Data
                    AssemblyPath = Directory.GetParent(AssemblyPath).FullName;//required

                    string PluginServerPath=AssemblyPath+@"\PluginServer";
#endif
#endif

            Debug.Log("Starting server from:" + PluginServerPath);

            kWidth = width;
            kHeight = height;



            _sharedFileName = sharedfilename;

            //randoms
            Guid inID = Guid.NewGuid();
            _outCommFile = inID.ToString();

            Guid outID = Guid.NewGuid();
            _inCommFile = outID.ToString();

            _initialURL = initialURL;
            _enableWebRTC = enableWebRTC;
            _enableGPU = enableGPU;

            if (BrowserTexture == null)
                BrowserTexture = new Texture2D(kWidth, kHeight, TextureFormat.BGRA32, false, true);

            sPixelLock = new object();


            string args = BuildParamsString();

            _connected = false;

            _inCommServer = new SharedCommServer(false);
            _outCommServer = new SharedCommServer(true);


            //尝试启动浏览器次数，超过则关闭
            int tryOpenProcessCount = 10;
            int curOpenProcessCount = 0;
            while (!_connected)
            {
                try
                {
                    if (_pluginProcess != null)
                        _pluginProcess.CloseMainWindow();

                    _pluginProcess = new Process()
                    {
                        StartInfo = new ProcessStartInfo()
                        {
                            WorkingDirectory = PluginServerPath,
                            FileName = PluginServerPath + @"\SharedPluginServer.exe",
                            Arguments = args

                        }
                    };

                    _pluginProcess.Start();
                    Initialized = false;
                    curOpenProcessCount++;
                }
                catch (Exception e)
                {
                    //log the file
                    Debug.Log("FAILED TO START SERVER FROM:" + PluginServerPath + @"\SharedPluginServer.exe" + ",errorCode:" + e.Message);
                    //throw;
                }
                yield return new WaitForSeconds(2.0f);
                //connected = ConnectTcp(out _clientSocket);

                _inCommServer.Connect(_inCommFile);
                bool b1 = _inCommServer.GetIsOpen();
                _outCommServer.Connect(_outCommFile);
                bool b2 = _outCommServer.GetIsOpen();
                _connected = b1 && b2;

                if (!_connected && curOpenProcessCount > tryOpenProcessCount)
                {
                    if (_pluginProcess != null)
                        _pluginProcess.CloseMainWindow();

                    Debug.LogWarning("启动Web服务器失败。");
                    break;
                }
            }
        }

        /// <summary>
        /// 构建浏览器启动参数
        /// </summary>
        /// <returns></returns>
        private string BuildParamsString()
        {
            string ret = kWidth.ToString() + " " + kHeight.ToString() + " ";
            ret = ret + _initialURL + " ";
            ret = ret + _sharedFileName + " ";
            ret = ret + _outCommFile + " ";
            ret = ret + _inCommFile + " ";

            if (_enableWebRTC)
                ret = ret + " 1" + " ";
            else
                ret = ret + " 0" + " ";

            if (_enableGPU)
                ret = ret + " 1" + " ";
            else
                ret = ret + " 0" + " ";

            return ret;
        }

        public delegate void BrowserEngineConnected(string message);
        /// <summary>
        /// 首次连接成功时会发出
        /// </summary>
        public event BrowserEngineConnected OnBrowserEngineConnected;
        #endregion

        #region SendEvents
        /// <summary>
        /// 发给浏览器前进/后退事件
        /// </summary>
        /// <param name="url"></param>
        /// <param name="back"></param>
        /// <param name="forward"></param>
        public void SendNavigateEvent(string url, bool back, bool forward)
        {
            if (Initialized)
            {
                GenericEvent ge = new GenericEvent()
                {
                    Type = GenericEventType.Navigate,
                    GenericType = MessageLibrary.BrowserEventType.Generic,
                    NavigateUrl = url
                };

                if (back)
                    ge.Type = GenericEventType.GoBack;
                else if (forward)
                    ge.Type = GenericEventType.GoForward;

                EventPacket ep = new EventPacket()
                {
                    Event = ge,
                    Type = MessageLibrary.BrowserEventType.Generic
                };

                /*MemoryStream mstr = new MemoryStream();
                BinaryFormatter bf = new BinaryFormatter();
                bf.Serialize(mstr, ep);
                byte[] b = mstr.GetBuffer();
                _outCommServer.WriteBytes(b);*/
                _outCommServer.WriteMessage(ep);
            }
        }
        /// <summary>
        /// 发给浏览器关闭事件
        /// </summary>
        public void SendShutdownEvent()
        {
            if (Initialized)
            {
                GenericEvent ge = new GenericEvent()
                {
                    Type = GenericEventType.Shutdown,
                    GenericType = MessageLibrary.BrowserEventType.Generic
                };

                EventPacket ep = new EventPacket()
                {
                    Event = ge,
                    Type = MessageLibrary.BrowserEventType.Generic
                };

                _outCommServer.WriteMessage(ep);
            }
        }
        /// <summary>
        /// 没啥用
        /// </summary>
        public void PushMessages()
        {
            if (Initialized)
                _outCommServer.PushMessages();
        }
        /// <summary>
        /// 发送对话框回答
        /// </summary>
        /// <param name="ok">成功/失败</param>
        /// <param name="dinput">返回给浏览器数据</param>
        public void SendDialogResponse(bool ok, string dinput)
        {
            if (Initialized)
            {
                DialogEvent de = new DialogEvent()
                {
                    GenericType = MessageLibrary.BrowserEventType.Dialog,
                    success = ok,
                    input = dinput
                };

                EventPacket ep = new EventPacket
                {
                    Event = de,
                    Type = MessageLibrary.BrowserEventType.Dialog
                };

                _outCommServer.WriteMessage(ep);
            }

        }
        /// <summary>
        /// 发送JS方法回答
        /// </summary>
        /// <param name="response"></param>
        public void SendQueryResponse(string response)
        {
            if (Initialized)
            {
                GenericEvent ge = new GenericEvent()
                {
                    Type = GenericEventType.JSQueryResponse,
                    GenericType = BrowserEventType.Generic,
                    JsQueryResponse = response
                };

                EventPacket ep = new EventPacket()
                {
                    Event = ge,
                    Type = BrowserEventType.Generic
                };

                _outCommServer.WriteMessage(ep);
            }
        }
        /// <summary>
        /// 发送键盘字符数据
        /// </summary>
        /// <param name="character"></param>
        /// <param name="type"></param>
        public void SendCharEvent(int character, KeyboardEventType type)
        {
            if (Initialized)
            {
                KeyboardEvent keyboardEvent = new KeyboardEvent()
                {
                    Type = type,
                    Key = character
                };
                EventPacket ep = new EventPacket()
                {
                    Event = keyboardEvent,
                    Type = MessageLibrary.BrowserEventType.Keyboard
                };

                _outCommServer.WriteMessage(ep);
            }
        }
        /// <summary>
        /// 发送鼠标数据
        /// </summary>
        /// <param name="msg"></param>
        public void SendMouseEvent(MouseMessage msg)
        {
            if (Initialized)
            {
                EventPacket ep = new EventPacket
                {
                    Event = msg,
                    Type = MessageLibrary.BrowserEventType.Mouse
                };

                _outCommServer.WriteMessage(ep);
            }

        }
        /// <summary>
        /// 发送需执行的JS方法
        /// </summary>
        /// <param name="js"></param>
        public void SendExecuteJSEvent(string js)
        {
            if (Initialized)
            {
                GenericEvent ge = new GenericEvent()
                {
                    Type = GenericEventType.ExecuteJS,
                    GenericType = BrowserEventType.Generic,
                    JsCode = js
                };

                EventPacket ep = new EventPacket()
                {
                    Event = ge,
                    Type = BrowserEventType.Generic
                };

                _outCommServer.WriteMessage(ep);
            }

        }
        /// <summary>
        /// 与浏览器保持连接,因为一定时间没操作会关闭浏览器,为了省资源
        /// </summary>
        public void SendPing()
        {
            if (Initialized)
            {
                GenericEvent ge = new GenericEvent()
                {
                    Type = GenericEventType.Navigate, //could be any
                    GenericType = BrowserEventType.Ping,

                };

                EventPacket ep = new EventPacket()
                {
                    Event = ge,
                    Type = BrowserEventType.Ping
                };

                _outCommServer.WriteMessage(ep);
            }
        }



        #endregion

        #region Helpers

        /// <summary>
        /// 调用JS初始化一些网页配置之类的
        /// </summary>
        /// <param name="js">JS code</param>
        public void RunJSOnce(string js)
        {
            _needToRunOnce = true;
            _runOnceJS = js;
        }

        #endregion

        /// <summary>
        /// 更新浏览器纹理数据
        /// </summary>
        public void UpdateTexture()
        {

            if (Initialized)
            {
                UpdateInitialized();

                //execute run-once functions
                if (_needToRunOnce)
                {
                    SendExecuteJSEvent(_runOnceJS);
                    _needToRunOnce = false;
                }
            }
            else
            {
                if (_connected)
                {
                    //Thread.Sleep(200); //give it some time to initialize
                    try
                    {
                        //init memory file
                        _mainTexArray = new SharedArray<byte>(_sharedFileName);

                        Initialized = true;

                        if (OnBrowserEngineConnected != null)
                            OnBrowserEngineConnected.Invoke("这个只是预留的，暂时没啥连接消息");
                    }
                    catch (Exception ex)
                    {
                        //SharedMem and TCP exceptions
                        Debug.Log("Exception on init:" + ex.Message + ".Waiting for plugin server");
                    }

                }


            }
        }

        /// <summary>
        /// 接收浏览器数据
        /// </summary>
        public void CheckMessage()
        {

            if (Initialized)
            {
                try
                {
                    // Ensure that no other threads try to use the stream at the same time.
                    EventPacket ep = _inCommServer.GetMessage();

                    if (ep != null)
                    {
                        //main handlers
                        if (ep.Type == BrowserEventType.Dialog)
                        {
                            DialogEvent dev = ep.Event as DialogEvent;
                            if (dev != null)
                            {
                                if (OnJavaScriptDialog != null)
                                    OnJavaScriptDialog(dev.Message, dev.DefaultPrompt, dev.Type);
                            }
                        }
                        if (ep.Type == BrowserEventType.Generic)
                        {
                            GenericEvent ge = ep.Event as GenericEvent;
                            if (ge != null)
                            {
                                if (ge.Type == GenericEventType.JSQuery)
                                {
                                    if (OnJavaScriptQuery != null)
                                        OnJavaScriptQuery(ge.JsQuery);
                                }
                            }

                            if (ge.Type == GenericEventType.PageLoaded)
                            {
                                if (OnPageLoaded != null)
                                    OnPageLoaded(ge.NavigateUrl);
                            }
                        }
                    }

                }
                catch //(Exception e)
                {
                    Debug.Log("Error reading from socket,waiting for plugin server to start...");
                }
            }
        }

        /// <summary>
        /// 关闭浏览器
        /// </summary>
        public void Shutdown()
        {
            SendShutdownEvent();

            Initialized = false;
            _connected = false;

            _mainTexArray = null;
            _inCommServer = null;
            _outCommServer = null;
            _pluginProcess = null;
            sPixelLock = null;
        }

        //////////Added//////////
        public void UpdateInitialized()
        {
            if (Initialized)
            {
                SendPing();

                if (_bufferBytes == null)
                {
                    long arraySize = _mainTexArray.Length;
                    Debug.Log("Memory array size:" + arraySize);
                    _bufferBytes = new byte[arraySize];
                }
                _mainTexArray.CopyTo(_bufferBytes, 0);

                lock (sPixelLock)
                {

                    BrowserTexture.LoadRawTextureData(_bufferBytes);
                    BrowserTexture.Apply();

                }
            }
        }
    }
}
