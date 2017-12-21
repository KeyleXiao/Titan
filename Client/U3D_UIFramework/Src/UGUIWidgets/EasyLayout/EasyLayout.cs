/*******************************************************************
** 文件名:	EasyLayout.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	EasyLayout元素布局 整合了官方的几种类型的布局
		
********************************************************************/
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using System.Collections.Generic;
using System.Linq;
using System;

namespace EasyLayout {
	/// <summary>
	/// Grid constraints.
	/// Flexible - Don't constrain the number of rows or columns.
	/// FixedColumnCount - Constraint the number of columns to a specified number.
	/// FixedRowCount - Constraint the number of rows to a specified number.
	/// </summary>
	public enum GridConstraints
	{
		Flexible = 0,
		FixedColumnCount = 1,
		FixedRowCount = 2,
	}

	/// <summary>
	/// Padding.
	/// </summary>
	[Serializable]
	public struct Padding
	{
		/// <summary>
		/// The left padding.
		/// </summary>
		[SerializeField]
		public float Left;

		/// <summary>
		/// The right padding.
		/// </summary>
		[SerializeField]
		public float Right;

		/// <summary>
		/// The top padding.
		/// </summary>
		[SerializeField]
		public float Top;

		/// <summary>
		/// The bottom padding.
		/// </summary>
		[SerializeField]
		public float Bottom;

		/// <summary>
		/// Initializes a new instance of the struct.
		/// </summary>
		/// <param name="left">Left.</param>
		/// <param name="right">Right.</param>
		/// <param name="top">Top.</param>
		/// <param name="bottom">Bottom.</param>
		public Padding(float left, float right, float top, float bottom)
		{
			Left = left;
			Right = right;
			Top = top;
			Bottom = bottom;
		}

		/// <summary>
		/// Returns a string that represents the current object.
		/// </summary>
		/// <returns>A string that represents the current object.</returns>
		public override string ToString()
		{
			return String.Format("Padding(left: {0}, right: {1}, top: {2}, bottom: {3})",
				Left,
				Right,
				Top,
				Bottom
			);
		}
	}

	/// <summary>
	/// Children size.
	/// DoNothing - Don't change size of children.
	/// SetPreferred - Set size of children to preferred.
	/// SetMaxFromPreferred - Set size of children to maximum size of children preferred.
	/// FitContainer - Stretch size of children to fit container.
	/// </summary>
	[Flags]
	public enum ChildrenSize {
		DoNothing = 0,
		SetPreferred = 1,
		SetMaxFromPreferred = 2,
		FitContainer = 3,
	}

	/// <summary>
	/// Anchors.
	/// UpperLeft - UpperLeft.
	/// UpperCenter - UpperCenter.
	/// UpperRight - UpperRight.
	/// MiddleLeft - MiddleLeft.
	/// MiddleCenter - MiddleCenter.
	/// MiddleRight - MiddleRight.
	/// LowerLeft - LowerLeft.
	/// LowerCenter - LowerCenter.
	/// LowerRight - LowerRight.
	/// </summary>
	[Flags]
	public enum Anchors {
		UpperLeft = 0,
		UpperCenter = 1,
		UpperRight = 2,
		
		MiddleLeft = 3,
		MiddleCenter = 4,
		MiddleRight = 5,
		
		LowerLeft = 6,
		LowerCenter = 7,
		LowerRight = 8,
	}
	
	/// <summary>
	/// Stackings.
	/// Horizontal - Horizontal.
	/// Vertical - Vertical.
	/// </summary>
	[Flags]
	public enum Stackings {
		Horizontal = 0,
		Vertical = 1,
	}

	/// <summary>
	/// Horizontal aligns.
	/// Left - Left.
	/// Center - Center.
	/// Right - Right.
	/// </summary>
	[Flags]
	public enum HorizontalAligns {
		Left = 0,
		Center = 1,
		Right = 2,
	}

	/// <summary>
	/// Inner aligns.
	/// Top - Top.
	/// Middle - Middle.
	/// Bottom - Bottom.
	/// </summary>
	[Flags]
	public enum InnerAligns {
		Top = 0,
		Middle = 1,
		Bottom = 2,
	}

	/// <summary>
	/// Layout type to use.
	/// Compact - Compact.
	/// Grid - Grid.
	/// </summary>
	[Flags]
	public enum LayoutTypes {
		Compact = 0,
		Grid = 1,
	}

