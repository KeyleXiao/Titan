using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using USpeedUI;
using USpeedUI.TooltipAndDialog;
using Team_ManagedDef;

namespace DataCenter
{
    public class TeamDataManager
    {

        public void Init()
        {
           
        }

        public void onTeamSubInvitedSideButton(IntPtr ptr, int nLen)
        {
            // 邀请加入队伍
            cmd_team_invited2view data = IntPtrHelper.toData<cmd_team_invited2view>(ptr);

            // 邀请加入队伍
            DIDAAddData msg = new DIDAAddData();
            msg.nItemID = (int)EDIDAType.EDIDA_Team;
            msg.bAllowRepeatItem = true;
            msg.nButtonIconID = (int)EDIDAType.EDIDA_Team;
            msg.strButtonText = ULocalizationService.Instance.Get("UIView", "Team", "inviteButtonDes");
            msg.strTooltip = ULocalizationService.Instance.Get("UIView", "Team", "inviteButtonDes");

            SideButtonDataBuildingSN dataInfo = new SideButtonDataBuildingSN();
            dataInfo.nBuildingSN = data.nBuildingSN;
            msg.sCustomData = dataInfo;
            msg.callBack = (CustomerInfo) =>
            {
                SideButtonDataBuildingSN customerInfo = CustomerInfo as SideButtonDataBuildingSN;
                UIDialogData tooltipData = new UIDialogData(
                                   title: ULocalizationService.Instance.Get("UIView", "Team", "TeamTitleDes"),
                                   message: "[" + data.szInviterName + "]" + ULocalizationService.Instance.Get("UIView", "Team", "inviteMsgBoxDes"),
                                   buttons: new UIWidgets.DialogActions()   { 
                                                                                { ULocalizationService.Instance.Get("UIView", "Team", "AgreeButtonDes"), () => { InvitedSideButtonCallBack(customerInfo, (int)EBuildFlowResult.EBuildFlowResult_Agree );return true; } }, 
                                                                                { ULocalizationService.Instance.Get("UIView", "Team", "DisAgreeButtonDes"), () => { InvitedSideButtonCallBack(customerInfo, (int)EBuildFlowResult.EBuildFlowResult_Disagree );return true; } } 
                                                                            });
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, tooltipData);
            };

            LogicDataCenter.didaDataManager.AddItem(msg);
        }

        private string a2utf8(string p)
        {
            throw new NotImplementedException();
        }

        public void InvitedSideButtonCallBack(SideButtonCustomData sCustomData, int Result)
        {
            //DIDARemoveData msg = new DIDARemoveData();
            //msg.nItemID = (int)EDIDAType.EDIDA_Team;
            //msg.sCustomData = sCustomData;
            //LogicDataCenter.didaDataManager.RemoveItem(msg);

            cmd_team_invited_respon data = new cmd_team_invited_respon();
            SideButtonDataBuildingSN SNData = sCustomData as SideButtonDataBuildingSN;
            data.nBuildingSN = SNData.nBuildingSN;
            data.nReslut = Result;
            ViewEventHelper.Instance.SendCommand<cmd_team_invited_respon>(GameLogicDef.GVIEWCMD_TEAM_INVITE_RESPON, ref data);

        }

