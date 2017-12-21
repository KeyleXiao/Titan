/*******************************************************************
** 文件名:	TalentPageWndView.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-4-12
** 版  本:	1.0
** 描  述:	天赋页界面视图模块
		
********************************************************************/

using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using ASpeedGame.Data.TalentPage;
using System.Collections.Generic;
using UnityEngine.EventSystems;
using UnityEngine.Events;
using System;
using GameLogic;
using DataCenter;
using USpeedUI.TalentPagePart;
using UIWidgets;
using USpeedUI.UExtensions;

namespace USpeedUI
{
    public class TalentChangePageModelInfo
    {
        public string szTalentPageName;
        public int nAttackPointAll;
        public int nTrickPointAll;
        public int nHelpPointAll;
        public int nLeftPoints;
    }

    public enum ButtonClickType
    {
        ButtonClickType_Left = 0,
        ButtonClickType_Right,
        ButtonClickType_Middel
    }


    public class TalentPageWndView : UIBaseWndView
    {

        TalentPageWnd talentPageWnd;
        public Button bPageAddButton;
        public Button[] arrayTalentButton;       // 四十个天赋点静态管理
        public List<GameObject> arrayTopButton;  // 20个页数点动态管理
        public Button bSaveButtons;
        public Button bRescoverButton;
        public Button bDeleteButton;
        public Button bResetButton;
        public InputField TalentInput;
        public Text AttackWord;
        public Text TrickWord;
        public Text HelpWord;
        public Text LeftWord;
        public GameObject topTalentPageNumPart;

        public string sOldName;
        public bool bHasLoad = false;
        public int nCurPageIndex;                // 显示的顶部页数index
        public int nMaxSortIndex = 0;
        public double nIntervalDistace = 0;

