/*******************************************************************
** 文件名:	ULocalizedText.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-3-30
** 版  本:	1.0
** 描  述:	本地化系统 将ULabel的text内容根据语言表进行本地化
		
********************************************************************/


using USpeedUI;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI
{
    [RequireComponent(typeof(Text))]
    public class ULocalizedText : MonoBehaviour
    {
        [HideInInspector]
        public string File;

        [HideInInspector]
        public string Filter;

        [HideInInspector]
        public string Key;

        [HideInInspector]
        public string Value;

        public string Fallback;

        public Text LocalizedText;

        /// <summary>
        /// 是否需要在Start()时取一下本地化数据
        /// 执行了Init方法并且已经获取了Value值可以不需要
        /// </summary>
        protected bool m_bStartReadLocal = true;

        public void Awake()
        {
            ULocalizationService.OnLanguageChanged += OnLocalization;
        }

        public void Start()
        {
            if(LocalizedText == null)
                LocalizedText = GetComponent<Text>();


            OnLocalization(ULocalizationService.Instance);

        }

        public void OnDisable()
        {
            m_bStartReadLocal = true;
        }

        public void OnDestroy()
        {
            m_bStartReadLocal = true;
            ULocalizationService.OnLanguageChanged -= OnLocalization;
        }

        public void OnLocalization(ULocalizationService localization)
        {
            if (LocalizedText == null || localization == null || localization.LocalizeDataTable== null)
                return;

            LocalizedText.text = localization.Get(File, Filter, Key, Fallback);
        }

        public void Init(Text _txt,string _strFile,string _strFilter,string _strKey,string _strValue,string _strFallback)
        {
            LocalizedText = _txt;
            File = _strFile;
            Filter = _strFilter;
            Key = _strKey;
            Value = _strValue;
            Fallback = _strFallback;
            m_bStartReadLocal = string.IsNullOrEmpty(_strValue);
        }
    }
}
