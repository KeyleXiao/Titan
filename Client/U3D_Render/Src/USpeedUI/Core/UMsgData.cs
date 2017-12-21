using System;
using System.Collections.Generic;
using U3D_Render;
using DataCenter;
using UnityEngine;
using War;

namespace USpeedUI
{
    // 窗口消息数据基类
    public class UIMsgData
    {
        public int msgID;       // 消息id
    }

    // 逻辑层command通用窗口消息数据
    public class UIMsgCmdData : UIMsgData
    {
        public int nParam;
        public string strParam;
        public IntPtr ptrParam;
        public int nPtrLen;

        public UIMsgCmdData(int msgID, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
        {
            this.msgID = msgID;
            this.nParam = nParam;
            this.strParam = strParam;
            this.ptrParam = ptrParam;
            this.nPtrLen = nPtrLen;
        }
    }

    // 缓存消息
    public class UIMsgDataCache
    {
        public int msgID;
        public UIMsgData msgData;
    }

    // WND_ID_PLAYERFRAME start
    public class UIButtonType : UIMsgData
    {
        public int nType;
        public bool bIsRequestSelf;
        public int nRequestPDBID;
        public bool bFake;
        public string strFakeName;
        public UIButtonType(int _nType)
        {
            nType = _nType;
            bIsRequestSelf = true;
            nRequestPDBID = 0;
            bFake = false;
            strFakeName = "";
        }
    }
    // WND_ID_PLAYERFRAME end

    // WND_ID_CHATBOX start
    public class ChatBoxChangeChannelData : UIMsgData
    {
        public int channel;
    }
    public class ChatBoxAddInputFieldItemData : UIMsgData
    {
        public Chat.SChatMessageObjectInfo objInfo;
        public bool bClearInputField = false;
		public bool bAutoSend = false;
        public bool bActiveInputField = true;
    }
    public class ChatBoxChangeImeData : UIMsgData
    {
        public bool bIsInImeList;
        public bool bIsIme;
        public string name;
    }
    public class ChatBoxPrivateChat : UIMsgData
    {
        public string name;     // 私聊对象名字
    }
    // WND_ID_CHATBOX end

    // WND_ID_PLAYERWARSTATISTIC start
    public class WarStatisticViewSingleWarData : UIMsgData
    {
        public int nEntryType;
        public long lWarID;
    }
	// WND_ID_PLAYERWARSTATISTIC end

	// WND_MSG_PLAYERWARSTATISTIC_VIEWACTORWAR		// 显示某个英雄的战绩,且选中某场战绩
	public class WarStatisticViewActorWarData : UIMsgData
	{
		public int nActorID;
		public long lWarID;
	}

	// WND_ID_SIDEBUTTONLIST start
	// 侧边栏按钮自定义回调数据基类       PS:请定义自定义数据相等函数
	public class SideButtonCustomData
    {
        public virtual bool Equals(SideButtonCustomData obj)
        {
            return true;
        }
    }

    public class SideButtonDataHeroID : SideButtonCustomData
    {
        public int nHeroID;

        public override bool Equals(SideButtonCustomData obj)
        {
            SideButtonDataHeroID info = obj as SideButtonDataHeroID;
            return this.nHeroID == info.nHeroID;
        }
    }

    public class SideButtonDataBuildingSN : SideButtonCustomData
    {
        public uint nBuildingSN;

        public override bool Equals(SideButtonCustomData obj)
        {
            SideButtonDataBuildingSN info = obj as SideButtonDataBuildingSN;
            return this.nBuildingSN == info.nBuildingSN;
        }
    }

    public class SideButtonLegendBuildingSN : SideButtonCustomData
    {
        public uint nLegendCupID;
        public int nSearchID;

        public override bool Equals(SideButtonCustomData obj)
        {
            SideButtonLegendBuildingSN info = obj as SideButtonLegendBuildingSN;
            return this.nLegendCupID == info.nLegendCupID && this.nSearchID == info.nSearchID;
        }
    }

