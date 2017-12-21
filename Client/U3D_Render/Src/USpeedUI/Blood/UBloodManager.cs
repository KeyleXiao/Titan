using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using U3D_Render;
using War;

namespace USpeedUI.Blood
{
    /// <summary>
    /// 血条管理类，负责血条创建、更新及销毁
    /// </summary>
    public partial class UBloodManager : MonoBehaviour
    {
        private Dictionary<int, UBlood> m_BloodMap = new Dictionary<int, UBlood>();
        private Queue<UBlood> m_InvalidList = new Queue<UBlood>();
        private Dictionary<BloodStyleType, Queue<BloodCanvasNode>> m_unUseCanvasNodeMap = new Dictionary<BloodStyleType, Queue<BloodCanvasNode>>();
        static private bool bInit = false;

        #region 初始化
        static public void Init()
        {
            if (bInit)
                return;

            ResNode bloodSystemPrefab = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/Blood/UIBloodSystem");

            if (null == bloodSystemPrefab)
                return;

            Instance = bloodSystemPrefab.InstanceMainRes<UBloodManager>();
            Instance.name = bloodSystemPrefab.AssetName;

            bInit = true;
        }

        public void Start()
        {
            if (null == renderCam)
                renderCam = this.transform.GetComponentInChildren<Camera>();

            if(null == renderCanvas)
                renderCanvas = this.transform.FindChild("BloodCanvas").GetComponent<RectTransform>();

            if(null == m_RenderTexture)
            {
                m_RenderTexture = new RenderTexture(bloodRenderTextureWidth, bloodRenderTextureHeight, 16);
                m_RenderTexture.filterMode = FilterMode.Point;
                m_RenderTexture.Create();

                if (renderCam != null)
                    renderCam.targetTexture = m_RenderTexture;
            }

            if (null == bossBloodParent)
                bossBloodParent = UISystem.Instance.GetWndLayerTrans(WndLayerDef.WND_LAYER_FIXED);

            foreach (BloodStyleInfo info in m_BloodStyleList)
            {
                if (info.bloodType == BloodStyleType.MAX)
                    continue;

                string path = "UI/Prefabs/Blood/BloodStyle/Blood_" + info.bloodType.ToString();
                ResNode node = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, path);
                if (null != node)
                    info.bloodRes = node;
            }

            actorBloodVisible = ASpeedGame.Data.GameSettingConfig.GameSettingConfig.Instance.GetGameSettingBoolValue(ASpeedGame.Data.GameSettingConfig.GameSettingIndex.ShowSelfBlood);
            numInfoVisible = ASpeedGame.Data.GameSettingConfig.GameSettingConfig.Instance.GetGameSettingBoolValue(ASpeedGame.Data.GameSettingConfig.GameSettingIndex.ShowBloodNum); 

            EntityFactory.SetEntityNumPropHandler(ResponseEntityNumPropEvent);
            TargetFrame.OnSelectedTarget += OnSelectTarget;
            TargetFrame.OnDisSelectedTarget += OnUnSelectTarget;
            SoldierCamera.SwitchLookAtSoliderEvent += SwitchBossBloodTarget;
            Initialize.OnDeviceStateChage += Refresh;
            Initialize.onApplicationFocusChangeEvent.AddListener(ForceRefresh);

            InitCache();
        }
        #endregion

        #region 销毁
        public void Clear()
        {
            if (!bInit)
            {
                return;
            }

            DestroyALL();

            foreach (BloodStyleInfo info in m_BloodStyleList)
            {
                if (null != info.bloodRes)
                    AssetBundleManager.DeleteAssets(ref info.bloodRes, true);
            }

            if (null != CacheBloodRoot)
            {
                GameObject.DestroyImmediate(CacheBloodRoot);
            }

            EntityFactory.RemoveEntityNumPropHandler(ResponseEntityNumPropEvent);
            TargetFrame.OnSelectedTarget -= OnSelectTarget;
            TargetFrame.OnDisSelectedTarget -= OnUnSelectTarget;
            SoldierCamera.SwitchLookAtSoliderEvent -= SwitchBossBloodTarget;
            Initialize.OnDeviceStateChage -= Refresh;
            Initialize.onApplicationFocusChangeEvent.RemoveListener(ForceRefresh);

            m_BloodStyleList.Clear();
            m_RenderTexture.Release();
            m_RenderTexture = null;

            renderCam = null;
            renderCanvas = null;
            bossBloodParent = null;

            bInit = false;
        }

        public void DestroyALL()
        {
            ClearCache();

            if (null != m_BloodMap)
            {
                foreach (KeyValuePair<int, UBlood> var in m_BloodMap)
                {
                    UBlood ub = var.Value;
                    if (ub != null)
                    {
                        var temp = ub.gameObject;
                        ub.resNode.DestroyInstanceRes(ref temp);
                        ub.Clear();
                    }
                }

                m_curIndexArry = new Vector2[3];
                m_BloodMap.Clear();
            }

            if (null != BossBlood)
            {
                var temp = BossBlood.gameObject;
                BossBlood.resNode.DestroyInstanceRes(ref temp);
                BossBlood.Clear();
                BossBlood = null;

                CurrentBossBloodHostID = -1;
            }
            BossBloodEntityTable.Clear();
            BossBloodProityList.Clear();

            m_unUseCanvasNodeMap.Clear();
        }

