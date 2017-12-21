using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using UIWidgets;
using U3D_Render.Common;
using USpeedUI.UExtensions;
using DataCenter;
using ASpeedGame.Data.GameMobaSchemes;

namespace USpeedUI.WarDeathReplay
{
    public class WarDeathReplayWnd : UIBaseWnd<WarDeathReplayWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_DEATHREPLAY;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override string GetPrefabPath()
        {
            return UPath.UDynamicMobaState + "/WarDeathReplay/WarDeathReplayView";
        }

        public override bool OnCloseKey()
        {
            if(m_wndView != null)
            {
                return m_wndView.MainFrameShow();
            }
            return base.OnCloseKey();
        }

        public override bool Init()
        {
            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_DEATHREPLAY_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_DEATHREPLAY_SHOWMURDERINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_DEATHREPLAY_CLOSE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_DEATHREPLAY_UNLOAD, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);

            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_DEATHREPLAY_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_DEATHREPLAY_SHOWMURDERINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_DEATHREPLAY_CLOSE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_DEATHREPLAY_UNLOAD, this);
            //退订战场结束事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        LoadView();
                        if (m_wndView != null)
                        {
                            m_wndView.ShowTitleFrame(false);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_DEATHREPLAY_SHOW:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.ShowTitleFrame(true);
                            m_wndView.LoadDamageInfo();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_DEATHREPLAY_SHOWMURDERINFO:
                    {
                        if (m_wndView != null)
                        {
                            DeathReplayMurderData murderData = data as DeathReplayMurderData;
                            m_wndView.ShowMurderInfo(murderData);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_DEATHREPLAY_CLOSE:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.ShowTitleFrame(false);
                            m_wndView.UnLoadDamageInfo();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                case WndMsgID.WND_MSG_WAR_DEATHREPLAY_UNLOAD:
                case WndMsgID.WND_MSG_COMMON_WAREND:
                    {
                        UnloadView();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    //死亡回放角色类型
    [Serializable]
    public enum MurderType : int
    {
        MurderType_Murder = 0,     //击杀者
        MurderType_MainAssister,   //主助攻者
        MurderType_Assister,       //副助攻者
    }

    //伤害数据分项(细化伤害)
    [Serializable]
    public class DeathReplayDamageData
    {
        public Image damageIcon;
        public Text damageType;
        public Text damageNum;
    }

    //死亡回放数据项
    [Serializable]
    public class DeathReplayDataItem
    {
        public Image headIcon;
        public List<Image> goodsIcon;
        public Image skillIcon;
        //public Text roleName;
        public Text skinName;
        public List<DeathReplayDamageData> damageData;
    }

    /// <summary>
    /// 死亡回放凶手信息类
    /// 移动到对应角色上时，死亡回放界面底部显示该英雄说明信息
    /// </summary>
    public class DeathReplayMurderInfo : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler
    {
        public MurderType m_MurderType;

        public virtual void OnPointerEnter(PointerEventData eventData)
        {
            DeathReplayMurderData data = new DeathReplayMurderData(m_MurderType, true);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_DEATHREPLAY_SHOWMURDERINFO, data);
        }

        public virtual void OnPointerExit(PointerEventData eventData)
        {
            DeathReplayMurderData data = new DeathReplayMurderData(m_MurderType, false);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_DEATHREPLAY_SHOWMURDERINFO, data);
        }
    }

    public class WarDeathReplayWndView : UIBaseWndView
    {
        //死亡回放数据表，存放UI相关控件
        public List<DeathReplayDataItem> m_DeathReplayDataList = new List<DeathReplayDataItem>();
        public GameObject m_DeathReplayFrame;
        public GameObject m_MurderInfoFrame;
        public GameObject m_TipsInfoFrame;
        public Text m_DeathReplayTitle;
        public Text m_DeathReplayBtnLabel;
        public Text murderLabel;
        public Text[] AssistsLabel;
        public Text m_HurtInfo;
        public Image m_MurderIcon;
        public Text m_MurderType;
        public Text m_MurderDesc;

        //死亡回放数据表，存放死亡回放角色信息所有数据
        private List<WarDeathReplayDataManager.DeathReplayData> DamageDataList = new List<WarDeathReplayDataManager.DeathReplayData>();
        private WarDeathReplayDataManager.DeathReplayData murderData;
        private int nPlayerBlood;

        private string szTitleName;
        private string szOpenBtnTip;
        private string szMurderDesc;
        private string szAssistsDesc;
        private string szHurtInfo;
        private string szTrueDamage;
        private string szPhysicalDamage;
        private string szMagicDamage;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);
            ClearDeathReplayViewData();

            szTitleName = ULocalizationService.Instance.Get("UIView","DeathReplay","szTitleName");
            szOpenBtnTip = ULocalizationService.Instance.Get("UIView", "DeathReplay", "szOpenBtnTip");
            szHurtInfo = ULocalizationService.Instance.Get("UIView", "DeathReplay", "szHurtInfo");
            szTrueDamage = ULocalizationService.Instance.Get("UIView", "DeathReplay", "szTrueDamage");
            szPhysicalDamage = ULocalizationService.Instance.Get("UIView", "DeathReplay", "szPhysicalDamage");
            szMagicDamage = ULocalizationService.Instance.Get("UIView", "DeathReplay", "szMagicDamage");
            szMurderDesc = ULocalizationService.Instance.Get("UIView", "DeathReplay", "szMurderDesc");
            szAssistsDesc = ULocalizationService.Instance.Get("UIView", "DeathReplay", "szAssistsDesc");

            m_DeathReplayTitle.text = szTitleName;
            m_DeathReplayBtnLabel.text = szOpenBtnTip;
            murderLabel.text = szMurderDesc;
            foreach (Text tt in AssistsLabel)
            {
                tt.text = szAssistsDesc;
            }

            return true;
        }

        public bool MainFrameShow()
        {
            return m_DeathReplayFrame.gameObject.activeInHierarchy;
        }

        internal override void SetRendererVisible(bool _bVisible)
        {
            m_DeathReplayFrame.gameObject.SetActive(_bVisible); 
        }

        public override void Destroy()
        {
            base.Destroy();
        }

        public void ShowTitleFrame(bool bShow)
        {
            this.gameObject.SetActive(bShow);
        }

        public void LoadDamageInfo()
        {
            if (null != EntityFactory.MainHeroView)
                nPlayerBlood = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MAX_HP);
            
            DamageDataList = DataCenter.LogicDataCenter.deathReplayDataManager.GetDeathReplayInfo();
            if (DamageDataList.Count < 1)
                return;

            uint startTime = DataCenter.LogicDataCenter.deathReplayDataManager.StartTime;
            uint endTime = DataCenter.LogicDataCenter.deathReplayDataManager.EndTime;
            if (startTime > 0 && endTime > startTime)
                m_HurtInfo.text = string.Format(szHurtInfo, Mathf.Clamp((endTime - startTime) / 1000, 1, 60), DataCenter.LogicDataCenter.deathReplayDataManager.TotalHurtNum);

            int murderSkin = DataCenter.LogicDataCenter.deathReplayDataManager.MurderSkinID;
            int index = DamageDataList.FindIndex((x) => { return x.skinID == murderSkin; });
            if (index > -1)
            {
                murderData = DamageDataList[index];
                SetDamageData(0, DamageDataList[index]);
                DamageDataList.RemoveAt(index);
            }

            for (int i = 0; i < Mathf.Clamp(DamageDataList.Count, 0, m_DeathReplayDataList.Count - 1); i++)
                SetDamageData(i + 1, DamageDataList[i]);
        }

        public void OnShowDeathReplayBtnClick()
        {
            m_DeathReplayFrame.gameObject.SetActive(true);
        }

        public void OnCloseBtnClick()
        {
            m_DeathReplayFrame.gameObject.SetActive(false);
        }

        public void UnLoadDamageInfo()
        {
            m_DeathReplayFrame.gameObject.SetActive(false);
            DataCenter.LogicDataCenter.deathReplayDataManager.ClearDeathReplayData();
            ClearDeathReplayViewData();
        }

        public void ShowMurderInfo(DeathReplayMurderData data)
        {
            if (data.bShowMurderInfo)
            {
                m_MurderIcon.sprite = m_DeathReplayDataList[(int)data.murderType].headIcon.sprite;
                if (data.murderType == MurderType.MurderType_Murder)
                {
                    m_MurderType.text = szMurderDesc;
                    if (null != murderData)
                    {
                        string desc = murderData.heroDesc;
                        UBB.toHtml(ref desc);
                        m_MurderDesc.text = desc;
                    }
                }
                else
                {
                    m_MurderType.text = szAssistsDesc;
                    if (DamageDataList.Count >= (int)data.murderType)
                    {
                        string desc = DamageDataList[(int)data.murderType - 1].heroDesc;
                        UBB.toHtml(ref desc);
                        m_MurderDesc.text = desc;
                    }
                }
                m_MurderInfoFrame.gameObject.SetActive(true);
                m_TipsInfoFrame.gameObject.SetActive(false);
            }
            else
            {
                m_MurderIcon.sprite = USpriteManager.Instance.DefaultSprite;
                m_MurderType.text = "";
                m_MurderDesc.text = "";
                m_MurderInfoFrame.gameObject.SetActive(false);
                m_TipsInfoFrame.gameObject.SetActive(true);
            }
        }

        private void SetDamageData(int index, WarDeathReplayDataManager.DeathReplayData data)
        {
            DeathReplayDataItem dataItem = m_DeathReplayDataList[index];

            //名称
            //dataItem.roleName.text = data.roleName;
            dataItem.skinName.text = data.skinName;

            //头像和召唤师技能
            if (null != data.personInfo)
            {
                dataItem.headIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_DEATHREPLAY, 1, 3, data.personInfo.nHeroId);
                dataItem.skillIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_WAR_DEATHREPLAY, 1, data.personInfo.nSummerIconID);
                //召唤师技能Tip
                //UTooltipTrigger skillTrigger = dataItem.skillIcon.GetComponent<UTooltipTrigger>();
                //if(null != skillTrigger)
                //{
                //    string skillDesc = data.personInfo.sSummerDesc;
                //    UBB.toHtml(ref skillDesc);
                //    skillTrigger.enabled = data.personInfo.nSummerIconID > 0;
                //    skillTrigger.SetText(UTooltipParamName.BodyText, skillDesc);
                //}
                
            }
            if(data.entityType == ENTITY_TYPE.TYPE_MONSTER)
            {
                int headId = GameLogicAPI.getMonsterHeadIconID(data.monsterID);
                dataItem.headIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MonsterHeadPortrait, WndID.WND_ID_WAR_DEATHREPLAY, headId, 1);
            }

            //物品信息
            if(null != data.goodsInfo)
            {
                for (int i = 0; i < Mathf.Clamp(data.goodsInfo.Count, 0, dataItem.goodsIcon.Count); i++)
                {
                    SSchemeMobaGoods good;
                    LogicDataCenter.warMobaGoodsDataManager.GetMobaGoods_Out(data.goodsInfo[i].nGoodID, out good);
                    dataItem.goodsIcon[i].sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MobaGoods, WndID.WND_ID_WAR_DEATHREPLAY, 1, good.GoodsIconID);
                    //装备Tip
                    //UTooltipTrigger goodTipTrigger = dataItem.goodsIcon[i].GetComponent<UTooltipTrigger>();
                    //if (null != goodTipTrigger)
                    //{
                    //    goodTipTrigger.enabled = good.GoodsID > 0;
                    //    string sGoodName = good.GoodsName;
                    //    string sGoodsSlotDes = good.GoodsSlotDes;
                    //    string sGoodsDes = good.GoodsDes;
                    //    UBB.toHtml(ref sGoodName);
                    //    UBBProperty.GetInstance().getRatio(sGoodsDes);
                    //    UBB.toHtml(ref sGoodsDes);
                    //    goodTipTrigger.SetImage("GoodsIcon", dataItem.goodsIcon[i].sprite);
                    //    goodTipTrigger.SetText("GoodsName", sGoodName);
                    //    goodTipTrigger.SetText("GoodsSlotDesc", sGoodsSlotDes);
                    //    goodTipTrigger.SetText("GoodsInfo", sGoodsDes);
                    //}
                }
            }

            //伤害信息
            for (int i = 0; i < Mathf.Clamp(data.skillInfoList.Count, 0, dataItem.damageData.Count); i++)
            {
                DeathReplayDamageData damageData = dataItem.damageData[i];
                //伤害技能图标
                damageData.damageIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_WAR_DEATHREPLAY, 1, data.skillInfoList[i].skillIcon);
                //技能名称
                string name = data.skillInfoList[i].szSkillName;
                UBB.toHtml(ref name);
                dataItem.damageData[i].damageType.text = name;
                //技能伤害数值
                int num = data.skillInfoList[i].damageNum;
                int type = data.skillInfoList[i].spellEffectType;

                if((type & (int)SPELL.SPELL_EFFECT_TYPE.SpellEffectType_PhysicalAttack) != 0 && nPlayerBlood > 0)
                    damageData.damageNum.text = string.Format(szPhysicalDamage, num, Mathf.Clamp(num * 100 / nPlayerBlood, 0, 100));
                else if ((type & (int)SPELL.SPELL_EFFECT_TYPE.SpellEffectType_MagicAttack) != 0 && nPlayerBlood > 0)
                    damageData.damageNum.text = string.Format(szMagicDamage, num, Mathf.Clamp(num * 100 / nPlayerBlood, 0, 100));
                else if ((type & (int)SPELL.SPELL_EFFECT_TYPE.SpellEffectType_TrueDamage) != 0 && nPlayerBlood > 0)
                    damageData.damageNum.text = string.Format(szTrueDamage, num, Mathf.Clamp(num * 100 / nPlayerBlood, 0, 100));
                
                //技能Tip
                //UTooltipTrigger damageTrigger = dataItem.damageData[i].damageIcon.GetComponent<UTooltipTrigger>();
                //if(null != damageTrigger)
                //{
                //    string damageDesc = data.skillInfoList[i].szSkillDesc;
                //    UBBProperty.GetInstance().getRatio(damageDesc);
                //    UBB.toHtml(ref damageDesc);
                //    damageTrigger.enabled = data.skillInfoList[i].skillID > 0;
                //    damageTrigger.SetText(UTooltipParamName.BodyText, damageDesc);
                //}
            }
        }

        /// <summary>
        /// 清数据，主要是清Tips
        /// </summary>
        private void ClearDeathReplayViewData()
        {
            Sprite defaultSprite = USpriteManager.Instance.DefaultSprite;
            DamageDataList = null;
            murderData = null;

            foreach (var dataItem in m_DeathReplayDataList)
            {
                //dataItem.roleName.text = "";
                dataItem.skinName.text = "";

                dataItem.headIcon.sprite = defaultSprite;
                dataItem.skillIcon.sprite = defaultSprite;
                UTooltipTrigger skillTrigger = dataItem.skillIcon.GetComponent<UTooltipTrigger>();
                if (null != skillTrigger)
                {
                    skillTrigger.Clear();
                    skillTrigger.enabled = false;
                }

                foreach (var good in dataItem.goodsIcon)
                {
                    good.sprite = defaultSprite;
                    UTooltipTrigger goodTipTrigger = good.GetComponent<UTooltipTrigger>();
                    if (null != goodTipTrigger)
                    {
                        goodTipTrigger.Clear();
                        goodTipTrigger.enabled = false;
                    }
                }

                foreach (var damage in dataItem.damageData)
                {
                    damage.damageIcon.sprite = defaultSprite;
                    UTooltipTrigger damageTrigger = damage.damageIcon.GetComponent<UTooltipTrigger>();
                    if (null != damageTrigger)
                    {
                        damageTrigger.Clear();
                        damageTrigger.enabled = false;
                    }
                    damage.damageType.text = "";
                    damage.damageNum.text = "";
                }
            }

            m_MurderIcon.sprite = defaultSprite;
            m_MurderInfoFrame.gameObject.SetActive(false);
            m_TipsInfoFrame.gameObject.SetActive(true);
            m_HurtInfo.text = "";
            m_MurderType.text = "";
            m_MurderDesc.text = "";
        }
    }
}