    public class SideButtonDataPkInfo : SideButtonCustomData
    {
        public uint uidOperator;
        public override bool Equals(SideButtonCustomData obj)
        {
            SideButtonDataPkInfo info = obj as SideButtonDataPkInfo;
            return this.uidOperator == info.uidOperator;
        }
    }

    public class SNSAskAddBuddyInfo : SideButtonCustomData
    {
        public int dwUserID;
        public string askBuddyName;
        public override bool Equals(SideButtonCustomData obj)
        {
            SNSAskAddBuddyInfo info = obj as SNSAskAddBuddyInfo;
            return this.dwUserID == info.dwUserID;
        }
    }

    public class SNSNewChatMsgInfo : SideButtonCustomData
    {
        public long SessionID;
        public override bool Equals(SideButtonCustomData obj)
        {
            SNSNewChatMsgInfo info = obj as SNSNewChatMsgInfo;
            return SessionID == info.SessionID;
        }
    }

    public class SideButtonDataMentorshipInfo : SideButtonCustomData
    {
        public int nSrcPDBID;
        public override bool Equals(SideButtonCustomData obj)
        {
            SideButtonDataMentorshipInfo info = obj as SideButtonDataMentorshipInfo;
            return this.nSrcPDBID == info.nSrcPDBID;
        }
    }

    public class ClanInviteInfo : SideButtonCustomData
    {
        public int nInviterID;
        public override bool Equals(SideButtonCustomData obj)
        {
            ClanInviteInfo info = obj as ClanInviteInfo;
            return this.nInviterID == info.nInviterID;
        }
    }

    public class KinInviteInfo : SideButtonCustomData
    {
        public int nInviterID;
        public override bool Equals(SideButtonCustomData obj)
        {
            KinInviteInfo info = obj as KinInviteInfo;
            return this.nInviterID == info.nInviterID;
        }
    }

    public class KinApplyInfo : SideButtonCustomData
    {
        public int nApplyID;
        public override bool Equals(SideButtonCustomData obj)
        {
            KinApplyInfo info = obj as KinApplyInfo;
            return this.nApplyID == info.nApplyID;
        }
    }

    public class SideButtonDataClanApplyInfo : SideButtonCustomData
    {
        public int nKinID;
        public override bool Equals(SideButtonCustomData obj)
        {
            SideButtonDataClanApplyInfo info = obj as SideButtonDataClanApplyInfo;
            return this.nKinID == info.nKinID;
        }
    }

	public class SNSInvitePlayGameInfo : SideButtonCustomData
	{
		public int nRoomID;
        public int dwInviteActorID;
		public string inviterName;
		public override bool Equals(SideButtonCustomData obj)
		{
			SNSInvitePlayGameInfo info = obj as SNSInvitePlayGameInfo;
			return this.nRoomID == info.nRoomID && this.inviterName == info.inviterName;
		}
	}


    // 侧边栏按钮自定义定时回调基类       PS:请定义自定义数据相等函数
    public class SideButtonOnTimerCustomData
    {
        public virtual bool Equals(SideButtonOnTimerCustomData obj)
        {
            return true;
        }
    }

	// WND_MSG_SIDEBUTTONLIST_ADD
	public class SideButtonListAddData : UIMsgData
    {
        public int nItemID;
        public bool bAllowRepeatItem;
        public int nButtonIconID;
        public String strButtonText;
        public String strTooltip;

        public SideButtonCustomData sCustomData;
        public Action<SideButtonCustomData> callBack;

        public SideButtonOnTimerCustomData sOnTimerCustomData;
        public Action<SideButtonOnTimerCustomData> onTimerCallBack;
    }
    // WND_MSG_SIDEBUTTONLIST_REMOVE
    public class SideButtonListRemoveData : UIMsgData
    {
        public int nItemID;

    }
    // WND_ID_SIDEBUTTONLIST end


    // WND_MSG_RECORDTABLE_FIGHTING_BOSSCDTIME
    public class BossCdTime : UIMsgData
    {
        public int nIndex;
        public int nTime;
    }
    // WND_MSG_WARRELIVETIME_SHOWRELIVETIME
    public class UIWarShowReliveTimeData : UIMsgData
    {
        public int nReliveTime;
		public int uid;
		public int pdbid;
		public bool isHero;		// 是否主角

    }

