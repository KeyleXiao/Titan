/*******************************************************************
** 文件名:	WarBottonCenterWndView.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-6-3
** 版  本:	1.0
** 描  述:	战场底部技能物品主界面
		
********************************************************************/

using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.EventSystems;
using UnityEngine.Events;
using System;
using GameLogic;
using DataCenter;
using USpeedUI.WarBottonCenterPart;
using UIWidgets;
using USpeedUI.UExtensions;
using System.Runtime.InteropServices;
using ASpeedGame.Data.GameMobaSchemes;
using U3D_Render.Common;
using USpeedUI.UEffect;
using ASpeedGame.Data.HeroTalentConfig;
using DG.Tweening;
using U3D_Render;

namespace USpeedUI
{
    public enum EBarObjMoveType
    {
        EBOMT_LEFT,
        EBOMT_RIGHT,
    }

    public class PropertyItem : MonoBehaviour, IPointerEnterHandler
    {
        public UTooltipTrigger triger;
        private string m_sDesc;

        public void SetToolTipDesc(string sDesc)
        {
            if (string.IsNullOrEmpty(sDesc))
            {
                return;
            }

            m_sDesc = sDesc;
        }

        public void OnPointerEnter(PointerEventData eventData)
        {
            if (triger == null)
            {
                return;
            }

            if (string.IsNullOrEmpty(m_sDesc))
            {
                return;
            }

            string sTemp = m_sDesc;
            UBBProperty.GetInstance().getRatio(sTemp);
            UBB.toHtml(ref sTemp);
            triger.SetText(UTooltipParamName.BodyText, sTemp);
        }
    }

    public class SkillLevelGroup : MonoBehaviour
    {
        public GameObject[] skillLightArray;
    }

    #region 物品part
    public class WarCenterGoodPart : MonoBehaviour
    {
        public WarCenterGoodButtonItemDrag[] goodItemArray;
        public WarCenterGoodButtonItemDrop[] goodDropItemArray;

        public void SetGoodsSlotShortcutKey(int nSlotID, string strShortcut)
        {
            if (IsOutOfGoodArray(nSlotID))
            {
                return;
            }

            goodDropItemArray[nSlotID].SetGoodsSlotShortcutKey(strShortcut);
        }

        public void AddGoodsButtonToSlot(cmd_war_persongoodsinfo data, SSchemeMobaGoods goodData)
        {
            int nSlotID = data.nSlotIndex;
            if (IsOutOfGoodArray(nSlotID))
            {
                return;
            }

            goodItemArray[nSlotID].AddGoodsButtonToSlot(data, goodData);
        }

        public void RemoveGoodsButtonFromSlot(int nSlotID)
        {
            if (IsOutOfGoodArray(nSlotID))
            {
                return;
            }

            goodItemArray[nSlotID].RemoveGoodsButtonFromSlot();
        }

        public void PlayPurchasedGoodsUseEffect(int nSlotID, int nBeginTime, int nEndTime)
        {
            if (IsOutOfGoodArray(nSlotID))
            {
                return;
            }

            goodItemArray[nSlotID].PlayPurchasedGoodsUseEffect(nBeginTime, nEndTime);
        }

        public void SlotCanDragDrop(int nSlotID, bool bCanDragDrop)
        {
            if (IsOutOfGoodArray(nSlotID))
            {
                return;
            }

            goodItemArray[nSlotID].SlotCanDragDrop(bCanDragDrop);
        }

        public bool IsOutOfGoodArray(int nSlotID)
        {
            if (nSlotID < 0 || nSlotID >= goodItemArray.Length || goodItemArray[nSlotID] == null)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public bool IsOutOfGoodDropArray(int nSlotID)
        {
            if (nSlotID < 0 || nSlotID >= goodDropItemArray.Length || goodDropItemArray[nSlotID] == null)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public void ResetView()
        {
            if (goodItemArray != null)
            {
                foreach (WarCenterGoodButtonItemDrag item in goodItemArray)
                {
                    if (item != null)
                    {
                        item.ResetGoodSlotView();
                    }
                }
            }

            if (goodDropItemArray != null)
            {
                foreach (WarCenterGoodButtonItemDrop item in goodDropItemArray)
                {
                    if (item != null)
                    {
                        item.shortText.text = "";
                    }
                }
            }
        }
    }

    public class WarCenterGoodButtonItemDrop : MonoBehaviour, IDropHandler
    {
        public GameObject dynamicSlotObject;
        public Text shortText;
        public WarCenterGoodPart warCenterGoodPart;

        public void OnDrop(PointerEventData eventData)
        {
            if (dynamicSlotObject == null)
            {
                return;
            }

            GameObject dragGoodObject = eventData.pointerDrag;
            if (dragGoodObject == null)
            {
                return;
            }

            var goodItemDrag = dragGoodObject.GetComponent<WarCenterGoodButtonItemDrag>();
            var goodItemDrop = dynamicSlotObject.GetComponent<WarCenterGoodButtonItemDrag>();

            var dragRect = goodItemDrag.staticSlotObject.GetComponent<RectTransform>().anchoredPosition;
            var dropRect = this.gameObject.GetComponent<RectTransform>().anchoredPosition;
            var tempStaticSlotObject = goodItemDrag.staticSlotObject;

            int nDragSlotId = goodItemDrag.nSlotId;
            int nDropSlotId = goodItemDrop.nSlotId;

            goodItemDrag.nSlotId = goodItemDrop.nSlotId;
            dragGoodObject.GetComponent<RectTransform>().anchoredPosition = dropRect;
            goodItemDrag.bDropInSlot = true;
            goodItemDrag.staticSlotObject.GetComponent<WarCenterGoodButtonItemDrop>().dynamicSlotObject = dynamicSlotObject;
            goodItemDrag.staticSlotObject = dynamicSlotObject.GetComponent<WarCenterGoodButtonItemDrag>().staticSlotObject;
            warCenterGoodPart.goodItemArray[nDragSlotId] = goodItemDrop;

            goodItemDrop.nSlotId = nDragSlotId;
            dynamicSlotObject.GetComponent<RectTransform>().anchoredPosition = dragRect;
            dynamicSlotObject.GetComponent<WarCenterGoodButtonItemDrag>().staticSlotObject = tempStaticSlotObject;
            dynamicSlotObject = dragGoodObject;
            warCenterGoodPart.goodItemArray[nDropSlotId] = goodItemDrag;

            onPurchasedSlotDrop(goodItemDrop.nSlotId, goodItemDrag.nSlotId);
        }

        public void onPurchasedSlotDrop(int nOrigDragSlotID, int nTargetDropSlotID)
        {
            cmd_war_personexchangeslot data = new cmd_war_personexchangeslot();
            data.nSrcSlotIndex = nOrigDragSlotID;
            data.nTargetSlotIndex = nTargetDropSlotID;

            IntPtrHelper helper = new IntPtrHelper();
            IntPtr ptr = helper.toPtr<cmd_war_personexchangeslot>(ref data);
            int len = Marshal.SizeOf(data);

            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_WAR_EXCHANGE_SLOT, len, "", ptr, len);
        }

        public void SetGoodsSlotShortcutKey(string strShortcut)
        {
            shortText.text = strShortcut;
        }
    }

