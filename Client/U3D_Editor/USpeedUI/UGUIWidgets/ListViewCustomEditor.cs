/*******************************************************************
** 文件名:	ListViewCustomEditor.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	
		
********************************************************************/

using UnityEditor;
using UnityEngine;
using System.Collections.Generic;
using System;
using System.Reflection;

namespace UIWidgets
{
	public class ListViewCustomEditor : ListViewCustomBaseEditor
	{
		public ListViewCustomEditor()
		{
			IsListViewCustom = true;
		}
	}
}
