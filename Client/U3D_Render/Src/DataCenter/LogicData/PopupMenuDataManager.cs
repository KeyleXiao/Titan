/*******************************************************************
** 文件名:	PopupMenuDataManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2017-3-6
** 版  本:	1.0
** 描  述:	右键弹出菜单元素管理工具
            使用方法：
            1、EPopupItemInfoKey 添加新的项
            2、用户先执行 PopupMenuDataManager::RegisterPopupupItem 注册信息，不需要时执行 PopupMenuDataManager::UnRegisterPopupItem
            3、用户自己记录 UPopupItemRegisterInfo 这个数据，日后有用。
            4、管理器会通知用户进行数据填充 UPopupItemRegisterInfo::FillHoleAction
            5、PopupMenuDataManager::SetPopupItemData 用户可以选择马上填充 或者 等自己所需的数据处理准备好了再填充。
            6、增加了在一定时间后强制显示弹出菜单，避免有些情况用户填充时间太长。
********************************************************************/
using GameLogic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using U3D_Render;
using UnityEngine;
using UnityEngine.Events;
using USpeedUI;
using USpeedUI.UWidgets;

namespace DataCenter
{
    public class PopupMenuDataManager : Singleton<PopupMenuDataManager>, IModelData,ITimerHanlder
    {
        /// <summary>
        /// 右键显示列表元素关键字
        /// 特别作用是规定元素显示顺序
        /// </summary>
        public enum EPopupItemInfoKey
        {
            EInfo_AddOrDelBuddy,	// 添加或删除好友
            EInfo_AddBlackList,		// 加入黑名单
            EInfo_PrivateChat,		// 私聊
            EInfo_ViewPlayer,       // 查看玩家
            EInfo_TeamInvite,       // 邀请组队
            EInfo_TeamRequest,      // 请求组队
            EInfo_KinInvite,        // 邀请加入战队
            EInfo_ClanInvite,       // 邀请加入联盟
            EInfo_AddMaster,        // 拜师
            EInfo_AddPrentice,      // 收徒
            EInfo_PK,               // PK
            EInfo_BuddyChat,        // 好友聊听
        }

        public class UPopupItemRegisterInfo
        {
            public UnityAction<int> FillHoleAction;
            public EPopupItemInfoKey TheHoleKey;
            public UListComponentItemDataPopup ItemData;

            /// <param name="_strKey">注册关键字</param>
            /// <param name="_action">通知用户要给弹出菜单列表填数据,传来的int是右键选中目标的EntityID</param>
            public UPopupItemRegisterInfo(EPopupItemInfoKey _eKey,UnityAction<int> _action)
            {
                FillHoleAction = _action;
                TheHoleKey = _eKey;
            }
        }

        public void Init()
        {
            foreach(var i in Enum.GetValues(typeof(EPopupItemInfoKey)))
            {
                m_dicPopupItemInfo.Add((EPopupItemInfoKey)i, null);
            }
        }

        public void Clear()
        {
            if (m_PopupMenuAction != null)
            {
                m_PopupMenuAction.Clear();
                m_PopupMenuAction = null;
            }
                
            if(m_MenuData != null)
            {
                m_MenuData = null;
            }

            if(m_dicPopupItemInfo != null)
            {
                m_dicPopupItemInfo.Clear();
                m_dicPopupItemInfo = null;
            }
        }

        protected Dictionary<EPopupItemInfoKey, UPopupItemRegisterInfo> m_dicPopupItemInfo = new Dictionary<EPopupItemInfoKey, UPopupItemRegisterInfo>();
        protected USpeedUI.UWidgets.UPopupAction m_PopupMenuAction = new USpeedUI.UWidgets.UPopupAction();
        protected UIPopupMenuData m_MenuData = new UIPopupMenuData(null, null, UIWidgets.TipPosition.MouseBottomRightCorner);

        public int PopupMenuTargetEntityID;

        /// <summary>
        /// 注册弹出元素信息
        /// </summary>
        /// <param name="_info"></param>
        /// <returns>true注册成功</returns>
        public bool RegisterPopupupItem(ref UPopupItemRegisterInfo _info)
        {
            bool result = false;

            if(_info != null && m_dicPopupItemInfo.ContainsKey(_info.TheHoleKey))
            {
                m_dicPopupItemInfo[_info.TheHoleKey] = _info;
                result = true;
            }
            if(!result)
            {
                Trace.LogWarning("注册右键弹出菜单元素:" + _info.TheHoleKey + " 失败，检查是不是元素显示的名字为空或者已经注册了该显示名字的数据。");
            }
            return result;
        }

        /// <summary>
        /// 反注册弹出元素信息
        /// </summary>
        /// <param name="_info"></param>
        /// <returns></returns>
        public bool UnRegisterPopupItem(ref UPopupItemRegisterInfo _info)
        {
            bool result = false;

            if (_info != null && m_dicPopupItemInfo.ContainsKey(_info.TheHoleKey))
            {
                m_dicPopupItemInfo[_info.TheHoleKey] = null;
                result = true;
            }
            if (!result)
            {
                Trace.LogWarning("反注册右键弹出菜单元素失败:" + _info.TheHoleKey + " ，检测是不是元素显示的名字为空或者该显示名字的数据未注册了。");
            }

            return result;
        }

