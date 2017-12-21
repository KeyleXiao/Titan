/*******************************************************************
** 文件名:	WarPlayersVsInfoWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-9-27
** 版  本:	1.0
** 描  述:	对战玩家界面
		
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
namespace USpeedUI.WarPlayersVsInfoWnd
{
    public class WarPlayersVsInfoWnd : UIPopupWnd<WarPlayersVsInfoView>
    {
        // 获取ID
        private int m_nModelPlayerCount = 10;
        public override WndID GetID()
        {
            return WndID.WND_ID_PLAYERSVSINFO;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "DynamicState_Moba/WarPlayersVsInfoView/WarPlayersVsInfoView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_OVERHEAD; ;
        }

        public override int GetLoadMask()
        {
            return (int)WndLoadMaskDef.WND_LoadMask_Moba;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);

            UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.OpenWarVSInfo, KeyStatus.Down, this);
            UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.OpenWarVSInfo, KeyStatus.Up, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);

            UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.OpenWarVSInfo, KeyStatus.Down, this);
            UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.OpenWarVSInfo, KeyStatus.Up, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        if (m_wndView )
                        {
                            return;
                        }

                        this.LoadView();
                        this.SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_WAREND:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        this.UnloadView();
                    }
                    break;
                default:
                    break;
            }
        }

        public override bool OnKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status)
        {
            if (m_wndView == null)
                return false;

			if (index == FuntionShortCutSlotIndex.OpenWarVSInfo)
			{
				if (status == KeyStatus.Down)
				{
                    m_wndView.UpdatePlayerVSViewShow();
                }
				else if (status == KeyStatus.Up)

				{
					m_wndView.HideMainView();
                }
            }

            return true;
        }


        public int GetPlayerModelCount()
        {
            return m_nModelPlayerCount;
        }
    }

    public class WarPlayersVsInfoView : UIBaseWndView
    {
        public GameObject DefualtWarPlayersVsItemObj;
        public GameObject MainView;
        public Transform WarPlayersVsListFrame;
        // public Transform BlueWarPlayersVsListFrame;
        private WarPlayersVsInfoItem[] m_WarPlayersVsItemList;
        private WarPlayersVsInfoWnd m_PlayerVsWnd;

        public override void Start()
        {
            return;
        }

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            if (DefualtWarPlayersVsItemObj)
            DefualtWarPlayersVsItemObj.SetActive(false);

            if(MainView)
            MainView.SetActive(false);

            m_PlayerVsWnd = wnd as WarPlayersVsInfoWnd;
            InitPlayerVsItemList();
            return true;
        }

        private void InitPlayerVsItemList()
        {
            if (WarPlayersVsListFrame == null || DefualtWarPlayersVsItemObj == null)
            {
                Debug.Log("WarPlayersVsListFrame == null || DefualtWarPlayersVsItemObj == null");
                return;
            }

            int nModelPlayerNum = m_PlayerVsWnd.GetPlayerModelCount();
            if (nModelPlayerNum <= 0)
            {
                Debug.Log("nModelPlayerNum <=0 ");
                return;
            }

            if (nModelPlayerNum % 2 != 0)
            {
                Debug.Log("nModelPlayerNum Is Odd Number");
                return;
            }

            m_WarPlayersVsItemList = new WarPlayersVsInfoItem[nModelPlayerNum];
            for (int i = 0; i < nModelPlayerNum; ++i)
            {
                GameObject newPlayerVsItemObj = ResNode.InstantiateRes(DefualtWarPlayersVsItemObj);
                if (newPlayerVsItemObj == null)
                {
                    return;
                }

                WarPlayersVsInfoItem newPlayerVsItem = newPlayerVsItemObj.GetComponent<WarPlayersVsInfoItem>();
                if (newPlayerVsItem == null)
                {
                    return;
                }

                //Transform parentTransform = i >= nModelPlayerNum / 2 ? WarPlayersVsListFrame : BlueWarPlayersVsListFrame;
                newPlayerVsItemObj.SetActive(true);
                newPlayerVsItemObj.transform.SetParent(WarPlayersVsListFrame);
                m_WarPlayersVsItemList[i] = newPlayerVsItem;
            }
        }

        public override void Update()
        {
            //base.Update();
            //if (InputManager.GetKeyDown(KeyCode.Tab))
            //{
            //     UpdatePlayerVSViewShow();
            //}

            //if (InputManager.GetKeyUp(KeyCode.Tab))
            //{
            //    if(MainView)
            //        MainView.SetActive(false);
            //}
        }

        public void HideMainView()
        {
            if (MainView)
                MainView.SetActive(false);
        }

        public void UpdatePlayerVSViewShow()
        {
            if(MainView == null)
            {
                return;
            }

            MainView.SetActive(true);
            this.transform.SetAsLastSibling();
            if (m_WarPlayersVsItemList == null)
            {
                return;
            }

            int nModelPlayerNum = m_PlayerVsWnd.GetPlayerModelCount();
            if (nModelPlayerNum <= 0)
            {
                Debug.Log("nModelPlayerNum <=0 ");
                return;
            }

            if (nModelPlayerNum % 2 != 0)
            {
                Debug.Log("nModelPlayerNum Is Odd Number");
                return;
            }

            Dictionary<ulong, DataCenter.WarRecordTableDataManager.WarPlayerVsInfo> dicPlayersVsInfo = LogicDataCenter.recordTableDataManager.GetWarDicPlayerVsInfo();
            if (dicPlayersVsInfo == null)
            {
                return;
            }

            foreach (WarPlayersVsInfoItem item in m_WarPlayersVsItemList)
            {
                item.ResetData();
            }

            foreach (DataCenter.WarRecordTableDataManager.WarPlayerVsInfo info in dicPlayersVsInfo.Values)
            {
                int nCampSlotID = LogicDataCenter.recordTableDataManager.GetAlwaysMyTeamLeftIndex(info.nCampIndex, info.nCamp + 1, (int)CAMP_TYPE.CAMP_BLUE);
                if (IsOutOfPlayerVsArray(nCampSlotID))
                {
                    return;
                }

                m_WarPlayersVsItemList[nCampSlotID].UpdatePlayerVSViewShow(info);
            }
        }

        public bool IsOutOfPlayerVsArray(int nSlotID)
        {
            if (nSlotID < 0 || nSlotID >= m_WarPlayersVsItemList.Length || m_WarPlayersVsItemList[nSlotID] == null)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public void MovePositionInWar()
        {
            RectTransform transForm = this.transform.GetComponent<RectTransform>();
            transForm.anchorMax = new Vector2(0f, 0.5f);
            transForm.anchorMin = new Vector2(0f, 0.5f);

            transForm.anchoredPosition = new Vector3(200, this.transform.localPosition.y - 40);
        }
    }

    public class WarPlayersVsInfoItem : MonoBehaviour
    {
        public Image HeroIconImage;
        public Image SexIconImage;
        public Text RankLevelText;
        public Text GameNumText;
        public Text WinRateText;
        public GameObject NoPlayerObj;
		public Image playerBackGround;
		public Sprite[] SexSprites;

        public void UpdatePlayerVSViewShow(WarRecordTableDataManager.WarPlayerVsInfo info)
        {
            if (HeroIconImage == null || SexIconImage == null || RankLevelText == null || GameNumText == null || WinRateText == null || SexSprites == null || NoPlayerObj == null)
            {
                return;
            }

            int nSexID = info.nSex;
            if (nSexID < 0 || nSexID >= SexSprites.Length || SexSprites[nSexID] == null)
            {
                return;
            }

            HeroIconImage.gameObject.SetActive(true);
            HeroIconImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_BOTTOMCENTERVIEW, 1, 7, info.nHeroID);
            SexIconImage.sprite = SexSprites[nSexID];
            RankLevelText.text = info.sRankDesc;
            GameNumText.text = info.nHeroGameNum.ToString();
            WinRateText.text = "("+info.fHeroWinRate.ToString("0.0")+"%)";
            NoPlayerObj.SetActive(false);

			// 判断是否本地玩家
			if(EntityFactory.MainHeroView != null)
			{
				int pdbid = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
				if(pdbid == info.uPdbid)
				{
					playerBackGround.gameObject.SetActive(true);
				}
			}
		}

        public void ResetData()
        {
            int nSexID =0;
            if (SexSprites == null || nSexID < 0 || nSexID >= SexSprites.Length || SexSprites[nSexID] == null)
            {
                return;
            }

            if (HeroIconImage == null || SexIconImage == null || RankLevelText == null || GameNumText == null || WinRateText == null || SexSprites == null || NoPlayerObj == null)
            {
                return;
            }

            NoPlayerObj.SetActive(true);
            HeroIconImage.gameObject.SetActive(false);
            //HeroIconImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_BOTTOMCENTERVIEW, 1, 7, 1);
            SexIconImage.sprite = SexSprites[nSexID];
            RankLevelText.text = "--";
            GameNumText.text = "--";
            WinRateText.text = "(--)";
        }
    }
}