    public class WarCenterGoodButtonItemDrag : TimeCountDown, IBeginDragHandler, IEndDragHandler, IDragHandler, IPointerEnterHandler
    {
        public int nSlotId;
        public int nGoodId;
        public Text useTime;
        public Image goodIcon;
        public bool bCanDrag;
        public bool bDropInSlot;
        private RectTransform draggingTransform;
        public GameObject warCenterView;
        public Image[] dropRecastArray;
        public GameObject staticSlotObject;
        public Image timeCountImage;
        private string m_sGoodDesc = null;
        private string m_sGoodName = null;
        private int m_sGoodCost = 0;
        private float nEndTime_ = 0;//-


        //   public Text timeCountText;
        public virtual void Start()
        {
            draggingTransform = warCenterView.transform as RectTransform;
        }

        public void OnBeginDrag(PointerEventData eventData)
        {
            if (!bCanDrag || !goodIcon.gameObject.activeSelf)
            {
                return;
            }

            this.transform.SetAsLastSibling();
            var rt = this.gameObject.GetComponent<RectTransform>();
            Vector3 globalMousePos;
            if (RectTransformUtility.ScreenPointToWorldPointInRectangle(draggingTransform, eventData.position, eventData.pressEventCamera, out globalMousePos))
            {
                rt.SetPosition(globalMousePos);
                rt.rotation = draggingTransform.rotation;
            }
            for (int i = 0; i < dropRecastArray.Length; i++)
            {
                dropRecastArray[i].raycastTarget = true;
            }

        }

        public void OnDrag(PointerEventData eventData)
        {
            if (!bCanDrag || !goodIcon.gameObject.activeSelf)
            {
                return;
            }

            var rt = this.gameObject.GetComponent<RectTransform>();
            Vector3 globalMousePos;
            if (RectTransformUtility.ScreenPointToWorldPointInRectangle(draggingTransform, eventData.position, eventData.pressEventCamera, out globalMousePos))
            {
                rt.SetPosition(globalMousePos);
                rt.rotation = draggingTransform.rotation;
            }
        }

        public void OnEndDrag(PointerEventData eventData)
        {
            if (!bCanDrag || !goodIcon.gameObject.activeSelf)
            {
                return;
            }

            for (int i = 0; i < dropRecastArray.Length; i++)
            {
                dropRecastArray[i].raycastTarget = false;
            }

            if (!bDropInSlot)
            {
                this.GetComponent<RectTransform>().anchoredPosition = this.staticSlotObject.GetComponent<RectTransform>().anchoredPosition;
            }
            else
            {
                bDropInSlot = false;
            }
        }

        public void AddGoodsButtonToSlot(cmd_war_persongoodsinfo data, SSchemeMobaGoods goodData)
        {
            if (goodIcon == null)
            {
                return;
            }
            SetCanUseTimes(data.nCanUseTimes, 0);
            nGoodId = data.nGoodID;
            this.goodIcon.gameObject.SetActive(true);
            goodIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MobaGoods, WndID.WND_ID_WAR_BOTTOMCENTERVIEW, 1, goodData.GoodsIconID);

            m_sGoodDesc = goodData.GoodsSlotDes;
            m_sGoodName = goodData.GoodsName;
            m_sGoodCost = goodData.GoodsSoldOutCost;
        }

        public void RemoveGoodsButtonFromSlot()
        {
            ResetGoodSlotView();
        }

        public void ResetGoodSlotView()
        {
            if (useTime == null || dropRecastArray == null)
            {
                return;
            }

            useTime.text = "";
            nGoodId = 0;
            this.goodIcon.gameObject.SetActive(false);

            for (int i = 0; i < dropRecastArray.Length; i++)
            {
                dropRecastArray[i].raycastTarget = false;
            }

            bDropInSlot = false;

            CountDownCompleteCallBack();
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="nBeginTime"></param>
        /// <param name="nEndTime"></param>
        public void PlayPurchasedGoodsUseEffect(int nBeginTime, int nEndTime)
        {
            if (timeCountImage == null)
            {
                return;
            }
            //delta
            int nEffectTime = nEndTime - nBeginTime;

            nEndTime_ = (float)nEndTime / 1000;//-

            if (nEffectTime == 0)
            {
                CountDownCompleteCallBack();
                return;
            }

            CountDownCompleteCallBack();
            this.StartCountDown(((float)nEffectTime / 1000), 0.1f, 0.1f);

            this.timeCountImage.gameObject.SetActive(true);
        }

        protected override void CountDownRepeatCallBack()
        {
            base.CountDownRepeatCallBack();
            timeCountImage.fillAmount = (fCurTime / nEndTime_);
            //timeCountImage.fillAmount = (fCurTime / this.fOriginalTime);
        }

        protected override void CountDownCompleteCallBack()
        {
            base.CountDownCompleteCallBack();

            this.timeCountImage.fillAmount = 1.0f;
            this.timeCountImage.gameObject.SetActive(false);
        }

        public void SlotCanDragDrop(bool bCanDragDrop)
        {
            this.bCanDrag = bCanDragDrop;
        }

        public void SetCanUseTimes(int nCanUseTime, int nDisplayMaxTime)
        {
            if (useTime == null)
            {
                return;
            }

            if (nCanUseTime > nDisplayMaxTime)
            {
                useTime.text = nCanUseTime.ToString();
            }
            else
            {
                useTime.text = "";
            }
        }

        public void OnPointerEnter(PointerEventData eventData)
        {
            if (string.IsNullOrEmpty(m_sGoodName) || string.IsNullOrEmpty(m_sGoodDesc))
            {
                return;
            }

            UTooltipTrigger buffTip = goodIcon.GetComponent<UTooltipTrigger>();
            if (buffTip == null)
            {
                buffTip = gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
                buffTip.backgroundTint = Color.grey;
            }

            string sGoodName = m_sGoodName;
            string sGoodsDes = m_sGoodDesc;
            UBBProperty.GetInstance().getRatio(sGoodsDes);
            UBB.toHtml(ref sGoodName, UBB_FORMAT_TYPE.UGUI);
            UBB.toHtml(ref sGoodsDes, UBB_FORMAT_TYPE.UGUI);
            buffTip.SetText(UTooltipParamName.BodyText, sGoodName + "\n" + "<color=#50E9EF><size=18>" + "价格 :  " + m_sGoodCost + "</size></color> " + "\n\n" + sGoodsDes);
        }
    }
    #endregion

    #region 技能part
    public class WarCenterSkillPart : MonoBehaviour
    {
        public WarCenterSkillButtonItem[] skillItemArray;//技能button对象数组

        public void Init()
        {
            if(skillItemArray == null)
            {
                return;
            }

            foreach(var item in skillItemArray)
            {
                item.Init();
            }
        }

        public void SetSpellSlot(int nSlotID, int nSlotType, int nSlotIconID, string strSlotTipsDesc, int nSlotLevel, bool bSpellUseFlag)
        {
            if (IsOutOfSkillArray(nSlotID))
            {
                return;
            }

            if (nSlotID == 4 || nSlotID == 3 || nSlotID == 5 || nSlotID == 6 || nSlotID == 7)
            {
                nSlotLevel = 1;
            }

            this.skillItemArray[nSlotID].SetSpellSlot(nSlotID, nSlotType, nSlotIconID, strSlotTipsDesc, nSlotLevel, bSpellUseFlag);

            // 激活大招特效
            if (nSlotID == 2 && nSlotLevel == 0)
            {
                UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_UWarBottomCenter_ActiveSkill_Large, _tfAttachParent: skillItemArray[nSlotID].transform);
                UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);
            }
        }

        public void SetSkillBtnEffectClear(int nSlotID)
        {
            if (IsOutOfSkillArray(nSlotID))
            {
                return;
            }

            this.skillItemArray[nSlotID].SetSkillBtnEffectClear();
        }

