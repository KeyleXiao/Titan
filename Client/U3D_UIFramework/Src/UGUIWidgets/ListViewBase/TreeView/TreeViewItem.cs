/*******************************************************************
** 文件名:	TreeViewItem.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
** 描  述:	树形列表显示对象基类，主要是Toggle功能的实现（开关动画，事件分发)
		
********************************************************************/

using UnityEngine;
using System.ComponentModel;

namespace UIWidgets {
	[System.Serializable]
	/// <summary>
	/// Tree view item.
	/// </summary>
	public class TreeViewItem : IObservable, INotifyPropertyChanged {
		/// <summary>
		/// OnChange event.
		/// </summary>
		public event OnChange OnChange;

		/// <summary>
		/// Occurs when a property value changes.
		/// </summary>
		public event PropertyChangedEventHandler PropertyChanged;

		/// <summary>
		/// The icon.
		/// </summary>
		[SerializeField]
        protected Sprite icon;

		/// <summary>
		/// Gets or sets the icon.
		/// </summary>
		/// <value>The icon.</value>
		public Sprite Icon {
			get {
				return icon;
			}
			set {
				icon = value;
				Changed("Icon");
			}
		}
		
		/// <summary>
		/// The name.
		/// </summary>
		[SerializeField]
        protected string name;

		/// <summary>
		/// Gets or sets the name.
		/// </summary>
		/// <value>The name.</value>
		public string Name {
			get {
				return name;
			}
			set {
				name = value;
				Changed("Name");
			}
		}

		[System.NonSerialized]
		string localizedName;

		/// <summary>
		/// The localized name.
		/// </summary>
		public string LocalizedName {
			get {
				return localizedName;
			}
			set {
				localizedName = value;
				Changed("LocalizedName");
			}
		}

		[SerializeField]
        protected int _value;
		
		/// <summary>
		/// Gets or sets the value.
		/// </summary>
		/// <value>The value.</value>
		public int Value {
			get {
				return _value;
			}
			set {
				_value = value;
				Changed("Value");
			}
		}

		[SerializeField]
        protected object tag;
		
		/// <summary>
		/// Gets or sets the tag.
		/// </summary>
		/// <value>The tag.</value>
		public object Tag {
			get {
				return tag;
			}
			set {
				tag = value;
				Changed("Tag");
			}
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="UIWidgets.TreeViewItem"/> class.
		/// </summary>
		/// <param name="itemName">Item name.</param>
		/// <param name="itemIcon">Item icon.</param>
		public TreeViewItem(string itemName, Sprite itemIcon = null)
		{
			name = itemName;
			icon = itemIcon;
		}

		protected void Changed(string propertyName = "")
		{
			if (OnChange!=null)
			{
				OnChange();
			}
			if (PropertyChanged!=null)
			{
				PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
			}
		}
	}
}
