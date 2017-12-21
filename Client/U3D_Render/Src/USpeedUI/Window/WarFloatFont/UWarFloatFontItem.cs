using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using DG.Tweening;
using USpeedUI.UExtensions;
using USpeedUI.UWidgets;
using USpeedUI.UEffect;

namespace USpeedUI.WarFloatFont
{
    public class UWarFloatFontItem : UCustomUIBehaviour
    {
        public RectTransform fontFrame;
        public Text t_txt;
        public Image icon;
        public UGradient gradient;
        public Sprite FatalIcon;
        public Sprite coinIcon;

        //是否激活
        private bool m_enable = false;
        //飘字内容
        private string m_FontVal = null;
        //飘字类型
        private WarFloatFontType m_type = WarFloatFontType.Invalid;
        //动画时间
        private float liveTime = 0.0f;
        //计时器
        private float timer = 0.0f;
        private GameObject m_Entity = null;
        private CreatureProperty m_Property = null;
        private FloatFontAnimData m_Anim = null;
        //动画序列
        private Sequence m_Sequence = null;

        //画布设计的分辨率
        private const float resolutionWidth = 1920;
        private const float resolutionHeight = 1080;

        private UViewWarFloatFont floatFontView;
        private WarFloatFontAnim animManager;

        public void SetFloatFontView(UViewWarFloatFont view)
        {
            floatFontView = view;
        }

        public void SetAnimManager(WarFloatFontAnim anim)
        {
            if (anim != null)
                animManager = anim;
        }

        public void Clear()
        {
            fontFrame.localScale = Vector3.one;
            fontFrame.anchoredPosition = Vector2.zero;
            m_FontVal = null;
            liveTime = 0;
            timer = 0;
            m_Entity = null;
            m_Property = null;
            m_enable = false;
            m_Sequence = null;
            m_Anim = null;
            m_type = WarFloatFontType.Invalid;
        }

        //设置飘字节点对应实体
        public void ChangeHostEntity(GameObject entity)
        {
            m_Entity = entity;
            m_Property = entity.GetComponent<CreatureProperty>();
        }

        //设置飘字类型
        public bool ChangeType(WarFloatFontType type)
        {
            m_type = type;
            FloatFontAnimData anim = null;
            if(null == animManager)
                animManager = GetRectTransform.parent.GetComponent<WarFloatFontAnim>();
            if (null != animManager)
                anim = animManager.GetAnimData(type);
            if (null != anim)
            {
                m_Anim = anim;
                return true;
            }
            return false;
        }

        public void SetText(string text)
        {
            if (string.IsNullOrEmpty(text))
            {
                return;
            }
            m_FontVal = text;
        }

        public void SetEnable(bool enable)
        {
            if (m_enable != enable)
            {
                m_enable = enable;
            }
            UpdateVisible();
        }

        public bool GetEnable()
        {
            return m_enable;
        }

        public bool UpdatePos()
        {
            if (null != m_Entity)
            {
                Vector3 entityPos = m_Entity.transform.position;
                float creatureHeight = 0.0f;
                if (m_Property)
                {
                    creatureHeight = m_Property.CreatureHeightInMeters * m_Property.transform.localScale.y;
                    if (creatureHeight > 4)
                        creatureHeight = 4;
                    entityPos.y += creatureHeight;
                }
                if (CheckIsOnCameraView(entityPos))
                {
                    SetCanvasLocalPosition(GenratePos(m_type, entityPos));
                    return true;
                }
            }
            return false;
        }

        //激活节点
        public void ActivateFont()
        {
            liveTime = m_Anim.animTime;
            ChooseFontStyle(m_type, m_FontVal);
            SetEnable(true);
        }