	/// <summary>
	/// EasyLayout.
	/// Warning: using RectTransform relative size with positive size delta (like 100% + 10) with ContentSizeFitter can lead to infinite increased size.
	/// </summary>
	[ExecuteInEditMode]
	[RequireComponent(typeof(RectTransform))]
	[AddComponentMenu("UI/UIWidgets/EasyLayout")]
	public class EasyLayout : UnityEngine.UI.LayoutGroup {
		/// <summary>
		/// The group position.
		/// </summary>
		[SerializeField]
		public Anchors GroupPosition = Anchors.UpperLeft;

		/// <summary>
		/// The stacking type.
		/// </summary>
		[SerializeField]
		public Stackings Stacking = Stackings.Horizontal;
		
		/// <summary>
		/// The type of the layout.
		/// </summary>
		[SerializeField]
		public LayoutTypes LayoutType = LayoutTypes.Compact;

		/// <summary>
		/// Which constraint to use for the Grid layout.
		/// </summary>
		[SerializeField]
		public GridConstraints GridConstraint = GridConstraints.Flexible;

		/// <summary>
		/// How many cells there should be along the constrained axis.
		/// </summary>
		[SerializeField]
		public int GridConstraintCount = 1;

		/// <summary>
		/// The row align.
		/// </summary>
		[SerializeField]
		public HorizontalAligns RowAlign = HorizontalAligns.Left;

		/// <summary>
		/// The inner align.
		/// </summary>
		[SerializeField]
		public InnerAligns InnerAlign = InnerAligns.Top;

		/// <summary>
		/// The cell align.
		/// </summary>
		[SerializeField]
		public Anchors CellAlign = Anchors.UpperLeft;

		/// <summary>
		/// The spacing.
		/// </summary>
		[SerializeField]
		public Vector2 Spacing = new Vector2(5, 5);

		/// <summary>
		/// Symmetric margin.
		/// </summary>
		[SerializeField]
		public bool Symmetric = true;

		/// <summary>
		/// The margin.
		/// </summary>
		[SerializeField]
		public Vector2 Margin = new Vector2(5, 5);

		/// <summary>
		/// The padding.
		/// </summary>
		[SerializeField]
		public Padding PaddingInner = new Padding();

		/// <summary>
		/// The margin top.
		/// </summary>
		[SerializeField]
		public float MarginTop = 5f;

		/// <summary>
		/// The margin bottom.
		/// </summary>
		[SerializeField]
		public float MarginBottom = 5f;

		/// <summary>
		/// The margin left.
		/// </summary>
		[SerializeField]
		public float MarginLeft = 5f;

		/// <summary>
		/// The margin right.
		/// </summary>
		[SerializeField]
		public float MarginRight = 5f;

		/// <summary>
		/// The right to left stacking.
		/// </summary>
		[SerializeField]
		public bool RightToLeft = false;

		/// <summary>
		/// The top to bottom stacking.
		/// </summary>
		[SerializeField]
		public bool TopToBottom = true;

		/// <summary>
		/// The skip inactive.
		/// </summary>
		[SerializeField]
		public bool SkipInactive = true;

		/// <summary>
		/// The filter.
		/// </summary>
		public Func<IEnumerable<GameObject>,IEnumerable<GameObject>> Filter = null;

		/// <summary>
		/// How to control width of the children.
		/// </summary>
		public ChildrenSize ChildrenWidth;

		/// <summary>
		/// How to control height of the children.
		/// </summary>
		public ChildrenSize ChildrenHeight;

		/// <summary>
		/// Control width of children.
		/// </summary>
		[SerializeField]
		[Obsolete("Use ChildrenWidth with ChildrenSize.SetPreferred instead.")]
		public bool ControlWidth;

		/// <summary>
		/// Control height of children.
		/// </summary>
		[SerializeField]
		[Obsolete("Use ChildrenHeight with ChildrenSize.SetPreferred instead.")]
		public bool ControlHeight;

		/// <summary>
		/// Sets width of the chidren to maximum width from them.
		/// </summary>
		[SerializeField]
		[Obsolete("Use ChildrenWidth with ChildrenSize.SetMaxFromPreferred instead.")]
		public bool MaxWidth;
		
		/// <summary>
		/// Sets height of the chidren to maximum height from them.
		/// </summary>
		[SerializeField]
		[Obsolete("Use ChildrenHeight with ChildrenSize.SetMaxFromPreferred instead.")]
		public bool MaxHeight;

		Vector2 _blockSize;

