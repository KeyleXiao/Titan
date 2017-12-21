using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UWidgets;

namespace USpeedUI.WarGoodsStore
{
    #region 子类型
    public class UTopBarTypeBase : UCustomUIBehaviour
    {
        public virtual void SetVisible(bool _bNewValue)
        {
            this.gameObject.SetActive(_bNewValue);
        }
    }
    public class UTopBarType_Recommend : UTopBarTypeBase
    {
        public Text DispositionName;
        public Text DispositionCreator;
        public Text DispositionTitle;
        public Toggle ToggleListView = null;

        protected override void Start()
        {
            base.Start();

            ToggleListView.isOn = false;
            ToggleListView.onValueChanged.AddListener(OnToggleListView);

        }

        public void SetSelectedGoodsDispositionInfo(UWGS_SelectedGoodsDispositionMsgData _msgData)
        {
            SetText(DispositionName, _msgData.SelectedDisposition.DispositionName);
            SetText(DispositionCreator, _msgData.SelectedDisposition.DispositionName);
            SetText(DispositionName, _msgData.SelectedDisposition.DispositionName);
            ToggleListView.isOn = false;
        }


        protected void OnToggleListView(bool _bIsOn)
        {
            int result = _bIsOn ? 1 : 0;
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_GOODSSTORE, WndMsgID.WND_MSG_WAR_GOODSSTORE_GET_TOGGLEDISPOSITIONSTATE,
                new UIMsgCmdData((int)WndMsgID.WND_MSG_WAR_GOODSSTORE_GET_TOGGLEDISPOSITIONSTATE, result, string.Empty, IntPtr.Zero, 0));
        }

        public void SetToggleState(bool _bOpen)
        {
            ToggleListView.isOn = _bOpen;
        }

        protected void SetText(Text _txt,string _strTxt)
        {
            if (_txt != null)
            {
                _txt.text = _strTxt;
            }
        }
    }
    public class UTopBarType_Search : UTopBarTypeBase
    {
        public InputField SearchInputField;
        public Button BtnSearchClear;

        public char[] DelimiterChars = new char[] { ' ' };
        public EWarGoodsStoreSearchFilter SearchFilter = EWarGoodsStoreSearchFilter.ESF_Contains;

        protected override void Start()
        {
            base.Start();
            InitWidget();
        }

        protected void InitWidget()
        {
            {
                Transform SearchInputTrans = null;
                Transform SearchClearTrans = null;
                if (SearchInputField == null)
                {
                    SearchInputTrans = GetRectTransform.FindChild("Frame/SearchInput");
                    if (SearchInputTrans != null)
                    {
                        SearchInputField = SearchInputTrans.GetComponent<InputField>();
                    }
                    else
                        Trace.LogError("没找到对象:SearchInput ");
                }
                if (SearchInputField == null)
                    Trace.LogError("对象:SearchInput 没找到InputField组件");
                else
                {
                    SearchInputField.onValueChanged.AddListener(OnSearchValueChange);
                    SearchInputField.onEndEdit.AddListener(OnSearchValueEndEdit);
                    SearchClearTrans = SearchInputField.transform.FindChild("BtnClose");
                }
                if (SearchClearTrans == null)
                    Trace.LogError("没找到对象:SearchInput.BtnClose");
                else
                {
                    BtnSearchClear = SearchClearTrans.GetComponent<Button>();
                    if (BtnSearchClear != null)
                    {
                        BtnSearchClear.onClick.AddListener(() =>
                        {
                            if (SearchInputField != null)
                                SearchInputField.text = string.Empty;
                        });
                    }
                }

            }
        }


        /// <summary>
        /// 查找文本改变事件
        /// </summary>
        /// <param name="_strText"></param>
        public void OnSearchValueChange(string _strText)
        {
            //移除空格
            int end_position = SearchInputField.caretPosition;

            var text = SearchInputField.text.Substring(0, end_position);
            var start_position = text.LastIndexOfAny(DelimiterChars) + 1;

            _strText = text.Substring(start_position).Trim();


            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_GOODSSTORE, WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_SEARCHVALUECHANGED, new UIMsgCmdData((int)WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_SEARCHVALUECHANGED, (int)SearchFilter, _strText, IntPtr.Zero, 0));
        }
        public void OnSearchValueEndEdit(string _strText)
        {
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_WAR_GOODSSTORE, WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_SEARCHVALUECHANGEEND, new UIMsgCmdData((int)WndMsgID.WND_MSG_WAR_GOODSSTORE_SET_SEARCHVALUECHANGED, 0, _strText, IntPtr.Zero, 0));
        }
    }
    #endregion
    public class UWarTopBarGoodsView : UWarGoodsStoreSubViewBase
    {

        public UTopBarType_Recommend Recommend;
        public UTopBarType_Search Search;

        public override bool Init(UWarGoodsStoreView _viewParent)
        {
            return base.Init(_viewParent);
        }
        public override EGoodsStoreSubView GetSubViewType
        {
            get
            {
                return EGoodsStoreSubView.EGSSV_TopBarGoods;
            }
        }

        public void TopBarTypeChange(EGoodsStoreSubView _eNewType)
        {
            switch (_eNewType)
            {
                case EGoodsStoreSubView.EGSSV_AllGoods:
                    {
                        Recommend.SetVisible(false);
                        Search.SetVisible(true);
                    }
                    break;
                case EGoodsStoreSubView.EGSSV_RecommendGoods:
                    {
                        //现在不用显示这个了，功能先不移除
                        Recommend.SetVisible(false);
                        Search.SetVisible(false);
                    }
                    break;
            }
        }

        public void SetSelectedGoodsDispositionInfo(UWGS_SelectedGoodsDispositionMsgData _msgData)
        {
            if(Recommend != null)
            {
                Recommend.SetSelectedGoodsDispositionInfo(_msgData);
            }
        }

        public void SetToggleState(bool _bOpen)
        {
            if(Recommend != null)
            {
                Recommend.SetToggleState(_bOpen);
            }
        }
    }
}