        //根据类型选择飘字样式及动画效果
        private void ChooseFontStyle(WarFloatFontType type, string val)
        {
            if (null == t_txt || null == gradient || null == icon || null == m_Anim)
                return;

            t_txt.color = Color.white;
            t_txt.fontSize = m_Anim.fontSize;
            gradient.SetGradientColorKey(m_Anim.topColor, m_Anim.bottomColor);

            //gradient.vertex1 = m_Anim.topColor;
            //gradient.vertex2 = m_Anim.bottomColor;
            icon.gameObject.SetActive(false);

            Vector3 temp = t_txt.rectTransform.anchoredPosition;
            switch (type)
            {
                case WarFloatFontType.Fatal:
                    {
                        t_txt.text = val;
                        if (null != FatalIcon)
                        {
                            int size = m_Anim.fontSize - 4;
                            icon.sprite = FatalIcon;
                            icon.rectTransform.sizeDelta = new Vector2(size, size);
                            icon.gameObject.SetActive(true);
                        }
                    }
                    break;
                case WarFloatFontType.TrueDamage:
                case WarFloatFontType.MagicAttack:
                case WarFloatFontType.PhysicalAttack:
                    t_txt.text = val;
                    break;
                case WarFloatFontType.Cure:
                        t_txt.text = "+" + val;
                    break;
                case WarFloatFontType.Coin:
                    {
                        t_txt.text = "+" + val;
                        if (null != coinIcon)
                        {
                            int size = m_Anim.fontSize - 4;
                            icon.sprite = coinIcon;
                            icon.rectTransform.sizeDelta = new Vector2(size, size);
                            icon.gameObject.SetActive(true);
                        }
                    }
                    break;
                case WarFloatFontType.LevelUp:
                    t_txt.text = "升 级";
                    break;
                default:
                    break;
            }
            fontFrame.localScale = new Vector3(m_Anim.expandMinPercent, m_Anim.expandMinPercent, m_Anim.expandMinPercent);
            m_Sequence = DOTween.Sequence();
            m_Sequence.Append(fontFrame.DOScale(m_Anim.expandMaxPercent, m_Anim.expandTime))
                                .Insert(0, fontFrame.DOJumpAnchorPos(new Vector2(fontFrame.anchoredPosition.x + m_Anim.xOffset, fontFrame.anchoredPosition.y), m_Anim.jumpPower, 1, m_Anim.disappearTime).SetDelay(m_Anim.expandTime + m_Anim.stagnationTime))
                                .Insert(0, fontFrame.DOAnchorPosY(fontFrame.anchoredPosition.y + m_Anim.yOffset, m_Anim.disappearTime).SetDelay(m_Anim.expandTime + m_Anim.stagnationTime))
                                .Insert(0, t_txt.DOFade(0, m_Anim.disappearTime).SetDelay(m_Anim.expandTime + m_Anim.stagnationTime))
                                .Insert(0, fontFrame.DOScale(0, m_Anim.disappearTime).SetDelay(m_Anim.expandTime + m_Anim.stagnationTime))
                                .OnComplete(SequenceComplete);
        }

        private void SequenceComplete()
        {
            if(m_Sequence != null)
                m_Sequence.Kill();
        }

        //动画播放完，通知窗口将本节点移出活动列表
        public void Update()
        {
            if (m_enable)
            {
                timer += Time.deltaTime;
                if (timer >= liveTime || !UpdatePos())
                {
                    floatFontView.DisableFloatFontItem(this);
                }
            }
        }

        private void UpdateVisible()
        {
            this.gameObject.SetActive(m_enable);
        }

        /// <summary>
        /// 计算屏幕位置，由世界坐标转换
        /// </summary>
        /// <param name="type">飘字类型</param>
        /// <param name="entityPos">实体世界位置</param>
        /// <param name="creatureHeight">实体高度</param>
        /// <returns></returns>
        private Vector2 GenratePos(WarFloatFontType type, Vector3 entityPos)
        {
            Vector3 pixelPos = Vector3.zero;

            pixelPos = Initialize.mainCam.WorldToScreenPoint(entityPos);
            Vector2 UIlocalPos = Vector2.zero;
            RectTransformUtility.ScreenPointToLocalPointInRectangle(this.GetTransform.parent as RectTransform, pixelPos, UISystem.Instance.GetCamera(), out UIlocalPos);

            if (m_Anim.bRandomStartPos)
                UIlocalPos.x += UnityEngine.Random.Range(-Mathf.Abs(m_Anim.startXOffset), Mathf.Abs(m_Anim.startXOffset));
            else
                UIlocalPos.x += m_Anim.startXOffset;

            UIlocalPos.y += m_Anim.startYOffset;

            return UIlocalPos;
        }

        //设置飘字在屏幕上的位置
        private void SetCanvasLocalPosition(Vector2 pixelPos)
        {
            GetRectTransform.anchoredPosition = new Vector2(pixelPos.x, pixelPos.y);
        }

        //判断位置是否在摄像机视口内
        private bool CheckIsOnCameraView(Vector3 pixelpos)
        {
            Vector3 ViewPos = Initialize.mainCam.WorldToViewportPoint(pixelpos);
            if (ViewPos.x < 0 || ViewPos.x > 1 || ViewPos.y < 0 || ViewPos.y > 1 || ViewPos.z < 0)
            {
                return false;
            }
            return true;
        }
    }
}

