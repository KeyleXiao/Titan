/*******************************************************************
** 文件名:	Utilites.cs 
** 版  权:	NewUIWidgets
** 创建人:	NewUIWidgets
** 日  期:	2016-4-14
** 版  本:	1.8.0
            共用方法集合
		
********************************************************************/
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using Object = UnityEngine.Object;
using System;
using System.Collections;

namespace UIWidgets {

	/// <summary>
	/// Utilites.
	/// </summary>
	static public class Utilites {

		/// <summary>
		/// Fixs the instantiated object (in some cases object have wrong position, rotation and scale).
		/// </summary>
		/// <param name="source">Source.</param>
		/// <param name="instance">Instance.</param>
		static public void FixInstantiated(Component source, Component instance)
		{
			FixInstantiated(source.gameObject, instance.gameObject);
		}

		/// <summary>
		/// Fix the instantiated object (in some cases object have wrong position, rotation and scale).
		/// </summary>
		/// <param name="source">Source.</param>
		/// <param name="instance">Instance.</param>
		static public void FixInstantiated(GameObject source, GameObject instance)
		{
			var defaultRectTransform = source.transform as RectTransform;

			var rectTransform = instance.transform as RectTransform;

            FixInstantiated(instance, defaultRectTransform.localPosition, defaultRectTransform.localRotation,
                           defaultRectTransform.localScale,defaultRectTransform.anchoredPosition,defaultRectTransform.sizeDelta );
		}

        static public void FixInstantiated(GameObject instance, Vector3 localPosition,Quaternion localRoation,
                                           Vector3 localScale, Vector2 anchoredPosition,
                                            Vector2 sizeDelta)
        {
            var rectTransform = instance.transform as RectTransform;

            rectTransform.localPosition = localPosition;
            rectTransform.localRotation = localRoation;
            rectTransform.localScale = localScale;
            rectTransform.anchoredPosition = anchoredPosition;
            rectTransform.sizeDelta = sizeDelta;
        }

		/// <summary>
		/// Finds the canvas.
		/// </summary>
		/// <returns>The canvas.</returns>
		/// <param name="currentObject">Current object.</param>
		static public Transform FindCanvas(Transform currentObject)
		{
			var canvas = currentObject.GetComponentInParent<Canvas>();
			if (canvas==null)
			{
				return null;
			}
			return canvas.transform;
		}

		/// <summary>
		/// Finds the topmost canvas.
		/// </summary>
		/// <returns>The canvas.</returns>
		/// <param name="currentObject">Current object.</param>
		static public Transform FindTopmostCanvas(Transform currentObject)
		{
			var canvases = currentObject.GetComponentsInParent<Canvas>();
			if (canvases.Length==0)
			{
				return null;
			}
			return canvases[canvases.Length - 1].transform;
		}

		/// <summary>
		/// Calculates the drag position.
		/// </summary>
		/// <returns>The drag position.</returns>
		/// <param name="screenPosition">Screen position.</param>
		/// <param name="canvas">Canvas.</param>
		/// <param name="canvasRect">Canvas rect.</param>
		static public Vector3 CalculateDragPosition(Vector3 screenPosition, Canvas canvas, RectTransform canvasRect)
		{
			Vector3 result;
			var canvasSize = canvasRect.sizeDelta;
			Vector2 min = Vector2.zero;
			Vector2 max = canvasSize;
			
			var isOverlay = canvas.renderMode == RenderMode.ScreenSpaceOverlay;
			var noCamera = canvas.renderMode == RenderMode.ScreenSpaceCamera && canvas.worldCamera == null;
			if (isOverlay || noCamera)
			{
				result = screenPosition;
			}
			else
			{
				var ray = canvas.worldCamera.ScreenPointToRay(screenPosition);
				var plane = new Plane(canvasRect.forward, canvasRect.position);
				
				float distance;
				plane.Raycast(ray, out distance);
				
				result = canvasRect.InverseTransformPoint(ray.origin + (ray.direction * distance));
				
				min = - Vector2.Scale(max, canvasRect.pivot);
				max = canvasSize - min;
			}
			
			result.x = Mathf.Clamp(result.x, min.x, max.y);
			result.y = Mathf.Clamp(result.y, min.x, max.y);
			
			return result;
		}

		/// <summary>
		/// Gets the cursor mode.
		/// </summary>
		/// <returns>The cursor mode.</returns>
		static public CursorMode GetCursorMode()
		{
			#if UNITY_WEBGL
			return CursorMode.ForceSoftware;
			#else
			return CursorMode.Auto;
			#endif
		}

		/// <summary>
		/// Updates the layout.
		/// </summary>
		/// <param name="layout">Layout.</param>
		static public void UpdateLayout(LayoutGroup layout)
		{
			if (layout==null)
			{
				return ;
			}
			if (layout is EasyLayout.EasyLayout)
			{
				(layout as EasyLayout.EasyLayout).UpdateLayout();
			}
			else
			{
				layout.CalculateLayoutInputHorizontal();
				layout.SetLayoutHorizontal();
				layout.CalculateLayoutInputVertical();
				layout.SetLayoutVertical();
			}
		}

        /// <summary>Searches all canvases in the scene and tries to find the root one (isRootCanvas TRUE).</summary>
        /// <remarks>NOTE: This method uses FindObjectsOfType(), which is slow. Do not call this method often.</remarks>
        public static Canvas GetRootCanvas()
        {
            Canvas[] canvases = Object.FindObjectsOfType<Canvas>();
            if (canvases.Length == 0)
            {
                Debug.LogError("No canvas found in scene.");
                return null;
            }
            for (int i = 0; i < canvases.Length; i++)
            {
                if (canvases[i].isRootCanvas)
                    return canvases[i];
            }

            Debug.LogError("No canvas found at the root level of the scene.");
            return null;
        }

        public static IEnumerator WaitForFrames(int frameCount)
        {
            while (frameCount > 0)
            {
                frameCount--;
                yield return null;
            }
        }
    }
}
