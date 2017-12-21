/*******************************************************************
** 文件名:	RunePageDataManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-8-8
** 版  本:	1.0
** 描  述:	皮肤数据管理
		
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ASpeedGame.Data.RunePage;
using System.Collections;
using GameLogic;
using System.Runtime.InteropServices;
using USpeedUI;
using UnityEngine;
using Data.PlayerLocalHeroConfig;

namespace DataCenter
{
    public class HeroSkinDataManager
    {
        private Dictionary<int, List<cmd_Hero_Skin_Info>> dicSkinInfoData = new Dictionary<int, List<cmd_Hero_Skin_Info>>();

        public void UpdateHeroSkinInfo(cmd_Hero_Skin_Info data)
        {
            int nHeroId = data.nHeroId;
            if (dicSkinInfoData.ContainsKey(nHeroId))
            {
                foreach(cmd_Hero_Skin_Info item in dicSkinInfoData[nHeroId])
                {
                    if (item.nSkinId == data.nSkinId)
                    {
                        dicSkinInfoData[nHeroId].Remove(item);
                        break;
                    }
                }

                dicSkinInfoData[nHeroId].Add(data);
            }
            else
            {
                List<cmd_Hero_Skin_Info> listInfo = new List<cmd_Hero_Skin_Info>();
                listInfo.Add(data);
                dicSkinInfoData[nHeroId] = listInfo;
            }

            PlayerHeroSkinUpdateData msgData = new PlayerHeroSkinUpdateData();
            msgData.nHeroID = nHeroId;
            msgData.nReqPDBID = 0;
            msgData.nSkinID = data.nSkinId;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HEROSKINVIEW_UPDATESKINDATA, msgData);

            //HeroSettingData settingData = new HeroSettingData();
            //settingData.nHeroID = nHeroId;
            //settingData.nDeSkinID = data.nSkinId;
            //HeroSettingConfig.Instance.ModifyLocalData(settingData.nHeroID, settingData);
            PlayerLocalHeroConfig.Instance.SetIntData(ELocalHero.ELH_DeSkinID, nHeroId, data.nSkinId);
        }

        public void InitHeroSkinInfo(IntPtr ptrParam, int nPtrLen)
        {
            if (dicSkinInfoData == null)
            {
                return;
            }

            dicSkinInfoData.Clear();
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_count heroCount = helper.get<cmd_count>();
            for (int i = 0; i < heroCount.nCount; ++i)
            {
                cmd_count talentCount = helper.get<cmd_count>();
                for (int j = 0; j < talentCount.nCount; ++j)
                {
                    cmd_Hero_Skin_Info cmdInfo = helper.get<cmd_Hero_Skin_Info>();
                    int nHeroId = cmdInfo.nHeroId;

                    if (dicSkinInfoData.ContainsKey(cmdInfo.nHeroId))
                    {
                        dicSkinInfoData[nHeroId].Add(cmdInfo);
                    }
                    else
                    {
                        List<cmd_Hero_Skin_Info> listInfo = new List<cmd_Hero_Skin_Info>();
                        listInfo.Add(cmdInfo);
                        dicSkinInfoData[nHeroId] = listInfo;
                    }
                }
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HEROSKINVIEW_INITSKINDATA, null);
        }


        public Dictionary<int, List<cmd_Hero_Skin_Info>> HeroSkinData
        {
            set { dicSkinInfoData = value; }
            get { return dicSkinInfoData; }
        }

        public bool GetHeroSkinInfo(int nHeroID,int nSkinID,ref cmd_Hero_Skin_Info data)
        {
            if (!dicSkinInfoData.ContainsKey(nHeroID))
            {
                return false;
            }

            foreach (cmd_Hero_Skin_Info item in dicSkinInfoData[nHeroID])
            {
                if(item.nSkinId == nSkinID)
                {
                    data = item;
                    return true;
                }
            }

            return false;
        }

        public bool CheckIsHaveSkin(int nHeroID, int nSkinID)
        {
            List<cmd_Hero_Skin_Info> info = null;
            if (!dicSkinInfoData.TryGetValue(nHeroID, out info))
            {
                return false;
            }

            if (info == null)
                return false;

            foreach (cmd_Hero_Skin_Info item in info)
            {
                if (item.nSkinId == nSkinID)
                {
                    return true;
                }
            }

            return false;
        }

        public void Clear()
        {
            dicSkinInfoData.Clear();
        }
    }
}