    // WND_MSG_WAR_UPDATE_HEROICON   更新战场英雄头像
    public class UIWarUpdateHeroIcon: UIMsgData
    {
        public ulong nPdbid;
        public int nHeroID;
        public int nSkinID;
    }

    public class HighLadderInfo
    {
        public int OrderID;
        public int nPDBID;
        public string PlayerName;
        public int RankGrade;
        public int RankScore;
        public string RankGradeDes;
    }

    //WND_MSG_HIGHLADDER_GETINFO
    public class HighLadderArray : UIMsgData
    {
        public int nLadderCount;
        public HighLadderInfo[] allLadderInfo;
        public HighLadderArray(int count)
        {
            allLadderInfo = new HighLadderInfo[count];
        }
    }

    public class ButtomOfRankData : UIMsgData
    {
        public int nLastIndex;
        public string szRankName;
        public int nLastScore;
    }

    //WND_MSG_COMMON_POPUPMENU
    public class UIPopupMenuData : UIMsgData
    {
        public UWidgets.UPopupAction actionPopup;
        public UnityEngine.Transform actionTarget;
        public UIWidgets.TipPosition actionTipPostion;
        public UIPopupMenuData(UWidgets.UPopupAction _actionPopup, UnityEngine.Transform _actonTarget, UIWidgets.TipPosition _actionTipPosition)
        {
            actionPopup = _actionPopup;
            actionTarget = _actonTarget;
            actionTipPostion = _actionTipPosition;
        }
    }
    // WND_ID_WAR_MINIMAP start
    public class MinimapObjData : UIMsgData
    {
        public EntityView ev;
        public EntityPosAngleInfo pos;
    }

    public class MinimapObjectAttachEffect : UIMsgData
    {
        public int nObjID;
        public int nEffectType;
    }

    public class MinimapStaticObjData : UIMsgData
    {
        public int nObjType;
    }

    public class MinimapStaticObjCountdown : UIMsgData
    {
        public int nObjType;
        public int nCountdown;
    }
    public class MinimapAddSignal : UIMsgData
    {
        public float fPosX;
        public float fPosY;
        public int nType;
    }

    public class MinimapSetObjRangeTip : UIMsgData
    {
        public bool bIsAdd;
        public int nObjID;
        public int nRange;
    }

    // WND_ID_WAR_MINIMAP end

    // WND_ID_WAR_MAPSIGNAL start
    // WND_MSG_WAR_MAPSIGNAL_SETSELECTSIGNAL
    public class MapSignalSetSelectSignal : UIMsgData
    {
        public int nType;
    }
    // WND_ID_WAR_MAPSIGNAL end

    // REDPOINT start
    public class RedPointSetCount : UIMsgData
    {
        public int nSubType;
        public int nCount;
    }
    // REDPOINT end

    // WND_ID_PKINFO start
    public class PKInfoItem : UIMsgData
    {
        public uint uidOperator;
        public string szInfo;
        public PKInfoItem(uint uid, string info)
        {
            uidOperator = uid;
            szInfo = info;
        }
    }

    public class PKPosInfo : UIMsgData
    {
        public float[] pos;
    }
    public class PKResult : UIMsgData
    {
        public int nResult;
        public bool bForceEnd;
    }
    // WND_ID_PKINFO end

    // WND_ID_POINTSHOP start
    public class PointShopOpenData : UIMsgData
    {
        public int nPageType;
    }
    // WND_ID_POINTSHOP end

    // WND_ID_POINTSHOPBUYGOODS start
    public class PointShopBuyGoodsInitData : UIMsgData
    {
        public int nSellID;
    }
    // WND_ID_POINTSHOPBUYGOODS end

    // WND_ID_PLAYERBUTTONFRAME start
    public class PlayerButtonFrameSimulateOpenData : UIMsgData
    {
        public PlayerButtonFrame.ESimulateOpenViewType eViewType;
    }
    // WND_ID_PLAYERBUTTONFRAME end

