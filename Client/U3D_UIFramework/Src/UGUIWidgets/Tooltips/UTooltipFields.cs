using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;

namespace UIWidgets
{
    [Serializable]
    public class DynamicImageField
    {
        /// <summary>The user-friendly name of the dynamic image field (Ex: "BodyImage").</summary>
        public string name;
        /// <summary>The placeholder image to replace, required for resetting the image after replacing.</summary>
        public Sprite placeholderSprite;
        /// <summary>The sprite to replace the placeholder with.</summary>
        public Sprite replacementSprite;
    }
    /// <summary>
    /// Tooltip 用到动态图片时，用这个组件挂在对象上
    /// </summary>
    public class DynamicImage : MonoBehaviour
    {
        public string placeholderName;

        [HideInInspector]
        public Image image;

        /// <summary>The user-friendly name of the placeholder field, without the delimiters.</summary>
        public string Name { get; set; }

        /// <summary>Used during Editor time to retrieve the image. Not used during runtime for performance reasons.</summary>
        public Image PlaceholderImage
        {
            get { return GetComponent<Image>(); }
        }
    }


    [Serializable]
    public class DynamicSectionField
    {
        /// <summary>The user-friendly name of the dynamic section field (Ex: "EquippedItemSection").</summary>
        public string name;

        /// <summary>Whether the dynamic section is turned on or off.</summary>
        public bool isOn;
    }
    /// <summary>
    /// tooltip 需要显示额外部件时用这个组件挂在对象上
    /// </summary>
    public class DynamicSection : MonoBehaviour
    {
        public string placeholderName;

        /// <summary>The user-friendly name of the placeholder field, without the delimiters.</summary>
        public string Name { get; set; }
    }


    [Serializable]
    public class ParameterizedTextField
    {
        /// <summary>The user-friendly name of the parameterized text field (Ex: "BodyText").</summary>
        public string name;
        /// <summary>The raw placeholder text, required for resetting the field when hidden (Ex: "%BodyText%").</summary>
        public string placeholder;
        /// <summary>The value to replace the placeholder text with (Ex: "This is some sample body text").</summary>
        public string value;
    }
}