        public Button roomTalentCloseBtn;
        public GameObject roomBasePic;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);
            if (m_wnd == null)//测试用
            {
                //  talentPageWnd = new TalentPageWnd();
                //talentPageWnd.m_wndView = this;
            }
            else
            {
                talentPageWnd = (TalentPageWnd)m_wnd;
            }
            arrayTopButton = new List<GameObject>(20);
            return true;
        }

        public override void Start()
        {
            base.Start();
            SetAllButtonRightListen();
            SetButtonState(false);

            talentPageWnd.OnRequestTalentPagePoints();//一切准备就绪后请求数据
        }



        ////////////////////////////////////天赋按钮鼠标点击//////////////////////////////////////

        public void SetAllButtonRightListen()
        {
            for (int i = 0; i < this.arrayTalentButton.Length; i++)
            {
                GameObject gameObjectTalent = this.arrayTalentButton[i].gameObject;
                var trigger = gameObjectTalent.AddComponent<EventTrigger>();
                trigger.triggers = new List<EventTrigger.Entry>();
                EventTrigger.Entry entry = new EventTrigger.Entry();

                // 设置 事件类型
                entry.eventID = EventTriggerType.PointerDown;
                // 设置回调函数
                entry.callback = new EventTrigger.TriggerEvent();

                UnityAction<BaseEventData> callback = new UnityAction<BaseEventData>(this.onTalentBtnTriggerClick);
                entry.callback.AddListener(callback);
                // 添加事件触发记录到GameObject的事件触发组件
                trigger.triggers.Add(entry);

                UTooltipTrigger triggerTip = gameObjectTalent.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
                triggerTip.backgroundTint = Color.grey;
                triggerTip.IsUpStay = true; 
            }
        }

        public void onTalentBtnTriggerClick(BaseEventData eventData2)
        {
            PointerEventData eventData = (PointerEventData)eventData2;
            string sTalentId = eventData.pointerEnter.gameObject.name;
            if (sTalentId == "BasePic")
            {
                return;
            }

            int nTalentId = 0;
            int.TryParse(sTalentId,out nTalentId);
            if(nTalentId <= 0)
            {
                return; 
            }

            if (eventData.button == PointerEventData.InputButton.Left)
            {
                onTalentLeftBtnClick(nTalentId);
            }

            else if (eventData.button == PointerEventData.InputButton.Right)
            {
                onTalentRightBtnClick(nTalentId);
            }
        }

        public void onTalentLeftBtnClick(int nTalentId)
        {
            if (nCurPageIndex == 0)
            {
                return;
            }

            talentPageWnd.onTalentPageLeftBtnClickSignal(nTalentId);
        }

        public void onTalentRightBtnClick(int nTalentId)
        {
            if (nCurPageIndex == 0)
            {
                return;
            }

            talentPageWnd.onTalentPageRightBtnClickSignal(nTalentId);
        }



        ////////////////////////////////////顶部显示操作部分//////////////////////////////////////

        public void onAddBtnClick()
        {
            if (this.nMaxSortIndex >= 20)
            {
                return;
            }

            if (nCurPageIndex != 0)
            {
                talentPageWnd.SetSaveSingleTalentPage();                   // 存旧页
            }

            this.nMaxSortIndex++;
            nCurPageIndex = nMaxSortIndex;
            talentPageWnd.SetNewPage(nMaxSortIndex);
            CreateTalentNum();
            talentPageWnd.SetSaveSingleTalentPage();                       // 存新页
            SetSelectTopBtnState(nMaxSortIndex);
        }

        public void CreateAllTopTalentButton(int nTotalPageNum)
        {
            nMaxSortIndex = 0;
            for (int i = 0; i < nTotalPageNum; i++)
            {
                nMaxSortIndex++;
                CreateTalentNum();
            }

            if (this.nMaxSortIndex != 0)
            {
                SetSelectTopBtnState();
            }
        }

        public void CreateTalentNum()
        {
            ResNode uiTopButtonPrefab = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Widgets/BUttons/TopButton");
            GameObject talentNumTop = uiTopButtonPrefab.InstanceMainRes();

            if (talentNumTop == null)
            {
                return;
            }

            talentNumTop.name = nMaxSortIndex.ToString();
            talentNumTop.transform.SetParent(this.topTalentPageNumPart.transform, false);
            talentNumTop.transform.localPosition = new Vector3((float)((nMaxSortIndex - 1) * nIntervalDistace), bPageAddButton.transform.localPosition.y, 0);
            talentNumTop.transform.FindChild("Text").GetComponent<Text>().text = (nMaxSortIndex).ToString();

            var buttonTop = talentNumTop.GetComponent<UButtonGroupItem>();
            buttonTop.Group = topTalentPageNumPart.GetComponent<UButtonGroup>();
            this.arrayTopButton.Add(talentNumTop);
            buttonTop.onSelectedChanged.AddListener(delegate(bool bSeletect)
            {
                if (bSeletect)
                {
                    this.onNumListBtnClick(talentNumTop);
                }
            });

            TalentChangePageModelInfo talentInfo = talentPageWnd.GetAllViewData(this.nMaxSortIndex);
            SetTopPageDescShow(talentInfo, this.nMaxSortIndex);

            talentNumTop.transform.SetAsLastSibling();
            bPageAddButton.transform.localPosition = new Vector3((float)(nMaxSortIndex * nIntervalDistace), bPageAddButton.transform.localPosition.y, 0);//= 
        }

        public void onNumListBtnClick(GameObject gTopButton)          // 切换页的时候弹窗询问。目前暂时直接保存
        {
            int nSortIndex = int.Parse(gTopButton.transform.FindChild("Text").GetComponent<Text>().text);
            ChangeTalentPageSortIndex(nSortIndex);
        }

        public void ChangeTalentPageSortIndex(int nSortIndex)
        {
            if (nSortIndex == nCurPageIndex)
            {
                return;
            }

            talentPageWnd.SetSaveSingleTalentPage();                // 通知model储存数据
            this.nCurPageIndex = nSortIndex;
            talentPageWnd.ChangeTalentPageSortIndex(nSortIndex);    // 处理数据逻辑
        }

        public void removeBtn(int nButtonIndex)                     // 显示的删除
        {
            var temp = arrayTopButton[nButtonIndex - 1].gameObject;
            ResNode.DestroyRes(ref temp);
            arrayTopButton.RemoveAt(nButtonIndex - 1);

            for (int i = 0; i < arrayTopButton.Count; i++)
            {

                arrayTopButton[i].transform.localPosition = new Vector3((float)(i * nIntervalDistace), arrayTopButton[i].transform.localPosition.y, 0);

                arrayTopButton[i].transform.FindChild("Text").GetComponent<Text>().text = (i + 1).ToString();
            }

            if (nCurPageIndex == nMaxSortIndex)
            {
                this.nCurPageIndex--;

            }

            nMaxSortIndex--;

            if (arrayTopButton.Count != 0)
            {
                SetSelectTopBtnState();
            }

            bPageAddButton.transform.localPosition = new Vector3((float)(nMaxSortIndex * nIntervalDistace), bPageAddButton.transform.localPosition.y, 0);
        }


        ///////////////////////////////////按钮的主显示部分//////////////////////////////////////////
        // 切换页，新页显示控制 1、显示点 2、显示三类加点总和、3、显示页名
        public void SetOnePagePointView(TalentChangePageModelInfo talentPageInfo, Dictionary<int, int> dicAllTalentPoint)
        {
            int nButtonIndex = 0;

            foreach (int nTalentId in TalentPage.Instance.allTalentPageTable.Keys)
            {
                nButtonIndex++;
                GameObject talentObject = this.arrayTalentButton[nButtonIndex - 1].gameObject;
                var buttonText = talentObject.transform.FindChild("Text").GetComponent<Text>();

                int nShowPoints = 0;
                if (dicAllTalentPoint.ContainsKey(nTalentId))
                {
                    nShowPoints = dicAllTalentPoint[nTalentId];
                }
                if (buttonText.text[2] == '5')
                {
                    buttonText.text = nShowPoints.ToString() + "/" + "5";
                }
                else
                {
                    buttonText.text = nShowPoints.ToString() + "/" + "1";
                }

                // 置灰和颜色
                if (nShowPoints == 0)
                {
                    talentObject.GetComponent<Image>().AddNewMaterial(EUIShaderType.EST_Gray);
                }
                else
                {
                    talentObject.GetComponent<Image>().ResetToDefaultMaterial();
                }

                // 加tip
                UTooltipTrigger tooltipTrigger = talentObject.GetComponent<UTooltipTrigger>();
                if (tooltipTrigger == null)
                {
                    continue;
                }

                SSchemeTalentConfig talentConfig = TalentPage.Instance.allTalentPageTable[nTalentId] as SSchemeTalentConfig;
                if (talentConfig == null)
                {
                    continue;
                }

                int nTypePoints = GetTypeAllPoints(talentConfig.nTalentType, talentPageInfo);
                string sTalentDesc = GetTalentDesc(nTalentId, nShowPoints, nTypePoints, talentPageInfo.nLeftPoints, talentConfig);
                if (sTalentDesc == null || sTalentDesc == "")
                {
                    continue;
                }

                tooltipTrigger.SetText(UTooltipParamName.BodyText, sTalentDesc);
            }

            this.SetThreeTypeNumShow(talentPageInfo.nAttackPointAll, talentPageInfo.nTrickPointAll, talentPageInfo.nHelpPointAll, talentPageInfo.nLeftPoints);
            this.SetNameShow(talentPageInfo.szTalentPageName);
            SetTopPageDescShow(talentPageInfo);
        }

        public void showTalentButtonPoints(int nShowPoints, int nTalentId, int nTypePoints, int nLeftPoints)
        {
            Button talentButton = this.arrayTalentButton[nTalentId - 1];
            var buttonText = talentButton.transform.FindChild("Text").GetComponent<Text>();

            if (buttonText.text[2] == '5')
            {
                buttonText.text = nShowPoints.ToString() + "/" + "5";
            }
            else
            {
                buttonText.text = nShowPoints.ToString() + "/" + "1";
            }

            if (nShowPoints == 0)
            {
                talentButton.gameObject.GetComponent<Image>().AddNewMaterial(EUIShaderType.EST_Gray);
            }
            else
            {
                talentButton.gameObject.GetComponent<Image>().ResetToDefaultMaterial();
            }
            // tip
            UTooltipTrigger tooltipTrigger = talentButton.gameObject.GetComponent<UTooltipTrigger>();
            if (tooltipTrigger == null)
            {
                return;
            }

            SSchemeTalentConfig talentConfig = TalentPage.Instance.allTalentPageTable[nTalentId] as SSchemeTalentConfig;
            if (talentConfig == null)
            {
                return;
            }

            string sTalentDesc = GetTalentDesc(nTalentId, nShowPoints, nTypePoints, nLeftPoints, talentConfig);
            if (sTalentDesc == null || sTalentDesc == "")
            {
                return;
            }

            tooltipTrigger.SetText(UTooltipParamName.BodyText, sTalentDesc);
        }

        public void SetTypeNumShow(int nTalentType, int nTypePoints, int nLeftPoints)
        {
            LeftWord.text = nLeftPoints.ToString();

            if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_ATTACK)
            {
                this.AttackWord.text = nTypePoints.ToString();
            }
            else if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_TRICK)
            {
                this.TrickWord.text = nTypePoints.ToString();
            }
            else if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_HELP)
            {
                this.HelpWord.text = nTypePoints.ToString();
            }
        }

        public void SetThreeTypeNumShow(int nAttackPoints, int nTrickPoints, int nHelpPoints, int nLeftPoints)
        {
            this.AttackWord.text = nAttackPoints.ToString();
            this.TrickWord.text = nTrickPoints.ToString();
            this.HelpWord.text = nHelpPoints.ToString();
            LeftWord.text = nLeftPoints.ToString();
        }

        public void SetAllTalentLimitDescChange(Dictionary<int, int> dicAllTalentPoint, TalentChangePageModelInfo talentPageInfo)
        {
            int nButtonIndex = 0;
            foreach (int nTalentId in TalentPage.Instance.allTalentPageTable.Keys)
            {
                nButtonIndex++;
                if (dicAllTalentPoint.ContainsKey(nTalentId))
                {
                    if (dicAllTalentPoint[nTalentId] != 0)
                    {
                        continue;
                    }
                }

                GameObject talentObject = this.arrayTalentButton[nButtonIndex - 1].gameObject;

                SSchemeTalentConfig talentConfig = TalentPage.Instance.allTalentPageTable[nTalentId] as SSchemeTalentConfig;
                if (talentConfig == null)
                {
                    continue;
                }

                int nTypePoints = GetTypeAllPoints(talentConfig.nTalentType, talentPageInfo);
                if (talentConfig.nTalentAllowNum < nTypePoints)
                {
                    continue;
                }

                // 加tip
                UTooltipTrigger tooltipTrigger = talentObject.GetComponent<UTooltipTrigger>();
                if (tooltipTrigger == null)
                {
                    continue;
                }

                string sTalentDesc = GetTalentDesc(nTalentId, 0, nTypePoints, talentPageInfo.nLeftPoints, talentConfig);
                if (sTalentDesc == null || sTalentDesc == "")
                {
                    continue;
                }

                tooltipTrigger.SetText(UTooltipParamName.BodyText, sTalentDesc);
            }
        }

        public void SetNextTalentLimitDescChange(int nTalentId, int nTypePoints, int nLeftPoints)
        {
            SSchemeTalentConfig talentConfig = TalentPage.Instance.allTalentPageTable[nTalentId] as SSchemeTalentConfig;
            if (talentConfig == null)
            {
                return;
            }

            if (talentConfig.nTalentLevel == (int)TalentLevel.ENUM_TalentLevel_Max - 1)
            {
                return;
            }

            int nNextLevel = talentConfig.nTalentLevel;
            for (int j = 0; j < TalentPage.DefaultSameLevelCount; j++)
            {
                int nNextLevelTalentId = TalentPage.Instance.aTalentArray[talentConfig.nTalentType - 1, nNextLevel, j];
                if (nNextLevelTalentId == 0)
                {
                    continue;
                }

                GameObject talentObject = this.arrayTalentButton[nNextLevelTalentId - 1].gameObject;

                SSchemeTalentConfig talentNextConfig = TalentPage.Instance.allTalentPageTable[nNextLevelTalentId] as SSchemeTalentConfig;
                if (talentConfig == null)
                {
                    continue;
                }

                if (talentNextConfig.nTalentAllowNum < nTypePoints)
                {
                    continue;
                }
                // 加tip
                UTooltipTrigger tooltipTrigger = talentObject.GetComponent<UTooltipTrigger>();
                if (tooltipTrigger == null)
                {
                    continue;
                }

                string sTalentDesc = GetTalentDesc(nNextLevelTalentId, 0, nTypePoints, nLeftPoints, talentNextConfig);
                if (sTalentDesc == null || sTalentDesc == "")
                {
                    continue;
                }

                tooltipTrigger.SetText(UTooltipParamName.BodyText, sTalentDesc);
            }
        }

        public void SetTopPageDescShow(TalentChangePageModelInfo talentInfo, int nPageIndex = -1)
        {
            if (nPageIndex == -1)
            {
                nPageIndex = this.nCurPageIndex;
            }

            if (nPageIndex <= 0 || nPageIndex > this.arrayTopButton.Count)
            {
                return;
            }

            UTooltipTrigger pageTooltipTrigger = this.arrayTopButton[nPageIndex - 1].GetComponent<UTooltipTrigger>();
            if (pageTooltipTrigger == null)
            {
                pageTooltipTrigger = this.arrayTopButton[nPageIndex - 1].AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
                pageTooltipTrigger.backgroundTint = Color.grey;
            }

            string sTalentDesc = LogicDataCenter.talentPageDataManager.uCurTalentPageData.sTempName;
            pageTooltipTrigger.SetText(UTooltipParamName.BodyText, " " + talentInfo.szTalentPageName + "\n" +"    " +talentInfo.nAttackPointAll + "  " + talentInfo.nTrickPointAll + "  " + talentInfo.nHelpPointAll);
        }

        ///////////////////////////////////保存恢复等按钮监听///////////////////////////////////////

        public void onSaveBtnClick()
        {
            if (nCurPageIndex == 0)
            {
                return;
            }

            this.talentPageWnd.SetSaveSingleTalentPage();
        }

        public void onRescoverBtnClick()
        {
            // 重置以后名字返回初始
            if (nCurPageIndex == 0)
            {
                return;
            }

            talentPageWnd.SetRescoverCurPage();
        }

        public void onDeleteBtnClick()
        {
            if (this.nMaxSortIndex <= 1)
            {
                return;
            }

            removeBtn(nCurPageIndex);
            talentPageWnd.SetDeleteCurPage();
        }

        public void onResetBtnClick()
        {
            if (nCurPageIndex == 0)
            {
                return;
            }

            talentPageWnd.SetResetCurPage();
        }

        public void SetNameShow(string sName)
        {
            sOldName = sName;
            TalentInput.text = sName;
        }

        public void onTextEndEdit()
        {
            // 本地存一份数据
            if (nCurPageIndex == 0)
            {
                return;
            }

            if (sOldName != this.TalentInput.text)
            {
                sOldName = this.TalentInput.text;
                talentPageWnd.SetPageName(this.TalentInput.text);
            }
        }

        //////////////////////////////////////////////////////////////////////////

        public int GetTypeAllPoints(int nTalentType, TalentChangePageModelInfo talentPageInfo)
        {
            int nTypePoints = 0;
            if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_ATTACK)
            {
                nTypePoints = talentPageInfo.nAttackPointAll;
            }
            else if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_TRICK)
            {
                nTypePoints = talentPageInfo.nTrickPointAll;
            }
            else if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_HELP)
            {
                nTypePoints = talentPageInfo.nHelpPointAll;
            }

            return nTypePoints;
        }

        public string GetTypeString(int nTalentType)
        {
            string sTypeName = "";
            if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_ATTACK)
            {
                sTypeName = "凶猛";
            }
            else if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_TRICK)
            {
                sTypeName = "诡诈";
            }
            else if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_HELP)
            {
                sTypeName = "坚决";
            }

            return sTypeName;
        }

        public string GetLimitTalentDesc(int nTalentAllowNum, int nTalentType, int nTypePoints, int nLeftPoints)
        {
            if (nLeftPoints == 0 || nTypePoints < nTalentAllowNum)
            {
                return "<color=red>需要在" + GetTypeString(nTalentType) + "至少分配" + nTalentAllowNum + "点</color>\n";
            }
            else
            {
                return "";
            }
        }

        public string GetTalentDesc(int nTalentId, int nShowPoints, int nTypePoints, int nLeftPoints, SSchemeTalentConfig talentConfig)
        {

            int nEffectId = 0;
            int nNextEffectId = 0;
            string sTalentName = talentConfig.szTalentName + "\n";
            string sLimitDesc = "";
            string sLevelDesc = "";
            string sCurTalentDesc = "";
            string sNextTalentDesc = "";

            if (talentConfig.szTalentArray == "")
            {
                return null;
            }

            string[] arrayEffectId = talentConfig.szTalentArray.Split(';');
            if (arrayEffectId.Length < nShowPoints)
            {
                return null;
            }

            if (nShowPoints == 0)
            {
                sLimitDesc = GetLimitTalentDesc(talentConfig.nTalentAllowNum, talentConfig.nTalentType, nTypePoints, nLeftPoints);
            }

            if (nShowPoints != 0)
            {
                nEffectId = int.Parse(arrayEffectId[nShowPoints - 1]);
                SSchemeTalentEffectConfig talentEffectConfig = TalentPageEffect.Instance.allTalentEffectTable[nEffectId] as SSchemeTalentEffectConfig;
                if (talentEffectConfig == null)
                {
                    return null;
                }

                sCurTalentDesc = talentEffectConfig.szTalentDESC;
                if (nShowPoints != talentConfig.nMaxAddNum)
                {
                    sCurTalentDesc += "\n";
                }
            }

            if (nShowPoints != talentConfig.nMaxAddNum)
            {
                nNextEffectId = int.Parse(arrayEffectId[nShowPoints]);
                SSchemeTalentEffectConfig talentEffectNextConfig = TalentPageEffect.Instance.allTalentEffectTable[nNextEffectId] as SSchemeTalentEffectConfig;
                if (talentEffectNextConfig == null)
                {
                    return null;
                }

                if (nShowPoints == 0)
                {
                    sNextTalentDesc = talentEffectNextConfig.szTalentDESC;
                }
                else
                {
                    sNextTalentDesc = "下一級：" + talentEffectNextConfig.szTalentDESC;
                }
            }

            if (talentConfig.nTalentLevel % 2 == 0)
            {
                sLevelDesc = "等级;" + nShowPoints + "/1" + "\n";
            }
            else
            {
                sLevelDesc = "等级;" + nShowPoints + "/5" + "\n";
            }

            return sTalentName + sLevelDesc + sLimitDesc + sCurTalentDesc + sNextTalentDesc;
        }

        public void SetButtonState(bool bChange)
        {
            if (bChange)
            {

                this.bSaveButtons.gameObject.GetComponent<Image>().ResetToDefaultMaterial();
                this.bRescoverButton.gameObject.GetComponent<Image>().ResetToDefaultMaterial();
            }
            else
            {
                this.bRescoverButton.gameObject.GetComponent<Image>().AddNewMaterial(EUIShaderType.EST_Gray);
                this.bSaveButtons.gameObject.GetComponent<Image>().AddNewMaterial(EUIShaderType.EST_Gray);

            }

            this.bSaveButtons.enabled = bChange;
            this.bRescoverButton.enabled = bChange;
        }

       public void EnterRoomTalentState() // 目前进入房间打开天赋先只加一个关闭按钮
       {
           SetTalentObjectsActive(true);
       }

       public void onRoomCloseTalentBtnClick()
       {
           talentPageWnd.LeaveRoomTalentState();
       }

        public void SetTalentObjectsActive(bool bActive)
        {
            this.roomTalentCloseBtn.gameObject.SetActive(bActive);
            roomBasePic.gameObject.SetActive(bActive);
        }

        public void SetSelectTopBtnState(int nPageIndex = -1)
        {
            if (nPageIndex==-1)
            {
                nPageIndex = this.nCurPageIndex;
            }

            if (nPageIndex <= 0 || nPageIndex > this.arrayTopButton.Count)
            {
                return;
            }

            this.arrayTopButton[nPageIndex - 1].GetComponent<UButtonGroupItem>().Selected = true;
        }
    }
}
