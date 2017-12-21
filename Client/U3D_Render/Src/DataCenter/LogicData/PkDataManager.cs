using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using USpeedUI;
using USpeedUI.TooltipAndDialog;
using ASpeedGame.Data.Scheme;
using USpeedUI.Blood;

namespace DataCenter
{
    public class PkDataManager
    {
        // 主角是否在PK状态
        private bool m_bIsPK;
        public bool BIsPK
        {
            get { return m_bIsPK; }
            set { m_bIsPK = value; }
        }

        public void Init()
        {
            pkTargetSkinID = 0;
            pkSrcSkinID = 0;
            m_bIsPK = false;
        }

        private int pkTargetSkinID;
        private int pkSrcSkinID;

        public void onPkDidaShow(IntPtr ptr, int nLen)
        {
            cmd_creature_pk_show_pk_info data = IntPtrHelper.toData<cmd_creature_pk_show_pk_info>(ptr);
            DIDAAddData msg = new DIDAAddData();
            msg.nDIDATime = (int)data.dwTime / 1000;
            msg.nItemID = (int)EDIDAType.EDIDA_PK;
            msg.bAllowRepeatItem = true;
            msg.nButtonIconID = (int)EDIDAType.EDIDA_PK;
            msg.strTooltip = ULocalizationService.Instance.Get("UIView", "PK", "ShowPrompt");

            SideButtonDataPkInfo pkInfo = new SideButtonDataPkInfo();
            pkInfo.uidOperator = data.uidOperator;
            msg.sCustomData = pkInfo;
            msg.callBack = (CustomerInfo) =>
                {
                    SideButtonDataPkInfo customerInfo = CustomerInfo as SideButtonDataPkInfo;
                    UIDialogData toolTipData = new UIDialogData(
                        title: ULocalizationService.Instance.Get("UIView", "PK", "ShowPrompt"),
                        message: data.szInfo,
                        buttons: new UIWidgets.DialogActions() {
                            {ULocalizationService.Instance.Get("UIView", "PK", "Agree"),() => {PkDidaCallBack(customerInfo, 1);return true;}},
                            {ULocalizationService.Instance.Get("UIView", "PK", "Cancel"),() => {PkDidaCallBack(customerInfo, 0);return true;}}
                        });
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, toolTipData);
                };
                    LogicDataCenter.didaDataManager.AddItem(msg);

        }

