using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using USpeedUI.UWidgets;
using UnityEngine.UI;
using ASpeedGame.Data.RunePage;
using DataCenter;
using System.Collections;
using UnityEngine.EventSystems;
using UnityEngine.Events;
using UIWidgetsSamples;
using USpeedUI.UExtensions;
using U3D_Render.Common;
using USpeedUI.PlayerButtonFrame;
using GameLogic;
using USpeedUI.TooltipAndDialog;

namespace USpeedUI.PlayerFrame
{
    public enum RunePlayerType
    {
        RUNEPLAYERTYPE_SELF,
        RUNEPLAYERTYPE_OTHER,
    }

    public class RunePageWnd : UIBaseSubWnd<RunePageWndView>
    {
        private const int nDefaultPageIndex = 1;
        private int m_nOtherPlayerActorId = 0;

        public override int GetWndType()
        {
            return (int)PlayerSubWndType.WND_RUNE;
        }

        public override String GetPrefabPath()
        {
            return "RunePage/PlayerRuneView";
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if (m_wndView == null)
                return;

            if (visible)
            {
                bool bRequestSelf = LogicDataCenter.playerSystemDataManager.bRequestSelf;
                int nPDBIU = LogicDataCenter.playerSystemDataManager.nRequestPDBID;
                OnRequestRunePagePoints(bRequestSelf, nPDBIU);
                m_wndView.gameObject.transform.SetAsLastSibling();
            }
            else
            {
                OnMsgBoxSaveInfo();
            }  
        }

        public override bool Init()
        {
            LoadView();
            SetVisible(false);
            return base.Init();
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_RUNEPAGE_INITRUNEDATA:
                    {
                        if (m_wndView == null || m_wndView.PlayerTypeInRune != RunePlayerType.RUNEPLAYERTYPE_SELF)
                            return;

                        UIMsgCmdData uiRunePage = (UIMsgCmdData)msgData;
                        if (!LogicDataCenter.runePageDataManager.ReceiveSelfEntityViewRuneInfo(ref uiRunePage.ptrParam, uiRunePage.nPtrLen))
                            return;

                        InitRuneToShow();
                    }
                    break;
                case WndMsgID.WND_MSG_RUNEPAGE_INITOTHERRUNEDATA:
                    {
                        if (m_wndView == null || m_wndView.PlayerTypeInRune != RunePlayerType.RUNEPLAYERTYPE_OTHER)
                            return;

                        if (m_nOtherPlayerActorId <= 0)
                            return;

                        if (!LogicDataCenter.runePageDataManager.ReceiveOtherPlayerEntityViewRuneInfo(m_nOtherPlayerActorId))
                            return;

                        InitRuneToShow();
                    }
                    break;
                case WndMsgID.WND_MSG_RUNEPAGE_PUTRUNE:
                    {
                        if (m_wndView == null)
                            return;
                            
                        OnPutRuneItem();
                    }
                    break;
                case WndMsgID.WND_MSG_RUNEPAGE_CHANGEPAGE:
                    {
                        OnSendRunePageIndexForEffect();
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MAIN_OPEN:
                    {
                        OnRequestRunePagePoints(true, 0);
                    }
                    break;
                default:
                    break;
            }
        }

        private void ClearRuneView()
        {
            m_wndView.ClearRuneView();
        }


        public void OnMsgBoxSaveInfo()
        {

            if (m_isVisible && LogicDataCenter.runePageDataManager.GetRunePageChange())
            {
                UIDialogData data = new UIDialogData(title: "符文保存询问",
                                            message: "是否保存当前符文页",
                                            buttons: new UIWidgets.DialogActions() { { "确认", SetSaveSingleRunePage },
                                               { "取消", SetSaveDisable}
                                            },
                                            closeButtonAction: new UnityEngine.Events.UnityAction(SetSaveDisableEx)

                                           );
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
            }
        }

        public void SetSaveDisableEx()
        {
            SetSaveDisable();
            return;
        }

        public bool SetSaveDisable()
        {
            SetRescoverCurPage();
            return true;
        }

        public void SetTalentIsChange(bool bChange)
        {
            if (m_wndView == null)
            {
                return;
            }

            m_wndView.SetButtonState(bChange);
        }

        public void SetPutRuneInfo(PointerEventData eventData)
        {
            if (m_wndView == null)
            {
                return;
            }

            m_wndView.SetPutRuneInfo(eventData);
        }

        public bool SetSaveSingleRunePage()
        {
            cmd_Rune_Single_PageInfo cmdSinglePoint = new cmd_Rune_Single_PageInfo();
            bool bSave = LogicDataCenter.runePageDataManager.SetSaveSingleRunePage(ref cmdSinglePoint);

            if (!bSave)
            {
                return true;
            }

            EntityEventHelper.Instance.SendCommand<cmd_Rune_Single_PageInfo>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_SAVE_RUNE_PAGE, ref cmdSinglePoint);
            return true;
        }

        public void SetRescoverCurPage()
        {
            LogicDataCenter.runePageDataManager.SetRescoverCurPage();

            GetViewDataAndShow();
        }

        public void SetDeleteCurPage()
        {
            cmd_Delete_Rune_Page cmdPointDelete = new cmd_Delete_Rune_Page();
            LogicDataCenter.runePageDataManager.SetDeletePage(ref cmdPointDelete);
            EntityEventHelper.Instance.SendCommand<cmd_Delete_Rune_Page>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_DELETE_RUNE_PAGE, ref cmdPointDelete);

