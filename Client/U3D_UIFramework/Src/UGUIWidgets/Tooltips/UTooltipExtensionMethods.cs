using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using UnityEngine;
using UnityEngine.UI;

namespace UIWidgets
{
    public static class UTooltipExtensionMethods
    {
        /// <summary>Fills a list with string names of parameterized text fields found within an array of Text UI fields.</summary>
        /// <param name="textFields">An array of Text UI fields to scan for the existence of parameterized fields within its text.</param>
        /// <param name="parameterizedTextFields">The list of parameterized text fields to fill. Passing by ref and filling is done for performance, to avoid GC Allocation.</param>
        /// <param name="delimiter">The delimiter to use for finding parameterized fields. Ex: "Hello, %FirstName%!" would have "%" as the delimeter for the FirstName field.</param>
        public static void FillParameterizedTextFields(this Text[] textFields, ref List<ParameterizedTextField> parameterizedTextFields, string delimiter)
        {
            List<string> fieldNames = new List<string>();
            foreach (Text textField in textFields)
            {
                string searchPattern = String.Format("{0}\\w*{0}", delimiter);
                MatchCollection matches = Regex.Matches(textField.text, searchPattern,
                    RegexOptions.Multiline | RegexOptions.IgnoreCase);

                // Add fields that aren't already in the list.
                foreach (Match match in matches)
                {
                    // Trim the field name and add it to the list of names.
                    string fieldName = match.Value.Trim('%');
                    if (!fieldNames.Contains(fieldName))
                        fieldNames.Add(fieldName);

                    // Check to see if this field name is already found in the list of fields passed in.
                    bool fieldFound = false;
                    foreach (ParameterizedTextField field in parameterizedTextFields)
                    {
                        if (fieldName == field.name)
                        {
                            field.placeholder = match.Value;
                            fieldFound = true;
                        }
                    }
                    // If not found, add it.
                    if (!fieldFound)
                        parameterizedTextFields.Add(new ParameterizedTextField() { name = fieldName, placeholder = match.Value, value = String.Empty });
                }
            }

            // Remove any parameterized fields that no longer exist in the associated text fields.
            parameterizedTextFields.RemoveAll(x => !fieldNames.Contains(x.name));
        }

        /// <summary>Fills a list with dynamic image fields found within an array of Image UI fields.</summary>
        /// <param name="imageFields">An array of dynamic image fields to replace.</param>
        /// <param name="dynamicImageFields">The list of dynamic image fields to fill. Passing by ref and filling is done for performance, to avoid GC Allocation.</param>
        /// <param name="delimiter">The delimiter to use for the image placeholder value (this just gets trimmed off, it's just there for visual consistency with the parameterized text fields).</param>
        public static void FillDynamicImageFields(this DynamicImage[] imageFields, ref List<DynamicImageField> dynamicImageFields, string delimiter)
        {
            List<string> fieldNames = new List<string>();

            // Add fields that aren't already in the list.
            foreach (DynamicImage imageField in imageFields)
            {
                // Trim the field name and add it to the list of names.
                string fieldName = imageField.placeholderName.Trim('%');
                if (!fieldNames.Contains(fieldName))
                    fieldNames.Add(fieldName);

                // Get the Image component on this object (all DynamicImage objects should have an associated Image UI component).
                Image placeholderImage = imageField.PlaceholderImage;

                // Check to see if this field name is already found in the list of fields passed in.
                bool fieldFound = false;
                foreach (DynamicImageField field in dynamicImageFields)
                {
                    if (fieldName == field.name)
                        fieldFound = true;
                }
                // If not found, add it.
                if (!fieldFound)
                    dynamicImageFields.Add(new DynamicImageField() { name = fieldName, placeholderSprite = placeholderImage.sprite, replacementSprite = null });
            }

            // Remove any parameterized fields that no longer exist in the associated text fields.
            dynamicImageFields.RemoveAll(x => !fieldNames.Contains(x.name));
        }

