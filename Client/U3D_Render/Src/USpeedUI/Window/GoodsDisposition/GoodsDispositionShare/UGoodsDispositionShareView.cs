using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UWidgets;

namespace USpeedUI.GoodsDisposition
{

    class UGDS_SubView_GoodsDisposition : UIBaseSubWndView
    {
        public Text DispositionNameTF = null;

        public List<UGD_GoodsDispositionRendererBase> GoodsDispositionRenderer = null;

        public Button CancelBtn = null;
        public Button ConfirmBtn = null;

        public override bool Init(IUISubWnd wnd)
        {
            bool result = base.Init(wnd);

            if(CancelBtn != null)
            {
                CancelBtn.onClick.AddListener(OnCancelBtnClick);
            }
            if(ConfirmBtn != null)
            {
                ConfirmBtn.onClick.AddListener(OnConfirmBtnClick);
            }

            return result;
        }

        public void SetData(UEquipSchemeMsgData _data)
        {
            DispositionNameTF.text = _data.scheme.szSchemeName;
            if(_data.scheme.nEquipIDList != null && GoodsDispositionRenderer.Count == _data.scheme.nEquipIDList.Length)
            {
                for (int i = 0; i < _data.scheme.nEquipIDList.Length; ++i)
                {
                    GoodsDispositionRenderer[i].SetData(_data.scheme.nEquipIDList[i]);
                }
            }
        }

        protected void OnCancelBtnClick()
        {
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION_SHARE, WndMsgID.WND_MSG_GOODSDISPOSITION_SHARE_CLOSE, null);
        }

        protected void OnConfirmBtnClick()
        {
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION_SHARE, WndMsgID.WND_MSG_GOODSDISPOSITION_SHARE_SAVE_DISPOSITIONINFO, null);
        }
    }

    class UGDS_SubView_DispositionFilled : UIBaseSubWndView
    {
        public UGD_DispositionFilledList DispositionList = null;
        public Button CancelBtn = null;
        public Button ConfirmBtn = null;

        public override bool Init(IUISubWnd wnd)
        {
            bool result = base.Init(wnd);

            if (CancelBtn != null)
            {
                CancelBtn.onClick.AddListener(OnCancelBtnClick);
            }

            if(ConfirmBtn != null)
            {
                ConfirmBtn.onClick.AddListener(OnConfirmBtnClick);
            }

            return result;
        }
        protected void OnCancelBtnClick()
        {
            SetVisible(false);
        }

        protected void OnConfirmBtnClick()
        {
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION_SHARE, WndMsgID.WND_MSG_GOODSDISPOSITION_SHARE_SAVE_DISPOSITIONINFO, null);
        }

        public void SetDispositionFilled(int _nHeroID)
        {
            List<GameLogic.cmd_war_equipscheme> schemeList = null;
            DataCenter.EquipSchemeDataManager.Instance.getDiyEquipSchemeList(_nHeroID,out schemeList);

            UIWidgets.ObservableList<UGD_GoodsDispositionItem> schemeItemList = new UIWidgets.ObservableList<UGD_GoodsDispositionItem>();

            schemeItemList.BeginUpdate();
            for (int i = 0; i < schemeList.Count; ++i)
            {
                UGD_GoodsDispositionItem item = new UGD_GoodsDispositionItem();
                item.From(schemeList[i]);

                schemeItemList.Add(item);
            }
            schemeItemList.EndUpdate();
            DispositionList.DataSource = schemeItemList;
        }
    }

    public class UGoodsDispositionShareView : UIBaseWndView
    {
        protected UGoodsDispositionShareWnd Wnd
        { get { return m_wnd as UGoodsDispositionShareWnd; } }

        public RectTransform ContentFrame = null;
        public RectTransform PopupFrame = null;
        public Text TitleTxt = null;
        public Button CloseBtn = null;

        public override bool Init(IUIWnd wnd)
        {
            bool result = base.Init(wnd);

            Wnd.InitSubView(UGoodsDispositionShareWnd.ELevel1_SubWnd.GoodsDisposition, ContentFrame, true);
            Wnd.InitSubView(UGoodsDispositionShareWnd.ELevel1_SubWnd.DispositionFilled, PopupFrame, false);

            if(CloseBtn != null)
            {
                CloseBtn.onClick.AddListener(OnCloseBtnClick);
            }

            return result;
        }

        public void SetTitleData(string _strTitle)
        {
            if(TitleTxt != null)
            {
                TitleTxt.text = _strTitle;
            }
        }


        protected void OnCloseBtnClick()
        {
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_GOODSDISPOSITION_SHARE, WndMsgID.WND_MSG_GOODSDISPOSITION_SHARE_CLOSE, null);
        }
    }
}
