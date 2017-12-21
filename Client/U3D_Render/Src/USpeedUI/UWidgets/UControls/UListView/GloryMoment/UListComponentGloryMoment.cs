using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using USpeedUI;
using UIWidgets;
using USpeedUI.PlayerFrame;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListComponentGloryMoment : ListViewItem
    {
        public Image HighLine;
        public Image TagIcon;
        public RawImage ShotRawImg;
        public Image HeroIcon;
        public Text DateLabel;
        public Text TimeLabel;
        public GameObject PlayBtnGo;
        public GameObject ShareBtnGo;
        public Text ShareBtnTxt;
        public int shotImgWidth;
        public int shotImgHeight;
        public List<Sprite> TagIconList;

        private int fileIndex;
        private Int64 dbWarID;
        private Texture2D shotImg;
        private string fileName;
        private byte[] imgData;
        private EMGloryMomentPageType itemType;

        public virtual void SetData(UListItemGloryMoment item)
        {
            fileIndex = item.Index;
            imgData = item.imgData;
            fileName = item.fileName;
            itemType = item.pageType;
            dbWarID = item.dbWarID;

            if (item.imgData != null && item.imgData.Length > 0)
            {
                if (shotImg == null)
                    shotImg = new Texture2D(shotImgWidth, shotImgHeight);

                if (shotImg != null)
                {
                    shotImg.LoadImage(item.imgData);
                    if (ShotRawImg != null)
                        ShotRawImg.texture = shotImg;
                }
            }

            if(PlayBtnGo != null)
            {
                PlayBtnGo.gameObject.SetActive(item.pageType == EMGloryMomentPageType.EMPage_Video);
            }

            if (TagIcon != null)
            {
                TagIcon.gameObject.SetActive(false);
                if(item.TagIconID >= 0 && TagIconList.Count > item.TagIconID)
                {
                    TagIcon.sprite = TagIconList[item.TagIconID];
                    TagIcon.gameObject.SetActive(true);
                }
            }

            if (HeroIcon != null)
            {
                HeroIcon.gameObject.SetActive(true);
                if (item.heroID > 0)
                {
                    Sprite heroSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 3, item.heroID);
                    HeroIcon.sprite = heroSprite;
                    HeroIcon.gameObject.SetActive(true);
                }
            }

            if (DateLabel != null)
            {
                int nIndex = item.date.IndexOf('-');
                DateLabel.text = item.date.Substring(nIndex + 1);
            }

            if (TimeLabel != null)
            {
                TimeLabel.text = item.time;
            }
        }

        public override void OnPointerEnter(PointerEventData eventData)
        {
            base.OnPointerEnter(eventData);
            if (HighLine != null)
            {
                HighLine.gameObject.SetActive(true);
            }
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            base.OnPointerExit(eventData);
            if (HighLine != null)
            {
                HighLine.gameObject.SetActive(false);
            }
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            base.OnPointerClick(eventData);
            OpenShare();
        }

        public void OpenShare()
        {
            if (itemType == EMGloryMomentPageType.EMPage_Image)
            {
                USocialShareShowMsgData msgData = new USocialShareShowMsgData();
                msgData.fileIndex = fileIndex;
                msgData.imgPath = fileName;
                msgData.imgData = imgData;
                msgData.nDBWarID = dbWarID;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_OPEN_SOCIALSHARE_FRAME, msgData);
            }
            else
            {
                //try
                //{
                //    System.Diagnostics.Process.Start(fileName);
                //}
                //catch (System.Exception ex)
                //{
                //    Trace.LogError(ex.ToString());
                //}
                UGloryMomentVideoMsgData msgData = new UGloryMomentVideoMsgData();
                msgData.fileIndex = fileIndex;
                msgData.videoPath = fileName;
                msgData.nDBWarID = dbWarID;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_OPEN_GLORYMOMENT_VIDEO_FRAME, msgData);
            }
        }

        protected override void OnDestroy()
        {
            if (shotImg != null)
            {
                DestroyImmediate(shotImg);
            }
            base.OnDestroy();
        } 
    }
}
