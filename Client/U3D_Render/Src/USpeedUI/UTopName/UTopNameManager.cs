using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using U3D_Render;
using UnityEngine.UI;
using GameLogic;
using ASpeedGame.Data.GameSettingConfig;

namespace USpeedUI
{
    public struct TopNameNode
    {
        public bool isValid;
        public Vector2 start;
        public Vector2 end;
        public Vector2 vSize;
        public int RowIndex;
        public int ColIndex;
    }

    public class UTopNameManager : MonoBehaviour
    {
        #region 成员变量
        private RenderTexture m_RenderTexture = null;
        public const int topNameRenderTextureWidth = 1920;
        public const int topNameRenderTextureHeight = 1080;

        //渲染摄像机，只渲所有的头顶信息UI
        public Camera renderCam;
        //渲染画布，所有的头顶信息UI都挂在它下面
        private RectTransform renderCanvas;


        private ResNode TopNamePrefabRes = null;
        private static ResNode TopNameSystemPrefabRes = null;
        //头顶名称实例表
        private Dictionary<int, UTopName> m_TopNameMap = new Dictionary<int, UTopName>();

        private static bool bInit = false;
        //无效的实例表，用于缓存
        private Queue<UTopName> m_InvalidList = new Queue<UTopName>();
        //无用的节点，由于位置有限，无用的节点必须腾出来给新创的用
        private Queue<TopNameNode> m_UnUseCanvasNodeList = new Queue<TopNameNode>();

        private GameObject CacheTopNameRoot = null;
        //头顶信息缓存表
        private Queue<UTopName> m_CacheTopName = new Queue<UTopName>();

        //缓存数量，用于预热时创建，目前暂定50个
        private const int TopNameCacheCount = 50;
        private bool bPreWarmTopName = false;

        public bool bShowRoleName = true;
        public bool bShowKinName = true;
        public bool bShowClanName = true;

        //private GameSettingsXml xmlNode;

        public static int PkTargetID { get { return m_PkTargetID; } set { m_PkTargetID = value; } }
        private static int m_PkTargetID = -1;

        //RT贴图，给公告板用
        public RenderTexture TopNameRenderTexture
        {
            get
            {
                if (m_RenderTexture == null)
                {
                    m_RenderTexture = new RenderTexture(topNameRenderTextureWidth, topNameRenderTextureHeight, 0);
                    m_RenderTexture.filterMode = FilterMode.Point;
                    m_RenderTexture.Create();
                }
                return m_RenderTexture;
            }
        }

        public static UTopNameManager Instance = null;
        #endregion

        #region 初始化
        /// <summary>
        /// 初始化
        /// </summary>
        public static void Init()
        {
            if (!bInit)
            {
                TopNameSystemPrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/TopName/UITopNameSystem");

                if (TopNameSystemPrefabRes == null)
                {
                    return;
                }

                GameObject topNameInstance = TopNameSystemPrefabRes.InstanceMainRes();

                if (topNameInstance == null)
                {
                    return;
                }

                topNameInstance.name = TopNameSystemPrefabRes.AssetName;
                bInit = true;
            }
        }

        public void Awake()
        {
            Instance = this;
            bShowRoleName = GameSettingConfig.Instance.GetGameSettingBoolValue(GameSettingIndex.ShowRoleName);
            bShowKinName = GameSettingConfig.Instance.GetGameSettingBoolValue(GameSettingIndex.ShowKinName);
            bShowClanName = GameSettingConfig.Instance.GetGameSettingBoolValue(GameSettingIndex.ShowClanName);
            InitCache();
        }

        public void Start()
        {
            renderCam = transform.GetComponentInChildren<Camera>();
            renderCanvas = transform.FindChild("TopNameCanvas").GetComponent<RectTransform>();

            m_RenderTexture = new RenderTexture(topNameRenderTextureWidth, topNameRenderTextureHeight, 0);
            m_RenderTexture.filterMode = FilterMode.Point;
            m_RenderTexture.Create();

            //指定TopName系统相机的目标RenderTexture
            if (renderCam != null && m_RenderTexture != null)
            {
                renderCam.targetTexture = m_RenderTexture;
            }
            TopNamePrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/TopName/TopName");

            Initialize.OnDeviceStateChage += Refresh;
            EntityFactory.SetEntityNumPropHandler(ResponseEntityNumPropEvent);

            //预加载头顶信息
            PreWarmTopName();
        }
        #endregion

