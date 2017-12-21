using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using DataCenter;
using Kin_ManagerDef;

namespace USpeedUI.Social
{
    public class KinWnd : UIBaseSubWnd<KinWndView>
    {
        // 获取ID
        public override int GetWndType()
        {
            return (int)SocialSubWndType.WND_KIN;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewKin";
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            SetVisible(false);
            return true;
        }

        // 设置是否可见
        public override void SetVisible(bool visible)
        {
            if (visible)
                OnOpenKinWnd();

            base.SetVisible(visible);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData data = msgData as UIMsgCmdData;
            switch (msgID)
            {
                case WndMsgID.WND_MSG_KIN_REQUESTKINLIST:
                    {
                        if (m_wndView != null)
                            m_wndView.onRecvKinList();
                    }
                    break;
                case WndMsgID.WND_MSG_KIN_UPDATEKINLIST:
                    {
                        if (m_wndView != null)
                            m_wndView.OnUpdateKinList(data.nParam);
                    }
                    break;
                
                default:
                    break;
            }
        }

        public void OnOpenKinWnd()
        {
            // 打开战队列表
            GameLogic.cmd_kin_requestkinlist cmdData = new GameLogic.cmd_kin_requestkinlist();
            cmdData.nCurIndex = 1;
            cmdData.nPageNum = 1;
            cmdData.nState = 1;
            cmdData.szKinName = "";
            ViewEventHelper.Instance.SendCommand<GameLogic.cmd_kin_requestkinlist>(GameLogicDef.GVIEWCMD_KIN_REQUESTKINLIST, ref cmdData);
        }
    }

    public class KinWndView : UIBaseSubWndView
    {
        public Transform SerchTI;
        public UListKin ListKin;
        private int nCurSelectIndex;
        private UListItemKin CurSelectItem;
        private UIWidgets.ObservableList<UListItemKin> AllDataSource = new UIWidgets.ObservableList<UListItemKin>();
        InputField SerchInfo;
        public Text searchPlaceHolder;
        public Text HeadName;
        public Text HeadCount;
        public Text HeadMasterName;
        public Text HeadJoinedDes;
        public Text HeadApplyDes;
        public Text SysDes;
        public Text KinCreateBtnDes;
        public Text KinApplayBtnDes;
        public GameObject KinCreateBtn;
        public GameObject KinApplyBtn;
        public GameObject KinDetailBtn;

        public Button prevPageBtn;
        public Button nextPageBtn;
        public Text pageText;
        private int m_nCurPage;
        private int m_nMaxPage;
        private int pageMaxItemCount = 12;

        public override bool Init(IUISubWnd wnd)
        {
            SerchInfo = SerchTI.GetComponent<InputField>();

            HeadName.text = ULocalizationService.Instance.Get("UIView", "KinList", "HeadName");
            HeadCount.text = ULocalizationService.Instance.Get("UIView", "KinList", "HeadCount");
            HeadMasterName.text = ULocalizationService.Instance.Get("UIView", "KinList", "HeadMasterName");
            HeadJoinedDes.text = ULocalizationService.Instance.Get("UIView", "KinList", "HeadJoinedDes");
            HeadApplyDes.text = ULocalizationService.Instance.Get("UIView", "KinList", "HeadApplyDes");
            KinCreateBtnDes.text = ULocalizationService.Instance.Get("UIView", "KinList", "KinCreateBtnDes");
            KinApplayBtnDes.text = ULocalizationService.Instance.Get("UIView", "KinList", "KinApplayBtnDes");
            searchPlaceHolder.text = ULocalizationService.Instance.Get("UIView", "KinList", "searchPlaceHolder");

            prevPageBtn.onClick.AddListener(OnPrevPageBtn);
            nextPageBtn.onClick.AddListener(OnNextPageBtn);
            pageText.text = "";
            m_nMaxPage = 0;
            m_nCurPage = 0;
            return base.Init(wnd);
        }

