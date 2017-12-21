using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using DataCenter;
using UIWidgets;
using DG.Tweening;
using USpeedUI.Social;

namespace USpeedUI.ClanMember
{
    public class ClanMemberWnd : UIBaseSubWnd<ClanMemberWndView>
    {
        // 获取ID
        public override int GetWndType()
        {
            return (int)ClanListFrameWndType.WND_CLAN_MEMBER;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewClanMember";
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            return true;
        }

        // 设置是否可见
        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData data = msgData as UIMsgCmdData;
            switch (msgID)
            {
                case WndMsgID.WND_MSG_CLAN_UPDATEMEMBER:
                    {
                        if (m_wndView != null)
                            m_wndView.OnClanUpdateMember();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class ClanMemberWndView : UIBaseSubWndView
    {
        public Text listNameDesc;
        public Text listJobDesc;
        public Text listGaveThisWeekDesc;
        public Text listPowerDesc;
        public Text listRankDesc;
        public Text listGradeDesc;
        public Text listStatusDesc;
        public Text listKinDesc;
        private bool isNameDescending = false;
        private bool isJobDescending = false;
        private bool isLevelDescending = false;
        private bool isGaveThisWeekDescending = false;
        private bool isPowerDescending = false;
        private bool isRankDescending = false;
        private bool isGradeDescending = false;
        private bool isStatusDescending = false;
        private bool isKinDescending = false;

        public UListClanMember listClanMember;

        public Toggle autoShowLeaveTog;

        public override bool Init(IUISubWnd wnd)
        {
            listNameDesc.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ListName");
            listJobDesc.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ListJob");
            listGaveThisWeekDesc.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ListGaveThisWeek");
            listPowerDesc.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ListPower");
            listRankDesc.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ListRank");
            listGradeDesc.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ListGrade");
            listStatusDesc.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ListStatus");
            listKinDesc.text = ULocalizationService.Instance.Get("UIView", "ClanDetail", "ListKin");

            autoShowLeaveTog.isOn = true;

            return base.Init(wnd);
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if (visible)
            {
                OnClanUpdateMember();
            }
        }

        public void OnClanUpdateMember()
        {
            UpdateClanMember(LogicDataCenter.clanDataManager.ClanMemberData);
        }

        public void AutoSetting(bool _bToggle)
        {
            //ImageSetting.currAsinoEnabled = _bToggle ? 1 : 0;
            //ImageSetting.SetTextureAniso(ImageSetting.currAsinoEnabled);

            OnClanUpdateMember();
        }

#region // 点击标签排序
        public void OnNameBtnClicked()
        {
            List<cmd_clan_memberlist> memberList = LogicDataCenter.clanDataManager.ClanMemberData;
            if (memberList != null)
            {
                UpdateClanMember(isNameDescending ? memberList.OrderByDescending(n => n.szName).ToList() : memberList.OrderBy(n => n.szName).ToList());
            }

            isNameDescending = !isNameDescending;
        }

        public void OnJobBtnClicked()
        {
            List<cmd_clan_memberlist> memberList = LogicDataCenter.clanDataManager.ClanMemberData;
            if (memberList != null)
            {
                memberList.Sort((item1, item2) => {
                    int x, y;
                    x = (byte)((item1.nIdentity == (byte)Clan_ManagerDef.EMClanIdentity.EMCLANIDENTITY_SHAIKH) ? item1.nIdentity + 2 : item1.nIdentity);
                    y = (byte)((item2.nIdentity == (byte)Clan_ManagerDef.EMClanIdentity.EMCLANIDENTITY_SHAIKH) ? item2.nIdentity + 2 : item2.nIdentity);

                    return isJobDescending ? y - x : x - y;
                });

                UpdateClanMember(memberList);
            }

            isJobDescending = !isJobDescending;
        }

        public void OnLevelBtnClicked()
        {
            List<cmd_clan_memberlist> memberList = LogicDataCenter.clanDataManager.ClanMemberData;
            if (memberList != null)
            {
                UpdateClanMember(isLevelDescending ? memberList.OrderByDescending(n => n.nLevel).ToList() : memberList.OrderBy(n => n.nLevel).ToList());
            }

            isLevelDescending = !isLevelDescending;
        }

        public void OnGaveThisWeekBtnClicked()
        {
            List<cmd_clan_memberlist> memberList = LogicDataCenter.clanDataManager.ClanMemberData;
            if (memberList != null)
            {
                UpdateClanMember(isGaveThisWeekDescending ? memberList.OrderByDescending(n => n.nDonateThisWeek).ToList() : memberList.OrderBy(n => n.nDonateThisWeek).ToList());
            }

            isGaveThisWeekDescending = !isGaveThisWeekDescending;
        }

        public void OnGradeBtnClicked()
        {
            List<cmd_clan_memberlist> memberList = LogicDataCenter.clanDataManager.ClanMemberData;
            if (memberList != null)
            {
                UpdateClanMember(isGradeDescending ? memberList.OrderByDescending(n => n.szGrade).ToList() : memberList.OrderBy(n => n.szGrade).ToList());

                memberList.Sort((item1, item2) => {
                    int x = 0;
                    int y = 0;
                    int.TryParse(item1.szGrade, out x);
                    int.TryParse(item2.szGrade, out y);
                    return isJobDescending ? y - x : x - y;
                });
            }

            isGradeDescending = !isGradeDescending;
        }

        public void OnPowerBtnClicked()
        {
            List<cmd_clan_memberlist> memberList = LogicDataCenter.clanDataManager.ClanMemberData;
            if (memberList != null)
            {
                UpdateClanMember(isPowerDescending ? memberList.OrderByDescending(n => n.dwBattleVal).ToList() : memberList.OrderBy(n => n.dwBattleVal).ToList());
            }

            isPowerDescending = !isPowerDescending;
        }

        public void OnRankBtnClicked()
        {
            List<cmd_clan_memberlist> memberList = LogicDataCenter.clanDataManager.ClanMemberData;
            if (memberList != null)
            {
                UpdateClanMember(isRankDescending ? memberList.OrderByDescending(n => n.dwBVOrder).ToList() : memberList.OrderBy(n => n.dwBVOrder).ToList());
            }

            isRankDescending = !isRankDescending;
        }

        public void OnStatusBtnClicked()
        {
            List<cmd_clan_memberlist> memberList = LogicDataCenter.clanDataManager.ClanMemberData;
            if (memberList != null)
            {
                UpdateClanMember(isStatusDescending ? memberList.OrderByDescending(n => n.nGameStatus).ToList() : memberList.OrderBy(n => n.nGameStatus).ToList());
            }

            isStatusDescending = !isStatusDescending;
        }

        public void OnKinBtnClicked()
        {
            List<cmd_clan_memberlist> memberList = LogicDataCenter.clanDataManager.ClanMemberData;
            if (memberList != null)
            {
                UpdateClanMember(isKinDescending ? memberList.OrderByDescending(n => n.szKinName).ToList() : memberList.OrderBy(n => n.szKinName).ToList());
            }

            isKinDescending = !isKinDescending;
        }
#endregion

        private void UpdateClanMember(List<cmd_clan_memberlist> clanMemberList)
        {
            if (clanMemberList == null)
            {
                return;
            }

            listClanMember.DataSource.BeginUpdate();

            listClanMember.DataSource.Clear();
            for (int i = 0; i < clanMemberList.Count; ++i)
            {
                // 不显示离线成员
                if (!autoShowLeaveTog.isOn)
                {
                    // 玩家不在线,不处理
                    if (clanMemberList[i].nIsOnline <= 0)
                    {
                        continue;
                    }
                }
                UListItemClanMember item = new UListItemClanMember();
                item.data = clanMemberList[i];
                listClanMember.DataSource.Add(item);
            }

            listClanMember.DataSource.EndUpdate();
        }
    }
}
