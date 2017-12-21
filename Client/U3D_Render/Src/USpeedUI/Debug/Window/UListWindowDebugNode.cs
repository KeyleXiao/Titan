using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine.UI;
using UnityEngine;
using USpeedUI;
namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListItemUIWindowDebug
    {
        public int OrginalLayer = 0;
        public int CurrentLayer = 0;
        public UIBaseWndView uiWindow = null;
    }

    public class UListComponentUIWindowDebug : ListViewItem
    {
        public Text Name;
        public Toggle toggle;

        private UListItemUIWindowDebug instance = null;
        private bool isSetingData = false;
        protected override void Start()
        {
            base.Start();
            toggle.onValueChanged.AddListener(OnValueChanged);
        }

        private void OnValueChanged(bool value)
        {
            if (null == instance || isSetingData)
            {
                return;
            }
            if(value)
            {
                GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerUI), instance.uiWindow.gameObject, true);
            }
            else
            {
                GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerIngoreRayCast), instance.uiWindow.gameObject, true);
            }
        }


        public virtual void SetData(UListItemUIWindowDebug item)
        {
            Name.text = "显示" + item.uiWindow.name;
            isSetingData = true;
            toggle.isOn = item.uiWindow.gameObject.layer != LayerMask.NameToLayer(Config.LayerIngoreRayCast);
            isSetingData = false;
            instance = item;
        }

        protected override void OnDestroy()
        {
            base.OnDestroy();
            toggle.onValueChanged.RemoveAllListeners();
        }
    }

    public class UIWindowDebugNode : TileView<UListComponentUIWindowDebug, UListItemUIWindowDebug>
    {
        protected override void SetData(UListComponentUIWindowDebug component, UListItemUIWindowDebug item)
        {
            component.SetData(item);
        }
    }
}
