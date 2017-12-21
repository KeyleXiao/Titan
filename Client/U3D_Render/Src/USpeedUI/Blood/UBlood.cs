using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using U3D_Render;
using DG.Tweening;
using USpeedUI.UWidgets;

namespace USpeedUI.Blood
{
    public enum DebuffType
    {
        Debuff_Immobilized,
        Debuff_Silence,
        Debuff_Dizziness,
        Debuff_Decelerate,
    }

    public class UBlood_DebuffProgressBar : MonoBehaviour
    {
        public Image Circle;
        private Tweener m_tweener;
        private float nAnimTime;

        public bool Valid
        {
            get { return m_valid; }
        }
        private bool m_valid;

        public bool Show
        {
            get { return this.gameObject.activeInHierarchy; }
        }

        public void Clear()
        {
            ShowProgressBar(false);
            if (m_tweener != null && m_tweener.IsPlaying())
            {
                m_tweener.Kill();
            }
            m_tweener = null;
            m_valid = false;
        }

        //控制状态进度条策划显示需求：
        //3.受到同种类状态时根据持续时间控制是否替换显示； 
        //（1.后加的状态持续时间大于当前剩余时间，替换当前，并重新读条 
        //（2.后加的状态持续时间小于当前剩余时间，不替换
        public void SetData(float nTime, bool show)
        {
            if(Circle.fillAmount * nAnimTime < nTime)
            {
                if (m_tweener != null && m_tweener.IsPlaying())
                {
                    m_tweener.Kill();
                }
                Circle.fillAmount = 1;
                ShowProgressBar(show);
                m_tweener = Circle.DOFillAmount(0, nTime / 1000).OnComplete( ()=> { this.gameObject.SetActive(false); });
                nAnimTime = nTime;
                m_valid = true;
            }
        }

        public void Remove(float nTime)
        {
            if (nAnimTime != nTime)
                return;

            Clear();
        }

        public void ShowProgressBar(bool val)
        {
            this.gameObject.SetActive(val);
        }
    }

    /// <summary>
    /// UBlood类
    /// </summary>
    public class UBlood :MonoBehaviour
    {
        #region 外部成员
        public int ID    
        {
            get
            {
                return m_ID;
            }
        }

        public EntityView Entity
        {
            get
            {
                return m_Entity;
            }
            set
            {
                m_Entity = value;
                if(!EntityView.isNullOrEmpty(m_Entity))
                {
                    m_ID = m_Entity.ID;
                }
                else
                {
                    m_ID = -1;
                }
            }
        }

        public BloodCanvasNode CanvasNode
        {
            get
            {
                return m_CanvasNode;
            }
        }

        public bool ISHero
        {
            get
            {
                return m_Entity == EntityFactory.MainHeroView;
            }
        }

        public BloodStyleType BloodType
        {
            get
            {
                return m_BloodType;
            }
        }

        [System.NonSerialized]
        public ResNode resNode = null;
        #endregion

        #region 显示状态
        //实体是否死亡
        private bool m_bDead = false;
        //实体是否可见，由实体逻辑控制，如mask刷新、缓存时控制隐藏等
        private bool m_bVisible = true;
        //皮肤可见性,由皮肤及状态机可见性决定
        private bool m_bEnable = true;
        //是否超出隐藏距离
        private bool m_IsOutOfVisibleDistance = false;
        //玩家是否主动关闭血条
        private bool m_IsClosed = false;
        //最终可见性，由上面共同决定
        private bool m_FinalVisible = true;
        //是否显示血量数值信息
        private bool m_BloodInfoVisible = false;
        #endregion

        #region 内部成员
        private int m_ID = -1;
        private int m_MaxValue = 100;
        private int m_CurValue = 100;
        private int m_TempValue = 0;
        private int m_MpValue = 100;
        private int m_MpMaxValue = 100;
        private int m_CamType = 0;
        private int m_Level = 1;
        private string m_RoleName = "";
        private int m_buffShowCount
        {
            get
            {
                int count = 0;
                foreach(var child in DebuffList)
                {
                    if (child.gameObject.activeInHierarchy)
                        count++;
                }
                return count;
            }
        }

        private U3D_Render.EntityView m_Entity = null;
        private UBillBoard m_BillBoard = null;
        private static Material BloodMaterial = null;
        private BloodCanvasNode m_CanvasNode;              //画布节点
        private BloodStyleType m_BloodType = BloodStyleType.Player;
        private BloodStyleInfo m_StyleInfo = null;         //血条样式信息
        private bool bInit = false;
        private const float DistanceCorreFactor = 0.23f;  //距离缩放矫正，用来纠正距离远之后，血条的高度
        private Sequence m_Sequence = null;
        #endregion

