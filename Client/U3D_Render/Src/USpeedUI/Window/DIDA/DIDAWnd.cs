/*******************************************************************
** 文件名:	DIDAWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	2016-9-7
** 版  本:	1.0
** 描  述:	DIDA界面逻辑模块(接发消息)+视图模块(显示数据)	
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using UIWidgets;
using DataCenter;
using USpeedUI.UEffect;

namespace USpeedUI.DIDA
{
    public class DIDAWndItem : MonoBehaviour
    {
        public Button button;
        public Image buttonIcon;
        public UTooltipTrigger toolTip;

        private int m_nItemID;
        private SideButtonCustomData sCustomData;
        private Action<SideButtonCustomData> callBack;

        private UEffectParamBase param;

        public void SetData(DIDAAddData data)
        {
            m_nItemID = data.nItemID;
            sCustomData = data.sCustomData;
            callBack = data.callBack;

            const int nOffset = 2;
            buttonIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_SideButtonIcon, WndID.WND_ID_DIDA, data.nButtonIconID + nOffset);

            if (data.strTooltip == null || data.strTooltip.Length == 0)
            {
                toolTip.enabled = false;
            }
            else
            {
                toolTip.enabled = true;
                toolTip.SetText(UTooltipParamName.BodyText, data.strTooltip);
            }

            if (callBack != null)
            {
                button.interactable = true;
            }
            else
            {
                button.interactable = false;
            }

            if (param != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
            }
            param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_DIDAView_DiDa, _tfAttachParent: this.gameObject.transform, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);
            
            gameObject.SetActive(true);
        }

        public void Clear()
        {
            if (param != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
            }
            
            gameObject.SetActive(false);
        }

        public void OnClick()
        {
            //if (LogicDataCenter.didaDataManager.bInProcessList[m_nItemID])
            //{
            //    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_ENTITY_DIDA_CANNOT_OPEN);
            //    return;
            //}

            //// 置位该类型正在处理中
            //LogicDataCenter.didaDataManager.bInProcessList[m_nItemID] = true;

            callBack(sCustomData);

            DIDARemoveData msg = new DIDARemoveData();
            msg.nItemID = m_nItemID;
            msg.sCustomData = sCustomData;
            LogicDataCenter.didaDataManager.RemoveItem(msg);
        }
    }

    public class DIDAWnd : UIBaseWnd<DIDAWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_DIDA;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "DIDA/DIDAView";
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
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_DIDA_UPDATE, this);
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_DIDA_UPDATE, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                            m_wndView.RefreshList(msgData as DIDARefreshData);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        if (m_wndView != null)
                            m_wndView.DealNotInWarDida();
                        //UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        if (m_wndView != null)
                            m_wndView.RefreshLayoutPosition(EDIDALayout.EDT_Left, EDIDALayoutMoveType.MoveType_Up);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        if (m_wndView != null)
                            m_wndView.RefreshLayoutPosition(EDIDALayout.EDT_Left, EDIDALayoutMoveType.MoveType_Down);
                    }
                    break;
                case WndMsgID.WND_MSG_DIDA_UPDATE:
                    {
                        if (m_wndView != null)
                            m_wndView.RefreshList(msgData as DIDARefreshData);
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public enum EDIDALayoutMoveType
    {
        MoveType_Up,
        MoveType_Down,
    }

    public class DIDAWndView : UIBaseWndView
    {
        public RectTransform[] LayoutGroups;
        public DIDAWndItem[] buttonList;
        public float LayoutMovePixel = 300;  

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            for (int i = 0; i < buttonList.Length; i++)
            {
                buttonList[i].gameObject.SetActive(false);
            }

            return true;
        }

        public void RefreshList(DIDARefreshData reFreshData)
        {
            if (null == reFreshData)
                return;

            List<DIDAAddData> viewList = LogicDataCenter.didaDataManager.ViewList;

            if (viewList.Count > buttonList.Length)
                return;

            int index = 0;
            int maxCount = LogicDataCenter.didaDataManager.MaxDidaItemNum;
            int startIndex = (int)reFreshData.eLayout * maxCount;
            for (int i = startIndex; i < startIndex + maxCount; i++)
            {
                if (i < startIndex + viewList.Count)
                {
                    buttonList[i].SetData(viewList[index]);
                    index++;
                }
                else
                {
                    buttonList[i].Clear();
                }
            }
        }

        public void RefreshLayoutPosition(EDIDALayout layout, EDIDALayoutMoveType moveType)
        {
            if (LayoutGroups.Length <= (int)layout)
                return;

            Vector2 newPos = new Vector2(LayoutGroups[(int)layout].anchoredPosition.x, LayoutGroups[(int)layout].anchoredPosition.y);

            if (moveType == EDIDALayoutMoveType.MoveType_Up)
                newPos.y += Mathf.Abs(LayoutMovePixel);
            else if(moveType == EDIDALayoutMoveType.MoveType_Down)
                newPos.y += -Mathf.Abs(LayoutMovePixel);

            LayoutGroups[(int)layout].anchoredPosition = newPos;
        }

        public void DealNotInWarDida()
        {
            Dictionary<EDIDALayout, List<DIDAAddData> > didaDic = LogicDataCenter.didaDataManager.ItemList;
            Dictionary<EDIDALayout, List<float>>  timeDic = LogicDataCenter.didaDataManager.TimeList;
            if (didaDic == null || timeDic == null)
                return;

            for(int i = 0; i < (int)didaDic.Count; i++)
            {
                List<DIDAAddData> dataList = didaDic[(EDIDALayout)i];
                List<float> timeList = timeDic[(EDIDALayout)i];
                for (int j = dataList.Count - 1; j >= 0; --j)
                {
                    if (!dataList[j].bShowInWar)
                    {
                        dataList.RemoveAt(j);
                        timeList.RemoveAt(j);
                    }
                }
                didaDic[(EDIDALayout)i] = dataList;
                timeDic[(EDIDALayout)i] = timeList;
                LogicDataCenter.didaDataManager.RefreshViewData((EDIDALayout)i);
            }
        }

    }
}
