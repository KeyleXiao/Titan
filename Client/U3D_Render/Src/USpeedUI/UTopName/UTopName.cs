using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using U3D_Render;
using UnityEngine.UI;
using USpeedUI.UWidgets;


namespace USpeedUI
{
    public class UTopName : MonoBehaviour
    {
        #region 外部成员
        [System.NonSerialized]
        public ResNode resNode;

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
                if (!EntityView.isNullOrEmpty(m_Entity))
                {
                    m_ID = m_Entity.ID;
                }
                else
                {
                    m_ID = -1;
                }
            }
        }

        public TopNameNode CanvasNode
        {
            get
            {
                return m_CanvasNode;
            }
        }
        #endregion

        #region 内部成员
        private int m_ID;
        private EntityView m_Entity;
        private UBillBoard m_BillBoard = null;
        private static Material TopNameMaterial = null;
        private TopNameNode m_CanvasNode;

        private string m_RoleName = "";
        private string m_ClanName = "";
        private string m_KinName = "";
        private int m_RankLv = -1;
        private int m_Sex = -1;

        private bool m_Enable = true;
        private bool m_IsOutOfVisibleDistance = false;
        private bool FinalVisible = true;
        private bool m_IsPKMode = false;

        #endregion

        public Image RankIcon;
        public Image SexIcon;
        public Text RoleNameTxt;
        public Text ClanNameTxt;
        public Text KinNameTxt;

        public float hideInDis = 30.0f;

        public string RoleName { get { return m_RoleName; } }
        public string KinName { get { return m_KinName; } }

        public void Init(EntityView ev)
        {
            Reset();

            m_Entity = ev;

            if (!EntityView.isNullOrEmpty(ev))
            {
                m_ID = ev.ID;
                ChangeParent(ev.transform);
            }
         
            if (m_BillBoard)
            {
                //初始化的时候给个相对值，不然创建后第一次计算比例后,会突然改变一下比例,体验会很差
                m_BillBoard.transform.localScale = new Vector3(1.5f,0.5f,1);
                m_BillBoard.gameObject.SetActive(true);

                m_BillBoard.gameObject.layer = LayerMask.NameToLayer(Config.LayerTopName);
            }

            RefreshActivity();
        }

        public void CreatBillBoard()
        {
            if (null == TopNameMaterial)
            {
                TopNameMaterial = new Material(AssetBundleManager.GetShader("Effect/BillBorad"));
                TopNameMaterial.SetTexture("_MainTex", UTopNameManager.Instance.TopNameRenderTexture);
                TopNameMaterial.name = "TopNameMaterial";
                //TopNameMaterial.hideFlags = HideFlags.HideAndDontSave;
            }
            else
                TopNameMaterial.SetTexture("_MainTex", UTopNameManager.Instance.TopNameRenderTexture);

            if (m_BillBoard == null)
            {
                m_BillBoard = UBillBoard.CreateBillBorad((int)m_CanvasNode.vSize.x, (int)m_CanvasNode.vSize.y, TopNameMaterial, "TopName -- temp");
            }
        }

        /// <summary>
        /// 根据节点信息，算出贴图纹理像素起始和结束值，在rt贴图上取像素赋值给公告板
        /// </summary>
        /// <param name="trs"></param>
        public void ChangeCanvasNode(TopNameNode trs)
        {
            if (!m_BillBoard)
            {
                return;
            }
            m_CanvasNode = trs;

            m_BillBoard.SetPixelSize((int)m_CanvasNode.vSize.x, (int)m_CanvasNode.vSize.y);
            //根据在画布上的位置算出贴图上对应的位置
            Vector2 texStart = UTopNameManager.TransformTextureOffsetFromPixelPos(m_CanvasNode.start);
            Vector2 texEnd = UTopNameManager.TransformTextureOffsetFromPixelPos(m_CanvasNode.end);

            m_BillBoard.SetTextureOffset(texStart, texEnd);
        }

        public void ChangeParent(Transform paren)
        {
            if (m_BillBoard)
            {
                m_BillBoard.transform.parent = paren;
                m_BillBoard.transform.localPosition = new Vector3(0, 2, 0);
                m_BillBoard.transform.localRotation = Quaternion.identity;
            }
        }

        public void RefreshTexture()
        {
            Clear();
            CreatBillBoard();
            ChangeCanvasNode(m_CanvasNode);
            if(null != m_Entity)
                ChangeParent(m_Entity.transform);
            if (null != m_BillBoard)
                m_BillBoard.gameObject.SetActive(FinalVisible);
        }

        public void OnUpdate()
        {
            UpdateTransform();

            if(m_BillBoard != null)
                m_BillBoard.gameObject.layer = LayerMask.NameToLayer(Config.LayerTopName);
        }

        public bool GetEnable()
        {
            return m_Enable;
        }

        public void SetEnable(bool enable)
        {
            if (m_Enable != enable)
            {
                m_Enable = enable;
                UpdateVisible();
            }
        }

        public void SetPKMode(bool bIsPK)
        {
            if (m_IsPKMode != bIsPK)
            {
                m_IsPKMode = bIsPK;
                UpdateVisible();
            }
        }

        public void Clear(bool fullClear = false)
        {
            if (fullClear)
            {
                m_ID = -1;
                resNode = null;
            }            
            if (m_BillBoard)
            {
                m_BillBoard.Clear();
                m_BillBoard = null;
            }
        }

        public void Reset()
        {
            m_RoleName = "";
            m_KinName = "";
            m_ClanName = "";
            m_RankLv = -1;
            m_Sex = -1;

            RoleNameTxt.text = "";
            KinNameTxt.text = "";
            ClanNameTxt.text = "";
            RankIcon.GetComponent<Image>().enabled = false;
            RankIcon.transform.parent.gameObject.SetActive(false);
            SexIcon.GetComponent<Image>().enabled = false;
            SexIcon.transform.parent.gameObject.SetActive(false);
            ClanNameTxt.enabled = false;
            KinNameTxt.enabled = false;

            m_Enable = true;
            m_IsOutOfVisibleDistance = false;
            m_IsPKMode = false;
            FinalVisible = true;
        }

        public void ClearName()
        {
            SetRoleName("");
            SetClanName("");
            SetKinName("");
        }

        public void RefreshActivity()
        {
            RankIcon.gameObject.SetActive(UTopNameManager.Instance.bShowRoleName);
            SexIcon.gameObject.SetActive(UTopNameManager.Instance.bShowRoleName);
            RoleNameTxt.gameObject.SetActive(UTopNameManager.Instance.bShowRoleName);
            KinNameTxt.gameObject.SetActive(UTopNameManager.Instance.bShowKinName);
            ClanNameTxt.gameObject.SetActive(UTopNameManager.Instance.bShowClanName);
        }

        #region 设置属性
        public void SetRoleName(string roleName)
        {
            m_RoleName = roleName;
            RoleNameTxt.text = m_RoleName;
        }

        public void SetClanName(string clanName)
        {
            m_ClanName = clanName;

            if (m_ClanName == "")
            {
                ClanNameTxt.text = "";
                ClanNameTxt.enabled = false;
            }
            else
            {
                ClanNameTxt.text = "<" + m_ClanName + ">";
                ClanNameTxt.enabled = true;
            }
        }

        public void SetKinName(string KinName)
        {
            m_KinName = KinName;

            if (m_KinName == "")
            {
                KinNameTxt.text = "";
                KinNameTxt.enabled = false;
            }
            else
            {
                KinNameTxt.text = "*" + m_KinName + "*";
                KinNameTxt.enabled = true;
            }
        }

        public void SetRankLv(int lv)
        {
            m_RankLv = lv;

            RankIcon.GetComponent<Image>().enabled = false;
            RankIcon.transform.parent.gameObject.SetActive(false);
            if (m_RankLv > 0)
            {
                RankIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, WndID.WND_ID_CHART, 2, m_RankLv);
                RankIcon.GetComponent<Image>().enabled = true;
                RankIcon.transform.parent.gameObject.SetActive(true);
            }
        }

        public void SetSex(int sex)
        {
            m_Sex = sex;
            SexIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_CHART, m_Sex + 1);
            SexIcon.GetComponent<Image>().enabled = true;
            SexIcon.transform.parent.gameObject.SetActive(true);
        }
        #endregion

        private void UpdateTransform()
        {
            if (!m_Enable ||  EntityView.isNullOrEmpty(m_Entity) || !m_BillBoard)
            {
                return;
            }
            CreatureProperty pero = m_Entity.Property;
            if (!pero)
            {
                return;
            }
            //Vector3 off = Vector3.zero;
            //float RealY = pero.CreatureHeightInMeters;
            //off.y = RealY;
            //off.y += pero.TopNameYOffset * m_BillBoard.PixelToMeterScale;
            //m_BillBoard.transform.localPosition = off;

            float offsetY = m_BillBoard.HeightInMeter * 0.2f + pero.CreatureHeightInMeters + pero.TopNameYOffset;
            m_BillBoard.transform.localPosition = new Vector3(0, offsetY, 0);

            float dis = 0;
            if (null != Initialize.Instance.SoliderTargetGO)
            {
                Vector3 camPos = Initialize.Instance.SoliderTargetGO.transform.position;
                Vector3 billBoardPos = m_BillBoard.transform.position;
                camPos.y = billBoardPos.y;
                dis = (camPos - billBoardPos).sqrMagnitude;
            }
            if (dis > hideInDis * hideInDis)//距离超过hideinDis，隐藏
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

        private void UpdateVisible()
        {
            bool bVisible = !m_IsOutOfVisibleDistance && m_Enable && !m_IsPKMode;
            if (FinalVisible != bVisible)
            {
                FinalVisible = bVisible;
                if (m_BillBoard)
                {
                    m_BillBoard.gameObject.SetActive(FinalVisible);
                }
            }
        }


    }
}
