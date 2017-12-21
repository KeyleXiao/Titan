using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using System.ComponentModel;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListItemHallData //: INotifyPropertyChanged
    {
        public int nRoomIndex;
        public int nRoomId;
        public string RoomName;
        public string GameMap;

        public int nTotalPlayerCount;
        public bool bLock;

        protected int m_nCurPlayerCount;
        public int nCurPlayerCount
        {
            get { return m_nCurPlayerCount; }

            set
            {
                if (m_nCurPlayerCount != value)
                {
                    m_nCurPlayerCount = value;
                   // Changed("m_nCurPlayerCount");
                }
            }
        }


        //暂时不需要，有需要再启用数据驱动更新列表
        //public event PropertyChangedEventHandler PropertyChanged;

        //void Changed(string propertyName = "")
        //{
        //    if (PropertyChanged != null)
        //    {
        //        PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        //    }
        //}
    }
}