    public class DIDARefreshData : UIMsgData
    {
        public EDIDALayout eLayout;
    }

    public class DIDAAddData : SideButtonListAddData
    {
        public EDIDALayout eLayout = EDIDALayout.EDT_Middle;
        public int nDIDATime;
        public bool bShowInWar = false;
    }

    public class DIDARemoveData : SideButtonListRemoveData
    {
        public SideButtonCustomData sCustomData;
    }
    // WND_ID_HEROTALENTVIEW 英雄天赋显示控制
    public class HeroTalentVisibleData : UIMsgData
    {
        public bool bVisible;
        public int nHeroID;
    }

    public class PrizeTipData : UIMsgData
    {
        public EMPrizeTipType eTipType;
        public int nPrizeID;
    }

    // 
    public class ChatForceConditionHide : UIMsgData
    {
        public bool bForceHide;
    }

    public class FullViewForceConditionHide : UIMsgData
    {
        public bool bForceHide;
    }

    public class DeathReplayMurderData : UIMsgData
    {
        public USpeedUI.WarDeathReplay.MurderType murderType;
        public bool bShowMurderInfo;

        public DeathReplayMurderData(USpeedUI.WarDeathReplay.MurderType type, bool show)
        {
            this.murderType = type;
            this.bShowMurderInfo = show;
        }
    }

    public class PlayerAliveAssistData : UIMsgData
    {
        public int nHeroID;
        public string nPlayerName;
        public string szDesc;
        public int nSoundID;
        public bool bChatShow;
        public bool bSameCamp;
        public int nAssistType;
        public int nAssistCount;
    }

    public class PlayerHeroUpdateData : UIMsgData
    {
        public int nHeroID;
        public int nReqPDBID;
    }

    public class PlayerHeroSkinUpdateData : UIMsgData
    {
        public int nHeroID;
        public int nReqPDBID;
        public int nSkinID;
    }

    //WND_MSG_TASK_DELETE
    public class TaskDeleteArray : UIMsgData
    {
        public int nTaskDeleteCount;
        public int[] nTaskDelete;

        public TaskDeleteArray(int nCount)
        {
            nTaskDelete = new int[nCount];
        }
    }

    //GVIEWCMD_SEND_TASK_PRIZE
    public class TaskPrize : UIMsgData
    {
        public int nTaskID;
    }

    public class VoiceRoomBaseInfoData : UIMsgData
    {
        public int nRoomID;                        //id
        public int nUserNum;                      //用户数
        public int isSpeaking;                      //是否正在语音
        public int nLock;                           //是否有密码
    }

    public class VoiceRoomRigthInfo : UIMsgData
    {
        public int nMicMode;
        public bool bManageMicMode;
        public bool bInMicOrder;
        public bool bEnableGraspMic;
        public bool bForbidMic;
        public bool bOpenMicphone;
        public bool bOpenEarphone;
    }

    public class LegendCupNodeWarData : UIMsgData
    {
        public long nLegendCupID;
        public int nRoundID;
        public int nSearchID;
    }

    public class LegendCupJudgeWarData : UIMsgData
    {
        public long nLegendCupID;
        public int nRoundID;
        public int nSearchID;
        public long nWarID;                                 // 战场ID
        public int nKin1ID;                                 // 战队1ID
        public int nKin2ID;                                 // 战队2ID
        public string szKin1Name;                           // 战队1名称
        public string szKin2Name;                           // 战队2名称
        public int nWinKinID;                               // 胜利战队ID
        public byte nBoCount;								// 第几场(裁决)
    }

    public class LegendCupOnlineResetData : UIMsgData
    {
        public long nLegendCupID;
        public int nRoundID;
        public int nSearchID;
        public int nKin1ID;                                 // 战队1ID
        public int nKin2ID;                                 // 战队2ID
        public string szKin1Name;                           // 战队1名称
        public string szKin2Name;                           // 战队2名称
        public int nResetEndTime;                          // 最终时间点
    }