        /// <summary>
        /// 填充弹出元素数据(显示文本、选中回调等)
        /// </summary>
        /// <param name="_info"></param>
        /// <param name="_popupData"></param>
        /// <returns></returns>
        public bool SetPopupItemData(UPopupItemRegisterInfo _info,ref UListComponentItemDataPopup _popupData)
        {
            bool result = false;

            if (_info != null && _popupData != null)
            {
                if (m_dicPopupItemInfo[_info.TheHoleKey] != null)
                {
                    m_dicPopupItemInfo[_info.TheHoleKey].ItemData = _popupData;
                    result = true;
                }
            }                

            if(result && IsAllRegisterFillItemData())
            {
                TimerManager.KillTimer(this, 0);
                SendPopupMenuMessage();
            }

            return result;
        }

        #region 使用时不需要关心的
        /// <summary>
        /// 每次需要显示弹出菜单前清空之前的数据
        /// </summary>
        protected void ClearPopupItemData()
        {
            foreach (UPopupItemRegisterInfo info in m_dicPopupItemInfo.Values)
            {
                if (info != null)
                    info.ItemData = null;
            }
        }

        /// <summary>
        /// 通知用户要给这边填充菜单数据
        /// </summary>
        protected void InvokeFillHoleAction()
        {
            foreach(UPopupItemRegisterInfo info in m_dicPopupItemInfo.Values)
            {
                if(info != null)
                    info.FillHoleAction.Invoke(PopupMenuTargetEntityID);
            }
        }

        /// <summary>
        /// 是否所有已注册的用户都填充完菜单数据
        /// </summary>
        /// <returns></returns>
        protected bool IsAllRegisterFillItemData()
        {
            foreach(UPopupItemRegisterInfo info in m_dicPopupItemInfo.Values)
            {
                if(info != null && info.ItemData == null)
                {
                    return false;
                }
            }

            return true;
        }

        /// <summary>
        /// 通知UI将菜单数据进行显示
        /// </summary>
        protected void SendPopupMenuMessage()
        {
            m_PopupMenuAction.Clear();

            foreach (UPopupItemRegisterInfo info in m_dicPopupItemInfo.Values)
            {
                if(info != null && info.ItemData !=null)
                {
                    m_PopupMenuAction.Add(info.ItemData.ItemTypeName, info.ItemData);
                }
            }

            m_MenuData.actionPopup = m_PopupMenuAction;

            EntityView ev = EntityFactory.getEntityViewByID(PopupMenuTargetEntityID);

            if (ev == null)
            {
                ev = EntityFactory.MainHeroView;
                m_MenuData.actionTipPostion = UIWidgets.TipPosition.MouseBottomRightCorner;
            }
            m_MenuData.actionTarget = ev.gameObject.transform;
            

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_POPUPMENU, m_MenuData);
        }

        ///// <summary>
        ///// 接收弹出菜单的内容
        ///// </summary>
        //public void RecivePopupMenuInfo(IntPtr ptr, int nLen)
        //{
        //    m_PopupMenuAction.Clear();
            
        //    IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
        //    cmd_entity_popmenu_target obj = helper.get<cmd_entity_popmenu_target>();
            
        //    int nCount = obj.nCount;

        //    for (int i = 0; i < nCount; ++i)
        //    {
        //        cmd_entity_popmenuinfo_target cmdInfo = helper.get<cmd_entity_popmenuinfo_target>();

        //        m_PopupMenuAction.Add(ULocalizationService.Instance.Get(cmdInfo.szLocalizationFile, cmdInfo.szLocalizationFilter, cmdInfo.szLocalizationKey, string.Empty),
        //            new USpeedUI.UWidgets.UPopupItemDataSelectionInfo(cmdInfo.nSelectInfoID,cmdInfo.nSelectInfoGroupID));
        //    }

        //    m_MenuData.actionPopup = m_PopupMenuAction;
        //    EntityView ev = EntityFactory.getEntityViewByID(obj.nTargetEntityID);

        //    if (ev == null)
        //    {
        //        ev = EntityFactory.MainHeroView;
        //        m_MenuData.actionTipPostion = UIWidgets.TipPosition.MouseBottomRightCorner;
        //    }
        //    m_MenuData.actionTarget = ev.gameObject.transform;

        //    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_POPUPMENU, m_MenuData);

        //}

        ///// <summary>
        ///// 通知逻辑层当前弹出菜单列表选中的选项ID
        ///// </summary>
        ///// <param name="_nSelectID"></param>
        //public void SendPopupMenuSelectInfoID(int _nSelectID)
        //{
        //    EntityEventHelper.instance.SendCommand(EntityFactory.getMainPlayer().ID, EntityLogicDef.ENTITY_CMD_POPUPMENU_SELECTINFOID, _nSelectID);
        //}

        /// <summary>
        /// 通知逻辑层当前右键选中的实体ID
        /// </summary>
        /// <param name="_nEntityID"></param>
        public void SendPopupMenuEntityID(int _nEntityID,UnityAction _actonRelease = null)
        {
            //EntityEventHelper.instance.SendCommand(EntityFactory.getMainPlayer().ID, EntityLogicDef.ENTITY_CMD_POPUPMENU_TARGET, _nEntityID);

            PopupMenuTargetEntityID = _nEntityID;
            m_ReleaseEntityPopupMenu = _actonRelease;

            ClearPopupItemData();

            TimerManager.SetTimer(this, 0, 0.2f, 1);

            InvokeFillHoleAction();
        }

        public void OnTimer(int nTimerID)
        {
            SendPopupMenuMessage();
        }


        UnityAction m_ReleaseEntityPopupMenu = null;
        /// <summary>
        /// PopupMenu 的弹出列表释放时执行
        /// </summary>
        public void onReleaseAllPopupList()
        {
            if(m_ReleaseEntityPopupMenu != null)
            {
                m_ReleaseEntityPopupMenu.Invoke();
            }
        }
        #endregion
    }
}
