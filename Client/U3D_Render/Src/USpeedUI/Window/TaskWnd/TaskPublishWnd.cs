using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using U3D_Render;
using USpeedUI;
using USpeedUI.UWidgets;
using UnityEngine.UI;
using DataCenter;
using UnityEngine;
using UnityEngine.EventSystems;
using GameLogic;
using ASpeedGame.Data.TaskConfig;
using ASpeedGame.Data.ShieldWorld;
using DG.Tweening;
using USpeedUI.UEffect;

namespace USpeedUI.TaskWnd
{
    // 任务发布界面Wnd
    public class TaskPublishWnd : UIBaseWnd<TaskPublishWndView>
    {
        // 获取WndID
        public override WndID GetID()
        {
            return WndID.WND_ID_TASK_PUBLISH;
        }

        // 获取预制体路径
        public override string GetPrefabPath()
        {
            return "Task/TaskPublishView";
        }

        // 获取UI所在层
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();
            m_isKeyClose = true;

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_PUBLISH_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PLAYERINFO_INITINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_PUBLISH_CLOSE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_CLOSE_OTHER, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_PUBLISH_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PLAYERINFO_INITINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_PUBLISH_CLOSE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_CLOSE_OTHER, this);
        }

        // 接收消息
        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_TASK_PUBLISH_SHOW:
                    {
                        SetVisible(true);
                        if (m_wndView != null)
                        {
                            UIMsgCmdData cmdData = (UIMsgCmdData)data;
                            m_wndView.OnShowPublishWnd(cmdData.nParam);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_PLAYERINFO_INITINFO:
                    {
                        if (m_wndView != null)
                            m_wndView.SetDisplayTypeData();
                    }
                    break;
                case WndMsgID.WND_MSG_TASK_PUBLISH_CLOSE:
                    {
                        if (m_wndView != null)
                            m_wndView.OnTaskPublishSuccess();
                    }
                    break;
                case WndMsgID.WND_MSG_TASK_MAIN_CLOSE_OTHER:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnClickCloseBtn();
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        // ESC退出
        public override bool OnCloseKey()
        {
            base.OnCloseKey();

            if (m_wndView != null)
            {
                m_wndView.OnClickCloseBtn();
            }

            return true;
        }
    }

    // 具体选项
    public class SpecificTypeItem : UCustomUIBehaviour
    {
        public Text SpecificTypeDesc;
        public Text PublishDesc;

        private TaskPublishWndView m_wndView;
        private SSchemeContactTaskConfig m_CSpecificConfig;
        public SSchemeContactTaskConfig CSpecificConfig { get { return m_CSpecificConfig; } }
        private long m_llTagID;
        public long llTagID { get { return m_llTagID; } }

        // 关联父节点
        public void SetWndView(TaskPublishWndView view)
        {
            m_wndView = view;
        }

        // 显示数据
        public void SetData(SSchemeContactTaskConfig config, long lWarID = 0)
        {
            m_CSpecificConfig = config;
            m_llTagID = lWarID;

            SpecificTypeDesc.text = config.strSpecificTypeName;
            PublishDesc.text = config.strPublishDescription;

            this.gameObject.SetActive(true);
        }

        // 隐藏数据
        public void Clear()
        {
            this.gameObject.SetActive(false);
        }

        // 点击响应
        public void OnClickItem()
        {
            if(m_wndView != null)
                m_wndView.OnSelectSpecificType(m_CSpecificConfig);
        }
    }

    // 任务发布界面View
    public class TaskPublishWndView : UIBaseWndView
    {
        public Text PublishTip;
        public Text InputCountDesc;
        public Text PublishBtnDesc;

        public Text SubTypeTitle;
        public SpecificTypeItem[] ListSpecificType;
        public UButtonGroup SpecificTypeBtnGroup;

        public GameObject InputFieldGo;
        public Text SpecificTypeTitle;
        public InputField EmotionWordsIF;

        private int m_nSubType;
        private List<SSchemeContactTaskConfig> m_listTaskConfig;

        public Transform LogicFrame;
        public Transform publishAttachPoint;
        private UEffectParamBase publishParam;
        //public float DoScaleTime = 0.7f;

        // 初始化
        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            PublishBtnDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "Publish");
            InputCountDesc.text = "0/40";
            SubTypeTitle.text = "";

            for (int i = 0; i < ListSpecificType.Length; i++)
            {
                ListSpecificType[i].SetWndView(this);
                ListSpecificType[i].Clear();
            }

            return true;
        }

        // 显示发布界面
        public void OnShowPublishWnd(int nSubType)
        {
            m_listTaskConfig = TaskConfig.Instance.GetSubTypeData(nSubType);
            if (m_listTaskConfig == null || m_listTaskConfig.Count <= 0)
                return;

            m_nSubType = nSubType;

            // 炫耀类型特殊处理，带战绩数据
            if (nSubType == (int)ENTASK_SUB_TYPE.ENTASK_DISPLAY)
                SetDisplayTypeData();
            else
                SetNormalTypeData();

            // 子类型名
            SubTypeTitle.text = m_listTaskConfig[0].strSubTypeName;

            // 输入部分隐藏
            InputFieldGo.SetActive(false);
        }

        // 设置炫耀类型数据,需带warID
        public void SetDisplayTypeData()
        {
            if (m_nSubType != (int)ENTASK_SUB_TYPE.ENTASK_DISPLAY)
                return;

            if (EntityFactory.MainHeroView == null)
                return;
            int nPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            ActorPlayerInfo info = LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo(nPDBID);
            if (info.playerInfo.pDbid == 0)
                return;

            cmd_honor_page_info honorInfo;
            honorInfo.arrGlory = info.playerInfo.nGlory;
            honorInfo.llWarID = info.playerInfo.llWarID;

            int nIndex = 0;
            bool bFind = false;// 5杀、4杀、3杀中选择一个
            // 显示
            for (int i = 0; i < m_listTaskConfig.Count && nIndex < ListSpecificType.Length; i++)
            {
                switch (m_listTaskConfig[i].nSpecificType)
                {
                    case (int)ENTASK_SPECIFIC_TYPE.ENTASK_DISPLAY_5KILL:
                        {
                            if (bFind)
                                continue;

                            if (honorInfo.arrGlory[(int)ENACHIEVE.ENACHIEVE_5KILLNUM] > 0 && honorInfo.llWarID[(int)ENACHIEVE.ENACHIEVE_5KILLNUM] > 0)
                            {
                                ListSpecificType[nIndex].SetData(m_listTaskConfig[i], honorInfo.llWarID[(int)ENACHIEVE.ENACHIEVE_5KILLNUM]);
                                nIndex++;

                                bFind = true;
                            }
                        }
                        break;
                    case (int)ENTASK_SPECIFIC_TYPE.ENTASK_DISPLAY_4KILL:
                        {
                            if (bFind)
                                continue;

                            if (honorInfo.arrGlory[(int)ENACHIEVE.ENACHIEVE_4KILLNUM] > 0 && honorInfo.llWarID[(int)ENACHIEVE.ENACHIEVE_4KILLNUM] > 0)
                            {
                                ListSpecificType[nIndex].SetData(m_listTaskConfig[i], honorInfo.llWarID[(int)ENACHIEVE.ENACHIEVE_4KILLNUM]);
                                nIndex++;

                                bFind = true;
                            }
                        }
                        break;
                    case (int)ENTASK_SPECIFIC_TYPE.ENTASK_DISPLAY_3KILL:
                        {
                            if (bFind)
                                continue;

                            if (honorInfo.arrGlory[(int)ENACHIEVE.ENACHIEVE_3KILLNUM] > 0 && honorInfo.llWarID[(int)ENACHIEVE.ENACHIEVE_3KILLNUM] > 0)
                            {
                                ListSpecificType[nIndex].SetData(m_listTaskConfig[i], honorInfo.llWarID[(int)ENACHIEVE.ENACHIEVE_3KILLNUM]);
                                nIndex++;

                                bFind = true;
                            }
                        }
                        break;
                    case (int)ENTASK_SPECIFIC_TYPE.ENTASK_DISPLAY_SuperGod:
                        {
                            if (honorInfo.arrGlory[(int)ENACHIEVE.ENACHIEVE_SUPERGODNUM] > 0 && honorInfo.llWarID[(int)ENACHIEVE.ENACHIEVE_SUPERGODNUM] > 0)
                            {
                                ListSpecificType[nIndex].SetData(m_listTaskConfig[i], honorInfo.llWarID[(int)ENACHIEVE.ENACHIEVE_SUPERGODNUM]);
                                nIndex++;
                            }
                        }
                        break;
                    case (int)ENTASK_SPECIFIC_TYPE.ENTASK_DISPLAY_MVP:
                        {
                            if (honorInfo.arrGlory[(int)ENACHIEVE.ENACHIEVE_MVPNUM] > 0 && honorInfo.llWarID[(int)ENACHIEVE.ENACHIEVE_MVPNUM] > 0)
                            {
                                ListSpecificType[nIndex].SetData(m_listTaskConfig[i], honorInfo.llWarID[(int)ENACHIEVE.ENACHIEVE_MVPNUM]);
                                nIndex++;
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
            // 隐藏
            if (nIndex < ListSpecificType.Length)
            {
                for (int i = nIndex; i < ListSpecificType.Length; i++)
                {
                    ListSpecificType[i].Clear();
                }
            }
            // 发布说明
            if (nIndex == 0)
            {
                PublishTip.text = ULocalizationService.Instance.Get("UIView", "Task", "NoMatchPublishType");
            }
            else
            {
                PublishTip.text = ULocalizationService.Instance.Get("UIView", "Task", "SelectSubType");
            }
        }

        // 设置普通类型数据
        private void SetNormalTypeData()
        {
            for (int i = 0; i < ListSpecificType.Length; i++)
            {
                if (i < m_listTaskConfig.Count)
                {
                    ListSpecificType[i].SetData(m_listTaskConfig[i]);
                }
                else
                {
                    ListSpecificType[i].Clear();
                }
            }

            // 发布说明
            if (ListSpecificType.Length <= 0)
            {
                PublishTip.text = ULocalizationService.Instance.Get("UIView", "Task", "NoMatchPublishType");
            }
            else
            {
                PublishTip.text = ULocalizationService.Instance.Get("UIView", "Task", "SelectSubType");
            }
        }

        // 点击具体任务响应
        public void OnSelectSpecificType(SSchemeContactTaskConfig config)
        {
            PublishTip.text = config.strPublishDescription;
            SpecificTypeTitle.text = config.strSpecificTypeName;
            EmotionWordsIF.placeholder.GetComponent<Text>().text = config.strEmotionWords;
            InputFieldGo.gameObject.SetActive(true);
        }

        // 发布按钮响应
        public void OnClickPublishBtn()
        {
            int nSelect = SpecificTypeBtnGroup.SelectedIndex;
            if (nSelect < 0 || nSelect >= ListSpecificType.Length)
                return;

            string strEmotionWords = String.IsNullOrEmpty(EmotionWordsIF.text) ? EmotionWordsIF.placeholder.GetComponent<Text>().text : EmotionWordsIF.text;
            if (ShieldWord.Instance.filterShieldWord(ref strEmotionWords))
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LUA_TASK_EMOTION_CONTAIN_SHIELD_WORD);
                return;
            }

            cmd_task_publish_v2c cmdData;
            cmdData.nSubType = m_nSubType;
            cmdData.nSpecificType = ListSpecificType[nSelect].CSpecificConfig.nSpecificType;
            cmdData.szEmotionWords = strEmotionWords;
            cmdData.llTagID = ListSpecificType[nSelect].llTagID;
            ViewEventHelper.Instance.SendCommand<cmd_task_publish_v2c>(GameLogicDef.GVIEWCMD_PUBLISH_TASK, ref cmdData);

            EmotionWordsIF.text = "";
        }

        // 输入响应
        public void OnInputChange()
        {
            InputCountDesc.text = String.Format("{0}/40", EmotionWordsIF.text.Length);
        }

        // 关闭按钮
        public void OnClickCloseBtn()
        {
            SpecificTypeBtnGroup.SetAllItemDeSelected();
            SetVisible(false);

            // 删除光效
            if (publishParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref publishParam);
            }
        }

        // 子关闭按钮
        public void OnClickSubCloseBtn()
        {
            SpecificTypeBtnGroup.SetAllItemDeSelected();
            InputFieldGo.gameObject.SetActive(false);
        }

        // 发布成功
        public void OnTaskPublishSuccess()
        {
            Sequence logicSequence = DOTween.Sequence();
            logicSequence.Append(LogicFrame.DOScale(0.0f, 0.4f)).AppendInterval(0.6f).OnComplete(OnSequenceComplete);

            // 光效
            if (publishParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref publishParam);
            }
            publishParam = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_TaskPublish_Send, _tfAttachParent: publishAttachPoint, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref publishParam);
        }

        // 动画结束回调
        private void OnSequenceComplete()
        {
            // 关闭
            OnClickCloseBtn();
            // 还原
            LogicFrame.localScale = new Vector3(1.0f, 1.0f, 1.0f);
        }

    }
}
