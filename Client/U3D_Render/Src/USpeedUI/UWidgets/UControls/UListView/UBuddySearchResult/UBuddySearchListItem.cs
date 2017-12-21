/*******************************************************************
** �ļ���:	UBuddySearchListView.cs
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2014 - Speed
** ������:	���к�
** ��  ��:	2017/4/11
** ��  ��:	1.0
** ��  ��:	����ϵͳ������������ѽ���б�
** Ӧ  ��:  

**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
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
	/// ����userID
	/// </summary>
	public int UserID;
}