        #region 控件
        public Image m_BloodImg;
        public Image m_MpImg;
        public Image m_BloodShieldImg;
        public Image m_BloodTempImg;
        
        public Text m_LevelTex;
        public Text m_RoleNameTex;
        public Text m_BloodNumTex;
        public Transform m_ProgressBarRoot;
        public int m_ProgressBarShowMax = 3;
        [SerializeField]
        public List<UBlood_DebuffProgressBar> DebuffList;
        #endregion

        #region 数据更新标志位
        private bool bNeedToUpdateHpValue = false;
        private bool bNeedToUpdateTempHp = false;
        private bool bNeedToUpdateMpValue = false;
        #endregion       
        
        #region 创建并初始化
        public void Init(U3D_Render.EntityView ev,int cam, BloodStyleType bt, BloodCanvasNode node)
        {
            ResetData();
            m_Entity = ev;
            if(!U3D_Render.EntityView.isNullOrEmpty(ev))
            {
                m_ID = m_Entity.ID;
            }
            m_BloodType = bt;
            m_CanvasNode = node;

            m_StyleInfo = UBloodManager.Instance.m_BloodStyleList.Find((x) => { return x.bloodType == m_BloodType; });

            if(null == m_StyleInfo || null == m_BloodImg)
                return;

            bInit = true;

            m_BloodInfoVisible = UBloodManager.Instance.NumInfoVisible;


            if (null != m_BloodNumTex)
            {
                m_BloodNumTex.text = "";
                m_BloodNumTex.gameObject.SetActive(m_BloodInfoVisible);
            }

            if (m_BillBoard)
            {
                m_BillBoard.gameObject.SetActive(m_FinalVisible);
            }

            if(m_BloodType == BloodStyleType.Boss)
            {
                this.gameObject.SetActive(m_bVisible);
            }

            if (m_Sequence == null)
                m_Sequence = DOTween.Sequence();

            SetCampType(cam);

            if(m_Entity != null)
            {
                CreatBillBoard();
                ChangeCanvasNode();
            }
        }

        private void CreatBillBoard()
        {
            if (!bInit)
                return;

            if (null == BloodMaterial)
            {
                BloodMaterial = new Material(AssetBundleManager.GetShader("Effect/BillBorad"));
                BloodMaterial.SetTexture("_MainTex", UBloodManager.Instance.BloodRenderTexture);
                BloodMaterial.name = "BloodMaterial";
                //BloodMaterial.hideFlags = HideFlags.HideAndDontSave;
            }
            else
                BloodMaterial.SetTexture("_MainTex", UBloodManager.Instance.BloodRenderTexture);

            if (null == m_BillBoard)
                m_BillBoard = UBillBoard.CreateBillBorad((int)m_CanvasNode.vSize.x, (int)m_CanvasNode.vSize.y, BloodMaterial, "Blood -- temp");
            
            if (null != m_BillBoard)
                m_BillBoard.gameObject.SetActive(m_FinalVisible);
        }

        private void ChangeCanvasNode()
        {
            if (!bInit || !m_BillBoard)
                return;

            m_BillBoard.SetPixelSize((int)m_CanvasNode.vSize.x, (int)m_CanvasNode.vSize.y);
            m_BillBoard.SetScale(m_CanvasNode.scale);
            Vector2 texStart = UBloodManager.Instance.TransformTextureOffsetFromPixelPos(m_CanvasNode.start);
            Vector2 texEnd = UBloodManager.Instance.TransformTextureOffsetFromPixelPos(m_CanvasNode.end);
            m_BillBoard.SetTextureOffset(texStart, texEnd);
            if(null != m_Entity && m_Entity.IsValid)
            {
                ChangeParent(m_Entity.gameObject.transform);
            }
               
        }

        public void ChangeParent(Transform paren)
        {
            if (bInit && m_BillBoard)
            {
                m_BillBoard.transform.parent = paren;
                m_BillBoard.transform.localPosition = new Vector3(0,2,0);
                m_BillBoard.transform.localRotation = Quaternion.identity;
            }
        }

        /// <summary>
        /// 直接设置血条的位置，这里设置的是血条在canvas下的坐标，只用于大血条
        /// </summary>
        /// <param name="pos"></param>
        public void SetPositionDirectly_ScreenPoint(Vector2 pos)
        {
            if (!bInit)
                return;

            RectTransform rt = this.GetComponent<RectTransform>();
            if (rt != null)
                rt.anchoredPosition = pos;
        }
        #endregion

