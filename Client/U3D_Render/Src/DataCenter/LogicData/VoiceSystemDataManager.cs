using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using USpeedUI;

namespace DataCenter
{
    public class VoiceSystemDataManager
    {
        public int ClientID { get { return nClientID; } }
        //public int UserType { get { return nUserType; } }
        public int UserRight { get { return nUserRight; } }
        public int CurrentChannelType { get{return nCurrentChannelType;}}
        public int CurrentRoomType { get { return nCurrentRoomID; } }
        //public int CurrentRoomFlag { get { return nCurrentRoomFlag; } }
        public int CurrentMicOrderMode { get { return nCurrentMicOrderMode; } }
        public int CurrentMicOrderInterval { get { return nCurrentMicOrderInterval; } }
        //public int CurrentMicUserClientID { get { return nCurrentMicUserClientID; } }
        public List<int> ChannelActiveList { get { return m_ChannelActiveList; } }
        //public List<cmd_voice_update_room_base_info> RoomList { get { return m_RoomList; } }
        public List<cmd_voice_update_user_info> MicOrderList { get { return m_MicOrderList; } }
        public List<cmd_voice_update_user_info> UserInfoList { get { return m_UserInfoList; } }
        public List<cmd_voice_update_user_info> TempUserInfoList { get { return m_TempUserInfoList; } }

        private List<int> m_ChannelActiveList;
        private List<cmd_voice_update_room_base_info> m_RoomList;
        private List<cmd_voice_update_user_info> m_MicOrderList;
        private List<cmd_voice_update_user_info> m_UserInfoList;
        private List<cmd_voice_update_user_info> m_TempUserInfoList;
        private Dictionary<int, int> m_UserSpeakStateDic;

        private int nClientID;
        private int nWorldID;
        private int nPDBID;
        private int nUDBID;
        private int nUserType;
        private int nUserState;
        private int nUserRight;
        private bool bInMicOrder = false;

        private int nCurrentChannelType;
        private int nCurrentRoomID;
        private int nCurrentRoomFlag;
        private int nCurrentMicOrderMode;
        private int nCurrentMicOrderInterval;
        private int nCurrentMicUserClientID;

        public void Init()
        {
            nCurrentChannelType = 0;
            nCurrentRoomID = 0;
            nUserType = 0;
            nCurrentRoomFlag = 0;
            nCurrentMicOrderMode = 0;
            nCurrentMicOrderInterval = 0;
            m_ChannelActiveList = new List<int>();
            m_RoomList = new List<cmd_voice_update_room_base_info>();
            m_MicOrderList = new List<cmd_voice_update_user_info>();
            m_UserInfoList = new List<cmd_voice_update_user_info>();
            m_TempUserInfoList = new List<cmd_voice_update_user_info>();
            m_UserSpeakStateDic = new Dictionary<int, int>();
        }

        public void Clear()
        {
            m_ChannelActiveList.Clear();
            m_ChannelActiveList = null;
            m_RoomList.Clear();
            m_RoomList = null;
            m_MicOrderList.Clear();
            m_MicOrderList = null;
            m_UserInfoList.Clear();
            m_UserInfoList = null;
            m_TempUserInfoList.Clear();
            m_TempUserInfoList = null;
            m_UserSpeakStateDic.Clear();
            m_UserSpeakStateDic = null;

            nCurrentChannelType = -1;
            nCurrentRoomID = -1;
            nClientID = 0;
        }

        public void UpdateChannelList(cmd_voice_update_channel_list list)
        {
            m_ChannelActiveList.Clear();
            foreach (int i in list.channelList)
            {
                m_ChannelActiveList.Add(i);
            }
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATECHANNELLIST, null);
        }