        public void DestroyBlood(int entityID)
        {
            UBlood ub = FindBlood(entityID);
            if(ub != null)
            {
                DestroyBlood(ub);
            }
        }

        /// <summary>
        /// 摧毁血条实例
        /// </summary>
        /// <param name="instance"></param>
        private void DestroyBlood(UBlood instance)
        {
            if (null == instance)
            {
                return;
            }

            int id = instance.ID;

            //节点有效则缓存节点，大血条不用
            if (instance.CanvasNode.isValid && instance.BloodType != BloodStyleType.Boss)
            {
                if (!m_unUseCanvasNodeMap.ContainsKey(instance.BloodType))
                    m_unUseCanvasNodeMap.Add(instance.BloodType, new Queue<BloodCanvasNode>());

                m_unUseCanvasNodeMap[instance.BloodType].Enqueue(instance.CanvasNode);
            }

            var temp = instance.gameObject;
            instance.resNode.DestroyInstanceRes(ref temp);
            instance.Clear();

            if (m_BloodMap.ContainsKey(id))
            {
                m_BloodMap[id] = null;
                m_BloodMap.Remove(id);
            }
        }
        #endregion

        #region 更新
        public void Update()
        {
            if (!bInit || Initialize.isDeviceLost)
                return;

            if (m_RenderTexture == null || !m_RenderTexture.IsCreated())
            {
                Refresh();
            }
        }

        public void ForceRefresh(bool bFoucs)
        {
            if(bFoucs)
                Refresh();
        }

        public void Refresh(bool bLost = false)
        {
            if (bLost || !bInit || null == renderCam)
                return;

            foreach (UBlood blood in m_BloodMap.Values)
            {
                blood.RefreshTexture();
            }
        }

        public void LateUpdate()
        { 
            if (!bInit)
                return;

            NormalUpdate();
            UpdateBossBlood();
            UpdateNearTowerSoldier();
        }

        /// <summary>
        /// 普通血条更新
        /// </summary>
        private void NormalUpdate()
        {
            m_InvalidList.Clear();
            if (null != m_BloodMap)
            {
                foreach (UBlood var in m_BloodMap.Values)
                {
                    UBlood ub = var;
                    EntityView view = ub.Entity;
                    if (view == null || !view.IsValid)
                    {
                        m_InvalidList.Enqueue(ub);
                        continue;
                    }
                    //如果实体未激活,或者尚未加载完毕则隐藏血条
                    if (SkinInstance.isNullOrEmpry(view.SkinControl.CurrentSkinInstance) || !view.StateMachine.GetVisible())
                    {
                        ub.SetEnable(false);
                        if (view.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
                            ub.OnUpdate();
                    }
                    else
                    {
                        ub.SetEnable(true);
                        //需要更新的血条进行正常更新
                        ub.OnUpdate();
                    }
                }
            }
            while (m_InvalidList.Count > 0)
            {
                UBlood ub = m_InvalidList.Dequeue();
                if (bPreWarmBlood)
                {
                    CacheBlood(ub);
                }
                else
                {
                    DestroyBlood(ub);
                }
            }
            m_InvalidList.Clear();
        }

        /// <summary>
        /// 更新大血条
        /// </summary>
        private void UpdateBossBlood()
        {
            if (null == BossBlood)
            {
                return;
            }

            JitterBossBlood();

            for (int i = 0; i < BossBloodProityList.Count; i++)
            {
                EntityView ev = BossBloodProityList[i];
                if (null == ev)
                {
                    continue;
                }
                if (!ev.gameObject)
                {
                    continue;
                }
                if (ev.BossBloodShowDistance > 0)
                {
                    if (bossBloodTarget == null && EntityFactory.MainHero != null)
                        bossBloodTarget = EntityFactory.MainHero.transform;

                    float dis = Vector3.SqrMagnitude(bossBloodTarget.position - ev.gameObject.transform.position);
                    if (dis < ev.BossBloodShowDistance * ev.BossBloodShowDistance)
                    {
                        if (ev.CampFlag != SPELL.CampFlag.CampFlag_Friend && ev.CampFlag != SPELL.CampFlag.CampFlag_Self)
                        {
                            EntityEnterBossBloodDistance(ev.ID);
                        }
                        break;
                    }
                    else
                    {
                        EntityLeaveBossBloodDistance(ev.ID);
                    }
                }
            }

            if (null != BossBlood && CurrentBossBloodHostID > 0)
            {

                EntityView ev = EntityFactory.getEntityViewByID(CurrentBossBloodHostID);
                if (ev != null && ev.StateMachine != null)
                {
                    UpdateBossBloodAndTowerSoldier(ev, ev.StateMachine.GetVisible());
                }
                else
                    UpdateBossBloodAndTowerSoldier(ev, false);

                BossBlood.OnUpdate();
            }

        }

        private void UpdateBossBloodAndTowerSoldier(EntityView TargetEv, bool bVisible)
        {
            if (bVisible)
            {
                if (TargetEv != null)
                {
                    if (BossBlood != null)
                        BossBlood.SetBossBloodVisible(bVisible);

                    if (TargetEv.CampFlag == SPELL.CampFlag.CampFlag_Enemy)
                    {
                        if (TargetEv.Property != null && TargetEv.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SEX) == (int)MONSTER_TYPE.MONSTER_TYPE_TOWER)
                        {
                            EnterTower(TargetEv);
                        }
                        else
                            ExitTower();
                    }
                    else
                        ExitTower();
                }
            }
            else
            {
                if (BossBlood != null)
                    BossBlood.SetBossBloodVisible(false);

                ExitTower();
            }
        }
        #endregion