        #region API方法
        /// <summary>
        /// 创建头顶信息实例
        /// </summary>
        /// <param name="objev"></param>
        /// <param name="entityID"></param>
        /// <param name="host"></param>
        /// <param name="ptr"></param>
        /// <param name="nLen"></param>
        /// <returns></returns>
        public UTopName CreateTopName(EntityView objev, IntPtr ptr, int nLen)
        {
            if (!bInit)
            {
                Trace.LogError("UTopNameManager必须初始化！");
                return null;
            }
            if (EntityView.isNullOrEmpty(objev))
            {
                Trace.LogWarning("TopName--实体视图无效!");
                return null;
            }
            int entityID = objev.ID;
            //如果表里面已含，则更新信息并返回
            if (m_TopNameMap.ContainsKey(entityID))
            {
                Trace.LogWarning(entityID + "已经有TopName，返回已有的实例");
                UTopName ut = m_TopNameMap[entityID];
                //接受逻辑层信息并更新
                SetNameInfo(ptr,nLen,ut);
                return ut;
            }
            //分配一个节点
            TopNameNode bcn = new TopNameNode();
            UTopName utn = GetTopName(ref bcn);
            if (null == utn)
            {
                return null;
            }
            utn.CreatBillBoard();
            utn.ChangeCanvasNode(bcn);
            utn.Init(objev);

            m_TopNameMap.Add(entityID, utn);

            SetNameInfo(ptr, nLen, utn);

            if (objev.Property)
            {
                int nMatchType = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MATCHTYPE);
                int nRankScore = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_RANKSCORE);
                int nRankGrade = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_RANKGRADE);
                int nRankIcon = GameLogicAPI.getRankIconIDByGrade(nMatchType, nRankGrade);
                if (nRankIcon > 0)
                    utn.SetRankLv(nRankIcon);
                int nSex = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SEX);
                utn.SetSex(nSex);
            }

            return utn;
        }

        public UTopName FindTopName(int entityID)
        {
            if (m_TopNameMap.ContainsKey(entityID))
            {
                UTopName ut = m_TopNameMap[entityID];
                return ut;
            }
            return null;
        }

        public void UpdateTopName(EntityView ev, IntPtr ptr, int nLen)
        {
            int entityID = -1;

            if(!EntityView.isNullOrEmpty(ev))
            {
                entityID = ev.ID;
            }
            if (m_TopNameMap.ContainsKey(entityID))
            {
                UTopName ut = m_TopNameMap[entityID];
                ut.Entity = ev;
                SetNameInfo(ptr, nLen, ut);
            }
        }

        public void SetPKMode(int entityID, bool bIsPK)
        {
            if (m_TopNameMap.ContainsKey(entityID))
            {
                UTopName ut = m_TopNameMap[entityID];
                ut.SetPKMode(bIsPK);
            }
        }

        //预加载
        public void PreWarmTopName()
        {
            if (bPreWarmTopName)
            {
                return;
            }
            bPreWarmTopName = true;
            for (int i = 0; i < TopNameCacheCount; i++)
            {
                TopNameNode canvNode = new TopNameNode();
                if (!AllocTopNamePos(ref canvNode))
                {
                    continue;
                }
                UTopName utn = InstanceUTopName(canvNode);
                utn.CreatBillBoard();
                utn.ChangeParent(CacheTopNameRoot.transform);
                utn.Init(null);
                utn.ChangeParent(CacheTopNameRoot.transform);
                utn.SetEnable(false);
                utn.ChangeCanvasNode(canvNode);
                m_CacheTopName.Enqueue(utn);
            }
        }

        /// <summary>
        /// 销毁所有头顶信息实例
        /// </summary>
        public void DestroyALL()
        {
            ClearCache();
            if (null != m_TopNameMap)
            {
                foreach (KeyValuePair<int, UTopName> var in m_TopNameMap)
                {
                    UTopName utn = var.Value;
                    if (utn != null)
                    {
                        var temp = utn.gameObject;
                        utn.resNode.DestroyInstanceRes(ref temp);
                        utn.Clear(true);
                    }
                }
                currentColIndex = 0;
                currentRowIndex = 0;

                m_UnUseCanvasNodeList.Clear();
                m_TopNameMap.Clear();
            }
        }
        #endregion

        public void Clear()
        {
            if (!bInit)
            {
                return;
            }
            DestroyALL();
            m_RenderTexture.Release();

            renderCam = null;
            renderCanvas = null;
            AssetBundleManager.DeleteAssets(ref TopNamePrefabRes, true);
            AssetBundleManager.DeleteAssets(ref TopNameSystemPrefabRes, true);
            TopNameSystemPrefabRes = null;
            TopNamePrefabRes = null;
            if (CacheTopNameRoot != null)
            {
                GameObject.DestroyImmediate(CacheTopNameRoot);
            }

            EntityFactory.RemoveEntityNumPropHandler(ResponseEntityNumPropEvent);
            Initialize.OnDeviceStateChage -= Refresh;
            bInit = false;
        }

        public void Update()
        {
            if (!bInit || Initialize.isDeviceLost || !SceneManager.Instance.isMainCity())
                return;

            if (m_RenderTexture == null || !m_RenderTexture.IsCreated())
            {
                Refresh();
            }
        }

        public void LateUpdate()
        {
            if (!bInit)
                return;

            NormalUpdate();
        }

        public void Refresh(bool bLost = false)
        {
            if (bLost || !bInit || null == renderCam)
                return;

            if (m_RenderTexture)
            {
                m_RenderTexture.Release();
                m_RenderTexture = null;
            }
            renderCam.targetTexture = null;
            m_RenderTexture = new RenderTexture(topNameRenderTextureWidth, topNameRenderTextureHeight, 0);
            m_RenderTexture.Create();
            renderCam.targetTexture = m_RenderTexture;
            foreach (UTopName name in m_TopNameMap.Values)
            {
                name.RefreshTexture();
            }
        }

        /// <summary>
        /// 切换名称显示
        /// </summary>
        /// <param name="nType">0:RoleName; 1:KinName; 2:ClanName</param>
        /// <param name="bShow"></param>
        public void SwitchNameShow(int nType, bool bShow)
        {
            switch (nType)
            {
                case 0:
                    bShowRoleName = bShow;
                    break;
                case 1:
                    bShowKinName = bShow;
                    break;
                case 2:
                    bShowClanName = bShow;
                    break;
            }

            foreach(UTopName item in m_TopNameMap.Values)
            {
                item.RefreshActivity();
            }

        }

        #region 缓存相关
        private void InitCache()
        {
            if (!CacheTopNameRoot)
            {
                CacheTopNameRoot = new GameObject();
                CacheTopNameRoot.name = "UITopNameCacheRoot";
                GameObject.DontDestroyOnLoad(CacheTopNameRoot);
            }
        }

        private void CacheTopName(UTopName ut)
        {
            if (null == ut)
            {
                return;
            }
            if (m_TopNameMap.ContainsKey(ut.ID))
            {
                ut.SetEnable(false);
                ut.Reset();
                ut.ChangeParent(CacheTopNameRoot.transform);
                m_CacheTopName.Enqueue(ut);
                m_TopNameMap.Remove(ut.ID);
            }
        }

        private void ClearCache()
        {
            foreach (UTopName ut in m_CacheTopName)
            {
                DestroyTopName(ut);
            }
            m_CacheTopName.Clear();
            bPreWarmTopName = false;
        }

        /// <summary>
        /// 销毁实例
        /// </summary>
        /// <param name="instance"></param>
        private void DestroyTopName(UTopName instance)
        {
            if (null == instance)
            {
                return;
            }

            int id = instance.ID;

            if (instance.CanvasNode.isValid)
            {
                m_UnUseCanvasNodeList.Enqueue(instance.CanvasNode);
            }

            var temp = instance.gameObject;
            instance.resNode.DestroyInstanceRes(ref temp);
            instance.Clear(true);

            if (m_TopNameMap.ContainsKey(id))
            {
                m_TopNameMap[id] = null;
                m_TopNameMap.Remove(id);
            }
        }
        #endregion

        #region 正常更新
        private void NormalUpdate()
        {
            m_InvalidList.Clear();
            if (null != m_TopNameMap)
            {
                foreach (UTopName var in m_TopNameMap.Values)
                {
                    UTopName utn = var;
                    EntityView view = utn.Entity;
                    if (EntityView.isNullOrEmpty(view))
                    {
                        m_InvalidList.Enqueue(utn);
                        continue;
                    }
                    //如果实体未激活则隐藏头顶称号
                    if (!view.gameObject.activeSelf || !view.StateMachine.GetVisible())
                    {
                        if (utn.GetEnable())
                        {
                            utn.SetEnable(false);
                        }
                    }
                    else
                    {
                        if (!utn.GetEnable())
                        {
                            utn.SetEnable(true);
                        }
                        //需要更新的头顶称号进行正常更新
                        utn.OnUpdate();
                    }
                }
            }
            //把无效的头顶称号缓存起来
            while (m_InvalidList.Count > 0)
            {
                UTopName ut = m_InvalidList.Dequeue();
                CacheTopName(ut);

            }
            m_InvalidList.Clear();
        }
        #endregion

        #region 分配位置
        public const int TopNameCell_Width = 180;
        public const int TopNameCell_Height = 60;
        public const int MaxRowCount = topNameRenderTextureWidth / TopNameCell_Width;
        public const int MaxColCount = topNameRenderTextureHeight / TopNameCell_Height;
        private static int currentRowIndex = 0;
        private static int currentColIndex = 0;

        private bool AllocTopNamePos(ref TopNameNode tnn)
        {
            Vector2 topNameSize = new Vector2(TopNameCell_Width, TopNameCell_Height);
            tnn.isValid = false;
            if (m_UnUseCanvasNodeList.Count > 0)
            {
                TopNameNode tn = m_UnUseCanvasNodeList.Dequeue();
                tnn.RowIndex = tn.RowIndex;
                tnn.ColIndex = tn.ColIndex;
                tnn.start.x = TopNameCell_Width * tn.RowIndex;
                tnn.start.y = TopNameCell_Height * tn.ColIndex;
                tnn.end.x = tn.start.x + topNameSize.x;
                tnn.end.y = tn.start.y + topNameSize.y;
                tnn.vSize = topNameSize;
                tnn.isValid = true;
            }
            else
            {
                tnn = new TopNameNode();
                tnn.start.x = TopNameCell_Width * currentRowIndex;
                tnn.start.y = TopNameCell_Height * currentColIndex;

                tnn.end.x = tnn.start.x + topNameSize.x;
                tnn.end.y = tnn.start.y + topNameSize.y;
                tnn.vSize = topNameSize;
                tnn.RowIndex = currentRowIndex;
                tnn.ColIndex = currentColIndex;
                currentRowIndex++;
                if (currentRowIndex >= MaxRowCount)
                {
                    currentColIndex++;
                    if (currentColIndex >= MaxColCount)
                    {
                        return false;
                    }
                    currentRowIndex = 0;
                }
                tnn.isValid = true;
            }
            return true;
        }
        #endregion

        #region 创建相关
        private UTopName GetTopName(ref TopNameNode canvNode)
        {
            UTopName ut = null;

            if (m_CacheTopName.Count > 0)
            {
                ut = m_CacheTopName.Dequeue();
                canvNode = ut.CanvasNode;
                ut.RefreshTexture();
            }
            else
            {
                if (!AllocTopNamePos(ref canvNode))
                {
                    return null;
                }
                ut = InstanceUTopName(canvNode);
            }
            return ut;
        }

        /// <summary>
        /// 实例化创建头顶称号
        /// </summary>
        /// <param name="bcn">节点信息</param>
        /// <returns></returns>
        private UTopName InstanceUTopName(TopNameNode bcn)
        {
            GameObject go = TopNamePrefabRes.InstanceMainRes();
            if (RectPosition(go, bcn))
            {
                UTopName ut = go.GetComponent<UTopName>();
                ut.resNode = TopNamePrefabRes;
                return ut;
            }
            return null;
        }

        //转换像素纹理值，算出需要的贴图纹理值信息
        public static Vector2 TransformTextureOffsetFromPixelPos(Vector2 pixelOffset)
        {
            Vector2 off = pixelOffset;
            off.x = off.x / topNameRenderTextureWidth;
            off.y = off.y / topNameRenderTextureHeight;

            return off;
        }

        /// <summary>
        /// 调整实例的位置
        /// </summary>
        /// <param name="go">实例GameObject</param>
        /// <param name="bcn">节点信息</param>
        /// <returns></returns>
        private bool RectPosition(GameObject go, TopNameNode bcn)
        {
            if (go == null || bcn.start == null)
                return false;

            RectTransform rt = go.transform.GetComponent<RectTransform>();

            if (rt == null || renderCanvas == null)
                return false;

            rt.SetParent(renderCanvas,false);
            rt.anchorMin = new Vector2(0, 0);
            rt.anchorMax = new Vector2(0, 0);
            rt.pivot = new Vector2(0, 0);
            rt.anchoredPosition = new Vector2(bcn.start.x, bcn.start.y);
            return true;
        }
        #endregion

        /// <summary>
        /// 接受逻辑层数据
        /// </summary>
        /// <param name="ptr"></param>
        /// <param name="nLen"></param>
        /// <param name="utn">头顶信息实例</param>
        private void SetNameInfo(IntPtr ptr, int nLen, UTopName utn)
        {
            if (null == utn)
                return;
            utn.ClearName();
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_count cmdCount = helper.get<cmd_count>();
            for (int i = 0; i < cmdCount.nCount; ++i)
            {
                cmd_entity_show_title_node node = helper.get<cmd_entity_show_title_node>();
                switch (node.nTitleID)
                {
                    case 0:
                        utn.SetRoleName(node.szTitleName);
                        break;
                    case 1:
                        utn.SetKinName(node.szTitleName);
                        break;
                    case 2:
                        utn.SetClanName(node.szTitleName);
                        break;
                    default:
                        break;
                }
            }
        }

        /// <summary>
        /// 数值属性回调
        /// </summary>
        /// <param name="e"></param>
        private void ResponseEntityNumPropEvent(EntityNumPropEventArgs e)
        {
            switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_RANKGRADE:
                    {
                        if (EntityView.isNullOrEmpty(e.objev) || null == e.objev.Property)
                            return;

                        UTopName ut = FindTopName(e.data.nEntityID);
                        if (ut != null)
                        {
                            int nMatchType = e.objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MATCHTYPE);
                            int nRankGrade = e.data.nValue;
                            int nRankIcon = GameLogicAPI.getRankIconIDByGrade(nMatchType, nRankGrade);
                            if (nRankIcon > 0)
                                ut.SetRankLv(nRankIcon);
                        }
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_RANKSCORE:
                    {
                        if (EntityView.isNullOrEmpty(e.objev) || null == e.objev.Property)
                            return;

                        UTopName ut = FindTopName(e.data.nEntityID);
                        if (ut != null)
                        {
                            int nMatchType = e.objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MATCHTYPE);
                            int nRankScore = e.objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_RANKGRADE);
                            int nRankIcon = GameLogicAPI.getRankIconIDByGrade(nMatchType, nRankScore);
                            if (nRankIcon > 0)
                                ut.SetRankLv(nRankIcon);
                        }
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_SEX:
                    {
                        if (EntityView.isNullOrEmpty(e.objev) || null == e.objev.Property)
                            return;

                        UTopName ut = FindTopName(e.data.nEntityID);
                        if (ut != null)
                        {
                            ut.SetSex(e.data.nValue);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
}