		/// <summary>
		/// Gets or sets the size of the inner block.
		/// </summary>
		/// <value>The size of the inner block.</value>
		public Vector2 BlockSize {
			get {
				return _blockSize;
			}
			protected set {
				_blockSize = value;
			}
		}

		Vector2 _uiSize;
		/// <summary>
		/// Gets or sets the UI size.
		/// </summary>
		/// <value>The UI size.</value>
		public Vector2 UISize {
			get {
				return _uiSize;
			}
			protected set {
				_uiSize = value;
			}
		}

		/// <summary>
		/// Gets the minimum height.
		/// </summary>
		/// <value>The minimum height.</value>
		public override float minHeight
		{
			get
			{
				//CalculateLayoutSize();
				return BlockSize[1];
			}
		}

		/// <summary>
		/// Gets the minimum width.
		/// </summary>
		/// <value>The minimum width.</value>
		public override float minWidth
		{
			get
			{
				//CalculateLayoutSize();
				return BlockSize[0];
			}
		}

		/// <summary>
		/// Gets the preferred height.
		/// </summary>
		/// <value>The preferred height.</value>
		public override float preferredHeight
		{
			get
			{
				//CalculateLayoutSize();
				return BlockSize[1];
			}
		}

		/// <summary>
		/// Gets the preferred width.
		/// </summary>
		/// <value>The preferred width.</value>
		public override float preferredWidth
		{
			get
			{
				//CalculateLayoutSize();
				return BlockSize[0];
			}
		}

		static readonly List<Vector2> groupPositions = new List<Vector2>{
			new Vector2(0.0f, 1.0f),//Anchors.UpperLeft
			new Vector2(0.5f, 1.0f),//Anchors.UpperCenter
			new Vector2(1.0f, 1.0f),//Anchors.UpperRight

			new Vector2(0.0f, 0.5f),//Anchors.MiddleLeft
			new Vector2(0.5f, 0.5f),//Anchors.MiddleCenter
			new Vector2(1.0f, 0.5f),//Anchors.MiddleRight
			
			new Vector2(0.0f, 0.0f),//Anchors.LowerLeft
			new Vector2(0.5f, 0.0f),//Anchors.LowerCenter
			new Vector2(1.0f, 0.0f),//Anchors.LowerRight
		};

		static readonly List<float> rowAligns = new List<float>{
			0.0f,//HorizontalAligns.Left
			0.5f,//HorizontalAligns.Center
			1.0f,//HorizontalAligns.Right
		};
		static readonly List<float> innerAligns = new List<float>{
			0.0f,//InnerAligns.Top
			0.5f,//InnerAligns.Middle
			1.0f,//InnerAligns.Bottom
		};

		/// <summary>
		/// Raises the disable event.
		/// </summary>
        protected override void OnDisable()
		{
			propertiesTracker.Clear();
			base.OnDisable();
		}

		/// <summary>
		/// Raises the rect transform removed event.
		/// </summary>
		void OnRectTransformRemoved()
		{
			SetDirty();
		}

		/// <summary>
		/// Sets the layout horizontal.
		/// </summary>
		public override void SetLayoutHorizontal()
		{
			RepositionUIElements();
		}

		/// <summary>
		/// Sets the layout vertical.
		/// </summary>
		public override void SetLayoutVertical()
		{
			RepositionUIElements();
		}

		/// <summary>
		/// Calculates the layout input horizontal.
		/// </summary>
		public override void CalculateLayoutInputHorizontal()
		{
			base.CalculateLayoutInputHorizontal();
			CalculateLayoutSize();
		}

		/// <summary>
		/// Calculates the layout input vertical.
		/// </summary>
		public override void CalculateLayoutInputVertical()
		{
			CalculateLayoutSize();
		}

		/// <summary>
		/// Gets the length.
		/// </summary>
		/// <returns>The length.</returns>
		/// <param name="ui">User interface.</param>
		/// <param name="scaled">If set to <c>true</c> scaled.</param>
		public float GetLength(RectTransform ui, bool scaled=true)
		{
			if (scaled)
			{
				return Stacking==Stackings.Horizontal ? EasyLayoutUtilites.ScaledWidth(ui) : EasyLayoutUtilites.ScaledHeight(ui);
			}
			return Stacking==Stackings.Horizontal ? ui.rect.width : ui.rect.height;
		}

