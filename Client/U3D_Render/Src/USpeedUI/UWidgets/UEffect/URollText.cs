/*******************************************************************
** �ļ���:	URollText.cs
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - Speed
** ������:	����
** ��  ��:	2016/9/18
** ��  ��:	1.0
** ��  ��:	UI��Text�������Ч��
** Ӧ  ��:  ��Text�����Ӵ˽ű���������������

**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
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
        /// ��������֮��ļ��
        /// </summary>
        public float subTextBlank = 10.0f;

        /// <summary>
        /// �����ٶ�
        /// </summary>
        public float roolSpeed = 10.0f;

        /// <summary>
        /// ���ִ��ҷ���ʼ����
        /// </summary>
        public bool fromRight = false;

        /// <summary>
        /// ���ֿ�ʼ�Ⱦ�ֹ��ô�룬Ȼ��ſ�ʼ����
        /// </summary>
        public float startTime = 0.0f;


        private Text m_text; //ԭʼ��text
        private Image m_image; //����Mask��image
        private List<Text> m_subText; //����image��text
        private float textLength = 0.0f; //ԭʼ�ı������ֵĳ���
        private float widthPoint = 0.0f; //����image��text�Ŀ�ȣ�Ҳ����β�͵�λ�ã����ڱ��

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

            //���������������Ҫ�������ִ������Ҷ�һ�����ִ�ƴ����
            float totalWidth = blockWidth + textLength + subTextBlank;            
            //���������������Ҫ�������ִ�ƴ��
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