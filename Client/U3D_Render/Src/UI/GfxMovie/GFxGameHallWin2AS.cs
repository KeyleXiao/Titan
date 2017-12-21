/*******************************************************************
** 文件名:	GFxGameHallWin2AS.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	吴欣伟
** 日  期:	2015/09/28
** 版  本:	1.0
** 描  述:	GFx 游戏大厅界面, 拆分C#代码中与AS通讯的逻辑
				
** 应  用:  	对应Flash Layout文件：GameHallWin.swf/gfx

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;
using Scaleform;
using GameLogic;

public partial class GFxGameHallWin
{
    #region C# -> AS 
    public void SetRoomItemData(Value _item,cmd_match_hall_add_room _data)
    {
        string strIndex,strRoomName,strGameMode,strMapName,strPlayerNum,strRoomState;
        uint nMapID;

        strIndex = _data.nRoomIndex.ToString(); 
        strRoomName = _data.szRoomName;
        strGameMode = _data.nRoomMode.ToString();
        strMapName = _data.szMapName;
        strPlayerNum = _data.nCurPlayerNum + "/"+_data.nMaxPlayerNum;
        strRoomState = "0/2";
        nMapID = (uint)_data.nMapID;

        if (_item.Invoke("SetRoomItem", strIndex, strRoomName, strGameMode, strMapName, strPlayerNum, strRoomState, nMapID) == null)
        {
            Trace.LogError("call AS:SetRoomItem faild");
        }
    }
    /// <summary>
    /// 房间属性添加进房间列表
    /// </summary>
    /// <param name="data"></param>
    public void AddHallRoomItem(cmd_match_hall_add_room _data)
    {
        if (!Value.IsValueValid(vRoomItemStruct) || !Value.IsValueValid(vRoomListFrame))
            return;

        vRoomItemStruct.ClearElements();
        vRoomItemStruct.SetElement(0, _data.nRoomIndex);
        vRoomItemStruct.SetElement(1, _data.szRoomName);
        vRoomItemStruct.SetElement(2, _data.szMode);
        vRoomItemStruct.SetElement(3, _data.szMapName);
        string strPlayerNum = _data.nCurPlayerNum + "/" + _data.nMaxPlayerNum;
        vRoomItemStruct.SetElement(4, strPlayerNum);
        string strRoomState = _data.szState;
        vRoomItemStruct.SetElement(5, strRoomState);
        vRoomItemStruct.SetElement(6, _data.nMapID);
        vRoomItemStruct.SetElement(7, _data.szMapDes);

        if (vRoomListFrame.Invoke("AddHallRoomItem") == null)
        {
            Trace.LogError("call AddHallRoomItem failed!!");
        }

    }
    public void FreshHallRoomItem(cmd_match_hall_add_room _data)
    {
        if (!Value.IsValueValid(vRoomItemStruct) || !Value.IsValueValid(vRoomListFrame))
            return;

        vRoomItemStruct.ClearElements();
        vRoomItemStruct.SetElement(0, _data.nRoomIndex);
        vRoomItemStruct.SetElement(1, _data.szRoomName);
        vRoomItemStruct.SetElement(2, _data.szMode);
        vRoomItemStruct.SetElement(3, _data.szMapName);
        string strPlayerNum = _data.nCurPlayerNum + "/" + _data.nMaxPlayerNum;
        vRoomItemStruct.SetElement(4, strPlayerNum);
        string strRoomState = _data.szState;
        vRoomItemStruct.SetElement(5, strRoomState);
        vRoomItemStruct.SetElement(6, _data.nMapID);
        vRoomItemStruct.SetElement(7, _data.szMapDes);

        if (vRoomListFrame.Invoke("FreshHallRoomItem") == null)
        {
            Trace.LogError("call FreshHallRoomItem error!!");
        }
    }
    public void RemoveHallRoomAllItem()
    {
        if (!Value.IsValueValid(vRoomListFrame))
            return;
        if (vRoomListFrame.Invoke("RemoveHallRoomAllItem") == null)
        {
            Trace.LogError("call RemoveHallRoomAllItem failed");
        }
    }
    public void RemoveHallRoomItem(int _index)
    {
        if (!Value.IsValueValid(vRoomListFrame))
            return;
        if (vRoomListFrame.Invoke("RemoveHallRoomItem", _index) == null)
        {
            Trace.LogError("call RemoveHallRoomItem failed");
            return;
        }
    }
    public void SetHallMapInfo(cmd_match_create_room_map_info _data)
    {
        if (!Value.IsValueValid(vMapFrame))
            return;

        string mapName = _data.szPicPath;
        string mapDes = _data.szDesc;

        if (vMapFrame.Invoke("SetHallMapInfo", mapName, mapDes) == null)
        {
            Trace.LogError("call SetHallMapInfo failed");
            return;
        }
    }

    #endregion
}