        public void onRecvKinList()
        {
            // 有战队时只显示返回Btn
            if (LogicDataCenter.kinDataManager.IsKinBaseDataValid())
            {
                KinCreateBtn.SetActive(false);
                KinApplyBtn.SetActive(false);
                KinDetailBtn.SetActive(true);
            }
            else
            {
                KinCreateBtn.SetActive(true);
                KinApplyBtn.SetActive(true);
                KinDetailBtn.SetActive(false);
            }

            ListKin.OnSelect.RemoveListener(onSelectKin);
            ListKin.DataSource.Clear();
            AllDataSource.Clear();

            cmd_kin_list_head listHead = LogicDataCenter.kinDataManager.KinListHead;
            Dictionary<int,cmd_kin_list_node> listData = LogicDataCenter.kinDataManager.KinListData;

            UIWidgets.ObservableList<UListItemKin> dataSource = new UIWidgets.ObservableList<UListItemKin>();

            int nIndex = 1;
            ICollection<cmd_kin_list_node> kinList = LogicDataCenter.kinDataManager.SortKinList(listData.Values);
            if (kinList != null)
            {
                foreach (cmd_kin_list_node keyValue in kinList)
                {
                    GameLogic.cmd_kin_list_node cmdNodeData = keyValue;
                    UListItemKin item = new UListItemKin();
                    item.nIndex = nIndex;
                    item.KinID = cmdNodeData.dwID;
                    item.Name += cmdNodeData.szName;
                    item.MasterName += cmdNodeData.szShaikhName;
                    item.Count += cmdNodeData.nMemberCount;
                    if (cmdNodeData.dwLeftMemberCount > 0)
                    {
                        item.KinDes = String.Format(ULocalizationService.Instance.Get("UIView", "KinList", "KinMemberLeftDes"), cmdNodeData.dwLeftMemberCount);
                    }
                    else
                    {
                        item.KinDes = ULocalizationService.Instance.Get("UIView", "KinList", "MemberFull");
                    }

                    if (cmdNodeData.bIsApply > 0)
                    {
                        item.ApplyDes = ULocalizationService.Instance.Get("UIView", "KinList", "HaveApply");
                        item.ApplyDes = "<color=#18F00F>" + item.ApplyDes + "</color>";
                    }
                    else
                    {
                        if (cmdNodeData.bAutoAccpet > 0)
                        {
                            string sexDes = string.Empty;
                            if (((TKinAutoFlag)cmdNodeData.dwAcceptFlag & TKinAutoFlag.emKinAuto_AllSex) == TKinAutoFlag.emKinAuto_AllSex)
                            {

                                sexDes = ULocalizationService.Instance.Get("UIView", "KinList", "AcceptNoSexLimitDes");
                            }
                            else
                            {
                                if (((TKinAutoFlag)cmdNodeData.dwAcceptFlag & TKinAutoFlag.emKinAuto_Male) == TKinAutoFlag.emKinAuto_Male)
                                {
                                    sexDes = ULocalizationService.Instance.Get("UIView", "KinList", "AcceptMaleDes");

                                }
                                else if (((TKinAutoFlag)cmdNodeData.dwAcceptFlag & TKinAutoFlag.emKinAuto_Female) == TKinAutoFlag.emKinAuto_Female)
                                {
                                    sexDes = ULocalizationService.Instance.Get("UIView", "KinList", "AcceptFemleDes");
                                }
                            }

                            item.ApplyDes = String.Format(ULocalizationService.Instance.Get("UIView", "KinList", "AutoAcceptDes"), cmdNodeData.nLevelLimit, sexDes);
                        }
                        else
                        {
                            item.ApplyDes = ULocalizationService.Instance.Get("UIView", "KinList", "NotSetAcceptDes");
                        }
                        item.ApplyDes = "<color=#C0A47B>" + item.ApplyDes + "</color>";
                    }

                    dataSource.Add(item);
                    nIndex++;
                }
            }
            m_nMaxPage = (dataSource.Count % pageMaxItemCount == 0) ? dataSource.Count / pageMaxItemCount : dataSource.Count / pageMaxItemCount + 1;
            // 保存全部数据 供还原查询数据用
            UListItemKin[] temp = new UListItemKin[dataSource.Count];
            dataSource.CopyTo(temp);

            foreach (UListItemKin Item in temp)
            {
                AllDataSource.Add(Item);
            }

            ListKin.DataSource = dataSource;
            ListKin.OnSelect.AddListener(onSelectKin);

            // 显示当前的页数数据
            m_nCurPage = m_nMaxPage == 0 ? 0 : 1;
            ShowPageText();
        }

