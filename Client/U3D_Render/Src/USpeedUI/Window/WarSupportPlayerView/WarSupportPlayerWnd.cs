/*******************************************************************
** 文件名:	WarSupportPlayerWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-12-19
** 版  本:	1.0
** 描  述:	点赞界面
		
********************************************************************/
using System.Collections.Generic;
using GameLogic;
using System;
using System.Runtime.InteropServices;
using DataCenter;
using USpeedUI.TooltipAndDialog;
using UnityEngine;
using System.Collections;
using U3D_Render;
using UnityEngine.UI;
using ASpeedGame.Data.WarSupportPlayerConfig;
using ASpeedGame.Data.PersonModelTransFormInfo;
using USpeedUI.UExtensions;
using Effect;
using UnityEngine.EventSystems;
using DG.Tweening;
using Data.UISoundConfig;

namespace USpeedUI.WarSupportPlayer
{
	public class WarSupportPlayerWnd : UIFillWnd<WarSupportPlayerView>
	{
		// 获取ID
		public override WndID GetID()
		{
			return WndID.WND_ID_WARSURPOORTPLAYERVIEW;
		}

		// 获取资源路径
		public override String GetPrefabPath()
		{
			return "DynamicState_Moba/WarSupportPlayerView/WarSupportPlayerView";
		}

		// 获取所在层次
		public override WndLayerDef GetLayer()
		{
			return WndLayerDef.WND_LAYER_FILL;
		}

		// 初始化
		public override bool Init()
		{
			base.Init();

			// 订阅聊天消息更新消息
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_FILTER_SUPPORT_PLAYER, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_DISABLE_ALL_VOTE_BUTTON, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_HAS_ADD_LABEL, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_SHOW_SUPPORT_WND, this);
			UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
			return true;
		}

