/*******************************************************************
** 文件名:	WarReliveTimeWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	2016-6-28
** 版  本:	1.0
** 描  述:	龙珠界面逻辑模块(接发消息)+视图模块(显示数据)	
********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using GameLogic;

namespace USpeedUI.DragonBall
{
    public class UDragonBallWnd : UIPopupWnd<UDragonBallWndView>
    {
        private string m_strRoleStartName = "taitan1001";
        private string m_strRoleEndName = "taitan2000";

        public override WndID GetID()
        {
            return WndID.WND_ID_DRAGONBALL;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        public override string GetPrefabPath()
        {
            return "DragonBall/DragonBallView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);

            UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.DragonBall, KeyStatus.Down, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);

            UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.DragonBall, KeyStatus.Down, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgdata)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        this.LoadView();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                case WndMsgID.WND_MSG_COMMON_WAREND:
                    {
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

            if (index == FuntionShortCutSlotIndex.DragonBall && status == KeyStatus.Down)
            {
                if (IsForbitBall())
                    return false;

                if (!m_wndView.IsVisible())
                {
                    m_wndView.SetVisible(true);
                }
                else
                {                  
                    m_wndView.SetVisible(false);
                    m_wndView.ChangeDragonBallToFirstPage();
                }
            }
                
            return true;
        }

        // 是否禁止使用龙珠功能
        private bool IsForbitBall()
        {
            try
            {
                // 是否打开七龙珠功能
                if (!GameLogicAPI.isOpenSupperStone())
                {
                    return true;
                }

                // 硬编码定死策划数值账号段
                U3D_Render.EntityView ev = EntityFactory.MainHeroView;
                if (ev != null && ev.Property != null)
                {
                    string strName = Config.AccountName;
                    // 外部及策划数值测试账号，禁止使用七龙珠
                    if (strName.ToLower().StartsWith("g") || string.Compare(strName, m_strRoleStartName, true) >= 0 && string.Compare(strName, m_strRoleEndName, true) <= 0)
                    {
                        return true;
                    }
                }
            }
            catch
            {
            }
            return false;
        }
    }

    public class UDragonBallWndView : UIBaseWndView
    {
        public Text SecondPageTitle;
        public GameObject FirstPage;
        public GameObject SecondPage;
        public GameObject[] gStaticText;
        public GameObject[] gTextContent;
        public InputField[] tStaticText;
        public InputField[] tTextContent;

        private string strFuncName;
        private int nStrParamCount;
        private int nStrParamCountMax = 7;

        public override bool Init(IUIWnd wnd)
        {
            ChangeDragonBallToFirstPage();

            return base.Init(wnd);
        }

        public void DragonBallCallCustomMonster()
        {
            strFuncName = "SupperStone_CallCustomMonster";
            nStrParamCount = 7;
            SetTitleLabel("创建自定义怪物:(某坐标填0表示用玩家当前的相对应坐标)");
            SetDefaultLabelTextValue();
            SetDefaultTextValue();
            ShowDragonBallSecondPage();
        }

        public void DragonBallAddExperience()
        {
            strFuncName = "SupperStone_AddExperience";
            nStrParamCount = 1;
            SetTitleLabel("请输入要设置的经验值:");
            SetDefaultLabelTextValue("经验:");
            SetDefaultTextValue("9999");
            ShowDragonBallSecondPage();
        }

        public void DragonBallSetLevel()
        {
            strFuncName = "SupperStone_SetLevel";
            nStrParamCount = 1;
            SetTitleLabel("请输入要设置的等级值:【参数1:等级值】");
            SetDefaultLabelTextValue("等级:");
            SetDefaultTextValue("7");
            ShowDragonBallSecondPage();
        }

        public void DragonBallChangeHeroSkin()
        {
            strFuncName = "SupperStone_ChangeHeroSkin";
            nStrParamCount = 2;
            SetTitleLabel("切换英雄ID和皮肤(0为默认皮肤)");
            SetDefaultLabelTextValue("HeroID:", "SkinID:");
            SetDefaultTextValue("0", "0");
            ShowDragonBallSecondPage();
        }

        public void DragonBallRetrieve()
        {
            strFuncName = "SupperStone_Retrieve";
            nStrParamCount = 1;
            SetTitleLabel("全面恢复 UID为0表示恢复玩家本身");
            SetDefaultLabelTextValue("UID:");
            SetDefaultTextValue("0");
            ShowDragonBallSecondPage();
        }
        public void DragonBallSetBuff()
        {
            strFuncName = "SupperStone_SetBuff";
            nStrParamCount = 3;
            SetTitleLabel("设置buff BID为buff的id id从1002开始 BIV 为buff的等级 UID为0取消完所有buff 对应BIV为0取消该buff");
            SetDefaultLabelTextValue("UID:", "BUFF_ID:", "BUFF_LEVEL:");
            SetDefaultTextValue("0", "1002", "1");
            ShowDragonBallSecondPage();
        }
        public void DragonBallEndTheWar()
        {
            strFuncName = "SupperStone_EndtheWar";
            nStrParamCount = 1;
            SetTitleLabel("SID为场景ID，0 位本场景ID 确定结束战场请点击OK");
            SetDefaultLabelTextValue("SID:");
            SetDefaultTextValue("0");
            ShowDragonBallSecondPage();
        }

        public void DragonBallShowPosition()
        {
            strFuncName = "SupperStone_ShowPosition";
            nStrParamCount = 1;
            SetTitleLabel("输入ID值 ID=0为自己 点击OK 获得客户端与服务器端人物坐标");
            SetDefaultLabelTextValue("ID:");
            SetDefaultTextValue("0");
            ShowDragonBallSecondPage();
        }

        public void DragonBallAddTalentPoint()
        {
            strFuncName = "SupperStone_AddTalentPoint";
            nStrParamCount = 1;
            SetTitleLabel("请输入要增加的天赋点:");
            SetDefaultLabelTextValue("天赋点:");
            SetDefaultTextValue("9999");
            ShowDragonBallSecondPage();
        }

        public void DragonBallRelive()
        {
            strFuncName = "SupperStone_Relive";
            nStrParamCount = 0;
            SetTitleLabel("复活自身");
            ShowDragonBallSecondPage();
        }

        public void DragonBallClearCD()
        {
            strFuncName = "SupperStone_ClearCD";
            nStrParamCount = 0;
            SetTitleLabel("清除所有技能CD");
            ShowDragonBallSecondPage();
        }

        public void DragonBallAddEnmity()
        {
            strFuncName = "SupperStone_AddEnmity";
            nStrParamCount = 2;
            SetTitleLabel("增加目标仇恨 默认增加一万点仇恨");
            SetDefaultLabelTextValue("UID:", "AddValue:");
            SetDefaultTextValue("0", "10000");
            ShowDragonBallSecondPage();
        }

        public void DragonBallAddNewHero()
        {
            strFuncName = "SupperStone_AddNewHero";
            nStrParamCount = 1;
            SetTitleLabel("增加/删除英雄（正数为添加，负数为删除，注：删除英雄需要重登陆生效）");
            SetDefaultLabelTextValue("英雄ID:");
            SetDefaultTextValue("0");
            ShowDragonBallSecondPage();
            
        }

        public void DragonBallAddGold()
        {
            strFuncName = "SupperStone_AddGold";
            nStrParamCount = 2;
            SetTitleLabel("增加金币,7代表金币,8代表绑金");
            SetDefaultLabelTextValue("金币类型:", "金币数量:");
            SetDefaultTextValue();
            ShowDragonBallSecondPage();
        }

        public void DragonBallAddCard()
        {
            strFuncName = "SupperStone_AddCard";
            nStrParamCount = 2;
            SetTitleLabel("增加卡牌");
            SetDefaultLabelTextValue("卡牌id:", "卡牌数量:");
            SetDefaultTextValue();
            ShowDragonBallSecondPage();
        }

        public void DragonBallAddPlayerExp()
        {
            strFuncName = "SupperStone_AddPlayerExp";
            nStrParamCount = 1;
            SetTitleLabel("增加玩家经验");
            SetDefaultLabelTextValue("经验值:");
            SetDefaultTextValue();
            ShowDragonBallSecondPage();
        }

        public void DragonBallAddStaticHeroExp()
        {
            strFuncName = "SupperStone_AddStaticHeroExp";
            nStrParamCount = 2;
            SetTitleLabel("增加战场外玩家英雄的经验");
            SetDefaultLabelTextValue("英雄id:", "经验值:");
            SetDefaultTextValue();
            ShowDragonBallSecondPage();
        }

        public void DragonBallSetStarLv()
        {
            strFuncName = "SupperStone_SetStarLv";
            nStrParamCount = 2;
            SetTitleLabel("设置英雄星级");
            SetDefaultLabelTextValue("英雄id", "英雄星级");
            SetDefaultTextValue();
            ShowDragonBallSecondPage();
        }

        public void DragonBallSetHeroOrPlayerLv()
        {
            strFuncName = "SupperStone_SetHeroOrPlayerLv";
            nStrParamCount = 3;
            SetTitleLabel("设置英雄或者玩家等级");
            SetDefaultLabelTextValue("英雄id:", "英雄等级:", "玩家等级:");
            SetDefaultTextValue("0", "0", "0");
            ShowDragonBallSecondPage();
        }

        public void DragonBallCreateKin()
        {
            strFuncName = "SupperStone_CreateKin";
            nStrParamCount = 1;
            SetTitleLabel("战队名字");
            SetDefaultLabelTextValue("输入战队名字:");
            SetDefaultTextValue("KinName");
            ShowDragonBallSecondPage();
        }

        public void DragonBallJoinKin()
        {
            strFuncName = "SupperStone_JoinKin";
            nStrParamCount = 1;
            SetTitleLabel("申请加入");
            SetDefaultLabelTextValue("战队ID:");
            SetDefaultTextValue("ID");
            ShowDragonBallSecondPage();
        }

        public void DragonBallDismissKin()
        {
            strFuncName = "SupperStone_DismissKin";
            nStrParamCount = 1;
            SetTitleLabel("解散战队");
            SetDefaultLabelTextValue("您将解散战队！");
            SetDefaultTextValue();
            ShowDragonBallSecondPage();
        }

        public void DragonBallQuitKin()
        {
            strFuncName = "SupperStone_QuitKin";
            nStrParamCount = 1;
            SetTitleLabel("退出战队");
            SetDefaultLabelTextValue("您将退出战队！");
            SetDefaultTextValue();
            ShowDragonBallSecondPage();
        }

        public void DragonBallAddChest()
        {
            strFuncName = "SupperStone_AddChest";
            nStrParamCount = 2;
            SetTitleLabel("增加宝箱数量");
            SetDefaultLabelTextValue("ID:", "数量:");
            SetDefaultTextValue("0", "1");
            ShowDragonBallSecondPage();
        }

        public void DragonBallSetHeroMatchTypeRank()
        {
            strFuncName = "SupperStone_SetHeroMatchTypeRank";
            nStrParamCount = 4;
            SetTitleLabel("设置玩家Rank分数");
            SetDefaultLabelTextValue("PDBID:", "比赛类型:", "分数(+ -)):","隐藏分数(+ -)):");
            SetDefaultTextValue("0", "1", "0","0");
            ShowDragonBallSecondPage();
        }

        public void DragonBallResetMatchTypeRank()
        {
            strFuncName = "SupperStone_ResetMatchTypeRank";
            nStrParamCount = 2;
            SetTitleLabel("重置玩家Rank分数");
            SetDefaultLabelTextValue("PDBID:", "比赛类型:");
            SetDefaultTextValue("0", "1");
            ShowDragonBallSecondPage();
        }

        public void DragonBallSetHeroTalentActive()
        {
            strFuncName = "SupperStone_SetHeroTalentActive";
            nStrParamCount = 2;
            SetTitleLabel("获得场外英雄天赋");
            SetDefaultLabelTextValue("玩家英雄Id:", "玩家天赋Id:");
            SetDefaultTextValue("2", "10201");
            ShowDragonBallSecondPage();
        }

        public void DragonBallSetHeroXPSkill()
        {
            strFuncName = "SupperStone_SetHeroXPSkill";
            nStrParamCount = 2;
            SetTitleLabel("增加英雄xp技能");
            SetDefaultLabelTextValue("玩家英雄Id:", "XP技能Id:");
            SetDefaultTextValue("2", "1074");
            ShowDragonBallSecondPage();
        }

        public void DragonBallSetCrownGemstone()
        {
            strFuncName = "SupperStone_SetCrownGemstone";
            nStrParamCount = 3;
            SetTitleLabel("增加玩家宝石");
            SetDefaultLabelTextValue("宝石Id:", "宝石个数:", "宝石等级");
            SetDefaultTextValue("1", "1", "2");
            ShowDragonBallSecondPage();
        }

        public void DragonBallAddRuneStoreItem()
        {
            strFuncName = "SupperStone_AddRuneStoreItem";
            nStrParamCount = 2;
            SetTitleLabel("增加符文");
            SetDefaultLabelTextValue("符文Id:", "符文个数:");
            SetDefaultTextValue("1", "1");
            ShowDragonBallSecondPage();
        }

        public void DragonBallAddHeroSkin()
        {
            strFuncName = "SupperStone_AddHeroSkin";
            nStrParamCount = 3;
            SetTitleLabel("增加皮肤 时间为天单位 0为永久");
            SetDefaultLabelTextValue("玩家英雄Id:", "皮肤Id:", "时间(天)：");
            SetDefaultTextValue("1", "1","1");
            ShowDragonBallSecondPage();
        }

        public void DragonBallSwitchScene()
        {
            strFuncName = "SupperStone_SwitchScene";
            nStrParamCount = 1;
            SetTitleLabel("切换场景");
            SetDefaultLabelTextValue("目标场景ID");
            SetDefaultTextValue("1");
            ShowDragonBallSecondPage();
        }

        public void DragonBallAcceptTask()
        {
            strFuncName = "SupperStone_AcceptTask";
            nStrParamCount = 1;
            SetTitleLabel("接受任务");
            SetDefaultLabelTextValue("任务ID");
            SetDefaultTextValue("1");
            ShowDragonBallSecondPage();
        }

        public void DragonBallFinishTask()
        {
            strFuncName = "SupperStone_FinishTask";
            nStrParamCount = 1;
            SetTitleLabel("完成任务");
            SetDefaultLabelTextValue("任务ID");
            SetDefaultTextValue("1");
            ShowDragonBallSecondPage();
        }

        public void DragonBallDeleteTask()
        {
            strFuncName = "SupperStone_DeleteTask";
            nStrParamCount = 1;
            SetTitleLabel("删除任务（0:删除所有任务）");
            SetDefaultLabelTextValue("任务ID");
            SetDefaultTextValue("1");
            ShowDragonBallSecondPage();
        }

        public void DragonBallRemoveTaskDayAndWeekCount()
        {
            strFuncName = "SupperStone_RemoveTaskDayAndWeekCount";
            nStrParamCount = 1;
            SetTitleLabel("移除任务日,周次数,0所有任务日,周次数");
            SetDefaultLabelTextValue("任务ID");
            SetDefaultTextValue("1");
            ShowDragonBallSecondPage();
        }

        public void DragonBallSetTaskData()
        {
            strFuncName = "SupperStone_SetTaskData";
            nStrParamCount = 2;
            SetTitleLabel("设置任务数据值");
            SetDefaultLabelTextValue("任务ID:", "任务数据值:");
            SetDefaultTextValue("1", "0");
            ShowDragonBallSecondPage();
        }

        public void DragonBallSendMail()
        {
            strFuncName = "SupperStone_SendMail";
            nStrParamCount = 7;
            SetTitleLabel("设置发送邮件数据:");
            SetDefaultLabelTextValue("附件金币：", "附件经验：", "奖励ID1：", "奖励ID2：", "奖励ID3：", "奖励ID4：", "奖励ID5：");
            SetDefaultTextValue("0", "0", "0", "0", "0", "0", "0");
            ShowDragonBallSecondPage();
        }

        public void DragonBallPunishActor()
        {
            strFuncName = "SupperStone_PunishActor";
            nStrParamCount = 2;
            SetTitleLabel("惩罚类型（添加类型： 1、添加 0、移除）");
            SetDefaultLabelTextValue("添加类型：","类型ID");
            SetDefaultTextValue("1","1");
            ShowDragonBallSecondPage();
        }

        public void DragonBallResetLifeHero()
        {
            strFuncName = "SupperStone_ResetLifeHero";
            nStrParamCount = 0;
            SetTitleLabel("清除本命英雄");
            ShowDragonBallSecondPage();
        }

        public void DragonBallAddLifeHero()
        {
            strFuncName = "SupperStone_AddLifeHero";
            nStrParamCount = 2;
            SetTitleLabel("添加本命英雄:");
            SetDefaultLabelTextValue("英雄ID：", "培养仓ID（0-6）：");
            SetDefaultTextValue("3", "0");
            ShowDragonBallSecondPage();
        }

        public void DragonBallReflushTaskOptCount()
        {
            strFuncName = "SupperStone_ReflushTaskOptCount";
            nStrParamCount = 2;
            SetTitleLabel("添加任务操作次数:");
            SetDefaultLabelTextValue("获取次数：", "发布次数：");
            SetDefaultTextValue("0", "0");
            ShowDragonBallSecondPage();
        }

        public void DragonBallSetGuideStep()
        {
            strFuncName = "SupperStone_SetGuideStep";
            nStrParamCount = 1;
            SetTitleLabel("设置新手引导步骤(0-7)：");
            SetDefaultLabelTextValue("新手引导步骤：");
            SetDefaultTextValue("0");
            ShowDragonBallSecondPage();
        }

        public void DragonBallTransport()
        {
            strFuncName = "SupperStone_Transport";
            nStrParamCount = 3;
            SetTitleLabel("传送目标点：");
            SetDefaultLabelTextValue("x坐标：", "y坐标:", "z坐标:");
            SetDefaultTextValue("0");
            ShowDragonBallSecondPage();
        }
        public void DragonBallCollectHeroGenic()
        {
            strFuncName = "SupperStone_CollectHeroGenic";
            nStrParamCount = 2;
            SetTitleLabel("战场收集英雄基因：");
            SetDefaultLabelTextValue("英雄ID：", "皮肤ID（0取默认）:");
            SetDefaultTextValue("0");
            ShowDragonBallSecondPage();
        }

        public void onOkButtonClick()
        {
            String strParam = strFuncName + "(";
            for (int i = 1; i < nStrParamCount + 1; i++)
            {
                String strTemp = this.gameObject.transform.FindChild("SecondPage").gameObject.transform.FindChild("TextContent_" + i.ToString()).gameObject.GetComponent<InputField>().text;
                if (strTemp == "")
                    return;
                if (i == nStrParamCount)
                    strParam = strParam + strTemp;
                else
                    strParam = strParam + strTemp + ",";                
            }
            strParam += ")";

            IntPtrHelper helper = new IntPtrHelper();
            IntPtr strParam_ptr = helper.Str2Ptr(strParam);
            int eventID = (int)EntityLogicDef.ENTITY_CMD_DRAGONBALL;
            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, eventID, 0, strParam, strParam_ptr, GameLogicAPI.getStringLen(strParam_ptr));
        }

        public void onCancelButtonClick()
        {
            ChangeDragonBallToFirstPage();
        }

        private void SetTitleLabel(string str)
        {
            SecondPageTitle.text = str;
        }

        private void SetDefaultLabelTextValue(String str1 = "ID", String str2 = "x坐标:", String str3 = "y坐标:", String str4 = "z坐标:", String str5 = "数量:", String str6 = "阵营:", String str7 = "等级:")
        {
            tStaticText[0].text = str1;
            tStaticText[1].text = str2;
            tStaticText[2].text = str3;
            tStaticText[3].text = str4;
            tStaticText[4].text = str5;
            tStaticText[5].text = str6;
            tStaticText[6].text = str7;
        }

        private void SetDefaultTextValue(String str1 = "1", String str2 = "0", String str3 = "0", String str4 = "0", String str5 = "1", String str6 = "0", String str7 = "1")
        {
            tTextContent[0].text = str1;
            tTextContent[1].text = str2;
            tTextContent[2].text = str3;
            tTextContent[3].text = str4;
            tTextContent[4].text = str5;
            tTextContent[5].text = str6;
            tTextContent[6].text = str7;
        }

        private void ShowDragonBallSecondPage()
        {
            for (int i = 0; i < nStrParamCount; i++)
            {
                gStaticText[i].SetActive(true);
                gTextContent[i].SetActive(true);
            }
            for (int i = nStrParamCount; i < nStrParamCountMax; i++)
            {
                gStaticText[i].SetActive(false);
                gTextContent[i].SetActive(false);
            }

            SecondPage.SetActive(true);
            FirstPage.SetActive(false);
        }

        public void ChangeDragonBallToFirstPage()
        {
            SetDefaultLabelTextValue();
            SetDefaultTextValue();
            SecondPage.SetActive(false);
            FirstPage.SetActive(true);
        }
    }
}
