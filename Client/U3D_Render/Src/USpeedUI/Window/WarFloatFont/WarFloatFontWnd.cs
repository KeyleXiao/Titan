using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UWidgets;

namespace USpeedUI.WarFloatFont
{
    //飘字类型
    public enum WarFloatFontType : int
    {
        Invalid = 0,
        Fatal,
        TrueDamage,
        MagicAttack,
        PhysicalAttack,
        LevelUp,
        Cure,
        Coin,
        Max,
    }


    public class UWarFloatFontWnd : UIBaseWnd<UViewWarFloatFont>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_FLOATFONTFRAME;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override string GetPrefabPath()
        {
            return "DynamicState_Moba/FloatFont/FloatFont";
        }

        public override bool Init()
        {
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_FLOATFONT_UNLOAD, this);
            //订阅进入MOBA消息事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            //订阅离开战场事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            //订阅创建飘字消息事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_FLOATFONT_CREATE, this);
            //订阅移除飘字节点消息事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_FLOATFONT_REMOVEITEM,this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);

            //订阅数值属性回调
            EntityFactory.SetEntityNumPropHandler(ResponseEntityNumPropEvent);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_FLOATFONT_UNLOAD, this);
            //退订进入MOBA消息事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            //退订离开战场事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            //退订创建飘字消息事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_FLOATFONT_CREATE, this);
            //退订移除飘字节点消息事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_FLOATFONT_REMOVEITEM, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            //退订数值属性回调
            EntityFactory.RemoveEntityNumPropHandler(ResponseEntityNumPropEvent);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_WAREND:
                case WndMsgID.WND_MSG_WAR_FLOATFONT_UNLOAD:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        this.LoadView();
                        this.SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_FLOATFONT_CREATE:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }
                        UWarFloatFontData FontData = data as UWarFloatFontData;
                        m_wndView.CreateFloatFont(FontData.Entity,FontData.Text,FontData.Type);
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_FLOATFONT_REMOVEITEM:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }
                        UWarFloatFontItemData fontItemData = data as UWarFloatFontItemData;
                        m_wndView.DisableFloatFontItem(fontItemData.FloatFontItem);
                    }
                    break;
                default:
                    break;
            }
        }

        private void ResponseEntityNumPropEvent(EntityNumPropEventArgs e)
        {
            switch (e.data.nPropID)
            {
                case(int)ENTITY_PROPERTY.PROPERTY_TALENT_POINT:
                    {
                        int deltaNum = e.data.nValue - e.data.nBeforeValue;
                        if (e.data.nEntityID == EntityFactory.MainHeroID && deltaNum > 0)
                        {
                            if (m_wndView != null)
                            {
                                m_wndView.CreateFloatFont(EntityFactory.MainHero, deltaNum.ToString(), WarFloatFontType.Coin);
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class UViewWarFloatFont : UIBaseWndView
    {
        private RectTransform m_Container;
        private WarFloatFontAnim m_Anim;
        private ResNode floatFontItemRes;
        //活动列表，表示当前激活的飘字
        private LinkedList<UWarFloatFontItem> ActiviteList = new LinkedList<UWarFloatFontItem>();
        //非活动列表，表示当前隐藏的飘字
        private LinkedList<UWarFloatFontItem> UnActiviteList = new LinkedList<UWarFloatFontItem>();
        //隐藏距离，当实体和摄像机超过这个距离，就不创建飘字了
        private const float HideInDistance = 30.0f;

        public override bool Init(IUIWnd wnd)
        {
            m_Container = this.GetComponent<RectTransform>();
            m_Anim = this.GetComponent<WarFloatFontAnim>();
            floatFontItemRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/DynamicState_Moba/FloatFont/FloatFontItem");
            return base.Init(wnd);
        }

        /// <summary>
        /// 创建飘字
        /// </summary>piaozi
        /// <param name="entry">实体</param>
        /// <param name="text">飘字内容</param>
        /// <param name="type">飘字类型</param>
        /// <returns></returns>
        public UWarFloatFontItem CreateFloatFont(GameObject entry, String text, WarFloatFontType type)
        {
            UWarFloatFontItem tempItem = null;
            //判断是否需要创建
            if (!NeedToCreate(entry))
                return null;
            if (type == WarFloatFontType.Invalid)
                return null;

            //先从非活动列表取
            if (UnActiviteList.Count > 0)
            {
                tempItem = UnActiviteList.First.Value;
                UnActiviteList.RemoveFirst();

                tempItem.ChangeHostEntity(entry);
                if (!tempItem.ChangeType(type))
                {
                    tempItem.SetEnable(false);
                    tempItem.Clear();
                    UnActiviteList.AddLast(tempItem);
                    return null;
                }
                tempItem.SetText(text);
                if (tempItem.UpdatePos())
                {
                    tempItem.ActivateFont();
                    ActiviteList.AddLast(tempItem);
                }
                else
                {
                    tempItem.SetEnable(false);
                    tempItem.Clear();
                    UnActiviteList.AddLast(tempItem);
                    return null;
                }
            }
            //否则实例化新节点
            else
            {
                tempItem = CreateNewFont();
                if (null == tempItem)
                {
                    return null;
                }
                RectPosition(tempItem);
                tempItem.ChangeHostEntity(entry);
                if (!tempItem.ChangeType(type))
                {
                    tempItem.SetEnable(false);
                    tempItem.Clear();
                    UnActiviteList.AddLast(tempItem);
                    return null;
                }
                tempItem.SetText(text);
                if (tempItem.UpdatePos())
                {
                    tempItem.ActivateFont();
                    ActiviteList.AddLast(tempItem);
                }
                else
                {
                    tempItem.SetEnable(false);
                    tempItem.Clear();
                    UnActiviteList.AddLast(tempItem);
                }
            }
            return tempItem;
        }

        //如果非活动列表里飘字太多，则删一部分，保持在20个
        public override void Update()
        {
            if (UnActiviteList.Count > 20)
            {
                int count = UnActiviteList.Count - 20;
                for (int i = 0; i < count; i++)
                {
                    UWarFloatFontItem temp = UnActiviteList.First.Value;
                    UnActiviteList.RemoveFirst();
                    temp.Clear();
                    var temp1 = temp.gameObject;
                    ResNode.DestroyRes(ref temp1);
                }
            }
        }

        public override void Destroy()
        {
            base.Destroy();

            DestroyALL();
            ActiviteList.Clear();
            UnActiviteList.Clear();
            ActiviteList = null;
            UnActiviteList = null;
            m_Container = null;
            m_Anim = null;
            AssetBundleManager.DeleteAssets(ref floatFontItemRes, true);
        }

        private void DestroyALL()
        {
            if (null != ActiviteList)
            {
                foreach (UWarFloatFontItem temp in ActiviteList)
                {
                    if (temp != null)
                    {
                        temp.Clear();
                        var temp1 = temp.gameObject;
                        ResNode.DestroyRes(ref temp1);
                    }
                }
                foreach (UWarFloatFontItem temp in UnActiviteList)
                {
                    if (temp != null)
                    {
                        temp.Clear();
                        var temp1 = temp.gameObject;
                        ResNode.DestroyRes(ref temp1);
                    }
                }
            }
        }

        //实例化新飘字节点
        private UWarFloatFontItem CreateNewFont()
        {
            if(null == floatFontItemRes)
            {
                return null;
            }
            GameObject go = floatFontItemRes.InstanceMainRes();
            if(null == go)
            {
                return null;
            }
            UWarFloatFontItem uFontItem = go.GetComponent<UWarFloatFontItem>();
            if(null == uFontItem)
            {
                return null;
            }
            else
            {
                uFontItem.SetFloatFontView(this);

                if(m_Anim != null)
                    uFontItem.SetAnimManager(m_Anim);

                return uFontItem;
            }
        }

        //激活飘字节点
        private void EnableFloatFontItem(UWarFloatFontItem fontItem)
        {
            ActiviteList.AddLast(fontItem);
            UnActiviteList.Remove(fontItem);
        }

        //禁用飘字节点
        public void DisableFloatFontItem(UWarFloatFontItem fontItem)
        {
            ActiviteList.Remove(fontItem);
            UnActiviteList.AddLast(fontItem);
            fontItem.SetEnable(false);
            fontItem.Clear();
        }

        //判断是否需要创建飘字
        private bool NeedToCreate(GameObject entry)
        {
            if (entry == null || Initialize.mainCam == null)
                return false;

            float dis = Vector3.Distance(entry.transform.position, Initialize.mainCam.transform.position);
            float CameraDis = SoldierCamera.MainInstance<SoldierCamera>() ? SoldierCamera.MainInstance<SoldierCamera>().CurDistance : 8.0f;
            float hideinDis = HideInDistance + CameraDis;
            if (dis > hideinDis)
            {
                return false;
            }
            return true;
        }

        //重置位置，将新实例化出的节点放在飘字窗口下
        private void RectPosition(UWarFloatFontItem fontItem)
        {
            fontItem.GetRectTransform.SetParent(m_Container, false);
            fontItem.GetRectTransform.localScale = Vector3.one;
            fontItem.GetRectTransform.anchoredPosition = Vector3.zero;
        }
    }
}

