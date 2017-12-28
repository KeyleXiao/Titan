/*******************************************************************
** 文件名:	WarOBDataManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2017 - Speed
** 创建人:	王全申
** 日  期:	2017/12/4
** 版  本:	1.0
** 描  述:	OBUI数据管理
** 应  用:  
********************************************************************/

using GameLogic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using U3D_Render;
using UnityEngine;
using USpeedUI;

namespace DataCenter
{
    public class OB_PersonSpellData
    {
        public cmd_creature_set_slot slotSpell;
        public cmd_set_spell_overlay spellOverlay;
        public cmd_creature_set_slot changeSlot;
        public int initCDTime;
    }
    public class WarOBUIDataManager
    {
        public Dictionary<int, Dictionary<int, OB_PersonSpellData>> OBPersonData = new Dictionary<int, Dictionary<int, OB_PersonSpellData>>();
        public int CurrentUID;
        public void Init()
        {

        }

        public void Clear()
        {
            OBPersonData.Clear();
        }

        private OB_PersonSpellData GetPersonSpellData(int nEntityID, int nSlotID)
        {
            if (!OBPersonData.ContainsKey(nEntityID))
            {
                OBPersonData[nEntityID] = new Dictionary<int, OB_PersonSpellData>();
                OBPersonData[nEntityID][nSlotID] = new OB_PersonSpellData();
            }
            else if (!OBPersonData[nEntityID].ContainsKey(nSlotID))
            {
                OBPersonData[nEntityID][nSlotID] = new OB_PersonSpellData();
            }

            return OBPersonData[nEntityID][nSlotID];
        }

        public void SetSpellSlot(EntityView ev, IntPtr ptrParam)
        {
            CurrentUID = ev.createinfo.EntityID;
            cmd_creature_set_slot data = IntPtrHelper.toData<cmd_creature_set_slot>(ptrParam);
            OB_PersonSpellData spellData = GetPersonSpellData(ev.createinfo.EntityID, data.nSlotIndex);
            spellData.slotSpell = data;
        }

        public void ChangeSlot(EntityView ev, IntPtr ptrParam)
        {
            CurrentUID = ev.createinfo.EntityID;
            cmd_creature_set_slot data = IntPtrHelper.toData<cmd_creature_set_slot>(ptrParam);
            OB_PersonSpellData spellData = GetPersonSpellData(ev.createinfo.EntityID, data.nSlotIndex);
            spellData.changeSlot = data;
            spellData.initCDTime = (int)GameLogicAPI.getTickCount() - (data.slotview.nMaxData - data.slotview.nCurData);
        }

        public void SetSpellOverlay(EntityView ev, IntPtr ptrParam)
        {
            CurrentUID = ev.createinfo.EntityID;
            cmd_set_spell_overlay data = IntPtrHelper.toData<cmd_set_spell_overlay>(ptrParam);
            OB_PersonSpellData spellData = GetPersonSpellData(ev.createinfo.EntityID, data.nSlotIndex);
            spellData.spellOverlay = data;
        }
    }
}
