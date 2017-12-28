/*******************************************************************
** 文件名:	GameWaitingRoomWndView.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-6-25
** 版  本:	1.0
** 描  述:	选人大厅
		
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
using USpeedUI.GameWaitingRoomPart;
using UIWidgets;
using USpeedUI.UExtensions;
using USpeedUI.UWidgets;
using UIWidgetsSamples;
using Stage_ManagerDef;
using System.Runtime.InteropServices;
using ASpeedGame.Data.HeroAIConfig;
using UIWidgetsMachines;
using UIWidgetsMachineAIs;
using ASpeedGame.Data.PersonModelTransFormInfo;
using DG.Tweening;
using USpeedUI.UEffect;
using U3D_Render.Common;
using Effect;
using USpeedUI.PointShop;
using ASpeedGame.Data.HeroTypeConfig;
using ASpeedGame.Data.MatchRoomOpenAILevel;
using Match_ManagerDef;
using Data.PointShopConfig;
using Data.PlayerLocalHeroConfig;
using Data.PersonModel;
using USpeedUI.UWidgets.UAnimator;

namespace USpeedUI
{
    #region 队伍part
    public class RoomTeamPart : MonoBehaviour
    {
        public RoomTeamFrame[] TeamFrameArray;
        private int m_nPatternCampPlayerCount = 0;
        private const int m_nRedCamp = 0;
        private const int m_nBlueCamp = 1;
        private EMHallRoomType m_RoomType = EMHallRoomType.HallRoomType_Unknown;
        private bool m_bIsRoomOwner = false;

        public void SetGameRoomModel(EMHallRoomType RoomType, bool bIsRoomOwner)
        {
            m_RoomType = RoomType;
            m_bIsRoomOwner = bIsRoomOwner;
            InitRoomModelTeamState();
        }

        public void ResetData()// 暂时离开房间
        {
            m_bIsRoomOwner = false;
            m_RoomType = EMHallRoomType.HallRoomType_Unknown;
            if (TeamFrameArray == null)
            {
                return;
            }

            for (int i = 0; i < TeamFrameArray.Length; ++i)
            {
                TeamFrameArray[i].ResetData();
            }
        }

        public void SetGamePatternTeamSlotCreate()
        {
            if (this.TeamFrameArray == null)
            {
                return;
            }

            if (this.TeamFrameArray.Length < m_nBlueCamp)
            {
                return;
            }

            SetTeamSlotItemCreate();
        }

        private void SetTeamSlotItemCreate()// 动态设槽位 
        {
            if (this.TeamFrameArray.Length != m_nBlueCamp + 1)
            {
                Debug.Log("CampCount Is Error" + this.TeamFrameArray.Length);
            }

            for (int i = 0; i < this.TeamFrameArray.Length; ++i)
            {
                TeamFrameArray[i].SetTeamSlotItemCreate(m_nPatternCampPlayerCount, i);
            }
        }

        public void SetAllTeamItemWarId(int nWarId)
        {
            if (TeamFrameArray == null)
            {
                return;
            }

            for (int i = 0; i < TeamFrameArray.Length; ++i)
            {
                TeamFrameArray[i].SetAllTeamItemWarId(nWarId);
            }
        }

        public void setAllPosNotBright()
        {
            if (TeamFrameArray == null)
            {
                return;
            }

            for (int i = 0; i < TeamFrameArray.Length; ++i)
            {
                TeamFrameArray[i].setAllPosNotBright();
            }
        }

        public void SetGamePatternCampPlayerCount(int nCampPlayerCount)
        {
            m_nPatternCampPlayerCount = nCampPlayerCount;
        }

        public void SetRoleEnterRoom(bool bSelf, bool bRoomOwner, int nCamp, int nHeroIconId, int nTeamSlotId, string sPlayerName, int nSelfCamp, bool bIsComputerPlayer)
        {
            if (IsOutofTeamFrameArray(nCamp))
            {
                return;
            }

            TeamFrameArray[nCamp].SetRoleEnterRoom(bSelf, bRoomOwner, nCamp, nHeroIconId, nTeamSlotId, sPlayerName, nSelfCamp, bIsComputerPlayer);
        }

        public bool IsOutofTeamFrameArray(int nCamp)
        {
            if (TeamFrameArray == null)
            {
                return true;
            }

            if (nCamp < 0 || nCamp >= TeamFrameArray.Length)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        private void InitRoomModelTeamState() // 根据比赛模式对队伍界面进行一系列的显示调整
        {
            if (m_RoomType == EMHallRoomType.HallRoomType_Unknown)
            {
                return;
            }

            if (m_RoomType == EMHallRoomType.HallRoomType_MacthMachine)
            {
                SetRoomModelComputer();
            }
            else if (m_RoomType == EMHallRoomType.HallRoomType_Macth)
            {
                SetRoomModelMatch();
            }
            else if (m_RoomType == EMHallRoomType.HallRoomType_Custom)
            {
                SetRoomModelCustom();
            }
            else if (m_RoomType == EMHallRoomType.HallRoomType_LegendCup)
            {
                SetRoomModelLegendCup();
            }
        }

        private void SetRoomModelRank()
        {

        }

        private void SetRoomModelMatch()
        {
            if (TeamFrameArray == null)
            {
                return;
            }

            for (int i = 0; i < TeamFrameArray.Length; ++i)
            {
                TeamFrameArray[i].SetMatchStateGameModel();
            }
        }

        private void SetRoomModelComputer()
        {

        }

        private void SetRoomModelCustom()// 对自定义模式下的队伍界面处理
        {
            if (TeamFrameArray == null)
            {
                return;
            }

            for (int i = 0; i < TeamFrameArray.Length; ++i)
            {
                TeamFrameArray[i].SetCustomStateGameModel(m_bIsRoomOwner);
            }
        }

        private void SetRoomModelLegendCup()
        {
            if (TeamFrameArray == null)
            {
                return;
            }

            for (int i = 0; i < TeamFrameArray.Length; ++i)
            {
                TeamFrameArray[i].SetLegendCupStateGameModel();
            }
        }

        public void SetChangeTeamItemHeroIcon(int nCamp, int nTeamSlotId, int nHeroIconId, int nHeroStarLv, int nHeroStaticLv)
        {
            if (IsOutofTeamFrameArray(nCamp))
            {
                return;
            }

            TeamFrameArray[nCamp].SetChangeTeamItemHeroIcon(nTeamSlotId, nHeroIconId, nHeroStarLv, nHeroStaticLv);
        }

        public void SetTeamItemHeroConfirm(int nCamp, int nSlotId)
        {
            if (IsOutofTeamFrameArray(nCamp))
            {
                return;
            }

            TeamFrameArray[nCamp].SetTeamItemHeroConfirm(nSlotId);
        }

        public void SetTeamItemSummerInfo(int nCamp, int nTeamSlotId, int nSummerIdOne, int nSummerIdTwo)
        {
            if (IsOutofTeamFrameArray(nCamp))
            {
                return;
            }

            TeamFrameArray[nCamp].SetTeamItemSummerInfo(nTeamSlotId, nSummerIdOne, nSummerIdTwo);
        }

        public void SetComputerHeadIconChange(cmd_match_room_computer_hero_change cmdData)
        {
            if (IsOutofTeamFrameArray(cmdData.nCamp))
            {
                return;
            }

            TeamFrameArray[cmdData.nCamp].SetComputerHeadIconChange(cmdData);
        }

        public void SetCampComputerCanUse(int nCamp, bool bSelect, int nHeroId)
        {
            if (IsOutofTeamFrameArray(nCamp))
            {
                return;
            }

            TeamFrameArray[nCamp].SetCampComputerCanUse(bSelect, nHeroId);
        }

        public void SetTeamItemClear(int nCamp, int nTeamSlotId)
        {
            if (IsOutofTeamFrameArray(nCamp))
            {
                return;
            }

            TeamFrameArray[nCamp].SetTeamItemClear(nTeamSlotId);
        }

        public void SetRoomTeamItemChangePosition(int nCamp, int nTeamSlotId, bool bIsSelf, bool bRoomOwner, string sPlayerName)
        {
            if (IsOutofTeamFrameArray(nCamp))
            {
                return;
            }

            TeamFrameArray[nCamp].SetRoomTeamItemChangePosition(nTeamSlotId, bIsSelf, bRoomOwner, sPlayerName);
        }

        public void SetTeamItemRoomOwner(int nCamp, int nTeamSlotId)
        {
            if (IsOutofTeamFrameArray(nCamp))
            {
                return;
            }

            TeamFrameArray[nCamp].SetTeamItemRoomOwner(nTeamSlotId);
        }

        public void SetRoomTeamItemChangePosition(int nCampFrom, int nCampTo, int nPlayerSlotIDFrom, int nPlayerSlotIDTo)
        {
            RoomTeamItem item = GetRoomTeamItem(nCampFrom, nPlayerSlotIDFrom);
            if (item == null)
            {
                return;
            }

            if (IsOutofTeamFrameArray(nCampTo))
            {
                return;
            }

            if (IsOutofTeamFrameArray(nCampFrom))
            {
                return;
            }

            TeamFrameArray[nCampTo].SetRoomTeamItemChangePosition(nPlayerSlotIDTo, item.bIsSelf, item.RoomOwner, item.PlayerName);
            TeamFrameArray[nCampFrom].SetTeamItemClear(nPlayerSlotIDFrom);
        }

        public RoomTeamItem GetRoomTeamItem(int nCamp, int nTeamSlotId)
        {
            if (IsOutofTeamFrameArray(nCamp))
            {
                return null;
            }

            return TeamFrameArray[nCamp].GetRoomTeamItem(nTeamSlotId);
        }

        public void SetHideTeamCustomModeButtons()
        {
            if (!m_bIsRoomOwner)
            {
                return;
            }

            if (m_RoomType != EMHallRoomType.HallRoomType_Custom)
            {
                return;
            }

            if (TeamFrameArray == null)
            {
                return;
            }

            for (int i = 0; i < TeamFrameArray.Length; ++i)
            {
                TeamFrameArray[i].SetHideTeamCustomModeButtons();
            }
        }

        public int GetRoomTeamItemComputerId(int nCamp, int nTeamSlotId)
        {
            if (IsOutofTeamFrameArray(nCamp))
            {
                return 0;
            }

            return TeamFrameArray[nCamp].GetRoomTeamItemComputerId(nTeamSlotId);
        }

        public int GetTeamItemHeroId(int nCamp, int nSlotId)
        {
            if (IsOutofTeamFrameArray(nCamp))
            {
                return 0;
            }

            return TeamFrameArray[nCamp].GetTeamItemHeroId(nSlotId);
        }

        public int GetTeamCampPlayerCount(int nCamp)
        {
            if (IsOutofTeamFrameArray(nCamp))
            {
                return 0;
            }

            return TeamFrameArray[nCamp].GetTeamCampPlayerCount();
        }

        public bool GetTeamItemIsSelf(int nCamp, int nSlotId)
        {
            if (IsOutofTeamFrameArray(nCamp))
            {
                return false;
            }

            return TeamFrameArray[nCamp].GetTeamItemIsSelf(nSlotId);
        }


        #region 队伍元素动画部分
        public UViewAnim AnimTeamFrameItem;
        public Vector2 AnimItemOffset = new Vector2(30,0);
        public float AnimItemDelayOffset = 0.1f;
        public float AnimItemDuration = 0.5f;
        public void SetTeamFrameAnim()
        {
            if (AnimTeamFrameItem == null)
                AnimTeamFrameItem = gameObject.AddComponent<UViewAnim>();
            AnimTeamFrameItem.EnterState = new UViewAnimState();

            float curInsertDelta = 0f;
            RectTransform curTargetTrans;
            for (int i = 0; i < TeamFrameArray.Length; ++i)
            {
                curInsertDelta = 0f;
                for (int j=0;j< TeamFrameArray[i].TeamItemArray.Length;++j)
                {
                    curTargetTrans = TeamFrameArray[i].TeamItemArray[j].transform as RectTransform;
                    //move
                    {
                        UViewAnimElement element = new UViewAnimElement();
                        element.TweenType = UViewAnimElement.ETweenType.ETT_AnchorPosition;
                        element.TweenAttachType = UViewAnimElement.ETweenAttachType.ETA_Insert;
                        element.TweenAttachType_Insert_Pos = curInsertDelta;
                        element.DurationValue = AnimItemDuration;
                        element.DelayValue = curInsertDelta;
                        element.IsCustomTarget = true;
                        element.CustomTarget = curTargetTrans;
                        element.Vector2TypeValue.BeginValue = curTargetTrans.anchoredPosition + AnimItemOffset * (i == 0?-1:1);
                        element.Vector2TypeValue.EndValue = curTargetTrans.anchoredPosition;


                        AnimTeamFrameItem.EnterState.AddElement(element);
                    }
                    //fade
                    {
                        UViewAnimElement element = new UViewAnimElement();
                        element.TweenType = UViewAnimElement.ETweenType.ETT_Fade;
                        element.TweenAttachType = UViewAnimElement.ETweenAttachType.ETA_Insert;
                        element.TweenAttachType_Insert_Pos = curInsertDelta;
                        element.DurationValue = AnimItemDuration;
                        element.DelayValue = curInsertDelta;
                        element.IsCustomTarget = true;
                        element.CustomTarget = curTargetTrans;
                        element.FloatTypeValue.BeginValue = 0f;
                        element.FloatTypeValue.EndValue = 1;

                        AnimTeamFrameItem.EnterState.AddElement(element);
                    }

                    curInsertDelta += AnimItemDelayOffset;
                }
            }


        }
        #endregion
    }

    public class RoomTeamFrame : MonoBehaviour
    {
        public RoomTeamItem[] TeamItemArray;

        public bool IsOutofTeamItemArray(int nSlotId)
        {
            if (TeamItemArray == null)
            {
                return true;
            }

            if (nSlotId < 0 || nSlotId >= TeamItemArray.Length)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public void setAllPosNotBright()
        {
            if (TeamItemArray == null)
            {
                return;
            }

            for (int i = 0; i < TeamItemArray.Length; i++)
            {
                //TeamItemArray[i].SetTeamItemSlotLight();
            }
        }

        public void SetRoleEnterRoom(bool bIsSelf, bool bRoomOwner, int nCamp, int nHeroIconId, int nTeamSlotId, string sPlayerName, int nSelfCamp, bool bIsComputerPlayer)
        {
            if (IsOutofTeamItemArray(nTeamSlotId))
            {
                return;
            }

            TeamItemArray[nTeamSlotId].SetRoleEnterRoom(bIsSelf, bRoomOwner, nCamp, nTeamSlotId, nHeroIconId, sPlayerName, nSelfCamp, bIsComputerPlayer);
        }

        public void ResetData()
        {
            TeamItemArray = null;
            int count = transform.childCount;
            for (int j = 0; j < count; j++)
            {
                var itemChild = this.gameObject.transform.GetChild(0).gameObject;
                RoomTeamItem item = itemChild.GetComponent<RoomTeamItem>();
                item.ResetData();
                item.ResetModel();

                ResNode.DestroyRes(ref itemChild);
            }
        }

        public void SetTeamSlotItemCreate(int nPatternCampPlayerCount, int nCamp)
        {
            GameWaitingRoomView view = transform.parent.parent.gameObject.GetComponent<GameWaitingRoomView>();
            TeamItemArray = new RoomTeamItem[nPatternCampPlayerCount];
            for (int i = 0; i < nPatternCampPlayerCount; i++)
            {
                string sPrefabName = "";
                int nInitOffset = 0;
                if (nCamp == 0)
                {
                    sPrefabName = "TeamItemRed";
                    nInitOffset = 59;
                }
                else
                {
                    sPrefabName = "TeamItemBlue";
                    nInitOffset = -59;
                }

                ResNode TeamItemPrefab = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,UPath.UPrefabsPath + "GameWaitingRoomView/" + sPrefabName);
                if (TeamItemPrefab == null)
                {
                    Debug.Log("TeamItemPrefab Is Null");
                    return;
                }

                GameObject TeamItemObject = TeamItemPrefab.InstanceMainRes();
                if (TeamItemObject == null)
                {
                    return;
                }

                RoomTeamItem item = TeamItemObject.GetComponent<RoomTeamItem>();
                item.Camp = nCamp;
                item.SlotId = i;
                TeamItemArray[i] = item;
                TeamItemArray[i].SetWndView(view);
                TeamItemObject.transform.SetParent(this.transform, false);
                Vector3 vOldPosition = TeamItemObject.transform.localPosition;
                float fNewPositionY = vOldPosition.y - i * 100;
                float fNewPositionX = vOldPosition.x;
                // 进行位置上的处理
                if (i % 2 != 0)
                {
                    fNewPositionX += nInitOffset;
                }

                TeamItemObject.transform.localPosition = new Vector3(fNewPositionX, fNewPositionY, 0);
            }
        }

        public void SetChangeTeamItemHeroIcon(int nTeamSlotId, int nHeroIconId, int nHeroStarLv, int nHeroStaticLv)
        {
            if (IsOutofTeamItemArray(nTeamSlotId))
            {
                return;
            }

            TeamItemArray[nTeamSlotId].SetChangeTeamItemHeroIcon(nHeroIconId, nHeroStarLv, nHeroStaticLv);
        }

        public void SetTeamItemHeroConfirm(int nTeamSlotId)
        {
            if (IsOutofTeamItemArray(nTeamSlotId))
            {
                return;
            }

            TeamItemArray[nTeamSlotId].SetTeamItemHeroConfirm();
        }

        public void SetTeamItemSummerInfo(int nTeamSlotId, int nSummerIdOne, int nSummerIdTwo)
        {
            if (IsOutofTeamItemArray(nTeamSlotId))
            {
                return;
            }

            TeamItemArray[nTeamSlotId].SetTeamItemSummerInfo(nSummerIdOne, nSummerIdTwo);
        }

        public void SetAllTeamItemWarId(int nWarId)
        {
            if (TeamItemArray == null)
            {
                return;
            }

            for (int i = 0; i < TeamItemArray.Length; i++)
            {
                TeamItemArray[i].WarId = nWarId;
            }
        }

        public void SetCustomStateGameModel(bool bIsRoomOwner)
        {
            if (TeamItemArray == null)
            {
                return;
            }

            for (int i = 0; i < TeamItemArray.Length; i++)
            {
                TeamItemArray[i].SetCustomStateGameModel(bIsRoomOwner);
            }
        }

        public void SetComputerHeadIconChange(cmd_match_room_computer_hero_change cmdData)
        {
            if (IsOutofTeamItemArray(cmdData.nPlayerSlotID))
            {
                return;
            }

            TeamItemArray[cmdData.nPlayerSlotID].SetComputerHeadIconChange(cmdData.nHeroID, cmdData.nAIID, cmdData.szHeroName);
        }

        public void SetCampComputerCanUse(bool bSelect, int nHeroId)
        {
            if (TeamItemArray == null)
            {
                return;
            }

            for (int i = 0; i < TeamItemArray.Length; i++)
            {
                TeamItemArray[i].SetCampComputerCanUse(bSelect, nHeroId);
            }
        }

        public void SetTeamItemClear(int nTeamSlotId)
        {
            if (IsOutofTeamItemArray(nTeamSlotId))
            {
                return;
            }

            TeamItemArray[nTeamSlotId].SetTeamItemClear();
        }

        public void SetTeamItemRoomOwner(int nTeamSlotId)
        {
            if (IsOutofTeamItemArray(nTeamSlotId))
            {
                return;
            }

            TeamItemArray[nTeamSlotId].SetTeamItemRoomOwner();
            
        }

        public RoomTeamItem GetRoomTeamItem(int nTeamSlotId)
        {
            if (IsOutofTeamItemArray(nTeamSlotId))
            {
                return null;
            }

            return TeamItemArray[nTeamSlotId];
        }

        public void SetRoomTeamItemChangePosition(int nPlayerSlotID, bool bIsSelf, bool bRoomOwner, string sPlayerName)
        {
            if (IsOutofTeamItemArray(nPlayerSlotID))
            {
                return;
            }

            TeamItemArray[nPlayerSlotID].SetRoomTeamItemChangePosition(bIsSelf, bRoomOwner, sPlayerName);
        }

        public void SetHideTeamCustomModeButtons()
        {
            if (TeamItemArray == null)
            {
                return;
            }

            for (int i = 0; i < TeamItemArray.Length; i++)
            {
                TeamItemArray[i].SetHideTeamCustomModeButtons();
            }
        }

        public int GetRoomTeamItemComputerId(int nTeamSlotId)
        {
            if (IsOutofTeamItemArray(nTeamSlotId))
            {
                return 0;
            }

            return TeamItemArray[nTeamSlotId].GetRoomTeamItemComputerId();
        }

        public int GetTeamItemHeroId(int nSlotId)
        {
            if (IsOutofTeamItemArray(nSlotId))
            {
                return 0;
            }

            return TeamItemArray[nSlotId].GetTeamItemHeroId();
        }

        public int GetTeamCampPlayerCount()
        {
            if (TeamItemArray == null)
            {
                return 0;
            }

            int nCount = 0;
            for (int i = 0; i < TeamItemArray.Length; i++)
            {
                nCount += TeamItemArray[i].GetTeamCampPlayerCount();
            }

            return nCount;
        }

        public void SetMatchStateGameModel()
        {
            if (TeamItemArray == null)
            {
                return;
            }

            for (int i = 0; i < TeamItemArray.Length; i++)
            {
                TeamItemArray[i].SetMatchStateGameModel();
            }
        }

        public bool GetTeamItemIsSelf(int nSlotId)
        {
            if (IsOutofTeamItemArray(nSlotId))
            {
                return false;
            }

            return TeamItemArray[nSlotId].bIsSelf;
        }

        public void SetLegendCupStateGameModel()
        {
            if (TeamItemArray == null)
            {
                return;
            }

            for (int i = 0; i < TeamItemArray.Length; i++)
            {
                TeamItemArray[i].SetLegendCupStateGameModel();
            }
        }
    }

    public class RoomTeamItem : MonoBehaviour
    {
        public enum TeamSlotFrameType
        {
            TEAMSLOTFRAMETYPE_SELF = 0,
            TEAMSLOTFRAMETYPE_OTHER,
            //TEAMSLOTFRAMETYPE_RANDOM,
            //TEAMSLOTFRAMETYPE_MACHINE,
        };

        public GameObject HeroStarEffectFrame;
        public Image HeroStarGradeFrame;
        public GameObject OtherFrame;
        public Button MachineAddButton;
        public GameObject HeadMask;
        public GameObject RemoveSlotButton;
        public GameObject RandomObject;
        public RectTransform HeroStarFrame;
        public Image SummerOneImage;
        public Image HeroIconImage;
        public ComboboxMachineListView ComputerSelectCombo;
        public ComboboxMachineAIListView ComputerDifficultyCombo;
        public GameObject ComputerSelectComboObject;
        public GameObject ComputerDifficultyComboObject;
        public GameObject OwnerFlag;
        public Text PlayerNameText;
        private bool m_bOwner = false;
        private bool m_bCanChangeSlot;
        private int m_nCamp = 0;
        private int m_nSlotId = 0;
        private int m_nWarId = 0;
        private bool m_bHideItemAllView = false;
        private bool m_bHideItemAllExceptMask = false;
        private bool m_bIsRoomOwner = false;
        private bool m_bIsSelf = false;
        private int m_nHeroId = 0;
        private int m_nCurComputerId = 0;
        private int m_nCurComputerAIId = 0;
        private bool m_bHasPlayer = false;
        private Dictionary<int, int> dicHeroIdWithComboIndex = new Dictionary<int, int>();
        private int m_nSelfCamp = 0;
        private HeroStarItem heroStarItem;
        private bool m_bComputerVisible = false;
        private bool m_bIsRealPlayer = false;
        private GameWaitingRoomView m_wndView;

        private UEffectParamBase param;

        public virtual void Awake()
        {
            ResetData();
            ResetModel();
        }

        public void SetWndView(GameWaitingRoomView view)
        {
            m_wndView = view;
        }

        public void ResetData()
        {
            m_bIsRealPlayer = false;
            m_bHasPlayer = false;
            m_nHeroId = 0;
            m_bIsSelf = false;
            m_bOwner = false;
            m_nCurComputerId = 0;
            m_bComputerVisible = false;

            PlayerNameText.gameObject.SetActive(true);
            HeadMask.SetActive(false);
            RemoveSlotButton.SetActive(false);
            MachineAddButton.gameObject.SetActive(false);
            OwnerFlag.SetActive(false);
            RandomObject.SetActive(false);
            if (heroStarItem == null)
            {
                heroStarItem = LogicDataCenter.pointShopDataManager.CreateHeroStarItem(HeroStarFrame);
            }
            heroStarItem.gameObject.SetActive(false);
            SummerOneImage.gameObject.SetActive(false);
            PlayerName = "";
            ComputerDifficultyComboObject.SetActive(false);
            ComputerSelectComboObject.SetActive(false);
            HeroIconImage.gameObject.SetActive(false);
            HeroStarGradeFrame.gameObject.SetActive(false);
            HeroStarEffectFrame.SetActive(false);
            ChangeItemSlotFrame(TeamSlotFrameType.TEAMSLOTFRAMETYPE_OTHER);
        }

        public void ResetModel()
        {
            m_bIsRoomOwner = false;
            m_bHideItemAllView = false;
            m_bHideItemAllExceptMask = false;
        }

        public virtual void Destroy()
        {
            dicHeroIdWithComboIndex.Clear();

            if (param != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
            }
        }

        public void SetRoleEnterRoom(bool bIsSelf, bool bRoomOwner, int nCamp, int nSlotId, int nHeroIconId, string sPlayerName, int nSelfCamp, bool bIsComputerPlayer)
        {
            m_bIsRealPlayer = !bIsComputerPlayer;
            m_bHasPlayer = true;
            m_nSelfCamp = nSelfCamp;
            RandomObject.SetActive(true);
            if (IsDifferenceCampCanNotSee())
            {
                return;
            }

            m_bIsSelf = bIsSelf;
            m_bOwner = bRoomOwner;
            PlayerNameText.text = sPlayerName;
            HeadMask.SetActive(false);
            MachineAddButton.gameObject.SetActive(false);
            OwnerFlag.SetActive(m_bOwner);

            if (bIsComputerPlayer)
            {
                SetComputerHeadIconChange(nHeroIconId, 0, sPlayerName);
                
                // 是电脑且是房主则隐藏显示名称
                if (m_bIsRoomOwner)
                {
                    PlayerNameText.gameObject.SetActive(false);
                }
                else
                {
                    PlayerNameText.gameObject.SetActive(m_bComputerVisible);
                }

                heroStarItem.gameObject.SetActive(false);
            }

            if (m_bIsRoomOwner)
            {
                RemoveSlotButton.SetActive(true);
            }

            if (m_bIsSelf)
            {
                ChangeItemSlotFrame(TeamSlotFrameType.TEAMSLOTFRAMETYPE_SELF);
                RemoveSlotButton.SetActive(false);
            }
        }

        public void SetCustomStateGameModel(bool bIsRoomOwner)// 自定义
        {
            m_bComputerVisible = true;
            m_bCanChangeSlot = true;
            if (!bIsRoomOwner)
            {
                return;
            }

            m_bIsRoomOwner = bIsRoomOwner;
            ComputerSelectCombo.OnSelect.RemoveListener(onChangeMachineBtnClick);
            ComputerSelectCombo.OnSelect.AddListener(onChangeMachineBtnClick);

            ComputerDifficultyCombo.OnSelect.RemoveListener(onChangeHeroAIBtnClick);
            ComputerDifficultyCombo.OnSelect.AddListener(onChangeHeroAIBtnClick);
            if (m_bIsRealPlayer)
            {
                RemoveSlotButton.SetActive(true);
                return;
            }

            if (m_nCurComputerId > 0)
            {
                RemoveSlotButton.SetActive(true);
                ComputerDifficultyComboObject.SetActive(true);
                ComputerSelectComboObject.SetActive(true);
                SetMachineInfo();
                SetComputerHeadIconChange(m_nCurComputerId, 0);
                PlayerNameText.gameObject.SetActive(false);
            }
            else
            {
                MachineAddButton.gameObject.SetActive(true);
                RemoveSlotButton.SetActive(false);
                ComputerDifficultyComboObject.SetActive(false);
                ComputerSelectComboObject.SetActive(false);
            }
        }

        public void SetMachineStateGameModel()// 人机模式
        {
            HeadMask.SetActive(true);
            m_bHideItemAllView = true;
        }

        public void SetMatchStateGameModel()// 匹配模式
        {
            m_bHideItemAllExceptMask = true;
            m_bComputerVisible = true;
        }

        public void SetLegendCupStateGameModel()// 杯赛模式
        {

        }

        public void SetChangeTeamItemHeroIcon(int nHeroIconId, int nHeroStarLv, int nHeroStaticLv)
        {
            if (IsDifferenceCampCanNotSee())
            {
                return;
            }

            m_nHeroId = nHeroIconId;
            if ((VOCATION)nHeroIconId == VOCATION.VOCATION_INVALID)
            {
                RandomObject.SetActive(true);
                MachineAddButton.gameObject.SetActive(false);

                heroStarItem.SetData(0, 0, false, false);
                heroStarItem.gameObject.SetActive(m_nCurComputerId <= 0);

                HeroIconImage.gameObject.SetActive(false);
                HeroStarGradeFrame.gameObject.SetActive(false);
                HeroStarEffectFrame.SetActive(false);
            }
            else
            {
                OtherFrame.SetActive(false);
                RandomObject.SetActive(false);
                MachineAddButton.gameObject.SetActive(false);

                heroStarItem.SetData(nHeroIconId, nHeroStarLv, false, false);
                heroStarItem.gameObject.SetActive(m_nCurComputerId <= 0);

                HeroIconImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, 7, nHeroIconId);
                HeroIconImage.gameObject.SetActive(HeroIconImage.sprite == null ? false : true);
                if (nHeroStarLv < 0)
                {
                    HeroStarGradeFrame.gameObject.SetActive(false);
                    HeroStarEffectFrame.SetActive(false);
                }
                else
                {
                    int nStarGradeFrameId = GameLogicAPI.getHeroStarFrameID(nHeroIconId, nHeroStarLv);
                    HeroStarGradeFrame.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_StaticState, WndID.WND_ID_GAMEWAITINGROOMVIEW, nStarGradeFrameId + 20);
                    HeroStarGradeFrame.gameObject.SetActive(true);
                    // 星级光效设置
                    HeroStarEffectFrame.SetActive(true);
                    int nStarEffectID = GameLogicAPI.getHeroStarEffectID(nHeroIconId, nHeroStarLv);
                    if (param != null)
                    {
                        UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
                    }
                    if (nStarEffectID > 0)
                    {
                        param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_GameWaitingRoomView_GradeFrame1 + nStarEffectID - 1, _tfAttachParent: HeroStarEffectFrame.transform, _bAutoDestroy: false);
                        UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);
                    }
                }
            }

            // 玩家自身更换英雄,取本地的召唤师技能
            if (!m_bIsSelf)
                return;
            // 默认治疗
            int nSummonID;
            //if (!HeroSummonSettingLocalConfig.Instance.GetLocalData(m_nHeroId, out nSummonID) || nSummonID <= 0)
            //    nSummonID = 486;
            if (!PlayerLocalHeroConfig.Instance.GetIntData(ELocalHero.ELH_SummonID, m_nHeroId, out nSummonID) || nSummonID <= 0)
                nSummonID = 486;
            //return;
            // 设置左值
            if (m_wndView == null)
                return;
            if (m_wndView.BottomFunctionPart.SummerFrame.OutSummerButtonGroup.Length <= 0)
                return;
            if (m_wndView.BottomFunctionPart.SummerFrame.OutSummerButtonGroup[0] == null)
                return;
            m_wndView.BottomFunctionPart.SummerFrame.CurSelectIndex = m_wndView.BottomFunctionPart.SummerFrame.OutSummerButtonGroup[0].SlotID;
            // 设置右值
            foreach (RoomSummerButtonInSummerFrame item in m_wndView.BottomFunctionPart.SummerFrame.InSummerButtonGroupArray)
            {
                if (item != null && item.SkillID == nSummonID)
                {
                    item.OnSummerButtonInClick();
                    break;
                }
            }

            // 设置宝石页，默认0页
            int nGemPageIndex;
            //if (!HeroGemPageSettingConfig.Instance.GetLocalData(m_nHeroId, out nGemPageIndex) || nGemPageIndex < 0)
            //    nGemPageIndex = 0;
            if (!PlayerLocalHeroConfig.Instance.GetIntData(ELocalHero.ELH_GemPageID, m_nHeroId, out nGemPageIndex) || nGemPageIndex < 0)
                nGemPageIndex = 0;

            int nItemsCount = m_wndView.BottomFunctionPart.RuneComboboxList.GetItemsCount();
            if (nItemsCount <= 0)
                return;
            if (nGemPageIndex < 0 || nGemPageIndex >= nItemsCount)
                nGemPageIndex = 0;

            m_wndView.BottomFunctionPart.RuneComboboxList.SelectedIndex = nGemPageIndex;
        }

        public void SetTeamItemHeroConfirm()
        {
            HeadMask.SetActive(true);
        }

        public int Camp
        {
            set
            {
                m_nCamp = value;
            }
            get
            {
                return m_nCamp;
            }
        }

        public int SlotId
        {
            set
            {
                m_nSlotId = value;
            }
            get
            {
                return m_nSlotId;
            }
        }

        public int WarId
        {
            set
            {
                m_nWarId = value;
            }
            get
            {
                return m_nWarId;
            }
        }

        public bool CanChangeSlot
        {
            set
            {
                m_bCanChangeSlot = value;
            }
            get
            {
                return m_bCanChangeSlot;
            }
        }

        public bool IsRoomOwner
        {
            set
            {
                m_bIsRoomOwner = value;
            }
            get
            {
                return m_bIsRoomOwner;
            }
        }

        public bool RoomOwner
        {
            set
            {
                m_bOwner = value;
            }
            get
            {
                return m_bOwner;
            }
        }

        public bool bIsSelf
        {
            set
            {
                m_bIsSelf = value;
            }
            get
            {
                return m_bIsSelf;
            }
        }

        public bool HasPlayer
        {
            set
            {
                m_bHasPlayer = value;
            }
            get
            {
                return m_bHasPlayer;
            }
        }

        public int HeroId
        {
            set
            {
                m_nHeroId = value;
            }
            get
            {
                return m_nHeroId;
            }
        }

        public string PlayerName
        {
            set
            {
                this.PlayerNameText.text = value;
            }
            get
            {
                return this.PlayerNameText.text;
            }
        }

        public void ChangeItemSlotFrame(TeamSlotFrameType frameType)
        {
            Color clr;
            ColorUtility.TryParseHtmlString("#ffffff", out clr);
            if (frameType == TeamSlotFrameType.TEAMSLOTFRAMETYPE_SELF)
            {
                ColorUtility.TryParseHtmlString("#1697B7", out clr);
                //SelfFrame.SetActive(true);
                OtherFrame.SetActive(false);
            }
            else if (frameType == TeamSlotFrameType.TEAMSLOTFRAMETYPE_OTHER)
            {
                //SelfFrame.SetActive(false);
                OtherFrame.SetActive(true);
            }
            PlayerNameText.color = clr;
        }


        public void OnMachineAddButtonClick()
        {
            ComputerSelectComboObject.SetActive(true);
            ComputerDifficultyComboObject.SetActive(true);
            RemoveSlotButton.SetActive(true);

            // 给逻辑层发送添加假人信息，数据结构后续定义
            cmd_computerplayer_add data = new cmd_computerplayer_add();
            data.nPos = m_nSlotId;
            data.nCamp = m_nCamp;
            ViewEventHelper.Instance.SendCommand<cmd_computerplayer_add>(GameLogicDef.GVIEWCMD_MATCH_COMPUTERPLAYER_ADD, ref data);

            SetMachineInfo();
        }

        public void onRemoveMachineBtnClick()
        {
            // 给逻辑层发送移除加假人信息，数据结构后续定义
            cmd_computerplayer_remove data = new cmd_computerplayer_remove();
            data.nPos = m_nSlotId;
            data.nCamp = m_nCamp;
            ViewEventHelper.Instance.SendCommand<cmd_computerplayer_remove>(GameLogicDef.GVIEWCMD_MATCH_COMPUTERPLAYER_REMOVE, ref data);
        }

        //点击移除房间成员按钮
        public void onRemovePlayerBtnClick()
        {
            cmd_match_room_hero_exit data = new cmd_match_room_hero_exit();
            data.bIsOb = 0;
            data.nPos = m_nCamp * m_wndView.m_roomWnd.CampPlayerCount + m_nSlotId;

            ViewEventHelper.Instance.SendCommand<cmd_match_room_hero_exit>(GameLogicDef.GVIEWCM_MATCH_MASTER_KICK_ROLE, ref data);
        }

        public void OnRemoveButtonClick()
        {
            if (ComputerSelectComboObject.activeSelf)
            {
                onRemoveMachineBtnClick();
            }
            else
            {
                onRemovePlayerBtnClick();
            }
        }
        // 给逻辑层发送选择假人信息，数据结构后续定义
        public void onChangeMachineBtnClick(int index, ListViewItem component)// 下拉的位置
        {
            if (component == null)
            {
                Trace.Log("component == null");
                return;
            }

            ComboboxMachineListViewComponent item = (ComboboxMachineListViewComponent)component;
            if (item.Item.bLock)
            {
                return;
            }

            int nMachineId = item.Item.nHeroId;
            if (nMachineId == m_nCurComputerId)
            {
                return;
            }

            cmd_computerplayer_herochange data = new cmd_computerplayer_herochange();
            data.nPos = m_nSlotId;
            data.nCamp = m_nCamp;
            data.nHeroID = nMachineId;
            m_nCurComputerId = nMachineId;
            ViewEventHelper.Instance.SendCommand<cmd_computerplayer_herochange>(GameLogicDef.GVIEWCMD_MATCH_COMPUTERPLAYER_HEROCHANGE, ref data);

        }

        public void SetComputerHeadIconChange(int nHeroIconId, int nAIID, string szHeroName = "")
        {
            if (nHeroIconId == 0)
            {
                return;
            }

            if (szHeroName != "")
            {
                PlayerName = szHeroName;
            }
            
            m_bHasPlayer = true;
            m_nCurComputerId = nHeroIconId;
            SetChangeTeamItemHeroIcon(nHeroIconId, -1, -1);
            SetMachineInfo();
            SetMachineAIInfo(nHeroIconId, nAIID);

            if (m_bIsRoomOwner)
            {
                if (!dicHeroIdWithComboIndex.ContainsKey(nHeroIconId))
                {
                    Trace.Log("SetComputerHeadIconChange nHeroIconId=" + nHeroIconId);
                    return;
                }

                ComputerSelectCombo.SelectedIndex = dicHeroIdWithComboIndex[nHeroIconId];
            }
        }


        private void SetMachineAIInfo(int nHeroID, int nDifficultyAIID)
        {
            if (!m_bIsRoomOwner)
            {
                return;
            }

            ComputerDifficultyComboObject.SetActive(true);
            if (ComputerDifficultyCombo.DataSource.Count > 0)
            {
                ComputerDifficultyCombo.DataSource.Clear();
            }
            // 查找英雄AI配置
            SSchemeHeroAIConfig SchmeInfo = null;
            HeroAIConfig.Instance.GetHeroAIInfo(m_nWarId, nHeroID, ref SchmeInfo);
            //用户名数组下表,用于传递给AS 
            if (SchmeInfo == null)
            {
                return;
            }

            string szAIDescList = SchmeInfo.szAIDESArray;
            string[] AIDescArrayStr = szAIDescList.Split(';');
            int nAIDescArrayLength = AIDescArrayStr.Length;

            string szAIIDList = SchmeInfo.szAIIDArray;
            string[] AIIDArrayStr = szAIIDList.Split(';');
            int nAIIDArrayLength = AIIDArrayStr.Length;

            if (nAIDescArrayLength != nAIIDArrayLength)
            {
                Trace.LogError("配置出错 请检查 HeroAIConfig.csv heroID = " + nHeroID);
            }

            uint nHeroAINum = (uint)nAIDescArrayLength;

            ComputerDifficultyCombo.Start();
            int nIndex = 0;
            int nSelectIndex = 0;
            for (uint i = 0; i < nHeroAINum; i++)
            {
                // 防止配置出错
                int nHeroAIID = 0;
                if (!int.TryParse(AIIDArrayStr[i], out nHeroAIID))
                {
                    continue;
                }

                if (nHeroAIID <= 0)
                {
                    continue;
                }

                
                ComboboxMachineAIListViewData comBoxData = new ComboboxMachineAIListViewData();
                comBoxData.sMachineAIDesc = AIDescArrayStr[i];
                comBoxData.nAIID = nHeroAIID;
                comBoxData.nHeroID = SchmeInfo.nHeroId;
                comBoxData.nShowCombo = nAIDescArrayLength > 1 ? 1 : 0;
                ComputerDifficultyCombo.Add(comBoxData);

                // 有效的AIID
                if (nDifficultyAIID == nHeroAIID)
                {
                    m_nCurComputerAIId = nDifficultyAIID;
                    nSelectIndex = nIndex;
                }
                nIndex++;
            }

            ComputerDifficultyCombo.SelectedIndex = nSelectIndex;
        }

        // 给逻辑层发送选择假人信息，数据结构后续定义
        public void onChangeHeroAIBtnClick(int index, ListViewItem component)// 下拉的位置
        {
            if (component == null)
            {
                return;
            }

            ComboboxMachineAIListViewComponent item = (ComboboxMachineAIListViewComponent)component;
            int nMachineAIId = item.Item.nAIID;
            if (nMachineAIId == m_nCurComputerAIId)
            {
                return;
            }

            // 不同的AI选择 通知服务器校验
            cmd_computerplayer_heroaichange data = new cmd_computerplayer_heroaichange();
            data.nPos = m_nSlotId;
            data.nCamp = m_nCamp;
            data.nHeroID = item.Item.nHeroID;
            data.nAIID = nMachineAIId;
            m_nCurComputerAIId = nMachineAIId;
            ViewEventHelper.Instance.SendCommand<cmd_computerplayer_heroaichange>(GameLogicDef.GVIEWCMD_MATCH_COMPUTERPLAYER_HEROAI_CHANGE, ref data);
        }

        public void SetMachineInfo()
        {
            if (!m_bIsRoomOwner)
            {
                return;
            }

            ComputerSelectComboObject.SetActive(true);
            if (ComputerSelectCombo.DataSource.Count > 0)
            {
                ComputerSelectCombo.DataSource.Clear();
            }
         
            int nComBoIndex = 0;
            string strHeroName = "";
            ObservableList<ComboboxMachineListViewData> listMachine = new ObservableList<ComboboxMachineListViewData>();
            PersonSkinModelItem modelData = new PersonSkinModelItem();
            for (int i = (int)VOCATION.VOCATION_RANDOM; i < (int)VOCATION.VOCATION_MAX; i++)
            {

                // 查找英雄AI配置
                SSchemeHeroAIConfig SchmeInfo = null;
                HeroAIConfig.Instance.GetHeroAIInfo(m_nWarId, i, ref SchmeInfo);
                if (SchmeInfo == null)
                {
                    continue;
                }
                if (SchmeInfo.bIsOpen == false)
                {
                    continue;
                }

                strHeroName = "";
                if (i >= 0 && GameLogicAPI.GetPersonModel((UInt16)i, ref modelData))
                {
                    strHeroName = modelData.szHeroName;
                }

                ComboboxMachineListViewData comBoxData = new ComboboxMachineListViewData();
                comBoxData.sMachineDesc = strHeroName;
                comBoxData.bLock = false;
                comBoxData.nHeroId = i;
                listMachine.Add(comBoxData);
                dicHeroIdWithComboIndex[comBoxData.nHeroId] = nComBoIndex++;
            }
            ComputerSelectCombo.Start();
            ComputerSelectCombo.DataSource = listMachine;
        }

        public void SetTeamItemSummerInfo(int nSummerIdOne, int nSummerIdTwo)
        {
            if (IsDifferenceCampCanNotSee())
            {
                return;
            }

            SummerOneImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, nSummerIdOne);
            SummerOneImage.gameObject.SetActive(SummerOneImage.sprite == null ? false : true);
        }

        

        public void SetCampComputerCanUse(bool bSelect, int nHeroId)
        {
            int nComIndex = dicHeroIdWithComboIndex.ContainsKey(nHeroId) ? dicHeroIdWithComboIndex[nHeroId] : -1;
            if (nComIndex < 0 || nComIndex >= ComputerSelectCombo.DataSource.Count)
            {
                return;
            }

            ComputerSelectCombo.DataSource[nComIndex].bLock = !bSelect;
        }

        public void SetTeamItemClear()
        {
            if (ComputerSelectComboObject.activeSelf)
            {
                ResetData();
                MachineAddButton.gameObject.SetActive(true);
            }
            else
            {
                ResetData();
                MachineAddButton.gameObject.SetActive(m_bIsRoomOwner);
            }
        }

        public void SetTeamItemRoomOwner()
        {
            m_bOwner = true;
            OwnerFlag.SetActive(true);
            RemoveSlotButton.SetActive(false);
        }

        public void SetRoomTeamItemChangePosition(bool bIsSelf, bool bRoomOwner, string sPlayerName)
        {
            m_bIsRealPlayer = true;
            m_bIsSelf = bIsSelf;
            m_bOwner = bRoomOwner;
            PlayerName = sPlayerName;
            HeadMask.SetActive(false);
            MachineAddButton.gameObject.SetActive(false);
            m_bHasPlayer = true;
            RandomObject.SetActive(true);
            OwnerFlag.SetActive(m_bOwner);

            if (m_bIsRoomOwner)
            {
                RemoveSlotButton.SetActive(true);
            }

            if (m_bIsSelf)
            {
                ChangeItemSlotFrame(TeamSlotFrameType.TEAMSLOTFRAMETYPE_SELF);
                RemoveSlotButton.SetActive(false);
            }
        }

        public void OnRoleSlotMouseClick()
        {
            if (!CanChangeSlot || HasPlayer)
            {
                return;
            }

            //this.transform.parent.parent.parent.GetComponent<GameWaitingRoomView>().SendPlayerChangeTeamSlotMesseg(Camp, SlotId);
            cmd_match_room_ob_pos_change cmdChange;
            cmdChange.idActor = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            cmdChange.bSelfIsOBPos = m_wndView.ObPart.CheckActorIsOb(cmdChange.idActor) ? 1 : 0;
            cmdChange.bDesIsOBPos = 0;
            cmdChange.nPos = m_nCamp * m_wndView.m_roomWnd.CampPlayerCount + m_nSlotId;
            m_wndView.m_roomWnd.SendPlayerChangeTeamSlotMesseg(cmdChange);
        }

        public void SetHideTeamCustomModeButtons()
        {
            RemoveSlotButton.SetActive(false);
            MachineAddButton.gameObject.SetActive(false);
            ComputerSelectComboObject.SetActive(false);
            ComputerDifficultyComboObject.SetActive(false);
            PlayerNameText.gameObject.SetActive(true);
            heroStarItem.gameObject.SetActive(m_nCurComputerId > 0 ? true : false);
            HeadMask.gameObject.SetActive(m_nCurComputerId > 0 ? true : false);
        }

        public int GetRoomTeamItemComputerId()
        {
            return m_nCurComputerId;
        }

        public int GetTeamItemHeroId()
        {
            return m_nHeroId;
        }

        public int GetTeamCampPlayerCount()
        {
            if (m_bHasPlayer)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }

        public bool IsDifferenceCampCanNotSee()
        {
            if (m_bHideItemAllExceptMask || m_bHideItemAllView)
            {
                if (Camp != m_nSelfCamp)
                {
                    return true;
                }

            }

            return false;
        }
    }
    #endregion

    #region 底部功能(天赋符文召唤师开始按钮)part
    public class RoomBottomFunctionPart : MonoBehaviour
    {
        public RoomStartFunctionButton StartButton;
        public RoomBuyButton BuyButton;
        public ComboboxSampleListView RuneComboboxList;
        public ComboboxSampleListView TalentComboboxList;
        public RoomSummerFrame SummerFrame;
        public RoomXpSkillFrame XpSkillFrame;
        public Button ReturnButton;
        public Button InviteBtn;
        public GameObject AddComputerCombobox;
        public Transform ButtonFrame;

        public virtual void Start()
        {
            TalentComboboxList.OnSelect.RemoveListener(onTalentPageIndexChange);
            TalentComboboxList.OnSelect.AddListener(onTalentPageIndexChange);

            RuneComboboxList.OnSelect.RemoveListener(onRunePageIndexChange);
            RuneComboboxList.OnSelect.AddListener(onRunePageIndexChange);
            SummerFrame.SetWndView();
			InviteBtn.onClick.AddListener(OnClickInviteBtn);

            ButtonFrame = this.transform.FindChild("GameObject");
        }

        public void SetRuneAndTalentComBobox()
        {
            if (LogicDataCenter.crownPageDataManager == null || LogicDataCenter.crownPageDataManager.CrownPageDic.Count <= 0)
            {
                return;
            }

            RuneComboboxList.Clear();
            foreach (CrownPageItemData pageItem in LogicDataCenter.crownPageDataManager.CrownPageDic.Values)
            {
                ComboboxSampleListViewData comBoxData = new ComboboxSampleListViewData();
                comBoxData.ComboboxText = pageItem.reviseData.szName;
                RuneComboboxList.Add(comBoxData);
            }
            RuneComboboxList.SelectedIndex = 0;
        }

        public void SetOwnerBtnVisible(bool bVisible)
        {
            if (InviteBtn == null)
            {
                return;
            }
            InviteBtn.GetComponent<Button>().gameObject.SetActive(bVisible);
        }
        
        public void onAddComputerIndexChange(int nPageIndex, ListViewItem component)
        {

            //ComboboxSampleListViewComponent selectComponent = component as ComboboxSampleListViewComponent;
            
            //if (!dicComputerLevel.ContainsKey(selectComponent.ComboboxText.text))
            //{
            //    return;
            //}

            //int nLevelScore = 0;
            //int.TryParse(dicComputerLevel[selectComponent.ComboboxText.text], out nLevelScore);

            //ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_ROOM_MASTER_COVER_COMPUTER, nLevelScore);
        }

        public void onTalentPageIndexChange(int nPageIndex, ListViewItem component)
        {
            int nOldPageIndex = LogicDataCenter.talentPageDataManager.nCurPageSortIndex;
            int nCurPageIndex = nPageIndex + 1;
            if (nOldPageIndex == nCurPageIndex)
            {
                return;
            }

            LogicDataCenter.talentPageDataManager.SetCurPageIndex(nCurPageIndex);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TALENTPAGE_CHANGEPAGE, null);
        }

        public void onRunePageIndexChange(int nPageIndex, ListViewItem component)
        {
            LogicDataCenter.crownPageDataManager.RoomSelectPage(nPageIndex);

            SetGemPageToLocalConfig();
        }

       

        private void SetGemPageToLocalConfig()
        {
            int nHeroID = 0;
            if (SummerFrame.WndView == null || SummerFrame.WndView.TeamPart == null || SummerFrame.WndView.TeamPart.TeamFrameArray == null)
                return;

            foreach (RoomTeamFrame teamFrame in SummerFrame.WndView.TeamPart.TeamFrameArray)
            {
                if (teamFrame.TeamItemArray == null)
                    return;
                foreach (RoomTeamItem teamItem in teamFrame.TeamItemArray)
                {
                    if (teamItem.bIsSelf && teamItem.HeroId > 0)
                    {
                        nHeroID = teamItem.HeroId;
                        break;
                    }
                }
                if (nHeroID > 0)
                    break;
            }
            if (nHeroID <= 0)
                return;

            int nGemPageIndex;
            PlayerLocalHeroConfig.Instance.GetIntData(ELocalHero.ELH_GemPageID, nHeroID, out nGemPageIndex);

            if (nGemPageIndex != RuneComboboxList.SelectedIndex)
            {
                PlayerLocalHeroConfig.Instance.SetIntData(ELocalHero.ELH_GemPageID, nHeroID, RuneComboboxList.SelectedIndex);
            }
        }

        public void onTalentOpenButtonClick()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TALENTPAGE_ROOMOPENTALENT, null);
        }

        public void SetStartButtonToReadyState()
        {
            StartButton.SetStartButtonReadyState();
        }

        public void SetCustomStartButtonInitState(bool bRoomOwner)
        {
            StartButton.SetCustomStartButtonInitState(bRoomOwner);
        }

        public void SetOnlineLegendStartButtonInitState()
        {
            StartButton.SetStartButtonWaitForSystemState();
        }

        public void SetStartButtonToWaitForSelectState()
        {
            StartButton.SetStartButtonWaitForSelectState();
        }

        public void SetInitSelectSummonerSkill(int nIndex, int nIconId, int nSkillId, string sDesc)
        {
            SummerFrame.SetInitSelectSummonerSkill(nIndex, nIconId, nSkillId, sDesc);
        }

        public void SetAllSummerSkill(int nIndex, int nIconId, int nSkillId, bool bLock, string sDesc)
        {
            SummerFrame.SetAllSummerSkill(nIndex, nIconId, nSkillId, bLock, sDesc);
        }

        public void SetInitSelectXpSkill(int nIndex, int nIconId, int nSkillId, string sDesc, bool bCanUseHero)
        {
            XpSkillFrame.SetInitSelectXpSkill(nIndex, nIconId, nSkillId, sDesc, bCanUseHero);
        }

        public void SetAllXpSkill(int nIndex, int nIconId, int nSkillId, bool bLock, string sDesc,int nHeroID)
        {
            XpSkillFrame.SetAllXpSkill(nIndex, nIconId, nSkillId, bLock, sDesc,nHeroID);
        }

        public void UpdateTalentComBoxData()
        {
            foreach (TalentPageDataItem pageItem in LogicDataCenter.talentPageDataManager.talentPageDataManager)
            {
                ComboboxSampleListViewData comBoxData = new ComboboxSampleListViewData();
                comBoxData.ComboboxText = pageItem.sTempName;
                TalentComboboxList.Add(comBoxData);
            }

            TalentComboboxList.SelectedIndex = LogicDataCenter.talentPageDataManager.nCurPageSortIndex - 1;
        }

        public void SetGameRoomModel(EMHallRoomType roomType, bool bRoomOwner, bool bOnlineLegendCup)
        {
            if (roomType == EMHallRoomType.HallRoomType_Unknown)
            {
                return;
            }

            if (roomType == EMHallRoomType.HallRoomType_MacthMachine)
            {
                SetComputerStartButtonInitState();
            }
            else if (roomType == EMHallRoomType.HallRoomType_Macth)
            {
                SetMatchStartButtonInitState();
            }
            else if (roomType == EMHallRoomType.HallRoomType_Macth)
            {

            }
            else if (roomType == EMHallRoomType.HallRoomType_Custom)
            {
                SetCustomStartButtonInitState(bRoomOwner);
            }
            else if (roomType == EMHallRoomType.HallRoomType_LegendCup)
            {
                if (bOnlineLegendCup)
                {
                    SetOnlineLegendStartButtonInitState();
                }
                else
                {
                    SetCustomStartButtonInitState(bRoomOwner);
                }
            }
        }

        private void SetComputerStartButtonInitState()
        {
            SetStartButtonToWaitForSelectState();
        }

        private void SetMatchStartButtonInitState()
        {
            SetStartButtonToWaitForSelectState();
        }

        public void Update()
        {
            StartButton.gameObject.SetActive(!BuyButton.gameObject.activeSelf);
            BuyButton.gameObject.SetActive(!StartButton.gameObject.activeSelf);
        }

        public void ResetData()
        {
            SummerFrame.ResetData();
            XpSkillFrame.ResetData();
            StartButton.ResetData();
            BuyButton.ResetData();
        }

        public void CloseComboListView()
        {
            // 关闭符文下拉
            if (SummerFrame.listParent != null)
            {
                SummerFrame.Hide();
            }
            if (XpSkillFrame.listParent != null)
            {
                XpSkillFrame.Hide();
            }
        }

        public void SelectDefaultSummerSkill()
        {
            SummerFrame.SelectDefaultSummerSkill();
        }

        public void SelectDefaultXPSkill()
        {
            XpSkillFrame.SelectDefaultXpSkill();
        }
        
        public void SetReturnButtonInvalid()
        {
          if(ReturnButton == null)
          {
              return;
          }

          ReturnButton.interactable = false;
        }

        public void SetInviteButtonVisble(bool bVisble)
        {
            if (InviteBtn == null)
            {
                return;
            }

            InviteBtn.GetComponent<Button>().gameObject.SetActive(bVisble);
        }

        public void ClearXPSkillFrame()
        {
            XpSkillFrame.ResetData();
        }

		public void OnClickInviteBtn()
		{
			UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SNS_SHOW_INVITE_PLAYGAME_WND, null);
		}
    }

    public class RoomBuyButton : MonoBehaviour
    {
        enum EBuyType
        {
            EBT_INVALID,
            EBT_HERO,
            EBT_SKIN,

            EBT_MAX,
        }

        public GameWaitingRoomView WaitingRoomView;
        public Text BuyText;
        public Text BuyDesc;
        public UTileViewRoomHeroSkin RoomHeroListSkin;

        private EBuyType m_eBuyType;
        private UTileViewItemRoomHero m_curSelectHeroItem;
        private UTileViewItemRoomHeroSkin m_curSelectSkinItem;
 
        public void Awake()
        {
            RoomHeroListSkin.OnSelect.RemoveListener(OnSkinListViewItemClick);
            RoomHeroListSkin.OnSelect.AddListener(OnSkinListViewItemClick);
        }

        public void UpdateFrame()
        {
            if (WaitingRoomView == null || WaitingRoomView.m_roomWnd == null)
                return;
            if (!WaitingRoomView.m_roomWnd.IsThisStateInRoom(EMatchRoomState.MatchRoomState_Wait))
                return;

            bool bVisible = false;
            if (m_eBuyType == EBuyType.EBT_HERO && m_curSelectHeroItem != null)
            {
                bVisible = !m_curSelectHeroItem.bIsHaveHero && !m_curSelectHeroItem.bWeekFree;

                BuyText.text = ULocalizationService.Instance.Get("UIView", "Common", "ChargeUnlock");
                int nUnlockLevel = GameLogicAPI.getUnlockLevel(m_curSelectHeroItem.nHeroIconId);
                int nCurLevel = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
                BuyDesc.text = String.Format(ULocalizationService.Instance.Get("UIView", "Common", "UnlockHeroDesc"), nCurLevel, nUnlockLevel - nCurLevel);
                BuyDesc.gameObject.SetActive(nUnlockLevel > nCurLevel);
            }
            else if (m_eBuyType == EBuyType.EBT_SKIN && m_curSelectHeroItem != null && m_curSelectSkinItem != null)
            {
                bVisible = !m_curSelectSkinItem.bIsHaveSkin || (!m_curSelectHeroItem.bIsHaveHero && !m_curSelectHeroItem.bWeekFree);

                BuyText.text = ULocalizationService.Instance.Get("UIView", "Common", "ChargeUnlock");
                BuyDesc.gameObject.SetActive(false);
            }
            
            this.gameObject.SetActive(bVisible);
        }

        public void OnListViewItemClick(ListViewItem item)
        {
            if (item == null)
                return;

            if (m_eBuyType <= EBuyType.EBT_INVALID || m_eBuyType >= EBuyType.EBT_MAX)
                return;

            if (m_eBuyType == EBuyType.EBT_HERO)
                m_curSelectHeroItem = (item as UTileViewComponentRoomHero).GetData();
            else if (m_eBuyType == EBuyType.EBT_SKIN)
                m_curSelectSkinItem = (item as UTileViewComponentRoomHeroSkin).GetData();

            UpdateFrame();
        }

        public void OnSkinListViewItemClick(int nIndex, ListViewItem item)
        {
            m_eBuyType = EBuyType.EBT_SKIN;
            OnListViewItemClick(item);
        }

        public void OnBuyTypeSelect(int nBuyType)
        {
            m_eBuyType = (EBuyType)nBuyType;
            UpdateFrame();
        }

        public void ResetData()
        {
            this.gameObject.SetActive(false);
            m_eBuyType = EBuyType.EBT_INVALID;
            m_curSelectHeroItem = null;
            m_curSelectSkinItem = null;
        }

        public void OnBuyClick()
        {
            if (m_eBuyType == EBuyType.EBT_INVALID)
                return;

            if (m_eBuyType == EBuyType.EBT_HERO)
            {
                if (!m_curSelectHeroItem.bIsHaveHero)
                {
                    int nSellID = PointShopConfig.Instance.GetPointShopSellIDByHeroID(m_curSelectHeroItem.nHeroIconId);
                    if (nSellID > 0)
                    {
                        PointShopBuyGoodsInitData uiData = new PointShopBuyGoodsInitData();
                        uiData.nSellID = nSellID;
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_OPEN, uiData);
                    }
                }
            }
            else if (m_eBuyType == EBuyType.EBT_SKIN)
            {
                if (!m_curSelectHeroItem.bIsHaveHero)
                {
                    // TODO提示，未拥有英雄，请先解锁英雄
                    if (!m_curSelectHeroItem.bIsHaveHero)
                    {
                        int nSellID = PointShopConfig.Instance.GetPointShopSellIDByHeroID(m_curSelectHeroItem.nHeroIconId);
                        if (nSellID > 0)
                        {
                            PointShopBuyGoodsInitData uiData = new PointShopBuyGoodsInitData();
                            uiData.nSellID = nSellID;
                            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_OPEN, uiData);
                        }
                    }
                }
                else
                {
                    if (!m_curSelectSkinItem.bIsHaveSkin)
                    {
                        int nSellID = PointShopConfig.Instance.GetPointShopSellIDBySkinID(m_curSelectSkinItem.nHeroID, m_curSelectSkinItem.nSkinId);
                        if (nSellID > 0)
                        {
                            PointShopBuyGoodsInitData uiData = new PointShopBuyGoodsInitData();
                            uiData.nSellID = nSellID;
                            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_OPEN, uiData);
                        }
                        else
                        {
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_POINTSHOP_NOT_IN_SELL_LIST);
                        }
                    }
                }
            }
        }
    }

    public class RoomStartFunctionButton : Selectable, IPointerClickHandler
    {
        public enum StartButtonState
        {
            STARTBUTTONSTATE_NONE = 0,
            STARTBUTTONSTATE_BEGIN,
            STARTBUTTONSTATE_WAITFOROWNER,
            STARTBUTTONSTATE_WAITFORSELECT,
            STARTBUTTONSTATE_READY,
            STARTBUTTONSTATE_READYDISABLE,
        }
        public GameWaitingRoomView WaitingRoomView;
        public Text ButtonText;
        private StartButtonState m_ButtonState = StartButtonState.STARTBUTTONSTATE_NONE;
        private UEffectParamBase m_OldEffectParam;

        private bool bFPSTipWndShow = false;    // 防止FPS提示窗口多次弹出

        public void SetCustomStartButtonInitState(bool bRoomOwner)
        {
            if (bRoomOwner)
            {
                SetStartButtonBeginState();
            }
            else
            {
                SetStartButtonWaitForOwnerState();
            }
        }

        public void SetStartButtonBeginState()
        {
            this.interactable = true;
            m_ButtonState = StartButtonState.STARTBUTTONSTATE_BEGIN;
            base.DoStateTransition(SelectionState.Normal, false);
            ButtonText.text = "开始游戏";
        }

        public void SetStartButtonWaitForSystemState()
        {
            m_ButtonState = StartButtonState.STARTBUTTONSTATE_WAITFOROWNER;
            base.DoStateTransition(SelectionState.Disabled, false);
            ButtonText.text = "等待开始";
            this.interactable = false;
        }

        public void SetStartButtonWaitForOwnerState()
        {
            m_ButtonState = StartButtonState.STARTBUTTONSTATE_WAITFOROWNER;
            base.DoStateTransition(SelectionState.Disabled, false);
            ButtonText.text = "等候房主";
            this.interactable = false;
        }

        public void SetStartButtonWaitForSelectState()
        {
            m_ButtonState = StartButtonState.STARTBUTTONSTATE_WAITFORSELECT;
            base.DoStateTransition(SelectionState.Disabled, false);
            ButtonText.text = "选择英雄";
            this.interactable = false;
        }

        public void SetStartButtonReadyState()
        {
            this.interactable = true;
            m_ButtonState = StartButtonState.STARTBUTTONSTATE_READY;
            base.DoStateTransition(SelectionState.Normal, false);
            ButtonText.text = "准备开始";

            SetReadyButtonLightEffect();
        }

        public void SetStartButtonReadyDisableState()
        {
            m_ButtonState = StartButtonState.STARTBUTTONSTATE_READYDISABLE;
            base.DoStateTransition(SelectionState.Disabled, false);
            this.interactable = false;

            DestoryReadyButtonLightEffect();
        }

        public void OnStartButtonClick()
        {
            if (WaitingRoomView == null)
            {
                return;
            }

            switch (m_ButtonState)
            {
                case StartButtonState.STARTBUTTONSTATE_BEGIN:
                    {
                        WaitingRoomView.OnStartButtonBeginClick();
                    }
                    break;
                case StartButtonState.STARTBUTTONSTATE_READY:
                    {
                        if (WaitingRoomView.SelectHeroPart.RoomHeroList.SelectHeroItemBackupsData != null && WaitingRoomView.SelectHeroPart.RoomHeroList.SelectHeroItemBackupsData.bFpsHero)
                        {
                            if (bFPSTipWndShow)
                                return;
                            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                            title: ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "FpsConfirmUseTitle"),
                            message: ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "FpsConfirmUseContent"),
                            buttons: new UIWidgets.DialogActions() 
                                        { 
                                        { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), OnConfirmStartButtonReady }, 
                                        { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), OnCancelFPSTipWnd } 
                                        },
                            closeButtonAction: new UnityEngine.Events.UnityAction(OnCloseFPSTipWnd)
                            );
                            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
                            bFPSTipWndShow = true;
                        }
                        else
                        {
                            OnConfirmStartButtonReady();
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public bool OnConfirmStartButtonReady()
        {
            WaitingRoomView.OnStartButtonReadyClick();
            SetStartButtonReadyDisableState();

            bFPSTipWndShow = false;

            return true;
        }

        public bool OnCancelFPSTipWnd()
        {
            bFPSTipWndShow = false;

            return true;
        }

        public void OnCloseFPSTipWnd()
        {
            bFPSTipWndShow = false;
        }

        public void OnPointerClick(PointerEventData eventData)
        {
            if (eventData.button != PointerEventData.InputButton.Left)
                return;

            OnStartButtonClick();
        }

        public void ResetData()
        {
            base.DoStateTransition(SelectionState.Normal, false);
            DestoryReadyButtonLightEffect();

            bFPSTipWndShow = false;
        }

        public void SetReadyButtonLightEffect()
        {
            if (m_OldEffectParam != null)
            {
                return;
            }

            UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_UWaitingRoom_ReadyButtonLight, _tfAttachParent: this.transform);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);

            m_OldEffectParam = param;
            UEffectPrefabParam paramPrefab = (UEffectPrefabParam)param;
            paramPrefab.EffectGameObject.transform.localPosition = new Vector3(0, 0, -1000);
        }

        public void DestoryReadyButtonLightEffect()
        {
            if (m_OldEffectParam == null)
            {
                return;
            }

            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_OldEffectParam);
            m_OldEffectParam = null;
        }
    }

    public class RoomSummerButtonOutSummerFrame : MonoBehaviour
    {
        public Image SummerImage;
        public RoomSummerFrame SummerFrame;
        private int m_nSummerSkillId = 0;
        public int SummerSkillId { set { m_nSummerSkillId = value; } get { return m_nSummerSkillId; } }
        private int m_nSlotID = 0;
        public int SlotID { set { m_nSlotID = value; } get { return m_nSlotID; } }
        
        public void SetInitSelectSummonerSkill(int nIndex, int nIconId, int nSkillId, string sDesc)
        {
            m_nSummerSkillId = nSkillId;
            m_nSlotID = nIndex;
            SummerImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, nIconId);
            if (SummerImage.sprite == null)
            {
                SummerImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, 1);
            }
            SetToolTipDesc(sDesc);
        }

        public void OnSummerButtonOutClick()
        {
            SummerFrame.gameObject.SetActive(!SummerFrame.gameObject.activeSelf);
            SummerFrame.CurSelectIndex = m_nSlotID;
        }
        public void SetToolTipDesc(string sDesc)
        {
            UBB.toHtml(ref sDesc);
            UTooltipTrigger trigger = this.gameObject.GetComponent<UTooltipTrigger>();
            if (trigger == null)
            {
                trigger = this.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_SummerInRoom);
            }

            trigger.SetText(UTooltipParamName.BodyText, sDesc);
        }
    }

    public class RoomSummerButtonInSummerFrame : MonoBehaviour
    {
        public Image SummerImage;
        private int m_nSkilId = 0;
        public int SkillID { get { return m_nSkilId; } }
        public GameObject Mask;
        private RoomSummerFrame SummerFrame;
        private string m_sDesc = null;
        public void SetRoomSummerFrame(RoomSummerFrame summerFrame)
        {
            this.SummerFrame = summerFrame;
        }
        // 当召唤师技能改变之后回调
        public void  OnSummerButtonInClick()
        {
            SummerFrame.SetOutSummerChangeImage(SummerImage.sprite, m_sDesc);

            if (SummerFrame.gameObject.activeSelf)
            {
                SummerFrame.Hide();
            }
            cmd_SmSillInfo_Return cmdInfo = new cmd_SmSillInfo_Return();
            cmdInfo.nAllSoltSkillId = new int[2];

            cmdInfo.nAllSoltSkillId[0] = m_nSkilId;
            cmdInfo.nAllSoltSkillId[1] = 0;
            IntPtrHelper helper = new IntPtrHelper();
            IntPtr ptr = helper.toPtr<cmd_SmSillInfo_Return>(ref cmdInfo);
            int len = Marshal.SizeOf(cmdInfo);

            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_SUMMONER_BROAD_INFO, len, "", ptr, len);

            // 英雄选择的召唤师技能存储本地
            SetSummonToLocalConfig();
        }

        public void SetSummerItemSkill(int nIconId, int nSkillId, bool bLock, string sDesc)
        {
            m_sDesc = sDesc;
            SummerImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, nIconId);
            SummerImage.gameObject.SetActive(SummerImage.sprite == null ? false : true);
            m_nSkilId = nSkillId;
            Mask.SetActive(bLock);
            UBB.toHtml(ref sDesc);
            UTooltipTrigger trigger = this.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_SummerInRoom);
            trigger.SetText(UTooltipParamName.BodyText, sDesc);
        }

        private void SetSummonToLocalConfig()
        {
            int nHeroID = 0;
            if (SummerFrame.WndView == null || SummerFrame.WndView.TeamPart == null || SummerFrame.WndView.TeamPart.TeamFrameArray == null)
                return;
            
            foreach (RoomTeamFrame teamFrame in SummerFrame.WndView.TeamPart.TeamFrameArray)
            {
                if (teamFrame.TeamItemArray == null)
                    return;
                foreach (RoomTeamItem teamItem in teamFrame.TeamItemArray)
                {
                    if (teamItem.bIsSelf && teamItem.HeroId > 0)
                    {
                        nHeroID = teamItem.HeroId;
                        break;
                    }
                }
                if (nHeroID > 0)
                    break;
            }
            if (nHeroID <= 0)
                return;

            int nSkillID;
            //HeroSummonSettingLocalConfig.Instance.GetLocalData(nHeroID, out nSkillID);
            PlayerLocalHeroConfig.Instance.GetIntData(ELocalHero.ELH_SummonID, nHeroID, out nSkillID);
            if (nSkillID != m_nSkilId)
            {
                //HeroSummonSettingLocalConfig.Instance.SetLocalData(nHeroID, m_nSkilId);
                PlayerLocalHeroConfig.Instance.SetIntData(ELocalHero.ELH_SummonID, nHeroID, m_nSkilId);
            }
        }
    }

    public class RoomSummerFrame : MonoBehaviour
    {
        public RoomSummerButtonOutSummerFrame [] OutSummerButtonGroup;

        private List<RoomSummerButtonInSummerFrame> InSummerButtonGroup;
        public List<RoomSummerButtonInSummerFrame> InSummerButtonGroupArray { get { return InSummerButtonGroup; } }
        private int m_nCurSelectIndex = 0;
        public int CurSelectIndex { set { m_nCurSelectIndex = value; } get { return m_nCurSelectIndex; } }
        private GameWaitingRoomView m_wndView;
        public GameWaitingRoomView WndView { get { return m_wndView; } }

        public void SetAllSummerSkill(int nIndex, int nIconId, int nSkillId, bool bLock, string sDesc)
        {
            if(InSummerButtonGroup == null)
            {
                InSummerButtonGroup = new List<RoomSummerButtonInSummerFrame>();
            }

            ResNode SummerButtonPref = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, UPath.UPrefabsPath + "GameWaitingRoomView/" + "SummerInItem");
            if (SummerButtonPref == null)
            {
                Debug.Log("TeamItemPrefab Is Null");
                return;
            }

            GameObject SummerButtonObject = SummerButtonPref.InstanceMainRes();
            if (SummerButtonObject == null)
            {
                return;
            }

            RoomSummerButtonInSummerFrame SummerItem = SummerButtonObject.GetComponent<RoomSummerButtonInSummerFrame>();
            SummerItem.SetRoomSummerFrame(this);
            SummerItem.SetSummerItemSkill(nIconId, nSkillId, !bLock, sDesc);
            SummerButtonObject.transform.SetParent(this.transform, false);
            InSummerButtonGroup.Add(SummerItem);
        }

        public void SetInitSelectSummonerSkill(int nIndex, int nIconId, int nSkillId, string sDesc)
        {
            if (OutSummerButtonGroup == null)
            {
                return;
            }

            if(nIndex <= 0||nIndex>OutSummerButtonGroup.Length)
            {
                return;
            }

            OutSummerButtonGroup[nIndex-1].SetInitSelectSummonerSkill(nIndex, nIconId, nSkillId, sDesc);
        }

        public void ResetData()
        {
			int nChildCount = this.gameObject.transform.childCount;
            for (int j = 0; j < nChildCount; j++)
            {
                var itemChild = this.gameObject.transform.GetChild(0).gameObject;
                ResNode.DestroyRes(ref itemChild);
            }

            this.gameObject.SetActive(false);
            m_nCurSelectIndex = 0;
            InSummerButtonGroup = null;
        }

        public void SelectDefaultSummerSkill()
        {
            RoomSummerButtonOutSummerFrame outSummer = OutSummerButtonGroup[0];
            if (outSummer == null || InSummerButtonGroup == null)
                return;

            m_nCurSelectIndex = 1;

            if (outSummer.SummerSkillId > 0)
            {
                int nIndex = checkInSummerContainsOutSummer(outSummer.SummerSkillId);
                if (nIndex >= 0)
                {
                    InSummerButtonGroup[nIndex].OnSummerButtonInClick();
                    return;
                }
            }

            System.Random rd = new System.Random((int)(UnityEngine.Time.unscaledTime));
            int nRand = rd.Next() % InSummerButtonGroup.Count;
            InSummerButtonGroup[nRand].OnSummerButtonInClick();              
        }

        public int checkInSummerContainsOutSummer(int nSummorID)
        {
            if (InSummerButtonGroup == null)
                return -1;

            for (int i = 0; i < InSummerButtonGroup.Count; i++)
            {
                if (InSummerButtonGroup[i].SkillID == nSummorID)
                {
                    return i;
                }
            }

            return -1;
        }

        public void SetOutSummerChangeImage(Sprite spriteInSummer,string sDesc)
        {
            if (OutSummerButtonGroup == null)
            {
                return;
            }

            if (m_nCurSelectIndex <= 0 || m_nCurSelectIndex > OutSummerButtonGroup.Length)
            {
                return;
            }

            OutSummerButtonGroup[m_nCurSelectIndex - 1].SummerImage.sprite = spriteInSummer;
            OutSummerButtonGroup[m_nCurSelectIndex - 1].SetToolTipDesc(sDesc);
        }
        int ModalKey;
        public Transform listCanvas = null;
        public Transform listParent = null;

        protected void OnEnable()
        {
            if (listParent == null || listCanvas == null)
            {
                listParent = transform.parent;
                listCanvas = UISystem.Instance.GetWndLayerTrans(WndLayerDef.WND_LAYER_POPUP02);
            }
            ModalKey = ModalHelper.Open(this, null, new Color(0.15f, 0.15f, 0.15f, 0f), Hide, this.transform as RectTransform);
            if (listCanvas != null)
            {
                transform.SetParent(listCanvas);
            }
        }

        public void Hide()
        {
            ModalHelper.Close((int)ModalKey);
            gameObject.SetActive(false);
            transform.SetParent(listParent);
        }

        public void SetWndView()
        {
            m_wndView = transform.parent.parent.gameObject.GetComponent<GameWaitingRoomView>();
        }
    }


    public class RoomXpSkillButtonOutXpSkillFrame : MonoBehaviour
    {
        public Image XpSkillImage;
        public RoomXpSkillFrame XpSkillFrame;
        private bool m_bHasSkill = false;
        private int m_nSlotID = 0;
        private bool m_bCanUseHero = false;
        public void SetInitSelectSummonerSkill(int nIndex, int nIconId, int nSkillId, string sDesc, bool bCanUseHero)
        {
            XpSkillImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, nIconId);
            m_bHasSkill = nSkillId > 0;
            m_nSlotID = nIndex;
            m_bCanUseHero = bCanUseHero;
            if (XpSkillImage.sprite == null)
            {
                XpSkillImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, 1);
            }
            // XpSkillImage.gameObject.SetActive(XpSkillImage.sprite == null ? false : true);
            SetToolTipDesc(sDesc);
        }

        public void OnXpSkillButtonOutClick()
        {
            XpSkillFrame.gameObject.SetActive(!XpSkillFrame.gameObject.activeSelf);
            XpSkillFrame.CurSelectIndex = m_nSlotID;
        }

        public int SlotID
        {
            set { m_nSlotID = value; }
            get { return m_nSlotID; }
        }


        public bool HasXpSkillSkill
        {
            set { m_bHasSkill = value; }
            get { return m_bHasSkill; }
        }

        public void SetToolTipDesc(string sDesc)
        {
            UBB.toHtml(ref sDesc);
            UTooltipTrigger trigger = this.gameObject.GetComponent<UTooltipTrigger>();
            if (trigger == null)
            {
                trigger = this.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_SummerInRoom);
            }
            trigger.SetText(UTooltipParamName.BodyText, sDesc);

            trigger.enabled = !string.IsNullOrEmpty(sDesc);
        }

        public void ClearXpSkillButtonOutXp()
        {
            if(!m_bCanUseHero)
            {
                XpSkillImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, -1);
            }
        }

        public void ResetData()
        {
            XpSkillImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, -1);
            UTooltipTrigger trigger = this.gameObject.GetComponent<UTooltipTrigger>();
            if (trigger != null)
            {
                this.gameObject.RemoveComponent<UTooltipTrigger>();
            }
        }
    }

    public class RoomXpSkillButtonInXpSkillFrame : MonoBehaviour
    {
        public Image XpSkillImage;
        private int m_nSkilId = 0;
        private int m_nHeroId = 0;
        public GameObject Mask;
        private RoomXpSkillFrame XpSkillFrame;
        private string m_sDesc = null;
		private bool m_IsLock = false;
        public void SetRoomXpSkillFrame(RoomXpSkillFrame XpSkillFrame)
        {
            this.XpSkillFrame = XpSkillFrame;
        }
        // 当召唤师技能改变之后回调
        public void OnXpSkillButtonInClick()
        {
            if (XpSkillFrame.gameObject.activeSelf)
            {
                XpSkillFrame.Hide();
            }

			if(m_IsLock)
			{
				UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_WAITROOM_XP_UNLOCK);
				return;
			}

            XpSkillFrame.SetOutXpSkillChangeImage(XpSkillImage.sprite, m_sDesc);
            //this.gameObject.transform.transform.parent.gameObject.SetActive(false);


            cmd_Hero_XP_Skill_Info cmdInfo = new cmd_Hero_XP_Skill_Info();

            cmdInfo.nHeroID = m_nHeroId;
            cmdInfo.nXPSkillID = m_nSkilId;
            EntityEventHelper.Instance.SendCommand<cmd_Hero_XP_Skill_Info>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_XP_SKILL_OPERATE_SELECT, ref cmdInfo);
        }

        public void SetXpSkillItemSkill(int nIconId, int nSkillId, bool bLock, string sDesc, int nHeroID)
        {
            m_sDesc = sDesc;
            XpSkillImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, nIconId);
            XpSkillImage.gameObject.SetActive(XpSkillImage.sprite == null ? false : true);
            m_nSkilId = nSkillId;
            m_nHeroId = nHeroID;
			m_IsLock = bLock;
            Mask.SetActive(bLock);
            UBB.toHtml(ref sDesc);
            UTooltipTrigger trigger = this.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_SummerInRoom);
            trigger.SetText(UTooltipParamName.BodyText, sDesc);
        }
    }

    public class RoomXpSkillFrame : MonoBehaviour
    {
        private RoomXpSkillButtonInXpSkillFrame[] InXpSkillButtonGroup;
        public RoomXpSkillButtonOutXpSkillFrame[] OutXpSkillButtonGroup;
        private int m_nCurSelectIndex = 0;
        public void SetAllXpSkill(int nIndex, int nIconId, int nSkillId, bool bLock, string sDesc, int nHeroID)
        {
            if (InXpSkillButtonGroup == null)
            {
                InXpSkillButtonGroup = new RoomXpSkillButtonInXpSkillFrame[30];
            }

            ResNode XpSkillButtonPref = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,UPath.UPrefabsPath + "GameWaitingRoomView/" + "XpSkillInItem");
            if (XpSkillButtonPref == null)
            {
                Debug.Log("TeamItemPrefab Is Null");
                return;
            }

            GameObject XpSkillButtonObject = XpSkillButtonPref.InstanceMainRes();
            if (XpSkillButtonObject == null)
            {
                return;
            }

            RoomXpSkillButtonInXpSkillFrame XpSkillItem = XpSkillButtonObject.GetComponent<RoomXpSkillButtonInXpSkillFrame>();
            XpSkillItem.SetRoomXpSkillFrame(this);
            XpSkillItem.SetXpSkillItemSkill(nIconId, nSkillId, bLock, sDesc, nHeroID);
            XpSkillButtonObject.transform.SetParent(this.transform, false);
            InXpSkillButtonGroup[nIndex - 1] = XpSkillItem;

        }

        public void SetInitSelectXpSkill(int nIndex, int nIconId, int nSkillId, string sDesc, bool bCanUseHero)
        {
            if (OutXpSkillButtonGroup == null)
            {
                return;
            }

            if (nIndex <= 0 || nIndex > OutXpSkillButtonGroup.Length)
            {
                return;
            }

            OutXpSkillButtonGroup[nIndex - 1].SetInitSelectSummonerSkill(nIndex, nIconId, nSkillId, sDesc, bCanUseHero);
            if (nSkillId <= 0)
                SelectDefaultXpSkill();
        }

        public void ResetData()
        {
			int nChildCount = this.gameObject.transform.childCount;
            for (int j = 0; j < nChildCount; j++)
            {
                var itemChild = this.gameObject.transform.GetChild(0).gameObject;
                ResNode.DestroyRes(ref itemChild);
            }

            this.gameObject.SetActive(false);
            m_nCurSelectIndex = 0;
            InXpSkillButtonGroup = null;
            ResetXpSkillButtonOutXp();
        }

        public void SelectDefaultXpSkill()
        {
            if (InXpSkillButtonGroup == null)
            {
                return;
            }

            int nOutIndex = 0;
            foreach (RoomXpSkillButtonOutXpSkillFrame item in OutXpSkillButtonGroup)
            {
                nOutIndex++;

                if (item.HasXpSkillSkill)
                {
                    continue;
                }

                foreach (RoomXpSkillButtonInXpSkillFrame inItem in InXpSkillButtonGroup)
                {
                    if (inItem == null)
                        continue;

                    if (inItem.Mask.gameObject.activeSelf)
                        continue;

                    inItem.OnXpSkillButtonInClick();
                    CurSelectIndex = nOutIndex;
                    break;
                }
            }
        }

        public void SetOutXpSkillChangeImage(Sprite spriteInXpSkill, string sDesc)
        {
            m_nCurSelectIndex = 1;
            if (OutXpSkillButtonGroup == null)
            {
                return;
            }

            if (m_nCurSelectIndex <= 0 || m_nCurSelectIndex > OutXpSkillButtonGroup.Length)
            {
                return;
            }

            OutXpSkillButtonGroup[m_nCurSelectIndex - 1].XpSkillImage.sprite = spriteInXpSkill;
            OutXpSkillButtonGroup[m_nCurSelectIndex - 1].SetToolTipDesc(sDesc);
        }

        public int CurSelectIndex
        {
            set { m_nCurSelectIndex = value; }
            get { return m_nCurSelectIndex; }
        }

        public void ClearXpSkillButtonOutXp()
        {
            foreach (RoomXpSkillButtonOutXpSkillFrame item in OutXpSkillButtonGroup)
            {
                if (item)
                    item.ClearXpSkillButtonOutXp();
            }
        }

        public void ResetXpSkillButtonOutXp()
        {
            foreach (RoomXpSkillButtonOutXpSkillFrame item in OutXpSkillButtonGroup)
            {
                if (item)
                    item.ResetData();
            }
        }



        int nModelKey;
        public Transform listCanvas = null;
        public Transform listParent = null;

        protected void OnEnable()
        {
            if (listParent == null || listCanvas == null)
            {
                listParent = transform.parent;
                listCanvas = UISystem.Instance.GetWndLayerTrans(WndLayerDef.WND_LAYER_POPUP02);
            }
            nModelKey = ModalHelper.Open(this, null, new Color(0.15f, 0.15f, 0.15f, 0f), Hide, this.transform as RectTransform);
            if (listCanvas != null)
            {
                transform.SetParent(listCanvas);
            }
        }

        public void Hide()
        {
            ModalHelper.Close((int)nModelKey);
            gameObject.SetActive(false);
            transform.SetParent(listParent);
        }
        
    }

    #endregion

    #region 技能描述part
    public class RoomSkillTipPart : MonoBehaviour
    {
        public RoomSkillTipItem[] SkillTipArray;

        public void SetSkillInfo(IntPtr nSkillTextInfo, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(nSkillTextInfo, nPtrLen);
            cmd_match_room_hero_skill_desc_count SkillDescCountdata = helper.get<cmd_match_room_hero_skill_desc_count>();
            for (int i = 0; i < SkillDescCountdata.nSkillDescCount; ++i)
            {
                cmd_match_room_hero_skill_desc data = helper.get<cmd_match_room_hero_skill_desc>();
                if (data.nSpellIconID == 0 || data.heroSkillDes == null)
                    continue;

                UBB.toHtml(ref data.heroSkillDes, UBB_FORMAT_TYPE.UGUI);
                SkillTipArray[data.nSlotID].SetItemData(data.nSpellIconID, data.heroSkillDes);
            }
        }
    }

    public class RoomSkillTipItem : MonoBehaviour
    {
        public Image SkillIcon;
        public UTooltipTrigger SkillTip;

        public void SetItemData(int nSkillIconID, string strSkillDesc)
        {
            SkillIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, nSkillIconID);
            SkillTip.SetText(UTooltipParamName.BodyText, strSkillDesc);
        }
    }

    #endregion

    #region 顶部倒计时part
    public class RoomTopTimePart : TimeCountDown
    {
        public Text TimeCount;
        public Text TimeWord;
        public Text OffLineLegendCupDesc;

        public Text HeroNameText;
        public Text HeroTitleText;
        public Text HeroPlayWaysTipText;

        private void UpdateTimeText()
        {
            if ((int)fCurTime > 60)
            {
                int minutes = (int)(fCurTime / 60f);
                int seconds = (int)fCurTime % 60;

                string strMin = (minutes >= 10 ? minutes.ToString() : ("0" + minutes.ToString()));
                string strSec = (seconds >= 10 ? seconds.ToString() : ("0" + seconds.ToString()));
                TimeCount.text = strMin + ":" + strSec;
            }
            else
            {
                TimeCount.text = ((int)fCurTime).ToString();
            }
        }

        protected override void CountDownRepeatCallBack()
        {
            base.CountDownRepeatCallBack();

            UpdateTimeText();
        }

        protected override void CountDownCompleteCallBack()
        {
            base.CountDownCompleteCallBack();
            TimeCount.gameObject.SetActive(false);
            TimeWord.gameObject.SetActive(false);
        }

        public void SetHideTime()
        {
            base.CountDownCompleteCallBack();
            TimeCount.gameObject.SetActive(false);
            TimeWord.gameObject.SetActive(false);
            OffLineLegendCupDesc.gameObject.SetActive(false);
        }

        public void ResetData()
        {
            SetHideTime();
            SetHideHeroName();
        }

        public void SetHideHeroName()
        {
            if (HeroNameText == null )
                return;

            HeroNameText.gameObject.SetActive(false);
        }

        public void ChangeTopHeroName(int nHeroID, string sName, string sPlayWaysTip)
        {
            if (HeroNameText == null || HeroNameText.gameObject == null || sName == "")
            {
                return;
            }

            SSchemePersonModelData sPersonModel = null;
            if (PersonModelConfig.Instance.GetData_Out(nHeroID, out sPersonModel))
                HeroTitleText.text = sPersonModel.szHeroTitle;

            HeroNameText.gameObject.SetActive(true);
            HeroNameText.text = sName;
            if (sPlayWaysTip != "")
                HeroPlayWaysTipText.text = sPlayWaysTip;
            else
                HeroPlayWaysTipText.text = "";
        }

        public void SetLegendCupTimeCountDown(int nTime)
        {
            this.CountDownCompleteCallBack();
            this.StartCountDown((float)nTime, 1.0f, 1f);

            UpdateTimeText();
            // 设置杯赛提示
            TimeWord.text = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "TimeWordLegendCupWait");

            TimeCount.gameObject.SetActive(true);
            TimeWord.gameObject.SetActive(true);
            OffLineLegendCupDesc.gameObject.SetActive(false);
        }

        public void SetBanHeroTimeCountDown(int nTime)
        {
            this.CountDownCompleteCallBack();
            this.StartCountDown((float)nTime, 1.0f, 1f);

            UpdateTimeText();
            
            // 设置TimeWordInfo
            TimeWord.text = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "TimeWordPreSelectBanHero");

            TimeWord.gameObject.SetActive(true);
            TimeCount.gameObject.SetActive(true);
            OffLineLegendCupDesc.gameObject.SetActive(false);
        }

        public void SetShowEnterWarTime(int nTime)
        {
            this.CountDownCompleteCallBack();
            this.StartCountDown((float)nTime / 1000, 1.0f, 1f);
            UpdateTimeText();
            // 设置杯赛提示
            TimeWord.text = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "TimeWordEnterWar");

            TimeCount.gameObject.SetActive(true);
            TimeWord.gameObject.SetActive(true);
            OffLineLegendCupDesc.gameObject.SetActive(false);
        }

        public void SetShowSelectHeroTime(int nTime)
        {
            this.CountDownCompleteCallBack();
            this.StartCountDown((float)nTime / 1000, 1.0f, 1f);
            UpdateTimeText();
            TimeWord.text = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "TimeWordSelectHero");

            TimeCount.gameObject.SetActive(true);
            TimeWord.gameObject.SetActive(true);
            OffLineLegendCupDesc.gameObject.SetActive(false);
        }

        public void SetOfflineLegendCupDesc()
        {
            string strDesc = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "OffLineLegendCupDesc");
            UBB.toHtml(ref strDesc);
            OffLineLegendCupDesc.text = strDesc;

            TimeCount.gameObject.SetActive(false);
            TimeWord.gameObject.SetActive(false);
            OffLineLegendCupDesc.gameObject.SetActive(true);
        }
    }
    #endregion

    #region 阵营提示part
    public class RoomSelectBattleArrayTipPart : MonoBehaviour
    {
        public RoomSelectBattleArrayTipItem[] SelectBattleItemArray;

        public void ClearCampTeamSelectTip()
        {
            if (SelectBattleItemArray == null)
            {
                return;
            }

            for (int i = 0; i < SelectBattleItemArray.Length; ++i)
            {
                SelectBattleItemArray[i].ClearCampTeamSelectTip();
            }
        }

        public void SetSelectBattleArrayItemTipInfo(int nTipTypeIndex, string sTipDesc)
        {
            if (IsOutofSelectBattleItemArray(nTipTypeIndex))
            {
                return;
            }

            SelectBattleItemArray[nTipTypeIndex].SetSelectBattleArrayItemTipInfo(sTipDesc);
        }

        public bool IsOutofSelectBattleItemArray(int nTipTypeIndex)
        {
            if (SelectBattleItemArray == null)
            {
                return true;
            }

            if (nTipTypeIndex < 0 || nTipTypeIndex >= SelectBattleItemArray.Length)
            {
                return true;
            }
            else
            {
                return false;
            }
        }


        public void ResetData()
        {
            ClearCampTeamSelectTip();
        }
    }

    public class RoomSelectBattleArrayTipItem : MonoBehaviour
    {
        public Text TipWordText;

        public void SetSelectBattleArrayItemTipInfo(string sDesc)
        {
            TipWordText.text = sDesc;
            this.gameObject.SetActive(TipWordText.text == "" ? false : true);
        }

        public void ClearCampTeamSelectTip()
        {
            this.gameObject.SetActive(false);
            TipWordText.text = "";
        }
    }

    #endregion

    #region 人物模型缩放旋转
    public class PointRoomHeroPageDragHeroTool : MonoBehaviour, IBeginDragHandler, IDragHandler, IEndDragHandler,ITimerHanlder
    {
        private GameWaitingRoomView m_parent;
        private bool m_draging;
        private Vector3 m_lastPos;
        private bool m_bCanDrag;

        public void SetParent(GameWaitingRoomView parent)
        {
            m_parent = parent;
        }

        public void Update()
        {
            return;
            float fMouseScrollValue = InputManager.GetAxis(InputAxisSet.Axis_MouseScrollWheel);
            if (fMouseScrollValue != 0f)
            {
                m_parent.ScaleHeroModel(fMouseScrollValue * 0.2f);
            }
        }

        // IBeginDragHandler
        public virtual void OnBeginDrag(PointerEventData eventData)
        {
            if (m_bCanDrag && m_draging == false && eventData.button == PointerEventData.InputButton.Left)
            {
                m_lastPos = eventData.position;
                m_draging = true;
            }
        }

        // IDragHandler
        public virtual void OnDrag(PointerEventData eventData)
        {
            if (m_draging && m_parent != null)
            {
                float fDeltaX = eventData.position.x - m_lastPos.x;
                m_parent.RotateHeroModel(-fDeltaX * 0.4f);
                m_lastPos = eventData.position;
            }
        }

        // IEndDragHandler
        public virtual void OnEndDrag(PointerEventData eventData)
        {
            if (m_draging)
            {
                m_draging = false;
            }
        }

        public virtual void OnDisable()
        {
            TimerManager.KillTimer(this, 0);
            m_bCanDrag = false;
        }


        public void WaitDrag()
        {
            TimerManager.KillTimer(this, 0);
            TimerManager.SetTimer(this, 0, 5, 1);
            m_bCanDrag = false;
        }

        public void OnTimer(int nTimerID)
        {
            m_bCanDrag = true;
        }
    }
    #endregion

    #region 房间id part
    public class RoomIDPart : MonoBehaviour
    {
        public Text RoomIDText;
        public void SetRoomIDText(string sDesc)
        {
            if (RoomIDText == null)
            {
                return;
            }

            RoomIDText.text = sDesc;
        }
    }

    #endregion

    #region 玩家持有增益卡信息
    public class PlayerBenefitCardInfoPart : MonoBehaviour
    {
        public Image GoldBenefitCard;
        public Image ActorExpBenefitCard;
        public Image HeroExpBenefitCard;

        public void Awake()
        {
            addTooltip(ref GoldBenefitCard);
            addTooltip(ref ActorExpBenefitCard);
            addTooltip(ref HeroExpBenefitCard);
        }

        public void updateBenefitCardInfo()
        {
            if (GoldBenefitCard == null || ActorExpBenefitCard == null || HeroExpBenefitCard == null)
            {
                return;
            }

            GoldBenefitCard.gameObject.SetActive(false);
            ActorExpBenefitCard.gameObject.SetActive(false);
            HeroExpBenefitCard.gameObject.SetActive(false);

            Dictionary<int, string> remainList = LogicDataCenter.playerSystemDataManager.GetBenefitCardManager().getRemainDesc();
            foreach (var item in remainList)
            {
                if (item.Key == (int)BENEFITCARDTYPE.BENEFITCARDTYPE_GOLD)
                {
                    GoldBenefitCard.gameObject.SetActive(true);
                    GoldBenefitCard.gameObject.GetComponent<UTooltipTrigger>().SetText(UTooltipParamName.BodyText, item.Value);
                }
                else if (item.Key == (int)BENEFITCARDTYPE.BENEFITCARDTYPE_ACTOR_EXP)
                {
                    ActorExpBenefitCard.gameObject.SetActive(true);
                    ActorExpBenefitCard.gameObject.GetComponent<UTooltipTrigger>().SetText(UTooltipParamName.BodyText, item.Value);
                }
                else if (item.Key == (int)BENEFITCARDTYPE.BENEFITCARDTYPE_HERO_EXP)
                {
                    HeroExpBenefitCard.gameObject.SetActive(true);
                    HeroExpBenefitCard.gameObject.GetComponent<UTooltipTrigger>().SetText(UTooltipParamName.BodyText, item.Value);
                }
            }
        }

        private void addTooltip(ref Image ob)
        {
            UTooltipTrigger tooltipTrigger = ob.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
            tooltipTrigger.SetText(UTooltipParamName.BodyText, "");
            tooltipTrigger.backgroundTint = Color.cyan;
            tooltipTrigger.tipPosition = TipPosition.BottomLeftCorner;
        }
    }
    #endregion

    #region 选人 part
    public class RoomSelectHeroPart : MonoBehaviour
    {
        public UTileViewRoomHero RoomHeroList;
        public UTileViewRoomHeroSkin RoomHeroListSkin;
        public UButtonGroup ButtonSelectTypeGroup;
        public UButtonGroup ButtonHeroTypeGroup;
        public UButtonGroup ButtonSkinTypeGroup;
        public GameObject SkinTypeObj;
        public UButtonGroupItem HeroTypeItem;
        
        private bool m_bUpdateSkin = false;
        private GameWaitingRoomView m_wndView;

        public void InitSelectHeroPart(GameWaitingRoomView view)
        {
            if (ButtonSelectTypeGroup == null || ButtonHeroTypeGroup == null || ButtonSkinTypeGroup == null || SkinTypeObj == null || HeroTypeItem == null)
            {
                return;
            }

            m_wndView = view;
            m_bUpdateSkin = false;
            ButtonSelectTypeGroup.SetAllItemDeSelected();
            ButtonSelectTypeGroup.SetSelectedItem(HeroTypeItem, true);
            ButtonHeroTypeGroup.SetAllItemDeSelected();
            ButtonHeroTypeGroup.SetItemSelectedItem(0, true);
        }

        public void DoSkinGroupItemShow()
        {
            if (SkinTypeObj == null)
                return;

            SkinTypeObj.SetActive(true);
            m_bUpdateSkin = true;
        }

        public void SortSelectData()
        {
            if (RoomHeroList == null)
                return;

            RoomHeroList.SortSelectData();
        }

        public void AddRoomSelectHeroItemData(List<cmd_match_room_hero_slot> heroList)
        {
            RoomHeroList.DataSource.Clear();
            RoomHeroList.DataSource.BeginUpdate();
            foreach (var data in heroList)
            {
                SSchemeHeroTypeConfig config = HeroTypeConfig.Instance.GetHeroTypeConfig(data.nHeroID);
                if ((data.nHeroID > 0 && config == null) || RoomHeroList == null)
                {
                    return;
                }

                bool bIsLock = Convert.ToBoolean(data.bIsLock);
                bool bIsHave = Convert.ToBoolean(data.bCurrentIsHave);
                bool bIsWeekFree = Convert.ToBoolean(data.bIsWeekFree);

                UTileViewItemRoomHero heroItemData = new UTileViewItemRoomHero();
                heroItemData.bIsLock = data.bIsWeekFree <= 0 && data.bCurrentIsHave <= 0;
                heroItemData.nSlotId = data.nSlotIndex;
                heroItemData.nHeroIconId = data.nHeroID;
                heroItemData.bIsHaveHero = bIsHave;
                heroItemData.sHeroDesc = data.szHeroTooltip;
                heroItemData.bSelect = false;
                heroItemData.nHeroType = data.nHeroType;
                heroItemData.bIsLockByHero = false;
                heroItemData.bIsLockByComputer = false;
                heroItemData.nPractisedLevel = data.nPractisedLevel;
                heroItemData.sHeroName = data.szHeroName;
                heroItemData.eRoomState = EMatchRoomState.MatchRoomState_Wait;
                heroItemData.bWeekFree = bIsWeekFree;
                heroItemData.nSortIndex = config == null ? data.nSlotIndex : config.nSortIndex;
                heroItemData.nSkinIDList = data.nSkinIDList;
                heroItemData.sPlayWaysTip = config == null ? "" : config.strPlayWay;
                heroItemData.nUnlockLevel = GameLogicAPI.getUnlockLevel(data.nHeroID);
                heroItemData.bFpsHero = GameLogicAPI.checkIsFpsHero(data.nHeroID);
                heroItemData.nUnlockTime = (int)data.dwUnlockTime;
                heroItemData.nIsNew = (data.nHeroID > 0 && bIsHave && data.nMatchNum < 1) ? 1 : 0;
                RoomHeroList.SelectHeroData.Add(heroItemData);
            }
            RoomHeroList.DataSource.EndUpdate();
        }

        public void OnSkinTypeGroupButtonClick(bool bIsOn)
        {
            if (!bIsOn || !m_bUpdateSkin || RoomHeroList == null  || RoomHeroListSkin == null)
            {
                return;
            }

            UTileViewItemRoomHero heroData = new UTileViewItemRoomHero();
            if (RoomHeroList.SelectHeroItem && RoomHeroList.SelectHeroItem.GetData().nHeroIconId > 0)
                heroData = RoomHeroList.SelectHeroItem.GetData();
            else if (RoomHeroList.SelectHeroItemBackupsData!=null)
                heroData = RoomHeroList.SelectHeroItemBackupsData;

            if (heroData == null || heroData.nHeroIconId <= 0)
            {
                return;
            }
           
            int[] skinIDList = heroData.nSkinIDList;
            if (skinIDList != null)
            {
                RoomHeroListSkin.ClearSelectState();
                RoomHeroListSkin.ResetData();
                int nDeSkinID = 0;
                if (!PlayerLocalHeroConfig.Instance.GetIntData(ELocalHero.ELH_DeSkinID, heroData.nHeroIconId, out nDeSkinID))
                    nDeSkinID = -1;

                int nDeSelectIndex = -1;
                int nNormalSelectIndex = 0;
                for (int j = 0; j < skinIDList.Length; ++j)
                {
                    int nSkinID = skinIDList[j];

                    SkinInfoScheme skinConfig = new SkinInfoScheme();
                    if (!GameLogicAPI.GetHeroSkinInfo(nSkinID, ref skinConfig))
                    {
                        continue;
                    }

                    UTileViewItemRoomHeroSkin skinItemData = new UTileViewItemRoomHeroSkin();
                    cmd_Hero_Skin_Info skinCmd = new cmd_Hero_Skin_Info();
                    if (LogicDataCenter.heroSkinDataManager.GetHeroSkinInfo(heroData.nHeroIconId, nSkinID, ref skinCmd)) 
                    {
                        skinItemData.bIsHaveSkin = true;
                        skinItemData.dwSkinLimitTime = skinCmd.dwDurationTime;
                    }
                    else
                    {
                        skinItemData.bIsHaveSkin = skinConfig.nSkinLevel == (int)SKIN_LEVEL_TYPE.ESLT_NORMAL ? true : false;
                        skinItemData.dwSkinLimitTime = 0;
                        if (skinConfig.nSkinLevel == (int)SKIN_LEVEL_TYPE.ESLT_NORMAL)
                        nNormalSelectIndex = j;
                    }

                    if (nDeSkinID == nSkinID && skinItemData.bIsHaveSkin)
                        nDeSelectIndex = j;

                    skinItemData.nHeroID = heroData.nHeroIconId;
                    skinItemData.bSelect = false;
                    skinItemData.nSkinId = nSkinID;
                    skinItemData.nSkinType = skinConfig.nSkinLevel;
                    skinItemData.nSkinIconId = skinConfig.nSkinIconID;
                    skinItemData.szSkinName = skinConfig.szSkinName;
                    skinItemData.eRoomState = m_wndView.m_roomWnd.RoomState;

                    RoomHeroListSkin.SelectHeroData.Add(skinItemData);
                }

                ButtonSkinTypeGroup.SetAllItemDeSelected();
                ButtonSkinTypeGroup.SetItemSelectedItem(0, true);

                if (nDeSelectIndex == -1)
                    nDeSelectIndex = nNormalSelectIndex;

                if (nDeSelectIndex >= 0 && nDeSelectIndex < RoomHeroListSkin.SelectHeroData.Count)
                {
                    RoomHeroListSkin.SelectHeroData[nDeSelectIndex].bSelect = false;
                    RoomHeroListSkin.SelectHeroData[nDeSelectIndex].bSelect = true;
                    RoomHeroListSkin.Select(nDeSelectIndex);
                    UTileViewItemRoomHeroSkin data = RoomHeroListSkin.SelectHeroData[nDeSelectIndex];
                    this.transform.parent.GetComponent<GameWaitingRoomView>().CacheSelectSkinInfo(data.nHeroID, data.nSkinId);
                }

                RoomHeroListSkin.SortSelectData();
                m_bUpdateSkin = false;
                RoomHeroListSkin.UpdateItem();
            }
        }

        public void UpdateRoomSkinList(int nHeroID,int m_nUpdateSkinID)
        {
            RoomHeroListSkin.UpdateRoomSkinList(nHeroID,m_nUpdateSkinID);

            PlayerLocalHeroConfig.Instance.SetIntData(ELocalHero.ELH_DeSkinID, nHeroID, m_nUpdateSkinID);
        }

        public bool UpdateRoomHeroList(int m_nUpdateHeroID)
        {
           return RoomHeroList.UpdateRoomHeroList(m_nUpdateHeroID);
        }
    }

    #endregion

    #region Ban选 part
    public class RoomBanHeroItem : MonoBehaviour
    {
        public Image HeroIcon;
        public Image BanFrame;

        private int m_nHeroId;
        public int HeroId { get { return m_nHeroId; } }

        public void SetData(GameWaitingRoomView view, int nHeroId)
        {
            if (nHeroId <= 0)
                return;

            m_nHeroId = nHeroId;

            HeroIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, 6, nHeroId);
            this.gameObject.SetActive(true);
            BanFrame.gameObject.SetActive(view.m_roomWnd.RoomState == EMatchRoomState.MatchRoomState_SelectHero);
        }
    }

    public class RoomBanHeroPart : UCustomUIBehaviour
    {
        public RectTransform[] CampBanHeroFrames;
        public GameObject BanHeroTemplate;
        public GameWaitingRoomView m_wndView;

        private const int CampBanHeroCount = 5;
        private RoomBanHeroItem[,] CampBanHeroItems;
        private List<int> m_nBanHeroIdList;
        public List<int> BanHeroIdList { get { return m_nBanHeroIdList; } }

        protected override void Awake()
        {
            base.Awake();

            m_nBanHeroIdList = new List<int>();
            InitFrame();
        }

        private void InitFrame()
        {
            if (CampBanHeroFrames == null || CampBanHeroFrames.Length <= 0)
                return;

            CampBanHeroItems = new RoomBanHeroItem[CampBanHeroFrames.Length, CampBanHeroCount];
            for (int j = 0; j < CampBanHeroFrames.Length; j++)
            {
                int nIndex = j;
                RectTransform rect = CampBanHeroFrames[j];

                int nCount = rect.childCount;
                for (int i = nCount - 1; i >= 0; --i)
                {
                    Transform childTrans = rect.GetChild(i);
                    if (childTrans)
                    {
                        var go = childTrans.gameObject;
                        ResNode.DestroyRes(ref go);
                    }
                }

                for (int i = 0; i < CampBanHeroCount; ++i)
                {
                    GameObject banHeroItemObj = ResNode.InstantiateRes(BanHeroTemplate);
                    if (!banHeroItemObj)
                        return;
                    RectTransform rectTrans = banHeroItemObj.transform as RectTransform;
                    rectTrans.SetParent(rect, false);

                    RoomBanHeroItem banHeroItem = banHeroItemObj.GetComponent<RoomBanHeroItem>();
                    if (!banHeroItem)
                        return;
                    CampBanHeroItems[nIndex, i] = banHeroItem;
                }
            }
        }

        public void RecvSingleBanHero(int nCampMaxPlayerCount, List<cmd_room_banInfo_sub> banHeroList)
        {
            foreach (var node in banHeroList)
            {
                int nCamp = node.nPos / nCampMaxPlayerCount;
                int nIndex = node.nPos % nCampMaxPlayerCount;
                if (CampBanHeroItems[nCamp, nIndex])
                    CampBanHeroItems[nCamp, nIndex].SetData(m_wndView, node.nHeroID);


                if (m_wndView.TeamPart.TeamFrameArray.Length > nCamp &&
                    m_wndView.TeamPart.TeamFrameArray[nCamp].TeamItemArray.Length > nIndex &&
                    m_wndView.TeamPart.TeamFrameArray[nCamp].TeamItemArray[nIndex].bIsSelf)
                {
                    m_wndView.banHeroView.gameObject.SetActive(false);
                }
            }

            m_nBanHeroIdList.Clear();
            for (int i = 0; i < CampBanHeroFrames.Length; i++)
            {
                for (int j = 0; j < CampBanHeroCount; j++)
                {
                    if (CampBanHeroItems[i, j].HeroId > 0)
                    {
                        m_nBanHeroIdList.Add(CampBanHeroItems[i, j].HeroId);
                    }
                }
            }
        }

        public void RecvAllBanHero(int nCampMaxPlayerCount, List<cmd_room_banInfo_sub> banHeroList)
        {
            InitFrame();
            RecvSingleBanHero(nCampMaxPlayerCount, banHeroList);
        }

        public void ResetData()
        {
            InitFrame();
        }
    }

    public class RoomBanHeroView : UCustomUIBehaviour
    {
        public Text TitleDesc;
        public Text TipDesc;
        public Text ConfirmBtnDesc;
        public Text ReturnBtnDesc;
        public Button ConfirmBtn;

        public UTileRoomBanHero RoomBanHeroList;
        private int m_CurSelectIndex;
        private UTileItemRoomBanHero m_CurSelectItem;
        private UTileComponentRoomBanHero m_CurSelectComponent;

        protected override void Awake()
        {
            base.Awake();

            TitleDesc.text = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "TitleDesc");
            TipDesc.text = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "TipDesc");
            ConfirmBtnDesc.text = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "ConfirmBtnDesc");
            ReturnBtnDesc.text = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "ReturnBtnDesc");
        }

        public void OnRecvHeroList(List<cmd_match_room_hero_slot> heroList)
        {
            RoomBanHeroList.OnDeselect.RemoveListener(OnDeSelectItem);
            RoomBanHeroList.OnSelect.RemoveListener(OnSelectItem);
            RoomBanHeroList.DataSource.Clear();
            ObservableList<UTileItemRoomBanHero> dataSource = new ObservableList<UTileItemRoomBanHero>();
            foreach (cmd_match_room_hero_slot node in heroList)
            {
                UTileItemRoomBanHero item = new UTileItemRoomBanHero();
                if (node.nHeroID <= 0)
                    continue;

                item.nHeroID = node.nHeroID;
                item.strHeroName = node.szHeroName;
                item.bIsBan = false;
                dataSource.Add(item);
            }
            RoomBanHeroList.DataSource = dataSource;
            RoomBanHeroList.OnSelect.AddListener(OnSelectItem);
            RoomBanHeroList.OnDeselect.AddListener(OnDeSelectItem);
        }

        public void RecvSingleBanHero(List<int> banHeroList)
        {
            for (int i = 0; i < RoomBanHeroList.DataSource.Count; i++)
            {
                RoomBanHeroList.DataSource[i].bIsBan = false;
                RoomBanHeroList.DataSource[i].nBanCount = 0;
            }

            foreach (var item in banHeroList)
            {
                for (int i = 0; i < RoomBanHeroList.DataSource.Count; i++)
                {
                    if (RoomBanHeroList.DataSource[i].nHeroID == item)
                    {
                        RoomBanHeroList.DataSource[i].bIsBan = true;
                        RoomBanHeroList.DataSource[i].nBanCount++;
                    }
                }
            }

            RoomBanHeroList.UpdateItems();
        }

        public void OnSelectItem(int index, ListViewItem item)
        {
            if (RoomBanHeroList == null)
                return;

            m_CurSelectIndex = index;
            m_CurSelectItem = RoomBanHeroList.DataSource[index];
            m_CurSelectComponent = item as UTileComponentRoomBanHero;
            m_CurSelectComponent.SelectColoring();
        }

        public void OnDeSelectItem(int index, ListViewItem item)
        {
            if (RoomBanHeroList == null)
                return;

            UTileComponentRoomBanHero deSelectComponent = item as UTileComponentRoomBanHero;
            deSelectComponent.DefaultColoring();
        }

        public void OnClickConfirmBtn()
        {
            if (m_CurSelectItem == null)
                return;

            cmd_ban_hero banHero = new cmd_ban_hero();
            banHero.nHeroID = m_CurSelectItem.nHeroID;
            ViewEventHelper.Instance.SendCommand<cmd_ban_hero>(GameLogicDef.GVIEWCMD_MATCH_ROOM_BAN_HERO, ref banHero); 
        }

        public void OnClickReturnBtn()
        {
            OnClickCloseBtn();
        }

        public void OnClickCloseBtn()
        {
            this.gameObject.SetActive(false);
        }

        public void ResetData()
        {
            this.gameObject.SetActive(false);
            ConfirmBtn.interactable = true;
        }
    }

    #endregion

    #region Ob part
    public class RoomObPart : UCustomUIBehaviour
    {
        public Button ViewObBtn;
        public Button AddObBtn;
        public Text ViewObBtnDesc;
        public UTileRoomOb URoomObList;
        public GameObject ObListGo;
        public GameWaitingRoomView m_wndView;

        private int m_nObMaxCount = 0;
        private Dictionary<int, cmd_match_room_ob_data> m_dicRoomOb;    // pos->ob
        private bool m_bIsRoomMaster = false;

        protected override void Awake()
        {
            m_dicRoomOb = new Dictionary<int, cmd_match_room_ob_data>();
            ObListGo.SetActive(false);
        }

        public void OnMsgInitOb(bool bPermitOb, int nObMaxCount)
        {
            if (bPermitOb == false)
            {
                this.gameObject.SetActive(false);
                return;
            }

            this.gameObject.SetActive(true);
            m_nObMaxCount = nObMaxCount;
            UpdateRoomObCount();
        }

        public void OnMsgAddOb(cmd_match_room_ob_data data)
        {
            if (m_wndView != null && m_wndView.m_roomWnd != null)
            {
                m_bIsRoomMaster = m_wndView.m_roomWnd.LocalRoomOwner;
            }
            m_dicRoomOb[data.nObPos] = data;

            UpdateRoomObCount();
            UpdateRoomObView();
        }

        public void OnMsgDeleteOb(int nPos)
        {
            if (m_wndView != null && m_wndView.m_roomWnd != null)
            {
                m_bIsRoomMaster = m_wndView.m_roomWnd.LocalRoomOwner;
            }
            m_dicRoomOb.Remove(nPos);

            UpdateRoomObCount();
            UpdateRoomObView();
        }

        private void UpdateRoomObCount()
        {
            string str = string.Format("<color=#48b7fe>观战席（<color=#ffffff>{0}</color>/{1}）</color>", m_dicRoomOb.Count, m_nObMaxCount);
            UBB.toHtml(ref str);
            ViewObBtnDesc.text = str;
        }

        private void UpdateRoomObView()
        {
            URoomObList.DataSource.Clear();
            ObservableList<UTileItemRoomOb> dataSource = new ObservableList<UTileItemRoomOb>();
            foreach (var node in m_dicRoomOb)
            {
                UTileItemRoomOb item = new UTileItemRoomOb();
                item.wndView = m_wndView;
                item.bIsRoomMaster = m_bIsRoomMaster;
                item.data = node.Value;
                dataSource.Add(item);
            }
            URoomObList.DataSource = dataSource;
        }

        public void OnClickViewObBtn()
        {
            ObListGo.SetActive(!ObListGo.activeSelf);
            if (ObListGo.activeSelf)
            {
                Show();
            }

            //if (m_bIsRoomMaster == m_wndView.m_roomWnd.LocalRoomOwner)
            //    return;

            m_bIsRoomMaster = m_wndView.m_roomWnd.LocalRoomOwner;
            UpdateRoomObView();
        }

        public void OnClickAddObBtn()
        {
            int nEmptyPos = -1;
            if (!FindObEmptyPosForEnter(ref nEmptyPos))
            {
                return;
            }

            cmd_match_room_ob_pos_change cmdChange;
            cmdChange.idActor = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            cmdChange.bSelfIsOBPos = 0;
            cmdChange.bDesIsOBPos = 1;
            cmdChange.nPos = nEmptyPos;
            m_wndView.m_roomWnd.SendPlayerChangeTeamSlotMesseg(cmdChange);
        }

        public bool FindObEmptyPosForEnter(ref int nPos)
        {
            if (m_dicRoomOb.Count >= m_nObMaxCount)
	        {
                nPos = -1;
		        return false;
	        }

            for (int i = 0; i < m_nObMaxCount; ++i)
			{
			    if (!m_dicRoomOb.ContainsKey(i))
	            {
		            nPos = i;
                    return true;
	            }
			}

            return false;
        }

        public bool CheckActorIsOb(int nActorId)
        {
            foreach (var item in m_dicRoomOb)
            {
                if (item.Value.nActorID == nActorId)
                {
                    return true;
                }
            }

            return false;
        }

        public void ResetData()
        {
            m_dicRoomOb.Clear();
        }

        int nModelKey;
        private Transform listCanvas = null;
        private Transform listParent = null;

        public void Show()
        {
            if (listParent == null || listCanvas == null)
            {
                listParent = ObListGo.transform.parent;
                listCanvas = UISystem.Instance.GetWndLayerTrans(WndLayerDef.WND_LAYER_POPUP02);
            }
            nModelKey = ModalHelper.Open(this, null, new Color(0.15f, 0.15f, 0.15f, 0f), Hide, ObListGo.transform as RectTransform);
            if (listCanvas != null)
            {
                ObListGo.transform.SetParent(listCanvas);
            }
        }

        public void Hide()
        {
            ModalHelper.Close((int)nModelKey);
            ObListGo.SetActive(false);
            ObListGo.transform.SetParent(listParent);
        }
    }

    #endregion

    #region 主控制界面GameWaitingRoomView
    public class GameWaitingRoomView : UIBaseWndView
    {
        public RoomTeamPart TeamPart;
        public RoomBottomFunctionPart BottomFunctionPart;
        public RoomTopTimePart TopTimePart;
        public RoomSelectBattleArrayTipPart SelectBattleTipPart;
        public RoomSkillTipPart SkillTipPart;
        public RoomIDPart IDPart;
        public PlayerBenefitCardInfoPart BenefitCardInfoPart;    // 增益卡信息栏
        public RoomSelectHeroPart SelectHeroPart;
        public RoomObPart ObPart;

        public Transform MeshRenderNode;
        public GameObject warningPanel;
        public Text PromptText;
        public Button confirmButton;
        public Text confirmText;
        public Text cancleText;
        public PointRoomHeroPageDragHeroTool HeroTool;

        public Button banHeroBtn;
        public RoomBanHeroPart banHeroPart;
        public RoomBanHeroView banHeroView;

        public GameWaitingRoomWnd m_roomWnd;
        private SceneEffectParamBase m_LastSkinEffect;
        private int m_nLastSkinId;
        private bool m_bAllowMachineLock = false;

        private int m_nSkinSoundID = 0;

        private Transform m_SelectHeroTransform;
        public Transform SelectTransForm { set { m_SelectHeroTransform = value; } get { return m_SelectHeroTransform; } }


        #region 显示动画部分，目前先在OnEnable执行时播放，界面显示跟场景混一起有点乱
        public UViewAnim AnimWaitingRoomView;
        public void EnterViewAnim()
        {
            if (TeamPart != null && TeamPart.AnimTeamFrameItem != null)
                TeamPart.AnimTeamFrameItem.PlayForward();
            if (AnimWaitingRoomView == null)
                AnimWaitingRoomView = GetComponent<UViewAnim>();
            if (AnimWaitingRoomView != null)
                AnimWaitingRoomView.PlayForward();
        }
        protected void OnEnable()
        {
            EnterViewAnim();
        }
        #endregion


        public override void Start()
        {
            base.Start();

            if (MeshRenderNode == null)
                MeshRenderNode = GetTransform().FindChild("MeshRenderNode");

            m_roomWnd = (GameWaitingRoomWnd)m_wnd;

            HeroTool.SetParent(this);
            
            confirmText.text = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "PanelConfirmDes");
            cancleText.text = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "PanelCancelDes");
        }

        public void SetRoleEnterRoom(bool bSelf, bool bRoomOwner, int nCamp, int nHeroIconId, int nTeamSlotId, string sPlayerName, int nSelfCamp, bool bIsComputerPlayer = false)
        {
            if (TeamPart == null)
                return;

            TeamPart.SetRoleEnterRoom(bSelf, bRoomOwner, nCamp, nHeroIconId, nTeamSlotId, sPlayerName, nSelfCamp, bIsComputerPlayer);
        }

        public void AddRoomSelectHeroItemData(IntPtr nHeroHeadSlotInfo, int nPtrLen)
        {
            // 1、解析Ban选数据
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(nHeroHeadSlotInfo, nPtrLen);
            cmd_match_room_hero_count banCount = helper.get<cmd_match_room_hero_count>();
            List<int> roomBanHero = new List<int>();
            for (int i = 0; i < banCount.nHeroCount; i++)
            {
                int nBanHeroID = helper.get<int>();
                roomBanHero.Add(nBanHeroID);
            }

            // 2、DataCenter英雄数据填充
            ActorHeroInfo actorHeroInfo = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
            if (actorHeroInfo == null)
                return;

            List<cmd_match_room_hero_slot> roomHeroList = new List<cmd_match_room_hero_slot>();
            int nHeroCount = 0;
            // 随机英雄
            cmd_match_room_hero_slot randomNode = new cmd_match_room_hero_slot();
            randomNode.nSkinIDList = new int[4];
            randomNode.bIsLock = 1;
            randomNode.nHeroID = (int)VOCATION.VOCATION_RANDOM;
            randomNode.nSlotIndex = nHeroCount++;
            randomNode.bCurrentIsHave = 1;
            randomNode.nHeroType = -1;
            randomNode.nPractisedLevel = 1;
            randomNode.nMatchNum = 0;
            roomHeroList.Add(randomNode);

            // 可选英雄
            foreach (var item in actorHeroInfo.dicEntityHeroInfo)
            {
                int nHeroID = item.Key;
                cmd_match_room_hero_slot haveNode = new cmd_match_room_hero_slot();
                haveNode.nSkinIDList = new int[4];
                // 去除Ban选
                int nIndex = roomBanHero.FindIndex(banNode => banNode == nHeroID);
                if (nIndex >= 0)
                    continue;
                // 配置
                PersonSkinModelItem modelData = new PersonSkinModelItem();
                if (!GameLogicAPI.GetPersonModel((UInt16)nHeroID, ref modelData))
                    continue;

                haveNode.nSlotIndex = nHeroCount++;
                haveNode.nHeroID = nHeroID;
                haveNode.nHeroType = modelData.nType;
                haveNode.nPractisedLevel = item.Value.nLv;
                haveNode.bIsLock = (byte)((item.Value.bIsBuy > 0 || item.Value.bIsWeekFree > 0) ? 1 : 0);
                haveNode.bCurrentIsHave = (byte)(item.Value.bIsBuy > 0 ? 1 : 0);
                haveNode.bIsWeekFree = (byte)(item.Value.bIsWeekFree > 0 ? 1 : 0);
                haveNode.szHeroName = modelData.szHeroName;	            
                for (int i = 0; i < 4; i++)
			    {
                    haveNode.nSkinIDList[i] = modelData.nSkinIDList[i];     
			    }
                haveNode.dwUnlockTime = (int)item.Value.ulUnlockTime;
                haveNode.nMatchNum = item.Value.nGameCount;
                roomHeroList.Add(haveNode);
            }
            // 不可选英雄
            foreach (var item in actorHeroInfo.dicEntityNoHaveHeroInfo)
            {
                int nHeroID = item.Key;
                cmd_match_room_hero_slot noHaveNode = new cmd_match_room_hero_slot();
                noHaveNode.nSkinIDList = new int[4];
                // 去除Ban选
                int nIndex = roomBanHero.FindIndex(banNode => banNode == nHeroID);
                if (nIndex >= 0)
                    continue;
                // 配置
                PersonSkinModelItem modelData = new PersonSkinModelItem();
                if (!GameLogicAPI.GetPersonModel((UInt16)nHeroID, ref modelData))
                    continue;

                noHaveNode.nSlotIndex = nHeroCount++;
                noHaveNode.nHeroID = nHeroID;
                noHaveNode.nHeroType = modelData.nType;
                noHaveNode.nPractisedLevel = 1;
                noHaveNode.bIsLock = 0;
                noHaveNode.bCurrentIsHave = 0;
                noHaveNode.bIsWeekFree = 0;
                noHaveNode.szHeroName = modelData.szHeroName;
                for (int i = 0; i < 4; i++)
                {
                    noHaveNode.nSkinIDList[i] = modelData.nSkinIDList[i];
                }
                noHaveNode.dwUnlockTime = 0;
                noHaveNode.nMatchNum = 0;
                roomHeroList.Add(noHaveNode);
            }

            if (SelectHeroPart)
                SelectHeroPart.AddRoomSelectHeroItemData(roomHeroList);
            if (banHeroView)
                banHeroView.OnRecvHeroList(roomHeroList);
        }

        public void SetRoomSelectHeroItemData()
        {
            if (SelectHeroPart)
                SelectHeroPart.SortSelectData();
        }

        public void SetSelectHeroListLock(bool bLock)
        {
            if (this.SelectHeroPart.RoomHeroList == null)
            {
                return;
            }

            foreach (UTileViewItemRoomHero ItemData in SelectHeroPart.RoomHeroList.SelectHeroData)
            {
                ItemData.bIsLock = bLock;
                ItemData.bIsLockByHero = true;
            }

            SelectHeroPart.RoomHeroList.UpdateItem();
        }

        public void SetSelectCanHeroSelect(bool bLock, int nHeroId)
        {
            if (this.SelectHeroPart.RoomHeroList == null)
            {
                return;
            }

            foreach (UTileViewItemRoomHero ItemData in SelectHeroPart.RoomHeroList.SelectHeroData)
            {
                if (nHeroId == ItemData.nHeroIconId)
                {
                    ItemData.Lock = bLock;
                    ItemData.bIsLockByHero = true;
                }
            }

            SelectHeroPart.RoomHeroList.UpdateItem();
        }

        public void SetCampSelectHeroCanUseByComputer(bool bLock, int nHeroId, bool bSelfCamp)
        {
            if (this.SelectHeroPart.RoomHeroList == null)
            {
                return;
            }

            if (!bSelfCamp)
            {
                return;
            }

            foreach (UTileViewItemRoomHero ItemData in SelectHeroPart.RoomHeroList.SelectHeroData)
            {
                if (nHeroId == ItemData.nHeroIconId)
                {
                    ItemData.Lock = bLock;
                    ItemData.bIsLockByComputer = bLock;
                }
            }

            if (!m_bAllowMachineLock)
            {
                return;
            }

            SelectHeroPart.RoomHeroList.UpdateItem();
        }

        private void ResetTopTimePart()
        {
            if (TopTimePart == null)
                return;

            TopTimePart.ResetData();
        }

        private void ResetTeamFramePart()
        {
            if (TeamPart == null)
                return;

            TeamPart.ResetData();
        }

        private void ResetSelectHeroFramePart()
        {
            SelectHeroPart.RoomHeroList.ResetData();
        }

        private void ResetBottomFunctionPart()
        {
            if (BottomFunctionPart == null)
            {
                return;
            }

            BottomFunctionPart.ResetData();
        }

        public void ResetRoomViewData()
        {
            ResetBottomFunctionPart();  // 重置天赋符文召唤师技能和确定按钮为最初状态
            ResetSelectHeroFramePart();
            ResetTeamFramePart();
            ResetTopTimePart();
            ResetMainViewPart();
            ResetSelecTipPart();

            banHeroPart.ResetData();
            banHeroView.ResetData();
            banHeroBtn.gameObject.SetActive(false);
            warningPanel.SetActive(false);
            ObPart.ResetData();
            if (BottomFunctionPart.ButtonFrame != null)
            {
                BottomFunctionPart.ReturnButton.transform.SetParent(BottomFunctionPart.ButtonFrame);
            }
        }

        private void ResetSelecTipPart()
        {
            if (SelectBattleTipPart == null)
                return;

            SelectBattleTipPart.ResetData();
        }

        public void ResetRoomEnterViewData()
        {
            ResetSelectHeroFramePart();
            ResetTeamFramePart();
            ResetTopTimePart();
            ResetMainViewPart();
        }

        private void ResetMainViewPart()
        {
            ClearSelectSkinModel();
        }

        public void SetGamePatternCampPlayerCount(int nCampPlayerCount)
        {
            if (TeamPart == null)
            {
                return;
            }

            TeamPart.SetGamePatternCampPlayerCount(nCampPlayerCount);
            TeamPart.SetGamePatternTeamSlotCreate();
            TeamPart.SetTeamFrameAnim();
            //SetModelHeroLightEffect();
        }

        public void SetCurPageIndexofStartGame()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RUNEPAGE_CHANGEPAGE, null);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TALENTPAGE_CHANGEPAGE, null);
        }
        // 将确认后的召唤师技能信息发送到逻辑增

        public void OnSelectHeroItemClick(int nIndex, ListViewItem item)
        {
            if (m_roomWnd == null)
                return;

            if (m_roomWnd.IsThisStateInRoom(EMatchRoomState.MatchRoomState_SelectHero))
                OnSelectHeroItemClickInSelectState(DealWithSelectItem(item));
            if (m_roomWnd.IsThisStateInRoom(EMatchRoomState.MatchRoomState_Wait))
                OnSelectHeroItemClickInWaitState(DealWithSelectItem(item));

            HeroTool.WaitDrag();
        }

        private void OnSelectHeroItemClickInWaitState(int nHeroId)
        {
            if (nHeroId == 0)
                return;

            m_roomWnd.SendPreSelectHeroMessage(nHeroId);

        }

        private int DealWithSelectItem(ListViewItem item)
        {
            if (SelectHeroPart.RoomHeroList.SelectDown)
            {
                return 0;
            }

            UTileViewComponentRoomHero heroItem = (UTileViewComponentRoomHero)item;
            if ((heroItem.GetData().bIsLock || heroItem.GetData().bIsBan) && m_roomWnd.IsThisStateInRoom(EMatchRoomState.MatchRoomState_SelectHero))
            {
                return 0;
            }

            int nHeroId = heroItem.GetData().nHeroIconId;

            UTileViewComponentRoomHero CurSelectHeroItem = SelectHeroPart.RoomHeroList.SelectHeroItem;
            if (CurSelectHeroItem)
            {
                int nOldHeadId = CurSelectHeroItem.GetData().nHeroIconId;
                if (nOldHeadId == nHeroId && nOldHeadId != -1)
                {
                    return 0;
                }

                CurSelectHeroItem.DestorySelectDownShotEffect();
            }

            // 清空选中状态，只置为当前选中项
            foreach (var removeSelectNode in SelectHeroPart.RoomHeroList.SelectHeroData)
            {
                removeSelectNode.bSelect = false;
            }
            SelectHeroPart.RoomHeroList.SelectHeroItem = heroItem;
            SelectHeroPart.RoomHeroList.SelectHeroItem.SetSelectDownShotEffect();
            SelectHeroPart.RoomHeroList.SelectHeroItem.GetData().bSelect = true;

            int nDeSkinID = 0;
            if (!PlayerLocalHeroConfig.Instance.GetIntData(ELocalHero.ELH_DeSkinID, nHeroId, out nDeSkinID))
                nDeSkinID = -1;
            cmd_Hero_Skin_Info skinCmd = new cmd_Hero_Skin_Info();
            if (!LogicDataCenter.heroSkinDataManager.GetHeroSkinInfo(nHeroId, nDeSkinID, ref skinCmd))
            {
                PersonSkinModelItem modelData = new PersonSkinModelItem();
                if (GameLogicAPI.GetPersonModel((UInt16)nHeroId, ref modelData))
                {
                    nDeSkinID = modelData.nDefaultSkinID;
                }
            }
            CacheSelectSkinInfo(nHeroId, nDeSkinID);
            SetHeroSkinModel(nDeSkinID, nHeroId);

            if (TopTimePart)
                TopTimePart.ChangeTopHeroName(nHeroId, heroItem.GetData().sHeroName, heroItem.GetData().sPlayWaysTip);

            if (SkillTipPart)
            {
                SkillTipPart.gameObject.SetActive(true);
                cmd_match_room_Mouse_HeroArea data = new cmd_match_room_Mouse_HeroArea();
                data.nHeroVocationID = nHeroId;
                ViewEventHelper.Instance.SendCommand<cmd_match_room_Mouse_HeroArea>(GameLogicDef.GVIEWCM_MATCH_MOUSE_LEFT_HERO_AREA, ref data);
            }

            if (SelectHeroPart)
                SelectHeroPart.DoSkinGroupItemShow();

            return nHeroId;
        }

        public void SetHeroSkinModel(int nSkinID, int nHeroId)
        {
            if (nSkinID <= 0 || nSkinID != m_nLastSkinId)
            {
                // 音效
                SkinInfoScheme skinConfig = new SkinInfoScheme();
                if (GameLogicAPI.GetHeroSkinInfo(nSkinID, ref skinConfig))
                {
                    SoundManager.DeleteSound(m_nSkinSoundID);
                    m_nSkinSoundID = skinConfig.nSoundID;
                }

                SkinManager.SetChangeHeroSkinEffect(nSkinID, ref m_LastSkinEffect, ref m_nLastSkinId, SelectTransForm, PERSONMODELVIEWID.PERSONMODELVIEWID_GAMEWAITINGROOM, OnSkinLoadFinish, this);
            }  
        }

        private void OnSkinLoadFinish(object obj)
        {
            SoundManager.CreateUISound(m_nSkinSoundID);
        }

        private void OnSelectHeroItemClickInSelectState(int nHeroId)
        {
            if (nHeroId == 0 || m_roomWnd == null || BottomFunctionPart == null)
                return;

            m_roomWnd.SendSelectHeroMessage(nHeroId);

            BottomFunctionPart.SetStartButtonToReadyState();
        }


        public void OnSelectHeroSkinItemClick(int nIndex, ListViewItem item)
        {
            if (m_roomWnd == null)
                return;

            UTileViewComponentRoomHeroSkin heroItem = (UTileViewComponentRoomHeroSkin)item;
            int nSkinId = heroItem.GetData().nSkinId;
            m_roomWnd.CacheSelectSkinInfo(heroItem.GetData().nHeroID, nSkinId);
            UTileViewComponentRoomHeroSkin CurSelectHeroItem = SelectHeroPart.RoomHeroListSkin.SelectHeroItem;
            if (CurSelectHeroItem)
            {
                int nOldSkinId = CurSelectHeroItem.GetData().nSkinId;
                if (nOldSkinId == nSkinId && nOldSkinId != -1)
                {
                    return ;
                }

                CurSelectHeroItem.SelectFrame.gameObject.SetActive(false);
                CurSelectHeroItem.SkinNameText.gameObject.SetActive(false);
                CurSelectHeroItem.GetData().bSelect = false;
                CurSelectHeroItem.DestorySelectDownShotEffect();
            }

            SelectHeroPart.RoomHeroListSkin.SelectHeroItem = heroItem;
            SelectHeroPart.RoomHeroListSkin.SelectHeroItem.SelectFrame.gameObject.SetActive(true);
            SelectHeroPart.RoomHeroListSkin.SelectHeroItem.SkinNameText.gameObject.SetActive(true);
            SelectHeroPart.RoomHeroListSkin.SelectHeroItem.SetSelectDownShotEffect();
            SelectHeroPart.RoomHeroListSkin.SelectHeroItem.GetData().bSelect = true;
            SetHeroSkinModel(nSkinId, heroItem.GetData().nHeroID);
            if(heroItem.GetData().bIsHaveSkin)
            {
                PlayerLocalHeroConfig.Instance.SetIntData(ELocalHero.ELH_DeSkinID, heroItem.GetData().nHeroID, nSkinId);
            }
        }

        public void SetChangeTeamItemHeroIcon(int nCamp, int nSlotId, int nHeroId, int nHeroStarLv, int nHeroStaticLv, bool bServerRand = false)
        {
            if (TeamPart == null || BottomFunctionPart == null)
                return;

            TeamPart.SetChangeTeamItemHeroIcon(nCamp, nSlotId, nHeroId, nHeroStarLv, nHeroStaticLv);

            // 自己
            if (!TeamPart.GetTeamItemIsSelf(nCamp, nSlotId))
                return;

            if ((VOCATION)nHeroId == VOCATION.VOCATION_INVALID)
            {
                ClearSelectSkinModel();
                ClearWaitStateSelectItem();
                return;
            }

            // 选人状态下,切换状态
            if (m_roomWnd.IsThisStateInRoom(EMatchRoomState.MatchRoomState_SelectHero))
                BottomFunctionPart.SetStartButtonToReadyState();

            // 设置随机状态下数据
            if (bServerRand)
            {
                int nDeSkinID = 0;
                if (!PlayerLocalHeroConfig.Instance.GetIntData(ELocalHero.ELH_DeSkinID, nHeroId, out nDeSkinID))
                    nDeSkinID = -1;
                cmd_Hero_Skin_Info skinCmd = new cmd_Hero_Skin_Info();
                if (!LogicDataCenter.heroSkinDataManager.GetHeroSkinInfo(nHeroId, nDeSkinID, ref skinCmd))
                {
                    PersonSkinModelItem modelData = new PersonSkinModelItem();
                    if (GameLogicAPI.GetPersonModel((UInt16)nHeroId, ref modelData))
                    {
                        nDeSkinID = modelData.nDefaultSkinID;
                    }
                }
                CacheSelectSkinInfo(nHeroId, nDeSkinID);
                SetHeroSkinModel(nDeSkinID, nHeroId);

                UTileViewItemRoomHero selectHeroItem = null;
                foreach (var item in SelectHeroPart.RoomHeroList.SelectHeroData)
                {
                    item.bSelect = false;
                    if (item.nHeroIconId == nHeroId)
                    {
                        selectHeroItem = item;
                    }
                }
                if (selectHeroItem == null)
                    return;
                SelectHeroPart.RoomHeroList.SelectHeroItemBackupsData = selectHeroItem;
                SelectHeroPart.RoomHeroList.SelectHeroItemBackupsData.bSelect = true;

                if (TopTimePart)
                    TopTimePart.ChangeTopHeroName(nHeroId, selectHeroItem.sHeroName, selectHeroItem.sPlayWaysTip);

                if (SkillTipPart)
                {
                    SkillTipPart.gameObject.SetActive(true);
                    cmd_match_room_Mouse_HeroArea data = new cmd_match_room_Mouse_HeroArea();
                    data.nHeroVocationID = nHeroId;
                    ViewEventHelper.Instance.SendCommand<cmd_match_room_Mouse_HeroArea>(GameLogicDef.GVIEWCM_MATCH_MOUSE_LEFT_HERO_AREA, ref data);
                }
            }

            SelectHeroPart.RoomHeroList.UpdateItem();

            if (SelectHeroPart)
                SelectHeroPart.DoSkinGroupItemShow();

            m_roomWnd.InitXPSkillFrame(nHeroId);
        }

        public void SetShowEnterWarTime(int nTime)
        {
            if (TopTimePart == null)
                return;

            TopTimePart.SetShowEnterWarTime(nTime);
        }

        public void SetTeamItemHeroConfirm(int nCamp, int nSlotId)
        {
            if (TeamPart == null)
                return;

            TeamPart.SetTeamItemHeroConfirm(nCamp, nSlotId);

            if (TeamPart.GetTeamItemIsSelf(nCamp, nSlotId))
                SelectHeroPart.RoomHeroList.SelectDown = true;
        }

        public void SetShowSelectHeroTime(int nTime)
        {
            if (TopTimePart == null)
                return;

            TopTimePart.SetShowSelectHeroTime(nTime);
        }

        public void SetStartButtonToReadyState()
        {
            if (BottomFunctionPart == null)
                return;

            BottomFunctionPart.SetStartButtonToReadyState();
        }

        public void SetStartButtonToWaitForSelectState()
        {
            if (BottomFunctionPart == null)
                return;

            BottomFunctionPart.SetStartButtonToWaitForSelectState();


            // 初选未购买英雄，则清除初选状态
            if (SelectHeroPart.RoomHeroList.SelectHeroItemBackupsData.bIsLock)
            {
                ClearWaitStateSelectItem();
                ClearSelectSkinModel();
                ClearXpSkillButtonOutXp();

                BottomFunctionPart.BuyButton.gameObject.SetActive(false);
            }
        }

        private void ClearXpSkillButtonOutXp()
        {
            if (BottomFunctionPart != null && BottomFunctionPart.XpSkillFrame != null)
                BottomFunctionPart.XpSkillFrame.ClearXpSkillButtonOutXp();
        }

        private void ClearSelectSkinModel()
        {
            if (m_LastSkinEffect == null)
                return;

            SceneEffectManager.Instance.DestroySceneEffect(ref m_LastSkinEffect);
            m_LastSkinEffect = null;
            m_nLastSkinId = 0;

            SoundManager.DeleteSound(m_nSkinSoundID);

            SkillTipPart.gameObject.SetActive(false);
        }

        public void ClearSkinSound()
        {
            SoundManager.DeleteSound(m_nSkinSoundID);
        }

        public void OnStartButtonBeginClick()
        {
            // 显示警告界面
            if (m_roomWnd.GetRoomType() == EMHallRoomType.HallRoomType_LegendCup)
            {
                // todo 杯赛做开始校验 做其他提示
                // 杯赛先不做人数校验
            }
            else
            {
                if (!checkIsCanGetAwake())
                {
                    string szDes = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "PromptTextDes");
                    UBB.toHtml(ref szDes, UBB_FORMAT_TYPE.UGUI);
                    PromptText.text = szDes;
                    warningPanel.SetActive(true);
                    return;
                }
            }
            
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_START_GAME);
            SetHideTeamCustomModeButtons();
        }

        // 当点击警告界面的确认按钮时
        public void onClickWarningPanelConfirmButton()
        {
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_ROOM_MASTER_COVER_COMPUTER);
            ClearWaitStateSelectItem();
            warningPanel.SetActive(false);
        }

        public void onClickWarningPanelCancelButton()
        {
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_START_GAME);
            SetHideTeamCustomModeButtons();
            ClearWaitStateSelectItem();
            warningPanel.SetActive(false);
        }

        // 检测比赛过后时候可以获得奖励
        bool checkIsCanGetAwake()
        {
            int nMatchTypeID = (int)m_roomWnd.GetMatchType();
            int nCampCount = TeamPart.TeamFrameArray.Length;
            for (int i = 0; i < nCampCount; ++i)
            {
                int nCount = GetTeamCampPlayerCount(i);
                int nSelfLimit = GameLogicAPI.getCampPersonNumRestriction(nMatchTypeID, i + 1);
                if (nCount < nSelfLimit)
                {
                    return false;
                }
            }
            return true;
        }

        private void ClearWaitStateSelectItem()
        {
            SelectHeroPart.RoomHeroList.ClearSelectState();
            SelectHeroPart.RoomHeroList.UpdateItem();

            if (TopTimePart)
                TopTimePart.SetHideHeroName();
        }

        public void OnStartButtonReadyClick()
        {
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_ROOM_CONFIRM_HERO);

            if (GameLogicAPI.getPlayerNoviceGuideTypeData() < (int)ENNOVICEGUIDE_TYPE.ENNOVICEGUIDE_TYPE_GUIDE_TWOSTEP)
            {
                // 发送到逻辑层 引导相关记录
                cmd_guide_info guideInfo = new cmd_guide_info();
                guideInfo.nGuideID = (int)EMMatchGuideStep.GuideStep_RadomMachine_ConfrimHero;

                ViewEventHelper.Instance.SendCommand<cmd_guide_info>(GameLogicDef.GVIEWCMD_MATCH_GUIDE_INFO, ref guideInfo);
            }

            SetCurPageIndexofStartGame();

        }

        private void SetHideTeamCustomModeButtons()
        {
            if (TeamPart == null)
                return;

            TeamPart.SetHideTeamCustomModeButtons();
        }

        public void SetGameRoomModel(EMHallRoomType roomType, bool bLocalRoomOwner, bool bOnlineLegendCup)
        {
            if (TeamPart == null || BottomFunctionPart == null)
                return;

            TeamPart.SetGameRoomModel(roomType, bLocalRoomOwner);
            BottomFunctionPart.SetGameRoomModel(roomType, bLocalRoomOwner, bOnlineLegendCup);

            // 自定义房间显示填充电脑邀请玩家
            if (EMHallRoomType.HallRoomType_Custom == roomType)
                BottomFunctionPart.SetOwnerBtnVisible(bLocalRoomOwner);
            else
                BottomFunctionPart.SetOwnerBtnVisible(false);
        }

        public void SetInitSelectSummonerSkill(int nIndex, int nIconId, int nSkillId, string sDesc)
        {
            if (BottomFunctionPart == null)
                return;

            BottomFunctionPart.SetInitSelectSummonerSkill(nIndex, nIconId, nSkillId, sDesc);
        }

        public void SelectDefaultSummerSkill()
        {
            if (BottomFunctionPart == null)
                return;

            BottomFunctionPart.SelectDefaultSummerSkill();
        }

        public void SetAllSummerSkill(int nIndex, int nIconId, int nSkillId, bool bLock, string sDesc)
        {
            if (BottomFunctionPart == null)
                return;

            BottomFunctionPart.SetAllSummerSkill(nIndex, nIconId, nSkillId, bLock, sDesc);
        }

        public void SetInitSelectXpSkill(int nIndex, int nIconId, int nSkillId, string sDesc, bool bCanUseHero)
        {
            if (BottomFunctionPart == null)
                return;

            BottomFunctionPart.SetInitSelectXpSkill(nIndex, nIconId, nSkillId, sDesc, bCanUseHero);
        }

        public void SetAllXpSkill(int nIndex, int nIconId, int nSkillId, bool bLock, string sDesc,int nHeroID)
        {
            if (BottomFunctionPart == null)
                return;

            BottomFunctionPart.SetAllXpSkill(nIndex, nIconId, nSkillId, bLock, sDesc, nHeroID);
        }

        public void ClearXPSkillFrame()
        {
            if (BottomFunctionPart == null)
                return;

            BottomFunctionPart.ClearXPSkillFrame();
        }
        
        public void OnReturnButtonClick()
        {
            if (m_roomWnd == null)
                return;

            m_roomWnd.OnEscCallBack();
        }

        public void SetTeamItemSummerInfo(int nCamp, int nTeamSlotId, int nSummerIdOne, int nSummerIdTwo)
        {
            if (TeamPart == null)
                return;

            TeamPart.SetTeamItemSummerInfo(nCamp, nTeamSlotId, nSummerIdOne, nSummerIdTwo);
        }

        public void SetGameModelWarId(int nWarId)
        {
            if (TeamPart == null)
                return;

            TeamPart.SetAllTeamItemWarId(nWarId);
        }

        public void SetSelectListInit()
        {
            SelectHeroPart.RoomHeroList.DataSource.Clear();
            SelectHeroPart.RoomHeroList.OnSelect.RemoveListener(OnSelectHeroItemClick);
            SelectHeroPart.RoomHeroList.OnSelect.AddListener(OnSelectHeroItemClick);

            SelectHeroPart.RoomHeroListSkin.DataSource.Clear();
            SelectHeroPart.RoomHeroListSkin.OnSelect.RemoveListener(OnSelectHeroSkinItemClick);
            SelectHeroPart.RoomHeroListSkin.OnSelect.AddListener(OnSelectHeroSkinItemClick);
        }

        public void SetComputerHeadIconChange(cmd_match_room_computer_hero_change cmdData)
        {
            if (TeamPart == null)
                return;

            TeamPart.SetComputerHeadIconChange(cmdData);
        }

        public void SetCampComputerCanUse(int nCamp, bool bSelect, int nHeroId)
        {
            if (TeamPart == null)
                return;

            TeamPart.SetCampComputerCanUse(nCamp, bSelect, nHeroId);
        }

        public void SetTeamItemClear(int nCamp, int nTeamSlotId)
        {
            if (TeamPart == null)
                return;

            TeamPart.SetTeamItemClear(nCamp, nTeamSlotId);
        }

        public void SetOb2Team(int nCamp, int nTeamSlotId, bool bIsSelf, bool bRoomOwner, string sPlayerName)
        {
            if (TeamPart == null)
                return;

            TeamPart.SetRoomTeamItemChangePosition(nCamp, nTeamSlotId, bIsSelf, bRoomOwner, sPlayerName);
        }

        public void SetRoomMasterChanges(int nCamp, int nTeamSlotId)
        {
            if (TeamPart == null)
                return;

            TeamPart.SetTeamItemRoomOwner(nCamp, nTeamSlotId);
        }

        public void SetRoomTeamItemChangePosition(int nCampFrom, int nCampTo, int nPlayerSlotIDFrom, int nPlayerSlotIDTo)
        {
            if (TeamPart == null)
                return;

            TeamPart.SetRoomTeamItemChangePosition(nCampFrom, nCampTo, nPlayerSlotIDFrom, nPlayerSlotIDTo);
        }

        public void SetHideTime()
        {
            if (TopTimePart == null)
                return;

            TopTimePart.SetHideTime();
        }

        public void UpdateTalentComBoxData()
        {
            if (BottomFunctionPart == null)
                return;

            BottomFunctionPart.UpdateTalentComBoxData();
        }

        public void CloseComboListView()
        {
            if (BottomFunctionPart == null)
                return;

            BottomFunctionPart.CloseComboListView();
        }

        public void OnHeroTypeButtonClick(int nHeroType)
        {
            ObservableList<UTileViewItemRoomHero> newSelectHeroData = new ObservableList<UTileViewItemRoomHero>();
            foreach (UTileViewItemRoomHero item in SelectHeroPart.RoomHeroList.SelectHeroData)
            {
                if (item.nHeroType != nHeroType && nHeroType != -1)
                    continue;

                item.bSelect = false;
                newSelectHeroData.Add(item);
            }

            SelectHeroPart.RoomHeroList.ResetPageIndex();
            SelectHeroPart.RoomHeroList.SelectHeroDataFilter = newSelectHeroData;
            SelectHeroPart.RoomHeroList.SetOnePageListItem();
            SelectHeroPart.RoomHeroList.ClearSelectState();

            if (SelectHeroPart.SkinTypeObj)
                SelectHeroPart.SkinTypeObj.gameObject.SetActive(false);
        }

        public void OnSkinTypeButtonClick(int nSkinType)
        {
            ObservableList<UTileViewItemRoomHeroSkin> newSelectSkinData = new ObservableList<UTileViewItemRoomHeroSkin>();
            foreach (UTileViewItemRoomHeroSkin item in SelectHeroPart.RoomHeroListSkin.SelectHeroData)
            {
                if (item.nSkinType != nSkinType && nSkinType != -1)
                    continue;

                item.bSelect = false;
                newSelectSkinData.Add(item);
            }

            SelectHeroPart.RoomHeroListSkin.ResetPageIndex();
            SelectHeroPart.RoomHeroListSkin.SelectHeroDataFilter = newSelectSkinData;
            SelectHeroPart.RoomHeroListSkin.SetOnePageListItem();
            SelectHeroPart.RoomHeroListSkin.ClearSelectState();
        }

        public void UpdateSelectHeroFrameLockByComputer(int nCamp, int nCampCount)
        {
            if (TeamPart == null || SelectHeroPart.RoomHeroList == null)
                return;

            foreach (UTileViewItemRoomHero ItemData in SelectHeroPart.RoomHeroList.SelectHeroData)
            {
                if (ItemData.bIsLockByComputer == true)
                    ItemData.Lock = false;

                for (int i = 0; i < nCampCount; i++)
                {
                    int nComputerId = TeamPart.GetRoomTeamItemComputerId(nCamp, i);
                    if (nComputerId == ItemData.nHeroIconId)
                    {
                        ItemData.Lock = true;
                        ItemData.bIsLockByComputer = true;
                        break;
                    }
                }
            }

            if (!m_bAllowMachineLock)
                return;

            SelectHeroPart.RoomHeroList.UpdateItem();
        }

        public void ClearCampTeamSelectTip()
        {
            if (SelectBattleTipPart == null)
                return;

            SelectBattleTipPart.ClearCampTeamSelectTip();
        }

        public void SetCampTeamSelectTip(int nTipTypeIndex, string sTipDesc)
        {
            if (SelectBattleTipPart == null)
            {
                return;
            }

            SelectBattleTipPart.SetSelectBattleArrayItemTipInfo(nTipTypeIndex, sTipDesc);
        }

        public void SetHeroSkillInfo(IntPtr nSkillTextInfo, int nPtrLen)
        {
            if (SkillTipPart == null)
                return;

            SkillTipPart.SetSkillInfo(nSkillTextInfo, nPtrLen);
        }

        public void OnPersonModelLeftClick()
        {
            if (m_LastSkinEffect == null || m_LastSkinEffect.ReturnComponent == null)
                return;

            Vector3 rotation = m_LastSkinEffect.ReturnComponent.transform.localEulerAngles;
            rotation = new Vector3(rotation.x, rotation.y - 30, rotation.z);
            m_LastSkinEffect.ReturnComponent.transform.DORotate(rotation, 0.2f);
        }

        public void OnPersonModelRightClick()
        {
            if (m_LastSkinEffect == null || m_LastSkinEffect.ReturnComponent == null)
                return;

            Vector3 rotation = m_LastSkinEffect.ReturnComponent.transform.localEulerAngles;
            rotation = new Vector3(rotation.x, rotation.y + 30, rotation.z);
            m_LastSkinEffect.ReturnComponent.transform.DORotate(rotation, 0.2f);
        }

        public int GetTeamItemHeroId(int nCamp, int nSlotId)
        {
            if (TeamPart == null)
                return 0;

            return TeamPart.GetTeamItemHeroId(nCamp, nSlotId);
        }

        public int GetTeamCampPlayerCount(int nCamp)
        {
            if (TeamPart == null)
                return 0;

            return TeamPart.GetTeamCampPlayerCount(nCamp);
        }

        public void SetDelayJoin(int nTime)
        {
            // 这里设置是否可以挂起
            UIMsgCmdData uiDelayData = new UIMsgCmdData((int)WndMsgID.WND_MSG_WAITINGROOM_DELAYJOIN_SHOW, nTime, "", IntPtr.Zero, 0);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAITINGROOM_DELAYJOIN_SHOW, uiDelayData);
        }

        public void RotateHeroModel(float fDelta)
        {
            if (m_LastSkinEffect == null || m_LastSkinEffect.ReturnComponent == null)
                return;

            Vector3 rotation = m_LastSkinEffect.ReturnComponent.transform.localEulerAngles;
            rotation = new Vector3(rotation.x, rotation.y + fDelta, rotation.z);
            m_LastSkinEffect.ReturnComponent.transform.rotation = Quaternion.Euler(rotation);
        }

        public void ScaleHeroModel(float fDelta)
        {
            if (m_LastSkinEffect == null || m_LastSkinEffect.ReturnComponent == null)
                return;

            Vector3 scale = m_LastSkinEffect.ReturnComponent.transform.localScale;
            scale = new Vector3(scale.x + fDelta, scale.y + fDelta, scale.z + fDelta);
            m_LastSkinEffect.ReturnComponent.transform.localScale = scale;
        }

        public void SetMatchRoomState(EMatchRoomState eCurRoomState)
        {
            if (SelectHeroPart == null || SelectHeroPart.RoomHeroList == null || BottomFunctionPart == null)
                return;

            SelectHeroPart.RoomHeroList.SetRoomListItemStateData(eCurRoomState);

            if (eCurRoomState == EMatchRoomState.MatchRoomState_ReadyGo)
            {
                BottomFunctionPart.SetReturnButtonInvalid();
                BottomFunctionPart.SetOnlineLegendStartButtonInitState();
            }
            if (eCurRoomState != EMatchRoomState.MatchRoomState_Wait)
            {
                BottomFunctionPart.SetInviteButtonVisble(false);
            }
            if (eCurRoomState == EMatchRoomState.MatchRoomState_SelectHero)
            {
                banHeroBtn.gameObject.SetActive(false);
                banHeroView.gameObject.SetActive(false);
            }
        }

        public void SetGameWaitingRoomID(int nRoomID,bool bShow)
        {
            if (IDPart != null)
            {
                IDPart.gameObject.SetActive(bShow);
                if (bShow)
                    IDPart.SetRoomIDText("房间编号：" + nRoomID);
            }         
        }
        
        public void updateBenefitCard()
        {
            BenefitCardInfoPart.updateBenefitCardInfo();
        }

        public void InitSelectHeroPart()
        {
            SelectHeroPart.InitSelectHeroPart(this);
        }

        public void UpdateRoomSkinList(int nHeroID,int m_nUpdateSkinID)
        {
            SelectHeroPart.UpdateRoomSkinList( nHeroID,m_nUpdateSkinID);
            BottomFunctionPart.BuyButton.UpdateFrame();
            SetHeroSkinModel(m_nUpdateSkinID, nHeroID);
        }

        public bool UpdateRoomHeroList(int m_nUpdateHeroID)
        {
            bool bUpdate = SelectHeroPart.UpdateRoomHeroList(m_nUpdateHeroID);

            if (bUpdate)
                BottomFunctionPart.BuyButton.UpdateFrame();

            return bUpdate;
        }

        public void SetLegendCupTimeCountDown(int nTime)
        {
            if (TopTimePart == null)
            {
                return;
            }
            TopTimePart.SetLegendCupTimeCountDown(nTime);
        }

        public void SetOfflineLegendCupDesc()
        {
            if (TopTimePart == null)
            {
                return;
            }
            TopTimePart.SetOfflineLegendCupDesc();
        }

        public void SetBanHeroTimeCountDown(int nTime)
        {
            if (TopTimePart == null)
            {
                return;
            }
            TopTimePart.SetBanHeroTimeCountDown(nTime);
        }

        public void CacheSelectSkinInfo(int nHeroID, int nSkinID)
        {
            if (m_roomWnd != null)
                m_roomWnd.CacheSelectSkinInfo(nHeroID, nSkinID);
        }

        #region Ban选部分
        // wait阶段Ban选操作
        public void RecvSingleBanHero(IntPtr ptrParam, int nPtrLen)
         {
             if (banHeroPart == null || banHeroView == null)
                 return;

             IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
             cmd_room_banInfo_head head = helper.get<cmd_room_banInfo_head>();

             List<cmd_room_banInfo_sub> banHeroList = new List<cmd_room_banInfo_sub>();
             for (int i = 0; i < head.nCount; i++)
             {
                 cmd_room_banInfo_sub node = helper.get<cmd_room_banInfo_sub>();
                 banHeroList.Add(node);
             }
             banHeroPart.RecvSingleBanHero(head.nCampMaxPlayerCount, banHeroList);

             List<int> banHeroIdList = banHeroPart.BanHeroIdList;
             banHeroView.RecvSingleBanHero(banHeroIdList);

             for (int i = 0; i < SelectHeroPart.RoomHeroList.DataSource.Count; i++)
             {
                 SelectHeroPart.RoomHeroList.DataSource[i].bIsBan = false;
                 if (banHeroIdList.Contains(SelectHeroPart.RoomHeroList.DataSource[i].nHeroIconId))
                 {
                     SelectHeroPart.RoomHeroList.DataSource[i].bIsBan = true;
                 }
             }
             for (int i = 0; i < SelectHeroPart.RoomHeroList.SelectHeroDataFilter.Count; i++)
             {
                 SelectHeroPart.RoomHeroList.SelectHeroDataFilter[i].bIsBan = false;
                 if (banHeroIdList.Contains(SelectHeroPart.RoomHeroList.SelectHeroDataFilter[i].nHeroIconId))
                 {
                     SelectHeroPart.RoomHeroList.SelectHeroDataFilter[i].bIsBan = true;
                 }
             }
             SelectHeroPart.RoomHeroList.UpdateItems();
         }

        // select阶段Ban选结果
        public void RecvAllBanHero(IntPtr ptrParam, int nPtrLen)
        {
            if (banHeroPart == null || banHeroView == null)
                return;

            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_room_banInfo_head head = helper.get<cmd_room_banInfo_head>();

            // 更新banHeroPart显示
            List<cmd_room_banInfo_sub> banHeroList = new List<cmd_room_banInfo_sub>();
            for (int i = 0; i < head.nCount; i++)
            {
                cmd_room_banInfo_sub node = helper.get<cmd_room_banInfo_sub>();
                banHeroList.Add(node);
            }
            banHeroPart.RecvAllBanHero(head.nCampMaxPlayerCount, banHeroList);

            // 更新banHeroView显示
            List<int> banHeroIdList = banHeroPart.BanHeroIdList;
            banHeroView.RecvSingleBanHero(banHeroIdList);

            // 更新英雄选择列表显示
            for (int i = 0; i < SelectHeroPart.RoomHeroList.DataSource.Count; i++)
            {
                SelectHeroPart.RoomHeroList.DataSource[i].bIsBan = false;
                if (banHeroIdList.Contains(SelectHeroPart.RoomHeroList.DataSource[i].nHeroIconId))
                {
                    SelectHeroPart.RoomHeroList.DataSource[i].bIsBan = true;
                }
            }
            for (int i = 0; i < SelectHeroPart.RoomHeroList.SelectHeroDataFilter.Count; i++)
            {
                SelectHeroPart.RoomHeroList.SelectHeroDataFilter[i].bIsBan = false;
                if (banHeroIdList.Contains(SelectHeroPart.RoomHeroList.SelectHeroDataFilter[i].nHeroIconId))
                {
                    SelectHeroPart.RoomHeroList.SelectHeroDataFilter[i].bIsBan = true;
                }
            }
            SelectHeroPart.RoomHeroList.UpdateItems();
        }

        // Ban选许可通知
        public void RecvBanHeroFlag(int nFlag)
        {
            if (banHeroBtn == null)
                return;

            banHeroBtn.gameObject.SetActive(nFlag > 0);
        }

        // 打开Ban选界面
        public void OnClickBanHeroBtn()
        {
            if (banHeroView == null)
                return;

            banHeroView.gameObject.SetActive(true);
        }

        // 服务器禁用英雄数据
        public void SetBanHeroData(IntPtr ptrParam, int nParam)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nParam);
            cmd_count cmdCount = helper.get<cmd_count>();

            List<int> banHeroIdList = new List<int>();
            for (int i = 0; i < cmdCount.nCount; ++i)
            {
                int nHeroId = helper.getInt();
                banHeroIdList.Add(nHeroId);
            }

            for (int i = 0; i < SelectHeroPart.RoomHeroList.DataSource.Count; i++)
            {
                if (banHeroIdList.Contains(SelectHeroPart.RoomHeroList.DataSource[i].nHeroIconId))
                {
                    SelectHeroPart.RoomHeroList.DataSource[i].bIsBan = true;
                }
            }
            SelectHeroPart.RoomHeroList.UpdateItems();
        }
        #endregion

        #region Ob部分
        public void SetGameModeOb()
        {
            BottomFunctionPart.ReturnButton.transform.SetParent(this.transform);
            BottomFunctionPart.gameObject.SetActive(false);
            SelectHeroPart.gameObject.SetActive(false);
            ClearWaitStateSelectItem();
            ClearSelectSkinModel();
            //SelectBattleTipPart.gameObject.SetActive(false);
            //SkillTipPart.gameObject.SetActive(false);
            //BenefitCardInfoPart.gameObject.SetActive(false);
            //banHeroBtn.gameObject.SetActive(false);
        }

        public void SetGameModeNotOb()
        {
            if (BottomFunctionPart.ButtonFrame != null)
            {
                BottomFunctionPart.ReturnButton.transform.SetParent(BottomFunctionPart.ButtonFrame);
            }
            BottomFunctionPart.gameObject.SetActive(true);
            SelectHeroPart.gameObject.SetActive(true);
            //SelectBattleTipPart.gameObject.SetActive(true);
            //SkillTipPart.gameObject.SetActive(true);
            //BenefitCardInfoPart.gameObject.SetActive(true);
            //banHeroBtn.gameObject.SetActive(true);
        }

        public void OnMsgInitOb(bool bPermitOb, int nObMaxCount)
        {
            ObPart.OnMsgInitOb(bPermitOb, nObMaxCount);
        }

        public void OnMsgAddOb(cmd_match_room_ob_data data)
        {
            ObPart.OnMsgAddOb(data);
        }

        public void OnMsgDeleteOb(int nPos)
        {
            ObPart.OnMsgDeleteOb(nPos);
        }

        #endregion

        #region 新手引导部分
        internal void AddSelectHeroGuideUIWidget(UGuideWidgetMsgData _msgData)
        {
            if(SelectHeroPart != null)
            {
                Guide.UGuideWidget guideWidget = SelectHeroPart.gameObject.GetComponent<Guide.UGuideWidget>();
                if (guideWidget == null)
                    guideWidget = SelectHeroPart.gameObject.AddComponent<Guide.UGuideWidget>();
                guideWidget.GuideID = (GUIDE.EGuideNodeID)_msgData.nGuideID;
                guideWidget.GuideStepID = _msgData.nGuideStepID;
                guideWidget.GuideEffectID = _msgData.nGuideEffectID;

            }
        }

        internal void RemoveSelectHeroGuideUIWidget(UGuideWidgetMsgData _msgData)
        {
            if (SelectHeroPart != null)
            {
                Guide.UGuideWidget guideWidget = SelectHeroPart.gameObject.GetComponent<Guide.UGuideWidget>();
                if (guideWidget != null)
                {
                    SelectHeroPart.gameObject.RemoveComponent<Guide.UGuideWidget>();
                }
            }
        }

        internal void AddStartBtnGuideUIWidget(UGuideWidgetMsgData _msgData)
        {
            if (BottomFunctionPart != null && BottomFunctionPart.StartButton != null)
            {
                Guide.UGuideWidget guideWidget = BottomFunctionPart.StartButton.gameObject.GetComponent<Guide.UGuideWidget>();
                if (guideWidget == null)
                    guideWidget = BottomFunctionPart.StartButton.gameObject.AddComponent<Guide.UGuideWidget>();
                guideWidget.GuideID = (GUIDE.EGuideNodeID)_msgData.nGuideID;
                guideWidget.GuideStepID = _msgData.nGuideStepID;
                guideWidget.GuideEffectID = _msgData.nGuideEffectID;

            }
        }

        internal void RemoveStartBtnGuideUIWidget(UGuideWidgetMsgData _msgData)
        {
            if (BottomFunctionPart != null && BottomFunctionPart.StartButton != null)
            {
                Guide.UGuideWidget guideWidget = BottomFunctionPart.StartButton.gameObject.GetComponent<Guide.UGuideWidget>();
                if (guideWidget != null)
                {
                    BottomFunctionPart.StartButton.gameObject.RemoveComponent<Guide.UGuideWidget>();
                }
            }
        }

        internal void GuideActionBegin()
        {
            if(BottomFunctionPart != null && BottomFunctionPart.ReturnButton != null)
            {
                BottomFunctionPart.ReturnButton.transform.parent.gameObject.SetActive(false);
            }
            if(SelectHeroPart != null && SelectHeroPart.RoomHeroList != null)
            {
                SelectHeroPart.RoomHeroList.OnSelect.AddListener(GuideHeroSelect);

            }
        }
        internal void GuideActionEnd()
        {
            if (BottomFunctionPart != null && BottomFunctionPart.ReturnButton != null)
            {
                BottomFunctionPart.ReturnButton.transform.parent.gameObject.SetActive(true);
            }
            if (SelectHeroPart != null && SelectHeroPart.RoomHeroList != null)
            {
                SelectHeroPart.RoomHeroList.OnSelect.RemoveListener(GuideHeroSelect);
            }
        }

        internal void GuideHeroSelect(int index,ListViewItem item)
        {
            Guide.UGuideWidget guideWidget = SelectHeroPart.gameObject.GetComponent<Guide.UGuideWidget>();
            if(guideWidget != null)
            {
                guideWidget.Hide();
            }
        }
        #endregion
        #endregion
    }
}
