using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using Kin_ManagerDef;
using DataCenter;

namespace USpeedUI.KinAcceptMember
{
    public class KinAcceptMemberWnd : UIBaseWnd<KinAcceptMemberWndView>
    {
        private KinAcceptMemberWndView view { get { return m_wndView as KinAcceptMemberWndView; } }

        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_KIN_ACCEPTMEMBER;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewKinAcceptMember";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_OVERHEAD;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;

            // 预先载入
            //LoadView();
            SetVisible(false);

            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_RECEV_APPLY_LIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_AUTO_ACCEPT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_AUTO_ACCEPT_REQUEST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_UPDATE_APPLY_LIST, this);


            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_RECEV_APPLY_LIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_AUTO_ACCEPT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_AUTO_ACCEPT_REQUEST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_UPDATE_APPLY_LIST, this);
            
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData data = msgData as UIMsgCmdData;
            switch (msgID)
            {
                case WndMsgID.WND_MSG_KIN_RECEV_APPLY_LIST:
                    {
                        if (view != null)
                            view.OnRecevKinApplyList();
                    }
                    break;
                case WndMsgID.WND_MSG_KIN_UPDATE_APPLY_LIST:
                    {
                        if (m_wndView != null)
                            m_wndView.OnUpdateKinApplyList();
                    }
                    break;
                case WndMsgID.WND_MSG_KIN_AUTO_ACCEPT:
                    {
                        if (view != null)
                            view.OnRecvAutoAccept();
                    }
                    break;

                default:
                    break;
            }
        }

    }

    public class KinAcceptMemberWndView : UIBaseWndView
    {
        public UListKinAcceptMember ListKinAcceptMember;
        public Transform AutoAcceptTog;
        public Transform MaleTog;
        public Transform FemleTog;
        public Transform LevelTI;
        public Transform AcceptEditBtn;
        public Transform AcceptSaveBtn;
        public Transform AutoAcceptBK;

        public Text AdmitBtnDes;
        public Text RefuseBtnDes;
        public Text SaveAutoBtnDes;
        public Text EditAutoBtnDes;
        public Text SysDes;
        public Text MaleTogDes;
        public Text FemleTogDes;
        public Text AutoAcceptTogDes;
        public Text SexDes;
        public Text LevelDes;
        public Text AutoDes;
        public Text LevelConditionDes;
        public Text HeadName;
        public Text HeadPower;
        public Text HeadLevel;


        private UListItemKinAcceptMember CurSelectItem;
        private int nCurSelectIndex;

        public override void Start()
        {
        }

        public override bool Init(IUIWnd wnd)
        {

            AdmitBtnDes.text = ULocalizationService.Instance.Get("UIView", "KinAccept", "AdmitBtnDes");
            RefuseBtnDes.text = ULocalizationService.Instance.Get("UIView", "KinAccept", "RefuseBtnDes");
            SaveAutoBtnDes.text = ULocalizationService.Instance.Get("UIView", "KinAccept", "SaveAutoBtnDes");
            EditAutoBtnDes.text = ULocalizationService.Instance.Get("UIView", "KinAccept", "EditAutoBtnDes");
            SysDes.text = ULocalizationService.Instance.Get("UIView", "KinAccept", "SysDes");
            MaleTogDes.text = ULocalizationService.Instance.Get("UIView", "KinAccept", "MaleTogDes");
            FemleTogDes.text = ULocalizationService.Instance.Get("UIView", "KinAccept", "FemleTogDes");
            AutoAcceptTogDes.text = ULocalizationService.Instance.Get("UIView", "KinAccept", "AutoAcceptTogDes");
            SexDes.text = ULocalizationService.Instance.Get("UIView", "KinAccept", "SexDes");
            LevelDes.text = ULocalizationService.Instance.Get("UIView", "KinAccept", "LevelDes");
            AutoDes.text = ULocalizationService.Instance.Get("UIView", "KinAccept", "AutoDes");
            LevelConditionDes.text = ULocalizationService.Instance.Get("UIView", "KinAccept", "LevelConditionDes");
            HeadName.text = ULocalizationService.Instance.Get("UIView", "KinAccept", "HeadName");
            HeadPower.text = ULocalizationService.Instance.Get("UIView", "KinAccept", "HeadPower");
            HeadLevel.text = ULocalizationService.Instance.Get("UIView", "KinAccept", "HeadLevel");

            

            return base.Init(wnd);
        }

        public void OnRecevKinApplyList()
        {
            ListKinAcceptMember.DataSource.Clear();
            ListKinAcceptMember.OnSelect.RemoveListener(onSelectKin);

            UIWidgets.ObservableList<UListItemKinAcceptMember> dataSource = new UIWidgets.ObservableList<UListItemKinAcceptMember>();
            GameLogic.cmd_kin_applylist_head cmdHead = LogicDataCenter.kinDataManager.ApplyListHead;
            List<cmd_kin_applylist_node> ApplyListNode = LogicDataCenter.kinDataManager.ApplyListNode;
            ListKinAcceptMember.DataSource.Clear();
            for (int i = 0; i < cmdHead.nCount; ++i)
            {
                GameLogic.cmd_kin_applylist_node cmdNodeData = ApplyListNode[i];
                UListItemKinAcceptMember item = new UListItemKinAcceptMember();
                item.PlayerID = cmdNodeData.nPlayerPDBID;
                item.PersonName = cmdNodeData.szName;
                item.Level += cmdNodeData.nLevel;
                item.Power += cmdNodeData.nPower;
                dataSource.Add(item);
            }

            ListKinAcceptMember.DataSource = dataSource;
            ListKinAcceptMember.OnSelect.AddListener(onSelectKin);
        }

        public void OnUpdateKinApplyList()
        {
            ListKinAcceptMember.DataSource.BeginUpdate();
            ListKinAcceptMember.DataSource.Clear();
            List<cmd_kin_applylist_node> KinApplyList = LogicDataCenter.kinDataManager.ApplyListNode;
            for (int i = 0; i < KinApplyList.Count; ++i)
            {
                cmd_kin_applylist_node cmdNodeData = KinApplyList[i];
                UListItemKinAcceptMember item = new UListItemKinAcceptMember();
                item.PlayerID = cmdNodeData.nPlayerPDBID;
                item.PersonName = cmdNodeData.szName;
                item.Level += cmdNodeData.nLevel;
                item.Power += cmdNodeData.nPower;
                ListKinAcceptMember.DataSource.Add(item);
            }

            ListKinAcceptMember.DataSource.EndUpdate();
        }


        public void onSelectKin(int index, UIWidgets.ListViewItem item)
        {
            // 选择当前数据
            nCurSelectIndex = index;
            CurSelectItem = ListKinAcceptMember.DataSource[index];
        }

        public void OnAcceptClick()
        {
            if (CurSelectItem == null)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_KIN_SELECT_ACCEPT_MEMBER);
                return;
            }
            GameLogic.cmd_kin_admit cmdData = new GameLogic.cmd_kin_admit();
            cmdData.bAgree = 1;
            cmdData.nApplyPDBID = CurSelectItem.PlayerID;
            ViewEventHelper.Instance.SendCommand<cmd_kin_admit>(GameLogicDef.GVIEWCMD_KIN_ADMIT, ref cmdData);
        }

        public void OnRefuseClick()
        {
            if (CurSelectItem == null)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_KIN_SELECT_ACCEPT_MEMBER);
                return;
            }
            GameLogic.cmd_kin_admit cmdData = new GameLogic.cmd_kin_admit();
            cmdData.bAgree = 0;
            cmdData.nApplyPDBID = CurSelectItem.PlayerID;
            ViewEventHelper.Instance.SendCommand<cmd_kin_admit>(GameLogicDef.GVIEWCMD_KIN_ADMIT, ref cmdData);
        }

        public void OnCloseBtnClick()
        {
            // 关闭窗体显示
            UISystem.Instance.ShowWnd(WndID.WND_ID_KIN_ACCEPTMEMBER, false);
        }

        public void OnAcceptEditClick()
        {

            Toggle tAutoAccept = AutoAcceptTog.GetComponent<Toggle>();
            Toggle tFemleTog = MaleTog.GetComponent<Toggle>();
            Toggle tMaleTog = FemleTog.GetComponent<Toggle>();
            InputField tLevelTI = LevelTI.GetComponent<InputField>();

            tAutoAccept.enabled = true;
            tLevelTI.enabled = true;
            tFemleTog.enabled = true;
            tMaleTog.enabled = true;

            // 显示保存按钮
            AcceptSaveBtn.GetComponent<Button>().gameObject.SetActive(true);
            AcceptEditBtn.GetComponent<Button>().gameObject.SetActive(false);

            setAcceptEditState(false);
        }

        public void OnAcceptSaveClick()
        {
            GameLogic.cmd_kin_auto_accept cmdHead = new GameLogic.cmd_kin_auto_accept();
            // 更新到显示
            Toggle tAutoAccept = AutoAcceptTog.GetComponent<Toggle>();
            Toggle tMaleTog = MaleTog.GetComponent<Toggle>();
            Toggle tFemleTog = FemleTog.GetComponent<Toggle>();
            InputField tLevelTI = LevelTI.GetComponent<InputField>();

            if (tLevelTI.textComponent.text == string.Empty)
                cmdHead.nAcceptLevel = int.Parse(tLevelTI.placeholder.GetComponent<Text>().text);
            else
                cmdHead.nAcceptLevel = int.Parse(tLevelTI.textComponent.text);

            cmdHead.dwAcceptFlag |= (int)((tMaleTog.isOn == true) ? TKinAutoFlag.emKinAuto_Male : TKinAutoFlag.emKinAuto_None);
            cmdHead.dwAcceptFlag |= (int)((tFemleTog.isOn == true) ? TKinAutoFlag.emKinAuto_Female : TKinAutoFlag.emKinAuto_None);
            cmdHead.bAutoAccept = (int)((tAutoAccept.isOn == true) ? 1 : 0);
            ViewEventHelper.Instance.SendCommand<cmd_kin_auto_accept>(GameLogicDef.GVIEWCMD_KIN_AUTOACCEPT_SAVE, ref cmdHead);


            //设置可以编辑
            tAutoAccept.enabled = false;
            tLevelTI.enabled = false;
            tFemleTog.enabled = false;
            tMaleTog.enabled = false;

            // 显示编辑按钮
            AcceptSaveBtn.GetComponent<Button>().gameObject.SetActive(false);
            AcceptEditBtn.GetComponent<Button>().gameObject.SetActive(true);

            setAcceptEditState(true);
        }

        public void setAcceptEditState(bool bIsEdits)
        {
            Color tColor = Color.black;
           Graphic[] GraphicArrays = AutoAcceptBK.GetComponentsInChildren<Graphic>();
           foreach (var graphic in GraphicArrays)
           {
               tColor = graphic.color;
               if (bIsEdits)
               {
                   tColor.a = 0.5f;
               }
               else
               {
                   tColor.a = 1f;
               }
               graphic.color = tColor;
           }
        }

        public void OnRecvAutoAccept()
        {

            GameLogic.cmd_kin_auto_accept cmdHead = LogicDataCenter.kinDataManager.AutoAccept;
            // 更新到显示
            Toggle tAutoAccept = AutoAcceptTog.GetComponent<Toggle>();
            Toggle tMaleTog = MaleTog.GetComponent<Toggle>();
            Toggle tFemleTog = FemleTog.GetComponent<Toggle>();
            InputField tLevelTI = LevelTI.GetComponent<InputField>();

            //设置可以编辑
            tAutoAccept.enabled = true;
            tLevelTI.enabled = true;
            tFemleTog.enabled = true;
            tMaleTog.enabled = true;


            tLevelTI.placeholder.GetComponent<Text>().text = cmdHead.nAcceptLevel.ToString();
            tAutoAccept.isOn = cmdHead.bAutoAccept == 1 ? true : false;
            tMaleTog.isOn = (((TKinAutoFlag)cmdHead.dwAcceptFlag & TKinAutoFlag.emKinAuto_Male) == TKinAutoFlag.emKinAuto_Male) ? true : false;
            tFemleTog.isOn = (((TKinAutoFlag)cmdHead.dwAcceptFlag & TKinAutoFlag.emKinAuto_Female) == TKinAutoFlag.emKinAuto_Female) ? true : false;

            //设置可以编辑
            tAutoAccept.enabled = false;
            tLevelTI.enabled = false;
            tFemleTog.enabled = false;
            tMaleTog.enabled = false;

            // 显示编辑按钮
            AcceptSaveBtn.GetComponent<Button>().gameObject.SetActive(false);
            AcceptEditBtn.GetComponent<Button>().gameObject.SetActive(true);

            setAcceptEditState(true);
        }

    }
}
