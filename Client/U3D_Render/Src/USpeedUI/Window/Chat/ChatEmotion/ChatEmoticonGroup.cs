/*******************************************************************
** 文件名:	ChatEmoticonGroup.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2017 - Speed
** 创建人:	昔文博
** 日  期:	2017/12/14
** 版  本:	1.0
** 描  述:	聊天表情组
** 应  用:  
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using Data.ChatEmoticonConfig;

namespace USpeedUI.Chat
{
    public class ChatEmoticonGroup : MonoBehaviour
    {
        public GameObject DefaultItem;
        public RectTransform ItemContainer;
        public Button LockBtn;

        private int m_serialID;
        private int m_sellID;
        private bool m_locked;
        private bool m_init;

        private List<ChatEmoticonItemBase> m_chatEmoticonItemList;

        public bool Init(int _nSerial, int _nSellID, bool _bLock)
        {
            m_serialID = _nSerial;
            m_sellID = _nSellID;
            m_locked = _bLock;
            m_init = true;

            if (m_chatEmoticonItemList == null)
                m_chatEmoticonItemList = new List<ChatEmoticonItemBase>();

            SetLockState(m_locked);

            if (LockBtn != null)
            {
                LockBtn.onClick.RemoveListener(OnLockBtnClick);
                LockBtn.onClick.AddListener(OnLockBtnClick);
            }
            return true;
        }

        public void AddItem(SSchemeChatEmoticonData _sSchemeData)
        {
            if (!m_init)
                return;

            if (DefaultItem == null)
                return;

            GameObject itemGO = ResNode.InstantiateRes(DefaultItem);
            if (itemGO == null)
                return;

            ChatEmoticonItemBase item = AddEmoticonItemComponent(itemGO, _sSchemeData.eEmoticonType);
            if (item == null)
                return;

            item.SetData(_sSchemeData);
            item.transform.SetParent(ItemContainer, false);
            item.gameObject.SetActive(true);

            m_chatEmoticonItemList.Add(item);
        }

        public void SetLockState(bool _bLock)
        {
            if (!m_init)
                return;

            if (LockBtn == null)
                return;

            LockBtn.gameObject.SetActive(_bLock);

            foreach(ChatEmoticonItemBase item in m_chatEmoticonItemList)
            {
                item.SetLock(_bLock);
            } 
        }

        public void OnLockBtnClick()
        {
            //SetLockState(false);
            PointShopBuyGoodsInitData goodData = new PointShopBuyGoodsInitData();
            goodData.nSellID = m_sellID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_OPEN, goodData);

        }

        private ChatEmoticonItemBase AddEmoticonItemComponent(GameObject _itemGO, EMChatEmoticonType _emoticonType)
        {
            ChatEmoticonItemBase emoticonItem;
            switch (_emoticonType)
            {
                case EMChatEmoticonType.Emoticon_Default:
                    {
                        emoticonItem = _itemGO.AddComponent<ChatEmoticonItem_Default>();
                    }
                    break;
                case EMChatEmoticonType.Emoticon_Large:
                    {
                        emoticonItem = _itemGO.AddComponent<ChatEmoticonItem_Large>();
                    }
                    break;
                case EMChatEmoticonType.Emoticon_Skill:
                    {
                        emoticonItem = _itemGO.AddComponent<ChatEmoticonItem_Skill>();
                    }
                    break;
                default:
                    {
                        emoticonItem = null;
                    }
                    break;
            }

            return emoticonItem;
        }
    }
}