        #region 清除
        public void Clear()
        {
            if (!bInit)
                return;

            ResetData();
            if (m_BillBoard)
            {
                m_BillBoard.Clear();
                m_BillBoard = null;
            }
            m_ID = -1;
            resNode = null;
            bInit = false;
        }

        private void ResetData()
        {
            
            m_MaxValue = 100;
            m_CurValue = 100;
            m_TempValue = 0;
            m_MpValue = 100;
            m_MpMaxValue = 100;
            m_CamType = 0;
            m_Level = 0;
            m_RoleName = "";

            m_StyleInfo = null;
            m_Entity = null;
            m_Sequence = null;

            m_BloodType = BloodStyleType.Player;

            m_bDead = false;
            m_bVisible = true;
            m_bEnable = true;
            m_IsOutOfVisibleDistance = false;
            m_IsClosed = false;
            m_FinalVisible = true;

            if (null != m_BloodImg)
                m_BloodImg.fillAmount = 1;
            if (null != m_BloodTempImg)
                m_BloodTempImg.fillAmount = 0;
            if (null != m_BloodShieldImg)
                m_BloodShieldImg.fillAmount = 0;
            if (null != m_MpImg)
                m_MpImg.fillAmount = 1;
            if (null != m_RoleNameTex)
                m_RoleNameTex.text = "";
            if (null != m_LevelTex)
                m_LevelTex.text = "";
            if (null != m_BloodNumTex)
                m_BloodNumTex.text = "";

            foreach(UBlood_DebuffProgressBar bar in DebuffList)
            {
                bar.Clear();
            }
        }
        #endregion

        #region 更新
        public void OnUpdate()
        {
            if (!bInit)
                return;

            if(m_BillBoard != null)
                m_BillBoard.gameObject.layer = LayerMask.NameToLayer(Config.LayerTopName);

            UpdateTransform();
            UpdateData();
        }

        public void RefreshTexture()
        {
            if (null != m_BillBoard)
            {
                m_BillBoard.Clear();
                m_BillBoard = null;
            }

            if (!EntityView.isNullOrEmpty(m_Entity))
            {
                CreatBillBoard();
                ChangeCanvasNode();

                bool bShow = m_bVisible && !m_IsOutOfVisibleDistance && m_bEnable && !m_IsClosed && !m_bDead;
                if (m_BillBoard != null)
                    m_BillBoard.gameObject.SetActive(bShow);
            }
        }

        private void UpdateTransform()
        {
            SoldierCamera soldierCam = SoldierCamera.MainInstance<SoldierCamera>();
            if (!m_bEnable || EntityView.isNullOrEmpty(m_Entity) || !m_BillBoard || !soldierCam || !soldierCam.CurrentCamera)
            {
                return;
            }

            CreatureProperty pero = m_Entity.Property;
            if (!pero)
            {
                return;
            }
            //计算Y偏移值
            float offsetY = m_BillBoard.HeightInMeter * 0.2f + pero.CreatureHeightInMeters + pero.BloodYOffset;
            m_BillBoard.transform.localPosition = new Vector3(0, offsetY, 0);


            //算出相机到实体的距离
            float hideInDis = pero.TopInfoHideInDistance;
            //上帝视野要多加
            if (soldierCam.InGodViewMode)
            {
                hideInDis += UBloodManager.Instance.extendHideDistanceInGodView;
            }

            Vector3 camPos = soldierCam.CurrentCamera.transform.position;
            Vector3 billBoardPos = m_BillBoard.transform.position;
            camPos.y = billBoardPos.y;
            float dis = (camPos - billBoardPos).sqrMagnitude;

            if (dis > hideInDis * hideInDis)//距离已经超过了实体头顶信息隐藏距离，隐藏
            {
                m_IsOutOfVisibleDistance = true;
                UpdateVisible();
                return;
            }
            else
            {
                if (m_IsOutOfVisibleDistance)
                {
                    m_IsOutOfVisibleDistance = false;
                    UpdateVisible();
                }
            }
        }
        private void UpdateData()
        {
            if (bNeedToUpdateHpValue || bNeedToUpdateTempHp)
            {
                UpdateHpNum();
                bNeedToUpdateHpValue = false;
                bNeedToUpdateTempHp = false;
            }

            if (bNeedToUpdateMpValue)
            {
                UpdateMpNum();
                bNeedToUpdateMpValue = false;
            }
        }        
        #endregion

        #region 数据更新函数
        public void SetDead(bool bDead)
        {
            if (!bInit)
                return;
            if (m_bDead != bDead)
            {
                m_bDead = bDead;
                UpdateVisible();
            }
        }