    public class LegendCupIDData : UIMsgData
    {
        public long nLegendCupID;                           // 杯赛ID
    }

    public class LegendCupRegistKinData : UIMsgData
    {
        public long lLegendCupID;
        public int nKinID;
    }

    // WND_MSG_SNS_START 开始
    public class SNSAddChatMsgData : UIMsgData
    {
        public long SessionID;
    }

    // WND_MSG_SNS_SHOW_CHAT_WND 打开聊听窗口
    public class SNSOpenChatWndMsgData : UIMsgData
    {
        public long SessionID;
    }

	// WND_MSG_SNS_ADD_BUDDY 添加好友
	public class SNSAddBuddyWndMsgData : UIMsgData
	{
		public int nUserID;
		public string actorName;
	}

    // WND_MSG_SNS_SHOW_INPUTDIALOG 显示输入对话框
    public class SNSOpenInputDialogMsgData : UIMsgData
    {
        public enum EMDialogType
        {
            EM_COMMON,                          // 通用的，允许同时有多个对话框
            EM_CHANGE_REMARK,                   // 修改备注 
            EM_ADD_BUDDYGROUP,                  // 添加好友分组
            EM_CHANGE_BUDDYGROUP_NAME,          // 修改好友分组名字
        };

        public EMDialogType type = EMDialogType.EM_COMMON;     // 对话框类型
        public string title;                                // 对话框标题
        public string defaultText;                          // 输入框默认内容
        public string placeHolderText;                      // 输入框为空的时候显示的内容
        public SNS.SNSInputDialogConfirm onClickDelegate;  // 按钮点击委托
    }

    // WND_MSG_SNS_SEND_CHAT_MESSAGE 发送聊听消息
    public class SNSSendChatDataMsgData : UIMsgData
    {
        public int dwUserID;
        public string content;       // 聊听内容
        public int dwFontColor;     // 字体颜色
    }

    // WND_MSG_SNS_RECV_ASK_ADDBUDDY 收到添加好友请求
    public class SNSRecvAskAddBuddyMsgData : UIMsgData
    {
        public int dwUserID;
        public string name;
    }
    // WND_MSG_SNS_RSP_ASK_ADDBUDDY 同意或拒绝好友请求
    public class SNSRspAskAddBuddyMsgData : UIMsgData
    {
        public int dwUserID;
        public bool bAgree;
    }
    // WND_MSG_SNS_BLACKLIST_BY_OTHER  被对方拉入黑名单
    public class SNSBlackListByOtherMsgData : UIMsgData
    {
        public int dwUserID;
    }

    // WND_MSG_SNS_SELECT_SESSION  选择会话
    public class SNSSelectSessionMsgData : UIMsgData
    {
        public int dwUserID;
    }

    // WND_MSG_SNS_START 结束

    //WndMsgID.WND_MSG_FULLSCREEN_CHANGEIMAGE 全屏背景图片切换
    public class UFullScreenStateData : UIMsgData
    {
        public int FullScreenImageID;
        public bool bFullScreenShow;

        public UFullScreenStateData(int _nImageID = 0, bool _bShow = true)
        {
            FullScreenImageID = _nImageID;
            bFullScreenShow = _bShow;
        }
    }

    // WND_MSG_GAMEGUIDE_ADDNODEMESSAGE
    // WND_MSG_GAMEGUIDE_REMOVENODEMESSAGE
    // 显示/删除 引导UI
    public class UGuideMsgData : UIMsgData
    {
        public string strMsgText;
        public GUIDE.EGuideNodeID nGuideNodeID;
        public int nGuideSignSpriteID;
        public bool bImmediate;
        public bool bModal;
        public CustomUserData UserData;

        public class CustomUserData
        {
            public float? CustomXPos;
            public float? CustomYPos;
            public uint? CustomType;
            
            /// <param name="_fCustomXPox">x坐标偏移值</param>
            /// <param name="_fCustomYPos">y坐标偏移值</param>
            /// <param name="_uCustomType">0：头像在右边类型，1：头像在左边类型</param>
            public CustomUserData(float? _fCustomXPox = null,float? _fCustomYPos = null,uint? _uCustomType = null)
            {
                CustomXPos = _fCustomXPox;
                CustomYPos = _fCustomYPos;
                CustomType = _uCustomType;
            }
        }

