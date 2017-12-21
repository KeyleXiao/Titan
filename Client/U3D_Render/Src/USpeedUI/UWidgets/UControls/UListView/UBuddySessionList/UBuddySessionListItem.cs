/*******************************************************************
** �ļ���:	UBuddySessionListView.cs
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2014 - Speed
** ������:	���к�
** ��  ��:	2017/9/22
** ��  ��:	1.0
** ��  ��:	����ϵͳ�����Ự�б���
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
	/// ����userID
	/// </summary>
	public int UserID;

    // �Ƿ�ǰ�Ự
    public bool isCurSession;
}