        public void SetSkillBtnEffect(int nSlotID, int nEffectType, int beginTime, int endTime)
        {
            if (IsOutOfSkillArray(nSlotID))
            {
                return;
            }

            this.skillItemArray[nSlotID].SetSkillBtnEffect(nEffectType, beginTime, endTime);
        }

        public void SetSkillFlagEffect(int nSlotID, int nFlag, int nFreezeLeftTime, int nLeftTime, int nTotalTime)
        {
            if (IsOutOfSkillArray(nSlotID))
            {
                return;
            }

            this.skillItemArray[nSlotID].SetSkillFlagEffect(nFlag, nFreezeLeftTime, nLeftTime, nTotalTime);
        }

        public void SetSkillShortcutKey(int nSlotID, string keyCode)
        {
            if (IsOutOfSkillArray(nSlotID))
            {
                return;
            }

            this.skillItemArray[nSlotID].SetSkillShortcutKey(keyCode);
        }

        public void SetSkillFlagEffectOver(int nSlotID, int nFlag)
        {
            if (IsOutOfSkillArray(nSlotID))
            {
                return;
            }

            this.skillItemArray[nSlotID].SetSkillFlagEffectOver(nFlag);
        }

        public bool IsOutOfSkillArray(int nSlotID)
        {
            if (nSlotID < 0 || nSlotID >= skillItemArray.Length || skillItemArray[nSlotID] == null)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public void SetSlotSkillUse(int nSlotID, bool bUse)
        {
            if (IsOutOfSkillArray(nSlotID))
            {
                return;
            }

            this.skillItemArray[nSlotID].SetSlotSkillUse(bUse);
        }

        public bool GetSlotSkillUse(int nSlotID)
        {
            if (IsOutOfSkillArray(nSlotID))
            {
                return false;
            }

            return this.skillItemArray[nSlotID].GetSlotSkillUse();
        }

        public void SetTalentLearnActive(int nTalentId, int nHeroId)
        {
            SSchemeHeroTalentConfig config = HeroTalentConfigPage.Instance.GetHeroTalentConfig(nHeroId, nTalentId);
            if (config == null)
            {
                return;
            }

            int nSlotId = config.nSlotId;
            if (IsOutOfSkillArray(nSlotId))
            {
                return;
            }

            this.skillItemArray[nSlotId].SetTalentLearnActive(config.sTalentName, config.sTalenDescInWar);
        }

        public void ResetView()
        {
            if (skillItemArray != null)
            {
                foreach (WarCenterSkillButtonItem item in skillItemArray)
                {
                    item.ResetView();
                }
            }
        }
    }

    public class WarCenterSkillButtonItem : TimeCountDown, IPointerEnterHandler, IPointerExitHandler
    {
        public class ActiveTalentDescItemInfo
        {
            public int nTalentIndex = 0;
            public string sTalentDesc = null;
            public string sTalentName = null;
        }

        public Image skilImage;
        public bool bCanUse;
        public Image timeCount;
        public Text KeyText;

        private int nMaxFlagTime = 0;//-充能最大时间
        private bool bEffectNomalState = false;   //-是否处于使用CD--

        private int nLeftFlagTime = 0;
        private bool bEffectFlagState = false;    //是否处于充能
        private int nSkillFlag = 0;
        private int m_nActiveTalentCount = 0;
        private const int m_MaxActiveTalentCount = 5;
        private String SkillTooltip_TalentParamName = "TalentName";
        private String SkillTooltip_SkillParamDesc = "TalentDesc";
        private String SkillTooltip_SkillParamItem = "HeroTalentItem";
        public UTooltipTrigger SkilltipTrigger;
        private string m_sOriginalDesc = null;
        private List<ActiveTalentDescItemInfo> m_listActiveTalent = new List<ActiveTalentDescItemInfo>();
        private int m_nSlotID = 0;
        private Text m_SkillFlagText;
        private Color m_SkinFlagBGColor;
        private GameObject m_TimeCountWordObj;
        private Color m_SkinBGColor;

        private bool m_bInit = false;

        public void Init()
        {
            if(m_bInit)
            {
                return;
            }
            
            InitSkilltipTrigger();
            if (skilImage)
                skilImage.gameObject.SetActive(false);

            if (KeyText)
                KeyText.gameObject.SetActive(false);

            m_SkillFlagText = this.gameObject.transform.FindChild("UseCount").GetComponent<Text>();
            m_SkillFlagText.gameObject.SetActive(false);
            m_SkillFlagText.text = "0";
            m_SkinFlagBGColor = timeCount.gameObject.GetComponent<Image>().color;
            m_SkinBGColor = skilImage.gameObject.GetComponent<Image>().color;
            m_TimeCountWordObj = timeCount.transform.FindChild("Text").gameObject;

            m_bInit = true;
        }

        private void InitSkilltipTrigger()
        {
            for (int i = 1; i <= m_MaxActiveTalentCount; i++)
            {
                SkilltipTrigger.TurnSectionOff(SkillTooltip_SkillParamItem + i);
            }

            SkilltipTrigger.enabled = false;
        }

        public void SetSpellSlotSprite(int nSlotID, int nSlotType, Sprite skillSprite, string strSlotTipsDesc, int nSlotLevel, bool bSpellUseFlag)
        {
            if (skilImage == null)
            {
                return;
            }

            m_nSlotID = nSlotID;
            skilImage.gameObject.SetActive(true);
            if (skillSprite != null)
            {
                skilImage.sprite = skillSprite;
                this.gameObject.SetActive(true);
                KeyText.gameObject.SetActive(true);
            }

            bSpellUseFlag = nSlotLevel > 0 ? true : false;//目前并不知道这个flag的定义
            SetSlotSkillUse(bSpellUseFlag);
            m_sOriginalDesc = strSlotTipsDesc;
            if (nSlotLevel > 0 && nSlotLevel < 6)
            {
                var groupSkillLevel = gameObject.transform.FindChild("SkillLevel");
                if (groupSkillLevel)
                {
                    groupSkillLevel.GetComponent<SkillLevelGroup>().skillLightArray[nSlotLevel - 1].gameObject.SetActive(true);
                }
            }

            SkilltipTrigger.enabled = true;
        }

        public void SetSpellSlot(int nSlotID, int nSlotType, int nSlotIconID, string strSlotTipsDesc, int nSlotLevel, bool bSpellUseFlag)
        {
            if(m_bInit == false)
            {
                Init();
            }

            Debug.LogWarning("SetSpellSlot::nSlotID=" + nSlotID);
            if (skilImage == null)
            {
                Debug.LogWarning("SetSpellSlot::skilImage == null");
                return;
            }

            if (nSlotIconID <= 0)
            {
                Debug.LogWarning("SetSpellSlot::nSlotIconID <= 0");
                return;
            }

            m_nSlotID = nSlotID;
            skilImage.gameObject.SetActive(true);
            Sprite skillSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_WAR_BOTTOMCENTERVIEW, 1, nSlotIconID);
            if (skillSprite != null)
            {
                skilImage.sprite = skillSprite;
                this.gameObject.SetActive(true);
                KeyText.gameObject.SetActive(true);
            }

