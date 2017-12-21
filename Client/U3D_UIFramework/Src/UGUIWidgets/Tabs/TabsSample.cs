
using System;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;

namespace UIWidgetsSamples
{
    [Serializable]
    public class TabSample:Tab
    {
        //扩展其他属性
    }

    public class TabSampleButton:TabButton
    {
        /// <summary>
        /// The name.
        /// </summary>
        [SerializeField]
        public Text Name;

        public virtual void SetData(TabSample tab)
        {
            Name.text = tab.Name;
        }
    }

    public class TabsSample : TabsCustom<TabSample, TabSampleButton>
    {
        protected override void SetButtonData(TabSampleButton button, int index)
        {
            button.SetData(TabObjects[index]);

        }
    }
}
