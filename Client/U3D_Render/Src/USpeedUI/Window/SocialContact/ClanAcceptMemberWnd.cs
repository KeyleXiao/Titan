using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using DataCenter;

namespace USpeedUI.ClanAcceptMember
{
    public class ClanAcceptMemberWnd : UIBaseWnd<ClanAcceptMemberWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_CLAN_ACCEPTMEMBER;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewClanAcceptMember";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FILL;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;

            SetVisible(false);

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_APPLY_LIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_ADDAPPLY, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_REMOVEAPPLY, this);
            


            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_APPLY_LIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_ADDAPPLY, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_REMOVEAPPLY, this);
            
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);
            //请求数据
            if (_bVisible)
                ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_CLAN_APPLY_LIST);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData data = msgData as UIMsgCmdData;
            switch (msgID)
            {
                case WndMsgID.WND_MSG_CLAN_APPLY_LIST:
                    {
                        if (m_wndView != null)
                            m_wndView.OnRecvClanApplyList();
                    }
                    break;
                case WndMsgID.WND_MSG_CLAN_ADDAPPLY:
                    {
                        if (m_wndView != null)
                            m_wndView.OnClanAddApply(data.ptrParam, data.nPtrLen);
                    }
                    break;
                case WndMsgID.WND_MSG_CLAN_REMOVEAPPLY:
                    {
                        if (m_wndView != null)
                            m_wndView.OnClanRemoveApply(data.ptrParam, data.nPtrLen);
                    }
                    break;

                    
                default:
                    break;
            }
        }
    }

    public class ClanAcceptMemberWndView : UIBaseWndView
    {
        public Text titleText;
        public Text listKinNameDesc;
        public Text listKinLeaderDesc;
        public Text listKinMemberCountDesc;
        public Text acceptMemberBtnDesc;
        public Text refuseMemberBtnDesc;
        public Toggle autoJoinSet;
        public UListClanAcceptMember listClanAcceptMember;

        public override bool Init(IUIWnd wnd)
        {
            titleText.text = ULocalizationService.Instance.Get("UIView", "ClanAcceptMember", "AcceptMemberTitle");
            listKinNameDesc.text = ULocalizationService.Instance.Get("UIView", "ClanAcceptMember", "ListKinName");
            listKinLeaderDesc.text = ULocalizationService.Instance.Get("UIView", "ClanAcceptMember", "ListKinLeader");
            listKinMemberCountDesc.text = ULocalizationService.Instance.Get("UIView", "ClanAcceptMember", "ListKinMemberCount");
            acceptMemberBtnDesc.text = ULocalizationService.Instance.Get("UIView", "ClanAcceptMember", "AcceptBtn");
            refuseMemberBtnDesc.text = ULocalizationService.Instance.Get("UIView", "ClanAcceptMember", "RefuseBtn");
            autoJoinSet.GetComponentInChildren<Text>().text = ULocalizationService.Instance.Get("UIView", "ClanAcceptMember", "AutoAdmitMember");

            return base.Init(wnd);
        }

        internal override void SetRendererVisible(bool visible)
        {
            base.SetRendererVisible(visible);

            // 刚打开时清一次数据等下发新的
            if (visible)
                listClanAcceptMember.DataSource.Clear();
        }

        public void OnRecvClanApplyList()
        {
            listClanAcceptMember.DataSource.BeginUpdate();

            listClanAcceptMember.DataSource.Clear();
            int nCount = LogicDataCenter.clanDataManager.ClanApplyMemberData.Count;
            for (int i = 0; i < nCount; ++i)
            {
                UListItemClanAcceptMember item = new UListItemClanAcceptMember();
                item.data = LogicDataCenter.clanDataManager.ClanApplyMemberData[i];
                listClanAcceptMember.DataSource.Add(item);
            }

            listClanAcceptMember.DataSource.EndUpdate();

            if (nCount > 0)
            {
                listClanAcceptMember.Select(0);
            }


            autoJoinSet.enabled = false;

            bool bIsOn = (LogicDataCenter.clanDataManager.ClanBaseData.nAutoAcceptFlag == 1 ? true : false);
            autoJoinSet.isOn = bIsOn;

            if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property == null)
                return;

            int selfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            if (selfID == LogicDataCenter.clanDataManager.ClanBaseData.nShaikhID)
            {
                autoJoinSet.enabled = true;
            }
        }

        public void OnClanRemoveApply(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            GameLogic.cmd_clan_remove_apply cmdHead = helper.get<GameLogic.cmd_clan_remove_apply>();
            
            // 遍历 找到则删除
            foreach (var item in listClanAcceptMember.DataSource)
            {
                if (cmdHead.nKinID == item.data.nKinID)
                {
                    listClanAcceptMember.DataSource.Remove(item);
                    break;
                }
            }
        }

        public void OnClanAddApply(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            cmd_clan_add_apply cmdHead = helper.get<cmd_clan_add_apply>();

            UListItemClanAcceptMember item = new UListItemClanAcceptMember();
            item.data.nKinID = cmdHead.nKinID;
            item.data.szName = cmdHead.szName;
            item.data.szShaikhName = cmdHead.szShaikhName;
            item.data.nMemberCount = cmdHead.nMemberCount;
            listClanAcceptMember.DataSource.Add(item);
        }

        public void OnClanAcceptClick()
        {
            if (listClanAcceptMember.SelectedItem == null)
            {
                return;
            }

            GameLogic.cmd_clan_admit cmdData = new GameLogic.cmd_clan_admit();
            cmdData.bAgree = 1;
            cmdData.nKinID = listClanAcceptMember.SelectedItem.data.nKinID;
            ViewEventHelper.Instance.SendCommand<cmd_clan_admit>(GameLogicDef.GVIEWCMD_CLAN_ADMIT, ref cmdData);

            // 置空当前选择
            listClanAcceptMember.Select(-1);
        }

        public void OnClanRefuseClick()
        {
            if (listClanAcceptMember.SelectedItem == null)
            {
                return;
            }

            GameLogic.cmd_clan_admit cmdData = new GameLogic.cmd_clan_admit();
            cmdData.bAgree = 0;
            cmdData.nKinID = listClanAcceptMember.SelectedItem.data.nKinID;
            ViewEventHelper.Instance.SendCommand<cmd_clan_admit>(GameLogicDef.GVIEWCMD_CLAN_ADMIT, ref cmdData);

            // 置空当前选择
            listClanAcceptMember.Select(-1);
        }

        public void OnAutoJoinSetClick(bool bIsOn)
        {
            if (EntityFactory.MainHeroView == null)
                return;

            int selfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            if (selfID != LogicDataCenter.clanDataManager.ClanBaseData.nShaikhID)
                return;

            GameLogic.cmd_clan_set_autoacceptflag cmdData = new GameLogic.cmd_clan_set_autoacceptflag();

            cmdData.nAutoAcceptFlag = (autoJoinSet.isOn ? 1 : 0);

            ViewEventHelper.Instance.SendCommand<cmd_clan_set_autoacceptflag>(GameLogicDef.GVIEWCMD_CLAN_SET_AUTOACCEPT, ref cmdData);
            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CLAN_MODIFY_ENROLL_STUDENT);
        }

        public void OnClanCloseBtnClick()
        {
            // 关闭窗体显示
            UISystem.Instance.ShowWnd(WndID.WND_ID_CLAN_ACCEPTMEMBER, false);
        }
    }
}
