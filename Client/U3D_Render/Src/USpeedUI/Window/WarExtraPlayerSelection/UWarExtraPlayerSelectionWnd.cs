using GameLogic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using U3D_Render;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UWidgets;
using UIWidgets;
using DG.Tweening;
using UnityEngine.EventSystems;

namespace USpeedUI.WarExtraPlayerSelection
{
    public class UWarExtraPlayerSelectionWnd : UIPopupWnd<UWarExtraPlayerSelectionView>, ITimerHanlder
    {
        protected const int GetNearPlayerTimerID = 0;
        protected int SearchNearPlayerRadius = 35;
        protected SHOW_CHOOSE_TARGET_TYPE m_eChooseTargetType;
        public int SelectionSpellID = 0;

        public Dictionary<Int32, EntityView> RangeActivityEntity = new Dictionary<int, EntityView>();

        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_EXTRAPLAYERSELECTION;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return UPath.UDynamicMobaState + "/WarExtraPlayerSelection/WarExtraPlayerSelection";
        }

        public override bool Init()
        {
            //订阅显示范围内敌方额外头像
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_EXTRAPLAYERSELECTION_SHOWENEMY, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);

            return base.Init();
        }

        public override void Destroy()
        {
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_EXTRAPLAYERSELECTION_SHOWENEMY, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);

            base.Destroy();

        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {

            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        this.LoadView();
                        this.SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_WAREND:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        this.UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_EXTRAPLAYERSELECTION_SHOWENEMY:
                    {
                        if (msgData == null || m_wndView == null)
                            return;

                        UIMsgCmdData ShowChooseTargetData = (UIMsgCmdData)msgData;

                        if (ShowChooseTargetData == null || ShowChooseTargetData.ptrParam == null)
                            return;

                        cmd_creature_ShowChooseTarget data = IntPtrHelper.toData<cmd_creature_ShowChooseTarget>(ShowChooseTargetData.ptrParam);
                        
                        bool toggleSelection = Convert.ToBoolean(data.nType);
                        m_eChooseTargetType = (SHOW_CHOOSE_TARGET_TYPE)data.nTargetType;
                        SearchNearPlayerRadius = data.nDistance == 0 ? 35 : data.nDistance;

                        RangeActivityEntity.Clear();
                        
                        if (toggleSelection)
                            GetAroundEnemy();
                        SetVisible(toggleSelection);
                        SelectionSpellID = data.nSpellID;
                        m_wndView.ToggleExtraPlayerSelection(toggleSelection);
                    }
                    break;

            }
        }

        protected override void PreSetVisible(bool _bVisible)
        {
            base.PreSetVisible(_bVisible);
            if (_bVisible)
            {
                TimerManager.SetTimer(this, GetNearPlayerTimerID, 0.5f);
            }
            else
            {
                TimerManager.KillTimer(this, GetNearPlayerTimerID);
            }
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case GetNearPlayerTimerID:
                    GetAroundEnemy();
                    break;
            }
        }

        protected void SearchNearestEntity(EntityView _ev,out List<EntityView> _entityList)
        {
            switch (m_eChooseTargetType)
            {
                case SHOW_CHOOSE_TARGET_TYPE.SHOW_CHOOSE_TARGET_TYPE_HERO_ENEMY:
                    {
                        _entityList = MapFinderManager.Nearest(_ev.StateMachine.transform.position, SearchNearPlayerRadius, SPELL.CampFlag.CampFlag_Enemy) as List<EntityView>;
                    }
                    break;
                case SHOW_CHOOSE_TARGET_TYPE.SHOW_CHOOSE_TARGET_TYPE_HERO_SELF:
                    {
                        _entityList = MapFinderManager.Nearest(_ev.StateMachine.transform.position, SearchNearPlayerRadius, SPELL.CampFlag.CampFlag_Friend) as List<EntityView>;
                    }
                    break;
                case SHOW_CHOOSE_TARGET_TYPE.SHOW_CHOOSE_TARGET_TYPE_HERO:
                    {
                        List<EntityView> tmpList = null;

                        tmpList = MapFinderManager.Nearest(_ev.StateMachine.transform.position, SearchNearPlayerRadius, SPELL.CampFlag.CampFlag_Enemy) as List<EntityView>;
                        _entityList = MapFinderManager.Nearest(_ev.StateMachine.transform.position, SearchNearPlayerRadius, SPELL.CampFlag.CampFlag_Friend) as List<EntityView>;

                        _entityList.AddRange(tmpList);
                    }
                    break;
                case SHOW_CHOOSE_TARGET_TYPE.SHOW_CHOOSE_TARGET_TYPE_HERO_SELF_EXCLUDE_SELF:
                    {
                        _entityList = MapFinderManager.Nearest(_ev.StateMachine.transform.position, SearchNearPlayerRadius, SPELL.CampFlag.CampFlag_Friend) as List<EntityView>;

                        if (_entityList.Contains(EntityFactory.MainHeroView))
                        {
                            _entityList.Remove(EntityFactory.MainHeroView);
                        }
                    }
                    break;
                case SHOW_CHOOSE_TARGET_TYPE.SHOW_CHOOSE_TARGET_TYPE_HERO_EXCLUDE_SELF:
                    {
                        List<EntityView> tmpList = null;

                        tmpList = MapFinderManager.Nearest(_ev.StateMachine.transform.position, SearchNearPlayerRadius, SPELL.CampFlag.CampFlag_Enemy) as List<EntityView>;
                        _entityList = MapFinderManager.Nearest(_ev.StateMachine.transform.position, SearchNearPlayerRadius, SPELL.CampFlag.CampFlag_Friend) as List<EntityView>;

                        _entityList.AddRange(tmpList);

                        if (_entityList.Contains(EntityFactory.MainHeroView))
                        {
                            _entityList.Remove(EntityFactory.MainHeroView);
                        }
                    }
                    break;
                default:
                    _entityList = null;
                    break;
            }
        }

        public void GetAroundEnemy()
        {
            EntityView ev = EntityFactory.getMainPlayer();
            if (ev == null)
            {
                return;
            }
            List<Int32> TmpEntityIDList = new List<int>(RangeActivityEntity.Keys);
            List<EntityView> AroundEntitiesList = null;

            SearchNearestEntity(ev, out AroundEntitiesList);

            if (AroundEntitiesList == null)
            {
                return;
            }

            Int32 nTmpEntityID = 0;
            foreach (EntityView Entity_Ev in AroundEntitiesList)
            {
                nTmpEntityID = Entity_Ev.ID;
                if (Entity_Ev == null || Entity_Ev.StateMachine == null || Entity_Ev.StateMachine.GetState() == GameLogic.EntityState.Dead || Entity_Ev.Type != ENTITY_TYPE.TYPE_PLAYER_ROLE)
                    continue;

                if (!RangeActivityEntity.ContainsKey(nTmpEntityID))
                    RangeActivityEntity.Add(nTmpEntityID, Entity_Ev);
                else
                    TmpEntityIDList.Remove(nTmpEntityID);
            }
            while (TmpEntityIDList.Count > 0)
            {
                nTmpEntityID = TmpEntityIDList[0];
                RangeActivityEntity.Remove(nTmpEntityID);
                TmpEntityIDList.RemoveAt(0);
            }
        }
    }

    public class UWarExtraPlayerSelectionView : UIBaseWndView
    {
        protected UWarExtraPlayerSelectionWnd m_Mediator;
        /// <summary>
        /// 显示头像挂接点
        /// </summary>
        protected RectTransform m_EntityAttachPoint;
        /// <summary>
        /// 头像打包资源
        /// </summary>
        protected ResNode m_SelectionItemResNode;
        /// <summary>
        /// 头像挂接列表
        /// </summary>
        protected Dictionary<int, UWarExtraPlayerSelectionItem> m_dicAttachItems = new Dictionary<int, UWarExtraPlayerSelectionItem>();


        [Tooltip("显示对象挂接对象名字")]
        public string EntityAttachPointName = "EntityAttachPoint";
        [Tooltip("显示对象预制体")]
        public string SelectionItemPrefabPath = "UI/Prefabs/DynamicState_Moba/WarExtraPlayerSelection/WarExtraPlayerSelectionItem";
        /// <summary>
        /// 圆形布局工具
        /// </summary>
        public URadialLayout radiaLayout = null;
        /// <summary>
        /// 是否可以播放布局动画
        /// </summary>
        protected bool m_CanPlayLayoutAnim = false;
        /// <summary>
        /// 动画持续时间
        /// </summary>
        public float LayoutAnimDuration = 1.0f;

        public override bool Init(IUIWnd wnd)
        {
            m_Mediator = wnd as UWarExtraPlayerSelectionWnd;
            m_EntityAttachPoint = GetTransform().FindChild(EntityAttachPointName) as RectTransform;
            if (m_EntityAttachPoint == null)
            {
                Trace.LogError("获取EntityAttachPointName:" + EntityAttachPointName + " 失败");
                //没找到就创建一个
                Type[] InitComponent = { typeof(RectTransform) , typeof(URadialLayout) };
                var attachPointGO = new GameObject(EntityAttachPointName, InitComponent);
                attachPointGO.transform.SetParent(GetTransform());
                m_EntityAttachPoint = attachPointGO.transform as RectTransform;
            }
            radiaLayout = m_EntityAttachPoint.GetComponent<URadialLayout>();

            m_SelectionItemResNode = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,SelectionItemPrefabPath);
            if (m_SelectionItemResNode == null)
            {
                Trace.LogError("获取SelectionItemPrefabPath：" + SelectionItemPrefabPath + " 预制体失败");
                return false;
            }
            return base.Init(wnd);
        }

        public override void Destroy()
        {
            base.Destroy();
            AssetBundleManager.DeleteAssets(ref m_SelectionItemResNode, true);
        }

        public override void Start()
        {
            base.Start();
            //m_EntityAttachPoint = GetTransform().FindChild(EntityAttachPointName) as RectTransform;
            //if (m_EntityAttachPoint == null)
            //{
            //    Trace.LogError("获取EntityAttachPointName:" + EntityAttachPointName + " 失败");
            //    //没找到就创建一个
            //    var attachPointGO = new GameObject(EntityAttachPointName, typeof(RectTransform));
            //    attachPointGO.transform.SetParent(GetTransform());
            //    m_EntityAttachPoint = attachPointGO.transform as RectTransform;
            //}
            //m_SelectionItemResNode = AssetBundleManager.GetAssets(AssetType.Asset_PrefabSelectionItemPrefabPath);
            //if (m_SelectionItemResNode == null || m_SelectionItemResNode.ResObj == null)
            //{
            //    Trace.LogError("获取SelectionItemPrefabPath：" + SelectionItemPrefabPath + " 预制体失败");
            //    return;
            //}

            //m_SelectionItemPrefab = (m_SelectionItemResNode.ResObj as GameObject).GetComponent<UWarExtraPlayerSelectionItem>();
        }
        public override void Update()
        {
            base.Update();
            //TestFindNearObject();
            if(m_Mediator.IsVisible())
            {
                //if(aroundList.Count > 0)
                //{
                //    foreach(var obj in aroundList)
                //    {
                //        AttachEntityPoint(obj);
                //    }
                //}
                if (m_Mediator.RangeActivityEntity.Count > 0)
                {
                    foreach (var obj in m_Mediator.RangeActivityEntity.Values)
                    {
                        AttachEntityPoint(obj);
                    }
                }
                //foreach (var removeObj in attachList)
                //{
                //    if (!aroundList.Contains(removeObj))
                //    {
                //        DettachEntityPoint(removeObj);
                //        break;
                //    }                    
                //}
                foreach (var removeObj in m_dicAttachItems.Values)
                {
                    if (!m_Mediator.RangeActivityEntity.ContainsKey(removeObj.SelectionEntityID))
                    {
                        DeattachEntityPoint(removeObj.SelectionEntityView);
                        break;
                    }
                }
            }
        }
        public void ToggleExtraPlayerSelection(bool _bToggle)
        {
            if (_bToggle)
                BeginPlayerSelection();
            else
                EndPlayerSelection();
        }
        protected void BeginPlayerSelection()
        {            
            if (m_Mediator.RangeActivityEntity.Count == 0)
                return;
            foreach(var entity in this.m_Mediator.RangeActivityEntity)
            {
                    AttachEntityPoint(entity.Value);
            }
           
        }
        protected void EndPlayerSelection()
        {
            if(m_dicAttachItems.Count> 0)
            {                
                while(m_dicAttachItems.Count>0)
                {
                    var removeItem = m_dicAttachItems.Values.ToList()[0];
                    DeattachEntityPoint(removeItem.SelectionEntityView);
                }
            }
        }

        //public List<UWarExtraTest> TestObjects = new List<UWarExtraTest>();
        //public float FindDistance = 30f;
        //public List<UWarExtraTest> NearObjects = new List<UWarExtraTest>();
        //List<UWarExtraTest> aroundList = new List<UWarExtraTest>();

        //List<UWarExtraTest> attachList = new List<UWarExtraTest>();

        //public void TestFindNearObject()
        //{
        //    var myScreenPos = RectTransformUtility.WorldToScreenPoint(UISystem.Instance.GetCamera(), GetTransform().position);
        //    Vector2 rtScreenPos;
        //    List<UWarExtraTest> tmpList = new List<UWarExtraTest>(NearObjects);

        //    aroundList.Clear();
        //    foreach (var rt in TestObjects)
        //    {
        //        rtScreenPos = RectTransformUtility.WorldToScreenPoint(UISystem.Instance.GetCamera(), rt.GetRectTransform.position);
        //        if((myScreenPos - rtScreenPos).magnitude< FindDistance)
        //        {
        //            aroundList.Add(rt);
        //        }
        //    }
        //    if(aroundList.Count == 0)
        //    {
        //        return;
        //    }

        //    foreach(var rtt in aroundList)
        //    {
        //        if (!NearObjects.Contains(rtt))
        //            NearObjects.Add(rtt);
        //        else
        //            tmpList.Remove(rtt);
        //    }

        //    while (tmpList.Count > 0)
        //    {
        //        var tmprt = tmpList[0];
        //        NearObjects.Remove(tmprt);

        //        tmpList.RemoveAt(0);
        //    }

        //}
        //protected void AttachEntityPoint(UWarExtraTest newTest)
        //{
        //    if (attachList.Contains(newTest))
        //        return;
        //    var HeroSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_HEROBEARING, 1, 4, newTest.headID);
        //    var entitySelectionItem = ResNode.InstantiateRes<UWarExtraPlayerSelectionItem>(m_SelectionItemPrefab.gameobjct);
        //    entitySelectionItem.SelectionEntityHead.sprite = HeroSprite;
        //    entitySelectionItem.SelectionEntityID = newTest.entityID;
        //    entitySelectionItem.transform.SetParent(m_EntityAttachPoint, false);
        //    attachList.Add(newTest);
        //    if (attachList.Count > 0)
        //        PlayLayoutAnim();
        //}

        //protected void DettachEntityPoint(UWarExtraTest newTest)
        //{
        //    if (!attachList.Contains(newTest))
        //        return;
        //    var count = m_EntityAttachPoint.childCount;
        //    for (int i = 0; i < count; i++)
        //    {
        //        var com = m_EntityAttachPoint.GetChild(i).GetComponent<UWarExtraPlayerSelectionItem>();
        //        if (com.SelectionEntityID == newTest.entityID)
        //        {
        //            attachList.Remove(newTest);
        //            ResNode.DestroyRes(ref com.gameObject);
        //            break;
        //        }
        //    }
        //    ResetLayoutAnim();
        //}

        protected void AttachEntityPoint(EntityView _ev)
        {
            if (m_dicAttachItems.ContainsKey(_ev.ID))
                return;

            int nEntityHeroID = _ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
            var HeroSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_HEROBEARING, 1, 4, nEntityHeroID);
            int MaxHp = _ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP);
            int CurHp = _ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP);

            var entitySelectionItem = m_SelectionItemResNode.InstanceMainRes<UWarExtraPlayerSelectionItem>();
            entitySelectionItem.SelectionEntityHead.sprite = HeroSprite;
            entitySelectionItem.SelectionEntityID = _ev.ID;
            entitySelectionItem.SelectionEntityView = _ev;
            entitySelectionItem.SelectionSpellID = m_Mediator.SelectionSpellID;
            entitySelectionItem.SetHP(CurHp, MaxHp);
            entitySelectionItem.transform.SetParent(m_EntityAttachPoint,false);

            CreatureStateMachine playerMachine = (CreatureStateMachine)_ev.StateMachine;
            if (playerMachine)
            {
                int formType = playerMachine.creaturePropety.GetNumProp(ENTITY_PROPERTY.PROPERTY_FORMTYPE);
                if (formType == (int)FORM_TYPE.FORM_HATHPACE)
                {
                    entitySelectionItem.Standpoint.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MinimapIcon, "Icon_gaotai", GetUIWnd().GetID());
                    entitySelectionItem.Standpoint.SetNativeSize();
                    entitySelectionItem.Standpoint.gameObject.SetActive(true);
                }
                else if (formType == (int)FORM_TYPE.FORM_SKY)
                {
                    entitySelectionItem.Standpoint.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MinimapIcon, "Icon_chibang", GetUIWnd().GetID());
                    entitySelectionItem.Standpoint.SetNativeSize();
                    entitySelectionItem.Standpoint.gameObject.SetActive(true);
                }
                else
                {
                    entitySelectionItem.Standpoint.gameObject.SetActive(false);
                }
            }
            else
            {
                entitySelectionItem.Standpoint.gameObject.SetActive(false);
            }

            m_dicAttachItems.Add(_ev.ID, entitySelectionItem);
                PlayLayoutAnim();
        }

        protected void DeattachEntityPoint(EntityView _ev)
        {
            if (!m_dicAttachItems.ContainsKey(_ev.ID))
                return;

            var count = m_EntityAttachPoint.childCount;
            for (int i = 0; i < count; i++)
            {
                var com = m_EntityAttachPoint.GetChild(i).GetComponent<UWarExtraPlayerSelectionItem>();
                if (com.SelectionEntityID == _ev.ID)
                {
                    m_dicAttachItems.Remove(_ev.ID);
                    var temp = com.gameObject;
                    ResNode.DestroyRes(ref temp);
                    break;
                }
            }
            ResetLayoutAnim();
        }

        protected void PlayLayoutAnim()
        {
            if (m_dicAttachItems.Count > 0)
            {
                if (!m_CanPlayLayoutAnim)
                {
                    DOTween.To(() => radiaLayout.fDistance, x => radiaLayout.fDistance = x, 150, LayoutAnimDuration).SetAutoKill(true).OnUpdate(() => radiaLayout.CalculateRadial());
                    m_CanPlayLayoutAnim = true;
                }
            }
        }
        protected void ResetLayoutAnim()
        {
            if (m_dicAttachItems.Count <= 0)
            {
                radiaLayout.fDistance = 0;
                m_CanPlayLayoutAnim = false;
            }
                
        }
    }

    //class UWarExtraTest:UCustomUIBehaviour
    //{
    //    public int headID;
    //    public int entityID;
    //}

    public class UWarExtraPlayerSelectionItem : UCustomUIBehaviour, IPointerClickHandler
    {
        public int SelectionEntityID;
        public EntityView SelectionEntityView ;
        public Image SelectionEntityHead = null;
        public Image SelectionEntityHP = null;
        public int SelectionSpellID = 975;
        public Image Standpoint;

        protected override void Start()
        {
            base.Start();
        }

        public void Update()
        {
            SetHP(SelectionEntityView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP), SelectionEntityView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP));
        }

        public void SetHP(float _nCur, float _nMax)
        {
            this.SelectionEntityHP.fillAmount = _nCur / _nMax;
        }

        public void OnPointerClick(PointerEventData eventData)
        {
            SkillCtrl sc = EntityFactory.MainHero.GetComponent<SkillCtrl>();
            if (sc != null)
            {
                sc.UIUseSkill(SelectionSpellID, SelectionEntityID);
            }

        }

        public void OnPointerEnter(PointerEventData eventData)
        {

        }

        public void OnPointerDown(PointerEventData eventData)
        {
        }
    }
}
