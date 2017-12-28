/*******************************************************************
** 文件名:	LoadingWarWndView.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-6-21
** 版  本:	1.0
** 描  述:	载入战场等候界面
		
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
using USpeedUI.LoadingWarPart;
using UIWidgets;
using USpeedUI.UExtensions;
using ASpeedGame.Data.LoadingWarTipConfig;
using USpeedUI.UWidgets;
using Match_ManagerDef;
using U3D_Render.Common;
using ASpeedGame.Data.HeroXPskillConfig;

namespace USpeedUI
{
    enum CAMP_NUM
    {
        EACH_CAMP_MAX_PLAYER = 5
    }

    public class LoadingPlayerItem : MonoBehaviour
    {
        public Image HeroIcon;
        public Image XpIcon;
        public Image SummerIcon;
        public Image HeadIcon;
        public Text VocationText;
        public Text PlayerNameText;
        public Text ReadText;
        public Text ReadCountText;

        private int m_nNewLoadProgress = 0;
        private float m_fCurLoadProgress = 0;
        private bool m_bUpdate = false;

        public void SetItemData(cmd_match_load_scene_playerinfo data)
        {
            int nSkinIconID = 0;
            SkinInfoScheme skinConfig = new SkinInfoScheme();
            if (GameLogicAPI.GetHeroSkinInfo(data.nSkinID, ref skinConfig))
                nSkinIconID = skinConfig.nSkinIconID;
            Sprite skinSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_LOADINGWARVIEW, nSkinIconID, 1, data.nHeroID);
            if (skinSprite == null)
                skinSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_LOADINGWARVIEW, 1, 1, data.nHeroID);
            HeroIcon.sprite = skinSprite;

            SSchemeHeroXPskillConfig xpConfig = HeroXPskillConfig.Instance.GetHeroXPskillConfig(data.nHeroID, data.nXPSkillID);
            if (xpConfig != null)
            {
                XpIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_LOADINGWARVIEW, 1, xpConfig.nIconID);
                XpIcon.gameObject.SetActive(true);
            }
            else
            {
                XpIcon.gameObject.SetActive(false);
            }
            

            SummerIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_LOADINGWARVIEW, 1, data.nSupportSkill[0]);
            SummerIcon.gameObject.SetActive(SummerIcon.sprite != null);

            //HeadIcon
            int nSex = data.cmdPlayerVsDetails.nSex;
            if (nSex < (int)PERSON_SEX.SEX_MAX && nSex >= (int)PERSON_SEX.SEX_MALE)
            {
                HeadIcon.gameObject.SetActive(true);
                HeadIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, WndID.WND_ID_RANK_INVITE, 1, nSex + 1);
            }
            else
            {
                HeadIcon.gameObject.SetActive(false);
            }

            VocationText.text = data.szVocationName;
            VocationText.color = data.nCamp == data.nSelfCamp ? UDefines.CommomColor(ECommonColor.ECC_Blue1) : UDefines.CommomColor(ECommonColor.ECC_Red1);
            // Ob位加载界面，0阵营蓝色，1阵营红色
            if (data.nSelfCamp < 0)
            {
                VocationText.color = data.nCamp == 0 ? UDefines.CommomColor(ECommonColor.ECC_Blue1) : UDefines.CommomColor(ECommonColor.ECC_Red1);
            }

            PlayerNameText.text = data.szPlayerName;
            PlayerNameText.color = data.nSelfInfo > 0 ? UDefines.CommomColor(ECommonColor.ECC_Blue1) : UDefines.CommomColor(ECommonColor.ECC_White);

            if (data.nIsComputer > 0)
            {
                ReadText.text = "读取完毕";
                ReadCountText.text = "100%";
                m_bUpdate = false;
            }
            else
            {
                ReadText.text = "读取中";
                ReadCountText.text = "0%";
                m_bUpdate = true;
            }

            this.gameObject.SetActive(true);
        }

        public void SetItemClear()
        {
            this.gameObject.SetActive(false);
        }

        public void SetLoadingRate(int nRate)
        {
            m_nNewLoadProgress = nRate;
        }

        public virtual void Update()
        {
            if (!m_bUpdate)
            {
                return;
            }

            UpdateLoadingRate();
        }

        private void UpdateLoadingRate()
        {
            int deltaLoadProgress = (m_nNewLoadProgress - (int)m_fCurLoadProgress);

            if (deltaLoadProgress > 0)
            {
                m_fCurLoadProgress = m_fCurLoadProgress + (0.05f * deltaLoadProgress);
            }
            else if (m_fCurLoadProgress > 99)
            {
                m_fCurLoadProgress = 100;
                ReadText.text = "读取完毕";
                m_bUpdate = false;
            }

            ReadCountText.text = string.Format("{0}%", (int)m_fCurLoadProgress);
        }
    }

    public class LoadingWarNormalFrame : UCustomUIBehaviour
    {
        public RectTransform UpPlayerFrame;
        public RectTransform DownPlayerFrame;
        public GameObject PlayerTemplate;
        private LoadingPlayerItem[] m_upPlayerItems;
        private LoadingPlayerItem[] m_downPlayerItems;

        public Text TipDesc;

        public void Init()
        {
            InitFrameView(UpPlayerFrame, ref m_upPlayerItems);
            InitFrameView(DownPlayerFrame, ref m_downPlayerItems);
        }

        private void InitFrameView(RectTransform rect, ref LoadingPlayerItem[] itemContainer)
        {
            int nCount = rect.childCount;
            for (int i = nCount - 1; i >= 0; --i)
            {
                Transform childTrans = rect.GetChild(i);
                if (childTrans)
                {
                    var temp = childTrans.gameObject;
                    ResNode.DestroyRes(ref temp);
                }
            }

            PlayerTemplate.SetActive(true);
            itemContainer = new LoadingPlayerItem[(int)CAMP_NUM.EACH_CAMP_MAX_PLAYER];
            for (int i = 0; i < (int)CAMP_NUM.EACH_CAMP_MAX_PLAYER; ++i)
            {
                GameObject playerItemGo = ResNode.InstantiateRes(PlayerTemplate);
                if (!playerItemGo)
                    return;

                playerItemGo.name = "PlayerItem" + (i + 1);
                RectTransform rectTrans = playerItemGo.transform as RectTransform;
                rectTrans.SetParent(rect, false);

                itemContainer[i] = playerItemGo.GetComponent<LoadingPlayerItem>();
                if (!itemContainer[i])
                    return;
                itemContainer[i].SetItemClear();
            }
            PlayerTemplate.SetActive(false);
        }

        public void SetData(int nSelfCampIndex, int[] nCampCount, List<cmd_match_load_scene_playerinfo> playerInfoList)
        {
            // 上阵营
            for (int i = 0; i < nCampCount[0]; i++)
            {
                m_upPlayerItems[i].SetItemData(playerInfoList[i]);
            }
            for (int i = nCampCount[0]; i < (int)CAMP_NUM.EACH_CAMP_MAX_PLAYER; i++)
            {
                m_upPlayerItems[i].SetItemClear();
            }
            // 下阵营
            for (int i = 0; i < nCampCount[1]; i++)
            {
                m_downPlayerItems[i].SetItemData(playerInfoList[nCampCount[0] + i]);
            }
            for (int i = nCampCount[1]; i < (int)CAMP_NUM.EACH_CAMP_MAX_PLAYER; i++)
            {
                m_downPlayerItems[i].SetItemClear();
            }
            // 提示
            UpdateRandLoadingTip();

            this.gameObject.SetActive(true);
        }

        public void SetLoadingRate(int nIndex, int nRate)
        {
            LoadingPlayerItem updateItem = null;
            if (nIndex < (int)CAMP_NUM.EACH_CAMP_MAX_PLAYER)
            {
                updateItem = m_upPlayerItems[nIndex];
            }
            else if (nIndex < 2 * (int)CAMP_NUM.EACH_CAMP_MAX_PLAYER)
            {
                updateItem = m_downPlayerItems[nIndex - (int)CAMP_NUM.EACH_CAMP_MAX_PLAYER];
            }
            if (updateItem == null)
                return;

            updateItem.SetLoadingRate(nRate);
        }

        public void OnClickRandLoadingTip()
        {
            UpdateRandLoadingTip();
        }

        private void UpdateRandLoadingTip()
        {
            SSchemeLoadingWarTip tip = LoadingWarTipConfig.Instance.GetRandTipConfig();
            if (tip != null)
            {
                string strTemp = tip.sTipContent;
                UBB.toHtml(ref strTemp);
                TipDesc.text = strTemp;
            }     
        }
    }

    public class LoadingWarMultiCampFrame : UCustomUIBehaviour
    {
        public LoadingPlayerItem[] PlayerItems;

        public Image TenNumIcon;
        public Image OneNumIcon;
        public Sprite[] NumIcons;

        public void Init()
        {
            foreach (var item in PlayerItems)
            {
                item.SetItemClear();
            }
        }

        public void SetData(int nSelfCampIndex, int[] nCampCount, List<cmd_match_load_scene_playerinfo> playerInfoList)
        {
            // 玩家
            for (int i = 0; i < playerInfoList.Count; i++)
            {
                PlayerItems[i].SetItemData(playerInfoList[i]);
            }
            for (int j = playerInfoList.Count; j < PlayerItems.Length; j++)
            {
                PlayerItems[j].SetItemClear();
            }

            // 敌人数量
            int nEnemyCount = 0;
            for (int i = 0; i < nCampCount.Length; i++)
            {
                if (i == nSelfCampIndex)
                    continue;

                nEnemyCount += nCampCount[i];
            }
            TenNumIcon.sprite = NumIcons[nEnemyCount / 10];
            OneNumIcon.sprite = NumIcons[nEnemyCount % 10];
            TenNumIcon.gameObject.SetActive(nEnemyCount / 10 != 0);

            this.gameObject.SetActive(true);
        }

        public void SetLoadingRate(int nIndex, int nRate)
        {
            if (nIndex < 0 || nIndex >= PlayerItems.Length)
                return;

            PlayerItems[nIndex].SetLoadingRate(nRate);
        }
    }

    public class LoadingWarWndView : UIBaseWndView
    {
        public LoadingWarNormalFrame NormalFrame;
        public LoadingWarMultiCampFrame MultiCampFrame;

        public Image BackgroundImg;

        private int m_nMatchType;
        private Dictionary<int, int> m_dicPDBID2SlotID;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            NormalFrame.Init();
            MultiCampFrame.Init();
            NormalFrame.gameObject.SetActive(false);
            MultiCampFrame.gameObject.SetActive(false);

            m_nMatchType = 0;
            m_dicPDBID2SlotID = new Dictionary<int, int>();

            return true;
        }

        public void SetData(int nMatchType, int nMapID, int nSelfCampIndex, int[] nCampCount, List<cmd_match_load_scene_playerinfo> playerInfoList)
        {
            m_nMatchType = nMatchType;

            if (nMatchType == (int)EMMatchType.MatchType_MultiCampMatch)
            {
                MultiCampFrame.SetData(nSelfCampIndex, nCampCount, playerInfoList);

                for (int i = 0; i < playerInfoList.Count; i++)
                {
                    m_dicPDBID2SlotID[(int)playerInfoList[i].idActor] = i;
                }
            }
            else
            {
                NormalFrame.SetData(nSelfCampIndex, nCampCount, playerInfoList);

                for (int i = 0; i < nCampCount[0]; i++)
                {
                    m_dicPDBID2SlotID[(int)playerInfoList[i].idActor] = i;
                }
                for (int i = 0; i < nCampCount[1]; i++)
                {
                    m_dicPDBID2SlotID[(int)playerInfoList[nCampCount[0] + i].idActor] = (int)CAMP_NUM.EACH_CAMP_MAX_PLAYER + i;
                }
            }

            SetBackgroundImage(nMapID);
        }

        private void SetBackgroundImage(int _nMapID)
        {
            //有需要的话写到MathRoom配置里面 176-1 177-0
            int nId = 0;
            if (_nMapID == 176)
            {
                nId = 1;
            }
            else if (_nMapID == 177)
            {
                nId = 0;
            }
            Sprite sp = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_FullScreen, m_wnd.GetID(), nId);
            if (sp != null && sp != USpriteManager.Instance.DefaultSprite)
                BackgroundImg.sprite = sp;
            BackgroundImg.color = Color.white;
        }

        public void SetLoadingRate(cmd_load_scene_rate data)
        {
            if (!m_dicPDBID2SlotID.ContainsKey((int)data.idActor))
                return;

            if (m_nMatchType == (int)EMMatchType.MatchType_MultiCampMatch)
            {
                MultiCampFrame.SetLoadingRate(m_dicPDBID2SlotID[(int)data.idActor], data.nRate);
            }
            else
            {
                NormalFrame.SetLoadingRate(m_dicPDBID2SlotID[(int)data.idActor], data.nRate);
            }
        }
    }
}