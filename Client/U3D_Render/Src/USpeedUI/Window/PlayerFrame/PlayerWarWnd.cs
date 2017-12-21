using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using GameLogic;
using UnityEngine;
using UnityEngine.UI;
using DataCenter;
using ASpeedGame.Data.GameMobaSchemes;
using UnityEngine.EventSystems;
using UIWidgets;
using USpeedUI.UExtensions;
using ASpeedGame.Data.WarStatisticMap;
using War;
using U3D_Render.Common;
using ASpeedGame.Data.HeroXPskillConfig;
using ASpeedGame.Data.Scheme;
using SocialSharing;
using USpeedUI;
using USpeedUI.UWidgets;

namespace USpeedUI.PlayerFrame
{
    enum ViewSingleWarType
    {
        VSWT_HONORWND,      // 荣耀
        VSWT_CHATWND,       // 炫耀
        VSWT_GLORYMOMENT,	// 精彩时刻
    }

	// 战绩子窗口类型
	enum WarStatisticSubWndType
	{
		WSDT_VSINFO,			// 对战信息
		WSDT_DATAGRIDINFO,		// 数据网格
	}

	// 战绩子窗口基类
	public abstract class WarStatisticWndSubWnd : MonoBehaviour
	{
		public virtual bool Init()
		{
			return true;
		}

		public virtual void Clear()
		{

		}

		public virtual void UpdateView(WarStatisticInfo info)
		{

		}
	}

	public class PlayerWarStatisticWnd : UIBaseSubWnd<PlayerWarStatisticWndView>
    {
        public override int GetWndType()
        {
            return (int)PlayerSubWndType.WND_WAR;
        }

        public override String GetPrefabPath()
        {
            return "PlayerWarStatistic/WarStatistic";
        }