        public void EnterRoom(cmd_voice_enter_room roomInfo)
        {
            nCurrentChannelType = roomInfo.nType;
            nCurrentRoomID = roomInfo.nRoomID;
            //nUserRight = roomInfo.nRight;
            m_RoomList.Clear();
            m_MicOrderList.Clear();
            m_UserInfoList.Clear();
            m_TempUserInfoList.Clear();
            m_UserSpeakStateDic.Clear();

            if (nCurrentChannelType == 0)
            {
                Rest();
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_QUITVOICE, null);
            }
            else
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_ENTERROOM, null);
        }

        public void UpdateRoomBaseInfo(cmd_voice_update_room_base_info baseInfo)
        {
            int order;
            int index = m_RoomList.FindIndex(x => x.nRoomID == baseInfo.nRoomID);
            if (index != -1)
            {
                order = index;
                m_RoomList[index] = baseInfo;
            }
            else
            {
                m_RoomList.Add(baseInfo);
                order = m_RoomList.Count - 1;
            }

            VoiceRoomBaseInfoData data = new VoiceRoomBaseInfoData();
            data.nRoomID = order;
            data.nUserNum = baseInfo.nUserNum;
            data.isSpeaking = baseInfo.isSpeaking;
            data.nLock = baseInfo.isLocked;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATE_ROOMBASEINFO, data);
        }

        public void UpdateRoomInfo(cmd_voice_update_room_info roomInfo)
        {
            UpdateRoomBaseInfo(roomInfo.baseInfo);
            if (roomInfo.baseInfo.nRoomID == nCurrentRoomID)
            {
                nCurrentRoomFlag = roomInfo.nRoomFlag;
                nCurrentMicOrderMode = roomInfo.nMicOrderMode;
                nCurrentMicOrderInterval = roomInfo.nMicOrderInterval;
                nCurrentMicUserClientID = roomInfo.nCurMicUserClientID;
                VoiceRoomRigthInfo info = CalculateRight();
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATEROOMRIGHTINFO, info);
            }
        }

        public void UpdateMicOrder(cmd_voice_update_user_info micOrder)
        {
            if(micOrder.isAdd == 0 || micOrder.nMicIndex == 0)
            {
                int index = m_MicOrderList.FindIndex(x => x.nClientID == micOrder.nClientID);
                if(index > -1)
                    m_MicOrderList.RemoveAt(index);
            }
            else
            {
                int index = m_MicOrderList.FindIndex(x => x.nClientID == micOrder.nClientID);
                if (index > -1)
                {
                    m_MicOrderList[index] = micOrder;
                }
                else
                    m_MicOrderList.Add(micOrder);
            }

            m_MicOrderList.Sort((x, y) => x.nMicIndex.CompareTo(y.nMicIndex));

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATEMICORDER, null);
        }

        public void UpdateUserInfo(cmd_voice_update_user_info userInfo)
        {
            if (userInfo.nClientID == 0)
                return;
            if (null != EntityFactory.MainHeroView && null != EntityFactory.MainHeroView.Property)
            {
                if (userInfo.nPDBID == EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID))
                {
                    nClientID = userInfo.nClientID;
                    nWorldID = userInfo.nWorldID;
                    nPDBID = userInfo.nPDBID;
                    nUDBID = userInfo.nUDBID;
                    nUserType = userInfo.nUserType;
                    nUserRight = userInfo.nUserRight;
                    nUserState = userInfo.nUserState;
                    bInMicOrder = userInfo.nMicIndex > 0 ? true : false;
                    VoiceRoomRigthInfo info = CalculateRight();
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATEROOMRIGHTINFO, info);
                }
            }

            if (userInfo.isAdd == 0)
            {
                int index = m_UserInfoList.FindIndex(x => x.nClientID == userInfo.nClientID);
                if (index > -1)
                    m_UserInfoList.RemoveAt(index);
            }
            else
            {
                int index = m_UserInfoList.FindIndex(x => x.nClientID == userInfo.nClientID);
                if (index != -1)
                {
                    m_UserInfoList[index] = userInfo;
                }
                else
                    m_UserInfoList.Add(userInfo);
            }
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATEUSERINFO, null);
            UpdateMicOrder(userInfo);
        }

        public void UpdateUserSpeakingState(cmd_voice_update_user_ispeaking speakData)
        {
            if (m_UserSpeakStateDic.ContainsKey(speakData.nClientID))
                m_UserSpeakStateDic[speakData.nClientID] = speakData.isSpeaking;
            else
                m_UserSpeakStateDic.Add(speakData.nClientID, speakData.isSpeaking);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATEUSERINFO, null);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_UPDATEMICORDER, null);
        }

        public void RequestEnterRoom(int channel, int room, string password = "")
        {
            if (room < 0 || channel < 0)
                return;

            if (room != nCurrentRoomID || CurrentChannelType != channel)
            {
                cmd_voice_req_enter_room roomInfo = new cmd_voice_req_enter_room();
                roomInfo.nType = channel;
                roomInfo.nRoomID = channel == 0 ? 0 : room;
                roomInfo.szPassword = password;
                ViewEventHelper.Instance.SendCommand<cmd_voice_req_enter_room>(GameLogicDef.GVIEWCMD_VOICE_REQUEST_ENTER_ROOM, ref roomInfo);
            }
        }

        public void ReqSearchUserInfo(string content)
        {
            IEnumerable<cmd_voice_update_user_info> ListQuery = from item in m_UserInfoList
                                                                where item.szUserName.Contains(content)
                                                                select item;
            m_TempUserInfoList.Clear();
            foreach (cmd_voice_update_user_info item in ListQuery)
                m_TempUserInfoList.Add(item);
            //if (m_TempUserInfoList.Count > 0)
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_VOICESYSTEM_RECEIVEUSERSEARCHDATA, null);

        }

        public void ReqChangeMicMode(int nMode)
        {
            if (nMode == nCurrentMicOrderMode || nMode < 0)
                return;

            cmd_voice_req_change_mic_order_mode micMode = new cmd_voice_req_change_mic_order_mode();
            micMode.nMode = nMode;
            ViewEventHelper.Instance.SendCommand<cmd_voice_req_change_mic_order_mode>(GameLogicDef.GVIEWCMD_VOICE_REQUEST_CHANGE_MIC_ORDER_MODE, ref micMode);
        }

        public bool GetSpeakingState(int nClientID)
        {
            if (m_UserSpeakStateDic.ContainsKey(nClientID))
                return m_UserSpeakStateDic[nClientID] != 0 ? true : false;

            return false;
        }

        public bool GetRoomLockState(int room)
        {
            cmd_voice_update_room_base_info info = m_RoomList.Find(x => x.nRoomID == room);
            if (info.isLocked != 0)
                return true;
            else
                return false;
        }

        private void Rest()
        {
            m_ChannelActiveList.Clear();
            m_RoomList.Clear();
            m_MicOrderList.Clear();
            m_UserInfoList.Clear();
            m_TempUserInfoList.Clear();

            nCurrentChannelType = 0;
            nCurrentRoomID = 0;
            nClientID = 0;
        }

        private VoiceRoomRigthInfo CalculateRight()
        {
            VoiceRoomRigthInfo info = new VoiceRoomRigthInfo();
            info.nMicMode = CurrentMicOrderMode;

            if ((nUserRight & (int)EMVoiceRightFlag.VoiceRight_Mic) != 0)
                info.bManageMicMode = true;
            else
                info.bManageMicMode = false;

            if ((nCurrentRoomFlag & (int)EMVoiceRoomStateFlag.VoiceRoomState_AddMicOrder) != 0 && nCurrentMicOrderMode == (int)EMVoiceMicState.VoiceMicState_MicOrder)
                info.bEnableGraspMic = true;
            else
                info.bEnableGraspMic = false;

            info.bInMicOrder = bInMicOrder;

            info.bForbidMic = true;
            if (nUserType == (int)EMVoiceUserType.VoiceUserType_GM || nUserType == (int)EMVoiceUserType.VoiceUserType_OW ||
                nUserType == (int)EMVoiceUserType.VoiceUserType_Admin)
                info.bForbidMic = false;
            else
            {
                if (CurrentMicOrderMode == (int)EMVoiceMicState.VoiceMicState_MicFree)
                    info.bForbidMic = false;
                else if (CurrentMicOrderMode == (int)EMVoiceMicState.VoiceMicState_MicOrder)
                {
                    if (nCurrentMicUserClientID == LogicDataCenter.voiceSystemDataManager.ClientID)
                        info.bForbidMic = false;
                }
            }

            if ((nUserState & (int)EMVoiceUserStateFlag.VoiceUserStateFlag_Mic) != 0)
                info.bOpenMicphone = true;
            else
                info.bOpenMicphone = false;

            if ((nUserState & (int)EMVoiceUserStateFlag.VoiceUserStateFlag_NoListen) != 0)
                info.bOpenEarphone = false;
            else
                info.bOpenEarphone = true;

            return info;
        }
    }
}
