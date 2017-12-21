/*******************************************************************
** 文件名:	NPCWnd.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2017/2/7
** 版  本:	1.0
** 描  述:	NPC弹框
** 应  用:  
 *****************************************************************/

using System;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine;
using DataCenter;
using USpeedUI.WarGoodsStore;
using System.Linq;
using UIWidgets;
using USpeedUI.UWidgets;
using ASpeedGame.Data.GameMobaSchemes;
using USpeedUI.GoodsDisposition;
using GameLogic;
using System.Runtime.InteropServices;
using War;

namespace USpeedUI.NPC
{
    public enum ESubViewFrame
    {
        WING_FRAME,
        GOOODS_SHOP_FRAME,
        FRAME_MAX,
    }

    public abstract class UISubViewFrame : MonoBehaviour
    {
        public abstract int GeSubFrameType();
        public abstract void Init(NPCWndView masterView);

        public abstract void ProcessNPCResponse(UNPCResponseData data);
    }

    public class NPCWnd : UIBaseWnd<NPCWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_NPC;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        public override string GetPrefabPath()
        {
            return "NPC/NPCView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_NPC_RESPONSE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_NPC_CLOSE_DIALOGBOX, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOCATION_CHANGE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_NPC_BUY_GOODS_SUCCESS, this);
            
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_NPC_RESPONSE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_NPC_CLOSE_DIALOGBOX, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOCATION_CHANGE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_NPC_BUY_GOODS_SUCCESS, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        LoadView();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
                case WndMsgID.WND_MSG_NPC_CLOSE_DIALOGBOX:
                case WndMsgID.WND_MSG_COMMON_WAREND:
                    {
                        SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_NPC_RESPONSE:
                    {
                        if (m_wndView != null)
                            m_wndView.onNPCResponse(data as UNPCResponseData);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOCATION_CHANGE:
                    {
                        if (m_wndView != null)
                            m_wndView.onActorLocationChange(data as UActorLocationChangeMsgData);
                    }
                    break;
                case WndMsgID.WND_MSG_NPC_BUY_GOODS_SUCCESS:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnBuyGoodsSuccess(data as UIMsgCmdData);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class NPCWndView : UIBaseWndView
    {
        public UISubViewFrame[] arraySubViewFrame = new UISubViewFrame[(int)ESubViewFrame.FRAME_MAX];

        private uint m_CurNPCUID = 0;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            foreach (UISubViewFrame frame in arraySubViewFrame)
            {
                frame.Init(this);
            }

            return true;
        }

        public void OnBuyGoodsSuccess(UIMsgCmdData msg)
        {
            if (msg == null)
                return;

            if (arraySubViewFrame[(int)ESubViewFrame.GOOODS_SHOP_FRAME].gameObject.activeSelf)
                (arraySubViewFrame[(int)ESubViewFrame.GOOODS_SHOP_FRAME] as NPCGoodsShopFrame).OnBuyGoodsSuccess(msg);
        }

        public void onNPCResponse(UNPCResponseData data)
        {
            processNPCResponse(data);
        }

        // 处理NPC回应：以Button的方式触发Topic
        public void processNPCResponse(UNPCResponseData data)
        {
            // 显示对话框内容
            m_wnd.SetVisible(true);
            m_CurNPCUID = data.npcResp.NpcUID;
            int frameType;
            switch (data.npcResp.NpcViewType)
            {
                case (int)NPC_VIEW_TYPE.NPC_VIEW_WAR_WING:
                    {
                        frameType = (int)ESubViewFrame.WING_FRAME;
                    }
                    break;
                case (int)NPC_VIEW_TYPE.NPC_VIEW_WAR_GOODS_SHOP:
                    {
                        frameType = (int)ESubViewFrame.GOOODS_SHOP_FRAME;
                    }
                    break;
                default:
                    Debug.LogWarningFormat("NPCWnd:not match view type({0}).", data.npcResp.NpcViewType);
                    return;
            }

            SwitchFrame(frameType);
            arraySubViewFrame[frameType].ProcessNPCResponse(data);
        }

        public void SwitchFrame(int viewType)
        {
            foreach (UISubViewFrame frame in arraySubViewFrame)
            {
                frame.gameObject.SetActive(frame.GeSubFrameType() == viewType);
            }
        }

        public void ParseURL(string url)
        {
            int i = url.IndexOf('?');
            string handler = url.Substring(0, i);
            if (handler == "HTMLRequestHandler")
            {
                if (i + 1 >= url.Length)
                {
                    return;
                }

                HTMLRequestHandler(url.Substring(i + 1));
            }
        }

        public void HTMLRequestHandler(string paramsStr)
        {
            // 解析参数列表字符串
            List<string> paramList = new List<string>();
            string[] strList1 = paramsStr.Split('&');
            foreach (string str in strList1)
            {
                string[] stringList2 = str.Split('=');
                if (stringList2.Length == 2)
                {
                    paramList.Add(stringList2[1]);
                }
            }

            if (paramList.Count == 0)
            {
                Debug.LogError("HTTPRequestHandler: Params num is empty.");
                return;
            }

            // 第一个参数为要调用的函数名
            string funcName = paramList[0];

            paramList.RemoveAt(0);

            switch (funcName)
            {
                case "RequestBuyWings":
                    {
                        RequestBuyWings(paramList);
                    }
                    break;
                case "RequestWarCureMember":
                    {
                        RequestWarCureMember(paramList);

                    }
                    break;
                case "RequestBuyShopGoods":
                    {
                        RequestBuyShopGoods(paramList);
                    }
                    break;
                default:
                    Debug.LogError("HTTPRequestHandler:There is no the function,funcName=" + funcName);
                    break;
            }
        }

        public void RequestBuyWings(List<string> paramList)
        {
            // 判断参数个数是否一致
            if (paramList.Count != 3)
            {
                Debug.LogError("RequestBuyWings:The params num error, num=" + paramList.Count);
                return;
            }

            uint uidNPC;
            if (!uint.TryParse(paramList[0], out uidNPC))
            {
                Debug.LogError("RequestBuyWings:Get uidNPC failed," + paramList[0]);
                return;
            }

            string url = String.Format("Server/HTMLRequestHandler?1=RequestBuyWings&2={0}&3={1}&4={2}", paramList[0], paramList[1], paramList[2]);

            LogicDataCenter.npcDataManager.reqNPCFunc(uidNPC, url);
        }

        public void RequestWarCureMember(List<string> paramList)
        {
            // 判断参数个数是否一致
            if (paramList.Count != 3)
            {
                Debug.LogError("RequestWarCureMember:The params num error, num=" + paramList.Count);
                return;
            }

            uint uidNPC;
            if (!uint.TryParse(paramList[0], out uidNPC))
            {
                Debug.LogError("RequestWarCureMember:Get uidNPC failed," + paramList[0]);
                return;
            }

            string url = String.Format("Server/HTMLRequestHandler?1=RequestWarCureMember&2={0}&3={1}&4={2}", paramList[0], paramList[1], paramList[2]);

            LogicDataCenter.npcDataManager.reqNPCFunc(uidNPC, url);
        }

        public void RequestBuyShopGoods(List<string> paramList)
        {
            // 判断参数个数是否一致
            if (paramList.Count != 7)
            {
                Debug.LogError("RequestBuyWings:The params num error, num=" + paramList.Count);
                return;
            }

            uint uidNPC;
            if (!uint.TryParse(paramList[0], out uidNPC))
            {
                Debug.LogError("RequestBuyWings:Get uidNPC failed," + paramList[0]);
                return;
            }

            string url = String.Format("Server/HTMLRequestHandler?1=RequestBuyShopGoods&2={0}&3={1}&4={2}&5={3}&6={4}&7={5}&8={6}", paramList[0], paramList[1], paramList[2], paramList[3], paramList[4], paramList[5], paramList[6]);

            LogicDataCenter.npcDataManager.reqNPCFunc(uidNPC, url);
        }

        public void onClickClose()
        {
            m_wnd.SetVisible(false);
        }

        internal void onClickOpen()
        {
            m_wnd.SetVisible(true);
        }

        public void onActorLocationChange(UActorLocationChangeMsgData msgData)
        {
            if (!m_wnd.IsVisible())
                return;

            if (msgData == null)
                return;

            if (m_CurNPCUID <= 0)
                return;

            // 获取NPC位置
            entity_location location = new entity_location();
            if (!GameLogicAPI.GetEntityLocation((int)m_CurNPCUID, ref location))
                return;
            Vector3 NPCPosition = new Vector3(location.x, location.y, location.z);

            // 玩家自己的位置
            if (EntityFactory.MainHeroView == null || !EntityFactory.MainHeroView.IsValid)
                return;
            Vector3 selfPositon = msgData.vNewPos;

            // 与NPC距离超过20M,关闭NPC窗口
            if (Vector3.Distance(NPCPosition, selfPositon) > 20)
            {
                m_wnd.SetVisible(false);
                //onClickClose();
            }
        }
    }

    public class NPCWingFrame : UISubViewFrame
    {
        public Text m_Title;
        public URichText m_Dialog;
        public Transform m_TopicListTrans;
        public GameObject DefaultTopicBtn;

        private NPCWndView m_masterView;

        public override int GeSubFrameType()
        {
            return (int)ESubViewFrame.WING_FRAME;
        }

        public override void Init(NPCWndView masterView)
        {
            m_masterView = masterView;
            m_Dialog.onHrefClick.AddListener(onClickNPCFunc);
        }

        public override void ProcessNPCResponse(UNPCResponseData data)
        {
            m_Title.text = data.npcResp.NpcName;
            m_Dialog.text = data.npcResp.NpcDialog;

            // 清空Topic列表
            for (int i = m_TopicListTrans.childCount - 1; i >= 0; --i)
            {
                var temp = m_TopicListTrans.GetChild(i).gameObject;
                ResNode.DestroyRes(ref temp);
            }

            // 解析生成Topic列表,即NPC功能列表按钮
            foreach (var topic in data.npcResp.NPCTopicList)
            {
                GameObject topicObj = ResNode.InstantiateRes(DefaultTopicBtn);
                if (topicObj == null)
                {
                    continue;
                }

                topicObj.SetActive(true);
                topicObj.transform.SetParent(m_TopicListTrans, false);
                Button topicBtn = topicObj.GetComponent<Button>();
                topicBtn.transform.FindChild("Text").GetComponent<Text>().text = topic.Text;
                string url = topic.Link;
                topicBtn.onClick.AddListener(delegate
                {
                    onClickNPCFunc(url);
                });
            }
        }

        public void onClickNPCFunc(string url)
        {
            this.gameObject.SetActive(false);
            m_masterView.ParseURL(url);
        }
    }

    public class NPCGoodsShopFrame : UISubViewFrame
    {
        public Button buyGoodsBtn;
        public Text buyGoodsBtnDesc;
        public Text m_Title;
        public UNPCGoodsTileView goodsTile;

        private NPCWndView m_masterView;
        private UNPCResponseData m_responseData;

        public override int GeSubFrameType()
        {
            return (int)ESubViewFrame.GOOODS_SHOP_FRAME;
        }

        public override void Init(NPCWndView masterView)
        {
            m_masterView = masterView;
            goodsTile.onDoubleClick.AddListener(OnGoodsItemDoubleClick);
            goodsTile.onRightClick.AddListener(OnGoodsItemRightClick);
        }

        public override void ProcessNPCResponse(UNPCResponseData data)
        {
            m_responseData = data;
            m_Title.text = data.npcResp.NpcName;
            ObservableList<UNPCGoodsItem> dataSource = new ObservableList<UNPCGoodsItem>();
            bool allFree = true;
            foreach (var item in data.npcResp.NPCTopicList)
            {
                if (item.ParamList.Length > 6)
                {
                    int goodsID;
                    int goodsCost;
                    int lastBuyTime;
                    int coolTime;
                    if (!Int32.TryParse(item.ParamList[4], out goodsID))
                        return;

                    if (!Int32.TryParse(item.ParamList[5], out goodsCost))
                        return;

                    if (!Int32.TryParse(item.ParamList[6], out lastBuyTime))
                        return;

                    if (!Int32.TryParse(item.ParamList[7], out coolTime))
                        return;

                    SSchemeMobaGoods schemeGoods = new SSchemeMobaGoods();
                    if (GoodsSchemesDataManager.Instance.GetMobaGoods_Ref(goodsID, ref schemeGoods))
                    {
                        UNPCGoodsItem goodItem = new UNPCGoodsItem();
                        goodItem.schemeGoods = schemeGoods;
                        if (goodsCost >= 0)
                        {
                            goodItem.schemeGoods.GoodsCost = goodsCost;
                            if (allFree && goodsCost > 0)
                                allFree = false;
                        }   

                        goodItem.nLastBuyTime = lastBuyTime;
                        goodItem.nCoolTime = coolTime;
                        goodItem.uidNPC = (int)data.npcResp.NpcUID;
                        dataSource.Add(goodItem);
                    }
                    else
                    {
                        Debug.LogWarningFormat("not find goods by goodsID={0}", goodsID);
                    }
                }
            }
            goodsTile.SetAllDeselect();
            goodsTile.DataSource = dataSource;
            if (goodsTile.DataSource.Count > 0)
                goodsTile.Select(0);
            if (buyGoodsBtnDesc != null)
            {
                if (allFree && data.npcResp.TopicCount == 1)
                {
                    buyGoodsBtnDesc.text = "拾取";
                }
                else
                {
                    buyGoodsBtnDesc.text = "购买";
                }
            }

            InvokeRepeating("SetGoodsMask", 0.1f, 0.5f);
        }

        public void OnBuyGoodsSuccess(UIMsgCmdData msg)
        {
            if (msg == null)
                return;

            SNPCGoodsBuySuccess data = IntPtrHelper.toData<SNPCGoodsBuySuccess>(msg.ptrParam);
            goodsTile.UpdateGoodsMask(data);
        }

        public void SetGoodsMask()
        {
            goodsTile.SetGoodsIconMask();
        }

        public void OnGoodsItemRightClick(int index, ListViewItem item)
        {
            BuyGoods(goodsTile.DataSource[index].schemeGoods.GoodsID);
        }

        public void OnGoodsItemDoubleClick(int index, ListViewItem item)
        {
            BuyGoods(goodsTile.DataSource[index].schemeGoods.GoodsID);
        }

        public void OnBuyGoodsBtnClick()
        {
            if (goodsTile.SelectedIndex < 0 || goodsTile.SelectedIndex >= goodsTile.GetItemsCount())
            {
                return;
            }

            BuyGoods(goodsTile.SelectedItem.schemeGoods.GoodsID);
        }

        private void BuyGoods(int nGoodsID)
        {
            if (m_responseData == null)
                return;

            foreach (var item in m_responseData.npcResp.NPCTopicList)
            {
                int goodsID;
                if (!Int32.TryParse(item.ParamList[4], out goodsID))
                    return;

                if (goodsID == nGoodsID)
                {
                    m_masterView.ParseURL(item.Link);
                }
            }

            //cmd_war_personbuygoods buyGoods = new cmd_war_personbuygoods();
            //buyGoods.nGoodID = nGoodsID;

            //IntPtrHelper helper = new IntPtrHelper();
            //IntPtr ptr = helper.toPtr<cmd_war_personbuygoods>(ref buyGoods);
            //int len = Marshal.SizeOf(buyGoods);
            //GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_WAR_BUY_GOOD, len, "", ptr, len);
        }
    }
}