        public static void FillDynamicSectionFields(this DynamicSection[] sectionFields, ref List<DynamicSectionField> dynamicSectionFields, string delimiter)
        {
            List<string> fieldNames = new List<string>();

            // Add fields that aren't already in the list.
            foreach (DynamicSection sectionField in sectionFields)
            {
                // Trim the field name and add it to the list of names.
                string fieldName = sectionField.placeholderName.Trim('%');
                if (!fieldNames.Contains(fieldName))
                    fieldNames.Add(fieldName);

                // Get the GameObject reference for this object (this is what we will activate/deactivate dynamically).
                GameObject go = sectionField.gameObject;

                // Check to see if this field name is already found in the list of fields passed in.
                bool fieldFound = false;
                foreach (DynamicSectionField field in dynamicSectionFields)
                {
                    if (fieldName == field.name)
                        fieldFound = true;
                }
                // If not found, add it.
                if (!fieldFound)
                    dynamicSectionFields.Add(new DynamicSectionField() { name = fieldName, isOn = go.activeSelf });
            }

            // Remove any parameterized fields that no longer exist in the associated text fields.
            dynamicSectionFields.RemoveAll(x => !fieldNames.Contains(x.name));
        }

        /// <summary>Sets the position of the tooltip container.</summary>
        public static void SetPosition(this Tooltip tooltip, UTooltipTrigger trigger, Canvas canvas, Camera camera)
        {
            Vector3[] triggerCorners = new Vector3[4];

            GetWorldCorners(trigger.gameObject,canvas, camera, ref triggerCorners);

            // Set the initial tooltip position.
            tooltip.SetPosition(trigger.tipPosition, trigger.tooltipStyle, triggerCorners, tooltip.BackgroundImage, tooltip.RectTransform, canvas, camera);

            // If overflow protection is disabled, exit.
            if (!UTooltipManager.Instance.overflowProtection)
                return;

            UTooltipOverflow overflow;
            GetOverflow(tooltip.RectTransform, canvas, camera, out overflow);

            // If the tooltip overflows its boundary rectange, reposition it.
            if (overflow.IsAny)
                tooltip.SetPosition(overflow.SuggestNewPosition(trigger.tipPosition), trigger.tooltipStyle,
                    triggerCorners, tooltip.BackgroundImage, tooltip.RectTransform, canvas, camera);
        }

        /// <summary>This method does the heavy lifting for setting the correct tooltip position.</summary>
        private static void SetPosition(this Tooltip tooltip, TipPosition tipPosition, TooltipStyle style, Vector3[] triggerCorners, Image bkgImage, RectTransform tooltipRectTrans, Canvas canvas, Camera camera)
        {
            // Tooltip Trigger Corners:
            // 0 = bottom left
            // 1 = top left
            // 2 = top right
            // 3 = bottom right

            Vector3 pos = Vector3.zero;
            Vector3 offsetVector = Vector3.zero;

            switch (tipPosition)
            {
                case TipPosition.TopRightCorner:
                case TipPosition.MouseTopRightCorner:
                    offsetVector = new Vector3(-1 * style.tipOffset, -1 * style.tipOffset);
                    bkgImage.sprite = style.bottomLeftCorner;
                    break;
                case TipPosition.BottomRightCorner:
                case TipPosition.MouseBottomRightCorner:
                    offsetVector = new Vector3(-1 * style.tipOffset, style.tipOffset);
                    bkgImage.sprite = style.topLeftCorner;
                    break;
                case TipPosition.TopLeftCorner:
                case TipPosition.MouseTopLeftCorner:
                    offsetVector = new Vector3(style.tipOffset, -1 * style.tipOffset);
                    bkgImage.sprite = style.bottomRightCorner;
                    break;
                case TipPosition.BottomLeftCorner:
                case TipPosition.MouseBottomLeftCorner:
                    offsetVector = new Vector3(style.tipOffset, style.tipOffset);
                    bkgImage.sprite = style.topRightCorner;
                    break;
                case TipPosition.TopMiddle:
                case TipPosition.MouseTopMiddle:
                    offsetVector = new Vector3(0, -1 * style.tipOffset);
                    bkgImage.sprite = style.topMiddle;
                    break;
                case TipPosition.BottomMiddle:
                case TipPosition.MouseBottomMiddle:
                    offsetVector = new Vector3(0, style.tipOffset);
                    bkgImage.sprite = style.bottomMiddle;
                    break;
                case TipPosition.LeftMiddle:
                case TipPosition.MouseLeftMiddle:
                    offsetVector = new Vector3(style.tipOffset, 0);
                    bkgImage.sprite = style.leftMiddle;
                    break;
                case TipPosition.RightMiddle:
                case TipPosition.MouseRightMiddle:
                    offsetVector = new Vector3(-1 * style.tipOffset, 0);
                    bkgImage.sprite = style.rightMiddle;
                    break;
            }
            SetPosition(tipPosition, triggerCorners, tooltipRectTrans, canvas, camera, ref pos);
            tooltip.GameObject.transform.position = pos;
            tooltipRectTrans.anchoredPosition += (Vector2)offsetVector;
            pos = tooltipRectTrans.anchoredPosition3D;
            pos.z = 0;
            tooltipRectTrans.anchoredPosition3D = pos;
        }

