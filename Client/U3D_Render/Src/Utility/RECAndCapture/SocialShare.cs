using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using UnityEngine;
using USpeedUI;

namespace SocialSharing
{
    public enum SocialShareUploadWnd
    {
        GloryMoment,
        WarStatic,
        WarRecordEnd,
    }

    //分享平台
    public enum EMSocialSharePlatform
    {
        EMSSP_None,
        EMSSP_WECHAT,      //微信
        EMSSP_SINA,        //新浪
        EMSSP_QZONE,       //空间
        EMSSP_QQ,          //QQ
        MAX
    }

    //社交分享数据
    public class SocialShareData
    {
        public string szTitle;       //标题
        public string szDesc;        //描述
        public string imgUrl = "";   //图片路径，网络地址
        public EMSocialSharePlatform sharePlatform = EMSocialSharePlatform.EMSSP_None;  //分享平台
    }

    public class SocialShare : MonoBehaviour
    {
        private bool bUploadFinish = true;
        private const string defaultWebUrl = "http://tg.q1.com/tt/";

        private uint checkPoint;

        public delegate void SocialShareUploadHanlder(string szImgUrl);
        public static event SocialShareUploadHanlder SocialShareUploadEvent;

        private static SocialShare instance;

        public static SocialShare GetInstance()
        {
            if (instance == null)
            {
                //挂载在输入管理控件上
                if (InputManager.InstanceGo != null)
                    instance = InputManager.InstanceGo.AddComponent<SocialShare>();
                else
                {
                    GameObject go = new GameObject();
                    go.name = "SocialShare";
                    DontDestroyOnLoad(go);
                    instance = go.AddComponent<SocialShare>();
                }
            }
            return instance;
        }

        public void OnDestroy()
        {
            StopAllCoroutines();
        }

        public void Update()
        {
            if (!bUploadFinish)
            {
                uint nowTime = GameLogicAPI.getTickCount();

                if (nowTime - checkPoint > 5000)
                    bUploadFinish = true;
            }
        }

        //上传文件
        public void UploadFile(byte[] fileData)
        {
            if (bUploadFinish)
            {
                USystemFloatTextData uploadmsg = new USystemFloatTextData();
                uploadmsg.message = ULocalizationService.Instance.Get("UIView", "GloryMoment", "Uploading");
                uploadmsg.nPos = 1;
                uploadmsg.nShowTime = 2000;
                USpeedUI.UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMFLOATTEXT_CREATE, uploadmsg);

                StopCoroutine("TransmitPicture");
                StartCoroutine(TransmitPicture(fileData));
            }
        }

        //上传至服务器
        IEnumerator TransmitPicture(byte[] fileData)
        {
            bUploadFinish = false;
            checkPoint = GameLogicAPI.getTickCount();

            WWWForm form = new WWWForm();
            form.AddBinaryData("shareImg", fileData, "shareImg.jpg");
            WWW www = new WWW("http://res.szgla.com/UploadCS.axd?gameid=7&showlogo=80", form);

            yield return www;

            if (www.error != null)
            {
                Debug.Log(www.error);

                USystemFloatTextData failedMessage = new USystemFloatTextData();
                failedMessage.message = ULocalizationService.Instance.Get("UIView", "GloryMoment", "UploadFailed");
                failedMessage.nPos = 1;
                failedMessage.nShowTime = 2000;
                USpeedUI.UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMFLOATTEXT_CREATE, failedMessage);

                yield return null;
            }
            else
            {
                string url = www.text;

                if(SocialShareUploadEvent != null)
                {
                    SocialShareUploadEvent(url);
                }

                USystemFloatTextData successMessage = new USystemFloatTextData();
                successMessage.message = ULocalizationService.Instance.Get("UIView", "GloryMoment", "UploadSuccessed");
                successMessage.nPos = 1;
                successMessage.nShowTime = 2000;
                USpeedUI.UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMFLOATTEXT_CREATE, successMessage);

            }
            bUploadFinish = true;
        }

        //社交分享
        public static void ShareToSocialPlatform(SocialShareData data)
        {
            switch (data.sharePlatform)
            {
                case EMSocialSharePlatform.EMSSP_SINA:
                    {
                        string sourceUrl = "http://service.weibo.com/share/share.php?url={0}&title={1}  {2}&pic={3}";

                        string url = string.Format(sourceUrl, GetWebUrl(), System.Web.HttpUtility.UrlEncode(data.szTitle, Encoding.UTF8), System.Web.HttpUtility.UrlEncode(data.szDesc, Encoding.UTF8), data.imgUrl);

                        Application.OpenURL(url);
                    }
                    break;
                case EMSocialSharePlatform.EMSSP_QZONE:
                    {
                        string sourceUrl = "http://sns.qzone.qq.com/cgi-bin/qzshare/cgi_qzshare_onekey?url={0}&title={1}&desc={2}&pics={3}";

                        string url = string.Format(sourceUrl, GetWebUrl(), System.Web.HttpUtility.UrlEncode(data.szTitle, Encoding.UTF8), System.Web.HttpUtility.UrlEncode(data.szDesc, Encoding.UTF8), data.imgUrl);

                        Application.OpenURL(url);
                    }
                    break;
                case EMSocialSharePlatform.EMSSP_QQ:
                    {
                        //string sourceUrl = "http://connect.qq.com/widget/shareqq/index.html?url={0}&desc={1}&pics={2}";

                        //string url = string.Format(sourceUrl, GetWebUrl(), System.Web.HttpUtility.UrlEncode(data.szDesc, Encoding.UTF8), data.imgUrl);

                        //string url = string.Format(sourceUrl, webUrl, System.Web.HttpUtility.UrlEncode(data.szDesc), data.imgUrl);

                        string sourceUrl = "http://share.v.t.qq.com/index.php?c=share&a=index&title={0} {1}&url={2}&pic={3}";
                        string url = string.Format(sourceUrl, Uri.EscapeDataString(data.szTitle), Uri.EscapeDataString(data.szDesc), GetWebUrl(), data.imgUrl);


                        Application.OpenURL(url);
                    }
                    break;
                case EMSocialSharePlatform.EMSSP_None:
                case EMSocialSharePlatform.EMSSP_WECHAT:
                default:
                    break;
            }
        }

        private static string GetWebUrl()
        {
            string szWebUrl = "";

            szWebUrl = DataCenter.LogicDataCenter.gamePromotionDataManager.ShareURL;

            if (string.IsNullOrEmpty(szWebUrl))
                szWebUrl = defaultWebUrl;

            uint promotionID = GameLogicAPI.getPlayerUserID(EntityFactory.MainHeroID);

            if(promotionID > 0)
            {
                szWebUrl += promotionID;
            }

            return szWebUrl;
        }
    }
}