        public UGuideMsgData(string _strMsg, GUIDE.EGuideNodeID _nGuideNodeID, int _nGuideSignSpriteID = -1,bool _bImmediate=false,bool _bModal = false, CustomUserData _userData = null)
        {
            strMsgText = _strMsg;
            nGuideNodeID = _nGuideNodeID;
            nGuideSignSpriteID = _nGuideSignSpriteID;
            bImmediate = _bImmediate;
            bModal = _bModal;
            UserData = _userData;
        }
        public UGuideMsgData()
        {
            strMsgText = null;
            nGuideNodeID = GUIDE.EGuideNodeID.GuideNodeID_None;
            nGuideSignSpriteID = -1;
            bImmediate = false;
            bModal = false;
            UserData = null;
        }

        public void CloneTo(UGuideMsgData _cloneData)
        {
            this.strMsgText = _cloneData.strMsgText;
            this.nGuideNodeID = _cloneData.nGuideNodeID;
            this.nGuideSignSpriteID = _cloneData.nGuideSignSpriteID;
            this.bImmediate = _cloneData.bImmediate;
            this.bModal = _cloneData.bModal;
            this.UserData = _cloneData.UserData;
        }
    }

    public class UGuideMsgRemoveData : UIMsgData
    {
        public bool IsRemoveMessage;
        public bool IsRemoveModal;
        public bool IsHideView;

        public UGuideMsgRemoveData(bool _bRemoveMessage = true,bool _bRemoveModal = true,bool _bHideView = true)
        {
            IsRemoveMessage = _bRemoveMessage;
            IsRemoveModal = _bRemoveModal;
            IsHideView = _bHideView;
        }
    }


    public class ULoudspeakerData : UIMsgData
    {
        public string message;
    }

    public class UNPCResponseData : UIMsgData
    {
        public DataCenter.NPCDataManager.NPCResponse npcResp;
    }

    public class USystemFloatTextData : UIMsgData
    {
        public string message;
        public int nPos;
        public int nShowTime;
    }

    public class USystemSettingData : UIMsgData
    {
        public SystemSetting.SystemSettingSubWndType wndType;
    }

    public class USystemSettingFXAA: UIMsgData
    {
        public bool bEnableFXAA;

        public USystemSettingFXAA(bool bOpen)
        {
            bEnableFXAA = bOpen;
        }
    }

    public class UWarFloatFontData : UIMsgData
    {
        public GameObject Entity;
        public String Text;
        public WarFloatFont.WarFloatFontType Type;
        public UWarFloatFontData(int msgID, GameObject go, String tex, WarFloatFont.WarFloatFontType type)
        {
            this.msgID = msgID;
            this.Entity = go;
            this.Text = tex;
            this.Type = type;
        }
    }

    public class UWarFloatFontItemData : UIMsgData
    {
        public WarFloatFont.UWarFloatFontItem FloatFontItem;
    }

    public class UFrontSightPosData : UIMsgData
    {
        public Vector2 Pos;
    }

    public class UFrontSightColorData : UIMsgData
    {
        public Color nColor;
    }

    public class UFrontSightVisibleData : UIMsgData
    {
        public bool bShow;
    }

    public class UFPSBulletVisibleData : UIMsgData
    {
        public bool bShow;
    }

    // 玩家位置发生更新 WND_MSG_COMMON_LOCATION_CHANGE
    public class UActorLocationChangeMsgData : UIMsgData
	{
		public Vector3 vNewPos;
	}


    // 通用奖励弹出界面数据
    public class UPrizePopupData : UIMsgData
    {
        // 普通奖励
        public string strPrizeTitle;
        public string strPrizeDesc;
        public List<int> nPrizeIdList;

        // 天赋奖励
        public int nHeroID;
        public List<int> nTalentIDList;

