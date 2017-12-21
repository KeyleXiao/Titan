/*******************************************************************
** 文件名:	ModalHelper.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-19
** 版  本:	1.8.0
** 描  述:	模态基类
		
********************************************************************/

using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using System.Collections.Generic;

namespace UIWidgets
{
	[RequireComponent(typeof(RectTransform))]
	/// <summary>
	/// Modal helper for UI widgets.
	/// <example>modalKey = ModalHelper.Open(this, modalSprite, modalColor);
	/// //...
	/// ModalHelper.Close(modalKey);</example>
	/// </summary>
	public class ModalHelper : MonoBehaviour, ITemplatable, IPointerClickHandler
	{
		bool isTemplate = true;
		
		/// <summary>
		/// Gets a value indicating whether this instance is template.
		/// </summary>
		/// <value><c>true</c> if this instance is template; otherwise, <c>false</c>.</value>
		public bool IsTemplate {
			get {
				return isTemplate;
			}
			set {
				isTemplate = value;
			}
		}
		
		/// <summary>
		/// Gets the name of the template.
		/// </summary>
		/// <value>The name of the template.</value>
		public string TemplateName {
			get;
			set;
		}

		UnityEvent OnClick = new UnityEvent();
		
		static Templates<ModalHelper> Templates = new Templates<ModalHelper>();
		
		static Dictionary<int,ModalHelper> used = new Dictionary<int,ModalHelper>();
		
		static string key = "ModalTemplate";

        public void OnDestroy()
		{
			Templates.Delete(key);
		}

		/// <summary>
		/// Raises the pointer click event.
		/// </summary>
		/// <param name="eventData">Event data.</param>
		public void OnPointerClick(PointerEventData eventData)
		{
            //if (eventData.button!=PointerEventData.InputButton.Left)
            //{
            //    return;
            //}

			OnClick.Invoke();
		}

		/// <summary>
		/// Create modal helper with the specified parent, sprite and color.
		/// </summary>
		/// <param name="parent">Parent.</param>
		/// <param name="sprite">Sprite.</param>
		/// <param name="color">Color.</param>
		/// <param name="onClick">onClick callback</param>
		/// <returns>Modal helper index</returns>
		public static int Open(MonoBehaviour parent, Sprite sprite = null, Color? color = null, UnityAction onClick = null,RectTransform _tModalTarget = null)
		{
			//check if in cache
			if (!Templates.Exists(key))
			{
				Templates.FindTemplates();
				CreateTemplate();
			}
			
			var modal = Templates.Instance(key);
			
			modal.transform.SetParent(Utilites.FindTopmostCanvas(parent.transform), false);
			modal.gameObject.SetActive(true);
			modal.transform.SetAsLastSibling();
			
			var rect = modal.transform as RectTransform;
			rect.sizeDelta = new Vector2(0, 0);
			rect.anchorMin = new Vector2(0, 0);
			rect.anchorMax = new Vector2(1, 1);
			rect.anchoredPosition = new Vector2(0, 0);
			
			var img = modal.GetComponent<ModalGraphic>();
			//if (sprite!=null)
			//{
			//	img.sprite = sprite;
			//}
			if (color!=null)
			{
				img.color = (Color)color;
			}
            else
            {
                img.color = new Color(0, 0, 0, 200f / 255f);
            }
            img.TargetTransform = _tModalTarget;
            img.DoOnceUpdate = true;
            img.OnUpdate();

            modal.OnClick.RemoveAllListeners();
			if (onClick!=null)
			{
				modal.OnClick.AddListener(onClick);
			}
			
			used.Add(modal.GetInstanceID(), modal);
			return modal.GetInstanceID();
		}
		
		/// <summary>
		/// Creates the template.
		/// </summary>
		static void CreateTemplate()
		{
			var template = new GameObject(key);
			
			var modal = template.AddComponent<ModalHelper>();
			template.AddComponent<ModalGraphic>();
			
			Templates.Add(key, modal);

			template.gameObject.SetActive(false);
		}
		
		/// <summary>
		/// Close modal helper with the specified index.
		/// </summary>
		/// <param name="index">Index.</param>
		public static void Close(int index)
		{
			if ((used!=null) && (used.ContainsKey(index)))
			{
				used[index].OnClick.RemoveAllListeners();
				Templates.ToCache(used[index]);
				used.Remove(index);
			}
		}

        /// <summary>
        /// 判断当前是否有模态运行中
        /// </summary>
        /// <returns></returns>
        public static bool IsModalExit()
        {
            return (used != null) && used.Count > 0;
        }
	}

    public class ModalGraphic : MaskableGraphic, UnityEngine.ICanvasRaycastFilter
    {
        public RectTransform TargetTransform;
        public Vector2 center = Vector2.zero;
        public Vector2 size = Vector2.zero;
        public Camera UICamera;
        public Vector2 sizeoffset = new Vector2(4, 4);

        public  bool IsRaycastLocationValid(Vector2 sp, Camera eventCamera)
        {
            bool res = !RectTransformUtility.RectangleContainsScreenPoint(TargetTransform, sp, eventCamera);
            {
                // 点击在目标内部则无效，否则生效
                return res;
            }
        }