		/// <summary>
		/// Calculates the size of the group.
		/// </summary>
		/// <returns>The group size.</returns>
		/// <param name="group">Group.</param>
		Vector2 CalculateGroupSize(List<List<RectTransform>> group)
		{
			float width = 0f;
			if (LayoutType==LayoutTypes.Compact)
			{
				for (int i = 0; i < group.Count; i++)
				{
					float row_width = Spacing.x * (group[i].Count - 1);
					for (int j = 0; j < group[i].Count; j++)
					{
						row_width += EasyLayoutUtilites.ScaledWidth(group[i][j]);
					}
					width = Mathf.Max(width, row_width);
				}
			}
			else
			{
				GetMaxColumnsWidths(group, MaxColumnsWidths);
				for (int i = 0; i < MaxColumnsWidths.Count; i++)
				{
					width += MaxColumnsWidths[i];
				}
				width += MaxColumnsWidths.Count * Spacing.x - Spacing.x;
			}

			float height = Spacing.y * (group.Count - 1);
			for (int i = 0; i < group.Count; i++)
			{
				float row_height = 0f;
				for (int j = 0; j < group[i].Count; j++)
				{
					row_height = Mathf.Max(row_height, EasyLayoutUtilites.ScaledHeight(group[i][j]));
				}
				height += row_height;
			}

			width += PaddingInner.Left + PaddingInner.Right;
			height += PaddingInner.Top + PaddingInner.Bottom;

			return new Vector2(width, height);
		}

		/// <summary>
		/// Marks layout to update.
		/// </summary>
		public void NeedUpdateLayout()
		{
			UpdateLayout();
		}

		/// <summary>
		/// Updates the size of the block.
		/// </summary>
		void UpdateBlockSize()
		{
			if (Symmetric)
			{
				BlockSize = new Vector2(UISize.x + Margin.x * 2, UISize.y + Margin.y * 2);
			}
			else
			{
				BlockSize = new Vector2(UISize.x + MarginLeft + MarginRight, UISize.y + MarginTop + MarginBottom);
			}
		}

		/// <summary>
		/// Calculates the size of the layout.
		/// </summary>
		public void CalculateLayoutSize()
		{
			var group = GroupUIElements();
			if (group.Count==0)
			{
				UISize = new Vector2(0, 0);
				UpdateBlockSize();
				
				return ;
			}
			
			UISize = CalculateGroupSize(group);
			UpdateBlockSize();
		}

		/// <summary>
		/// Repositions the user interface elements.
		/// </summary>
		void RepositionUIElements()
		{
			var group = GroupUIElements();
			if (group.Count==0)
			{
				UISize = new Vector2(0, 0);
				UpdateBlockSize();
				
				//LayoutRebuilder.MarkLayoutForRebuild(rectTransform);
				return ;
			}
			
			UISize = CalculateGroupSize(group);
			UpdateBlockSize();
			
			var anchor_position = groupPositions[(int)GroupPosition];
			var start_position = new Vector2(
				rectTransform.rect.width * (anchor_position.x - rectTransform.pivot.x),
				rectTransform.rect.height * (anchor_position.y - rectTransform.pivot.y)
			);
			
			start_position.x -= anchor_position.x * UISize.x;
			start_position.y += (1 - anchor_position.y) * UISize.y;

			start_position.x += GetMarginLeft() * ((1 - anchor_position.x) * 2 - 1);
			start_position.y += GetMarginTop() * ((1 - anchor_position.y) * 2 - 1);
			
			start_position.x += PaddingInner.Left;
			start_position.y -= PaddingInner.Top;

			SetPositions(group, start_position, UISize);
			
			//LayoutRebuilder.MarkLayoutForRebuild(rectTransform);
		}

		/// <summary>
		/// Updates the layout.
		/// </summary>
		public void UpdateLayout()
		{
			CalculateLayoutInputHorizontal();

			RepositionUIElements();
		}

		/// <summary>
		/// Gets the user interface element position.
		/// </summary>
		/// <returns>The user interface position.</returns>
		/// <param name="ui">User interface.</param>
		/// <param name="position">Position.</param>
		/// <param name="align">Align.</param>
		Vector2 GetUIPosition(RectTransform ui, Vector2 position, Vector2 align)
		{
			var pivot_fix_x = EasyLayoutUtilites.ScaledWidth(ui) * ui.pivot.x;
			var pivox_fix_y = EasyLayoutUtilites.ScaledHeight(ui) * ui.pivot.y;
			var new_x = position.x + pivot_fix_x + align.x;
			var new_y = position.y - EasyLayoutUtilites.ScaledHeight(ui) + pivox_fix_y - align.y;
			
			return new Vector2(new_x, new_y);
		}
		
