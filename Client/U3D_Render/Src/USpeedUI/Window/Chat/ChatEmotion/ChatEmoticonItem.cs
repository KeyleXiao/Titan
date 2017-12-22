/*******************************************************************
** 文件名:	ChatEmoticonItem.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2017 - Speed
** 创建人:	昔文博
** 日  期:	2017/12/14
** 版  本:	1.0
** 描  述:	聊天表情子项
** 应  用:  
********************************************************************/
using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using UIWidgets;
using USpeedUI.UEffect;
using USpeedUI.UExtensions;
using Data.ChatEmoticonConfig;
using USpeedUI.UWidgets.UAnimator;

namespace USpeedUI.Chat
{
    public class ChatEmoticonItemBase : MonoBehaviour, IPointerClickHandler
    {
        public UFlipBook FilpBookAnim;

        protected int emotionID;
        protected int serialID;
        protected string emotionKey;
        protected string szCharInput;
        protected string szTipsDesc;

        protected EMChatEmoticonType emotionType;
        protected UEffectParamBase m_lockEffectParam;

        public virtual void SetData(SSchemeChatEmoticonData _sSchemeData)
        {
            emotionID = _sSchemeData.nEmoticonID;
            serialID = _sSchemeData.nEmoticonSerialID;
            emotionType = _sSchemeData.eEmoticonType;
            emotionKey = _sSchemeData.szEmoticonKey;
            szCharInput = _sSchemeData.szCharInput;
            szTipsDesc = _sSchemeData.szTipDesc;

            FilpBookAnim = this.GetComponent<UFlipBook>();

            if (FilpBookAnim != null)
                FilpBookAnim.Init(emotionKey);
        }

        public virtual void SetLock(bool _bLock)
        {
            if (_bLock)
            {
                if (m_lockEffectParam == null)
                {
                    m_lockEffectParam = new UEffectMaterialParam(UEffectMaterialType.EMT_Gray, this.GetComponent<RawImage>());
                    UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectMaterial, ref m_lockEffectParam);
                }
            }
            else
            {
                if (m_lockEffectParam != null)
                {
                    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectMaterial, ref m_lockEffectParam);
                    m_lockEffectParam = null;
                }
            }
        }

        public virtual void OnPointerClick(PointerEventData eventData)
        {
            UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_CHATBOX, WndMsgID.WND_MSG_CHATBOX_CLOSEEMOTICONPANEL, null);
        }
    }

    public class ChatEmoticonItem_Default : ChatEmoticonItemBase
    {
        public override void SetData(SSchemeChatEmoticonData _sSchemeData)
        {
            base.SetData(_sSchemeData);

            UTooltipTrigger tipTrigger = this.gameObject.GetComponent<UTooltipTrigger>();
            if(tipTrigger == null)
                tipTrigger = this.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_ChatEmotion);

            if (tipTrigger)
            {
                string tip = "<color='#20c53b'>" + szCharInput + "</color>" + "    " + szTipsDesc;
                tipTrigger.SetText("bodyTxt", tip);
            }
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            base.OnPointerClick(eventData);

            SChatMessageObjectInfo emotionObj;
            emotionObj.type = ChatItemType.ObjType_Image;
            emotionObj.subType = ChatItemSubType.ObjSubType_Image_Emotion;
            emotionObj.text = "";
            emotionObj.param = new Dictionary<String, String>();
            emotionObj.param.Add("id", emotionID.ToString());
            emotionObj.param.Add("type", ((int)emotionType).ToString());
            emotionObj.param.Add("serialid", serialID.ToString());
            emotionObj.param.Add("animkey", emotionKey);
            ChatBoxAddInputFieldItemData uiData = new ChatBoxAddInputFieldItemData();
            uiData.msgID = (int)WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM;
            uiData.objInfo = emotionObj;
            uiData.bActiveInputField = true;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM, uiData);
        }
    }

    public class ChatEmoticonItem_Large : ChatEmoticonItemBase
    {
        public override void SetData(SSchemeChatEmoticonData _sSchemeData)
        {
            base.SetData(_sSchemeData);

            UTooltipTrigger tipTrigger = this.gameObject.GetComponent<UTooltipTrigger>();
            if (tipTrigger == null)
                tipTrigger = this.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_ChatEmotion);

            if (tipTrigger)
            {
                tipTrigger.SetText("bodyTxt", szTipsDesc);
            }
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            base.OnPointerClick(eventData);

            SChatMessageObjectInfo emotionObj;
            emotionObj.type = ChatItemType.ObjType_Image;
            emotionObj.subType = ChatItemSubType.ObjSubType_Image_Emotion;
            emotionObj.text = "";
            emotionObj.param = new Dictionary<String, String>();
            emotionObj.param.Add("id", emotionID.ToString());
            emotionObj.param.Add("type", ((int)emotionType).ToString());
            emotionObj.param.Add("serialid", serialID.ToString());
            emotionObj.param.Add("animkey", emotionKey);
            ChatBoxAddInputFieldItemData uiData = new ChatBoxAddInputFieldItemData();
            uiData.msgID = (int)WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM;
            uiData.objInfo = emotionObj;
            uiData.bActiveInputField = false;
            uiData.bAutoSend = true;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM, uiData);
        }
    }

    public class ChatEmoticonItem_Skill : ChatEmoticonItemBase
    {
        private int emoticonSkillID;
        public override void SetData(SSchemeChatEmoticonData _sSchemeData)
        {
            base.SetData(_sSchemeData);

            emoticonSkillID = _sSchemeData.nEmoticonSkillID;

            UTooltipTrigger tipTrigger = this.gameObject.GetComponent<UTooltipTrigger>();
            if (tipTrigger == null)
                tipTrigger = this.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_ChatEmotion);

            if (tipTrigger)
            {
                tipTrigger.SetText("bodyTxt", szTipsDesc);
            }
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            if (emoticonSkillID < 0)
                return;

            base.OnPointerClick(eventData);

            if (U3D_Render.EntityView.isNullOrEmpty(EntityFactory.MainHeroView))
                return;

            SkillCtrl sc = EntityFactory.MainHeroView.SkillCtrl;
            if(sc != null)
            {
                sc.UseEmoticonSkill(emoticonSkillID);
            }
        }
    }
}
