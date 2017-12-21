/*******************************************************************
** 文件名:	URollText.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	梁成
** 日  期:	2016/9/18
** 版  本:	1.0
** 描  述:	UI的Text做跑马灯效果
** 应  用:  在Text下增加此脚本，调整各个参数

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI.UEffect
{
    //[AddComponentMenu("UI/Effects/URollText")]
    public class URollText : MonoBehaviour
    {
        /// <summary>
        /// 两段文字之间的间隔
        /// </summary>
        public float subTextBlank = 10.0f;

        /// <summary>
        /// 滚动速度
        /// </summary>
        public float roolSpeed = 10.0f;

        /// <summary>
        /// 文字从右方开始出现
        /// </summary>
        public bool fromRight = false;

        /// <summary>
        /// 文字开始先静止这么秒，然后才开始滚动
        /// </summary>
        public float startTime = 0.0f;


        private Text m_text; //原始的text
        private Image m_image; //用作Mask的image
        private List<Text> m_subText; //铺满image的text
        private float textLength = 0.0f; //原始文本中文字的长度
        private float widthPoint = 0.0f; //铺满image后text的宽度，也就是尾巴的位置，用于标记

        public void Start()
        {
            if (!Application.isPlaying)
            {
                return;
            }
            m_text = GetComponent<Text>();
            if (m_text == null) return;

            GameObject imageGo = new GameObject();
            imageGo.name = "ImageMask";
            Image image = imageGo.AddComponent<Image>();
            
            m_image = image;

            textLength = m_text.preferredWidth;
            widthPoint = 0.0f;
            float blockWidth = Mathf.Abs(m_text.rectTransform.offsetMax.x - m_text.rectTransform.offsetMin.x);

            //整个跑马灯最少需要铺满子字串，并且多一个子字串拼接用
            float totalWidth = blockWidth + textLength + subTextBlank;            
            //整个跑马灯最少需要两个子字串拼接
            //totalWidth = Mathf.Max(totalWidth, (textLength + subTextBlank * 2.0f));

            m_subText = new List<Text>();

            while (textLength > 0 && widthPoint <= totalWidth)
            {
                Text newText = ResNode.InstantiateRes<Text>(m_text.gameObject);
                GameObject newChild = newText.gameObject;
                newChild.name = "SubText";
                newText.transform.SetParent(imageGo.transform);
                newText.rectTransform.localPosition = new Vector3(widthPoint, 0, 0);
                foreach (Component comp in newChild.GetComponents<Component>())
                {
                    if (comp is URollText)
                    {
                        Component temp = comp;
                        ResNode.DestroyRes(ref temp);
                    }
                }
                m_subText.Add(newText);
                widthPoint += textLength + subTextBlank;
            }

            imageGo.transform.SetParent(m_text.transform);
            image.rectTransform.offsetMin = m_text.rectTransform.offsetMin;
            image.rectTransform.offsetMax = m_text.rectTransform.offsetMax;
            image.rectTransform.localPosition = Vector3.zero;

            Mask mask=imageGo.AddComponent<Mask>();
            mask.showMaskGraphic = false;

            if (fromRight)
            {
                foreach (Text text in m_subText)
                {
                    Vector3 position = text.rectTransform.localPosition;
                    position.x += blockWidth;
                    text.rectTransform.localPosition = position;
                }
            }

            m_text.enabled = false;
        }

        public void Update()
        {
            if (m_subText == null)
            {
                return;
            }
            if (startTime > 0.0f)
            {
                startTime -= Time.deltaTime;
                return;
            }

            float offsetX = Time.deltaTime * roolSpeed;
            foreach (Text text in m_subText)
            {
                Vector3 position = text.rectTransform.localPosition;
                position.x -= offsetX;
                if (position.x + textLength + subTextBlank < 0.0f)
                {
                    position.x += widthPoint;
                }
                text.rectTransform.localPosition = position;
            }
        }

        public void OnDestroy()
        {
            if (m_image != null)
            {
                var temp = m_image.gameObject;
                ResNode.DestroyRes(ref temp);
            }
        }
    }
}