        public void onTeamSubRequestSideButton(IntPtr ptr, int nLen)
        {
            // 请求加入队伍
            cmd_team_subrequest2view data = IntPtrHelper.toData<cmd_team_subrequest2view>(ptr);

            // 请求加入队伍
            string msgDes = "";
            if (data.bTargeInTeamFlag == 1)
            {
                msgDes = ULocalizationService.Instance.Get("UIView", "Team", "TargetInTeam1Des");
            }
            else
            {
                msgDes = ULocalizationService.Instance.Get("UIView", "Team", "TargetInTeam0Des");
            }

            DIDAAddData msg = new DIDAAddData();
            msg.nItemID = (int)EDIDAType.EDIDA_Team;
            msg.bAllowRepeatItem = true;
            msg.nButtonIconID = (int)EDIDAType.EDIDA_Team;
            msg.strButtonText = ULocalizationService.Instance.Get("UIView", "Team", "requestButtonDes");
            msg.strTooltip = ULocalizationService.Instance.Get("UIView", "Team", "requestButtonDes");

            SideButtonDataBuildingSN dataInfo = new SideButtonDataBuildingSN();
            dataInfo.nBuildingSN = data.nBuildingSN;
            msg.sCustomData = dataInfo;
            msg.callBack = (CustomerData) =>
            {
                SideButtonDataBuildingSN customerData = CustomerData as SideButtonDataBuildingSN;
                UIDialogData tooltipData = new UIDialogData(
                                   title: ULocalizationService.Instance.Get("UIView", "Team", "TeamTitleDes"),
                                   message: "[" + data.szRequestName + "]" + msgDes,
                                   buttons: new UIWidgets.DialogActions()   { 
                                                                                {ULocalizationService.Instance.Get("UIView", "Team", "AgreeButtonDes"), () => { RequestSideButtonCallBack(customerData, (int)EBuildFlowResult.EBuildFlowResult_Agree );return true; }}, 
                                                                                {ULocalizationService.Instance.Get("UIView", "Team", "DisAgreeButtonDes"), () => { RequestSideButtonCallBack(customerData, (int)EBuildFlowResult.EBuildFlowResult_Disagree );return true; } } 
                                                                            });
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, tooltipData);
            };

            LogicDataCenter.didaDataManager.AddItem(msg);
        }

        public void RequestSideButtonCallBack(SideButtonCustomData sCustomData, int Result)
        {
            //DIDARemoveData msg = new DIDARemoveData();
            //msg.nItemID = (int)EDIDAType.EDIDA_Team;
            //msg.sCustomData = sCustomData;
            //LogicDataCenter.didaDataManager.RemoveItem(msg);

            cmd_team_request_respon data = new cmd_team_request_respon();
            SideButtonDataBuildingSN SNData = sCustomData as SideButtonDataBuildingSN;
            data.nBuildingSN = SNData.nBuildingSN;
            data.nReslut = Result;
            ViewEventHelper.Instance.SendCommand<cmd_team_request_respon>(GameLogicDef.GVIEWCMD_TEAM_REQUEST_RESPON, ref data);
        }

        public void RequestJoin(int PlayerID)
        {
            cmd_team_request2logic data = new cmd_team_request2logic();
            data.nTargetPDBID = PlayerID;
            ViewEventHelper.Instance.SendCommand<cmd_team_request2logic>(GameLogicDef.GVIEWCMD_TEAM_REQUEST, ref data);
        }

        public void InviteJoin(int PlayerID)
        {
            cmd_team_invite2logic data = new cmd_team_invite2logic();
            data.nTargetPDBID = PlayerID;
            ViewEventHelper.Instance.SendCommand<cmd_team_invite2logic>(GameLogicDef.GVIEWCMD_TEAM_INVITE, ref data);
        }

        public void AppointCaptain(int PlayerID)
        {
            cmd_team_appointCaptain2logic data = new cmd_team_appointCaptain2logic();
            data.nTargetPDBID = PlayerID;
            ViewEventHelper.Instance.SendCommand<cmd_team_appointCaptain2logic>(GameLogicDef.GVIEWCMD_TEAM_APPOINTCAPTAIN, ref data);
        }

        public void KickoutMember(int PlayerID)
        {
            cmd_team_kickoutMember2logic data = new cmd_team_kickoutMember2logic();
            data.nTargetPDBID = PlayerID;
            ViewEventHelper.Instance.SendCommand<cmd_team_kickoutMember2logic>(GameLogicDef.GVIEWCMD_TEAM_KICKOUTMEMBER, ref data);
        }

        public void LeaveTeam()
        {
            IntPtr data = IntPtr.Zero;
            ViewEventHelper.Instance.SendCommand<IntPtr>(GameLogicDef.GVIEWCMD_TEAM_LEAVETEAM, ref data);
        }

        public void DisbandTeam()
        {
            IntPtr data = IntPtr.Zero;
            ViewEventHelper.Instance.SendCommand<IntPtr>(GameLogicDef.GVIEWCMD_TEAM_DISBANDTEAM, ref data);
        }        
    }
}
