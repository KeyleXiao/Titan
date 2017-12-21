using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using UIWidgets;
using DataCenter;
using ASpeedGame.Data.ConsumptionGuideConfig;

namespace USpeedUI.ConsumptionGuide
{
    public class ConsumptionGuideWnd : UIPopupWnd<ConsumptionGuideWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_CONSUMPTIONGUIDE;
        }

        public override String GetPrefabPath()
        {
            return "ConsumptionGuide/ConsumptionGuideView";
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        public override bool Init()
        {
            base.Init();
            m_isKeyClose = true;
            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CONSUMPTIONGUIDE_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            return true;
        }

        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CONSUMPTIONGUIDE_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_CONSUMPTIONGUIDE_SHOW:
                    {
                        SetVisible(true);

                        UConsumptionMsgData cmdData = msgData as UConsumptionMsgData;
                        if (m_wndView != null)
                            m_wndView.OnShowConsumptionGuideWnd(cmdData.nRequestType, cmdData.nGuideType);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        UnloadView();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class ConsumptionGuideTypeItem : MonoBehaviour
    {
        public Text GuideName;
        public UButtonGroupItem SelectBtn;
        private int m_nGuideType;
        public int GuideType { get { return m_nGuideType; } }
        private ConsumptionGuideWndView m_wndView;

        public void SetData(ConsumptionGuideWndView view, int nType, string strName)
        {
            m_wndView = view;
            m_nGuideType = nType;
            GuideName.text = strName;

            SelectBtn.onSelectedChanged.AddListener(OnSelectBtnClick);
            this.gameObject.SetActive(true);
        }

        public void OnSelectBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            m_wndView.SetGuideItemFrame(m_nGuideType);
        }
    }

    public class ConsumptionGuideWndItem : MonoBehaviour
    {
        public Text GuideDesc;
        public Text GotoBtnDesc;
        public Button GotoBtn;
        private DataCenter.TaskGuideManager.Func_Guide_Handler m_callback;
        private ConsumptionGuideWndView m_wndView;

        public void SetData(ConsumptionGuideWndView view, string strGuideDesc, DataCenter.TaskGuideManager.Func_Guide_Handler callBack)
        {
            m_wndView = view;
            m_callback = callBack;
            GuideDesc.text = strGuideDesc;
            GotoBtnDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "GotoBtnDesc");

            GotoBtn.onClick.AddListener(OnGotoBtnClick);
            this.gameObject.SetActive(true);
        }

        public void OnGotoBtnClick()
        {
            if (m_callback != null)
            {
                m_callback();
                // TODO 暂时关闭
                m_wndView.OnClickCloseBtn();
            }   
        }
    }

    public class ConsumptionGuideWndView : UIBaseWndView
    {
        public RectTransform GuideSelectFrame;
        public GameObject GuideSelctTemplate;
        private List<ConsumptionGuideTypeItem> m_typeItemList;

        public Text GuideTitle;
        public RectTransform GuideItemFrame;
        public GameObject GuideItemTemplate;

        public override bool Init(IUIWnd wnd)
        {
            m_typeItemList = new List<ConsumptionGuideTypeItem>();
            return base.Init(wnd);
        }

        public void OnShowConsumptionGuideWnd(int nReqType, int nGuideType)
        {
            // 处理引导选项
            SetGuideSelectFrame(nReqType, nGuideType);

            // 处理引导跳转窗口
            SetGuideItemFrame(nGuideType);
        }

        public void SetGuideSelectFrame(int nReqType, int nGuideType)
        {
            Dictionary<int, SSchemeGuideType> guideTypeConfigDic = ConsumptionGuideConfig.Instance.GetGuideTypeDic();
            if (guideTypeConfigDic == null || guideTypeConfigDic.Count <= 0)
                return;

            int nTypeCount = GuideSelectFrame.childCount;
            for (int i = nTypeCount - 1; i >= 0; --i)
            {
                Transform childTrans = GuideSelectFrame.GetChild(i);
                if (childTrans)
                {
                    var temp = childTrans.gameObject;
                    ResNode.DestroyRes(ref temp);
                }
            }

            m_typeItemList.Clear();
            for (int i = 1; i < guideTypeConfigDic.Count + 1; i++)
            {
                // 晋升需显示勋章和金币，Xp、元宝只显示对应项
                if (nReqType == (int)EConsumptionRequestType.Request_Upstar)
                {
                    if (guideTypeConfigDic[i].nTypeId != (int)EConsumptionGuideType.Guide_HeroCard && guideTypeConfigDic[i].nTypeId != (int)EConsumptionGuideType.Guide_Gold)
                        continue;
                }
                else
                {
                    if (guideTypeConfigDic[i].nTypeId != nGuideType)
                        continue;
                }

                // 实例化对应项
                GameObject typeObj = ResNode.InstantiateRes(GuideSelctTemplate);
                if (!typeObj)
                    return;

                RectTransform typeRect = typeObj.transform as RectTransform;
                if (!typeRect)
                    return;
                typeRect.SetParent(GuideSelectFrame, false);

                ConsumptionGuideTypeItem typeItem = typeObj.GetComponent<ConsumptionGuideTypeItem>();
                if (!typeItem)
                    return;
                typeItem.SetData(this, guideTypeConfigDic[i].nTypeId, guideTypeConfigDic[i].strGuideName);
                m_typeItemList.Add(typeItem);
            }
            GuideSelctTemplate.SetActive(false);

            foreach (var item in m_typeItemList)
            {
                if (item.GuideType == nGuideType)
                    item.SelectBtn.Selected = true;
                else
                    item.SelectBtn.Selected = false;
            }
        }

        public void SetGuideItemFrame(int nTypeId)
        {
            SSchemeGuideType guideTypeConfig = ConsumptionGuideConfig.Instance.GetGuideType(nTypeId);
            if (guideTypeConfig == null)
                return;

            GuideTitle.text = guideTypeConfig.strGuideTitle;

            int nItemCount = GuideItemFrame.childCount;
            for (int i = nItemCount - 1; i >= 0; --i)
            {
                Transform childTrans = GuideItemFrame.GetChild(i);
                if (childTrans)
                {
                    var temp = childTrans.gameObject;
                    ResNode.DestroyRes(ref temp);
                }
            }

            for (int i = 0; i < guideTypeConfig.guideWndList.Count; i++)
            {
                GameObject itemObj = ResNode.InstantiateRes(GuideItemTemplate);
                if (!itemObj)
                    return;

                RectTransform itemRect = itemObj.transform as RectTransform;
                if (!itemRect)
                    return;
                itemRect.SetParent(GuideItemFrame, false);

                ConsumptionGuideWndItem itemItem = itemObj.GetComponent<ConsumptionGuideWndItem>();
                if (!itemItem)
                    return;
                itemItem.SetData(this, guideTypeConfig.guideWndList[i].nGuideDesc, LogicDataCenter.taskDataManager.CTaskGuideManager.GuideHandlerMap[guideTypeConfig.guideWndList[i].nGuideWnd]);
            }
            GuideItemTemplate.SetActive(false);
        }

        public void OnClickCloseBtn()
        {
            SetVisible(false);
        }
    }
}
