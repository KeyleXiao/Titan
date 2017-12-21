using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using U3D_Render;
using SPELL;
using War;
using GameLogic;
using U3D_Render.Utility;
using UnityEngine.EventSystems;
using DataCenter;
using USpeedUI.UEffect;
using DG.Tweening;
using U3D_Render.CameraControl;
using UIWidgets;

namespace USpeedUI.WarMinimapWnd
{
    enum MinimapSignalSoundType
    {
        MSST_ATTACK = 47,
        MSST_RETREAT,
        MSST_DANGER,
        MSST_ONWAY,
        MSST_HELPME,
        MSST_LOSTEYE,
    }

    public class WarMinimapWnd : UIBaseWnd<WarMinimapWndView>
    {
        private bool m_bLoginingMoba;

        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_MINIMAP;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "DynamicState_Moba/WarMinimapView/Minimap";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOADSCENE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_ADDOBJECT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_REMOVEOBJECT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_UPDATEOBJECTVISIBLITY, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_OBJECTATTACHEFFECT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_ADDSTATICOBJECT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_REMOVESTATICOBJECT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_CLEARSTATICOBJECT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_STATICOBJECTCOUNTDOWN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_UPDATEOBJECTPOS, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_ADDMAPSIGNAL, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_SETOBJECTRANGETIP, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_AUTOMOVE_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_AUTOMOVE_HIDE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_STARTSPREADNOXSAFEZONE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_SETNOXSAFEZONE, this);
            