		void GetRowsWidths(List<List<RectTransform>> group, List<float> widths)
		{
			widths.Clear();

			for (int i = 0; i < group.Count; i++)
			{
				float width = 0f;
				for (int j = 0; j < group[i].Count; j++)
				{
					width += EasyLayoutUtilites.ScaledWidth(group[i][j]);
				}
				widths.Add(width + (group[i].Count - 1) * Spacing.x);
			}
		}

		void GetMaxColumnsWidths(List<List<RectTransform>> group, List<float> maxColumnsWidths)
		{
			maxColumnsWidths.Clear();

			for (var i = 0; i < group.Count; i++)
			{
				for (var j = 0; j < group[i].Count; j++)
				{
					if (maxColumnsWidths.Count <= j)
					{
						maxColumnsWidths.Add(0);
					}
					maxColumnsWidths[j] = Mathf.Max(maxColumnsWidths[j], EasyLayoutUtilites.ScaledWidth(group[i][j]));
				}
			}
		}

		void GetColumnsHeights(List<List<RectTransform>> group, List<float> heights)
		{
			heights.Clear();
			
			for (int i = 0; i < group.Count; i++)
			{
				float height = 0;
				for (int j = 0; j < group[i].Count; j++)
				{
					height += EasyLayoutUtilites.ScaledHeight(group[i][j]);
				}
				heights.Add(height + (group[i].Count - 1) * Spacing.y);
			}
		}

		float GetMaxRowHeight(List<RectTransform> row)
		{
			float height = 0;
			for (int i = 0; i < row.Count; i++)
			{
				height = Mathf.Max(height, EasyLayoutUtilites.ScaledHeight(row[i]));
			}
			return height;
		}

		void GetMaxRowsHeights(List<List<RectTransform>> group, List<float> heights)
		{
			heights.Clear();
			var transposed_group = EasyLayoutUtilites.Transpose(group);

			for (int i = 0; i < transposed_group.Count; i++)
			{
				heights.Add(GetMaxRowHeight(transposed_group[i]));
			}
		}

		Vector2 GetMaxCellSize(List<List<RectTransform>> group)
		{
			float x = 0f;
			float y = 0f;
			for (int i = 0; i < group.Count; i++)
			{
				for (int j = 0; j < group[i].Count; j++)
				{
					x = Mathf.Max(x, EasyLayoutUtilites.ScaledWidth(group[i][j]));
					y = Mathf.Max(y, EasyLayoutUtilites.ScaledHeight(group[i][j]));
				}
			}

			return new Vector2(x, y);
		}

		Vector2 GetMaxCellSize(List<RectTransform> row)
		{
			float x = 0f;
			float y = 0f;
			for (int i = 0; i < row.Count; i++)
			{
				x = Mathf.Max(x, EasyLayoutUtilites.ScaledWidth(row[i]));
				y = Mathf.Max(y, EasyLayoutUtilites.ScaledHeight(row[i]));
			}

			return new Vector2(x, y);
		}

		Vector2 GetAlignByWidth(RectTransform ui, float maxWidth, Vector2 cellMaxSize, float emptyWidth)
		{
			if (LayoutType==LayoutTypes.Compact)
			{
				return new Vector2(
					emptyWidth * rowAligns[(int)RowAlign],
					(cellMaxSize.y - EasyLayoutUtilites.ScaledHeight(ui)) * innerAligns[(int)InnerAlign]
				);
			}
			else
			{
				var cell_align = groupPositions[(int)CellAlign];

				return new Vector2(
					(maxWidth - EasyLayoutUtilites.ScaledWidth(ui)) * cell_align.x,
					(cellMaxSize.y - EasyLayoutUtilites.ScaledHeight(ui)) * (1 - cell_align.y)
				);
			}
		}

		Vector2 GetAlignByHeight(RectTransform ui, float maxHeight, Vector2 cellMaxSize, float emptyHeight)
		{
			if (LayoutType==LayoutTypes.Compact)
			{
				return new Vector2(
					(cellMaxSize.x - EasyLayoutUtilites.ScaledWidth(ui)) * innerAligns[(int)InnerAlign],
					emptyHeight * rowAligns[(int)RowAlign]
				);
			}
			else
			{
				var cell_align = groupPositions[(int)CellAlign];
				
				return new Vector2(
					(cellMaxSize.x - EasyLayoutUtilites.ScaledWidth(ui)) * (1 - cell_align.x),
					(maxHeight - EasyLayoutUtilites.ScaledHeight(ui)) * cell_align.y
				);
			}
		}

