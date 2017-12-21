using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using UIWidgets;
using GameLogic;

namespace USpeedUI.UWidgets
{
    public class UTileComponentRoomOb : ListViewItem
    {
        [SerializeField]
        public Image HeadIcon;

        [SerializeField]
        public Text ActorName;

        [SerializeField]
        public Image SexIcon;

        [SerializeField]
        public Button DeleteBtn;

        private GameWaitingRoomView m_wndView;
        private bool m_bIsMaster;
        private int m_nPos;

        public void SetData(UTileItemRoomOb item)
        {
            if (item == null)
                return;

            m_wndView = item.wndView;
            m_bIsMaster = item.bIsRoomMaster;
            m_nPos = item.data.nObPos;

            ActorName.text = item.data.szPlayerName;

            if (item.data.nSex < (int)PERSON_SEX.SEX_MAX && item.data.nSex >= (int)PERSON_SEX.SEX_MALE)
            {
                SexIcon.gameObject.SetActive(true);
                SexIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_GAMEWAITINGROOMVIEW, item.data.nSex + 1);
            }
            else
            {
                SexIcon.gameObject.SetActive(false);
            }

            DeleteBtn.onClick.RemoveListener(OnDeleteBtnClick);
            DeleteBtn.onClick.AddListener(OnDeleteBtnClick);
            int nSelfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            DeleteBtn.gameObject.SetActive(item.bIsRoomMaster);
        }

        public void OnDeleteBtnClick()
        {
            cmd_match_room_hero_exit data = new cmd_match_room_hero_exit();
            data.bIsOb = 1;
            data.nPos = m_nPos;
            ViewEventHelper.Instance.SendCommand<cmd_match_room_hero_exit>(GameLogicDef.GVIEWCM_MATCH_MASTER_KICK_ROLE, ref data);
        }
    }
}
