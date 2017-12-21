using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using SocialSharing;
using ASpeedGame.Data.Scheme;

namespace USpeedUI.UWidgets
{
    /// <summary>
    /// 社交分享条
    /// </summary>
    public class UShareBar : UIBehaviour
    {
        [SerializeField]
        public Button[] ShareBtnGroup;
        [SerializeField]
        public RectTransform BtnBorderTransform;
        [SerializeField]
        public RectTransform WeChatQRFrame;
        [SerializeField]
        public RawImage QRImg;

        private int QRImgWidth;

        private int nBtnSelectedIndex;

        private byte[] shareImgData;                    //分享图片数据

        private string szShareImgPath;                  //分享图片全路径

        private string szShareImgUrl;                   //分享图片网络链接

        private SocialSharingType m_shareType;          //分享类型

        private EMSocialSharePlatform m_SharePlatform;  //分享平台

        private bool bDownLoadQR;

        private bool bInit;


        protected override void Awake()
        {
            base.Awake();

            if(QRImg != null)
            {
                QRImgWidth = (int)QRImg.rectTransform.sizeDelta.x;
            }

            SocialShare.SocialShareUploadEvent -= UploadResult;
            SocialShare.SocialShareUploadEvent += UploadResult;
        }


        protected override void OnDestroy()
        {
            base.OnDestroy();

            Clear();

            SocialShare.SocialShareUploadEvent -= UploadResult;
        }

        /// <summary>
        /// 初始化数据，参数传分享类型+图片全路径
        /// </summary>
        /// <param name="sharingType"></param>
        /// <param name="imgFullPath"></param>
        public void SetData(SocialSharingType sharingType, string imgFullPath = "")
        {
            Clear();

            if (imgFullPath == "")
                return;

            szShareImgPath = imgFullPath;
            m_shareType = sharingType;

            bInit = true;
        }

        /// <summary>
        /// 初始化数据,参数传分享类型+图片数据
        /// </summary>
        /// <param name="sharingType"></param>
        /// <param name="imgBytes"></param>
        public void SetData(SocialSharingType sharingType, byte[] imgBytes = null)
        {
            Clear();

            if (null == imgBytes)
                return;

            shareImgData = imgBytes;
            m_shareType = sharingType;

            bInit = true;
        }

        public void Clear()
        {
            szShareImgPath = "";
            szShareImgUrl = "";
            shareImgData = null;

            nBtnSelectedIndex = -1;

            m_SharePlatform = EMSocialSharePlatform.EMSSP_None;

            m_shareType = SocialSharingType.TypeMax;

            BtnBorderTransform.gameObject.SetActive(false);

            CloseWeChatFrame();

            StopAllCoroutines();

            if (bDownLoadQR)
            {
                Destroy(QRImg.texture);
                QRImg.texture = null;
                bDownLoadQR = false;
            }

            bInit = false;
        }


        /// <summary>
        /// 分享按钮点击事件
        /// </summary>
        /// <param name="nIndex"></param>
        public void OnShareBtnClick(int nIndex)
        {
            if (!bInit)
                return;

            nIndex++;

            nBtnSelectedIndex = nIndex;
            m_SharePlatform = (EMSocialSharePlatform)nIndex;

            BtnBorderTransform.anchoredPosition = ((ShareBtnGroup[nIndex-1].transform) as RectTransform).anchoredPosition;

            BtnBorderTransform.gameObject.SetActive(false);

            if (m_SharePlatform != EMSocialSharePlatform.EMSSP_WECHAT)
            {
                CloseWeChatFrame();
            }

            if(szShareImgUrl == "")
            {
                if(null == shareImgData)
                {
                    shareImgData = ImageExtensions.GetAllByteData(szShareImgPath);
                }
                if (shareImgData != null)
                {
                    SocialShare.GetInstance().UploadFile(shareImgData);
                }
            }

            ShareToPlatform();
        }


        public void CloseWeChatFrame()
        {
            if (WeChatQRFrame != null)
            {
                WeChatQRFrame.gameObject.SetActive(false);
            }
        }


        private void ShareToPlatform()
        {
            if (!bInit)
                return;

            if (szShareImgUrl == "")
                return;

            if (m_SharePlatform == EMSocialSharePlatform.EMSSP_WECHAT)
            {
                if (WeChatQRFrame != null)
                {
                    WeChatQRFrame.gameObject.SetActive(true);
                }
                if (!bDownLoadQR)
                {
                    BuildQRLink();
                }
                return;
            }

            SocialShareData data = new SocialShareData();
            SocialSharingNode node = SchemeSocialSharing.Instance.RandomSharingNode(m_shareType);
            if (node != null)
            {
                data.szTitle = node.szTitle;
                data.szDesc = node.szDesc;
            }
            data.imgUrl = szShareImgUrl;
            data.sharePlatform = m_SharePlatform;

            SocialShare.ShareToSocialPlatform(data);
        }


        private void UploadResult(string imgUrl)
        {
            if (!bInit)
                return;

            if (string.IsNullOrEmpty(imgUrl))
                return;

            szShareImgUrl = imgUrl;
            ShareToPlatform();

            BuildQRLink();
        }


        private void BuildQRLink()
        {

            if (!bInit)
                return;

            if (bDownLoadQR)
                return;

            if (szShareImgUrl == "")
                return;

            //--二维码打开的链接
            StringBuilder link = new StringBuilder("http://tt.q1.com/h5/share.html");
            link.Append("?actorname=");
            link.Append(Uri.EscapeDataString(DataCenter.LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo().playerInfo.chPlayerName));
            link.Append("&picurl=");
            link.Append(szShareImgUrl);

            StringBuilder sb = new StringBuilder("http://m.q1.com/qr.aspx?data=");
            sb.Append(System.Web.HttpUtility.UrlEncode(link.ToString(), Encoding.UTF8));
            sb.Append("&w=");       
            sb.Append(QRImgWidth);  //--图片
            sb.Append("&bw=");
            sb.Append(2);           //--边框
            sb.Append("&l=");
            sb.Append(0);           //--图标

            StopAllCoroutines();
            if (WeChatQRFrame.gameObject.activeInHierarchy)
            {
                StartCoroutine(DownLoadLinkImage(sb.ToString()));
            }
        }


        IEnumerator DownLoadLinkImage(string imgPath)
        {
            WWW www = new WWW(imgPath);
            yield return www;
            if (string.IsNullOrEmpty(www.error))
            {
                string contentType = www.responseHeaders["CONTENT-TYPE"];
                var contents = contentType.Split('/');
                if (contents[0] == "image")
                {
                    if (contents[1].ToLower() == "png" || contents[1].ToLower() == "jpeg")
                    {
                        Texture2D t = www.texture;
                        if (null != QRImg && null != t)
                        {
                            QRImg.texture = t;
                            bDownLoadQR = true;
                        }
                    }
                }
            }
        }
    }
}
