using System;
using System.Collections.Generic;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListComponentMicOrder : ListViewItem
    {
        public Image stateIcon;
        public Text index;
        public Image vest;
        public Text playerName;
        public Sprite OnMic;
        public Sprite BanMic;
        public Sprite SuperVest;
        public Sprite AdminVest;
        public Sprite GuestVest;

        public virtual void SetData(UListItemMicOrder micOrder)
        {
            stateIcon.gameObject.SetActive(false);
            if (micOrder.isSpeaking)
            {
                stateIcon.sprite = OnMic;
                stateIcon.gameObject.SetActive(true);
            }
            else
            {
                //stateIcon.sprite = BanMic;
                stateIcon.gameObject.SetActive(false);
            }
            index.text = micOrder.index;

            if (micOrder.vest == (int)EMVoiceUserType.VoiceUserType_OW
            || micOrder.vest == (int)EMVoiceUserType.VoiceUserType_GM)
            {
                vest.sprite = SuperVest;
            }
            else if (micOrder.vest == (int)EMVoiceUserType.VoiceUserType_Admin
                        || micOrder.vest == (int)EMVoiceUserType.VoiceUserType_Host)
            {
                vest.sprite = AdminVest;
            }
            else
                vest.sprite = GuestVest;

            playerName.text = micOrder.name;
        }
    }
}