        // 金币、经验
        public int nMoneyCount;
        public int nExpCount;
    }

    public class UGuideWidgetMsgData : UIMsgData
    {
        public int nGuideID;
        public int nGuideStepID;
        public int nGuideEffectID;
        public object oUserData;
        public bool bForeceGuide;
        public bool bAcceptClickEvent;

        
        public UGuideWidgetMsgData(int _nGuideID,int _nGuideStepID,int _nGuideEffectID,object _oUserData = null,bool _bForceGuide = false,bool _bClickEvent = true)
        {
            nGuideID = _nGuideID;
            nGuideStepID = _nGuideStepID;
            nGuideEffectID = _nGuideEffectID;
            oUserData = _oUserData;
            bForeceGuide = _bForceGuide;
            bAcceptClickEvent = _bClickEvent;
        }
        public UGuideWidgetMsgData(UGuideWidgetMsgData _cloneFrom)
        {
            CloneData(_cloneFrom);
        }
        public void CloneData(UGuideWidgetMsgData _cloneFrom)
        {
            this.nGuideID = _cloneFrom.nGuideID;
            this.nGuideStepID = _cloneFrom.nGuideStepID;
            this.nGuideEffectID = _cloneFrom.nGuideEffectID;
            this.oUserData = _cloneFrom.oUserData;
            this.bForeceGuide = _cloneFrom.bForeceGuide;
            this.bAcceptClickEvent = _cloneFrom.bAcceptClickEvent;
        }
    }

    public class UChartRankingMsgData : UIMsgData
    {
        public int nRanking;
        public bool bShow = true;

        public UChartRankingMsgData() { }
        public UChartRankingMsgData(int _nRanking, bool _bShow = true)
        {
            nRanking = _nRanking;
            bShow = _bShow;
        }
    }

    // 解释界面标题内容
    public class UPopupTipMsgData : UIMsgData
    {
        public string strTitle;
        public string strContent;

        public UPopupTipMsgData(string _title, string _content)
        {
            strTitle = _title;
            strContent = _content;
        }
    }

    // 消费引导界面
    public class UConsumptionMsgData : UIMsgData
    {
        public int nRequestType;
        public int nGuideType;

        public UConsumptionMsgData(int _req, int _guide)
        {
            nRequestType = _req;
            nGuideType = _guide;
        }
    }

	// 增加标签
	public class UAddLabelMsgData : UIMsgData
	{
		public string operatorName;
		public string targetName;
		public int nLabelID;
	}

    // 英雄的操作类型:0默认，1第一人称
    public class OperationType : UIMsgData
    {
        public int operationType;
    }

    public class crownActiveSlotModelData : UIMsgData
    {
        public PlayerCrownWndPart.EGEMSTONEDRAGTYPE type;
        public int slotID;
        public bool bActive;
    }

	// WND_MSG_CHATBUBBLE_NEW_MESSAGE 聊天气泡新消息
	public class UChatBubbleMsgData : UIMsgData
	{
		public uint uid;	// 发送消息者uid
		public Chat.SChatMessageInfo info;
	}

	// WND_MSG_CHATBUBBLE_TEAM_CHAT_MESSAGE
	public class UTeamBubbleChatMessage : UIMsgData
	{
		public uint pdbid;	// 发送者pdbid
		public string message;
		public Chat.SChatMessageInfo info;
	}

	// 装备信息
	public class UEquipSchemeMsgData : UIMsgData
	{
		public int nHeroID;
		public GameLogic.cmd_war_equipscheme scheme;
	}

    //保存别人分享的装备配置已满消息
    public class USaveShareGoodsDispositionMsgData : UIMsgData
    {
        public int HeroID;
        public GameLogic.cmd_war_equipscheme scheme;
    }

    public class UGD_DispositionFilledMsgData : UIMsgData
    {
        public Action InteractiveCallback;
        public Action onConfirmCallback;
        
        public int HeroID;
        /// <summary>
        /// 当前要增加的配置方案
        /// </summary>
        public GameLogic.cmd_war_equipscheme NeedAddScheme;
    }