		List<float> RowsWidths = new List<float>();
		List<float> MaxColumnsWidths = new List<float>();

		List<float> ColumnsHeights = new List<float>();
		List<float> MaxRowsHeights = new List<float>();

		RectTransform currentUIElement;

		void SetPositions(List<List<RectTransform>> group, Vector2 startPosition, Vector2 groupSize)
		{
			if (Stacking==Stackings.Horizontal)
			{
				SetPositionsHorizontal(group, startPosition, groupSize);
			}
			else
			{
				SetPositionsVertical(group, startPosition, groupSize);
			}
		}

		void SetPositionsHorizontal(List<List<RectTransform>> group, Vector2 startPosition, Vector2 groupSize)
		{
			var position = startPosition;

			GetRowsWidths(group, RowsWidths);
			GetMaxColumnsWidths(group, MaxColumnsWidths);

			var align = new Vector2(0, 0);

			for (int coord_x = 0; coord_x < group.Count; coord_x++)
			{
				var row_cell_max_size = GetMaxCellSize(group[coord_x]);

				for (int coord_y = 0; coord_y < group[coord_x].Count; coord_y++)
				{
					currentUIElement = group[coord_x][coord_y];
					align = GetAlignByWidth(currentUIElement, MaxColumnsWidths[coord_y], row_cell_max_size, groupSize.x - RowsWidths[coord_x]);

					var new_position = GetUIPosition(currentUIElement, position, align);
					if (currentUIElement.localPosition.x != new_position.x || currentUIElement.localPosition.y != new_position.y)
					{
						currentUIElement.localPosition = new_position;
					}

					position.x += ((LayoutType==LayoutTypes.Compact)
						? EasyLayoutUtilites.ScaledWidth(currentUIElement)
						: MaxColumnsWidths[coord_y]) + Spacing.x;
				}
				position.x = startPosition.x;
				position.y -= row_cell_max_size.y + Spacing.y;
			}
		}

		void SetPositionsVertical(List<List<RectTransform>> group, Vector2 startPosition, Vector2 groupSize)
		{
			var position = startPosition;

			group = EasyLayoutUtilites.Transpose(group);
			GetColumnsHeights(group, ColumnsHeights);
			GetMaxRowsHeights(group, MaxRowsHeights);
			
			var align = new Vector2(0, 0);
			
			for (int coord_y = 0; coord_y < group.Count; coord_y++)
			{
				var column_cell_max_size = GetMaxCellSize(group[coord_y]);
				
				for (int coord_x = 0; coord_x < group[coord_y].Count; coord_x++)
				{
					currentUIElement = group[coord_y][coord_x];

					align = GetAlignByHeight(currentUIElement, MaxRowsHeights[coord_x], column_cell_max_size, groupSize.y - ColumnsHeights[coord_y]);
					
					var new_position = GetUIPosition(currentUIElement, position, align);
					if (currentUIElement.localPosition.x != new_position.x || currentUIElement.localPosition.y != new_position.y)
					{
						currentUIElement.localPosition = new_position;
					}
					
					position.y -= ((LayoutType==LayoutTypes.Compact)
						? EasyLayoutUtilites.ScaledHeight(currentUIElement)
						: MaxRowsHeights[coord_x]) + Spacing.y;
				}
				position.y = startPosition.y;
				position.x += column_cell_max_size.x + Spacing.x;
			}
		}

		float max_width = -1;
		float max_height = -1;
		void ResizeElements(List<RectTransform> elements)
		{
			propertiesTracker.Clear();

			if (ChildrenWidth==ChildrenSize.DoNothing && ChildrenHeight==ChildrenSize.DoNothing)
			{
				return ;
			}
			if (elements==null)
			{
				return ;
			}
			if (elements.Count==0)
			{
				return ;
			}
			if (LayoutType==LayoutTypes.Grid && GridConstraint!=GridConstraints.Flexible)
			{
				//elements.ForEach(Add2Tracker);
				//return ;
			}

			max_width = (ChildrenWidth==ChildrenSize.SetMaxFromPreferred) ? elements.Select<RectTransform,float>(EasyLayoutUtilites.GetPreferredWidth).Max() : -1f;
			max_height = (ChildrenHeight==ChildrenSize.SetMaxFromPreferred) ? elements.Select<RectTransform,float>(EasyLayoutUtilites.GetPreferredHeight).Max() : -1f;

			elements.ForEach(ResizeChild);
		}