            m_bLoginingMoba = false;

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOADSCENE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_ADDOBJECT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_REMOVEOBJECT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_UPDATEOBJECTVISIBLITY, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_OBJECTATTACHEFFECT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_ADDSTATICOBJECT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_REMOVESTATICOBJECT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_CLEARSTATICOBJECT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_STATICOBJECTCOUNTDOWN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_UPDATEOBJECTPOS, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_ADDMAPSIGNAL, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_SETOBJECTRANGETIP, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_AUTOMOVE_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_AUTOMOVE_HIDE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_STARTSPREADNOXSAFEZONE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MINIMAP_SETNOXSAFEZONE, this);

        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        m_bLoginingMoba = true;
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOADSCENE:
                    {
                        if (m_bLoginingMoba)
                        {
                            SetVisible(true);
                            if (m_wndView != null)
                                m_wndView.OnOpen();

                            m_bLoginingMoba = false;
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_WAREND:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        if (m_wndView != null)
                            m_wndView.OnClose();
                        UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MINIMAP_ADDOBJECT:
                    {
                        if (m_wndView != null)
                        {
                            MinimapObjData data = msgData as MinimapObjData;
                            m_wndView.AddObject(data.ev);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MINIMAP_REMOVEOBJECT:
                    {
                        if (m_wndView != null)
                        {
                            MinimapObjData data = msgData as MinimapObjData;
                            m_wndView.RemoveObject(data.ev);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MINIMAP_UPDATEOBJECTVISIBLITY:
                    {
                        if (m_wndView != null)
                        {
                            MinimapObjData data = msgData as MinimapObjData;
                            m_wndView.UpdateObjectVisiblity(data.ev);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MINIMAP_OBJECTATTACHEFFECT:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.ObjectAttachEffect(msgData as MinimapObjectAttachEffect);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MINIMAP_ADDSTATICOBJECT:
                    {
                        if (m_wndView != null)
                        {
                            MinimapStaticObjData data = msgData as MinimapStaticObjData;
                            m_wndView.AddStaticObject(data.nObjType);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MINIMAP_REMOVESTATICOBJECT:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.RemoveStaticObject(msgData as MinimapStaticObjData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MINIMAP_CLEARSTATICOBJECT:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.ClearStaticObject();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MINIMAP_STATICOBJECTCOUNTDOWN:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetStaticObjectCountdown(msgData as MinimapStaticObjCountdown);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MINIMAP_UPDATEOBJECTPOS:
                    {
                        if (m_wndView != null)
                        {
                            if (msgData != null)
                            {
                                MinimapObjData data = msgData as MinimapObjData;
                                m_wndView.UpdateObjectPos(data.ev, data.pos);
                            }
                            else
                            {
                                m_wndView.UpdateObjectPos();
                            }
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MINIMAP_ADDMAPSIGNAL:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.AddMapSignal(msgData as MinimapAddSignal);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MINIMAP_SETOBJECTRANGETIP:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetObjectRangeTip(msgData as MinimapSetObjRangeTip);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_AUTOMOVE_SHOW:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.DrawAutoMoveLine();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_AUTOMOVE_HIDE:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.EraseAutoMoveLine();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MINIMAP_STARTSPREADNOXSAFEZONE:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.StartSpreadNoxSafeZone();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MINIMAP_SETNOXSAFEZONE:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.SetNoxSafeZone();
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class WarMinimapDynamicItem : MonoBehaviour
    {
        public Image backImage;
        public Image frontImage;
        public Image positionImage;

        private RectTransform rectTrans;
        private WarMinimapWndView wndView;
        private bool m_bIsHero;
        public bool IsHero { get { return m_bIsHero; } }
        private bool m_bVisible;
        private int m_nSight;
        private int m_nCamp;
        private bool m_bPosInited;
        private GameObject m_rangeTip;
        
        private bool m_bIsMainHero;
        private float m_fNextUpdateRotTime = 0;

        public virtual bool Init(WarMinimapWndView view, EntityView ev, ref SMinimapIcon sIconInfo, int nRelation)
        {
            rectTrans = transform as RectTransform;
            wndView = view;
            m_bIsHero = (ev.createinfo.byIsHero > 0);
            m_bIsMainHero = (EntityFactory.MainHeroID == ev.ID);
            m_bVisible = true;
            m_nSight = ev.createinfo.nSight;
            m_nCamp = ev.createinfo.byCamp;
            m_bPosInited = false;

            String strFg = null;
            String strBg = null;
            if (nRelation == 0)
            {
                strFg = sIconInfo.szFriendCampFg;
                strBg = sIconInfo.szFriendCampBg;
            }
            else if (nRelation == 1)
            {
                strFg = sIconInfo.szEnemyCampFg;
                strBg = sIconInfo.szEnemyCampBg;
            }
            else if (nRelation == 2)
            {
                strFg = sIconInfo.szMidCampFg;
                strBg = sIconInfo.szMidCampBg;
            }
            else
            {
                if (StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB)
                {
                    strFg = null;
                    strBg = null;
                }
                else
                {
                    strFg = sIconInfo.szMainHeroFg;
                    strBg = sIconInfo.szMainHeroBg;
                }
            }

            Transform layer = null;
            int nIsPlayer = ((ENTITY_TYPE)ev.createinfo.EntityType == ENTITY_TYPE.TYPE_PLAYER_ROLE) ? 1 : 0;
            // 确定所在图层
            if (nIsPlayer != 0)
            {
                if (nRelation == 3) // 主角
                {
                    layer = wndView.selfIconLayer;
                }
                else if (nRelation == 0) // 友方英雄
                {
                    layer = wndView.friendIconLayer;
                }
                else if (nRelation == 1) // 敌方英雄
                {
                    layer = wndView.enemyIconLayer;
                }
                else // 容错
                {
                    layer = wndView.monsterIconLayer;
                }
            }
            // 推塔小龙图层放在主角和敌方之间，在此特殊处理
            else if (sIconInfo.nIcon == 2022) // 2022是MinimapIcon.csv里面的小地图图标ID
            {
                layer = wndView.monster2IconLayer;
            }
            else
            {
                layer = wndView.monsterIconLayer;
            }

            WndID wndID = wndView.GetUIWnd().GetID();
            if (strBg == null || strBg.Length == 0 || strBg == "empty")
            {
                backImage.gameObject.SetActive(false);
            }
            else
            {
                backImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MinimapIcon, strBg, wndID);
                if (backImage.sprite != null)
                {
                    backImage.SetNativeSize();
                    backImage.gameObject.SetActive(true);
                }
                else
                {
                    backImage.gameObject.SetActive(false);
                }
            }

            if (strFg == null || strFg.Length == 0 || strFg == "empty")
            {
                frontImage.gameObject.SetActive(false);
            }
            else
            {
                frontImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MinimapIcon, strFg, wndID);
                if (frontImage.sprite != null)
                {
                    frontImage.SetNativeSize();
                    frontImage.gameObject.SetActive(true);
                }
                else
                {
                    frontImage.gameObject.SetActive(false);
                }
            }

            positionImage.gameObject.SetActive(false);

            transform.SetParent(layer, false);
            transform.rotation = Quaternion.identity;
            SetVisible(false);

            return true;
        }

        public void SetVisible(bool bVisible)
        {
            m_bVisible = bVisible;
            if (m_bPosInited == false && bVisible == true)
                return;

            gameObject.SetActive(bVisible);
        }

        public virtual void Destroy()
        {
            var temp = gameObject;
            ResNode.DestroyRes(ref temp);
        }

        public void AttachEffect(int nEffectType)
        {
            // effectType 1--传送 2--进攻建筑物 3--防守建筑物 4--遭遇战斗
            if (nEffectType == 4)
            {
                GameObject animObject = wndView.CreateAnim(2);
                if (animObject == null)
                    return;

                animObject.transform.SetParent(transform, false);
            }
        }

        public void SetRangeTip(bool bAdd, int nRange)
        {
            if (bAdd)
            {
                if (m_rangeTip)
                    return;

                m_rangeTip = wndView.TipPrefab.InstanceMainRes();
                if (m_rangeTip)
                {
                    RectTransform trans = m_rangeTip.transform as RectTransform;
                    if (trans != null)
                    {
                        float fWidth = wndView.SceneLengthToMinimap(nRange);
                        trans.sizeDelta = new Vector2(fWidth * 2, fWidth * 2);
                    }
                    m_rangeTip.transform.SetParent(transform, false);
                }
            }
            else
            {
                if (m_rangeTip)
                {
                    ResNode.DestroyRes(ref m_rangeTip);
                    m_rangeTip = null;
                }
            }
        }

        public void Update()
        {
            if (!m_bIsMainHero || !Initialize.mainCam)
            {
                return;
            }

            if (m_fNextUpdateRotTime < Time.time)
            {
                if (EntityFactory.MainHero != null)
                    rectTrans.rotation = Quaternion.Euler(0f, 0f, wndView.CurRotateAngle - EntityFactory.MainHero.transform.eulerAngles.y);

                m_fNextUpdateRotTime = Time.time + 0.33f;
            }
        }

        public void UpdatePosInfo(EntityView ev, ref EntityPosAngleInfo info)
        {
            if (!m_bPosInited)
            {
                if (info.pos.x != 0f || info.pos.z != 0f)
                {
                    m_bPosInited = true;
                    if (m_bVisible)
                        SetVisible(true);
                }
            }
            Vector2 pos = wndView.SceneToMinimap(new Vector2(info.pos.x, info.pos.z));
            rectTrans.anchoredPosition = pos;

            if (m_bIsMainHero || ev.ID == EntityFactory.MainHeroID)
            {
                m_bIsMainHero = true;
                info.dir = Camera.main.transform.eulerAngles;
            }

            if (m_bIsHero && EntityFactory.MainHero != null)
            {
                 rectTrans.rotation = Quaternion.Euler(0f, 0f, wndView.CurRotateAngle - EntityFactory.MainHero.transform.eulerAngles.y);
            }
        }

        public virtual void UpdateHpInfo(float fHpPercent)
        {
        }

        public bool IsHaveSight(int nCamp)
        {
            if (m_bVisible == false)
                return false;

            if (StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB)
            {
                if (m_nSight <= 0 || (LogicDataCenter.warOBDataManager.CurMainCamp != 0 && LogicDataCenter.warOBDataManager.CurMainCamp != m_nCamp))
                    return false;
            }
            else
            {
                // 不是我方阵营 没视野
                if (m_nSight <= 0 || m_nCamp != nCamp)
                    return false;
            }
			
            if (!m_bPosInited)
                return false;

            return true;
        }

        public void DrawCircle(ref byte[] data, int nWidth, int nHeight, int nOffsetX, int nOffsetY)
        {
            Vector2 pos = rectTrans.anchoredPosition;
            int nPosX = (int)pos.x + nOffsetX;
            int nPosY = (int)pos.y + nOffsetY;

            // 网上复制的算法，Bresenham_Circle
            int r = m_nSight;
            int x = 0;
            int y = r;
            int d = 3 - 2 * r;
            CirclePlot(ref data, nWidth, nHeight, nPosX, nPosY, x, y);
            while (x < y)
            {
                if (d < 0)
                {
                    d = d + 4 * x + 6;
                }
                else
                {
                    d = d + 4 * (x - y) + 10;
                    y--;
                }
                x++;
                CirclePlot(ref data, nWidth, nHeight, nPosX, nPosY, x, y);
            }
        }

        void CirclePlot(ref byte[] data, int nWidth, int nHeight, int xc, int yc, int x, int y)
        {
            // (-x,y)-(x,y)
            FillCircleLine(ref data, nWidth, nHeight, xc - x, xc + x, yc + y, m_nSight - y);
            // (-y,x)-(y,x)
            FillCircleLine(ref data, nWidth, nHeight, xc - y, xc + y, yc + x, m_nSight - x);
            // (x,-y)-(-x,-y)
            FillCircleLine(ref data, nWidth, nHeight, xc - x, xc + x, yc - y, m_nSight - y);
            // (-y,-x)-(y,-x)
            FillCircleLine(ref data, nWidth, nHeight, xc - y, xc + y, yc - x, m_nSight - x);
        }

        void FillCircleLine(ref byte[] data, int nWidth, int nHeight, int xMin, int xMax, int y, int nYTransLv)
        {
            if (y >= 0 && y < nHeight)
            {
                for (int i = xMin; i <= xMax; ++i)
                {
                    if (i < 0 || i >= nWidth)
                        continue;

                    int nXTransLv = Mathf.Min(i - xMin, xMax - i);
                    int nTransLv = Mathf.Min(nXTransLv, nYTransLv);
                    byte nValue = 0xff;
                    if (nTransLv < 5)
                    {
                        nValue = (byte)(255 * (float)(nTransLv + 1) / 5.0f);
                    }
                    data[y * nWidth + i] = (byte)Mathf.Max(data[y * nWidth + i], nValue);
                }
            }
        }

        public Vector2 GetMinimapPos()
        {
            return rectTrans.anchoredPosition;
        }

        public float GetMinimapDistance(WarMinimapDynamicItem _other)
        {
            if (_other == null)
                return -1;

            return Vector2.Distance(GetMinimapPos(), _other.GetMinimapPos());
        }

        public void UpdateHeroHathpace(FORM_TYPE formType)
        {
            // 小地图上不显示自身的高台标识
            if (m_bIsHero)
            {
                positionImage.gameObject.SetActive(false);
                return;
            }

            String identification;
            if (formType == FORM_TYPE.FORM_HATHPACE) // 高台
            {
                identification = "Icon_gaotai";
            }
            else if (formType == FORM_TYPE.FORM_SKY) // 空中
            {
                identification = "Icon_chibang";
            }
            else // 其他情况，按地面处理
            {
                identification = null;
            }

            if (identification != null)
            {
                positionImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MinimapIcon, identification, wndView.GetUIWnd().GetID());
                if (positionImage.sprite != null)
                {
                    positionImage.SetNativeSize();
                    RectTransform transform = positionImage.GetComponent<RectTransform>();
                    Vector2 rect = transform.sizeDelta;
                    transform.sizeDelta = new Vector2(rect.x * 0.6f, rect.y * 0.6f);
                    positionImage.gameObject.SetActive(true);
                }
                else
                {
                    positionImage.gameObject.SetActive(false);
                }
            }
            else
            {
                positionImage.gameObject.SetActive(false);
            }
        }
    }

    // 配HP遮罩的动态对象
    public class WarMinimapDynamicItemHpMask : WarMinimapDynamicItem
    {
        public Image maskImage;

        private Texture2D m_maskTexture;

        public override bool Init(WarMinimapWndView view, EntityView ev, ref SMinimapIcon sIconInfo, int nRelation)
        {
            if (!base.Init(view, ev, ref sIconInfo, nRelation))
                return false;

            if (frontImage.sprite == null)
                return false;

            m_maskTexture = new Texture2D(1, 32, TextureFormat.Alpha8, false, true);
            m_maskTexture.name = "WarMinimapDynamicItemHpMask";
            maskImage.sprite = Sprite.Create(m_maskTexture, new Rect(0, 0, 1f, 32f), new Vector2(0f, 16f));
            maskImage.rectTransform.sizeDelta = frontImage.rectTransform.sizeDelta;

            return true;
        }


        public override void Destroy()
        {

            ResNode.DestroyRes(ref m_maskTexture);
            m_maskTexture = null;

            base.Destroy();
        }

        public override void UpdateHpInfo(float fHpPercent)
        {
            if (fHpPercent < 0f || fHpPercent > 1f)
                return;

            byte[] data = new byte[32];
            int nValue = (int)(32f * fHpPercent);
            for (int i = 0; i < 32; ++i)
            {
                if (i < nValue)
                    data[i] = 0xff;
                else
                    data[i] = 0;
            }
            m_maskTexture.LoadRawTextureData(data);
            m_maskTexture.Apply();
        }
    }

    public class WarMinimapStaticItem : MonoBehaviour
    {
        public Image bgImage;
        public Image iconImage;
        public Text countdownText;

        private int m_nCountdown;
        private float m_fStartCountdownTime;
        private SMinimapStaticIcon m_sIconInfo;
        private WarMinimapWndView m_wndView;
        private UEffectParamBase m_effectParam;

        public bool Init(WarMinimapWndView wndView, ref SMinimapStaticIcon sIconInfo)
        {
            m_wndView = wndView;
            m_sIconInfo = sIconInfo;
            iconImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MinimapIcon, sIconInfo.szIconName, wndView.GetUIWnd().GetID());
            iconImage.SetNativeSize();
            iconImage.gameObject.SetActive(true);
            countdownText.gameObject.SetActive(false);

            m_nCountdown = 0;
            m_fStartCountdownTime = 0f;
            
            if (transform.parent != wndView.monsterIconLayer)
            {
                transform.SetPosition(wndView.SceneToMinimap(new Vector2(sIconInfo.nX, sIconInfo.nZ)));
                transform.SetParent(wndView.monsterIconLayer, false);
            }
            transform.rotation = Quaternion.identity;

            bgImage.gameObject.SetActive(false);
            // 大龙、小龙、黄金怪特殊处理（加光效）
            if (sIconInfo.szIconName == "monster1" || sIconInfo.szIconName == "monster2" || sIconInfo.szIconName == "monster9")
            {
                if (sIconInfo.szIconName == "monster1")
                    bgImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MinimapIcon, "BgBigDragon", wndView.GetUIWnd().GetID());
                else
                    bgImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MinimapIcon, "BgSmallDragon", wndView.GetUIWnd().GetID());

                bgImage.SetNativeSize();
                bgImage.rectTransform.localScale = new Vector3(0, 0, 0);
                bgImage.gameObject.SetActive(true);
                bgImage.rectTransform.DOScale(1.0f, 1.0f).SetLoops(4, LoopType.Restart).OnComplete(() => OnBgAnimEnd());
                //bgImage.rectTransform.DOScale(0.3f, 1.0f).SetLoops(4, LoopType.Yoyo).OnComplete(() => OnBgAnimEnd());
            }

            return true;
        }

        public void OnBgAnimEnd()
        {
            bgImage.gameObject.SetActive(false);
            if (m_sIconInfo.szIconName != "monster1" && m_sIconInfo.szIconName != "monster2")
                return;

            USpeedUILayerElement layerElement = iconImage.gameObject.AddComponent<USpeedUILayerElement>();
            layerElement.UILayerElementType = USpeedUILayerElement.EUILayerElementType.ELET_UI;
            layerElement.SortingLayerOrderOffset = 1;

            ClearEffect();
            UEffectPrefabType prefabType = new UEffectPrefabType();
            if (m_sIconInfo.szIconName == "monster1")
            {
                prefabType = UEffectPrefabType.UEPT_MiniMap_BigDragonFire;
            }
            else
            {
                prefabType = UEffectPrefabType.UEPT_MiniMap_SmallDragonFire;
            }
            Vector3 pos = Vector3.zero;
            m_effectParam = new UEffectPrefabParam(prefabType, GetComponent<RectTransform>(), pos, 0, false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_effectParam);
        }

        public void SetVisible(bool bVisible)
        {
            gameObject.SetActive(bVisible);
        }

        public void Destroy()
        {
            // 结束 消除光效
            ClearEffect();
            var temp1 = gameObject;
            ResNode.DestroyRes(ref temp1);
        }

        public void Update()
        {
            if (m_nCountdown > 0)
            {
                float fPassedTime = Time.unscaledTime - m_fStartCountdownTime;
                int nLeftTime = m_nCountdown - (int)fPassedTime;
                if (nLeftTime > 0)
                {
                    int nMin = nLeftTime / 60;
                    int nSec = nLeftTime % 60;
                    countdownText.text = String.Format("{0}:{1:D2}", nMin, nSec);
                }
                else
                {
                    SetVisible(false);
                }
            }
        }

        public void SetCountDown(int nCountdown)
        {
            if (nCountdown <= 0)
                return;

            nCountdown /= 1000;
            m_nCountdown = nCountdown;
            m_fStartCountdownTime = Time.unscaledTime;
            iconImage.gameObject.SetActive(false);
            countdownText.gameObject.SetActive(true);

            // 实体死亡，消除光效
            ClearEffect();
        }

        private void ClearEffect()
        {
            if (m_effectParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_effectParam);
            }
        }
    }

    public class WarMinimapAnime : MonoBehaviour
    {
        public Image animImage;
        public Sprite[] animSprite;
        public int frameRate;
        public float lifeTime;

        private float m_fStartTime;
        private float m_fFrameTime;

        public void Start()
        {
            if (frameRate <= 0)
                frameRate = 8;

            m_fStartTime = Time.unscaledTime;
            m_fFrameTime = 1f / (float)frameRate;
        }

        public void Update()
        {
            float fCurLifeTime = Time.unscaledTime - m_fStartTime;
            if (fCurLifeTime > lifeTime)
            {
                var temp1 = gameObject;
                ResNode.DestroyRes(ref temp1);
                return;
            }

            int nCurFrame = (int)(fCurLifeTime / m_fFrameTime);
            nCurFrame %= animSprite.Length;
            animImage.sprite = animSprite[nCurFrame];
        }
    }

    public partial class WarMinimapWndView : UIBaseWndView, IEventHandler, IBeginDragHandler, IDragHandler, IEndDragHandler, IPointerDownHandler
    {
        public RectTransform mapFrame;
        public Image mapBaseImage;
        public Image mapDarkImage;
        public Image mapMaskImage;
        public Image godeyeRectImage;
        public RectTransform monsterIconLayer;
        public RectTransform friendIconLayer;
        public RectTransform enemyIconLayer;
        public RectTransform monster2IconLayer; // 在自己和敌方图层之间，用来放置推塔小龙图标
        public RectTransform selfIconLayer;
        public RectTransform animLayer;
        public ULineRenderer lineRenderer;
        public Button AlertBtnTrans;     // 警告按钮
        public Button MarkBtnTrans;      // 标记按钮

        private Vector3[] autoMovePointsScreen;
        private float mStartTime; // 用来记录警告按钮和标记按钮的单击时间

        public ResNode TipPrefab
        {
            get
            {
                return rangeTipPrefabRes;
            }
        }
        
        private ResNode dynamicItemPrefabRes = null;
        private ResNode dynamicHpItemPrefabRes = null;
        private ResNode staticItemPrefabRes = null;
        private ResNode[] animPrefabRes = null;
        private ResNode rangeTipPrefabRes = null;

        private Dictionary<int, WarMinimapDynamicItem> m_dynamicItemList;
        private Dictionary<int, WarMinimapStaticItem> m_staticItemList;
        private List<WarMinimapDynamicItem> m_dynamicItemMaskList;
        private int m_nCurMapID;
        private int m_MainHeroCamp;
        private float m_fMinimapWidth;
        private float m_fMinimapHeight;
        private SMinimapInfo m_sCurMinimapInfo;
        private float m_fCurRotateAngle;
        public float CurRotateAngle { get { return m_fCurRotateAngle; } }
        private bool m_bGodEyeDraging;
        private bool m_bGodEyeMode;
        private Texture2D m_maskTexture;
        private float m_fNextUpdateMaskTime;
        private byte[] MiniMapMaskData = null;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            dynamicItemPrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/DynamicState_Moba/WarMinimapView/MinimapDynamicItem");
            dynamicHpItemPrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/DynamicState_Moba/WarMinimapView/MinimapDynamicHpItem");
            staticItemPrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/DynamicState_Moba/WarMinimapView/MinimapStaticItem");
           
            rangeTipPrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/DynamicState_Moba/WarMinimapView/MinimapRangeTipItem");


            animPrefabRes = new ResNode[3];
            //这里的顺序是没改之前的值,而且只有三个

            animPrefabRes[0] = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/DynamicState_Moba/WarMinimapView/MinimapAlertAnim");
            animPrefabRes[1] = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/DynamicState_Moba/WarMinimapView/MinimapMarkAnim");
            animPrefabRes[2] = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/DynamicState_Moba/WarMinimapView/MinimapEncouterAnim");

            m_dynamicItemList = new Dictionary<int, WarMinimapDynamicItem>();
            m_staticItemList = new Dictionary<int, WarMinimapStaticItem>();
            m_dynamicItemMaskList = new List<WarMinimapDynamicItem>();

            m_nCurMapID = 0;
            m_MainHeroCamp = -1;
            m_fMinimapWidth = 0f;
            m_fMinimapHeight = 0f;
            m_sCurMinimapInfo = new SMinimapInfo();
            m_fCurRotateAngle = 0f;
            m_bGodEyeDraging = false;
            m_bGodEyeMode = false;

            AlertBtnTrans.gameObject.GetComponent<UTooltipTrigger>().SetText(UTooltipParamName.BodyText, ULocalizationService.Instance.Get("UIView", "WarMiniMap", "AlertAndMarkTooltip"));
            MarkBtnTrans.gameObject.GetComponent<UTooltipTrigger>().SetText(UTooltipParamName.BodyText, ULocalizationService.Instance.Get("UIView", "WarMiniMap", "AlertAndMarkTooltip"));
            Init_Zone();

            InitUIWhenOB();

            return true;
        }

        private void InitUIWhenOB()
        {
            if (StageManager.Instance.CurPermissionState != EStage_PermissionState.ESPS_OB)
                return;

            AlertBtnTrans.gameObject.SetActive(false);
            MarkBtnTrans.gameObject.SetActive(false);
        }

        public override void Destroy()
        {
            base.Destroy();
            AssetBundleManager.DeleteAssets(ref dynamicItemPrefabRes, true);
            AssetBundleManager.DeleteAssets(ref dynamicHpItemPrefabRes, true);
            AssetBundleManager.DeleteAssets(ref staticItemPrefabRes, true);
            AssetBundleManager.DeleteAssets(ref rangeTipPrefabRes, true);
            foreach(ResNode r in animPrefabRes)
            {
                ResNode temp = r;
                AssetBundleManager.DeleteAssets(ref temp, true);
            }
            animPrefabRes = null;
            Destroy_Zone();
        }

        public override void Update()
        {
            base.Update();

            if (m_fNextUpdateMaskTime < Time.time)
            {
                m_fNextUpdateMaskTime = Time.time + 0.33f;

                UpdateItemMask();

                UpdateMaskTexture();

                UpdateAutoMoveLine();

                UpdateDirTipsUI();
               
            }
            UpdateZone();
        }

        public Vector2 SceneToMinimap(Vector2 ptScene)
        {
            Vector2 ptMap;
            if (m_sCurMinimapInfo.fRight == m_sCurMinimapInfo.fLeft || m_sCurMinimapInfo.fBottom == m_sCurMinimapInfo.fTop)
            {
                ptMap.x = ptMap.y = 0f;
            }
            else
            {
                ptMap.x = m_fMinimapWidth * (ptScene.x - m_sCurMinimapInfo.fLeft) / (m_sCurMinimapInfo.fRight - m_sCurMinimapInfo.fLeft) - m_fMinimapWidth * 0.5f;
                ptMap.y = m_fMinimapHeight * (ptScene.y - m_sCurMinimapInfo.fBottom) / (m_sCurMinimapInfo.fTop - m_sCurMinimapInfo.fBottom) - m_fMinimapHeight * 0.5f;
            }

            return ptMap;
        }

        public Vector2 SceneToMinimap(Vector3 ptScene)
        {
            Vector2 ptMap;
            ptMap.x = ptScene.x;
            ptMap.y = ptScene.z;
            return SceneToMinimap(ptMap);
        }

        public float SceneLengthToMinimap(float fLength)
        {
            if (m_sCurMinimapInfo.fRight == m_sCurMinimapInfo.fLeft || m_sCurMinimapInfo.fBottom == m_sCurMinimapInfo.fTop)
                return 0f;
            return m_fMinimapWidth * fLength / (m_sCurMinimapInfo.fRight - m_sCurMinimapInfo.fLeft);
        }

        public Vector3 MinimapToScene(Vector2 ptMinimap)
        {
            Vector3 ptScene;
            if (m_fMinimapWidth == 0f || m_fMinimapHeight == 0f)
            {
                ptScene.x = ptScene.y = ptScene.z = 0f;
            }
            else
            {
                ptScene.x = m_sCurMinimapInfo.fLeft + (m_sCurMinimapInfo.fRight - m_sCurMinimapInfo.fLeft) * (ptMinimap.x + m_fMinimapWidth * 0.5f) / m_fMinimapWidth;
                ptScene.y = 0f;
                ptScene.z = m_sCurMinimapInfo.fBottom + (m_sCurMinimapInfo.fTop - m_sCurMinimapInfo.fBottom) * (ptMinimap.y + m_fMinimapWidth * 0.5f) / m_fMinimapHeight;
            }

            return ptScene;
        }

        public void OnOpen()
        {
            InitMinimapInfo(SceneManager.Instance.m_curMapID);

            EntityFactory.SetEntityNumPropHandler(ResponseEntityNumPropEvent);
            GodBaseCameraController.SwitchGodEyeModeEvent -= OnSwitchGodEyeMode;
            GodBaseCameraController.SwitchGodEyeModeEvent += OnSwitchGodEyeMode;
            OBBaseCameraController.SwitchOBModeEvent -= OnSwitchGodEyeMode;
            OBBaseCameraController.SwitchOBModeEvent += OnSwitchGodEyeMode;
            EventManager.Subscibe(this, EVENT.ENTITY_SHOW_HERO, 0);
            OnOpen_Zone();
        }

        public void OnClose()
        {
            EntityFactory.RemoveEntityNumPropHandler(ResponseEntityNumPropEvent);
            GodBaseCameraController.SwitchGodEyeModeEvent -= OnSwitchGodEyeMode;
            OBBaseCameraController.SwitchOBModeEvent -= OnSwitchGodEyeMode;
            EventManager.UnSubscibe(this, EVENT.ENTITY_SHOW_HERO, 0);

            ClearMinimapInfo();
            OnClose_Zone();
        }

        private void InitMinimapInfo(int nMapID)
        {
            m_nCurMapID = nMapID;

            // 获取小地图信息
            if (GameLogicAPI.getMinimapInfo(nMapID, ref m_sCurMinimapInfo) == false)
            {
                Trace.LogError("InitMinimapInfo failed, !GameLogicAPI.getMinimapInfo nMapID=" + nMapID);
                return;
            }

            WndID wndID = GetUIWnd().GetID();
            if (m_sCurMinimapInfo.szBaseMap != null && m_sCurMinimapInfo.szBaseMap.Length > 0)
                mapBaseImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MinimapMap, m_sCurMinimapInfo.szBaseMap, wndID);
            else
                mapBaseImage.sprite = null;

            if (mapBaseImage.sprite != null)
            {
                mapBaseImage.SetNativeSize();
                mapBaseImage.gameObject.SetActive(true);
            }
            else
            {
                mapBaseImage.gameObject.SetActive(false);
            }

            mapDarkImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MinimapMap, m_sCurMinimapInfo.szDarkBg, wndID);
            mapDarkImage.SetNativeSize();
            mapFrame.sizeDelta = mapDarkImage.rectTransform.sizeDelta;
            mapMaskImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MinimapMap, m_sCurMinimapInfo.szLightBg, wndID);
            mapMaskImage.SetNativeSize();
            m_fMinimapWidth = (int)mapDarkImage.preferredWidth;
            m_fMinimapHeight = (int)mapDarkImage.preferredHeight;
            Vector2 minimapSize = new Vector2(m_fMinimapWidth, m_fMinimapHeight);
            mapMaskImage.rectTransform.sizeDelta = minimapSize;
            monsterIconLayer.sizeDelta = minimapSize;
            friendIconLayer.sizeDelta = minimapSize;
            enemyIconLayer.sizeDelta = minimapSize;
            selfIconLayer.sizeDelta = minimapSize;

            // init函数里可能取不到布局后的大小
            m_maskTexture = new Texture2D((int)m_fMinimapWidth, (int)m_fMinimapHeight, TextureFormat.Alpha8, false, true);
            m_maskTexture.name = "MinmapMask";
            mapMaskImage.material.SetTexture("_MaskTex", m_maskTexture);
            MiniMapMaskData = new byte[(int)m_fMinimapWidth * (int)m_fMinimapHeight];

            Rect sceneRect = new Rect(m_sCurMinimapInfo.fLeft, m_sCurMinimapInfo.fTop, Math.Abs(m_sCurMinimapInfo.fRight - m_sCurMinimapInfo.fLeft), Math.Abs(m_sCurMinimapInfo.fBottom - m_sCurMinimapInfo.fTop));
            LogicDataCenter.warMinimapDataManager.SetSceneRect(sceneRect);

            // 创建静态打点
            SMinimapStaticIcon staticIcon = new SMinimapStaticIcon();
            int nFromIndex = -1;
            while (GameLogicAPI.getNextMinimapStaticIcon(nMapID, nFromIndex, ref staticIcon) == true)
            {
                if (staticIcon.nAutoCreate != 0)
                {
                    AddStaticObject(staticIcon.nIndex);
                }
                nFromIndex = staticIcon.nIndex;
            }

            // 旋转小地图
            if (nMapID != -1 && m_MainHeroCamp != -1)
            {
                DefaultRotate();
                DefaultPosition();
            }

            // 设置按钮位置
            DefaultBtnPosition();
        }

        public void ClearMinimapInfo()
        {
            foreach (var item in m_dynamicItemList)
            {
                item.Value.Destroy();
            }
            m_dynamicItemList.Clear();

            foreach (var item in m_staticItemList)
            {
                item.Value.Destroy();
            }
            m_staticItemList.Clear();
            
            m_dynamicItemMaskList.Clear();

            m_nCurMapID = 0;
            m_MainHeroCamp = -1;
            m_fMinimapWidth = 0f;
            m_fMinimapHeight = 0f;
            m_sCurMinimapInfo = new SMinimapInfo();
            m_fCurRotateAngle = 0f;
            m_bGodEyeDraging = false;
            m_bGodEyeMode = false;
            ResNode.DestroyRes(ref m_maskTexture);
            MiniMapMaskData = null;
            godeyeRectImage.gameObject.SetActive(false);
        }

        private void DefaultRotate()
        {
            string[] strRotation = ((string)(m_sCurMinimapInfo.szDefaultRotation)).Split(';');
            for (int idx = 0; idx + 1 < strRotation.Length; idx = idx + 4)
            {
                int nCamp = Convert.ToInt32(strRotation[idx]);
                if (m_MainHeroCamp == nCamp)
                {
                    int nAngle = Convert.ToInt32(strRotation[idx + 1]) + (360 - m_sCurMinimapInfo.nOriRotation);
                    m_fCurRotateAngle = (float)nAngle;
                    mapFrame.eulerAngles = new Vector3(0f, 0f, m_fCurRotateAngle);

                    // 把除主角外的其他图标都转回去
                    foreach (var item in m_dynamicItemList)
                    {
                        if (item.Value.IsHero)
                            continue;

                        item.Value.transform.rotation = Quaternion.identity;
                    }

                    foreach (var item in m_staticItemList)
                    {
                        item.Value.transform.rotation = Quaternion.identity;
                    }

                    godeyeRectImage.rectTransform.rotation = Quaternion.identity;
                    return;
                }
            }
        }

        private void DefaultPosition()
        {
            string[] strRotation = ((string)(m_sCurMinimapInfo.szDefaultRotation)).Split(';');
            int nRight = Convert.ToInt32(strRotation[2]);
            int nBottom = Convert.ToInt32(strRotation[3]);

            GetComponent<RectTransform>().anchoredPosition = new Vector2(nRight, nBottom);
        }

        private void DefaultBtnPosition()
        {
            string[] strPos = m_sCurMinimapInfo.szAlertBtnPos.Split(';');
            if(strPos.Length != 2)
            {
                Debug.LogError("小地图按钮位置配置不对");
                return;
            }

            int xPos, yPos;
            if (int.TryParse(strPos[0], out xPos) && int.TryParse(strPos[1], out yPos))
            {
                AlertBtnTrans.gameObject.GetComponent<RectTransform>().anchoredPosition = new Vector2(xPos, yPos);
            }

            strPos = m_sCurMinimapInfo.szMarkBtnPos.Split(';');
            if(strPos.Length != 2)
            {
                Debug.LogError("小地图按钮位置配置不对");
                return;
            }

            if (int.TryParse(strPos[0], out xPos) && int.TryParse(strPos[1], out yPos))
            {
                MarkBtnTrans.gameObject.GetComponent<RectTransform>().anchoredPosition = new Vector2(xPos, yPos);
            }
        }

        //响应数值属性消息函数
        private void ResponseEntityNumPropEvent(EntityNumPropEventArgs e)
        {
            switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_HP:
                case (int)ENTITY_PROPERTY.PROPERTY_MAX_HP:
                    {
                        EntityView ev = e.objev;
                        int nMaxHp = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP);
                        if (nMaxHp == 0)
                            return;

                        // 只有防御塔需要设置血量
                        int nCurHp = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_HP);
                        if (ev != null && ev.Type == ENTITY_TYPE.TYPE_MONSTER && (ev.Flag == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_DEFENSIVE_TOWER || ev.Flag == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_BASE))
                        {
                            WarMinimapDynamicItem item;
                            if (m_dynamicItemList.TryGetValue(ev.ID, out item) && item != null)
                            {
                                item.UpdateHpInfo((float)nCurHp / (float)nMaxHp);
                            }
                        }
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_MASK:
                    {
                        UpdateObjectVisiblity(e.objev);
                    }
                    break;
                case (int)ENTITY_PROPERTY.PROPERTY_FORMTYPE:
                    {
                        UpdateHeroHathpace(e.objev.ID, e.data.nValue);
                    }
                    break;
                default:
                    break;
            }
        }

        // 通知小地图进入/退出 上帝视角模式
        private void OnSwitchGodEyeMode(bool isGodEyeMode, Vector3 vPoint)
        {
            m_bGodEyeMode = isGodEyeMode;
            if (isGodEyeMode)
            {
                godeyeRectImage.gameObject.SetActive(true);

                godeyeRectImage.rectTransform.anchoredPosition = SceneToMinimap(new Vector2(vPoint.x, vPoint.z));
            }
            else
            {
                godeyeRectImage.gameObject.SetActive(false);
            }

            if (isGodEyeMode && StageManager.Instance.CurPermissionState != EStage_PermissionState.ESPS_OB)
            {
                WarFogManager.Instance.Begin();
            }
            else
            {
                WarFogManager.Instance.End();
            }
        }

        public void OnEvent(EVENT eventID, int sourceID, System.Object objEvent)
        {
            switch (eventID)
            {
                case EVENT.ENTITY_SHOW_HERO:
                    {
                        if (objEvent == null)
                            return;

                        SEntityShowHero pEvent = (SEntityShowHero)objEvent;
                        if (pEvent.ev == null)
                            return;

                        UpdateObjectVisiblity(pEvent.ev);
                    }
                    break;
                default:
                    break;
            }
        }

        // 更新物件可见性
        public void UpdateObjectVisiblity(EntityView ev)
        {
            WarMinimapDynamicItem item;
            if (m_dynamicItemList.TryGetValue(ev.ID, out item) && item != null)
            {
                item.SetVisible(CanShowOnMinimap(ev));
            }
        }

        // 增加物件
        public void AddObject(EntityView ev)
        {
            if (ev == null)
                return;

            WarMinimapDynamicItem oldItem;
            if (m_dynamicItemList.TryGetValue(ev.ID, out oldItem) && oldItem != null)
            {
                oldItem.Destroy();
                oldItem = null;
            }

            bool bIsMainHero = (ev.createinfo.byIsHero == 1);
            if (bIsMainHero)
            {
                m_MainHeroCamp = ev.createinfo.byCamp;
            }

            if (m_MainHeroCamp < 0)
            {
                Trace.LogError("MinimapAddObj failed: variable [mainHeroCamp] not ready!!!!");
                return;
            }

            int nRelation;
            if (bIsMainHero)
            {
                nRelation = 3; // 自己
            }
            else
            {
                if(StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB)
                {
                    if (ev.createinfo.byCamp == 0)
                        nRelation = 2;
                    else
                        nRelation = ev.createinfo.byCamp - 1;
                }
                else
                {
                    if (ev.createinfo.byCamp == m_MainHeroCamp)
                        nRelation = 0; // 友方
                    else if (ev.createinfo.byCamp == 0)
                        nRelation = 2; // 中立
                    else
                        nRelation = 1; // 敌方
                }
            }

            SMinimapIcon sIconInfo = new SMinimapIcon();
            if (GameLogicAPI.getMinimapIconInfo(ev.createinfo.nMinimapIcon, ref sIconInfo) == false)
            {
                Trace.Log("MinimapAddObj, GameLogicAPI.getMinimapIconInfo nIconID=" + ev.createinfo.nMinimapIcon);
                return;
            }

            // 阵营显示中敌方实体不在小地图上显示，不创建
            if (sIconInfo.nShowType == 0 && nRelation == 1)
                return;

            GameObject itemObj;
            if (ev.Type == ENTITY_TYPE.TYPE_MONSTER && (ev.createinfo.ndata1 == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_DEFENSIVE_TOWER || ev.createinfo.ndata1 == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_BASE))
            {
                itemObj = dynamicHpItemPrefabRes.InstanceMainRes();
            }
            else
            {
                itemObj = dynamicItemPrefabRes.InstanceMainRes();
            }
            if (itemObj == null)
                return;

            WarMinimapDynamicItem newItem = itemObj.GetComponent<WarMinimapDynamicItem>();
            if (newItem == null)
            {
                ResNode.DestroyRes(ref itemObj);
                return;
            }

            if (newItem.Init(this, ev, ref sIconInfo, nRelation) == false)
            {
                ResNode.DestroyRes(ref itemObj);
                return;
            }

            m_dynamicItemList[ev.ID] = newItem;

            // 根据主角阵营小地图自动选择到默认位置
            if (nRelation == 3 && m_nCurMapID > 0)
            {
                DefaultRotate();
                DefaultPosition();
            }
        }

        // 移除物件
        public void RemoveObject(EntityView ev)
        {
            if (ev == null)
                return;

            WarMinimapDynamicItem item;
            if (m_dynamicItemList.TryGetValue(ev.ID, out item))
            {
                item.Destroy();
                
                m_dynamicItemList.Remove(ev.ID);
            }
        }

        // 是否在小地图上可见
        private bool CanShowOnMinimap(EntityView ev)
        {
            if (ev == null)
                return false;

            // 判断mask标记
            int nMask = ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);
            if (ev.IsHero)
            {
                if ((nMask & (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_IN_SELF_MINIMAP) != 0)
                {
                    return false;
                }
            }
            else
            {
                if (ev.CampFlag == CampFlag.CampFlag_Friend)
                {
                    if ((nMask & (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_IN_FRIEND_MINIMAP) != 0)
                    {
                        return false;
                    }
                }
                else if (ev.CampFlag == CampFlag.CampFlag_Enemy)
                {
                    if ((nMask & (int)ENTITY_MASKFLAG.MASKFLAG_HIDE_IN_ENEMY_MIMIMAP) != 0)
                    {
                        return false;
                    }
                }
            }

            // 死亡状态 不允许显示
            if (ev.StateMachine.GetState() == GameLogic.EntityState.Dead)
            {
                return false;
            }

            // 没视野 不允许显示
            if (!ev.StateMachine.m_OnViewVisible)
            {
                return false;
            }

            // 魔王不判断是否黑暗角落
            if (ev.ID != LogicDataCenter.recordTableDataManager.CurDevilKing)
            {
                // 在黑暗的角落，不允许显示
                U3D_Render.EntityView mainEv = EntityFactory.getMainPlayer();
                if (mainEv == null || mainEv.Property == null)
                    return false;
                int nObserver = mainEv.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_OBSERVER);
                if (nObserver == 0 && ev.m_bInDarkCorner)
                {
                    return false;
                }
            }

            return true;
        }

        // 添加物件动画效果
        public void ObjectAttachEffect(MinimapObjectAttachEffect info)
        {
            if (info == null)
                return;

            WarMinimapDynamicItem item;
            if (m_dynamicItemList.TryGetValue(info.nObjID, out item))
            {
                item.AttachEffect(info.nEffectType);
            }
        }

        // 添加静态打点
        public void AddStaticObject(int nObjType)
        {
            SMinimapStaticIcon sIconInfo = new SMinimapStaticIcon();
            // 读静态打点配置   
            if (GameLogicAPI.getMinimapStaticObjInfo(SceneManager.Instance.m_curMapID, nObjType, ref sIconInfo) == false)
            {
                Trace.LogError("CreateStaticObj(" + nObjType + "), mapID=" + SceneManager.Instance.m_curMapID + " failed");
                return;
            }

            WarMinimapStaticItem oldItem;
            if (m_staticItemList.TryGetValue(nObjType, out oldItem) && oldItem != null)
            {
                oldItem.Init(this, ref sIconInfo);
                oldItem.SetVisible(true);
                return;
            }

            GameObject itemObj = staticItemPrefabRes.InstanceMainRes();
            if (itemObj == null)
                return;

            WarMinimapStaticItem newItem = itemObj.GetComponent<WarMinimapStaticItem>();
            if (newItem == null)
            {
                ResNode.DestroyRes(ref itemObj);
                return;
            }

            if (newItem.Init(this, ref sIconInfo) == false)
            {
                ResNode.DestroyRes(ref itemObj);
                return;
            }

            m_staticItemList[nObjType] = newItem;
        }

        // 移除静态打点
        public void RemoveStaticObject(MinimapStaticObjData info)
        {
            if (info == null)
                return;

            WarMinimapStaticItem item;
            if (m_staticItemList.TryGetValue(info.nObjType, out item))
            {
                item.SetVisible(false);
            }
        }

        // 清空静态打点
        public void ClearStaticObject()
        {
            foreach (var item in m_staticItemList)
            {
                item.Value.Destroy();
            }
            m_staticItemList.Clear();
        }

        // 设置静态打点倒计时
        public void SetStaticObjectCountdown(MinimapStaticObjCountdown info)
        {
            if (info == null)
                return;

            WarMinimapStaticItem item;
            if (m_staticItemList.TryGetValue(info.nObjType, out item))
            {
                item.SetCountDown(info.nCountdown);
                item.SetVisible(true);
            }
        }

        // 单独更新物件位置
        public void UpdateObjectPos(EntityView ev, EntityPosAngleInfo pos)
        {
            WarMinimapDynamicItem minimapItem;
            if (m_dynamicItemList.TryGetValue(ev.ID, out minimapItem))
            {
                minimapItem.UpdatePosInfo(ev, ref pos);
            }
        }

        // 批量更新物件位置(有延迟)
        public void UpdateObjectPos()
        {
            Dictionary<int, EntityPosAngleInfo> updateList = EntityBatchCommunicator.EntityInfo;
            if (updateList.Count == 0)
                return;

            foreach (var item in updateList)
            {
                EntityView ev = EntityFactory.getEntityViewByID(item.Key);
                if (ev == null)
                    continue;

                EntityPosAngleInfo info = item.Value;
                WarMinimapDynamicItem minimapItem;
                if (m_dynamicItemList.TryGetValue(item.Key, out minimapItem))
                {
                    minimapItem.UpdatePosInfo(ev, ref info);
                }

                // 小地图更新后处理
                ev.OnUpdateMinimap();
            }
        }

        public void AddMapSignal(MinimapAddSignal uiData)
        {
            UEffectPrefabType prefabType;
            MinimapSignalSoundType soundType;
            switch ((EMinimapClickType)uiData.nType)
            {
                case EMinimapClickType.EMCT_AttackSignal:
                    prefabType = UEffectPrefabType.UEPT_MapSignal_AttackSign;
                    soundType = MinimapSignalSoundType.MSST_ATTACK;
                    break;
                case EMinimapClickType.EMCT_RetreatSignal:
                    prefabType = UEffectPrefabType.UEPT_MapSignal_RetreatSign;
                    soundType = MinimapSignalSoundType.MSST_RETREAT;
                    break;
                case EMinimapClickType.EMCT_DangerSignal:
                    prefabType = UEffectPrefabType.UEPT_MapSignal_DangerSign;
                    soundType = MinimapSignalSoundType.MSST_DANGER;
                    break;
                case EMinimapClickType.EMCT_OnWaySignal:
                    prefabType = UEffectPrefabType.UEPT_MapSignal_OnWaySign;
                    soundType = MinimapSignalSoundType.MSST_ONWAY;
                    break;
                case EMinimapClickType.EMCT_HelpMeSignal:
                    prefabType = UEffectPrefabType.UEPT_MapSignal_HelpMeSign;
                    soundType = MinimapSignalSoundType.MSST_HELPME;
                    break;
                case EMinimapClickType.EMCT_LostEyeSignal:
                    prefabType = UEffectPrefabType.UEPT_MapSignal_LostEyeSign;
                    soundType = MinimapSignalSoundType.MSST_LOSTEYE;
                    break;
                default:
                    return;
            }

            Vector3 pos = SceneToMinimap(new Vector2(uiData.fPosX, uiData.fPosY));
            UEffectParamBase param = new UEffectPrefabParam(prefabType, animLayer, pos, null, true);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);

            SoundManager.CreateUISound((int)soundType);
        }

        public void SetObjectRangeTip(MinimapSetObjRangeTip uiData)
        {
            if (uiData == null)
                return;

            WarMinimapDynamicItem item;
            if (m_dynamicItemList.TryGetValue(uiData.nObjID, out item))
            {
                item.SetRangeTip(uiData.bIsAdd, uiData.nRange);
            }
        }

        public void UpdateHeroHathpace(int entityID, int standpoint)
        {
            WarMinimapDynamicItem item;
            if (m_dynamicItemList.TryGetValue(entityID, out item))
            {
                FORM_TYPE formType = (FORM_TYPE)standpoint;
                item.UpdateHeroHathpace(formType);
            }
        }
        
        // 像素坐标转化为大地图坐标
        private Vector3 ScreenPointToScene(Vector3 screenPoint)
        {
            Vector2 localPos;
            if (!RectTransformUtility.ScreenPointToLocalPointInRectangle(mapFrame, screenPoint, UISystem.Instance.GetCamera(), out localPos))
                return Vector3.zero;

            if (localPos.x < -m_fMinimapWidth * 0.5f || localPos.x > m_fMinimapWidth * 0.5f || localPos.y < -m_fMinimapHeight * 0.5f || localPos.y > m_fMinimapHeight * 0.5f)
                return Vector3.zero;

            return MinimapToScene(localPos);
        }

        private void UpdateGodEye(Vector3 scenePos)
        {
            if (EntityFactory.MainHero != null)
            {
                scenePos.y = EntityFactory.MainHero.transform.position.y;
            }

            Vector3 angle = new Vector3(50.0f, m_fCurRotateAngle, 0.0f);
            GodEyeStateParam param = new GodEyeStateParam(scenePos, angle, new Vector3(0, 0, -1), new Vector3(0, 0, 0), 40.0f, 30.0f);

            SoldierCamera.MainInstance<SoldierCamera>().UpdateGodEyePos(param);
        }

        public override void OnBeginDrag(PointerEventData eventData)
        {
            if (m_bGodEyeDraging == false && eventData.button == PointerEventData.InputButton.Left)
            {
                if(CheckDragEnable())
                    m_bGodEyeDraging = true;
            }
        }

        private bool CheckDragEnable()
        {
            if (EntityFactory.MainHeroView != null)   
            {
                PersonSkinModelItem item = new PersonSkinModelItem();
                int heroID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
                GameLogicAPI.GetPersonModel((UInt16)heroID, ref item);
                //非 fps英雄，没有任何信号操作的时候，可以拖动
                if (MouseCtrl.Instance.GetAttachType() == (int)MouseAttachFuncType.None && item.nPlayMode == 0)
                    return true;
                //fps英雄 按住alt时，也可以拖动
                else if (MouseCtrl.Instance.GetAttachType() == (int)MouseAttachFuncType.AltAttackSign && item.nPlayMode == 1)
                {
                    return true;
                }

            }
            return false;
        }

        public override void OnDrag(PointerEventData eventData)
        {
            if (m_bGodEyeDraging)
            {
                Vector3 scenePos = ScreenPointToScene(eventData.position);

                // 更新上帝视角位置
                SkillCtrl sc = EntityFactory.MainHero.GetComponent<SkillCtrl>();
                if (sc != null)
                    sc.MinimapClickPosInScene = scenePos;

                // 进攻撤退信号
                if (EntityFactory.MainHero != null)
                {
                    MouseCtrl mouseCtrl = EntityFactory.MainHero.GetComponentInChildren<MouseCtrl>();
                    if (mouseCtrl != null)
                    {
                        if (mouseCtrl.IsTeamCommandStart())
                        {
                            // 拖信息标示UI时屏蔽上帝视角消息
                            m_bGodEyeDraging = false;
                            return;
                        }
                    }
                }

                // 如果角色处于战斗状态,不能进上帝视角
                if (EntityViewCommandHandler.mainHeroState == (int)ENTITY_STATE.ENTITY_STATE_FIGHT)
                {
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_WAR_CHANGE_GODEYE_IN_BATTLE);
                    return;
                }

                // 进入上帝视角
                UpdateGodEye(scenePos);
            }
        }

        public override void OnEndDrag(PointerEventData eventData)
        {
            if (m_bGodEyeDraging)
            {
                m_bGodEyeDraging = false;
            }
            else
            {
                // 也许在拖动信号窗口
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MAPSIGNAL_MINIMAPDRAGEND, null);
            }
        }

        public void OnPointerDown(PointerEventData eventData)
        {
            if (EntityFactory.MainHero == null)
                return;

            if ((eventData.selectedObject != null)
                && ((eventData.selectedObject.gameObject == AlertBtnTrans.GetComponent<Button>().gameObject)
                || (eventData.selectedObject.gameObject == MarkBtnTrans.GetComponent<Button>().gameObject)))
            {
                return;
            }

            Vector3 scenePos = ScreenPointToScene(eventData.position);

            if (eventData.button == PointerEventData.InputButton.Left)
            {
                SkillCtrl sc = EntityFactory.MainHero.GetComponent<SkillCtrl>();
                if (sc != null)
                    sc.MinimapClickPosInScene = scenePos;
                if(sc.m_bPickOnMinimap)     //正在点击小地图施法，就不做任何处理
                {
                    return;
                }

                // 进攻撤退信号
                if (EntityFactory.MainHero != null)
                {
                    MouseCtrl mouseCtrl = EntityFactory.MainHero.GetComponentInChildren<MouseCtrl>();
                    if (mouseCtrl != null)
                    {
                        int type = mouseCtrl.GetAttachType();
                        if (Time.time - mStartTime >= 5)
                        {
                            if (type == (int)MouseAttachFuncType.CtrlAttackSign || type == (int)MouseAttachFuncType.DangerSign)
                            {
                                mouseCtrl.SetAttachType(MouseAttachFuncType.None);
                            }
                        }

                        if (mouseCtrl.OnMinimapClick(scenePos.x, scenePos.y, scenePos.z))
                            return;
                    }
                }

                // 进上帝视角
                if (EntityViewCommandHandler.mainHeroState == (int)ENTITY_STATE.ENTITY_STATE_FIGHT)
                {
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_WAR_CHANGE_GODEYE_IN_BATTLE);
                    return;
                }

                UpdateGodEye(scenePos);
            }
            if (eventData.button == PointerEventData.InputButton.Right)
            {
                BaseStateMachine sm = EntityFactory.MainHero.GetComponent<BaseStateMachine>();
                if (sm != null)
                {
                    sm.AutoMoveTo(scenePos);
                }
            }
        }
        
        private void UpdateMaskTexture()
        {
            for (int i = 0; i < MiniMapMaskData.Length;i++ )
            {
                MiniMapMaskData[i] = 0;
            }

            foreach (var itemPair in m_dynamicItemMaskList)//m_dynamicItemList.Values)//
            {
                WarMinimapDynamicItem item = itemPair;//itemPair.Value;
                if (item == null || item.IsHaveSight(m_MainHeroCamp) == false)
                    continue;

                // 绘制圆形                
                item.DrawCircle(ref MiniMapMaskData, (int)m_fMinimapWidth, (int)m_fMinimapHeight, (int)(m_fMinimapWidth * 0.5f), (int)(m_fMinimapHeight * 0.5f));
            }
            m_maskTexture.LoadRawTextureData(MiniMapMaskData);
      
            m_maskTexture.Apply();

        }

        public GameObject CreateAnim(int nAnimIndex)
        {
            if (nAnimIndex < 0 || nAnimIndex >= animPrefabRes.Length)
                return null;

            GameObject animObj = animPrefabRes[nAnimIndex].InstanceMainRes();
            return animObj;
        }

        private int nLastIndex;
        public void DrawAutoMoveLine()
        {
            CreatureStateMachine pFSM = EntityFactory.MainHeroView.StateMachine as CreatureStateMachine;
            autoMovePointsScreen = pFSM.moveCtrl.AutoMovePathPoint;

            if (null == autoMovePointsScreen || !lineRenderer)
            {
                return;
            }
            lineRenderer.Points = new Vector2[autoMovePointsScreen.Length];
            for (int i = 0; i < autoMovePointsScreen.Length; i++)
            {
                lineRenderer.Points[i] = SceneToMinimap(new Vector2(autoMovePointsScreen[i].x, autoMovePointsScreen[i].z));
            }

            nLastIndex = 0;
        }

        public void EraseAutoMoveLine()
        {
            lineRenderer.Points = new Vector2[0];
            autoMovePointsScreen = new Vector3[0];
        }

        private void UpdateAutoMoveLine()
        {
            if (lineRenderer.Points.Length == 0 || autoMovePointsScreen.Length == 0)
                return;

            BaseStateMachine pFSM = EntityFactory.MainHeroView.StateMachine;
            Vector3 curPoint = pFSM.creature.position;

            // 矩形动态取点算法
            int nIndex = 0;
            for (int i = 0; i < autoMovePointsScreen.Length - 1; i++)
            {
                Vector3 A = autoMovePointsScreen[i];
                Vector3 B = autoMovePointsScreen[i + 1];
                Vector3 C = curPoint;
                if (((C.x - A.x)*(C.x - B.x) < 0 && (C.z - A.z)*(C.z - B.z) < 0) || (C.x == A.x && C.z == A.z)) 
                {
                    // 剔除矩形重叠情况
                    if (nLastIndex > i)
                        continue;
                        
                    nIndex = i;
                    nLastIndex = nIndex;
                    break;
                }
            }
            // 剔除走出所有矩形情况
            if (nIndex == 0)
            {
                nIndex = nLastIndex;
            }

            int nCurPointNum = autoMovePointsScreen.Length - nIndex;
            lineRenderer.Points = new Vector2[nCurPointNum];
            lineRenderer.Points[0] = SceneToMinimap(new Vector2(curPoint.x, curPoint.z));
            for (int i = 1; i < nCurPointNum; i++)
            {
                lineRenderer.Points[i] = SceneToMinimap(new Vector2(autoMovePointsScreen[nIndex + i].x, autoMovePointsScreen[nIndex + i].z));
            }
        }

        public void OnAlertBtn()
        {
            mStartTime = Time.time;
            if (EntityFactory.MainHero != null)
            {
                MouseCtrl mouseCtrl = EntityFactory.MainHero.GetComponentInChildren<MouseCtrl>();
                if (mouseCtrl != null)
                    mouseCtrl.SetAttachType(MouseAttachFuncType.DangerSign);
            }
        }

        public void OnMarkBtn()
        {
            mStartTime = Time.time;
            if (EntityFactory.MainHero != null)
            {
                MouseCtrl mouseCtrl = EntityFactory.MainHero.GetComponentInChildren<MouseCtrl>();
                if (mouseCtrl != null)
                    mouseCtrl.SetAttachType(MouseAttachFuncType.CtrlAttackSign);
            }
        }

        public float ItemMaskDistance = 20f;
        public int MinueCount = 0;
        public int ItemListCount = 0;
        public int ItemMaskListCount;

        private void UpdateItemMask()
        {
            m_dynamicItemMaskList.Clear();
            ItemListCount = m_dynamicItemList.Count;
            List<WarMinimapDynamicItem> valueList = m_dynamicItemList.Values.ToList();

            for (int i=0;i< valueList.Count;++i)
            {
                if (valueList[i].IsHaveSight(m_MainHeroCamp) == false)
                    continue;
                else
                {
                    m_dynamicItemMaskList.Add(valueList[i]);
                    break;
                }
            }
            MinueCount = 0;
            float dis = 0;
            for(int i=0;i< valueList.Count;++i)// (var item in m_dynamicItemList.Values)
            {
                if (valueList[i].IsHaveSight(m_MainHeroCamp) == false)
                    continue;

                for(int j=0;j<m_dynamicItemMaskList.Count;++j)//foreach (var maskItem in m_dynamicItemMaskList)
                {
                    dis = valueList[i].GetMinimapDistance(m_dynamicItemMaskList[j]);
                    if (dis <= ItemMaskDistance)
                    {
                        MinueCount++;
                        continue;
                    }
                        
                    if (m_dynamicItemMaskList.Contains(valueList[i]))
                        continue;

                    m_dynamicItemMaskList.Add(valueList[i]);
                    break;
                }
            }
            ItemMaskListCount = m_dynamicItemMaskList.Count;
        }
    }
}
