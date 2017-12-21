using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;

namespace USpeedUI
{
    partial class ULocalizationService
    {
        private enum ELocalizationConfig
        {
            ELC_ID,
            ELC_ABB,//缩写
            ELC_DEF,//是否默认
        }
        private enum ELocalizationFile
        {
            ELF_View,
            ELF_Key,
            ELF_Value
        }

        /// <summary>
        /// CSV配置路径
        /// </summary>
        private FileInfo[] allCSVFiles;

        public const string LocalizationConfigFileName = "LocalizationConfig.csv";


        private void Load_LocalizationCSVConfig(string _strConfigPath)
        {
            ScpReader LocalizationConfigReader = new ScpReader(_strConfigPath, true,2);

            var d = ULanguageInfo.zhCn;
            Languages.Clear();
            for (int i = 0; i < LocalizationConfigReader.GetRecordCount(); ++i)
            {
                int id = LocalizationConfigReader.GetInt(i, (int)ELocalizationConfig.ELC_ID, -1);
                string strABBName = LocalizationConfigReader.GetString(i, (int)ELocalizationConfig.ELC_ABB, string.Empty);

                if (id != -1 && id < 1000)
                {
                    var language = new ULanguageInfo(strABBName);

                    Languages.Add(language);
                    //是否是默认语言
                    if (LocalizationConfigReader.GetInt(i, (int)ELocalizationConfig.ELC_DEF, 0) == 1)
                    {
                        d = language;
                    }
                }
                else if(id != -1 && id>=1000)
                {
                    if (!LanguagesCSVFiles.Contains(strABBName))
                        LanguagesCSVFiles.Add(strABBName);
                    else
                        Trace.LogWarning("本地化文件配置里有重复文件名：" + strABBName);
                }
            }
            Language = d;
        }

        /// <summary>
        /// 当前语言改变时，根据重新加载事件读取所有语言配置文件路径
        /// </summary>
        private void ReadCSVFolder()
        {
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
            //遍历该目录下所有csv文件
            allCSVFiles = rootDirInfo.GetFiles("*.csv");
            if (allCSVFiles.Length == 0)
            {
                UnityEngine.Debug.LogError(Language.Name + "文件夹下没有csv配置文件...");
                return;
            }

            //根据当前已读的配置先加载
            if(LocalizeDataTable != null)
            {
                List<string> LoadedFiles = LocalizeDataTable.Select(d => d.Key).ToList();
                LocalizeDataTable = new Dictionary<string, Dictionary<string, Dictionary<string, string>>>();

                foreach (string loadedFile in LoadedFiles)
                {
                    ReadCSVFile(loadedFile);
                }
            }
            else
            {
                LocalizeDataTable = new Dictionary<string, Dictionary<string, Dictionary<string, string>>>();
            }

            //编辑模式下都加载
#if UNITY_EDITOR
            if (!Application.isPlaying)
            {
                foreach (FileInfo file in allCSVFiles)
                {
                    ReadCSVFile(file);
                }
            }
#endif
        }

        /// <summary>
        /// 将File内容读进来并存储
        /// </summary>
        /// <param name="_fileInfo"></param>
        private void ReadCSVFile(FileInfo _fileInfo)
        {
            string file, strViewFilter, strKey, strValue;

            file = _fileInfo.Name.Split('.')[0];
            if(!LocalizeDataTable.ContainsKey(file))
                LocalizeDataTable.Add(file, new Dictionary<string, Dictionary<string, string>>());

            ScpReader tmpCSVDocument = new ScpReader(_fileInfo.FullName, true,2);

            string tmpCurrentView = string.Empty;

            for (int i = 0; i < tmpCSVDocument.GetRecordCount(); ++i)
            {
                //读"界面名"
                strViewFilter = tmpCSVDocument.GetString(i, (int)ELocalizationFile.ELF_View, i==0?string.Empty: tmpCurrentView);
                if (i == 0 && strViewFilter == string.Empty)
                {
                    Trace.LogError("配置表 "+ file + " 出错!!第一个ViewFilter不能是空的。");
                    break;
                }
                else if(strViewFilter == string.Empty)//表当前位置与上一个位置的ViewFilter一致
                {
                    strViewFilter = tmpCurrentView;
                }
                else if(strViewFilter != tmpCurrentView)//表当前位置属于另一个ViewFilter
                {
                    tmpCurrentView = strViewFilter;
                }
                //读"关键字"
                strKey = tmpCSVDocument.GetString(i, (int)ELocalizationFile.ELF_Key, string.Empty);
                if(strKey == string.Empty)
                {
                    Trace.LogError("配置表:" + file + "，在界面：" + strViewFilter + " 有个关键字是空的，这是不允许的出错。");
                    break;
                }
                //读"显示内容"
                strValue = tmpCSVDocument.GetString(i, (int)ELocalizationFile.ELF_Value, string.Empty);
                if(strValue == string.Empty)
                {
                    Trace.LogWarning("配置表:" + file + "，在界面：" + strViewFilter + "，有个值：" + strKey + " 显示的内容是空的，虽然这是允许，但请注意下。");
                }
                if(!LocalizeDataTable[file].ContainsKey(strViewFilter))
                    LocalizeDataTable[file].Add(strViewFilter, new Dictionary<string, string>());
                if (LocalizeDataTable[file][strViewFilter].ContainsKey(strKey))
                    Trace.LogError("配置表: " + file + "，在界面：" + strViewFilter + "，有个值：" + strKey + " 是重复的，这是不允许的，请修改。");
                else
                    LocalizeDataTable[file][strViewFilter].Add(strKey, strValue);

            }
        }