        public static void SetPosition(TipPosition tipPosition,Vector3[] targetRectCorners,RectTransform targetRectTrans, Canvas canvas, Camera camera ,ref Vector3 resultPos)
        {
            // Tooltip Trigger Corners:
            // 0 = bottom left
            // 1 = top left
            // 2 = top right
            // 3 = bottom right

            Vector3 pos = Vector3.zero;
            Vector2 newPivot = Vector2.zero;

            Vector3 offsetVector = Vector3.zero;
            bool useMousePos = tipPosition == TipPosition.MouseBottomLeftCorner || tipPosition == TipPosition.MouseTopLeftCorner || tipPosition == TipPosition.MouseBottomRightCorner || tipPosition == TipPosition.MouseTopRightCorner
                || tipPosition == TipPosition.MouseTopMiddle || tipPosition == TipPosition.MouseLeftMiddle || tipPosition == TipPosition.MouseRightMiddle || tipPosition == TipPosition.MouseBottomMiddle;

            Vector3 mousePos = Input.mousePosition;
            if (canvas.renderMode == RenderMode.ScreenSpaceCamera || canvas.renderMode == RenderMode.WorldSpace)
            {
                mousePos.z = canvas.planeDistance;
                mousePos = camera.ScreenToWorldPoint(mousePos);
            }

            switch (tipPosition)
            {
                case TipPosition.TopRightCorner:
                case TipPosition.MouseTopRightCorner:
                    pos = useMousePos ? mousePos : targetRectCorners[2];
                    newPivot = new Vector2(0, 0);
                    break;
                case TipPosition.BottomRightCorner:
                case TipPosition.MouseBottomRightCorner:
                    pos = useMousePos ? mousePos : targetRectCorners[3];
                    newPivot = new Vector2(0, 1);
                    break;
                case TipPosition.TopLeftCorner:
                case TipPosition.MouseTopLeftCorner:
                    pos = useMousePos ? mousePos : targetRectCorners[1];
                    newPivot = new Vector2(1, 0);
                    break;
                case TipPosition.BottomLeftCorner:
                case TipPosition.MouseBottomLeftCorner:
                    pos = useMousePos ? mousePos : targetRectCorners[0];
                    newPivot = new Vector2(1, 1);
                    break;
                case TipPosition.TopMiddle:
                case TipPosition.MouseTopMiddle:
                    pos = useMousePos ? mousePos : targetRectCorners[1] + (targetRectCorners[2] - targetRectCorners[1]) / 2;
                    newPivot = new Vector2(.5f, 0);
                    break;
                case TipPosition.BottomMiddle:
                case TipPosition.MouseBottomMiddle:
                    pos = useMousePos ? mousePos : targetRectCorners[0] + (targetRectCorners[3] - targetRectCorners[0]) / 2;
                    newPivot = new Vector2(.5f, 1);
                    break;
                case TipPosition.LeftMiddle:
                case TipPosition.MouseLeftMiddle:
                    pos = useMousePos ? mousePos : targetRectCorners[0] + (targetRectCorners[1] - targetRectCorners[0]) / 2;
                    newPivot = new Vector2(1, .5f);
                    break;
                case TipPosition.RightMiddle:
                case TipPosition.MouseRightMiddle:
                    pos = useMousePos ? mousePos : targetRectCorners[3] + (targetRectCorners[2] - targetRectCorners[3]) / 2;
                    newPivot = new Vector2(0, .5f);
                    break;
            }

            resultPos = pos;
            targetRectTrans.pivot = newPivot;
        }