        private void OnPopulateMeshTargetTransform(VertexHelper vh)
        {
            Vector4 outer = new Vector4(-rectTransform.pivot.x * rectTransform.rect.width,
                            -rectTransform.pivot.y * rectTransform.rect.height,
                            (1 - rectTransform.pivot.x) * rectTransform.rect.width,
                            (1 - rectTransform.pivot.y) * rectTransform.rect.height);

            Vector4 inner = new Vector4(center.x - size.x / 2,
                                        center.y - size.y / 2,
                                        center.x + size.x * 0.5f,
                                        center.y + size.y * 0.5f);

            vh.Clear();

            var vert = UIVertex.simpleVert;

            // left
            vert.position = new Vector2(outer.x, outer.y);
            vert.color = color;
            vh.AddVert(vert);//0

            vert.position = new Vector2(outer.x, outer.w);
            vert.color = color;
            vh.AddVert(vert);//1

            vert.position = new Vector2(inner.x, outer.w);
            vert.color = color;
            vh.AddVert(vert);//2

            vert.position = new Vector2(inner.x, outer.y);
            vert.color = color;
            vh.AddVert(vert);//3

            // top
            vert.position = new Vector2(inner.x, inner.w);
            vert.color = color;
            vh.AddVert(vert);//4

            vert.position = new Vector2(inner.x, outer.w);
            vert.color = color;
            vh.AddVert(vert);//5

            vert.position = new Vector2(inner.z, outer.w);
            vert.color = color;
            vh.AddVert(vert);//6

            vert.position = new Vector2(inner.z, inner.w);
            vert.color = color;
            vh.AddVert(vert);//7

            // right
            vert.position = new Vector2(inner.z, outer.y);
            vert.color = color;
            vh.AddVert(vert);//8

            vert.position = new Vector2(inner.z, outer.w);
            vert.color = color;
            vh.AddVert(vert);//9

            vert.position = new Vector2(outer.z, outer.w);
            vert.color = color;
            vh.AddVert(vert);//10

            vert.position = new Vector2(outer.z, outer.y);
            vert.color = color;
            vh.AddVert(vert);//11

            // bottom
            vert.position = new Vector2(inner.x, outer.y);
            vert.color = color;
            vh.AddVert(vert);//12

            vert.position = new Vector2(inner.x, inner.y);
            vert.color = color;
            vh.AddVert(vert);//13

            vert.position = new Vector2(inner.z, inner.y);
            vert.color = color;
            vh.AddVert(vert);//14

            vert.position = new Vector2(inner.z, outer.y);
            vert.color = color;
            vh.AddVert(vert);//15


            //left
            vh.AddTriangle(0, 1, 2);
            vh.AddTriangle(2, 3, 0);
            //top
            vh.AddTriangle(4, 5, 6);
            vh.AddTriangle(6, 7, 4);
            //right
            vh.AddTriangle(8, 9, 10);
            vh.AddTriangle(10, 11, 8);
            //bottom
            vh.AddTriangle(12, 13, 14);
            vh.AddTriangle(14, 15, 12);

        }

        protected override void OnPopulateMesh(VertexHelper vh)
        {
            if (TargetTransform != null)
                OnPopulateMeshTargetTransform(vh);
            else
                base.OnPopulateMesh(vh);
        }

        public void OnUpdate()
        {
            // 当引导位置或者大小改变后更新，
            if (TargetTransform != null && (center != TargetTransform.anchoredPosition || size != TargetTransform.sizeDelta))
            {
                this.center = switchToRectTransform(TargetTransform, rectTransform,UICamera);
                this.size = new Vector2(TargetTransform.rect.width- sizeoffset.x, TargetTransform.rect.height- sizeoffset.y);

                SetAllDirty();
            }
        }
        public bool DoOnceUpdate = true;//{ get; set; }

        private void LateUpdate()
        {
            //if (!Vector2.Equals(m_nOldSize,TargetTransform.sizeDelta))
            {
            //    DoOnceUpdate = false;
                OnUpdate();
            }

        }

        public Vector2 switchToRectTransform(RectTransform from, RectTransform to, Camera cam = null)
        {
            Vector2 localPoint;

            Vector2 fromPivotDerivedOffset = new Vector2(
                from.rect.width * from.pivot.x + from.rect.xMin,//- from.rect.center.x, 
                from.rect.height * from.pivot.y + from.rect.yMin);// - from.rect.center.y);
            Vector2 screenP = RectTransformUtility.WorldToScreenPoint(cam, from.position);
            screenP += fromPivotDerivedOffset;
            bool result = RectTransformUtility.ScreenPointToLocalPointInRectangle(to, screenP, cam, out localPoint);

            Vector2 pivotDerivedOffset = new Vector2(to.rect.width * to.pivot.x + to.rect.xMin, to.rect.height * to.pivot.y + to.rect.yMin);
            return to.anchoredPosition + localPoint - pivotDerivedOffset + from.rect.center;
        }

        public void ResetModal()
        {
            DoOnceUpdate = true;
            center = Vector2.zero;
            size = Vector2.zero;
    }
             
    }

}