		DrivenRectTransformTracker propertiesTracker;

		void Add2Tracker(RectTransform child)
		{
			DrivenTransformProperties driven_properties = DrivenTransformProperties.None;

			if (ChildrenWidth!=ChildrenSize.DoNothing)
			{
				driven_properties |= DrivenTransformProperties.SizeDeltaX;
			}
			if (ChildrenHeight!=ChildrenSize.DoNothing)
			{
				driven_properties |= DrivenTransformProperties.SizeDeltaY;
			}

			propertiesTracker.Add(this, child, driven_properties);
		}

		void ResizeChild(RectTransform child)
		{
			DrivenTransformProperties driven_properties = DrivenTransformProperties.None;

			if (ChildrenWidth!=ChildrenSize.DoNothing)
			{
				driven_properties |= DrivenTransformProperties.SizeDeltaX;
				var width = (max_width!=-1) ? max_width : EasyLayoutUtilites.GetPreferredWidth(child);
				child.SetSizeWithCurrentAnchors(RectTransform.Axis.Horizontal, width);
			}
			if (ChildrenHeight!=ChildrenSize.DoNothing)
			{
				driven_properties |= DrivenTransformProperties.SizeDeltaY;
				var height = (max_height!=-1) ? max_height : EasyLayoutUtilites.GetPreferredHeight(child);
				child.SetSizeWithCurrentAnchors(RectTransform.Axis.Vertical, height);
			}

			propertiesTracker.Add(this, child, driven_properties);
		}

		bool IsIgnoreLayout(Transform rect)
		{
			#if UNITY_4_6 || UNITY_4_7
			var ignorer = rect.GetComponent(typeof(ILayoutIgnorer)) as ILayoutIgnorer;
			#else
			var ignorer = rect.GetComponent<ILayoutIgnorer>();
			#endif
			return (ignorer!=null) && ignorer.ignoreLayout;
		}

		List<RectTransform> GetUIElements()
		{
			var elements = rectChildren;

			if (!SkipInactive)
			{
				elements = new List<RectTransform>();
				foreach (Transform child in transform)
				{
					if (!IsIgnoreLayout(child))
					{
						elements.Add(child as RectTransform);
					}
				}
			}

			if (Filter!=null)
			{
				var temp = Filter(elements.Convert<RectTransform,GameObject>(GetGameObject));
				var result = temp.Select<GameObject,RectTransform>(GetRectTransform).ToList();

				ResizeElements(result);

				return result;
			}

			ResizeElements(elements);

			return elements;
		}

		GameObject GetGameObject(RectTransform element)
		{
			return element.gameObject;
		}

		RectTransform GetRectTransform(GameObject go)
		{
			return go.transform as RectTransform;
		}

		/// <summary>
		/// Gets the margin top.
		/// </summary>
		public float GetMarginTop()
		{
			return Symmetric ? Margin.y : MarginTop;
		}
		
		/// <summary>
		/// Gets the margin bottom.
		/// </summary>
		public float GetMarginBottom()
		{
			return Symmetric ? Margin.y : MarginBottom;
		}

		/// <summary>
		/// Gets the margin left.
		/// </summary>
		public float GetMarginLeft()
		{
			return Symmetric ? Margin.x : MarginLeft;
		}

		/// <summary>
		/// Gets the margin right.
		/// </summary>
		public float GetMarginRight()
		{
			return Symmetric ? Margin.y : MarginRight;
		}

		void ReverseList(List<RectTransform> list)
		{
			list.Reverse();
		}

		void ClearUIElementsGroup()
		{
			for (int i = 0; i < uiElementsGroup.Count; i++)
			{
				uiElementsGroup[i].Clear();
				ListCache.Push(uiElementsGroup[i]);
			}
			uiElementsGroup.Clear();
		}

		static Stack<List<RectTransform>> ListCache = new Stack<List<RectTransform>>();

		/// <summary>
		/// Gets the List<RectTransform>.
		/// </summary>
		/// <returns>The rect transform list.</returns>
		public List<RectTransform> GetRectTransformList()
		{
			if (ListCache.Count > 0)
			{
				return ListCache.Pop();
			}
			else
			{
				return new List<RectTransform>();
			}
		}