        public static void GetWorldCorners(GameObject targetGameObject, Canvas canvas, Camera camera,ref Vector3[] resultCorners)
        {
            RectTransform triggerRectTrans = targetGameObject.GetComponent<RectTransform>();
            if (triggerRectTrans != null)
                triggerRectTrans.GetWorldCorners(resultCorners);
            else
            {
                // We're not using a trigger from a Canvas, so that means it's a regular world space game object.
                // So, find the collider bounds of the object and use that for the four corners.
                Collider coll = targetGameObject.GetComponent<Collider>();
                Vector3 center = coll.bounds.center;
                Vector3 extents = coll.bounds.extents;

                Vector3 frontBottomLeftCorner = new Vector3(center.x - extents.x, center.y - extents.y, center.z - extents.z);
                Vector3 frontTopLeftCorner = new Vector3(center.x - extents.x, center.y + extents.y, center.z - extents.z);
                Vector3 frontTopRightCorner = new Vector3(center.x + extents.x, center.y + extents.y, center.z - extents.z);
                Vector3 frontBottomRightCorner = new Vector3(center.x + extents.x, center.y - extents.y, center.z - extents.z);
                //这里暂时没处理3D 对象的近大远小问题
                resultCorners[0] = canvas.renderMode == RenderMode.ScreenSpaceOverlay? camera.WorldToScreenPoint(frontBottomLeftCorner) : frontBottomLeftCorner;
                resultCorners[1] = canvas.renderMode == RenderMode.ScreenSpaceOverlay ? camera.WorldToScreenPoint(frontTopLeftCorner) : frontTopLeftCorner;
                resultCorners[2] = canvas.renderMode == RenderMode.ScreenSpaceOverlay ? camera.WorldToScreenPoint(frontTopRightCorner) : frontTopRightCorner;
                resultCorners[3] = canvas.renderMode == RenderMode.ScreenSpaceOverlay ? camera.WorldToScreenPoint(frontBottomRightCorner): frontBottomRightCorner;
            }
        }

        public static void GetOverflow(RectTransform targetTransform,Canvas canvas,Camera camera,out UTooltipOverflow overflow)
        {
            // Check for overflow.
            Vector3[] tooltipCorners = new Vector3[4];
            targetTransform.GetWorldCorners(tooltipCorners);

            if (canvas.renderMode == RenderMode.ScreenSpaceCamera || canvas.renderMode == RenderMode.WorldSpace)
            {
                for (int i = 0; i < tooltipCorners.Length; i++)
                    tooltipCorners[i] = RectTransformUtility.WorldToScreenPoint(camera, tooltipCorners[i]);
            }
            else if (canvas.renderMode == RenderMode.ScreenSpaceOverlay)
            {
                for (int i = 0; i < tooltipCorners.Length; i++)
                    tooltipCorners[i] = RectTransformUtility.WorldToScreenPoint(null, tooltipCorners[i]);
            }

            Rect screenRect = new Rect(0, 0, Screen.width, Screen.height);
            overflow = new UTooltipOverflow
            {
                BottomLeftCorner = !screenRect.Contains(tooltipCorners[0]),
                // is the tooltip out of bounds on the Bottom Left?
                TopLeftCorner = !screenRect.Contains(tooltipCorners[1]),
                // is the tooltip out of bounds on the Top Left?
                TopRightCorner = !screenRect.Contains(tooltipCorners[2]),
                // is the tooltip out of bounds on the Top Right?
                BottomRightCorner = !screenRect.Contains(tooltipCorners[3])
                // is the tooltip out of bounds on the Bottom Right?
            };
        }
    }
}