        public void OnUpdateKinList(int nKinId)
        {
            cmd_kin_list_node node = new cmd_kin_list_node();
            if (!LogicDataCenter.kinDataManager.KinListData.TryGetValue(nKinId, out node))
            {
                return;
            }

            bool bHave = false;
            UListItemKin item = new UListItemKin();
            foreach (var kinData in AllDataSource)
            {
                if (kinData.KinID == node.dwID)
                {
                    bHave = true;
                    item = kinData;
                }
            }
            if (!bHave)
            {
                AllDataSource.Add(item);
                item.nIndex = AllDataSource.Count + 1;
            }

            item.KinID = node.dwID;
            item.Name = node.szName;
            item.MasterName = node.szShaikhName;
            item.Count = node.nMemberCount.ToString();
            if (node.dwLeftMemberCount > 0)
            {
                item.KinDes = String.Format(ULocalizationService.Instance.Get("UIView", "KinList", "KinMemberLeftDes"), node.dwLeftMemberCount);
            }
            else
            {
                item.KinDes = ULocalizationService.Instance.Get("UIView", "KinList", "MemberFull");
            }

            if (node.bIsApply > 0)
            {
                item.ApplyDes = ULocalizationService.Instance.Get("UIView", "KinList", "HaveApply");
                item.ApplyDes = "<color=#18F00F>" + item.ApplyDes + "</color>";
            }
            else
            {
                if (node.bAutoAccpet > 0)
                {
                    string sexDes = string.Empty;
                    if (((TKinAutoFlag)node.dwAcceptFlag & TKinAutoFlag.emKinAuto_AllSex) == TKinAutoFlag.emKinAuto_AllSex)
                    {

                        sexDes = ULocalizationService.Instance.Get("UIView", "KinList", "AcceptNoSexLimitDes");
                    }
                    else
                    {
                        if (((TKinAutoFlag)node.dwAcceptFlag & TKinAutoFlag.emKinAuto_Male) == TKinAutoFlag.emKinAuto_Male)
                        {
                            sexDes += ULocalizationService.Instance.Get("UIView", "KinList", "AcceptMaleDes");

                        }

                        if (!sexDes.Equals(string.Empty))
                        {
                            sexDes += "&";
                        }
                        if (((TKinAutoFlag)node.dwAcceptFlag & TKinAutoFlag.emKinAuto_Female) == TKinAutoFlag.emKinAuto_Female)
                        {
                            sexDes += ULocalizationService.Instance.Get("UIView", "KinList", "AcceptFemleDes");

                        }
                    }

                    item.ApplyDes = String.Format(ULocalizationService.Instance.Get("UIView", "KinList", "AutoAcceptDes"), node.nLevelLimit, sexDes);
                }
                else
                {
                    item.ApplyDes = ULocalizationService.Instance.Get("UIView", "KinList", "NotSetAcceptDes");
                }
                item.ApplyDes = "<color=#C0A47B>" + item.ApplyDes + "</color>";
            }

            // 显示当前的页数数据
            ShowKinListPageInfo(m_nCurPage);
        }

        void OnPrevPageBtn()
        {
            m_nCurPage = m_nCurPage <= 1 ? 1 : --m_nCurPage;
            ShowKinListPageInfo(m_nCurPage);
            ShowPageText();
        }

        void OnNextPageBtn()
        {
            m_nCurPage = m_nCurPage >= m_nMaxPage ? m_nMaxPage : ++m_nCurPage;
            ShowKinListPageInfo(m_nCurPage);
            ShowPageText();
        }

        void ShowPageText()
        {
            pageText.text = m_nCurPage + "/" + m_nMaxPage;
            if (m_nCurPage <= 1)
            {
                prevPageBtn.interactable = false;
            }
            else
            {
                prevPageBtn.interactable = true;
            }

            if (m_nCurPage >= m_nMaxPage)
            {
                nextPageBtn.interactable = false;
            }
            else
            {
                nextPageBtn.interactable = true;
            }
        }
        void ShowKinListPageInfo(int pageIndex)
        {
            if (pageIndex < 1)
            {
                return;
            }

            // 显示第几页数据
            IEnumerable<UListItemKin> ListQuery = from item in AllDataSource
                                                  where ((pageIndex - 1) * pageMaxItemCount + 1) <= item.nIndex && item.nIndex <= pageIndex * pageMaxItemCount
                                                  select item;

            UIWidgets.ObservableList<UListItemKin> dataSource = new UIWidgets.ObservableList<UListItemKin>();
            foreach (UListItemKin Item in ListQuery)
            {
                dataSource.Add(Item);
            }

            ListKin.DataSource = dataSource;

        }

        public void onSelectKin(int index, UIWidgets.ListViewItem item)
        {
            // 选择当前数据
            nCurSelectIndex = index;
            CurSelectItem = ListKin.DataSource[index];
        }

        public void OnCreateKinBtnClick()
        {
            // 创建战队界面显示
            UISystem.Instance.ShowWnd(WndID.WND_ID_KIN_MSGBOXCREATEKIN, true);

        }

        public void OnMyKinDetailBtnClick()
        {
            // 请求战队信息
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_RETURN_TO_SELFKIN, null);
        }

        public void OnKinApplyBtnClick()
        {
            // 请求加入战队
            if (CurSelectItem == null)
                return;
            GameLogic.cmd_kin_apply cmdData = new GameLogic.cmd_kin_apply();
            cmdData.nKinID = CurSelectItem.KinID;
            ViewEventHelper.Instance.SendCommand<cmd_kin_apply>(GameLogicDef.GVIEWCMD_KIN_APPLY, ref cmdData);
        }

        public void OnClickSearchKin()
        {
            // 查询数据
            if (SerchInfo == null)
            {
                return;
            }

            if (SerchInfo.text == string.Empty)
            {
                // 所有数据显示
                ShowKinListPageInfo(m_nCurPage);
                ShowPageText();
            }
            else
            {
                // 显示过滤的数据
                IEnumerable<UListItemKin> ListQuery = from item in AllDataSource
                                                      where item.Name.Contains(SerchInfo.text)
                                                      select item;

                UIWidgets.ObservableList<UListItemKin> dataSource = new UIWidgets.ObservableList<UListItemKin>();
                foreach (UListItemKin Item in ListQuery)
                {
                    dataSource.Add(Item);
                }

                ListKin.DataSource = dataSource;
            }
        }
    }
}
