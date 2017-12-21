using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using UIWidgets;
using System.Runtime.InteropServices;
using GameLogic;
using UnityEngine.EventSystems;
using DataCenter;

namespace USpeedUI.SideButtonList
{
    public class SideButtonListWnd : UIBaseWnd<SideButtonListWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_SIDEBUTTONLIST;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "SideButtonList/SideButtonList";
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
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_SIDEBUTTONLIST_REFRESH, this);
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_SIDEBUTTONLIST_REFRESH, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_SIDEBUTTONLIST_REFRESH:
                    {
                        if (m_wndView == null)
                            return;

                        m_wndView.RefreshList();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class SideButtonListItem : MonoBehaviour
    {
        public Button button;
        public Text buttonText;
        public Image buttonIcon;
        public UTooltipTrigger tooltip;

        private int m_nItemID;
        public int ItemID { get { return m_nItemID; } }
        private SideButtonCustomData sCustomData;
        private Action<SideButtonCustomData> callBack;

        public void SetData(SideButtonListAddData data, int nCount)
        {
            m_nItemID = data.nItemID;
            if (nCount > 1)
                buttonText.text = data.strButtonText + "(" + nCount + ")";
            else
                buttonText.text = data.strButtonText;
            buttonIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_SideButtonIcon, WndID.WND_ID_SIDEBUTTONLIST, data.nButtonIconID);
            if (data.strTooltip == null || data.strTooltip.Length == 0)
            {
                tooltip.enabled = false;
            }
            else
            {
                tooltip.enabled = true;
                tooltip.SetText(UTooltipParamName.BodyText, data.strTooltip);
            }

            sCustomData = data.sCustomData;
            callBack = data.callBack;
            if (callBack != null)
            {
                button.interactable = true;
            }
            else
            {
                button.interactable = false;
            }

            gameObject.SetActive(true);
        }

        public void Clear()
        {
            gameObject.SetActive(false);
        }

        public void Destroy()
        {
            var temp = gameObject;
            ResNode.DestroyRes(ref temp);
        }

        public void OnClick()
        {
            if (callBack != null)
            {
                callBack(sCustomData);
            }
        }
    }

    public class SideButtonListWndView : UIBaseWndView
    {
        private ResNode buttonPrefabRes;
        private GameObject buttonPrefab;
        public Transform buttonContainer;
        public Button upBtn;
        public Button downBtn;

        private SideButtonListItem[] buttonList;
        private int m_nCurIndexOffset;
        private const int m_nMaxListItemNum = 10;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);
            buttonPrefabRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, UPath.UPrefabsPath + "SideButtonList/SideButtonListItem");

            buttonList = new SideButtonListItem[m_nMaxListItemNum];
            float yOffset = -293f;
            for (int i = 0; i < m_nMaxListItemNum; ++i)
            {
                if (buttonPrefabRes != null)
                {
                    SideButtonListItem item = buttonPrefabRes.InstanceMainRes<SideButtonListItem>();
                    buttonList[i] = item;
                    RectTransform btnTrans = item.transform as RectTransform;
                    Vector3 btnPos = btnTrans.localPosition;
                    btnPos.y = yOffset;
                    btnTrans.localPosition = btnPos;
                    yOffset += 55f;
                    btnTrans.SetParent(buttonContainer, false);

                    item.gameObject.SetActive(false);
                }
            }
            upBtn.gameObject.SetActive(false);
            downBtn.gameObject.SetActive(false);

            m_nCurIndexOffset = 0;

            return true;
        }

        public override void Destroy()
        {
            buttonPrefab = null;
            AssetBundleManager.DeleteAssets(ref buttonPrefabRes, true);
            base.Destroy();
        }

        public void OnUpBtn()
        {
            ++m_nCurIndexOffset;
            RefreshList();
        }

        public void OnDownBtn()
        {
            --m_nCurIndexOffset;
            RefreshList();
        }

        public void RefreshList()
        {
            List<SideButtonListAddData> itemList = LogicDataCenter.sideButtonDataManager.ItemList;
            Dictionary<int, List<SideButtonListAddData>> bakItemList = LogicDataCenter.sideButtonDataManager.BakItemList;
            if (itemList == null || bakItemList == null)
                return;

            if (m_nCurIndexOffset + m_nMaxListItemNum > itemList.Count)
                m_nCurIndexOffset = itemList.Count - m_nMaxListItemNum;
            if (m_nCurIndexOffset < 0)
                m_nCurIndexOffset = 0;

            for (int i = 0; i < m_nMaxListItemNum; ++i)
            {
                int nIndex = i + m_nCurIndexOffset;
                if (nIndex < itemList.Count)
                {
                    int nCount = 1;
                    List<SideButtonListAddData> curBakList;
                    if (bakItemList.TryGetValue(itemList[nIndex].nItemID, out curBakList))
                    {
                        nCount += curBakList.Count;
                    }
                    buttonList[i].SetData(itemList[nIndex], nCount);
                }
                else
                {
                    buttonList[i].Clear();
                }
            }

            if (m_nCurIndexOffset > 0)
                downBtn.gameObject.SetActive(true);
            else
                downBtn.gameObject.SetActive(false);

            if (m_nCurIndexOffset + m_nMaxListItemNum < itemList.Count)
                upBtn.gameObject.SetActive(true);
            else
                upBtn.gameObject.SetActive(false);
        }
    }
}