            GetViewDataAndShow();
        }

        public void SetResetCurPage()
        {
            LogicDataCenter.runePageDataManager.SetResetCurPage();

            GetViewDataAndShow();
        }

        public void ChangeRunePageSortIndex(int nPageSortIndex)
        {
            LogicDataCenter.runePageDataManager.SetCurPageIndex(nPageSortIndex);

            GetViewDataAndShow();
        }

        public void SetNewPage(int nPageSortIndex)
        {
            LogicDataCenter.runePageDataManager.SetNewPage(nPageSortIndex);

            GetViewDataAndShow();
        }

        public void SetPageName(string sName)
        {
            LogicDataCenter.runePageDataManager.SetName(sName);

            LogicDataCenter.runePageDataManager.SetRunePageChange(true);

        }

        public void SetRunePropertyType(int nPropertyType)
        {
            LogicDataCenter.runePageDataManager.SetRunePropertyType(nPropertyType);
            m_wndView.SetStoreShow(LogicDataCenter.runePageDataManager.GetInlayTypeAllStore(), LogicDataCenter.runePageDataManager.GetRuneSlotInfo(), LogicDataCenter.runePageDataManager.GetRunePropertyType(), LogicDataCenter.runePageDataManager.GetRuneLevelState());
        }

        public void SetRuneLevelState(int nlevelIndex, bool bIsOn)
        {
            LogicDataCenter.runePageDataManager.SetRuneLevelState(nlevelIndex, bIsOn);
            m_wndView.SetStoreShow(LogicDataCenter.runePageDataManager.GetInlayTypeAllStore(), LogicDataCenter.runePageDataManager.GetRuneSlotInfo(), LogicDataCenter.runePageDataManager.GetRunePropertyType(), LogicDataCenter.runePageDataManager.GetRuneLevelState());
        }

        public void SetRunePagePointsInfo(int nSlotId, int nRuneId, bool bAddOrReduce)
        {
            LogicDataCenter.runePageDataManager.SetRunePagePointsInfo(nSlotId, nRuneId, bAddOrReduce);
        }

        public void OnRequestRunePagePoints(bool bRequestSelf, int nPDBIU)
        {
            m_wndView.PlayerTypeInRune = bRequestSelf ? RunePlayerType.RUNEPLAYERTYPE_SELF : RunePlayerType.RUNEPLAYERTYPE_OTHER;
            LogicDataCenter.runePageDataManager.runePageDataManager.Clear();
            // 请求所有加点情况       
            if (bRequestSelf)
            {
                if (LogicDataCenter.runePageDataManager.RescoverSelfRunePageDateManagerInfo())// 已经有数据恢复成功去显示,没数据恢复成功去请求
                {
                    LogicDataCenter.runePageDataManager.SetCurPageIndex(nDefaultPageIndex);
                    InitRuneToShow();
                }
                else
                {
                    cmd_Request_Rune_Info cmdRequestPointAll = new cmd_Request_Rune_Info();
                    cmdRequestPointAll.bIsRequestSelf = 1;
                    cmdRequestPointAll.RequestFriendID = 0;
                    EntityEventHelper.Instance.SendCommand<cmd_Request_Rune_Info>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_QUERY_RUNE, ref cmdRequestPointAll);
                }
            }
            else
            {
                OtherActorRuneInfo otherInfo = LogicDataCenter.playerSystemDataManager.GetActorAllRune(nPDBIU);
                if (otherInfo == null)
                {
                    return;
                }

                if (otherInfo.totalRune.nPageNum > 0) // 有数据直接显示
                {
                    m_nOtherPlayerActorId = 0;
                    if (!LogicDataCenter.runePageDataManager.ReceiveOtherPlayerEntityViewRuneInfo(nPDBIU))
                    {
                        return;
                    }

                    InitRuneToShow();
                }
                else
                {
                    m_nOtherPlayerActorId = nPDBIU;  // 没数据去请求
                }
            }
        }

        public void InitRuneToShow()
        {
            if (m_wndView == null || !m_isVisible)
            {
                return;
            }

            LogicDataCenter.runePageDataManager.SetCurPageIndex(nDefaultPageIndex);
            ClearRuneView();

            if (m_wndView.PlayerTypeInRune == RunePlayerType.RUNEPLAYERTYPE_SELF)
            {
                SetRuneViewInSelfMode();
            }
            else
            {
                SetRuneViewInOtherPlayerMode();
            }
        }

        private void SetRuneViewInSelfMode()
        {
            m_wndView.nCurPageIndex = nDefaultPageIndex;
            m_wndView.CreateAllTopRuneButton(LogicDataCenter.runePageDataManager.GetRunePageCount());
            m_wndView.SetMultilanguage();
            GetViewDataAndShow();
        }

        private void SetRuneViewInOtherPlayerMode()
        {
            SetRuneViewInSelfMode();
            m_wndView.SetOtherPlayerHidePartVisible(false);
        }

        public void OnSendRunePageIndexForEffect()
        {
            cmd_Effect_Rune_Page cmd_Effec_Page = new cmd_Effect_Rune_Page();
            cmd_Effec_Page.nPageIdx = LogicDataCenter.runePageDataManager.GetDataIndex();

            EntityEventHelper.Instance.SendCommand<cmd_Effect_Rune_Page>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_BEGIN_RUNE_PAGE_EFFECT, ref cmd_Effec_Page);
        }

        public void OnPutRuneItem()
        {
            m_wndView.SetStoreShow(LogicDataCenter.runePageDataManager.GetInlayTypeAllStore(), LogicDataCenter.runePageDataManager.GetRuneSlotInfo(), LogicDataCenter.runePageDataManager.GetRunePropertyType(), LogicDataCenter.runePageDataManager.GetRuneLevelState());
            m_wndView.SetPropertyShow(ref  LogicDataCenter.runePageDataManager.uCurRunePageData.dRuneTypeNameTempCount);
        }

        public void GetViewDataAndShow()
        {
            if (m_wndView == null)
            {
                return;
            }

            m_wndView.SetStoreShow(LogicDataCenter.runePageDataManager.GetInlayTypeAllStore(), LogicDataCenter.runePageDataManager.GetRuneSlotInfo(), LogicDataCenter.runePageDataManager.GetRunePropertyType(), LogicDataCenter.runePageDataManager.GetRuneLevelState());
            m_wndView.SetPropertyShow(ref  LogicDataCenter.runePageDataManager.uCurRunePageData.dRuneTypeNameTempCount);
            m_wndView.SetRuneSlotShow(ref  LogicDataCenter.runePageDataManager.uCurRunePageData.dRuneTempSlotInfo);
            m_wndView.SetNameShow(LogicDataCenter.runePageDataManager.uCurRunePageData.sTempName);
        }
    }

    [Serializable]
    public class URuneTypeTreeViewDataSource
    {
        /// <summary>
        /// The depth.
        /// </summary>
        [SerializeField]
        public int Depth;

        /// <summary>
        /// The is visible.
        /// </summary>
        [SerializeField]
        public bool IsVisible = true;

        /// <summary>
        /// The is expanded.
        /// </summary>
        [SerializeField]
        public bool IsExpanded;

        /// <summary>
        /// The icon.
        /// </summary>
        [SerializeField]
        public Sprite Icon;

        /// <summary>
        /// 符文名字
        /// </summary>
        [SerializeField]
        public string Name;

        /// <summary>
        /// 符文数量
        /// </summary>
        [SerializeField]
        public int Value;

        /// <summary>
        /// The tag.
        /// </summary>
        [SerializeField]
        public object Tag;

        /// <summary>
        /// The tag.
        /// </summary>
        [SerializeField]
        public int RuneId;

        [SerializeField]
        public int RuneType;

        [SerializeField]
        public string RuneDesc;
    }

    public class ListRuneGetWays : MonoBehaviour
    {
        public GameObject DefaultGetWaysItem;

        public void Awake()
        {
            if (DefaultGetWaysItem == null)
            {
                return;
            }

            DefaultGetWaysItem.SetActive(false);
        }

        public ListRuneGetWaysItem InitItemViewInfo(string sShowText)
        {
            if (string.IsNullOrEmpty(sShowText))
            {
                return null;
            }

            if (DefaultGetWaysItem == null)
            {
                return null;
            }

            GameObject getWaysObj = ResNode.InstantiateRes(DefaultGetWaysItem);
            if (!getWaysObj)
                return null;

            ListRuneGetWaysItem item = getWaysObj.GetComponent<ListRuneGetWaysItem>();
            if (item == null)
                return null;

            getWaysObj.SetActive(true);
            getWaysObj.transform.SetParent(this.transform, false);
            item.InitItemViewInfo(sShowText);

            return item;
        }
    }

    public class ListRuneGetWaysItem : MonoBehaviour
    {
        public Text WayText;
        public GameObject SelectPart;
        public GameObject NormalPart;
        public Button WayButton;
        public Toggle WayToggle;
        public void InitItemViewInfo(string sShowText)
        {
            if (WayText == null)
            {
                return;
            }

            WayText.text = sShowText;
        }

        public void OnToggleChange(bool bIsOn)
        {
            if (SelectPart == null || NormalPart == null)
            {
                return;
            }

            SelectPart.SetActive(bIsOn);
            NormalPart.SetActive(!bIsOn);
        }

        public void OnWayButtonChange()
        {
            if (WayToggle == null)
            {
                return;
            }

            WayToggle.isOn = true;
        }
    }

    public class RunePageWndView : UIBaseSubWndView
    {
        public URuneTypeTreeList RuneTypeTree;
        public ComboboxSampleListView RuneComboboxList;
        RunePageWnd runePageWnd;
        public List<GameObject> arrayTopButton = new List<GameObject>(20);  // 20个页数点动态管理
        public Image[] arrayRuneSlot;
        public Text[] arrayRuneProperty;
        public Button bPageAddButton;
        public Button bSaveButtons;
        public Button bRescoverButton;
        public Button bDeleteButton;
        public Button bResetButton;
        public InputField RuneInput;
        public GameObject topRunePageNumPart;

        public string sOldName;
        public int nCurPageIndex;                // 显示的顶部页数index
        public int nMaxSortIndex = 0;
        private int nIntervalDistace = 36;
        public float fLastClkCount = 0;

        private const int nComBoxMax = 7;
        private string sCream = "";
        private string sHalMark = "";
        private string sGlyph = "";
        private string sSigil = "";
        protected List<URuneTypeTreeViewDataSource> Data = new List<URuneTypeTreeViewDataSource>();
        private RunePlayerType m_ShowType;
        public GameObject OtherPlayerHidePart;
        public ListRuneGetWays ListRuneWays;
        //public override void Awake()
        //{
        //    base.Awake();

        //    runePageWnd = (RunePageWnd)m_wnd;
        //    SetAllButtonRightListen();
        //    RuneComboboxList.OnSelect.AddListener(ComboboxSelect);
        //    SetButtonState(false);
        //    RuneTypeTree.DefaultColor = Color.white;
        //    RuneTypeTree.SelectedColor = Color.white;
        //    RuneTypeTree.HighlightedColor = Color.white;
        //    RuneTypeTree.Start();
        //    ListRuneWaysInit();
        //}
        public override bool Init(IUISubWnd wnd)
        {
            base.Init(wnd);

            runePageWnd = (RunePageWnd)m_wnd;
            SetAllButtonRightListen();
            RuneComboboxList.OnSelect.AddListener(ComboboxSelect);
            SetButtonState(false);
            RuneTypeTree.DefaultColor = Color.white;
            RuneTypeTree.SelectedColor = Color.white;
            RuneTypeTree.HighlightedColor = Color.white;
            RuneTypeTree.Start();
            ListRuneWaysInit();

            return true;

        }

        public void SetMultilanguage()
        {
            if (sCream != "")
            {
                return;
            }

            var comboboxSampleListView = RuneComboboxList.GetComponent<ComboboxSampleListView>();

            for (int i = 0; i < nComBoxMax; i++)
            {
                ComboboxSampleListViewData comBoxData = new ComboboxSampleListViewData();
                comBoxData.ComboboxText = ULocalizationService.Instance.Get("UIView", "RuneView", "ComBox" + (i + 1));
                comboboxSampleListView.Add(comBoxData);
            }

            comboboxSampleListView.SelectedIndex = 0;

            sCream = ULocalizationService.Instance.Get("UIView", "RuneView", "Cream");
            sHalMark = ULocalizationService.Instance.Get("UIView", "RuneView", "HalMark");
            sGlyph = ULocalizationService.Instance.Get("UIView", "RuneView", "Glyph");
            sSigil = ULocalizationService.Instance.Get("UIView", "RuneView", "Sigil");

        }

        public void SetRuneTypeTreeViewDataSource()
        {
            var nodes = new ObservableList<TreeNode<URuneTypeTreeItemData>>();
            List2Tree(nodes);
            RuneTypeTree.Nodes = nodes;
        }

        public void ComboboxSelect(int index, ListViewItem component)
        {
            if (index == 0)
            {
                index = (int)RunePropertyType.ENUM_RUNEPROPERTYTYPE_ALL;
            }

            if (runePageWnd == null)
            {
                return;
            }

            runePageWnd.SetRunePropertyType(index);
        }

        public void onLevelToggle1(bool bIsOn)
        {
            if (runePageWnd == null)
            {
                return;
            }

            runePageWnd.SetRuneLevelState(1, bIsOn);
        }

        public void onLevelToggle2(bool bIsOn)
        {
            if (runePageWnd == null)
            {
                return;
            }

            runePageWnd.SetRuneLevelState(2, bIsOn);
        }

        public void onLevelToggle3(bool bIsOn)
        {
            if (runePageWnd == null)
            {
                return;
            }

            runePageWnd.SetRuneLevelState(3, bIsOn);
        }
        /// <summary>
        /// Convert flat list to tree.
        /// </summary>
        /// <param name="nodes">Nodes.</param>
        public virtual void List2Tree(ObservableList<TreeNode<URuneTypeTreeItemData>> nodes)
        {
            TreeNode<URuneTypeTreeItemData> last_node = null;
            for (int i = 0; i < Data.Count; i++)
            {
                var item = Data[i];
                item.IsVisible = true;

                //Debug.Log(item.Depth + " -> " + item.Name + " -> " + item.IsVisible);
                if (item.Depth == 0)
                {
                    last_node = Item2Node(item);
                    nodes.Add(last_node);
                }
                else if (item.Depth == (Data[i - 1].Depth + 1))
                {
                    var current_node = Item2Node(item);
                    last_node.Nodes.Add(current_node);

                    last_node = current_node;
                }
                else if (item.Depth <= Data[i - 1].Depth)
                {
                    int n = item.Depth - Data[i - 1].Depth + 1;
                    for (int j = 0; j < n; j++)
                    {
                        last_node = last_node.Parent;
                    }

                    var current_node = Item2Node(item);
                    last_node.Nodes.Add(current_node);

                    last_node = current_node;
                }
                else
                {
                    //Debug.LogWarning("Unknown case");
                }
            }
        }

        /// <summary>
        /// Convert item to node.
        /// </summary>
        /// <returns>The node.</returns>
        /// <param name="item">Item.</param>
        protected virtual TreeNode<URuneTypeTreeItemData> Item2Node(URuneTypeTreeViewDataSource item)
        {
            var nodeItem = new URuneTypeTreeItemData(item.Name, item.Icon);
            nodeItem.Value = item.Value;
            nodeItem.Tag = item.Tag;
            nodeItem.nRuneId = item.RuneId;
            nodeItem.Icon = item.Icon;
            nodeItem.nRuneType = item.RuneType;
            nodeItem.sRuneDesc = item.RuneDesc;
            return new TreeNode<URuneTypeTreeItemData>(nodeItem, new ObservableList<TreeNode<URuneTypeTreeItemData>>(), item.IsExpanded, item.IsVisible);
        }


        ////////////////////////////////////顶部显示操作部分//////////////////////////////////////

        public void onAddBtnClick()
        {
            if (this.nMaxSortIndex >= 20)
            {
                return;
            }

            if (runePageWnd == null)
            {
                return;
            }

            if (nCurPageIndex != 0)
            {
                runePageWnd.SetSaveSingleRunePage();                   // 存旧页
            }

            this.nMaxSortIndex++;
            nCurPageIndex = nMaxSortIndex;
            this.runePageWnd.SetNewPage(nMaxSortIndex);                  // 发到model储存数据
            CreateRuneNum();
            runePageWnd.SetSaveSingleRunePage();                       // 存新页
            SetSelectTopBtnState(nMaxSortIndex);
        }

        public void CreateAllTopRuneButton(int nTotalPageNum)
        {
            for (int i = 0; i < nTotalPageNum; i++)
            {
                nMaxSortIndex++;
                CreateRuneNum();
            }

            if (this.nMaxSortIndex != 0)
            {
                SetSelectTopBtnState();
            }
        }

        public void CreateRuneNum()
        {
            ResNode uiTopButtonPrefab = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Widgets/BUttons/TopButton");
            if (uiTopButtonPrefab == null)
            {
                return;
            }

            GameObject runeNumTop = uiTopButtonPrefab.InstanceMainRes();

            if (runeNumTop == null)
            {
                return;
            }

            this.arrayTopButton.Add(runeNumTop);
            runeNumTop.name = nMaxSortIndex.ToString();
            runeNumTop.transform.SetParent(this.topRunePageNumPart.transform, false);
            runeNumTop.transform.localPosition = new Vector3((float)((nMaxSortIndex - 1) * nIntervalDistace), bPageAddButton.transform.localPosition.y, 0);
            runeNumTop.transform.FindChild("Text").GetComponent<Text>().text = (nMaxSortIndex).ToString();
            var buttonTop = runeNumTop.GetComponent<UButtonGroupItem>();
            buttonTop.Group = topRunePageNumPart.GetComponent<UButtonGroup>();

            buttonTop.onSelectedChanged.AddListener(delegate(bool bSeletect)
            {
                if (bSeletect)
                {
                    this.onNumListBtnClick(runeNumTop);
                }
            });

            UTooltipTrigger pageTooltipTrigger = runeNumTop.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
            pageTooltipTrigger.backgroundTint = Color.grey;
            pageTooltipTrigger.SetText(UTooltipParamName.BodyText, LogicDataCenter.runePageDataManager.GetRuneOnePageName(nMaxSortIndex));

            runeNumTop.transform.SetAsLastSibling();
            bPageAddButton.transform.localPosition = new Vector3((float)(nMaxSortIndex * nIntervalDistace), bPageAddButton.transform.localPosition.y, 0);//= 
        }

        public void onNumListBtnClick(GameObject gTopButton)          // 切换页的时候弹窗询问。目前暂时直接保存
        {
            int nSortIndex = int.Parse(gTopButton.transform.FindChild("Text").GetComponent<Text>().text);
            ChangeRunePageSortIndex(nSortIndex);
        }

        public void ChangeRunePageSortIndex(int nSortIndex)
        {
            if (runePageWnd == null)
            {
                return;
            }

            runePageWnd.SetSaveSingleRunePage();                // 通知model储存数据
            this.nCurPageIndex = nSortIndex;
            runePageWnd.ChangeRunePageSortIndex(nSortIndex);    // 处理数据逻辑
        }

        public void removeBtn(int nButtonIndex)                     // 显示的删除
        {
            if (nButtonIndex <= 0 || nButtonIndex > arrayTopButton.Count)
            {
                return;
            }
            var temp = arrayTopButton[nButtonIndex - 1].gameObject;
            ResNode.DestroyRes(ref temp);
            arrayTopButton.RemoveAt(nButtonIndex - 1);

            for (int i = 0; i < arrayTopButton.Count; i++)
            {

                arrayTopButton[i].transform.localPosition = new Vector3((float)(i * nIntervalDistace), arrayTopButton[i].transform.localPosition.y, 0);

                arrayTopButton[i].transform.FindChild("Text").GetComponent<Text>().text = (i + 1).ToString();
            }

            if (nCurPageIndex == nMaxSortIndex)
            {
                this.nCurPageIndex--;
            }

            nMaxSortIndex--;
            bPageAddButton.transform.localPosition = new Vector3((float)(nMaxSortIndex * nIntervalDistace), bPageAddButton.transform.localPosition.y, 0);
        }


        ///////////////////////////////////保存恢复等按钮监听///////////////////////////////////////

        public void onSaveBtnClick()
        {
            if (nCurPageIndex == 0)
            {
                return;
            }

            if (runePageWnd == null)
            {
                return;
            }

            this.runePageWnd.SetSaveSingleRunePage();
        }

        public void onRescoverBtnClick()
        {
            // 重置以后名字返回初始
            if (nCurPageIndex == 0)
            {
                return;
            }

            if (runePageWnd == null)
            {
                return;
            }

            runePageWnd.SetRescoverCurPage();
        }

        public void onDeleteBtnClick()
        {
            if (this.nMaxSortIndex <= 1)
            {
                return;
            }

            if (runePageWnd == null)
            {
                return;
            }

            removeBtn(nCurPageIndex);
            runePageWnd.SetDeleteCurPage();
            if (arrayTopButton.Count != 0)
            {
                SetSelectTopBtnState();
            }
        }

        public void onResetBtnClick()
        {
            if (nCurPageIndex == 0)
            {
                return;
            }

            if (runePageWnd == null)
            {
                return;
            }

            runePageWnd.SetResetCurPage();
        }

        public void SetNameShow(string sName)
        {
            sOldName = sName;
            RuneInput.text = sName;

            SetTopPageDescShow();
        }

        public void onTextEndEdit()
        {
            // 本地存一份数据
            if (nCurPageIndex == 0)
            {
                return;
            }

            if (runePageWnd == null)
            {
                return;
            }

            if (sOldName != this.RuneInput.text)
            {
                sOldName = this.RuneInput.text;
                runePageWnd.SetPageName(this.RuneInput.text);

                SetTopPageDescShow();
            }
        }

        public void OnCloseButtonClick()
        {
            if (runePageWnd == null)
            {
                return;
            }

            runePageWnd.SetSaveSingleRunePage();

        }

        ///////////////////////////////////主显示部分//////////////////////////////////////////
        // 切页的时候显示部分，参数数据应该包括:镶嵌点情况，左侧全部符文剩余情况(这个目前有点难做，放着) ，右侧属性加成情况
        public void SetStoreShow(Dictionary<int, List<RuneItemInfo>> dicRuneTypeStore, Dictionary<int, int> dicAllRuneSlotInfo, int nPropertyType, List<int> lLevelState)
        {
            if (!IsSelfShowType())
            {
                return;
            }

            LogicDataCenter.runePageDataManager.UpdateRuneInlayTypeLeftStore();
            bool bHallMarkExpanded = false;
            bool bGlyphExpanded = false;
            bool bSigilExpanded = false;
            bool bCreamExpanded = false;
            foreach (TreeNode<URuneTypeTreeItemData> itemData in RuneTypeTree.Nodes)
            {
                if (itemData.IsExpanded)
                {
                    if (itemData.Item.Name == this.sCream)
                    {
                        bCreamExpanded = true;
                    }
                    else if (itemData.Item.Name == this.sHalMark)
                    {
                        bHallMarkExpanded = true;
                    }
                    else if (itemData.Item.Name == this.sGlyph)
                    {
                        bGlyphExpanded = true;
                    }
                    else if (itemData.Item.Name == this.sSigil)
                    {
                        bSigilExpanded = true;
                    }
                }
            }

            Data.Clear();
            foreach (int nRuneInlayType in dicRuneTypeStore.Keys)
            {
                if (nRuneInlayType == (int)RuneType.ENUM_RUNETYPE_HALLMARK)
                {
                    URuneTypeTreeViewDataSource itemData3 = new URuneTypeTreeViewDataSource();
                    itemData3.Name = this.sHalMark;
                    itemData3.IsExpanded = bHallMarkExpanded;
                    Data.Add(itemData3);
                }
                else if (nRuneInlayType == (int)RuneType.ENUM_RUNETYPE_GLYPH)
                {
                    URuneTypeTreeViewDataSource itemData1 = new URuneTypeTreeViewDataSource();
                    itemData1.Name = this.sGlyph;
                    itemData1.IsExpanded = bGlyphExpanded;
                    Data.Add(itemData1);
                }
                else if (nRuneInlayType == (int)RuneType.ENUM_RUNETYPE_SIGIL)
                {
                    URuneTypeTreeViewDataSource itemData2 = new URuneTypeTreeViewDataSource();
                    itemData2.Name = this.sSigil;
                    itemData2.IsExpanded = bSigilExpanded;
                    Data.Add(itemData2);
                }
                else if (nRuneInlayType == (int)RuneType.ENUM_RUNETYPE_CREAM)
                {
                    URuneTypeTreeViewDataSource itemData0 = new URuneTypeTreeViewDataSource();
                    itemData0.Name = this.sCream;
                    itemData0.IsExpanded = bCreamExpanded;
                    Data.Add(itemData0);
                }

                foreach (RuneItemInfo runeItemInfo in dicRuneTypeStore[nRuneInlayType])
                {
                    SSchemeRuneConfig runeConfig = RunePage.Instance.allRunePageTable[runeItemInfo.nRuneId] as SSchemeRuneConfig;
                    if (runeConfig == null)
                    {
                        continue;
                    }

                    if (nPropertyType != (int)RunePropertyType.ENUM_RUNEPROPERTYTYPE_ALL && nPropertyType != runeConfig.nRunePropertyType)
                    {
                        continue;
                    }

                    if (!lLevelState.Contains(runeConfig.nRuneLevel))
                    {
                        continue;
                    }

                    int nSlotRuneCount = 0;
                    foreach (int nRuneId in dicAllRuneSlotInfo.Values)
                    {
                        if (nRuneId == runeItemInfo.nRuneId)
                            nSlotRuneCount++;
                    }

                    int nRuneCount = runeItemInfo.nRuneNum - nSlotRuneCount;
                    if (nRuneCount <= 0)
                    {
                        continue;
                    }

                    string[] sRuneValueArray = ((string)(runeConfig.szParametersValueArray)).Split(';');
                    UBB.toHtml(ref runeConfig.szRuneName, UBB_FORMAT_TYPE.UGUI);
                    string[] sRuneNameArray = ((string)(runeConfig.szRuneName)).Split(';');
                    if (sRuneNameArray.Length != sRuneValueArray.Length)
                    {
                        Debug.LogWarning("SetStoreShow : runeinfo.csv doubleproperty config is wrong");
                        return;
                    }

                    URuneTypeTreeViewDataSource runeItemData = new URuneTypeTreeViewDataSource();
                    int nPropertyNameType = runeConfig.nPropertyNameType;
                    for (int i = 0; i < sRuneValueArray.Length; i++)
                    {
                        double nRuneProValue = (double.Parse(sRuneValueArray[i]));
                        runeItemData.Name = runeItemData.Name + this.GetPropertyName(nPropertyNameType, nRuneProValue) + " " + sRuneNameArray[i];
                    }

                    runeItemData.Depth = 1;
                    runeItemData.Value = nRuneCount;
                    runeItemData.RuneId = runeItemInfo.nRuneId;
                    runeItemData.RuneType = runeConfig.nRuneInlayType;
                    UBB.toHtml(ref runeConfig.szRuneDESC, UBB_FORMAT_TYPE.UGUI);
                    runeItemData.RuneDesc = runeConfig.szRuneDESC;

                    runeItemData.Icon = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Rune, WndID.WND_ID_PLAYERFRAME, runeConfig.nRuneInlayType, runeConfig.nRuneIconId);
                    Data.Add(runeItemData);
                }
            }

            if (this.IsVisible())
                SetRuneTypeTreeViewDataSource();
        }

        public void SetPropertyShow(ref  Dictionary<string, RuneTypeNameInfo> dicRuneTypeNameTempValue)
        {
            for (int i = 0; i < arrayRuneProperty.Length; i++)
            {
                this.arrayRuneProperty[i].GetComponent<Text>().gameObject.SetActive(false);
            }

            int nPropertyTextIndex = 0;
            foreach (string sPropertyName in dicRuneTypeNameTempValue.Keys)
            {
                if (arrayRuneProperty.Length <= nPropertyTextIndex)
                {
                    continue;
                }

                if (dicRuneTypeNameTempValue[sPropertyName].typeValue == 0)
                {
                    continue;
                }

                var propertyText = this.arrayRuneProperty[nPropertyTextIndex].GetComponent<Text>();
                propertyText.gameObject.SetActive(true);
                propertyText.text = sPropertyName + "\n\n" + this.GetPropertyName(dicRuneTypeNameTempValue[sPropertyName].nType, dicRuneTypeNameTempValue[sPropertyName].typeValue);
                nPropertyTextIndex++;
            }
        }

        public void SetRuneSlotShow(ref Dictionary<int, int> dicAllRuneSlotInfo)
        {
            for (int i = 0; i < arrayRuneSlot.Length; i++)
            {
                int nSlotId = i + 1;
                if (!dicAllRuneSlotInfo.ContainsKey(nSlotId))
                {
                    this.arrayRuneSlot[nSlotId - 1].transform.FindChild("Sprite").gameObject.SetActive(false);
                    continue;
                }

                int nRuneId = dicAllRuneSlotInfo[nSlotId];//id->iconid
                if (nRuneId == 0)
                {
                    this.arrayRuneSlot[nSlotId - 1].transform.FindChild("Sprite").gameObject.SetActive(false);
                    continue;
                }

                SSchemeRuneConfig runeConfig = RunePage.Instance.allRunePageTable[nRuneId] as SSchemeRuneConfig;
                if (runeConfig == null)
                {
                    continue;
                }

                this.arrayRuneSlot[nSlotId - 1].transform.FindChild("Sprite").gameObject.GetComponent<Image>().sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Rune, WndID.WND_ID_PLAYERFRAME, runeConfig.nRuneInlayType, runeConfig.nRuneIconId);
                this.arrayRuneSlot[nSlotId - 1].transform.FindChild("Sprite").gameObject.SetActive(true);

                UTooltipTrigger slotTooltipTrigger = this.arrayRuneSlot[nSlotId - 1].gameObject.GetComponent<UTooltipTrigger>();
                if (slotTooltipTrigger == null)
                {
                    slotTooltipTrigger = this.arrayRuneSlot[nSlotId - 1].gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
                    slotTooltipTrigger.backgroundTint = Color.grey;
                    slotTooltipTrigger.IsUpStay = false;
                }

                if (runeConfig == null)
                {
                    slotTooltipTrigger.SetText(UTooltipParamName.BodyText, "EmptyDesc");
                    return;
                }

                if (runeConfig.szParametersValueArray == "")
                {
                    slotTooltipTrigger.SetText(UTooltipParamName.BodyText, "EmptyDesc");
                    return;
                }

                UBB.toHtml(ref runeConfig.szRuneDESC, UBB_FORMAT_TYPE.UGUI);
                slotTooltipTrigger.SetText(UTooltipParamName.BodyText, runeConfig.szRuneDESC);
            }
        }


        ////////////////////////////////////符文右击事件/////////////////////////////////////
        public void SetAllButtonRightListen()
        {
            for (int i = 0; i < this.arrayRuneSlot.Length; i++)
            {
                GameObject gameObjectRune = this.arrayRuneSlot[i].gameObject;
                var trigger = gameObjectRune.AddComponent<EventTrigger>();
                trigger.triggers = new List<EventTrigger.Entry>();
                EventTrigger.Entry entry = new EventTrigger.Entry();

                // 设置 事件类型
                entry.eventID = EventTriggerType.PointerDown;
                // 设置回调函数
                entry.callback = new EventTrigger.TriggerEvent();

                UnityAction<BaseEventData> callback = new UnityAction<BaseEventData>(this.onRuneBtnTriggerClick);
                entry.callback.AddListener(callback);
                // 添加事件触发记录到GameObject的事件触发组件
                trigger.triggers.Add(entry);
            }
        }

        public void onRuneBtnTriggerClick(BaseEventData eventData2)
        {
            PointerEventData eventData = (PointerEventData)eventData2;
            if (eventData.pointerEnter == null)
            {
                return;
            }

            var slotInfo = eventData.pointerEnter.gameObject.GetComponent<URuneSlotItemComponent>();
            if (slotInfo == null)
            {
                return;
            }

            int nSlotId = slotInfo.nRuneSlotId;
            if (eventData.button == PointerEventData.InputButton.Left)
            {
                float nCurClkCount = eventData.clickTime;
                if (nCurClkCount - this.fLastClkCount < 1)
                {
                    onRuneDoubleLeftBtnClick(nSlotId);
                    this.fLastClkCount = 0;
                }
                else
                {
                    this.fLastClkCount = nCurClkCount;
                }
            }
            else if (eventData.button == PointerEventData.InputButton.Right)
            {
                onRuneRightBtnClick(nSlotId);
            }
        }

        public void onRuneDoubleLeftBtnClick(int nSlotId)
        {
            onRuneRightBtnClick(nSlotId);
        }

        public void onRuneRightBtnClick(int nSlotId)
        {
            if (!IsSelfShowType())
            {
                return;
            }

            if (nCurPageIndex == 0)
            {
                return;
            }

            if (runePageWnd == null)
            {
                return;
            }

            int nRuneId = 0;
            runePageWnd.SetRunePagePointsInfo(nSlotId, nRuneId, false);
            runePageWnd.OnPutRuneItem();
            this.arrayRuneSlot[nSlotId - 1].transform.FindChild("Sprite").gameObject.SetActive(false);
            this.arrayRuneSlot[nSlotId - 1].transform.FindChild("High").gameObject.SetActive(false);
            var trriger = this.arrayRuneSlot[nSlotId - 1].gameObject.GetComponent<UTooltipTrigger>();
            if (trriger == null)
            {
                return;
            }

            //trriger.StopHover();
            //trriger.enabled = false;
        }

        public void SetButtonState(bool bChange)
        {
            if (!IsSelfShowType())
            {
                return;
            }

            if (bChange)
            {

                this.bSaveButtons.gameObject.GetComponent<Image>().ResetToDefaultMaterial();
                this.bRescoverButton.gameObject.GetComponent<Image>().ResetToDefaultMaterial();
            }
            else
            {
                this.bRescoverButton.gameObject.GetComponent<Image>().AddNewMaterial(EUIShaderType.EST_Gray);
                this.bSaveButtons.gameObject.GetComponent<Image>().AddNewMaterial(EUIShaderType.EST_Gray);

            }

            this.bSaveButtons.enabled = bChange;
            this.bRescoverButton.enabled = bChange;
        }

        public void SetPutRuneInfo(PointerEventData eventData)
        {
            var treeItem = eventData.pointerDrag;
            if (treeItem == null)
            {
                return;
            }

            URuneSlotItemComponent slotInfo;
            for (int i = 0; i < this.arrayRuneSlot.Length; i++)
            {
                GameObject slotObject = this.arrayRuneSlot[i].gameObject;
                slotInfo = slotObject.GetComponent<URuneSlotItemComponent>();
                var dragData = eventData.pointerDrag.GetComponent<URuneTypeTreeItemComponent>();
                if (slotInfo.nRuneSlotType != dragData.Item.nRuneType)
                {
                    continue;
                }
                bool bActive = this.arrayRuneSlot[i].gameObject.transform.FindChild("Sprite").gameObject.activeSelf;
                if (bActive)
                {
                    continue;
                }

                var dropItem = slotObject.GetComponent<URuneItemDataDropSupport>();
                dropItem.OnDrop(eventData);
                break;
            }
        }

        public string GetPropertyName(int nType, double nValue)
        {
            string sOperater = "";
            string sPersent = "";
            nValue /= 100;

            sOperater = LogicDataCenter.runePageDataManager.IsAddOperater(nType) ? "+" : "-";
            nValue = LogicDataCenter.runePageDataManager.IsRuneGrow(nType) ? nValue * 18 : nValue;

            if (LogicDataCenter.runePageDataManager.IsPersentOperater(nType))
            {
                nValue /= 100;
                sPersent = "%";
            }

            if (LogicDataCenter.runePageDataManager.IsEnlargeHundredOperater(nType))
            {
                nValue /= 100;
            }

            return sOperater + nValue + sPersent;
        }

        public void SetSelectTopBtnState(int nPageIndex = -1)
        {
            if (nPageIndex == -1)
            {
                nPageIndex = this.nCurPageIndex;
            }

            if (nPageIndex <= 0 || nPageIndex > this.arrayTopButton.Count)
            {
                return;
            }

            this.arrayTopButton[nPageIndex - 1].GetComponent<UButtonGroupItem>().Selected = true;
        }

        public void SetTopPageDescShow(int nPageIndex = -1)
        {
            if (nPageIndex == -1)
            {
                nPageIndex = this.nCurPageIndex;
            }

            if (nPageIndex <= 0 || nPageIndex > this.arrayTopButton.Count)
            {
                return;
            }

            UTooltipTrigger pageTooltipTrigger = this.arrayTopButton[nPageIndex - 1].GetComponent<UTooltipTrigger>();
            if (pageTooltipTrigger == null)
            {
                pageTooltipTrigger = this.arrayTopButton[nPageIndex - 1].AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
                pageTooltipTrigger.backgroundTint = Color.grey;
            }

            pageTooltipTrigger.SetText(UTooltipParamName.BodyText, sOldName);
        }

        public void ClearRuneView()
        {
            SetOtherPlayerHidePartVisible(true);
            DestroyPageNum();
            ResetRuneData();
        }

        private void ResetRuneData()
        {
            sOldName = "";
            nCurPageIndex = 0;
            nMaxSortIndex = 0;
            nIntervalDistace = 36;
            fLastClkCount = 0;
            arrayTopButton.Clear();
            RuneInput.interactable = true;
        }

        private void DestroyPageNum()
        {
			int nChildCount = this.topRunePageNumPart.transform.childCount;

			for (int j = nChildCount - 1; j >= 0; --j)
            {
				// 不删除第一页
                if (j == 0)
                {
                    continue;
                }

                var itemChild = this.topRunePageNumPart.transform.GetChild(j).gameObject;
                ResNode.DestroyRes(ref itemChild);
            }
        }

        public bool IsSelfShowType()
        {
            if (m_ShowType == RunePlayerType.RUNEPLAYERTYPE_SELF)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public RunePlayerType PlayerTypeInRune
        {
            set { m_ShowType = value; }
            get { return m_ShowType; }
        }

        public void SetOtherPlayerHidePartVisible(bool bShow)
        {
            OtherPlayerHidePart.SetActive(bShow);
            this.bPageAddButton.gameObject.SetActive(bShow);
            RuneInput.interactable = bShow;
        }

        public void OnRuneGotoShopViewClick()
        {
            SendSimulateOpenMessege(ESimulateOpenViewType.ESOP_SHOP);
        }

        public void OnRuneGotoKinViewClick()
        {
            SendSimulateOpenMessege(ESimulateOpenViewType.ESOP_PLAYER_HERO);
        }

        public void OnRuneGotoMatchingViewClick()
        {
            SendSimulateOpenMessege(ESimulateOpenViewType.ESOP_WAR_MAIN);
        }

        private void SendSimulateOpenMessege(ESimulateOpenViewType eViewType)
        {
            PlayerButtonFrameSimulateOpenData uiData = new PlayerButtonFrameSimulateOpenData();
            uiData.eViewType = eViewType;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_SIMULATEVIEWOPEN, uiData);
        }

        public void ListRuneWaysInit()
        {
            if (ListRuneWays == null)
            {
                return;
            }

            ListRuneGetWaysItem item1 = ListRuneWays.InitItemViewInfo("请前往商城购买");
            ListRuneGetWaysItem item2 = ListRuneWays.InitItemViewInfo("解锁英雄等级获得");
            ListRuneGetWaysItem item3 = ListRuneWays.InitItemViewInfo("每日首胜获得");

            if (item1 == null || item2 == null || item3 == null)
            {
                return;
            }

            if (item1.WayButton == null || item2.WayButton == null || item3.WayButton == null)
            {
                return;
            }

            item1.WayButton.onClick.AddListener(OnRuneGotoShopViewClick);
            item2.WayButton.onClick.AddListener(OnRuneGotoKinViewClick);
            item3.WayButton.onClick.AddListener(OnRuneGotoMatchingViewClick);

            if (item1.WayToggle)
                item1.WayToggle.isOn = true;
        }
    }
}