        #region 创建、查找、更新
        /// <summary>
        /// 血条创建
        /// </summary>
        /// <param name="objev">实体View</param>
        /// <param name="entityID">实体ID</param>
        /// <param name="host">实体Object</param>
        /// <param name="camType">阵营</param>
        /// <returns></returns>
        public UBlood CreatBlood(EntityView objev, int campType)
        {
            if (!bInit)
            {
                Trace.LogError("UBloodManager必须初始化！");
                return null;
            }
            if (null == objev || !objev.IsValid || null == objev.Property)
            {
                Trace.LogWarning("Blood--EntityView无效！");
                return null;
            }

            CreateBossBlood();
            int entityID = objev.ID;
            if (m_BloodMap.ContainsKey(entityID))
            {
                Trace.LogWarning(entityID + "已经有血条，返回已有的实例");
                UBlood ub = m_BloodMap[entityID];
                return ub;
            }

            //按照策划要求，目前不创建己方小兵血条,其他己方物体不变
            //if (objev.Flag == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_SOLDIER && campType == (int)GFxCampTpye.CT_Friend && objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASTER_UID) <= 0)
            //{
            //    return null;
            //}

            int hideFlag = (int)ENTITY_MASKFLAG.MASKFLAG_PROPCHANGE_HIDE_SAME_CAMP;
            if (campType == (int)GFxCampTpye.CT_Friend && (objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK) & hideFlag) == hideFlag)
            {
                return null;
            }

            BloodStyleType bloodType = BloodStyleType.Player;

            if(objev.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
                bloodType = BloodStyleType.Player;
            else if (objev.Type == ENTITY_TYPE.TYPE_MONSTER && objev.createinfo.nBloodStyle < (int)BloodStyleType.Boss)
                bloodType = (BloodStyleType)objev.createinfo.nBloodStyle;

            BloodCanvasNode bcn = new BloodCanvasNode();
            UBlood blood = GetBlood(bloodType, ref bcn);
            if (null == blood)
                return null;

            blood.Init(objev, campType, bloodType, bcn);
            m_BloodMap.Add(entityID, blood);
            ChangeMask(objev, true);
            SetProperty(objev, blood);

            return blood;
        }

        public UBlood FindBlood(int entityID)
        {
            if (m_BloodMap.ContainsKey(entityID))
            {
                UBlood ub = m_BloodMap[entityID];
                return ub;
            }
            return null;
        }