        public void SetEnable(bool bEnable)
        {
            if (!bInit)
                return;
            if (m_bEnable != bEnable)
            {
                m_bEnable = bEnable;
                UpdateVisible();
            }
        }

        public void SetVisible(bool bVisible)
        {
            if (!bInit)
                return;
            if (m_bVisible != bVisible)
            {
                m_bVisible = bVisible;
                UpdateVisible();
            }
        }

        public void SetBossBloodVisible(bool bVisible)
        {
            if (!bInit)
                return;
            if (m_bVisible != bVisible)
            {
                m_bVisible = bVisible;
                if (m_BloodType == BloodStyleType.Boss)
                {
                    this.gameObject.SetActive(m_bVisible);
                }
            }
        }

        public void SetClosed(bool bClose)
        {
            if (!bInit)
                return;
            if (m_IsClosed != bClose)
            {
                m_IsClosed = bClose;
                UpdateVisible();
            }
        }

        //更新血条数值信息显示状态
        public void UpdateBloodInfoVisible(bool val)
        {
            if (!bInit || m_BloodInfoVisible == val)
                return;

            m_BloodInfoVisible = val;
            if (null != m_BloodNumTex)
            {
                m_BloodNumTex.gameObject.SetActive(m_BloodInfoVisible);
            }
        }

        //设置血条颜色
        public void ChangeBloodColor(bool bIsChange)
        {
        }

        public void SetMaxValue(int v)
        {
            if (!bInit || m_MaxValue == v)
                return;

            m_MaxValue = v;
            bNeedToUpdateHpValue = true;
        }

        public void SetCurValue(int v)
        {
            if (!bInit || m_CurValue == v)
                return;

            m_CurValue = v;
            bNeedToUpdateHpValue = true;
        }

        public void SetTempValue(int v)
        {
            if (!bInit || m_TempValue == v)
                return;

            m_TempValue = v;
            bNeedToUpdateTempHp = true;
        }

        public void SetMpValue(int v)
        {
            if (!bInit || m_MpValue == v)
                return;

            m_MpValue = v;
            bNeedToUpdateMpValue = true;
        }

        public void SetMpMaxValue(int v)
        {
            if (!bInit || m_MpMaxValue == v)
                return;

            m_MpMaxValue = v;
            bNeedToUpdateMpValue = true;
        }

        public void SetLevel(int lev)
        {
            if (!bInit || m_Level == lev)
                return;

            m_Level = lev;
            UpdateLevel();
        }

        public void SetCampType(int cam)
        {
            if (!bInit || m_CamType == cam)
                return;

            m_CamType = cam;
            UpdateCampType();
        }

        public void SetRoleName(string name)
        {
            if (!bInit || m_RoleName.Equals(name))
                return;

            m_RoleName = name;
            UpdateRoleName();
        }

        //控制状态进度条策划需求：
        //新增控制显示效果，通过控制buff调用效果显示对应控制状态UI提示控制时间 
        //1.分4类显示状态：定身，减速，封魔，眩晕， 
        //2.显示时在血条右边显示，最多同时显示3个， 
        //（1.当同时受到4种状态“减速”不显示，直到显示中的3个状态减少为2个显示时，减速显示出现继续显示（如果此时没有已没有减速效果则不显示） 
        //（2.当收到3种状态，并且其中一种为“减速”，此时再收到第4种状态则将减速替换为第4中状态显示
        public void ShowDebuffProgressBar(GameLogic.cmd_show_debuff_progress_bar progressBarData)
        {
            if (m_BloodType != BloodStyleType.Player)
                return;

            int type = progressBarData.nType;
            if (type < 0 || type > DebuffList.Count + 1 || null == DebuffList[type])
                return;

            if(progressBarData.nOpenFlag > 0)
            {
                AddDebuff(type, progressBarData.nTime);
            }
            else
            {
                RemoveDebuff(type, progressBarData.nTime);
            }
        }

        private void AddDebuff(int deBuffType, int buffTime)
        {
            if (deBuffType == (int)DebuffType.Debuff_Decelerate)
            {
                bool showBar = false;

                if (DebuffList[deBuffType].Show || m_buffShowCount < m_ProgressBarShowMax)
                    showBar = true;

                DebuffList[deBuffType].SetData(buffTime, showBar);
            }
            else
            {
                if(!DebuffList[deBuffType].Show && m_buffShowCount >= m_ProgressBarShowMax)
                {
                    if(DebuffList[(int)DebuffType.Debuff_Decelerate] != null)
                        DebuffList[(int)DebuffType.Debuff_Decelerate].ShowProgressBar(false);
                }
                DebuffList[deBuffType].SetData(buffTime, true);
            }
        }