            bSpellUseFlag = nSlotLevel > 0 ? true : false;//目前并不知道这个flag的定义
            SetSlotSkillUse(bSpellUseFlag);
            m_sOriginalDesc = strSlotTipsDesc;
            if (nSlotLevel > 0 && nSlotLevel < 6)
            {
                var groupSkillLevel = gameObject.transform.FindChild("SkillLevel");
                if (groupSkillLevel)
                {
                    groupSkillLevel.GetComponent<SkillLevelGroup>().skillLightArray[nSlotLevel - 1].gameObject.SetActive(true);
                }
            }

            SkilltipTrigger.enabled = true;
        }

        public void SetTalentLearnActive(string sTalentName, string sTalentDesc)
        {
            m_nActiveTalentCount++;
            if (m_nActiveTalentCount > m_MaxActiveTalentCount)
            {
                return;
            }

            ActiveTalentDescItemInfo info = new ActiveTalentDescItemInfo();
            info.nTalentIndex = m_nActiveTalentCount;
            info.sTalentName = sTalentName;
            info.sTalentDesc = sTalentDesc;
            m_listActiveTalent.Add(info);
        }

        public void SetSkillBtnEffectClear()
        {
            CountDownCompleteCallBack();
        }

        public void SetSkillBtnEffect(int effectType, int nBeginTime, int nEndTime)
        {
            int nEffectTime = nEndTime - nBeginTime;
            if (timeCount == null)
            {
                return;
            }

            if (bEffectFlagState)
            {
                SetSkillBtnFlagEffect(nEffectTime);
            }
            else
            {
                SetSkillBtnNormalEffect(nEffectTime);
            }
        }

        public void SetSkillBtnFlagEffect(int nTimeShow)
        {

            if (nSkillFlag </*=*/ 0)
            {
                return;
            }

            if (nTimeShow <= 0)
            {
                return;
            }
            int nLastFlagTime = nLeftFlagTime;
            nLeftFlagTime = 0;
            CountDownCompleteCallBack();
            bEffectNomalState = true;//-
            nLeftFlagTime = nLastFlagTime - nTimeShow;
            this.StartCountDown(((float)nTimeShow / 1000), 0.1f, 0.1f);
            this.timeCount.gameObject.SetActive(true);
            m_SkillFlagText.gameObject.SetActive(false);
            int nTmpFlag = nSkillFlag;
            nSkillFlag = 0;
            DoFlagChange();
            nSkillFlag = nTmpFlag;
        }
        public void SetSkillBtnNormalEffect(int nTimeShow)
        {
            CountDownCompleteCallBack();
            if (nTimeShow <= 0)
            {
                return;
            }

            this.StartCountDown(((float)nTimeShow / 1000), 0.1f, 0.1f);
            this.timeCount.gameObject.SetActive(true);
        }

        protected override void CountDownRepeatCallBack()
        {
            base.CountDownRepeatCallBack();

            float MaxfillAmount = bEffectNomalState ? this.fOriginalTime : (float)nMaxFlagTime / 1000;//-
            timeCount.fillAmount = (fCurTime / MaxfillAmount/*this.fOriginalTime*/);//-
            //timeCount.fillAmount = (fCurTime / this.fOriginalTime );
            if (fCurTime > 10f)
            {
                timeCount.gameObject.transform.FindChild("Text").GetComponent<Text>().text = fCurTime.ToString("0");
            }
            else
            {
                timeCount.gameObject.transform.FindChild("Text").GetComponent<Text>().text = fCurTime.ToString("0.0");
            }

        }

        protected override void CountDownCompleteCallBack()
        {
            base.CountDownCompleteCallBack();

            this.timeCount.fillAmount = 1.0f;
            timeCount.gameObject.transform.FindChild("Text").GetComponent<Text>().text = "";
            this.timeCount.gameObject.SetActive(false);

            //-----
            if (bEffectNomalState)
                bEffectNomalState = false;
            if (bEffectFlagState && nLeftFlagTime > 0)
            {
                StartNextFlagLeftCountDown();
                this.nLeftFlagTime = 0;
            }
        }

        private void StartNextFlagLeftCountDown()
        {
            this.StartCountDown((float)nLeftFlagTime / 1000, 0.1f, 0.1f);
            this.timeCount.gameObject.SetActive(true);
            m_SkillFlagText.gameObject.SetActive(true && nSkillFlag > 0);
            DoFlagChange();
        }
        public void SetSkillFlagEffect(int nFlag, int nLeftTime, int nFreezeLeftTime, int nTotalTime)// 这边只能做自然冷却
        {
            bEffectFlagState = true;
            SetSkillFlagCount(nFlag);
            if (nMaxFlagTime != nTotalTime) nMaxFlagTime = nTotalTime;//-
            //1.在充能中"使用"技能 
            if (nLeftTime != nTotalTime)
            {
                nLeftFlagTime = nLeftTime;
                //DoFlagChange();//-
                return;
            }
            // 2.恢复能量时 技能处于CD{ }                        
            if (bEffectNomalState)
            {
                if (nLeftFlagTime <= 0)//正常状态为负数 
                {
                    nLeftFlagTime += nTotalTime;
                }
                else  //补充能量时处于CD理论上LeftTime 为负  再此进行补偿（本地线程和服务器计时出现误差）
                {
                    nLeftFlagTime = nTotalTime;
                }
                return;
            }
            //3.恢复非最高能量且不处于CD
            //4.使用能量时处于最大能量
            int nLastFlagTime = nLeftFlagTime;
            nLeftFlagTime = 0;
            CountDownCompleteCallBack();
            nLeftFlagTime = nLastFlagTime;
            if (nLastFlagTime < 0)
            {
                nLeftTime += nLastFlagTime;
            }

            int nTimeShow = nLeftTime;
            if (nTimeShow > 0)
            {
                this.StartCountDown((float)nTimeShow / 1000, 0.1f, 0.1f);
                this.timeCount.gameObject.SetActive(true);
            }

            nLeftFlagTime = nLeftTime;
            m_SkillFlagText.gameObject.SetActive(true && nSkillFlag > 0);
            DoFlagChange();
        }
        public void SetSkillFlagCount(int nFlag)
        {
            if (m_SkillFlagText == null)
            {
                Debug.LogError("m_SkillFlagText == null");
                return;
            }

            nSkillFlag = nFlag;
            m_SkillFlagText.gameObject.SetActive(true);
            m_SkillFlagText.text = nFlag.ToString();
            m_SkillFlagText.gameObject.SetActive(nFlag > 0);
        }

        public void SetSkillFlagEffectOver(int nFlag)
        {
            bEffectFlagState = false;
            nLeftFlagTime = 0;
            SetSkillFlagCount(nFlag);
        }

        public void SetSkillShortcutKey(string keyCode)
        {
            if (string.IsNullOrEmpty(keyCode))
            {
                return;
            }

            if (KeyText == null)
            {
                return;
            }

            KeyText.text = keyCode;

            UTooltipTrigger skillTip = gameObject.GetComponent<UTooltipTrigger>();
            if (skillTip == null)
            {
                skillTip = gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
                skillTip.backgroundTint = Color.grey;
            }

            string sDesc = skillTip.parameterizedTextFields[0].value;
            if (string.IsNullOrEmpty(sDesc))
            {
                return;
            }

            UBBProperty.GetInstance().getRatio(sDesc);
            UBB.toHtml(ref sDesc, UBB_FORMAT_TYPE.UGUI);
            skillTip.SetText(UTooltipParamName.BodyText, sDesc);
        }

        public void SetSlotSkillUse(bool bUse)
        {
            if (skilImage == null)
            {
                return;
            }


            if (bUse)
            {
                skilImage.ResetToDefaultMaterial();
            }
            else
            {
                skilImage.AddNewMaterial(EUIShaderType.EST_Gray);
            }

            bCanUse = bUse;
        }