        public void UpdateBlood(EntityView view)
        {
            int entityID = -1;
            if (EntityView.isNullOrEmpty(view))
            {
                return;
            }
            entityID = view.ID;
            if (m_BloodMap.ContainsKey(entityID))
            {
                UBlood ub = m_BloodMap[entityID];
                ub.Entity = view;
                ub.ChangeParent(view.transform);
                if(view.Property != null)
                ub.SetCampType(view.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP));
            }
        }

        public void ChangeCanvasVisible(bool _bVisible)
        {
            if (renderCanvas != null)
                renderCanvas.gameObject.SetActive(_bVisible);
        }
        #endregion

        #region 血条节点创建
        //当前格子索引表，X：行；Y：列
        private Vector2[] m_curIndexArry = new Vector2[3];  //暂时只显示三种类型，Player、Buliding、Monster,大血条不用建格子

        /// <summary>
        /// 分配血条格子，同一类型放一起，左下为起点， 如：最下面3行放人物的，再依次往上4行放建筑的，最上面放怪物
        /// </summary>
        /// <param name="type"></param>
        /// <param name="bcn"></param>
        /// <returns></returns>
        private bool AllocBloodPos(BloodStyleType type, ref BloodCanvasNode bcn)
        {
            int nTypeIndex = (int)type;
            BloodStyleInfo bloodInfo = m_BloodStyleList.Find((x) => {return x.bloodType == type;});
            if (null != bloodInfo)
            {
                Vector2 bloodSize = bloodInfo.bloodSize;
                bcn.isValid = false;
                //先查缓存格子列有没有
                if (m_unUseCanvasNodeMap.ContainsKey(type) && m_unUseCanvasNodeMap[type].Count > 0)
                {
                    BloodCanvasNode bc = m_unUseCanvasNodeMap[type].Dequeue();
                    bcn.RowIndex = bc.RowIndex;
                    bcn.ColIndex = bc.ColIndex;
                    bcn.start = bc.start;
                    bcn.end = bc.end;
                    bcn.vSize = bc.vSize;
                    bcn.scale = bc.scale;
                    bcn.isValid = true;
                }
                else
                {
                    if (m_curIndexArry[nTypeIndex].x > MaxCellRowCountList[nTypeIndex])
                    {
                        bcn.isValid = false;
                        return false;
                    }

                    bcn = new BloodCanvasNode();

                    bcn.start.x = bloodInfo.bloodSize.x * m_curIndexArry[nTypeIndex].y;
                    float offset = 0;
                    if (nTypeIndex > 0)
                    {
                        for(int i = 0; i < nTypeIndex; i++)
                        {
                            BloodStyleInfo info = m_BloodStyleList.Find((x) => { return x.bloodType == (BloodStyleType)i; });
                            if(info != null)
                            {
                                offset += MaxCellRowCountList[i] * info.bloodSize.y;
                            }
                        }
                    }
                    bcn.start.y = offset + bloodInfo.bloodSize.y * m_curIndexArry[nTypeIndex].x;

                    bcn.end.x = bcn.start.x + bloodSize.x;
                    bcn.end.y = bcn.start.y + bloodSize.y;
                    bcn.vSize = bloodSize;
                    bcn.scale = bloodInfo.bloodScale;
                    bcn.RowIndex = (int)m_curIndexArry[nTypeIndex].x;
                    bcn.ColIndex = (int)m_curIndexArry[nTypeIndex].y;
                    //分配下一次的格子索引
                    m_curIndexArry[nTypeIndex].y++;
                    if (m_curIndexArry[nTypeIndex].y == (int)(bloodRenderTextureWidth / bloodInfo.bloodSize.x))
                    {
                        m_curIndexArry[nTypeIndex].x++;
                        m_curIndexArry[nTypeIndex].y = 0;
                    }
                    bcn.isValid = true;
                }
                return true;
            }
            bcn.isValid = false;
            return false;
        }

        /// <summary>
        /// 变换纹理坐标
        /// </summary>
        /// <param name="pixelOffset">像素坐标</param>
        /// <returns></returns>
        public Vector2 TransformTextureOffsetFromPixelPos(Vector2 pixelOffset)
        {
            Vector2 off = pixelOffset;
            off.x = off.x / bloodRenderTextureWidth;
            off.y = off.y / bloodRenderTextureHeight;

            return off;
        }

        /// <summary>
        /// 实例化普通血条
        /// </summary>
        /// <param name="entityType">实体类型</param>
        /// <param name="isHero">是否主角</param>
        /// <param name="bcn">血条节点数据</param>
        /// <returns></returns>
        private UBlood InstanceUBlood(BloodStyleType type, BloodCanvasNode bcn)
        {
            //大血条返回null,这个方法只创常规血条
            if (type == BloodStyleType.Boss)
                return null;

            BloodStyleInfo bloodInfo = m_BloodStyleList.Find((x) => {return x.bloodType == type;});
            if (null != bloodInfo && null != bloodInfo.bloodRes)
            {
                GameObject go = bloodInfo.bloodRes.InstanceMainRes();
                if (RectPosition(go, bcn))
                {
                    UBlood blood = go.GetComponent<UBlood>();
                    blood.resNode = bloodInfo.bloodRes;
                    return blood;
                }
            }
            return null;
        }

        /// <summary>
        /// 实例化大血条
        /// </summary>
        /// <returns></returns>
        private UBlood InstanceUBossBlood()
        {
            BloodStyleInfo bloodInfo = m_BloodStyleList.Find((x) => {return x.bloodType == BloodStyleType.Boss;});
            if (null != bloodInfo && null != bloodInfo.bloodRes && null != bossBloodParent)
            {
                GameObject go = bloodInfo.bloodRes.InstanceMainRes();
                RectTransform rt = go.transform.GetComponent<RectTransform>();
                rt.SetParent(bossBloodParent, false);

                UBlood blood = go.GetComponent<UBlood>();
                blood.resNode = bloodInfo.bloodRes;
                return blood;
            }
            return null;
        }

        /// <summary>
        /// 设置血条位置
        /// </summary>
        /// <param name="go"></param>
        /// <param name="bcn"></param>
        /// <returns></returns>
        private bool RectPosition(GameObject go, BloodCanvasNode bcn)
        {
            if (go == null || bcn.start == null)
                return false;

            RectTransform rt = go.transform.GetComponent<RectTransform>();

            if (rt == null || renderCanvas == null)
                return false;

            rt.SetParent(renderCanvas,false);
            rt.localPosition = new Vector3(bcn.start.x, bcn.start.y, 0);
            return true;
        }
        #endregion

        #region 血条数据初始化
        /// <summary>
        /// 设置角色属性
        /// </summary>
        /// <param name="objev"></param>
        private void SetProperty(EntityView objev, UBlood blood)
        {
            if (null == objev || null == objev.Property || null == blood)
                return;

            int nHeroID = objev.ID;
            int nMaxHp = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP);
            int nCurHp = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP);
            int nTempHp = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SHIELD);
            int nCurMp = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MP);
            int nMaxMp = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_MP);
            int nCamp = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP);

            int nLevel = 1;
            if(pkTargetID < 0)  //PKTarget<0,代表当前不在PK状态
            {
                nLevel = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            }
            else
                nLevel = GameLogicAPI.getPlayerLevel(nHeroID);
            int masterID = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASTER_UID);
            string sName = objev.Property.CreatureName;
            bool bIsMainHero = objev.IsHero;

            if (masterID > 0)
            {
                EntityView masterView = EntityFactory.getEntityViewByID(masterID);
                if (null != masterView && null != masterView.Property)
                {
                    //英雄分身
                    if (blood.BloodType == BloodStyleType.Player)
                    {
                        nMaxHp = masterView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP);
                        nCurHp = masterView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP);
                        nTempHp = masterView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SHIELD);
                        nCurMp = masterView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MP);
                        nMaxMp = masterView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_MP);

                        nLevel = 1;
                        if (pkTargetID < 0)  //PKTarget<0,代表当前不在PK状态
                            nLevel = masterView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
                        else
                            nLevel = GameLogicAPI.getPlayerLevel(masterID);
                        sName = masterView.Property.CreatureName;
                    }
                }
            }

            if(nHeroID != pkTargetID)
                blood.SetCampType(nCamp);
            blood.SetLevel(nLevel);
            blood.SetMaxValue(nMaxHp);
            blood.SetCurValue(nCurHp);
            blood.SetTempValue(nTempHp);
            blood.SetMpValue(nCurMp);
            blood.SetMpMaxValue(nMaxMp);
            if(!bIsMainHero)
                blood.SetRoleName(sName);
            else
                blood.SetClosed(!actorBloodVisible);
        }

        public void ChangeMask(EntityView ev, bool bUseInitMask = false)
        {
            if (ev != null)
            {
                //if (ev.Type == ENTITY_TYPE.TYPE_MONSTER)
                {
                    int mas = 0;
                    if (bUseInitMask)
                    {
                        mas = ev.InitMaskFlag;
                    }
                    else
                    {
                        mas = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);

                        if (!m_bFirstVisibleMask)
                        {
                            m_bFirstVisibleMask = true;
                            return;
                        }
                    }
                    if (IsHaveBossBlood(mas))
                    {
                        if (!BossBloodEntityTable.ContainsKey(ev.ID))
                        {
                            BossBloodEntityTable.Add(ev.ID, ev);
                            BossBloodProityList.Add(ev);
                            SortBossProityList();
                        }
                    }
                    if (mas != 0)
                    {
                        bool val = (mas & (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_BLOOD) == (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_BLOOD;
                        SetVisible(ev.ID, !val);
                    }
                    else
                    {
                        SetVisible(ev.ID, true);
                    }
                }
            }
        }
        #endregion

        #region 回调
        /// <summary>
        /// 实体死亡
        /// </summary>
        /// <param name="entityID"></param>
        /// <param name="t"></param>
        public void OnEntityDead(int entityID, ENTITY_TYPE t)
        {
            if (entityID == CurrentSelectedID ||
           entityID == EntityFactory.MainHeroID)
            {
                CurrentSelectedID = -1;
                ChangeBossBloodTarget(-1);
            }
            if (t == ENTITY_TYPE.TYPE_MONSTER)
            {
                if (m_BloodMap.ContainsKey(entityID))
                {
                   CacheBlood(m_BloodMap[entityID]);
                }
                if (BossBloodEntityTable.ContainsKey(entityID))
                {
                    BossBloodProityList.Remove(BossBloodEntityTable[entityID]);
                    BossBloodEntityTable.Remove(entityID);
                }
                EntityLeaveBossBloodDistance(entityID);
            }
            if (t == ENTITY_TYPE.TYPE_PLAYER_ROLE)
            {
                SetDead(entityID, true);
            }
        }

        /// <summary>
        /// 实体复活
        /// </summary>
        /// <param name="entityID"></param>
        /// <param name="t"></param>
        public void OnEntityRelive(int entityID, ENTITY_TYPE t)
        {
            if (t == ENTITY_TYPE.TYPE_PLAYER_ROLE)
            {
                if (entityID == EntityFactory.MainHeroID)
                {
                    //BossBlood.SetEnable(true);
                    bossBloodTarget = EntityFactory.MainHero.transform;
                }
                SetDead(entityID, false);
            }
        }

        /// <summary>
        /// 实体受击
        /// </summary>
        /// <param name="SrcID"></param>
        /// <param name="DstID"></param>
        public void OnEntityHit(int SrcID, int DstID)
        {
            if (DstID < 0 || CurrentBossBloodHostID < 0 ||
                SrcID != EntityFactory.MainHeroID)
            {
                return;
            }

            if (DstID == CurrentBossBloodHostID)
            {
                bBossBloodJitter = true;
                JitterTimer = 0.0f;
            }
        }

        /// <summary>
        /// 属性数值更新
        /// </summary>
        /// <param name="e"></param>
        public void ResponseEntityNumPropEvent(EntityNumPropEventArgs e)
        {
            int id = e.data.nEntityID;
            EntityView ev = e.objev;

            if (null == ev)
                return;

            switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_CAMP:	    // 阵营
                    {
                        if(id != pkTargetID)
                        {
                            int nCamp = e.data.nValue;
                            SetCamType(id, nCamp);
                        }
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_HP:			// 血量
                case (int)ENTITY_PROPERTY.PROPERTY_MAX_HP:
                case (int)ENTITY_PROPERTY.PROPERTY_SHIELD:		// 护盾（临时血量）
                    {
                        int MaxHp = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP);
                        int CurHp = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP);
                        int TempHp = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SHIELD);

                        //血量最大值为0，将会隐藏血条
                        SetMaxValue(id, MaxHp);
                        SetCurValue(id, CurHp);
                        SetTempValue(id, TempHp);

                        UpdateBossBloodData(id, ev.Property.CreatureName, MaxHp, CurHp, TempHp);
                    }
                    break;

                case (int)ENTITY_PROPERTY.PROPERTY_LEVEL:       // 等级变化
                    {
                        SetLevel(e.data.nEntityID, e.data.nValue);
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_MP:
                case (int)ENTITY_PROPERTY.PROPERTY_MAX_MP:
                    {
                        int Mp = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MP);
                        int MpMax = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_MP);

                        SetMpValue(id, Mp);
                        SetMpMaxValue(id, MpMax);
                    }
                    break;
                default:
                    break;
            }
        }

        /// <summary>
        /// 选择目标
        /// </summary>
        /// <param name="param"></param>
        public void OnSelectTarget(TargetFrame.SelectTargetParam param)
        {
            CurrentSelectedID = param.nTargetId;
            EntityView ev = EntityFactory.getEntityViewByID(CurrentSelectedID);
            if (null == ev)
            {
                CurrentSelectedID = -1;
            }
            else
            {
                if (!param.bIngoreBossMark && !IsHaveBossBlood(ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK)))
                {
                    CurrentSelectedID = -1;
                }
            }

            //如果有选中的目标或者没有进入BOSS血条距离的实体，则切换
            if (CurrentSelectedID > 0 || CurrentEntityEnterBossBloodDistanceID < 0)
            {
                ChangeBossBloodTarget(CurrentSelectedID, param.bIngoreBossMark);
            }
        }

        /// <summary>
        /// 解除选择目标
        /// </summary>
        /// <param name="entityID"></param>
        public void OnUnSelectTarget(int entityID)
        {
            CurrentSelectedID = -1;
            if (CurrentSelectedID > 0 || CurrentEntityEnterBossBloodDistanceID < 0)
            {
                ChangeBossBloodTarget(CurrentSelectedID);
            }
        }
        #endregion

        #region 血条属性设置
        public void ChangeBloodColor(int nId, bool bIsChange)
        {
            if (!m_BloodMap.ContainsKey(nId))
            {
                return;
            }
            m_BloodMap[nId].ChangeBloodColor(bIsChange);
        }

        public void ChangePlayerBlood(bool val)
        {
            if (!m_BloodMap.ContainsKey(EntityFactory.MainHeroID))
            {
                return;
            }
            if (actorBloodVisible == val)
            {
                return;
            }
            actorBloodVisible = val;
            m_BloodMap[EntityFactory.MainHeroID].SetClosed(!actorBloodVisible);
        }

        public void ChangeBloodInfoVisible(bool val)
        {
            if (numInfoVisible == val)
            {
                return;
            }
            numInfoVisible = val;
            foreach (UBlood var in m_BloodMap.Values)
            {
                var.UpdateBloodInfoVisible(numInfoVisible);
            }
        }

        public void SetDead(int entityID, bool v)
        {
            if (m_BloodMap.ContainsKey(entityID))
            {
                UBlood ub = m_BloodMap[entityID];
                ub.SetDead(v);
            }
        }

        public void SetVisible(int entityID, bool v)
        {
            if (m_BloodMap.ContainsKey(entityID))
            {
                UBlood ub = m_BloodMap[entityID];
                ub.SetVisible(v);
            }
        }

        public void SetRoleName(int entityID, string name)
        {
            if (m_BloodMap.ContainsKey(entityID))
            {
                UBlood ub = m_BloodMap[entityID];
                ub.SetRoleName(name);
            }
        }

        public void SetCamType(int entityID, int camp)
        {
            if (entityID == pkTargetID)
                return;

            if (m_BloodMap.ContainsKey(entityID))
            {
                UBlood ub = m_BloodMap[entityID];
                ub.SetCampType(camp);
            }
        }

        public void SetMaxValue(int entityID, int v)
        {

            if (m_BloodMap.ContainsKey(entityID))
            {
                UBlood ub = m_BloodMap[entityID];
                ub.SetMaxValue(v);
            }
        }

        public void SetCurValue(int entityID, int v)
        {
            if (m_BloodMap.ContainsKey(entityID))
            {
                UBlood ub = m_BloodMap[entityID];
                ub.SetCurValue(v);
            }
        }

        public void SetTempValue(int entityID, int v)
        {
            if (m_BloodMap.ContainsKey(entityID))
            {
                UBlood ub = m_BloodMap[entityID];
                ub.SetTempValue(v);
            }
        }

        public void SetMpValue(int entityID, int v)
        {
            if (m_BloodMap.ContainsKey(entityID))
            {
                UBlood ub = m_BloodMap[entityID];
                ub.SetMpValue(v);
            }
        }

        public void SetMpMaxValue(int entityID, int v)
        {
            if (m_BloodMap.ContainsKey(entityID))
            {
                UBlood ub = m_BloodMap[entityID];
                ub.SetMpMaxValue(v);
            }
        }

        public void SetLevel(int entityID, int lev)
        {

            if (m_BloodMap.ContainsKey(entityID))
            {
                UBlood ub = m_BloodMap[entityID];
                ub.SetLevel(lev);
            }
        }
        #endregion

        #region 大血条
        private Dictionary<int, EntityView> BossBloodEntityTable = new Dictionary<int, EntityView>();
        private List<EntityView> BossBloodProityList = new List<EntityView>();

        private int CurrentBossBloodHostID = -1;
        private int CurrentEntityEnterBossBloodDistanceID = -1;
        private int CurrentSelectedID = -1;
        private bool bBossBloodJitter = false;


        /// <summary>
        /// 抖动大血条
        /// </summary>
        private void JitterBossBlood()
        {
            if (!bBossBloodJitter || null == BossBlood)
            {
                return;
            }
            JitterTimer += Time.deltaTime;
            if (JitterTimer > JitterDuration)
            {
                bBossBloodJitter = false;
                JitterTimer = 0.0f;
                BossBlood.SetPositionDirectly_ScreenPoint(OrginalBossBloodPos);
            }
            Vector3 JitterPos = new Vector3();
            JitterPos.x = UnityEngine.Random.Range(BossBloodJitterX.x, BossBloodJitterX.y) + OrginalBossBloodPos.x;
            JitterPos.y = UnityEngine.Random.Range(BossBloodJitterY.x, BossBloodJitterY.y) + OrginalBossBloodPos.y;
            JitterPos.z = 0;
            BossBlood.SetPositionDirectly_ScreenPoint(JitterPos);
        }

        /// <summary>
        /// 创建大血条
        /// </summary>
        private void CreateBossBlood()
        {
            if (null == BossBlood)
            {
                BossBlood = InstanceUBossBlood();
                if (null == BossBlood)
                {
                    Trace.LogError("UGUI -- BossBlood创建失败！");
                }
                else
                {
                    BloodCanvasNode bcn = new BloodCanvasNode();
                    BossBlood.Init(null, 0, BloodStyleType.Boss, bcn);
                    BossBlood.SetMaxValue(100);
                    BossBlood.SetCurValue(100);
                    UpdateBossBloodAndTowerSoldier(null, false);
                    BossBlood.SetPositionDirectly_ScreenPoint(OrginalBossBloodPos);
                }
            }
        }

        /// <summary>
        /// 更新大血条数据
        /// </summary>
        /// <param name="id">实体ID</param>
        /// <param name="name">实体名称</param>
        /// <param name="maxValue">最大血量</param>
        /// <param name="curValue">当前血量</param>
        /// <param name="tempValue">临时血量</param>
        private void UpdateBossBloodData(int id, string name, int maxValue, int curValue, int tempValue)
        {
            if (null == BossBlood)
            {
                return;
            }
            if (id != CurrentBossBloodHostID || id < 0)
            {
                if (CurrentBossBloodHostID < 0)
                {
                    UpdateBossBloodAndTowerSoldier(null, false);
                }
                return;
            }
            BossBlood.SetMaxValue(maxValue);
            BossBlood.SetCurValue(curValue);
            BossBlood.SetTempValue(tempValue);
            BossBlood.SetRoleName(name);
        }

        /// <summary>
        /// 是否拥有大血条
        /// </summary>
        /// <param name="mas"></param>
        /// <returns></returns>
        private bool IsHaveBossBlood(int mas)
        {
            return (mas & (int)ENTITY_MASKFLAG.MASKFLAG_SHOW_BIGBLOOD) == (int)ENTITY_MASKFLAG.MASKFLAG_SHOW_BIGBLOOD;
        }

        /// <summary>
        /// 改变大血条目标
        /// </summary>
        /// <param name="entityID">实体ID</param>
        /// <param name="ingoreBossMark">Mask</param>
        private void ChangeBossBloodTarget(int entityID, bool ingoreBossMark = false)
        {
            if (null == BossBlood)
            {
                return;
            }
            if (entityID < 0)
            {
                CurrentBossBloodHostID = -1;
                UpdateBossBloodAndTowerSoldier(null, false);
                return;
            }
            EntityView ev = EntityFactory.getEntityViewByID(entityID);
            if (null == ev || null == ev.StateMachine)
            {
                UpdateBossBloodAndTowerSoldier(null, false);
                CurrentBossBloodHostID = -1;
            }
            else
            {
                int mas = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);
                if (mas == 0 && !ingoreBossMark)
                {
                    return;
                }
                if (!IsHaveBossBlood(mas))
                {
                    return;
                }
                bool bHaveShowBossBlood = ingoreBossMark || IsHaveBossBlood(mas);
                if (!bHaveShowBossBlood)
                {
                    UpdateBossBloodAndTowerSoldier(null, false);
                    CurrentBossBloodHostID = -1;
                    return;
                }
                UpdateBossBloodAndTowerSoldier(ev, ev.StateMachine.GetVisible());
                CurrentBossBloodHostID = entityID;
                int maxHp = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP);
                int curHp = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP);
                int tempHp = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SHIELD);
                UpdateBossBloodData(entityID, ev.Property.CreatureName, maxHp, curHp, tempHp);
            }
        }

        private void EntityEnterBossBloodDistance(int entityID)
        {
            if (CurrentEntityEnterBossBloodDistanceID == entityID ||
                CurrentSelectedID > 0)
            {
                return;
            }
            CurrentEntityEnterBossBloodDistanceID = entityID;
            ChangeBossBloodTarget(CurrentEntityEnterBossBloodDistanceID);
        }

        private void EntityLeaveBossBloodDistance(int entityID)
        {
            //有选中目标的话，优先选中的目标
            if (CurrentSelectedID > 0)
            {
                return;
            }

            if (CurrentEntityEnterBossBloodDistanceID < 0)
            {
                return;
            }
            if (CurrentEntityEnterBossBloodDistanceID != entityID)
            {
                return;
            }
            CurrentEntityEnterBossBloodDistanceID = -1;
            ChangeBossBloodTarget(CurrentEntityEnterBossBloodDistanceID);
        }

        /// <summary>
        /// 大血条列表排序，先看优先级再看距离
        /// </summary>
        private void SortBossProityList()
        {
            if (BossBloodProityList.Count > 1)
            {
                BossBloodProityList.Sort((EntityView lef, EntityView rig) =>
                    {
                        //先看优先级再看距离
                        if (lef.BossBloodShowPriority == rig.BossBloodShowPriority)
                        {
                            return lef.BossBloodShowDistance.CompareTo(rig.BossBloodShowDistance);
                        }
                        return lef.BossBloodShowPriority.CompareTo(rig.BossBloodShowPriority);
                    });
            }
        }

        private void SwitchBossBloodTarget(U3D_Render.EntityView target)
        {
            if (target != null && target.gameObject)
            {
                bossBloodTarget = target.gameObject.transform;
            }
                
        }
        #endregion

        #region 防御塔小兵数量
        private EntityView m_TowerEV;
        private int m_nearTowerID = -1;
        private Vector3 m_nearTowerPos = Vector3.one;
        private int m_soldierCount = 0;
        private float m_towerSoldierCheckTimer = 0f;

        void EnterTower(EntityView ev)
        {
            if(m_nearTowerID != ev.ID)
            {
                m_TowerEV = ev;
                m_nearTowerID = ev.ID;
                m_nearTowerPos = ev.gameObject.transform.position;

                m_soldierCount = 0;
                IList<U3D_Render.EntityView> enemyList = MapFinderManager.Nearest(m_nearTowerPos, TowerStrikingDistance, SPELL.CampFlag.CampFlag_Friend);
                foreach(EntityView entity in enemyList)
                {
                    if(entity.Property != null && entity.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SEX) == (int)MONSTER_TYPE.MONSTER_TYPE_COMMON)
                    {
                        if (entity.StateMachine != null && entity.StateMachine.GetState() != GameLogic.EntityState.Dead)
                        {
                            float dis = Vector3.SqrMagnitude(entity.gameObject.transform.position - m_TowerEV.gameObject.transform.position);
                            if (dis <= TowerStrikingDistance * TowerStrikingDistance)
                                m_soldierCount++;
                        }
                    }
                }

                UWarTowerSoldierMsgData data = new UWarTowerSoldierMsgData(true, m_soldierCount);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARTOWER_UPDATE, data);
            }
        }

        void ExitTower()
        {
            if(m_nearTowerID != -1)
            {
                m_nearTowerID = -1;
                m_nearTowerPos = Vector3.one;
                m_soldierCount = 0;
                m_TowerEV = null;
                UWarTowerSoldierMsgData data = new UWarTowerSoldierMsgData(false, m_soldierCount);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARTOWER_UPDATE, data);
            }
        }

        void UpdateNearTowerSoldier()
        {
            m_towerSoldierCheckTimer += Time.deltaTime;
            if (m_towerSoldierCheckTimer >= TowerSoldierCheckTime)
            {
                m_towerSoldierCheckTimer = 0f;

                if (m_nearTowerID > 0)
                {
                    if (m_TowerEV == null || m_TowerEV.gameObject == null)
                    {
                        m_nearTowerID = -1;
                        m_nearTowerPos = Vector3.one;
                        m_soldierCount = 0;
                        UWarTowerSoldierMsgData msgData = new UWarTowerSoldierMsgData(false, m_soldierCount);
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARTOWER_UPDATE, msgData);
                        return;
                    }

                    m_nearTowerPos = m_TowerEV.gameObject.transform.position;
                    m_soldierCount = 0;
                    IList<U3D_Render.EntityView> enemyList = MapFinderManager.Nearest(m_nearTowerPos, TowerStrikingDistance, SPELL.CampFlag.CampFlag_Friend);
                    foreach (EntityView entity in enemyList)
                    {
                        if (entity.Property != null && entity.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SEX) == (int)MONSTER_TYPE.MONSTER_TYPE_COMMON)
                        {
                            if (entity.StateMachine != null && entity.StateMachine.GetState() != GameLogic.EntityState.Dead)
                            {
                                float dis = Vector3.SqrMagnitude(entity.gameObject.transform.position - m_TowerEV.gameObject.transform.position);
                                if (dis <= TowerStrikingDistance * TowerStrikingDistance)
                                    m_soldierCount++;
                            }
                        }
                    }

                    UWarTowerSoldierMsgData data = new UWarTowerSoldierMsgData(true, m_soldierCount);
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARTOWER_UPDATE, data);
                }
            }
        }
        #endregion

        #region 控制效果
        public void ShowDebuffProgressBar(int entityID, GameLogic.cmd_show_debuff_progress_bar progressBar)
        {
            if (!m_BloodMap.ContainsKey(entityID))
            {
                return;
            }
            m_BloodMap[entityID].ShowDebuffProgressBar(progressBar);
        }
        #endregion
    }
}
