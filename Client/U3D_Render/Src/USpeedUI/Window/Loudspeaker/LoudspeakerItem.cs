/*******************************************************************
** 文件名:	AssetBundleManager_Prefab.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2016/10/25
** 版  本:	1.0
** 描  述:	喇叭消息
** 应  用:  
 *****************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DataCenter;
using UnityEngine;
using UnityEngine.UI;
using Data.ActorPrizeConfig;

namespace USpeedUI.Loudspeaker
{
    public interface ILoudspeakerItem
    {
        // 获取物件类型
        LoudspeakerManager.MsgObjType GetItemType();
        // 设置物件内容
        bool SetItemData(LoudspeakerManager.SLoudspeakerObjectInfo data);
        // 获取坐标组件
        RectTransform GetTransform();
        // 获取物件宽度
        float GetItemWidth();
        // 获取物件高度
        float GetItemHeight();

        // 销毁物件
        void Destroy();
        // 复制物件
        ILoudspeakerItem Clone();
    }

    public class LoudspeakerItemText_Base
    {
        protected LoudspeakerItemText m_obj;
        protected LoudspeakerManager.SLoudspeakerObjectInfo m_itemData;
        protected Color m_defaultColor = Color.white;
        protected Text m_UIText;

        protected LoudspeakerItemText m_prevObj;
        public LoudspeakerItemText PrevObj { get { return m_prevObj; } }
        protected LoudspeakerItemText m_nextObj;
        public LoudspeakerItemText NextObj { get { return m_nextObj; } }

        public virtual bool Init(LoudspeakerItemText obj, Text uiText, LoudspeakerManager.SLoudspeakerObjectInfo data)
        {
            m_obj = obj;
            m_UIText = uiText;
            m_itemData = data;

            if (data.param != null && data.param.ContainsKey("color"))
            {
                string colorParam = data.param["color"];
                int r, g, b;
                LogicDataCenter.chatMessageManager.ParseColorParam(colorParam, out r, out g, out b);
                float factor = 1.0f / 255.0f;
                m_defaultColor = new Color(r * factor, g * factor, b * factor);
            }

            m_UIText.text = data.text;
            m_UIText.color = m_defaultColor;

            return true;
        }

        // 获取坐标组件
        public virtual RectTransform GetTransform()
        {
            return m_UIText.rectTransform;
        }

        // 获取物件宽度
        public virtual float GetItemWidth()
        {
            return m_UIText.preferredWidth;
        }

        // 获取物件高度
        public virtual float GetItemHeight()
        {
            return m_UIText.preferredHeight;
        }

        // 复制物件
        public virtual ILoudspeakerItem Clone()
        {
            GameObject newObj = ResNode.InstantiateRes(m_obj.gameObject);
            LoudspeakerItemText newItem = newObj.GetComponent<LoudspeakerItemText>();
            newItem.SetItemData(m_itemData);

            return newItem;
        }

        public virtual void OnDestroy()
        {
        }

        public virtual void OnUpdate()
        {
        }
    }

    public class LoudspeakerItemText : MonoBehaviour, ILoudspeakerItem
    {
        private LoudspeakerItemText_Base m_subItem;
        public LoudspeakerItemText_Base SubItem { get { return m_subItem; } }

        private bool m_bIsSplit;
        public bool IsSplit { get { return m_bIsSplit; } set { m_bIsSplit = value; } }

        private float m_fClickIntervalTime = 10f;
        public float ClickIntervalTime { get { return m_fClickIntervalTime; } set { m_fClickIntervalTime = value; } }

        // 获取物件类型
        public virtual LoudspeakerManager.MsgObjType GetItemType()
        {
            return LoudspeakerManager.MsgObjType.ObjType_Text;
        }

        // 设置物件内容
        public virtual bool SetItemData(LoudspeakerManager.SLoudspeakerObjectInfo data)
        {
            switch (data.subType)
            {
                case LoudspeakerManager.MsgObjSubType.ObjSubType_Text_Text:
                    m_subItem = new LoudspeakerSubItemText_Text();
                    break;
                case LoudspeakerManager.MsgObjSubType.ObjSubType_Text_Prize:
                    m_subItem = new LoudspeakerSubItemText_ActorPrize();
                    break;
                default:
                    return false;
            }

            Text uiText = GetComponent<Text>();
            return m_subItem.Init(this, uiText, data);
        }

        // 获取坐标组件
        public virtual RectTransform GetTransform()
        {
            return m_subItem.GetTransform();
        }

        // 获取物件宽度
        public virtual float GetItemWidth()
        {
            return m_subItem.GetItemWidth();
        }

        // 获取物件高度
        public virtual float GetItemHeight()
        {
            return m_subItem.GetItemHeight();
        }

        // 销毁物件
        public virtual void Destroy()
        {
            m_subItem.OnDestroy();
            var temp = gameObject;
            ResNode.DestroyRes(ref temp);
        }

        // 复制物件
        public virtual ILoudspeakerItem Clone()
        {
            return m_subItem.Clone();
        }

        public void Update()
        {
            m_subItem.OnUpdate();
        }
    }

    public class LoudspeakerSubItemText_Text : LoudspeakerItemText_Base
    {
        public override bool Init(LoudspeakerItemText obj, Text uiText, LoudspeakerManager.SLoudspeakerObjectInfo data)
        {
            base.Init(obj, uiText, data);
            m_UIText.raycastTarget = false;
            return true;
        }
    }

    public class LoudspeakerSubItemText_ActorPrize : LoudspeakerItemText_Base
    {
        private int m_nPrizeID;

        public override bool Init(LoudspeakerItemText obj, Text uiText, LoudspeakerManager.SLoudspeakerObjectInfo data)
        {
            base.Init(obj, uiText, data);

            if (m_itemData.param == null || !m_itemData.param.ContainsKey("id"))
                return false;

            m_nPrizeID = Convert.ToInt32(m_itemData.param["id"]);

            SSchemeActorPrizeConfig prizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(m_nPrizeID);
            if (prizeConfig != null)
            {
                m_defaultColor = Color.white;
                if (prizeConfig.nPrizeGrade == 1)
                {
                    m_defaultColor = Color.green;
                }
                else if (prizeConfig.nPrizeGrade == 2)
                {
                    m_defaultColor = Color.blue;
                }
                else if (prizeConfig.nPrizeGrade == 3)
                {
                    m_defaultColor = new Color(0.5f, 0f, 0.5f);
                }
                else if (prizeConfig.nPrizeGrade == 4)
                {
                    m_defaultColor = Color.yellow;
                }
                else if (prizeConfig.nPrizeGrade == 5)
                {
                    m_defaultColor = Color.red;
                }
                else if (prizeConfig.nPrizeGrade == 6)
                {
                    m_defaultColor = new Color(1f, 0.647f, 0f);
                }
                m_UIText.color = m_defaultColor;

                m_UIText.text = "[" + prizeConfig.strPrizeName + "]";
            }

            return true;
        }
    }
}
