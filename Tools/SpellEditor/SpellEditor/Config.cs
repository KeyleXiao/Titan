using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Configuration;
using SPELL;
namespace SpellEditor
{
    public class Config
    {
        public static UTF8Encoding FileEncoding = new UTF8Encoding(true);
        public static List<string> VocationFilterList = new List<string>();

        const string VocationFilterKey = "VocationFilter";

        /// <summary>
        /// 获取职业对应的名称
        /// </summary>
        /// <param name="voc"></param>
        /// <returns></returns>
        public static string GetVocationName(VOCATION voc)
        {
            return Enum.GetName(typeof(VOCATION), voc);
        }

        /// <summary>
        /// 获取对应的职业枚举值
        /// </summary>
        /// <param name="strVocationName">职业名称</param>
        /// <returns></returns>
        public static VOCATION GetVocationByName(string strVocationName)
        {
            return (VOCATION)Enum.Parse(typeof(VOCATION), strVocationName, true);
        }

        public const string Vocation_Filter_ALL = "全部";

        const string ExportPatchKey = "ExportPatch";
        const string ImportPatchKey = "ImportPatch";

        static KeyValueConfigurationCollection ConfigSet = null;
        static Configuration configuration = null;

        /// <summary>
        /// 初始化配置
        /// </summary>
        public static void InitConfig()
        {
            configuration = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
            ConfigSet = configuration.AppSettings.Settings;
            VocationFilterList.Add(Vocation_Filter_ALL);

            // 初始化所有职业
            for (VOCATION v = VOCATION.VOCATION_INVALID; v < VOCATION.VOCATION_MAX; v++)
            {
                VocationFilterList.Add(DataTableSerializer.vocationNameDict[ GetVocationName(v) ]);
            }

        }

        #region 导出路径


        /// <summary>
        /// 取得导出路径
        /// </summary>
        /// <returns></returns>
        public static string GetExportPatch()
        {
            string s = "";
            KeyValueConfigurationElement expor = ConfigSet[ExportPatchKey];
            if (null == expor)
            {
                ConfigSet.Add(ExportPatchKey, System.Windows.Forms.Application.StartupPath + "\\");
                configuration.Save();
            }
            expor = ConfigSet[ExportPatchKey];

            s = expor.Value;
            return s;
        }

        /// <summary>
        /// 取得导入路径
        /// </summary>
        /// <returns></returns>
        public static string GetImportPatch()
        {
            string s = "";

            KeyValueConfigurationElement impor = ConfigSet[ImportPatchKey];
            //如果没有导入路径这个属性，打开文件夹选择面板，添加进去
            if (null == impor)
            {
                ConfigSet.Add(ImportPatchKey, System.Windows.Forms.Application.StartupPath + "\\");
                configuration.Save();
            }

            impor = ConfigSet[ImportPatchKey];
            s = impor.Value;
            return s;
        }

        /// <summary>
        /// 修改输出路径
        /// </summary>
        /// <returns></returns>
        public static string ChangeExportPatch()
        {
            string s = "";

            KeyValueConfigurationElement expor = ConfigSet[ExportPatchKey];
            s = DataTableSerializer.OpenFolderBrowserDialog();
            //没有ImportPatch，打开文件夹选择面板，添加进去
            if (null == expor)
            {

                ConfigSet.Add(ExportPatchKey, s);
            }
            else
            {
                expor.Value = s;
            }
            configuration.Save();

            return s;
        }


        /// <summary>
        /// 修改导入路径
        /// </summary>
        /// <returns></returns>
        public static string ChangeImportPatch()
        {
            string s = "";

            KeyValueConfigurationElement impor = ConfigSet[ImportPatchKey];
            s = DataTableSerializer.OpenFolderBrowserDialog();
            //如果没有这个属性，打开文件夹选择面板，添加进去
            if (null == impor)
            {
                ConfigSet.Add(ImportPatchKey, s);
            }
            else
            {
                impor.Value = s;
            }
            configuration.Save();
            return s;
        }


        /// <summary>
        /// 修改输出路径
        /// </summary>
        /// <returns></returns>
        public static string ChangeExportPatch(string patch)
        {
            string s = "";

            KeyValueConfigurationElement expor = ConfigSet[ExportPatchKey];
            s = patch;
            if (null == expor)
            {

                ConfigSet.Add(ExportPatchKey, s);
            }
            else
            {
                expor.Value = s;
            }
            configuration.Save();

            return s;
        }


        /// <summary>
        /// 修改导入路径
        /// </summary>
        /// <returns></returns>
        public static string ChangeImportPatch(string patch)
        {
            string s = "";

            KeyValueConfigurationElement impor = ConfigSet[ImportPatchKey];
            s = patch;
            if (null == impor)
            {
                ConfigSet.Add(ImportPatchKey, s);
            }
            else
            {
                impor.Value = s;
            }
            configuration.Save();
            return s;
        }
        #endregion


        /// <summary>
        /// 修改职业过滤
        /// </summary>
        /// <returns></returns>
        public static void ChangeProfessionalFilter(string filterstr)
        {
            KeyValueConfigurationElement filter = ConfigSet[VocationFilterKey];
            if (null == filter)
            {
                ConfigSet.Add(ImportPatchKey, filterstr);
            }
            else
            {
                filter.Value = filterstr;
            }
            configuration.Save();
        }


        /// <summary>
        /// 取得职业过滤
        /// </summary>
        /// <returns></returns>
        public static string GetProfessionalFilter()
        {
            KeyValueConfigurationElement filter = ConfigSet[VocationFilterKey];
            if (null == filter)
            {
                ConfigSet.Add(ImportPatchKey, GetVocationName(VOCATION.VOCATION_INVALID));
                filter = ConfigSet[VocationFilterKey];
            }
            configuration.Save();
            return filter.Value;
          
        }
    }
}