    public class UWGS_SelectedGoodsDispositionMsgData : UIMsgData
    {
        public WarGoodsStore.UWarGoodsStoreDispositionItem SelectedDisposition;
    }

    public class UWarTowerSoldierMsgData : UIMsgData
    {
        public bool bShow;
        public int nSoldierCount;

        public UWarTowerSoldierMsgData(bool show, int ncount)
        {
            bShow = show;
            nSoldierCount = ncount;
        }
    }


    public class USocialShareResultMsgData : UIMsgData
    {
        public int nResult;
        public string imgUrl;
        public SocialSharing.SocialShareUploadWnd shareWnd;
    }

    public class USocialShareShowMsgData : UIMsgData
    {
        public Int64 nDBWarID;
        public int fileIndex;
        public string imgPath;
        public byte[] imgData;
    }

    public class UGloryMomentVideoMsgData : UIMsgData
    {
        public Int64 nDBWarID;
        public int fileIndex;
        public string videoPath;
    }

    public class UGloryMomentReqMsgData : UIMsgData
    {
        public Int64 nDBWarID;
        public string szDataTime;
        public PlayerFrame.EMGloryMomentPageType pageType;
    }

    // 主城弹窗是否可见
    public class UCommonPopupWndVisible: UIMsgData
    {
        public int nWndID;               // 窗口ID
        public bool isVisible;          // 是否可见
    }

    // 主城是否打开全屏界面
    public class UCommonFullWndVisible : UIMsgData
    {
        public int nWndID;               // 窗口ID
        public bool isVisible;          // 是否可见
    }

    // 战场结束     WND_MSG_COMMON_WAREND
    public class UCommonWarEndMsgData : UIMsgData
    {
        public bool isSelfWin;      // 自己阵营是否胜利
        public uint nUseTime;       // 战场持续时间
        public float DelayTime;     // 延迟显示窗口的时间
        public bool isShowResultWnd;    // 是否显示战场结果窗口
    }

    public class UBattleWarLifeState : UIMsgData
    {
        public SWar2ViewMsgPersonLifeState lifeState;
    }

    // 新地图： 击杀英雄后，收集基因
    public class UBattleWarCollectGenic : UIMsgData
    {
        public SWar2ViewMsgGenicUpdate genicData;
    }

    public class ULifeHeroInjectHeroData : UIMsgData
    {
        public GameLogic.cmd_entity_lifehero_info lifeHero;
    }

    public class ULifeHeroCandidateData : UIMsgData
    {
        public LifeHero.ESelectViewState state;
        public int cultureID;
        public int heroID;
        public List<GameLogic.cmd_entity_hero_info> candidates;
    }

    public class UTaskUnfoldData : UIMsgData
    {
        public int nTaskID;
        public long llContactID;
    }

    public class UIReconnectData : UIMsgData
    {
        public int nActorID;
        public int nHeroID;
        public string szActorName;
    }

    public class UIXPSkillCD : UIMsgData
    {
        public SWar2ViewMsgXPBeginCD data;
    }

    public class UISWitchHero : UIMsgData
    {
        public int nEntityID;
    }

    public class UIBattleWarUpdateDevilking : UIMsgData
    {
        public SWar2ViewMsgDevilKingUpdate data;
    }

    public class UPromotionNavItem : UIMsgData
    {
        public int nNavID;
        public string szNavName;

        public UPromotionNavItem(int _navID, string _navName)
        {
            nNavID = _navID;
            szNavName = _navName;
        }
    }


    public class UPromotionWebUrl : UIMsgData
    {
        public int nNavID;
        public string szWebUrl;

        public UPromotionWebUrl(int _navID, string _webUrl)
        {
            nNavID = _navID;
            szWebUrl = _webUrl;
        }
    }

    public class UIOBHideFramePart : UIMsgData
    {
        public int frame;
    }

    public class UChatEmoticonGroupState : UIMsgData
    {
        public List<int> unlockEmotionGroup;

        public UChatEmoticonGroupState(List<int> _group)
        {
            unlockEmotionGroup = _group;
        }
    }
}