        public void PkDidaCallBack(SideButtonCustomData sCustomData, int Result)
        {
            //DIDARemoveData msg = new DIDARemoveData();
            //msg.nItemID = (int)EDIDAType.EDIDA_PK;
            //msg.sCustomData = sCustomData;
            //LogicDataCenter.didaDataManager.RemoveItem(msg);

            cmd_creature_pk_handle_info data = new cmd_creature_pk_handle_info();
            SideButtonDataPkInfo pkData = sCustomData as SideButtonDataPkInfo;
            data.uidOperator = pkData.uidOperator;
            data.nAgree = Result;
            EntityEventHelper.Instance.SendCommand<cmd_creature_pk_handle_info>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_PK_HANDLE_INFO, ref data);
        }

        /// <summary>
        /// 开始PK
        /// </summary>
        /// <param name="uidTarget">PK目标</param>
        /// <param name="pkLoc">PK点坐标</param>
        public void StartPK(int uidTarget, float[] pkLoc)
        {
            SkillCtrl sc = EntityFactory.MainHeroView.SkillCtrl;
            if (sc != null)
            {
                sc.doingPKSelection = false;
                sc.enablePKSelection = false;
            }

            PKPosInfo posInfo = new PKPosInfo();
            posInfo.pos = pkLoc;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PKINFO_STARTPK, posInfo);
            UISystem.Instance.ShowWnd(WndID.WND_ID_WAR_FLOATFONTFRAME, true);

            UTopNameManager.Instance.SetPKMode(EntityFactory.MainHeroID, true);
            UTopNameManager.Instance.SetPKMode(uidTarget, true);
            UTopNameManager.PkTargetID = uidTarget;

            U3D_Render.EntityView targetEV = EntityFactory.getEntityViewByID(uidTarget);

            if (targetEV == null || targetEV.Property == null)
            {
                Trace.LogError("pk target ev or pk target ev property null!");
                return;
            }

            UBloodManager.Instance.PkTargetID = uidTarget;
            UBloodManager.Instance.CreatBlood(EntityFactory.MainHeroView, (int)GFxCampTpye.CT_Self);
            UBloodManager.Instance.CreatBlood(targetEV,(int)GFxCampTpye.CT_Enemy);

            pkTargetSkinID = 0;
            pkTargetSkinID = targetEV.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SKIN);

            pkSrcSkinID = 0;
            pkSrcSkinID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SKIN);
            EffectHelper.LoadPreLight(LightType.ActorSkin, pkTargetSkinID, CachePriority.PRIORITY_MEDIUM);

            LoadPKRelateSkin(pkTargetSkinID);
            LoadPKRelateSkin(pkSrcSkinID);
            m_bIsPK = true;
        }

        /// <summary>
        /// 结束PK
        /// </summary>
        /// <param name="uidTarget">PK目标</param>
        /// <param name="nPKResult">PK结果</param>
        public void EndPK(int uidTarget, int nPKResult, int bForceEnd)
        {
            SkillCtrl sc = EntityFactory.MainHero.GetComponent<SkillCtrl>();
            if (sc != null)
            {
                sc.doingPKSelection = false;
                sc.enablePKSelection = true;
            }

            PKResult result = new PKResult();
            result.nResult = nPKResult;
            result.bForceEnd = bForceEnd == 1? true: false;

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PKINFO_ENDPK, result);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_FLOATFONT_UNLOAD, null);

            UTopNameManager.Instance.SetPKMode(EntityFactory.MainHeroID, false);
            UTopNameManager.Instance.SetPKMode(uidTarget, false);
            UTopNameManager.PkTargetID = -1;

            UBloodManager.Instance.PkTargetID = -1;
            UBloodManager.Instance.DestroyALL();

            EffectHelper.UnloadPreLight(LightType.ActorSkin, pkTargetSkinID);
            UnLoadPKRelateSkin(pkTargetSkinID);
            UnLoadPKRelateSkin(pkSrcSkinID);
            pkSrcSkinID = 0;
            pkTargetSkinID = 0;

            m_bIsPK = false;
        }

        private static void LoadPKRelateSkin(int sourceSkin)
        {
            int[] nRelateSkinID = ASpeedGame.Data.PreLoadSkinLayout.PreLoadSkinLayout.Instance.GetRelateSkinID(sourceSkin);
            if (nRelateSkinID != null)
            {
                for (int j = 0; j < nRelateSkinID.Length; ++j)
                {
                    if (nRelateSkinID[j] > 0)
                    {
                        SkinManager.GetSkinAsync(nRelateSkinID[j], OnRelateSkinLoadFinish, AssetLoadPriority.Priority_Exclusive);
                    }
                }
            }
        }

        private static void UnLoadPKRelateSkin(int sourceSkin)
        {
            int[] nRelateSkinID = ASpeedGame.Data.PreLoadSkinLayout.PreLoadSkinLayout.Instance.GetRelateSkinID(sourceSkin);
            if (nRelateSkinID != null)
            {
                for (int j = 0; j < nRelateSkinID.Length; ++j)
                {
                    if (nRelateSkinID[j] > 0)
                    {
                        SkinManager.DeleteSkinAsync(nRelateSkinID[j], OnRelateSkinLoadFinish);
                    }
                }
            }
        }

        private static void OnRelateSkinLoadFinish(Skin sk)
        {
            if (!sk.isValid)
            {
                return;
            }
            SkinInstance go = sk.Instantiate_Main();
            if (go != null)
            {
                SkinManager.CacheSkinInstance(go);
            }
        }
    }
}
