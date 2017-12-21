using Common.FSMSample;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI;

namespace DataCenter
{
    public class FPSBulletDataManager : IModelData, IFSMParam
    {
        public void Init()
        {
            EntityFactory.SetEntityNumPropHandler(OnCommand_UpdateNumProp);
        }

        public void Clear()
        {

        }

        /// <summary>
        /// 当前子弹数量
        /// </summary>
        public int CurBulletCount { get; private set; }
        /// <summary>
        /// 最大子弹数量
        /// </summary>
        public int MaxBulletCount { get;private set; }
        /// <summary>
        /// 子弹百分比
        /// </summary>
        public float BulletCountPercent
        {
            get
            {
                return (float)CurBulletCount / (float)MaxBulletCount;
            }
        }


        public void OnCommand_UpdateNumProp(EntityNumPropEventArgs e)
        {
            if (!e.objev.IsHero || e.objev == null)
                return;

            switch (e.data.nPropID)
            {
                case (int)ENTITY_PROPERTY.PROPERTY_BULLET_COUNT:
                    {
                        CurBulletCount = e.data.nValue;

                        CallFPSBulletViewUpdateData();
                    }
                    break;
            }
        }

        public void SetMaxBulletCount(int _nMaxValue)
        {
            MaxBulletCount = _nMaxValue;
            CallFPSBulletViewUpdateData();
        }

        protected void CallFPSBulletViewUpdateData()
        {
            if (UISystem.Instance.GetWnd(WndID.WND_ID_FPSBULLET).IsVisible())
            {
                UISystem.Instance.SendTargetWndMessage(WndID.WND_ID_FPSBULLET, WndMsgID.WND_MSG_FPSBULLTE_UPDATECOUNT, null);
            }
        }

    }
}
