using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using DataCenter;
using U3D_Render.Common;
using USpeedUI.UWidgets;

namespace USpeedUI.WarGoodsStore
{
    [Serializable]
    public class UWarGoodsStoreTreeComponentData : UCheckBoxTreeViewItem//TreeViewItem
    {
        //物品类型深度
        [SerializeField]
        public int Depth;
        /// <summary>
        /// 类型标签名字
        /// </summary>
        [SerializeField]
        public string GoodsLabel;
        /// <summary>
        /// 类型标签ID
        /// </summary>
        [SerializeField]
        public int GoodsLabelID;


        public UWarGoodsStoreTreeComponentData(string strItemName, UnityEngine.Sprite sprIcon)
            : base(strItemName, sprIcon)
        {
            GoodsLabel = strItemName;
        }
    }
    [Serializable]
    public class UWarGoodsStoreTreeComponent : UCheckboxTreeViewComponent<UWarGoodsStoreTreeComponentData>//TreeViewComponentBase<UWarGoodsStoreTreeComponentData>//, IPointerClickHandler
    {
        //UTooltipTrigger triggerTip;
        public float fLastClkCount = 0;

        protected override void Start()
        {
            base.Start();
        }

        protected override void Awake()
        {
            if ((LocalPositionZReset) && (transform.localPosition.z != 0f))
            {
                var pos = transform.localPosition;
                pos.z = 0f;
                transform.localPosition = pos;
            }
        }

        private int nItemDepth;
        public int ItemDepth
        {
            get { return nItemDepth; }
            protected set
            {
                nItemDepth = value;

                //if(triggerTip == null)
                //    triggerTip = this.gameObject.GetComponent<UTooltipTrigger>();

                //if (nItemDepth == 0)
                //{
                //    triggerTip.enabled = false;
                //}
                //else
                //    triggerTip.enabled = true;
            }

        }

        public override void SetData(TreeNode<UWarGoodsStoreTreeComponentData> node, int depth)
        {
            Node = node;
            ItemDepth = depth;
            base.SetData(Node, depth);
            // SetRuneDesc();
        }
    }
}
