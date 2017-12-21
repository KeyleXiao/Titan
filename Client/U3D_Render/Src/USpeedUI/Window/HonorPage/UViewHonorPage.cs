/*******************************************************************
** 文件名:	UViewHonorPage.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	2016-5-26
** 版  本:	1.0
** 描  述:	荣耀页界面逻辑模块(接发消息)+视图模块(显示数据)	
********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using DataCenter;
using USpeedUI.PlayerFrame;

namespace USpeedUI.HonorPage
{
    enum SHOWINDEX
    {
        ENACHIEVE_KILLBIGDRAGONNUM = 0,                 // 击杀大龙次数
        ENACHIEVE_CONTINUEWINMAX,                       // 最高连胜次数
        ENACHIEVE_KILLSMALLDRAGONNUM,                   // 击杀小龙次数
        ENACHIEVE_DAMAGEMAX,                            // 最高承受伤害
        ENACHIEVE_GOLDFIRST,                            // 经济第一,暂时没有数据
        ENACHIEVE_KILLMONSTERMAX,                       // 补兵最多
        ENACHIEVE_ONCEASSISTCOUNT,                      // 助攻数最高的次数
        ENACHIEVE_KILLMAX,                              // 最高击杀
        ENACHIEVE_FIRSTKILLTOWERNUM,                    // 推塔第一次数
        ENACHIEVE_SCOREMAX,                             // 最高评分
        ENACHIEVE_FIRSTDIENUM,                          // 死亡第一的次数
        ENACHIEVE_CONTINUEKILLMAX,                      // 最高连杀
        ENACHIEVE_ASSISTMAX,                            // 最高助攻
        ENACHIEVE_KILLTOWETMAX,                         // 最多推塔
        ENACHIEVE_OUTMAX,                               // 最高输出
        ENACHIEVE_TIMEMAX,                              // 最久比赛时间
        ENACHIEVE_MONEYMAX,                             // 最佳经济
        ENACHIEVE_TIMEMIN,                              // 最短比赛事件

        ENACHIEVE_MAX
    };

    public class UHonorItem : MonoBehaviour, IPointerDownHandler
    {
        public Image honorIcon;
        public Text honorLabel;
        public Text honorValue;

        private int nIndex;
        private long lWarID;

        public void Clear()
        {
            gameObject.SetActive(false);
        }

        public void SetData(int index, int value, long warID)
        {
            gameObject.SetActive(true);

            // 设置值
            nIndex = index;
            lWarID = warID;
            honorValue.text = value.ToString();

            // 设置图标,设置标题
            switch (index)
            {
                case (int)ENACHIEVE.ENACHIEVE_KILLBIGDRAGONNUM:                     // 击杀大龙次数
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "killBigDragon");
                        honorIcon.gameObject.SetActive(true);
                        honorIcon.sprite = GetHeroIcon((int)EDynamicStateMoba.EDSM_HonorView_KillBigDragon);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_KILLSMALLDRAGONNUM:                   // 击杀小龙次数
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "killSmallDragon");
                        honorIcon.gameObject.SetActive(true);
                        honorIcon.sprite = GetHeroIcon((int)EDynamicStateMoba.EDSM_HonorView_KillSmallDragon);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_FIRSTKILLTOWERNUM:                    // 推塔第一次数
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "towelFirst");
                        honorIcon.gameObject.SetActive(true);
                        honorIcon.sprite = GetHeroIcon((int)EDynamicStateMoba.EDSM_HonorView_TowelFirst);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_FIRSTDIENUM:                          // 死亡第一的次数
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "deathFirst");
                        honorIcon.gameObject.SetActive(true);
                        honorIcon.sprite = GetHeroIcon((int)EDynamicStateMoba.EDSM_HonorView_DeathFirst);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_ONCEASSISTCOUNT:                      // 助攻数最高的次数
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "assistFirst");
                        honorIcon.gameObject.SetActive(true);
                        honorIcon.sprite = GetHeroIcon((int)EDynamicStateMoba.EDSM_HonorView_AssistFirst);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_ADDVALUE:                             
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "goldFirst");
                        honorIcon.gameObject.SetActive(true);
                        honorIcon.sprite = GetHeroIcon((int)EDynamicStateMoba.EDSM_HonorView_EconomyFirst);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_CONTINUEWINMAX:                       // 最高连胜次数
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "mostContinueWin");
                        honorIcon.gameObject.SetActive(false);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_SCOREMAX:                             // 最高评分
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "topScore");
                        honorIcon.gameObject.SetActive(false);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_KILLMAX:                              // 最高击杀
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "mostKill");
                        honorIcon.gameObject.SetActive(false);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_ASSISTMAX:                            // 最高助攻
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "mostAssist");
                        honorIcon.gameObject.SetActive(false);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_CONTINUEKILLMAX:                      // 最高连杀
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "mostContinueKill");
                        honorIcon.gameObject.SetActive(false);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_OUTMAX:                               // 最高输出
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "mostDps");
                        honorIcon.gameObject.SetActive(false);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_DAMAGEMAX:                            // 最高承受伤害
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "mostBear");
                        honorIcon.gameObject.SetActive(false);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_MONEYMAX:                             // 最佳经济
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "mostGold");
                        honorIcon.gameObject.SetActive(false);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_KILLMONSTERMAX:                       // 补兵最多
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "mostKs");
                        honorIcon.gameObject.SetActive(false);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_KILLTOWETMAX:                         // 最多推塔
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "mostTower");
                        honorIcon.gameObject.SetActive(false);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_TIMEMAX:                              // 最久比赛时间
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "longestGame");
                        honorIcon.gameObject.SetActive(false);
                    }
                    break;
                case (int)ENACHIEVE.ENACHIEVE_TIMEMIN:                              // 最短比赛事件
                    {
                        honorLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "shortestGame");
                        honorIcon.gameObject.SetActive(false);
                    }
                    break;
                default:
                    break;
            }
        }

        public void OnPointerDown(PointerEventData eventData)
        {
            if (nIndex < (int)ENACHIEVE.ENACHIEVE_ADDVALUE)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_ADDVALUE_HAS_NO_SINGLE_WAR);
                return;
            }

            if (lWarID == 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_VALUE_HAS_NO_SOURCE);
                return;
            }

            UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_WAR);
            ButtonType.bIsRequestSelf = LogicDataCenter.playerSystemDataManager.bRequestSelf;
            ButtonType.nRequestPDBID = LogicDataCenter.playerSystemDataManager.nRequestPDBID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, ButtonType);

            WarStatisticViewSingleWarData data = new WarStatisticViewSingleWarData();
            data.msgID = (int)WndMsgID.WND_MSG_PLAYERWARSTATISTIC_VIEWSINGLEWAR;
            data.nEntryType = (int)ViewSingleWarType.VSWT_HONORWND;
            data.lWarID = lWarID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_VIEWSINGLEWAR, data);
        }

        private Sprite GetHeroIcon(int nIconID)
        {
            return USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_DynamicState_Moba, WndID.WND_ID_PLAYER_HONORPAGE, nIconID);
        }

    }

    public class UMediatorHonorPage : UIPopupWnd<UViewHonorPage>
    {
        #region 重载父类方法

        public override WndID GetID()
        {
            return WndID.WND_ID_PLAYER_HONORPAGE;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "HonorPage/HonorPageView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HONORPAGE_OPEN, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_HONORPAGE_OPEN, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_HONORPAGE_OPEN:
                    {                       
                        SetVisible(true);
                        if (m_wndView == null)
                            return;

                        m_wndView.InitHonorToShow(); 
                    }
                    break;
                default:
                    break;
            }
        }
        #endregion
    }

    public class UViewHonorPage : UIBaseWndView
    {
        public Text ViewTitle;
        public Text TotalKillLabel;
        public Text TotalGameLabel;
        public Text KillFirstLabel;
        public Text PentaKillLabel;
        public Text QuadraKillLabel;
        public Text TribleKillLabel;
        public Text GodLikeLabel;
        public Text[] honorNumArray;
        public RectTransform HonorItemFrame;
        public GameObject HonorItemTemplate;
        public GameObject HonorPreBtn;
        public GameObject HonorNextBtn;
        private const int nItemInPage = 12;
        private UHonorItem[] HonorItem;
        private cmd_honor_page_info honorInfo;
        private int[] table;
        private int curHonorPage;
        private int maxHonorPage;
        

        public override bool Init(IUIWnd wnd)
        {
            // 创建HonorItem
            HonorItem = new UHonorItem[nItemInPage];
            for (int i = 0; i < nItemInPage; ++i)
            {
                GameObject HonorItemObj = ResNode.InstantiateRes (HonorItemTemplate);
                if (!HonorItemObj)
                    return false;

                HonorItem[i] = HonorItemObj.GetComponent<UHonorItem>();
                if (!HonorItem[i])
                    return false;

                HonorItemObj.name = "HonorItem" + (i + 1);
                RectTransform rectTrans = HonorItemObj.transform as RectTransform;
                rectTrans.SetParent(HonorItemFrame, false);
            }
            HonorItemTemplate.SetActive(false);

            // label
            ViewTitle.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "title");
            TotalKillLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "totalKill");
            TotalGameLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "totalGame");
            KillFirstLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "killFirst");
            PentaKillLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "pentaKill");
            QuadraKillLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "quadraKill");
            TribleKillLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "tribleKill");
            GodLikeLabel.text = ULocalizationService.Instance.Get("UIView", "HonorPageView", "godLike");

            // 数据
            curHonorPage = 1;
            maxHonorPage = (int)SHOWINDEX.ENACHIEVE_MAX / nItemInPage;
            if ((int)SHOWINDEX.ENACHIEVE_MAX / nItemInPage != 0)
            {
                maxHonorPage++;
            }

            table = new int[(int)SHOWINDEX.ENACHIEVE_MAX];
            
            return base.Init(wnd);
        }

        public void onCloseButton()
        {
            SetVisible(false);
        }

        public void onClickPreButton()
        {
            if (curHonorPage > 1)
            {
                --curHonorPage;
                SetHonorPage(curHonorPage);
                SetPageBtn(curHonorPage);
            }
        }

        public void onClickNextButton()
        {
            if (curHonorPage < maxHonorPage)
            {
                ++curHonorPage;
                SetHonorPage(curHonorPage);
                SetPageBtn(curHonorPage);
            }
        }

        public void InitHonorToShow()
        {
            int nActorID = LogicDataCenter.playerSystemDataManager.nRequestPDBID;
            ActorPlayerInfo info = LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo(nActorID);
            if (info.playerInfo.pDbid == 0)
                return;

            honorInfo.arrGlory = info.playerInfo.nGlory;
            honorInfo.llWarID = info.playerInfo.llWarID;

            for (int i = 0; i < (int)ENACHIEVE.ENACHIEVE_KILLBIGDRAGONNUM; i++)
            {
                if (honorNumArray[i] == null)
                    continue;

                honorNumArray[i].text = honorInfo.arrGlory[i].ToString();
                if (i == (int)ENACHIEVE.ENACHIEVE_MVPNUM)
                {
                    honorNumArray[i].text = honorInfo.arrGlory[i].ToString() + "次";
                }
            }

            // 显示与数据对应
            table[(int)SHOWINDEX.ENACHIEVE_KILLBIGDRAGONNUM] = (int)ENACHIEVE.ENACHIEVE_KILLBIGDRAGONNUM;
            table[(int)SHOWINDEX.ENACHIEVE_CONTINUEWINMAX] = (int)ENACHIEVE.ENACHIEVE_CONTINUEWINMAX;
            table[(int)SHOWINDEX.ENACHIEVE_KILLSMALLDRAGONNUM] = (int)ENACHIEVE.ENACHIEVE_KILLSMALLDRAGONNUM;
            table[(int)SHOWINDEX.ENACHIEVE_DAMAGEMAX] = (int)ENACHIEVE.ENACHIEVE_DAMAGEMAX;
            // 暂时没有ENACHIEVE_GOLDFIRST数据，将其映射为ENACHIEVE_ADDVALUE
            table[(int)SHOWINDEX.ENACHIEVE_GOLDFIRST] = (int)ENACHIEVE.ENACHIEVE_ADDVALUE;
            table[(int)SHOWINDEX.ENACHIEVE_KILLMONSTERMAX] = (int)ENACHIEVE.ENACHIEVE_KILLMONSTERMAX;
            table[(int)SHOWINDEX.ENACHIEVE_ONCEASSISTCOUNT] = (int)ENACHIEVE.ENACHIEVE_ONCEASSISTCOUNT;
            table[(int)SHOWINDEX.ENACHIEVE_KILLMAX] = (int)ENACHIEVE.ENACHIEVE_KILLMAX;
            table[(int)SHOWINDEX.ENACHIEVE_FIRSTKILLTOWERNUM] = (int)ENACHIEVE.ENACHIEVE_FIRSTKILLTOWERNUM;
            table[(int)SHOWINDEX.ENACHIEVE_SCOREMAX] = (int)ENACHIEVE.ENACHIEVE_SCOREMAX;
            table[(int)SHOWINDEX.ENACHIEVE_FIRSTDIENUM] = (int)ENACHIEVE.ENACHIEVE_FIRSTDIENUM;
            table[(int)SHOWINDEX.ENACHIEVE_CONTINUEKILLMAX] = (int)ENACHIEVE.ENACHIEVE_CONTINUEKILLMAX;
            table[(int)SHOWINDEX.ENACHIEVE_ASSISTMAX] = (int)ENACHIEVE.ENACHIEVE_ASSISTMAX;
            table[(int)SHOWINDEX.ENACHIEVE_KILLTOWETMAX] = (int)ENACHIEVE.ENACHIEVE_KILLTOWETMAX;
            table[(int)SHOWINDEX.ENACHIEVE_OUTMAX] = (int)ENACHIEVE.ENACHIEVE_OUTMAX;
            table[(int)SHOWINDEX.ENACHIEVE_TIMEMAX] = (int)ENACHIEVE.ENACHIEVE_TIMEMAX;
            table[(int)SHOWINDEX.ENACHIEVE_MONEYMAX] = (int)ENACHIEVE.ENACHIEVE_MONEYMAX;
            table[(int)SHOWINDEX.ENACHIEVE_TIMEMIN] = (int)ENACHIEVE.ENACHIEVE_TIMEMIN;

            SetHonorPage(curHonorPage);
            SetPageBtn(curHonorPage);
        }

        private void SetHonorPage(int nPageIndex)
        {
            int itemPageOffset = (nPageIndex - 1) * nItemInPage;
            for (int i = 0; i < nItemInPage; i++)
            {
                int nIndex = itemPageOffset + i;
                if (nIndex < (int)SHOWINDEX.ENACHIEVE_MAX)
                {
                    HonorItem[i].SetData(table[nIndex], honorInfo.arrGlory[table[nIndex]], honorInfo.llWarID[table[nIndex]]);
                }
                else
                {
                    HonorItem[i].Clear();
                }
            }
        }

        private void SetPageBtn(int nPageIndex)
        {
            if (nPageIndex > 1)
                HonorPreBtn.SetActive(true);
            else
                HonorPreBtn.SetActive(false);

            if (nPageIndex < maxHonorPage)
                HonorNextBtn.SetActive(true);
            else
                HonorNextBtn.SetActive(false);
        }
    }
}