        private void RemoveDebuff(int deBuffType, int nTime)
        {
            DebuffList[deBuffType].Remove(nTime);

            if (DebuffList[(int)DebuffType.Debuff_Decelerate] != null)
            {
                if(DebuffList[(int)DebuffType.Debuff_Decelerate].Valid && !DebuffList[(int)DebuffType.Debuff_Decelerate].Show)
                    DebuffList[(int)DebuffType.Debuff_Decelerate].ShowProgressBar(true);
            }
        }
        #endregion

        #region 控件数据更新
        private void UpdateVisible()
        {
            bool bVisible = !m_bDead && m_bVisible && !m_IsOutOfVisibleDistance && m_bEnable && !m_IsClosed;
            if (m_FinalVisible != bVisible)
            {
                m_FinalVisible = bVisible;
                if (m_BillBoard)
                {
                    m_BillBoard.gameObject.SetActive(m_FinalVisible);
                }
            }
        }

        private void UpdateCampType()
        {
            //换算成m_StyleInfo.bloodColorArry索引,索引规则：0：self; 1:friend; 2:enemy; 3-n:MCAMP
            int nArryIndex = m_CamType;
            if(StageManager.Instance.CurPermissionState != EStage_PermissionState.ESPS_OB)
            {
                if(EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
                {
                    int mainHeroCamp = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP);
                    if(m_CamType == mainHeroCamp)
                        nArryIndex = 1;
                    else
                        nArryIndex = 2;
                }
                if (UBloodManager.Instance.PkTargetID > 0)
                    nArryIndex = 2;
            }
            else
            {
                if (nArryIndex < 1)
                    nArryIndex = 2;
            }

            if (ISHero)
                nArryIndex = 0;

            nArryIndex = Mathf.Clamp(nArryIndex, 0, m_StyleInfo.bloodColorArry.Length - 1);
            m_BloodImg.color = m_StyleInfo.bloodColorArry[nArryIndex];
            if(null != m_RoleNameTex)
                m_RoleNameTex.color = m_StyleInfo.bloodColorArry[nArryIndex];
        }

        private void UpdateLevel()
        {
            if (m_StyleInfo.bShowLevel && m_LevelTex != null)
            {
                m_LevelTex.text = m_Level.ToString();
            }
        }

        private void UpdateRoleName()
        {
            if (m_StyleInfo.bShowRoleName && m_RoleNameTex != null)
            {
                m_RoleNameTex.text = m_RoleName;
            }
        }

        private void UpdateMpNum()
        {
            if (m_StyleInfo.bShowMp && null != m_MpImg)
            {
                m_MpImg.DOFillAmount((float)m_MpValue / m_MpMaxValue, 0.3f);
            }
        }

        private void UpdateHpNum()
        {
            if (m_StyleInfo.bShowShield)
            {
                if (m_BloodImg == null || m_BloodShieldImg == null || m_BloodTempImg == null)
                    return;

                if (m_TempValue != 0)
                {
                    m_Sequence.Append(m_BloodImg.DOFillAmount((float)m_CurValue / (m_TempValue + m_MaxValue), 0.3f))
                                .Insert(1, m_BloodTempImg.DOFillAmount((float)m_CurValue / (m_TempValue + m_MaxValue), 1.0f))
                                .Insert(1, m_BloodShieldImg.DOFillAmount((float)(m_TempValue + m_CurValue) / (m_TempValue + m_MaxValue), 0.3f))
                                .OnComplete(()=> m_Sequence.Kill());
                }
                else
                {
                    m_Sequence.Append(m_BloodImg.DOFillAmount((float)m_CurValue / m_MaxValue, 0.3f))
                                .Insert(1, m_BloodTempImg.DOFillAmount((float)m_CurValue / m_MaxValue, 1.0f))
                                .Insert(1, m_BloodShieldImg.DOFillAmount(0, 0.3f))
                                .OnComplete(() => m_Sequence.Kill());
                }
            }
            else
            {
                if (m_BloodImg == null || m_BloodTempImg == null)
                    return;
                m_Sequence.Append(m_BloodImg.DOFillAmount((float)m_CurValue / m_MaxValue, 0.3f))
                            .Insert(1, m_BloodTempImg.DOFillAmount((float)m_CurValue / m_MaxValue, 1.0f))
                            .OnComplete(() => m_Sequence.Kill());
            }

            if (m_StyleInfo.bShowBloodNum && null != m_BloodNumTex)
            {
                string info = m_CurValue + "/" + m_MaxValue;
                m_BloodNumTex.text = info;
            }
        }
        #endregion
    }
}