		List<List<RectTransform>> uiElementsGroup = new List<List<RectTransform>>();
		List<List<RectTransform>> GroupUIElements()
		{
			var base_length = GetLength(rectTransform, false);
			base_length -= (Stacking==Stackings.Horizontal) ? (GetMarginLeft() + GetMarginRight()) : (GetMarginTop() + GetMarginBottom());

			var ui_elements = GetUIElements();

			ClearUIElementsGroup();
			if (LayoutType==LayoutTypes.Compact)
			{
				EasyLayoutCompact.Group(ui_elements, base_length, this, uiElementsGroup);

				if (Stacking==Stackings.Vertical)
				{
					uiElementsGroup = EasyLayoutUtilites.Transpose(uiElementsGroup);
				}
			}
			else
			{
				GridConstraintCount = Mathf.Max(1, GridConstraintCount);
				if (GridConstraint==GridConstraints.Flexible)
				{
					EasyLayoutGrid.GroupFlexible(ui_elements, base_length, this, uiElementsGroup);
				}
				else if (GridConstraint==GridConstraints.FixedRowCount)
				{
					if (Stacking==Stackings.Vertical)
					{
						EasyLayoutGrid.GroupByRowsVertical(ui_elements, this, GridConstraintCount, uiElementsGroup);
					}
					else
					{
						EasyLayoutGrid.GroupByRowsHorizontal(ui_elements, this, GridConstraintCount, uiElementsGroup);
					}
				}
				else if (GridConstraint==GridConstraints.FixedColumnCount)
				{
					if (Stacking==Stackings.Vertical)
					{
						EasyLayoutGrid.GroupByColumnsVertical(ui_elements, this, GridConstraintCount, uiElementsGroup);
					}
					else
					{
						EasyLayoutGrid.GroupByColumnsHorizontal(ui_elements, this, GridConstraintCount, uiElementsGroup);
					}
				}
			}

			if (!TopToBottom)
			{
				uiElementsGroup.Reverse();
			}
			
			if (RightToLeft)
			{
				uiElementsGroup.ForEach(ReverseList);
			}

			var width = rectTransform.rect.width - (GetMarginLeft() + GetMarginRight());
			var height = rectTransform.rect.height - (GetMarginTop() + GetMarginBottom());
			if (LayoutType==LayoutTypes.Grid)
			{
				if (ChildrenWidth==ChildrenSize.FitContainer)
				{
					EasyLayoutUtilites.ResizeColumnWidthToFit(width, uiElementsGroup, Spacing.x, PaddingInner.Left + PaddingInner.Right);
				}
				if (ChildrenHeight==ChildrenSize.FitContainer)
				{
					EasyLayoutUtilites.ResizeRowHeightToFit(height, uiElementsGroup, Spacing.y, PaddingInner.Top + PaddingInner.Bottom);
				}
			}
			else
			{
				if (Stacking==Stackings.Horizontal)
				{
					if (ChildrenWidth==ChildrenSize.FitContainer)
					{
						EasyLayoutUtilites.ResizeWidthToFit(width, uiElementsGroup, Spacing.x);
					}
					if (ChildrenHeight==ChildrenSize.FitContainer)
					{
						EasyLayoutUtilites.ResizeRowHeightToFit(height, uiElementsGroup, Spacing.y, PaddingInner.Top + PaddingInner.Bottom);
					}
				}
				else
				{
					if (ChildrenHeight==ChildrenSize.FitContainer)
					{
						EasyLayoutUtilites.ResizeHeightToFit(height, uiElementsGroup, Spacing.y);
					}
					if (ChildrenWidth==ChildrenSize.FitContainer)
					{
						EasyLayoutUtilites.ResizeColumnWidthToFit(width, uiElementsGroup, Spacing.x, PaddingInner.Left + PaddingInner.Right);
					}
				}
			}

			return uiElementsGroup;
		}

		/// <summary>
		/// Awake this instance.
		/// </summary>
        protected override void Awake()
		{
			base.Awake();
			Upgrade();
		}

		[SerializeField]
        protected int version = 0;

		#pragma warning disable 0618
		/// <summary>
		/// Upgrade to keep compatibility between versions.
		/// </summary>
		public virtual void Upgrade()
		{
			//upgrade to 1.6
			if (version==0)
			{
				if (ControlWidth)
				{
					ChildrenWidth = (MaxWidth) ? ChildrenSize.SetMaxFromPreferred : ChildrenSize.SetPreferred;
				}
				if (ControlHeight)
				{
					ChildrenHeight = (MaxHeight) ? ChildrenSize.SetMaxFromPreferred : ChildrenSize.SetPreferred;
				}
			}
			version = 1;
		}
		#pragma warning restore 0618
	}

}
