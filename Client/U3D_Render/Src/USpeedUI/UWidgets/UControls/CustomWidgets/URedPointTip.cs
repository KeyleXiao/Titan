using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI.UWidgets
{
    public class URedPointTip : MonoBehaviour
    {
        public Image baseImage;
        public Text countText;
        public Image backImage;

        static private ResNode redPointRes;
        static public URedPointTip CreateRedPointObject(RectTransform parent, float fOffsetX = 0f, float fOffsetY = 0f)
        {
            if (redPointRes == null)
            {
                redPointRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Widgets/RedPointTip/RedPointTip");
            }

            if (redPointRes != null)
            {
                URedPointTip redPoint = redPointRes.InstanceMainRes<URedPointTip>();
                if (redPoint)
                {
                    redPoint.AttachToRectTransform(parent, fOffsetX, fOffsetY);
                    redPoint.gameObject.SetActive(false);
                    return redPoint;
                }
            }

            return null;
        }

        static public void DestroyRedPointTip(URedPointTip redPoint)
        {
            if(!redPoint)
            {
                return;
            }

            if(null == redPointRes)
            {
                return;
            }
            var temp = redPoint.gameObject;
            redPointRes.DestroyInstanceRes(ref temp);
        }

        public void Awake()
        {
            gameObject.SetActive(false);
        }

        void AttachToRectTransform(RectTransform parent, float fOffsetX, float fOffsetY)
        {
            if (parent == null)
                return;

            RectTransform selfTrans = transform as RectTransform;
            selfTrans.SetParent(parent,false);
            selfTrans.anchoredPosition = new Vector2(parent.rect.xMax + fOffsetX, parent.rect.yMax + fOffsetY);
        }

        public void SetTipCount(int nCount)
        {
            if (nCount <= 0)
            {
                gameObject.SetActive(false);
                return;
            }

            gameObject.SetActive(true);

            if (nCount > 99)
            {
                if (backImage != null)
                {
                    baseImage.enabled = false;
                    backImage.enabled = true;
                }
                
                countText.text = " 99+";
            }
            else
            {
                if (backImage != null)
                    backImage.enabled = false;

                baseImage.enabled = true;
                countText.text = nCount.ToString();
            }            
        }
    }
}
