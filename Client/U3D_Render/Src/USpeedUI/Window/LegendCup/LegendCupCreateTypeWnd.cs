using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using ASpeedGame.Data.LegendCupConfig;
using GameLogic;

namespace USpeedUI.LegendCup
{
    public class LegendCupCreateTypeWnd : UIPopupWnd<LegendCupCreateTypeWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LEGENDCUP_CREATETYPE;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "LegendCup/LegendCupCreateTypeView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_CREATETYPE_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXCREATE_SHOW, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_CREATETYPE_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXCREATE_SHOW, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_LEGENDCUP_CREATETYPE_SHOW:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                        {
                            LegendCupJudgeWarData cmdData = (LegendCupJudgeWarData)data;
                            m_wndView.OnShowCreateTypeWnd();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_MSGBOXCREATE_SHOW:
                    {
                        if (m_wndView != null)
                        {
                            UnloadView();
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);

            UCommonFullWndVisible msgData = new UCommonFullWndVisible();
            msgData.msgID = (int)WndMsgID.WND_MSG_COMMON_FULLWND_VISIBLE;
            msgData.nWndID = (int)GetID();
            msgData.isVisible = _bVisible;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_FULLWND_VISIBLE, msgData);
        }
    }

    enum ChangeStateItemIndex
    {
        CSII_Base,
        CSII_Desc,
    }

    public class LegendCupTypeItem : MonoBehaviour
    {
        public Image CupTypeDes;
        public Image CupTypeIcon;

        private int m_nCupCreateType;
        public int CupCreateType { get { return m_nCupCreateType; } }

        public void SetData(LegendCupCreateTypeWndView view, int nType)
        {
            if (view == null || view.CupIconSprites == null || nType > view.CupIconSprites.Length || nType > view.CupDescNormalSprites.Length || nType > view.CupDescPressedSprites.Length)
                return;

            m_nCupCreateType = nType;

            UButtonGroupCombineItem item = this.gameObject.GetComponent<UButtonGroupCombineItem>();
            item.NormalSprites[(int)ChangeStateItemIndex.CSII_Desc] = view.CupDescNormalSprites[nType];
            item.PressedSprites[(int)ChangeStateItemIndex.CSII_Desc] = view.CupDescPressedSprites[nType];
            CupTypeDes.sprite = item.NormalSprites[(int)ChangeStateItemIndex.CSII_Desc];

            CupTypeIcon.sprite = view.CupIconSprites[nType];
        }
    }

    public class LegendCupCreateTypeWndView : UIBaseWndView
    {
        public Text TitleDes;
        public Text ConfirmBtnDes;
        public Text CancelBtnDes;
        public Text TypeDesc;

        public RectTransform CupTypeFrame;
        public GameObject CupTypeTemplate;
        public UButtonGroup CupTypeBtnGroup;
        public Sprite[] CupDescNormalSprites;
        public Sprite[] CupDescPressedSprites;
        public Sprite[] CupIconSprites;

        private List<LegendCupTypeItem> m_typeItem;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);
            TitleDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupCreateType", "Title");
            ConfirmBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupCreateType", "ConfirmBtn");
            CancelBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupCreateType", "CancelBtn");
            m_typeItem = new List<LegendCupTypeItem>();

            return true;
        }

        public void OnShowCreateTypeWnd()
        {
            int nTypeCount = CupTypeFrame.childCount;
            for (int i = nTypeCount - 1; i >= 0; i--)
            {
                Transform typeRect = CupTypeFrame.GetChild(i);
                if (typeRect)
                {
                    var temp = typeRect.gameObject;
                    ResNode.DestroyRes(ref temp);
                }
            }

            List<SSchemeLegendCupTypeConfig> typeConfigList = LegendCupConfig.Instance.GetCupTypeConfigList();
            if (typeConfigList == null || typeConfigList.Count <= 0)
                return;

            m_typeItem.Clear();
            int index = 0;
            CupTypeBtnGroup.UseGroupIndex = true;
            foreach (SSchemeLegendCupTypeConfig config in typeConfigList)
            {
                if (config.nIsShow <= 0)
                    continue;

                GameObject typeObj = ResNode.InstantiateRes(CupTypeTemplate);
                if (!typeObj)
                    return;
                RectTransform typeTrans = typeObj.transform as RectTransform;
                typeTrans.SetParent(CupTypeFrame, false);
                LegendCupTypeItem typeItem = typeObj.GetComponent<LegendCupTypeItem>();
                typeItem.SetData(this, config.nCreateType);
                m_typeItem.Add(typeItem);
                UButtonGroupItem btnItem = typeObj.GetComponent<UButtonGroupItem>();
                btnItem.GroupIndex = index++;

                btnItem.Group = CupTypeBtnGroup;
                typeObj.SetActive(true);
                
            }
            CupTypeTemplate.SetActive(false);

            CupTypeBtnGroup.onButtonGroupSelectedChange.RemoveListener(OnButtonChange);
            CupTypeBtnGroup.onButtonGroupSelectedChange.AddListener(OnButtonChange);
            CupTypeBtnGroup.SetItemSelectedItem(0);
        }

        public void OnButtonChange(UButtonGroupItem itemOld, UButtonGroupItem itemNew)
        {
            if (itemNew.GroupIndex == 0)
            {
                TypeDesc.text = ULocalizationService.Instance.Get("UIView", "LegendCupCreateType", "TypePerson");
            }
            else if (itemNew.GroupIndex == 1)
            {
                TypeDesc.text = ULocalizationService.Instance.Get("UIView", "LegendCupCreateType", "TypeClan");
            }
        }

        public void OnClickConfirmBtn()
        {
            if (CupTypeBtnGroup.SelectedIndex < 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NOT_SELECT_ANY_CUP_TYPE);
                return;
            }

            cmd_legendcup_req_create_cup_type cupType = new cmd_legendcup_req_create_cup_type();
            cupType.byLegendCupType = (byte)m_typeItem[CupTypeBtnGroup.SelectedIndex].CupCreateType;	// 杯赛创建类型
            cupType.nActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);

            ViewEventHelper.Instance.SendCommand<cmd_legendcup_req_create_cup_type>(GameLogicDef.GVIEWCMD_LEGENDCUP_CREATE_TYPE_REQ, ref cupType);

        }

        public void OnClickCancelBtn()
        {
            m_wnd.UnloadView();
        }

        public void OnClickCloseBtn()
        {
            m_wnd.UnloadView();
        }
    }
}
