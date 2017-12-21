using System;
using System.Collections.Generic;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI;
using USpeedUI.UWidgets;
using UnityEngine.EventSystems;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListComponentVoiceUser : ListViewItem
    {
        public Image stateIcon;
        public Image vestIcon;
        public Text playerName;
        public Image rankIcon;
        public Image sexIcon;
        public Text kinName;
        public Sprite OnMic;
        public Sprite BanMic;
        public Sprite SuperVest;
        public Sprite AdminVest;
        public Sprite GuestVest;
        //public UPopupMenuHandle popMenuHandle;

        private string szAddAdmin;
        private string szRemoveAdmin;

        private UPopupAction popupAction = new UPopupAction();

        public virtual void SetData(UListItemVoiceUser voiceUser)
        {
            szAddAdmin = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "szAddAdmin");
            szRemoveAdmin = ULocalizationService.Instance.Get("UIView", "VoiceSystemView", "szRemoveAdmin");

            stateIcon.gameObject.SetActive(false);
            if (voiceUser.isSpeaking)
            {
                stateIcon.sprite = OnMic;
                stateIcon.gameObject.SetActive(true);
            }
            else
            {
                //stateIcon.sprite = BanMic;
                stateIcon.gameObject.SetActive(false);
            }

            if (voiceUser.vest == (int)EMVoiceUserType.VoiceUserType_OW
            || voiceUser.vest == (int)EMVoiceUserType.VoiceUserType_GM)
            {
                vestIcon.sprite = SuperVest;
            }
            else if (voiceUser.vest == (int)EMVoiceUserType.VoiceUserType_Admin
                        || voiceUser.vest == (int)EMVoiceUserType.VoiceUserType_Host)
            {
                vestIcon.sprite = AdminVest;
            }
            else
                vestIcon.sprite = GuestVest;

            sexIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_VOICESYSTEM, voiceUser.sex + 1);

            playerName.text = voiceUser.playerName;
            rankIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, WndID.WND_ID_VOICESYSTEM, 1,voiceUser.rank);
            kinName.text = voiceUser.kinName;

            popupAction.Clear();
            popupAction.Add(szAddAdmin, new UListComponentItemDataPopup(() => { ReqAddAdmin(voiceUser.userID, true); }));
            popupAction.Add(szRemoveAdmin, new UListComponentItemDataPopup(() => { ReqAddAdmin(voiceUser.userID, false); }));
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            if (eventData.button == PointerEventData.InputButton.Right)
            {
                if ((DataCenter.LogicDataCenter.voiceSystemDataManager.UserRight & (int)EMVoiceRightFlag.VoiceRight_Owner) != 0)
                {
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_POPUPMENU, new UIPopupMenuData(popupAction, this.RectTransform, UIWidgets.TipPosition.MouseBottomRightCorner));         
                }
            }
        }

        private void ReqAddAdmin(int uid,bool add)
        {
            GameLogic.cmd_voice_req_add_vest data = new GameLogic.cmd_voice_req_add_vest();
            data.nUserID = uid;
            data.isAdd = add ? 1 : 0;
            data.nVestType = (int)EMVoiceUserType.VoiceUserType_Admin;
            ViewEventHelper.Instance.SendCommand<GameLogic.cmd_voice_req_add_vest>(GameLogic.GameLogicDef.GVIEWCMD_VOICE_REQUEST_ADD_VEST, ref data);
        }
    }
}
