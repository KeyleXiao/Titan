/*******************************************************************
** 文件名:	UBuddySessionListView.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2017/9/22
** 版  本:	1.0
** 描  述:	好友系统聊天框会话列表项
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


using UnityEngine;
using System.Collections.Generic;
using System.Linq;
using System;

[System.Serializable]
/// <summary>
/// ListViewIcons item description.
/// </summary>
public class UBuddySessionListItem
{
	/// <summary>
	/// The icon.
	/// </summary>
	[SerializeField]
	public Sprite Icon;

	/// <summary>
	/// The name.
	/// </summary>
	[SerializeField]
	public string Name;

	/// <summary>
	/// 好友userID
	/// </summary>
	public int UserID;

    // 是否当前会话
    public bool isCurSession;
}