/*******************************************************************
** 文件名:	Draggable.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-20
** 版  本:	1.8.0
** 描  述:	可拖拽对象
		
********************************************************************/

using UnityEngine;
using System.Collections;

namespace UIWidgets {

	/// <summary>
	/// Draggable UI object..
	/// </summary>
	[AddComponentMenu("UI/UIWidgets/Draggable")]
	[RequireComponent(typeof(RectTransform))]
	public class Draggable : MonoBehaviour {

		/// <summary>
		/// The handle.
		/// </summary>
		[SerializeField]
        protected GameObject handle;

		DraggableHandle handleScript;

		/// <summary>
		/// If specified, restricts dragging from starting unless the pointerdown occurs on the specified element.
		/// </summary>
		/// <value>The handler.</value>
		public GameObject Handle {
			get {
				return handle;
			}
			set {
				SetHandle(value);
			}
		}

        public void Start()
		{
			SetHandle(handle!=null ? handle : gameObject);
		}

		/// <summary>
		/// Sets the handle.
		/// </summary>
		/// <param name="value">Value.</param>
		protected virtual void SetHandle(GameObject value)
		{
			if (handle)
			{
				Destroy(handleScript);
			}
			handle = value;
			handleScript = handle.AddComponent<DraggableHandle>();
			handleScript.Drag(gameObject.transform as RectTransform);
		}
	}
}