        public bool GetSlotSkillUse()
        {
            return bCanUse;
        }

        public void UpdateSpellSlotDescInPropertyChange()
        {
            if (string.IsNullOrEmpty(m_sOriginalDesc))
            {
                return;
            }

            if (SkilltipTrigger == null)
            {
                return;
            }

            SkilltipTrigger.enabled = true;
            string strSlotTipsDesc = m_sOriginalDesc;
            UBBProperty.GetInstance().getRatio(m_sOriginalDesc);
            UBB.toHtml(ref strSlotTipsDesc, UBB_FORMAT_TYPE.UGUI);
            SkilltipTrigger.SetText(UTooltipParamName.BodyText, strSlotTipsDesc);

            if (m_listActiveTalent == null || m_listActiveTalent.Count <= 0)
            {
                return;
            }

            int nTalentIndex = 0;
            foreach (ActiveTalentDescItemInfo itemInfo in m_listActiveTalent)
            {
                string sTalentName = itemInfo.sTalentName;
                UBBProperty.GetInstance().getRatio(sTalentName);
                UBB.toHtml(ref sTalentName, UBB_FORMAT_TYPE.UGUI);

                string sTalentDesc = itemInfo.sTalentDesc;
                UBBProperty.GetInstance().getRatio(sTalentDesc);
                UBB.toHtml(ref sTalentDesc, UBB_FORMAT_TYPE.UGUI);

                nTalentIndex = itemInfo.nTalentIndex;
                //   SkilltipTrigger.SetText(SkillTooltip_TalentParamName + nTalentIndex, sTalentName);
                SkilltipTrigger.SetText(SkillTooltip_SkillParamDesc + nTalentIndex, "[" + sTalentName + "]" + sTalentDesc);
                SkilltipTrigger.TurnSectionOn(SkillTooltip_SkillParamItem + nTalentIndex);
            }
        }

        public void OnPointerEnter(PointerEventData eventData)
        {
            UpdateSpellSlotDescInPropertyChange();

            int nSlotType = m_nSlotID == 4 ? (int)SLOT_TYPE.SLOT_TYPE_SUMMONER : (int)SLOT_TYPE.SLOT_TYPE_SKILL;
            onAllSlotMouseEvent(m_nSlotID, nSlotType, 1);
        }

        public void OnPointerExit(PointerEventData eventData)
        {
            int nSlotType = m_nSlotID == 4 ? (int)SLOT_TYPE.SLOT_TYPE_SUMMONER : (int)SLOT_TYPE.SLOT_TYPE_SKILL;
            onAllSlotMouseEvent(m_nSlotID, nSlotType, 0);
        }