        public override bool Init()
        {
            SetVisible(false);
            return base.Init();
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_PLAYERWARSTATISTIC_UPDATETOTAL:
                    {
                        if (m_wndView != null)
                            m_wndView.OnUpdateTotalInfo();
                    }
                    break;
                case WndMsgID.WND_MSG_PLAYERWARSTATISTIC_UPDATEWAR:
                    {
                        if (m_wndView != null)
                            m_wndView.OnUpdateWarInfo();
                    }
                    break;
                case WndMsgID.WND_MSG_PLAYERWARSTATISTIC_VIEWSINGLEWAR:
                    {
                        if (m_wndView != null)
                            m_wndView.OnViewSingleWar(msgData as WarStatisticViewSingleWarData);
                    }
                    break;
				case WndMsgID.WND_MSG_PLAYERWARSTATISTIC_VIEWACTORWAR:  // 显示某个玩家的历史战绩（同时可显示指定场次）
					{
						if (m_wndView != null)
							m_wndView.onViewActorWar(msgData as WarStatisticViewActorWarData);

					}
					break;
				default:
                    break;
            }
        }
    }

	// 战绩历史Item
    public class PlayerWarStatisticHistoryItem : MonoBehaviour, IPointerDownHandler
    {
        public Image endType;
        public Image heroIcon;
        public Text result;
        public Text matchType;
        public Text matchDate;
		public Image HasGloryMomentImage;
		public Image highlightImage;

		private long nWarID;
        public long WarID { get { return nWarID; } }
        private PlayerWarStatisticWndView wndView;

        public void SetWndView(PlayerWarStatisticWndView view)
        {
            wndView = view;
        }

        public void Clear()
        {
            this.gameObject.SetActive(false);
        }

		public void SetData(cmd_entity_update_warstatistic_total_warinfo data)
		{
			nWarID = data.nWarID;

			if (data.nWinType > 0 && data.nWinType <= wndView.endTypeImage.Length)
			{
				endType.sprite = wndView.endTypeImage[data.nWinType - 1];
				endType.gameObject.SetActive(true);
			}
			else
			{
				endType.gameObject.SetActive(false);
			}

			heroIcon.sprite = PlayerWarStatisticWndView.GetHeroIcon(data.nHeroID);

			if (data.nWin > 0)
			{
				result.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WinWord");
				Color clr;
				ColorUtility.TryParseHtmlString("#55eb91", out clr);
				result.color = clr;
			}
			else
			{
				result.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "LoseWord");
				Color clr;
				ColorUtility.TryParseHtmlString("#d10912", out clr);
				result.color = clr;
			}

			string strMatchDes = "";
			if (WarStatisticMapConfig.Instance.MapConfig.ContainsKey(data.nWarType))
			{
				strMatchDes = WarStatisticMapConfig.Instance.MapConfig[data.nWarType].strName;
			}

			matchType.text = "(" + strMatchDes + ")";

			DateTime calcTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(data.nBeginTime);
			calcTime = calcTime.ToLocalTime();
			matchDate.text = String.Format("{0:D2}-{1:D2}", calcTime.Month, calcTime.Day);

			gameObject.SetActive(true);

			// 是否显示荣耀时刻提示图标
			if (LogicDataCenter.screenCaptureDataManager.FindScreenCaptureImg(data.nWarID))
				HasGloryMomentImage.gameObject.SetActive(true);
			else
				HasGloryMomentImage.gameObject.SetActive(false);

		}

        public virtual void OnPointerDown(PointerEventData eventData)
        {
            wndView.OnSelectWarHistory(nWarID);
        }

        public void OnFocus(bool bFocus)
        {
            highlightImage.gameObject.SetActive(bFocus);
        }
    }

	// 战绩窗口逻辑
	public class PlayerWarStatisticWndView : UIBaseSubWndView
	{
		// HistorySelect
		public GameObject historySelectObj;
		public RectTransform histroyItemFrame;
		public GameObject histroyItemTemplate;
		private const int histroyItemCount = 7;
		private PlayerWarStatisticHistoryItem[] historyItem;
		public Sprite[] endTypeImage;
		public Text historyPageText;
		//public Image historySelectImage;
		public Button historyPrevPageBtn;
		public Button historyNextPageBtn;
		public Text historyNoDataText;
        public Dropdown MatchTypeDropDown;
        private List<int>[] matchTypeIDList;
        private string[] matchTypeNameList;
		// SingleHistory

		public Text historyDate;
		public Text historyBeginTime;
		public Text historyLastTime;
		public Text historyWarType;
		public Text selfKillCount;
		public Text selfMoney;
		public Text winTeamLabel;
		public Text scoreLabel1;
		public Text moneyLabel1;
		public Text kdaLabel1;
		public Text itemLabel1;
		public Text loseTeamLabel;
		public Text scoreLabel2;
		public Text moneyLabel2;
		public Text kdaLabel2;
		public Text itemLabel2;

		public Text showBtnLabel;
		public Text returnHonorLabel;
        public Text gloryMomentBtnLabel;
        public Text shareBtnLabel;
		public GameObject showBtn;
        public GameObject shareBtn;
		public GameObject returnHonorBtn;
		public GameObject GloryMomentBtn;
		public WarStatisticWndSubWnd[] subWndList;

		public UButtonGroup ButtonGroup;


		// Data
		static public int actorID;
		private long curWarID;
		private long needShowWarID;
		private int curWarHistoryPage;
		private int curWarHistorySel;
		private bool bViewSingleWar;    // 荣耀、战绩单独查看某场次数据特殊处理

		// 当前显示的子窗口
		private WarStatisticSubWndType curSubWinType;

        #region recordShare
        public UShareBar shareBar;
        public RectTransform ShareContentRt;
        public GameObject RecordShareFrame;
        public RawImage ShareRawImg;

        private readonly string shareImgName = "PlayerWarTemp.jpg";

        private Texture2D shareTex2D;
        #endregion

        // 当前筛选的比赛类型
        private List<int> CurMatchType { get { return matchTypeIDList[MatchTypeDropDown.value]; } }

        public override bool Init(IUISubWnd wnd)
        {
            base.Init(wnd);
            // 创建historyItem
            historyItem = new PlayerWarStatisticHistoryItem[histroyItemCount];
            //float fHistroyItemOffsetY = -38f;
            for (int i = 0; i < histroyItemCount; ++i)
            {
                GameObject histroyItemObj = ResNode.InstantiateRes(histroyItemTemplate);
                if (!histroyItemObj)
                    return false;

                historyItem[i] = histroyItemObj.GetComponent<PlayerWarStatisticHistoryItem>();
                if (!historyItem[i])
                    return false;

                histroyItemObj.name = "HistroyItem" + (i + 1);
                historyItem[i].SetWndView(this);
                //RectTransform rectTrans = histroyItemObj.transform as RectTransform;
                histroyItemObj.transform.SetParent(histroyItemFrame, false);
                //rectTrans.anchoredPosition = new Vector2(142f, fHistroyItemOffsetY);
                //fHistroyItemOffsetY -= 77f;
            }
            histroyItemTemplate.SetActive(false);

            historyNoDataText.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "HistoryNoData");

            winTeamLabel.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarWinTeam");
            loseTeamLabel.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarLoseTeam");
            scoreLabel1.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarScore");
            moneyLabel1.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMoney");
            kdaLabel1.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarKDA");
            itemLabel1.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarGoods");
            scoreLabel2.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarScore");
            moneyLabel2.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMoney");
            kdaLabel2.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarKDA");
            itemLabel2.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarGoods");
            showBtnLabel.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShowBtnText");
            shareBtnLabel.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ShareBtnText");
            gloryMomentBtnLabel.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "GloryMomentBtnText");

            curSubWinType = WarStatisticSubWndType.WSDT_VSINFO;

            // 初始化子窗口
            foreach (var item in subWndList)
                item.Init();

            needShowWarID = 0;

            #region recordShare
            shareTex2D = new Texture2D((int)ShareRawImg.rectTransform.sizeDelta.x, (int)ShareRawImg.rectTransform.sizeDelta.y);
            #endregion

            matchTypeNameList = new string[] { "全部战绩", "匹配赛", "排位赛", "人机赛", "杯赛", "自定义" };
            matchTypeIDList = new List<int>[6];
            matchTypeIDList[0] = new List<int> { 0 };
            matchTypeIDList[1] = new List<int> { 5, 7 };
            matchTypeIDList[2] = new List<int> { 8 };
            matchTypeIDList[3] = new List<int> { 2 };
            matchTypeIDList[4] = new List<int> { 11,12};
            matchTypeIDList[5] = new List<int> { 1,6,9,13};

            MatchTypeDropDown.options.Clear();
            for(int i = 0; i < matchTypeIDList.Length; ++i)
            {
                MatchTypeDropDown.options.Add(new Dropdown.OptionData() { text = matchTypeNameList[i]});
            }
            // 刷新Dropdown
            MatchTypeDropDown.RefreshShownValue();
            MatchTypeDropDown.onValueChanged.AddListener((int value) => {
                OnUpdateTotalInfo();
            });


            return true;
		}

		public override void OnDestroy()
		{
			// 子窗口销毁
			foreach (var item in subWndList)
				item.Clear();

            #region recordShare
            if (shareTex2D != null)
            {
                DestroyImmediate(shareTex2D);
                shareTex2D = null;
            }
            #endregion

			base.OnDestroy();
		}

		public override void SetVisible(bool visible)
		{
			base.SetVisible(visible);

			if (visible)
			{
				actorID = LogicDataCenter.playerSystemDataManager.nRequestPDBID;
				curWarHistorySel = 0;
				OnUpdateTotalInfo();
			}
			else
			{
				curWarHistoryPage = 0;
				bViewSingleWar = false;
                CloseRecordShareFrame();
			}
		}

		public void OnUpdateTotalInfo()
		{
			if (bViewSingleWar)
				return;

            ActorStatisticInfo info = LogicDataCenter.warStatisticDataManager.GetActorOverview(actorID, CurMatchType);

			// 需要显示指定某局比赛
			if (needShowWarID > 0)
			{
				calcPageAndSelected(needShowWarID, info);
				needShowWarID = 0;
			}

			if (info.totalStatistic.nActorID != 0)
			{
				SetWarHistroyPage(curWarHistoryPage);
				SetWarHistorySelect(curWarHistorySel);
			}
			else
			{
				// 清空数据
				ClearHistoryPage();
			}
		}

		// 更新战绩窗口
		public void OnUpdateWarInfo(bool isSingleWar = false)
		{
            CloseRecordShareFrame();

			WarStatisticInfo info = LogicDataCenter.warStatisticDataManager.GetSingleWarInfo(curWarID, EGetSingleWarType.EGSWT_PlayerRecord);

			// 切换战绩时,默认显示对战信息
			ButtonGroup.SetItemSelectedItem((int)WarStatisticSubWndType.WSDT_VSINFO);

            returnHonorBtn.SetActive(false);
            showBtn.SetActive(false);
            shareBtn.SetActive(false);
            GloryMomentBtn.SetActive(false);

            // 不显示炫耀按钮
            if (LogicDataCenter.playerSystemDataManager.bRequestSelf)
			{
				showBtn.SetActive(true);
                shareBtn.SetActive(true);

				// 显示精彩时刻按钮
				if(LogicDataCenter.screenCaptureDataManager.FindScreenCaptureImg(info.mainInfo.nWarID))
				{
					GloryMomentBtn.SetActive(true);
				}
				else
				{
					GloryMomentBtn.SetActive(false);
				}
			}
			else
			{
				showBtn.SetActive(false);
                shareBtn.SetActive(false);
				GloryMomentBtn.gameObject.SetActive(false);
			}

			if (info.mainInfo.nWarID != 0)
			{
				bool CanAutoMilitary = CanAutoMilitaryExploits(info.actorInfo);
				showBtn.GetComponent<Button>().enabled = CanAutoMilitary;
                shareBtn.GetComponent<Button>().enabled = CanAutoMilitary;
				if (CanAutoMilitary)
				{
					showBtn.GetComponent<Image>().ResetToDefaultMaterial();
                    shareBtn.GetComponent<Image>().ResetToDefaultMaterial();
				}
				else
				{
					showBtn.GetComponent<Image>().AddNewMaterial(EUIShaderType.EST_Gray);
                    shareBtn.GetComponent<Image>().AddNewMaterial(EUIShaderType.EST_Gray);
				}

				DateTime calcTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(info.mainInfo.nBeginTime);
				calcTime = calcTime.ToLocalTime();
				historyDate.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarDate") + ": " + String.Format("{0:D2}-{1:D2}", calcTime.Month, calcTime.Day);
				historyBeginTime.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarStartTime") + ": " + String.Format("{0:D2}:{1:D2}", calcTime.Hour, calcTime.Minute);

				int lastTime = info.mainInfo.nEndTime - info.mainInfo.nBeginTime;
				lastTime /= 60;
				int nLastHour = lastTime / 60;
				int nLastMin = lastTime % 60;
				String lastTimeStr = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarLastTime") + ":";
				if (nLastHour > 0)
					lastTimeStr += nLastHour + ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarHour");
				lastTimeStr += nLastMin + ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMinute");
				historyLastTime.text = lastTimeStr;
				// 目前只有匹配赛
				string strMatchDes = "";
				if (WarStatisticMapConfig.Instance.MapConfig.ContainsKey(info.mainInfo.nWarType))
				{
					strMatchDes = WarStatisticMapConfig.Instance.MapConfig[info.mainInfo.nWarType].strName;
				}

				historyWarType.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMatchType") + ":" + strMatchDes;

                CAMP_TYPE nWinCamp = CAMP_TYPE.CAMP_RED;
                if (info.mainInfo.nEndBecause == 1 || info.mainInfo.nEndBecause == 4)
                    nWinCamp = CAMP_TYPE.CAMP_BLUE;

				// 统计阵营的击杀等数据
				int nCamp1KillCount = 0;
				int nCamp1GetMoney = 0;
				int nCamp2KillCount = 0;
				int nCamp2GetMoney = 0;
				for (int i = 0; i < info.mainInfo.nActorCount; ++i)
				{
					cmd_entity_update_warstatistic_war_actorinfo actorInfo = info.actorInfo[i];
					if (actorInfo.nCamp == 1)
					{
						nCamp1KillCount += actorInfo.nKillPersonNum;
						nCamp1GetMoney += actorInfo.nGetMoney;
					}
					else if (actorInfo.nCamp == 2)
					{
						nCamp2KillCount += actorInfo.nKillPersonNum;
						nCamp2GetMoney += actorInfo.nGetMoney;
					}
				}
				if (nWinCamp == CAMP_TYPE.CAMP_BLUE)
				{
					selfKillCount.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarKillNum") + ": " + nCamp1KillCount + "/" + nCamp2KillCount;
					//selfMoney.text = String.Format("{0}: {1:N1}k/{2:N1}k", ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMoney"), nCamp1GetMoney / 1000.0f, nCamp2GetMoney / 1000.0f);
					selfMoney.text = String.Format("{0}: {1}/{2}", ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMoney"), nCamp1GetMoney, nCamp2GetMoney);
				}
				else
				{
					selfKillCount.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarKillNum") + ": " + nCamp2KillCount + "/" + nCamp1KillCount;
					//selfMoney.text = String.Format("{0}: {1:N1}k/{2:N1}k", ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMoney"), nCamp2GetMoney / 1000.0f, nCamp1GetMoney / 1000.0f);
					selfMoney.text = String.Format("{0}: {1}/{2}", ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMoney"), nCamp2GetMoney, nCamp1GetMoney);
				}

                if (isSingleWar)
                {
                    cmd_entity_update_warstatistic_war_actorinfo curActorInfo = getActorInfo(info);
                    if(curActorInfo.nActorID > 0)
                    {
                        // 历史分页为空时特殊设置该条战绩信息
                        cmd_entity_update_warstatistic_total_warinfo warInfo;
                        warInfo.nWarID = info.mainInfo.nWarID;
                        warInfo.nHeroID = curActorInfo.nHeroID;
                        warInfo.nBeginTime = info.mainInfo.nBeginTime;
                        warInfo.nWarType = info.mainInfo.nWarType;
                        warInfo.nWin = (curActorInfo.nCamp == (int)nWinCamp) ? 1 : 0;
                        warInfo.nWinType = curActorInfo.nWinType;
                        warInfo.nBanHeroID1 = info.mainInfo.nBanHeroID1;
                        warInfo.nBanHeroID2 = info.mainInfo.nBanHeroID2;
                        historyItem[0].SetData(warInfo);

                        historyNoDataText.gameObject.SetActive(false);
                    }
                }

                // 更新子窗口信息
                foreach (var item in subWndList)
				{
					item.UpdateView(info);
				}
			}
			else
			{
				// 清空数据
				ClearWarInfoPage();
			}

            #region recordShare
            CaptureScreen.GetInstance().CaptureAreaImage(new CaptureAreaImgData(shareImgName, GetRecordRect()));
            #endregion
		}



        private cmd_entity_update_warstatistic_war_actorinfo getActorInfo(WarStatisticInfo info)
        {
            int nActorID = PlayerWarStatisticWndView.actorID;
            for (int i = 0; i < info.mainInfo.nActorCount; ++i)
            {
                if (info.actorInfo[i].nActorID == nActorID)
                {
                    return info.actorInfo[i];
                }
            }

            return new cmd_entity_update_warstatistic_war_actorinfo();
        }

        static public Sprite GetHeroIcon(int nHeroID)
		{
			Sprite sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 3, nHeroID);
			if (sprite == null)
			{
				sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 3, 10);
			}

			return sprite;
		}

		// 显示某玩家的战绩
		public void onViewActorWar(WarStatisticViewActorWarData data)
		{
			if (data == null)
				return;

			ClearHistoryPage();

			actorID = data.nActorID;
			needShowWarID = data.lWarID;

            OnUpdateTotalInfo();
			//ActorStatisticInfo info = LogicDataCenter.warStatisticDataManager.GetActorOverview(actorID);


		}

		// 显示单局战绩
		public void OnViewSingleWar(WarStatisticViewSingleWarData data)
		{
			if (data == null)
				return;
			bViewSingleWar = true;

			ClearHistoryPage();
			curWarID = data.lWarID;
			OnUpdateWarInfo(true);


			// 特殊处理按钮
			if (data.nEntryType == (int)ViewSingleWarType.VSWT_HONORWND)
			{
				returnHonorBtn.SetActive(true);
				returnHonorLabel.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ReturnHonorBtnText");
			}

			if (data.nEntryType == (int)ViewSingleWarType.VSWT_CHATWND)
			{
				showBtn.SetActive(false);
                shareBtn.SetActive(false);
				returnHonorBtn.SetActive(false);
			}


		}

		// 显示第几页的战绩
		private void SetWarHistroyPage(int nPageIndex)
		{
			ActorStatisticInfo info = LogicDataCenter.warStatisticDataManager.GetActorOverview(actorID, CurMatchType);
			if (info != null && info.totalStatisticWar != null)
			{
				int totalCount = info.totalStatisticWar.Count;
				int itemInPage = historyItem.Length;
				if (totalCount <= 0)
				{
					ClearHistoryPage();
					return;
				}
				historyNoDataText.gameObject.SetActive(false);
				historyPrevPageBtn.gameObject.SetActive(true);
				historyNextPageBtn.gameObject.SetActive(true);

				int maxPage = totalCount / itemInPage;
				if (totalCount % itemInPage > 0)
					++maxPage;
				nPageIndex = Mathf.Clamp(nPageIndex, 1, maxPage);
				int baseOffset = (nPageIndex - 1) * itemInPage;
				for (int i = 0; i < itemInPage; ++i)
				{
					int index = baseOffset + i;
					if (index < totalCount)
						historyItem[i].SetData(info.totalStatisticWar[index]);
					else
						historyItem[i].Clear();
				}

				historyPageText.text = nPageIndex + "/" + maxPage;
				curWarHistoryPage = nPageIndex;
			}
		}

		private void SetWarHistorySelect(int nSel)
		{
			if (nSel < 0 || nSel >= historyItem.Length || historyItem[nSel].gameObject.activeSelf == false)
			{
				curWarHistorySel = -1;
				ClearWarInfoPage();
				return;
			}

            // 旧的选中历史项
            if(curWarHistorySel >= 0)
            {
                historyItem[curWarHistorySel].OnFocus(false);
            }

            curWarHistorySel = nSel;

            historyItem[curWarHistorySel].OnFocus(true);

			curWarID = historyItem[nSel].WarID;

			OnUpdateWarInfo();
		}

		private void ClearHistoryPage()
		{
			for (int i = 0; i < historyItem.Length; ++i)
			{
				historyItem[i].Clear();
			}
			historyPageText.text = "";
			historyPrevPageBtn.gameObject.SetActive(false);
			historyNextPageBtn.gameObject.SetActive(false);
			historyNoDataText.gameObject.SetActive(true);
			curWarHistoryPage = 0;
			needShowWarID = 0;

			SetWarHistorySelect(-1);
		}

		private void ClearWarInfoPage()
		{
			historyDate.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarDate") + ":";
			historyBeginTime.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarStartTime") + ":";
			historyLastTime.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarLastTime") + ":";
			historyWarType.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMatchType") + ":";
			selfKillCount.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarKillNum") + ":";
			selfMoney.text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "WarMoney") + ":";
	
		}

		private bool CanAutoMilitaryExploits(List<cmd_entity_update_warstatistic_war_actorinfo> actorInfo)
		{
			int nAIMilitaryExploitsFlag = GameLogicAPI.getAIMilitaryExploitsFlag();
			if (nAIMilitaryExploitsFlag > 0)
			{
				return true;
			}

			bool bHasMachine = false;
			foreach (cmd_entity_update_warstatistic_war_actorinfo item in actorInfo)
			{
                if (item.nPlayerType == (int)EPlayerType.EWPT_COMPUTER || item.nPlayerType == (int)EPlayerType.EWPT_VIRTUALPEOPLE)
				{
					bHasMachine = true;
					break;
				}
			}

			return !bHasMachine;
		}


		// 选择战绩-根据WarID
		public void OnSelectWarHistory(long nWarID)
		{
			for (int i = 0; i < historyItem.Length; ++i)
			{
				if (historyItem[i].WarID == nWarID)
				{
					SetWarHistorySelect(i);
					break;
				}
			}
		}

		// 上一页战绩
		public void OnClickHistoryPrevBtn()
		{
			if (curWarHistoryPage > 1)
			{
				SetWarHistroyPage(curWarHistoryPage - 1);
				SetWarHistorySelect(0);
			}
		}

		// 下一页战绩
		public void OnClickHistoryNextBtn()
		{
			ActorStatisticInfo info = LogicDataCenter.warStatisticDataManager.GetActorOverview(actorID, CurMatchType);
			if (info != null && info.totalStatisticWar != null)
			{
				int totalCount = info.totalStatisticWar.Count;
				int itemInPage = historyItem.Length;
				if (totalCount <= 0)
				{
					return;
				}

				int maxPage = totalCount / itemInPage;
				if (totalCount % itemInPage > 0)
					++maxPage;

				if (curWarHistoryPage < maxPage)
				{
					SetWarHistroyPage(curWarHistoryPage + 1);
					SetWarHistorySelect(0);
				}
			}
		}

		// 打开荣耀界面
		public void OnClickHonorBtn()
		{
			UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_PLAYER);
			ButtonType.bIsRequestSelf = LogicDataCenter.playerSystemDataManager.bRequestSelf;
			ButtonType.nRequestPDBID = LogicDataCenter.playerSystemDataManager.nRequestPDBID;
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, ButtonType);

			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HONORPAGE_OPEN, null);
		}

		// 炫耀战绩
		public void OnClickShowBtn()
		{
			if (curWarID == 0)
				return;

			if (EntityFactory.MainHeroView == null)
				return;

			WarStatisticInfo warInfo = LogicDataCenter.warStatisticDataManager.GetSingleWarInfo(curWarID, EGetSingleWarType.EGSWT_PlayerRecord);
			if (warInfo.mainInfo.nWarID != curWarID)
				return;

			cmd_entity_update_warstatistic_war_actorinfo actorInfo = new cmd_entity_update_warstatistic_war_actorinfo();
			for (int i = 0; i < warInfo.actorInfo.Count; ++i)
			{
				if (warInfo.actorInfo[i].nActorID == actorID)
				{
					actorInfo = warInfo.actorInfo[i];
					break;
				}
			}

			if (actorInfo.nActorID == 0)
				return;

			// 炫耀战绩
			LogicDataCenter.warStatisticDataManager.ShowWarResult(actorID, curWarID, actorInfo.nGlory, actorInfo.nMultiKillMax, actorInfo.nContinueKillMax);
		}

		// 跳转到精彩时刻
		public void onClickGloryMomentBtn()
		{
			WarStatisticInfo warInfo = LogicDataCenter.warStatisticDataManager.GetSingleWarInfo(curWarID, EGetSingleWarType.EGSWT_PlayerRecord);
			if (warInfo.mainInfo.nWarID != curWarID)
				return;

			UGloryMomentReqMsgData data = new UGloryMomentReqMsgData();
			data.msgID = (int)WndMsgID.WND_MSG_OPEN_GLORYMOMENT_WARDATA;
			data.nDBWarID = curWarID;
			data.pageType = EMGloryMomentPageType.EMPage_Image;
			data.szDataTime = Api.convertTimeStamp2DateTime(warInfo.mainInfo.nBeginTime).ToString("yyyy-MM-dd") ;
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_OPEN_GLORYMOMENT_WARDATA, data);
		}

		// 切换战绩子窗口
		private void switchSubWnd(WarStatisticSubWndType type)
		{
			for(int i = 0; i < subWndList.Length; ++i)
			{
				if( (int)type == i)
				{
					subWndList[i].gameObject.SetActive(true);
				}
				else
				{
					subWndList[i].gameObject.SetActive(false);
				}
			}
		}

		// 点击对战信息按钮
		public void OnClickVsInfoBtn(bool bActive)
		{
			if (!bActive)
				return;

			switchSubWnd(WarStatisticSubWndType.WSDT_VSINFO);
			curSubWinType = WarStatisticSubWndType.WSDT_VSINFO;
		}

		// 点击数据网格按钮
		public void OnClickDataGridInfoBtn(bool bActive)
		{
			if (!bActive)
				return;

			switchSubWnd(WarStatisticSubWndType.WSDT_DATAGRIDINFO);
			curSubWinType = WarStatisticSubWndType.WSDT_DATAGRIDINFO;
		}

		// 根据WarID计算页数和位置
		public void calcPageAndSelected(long lWarID, ActorStatisticInfo info)
		{
			int index = 0;
            curWarHistoryPage = 0;
            curWarHistorySel = 0;
			foreach(var item in info.totalStatisticWar)
			{
				if(item.nWarID == lWarID)
				{
                    int nTotalWar = info.totalStatisticWar.Count;

                    curWarHistoryPage = index / historyItem.Length + 1;
                    curWarHistorySel = index % historyItem.Length;
                    return;
                }

				index++;
			}
		}

        #region recordShare
        public void OpenRecordShareFrame()
        {
            if (curWarID == 0)
                return;

            if (EntityFactory.MainHeroView == null)
                return;

            string shareImgFullPath = CaptureScreen.GetInstance().CaptureTempDir +"/" + shareImgName;
            if (File.Exists(shareImgFullPath))
            {
                byte[] imgData = ImageExtensions.GetAllByteData(shareImgFullPath);
                if (null != ShareRawImg)
                {
                    if (shareTex2D != null)
                        shareTex2D.LoadImage(imgData);
                    ShareRawImg.texture = shareTex2D;
                    ShareRawImg.gameObject.SetActive(true);
                }
                shareBar.SetData(SocialSharingType.WarRecord, imgData);
            }

            RecordShareFrame.gameObject.SetActive(true);
        }

        public void CloseRecordShareFrame()
        {
            RecordShareFrame.gameObject.SetActive(false);
        }

        private Rect GetRecordRect()
        {
            Vector3[] worldCorners = new Vector3[4];

            ShareContentRt.GetWorldCorners(worldCorners);

            Vector3 corner0 = UISystem.Instance.GetCamera().WorldToScreenPoint(worldCorners[0]);
            Vector3 corner1 = UISystem.Instance.GetCamera().WorldToScreenPoint(worldCorners[1]);
            Vector3 corner3 = UISystem.Instance.GetCamera().WorldToScreenPoint(worldCorners[3]);

            return new Rect(corner0.x, corner0.y, corner3.x - corner0.x, corner1.y - corner0.y);
        }
        #endregion
	}

	#region 对战信息子窗口
	// 对战信息玩家信息
	public class PlayerWarStatisticWarActorItem : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler
	{
		public Image heroIcon;
        public Image sexIcon;
        public GameObject achiIconContainer;
		public Text actorName;
		public Text score;
		public Text money;
		public Text kdaValue;
		public RectTransform equipFrame;
		public GameObject equipItemTemplate;
		public Sprite emptyHeroIcon;
		public Image BaseImage;
		public Sprite OverSprite;
		public Sprite DownSprite;
		public Sprite UpSprite;
		public Button AddBuddyBtn;
        public GameObject SelfIcon;
        public GameObject MVPIcon;

		private const int equipItemCount = 6;
		private Image[] itemIcon;
		private UTooltipTrigger[] equipTooltipTrigger;

		// 玩家战绩信息
		private cmd_entity_update_warstatistic_war_actorinfo actorInfo;

		// 玩家战绩信息tips
		private UTooltipTrigger actorTooltipTrigger;

		public void Init()
		{
			// 创建equipItem
			//float fEquipItemOffsetX = -110f;
			itemIcon = new Image[equipItemCount];
			equipTooltipTrigger = new UTooltipTrigger[equipItemCount];
			for (int i = 0; i < equipItemCount; ++i)
			{
				GameObject equipItemObj = ResNode.InstantiateRes(equipItemTemplate);
				if (!equipItemObj)
					return;

				itemIcon[i] = equipItemObj.GetComponent<Image>();
				equipTooltipTrigger[i] = equipItemObj.GetComponent<UTooltipTrigger>();

				equipItemObj.name = "EquipItem" + (i + 1);
				RectTransform rectTrans = equipItemObj.transform as RectTransform;
				rectTrans.SetParent(equipFrame, false);
				//rectTrans.anchoredPosition = new Vector2(fEquipItemOffsetX, 0f);
				//fEquipItemOffsetX += 44f;
			}

			equipItemTemplate.SetActive(false);

			actorInfo = new cmd_entity_update_warstatistic_war_actorinfo();

			actorTooltipTrigger = heroIcon.GetComponent<UTooltipTrigger>();
			actorTooltipTrigger.tipPosition = TipPosition.TopMiddle;

            actorTooltipTrigger.gameObject.SetActive(false);

            AddBuddyBtn.gameObject.SetActive(false);

            MVPIcon.transform.FindChild("Text").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "MvpDesc");

            actorName.GetComponent<ButtonAdvanced>().onPointerEnter.AddListener(OnActorNamePointerEnter);
            actorName.GetComponent<ButtonAdvanced>().onPointerExit.AddListener(OnActorNamePointerExit);
            actorName.GetComponent<ButtonAdvanced>().onClick.AddListener(OnActorNamePointerClick);
        }

		public void Clear()
		{
			gameObject.SetActive(true);

			heroIcon.sprite = emptyHeroIcon;
			actorName.text = "-";
			score.text = "-";
			money.text = "-";
			kdaValue.text = "-";
			for (int i = 0; i < itemIcon.Length; ++i)
			{
				itemIcon[i].sprite = emptyHeroIcon;
			}
			for (int i = 0; i < equipTooltipTrigger.Length; ++i)
			{
				equipTooltipTrigger[i].enabled = false;
			}

			actorInfo = new cmd_entity_update_warstatistic_war_actorinfo();

			ClearAchiIcon();

            SelfIcon.SetActive(false);

            MVPIcon.SetActive(false);

            sexIcon.gameObject.SetActive(false);

            BaseImage.sprite = UpSprite;

            actorTooltipTrigger.gameObject.SetActive(false);
        }

        private void ClearAchiIcon()
		{
			Transform containerTrans = achiIconContainer.transform;
			int nChildCount = containerTrans.childCount;
			for (int i = nChildCount - 1; i >= 0; --i)
			{
				var temp1 = containerTrans.GetChild(i).gameObject;
				ResNode.DestroyRes(ref temp1);
			}
		}

		public void SetData(cmd_entity_update_warstatistic_war_actorinfo data, int nScoreTop, int nMoneyTop, int nKillTop, int nDieTop, int nAssistTop)
		{
			actorInfo = data;

			heroIcon.sprite = PlayerWarStatisticWndView.GetHeroIcon(data.nHeroID);

            // 默认为男性
            int bySex = data.bySex > 0 ? data.bySex : 1;
            sexIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_SOCIALCONTACT, bySex);
            sexIcon.gameObject.SetActive(true);

            actorName.text = data.szActorName;

			if (nScoreTop > 0 && data.nScore == nScoreTop)
			    score.text = String.Format("<color=#FFCE34FF>{0}</color>", data.nScore);
			else
			    score.text = data.nScore.ToString();

			if (nMoneyTop > 0 && data.nGetMoney == nMoneyTop)
                money.text = String.Format("<color=#FFCE34FF>{0}</color>", data.nGetMoney);
			else
			    money.text = data.nGetMoney.ToString();

			String strKDAValue = "";
			if (nKillTop > 0 && data.nKillPersonNum == nKillTop)
				strKDAValue += "<color=#FFCE34FF>" + data.nKillPersonNum + "</color>";
			else
				strKDAValue += data.nKillPersonNum.ToString();
			strKDAValue += "/";
			if (nDieTop > 0 && data.nDieCount == nDieTop)
				strKDAValue += "<color=#FFCE34FF>" + data.nDieCount + "</color>";
			else
				strKDAValue += data.nDieCount.ToString();
			strKDAValue += "/";
			if (nAssistTop > 0 && data.nAssistCount == nAssistTop)
				strKDAValue += "<color=#FFCE34FF>" + data.nAssistCount + "</color>";
			else
				strKDAValue += data.nAssistCount.ToString();
			kdaValue.text = strKDAValue;

			for (int i = 0; i < itemIcon.Length; ++i)
			{
				Image iconImage = itemIcon[i];
				UTooltipTrigger tooltip = equipTooltipTrigger[i];
				SSchemeMobaGoods goodsInfo;
				if (data.nEquipID[i] <= 0 || !LogicDataCenter.goodsSchemesDataManager.GetMobaGoods_Out(data.nEquipID[i], out goodsInfo))
				{
					iconImage.sprite = emptyHeroIcon;
					tooltip.enabled = false;
					continue;
				}

				int nEquipIcon = goodsInfo.GoodsIconID;
				if (nEquipIcon > 0)
				{
					Sprite equipSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MobaGoods, WndID.WND_ID_PLAYERFRAME, 1, nEquipIcon);
					if (equipSprite == null)
						Debug.LogWarning("can't find mobagoods icon " + nEquipIcon);
					iconImage.sprite = equipSprite;
				}
				else
				{
					iconImage.sprite = emptyHeroIcon;
				}

				// tooltip
				UBB.toHtml(ref goodsInfo.GoodsName, UBB_FORMAT_TYPE.UGUI);
				tooltip.SetText(UTooltipParamName.ItemName, goodsInfo.GoodsName);
				tooltip.SetText(UTooltipParamName.ItemCost, goodsInfo.GoodsCost.ToString());
				UBB.toHtml(ref goodsInfo.GoodsDes, UBB_FORMAT_TYPE.UGUI);
				tooltip.SetText(UTooltipParamName.ItemDesc, goodsInfo.GoodsDes);
				tooltip.enabled = true;
			}

			ClearAchiIcon();
			if ((data.nGlory & (int)EWarMedal.EWM_SCORE) > 0)
            {
                //AddAchiIcon((int)AchiIconType.Icon_MVP);
                MVPIcon.SetActive(true);
            }
            else
            {
                MVPIcon.SetActive(false);
            }

			if ((data.nGlory & (int)EWarMedal.EWM_OUTPUT) > 0)
				AddAchiIcon((int)AchiIconType.Icon_TopDamage);
			if ((data.nGlory & (int)EWarMedal.EWM_KILLCOUNT) > 0)
				AddAchiIcon((int)AchiIconType.Icon_TopKill);
			if ((data.nGlory & (int)EWarMedal.EWM_ASSCOUNT) > 0)
				AddAchiIcon((int)AchiIconType.Icon_TopAssist);
			if ((data.nGlory & (int)EWarMedal.EWM_DAMAGE) > 0)
				AddAchiIcon((int)AchiIconType.Icon_TopTank);
			if ((data.nGlory & (int)EWarMedal.EWM_DESTORYTOWER) > 0)
				AddAchiIcon((int)AchiIconType.Icon_TopTowerKill);
			if ((data.nGlory & (int)EWarMedal.EWM_MONEY) > 0)
				AddAchiIcon((int)AchiIconType.Icon_TopRich);
            // 不显示补刀最多
			//if ((data.nGlory & (int)EWarMedal.EWM_KILLMONSTER) > 0)
			//	AddAchiIcon((int)AchiIconType.Icon_TopFarm);
			if ((data.nGlory & (int)EWarMedal.EWM_DEADLYCONTROL) > 0)
				AddAchiIcon((int)AchiIconType.Icon_DeadlyControl);
			if ((data.nGlory & (int)EWarMedal.EWM_THRILLSAVE) > 0)
				AddAchiIcon((int)AchiIconType.Icon_ThrilSave);
			if ((data.nGlory & (int)EWarMedal.EWM_TEAMCONTROL) > 0)
				AddAchiIcon((int)AchiIconType.Icon_TeamControl);
			if ((data.nAliveAssistMax) >= 15)
				AddAchiIcon((int)AchiIconType.Icon_AliveAssist);
			if (data.nContinueKillMax >= 8)
				AddAchiIcon((int)AchiIconType.Icon_Godlike);
			if (data.nMultiKillMax == 3)
				AddAchiIcon((int)AchiIconType.Icon_TripleKill);
			if (data.nMultiKillMax == 4)
				AddAchiIcon((int)AchiIconType.Icon_FourKill);
			if (data.nMultiKillMax == 5)
				AddAchiIcon((int)AchiIconType.Icon_FiveKill);

			gameObject.SetActive(true);

			// 设置玩家信息tips
			setActorInfoToolTips();

            // 是当前玩家
            if (actorInfo.nActorID == PlayerWarStatisticWndView.actorID)
                SelfIcon.SetActive(true);
            else
                SelfIcon.SetActive(false);
		}

		private void AddAchiIcon(int nIconIndex)
		{
			if (nIconIndex < 0)
				return;

			GameObject achiObj = LogicDataCenter.warStatisticDataManager.achiIconObj.InstanceMainRes();
			if (achiObj)
			{
				Image icon = achiObj.GetComponent<Image>();
				icon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_WarResultShow, "WarAchiType_" + nIconIndex, WndID.WND_ID_PLAYERFRAME);
				achiObj.transform.SetParent(achiIconContainer.transform, false);

				string stTitleName = "Title_" + nIconIndex;
				UTooltipTrigger titleTooltip = achiObj.GetComponent<UTooltipTrigger>();
				if (titleTooltip)
				{
					string stTitleDesc = ULocalizationService.Instance.Get("ToolTips", "RecordTableEnd", stTitleName);
					titleTooltip.SetText(UTooltipParamName.BodyText, "<color=#FFFFFFFF>" + stTitleDesc + "</color>");
                    titleTooltip.tipPosition = TipPosition.MouseTopRightCorner;
				}
			}
		}

		// 设置玩家信息tips
		private void setActorInfoToolTips()
		{
			PersonSkinModelItem heroItem = new PersonSkinModelItem();
			GameLogicAPI.GetPersonModel((ushort)actorInfo.nHeroID, ref heroItem);
			string heroName = heroItem.szHeroName;

			SSchemeHeroXPskillConfig xpConfig = HeroXPskillConfig.Instance.GetHeroXPskillConfig(actorInfo.nHeroID, actorInfo.nXPSkillID);
			if (xpConfig != null)
			{
				Sprite xpSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_PLAYERFRAME, 1, xpConfig.nIconID);
				actorTooltipTrigger.SetImage("XpSkillIcon", xpSprite);
			}

			int summerIcon = GameLogicAPI.getSpellIntData(actorInfo.nSummerID, (int)SINGLE_SPELL_DATA.SINGLE_SPDATA_ICON);
			Sprite summerSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_PLAYERFRAME, 1, summerIcon);
			actorTooltipTrigger.SetImage("SummerSkillIcon", summerSprite);

			actorTooltipTrigger.SetImage("HeroIcon", PlayerWarStatisticWndView.GetHeroIcon(actorInfo.nHeroID));
			actorTooltipTrigger.SetText("ActorName", actorInfo.szActorName);
			actorTooltipTrigger.SetText("HeroName", heroName);
			actorTooltipTrigger.SetText("KillNum", String.Format("{0}：{1}", ULocalizationService.Instance.Get("UIView", "WarStatistic", "KillNumDesc"), actorInfo.nKillPersonNum));
			actorTooltipTrigger.SetText("DiedNum", String.Format("{0}：{1}", ULocalizationService.Instance.Get("UIView", "WarStatistic", "DieNumDesc"), actorInfo.nDieCount));
			actorTooltipTrigger.SetText("AssistNum", String.Format("{0}：{1}", ULocalizationService.Instance.Get("UIView", "WarStatistic", "AssistNumDesc"), actorInfo.nAssistCount));
			actorTooltipTrigger.SetText("Gold", String.Format("{0}：{1}", ULocalizationService.Instance.Get("UIView", "WarStatistic", "GetMoney"), actorInfo.nGetMoney));
			actorTooltipTrigger.SetText("ContinueKillMax", String.Format("{0}：{1}", ULocalizationService.Instance.Get("UIView", "WarStatistic", "ContinueKillMax"), actorInfo.nContinueKillMax));
			actorTooltipTrigger.SetText("KillTowerNum", String.Format("{0}：{1}", ULocalizationService.Instance.Get("UIView", "WarStatistic", "KillTowerNum"), actorInfo.nKillTowerNum));
			actorTooltipTrigger.SetText("CureTotal", String.Format("{0}：{1}", ULocalizationService.Instance.Get("UIView", "WarStatistic", "CureTotal"), actorInfo.nCureTotal));
			actorTooltipTrigger.SetText("BearDamage", String.Format("{0}：{1}", ULocalizationService.Instance.Get("UIView", "WarStatistic", "BearDamageTotal"), actorInfo.nDamageTotal));
			actorTooltipTrigger.SetText("OutTotal", String.Format("{0}：{1}", ULocalizationService.Instance.Get("UIView", "WarStatistic", "OutDamageTotal"), actorInfo.nOutTotal));
			actorTooltipTrigger.SetText("OutMagic", String.Format("{0}：{1}", ULocalizationService.Instance.Get("UIView", "WarStatistic", "OutDamageMagic"), actorInfo.nOutHeroMagic));
			actorTooltipTrigger.SetText("OutPhysics", String.Format("{0}：{1}", ULocalizationService.Instance.Get("UIView", "WarStatistic", "OutDamagePhysics"), actorInfo.nOutHeroPhysics));

            actorTooltipTrigger.gameObject.SetActive(true);
        }


        public void OnBaseImageClick()
        {
            if (actorInfo.nActorID <= 0)
                return;

            // 电脑玩家不处理
            if ((EPlayerType)actorInfo.nPlayerType == EPlayerType.EWPT_COMPUTER)
                return;

            // 自己不处理
            if (actorInfo.nActorID == EntityUtil.getMainHeroPdbid())
                return;

            UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_PLAYER);
            int nSelfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            ButtonType.bIsRequestSelf = (nSelfID == actorInfo.nActorID);
            ButtonType.nRequestPDBID = actorInfo.nActorID;
			
			// 造假数据
            if (actorInfo.nPlayerType == (int)EPlayerType.EWPT_VIRTUALPEOPLE)
            {
                ButtonType.bFake = true;
                ButtonType.strFakeName = actorInfo.szActorName;
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, ButtonType);
        }

        private void OnActorNamePointerEnter(PointerEventData eventData)
        {
            // 自己不处理
            if (actorInfo.nActorID == EntityUtil.getMainHeroPdbid())
                return;

            Color clr = new Color();
            ColorUtility.TryParseHtmlString("#48b7fe", out clr);
            actorName.color = clr;
        }

        private void OnActorNamePointerExit(PointerEventData eventData)
        {
            // 自己不处理
            if (actorInfo.nActorID == EntityUtil.getMainHeroPdbid())
                return;

            actorName.color = Color.white;
        }

        private void OnActorNamePointerClick()
        {
            OnBaseImageClick();
        }

        public void OnPointerEnter(PointerEventData eventData)
		{
			if (actorInfo.nActorID <= 0)
				return;

			BaseImage.sprite = OverSprite;

            if(actorInfo.nActorID != EntityUtil.getMainHeroPdbid())
            {
                if ((EPlayerType)actorInfo.nPlayerType == EPlayerType.EWPT_REALPEOPLE
                    ||(EPlayerType)actorInfo.nPlayerType == EPlayerType.EWPT_VIRTUALPEOPLE)
                {
			        AddBuddyBtn.gameObject.SetActive(true);
                }
            }
        }

		public void OnPointerExit(PointerEventData eventData)
		{
			if (actorInfo.nActorID <= 0)
				return;

			BaseImage.sprite = UpSprite;
			AddBuddyBtn.gameObject.SetActive(false);
		}
		
		// 添加好友
		public void onClickAddBuddyBtn()
		{
            if (actorInfo.nPlayerType == (int)EPlayerType.EWPT_REALPEOPLE)
            {
                LogicDataCenter.snsDataManager.reqAddBuddy(actorInfo.nUserID, actorInfo.szActorName);
            }
            else if(actorInfo.nPlayerType == (int)EPlayerType.EWPT_VIRTUALPEOPLE)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_SNS_SEND_APPLY_SUCCESS);
            }
        }
	}

	// 对战信息据子窗口
	public class WSVsInfoSubWnd : WarStatisticWndSubWnd
	{
		public RectTransform warActorItemFrame;
		public GameObject warActorItemTemplate;
		public GameObject[] BlueBanHeroList;
		public GameObject[] RedBanHeroList;

		private const int warActorItemCount = 10;
		private PlayerWarStatisticWarActorItem[] warActorItem;

		public override bool Init()
		{
			// 创建对战信息中的玩家Item
			warActorItem = new PlayerWarStatisticWarActorItem[warActorItemCount];
			float fWarActorItemOffsetX = 324.5f;
			float fWarActorItemOffsetY = -43f;
			for (int i = 0; i < warActorItemCount; ++i)
			{
				GameObject warActorItemObj = ResNode.InstantiateRes(warActorItemTemplate);
				if (!warActorItemObj)
					return false;

				warActorItem[i] = warActorItemObj.GetComponent<PlayerWarStatisticWarActorItem>();
				if (!warActorItem[i])
					return false;

				warActorItemObj.name = "MemberItem" + (i + 1);
				warActorItem[i].Init();
				RectTransform rectTrans = warActorItemObj.transform as RectTransform;
				rectTrans.SetParent(warActorItemFrame, false);
				rectTrans.anchoredPosition = new Vector2(fWarActorItemOffsetX, fWarActorItemOffsetY);
				if (i == warActorItemCount / 2 - 1)
				{
					fWarActorItemOffsetX = 979.5f;
					fWarActorItemOffsetY = -43f;
				}
				else
					fWarActorItemOffsetY -= 90f;
			}
			warActorItemTemplate.SetActive(false);

			return true;
		}

		public override void Clear()
		{
			for (int i = 0; i < warActorItem.Length; ++i)
			{
				warActorItem[i].Clear();
			}
		}

		public override void UpdateView(WarStatisticInfo info)
		{
			// 当前战绩页面的玩家ID
			int actorID = PlayerWarStatisticWndView.actorID;

			int winIndex = 0;
			int loseIndex = 5;

			bool bCamp1Win = false;
			// 1表示1阵营胜利,4表示2阵营投降
			if (info.mainInfo.nEndBecause == 1 || info.mainInfo.nEndBecause == 4)
				bCamp1Win = true;

			// 清空玩家Item数据
			for (int i = 0; i < warActorItem.Length; ++i)
			{
				warActorItem[i].Clear();
			}

			int actorCount = info.actorInfo.Count;


			int nScoreTop = 0;
			int nMoneyTop = 0;
			int nKillTop = 0;
			int nDieTop = 0;
			int nAssistTop = 0;

			// 计算分数,金币,击杀数,死亡数,助攻数等的最大值
			for (int i = 0; i < actorCount; ++i)
			{
				cmd_entity_update_warstatistic_war_actorinfo actorInfo = info.actorInfo[i];
				if (nScoreTop < actorInfo.nScore)
					nScoreTop = actorInfo.nScore;
				if (nMoneyTop < actorInfo.nGetMoney)
					nMoneyTop = actorInfo.nGetMoney;
				if (nKillTop < actorInfo.nKillPersonNum)
					nKillTop = actorInfo.nKillPersonNum;
				if (nDieTop < actorInfo.nDieCount)
					nDieTop = actorInfo.nDieCount;
				if (nAssistTop < actorInfo.nAssistCount)
					nAssistTop = actorInfo.nAssistCount;
			}

			cmd_entity_update_warstatistic_war_actorinfo curActorInfo = new cmd_entity_update_warstatistic_war_actorinfo();
			bool bActorWin = false;
			for (int i = 0; i < actorCount; ++i)
			{
				cmd_entity_update_warstatistic_war_actorinfo actorInfo = info.actorInfo[i];

				bool bWin = false;
				if ((actorInfo.nCamp == 1 && bCamp1Win) || (actorInfo.nCamp != 1 && !bCamp1Win))
					bWin = true;

				if (bWin)
				{
					if (winIndex < warActorItem.Length)
					{
						
						if (actorInfo.nActorID == actorID)
						{
                            if (LogicDataCenter.playerSystemDataManager.bFake)
                            {
                                actorInfo.szActorName = LogicDataCenter.playerSystemDataManager.strCurFakeName;
                            }
							curActorInfo = actorInfo;
							bActorWin = bWin;
						}
                        warActorItem[winIndex].SetData(actorInfo, nScoreTop, nMoneyTop, nKillTop, nDieTop, nAssistTop);
						++winIndex;
					}
					else
					{
						Debug.Log("PlayerWarStatisticWndView OnUpdateWarInfo winIndex > length");
					}
				}
				else
				{
					if (loseIndex < warActorItem.Length)
					{
						
						if (actorInfo.nActorID == actorID)
						{
                            if (LogicDataCenter.playerSystemDataManager.bFake)
                            {
                                actorInfo.szActorName = LogicDataCenter.playerSystemDataManager.strCurFakeName;
                            }

							curActorInfo = actorInfo;
							bActorWin = !bWin;
						}
                        warActorItem[loseIndex].SetData(actorInfo, nScoreTop, nMoneyTop, nKillTop, nDieTop, nAssistTop);
						++loseIndex;
					}
					else
					{
						Debug.Log("PlayerWarStatisticWndView OnUpdateWarInfo loseIndex > length");
					}
				}
			}

            // Ban英雄列表
            int[] winCampBanList = info.mainInfo.nBanHeroID1;
            int[] lostCampBanList = info.mainInfo.nBanHeroID2;

            if (!bCamp1Win)
            {
                winCampBanList = info.mainInfo.nBanHeroID2;
                lostCampBanList = info.mainInfo.nBanHeroID1;
            }
            for (int i = 0; i < UDefines.WAR_PERSON_MAX_COUNT / 2; ++i)
			{
				// 胜利阵营
				int nHeroID = winCampBanList[i];
				if (nHeroID > 0)
				{
					BlueBanHeroList[i].transform.FindChild("HeadIcon").GetComponent<Image>().sprite = PlayerWarStatisticWndView.GetHeroIcon(nHeroID);
					BlueBanHeroList[i].SetActive(true);
				}
				else
				{
					BlueBanHeroList[i].SetActive(false);
				}

                // 失败阵营
                nHeroID = lostCampBanList[i];
				if (nHeroID > 0)
				{
					RedBanHeroList[i].transform.FindChild("HeadIcon").GetComponent<Image>().sprite = PlayerWarStatisticWndView.GetHeroIcon(nHeroID);
					RedBanHeroList[i].SetActive(true);
				}
				else
				{
					RedBanHeroList[i].SetActive(false);
				}
			}
		}
	}
	#endregion

	#region 网格数据子窗口
	// 玩家信息
	public class UWarStatisticPlayerInfo : MonoBehaviour
	{
		public Transform DataArray1;	// 战斗统计
		public Transform DataArray2;	// 团队贡献
		public Transform DataArray3;    // 精彩表现
        public Image selfImage;         // 当前玩家高亮背景

		private Text[] DataArrayText1;  // 战斗统计
		private Text[] DataArrayText2;  // 团队贡献
		private Text[] DataArrayText3;  // 精彩表现

		public void Start()
		{

		}

		public void Init()
		{
			DataArrayText1 = new Text[UDefines.DataArrayNum[0]];
            for(int i = 0; i < UDefines.DataArrayNum[0]; ++i)
            {
                string name = String.Format("Text{0}", i + 1);
			    DataArrayText1[i] = DataArray1.FindChild(name).GetComponent<Text>();
            }

            DataArrayText2 = new Text[UDefines.DataArrayNum[1]];
            for(int i = 0; i < UDefines.DataArrayNum[1]; ++i)
            {
                string name = String.Format("Text{0}", i + 1);
			    DataArrayText2[i] = DataArray2.FindChild(name).GetComponent<Text>();
            }

            DataArrayText3 = new Text[UDefines.DataArrayNum[2]];
            for (int i = 0; i < UDefines.DataArrayNum[2]; ++i)
            {
                string name = String.Format("Text{0}", i + 1);
                DataArrayText3[i] = DataArray3.FindChild(name).GetComponent<Text>();
            }
		}

		// 设置玩家的信息
		public void setPlayerInfo(cmd_entity_update_warstatistic_war_actorinfo data, cmd_entity_update_warstatistic_war_actorinfo topInfo)
		{
			// 没有
			if(data.nActorID <= 0)
			{
				clearPlayerInfo();
                return;
			}

            // 设置头像
            if(data.nActorID == PlayerWarStatisticWndView.actorID)
                selfImage.gameObject.SetActive(true);
            else
                selfImage.gameObject.SetActive(false);
           
            if (topInfo.nKillPersonNum > 0 && topInfo.nKillPersonNum == data.nKillPersonNum)
                DataArrayText1[0].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nKillPersonNum);
            else
                DataArrayText1[0].text = data.nKillPersonNum.ToString();

            if (topInfo.nDieCount > 0 && topInfo.nDieCount == data.nDieCount)
                DataArrayText1[1].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nDieCount);
            else
                DataArrayText1[1].text = data.nDieCount.ToString();

            if (topInfo.nAssistCount > 0 && topInfo.nAssistCount == data.nAssistCount)
                DataArrayText1[2].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nAssistCount);
            else
                DataArrayText1[2].text = data.nAssistCount.ToString();

            if (topInfo.nScore > 0 && topInfo.nScore == data.nScore)
                DataArrayText1[3].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nScore);
            else
                DataArrayText1[3].text = data.nScore.ToString();

            if (topInfo.nGPM > 0 && topInfo.nGPM == data.nGPM)
                DataArrayText1[4].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nGPM);
            else
                DataArrayText1[4].text = data.nGPM.ToString();

            if (data.bFirstBlood)
                DataArrayText1[5].text = String.Format("<color=#FFCE34FF>{0}</color>", ULocalizationService.Instance.Get("UIView", "WarStatistic", "IsFirstBlood"));
            else
                DataArrayText1[5].text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "IsntFirstBlood");

            if (topInfo.nDeadlyControl > 0 && topInfo.nDeadlyControl == data.nDeadlyControl)
                DataArrayText1[6].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nDeadlyControl);
            else
                DataArrayText1[6].text = data.nDeadlyControl.ToString();

            if (topInfo.nMoneyRate > 0 && topInfo.nMoneyRate == data.nMoneyRate)
                DataArrayText2[0].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nMoneyRate);
            else
                DataArrayText2[0].text = data.nMoneyRate.ToString();

            if (topInfo.nOutputRate > 0 && topInfo.nOutputRate == data.nOutputRate)
                DataArrayText2[1].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nOutputRate);
            else
                DataArrayText2[1].text = data.nOutputRate.ToString();

            if (topInfo.nJoinBattleRate > 0 && topInfo.nJoinBattleRate == data.nJoinBattleRate)
                DataArrayText2[2].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nJoinBattleRate);
            else
                DataArrayText2[2].text = data.nJoinBattleRate.ToString();

            if (topInfo.nOutTotal > 0 && topInfo.nOutTotal == data.nOutTotal)
                DataArrayText2[3].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nOutTotal);
            else
                DataArrayText2[3].text = data.nOutTotal.ToString();

            if (topInfo.nOutHeroPhysics > 0 && topInfo.nOutHeroPhysics == data.nOutHeroPhysics)
                DataArrayText2[4].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nOutHeroPhysics);
            else
                DataArrayText2[4].text = data.nOutHeroPhysics.ToString();

            if (topInfo.nOutHeroMagic > 0 && topInfo.nOutHeroMagic == data.nOutHeroMagic)
                DataArrayText2[5].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nOutHeroMagic);
            else
                DataArrayText2[5].text = data.nOutHeroMagic.ToString();

            if (topInfo.nOutHeroTrue > 0 && topInfo.nOutHeroTrue == data.nOutHeroTrue)
                DataArrayText2[6].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nOutHeroTrue);
            else
                DataArrayText2[6].text = data.nOutHeroTrue.ToString();

            if (topInfo.nDamageTotal > 0 && topInfo.nDamageTotal == data.nDamageTotal)
                DataArrayText2[7].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nDamageTotal);
            else
                DataArrayText2[7].text = data.nDamageTotal.ToString();

            if (topInfo.nBearPhysics > 0 && topInfo.nBearPhysics == data.nBearPhysics)
                DataArrayText2[8].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nBearPhysics);
            else
                DataArrayText2[8].text = data.nBearPhysics.ToString();

            if (topInfo.nBearMagic > 0 && topInfo.nBearMagic == data.nBearMagic)
                DataArrayText2[9].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nBearMagic);
            else
                DataArrayText2[9].text = data.nBearMagic.ToString();

            if (topInfo.nBearTrue > 0 && topInfo.nBearTrue == data.nBearTrue)
                DataArrayText2[10].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nBearTrue);
            else
                DataArrayText2[10].text = data.nBearTrue.ToString();

            if (topInfo.nGetMoney > 0 && topInfo.nGetMoney == data.nGetMoney)
                DataArrayText2[11].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nGetMoney);
            else
                DataArrayText2[11].text = data.nGetMoney.ToString();

            if (topInfo.nCureTotal > 0 && topInfo.nCureTotal == data.nCureTotal)
                DataArrayText2[12].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nCureTotal);
            else
                DataArrayText2[12].text = data.nCureTotal.ToString();

            if (topInfo.nRecoveryHP > 0 && topInfo.nRecoveryHP == data.nRecoveryHP)
                DataArrayText2[13].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nRecoveryHP);
            else
                DataArrayText2[13].text = data.nRecoveryHP.ToString();

            if (topInfo.nMultiKillMax > 0 && topInfo.nMultiKillMax == data.nMultiKillMax)
                DataArrayText3[0].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nMultiKillMax);
            else
                DataArrayText3[0].text = data.nMultiKillMax.ToString();

            if (topInfo.nContinueKillMax > 0 && topInfo.nContinueKillMax == data.nContinueKillMax)
                DataArrayText3[1].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nContinueKillMax);
            else
                DataArrayText3[1].text = data.nContinueKillMax.ToString();

            if (topInfo.nAliveAssistMax > 0 && topInfo.nAliveAssistMax == data.nAliveAssistMax)
                DataArrayText3[2].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nAliveAssistMax);
            else
                DataArrayText3[2].text = data.nAliveAssistMax.ToString();

            if (topInfo.nThrillSave > 0 && topInfo.nThrillSave == data.nThrillSave)
                DataArrayText3[3].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nThrillSave);
            else
                DataArrayText3[3].text = data.nThrillSave.ToString();

            if (topInfo.nCrildDamageMax > 0 && topInfo.nCrildDamageMax == data.nCrildDamageMax)
                DataArrayText3[4].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nCrildDamageMax);
            else
                DataArrayText3[4].text = data.nCrildDamageMax.ToString();

            if (topInfo.nKillTowerNum > 0 && topInfo.nKillTowerNum == data.nKillTowerNum)
                DataArrayText3[5].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nKillTowerNum);
            else
                DataArrayText3[5].text = data.nKillTowerNum.ToString();

            if (topInfo.nKillDragen > 0 && topInfo.nKillDragen == data.nKillDragen)    // 击杀小龙
                DataArrayText3[6].text = String.Format("<color=#FFCE34FF>{0}</color>", data.nKillDragen);
            else
                DataArrayText3[6].text = data.nKillDragen.ToString();
		}

		// 清除玩家状态
		public void clearPlayerInfo()
		{
			foreach (var item in DataArrayText1)
			{
				item.text = "-";
			}

			foreach (var item in DataArrayText2)
			{
				item.text = "-";
			}

			foreach (var item in DataArrayText3)
			{
				item.text = "-";
			}


            selfImage.gameObject.SetActive(false);
		}
	}

	// 网格数据子窗口
	public class WSGridDataSubWnd : WarStatisticWndSubWnd
	{
		// 战绩,数据表格
		public UWarStatisticPlayerInfo[] BlueTeamInfo;
		public UWarStatisticPlayerInfo[] RedTeamInfo;

        public Image[] BlueTeamHead;
        public Image[] RedTeamHead;

        public Text[] TitleLabel;
        public Transform DataArrayLabel1;
        public Transform DataArrayLabel2;
        public Transform DataArrayLabel3;

        public override bool Init()
		{
			// 初始化数据网格界面
			foreach (var item in RedTeamInfo)
				item.Init();
			foreach (var item in BlueTeamInfo)
				item.Init();

            foreach (var item in BlueTeamHead)
                item.gameObject.SetActive(false);
            foreach (var item in RedTeamHead)
                item.gameObject.SetActive(false);

            TitleLabel[0].text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "BattleStat");
            TitleLabel[1].text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "TeamContribution");
            TitleLabel[2].text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "GoodBehaviour");

            DataArrayLabel1.FindChild("Text1").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "KillNumDesc");
            DataArrayLabel1.FindChild("Text2").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "DieNumDesc");
            DataArrayLabel1.FindChild("Text3").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "AssistNumDesc");
            DataArrayLabel1.FindChild("Text4").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ScoreDesc");
            DataArrayLabel1.FindChild("Text5").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "GPM");
            DataArrayLabel1.FindChild("Text6").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "FirstBlood");
            DataArrayLabel1.FindChild("Text7").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "DeadlyControl");

            DataArrayLabel2.FindChild("Text1").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "MoneyRateRate");
            DataArrayLabel2.FindChild("Text2").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "OutputRate");
            DataArrayLabel2.FindChild("Text3").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "JoinBattleRate");
            DataArrayLabel2.FindChild("Text4").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "OutDamageTotal");
            DataArrayLabel2.FindChild("Text5").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "OutDamagePhysics");
            DataArrayLabel2.FindChild("Text6").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "OutDamageMagic");
            DataArrayLabel2.FindChild("Text7").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "OutDamageTrue");
            DataArrayLabel2.FindChild("Text8").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "BearDamageTotal");
            DataArrayLabel2.FindChild("Text9").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "BearDamagePhysics");
            DataArrayLabel2.FindChild("Text10").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "BearDamageMagic");
            DataArrayLabel2.FindChild("Text11").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "BearDamageTrue");
            DataArrayLabel2.FindChild("Text12").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "GetMoney");
            DataArrayLabel2.FindChild("Text13").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "CureTotal");
            DataArrayLabel2.FindChild("Text14").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "RecoveryHP");

            DataArrayLabel3.FindChild("Text1").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "MultiKillMax");
            DataArrayLabel3.FindChild("Text2").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ContinueKillMax");
            DataArrayLabel3.FindChild("Text3").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "AliveAssistMax");
            DataArrayLabel3.FindChild("Text4").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "ThrillSave");
            DataArrayLabel3.FindChild("Text5").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "CrildDamageMax");
            DataArrayLabel3.FindChild("Text6").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "KillTowerNum");
            DataArrayLabel3.FindChild("Text7").GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "WarStatistic", "KillSmallDragen");

            return true;
		}

		public override void Clear()
		{
			// 初始化数据网格界面
			foreach (var item in RedTeamInfo)
				item.clearPlayerInfo();
			foreach (var item in BlueTeamInfo)
				item.clearPlayerInfo();

            foreach (var item in BlueTeamHead)
                item.gameObject.SetActive(false);
            foreach (var item in RedTeamHead)
                item.gameObject.SetActive(false);
        }

		public override void UpdateView(WarStatisticInfo info)
		{
			// 设置数据网格数据
			int nRedIndex = 0;
			int nBlueIndex = 0;
            cmd_entity_update_warstatistic_war_actorinfo topInfo = getTopWarStat(info);
            for (int j = 0; j < info.mainInfo.nActorCount; ++j)
			{
				if (info.actorInfo[j].nCamp == (int)CAMP_TYPE.CAMP_BLUE)
				{
                    // 玩家战绩信息
					BlueTeamInfo[nBlueIndex].setPlayerInfo(info.actorInfo[j], topInfo);

                    // 英雄头像
                    Sprite sp = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 3, info.actorInfo[j].nHeroID);
                    BlueTeamHead[nBlueIndex].sprite = sp;
                    BlueTeamHead[nBlueIndex].gameObject.SetActive(true);

                    ++nBlueIndex;
                }
				else if (info.actorInfo[j].nCamp == (int)CAMP_TYPE.CAMP_RED)
				{
                    // 玩家战绩信息
					RedTeamInfo[nRedIndex].setPlayerInfo(info.actorInfo[j], topInfo);

                    // 玩家英雄头像
                    Sprite sp = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_PLAYERFRAME, 1, 3, info.actorInfo[j].nHeroID);
                    RedTeamHead[nRedIndex].sprite = sp;
                    RedTeamHead[nRedIndex].gameObject.SetActive(true);

                    ++nRedIndex;
				}
			}

			// 清空剩下的玩家信息
			for (; nBlueIndex < UDefines.WAR_PERSON_MAX_COUNT / 2; ++nBlueIndex)
            {
				BlueTeamInfo[nBlueIndex].clearPlayerInfo();
                BlueTeamHead[nBlueIndex].gameObject.SetActive(false);
            }
			for (; nRedIndex < UDefines.WAR_PERSON_MAX_COUNT / 2; ++nRedIndex)
            {
				RedTeamInfo[nRedIndex].clearPlayerInfo();
                RedTeamHead[nRedIndex].gameObject.SetActive(false);
            }
		}

        // 计算最佳战绩
        private cmd_entity_update_warstatistic_war_actorinfo getTopWarStat(WarStatisticInfo info)
        {
            cmd_entity_update_warstatistic_war_actorinfo topInfo = new cmd_entity_update_warstatistic_war_actorinfo();

            for(int i = 0; i < info.mainInfo.nActorCount; ++i)
            {
                if (info.actorInfo[i].nBearTrue > topInfo.nBearTrue)
                    topInfo.nBearTrue = info.actorInfo[i].nBearTrue;
                if (info.actorInfo[i].nCureTotal > topInfo.nCureTotal)
                    topInfo.nCureTotal = info.actorInfo[i].nCureTotal;
                if (info.actorInfo[i].nRecoveryHP > topInfo.nRecoveryHP)
                    topInfo.nRecoveryHP = info.actorInfo[i].nRecoveryHP;
                if (info.actorInfo[i].nKdaScore > topInfo.nKdaScore)
                    topInfo.nKdaScore = info.actorInfo[i].nKdaScore;
                if (info.actorInfo[i].nBearPhysics > topInfo.nBearPhysics)
                    topInfo.nBearPhysics = info.actorInfo[i].nBearPhysics;
                if (info.actorInfo[i].bFirstBlood)
                    topInfo.bFirstBlood = info.actorInfo[i].bFirstBlood;
                if (info.actorInfo[i].nGPM > topInfo.nGPM)
                    topInfo.nGPM = info.actorInfo[i].nGPM;
                if (info.actorInfo[i].nDeadlyControl > topInfo.nDeadlyControl)
                    topInfo.nDeadlyControl = info.actorInfo[i].nDeadlyControl;
                if (info.actorInfo[i].nMoneyRate > topInfo.nMoneyRate)
                    topInfo.nMoneyRate = info.actorInfo[i].nMoneyRate;
                if (info.actorInfo[i].nOutputRate > topInfo.nOutputRate)
                    topInfo.nOutputRate = info.actorInfo[i].nOutputRate;
                if (info.actorInfo[i].nJoinBattleRate > topInfo.nJoinBattleRate)
                    topInfo.nJoinBattleRate = info.actorInfo[i].nJoinBattleRate;
                if (info.actorInfo[i].nThrillSave > topInfo.nThrillSave)
                    topInfo.nThrillSave = info.actorInfo[i].nThrillSave;
                if (info.actorInfo[i].nKillDragen > topInfo.nKillDragen)
                    topInfo.nKillDragen = info.actorInfo[i].nKillDragen;
                if (info.actorInfo[i].nAliveAssistMax > topInfo.nAliveAssistMax)
                    topInfo.nAliveAssistMax = info.actorInfo[i].nAliveAssistMax;
                if (info.actorInfo[i].nBearMagic > topInfo.nBearMagic)
                    topInfo.nBearMagic = info.actorInfo[i].nBearMagic;
                if (info.actorInfo[i].nDamageTotal > topInfo.nDamageTotal)
                    topInfo.nDamageTotal = info.actorInfo[i].nDamageTotal;
                if (info.actorInfo[i].nOutTotal > topInfo.nOutTotal)
                    topInfo.nOutTotal = info.actorInfo[i].nOutTotal;
                if (info.actorInfo[i].nScore > topInfo.nScore)
                    topInfo.nScore = info.actorInfo[i].nScore;
                if (info.actorInfo[i].nGetMoney > topInfo.nGetMoney)
                    topInfo.nGetMoney = info.actorInfo[i].nGetMoney;
                if (info.actorInfo[i].nDieCount > topInfo.nDieCount)
                    topInfo.nDieCount = info.actorInfo[i].nDieCount;
                if (info.actorInfo[i].nAssistCount > topInfo.nAssistCount)
                    topInfo.nAssistCount = info.actorInfo[i].nAssistCount;
                if (info.actorInfo[i].nKillPersonNum > topInfo.nKillPersonNum)
                    topInfo.nKillPersonNum = info.actorInfo[i].nKillPersonNum;
                if (info.actorInfo[i].nKillMonsterNum > topInfo.nKillMonsterNum)
                    topInfo.nKillMonsterNum = info.actorInfo[i].nKillMonsterNum;
                if (info.actorInfo[i].nContinueKillMax > topInfo.nContinueKillMax)
                    topInfo.nContinueKillMax = info.actorInfo[i].nContinueKillMax;
                if (info.actorInfo[i].nMultiKillMax > topInfo.nMultiKillMax)
                    topInfo.nMultiKillMax = info.actorInfo[i].nMultiKillMax;
                if (info.actorInfo[i].nCrildDamageMax > topInfo.nCrildDamageMax)
                    topInfo.nCrildDamageMax = info.actorInfo[i].nCrildDamageMax;
                if (info.actorInfo[i].nKillTowerNum > topInfo.nKillTowerNum)
                    topInfo.nKillTowerNum = info.actorInfo[i].nKillTowerNum;
                if (info.actorInfo[i].nOutHeroPhysics > topInfo.nOutHeroPhysics)
                    topInfo.nOutHeroPhysics = info.actorInfo[i].nOutHeroPhysics;
                if (info.actorInfo[i].nOutHeroMagic > topInfo.nOutHeroMagic)
                    topInfo.nOutHeroMagic = info.actorInfo[i].nOutHeroMagic;
                if (info.actorInfo[i].nOutHeroTrue > topInfo.nOutHeroTrue)
                    topInfo.nOutHeroTrue = info.actorInfo[i].nOutHeroTrue;
            }
            


            return topInfo;
        }
    }
	#endregion
}
