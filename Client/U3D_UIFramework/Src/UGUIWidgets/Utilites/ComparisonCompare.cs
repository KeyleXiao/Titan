/*******************************************************************
** 文件名:	ComparisonComparer.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	对象比较方法
		
********************************************************************/

using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using System.ComponentModel;

namespace UIWidgets {
	public class ComparisonComparer<T> : IComparer<T>
	{
		readonly Comparison<T> comparison;
		
		public ComparisonComparer(Comparison<T> newComparison)
		{
			if (newComparison==null)
			{
				throw new ArgumentNullException("newComparison");
			}
			comparison = newComparison;
		}
		
		public int Compare(T x, T y)
		{
			return comparison(x, y);
		}
	}
}
