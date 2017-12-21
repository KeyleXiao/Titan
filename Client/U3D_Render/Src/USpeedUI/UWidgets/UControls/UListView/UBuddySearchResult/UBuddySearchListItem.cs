/*******************************************************************
** 文件名:	UBuddySearchListView.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2017/4/11
** 版  本:	1.0
** 描  述:	好友系统主面板搜索好友结果列表
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
public class UBuddySearchListItem
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
}