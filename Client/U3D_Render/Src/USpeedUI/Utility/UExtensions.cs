/*******************************************************************
** 文件名:	UExtensions.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-3-24
** 版  本:	1.0
** 描  述:	扩展类的方法，目前主要用于UI
		
********************************************************************/
using System;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI.UExtensions
{
    public static class GameObjectExtensions
    {
        /// <summary>
        /// 给对象添加TooltipTrigger脚本
        /// </summary>
        /// <param name="_tTrans">要添加Trigger的对象</param>
        /// <returns>TooltipTrigger脚本</returns>
        public static T AddComponent<T>(this GameObject _uGameObject, ETooltipStyle _eStyle) where T : UTooltipTrigger
        {
            T tmpTrigger = null;

            //将类型转为字符串并根据‘_’标识符切割
            string StyleName = _eStyle.ToString().Split('_')[1];
            string TriggerStylePath = UPath.UTooltipStyle + StyleName;

            if (_uGameObject.GetComponent<T>() == null)
            {
                tmpTrigger = _uGameObject.AddComponent<T>();
                ResNode stylePerfabe = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, TriggerStylePath);

                TooltipStyle InstanceTooltipStyle = stylePerfabe.InstanceMainRes<TooltipStyle>();
                TooltipStyle tooltipStyle = null;
                if (!UTooltipManager.Instance.Tooltips.ContainsKey(InstanceTooltipStyle.TooltipID))
                {
                    tooltipStyle = InstanceTooltipStyle;
                    tooltipStyle.name = tooltipStyle.name;
                    tooltipStyle.transform.SetParent(UTooltipManager.Instance.TooltipContainer.transform, false);
                    Tooltip newTooltip = new Tooltip() { GameObject = tooltipStyle.gameObject };
                    newTooltip.Initialize();
                    newTooltip.Deactivate();
                    UTooltipManager.Instance.Tooltips.Add(tooltipStyle.TooltipID, newTooltip);
                }
                else
                {
                    tooltipStyle = UTooltipManager.Instance.Tooltips[InstanceTooltipStyle.TooltipID].TooltipStyle;
                    var temp = InstanceTooltipStyle.gameObject;
                    stylePerfabe.DestroyInstanceRes(ref temp);
                }

                tmpTrigger.tooltipStyle = tooltipStyle;
            }
            else
            {
                tmpTrigger = _uGameObject.GetComponent<T>();
            }

            return tmpTrigger;
        }

        /// <summary>
        /// 移除对象上指定的组件
        /// </summary>
        /// <typeparam name="T">要移除的组件类型</typeparam>
        /// <param name="_tTrans">要移除的组件所在的对象</param>
        public static void RemoveComponent<T>(this GameObject _uGameObject) where T : UnityEngine.Component
        {
            T needRemoveComponent = _uGameObject.GetComponent<T>();
            if (needRemoveComponent != null)
            {
                if (Application.isEditor)
                    UnityEngine.Object.DestroyImmediate(needRemoveComponent, true);
                else
                    ResNode.DestroyRes(ref needRemoveComponent);

            }

        }

        /// <summary>
        /// 更改GameObject和所有子对象的Layer
        /// </summary>
        /// <param name="_uGameObject">需要更改Layer的父对象</param>
        /// <param name="_strLayerName">目标Layer</param>
        public static void ChangeLayersRecursively(this GameObject _uGameObject,string _strLayerName)
        {
            ChangeLayersRecursively(_uGameObject, LayerMask.NameToLayer(_strLayerName));
        }
        /// <summary>
        /// 更改GameObject和所有子对象的Layer
        /// </summary>
        /// <param name="_uGameObject">需要更改Layer的父对象</param>
        /// <param name="_nLayer">目标Layer</param>
        public static void ChangeLayersRecursively(this GameObject _uGameObject ,int _nLayer)
        {
            _uGameObject.layer = _nLayer;
            foreach (Transform child in _uGameObject.transform)
            {
                ChangeLayersRecursively(child.gameObject, _nLayer);
            }
        }

        public static EventHandlerBase AddUISoundTrigger(this GameObject _uGameObject, UnityEngine.EventSystems.EventTriggerType _eTriggerType, string _strFilter, string _strKey)
        {
            UEventTrigger tmpTrigger = null;
            if (_uGameObject.GetComponent<UEventTrigger>() == null)
            {
                tmpTrigger = _uGameObject.AddComponent<UEventTrigger>();
            }
            else
                tmpTrigger = _uGameObject.GetComponent<UEventTrigger>();

            EventHandlerBase handlerBase = tmpTrigger.CreateEventEntryHandler(EEventHandlerType.Sound);
            handlerBase.SetSoundConfig(_strFilter, _strKey);
            tmpTrigger.AddEventEntryHandler(_eTriggerType, handlerBase);

            return handlerBase;
        }
    }

    public static class GraphicExtensions
    {
        public static void AddNewMaterial(this Graphic _gGraphic, EUIShaderType _eType)
        {
            //将类型转为字符串并根据‘_’标识符切割
            string[] SplitTypeNames = _eType.ToString().Split('_');
            string TypeName = string.Empty;
            for (int i = 1; i < SplitTypeNames.Length; i++)
            {
                if (i != 1)
                    TypeName += "/";
                TypeName += SplitTypeNames[i];
            }

            //这个路径不是文件路径，是Shader文件声明的名字
            string ShaderTypePath = UPath.UEffectPath + TypeName;

            Shader TypeShader = AssetBundleManager.GetShader(ShaderTypePath);

            if (TypeShader == null)
            {
                Trace.LogWarning("不能找到名为:" + ShaderTypePath + "的shader");
                return;
            }

            Material mat = new Material(TypeShader);
            //mat.hideFlags = HideFlags.HideAndDontSave;
            _gGraphic.material = mat;
        }

        public static void ResetToDefaultMaterial(this Graphic _gGraphic)
        {
            _gGraphic.material = _gGraphic.defaultMaterial;
        }
    }

    public static class StringExtension
    {
        /// <summary>
        /// 当前字符串转换为使用艺术字体表示
        /// </summary>
        /// <param name="_eStyle">美术字体类型</param>
        /// <returns></returns>
        public static string ConvertToArtistFont(this string _uString, EArtistFont _eStyle)
        {
            char[] charArr = _uString.ToCharArray();
            char[] tmpArr = new char[1];
            string newString = string.Empty;
            StringBuilder strResult = new StringBuilder();

            if (_eStyle < EArtistFont.EAF_Special)
            {
                string tmpString = string.Empty;
                int asciiID = 0;
                for (int i = 0; i < charArr.Length; i++)
                {
#region 目前这里输入的基本都是ascii范围内的，所以这么算就好了,注释的是另一种转换方法，暂时不需要。
                    //字符串 -> 十六
                    //tmpString = ((int)charArr[i]).ToString("x");
                    //十六 -> 十
                    //ten = Convert.ToInt32(tmpString, 16);
                    ////偏移
                    //int offset = ten - (int)EArtistFont.EAF_Defalut;
                    //offset = offset + (int)_eStyle;
                    ////十 -> 十六
                    //tmpString = Convert.ToString(offset, 16);
                    //int charCode = Convert.ToInt32(tmpString, 16);
                    //strResult.Append((char)charCode);
#endregion

                    tmpArr[0] = charArr[i];
                    asciiID = (short)Encoding.UTF8.GetBytes(tmpArr)[0];
                    if ((asciiID >= 48 && asciiID <= 57) || (asciiID == 43) || asciiID == 45)
                    {
                        //偏移
                        int offset = asciiID - (int)EArtistFont.EAF_Defalut;
                        offset = offset + (int)_eStyle;
                        strResult.Append((char)offset);
                    }
                    else
                    {
                        Trace.LogWarning("当前是:" + charArr[i] + "，但艺术字只支持直接输入0-9、+、.和-这几个字符，其余的请使用特殊符号。");
                        continue;
                    }
                }
            }
            else if (_eStyle > EArtistFont.EAF_Special)
            {
                int charCode = (int)_eStyle;
                strResult.Append((char)charCode);
            }
            else
            {
                Trace.LogWarning("EArtistFont.EAF_Special 值只用作分割标识，不执行具体操作。");
            }
            return strResult.ToString();

        }

        /// 判断一个字符串是否为url
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        public static bool IsUrl(ref string _uString)
        {
            try
            {
                //去掉空格
                //去掉首尾'
                _uString.Trim();
                _uString = _uString.TrimStart('\'');
                _uString = _uString.TrimEnd('\'');
                //去掉首尾"
                _uString = _uString.TrimStart('\"');
                _uString = _uString.TrimEnd('\"');

                string Url = @"(((([?](\w)+){1}[=]*))*((\w)+){1}([\&](\w)+[\=](\w)+)*)*";
                return Regex.IsMatch(_uString, Url);
            }
            catch (Exception ex)
            {
                return false;
            }
        }

    }

    public static class TextExtension
    {
        /// <summary>
        /// 切换美术字体
        /// </summary>
        /// <param name="_uText">当前文本</param>
        /// <param name="_eStyle">美术字体类型</param>
        public static void ChangeArtistFontPage(this Text _uText, EArtistFontPage _eStyle)
        {
            string path = UPath.UArtistFontPath + _eStyle.ToString().Split('_')[1];

            ResNode pageFont = AssetBundleManager.GetAssets(AssetType.Asset_Font,path);
            if (pageFont == null)
            {
                Trace.LogWarning("没找到美术字体:" + path + " 使用默认的美术字体");
                pageFont = AssetBundleManager.GetAssets(AssetType.Asset_Font,UPath.UArtistFontPath + "Page0");
            }

            _uText.font = pageFont.ConvertRes<Font>();
        }
    }

    public static class ColorExtension
    {
        public static int HexDigit(char c)
        {
            int Result = 0;

            if (c >= '0' && c <= '9')
            {
                Result = c - '0';
            }
            else if (c >= 'a' && c <= 'f')
            {
                Result = c + 10 - 'a';
            }
            else if (c >= 'A' && c <= 'F')
            {
                Result = c + 10 - 'A';
            }
            else
            {
                Result = 0;
            }


            return Result;
        }

        public static Color32 FromHex(String HexString)
        {
            int StartIndex = (!string.IsNullOrEmpty(HexString) && HexString[0] == '#' ? 1 : 0);

            if (HexString.Length == 3 + StartIndex)
            {
                int R = HexDigit(HexString[StartIndex++]);
                int G = HexDigit(HexString[StartIndex++]);
                int B = HexDigit(HexString[StartIndex]);
                byte byteR = Convert.ToByte((R << 4) + R);
                byte byteG = Convert.ToByte((R << 4) + G);
                byte byteB = Convert.ToByte((R << 4) + B);
                return new Color32(byteR, byteG, byteB, 255);
            }

            if (HexString.Length == 6 + StartIndex)
            {
                byte byteR = Convert.ToByte((HexDigit(HexString[StartIndex + 0]) << 4) + (HexDigit(HexString[StartIndex + 1])));
                byte byteG = Convert.ToByte((HexDigit(HexString[StartIndex + 2]) << 4) + (HexDigit(HexString[StartIndex + 3])));
                byte byteB = Convert.ToByte((HexDigit(HexString[StartIndex + 4]) << 4) + (HexDigit(HexString[StartIndex + 5])));

                return new Color32(byteR, byteG, byteB, 255);
            }

            if (HexString.Length == 8 + StartIndex)
            {
                byte byteR = Convert.ToByte((HexDigit(HexString[StartIndex + 0]) << 4) + (HexDigit(HexString[StartIndex + 1])));
                byte byteG = Convert.ToByte((HexDigit(HexString[StartIndex + 2]) << 4) + (HexDigit(HexString[StartIndex + 3])));
                byte byteB = Convert.ToByte((HexDigit(HexString[StartIndex + 4]) << 4) + (HexDigit(HexString[StartIndex + 5])));
                byte byteA = Convert.ToByte((HexDigit(HexString[StartIndex + 6]) << 4) + (HexDigit(HexString[StartIndex + 7])));

                return new Color32(byteR, byteG, byteB, byteA);
            }

            return new Color32();
        }
    }

    public static class RectTransformExtension
    {
        public static Vector2 switchToRectTransform(this RectTransform from, RectTransform to,Camera cam = null)
        {
            Vector2 localPoint;
            Vector2 fromPivotDerivedOffset = new Vector2(from.rect.width * from.pivot.x + from.rect.xMin, from.rect.height * from.pivot.y + from.rect.yMin);
            Vector2 screenP = RectTransformUtility.WorldToScreenPoint(cam, from.position);
            screenP += fromPivotDerivedOffset;
            RectTransformUtility.ScreenPointToLocalPointInRectangle(to, screenP, cam, out localPoint);
            Vector2 pivotDerivedOffset = new Vector2(to.rect.width * to.pivot.x + to.rect.xMin, to.rect.height * to.pivot.y + to.rect.yMin);
            return to.anchoredPosition + localPoint - pivotDerivedOffset + from.rect.center;
        }

        public static Vector2 ToScreenPoint(this RectTransform from)
        {
            Vector2 fromPivotDerivedOffset = new Vector2(from.rect.width * from.pivot.x + from.rect.xMin, from.rect.height * from.pivot.y + from.rect.yMin);
            Vector2 screenP = RectTransformUtility.WorldToScreenPoint(UISystem.Instance.GetCamera(), from.position);
            screenP += fromPivotDerivedOffset;

            return screenP;
        }
    }
}