        /// <summary>
        /// 通过FileFilter找到File的路径并将File内容读取进来
        /// </summary>
        /// <param name="_strFileFilter"></param>
        private void ReadCSVFile(string _strFileFilter)
        {
            IEnumerable<FileInfo> KeyFiles = allCSVFiles.Where(curFile => curFile.Name.Split('.')[0] == _strFileFilter);
            
            foreach (FileInfo file in KeyFiles)
            {
                ReadCSVFile(file);               
            }

        }

        private void ReadCSVMPK()
        {
            if (LanguagesCSVFiles.Count <= 0)
                return;

            //根据当前已读的配置先加载
            if (LocalizeDataTable != null)
            {
                List<string> LoadedFiles = LocalizeDataTable.Select(d => d.Key).ToList();
                LocalizeDataTable = new Dictionary<string, Dictionary<string, Dictionary<string, string>>>();

                foreach (string loadedFile in LoadedFiles)
                {
                    ReadCSVMPKFile(loadedFile);
                }
            }
            else
            {
                LocalizeDataTable = new Dictionary<string, Dictionary<string, Dictionary<string, string>>>();
            }

            //编辑模式下都加载
#if UNITY_EDITOR
            if (!Application.isPlaying)
            {
                foreach (string file in LanguagesCSVFiles)
                {
                    ReadCSVMPKFile(file);
                }
            }
#endif
        }

        private void ReadCSVMPKFile(string _strFileName)
        {
            string strFilePath, strViewFilter, strKey, strValue;

            //file = _fileInfo.Name.Split('.')[0];
            string strFormatEditor = "{0}{1}/UI/{2}.csv";
            string strFormatRunning = "{0}{1}\\UI\\{2}.csv";
            string strConfigPath = Initialize.Instance != null ? ViewConfigManager.DataScpPath : ViewConfigManager.Editor_DataScpPath;
            strFilePath = string.Format(Initialize.Instance != null ? strFormatRunning : strFormatEditor, strConfigPath, Language.Name, _strFileName);//ViewConfigManager.DataScpPath + Language.Name + "\\UI\\" + _strFileName+".csv";//

            if (!LocalizeDataTable.ContainsKey(_strFileName))
                LocalizeDataTable.Add(_strFileName, new Dictionary<string, Dictionary<string, string>>());

            ScpReader tmpCSVDocument = new ScpReader(strFilePath, true, 2);

            string tmpCurrentView = string.Empty;

            for (int i = 0; i < tmpCSVDocument.GetRecordCount(); ++i)
            {
                //读"界面名"
                strViewFilter = tmpCSVDocument.GetString(i, (int)ELocalizationFile.ELF_View, i == 0 ? string.Empty : tmpCurrentView);
                if (i == 0 && strViewFilter == string.Empty)
                {
                    Trace.LogError("配置表 " + _strFileName + " 出错!!第一个ViewFilter不能是空的。");
                    break;
                }
                else if (strViewFilter == string.Empty)//表当前位置与上一个位置的ViewFilter一致
                {
                    strViewFilter = tmpCurrentView;
                }
                else if (strViewFilter != tmpCurrentView)//表当前位置属于另一个ViewFilter
                {
                    tmpCurrentView = strViewFilter;
                }
                //读"关键字"
                strKey = tmpCSVDocument.GetString(i, (int)ELocalizationFile.ELF_Key, string.Empty);
                if (strKey == string.Empty)
                {
                    Trace.LogError("配置表:" + _strFileName + "，在界面：" + strViewFilter + " 有个关键字是空的，这是不允许的出错。");
                    break;
                }
                //读"显示内容"
                strValue = tmpCSVDocument.GetString(i, (int)ELocalizationFile.ELF_Value, string.Empty);
                if (strValue == string.Empty)
                {
                    Trace.LogWarning("配置表:" + _strFileName + "，在界面：" + strViewFilter + "，有个值：" + strKey + " 显示的内容是空的，虽然这是允许，但请注意下。");
                }
                if (!LocalizeDataTable[_strFileName].ContainsKey(strViewFilter))
                    LocalizeDataTable[_strFileName].Add(strViewFilter, new Dictionary<string, string>());
                if (LocalizeDataTable[_strFileName][strViewFilter].ContainsKey(strKey))
                    Trace.LogError("配置表: " + _strFileName + "，在界面：" + strViewFilter + "，有个值：" + strKey + " 是重复的，这是不允许的，请修改。");
                else
                    LocalizeDataTable[_strFileName][strViewFilter].Add(strKey, strValue);

            }

        }
    }
}
