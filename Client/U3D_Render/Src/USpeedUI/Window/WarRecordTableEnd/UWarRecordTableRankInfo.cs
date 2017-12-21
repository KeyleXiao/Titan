/*******************************************************************
** 文件名:	UMatchInfoWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	2016-7-13
** 版  本:	1.0
** 描  述:	匹配信息界面逻辑模块(接发消息)+视图模块(显示数据)	
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using DataCenter;
using UnityEngine;
using U3D_Render.Common;
using SocialSharing;
using USpeedUI.UEffect;
using USpeedUI.UWidgets;
using System.IO;
using ASpeedGame.Data.Scheme;

namespace USpeedUI.WarEndRankInfo
{

    public class UWarRecordTableRankInfoWnd : UIPopupWnd<UWarRecordTableRankInfoWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_END_RANKINFO;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "DynamicState_Moba/WarRecordTable_End/WarRecordTableRankInfo";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_END_RANKINFO_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_END_RANKINFO_HIDE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_SHOW, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_END_RANKINFO_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_END_RANKINFO_HIDE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_SHOW, this);
            
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {

                case WndMsgID.WND_MSG_WAR_END_RANKINFO_SHOW:
                    {
                        if (m_wndView == null)
                        {
                            LoadView();
                        }
                        // 判定是否要显示排位界面
                        SetVisible(true);
                        
                    }
                    break;

                case WndMsgID.WND_MSG_WAR_END_RANKINFO_HIDE:
                    {
                        if (m_wndView == null)
                            return;
                        SetVisible(false);
                    }
                    break;

                case WndMsgID.WND_MSG_WAR_RECORDTABLE_END_SHOW:
                    {
                        if (m_wndView == null)
                        {
                            LoadView();
                        }
                        // 判定是否要显示排位界面
                        m_wndView.ShowRankInfo();

                    }
                    break;

                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        if (m_wndView != null)
                        {
                            SetVisible(false);
                            UnloadView();
                        }
                    }
                    break;
                
                default:
                    break;
            }
        }
    }

    public class UWarRecordTableRankInfoWndView : UIBaseWndView
    {
        public GameObject UpgradeFrame;
        public GameObject DwonFrame;

        public Text Title;
        public Image UpgradeImg;
        public Text UpgradeDes;
        public Text UpgradeConfirmDes;
        public Text UpRankName;
        private UEffectParamBase m_RankEffectParam;

        public Text DownDes;
        public Text DownConfirmDes;
        public Text DownDetailDes;
        public Text DownRankName;

        public UShareBar ShareBar;
        public RectTransform ShareContentRt;

        private readonly string shareImgName = "RankUpgradeTemp.jpg";

        private string shareImgPath;

        private void initRecordShare()
        {
            shareImgPath = "";

            int nActorID = 0;
            if (null != EntityFactory.MainHeroView && null != EntityFactory.MainHeroView.Property)
            {
                nActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            }

            shareImgPath = nActorID + shareImgName;

            ShareBar.SetData(SocialSharingType.RankUpgrad, CaptureScreen.GetInstance().CaptureTempDir + "/" + shareImgPath);
        }

        private void destroyRecordShare()
        {
            shareImgPath = "";
        }

        private Rect GetRecordRect()
        {
            Vector3[] worldCorners = new Vector3[4];

            ShareContentRt.GetWorldCorners(worldCorners);

            Vector3 corner0 = UISystem.Instance.GetCamera().WorldToScreenPoint(worldCorners[0]);
            Vector3 corner1 = UISystem.Instance.GetCamera().WorldToScreenPoint(worldCorners[1]);
            Vector3 corner3 = UISystem.Instance.GetCamera().WorldToScreenPoint(worldCorners[3]);

            return new Rect(corner0.x, corner0.y, corner3.x - corner0.x, corner1.y - corner0.y);
        }

        public override bool Init(IUIWnd wnd)
        {
            initRecordShare();
            Title.text = ULocalizationService.Instance.Get("UIView", "RankEndWnd", "Title");
            string strMsg = ULocalizationService.Instance.Get("UIView", "RankEndWnd", "DwonDes");
            UBB.toHtml(ref strMsg);
            DownDes.text = strMsg;

            strMsg = ULocalizationService.Instance.Get("UIView", "RankEndWnd", "UpRankDes");
            UBB.toHtml(ref strMsg);
            UpgradeDes.text = strMsg;

            //更新界面信息
            strMsg = ULocalizationService.Instance.Get("UIView", "RankEndWnd", "DownDetailDes");
            UBB.toHtml(ref strMsg);
            DownDetailDes.text = strMsg;

            UpgradeConfirmDes.text = ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn");
            DownConfirmDes.text = ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn");

            return base.Init(wnd);
        }

        public void ShowRankInfo()
        {
            if (LogicDataCenter.rankDataManager.getRankChanged())
            {
                //有显示
                if (LogicDataCenter.rankDataManager.getDownOrUp() == 0)
                {
                    SetVisible(false);
                }
                if (LogicDataCenter.rankDataManager.getDownOrUp() == 1)
                {
                    SetVisible(true);
                    showUpGradeFrame();
                }

                if (LogicDataCenter.rankDataManager.getDownOrUp() == -1)
                {
                    SetVisible(true);
                    showDownFrame();
                }
            }

            LogicDataCenter.rankDataManager.resetRankChanged();
        }

        public void showUpGradeFrame()
        {
            UpgradeFrame.SetActive(true);

            #region recordShareImg
            CaptureScreen.GetInstance().CaptureAreaImage(new CaptureAreaImgData(shareImgPath, GetRecordRect()));
            #endregion

            DwonFrame.SetActive(false);
            UpgradeImg.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, WndID.WND_ID_WAR_MAIN_FRAME, 1, LogicDataCenter.rankDataManager.getRankID());

            if (m_RankEffectParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_RankEffectParam);
                m_RankEffectParam = null;
            }

            int nOffset = LogicDataCenter.rankDataManager.getRankID() - 1;
            if (nOffset >= 0)
            {
                UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_WarMain_RankEffect1 + nOffset, _tfAttachParent: this.UpgradeImg.transform);
                UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);

                m_RankEffectParam = param;
                UEffectPrefabParam paramPrefab = (UEffectPrefabParam)param;
                paramPrefab.EffectGameObject.transform.SetAsFirstSibling();
            }

            // 显示描述
            UpRankName.text = LogicDataCenter.rankDataManager.getRankName();
        }

        public void showDownFrame()
        {
            UpgradeFrame.SetActive(false);
            DwonFrame.SetActive(true);
            // 显示描述
            DownRankName.text = LogicDataCenter.rankDataManager.getRankName();
        }

        public void OnConfirmClick()
        {
            SetVisible(false);
        }
    }
}