		// 销毁
		public override void Destroy()
		{
			base.Destroy();
			// 退订聊天消息更新消息
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_FILTER_SUPPORT_PLAYER, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_DISABLE_ALL_VOTE_BUTTON, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_HAS_ADD_LABEL, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_SHOW_SUPPORT_WND, this);
			UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
		}
        
        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
		{
			switch (msgID)
			{
				case WndMsgID.WND_MSG_WAR_FILTER_SUPPORT_PLAYER:
					{
						if (m_wndView == null)
						{
							this.LoadView();
						}

						if (m_wndView == null || m_wndView.SupportVoteFrame == null)
						{
							return;
						}
						else
						{
							m_wndView.SupportVoteFrame.UpdateSubFrame();
						}
					}
					break;
				case WndMsgID.WND_MSG_WAR_SHOW_SUPPORT_WND:
					{
                        //刷新摄像机状态
                        SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.EndWarControl, false);
                        // 激活输入
                        InputManager.Available = true;

						/// 判断是否显示点赞界面
						bool isEnableSupport = Data.WarSubFuncConfig.WarSubFuncConfig.Instance.isEnableWarSubFunc(
							(int)GameLogicAPI.getCurRoomMatchType(), 
							Data.WarSubFuncConfig.EWarSubFuncType.EWarSubFuncType_Support);

						// 判断点赞台人数是否为0
						int nCount = LogicDataCenter.labelDataManager.GetWarPlayersSupportCount();
						if (nCount == 0)
							isEnableSupport = false;

						if(isEnableSupport)
						{
							// 开启点赞界面
							this.SetVisible(true);
						}
						else
						{
							// 跳过点赞界面,直接显示战绩面板
							UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_SHOW, null);
						}

						// 聊天框下移
						UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_MOVEPOSDOWN, null);
					}
					break;
				case WndMsgID.WND_MSG_WAR_DISABLE_ALL_VOTE_BUTTON:
					{
						if (m_wndView != null)
							m_wndView.DisableAllVoteButton();
					}
					break;
				case WndMsgID.WND_MSG_WAR_HAS_ADD_LABEL:
					{
						if (m_wndView != null)
							m_wndView.onAddLabel(msgData as UAddLabelMsgData);
					}
					break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        EndWndView();
                    }
                    break;
                default:
					break;
			}
		}

		public override bool OnCloseKey()
		{
			return false;
		}


		protected override void PostSetVisible(bool _bVisible)
		{
			base.PostSetVisible(_bVisible);
			if (_bVisible)
            {
                SoundPlayHandler.PlaySceneSound((int)EMBGMRule.SupportPlayer);
                SceneManager.Instance.LoadWarSupportPlayerViewScene(SetRoomShowAfterLoadScene);
			}
		}

		public void SetRoomShowAfterLoadScene(SceneResInfoManager info)
		{
			if (m_wndView && m_wndView.SupportVoteFrame)
			{
				m_wndView.SupportVoteFrame.SelectTransForm = info.SceneRoot.transform;
			}
		}

		public void EndWndView()
		{
            Debug.Log(String.Format("EndWndView,卸载点赞场景,{0},{1}", Time.time, m_isVisible));

            if(m_isVisible)
            {
                // 清空本局标签数据
                LogicDataCenter.labelDataManager.ClearMobaData();

                this.UnloadView();

                SceneManager.Instance.UnLoadWarSupportPlayerViewScene();
            }
        }
	}

	public class WarEndSupportVotePersonItem : MonoBehaviour
	{
		private enum EEffectType
		{
			//EEY_BASE_1,
			//EEY_BASE_2,
			EEY_SUPPORT,
			EEY_FIVEPOINT,
			EEY_TENPOINT,

			EEY_MAX,
		}

		private uint m_PDBIU;
		public Image VotePointImage;
		public Image TitleImage;
		public Text VoteDescText;
		public Text PlayerNameText;
		public Button VoteButton;
		public GameObject DefaultLabelEffectText;

		private WarEndSupportVoteSubFrame parentsView;
		private GameObject[] m_EffectObjList = new GameObject[(int)EEffectType.EEY_MAX];
		private string[,] m_sEffectNameList = new string[2, (int)EEffectType.EEY_MAX]{
			{/*"ef_Thumbs_up RoomView_01_pt_blue01","ef_Thumbs_up RoomView_02_pt_blue02",*/"ef_Thumbs_up_RoomView_05_dianji_blue","ef_Thumbs_up_RoomView_08_Five_blue","ef_Thumbs_up_RoomView_09_Ten_blue"},
			{/*"ef_Thumbs_up RoomView_03_pt_red01","ef_Thumbs_up RoomView_04_pt_red02",*/"ef_Thumbs_up_RoomView_06_dianji_red","ef_Thumbs_up_RoomView_07_Five_red","ef_Thumbs_up_RoomView_10_Ten_red"}
		};

		public void ClearItem()
		{
			m_PDBIU = 0;
			VoteDescText.text = "未配置";
		}

		public void OnDestroy()
		{
			if (m_EffectObjList != null)
			{
				foreach (GameObject item in m_EffectObjList)
				{
					GameObject temp = item;
					if (item != null)
						ResNode.DestroyRes(ref temp);
				}
			}

			VotePointImage.sprite = null;
			VotePointImage.gameObject.SetActive(false);

			TitleImage.sprite = null;
			TitleImage.gameObject.SetActive(false);

			VoteDescText.text = "未配置";
			PlayerNameText.text = "";


			WarLabelSelectItem labelItem = VoteButton.GetComponent<WarLabelSelectItem>();
			labelItem.Clear();

			VoteButton.gameObject.SetActive(false);
		}

		public void SetData(cmd_update_support_vote_person data, int nPlayerCount)
		{
			if (data.uPDBIU <= 0)
				return;

			// 初始化标签轮盘
			WarLabelSelectItem labelItem = VoteButton.GetComponent<WarLabelSelectItem>();
			if (labelItem != null && labelItem.gameObject.activeSelf)
			{
				labelItem.Init(data.nVoteID, data.uPDBIU);
			}

			m_PDBIU = data.uPDBIU;

			// 计算玩家本次的点赞总数
			int nSupportNum = LogicDataCenter.labelDataManager.getPlayerSupportNum(m_PDBIU);
			if (parentsView)
			{
				VotePointImage.sprite = parentsView.GetSupportNumSprite(nSupportNum);
			}

            cmd_war_add_label lastLabel;
            int nLabelCount;
            // 当前玩家是否最新被点赞的玩家
            bool isLastPlayer = LogicDataCenter.labelDataManager.getLastPlayerLabel(m_PDBIU, out lastLabel, out nLabelCount);
            if (isLastPlayer)
            {
                // 播放标签特效
                loadLabelEffect(lastLabel,nLabelCount);
            }

			VotePointImage.SetNativeSize();
			VotePointImage.gameObject.SetActive(VotePointImage.sprite);
			SSchemeWarSupportPlayerConfig config = WarSupportPlayerConfigPage.Instance.GetWarSupportPlayerConfig(data.nVoteID);
			if (config != null)
			{
				VoteDescText.text = config.sVoteDesc;
				TitleImage.sprite = parentsView.GetSupportTitleSprite(config.nVoteIconID);
				TitleImage.gameObject.SetActive(TitleImage.sprite);
			}
            else
            {
                Debug.LogError(String.Format("GetWarSupportPlayerConfig failed. nVoteID={0},pdbid={1}", data.nVoteID, data.uPDBIU));
            }

			DataCenter.WarRecordTableDataManager.WarPersonInfo info = LogicDataCenter.recordTableDataManager.getWarPersonInfoByPDBID(data.uPDBIU);
			if (info == null)
				return;

			PlayerNameText.text = info.playerName;
			bool bSameCamp = info.nCamp == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP);
			string szColor = bSameCamp ? "7BDBEDFF" : "FF3434FF";
			string szEffectColor = bSameCamp ? "0072FF99" : "92000099";
			Color32 color = ColorExtension.FromHex(szColor);
			Color32 effectcolor = ColorExtension.FromHex(szEffectColor);
			PlayerNameText.color = color;
			PlayerNameText.gameObject.GetComponent<Outline>().effectColor = effectcolor;
			SSchemeWarSupportPositionConfig positionConfig = WarSupportPlayerConfigPage.Instance.GetWarSupportPositionConfig(data.nSortID, nPlayerCount);
			if (positionConfig == null)
				return;

			Vector3 oldAnchoredPosition = this.GetComponent<RectTransform>().anchoredPosition;
			this.GetComponent<RectTransform>().anchoredPosition = new Vector3(positionConfig.fUIPositionOffsetX, oldAnchoredPosition.y, oldAnchoredPosition.z);

            // 播放光效
            if (isLastPlayer)
            {
			    DoEffect(m_PDBIU, bSameCamp, nSupportNum, data.nSortID);
            }

            // 播放音效
            if (isLastPlayer)
            {
                DoSound(nSupportNum);
            }
		}



		public void setParents(WarEndSupportVoteSubFrame warEndSupportVoteSubFrame)
		{
			parentsView = warEndSupportVoteSubFrame;
		}

		public void DisableVoteButton()
		{
			VoteButton.gameObject.SetActive(false);
		}

		private void DoSound(int nVotePoint)
		{
			string strSoundKey = "";
			if(nVotePoint == 5)
			{
				strSoundKey = "FivePoint";
			}
			else if(nVotePoint == 10)
			{
				strSoundKey = "TenPoint";
			}

			if(strSoundKey.Length > 0)
			{
				SSchemeUSoundData soundData = new SSchemeUSoundData();
				if (UISoundConfig.Instance.GetData("support", strSoundKey, ref soundData))
				{
					SoundManager.CreateUISound(soundData.USoundID);
				}
			}
		}


		private void DoEffect(uint nCurPdbid, bool bSameCamp, int nVotePoint, int nSortID = 0)
		{
            if (m_EffectObjList == null)
				return;

			if (parentsView == null)
				return;

			if (parentsView.SelectTransForm == null)
				return;

			int nCampIndex = bSameCamp ? 0 : 1;
			//if (m_EffectObjList[(int)EEffectType.EEY_BASE_1] == null && nSortID == 1)
				//LoadEffect(ref m_EffectObjList[(int)EEffectType.EEY_BASE_1], m_sEffectNameList[nCampIndex, (int)EEffectType.EEY_BASE_1], nSortID);

			//if (m_EffectObjList[(int)EEffectType.EEY_BASE_2] == null && nSortID > 1)
				//LoadEffect(ref m_EffectObjList[(int)EEffectType.EEY_BASE_2], m_sEffectNameList[nCampIndex, (int)EEffectType.EEY_BASE_2], nSortID);

			if (m_EffectObjList[(int)EEffectType.EEY_SUPPORT] == null && nVotePoint >= 1)
				LoadEffect(ref m_EffectObjList[(int)EEffectType.EEY_SUPPORT], m_sEffectNameList[nCampIndex, (int)EEffectType.EEY_SUPPORT], nSortID);
            else if (m_EffectObjList[(int)EEffectType.EEY_SUPPORT] != null)
                m_EffectObjList[(int)EEffectType.EEY_SUPPORT].GetComponent<EffectNode>().ReplayEffect();

            if (m_EffectObjList[(int)EEffectType.EEY_FIVEPOINT] == null && nVotePoint >= 5)
				LoadEffect(ref m_EffectObjList[(int)EEffectType.EEY_FIVEPOINT], m_sEffectNameList[nCampIndex, (int)EEffectType.EEY_FIVEPOINT], nSortID);

			if (m_EffectObjList[(int)EEffectType.EEY_TENPOINT] == null && nVotePoint >= 10)
				LoadEffect(ref m_EffectObjList[(int)EEffectType.EEY_TENPOINT], m_sEffectNameList[nCampIndex, (int)EEffectType.EEY_TENPOINT], nSortID);
		}

		private void LoadEffect(ref GameObject effectObject, string sEffectName, int nSortID = 0)
		{
			if (parentsView == null)
				return;

			if (parentsView.SelectTransForm == null)
				return;

			if (nSortID <= 0)
				return;

			ResNode itemNode = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "Effect/Scene/UI/StaticState/Thumbs_up_RoomView/" + sEffectName);
			if (itemNode == null)
				return;


			effectObject = itemNode.InstanceMainRes();
			if (effectObject == null)
			{
				return;
			}

			effectObject.transform.SetParent(parentsView.SelectTransForm, false);
			effectObject.transform.transform.localPosition = GetEffectPosition(nSortID);
		}

		private Vector3 GetEffectPosition(int nSortID)
		{
			float fOffsetY_1 = 0f;
			float fOffsetX_1 = 2.5f;
			float fOffsetX_2 = 4.63f;
			switch (nSortID)
			{
				case 1:
					return new Vector3(0, 0, 0);
				case 2:
					return new Vector3(fOffsetX_1, fOffsetY_1, 0);
				case 3:
					return new Vector3(-fOffsetX_1, fOffsetY_1, 0);
				case 4:
					return new Vector3(fOffsetX_2, fOffsetY_1, 0); ;
				case 5:
					return new Vector3(-fOffsetX_2, fOffsetY_1, 0);
			}

			return new Vector3(0, 0, 0);
		}

		public void loadLabelEffect(cmd_war_add_label label, int nLabelCount)
		{
			SSchemeLabelConfig labelConfig = WarSupportPlayerConfigPage.Instance.GetLabelConfig(label.nLabelID);
			if (labelConfig == null)
				return;

			// 格式：“标签名称 +2”
			string labelStr = labelConfig.labelName + " +" + nLabelCount;

			Text labelEffectText = ResNode.InstantiateRes<Text>(DefaultLabelEffectText);
			labelEffectText.gameObject.SetActive(true);
			labelEffectText.gameObject.transform.SetParent(DefaultLabelEffectText.transform.parent, false);

			labelEffectText.text = labelStr;

			// 2s后变透明
			DOTween.ToAlpha(() => labelEffectText.color, x => labelEffectText.color = x, 0f, 2f).Play();
			// 2s后升高100像素
			labelEffectText.transform.DOBlendableLocalMoveBy(new Vector3(0f, 200f, 0), 2f).OnComplete(()=>
            {
                var temp = labelEffectText.gameObject;
                ResNode.DestroyRes(ref temp);
			});
		}
	}

	public class WarEndSupportVoteSubFrame : MonoBehaviour, ITimerHanlder
	{
        // 显示点赞模型数据
        struct SSetSKinModelInfo
        {
            public int nHeroID;
            public int nSkinID;
            public int nSortID;
        };

        public Transform SlotTrans;
		public GameObject TmpSlotItemObj;
		public Sprite[] NumImageList;
		public Sprite[] TitleImageList;
		public Text TimeDownTextFloat;
		public Text TimeDownText;
		public Button ContinuBtn;

		public CanvasGroup showFrame;
		public int TimeCount = 20;
		public int ShowContinueBtnTime = 10;
		public float FloatTextFadeTime = 1f;
		public float FloatTextMoveX = 700f;
		public float ShowFrameFadeTime = 1.5f;

		private WarEndSupportVotePersonItem[] SlotItemList;

		private Transform m_SelectHeroTransform;
		private SkinInstance[] m_LastSkinObject;
		private int[] m_LastSkinID;
		private bool m_bLoadSkin = false;
		private WarSupportPlayerView parentView;


		private string m_TimeCountStr;

		private Sequence TimeDownTextAnim = null;

        private bool isPlayAnim = false;

		public Transform SelectTransForm
		{
			set
			{
				m_SelectHeroTransform = value;
				UpdateSlotList();
			}
			get { return m_SelectHeroTransform; }
		}

		public void Awake()
		{
			if (TmpSlotItemObj)
				TmpSlotItemObj.SetActive(false);

			m_TimeCountStr = ULocalizationService.Instance.Get("UIView", "Support", "CountDownText");
			TimeDownText.text = string.Format(m_TimeCountStr, TimeCount);
			TimeDownTextFloat.text = string.Format(m_TimeCountStr, TimeCount);

			string continueBtnDesc = ULocalizationService.Instance.Get("UIView", "Support", "ContinueBtnDesc");
			ContinuBtn.transform.FindChild("Text").GetComponent<Text>().text = continueBtnDesc;

			// 浮动的倒计时文字先设置为透明
			Color clr = TimeDownTextFloat.color;
			clr.a = 0;
			TimeDownTextFloat.color = clr;

			// 设置为不可见且透明
			showFrame.gameObject.SetActive(false);
			showFrame.alpha = 0f;
		}

		public void OnEnable()
		{
			TimerManager.SetTimer(this, 1, 1.0f, TimeCount);

			TimeDownTextAnim = DOTween.Sequence();

            TimeDownTextAnim.Append(TimeDownTextFloat.transform.DOLocalMoveX(0, FloatTextFadeTime))
                .Insert(0, TimeDownTextFloat.DOFade(1, FloatTextFadeTime));
			TimeDownTextAnim.Play();
		}

		public void OnDisable()
		{
			TimerManager.KillTimer(this, 1);
		}

		public void OnDestroy()
		{
			TimerManager.KillTimer(this, 1);

			if(SlotItemList != null)
			{
				foreach(WarEndSupportVotePersonItem item in SlotItemList)
				{
					item.OnDestroy();
				}

				SlotItemList = null;
			}

			if (TimeDownTextAnim != null)
			{
				TimeDownTextAnim.Kill();
				TimeDownTextAnim = null;
			}
		}

		public void OnTimer(int nTimerID)
		{
			if (nTimerID == 1)	// 点赞倒计时计时器
			{
				TimeCount--;

				if (TimeCount <= 0)
					OnGoWarRecordViewButttonClick();

				TimeDownText.text = string.Format(m_TimeCountStr, TimeCount);
				TimeDownTextFloat.text = string.Format(m_TimeCountStr, TimeCount);

                if(!isPlayAnim && TimeCount <= ShowContinueBtnTime)
                {
                    isPlayAnim = true;

                    TimeDownTextAnim = DOTween.Sequence();
                    TimeDownTextAnim.Append(TimeDownTextFloat.transform.DOLocalMoveX(-FloatTextMoveX, FloatTextFadeTime))
                    	.Insert(0, TimeDownTextFloat.DOFade(0, FloatTextFadeTime)).OnKill(() =>
                    	{
                    		showFrame.gameObject.SetActive(true);
                    		showFrame.DOFade(1, ShowFrameFadeTime);
                    	});
                }
            }
		}

		public void Update()
		{
			if (m_SelectHeroTransform != null && m_bLoadSkin == false)
				UpdateSkinList();
		}

		public void Destroy()
		{
			ClearSelectSkinModel();
			clearAllSupportItem();
		}

		public void UpdateSubFrame()
		{
			UpdateSlotList();
		}

		private void UpdateSlotList()
		{
			DoInitSlotList();

			if (SlotItemList == null)
				return;

			int nSlotID = 0;
			Dictionary<ulong, cmd_update_support_vote_person> playersSupportInfo = LogicDataCenter.labelDataManager.GetWarPlayersSupportInfo();
			int nPlayerCount = playersSupportInfo.Count;
			foreach (cmd_update_support_vote_person voteItem in playersSupportInfo.Values)
			{
				if (nSlotID < 0 || nSlotID >= SlotItemList.Length)
					continue;

				SlotItemList[nSlotID].SetData(voteItem, nPlayerCount);
				nSlotID++;
			}
		}

		private void UpdateSkinList()
		{
			if (m_LastSkinID != null && m_LastSkinID[0] > 0)
				return;

			m_bLoadSkin = true;
			Dictionary<ulong, cmd_update_support_vote_person> playersSupportInfo = LogicDataCenter.labelDataManager.GetWarPlayersSupportInfo();
			int nPlayerCount = playersSupportInfo.Count;

            SSetSKinModelInfo[] heroInfoList = new SSetSKinModelInfo[nPlayerCount];
			int nSlotID = 0;
			foreach (cmd_update_support_vote_person voteItem in playersSupportInfo.Values)
			{
				if (nSlotID < 0 || nSlotID >= heroInfoList.Length)
					continue;

				DataCenter.WarRecordTableDataManager.WarPersonInfo info = LogicDataCenter.recordTableDataManager.getWarPersonInfoByPDBID(voteItem.uPDBIU);
				if (info == null)
					continue;

                heroInfoList[nSlotID].nSortID = voteItem.nSortID;
                heroInfoList[nSlotID].nHeroID = info.nHeroId;
                heroInfoList[nSlotID].nSkinID = info.nSkinID;

                nSlotID++;
            }

			setHeroSkinModel(heroInfoList);
		}

		public void DoInitSlotList()
		{
			if (SlotItemList != null)
				return;

			TmpSlotItemObj.SetActive(false);

			int nPlayerCount = LogicDataCenter.labelDataManager.GetWarPlayersSupportCount();
			if (nPlayerCount <= 0)
				return;

			SlotItemList = new WarEndSupportVotePersonItem[nPlayerCount];
			for (int i = 0; i < nPlayerCount; ++i)
			{
				GameObject SlotItemObject = ResNode.InstantiateRes(TmpSlotItemObj);
				if (SlotItemObject == null)
				{
					return;
				}
				WarEndSupportVotePersonItem item = SlotItemObject.GetComponent<WarEndSupportVotePersonItem>();
				if (item == null)
				{
					return;
				}

				item.setParents(this);
				item.ClearItem();

				SlotItemObject.SetActive(true);
				SlotItemList[i] = item;
				SlotItemObject.transform.SetParent(SlotTrans, false);
			}
		}

		private void ClearSelectSkinModel()
		{
			if (m_LastSkinObject == null)
			{
				return;
			}

			for (int i = 0; i < m_LastSkinObject.Length; ++i)
			{
                SkinManager.DeleteSkin(ref m_LastSkinObject[i]);
			}

			m_LastSkinID = null;
			m_LastSkinObject = null;

		}

		private void clearAllSupportItem()
		{
			List<GameObject> goList = new List<GameObject>();
			for (int i = 0; i < SlotTrans.childCount; i++)
			{
				goList.Add(SlotTrans.GetChild(i).gameObject);
			}
			foreach(var go in goList)
			{
                var temp = go;
                ResNode.DestroyRes(ref temp);
			}
		}

		private void setHeroSkinModel(SSetSKinModelInfo[] heroInfoList)
		{
			if (SelectTransForm == null)
				return;

            int nCount = heroInfoList.Length;
			m_LastSkinID = new int[heroInfoList.Length];
			m_LastSkinObject = new SkinInstance[heroInfoList.Length];

			int nListIndex = 0;
			foreach (var item in heroInfoList)
			{
                PersonSkinModelItem modelData = new PersonSkinModelItem();
                if (GameLogicAPI.GetPersonModel((UInt16)item.nHeroID, ref modelData))
                {
                    int nSkinID = item.nSkinID;
                    if(nSkinID <= 0)
                    {
                        Debug.LogWarning("skinID <= 0, nHeroID=" + item.nHeroID);

                        nSkinID = modelData.nDefaultSkinID;
                    }

					SkinManager.SetChangeHeroSkin(nSkinID, item.nHeroID, ref m_LastSkinObject[nListIndex], ref m_LastSkinID[nListIndex], SelectTransForm, HEROSKIN_RENDER_TYPE.SCENE, PERSONMODELVIEWID.PERSONMODELVIEWID_WARSUPPORT);
                    if (m_LastSkinObject[nListIndex] != null && m_LastSkinObject[nListIndex].isValid)
                    {
                        SetSkinItemModelPosition(m_LastSkinObject[nListIndex].SkinObj, item.nSortID, nCount);
                    }
		
				}

				nListIndex++;
			}
		}

		private void SetSkinItemModelPosition(GameObject gameObject, int nSortID, int nPlayerCount)
		{
			SSchemeWarSupportPositionConfig positionConfig = WarSupportPlayerConfigPage.Instance.GetWarSupportPositionConfig(nSortID, nPlayerCount);
			if (positionConfig == null)
				return;

			if (gameObject == null)
			{
				Debug.LogError("gameObject == null");
				return;
			}

			Vector3 oldPosition = gameObject.transform.localPosition;
			gameObject.transform.localPosition = new Vector3(positionConfig.playerOffsetVec3.x, oldPosition.y + positionConfig.playerOffsetVec3.y, oldPosition.z + positionConfig.playerOffsetVec3.z);
		}

		public void OnGoWarRecordViewButttonClick()
		{
            if (parentView)
            {
                parentView.EndWndView();
            }
            else
            {
                Debug.LogError("parentView == null");
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_RECORDTABLE_END_SHOW, null);
        }

        public void SetParentView(WarSupportPlayerView warSupportPlayerView)
		{
			parentView = warSupportPlayerView;
		}

		public Sprite GetSupportNumSprite(int nNum)
		{
			if (NumImageList == null)
				return null;

			nNum -= 1;
			if (nNum < 0 || nNum >= NumImageList.Length)
				return null;

			return NumImageList[nNum];
		}

		public Sprite GetSupportTitleSprite(int nIndex)
		{
			return USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_WarResultShow, "WarAchiType_" + nIndex, WndID.WND_ID_WARSURPOORTPLAYERVIEW);
		}

		public void DisableAllVoteButton()
		{
			if (SlotItemList == null)
				return;
			foreach (WarEndSupportVotePersonItem item in SlotItemList)
			{
				if (item == null)
					continue;

				item.DisableVoteButton();
			}
		}
	}

	public class WarSupportPlayerView : UIBaseWndView
	{
        public WarEndSupportVoteSubFrame SupportVoteFrame;

		private WarSupportPlayerWnd m_SupportWnd;
        public override void Start()
		{
			return;
		}

		public override bool Init(IUIWnd wnd)
		{
			base.Init(wnd);

			m_SupportWnd = wnd as WarSupportPlayerWnd;
			SupportVoteFrame.SetParentView(this);

			return true;
		}

        public override void Destroy()
		{
			SupportVoteFrame.OnDestroy();
		}

		public void EndWndView()
		{
			if (m_SupportWnd == null)
				return;

			m_SupportWnd.EndWndView();
		}

		public void DisableAllVoteButton()
		{
			if (SupportVoteFrame != null)
				SupportVoteFrame.DisableAllVoteButton();
		}

		public void onAddLabel(UAddLabelMsgData msgData)
		{
			if (msgData == null)
				return;

			if(SupportVoteFrame != null)
				SupportVoteFrame.UpdateSubFrame();

			SSchemeLabelConfig labelConfig = WarSupportPlayerConfigPage.Instance.GetLabelConfig(msgData.nLabelID);
			if (labelConfig == null)
				return;

			// 播放广播
			string strFormat = ULocalizationService.Instance.Get("UIView", "Support", "AddLabelMsg");
			string strMsg = string.Format(strFormat, ChatMessageManager.ChatItemFlag, msgData.operatorName,
				ChatMessageManager.ChatItemFlag, msgData.targetName, ChatMessageManager.ChatItemFlag, labelConfig.labelName);

			gamelogic_show_chat_message msg = new gamelogic_show_chat_message();
			msg.channel = (int)EMChatChannelType.CHAT_CHANNEL_WARSCENE;
			msg.message = strMsg;
			LogicDataCenter.chatMessageManager.AddChatMessage(ref msg);

		}
	}

	public class WarLabelSelectItem : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler 
	{
		public Text[] labelTextList;
		public GameObject LabelSelect;
		public Image highlightImage;
		public Image mouseLineImage;
		public RectTransform ClickEffect;

		// 是否显示出标签轮盘
		private bool m_isShowLabel;

		// 点赞按钮的其实位置(屏幕坐标)
		private Vector3 m_startPos;

		// 当前选择的标签
		private int m_curLabel;

		// 标签数量
		private int m_LabelNum;

		// 每个标签的角度
		private float m_LabelAngler;

		// 高亮图片偏移
		private const float m_highlightImageOffset = -60f;

		// 需要上标签的玩家
		private uint m_Pdbid;

		private int[] m_curLabelList;

		// 是否初始化了
		private bool m_isInit;

		public void Init(int nVoteID, uint pdbID)
		{
			if (pdbID <= 0 || m_isInit)
				return;

			m_Pdbid = pdbID;

			m_isShowLabel = false;
			m_curLabel = 0;

			// 获取点赞信息脚本
			SSchemeWarSupportPlayerConfig sConfig = WarSupportPlayerConfigPage.Instance.GetWarSupportPlayerConfig(nVoteID);
			if (sConfig == null)
				return;

			if(sConfig.labelIDList.Length != labelTextList.Length)
			{
				Debug.LogError("UI标签个数和配置表不一致");
				return;
			}

			for (int i = 0; i < labelTextList.Length; ++i)
			{
				Text label = labelTextList[i];
				if (label == null)
					continue;

				// 根据标签ID获取标签信息
				int nLabelID = sConfig.labelIDList[i];
				SSchemeLabelConfig labelConfig = WarSupportPlayerConfigPage.Instance.GetLabelConfig(nLabelID);
				if (labelConfig == null)
					continue;

				label.text = labelConfig.labelName;
			}

			m_curLabelList = sConfig.labelIDList;

			m_startPos = RectTransformUtility.WorldToScreenPoint(UISystem.Instance.GetCamera(), transform.position);

			m_LabelNum = labelTextList.Length;

			m_LabelAngler = 360f / m_LabelNum;

			m_isInit = true;

            if (StageManager.Instance.CurPermissionState == EStage_PermissionState.ESPS_OB)
            {
                gameObject.SetActive(false);
            }

        }

        public void Clear()
		{
			m_isInit = false;
		}

		public void Update()
		{
			if (m_isShowLabel)
			{
				// 计算鼠标与点赞按钮的角度
				Vector2 vec = InputManager.MousePosition - m_startPos;
				float rot = (float)Math.Atan2(vec.y, vec.x);
				float angles = rot * Mathf.Rad2Deg;
				if (angles < 0)
					angles += 360;

				int newLabel = (int)(angles / m_LabelAngler);

				if(newLabel != m_curLabel)
				{
					Vector3 newAngles = new Vector3(0f, 0f, (int)newLabel * m_LabelAngler + m_highlightImageOffset);
					m_curLabel = newLabel;
					highlightImage.rectTransform.eulerAngles = newAngles;

					ClickEffect.eulerAngles = newAngles;

                    // 播放划过音效
                    SSchemeUSoundData soundData = new SSchemeUSoundData();
                    if (UISoundConfig.Instance.GetData("support", "Support_slide", ref soundData))
                    {
                        SoundManager.CreateUISound(soundData.USoundID);
                    }
                }
			}
		}

		public void OnPointerEnter(PointerEventData eventData)
		{
			if(!m_isShowLabel)
			{
				LabelSelect.SetActive(true);
				gameObject.GetComponent<Image>().raycastTarget = false;
				LabelSelect.gameObject.GetComponent<Image>().raycastTarget = true;

				m_startPos = RectTransformUtility.WorldToScreenPoint(UISystem.Instance.GetCamera(), transform.position);
				m_isShowLabel = true;

			}
		}

		public void OnPointerExit(PointerEventData eventData)
		{
			if (m_isShowLabel)
			{
				LabelSelect.SetActive(false);
				gameObject.GetComponent<Image>().raycastTarget = true;
				LabelSelect.gameObject.GetComponent<Image>().raycastTarget = false;

				m_isShowLabel = false;

			}
		}

		public void onClick()
		{
			if (m_Pdbid <= 0 || !m_isShowLabel || m_curLabelList == null)
				return;
			
			// 显示点击特效
			ClickEffect.gameObject.SetActive(true);

            // 播放点击音效
            SSchemeUSoundData soundData = new SSchemeUSoundData();
            if (UISoundConfig.Instance.GetData("support", "Support_click", ref soundData))
            {
                SoundManager.CreateUISound(soundData.USoundID);
            }

            // 延迟隐藏点赞按钮,等点击特效播放
            StartCoroutine(delayHideAllVoteBtn());

			int nCurLabelID = m_curLabelList[m_curLabel];

			cmd_war_add_label msgInfo = new cmd_war_add_label();
			msgInfo.targetPdbid = m_Pdbid;
			msgInfo.nLabelID = nCurLabelID;


			EntityEventHelper.Instance.SendCommand<cmd_war_add_label>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_WAR_ADD_LABEL, ref msgInfo);
		}

		private IEnumerator delayHideAllVoteBtn()
		{
			yield return new WaitForSeconds(0.5f);

			// 隐藏所有点赞按钮
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_DISABLE_ALL_VOTE_BUTTON, null);
		}
	}
}
