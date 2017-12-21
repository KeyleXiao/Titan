using UnityEngine;
using System.Collections;
using U3D_Render;
using GameLogic;
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
/// <summary>
/// 性能统计信息
/// </summary>
public class PrefamceInfo : MonoBehaviour {

    enum EPreframeArea
    {
        EPA_System,
        EPA_Entity,
        EPA_Max
    }
    bool bShow = false;
    bool[] bAreaVisibleArr = new bool[(int)EPreframeArea.EPA_Max];

    float deltaTime = 0.0f;
    float fixedDelatTime = 0.0f;
    private int bDebugScreenRayCastLayer = 0;
    private EntityView Debug_CurrentSelectEntity = null;
    GUIStyle style = new GUIStyle();

    public static PrefamceInfo Instance = null;
    private int curUid = 0;
    private string str_pa = "-";
    private string str_hp = "-";
    private string str_pd = "-";
    private string str_md = "-";
    private bool DrawAssetDebugInfo = false;
    public void Start()
    {
        InitEntityInfo();
        
        style.alignment = TextAnchor.UpperLeft;
        style.fontSize = 16;
        style.normal.textColor = Color.white;

        Instance = this;
    }
    public void Update()
    {

        if (Input.GetKey(KeyCode.Keypad0) && Input.GetKey(KeyCode.PageDown) && Input.GetKeyUp(KeyCode.KeypadEnter))
        {
            Initialize.isDebugMode = true;
        }
#if ENABLE_YUNYING_MODE
        if (Input.GetKeyUp(KeyCode.F8))
        {
            OptimitzedControlPlane.Instance.EanableUI = !OptimitzedControlPlane.Instance.EanableUI;
            if (OptimitzedControlPlane.Instance.EanableUI)
            {
                USpeedUI.UISystem.Instance.OpenUI();
            }
            else
            {
                USpeedUI.UISystem.Instance.CloseUI();
            }
        }
#endif
        if (Input.GetKeyUp(KeyCode.F11))
        {
            
            bShow = !bShow;
            ResetState();

            if (!bShow)
            {
                // 关闭指定实体属性值
                GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_ENTITY_PROPERTY_DRAGONBALL_CLOSE, 0, "", IntPtr.Zero, 0);
            }
            else
            {
                if (curUid == 0)
                    return;
                    
                cmd_Entity_Property_Prefamce_Req data = new cmd_Entity_Property_Prefamce_Req();
                data.uidReq = (uint)curUid;
                IntPtrHelper helper = new IntPtrHelper();
                IntPtr ptr = helper.toPtr<cmd_Entity_Property_Prefamce_Req>(ref data);
                int len = Marshal.SizeOf(data);
                GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_ENTITY_PROPERTY_DRAGONBALL, len, "", ptr, len);
            }
        }
        deltaTime += (Time.deltaTime - deltaTime) * 0.1f;
    }

    public void FixedUpdate()
    {
        fixedDelatTime += (Time.fixedDeltaTime - fixedDelatTime) * 0.1f;
    }

    public void OnGUIInterial()
    {
        if (!Application.isEditor && !Debug.isDebugBuild && !Initialize.isDebugMode)
        {
            return;
        }

        if (!bShow)
        {
            CloseEntityRequest();
            return;
        }
        int w = Screen.width, h = Screen.height;

        if (!DrawAssetDebugInfo)
        {
            DebugSystemInfo(w, h);

            DebugEntityInfo(w, h);

            DrawOptimtizedControlPlane();

            DrawRenderControlPlane();

            if (EntityFactory.Instance)
            {
                EntityFactory.Instance.OnGUIInterial();
            }
        }
        else
        {
            GUILayout.BeginArea(new Rect(0, 0, Screen.width, Screen.height));
            if(GUILayout.Button("返回"))
            {
                DrawAssetDebugInfo = false;
            }
            AssetBundleManager.DrawDebugInfo();
            GUILayout.EndArea();
        }

       
    }

    void ResetState()
    {
        for(int i = 0;i<bAreaVisibleArr.Length;i++)
        {
            bAreaVisibleArr[i] = true;
        }
    }
    void SetAreaVisible(EPreframeArea ePA,bool bVisible)
    {
        if (ePA == EPreframeArea.EPA_Max)
            return;
        bAreaVisibleArr[(int)ePA] = bVisible;

        foreach(var b in bAreaVisibleArr)
        {
            if (b)
                return;     
        }
        this.bShow = false;

        // 关闭指定实体属性值
        GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_ENTITY_PROPERTY_DRAGONBALL_CLOSE, 0, "", IntPtr.Zero, 0);
    }
    bool CheckVisible(EPreframeArea ePa)
    {
        return bAreaVisibleArr[(int)ePa];
    }
    void DebugMessageTemplate(EPreframeArea _ePa,string _strText,Rect _rMessageRect)
    {
        GUILayout.BeginArea(new Rect(_rMessageRect.x, _rMessageRect.y, _rMessageRect.width, _rMessageRect.height));

        GUILayout.BeginVertical("Box");
        if (GUILayout.Button("关闭"))
        {
            SetAreaVisible(_ePa, false);
        }
        GUILayout.Label(_strText, style);

        GUILayout.EndVertical();

        GUILayout.EndArea();

    }

    #region 打印系统&网络信息
    private void DebugSystemInfo(int _nScreenWidth,int _nScreenHeight)
    {
        if (!CheckVisible(EPreframeArea.EPA_System))
            return;
        string text = MainThread() + PhysicThread() + OperatingSystem() + GraphicsMemorySize() +
              SystemMemorySize() + GraphicsDeviceName() + ProcessorType() +
              NetLatency() + PendingCount() + EffectCount() + HitEffectCount() + CurMapID() + NativeMemory();

        DebugMessageTemplate(EPreframeArea.EPA_System,text,new Rect(10,10,(int)(_nScreenWidth*0.5f),_nScreenHeight));
    }
    private string MainThread()
    {
        float msec = deltaTime * 1000.0f;
        float fps = 1.0f / deltaTime;
        return string.Format("主线程：{0:0.0} ms ({1:0.} fps)\n",msec,fps);
    }
    private string PhysicThread()
    {
        float fixedmsec = fixedDelatTime * 1000.0f;
        float FixedFps = 1.0f / fixedDelatTime;
        return string.Format("物理线程：{0:0.0} ms ({1:0.} fps\n", fixedmsec, FixedFps);
    }
    private string OperatingSystem()
    {
        return ("系统："+SystemInfo.operatingSystem+"\n");
    }
    private string GraphicsMemorySize()
    {
        return ("显存：" + SystemInfo.graphicsMemorySize + "MB\n");
    }
    private string SystemMemorySize()
    {
        return ("内存：" + SystemInfo.systemMemorySize + "MB\n");
    }
    private string GraphicsDeviceName()
    {
        return ("显卡：" + SystemInfo.graphicsDeviceName + "\n");
    }
    private string ProcessorType()
    {
        return ("CPU：" + SystemInfo.processorType + "\n");
    }
    private string NetLatency()
    {
        return ("网络延迟：" + GameLogicAPI.getNetLatency()+"\n");
    }
    private string PendingCount()
    {
        return ("网络包：" + GameLogicAPI.getPendingCount() + "\n");
    }

    private string EffectCount()
    {
        return ("光效数量：" + Effect.EffectNode.AllEffectCount + "\n");
    }

    private string HitEffectCount()
    {
        return ("受击光效数量：" + LightingEffectManager.HitEffectCount + "\n");
    }

    private string NativeMemory()
    {
        return string.Empty;// ("Native内存：" + GameLogicAPI.GetNativeMemorySizeInMB().ToString("F4") + "\n");
    }

    private string CurMapID()
    {
        return ("当前地图：" + EntityUtil.getMainHeroCurMapID() + "\n");
    }
    
    #endregion

    #region 打印实体信息
    private void InitEntityInfo()
    {
        bDebugScreenRayCastLayer = 0;
        bDebugScreenRayCastLayer = 1 << LayerMask.NameToLayer(Config.LayerScreenRaycastEnemy);
        bDebugScreenRayCastLayer = bDebugScreenRayCastLayer | 1 << LayerMask.NameToLayer(Config.LayerHero);
        bDebugScreenRayCastLayer = bDebugScreenRayCastLayer | 1 << LayerMask.NameToLayer(Config.LayerScreenRaycastFriend);
        bDebugScreenRayCastLayer = bDebugScreenRayCastLayer | 1 << LayerMask.NameToLayer(Config.LayerMonsterEnemy);
        bDebugScreenRayCastLayer = bDebugScreenRayCastLayer | 1 << LayerMask.NameToLayer(Config.LayerMonsterFriend);
        bDebugScreenRayCastLayer = bDebugScreenRayCastLayer | 1 << LayerMask.NameToLayer(Config.LayerNeutrality);
        bDebugScreenRayCastLayer = bDebugScreenRayCastLayer | 1 << LayerMask.NameToLayer(Config.LayerPlayerEnemy);
        bDebugScreenRayCastLayer = bDebugScreenRayCastLayer | 1 << LayerMask.NameToLayer(Config.LayerPlayerFriend);
    }
    private void DebugEntityInfo(int _nScreenWidth,int _nScreenHeight)
    {
        if (!CheckVisible(EPreframeArea.EPA_Entity))
            return;

        if (InputManager.GetMouseButtonUp(0))
        {
            RaycastHit hit;
            Ray ray = Initialize.mainCam.ScreenPointToRay(InputManager.MousePosition);
            if (Physics.Raycast(ray, out hit, 100, bDebugScreenRayCastLayer))
            {
                BaseStateMachine bs = hit.transform.GetComponentInParent<BaseStateMachine>();
                if(bs)
                {
                    Debug_CurrentSelectEntity = EntityFactory.getEntityViewByID(bs.entityID);

                    // 请求指定实体的属性值
                    if (curUid == bs.entityID)
                        return;

                    curUid = bs.entityID;
                    cmd_Entity_Property_Prefamce_Req data = new cmd_Entity_Property_Prefamce_Req();
                    data.uidReq = (uint)curUid;
                    IntPtrHelper helper = new IntPtrHelper();
                    IntPtr ptr = helper.toPtr<cmd_Entity_Property_Prefamce_Req>(ref data);
                    int len = Marshal.SizeOf(data);
                    GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_ENTITY_PROPERTY_DRAGONBALL, len, "", ptr, len);
                }

            }
        }
        string str_id = "-";
        string str_name = "-";
        string str_clientpos = "-";
        string str_serverpos = "-";
        string str_state = "-";
        string Gravity = "";
        string SyncVectory = "";
        string SyncPosition = "";
        string SkinID = "";
        string strSkinInstance = "";
        string VisibleState = "";
        string MaskValue = "";
        string LayerName = "";
        if (null != Debug_CurrentSelectEntity)
        {
            if (Debug_CurrentSelectEntity.StateMachine)
            {
                //打开请求服务器位置开关
                Debug_CurrentSelectEntity.StateMachine.requestServerPos = true;
                str_id = Debug_CurrentSelectEntity.ID.ToString();
                if (null != Debug_CurrentSelectEntity.Property)
                {
                    str_id += "[" + Debug_CurrentSelectEntity.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID).ToString() + "]";
                }
                str_name = Debug_CurrentSelectEntity.Property.CreatureName;
                str_clientpos = Debug_CurrentSelectEntity.StateMachine.creature.position.ToString();
                str_state = Debug_CurrentSelectEntity.StateMachine.GetState().ToString();
                str_serverpos = Debug_CurrentSelectEntity.StateMachine.ServerPos.ToString();
                CreatureStateMachine csm = Debug_CurrentSelectEntity.StateMachine as CreatureStateMachine;
                if (csm)
                {
                    SyncVectory = csm.syncVelocity.ToString();
                    SyncPosition = csm.syncPosition.ToString();
                }
              
                Gravity = Debug_CurrentSelectEntity.StateMachine.currentGravity.ToString();
                VisibleState = "OnView:\n" + Debug_CurrentSelectEntity.StateMachine.m_OnViewVisible + "\n" + "MaskVisible:" + Debug_CurrentSelectEntity.StateMachine.m_MaskVisible;
                SkinID = SkinID = Debug_CurrentSelectEntity.StateMachine.SkinControl.currentSkinId.ToString();
                strSkinInstance = "No SkinInstance";
                if(Debug_CurrentSelectEntity.StateMachine.SkinControl.CurrentSkinInstance != null)
                {
                    strSkinInstance = Debug_CurrentSelectEntity.StateMachine.SkinControl.CurrentSkinInstance.nSkinID.ToString();
                }

                MaskValue = Debug_CurrentSelectEntity.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK).ToString();
                LayerName = LayerMask.LayerToName(Debug_CurrentSelectEntity.gameObject.layer);
            }

        }
        string EntityMessageText;
        EntityMessageText = EntityFormatText("实体调试信息:", " ") + EntityFormatText("ID:", str_id) + EntityFormatText("Name:", str_name) +
                             EntityFormatText("ClientPos:", str_clientpos) + EntityFormatText("ServerPos:", str_serverpos) + EntityFormatText("CurrentState:" , str_state) +
                             EntityFormatText("攻击力:", str_pa) + EntityFormatText("生命值:", str_hp) + EntityFormatText("护甲值:", str_pd) +
                             EntityFormatText("魔抗值:", str_md) + EntityFormatText("重力速度:", Gravity) + EntityFormatText("SyncVectory:", SyncVectory) + EntityFormatText("SyncPosition:", SyncPosition) +
                             EntityFormatText("SkinID:", SkinID) + EntityFormatText("SkinInstance:", strSkinInstance) + EntityFormatText("VisibleState:", VisibleState) + EntityFormatText("MaskValue:", MaskValue)
                             + EntityFormatText("LayerName:", LayerName);

        DebugMessageTemplate(EPreframeArea.EPA_Entity, EntityMessageText, new Rect(_nScreenWidth - _nScreenWidth * 0.2f - 10, 10, _nScreenWidth * 0.2f, _nScreenHeight));

    }
    private void CloseEntityRequest()
    {
        if (null != Debug_CurrentSelectEntity)
        {
            if (Debug_CurrentSelectEntity.StateMachine)
            {
                //关闭请求服务器位置开关
                if (Debug_CurrentSelectEntity.StateMachine.requestServerPos)
                {
                    Debug_CurrentSelectEntity.StateMachine.requestServerPos = false;
                }
            }
        }
    }
    private string EntityFormatText(string _strDes,string _strValue)
    {
        return (_strDes + _strValue + "\n");
    }
    public void UpdateEntityProperty(IntPtr ptrParam, int nPtrLen)
    {
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        cmd_Entity_Property_Prefamce_Return heroDetailInfo = helper.get<cmd_Entity_Property_Prefamce_Return>();

        if (Debug_CurrentSelectEntity.ID == heroDetailInfo.uidReq)
        {
            str_pa = heroDetailInfo.nPa.ToString();
            str_hp = heroDetailInfo.nHp.ToString();
            str_pd = heroDetailInfo.nPd.ToString();
            str_md = heroDetailInfo.nMd.ToString();
        }
    }

    #endregion

    #region 优化面板

    private Vector2 OptimtizedControlScrollPos = Vector2.zero;
    private Vector2 RenderControllScrollPos = Vector2.zero;
    private void DrawRenderControlPlane()
    {
        Rect rt = new Rect(210, Screen.height * 0.6f, 250, 200);
        Rect rt1 = new Rect(0, 0, 250, Screen.height );
        RenderControllScrollPos = GUI.BeginScrollView(rt, RenderControllScrollPos, rt1);

        int layerIndex = 0;
        GUI.Label(new Rect(0, 30 * layerIndex, 200, 20), "渲染面板");
        layerIndex++;
        GUI.Label(new Rect(0, 30 * layerIndex, 200, 20), "当前后处理列表:");

        List<ImageEffects.ImageEffectNode> list = ImageEffects.ImageEffectManager.API_GetImangeEffectList(Initialize.mainCam);
        if(null != list)
        {
            layerIndex++;
            GUI.Label(new Rect(0, 30 * layerIndex, 200, 20), "CameraRenderingPath:" + Initialize.mainCam.renderingPath);
            foreach(ImageEffects.ImageEffectNode nod in list)
            {
                layerIndex++;
                nod.enabled = GUI.Toggle(new Rect(0,  30 * layerIndex, 200, 20),nod.enabled, nod.GetType().Name);
            }
        }
        
        GUI.EndScrollView();
    }

    private void DrawOptimtizedControlPlane()
    {
        Rect rt = new Rect(0, Screen.height * 0.6f, 200,200);
        Rect rt1 = new Rect(0, 0, 200, Screen.height);
        OptimtizedControlScrollPos = GUI.BeginScrollView(rt, OptimtizedControlScrollPos, rt1);

        GUILayout.BeginVertical("Box");
        GUILayout.Label("调试面板");

        GUILayout.Space(5.0f);

        OptimitzedControlPlane.Instance.EnabledNormalFloatFont = GUILayout.Toggle(OptimitzedControlPlane.Instance.EnabledNormalFloatFont, "开启普通飘字");
        OptimitzedControlPlane.Instance.EnabledLightingEffect = GUILayout.Toggle(OptimitzedControlPlane.Instance.EnabledLightingEffect, "开启光效");
        LightingEffectFactory.ClosedOptimiezed = GUILayout.Toggle(LightingEffectFactory.ClosedOptimiezed, "关闭光效优化");

        //if (GUILayout.Button("UI调试面板"))
        //{
        //    UIWindowDebugPlane.OpenUIWindowDebugPlane();
        //}

        if (GUILayout.Button("打印光效信息"))
        {
            Trace.Log("-------------------EffectDebugInfo------------------------");
            foreach (string s in EntityViewCommandHandler.lightingEffectDebugInfo)
            {
                Trace.Log(s);
            }
            Trace.Log("-------------------EffectDebugInfo------------------------");

            SkinManager.TraceCullingList();
            System.Diagnostics.Process.Start(Initialize.LogFileName);


        }

        if (GUILayout.Button("刷新场景碰撞体"))
        {
            SceneResInfoManager.RefershAllCollider();
        }

        if (GUILayout.Button("刷新人物碰撞体"))
        {
            EntityFactory.RefershPlayerCollider();
        }

        if (SceneResInfoManager.ShowMeshColliderFlags)
        {
            if (GUILayout.Button("关闭碰撞体显示"))
            {
                SceneResInfoManager.ShowMeshColliderFlags = false;
                SceneResInfoManager.HideMeshCollider();
            }
        }
        else
        {
            if (GUILayout.Button("打开碰撞体显示"))
            {
                SceneResInfoManager.ShowMeshColliderFlags = true;
                SceneResInfoManager.ShowMeshCollider();
            }
        }

        if (!OptimitzedControlPlane.Instance.EnabledImageEffects)
        {
            if (GUILayout.Button("开启后处理"))
            {
                OptimitzedControlPlane.Instance.EnabledImageEffects = true;
                foreach (ImageEffects.ImageEffectManager ins in ImageEffects.ImageEffectManager.ImageEffectManagerTable.Values)
                {
                    if (ins)
                    {
                        ins.enabled = true;
                    }
                }
            }
        }
        else
        {
            if (GUILayout.Button("关闭后处理"))
            {
                OptimitzedControlPlane.Instance.EnabledImageEffects = false;
                foreach (ImageEffects.ImageEffectManager ins in ImageEffects.ImageEffectManager.ImageEffectManagerTable.Values)
                {
                    if (ins)
                    {
                        ins.enabled = false;
                    }
                }
            }
        }


        if (GUILayout.Button("输出pickCount信息"))
        {
            Trace.Log("--------------pickcount Debug info:-----------");

            ICollection<U3D_Render.EntityView> heros = EntityFactory.GetPlayerList();
            foreach (U3D_Render.EntityView hero in heros)
            {
                if (hero.StateMachine)
                {
                    Trace.Log(hero.StateMachine.name + " " + hero.StateMachine.pickableCount);
                }
            }

            System.Diagnostics.Process.Start(Initialize.LogFileName);
        }
        if (!InputManager.foceOpenInput)
        {
            if (GUILayout.Button("强制开启输入"))
            {
                InputManager.foceOpenInput = true;
            }
        }
        else
        {
            if (GUILayout.Button("强制关闭输入"))
            {
                InputManager.foceOpenInput = false;
            }
        }

        if (!OptimitzedControlPlane.Instance.EanableUI)
        {
            if (GUILayout.Button("开启UI"))
            {
                OptimitzedControlPlane.Instance.EanableUI = true;
                USpeedUI.UISystem.Instance.OpenUI();
            }
        }
        else
        {
            if (GUILayout.Button("关闭UI"))
            {
                OptimitzedControlPlane.Instance.EanableUI = false;
                USpeedUI.UISystem.Instance.CloseUI();
            }
        }

        if (GUILayout.Button("取消帧数限制"))
        {
            QualitySettings.vSyncCount = 0;
            Application.targetFrameRate = 0;
        }

        if (GUILayout.Button("删除所有UI"))
        {
            if (USpeedUI.UISystem.Instance)
            {
                GameObject.Destroy(USpeedUI.UISystem.Instance.gameObject);
            }

            if (USpeedUI.Blood.UBloodManager.Instance)
            {
                GameObject.Destroy(USpeedUI.Blood.UBloodManager.Instance.gameObject);
            }

            if (USpeedUI.UTopNameManager.Instance)
            {
                GameObject.Destroy(USpeedUI.UTopNameManager.Instance.gameObject);
            }
        }

        if(AssetBundleManager.isOnRunTimeMode)
        {
            if (GUILayout.Button("卸载所有资源（不可逆）"))
            {
                AssetBundleManager.Clear();
            }
        }


        if(!Application.isEditor)
        {
            if (GUILayout.Button("资源调试面板"))
            {
                DrawAssetDebugInfo = true;
            }
        }



        GUILayout.EndVertical();
        GUI.EndScrollView();
    }
    #endregion

}
