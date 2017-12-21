using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine.UI;
using USpeedUI;
using UnityEngine;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListComponentHall : ListViewItem
    {
        public Text RoomId;
        public Text RoomName;
        public Text GameMap;
        public Text PlayerCount;
        public GameObject LockState;
        public Image SelectIcon;
        public int nRoomId;
        public int fClickTime = 0;
        public virtual void SetData(UListItemHallData item)
        {
            RoomId.text = item.nRoomIndex.ToString();
            RoomName.text = item.RoomName;
            GameMap.text = item.GameMap;
            PlayerCount.text = item.nCurPlayerCount+" / "+item.nTotalPlayerCount;
            LockState.SetActive(item.bLock);
            nRoomId = item.nRoomId;
            SelectIcon.gameObject.SetActive(false);
        }

        public void SelectColoring()
        {
            SelectIcon.gameObject.SetActive(true);
        }

        public void DefaultColoring()
        {
            SelectIcon.gameObject.SetActive(false);
        }
    }
}