        public void onAllSlotMouseEvent(int nSlotId, int nSlotType, int nMouseEventType)
        {
            cmd_entity_mouse_enter_slot data = new cmd_entity_mouse_enter_slot();

            data.nEnterSlot = nMouseEventType;
            data.nSlotType = nSlotType;
            data.nSlotIndex = nSlotId;

            IntPtrHelper helper = new IntPtrHelper();
            IntPtr ptr = helper.toPtr<cmd_entity_mouse_enter_slot>(ref data);
            int len = Marshal.SizeOf(data);

            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_MOUSE_ENTER_SLOT, len, "", ptr, len);
        }

        public void DoFlagChange()
        {
            m_SkinFlagBGColor.a = nSkillFlag <= 0 ? 0.9f : 0.55f;
            timeCount.gameObject.GetComponent<Image>().color = m_SkinFlagBGColor;
            m_TimeCountWordObj.gameObject.SetActive(nSkillFlag <= 0);

            m_SkinBGColor = nSkillFlag <= 0 ? new Color(0.6f, 0.6f, 0.6f, 1f) : new Color(1f, 1f, 1f, 1f);
            skilImage.gameObject.GetComponent<Image>().color = m_SkinBGColor;
        }

        public void ResetView()
        {
            CountDownCompleteCallBack();
            if (skilImage)
                skilImage.gameObject.SetActive(false);
        }
    }
    #endregion

    #region Buffpart
    public class WarCenterBuffPart : MonoBehaviour
    {
        //旧版使用数组固定Buff增加删除位置
        //public WarCenterBuffButtonItem[] buffItemArray;

        //private Dictionary<int, int> dicBuffIndex = new Dictionary<int, int>(10);

        //public void AddBuff(int nIndex, int nIcon, int dwTime, string sName, string sTip, int nLevel, bool bShow)
        //{
        //    for (int nSlotId = 0; nSlotId < buffItemArray.Length; ++nSlotId)
        //    {

        //        if (buffItemArray[nSlotId].CanUse)
        //        {

        //            dicBuffIndex[nIndex] = nSlotId;
        //            buffItemArray[nSlotId].AddBuff(nIcon, dwTime, sName, sTip, nLevel, bShow);
        //            break;
        //        }
        //    }
        //}

        //public void RemoveBuff(int nIndex)
        //{
        //    if (!dicBuffIndex.ContainsKey(nIndex))
        //    {
        //        return;
        //    }

        //    int nBuffSlotID = dicBuffIndex[nIndex];
        //    if (IsOutOfBuffArray(nBuffSlotID))
        //    {
        //        return;
        //    }

        //    buffItemArray[nBuffSlotID].RemoveBuff();
        //    dicBuffIndex.Remove(nIndex);
        //}
        //public bool IsOutOfBuffArray(int nSlotID)
        //{
        //    if (nSlotID < 0 || nSlotID >= buffItemArray.Length)
        //    {
        //        return true;
        //    }
        //    else
        //    {
        //        return false;
        //    }
        //}

          /// <summary>
        /// 新版使用列表，Buff位置顺序增加,删除时移至队尾（应对解决隐藏激活过快PointEnter无效）
        /// </summary>
        public List< WarCenterBuffButtonItem> buffItemList; 
        private Dictionary<int, WarCenterBuffButtonItem> dicBuffIndex_ = new Dictionary<int, WarCenterBuffButtonItem>(10);
        public void AddBuff(int nIndex, int nIcon, int dwTime, string sName, string sTip, int nLevel, bool bShow)
        {
            for (int nSlotId = 0; nSlotId < buffItemList.Count; ++nSlotId)
            {
                if (buffItemList[nSlotId].CanUse)
                {
                    buffItemList[nSlotId].AddBuff(nIcon, dwTime, sName, sTip, nLevel, bShow);
                    dicBuffIndex_[nIndex] = buffItemList[nSlotId];
                    break;
                }
            }
        }
        public void RemoveBuff(int nIndex)
        {
            if (!dicBuffIndex_.ContainsKey(nIndex))
            {
                return;
            }
            WarCenterBuffButtonItem ButtonItem = dicBuffIndex_[nIndex];
            buffItemList.Remove(ButtonItem);
            buffItemList.Add(ButtonItem);
            ButtonItem.RemoveBuff();
            dicBuffIndex_.Remove(nIndex);
        }

    }

    public class WarCenterBuffButtonItem : TimeCountDown
    {
        public Image timeCount;
        public Text levelText;
        private bool m_bCanUses = true;
        public void AddBuff(int nIcon, int nTime, string sName, string sTip, int nLevel, bool bShowLevel)
        {
            m_bCanUses = false;
            if (levelText == null || timeCount == null)
            {
                Debug.Log("levelText or timeCount is miss");
                return;
            }

            this.gameObject.SetActive(true);

            UTooltipTrigger buffTip = gameObject.GetComponent<UTooltipTrigger>();
            if (buffTip == null)
            {
                buffTip = gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
                buffTip.backgroundTint = Color.grey;
            }

            UBBProperty.GetInstance().getRatio(sTip);
            UBB.toHtml(ref sName, UBB_FORMAT_TYPE.UGUI);
            UBB.toHtml(ref sTip, UBB_FORMAT_TYPE.UGUI);
            buffTip.SetText(UTooltipParamName.BodyText, sName + "\n" + sTip);

            if (nTime != 0)
            {
                CountDownCompleteCallBack();
                this.StartCountDown((float)nTime / 1000, 0.1f, 0.1f);
                timeCount.gameObject.SetActive(true);
                this.gameObject.SetActive(true);
            }

            levelText.text = nLevel.ToString();
            levelText.gameObject.SetActive(nLevel > 1 && bShowLevel);
            this.gameObject.GetComponent<Image>().sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_WAR_BOTTOMCENTERVIEW, 1, nIcon);
        }

        public void RemoveBuff()
        {
            m_bCanUses = true;
            transform.SetAsLastSibling();//-
            CountDownCompleteCallBack();
        }

        protected override void CountDownRepeatCallBack()
        {
            if (levelText == null || timeCount == null)
            {
                Debug.Log("levelText or timeCount is miss");
                return;
            }

            base.CountDownRepeatCallBack();

            timeCount.fillAmount = (fCurTime / this.fOriginalTime);
        }

        protected override void CountDownCompleteCallBack()
        {
            if (levelText == null || timeCount == null)
            {
                Debug.Log("levelText or timeCount is miss");
                return;
            }

            base.CountDownCompleteCallBack();
            timeCount.fillAmount = 1.0f;
            timeCount.gameObject.SetActive(false);
            this.gameObject.SetActive(false);
            levelText.text = "";
        }

        public bool CanUse
        {
            set { m_bCanUses = value; }
            get { return m_bCanUses; }
        }
    }
    #endregion
    public class WarBottonCenterWndView : UIBaseWndView
    {
        private WarBottonCenterWnd warBottonCenterWnd;
        public WarCenterGoodPart warCenterGoodPart;
        public WarCenterSkillPart warCenterSkillPart;
        public WarCenterBuffPart warCenterBuffPart;

        public GameObject warCenterPropertyPart;
        public GameObject warTalentBtn;
        /*经验条*/
        public Image expBar;
        /*体力值*/
        public Image staminaBar;
        /*生命条*/
        public Slider hpBar;
        /*护盾条*/
        public Slider shieldBar;
        /*魔法条*/
        public Slider mpBar;
        /*等级文本*/
        public Text levelTF;
        /*HP文本*/
        public Text hPTF;
        /*MP文本*/
        public Text mPTF;
        /*HP文本*/
        public Text hPTFMax;
        /*MP文本*/
        public Text mPTFMax;

        public Text expTF;
        public Text vitTF;
        public Text expTFMax;
        public Text vitTFMax;

        public Text goldTF;
        /*物品购买界面按钮*/
        public GameObject goodsStoreBtn;

        /*英雄头像按钮*/
        public Image heroProtraitBtn;

        ///*等级文本*/
        //public Text mATF;
        ///*HP文本*/
        //public Text pATF;
        ///*MP文本*/
        //public Text pDTF;
        ///*等级文本*/
        //public Text mDTF;
        ///*HP文本*/
        //public Text aSPDTF;
        ///*MP文本*/
        //public Text cRIlTF;
        ///*等级文本*/
        //public Text sDTF;
        ///*HP文本*/
        //public Text cDTF;

        public Text[] propertyDesc;

        public Image mpMoveObj;
        public Image hpMoveObj;

        public Image expMoveObj;
        public Image vitMoveObj;

        public GameObject leftPartObj;
        //public GameObject rightPartObj;
        public GameObject talentPartObj;

        public Animator propertyPanelMove;
        public Animator firstSkillUpdateMove;
        public Animator thirdSkillUpdateMove;
        public Animator secondSkillUpdateMove;
        public int currentSpellUpgradePoint = 0;
        public PropertyItem[] PropertyItemGroup;

        private UEffectParamBase m_OldEffectParam;
        private int m_nMainHeroVocationId = 0;
        private bool m_TalentViewVisible = false;
        private float m_fMoveTime = 3.0f;

        private UEffectPrefabParam HPLackEffect = null;

        public override void Start()
        {
            Debug.Log("WarBottonCenterWndView:Start");
            if (m_wnd == null)//测试用
            {   //{
                //    WarBottonCenterWnd = new WarBottonCenterWnd();
                //    WarBottonCenterWnd.m_wndView = this;
            }
            else
            {
                warBottonCenterWnd = (WarBottonCenterWnd)m_wnd;
            }

            //SetHeroHP(0, 540);

            //SetHeroMP(0, 440);
            //SetHeroExp(0, 440);
            //SetHeroStamina(0, 440);

            leftPartObj.transform.SetParent(this.transform.parent);
            //rightPartObj.transform.SetParent(this.transform.parent);
            leftPartObj.GetComponent<RectTransform>().anchoredPosition = Vector3.zero;
            //rightPartObj.GetComponent<RectTransform>().anchoredPosition = Vector3.zero;
            SetPropertyViewToolTip();

            warCenterSkillPart.Init();

            InitPropertyValue();

        }

        public override void Destroy()
        {
            leftPartObj.transform.SetParent(this.transform);
            //rightPartObj.transform.SetParent(this.transform);
            base.Destroy();
        }

        public void ResetView()
        {
            goldTF.text = "0";
            hPTF.text = "0";
            hPTFMax.text = "0";
            mPTF.text = "0";
            mPTFMax.text = "0";
            expTF.text = "0";
            expTFMax.text = "0";
            vitTF.text = "0";
            vitTFMax.text = "0";

            warTalentBtn.SetActive(false);

            if (warCenterGoodPart != null)
                warCenterGoodPart.ResetView();

            if (warCenterSkillPart != null)
                warCenterSkillPart.ResetView();
        }

        public override void Update()
        {
            base.Update();
            CheckSpellUpdateByShortcut();
            CheckTalentButtonShortcut();
        }

        public void DynamicsSetTalentChild(GameObject talentObj)
        {
            talentObj.transform.SetParent(this.warTalentBtn.transform, false);
            talentObj.GetComponent<RectTransform>().anchoredPosition = new Vector3(156, 516, 0);
        }

        #region CS->UGUI
        public void SetHeroVocationProtrait(int nHeroIconId)
        {
            heroProtraitBtn.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_BOTTOMCENTERVIEW, 1, 4, nHeroIconId);
            m_nMainHeroVocationId = nHeroIconId;
        }

        public void SetHeroExp(int nCurValue, int nMaxValue)
        {
            //expBar.fillAmount = fFillCount;
            expTF.text = nCurValue.ToString();
            expTFMax.text = nMaxValue.ToString();

            DoBarGameObjectMoveEx(expMoveObj, nCurValue, nMaxValue);
        }

        public void SetHeroLevel(int nLevel)
        {
            nLevel = nLevel > 99 ? 99 : nLevel;
            levelTF.text = nLevel.ToString();
        }

        public void SetHeroHP(int nCurValue, int nMaxValue)
        {
            // hpBar.value = (float)nCurHp/nMaxHp;
            hPTF.text = nCurValue.ToString();
            hPTFMax.text = nMaxValue.ToString();

            DoBarGameObjectMoveEx(hpMoveObj, nCurValue, nMaxValue);

            // 播放残血光效
            PlayHPEffect(nCurValue, nMaxValue);
        }

        public void SetHeroMP(int nCurValue, int nMaxValue)
        {
            //mpBar.value = (float)nCurMp / nMaxMp;
            mPTF.text = nCurValue.ToString();
            mPTFMax.text = nMaxValue.ToString();

            DoBarGameObjectMoveEx(mpMoveObj, nCurValue, nMaxValue);
        }

        //public void SetAllPropertyValue(string strATK, string strMA, string strDEF, string strMDEF, string strAF, string strCOOLREDUCE, string strCRC, string strSPD)
        //{
        //    pATF.text = strATK;
        //    mATF.text = strMA;
        //    pDTF.text = strDEF;
        //    mDTF.text = strMDEF;
        //    aSPDTF.text = strAF;
        //    cDTF.text = strCOOLREDUCE + "%";
        //    cRIlTF.text = strCRC + "%";
        //    sDTF.text = strSPD;
        //}
        private void InitPropertyValue()
        {
            propertyDesc[0].text = "0";
            propertyDesc[1].text = "0";
            propertyDesc[2].text = "0";
            propertyDesc[3].text = "0%";
            propertyDesc[4].text = "0";
            propertyDesc[5].text = "0";
            propertyDesc[6].text = "0%";
            propertyDesc[7].text = "0";
            propertyDesc[8].text = "0%";
        }

        public void SetPropertyValue(int nIndex, string strDesc)
        {
            if (nIndex >= 0 && nIndex < propertyDesc.Length)
            {
                propertyDesc[nIndex].text = strDesc;
            }
        }

        public void SetHeroStamina(int nCurValue, int nMaxValue)
        {
            // staminaBar.fillAmount = fFillCount;
            vitTF.text = nCurValue.ToString();
            vitTFMax.text = nMaxValue.ToString();
            DoBarGameObjectMoveEx(vitMoveObj, nCurValue, nMaxValue);
        }

        public void DoBarGameObjectMove(GameObject moveObj, EBarObjMoveType eType, int nCurValue, int nMaxValue)
        {
            if (nMaxValue <= 0 || nCurValue > nMaxValue)
            {
                return;
            }

            float fTotalLen = moveObj.GetComponent<RectTransform>().sizeDelta.x;
            float fLostRatio = (nMaxValue - nCurValue) / (float)nMaxValue;
            float fCurPosition = fLostRatio * fTotalLen;
            float fCurMoveTime = fLostRatio * m_fMoveTime;
            fCurPosition = eType == EBarObjMoveType.EBOMT_LEFT ? fCurPosition * -1 : fCurPosition;
            fCurMoveTime = fCurMoveTime <= 0 ? 1 : fCurMoveTime;

            moveObj.transform.DOPause();
            moveObj.transform.DOLocalMoveX(fCurPosition, fCurMoveTime);
        }

        private void DoBarGameObjectMoveEx(Image img, int nCurValue, int nMaxValue)
        {
            if (nMaxValue <= 0 || nCurValue > nMaxValue)
            {
                return;
            }

            float fLostRatio = (nMaxValue - nCurValue) / (float)nMaxValue;
            float fCurMoveTime = fLostRatio * m_fMoveTime;

            float curAmount = (float)nCurValue / (float)nMaxValue;

            img.DOPause();
            img.DOFillAmount(curAmount, fCurMoveTime);
        }

        public void SetGoodsStoreMoney(int nMoney)
        {
            nMoney = nMoney > 100000 ? 100000 : nMoney;
            goldTF.text = nMoney.ToString();
        }

        public void SetSpellSlotUpgrade(cmd_creature_spell_point data)
        {
            if (data.nAddSpellPoint[0] > 0)
            {
                firstSkillUpdateMove.SetBool("LevelUp", true);
            }
            if (data.nAddSpellPoint[1] > 0)
            {
                secondSkillUpdateMove.SetBool("LevelUp", true);
            }
            if (data.nAddSpellPoint[2] > 0)
            {
                thirdSkillUpdateMove.SetBool("LevelUp", true);
            }

            currentSpellUpgradePoint = data.nSpellPoint;
            if (currentSpellUpgradePoint > 0)
            {
                SetBuffPartMoveUp();
            }
        }

        public void SetSpellSlot(int nSlotID, int nSlotType, int nSlotIconID, string strSlotTipsDesc, int nSlotLevel, bool bSpellUseFlag)
        {
            warCenterSkillPart.SetSpellSlot(nSlotID, nSlotType, nSlotIconID, strSlotTipsDesc, nSlotLevel, bSpellUseFlag);
        }

        private void CheckSpellUpdateByShortcut()
        {
            //没技能点就不检测
            if (currentSpellUpgradePoint <= 0)
            {
                return;
            }

            // 检测是否按下Ctrl,不然会与激活天赋快捷键冲突
            if (InputManager.GetKey(KeyCode.LeftControl) || InputManager.GetKey(KeyCode.RightControl))
            {
                return;
            }

            for (int slotID = 0; slotID < 4; slotID++)
            {
                if (FuntionShortCutCtrl.CheckFunctionShortcutKeyDown(FuntionShortCutSlotIndex.Com_UpgradeSpellSlot0 + slotID))
                {
                    OnSkillUpgradeClick(slotID);
                }
            }
        }

        private void CheckTalentButtonShortcut()
        {
            if (InputManager.GetKeyDown(KeyCode.LeftControl))
            {
                //OnWarTalentButtonClick();
                ShowTalentView(true);

            }

            if (InputManager.GetKeyUp(KeyCode.LeftControl))
            {
                ShowTalentView(false);
            }
        }

        public void SetSkillBtnEffectClear(int nSlotID)
        {
            warCenterSkillPart.SetSkillBtnEffectClear(nSlotID);
        }

        public void SetSkillBtnEffect(int nSlotID, int effectType, int beginTime, int endTime)
        {
            warCenterSkillPart.SetSkillBtnEffect(nSlotID, effectType, beginTime, endTime);
        }

        public void SetSkillFlagEffect(int nSlotID, int nFlag, int nFreezeLeftTime, int nLeftTime, int nTotalTime)
        {
            warCenterSkillPart.SetSkillFlagEffect(nSlotID, nFlag, nFreezeLeftTime, nLeftTime, nTotalTime);
        }

        public void SetSkillShortcutKey(int nSlotId, string keyCode)
        {
            warCenterSkillPart.SetSkillShortcutKey(nSlotId, keyCode);
        }

        public void SetSkillFlagEffectOver(int nSlotID, int nFlag)
        {
            warCenterSkillPart.SetSkillFlagEffectOver(nSlotID, nFlag);
        }

        public void SetMpEffectSpell(int nSlotID, bool bUse)
        {
            warCenterSkillPart.SetSlotSkillUse(nSlotID, bUse);
        }

        public void AddBuff(int nSlotID, int nIcon, int dwTime, string sName, string sTip, int nLevel, bool bShow)
        {
            warCenterBuffPart.AddBuff(nSlotID, nIcon, dwTime, sName, sTip, nLevel, bShow);
        }

        public void RemoveBuff(int nSlotID)
        {
            warCenterBuffPart.RemoveBuff(nSlotID);
        }

        public void SetBuffPartMoveUp()
        {
            Vector3 vOldPosition = warCenterBuffPart.gameObject.transform.localPosition;
            warCenterBuffPart.gameObject.transform.localPosition = new Vector3(vOldPosition.x, 220, vOldPosition.z);
        }

        public void SetBuffPartMoveDown()
        {
            Vector3 vOldPosition = warCenterBuffPart.gameObject.transform.localPosition;
            warCenterBuffPart.gameObject.transform.localPosition = new Vector3(vOldPosition.x, 220, vOldPosition.z);
        }

        // GoodPart
        public void SetGoodsSlotShortcutKey(int nSlotId, string keyCode)
        {
            warCenterGoodPart.SetGoodsSlotShortcutKey(nSlotId, keyCode);
        }

        public void AddGoodsButtonToSlot(cmd_war_persongoodsinfo data, SSchemeMobaGoods goodData)
        {
            warCenterGoodPart.AddGoodsButtonToSlot(data, goodData);
        }

        public void RemoveGoodsButtonFromSlot(int nSlotId)
        {
            warCenterGoodPart.RemoveGoodsButtonFromSlot(nSlotId);
        }

        public void SetPurchasedGoodsCanDragDrop(int nSlotID, bool bDrag)
        {
            warCenterGoodPart.SlotCanDragDrop(nSlotID, bDrag);
        }

        public void SetPurchasedGoodsUseEffect(int nSlotID, int nBeginTIme, int nEndTime)
        {
            warCenterGoodPart.PlayPurchasedGoodsUseEffect(nSlotID, nBeginTIme, nEndTime);
        }

        public void SetXpSkillFireEffect()
        {
            UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_UWarBottomCenter_XpSkillFire, _tfAttachParent: this.heroProtraitBtn.transform);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);

            m_OldEffectParam = param;
        }

        public void DestoryXpSkillFireEffect()
        {
            if (m_OldEffectParam == null)
            {
                return;
            }

            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_OldEffectParam);
        }

        public void SetTalentLearnActive(int nTalentId)
        {
            if (warCenterSkillPart == null)
            {
                return;
            }

            warCenterSkillPart.SetTalentLearnActive(nTalentId, m_nMainHeroVocationId);
        }

        public void SetPropertyViewToolTip()
        {
            if (PropertyItemGroup == null || PropertyItemGroup.Length <= 0)
            {
                return;
            }

            int nIndex = 0;
            string sPropertyName = "PropertyName_";
            foreach (PropertyItem item in PropertyItemGroup)
            {
                if (item == null)
                    continue;
                string sDesc = ULocalizationService.Instance.Get("ToolTips", "WarBottomView", sPropertyName + nIndex++);
                item.SetToolTipDesc(sDesc);
            }
        }

        private void PlayHPEffect(int nCurValue, int nMaxValue)
        {
            if (nMaxValue == 0)
                return;

            bool bActive = false;

            // 残血光效
            if (nCurValue <= 0)
            {
                bActive = false;
            }
            else
            {
                // 血量小于20%,则播放特效
                if (nCurValue < nMaxValue * 0.2)
                    bActive = true;
                else
                    bActive = false;
            }

            if (bActive)
            {
                if (HPLackEffect == null)
                {
                    UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_UWarBottomCenter_LackHP, _tfAttachParent: hpBar.transform);
                    UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);
                    HPLackEffect = (UEffectPrefabParam)param;
                }
                else
                {
                    HPLackEffect.EffectGameObject.SetActive(true);
                }
            }
            else
            {
                if (HPLackEffect != null)
                    HPLackEffect.EffectGameObject.SetActive(false);
            }
        }

        #endregion

        #region UGUI->CS
        public void OnHeadClick()
        {
            bool bShow = propertyPanelMove.GetBool("PropertyPanelIn");
            propertyPanelMove.SetBool("PropertyPanelIn", !bShow);
        }

        public void OnSkillUpgradeClick(int nClickSlotId)
        {
            bool bFirstUp = firstSkillUpdateMove.GetBool("LevelUp");
            bool bSecondUp = secondSkillUpdateMove.GetBool("LevelUp");
            bool bThirdUp = thirdSkillUpdateMove.GetBool("LevelUp");

            if (nClickSlotId == 0 && !bFirstUp)
            {
                return;
            }
            else if (nClickSlotId == 1 && !bSecondUp)
            {
                return;
            }
            else if (nClickSlotId == 2 && !bThirdUp)
            {
                return;
            }

            firstSkillUpdateMove.SetBool("LevelUp", false);
            secondSkillUpdateMove.SetBool("LevelUp", false);
            thirdSkillUpdateMove.SetBool("LevelUp", false);
            cmd_creature_upgrade_spell data = new cmd_creature_upgrade_spell();
            data.nSlotIndex = nClickSlotId;

            IntPtrHelper helper = new IntPtrHelper();
            IntPtr ptr = helper.toPtr<cmd_creature_upgrade_spell>(ref data);
            int len = Marshal.SizeOf(data);

            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_UPGRADE_SPELL, len, "", ptr, len);

            if (currentSpellUpgradePoint <= 1)
            {
                SetBuffPartMoveDown();
            }
        }

        public void OpenGoodsStoreFrameClick()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_TOGGLEVISIBLE, null);
        }

        public void onMobaGoodsItemClick(WarCenterGoodButtonItemDrag goodItem)
        {
            if (warBottonCenterWnd == null)
            {
                return;
            }

            warBottonCenterWnd.GetMyHeroCtrl().Press_Skill((int)SLOT_TYPE.SLOT_TYPE_GOODS, goodItem.nSlotId);
        }

        public void onSkillBtnClick(int nSkillSlotID)
        {
            if (warBottonCenterWnd == null)
            {
                return;
            }

            if (!warCenterSkillPart.GetSlotSkillUse(nSkillSlotID))
            {
                return;
            }

            if (nSkillSlotID >= 5 && nSkillSlotID <= 6)
            {
                nSkillSlotID += 9;
            }

            warBottonCenterWnd.GetMyHeroCtrl().Press_Skill((int)SLOT_TYPE.SLOT_TYPE_SKILL, nSkillSlotID);
        }

        public void OnWarTalentButtonClick()
        {
            ShowTalentView(!m_TalentViewVisible);
        }

        public void CloseWarTalentView()
        {
            ShowTalentView(false);
        }

        public void ShowTalentView(bool bTalentVisible)
        {
            m_TalentViewVisible = bTalentVisible;
            HeroTalentVisibleData UIData = new HeroTalentVisibleData();
            UIData.bVisible = m_TalentViewVisible;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_HEROTALENTVIEW_HEROTALENTVISIBLE, UIData);

            //SendChatVisibleMessage(m_TalentViewVisible);
        }

        public void OnPropertyButtonClick()
        {
            if (warCenterPropertyPart == null)
            {
                return;
            }

            warCenterPropertyPart.SetActive(!warCenterPropertyPart.activeSelf);
            // SendChatVisibleMessage(m_TalentViewVisible || warCenterPropertyPart.activeSelf);
        }

        public void SendChatVisibleMessage(bool bForceHide)
        {
            ChatForceConditionHide ChatData = new ChatForceConditionHide();
            ChatData.bForceHide = bForceHide;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_CONDITIONHIDE, ChatData);
        }

        public void OnLeftIndentClick()
        {
            if (warCenterGoodPart == null)
            {
                return;
            }

            warCenterGoodPart.gameObject.SetActive(!warCenterGoodPart.gameObject.activeSelf);
        }

        public void onTabBtnClick()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RECORDTABLE_FIGHTING_SWITCH_WND, null);
        }
        #endregion
    }
}