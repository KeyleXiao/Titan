/*******************************************************************
** 文件名:	ULocalizationService.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-3-30
** 版  本:	1.0
** 描  述:	本地化系统中心管理区，配置脚本加载，切换等等
		
********************************************************************
**************************** 修改记录 ******************************
** 修改人: 林建聪
** 日  期: 2016.12.29
** 描  述: 本地化配置文件发布版将会走MPK包，不方便查找多语言相关的文件来注册。
            所以，多语言相关的文件的文件名都写在LocalizationConfig.csv 最后一部分以方便注册。
           
********************************************************************/

using HeTuGame.Data;
using System.Collections.Generic;
using System.Xml;
using USpeedUI;
using System;
using System.Collections;
using System.IO;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI
{
    
    public partial class ULocalizationService : Singleton<ULocalizationService>
    {  

        /// <summary>
        /// 加载本地化配置表
        /// </summary>
        /// <param name="_strXmlPath"></param>
        public void Load(string _strConfigPath)
        {
            //不是有xml配置
            //if (xmlDocument == null)
            //{
            //    xmlDocument = new XmlDocument();
            //    xmlDocument.Load(_strXmlPath);
            //    Load(xmlDocument);
            //}
            if(allCSVFiles == null || allCSVFiles.Length == 0)
            {
                Load_LocalizationCSVConfig(_strConfigPath);
            }
        }
        /// <summary>
        /// 卸载所有本地化配置表
        /// </summary>
        public void Unload()
        {
            OnLanguageChangedReRead -= OnLanguageRead;
            Languages.Clear();
            //xmlDocument = null;
            //Array.Clear(allCSVFiles, 0, allCSVFiles.Length);
            LanguagesCSVFiles.Clear();
            LocalizeDataTable.Clear();
        }
        /// <summary>
        /// 重加载同一种语言
        /// </summary>
        public void ReLoad()
        {
            ULanguageInfo oldLanguage = Language;
            Language = Languages[Languages.Count-1];
            Language = oldLanguage;
        }

        /// <summary>
        /// 获取指定的本地化数据
        /// </summary>
        /// <param name="_strFile">数据所存在的文件</param>
        /// <param name="_strFilter">数据所存在的组</param>
        /// <param name="_strKey">数据的Key</param>
        /// <param name="_strFallback">没找到指定数据时返回的值</param>
        /// <returns>本地化数据</returns>
        public string Get(string _strFile,string _strViewFilter,string _strKey,string _strFallback)
        {
            if(LocalizeDataTable == null)
            {
                Trace.LogWarning("是否没加载本地化数据表？");
                return _strFallback;
            }
            if (!LocalizeDataTable.ContainsKey(_strFile))
            {
                //ReadCSVFile(_strFile);
                ReadCSVMPKFile(_strFile);
            }
            if( !LocalizeDataTable[_strFile].ContainsKey(_strViewFilter) ||
                !LocalizeDataTable[_strFile][_strViewFilter].ContainsKey(_strKey))
            {
                Debug.LogWarning("没找到用于本地化的数据：" + _strFile + "->" + _strViewFilter + "->" + _strKey);
                return _strFallback;
            }
            return LocalizeDataTable[_strFile][_strViewFilter][_strKey];
        }
        public string Get(string _strFile,string _strViewFilter,string _strKey)
        {
            return Get(_strFile, _strViewFilter, _strKey, string.Empty);
        }
        /// <summary>
        /// 该方法会给Text添加一个本地化控制脚本，方便在不关闭游戏情况下切换本地化数据
        /// </summary>
        public string Get(Text _txt, string _strFile,string _strViewFilter,string _strKey,string _strFallback="")
        {
            string result = Get(_strFile, _strViewFilter, _strKey, _strFallback); 
            if (_txt != null)
            {
                ULocalizedText localizedTxt = _txt.GetComponent<ULocalizedText>();
                if (localizedTxt == null)
                    localizedTxt = _txt.gameObject.AddComponent<ULocalizedText>();
                localizedTxt.Init(_txt,_strFile, _strViewFilter, _strKey, result, _strFallback);
            }
            return result;
        }
        
        /// <summary>
        /// 当前语言发生改变，重新读取需要的语言文件
        /// </summary>
        public static event Action<ULocalizationService> OnLanguageChangedReRead;

        /// <summary>
        /// 当前语言发生改变，所有需要用到多语言的地方重新设置
        /// </summary>
        public static event Action<ULocalizationService> OnLanguageChanged;


        #region 本地化配置表
        private XmlDocument xmlDocument;
        /// <summary>
        /// 本地化配置表里已配置的语言列表
        /// </summary>
        public List<ULanguageInfo> Languages = new List<ULanguageInfo> ();
        /// <summary>
        /// 多语言配置文件名
        /// </summary>
        public List<string> LanguagesCSVFiles = new List<string>();
        private ULanguageInfo _language = null;

        /// <summary>
        /// 当前使用的配置语言
        /// </summary>
        public ULanguageInfo Language
        {
            get { return _language; }
            set
            {
                if (_language == value)
                    return;
                if (!HasLanguage(value))
                {
                    UnityEngine.Debug.LogError("Invalid Language " + value);
                    return;
                }

                _language = value;
                if(_language.Name != String.Empty)   
                    RaiseLanguageChanged();
            }
        }
        public ULocalizationService()
        {
            OnLanguageChangedReRead += OnLanguageRead;
        }
        /// <summary>
        /// 判断该语言是否已配置
        /// </summary>
        /// <param name="language">需要判断的语言</param>
        /// <returns>true该语言已配置</returns>
        public bool HasLanguage(ULanguageInfo language)
        {
            foreach (var systemLanguage in Languages)
            {
                if (systemLanguage.Equals(language))
                    return true;
            }
            return false;
        }
        private void Load(XmlDocument _docXML)
        {
            if(_docXML != null)
            {
                XmlNodeList NodeList = _docXML.SelectNodes(LanguageElement);
                LoadAllLanguages(NodeList);
            }
        }
        private void LoadAllLanguages(XmlNodeList _nodeList)
        {
            var d = ULanguageInfo.zhCn;
            Languages.Clear();
            foreach (XmlNode node in _nodeList)
            {
                var language = new ULanguageInfo(GetNameAttribute(node));

                Languages.Add(language);
                //是否是默认语言
                if (GetDefaultAttribute(node))
                {
                    d = language;
                }
            }
            Language = d;
        }
        /// <summary>
        /// 语言表改变时分发事件 分别处理重新加载文件与重新设置文本内容
        /// </summary>
        private void RaiseLanguageChanged()
        {
            //if (Application.isPlaying)
            {
                if (OnLanguageChangedReRead != null)
                    OnLanguageChangedReRead(this);

                if (OnLanguageChanged != null)
                    OnLanguageChanged(this);
            }
        }
        #endregion

        #region 语言表
        //存储语言文件夹内所有配置表信息
        //<File,<Filter<Key,Value>>>
        public Dictionary<string,Dictionary<string,Dictionary<string,string>>> LocalizeDataTable { get;private set; }

        /// <summary>
        /// 当前语言改变时，根据事件读取语言表
        /// </summary>
        private void ReadXMLFolder()
        {
            LocalizeDataTable = new Dictionary<string, Dictionary<string, Dictionary<string, string>>>();
            DirectoryInfo rootDirInfo;
            //根据当前语言版本获取语言文件根目录

            if (GameLogicConfigApi.LanguageDirName != string.Empty && Application.isPlaying)
            {
                rootDirInfo = new DirectoryInfo(ViewConfigManager.DataScpPath + "UI/");
            }
            else
            {
                string path = ViewConfigManager.DataScpPath;
                if (GameLogicConfigApi.LanguageDirName != string.Empty)
                {
                    int index = ViewConfigManager.DataScpPath.LastIndexOf(GameLogicConfigApi.LanguageDirName);
                    path = ViewConfigManager.DataScpPath.Substring(0, index);
                }
                    

               rootDirInfo = new DirectoryInfo(path + Language.Name + "/UI/");
            }
            


            if (rootDirInfo == null || !rootDirInfo.Exists)
            {
                UnityEngine.Debug.LogError("没找到语言文件夹:" + Language.Name);
                return;
            }
            //遍历该目录下所有xml文件
            FileInfo[] allXmlFiles = rootDirInfo.GetFiles("*.xml");
            if(allXmlFiles.Length == 0)
            {
                UnityEngine.Debug.LogError(Language.Name+"文件夹下没有xml配置文件...");
                return;
            }
            foreach (FileInfo file in allXmlFiles)
            {
                ReadXMLFile(file);
            }
        }
        private void ReadXMLFile(FileInfo _fileInfo)
        {
            string file,filter, key, value;

            file = _fileInfo.Name.Split('.')[0];
            LocalizeDataTable.Add(file, new Dictionary<string, Dictionary<string, string>>());
            XmlDocument tmpXMLDocument = new XmlDocument();
            tmpXMLDocument.Load(_fileInfo.FullName);
            XmlNodeList FilterNodeList = tmpXMLDocument.SelectNodes(FilterElement);

            XmlNodeList SymbolsNodeList;
            foreach (XmlNode filterNode in FilterNodeList)
            {
                filter = GetKeyAttribute(filterNode);
                LocalizeDataTable[file].Add(filter, new Dictionary<string, string>());
                SymbolsNodeList = filterNode.SelectNodes(SymbolsElement);
                foreach (XmlNode symbolsNode in SymbolsNodeList)
                {
                    key = GetKeyAttribute(symbolsNode);
                    value = GetValueAttribute(symbolsNode);
                    LocalizeDataTable[file][filter].Add(key, value);
                }
            }
        }
        private void OnLanguageRead(ULocalizationService _server)
        {
            //ReadXMLFolder();
            //ReadCSVFolder();
            ReadCSVMPK();
        }
        #endregion

        #region XML辅助内容
        /// <summary>
        /// 本地化配置表里的语言名字属性
        /// </summary>
        private const string NameAttribute = "Name";
        /// <summary>
        /// 本地化配置表里的缩写属性
        /// </summary>
        private const string AbbreviationAttribute = "Abbreviation";
        /// <summary>
        /// 本地化配置表里默认语言属性
        /// </summary>
        private const string DefaultAttribute = "Default";
        /// <summary>
        /// 配置表Key属性
        /// </summary>
        private const string KeyAttribute = "Key";
        /// <summary>
        /// 配置表Value属性
        /// </summary>
        private const string ValueAttribute = "Value";
        /// <summary>
        /// 配置表Language 元素
        /// </summary>
        private const string LanguageElement = "root/Language";
        /// <summary>
        /// 配置表Filter 元素
        /// </summary>
        private const string FilterElement = "root/Filter";
        /// <summary>
        /// 配置表Symbols 元素
        /// </summary>
        private const string SymbolsElement = "Symbols";

        /// <summary>
        /// 获取配置表Key属性
        /// </summary>
        /// <param name="_node">当前需要获取的节点</param>
        /// <returns>Key属性</returns>
        private string GetKeyAttribute(XmlNode _node)
        {
            return XmlHelper.GetStringValue(_node, KeyAttribute, "Key");
        }
        /// <summary>
        /// 获取配置表Value属性
        /// </summary>
        /// <param name="_node">当前需要获取的节点</param>
        /// <returns>Value属性</returns>
        private string GetValueAttribute(XmlNode _node)
        {
            return XmlHelper.GetStringValue(_node, ValueAttribute, "Value");
        }
        /// <summary>
        /// 获取本地化配置 语言名字属性
        /// </summary>
        /// <param name="_node">当前需要获取的节点</param>
        /// <returns>语言名字</returns>
        private string GetNameAttribute(XmlNode _node)
        {
            return XmlHelper.GetStringValue(_node, NameAttribute, "Name");
        }
        /// <summary>
        /// 获取本地化配置 语言名字缩写属性
        /// </summary>
        /// <param name="_node">当前需要获取的节点</param>
        /// <returns>语言名字缩写</returns>
        private string GetAbbreviationAttribute(XmlNode _node)
        {
            return XmlHelper.GetStringValue(_node, AbbreviationAttribute, "Abbreviation");
        }
        /// <summary>
        /// 获取本地化配置 是否默认语言属性
        /// </summary>
        /// <param name="_node">当前需要获取的节点</param>
        /// <returns>true默认语言</returns>
        private bool GetDefaultAttribute(XmlNode _node)
        {
            return XmlHelper.GetBoolValue(_node, DefaultAttribute, false);
        }
        #endregion
    }
}
