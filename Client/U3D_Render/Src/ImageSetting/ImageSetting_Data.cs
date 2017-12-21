/*******************************************************************
** 文件名:	ImageSetting_Data.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	2016-2-24
** 版  本:	1.0
** 描  述:	ImageSetting 图像设置面板 数据

********************************************************************/
using UnityEngine;
using System;
using Effect;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using GameLogic;
using System.IO;
using ASpeedGame.Data.PlayerSettingLocalConfig;


/// <summary>
/// 画面设置
/// </summary>
public partial class ImageSetting : MonoBehaviour
{
    private static ImageSettingData currData = new ImageSettingData();

    private static ImageQuality OldImageQuality = ImageQuality.Invaild;
    private static ShadowQuality OldShadowQuality = ShadowQuality.Invaild;
    private static TextureQuality OldTextureQuality = TextureQuality.Invaild;
    private static SceneRenderingQuality OldSceneRenderingQuality = SceneRenderingQuality.Invaild;
    private static EffectQuality OldEffectQuality = EffectQuality.Invalid;
    private static WindowType OldWindowType = WindowType.Invalid;


    private static bool LoadConfig()
    {
        #region 暂时废弃
        //if (!File.Exists(ConfigFileName))
        //{
        //    return false;
        //}

        //string stSchemeAllText = System.IO.File.ReadAllText(ConfigFileName);

        ////去掉\n,因为ScpReader以"\r"分割
        //stSchemeAllText = stSchemeAllText.Replace("\n", "");
        ////再干掉最后的"\r"
        //stSchemeAllText = stSchemeAllText.Remove(stSchemeAllText.Length - 1, 1);
        //ScpReader configReader = new ScpReader(stSchemeAllText, "ImageSetting", 0);

        //string result = configReader.GetString((int)ConfigRow.Shadow,1, string.Empty);
        //System.Object reslutObj = null;

        //if (StringHelper.StringToEnum(typeof(ShadowQuality), result, out reslutObj))
        //{
        //    currShadowQuality = (ShadowQuality)reslutObj;
        //}
        ////else
        ////{
        ////    configReader = null;
        ////    return false;
        ////}

        //result = configReader.GetString((int)ConfigRow.Texture, 1, string.Empty);
        //if (StringHelper.StringToEnum(typeof(TextureQuality), result, out reslutObj))
        //{
        //    currTextureQuality = (TextureQuality)reslutObj;
        //}
        ////else
        ////{
        ////    configReader = null;
        ////    return false;
        ////}

        //result = configReader.GetString((int)ConfigRow.PixelLight,1 , string.Empty);
        //if (StringHelper.StringToEnum(typeof(PixelLightQuality), result, out reslutObj))
        //{
        //    currPixelLightQuality = (PixelLightQuality)reslutObj;
        //}
        ////else
        ////{
        ////    configReader = null;
        ////    return false;
        ////}

        //result = configReader.GetString((int)ConfigRow.RealTimeReflectionProbe, 1, string.Empty);
        //int iResult = -1;
        //if (int.TryParse(result, out iResult))
        //{
        //    currRealTimeReflectionProbeEnabled = iResult;
        //}
        ////else
        ////{
        ////    configReader = null;
        ////    return false;
        ////}

        //result = configReader.GetString((int)ConfigRow.Asino, 1, string.Empty);
        //iResult = -1;
        //if (int.TryParse(result, out iResult))
        //{
        //    currAsinoEnabled = iResult;
        //}
        ////else
        ////{
        ////    configReader = null;
        ////    return false;
        ////}

        //result = configReader.GetString((int)ConfigRow.AutoQuality, 1, string.Empty);
        //iResult = -1;
        //if (int.TryParse(result, out iResult))
        //{
        //    AutoAdjustQuality = iResult;
        //}
        ////else
        ////{
        ////    configReader = null;
        ////    return false;
        ////}

        //result = configReader.GetString((int)ConfigRow.ImageLevel, 1, string.Empty);
        //if (StringHelper.StringToEnum(typeof(ImageQuality), result, out reslutObj))
        //{
        //    currImageQuality = (ImageQuality)reslutObj;
        //}
        ////else
        ////{
        ////    configReader = null;
        ////    return false;
        ////}
        

        //result = configReader.GetString((int)ConfigRow.EffectQuality, 1, string.Empty);
        //if (StringHelper.StringToEnum(typeof(EffectQuality), result, out reslutObj))
        //{
        //    currEffectQuality = (EffectQuality)reslutObj;
        //}
        ////else
        ////{
        ////    configReader = null;
        ////    return false;
        ////}

        //result = configReader.GetString((int)ConfigRow.WaterQuality, 1, string.Empty);
        //if (StringHelper.StringToEnum(typeof(WaterQuality), result, out reslutObj))
        //{
        //    currWaterQuality = (WaterQuality)reslutObj;
        //}
        ////else
        ////{
        ////    configReader = null;
        ////    return false;
        ////}

        ////非编辑模式才加载分辨率相关
        //if(!Application.isEditor)
        //{
        //    result = configReader.GetString((int)ConfigRow.Resolution, 1, string.Empty);
        //    if (!result.Equals(string.Empty))
        //    {
        //        currResolution = result;
        //    }
        //    //else
        //    //{
        //    //    configReader = null;
        //    //    return false;
        //    //}

        //    result = configReader.GetString((int)ConfigRow.WindowType, 1, string.Empty);
        //    if (StringHelper.StringToEnum(typeof(WindowType), result, out reslutObj))
        //    {
        //        currWindowType = (WindowType)reslutObj;
        //    }
        //    //else
        //    //{
        //    //    configReader = null;
        //    //    return false;
        //    //}
        //}


        //result = configReader.GetString((int)ConfigRow.SceneRenderingQuality, 1, string.Empty);
        //if (StringHelper.StringToEnum(typeof(SceneRenderingQuality), result, out reslutObj))
        //{
        //    currSceneRenderingQuality = (SceneRenderingQuality)reslutObj;
        //}
        ////else
        ////{
        ////    configReader = null;
        ////    return false;
        ////}

        //configReader = null;
        //return true;
        #endregion

        List<PlayerSettingData> dataList;

        if (!PlayerSettingLocalConfig.Instance.GetLocalData(PlayerSettingDataType.ImageSetting, out dataList))
            return false;

        foreach (PlayerSettingData data in dataList)
        {
            string result = data.m_DataValueList[0];
            int iResult = -1;
            System.Object reslutObj = null;
            switch (data.m_DataName)
            {
                case "ImageQuality":
                    {
                        if (StringHelper.StringToEnum(typeof(ImageQuality), result, out reslutObj))
                        {
                            currData.currImageQuality = (ImageQuality)reslutObj;
                        }
                    }
                    break;
                case "ShadowQuality":
                    {
                        if (StringHelper.StringToEnum(typeof(ShadowQuality), result, out reslutObj))
                        {
                            currData.currShadowQuality = (ShadowQuality)reslutObj;
                        }
                    }
                    break;
                case "TextureQuality":
                    {
                        if (StringHelper.StringToEnum(typeof(TextureQuality), result, out reslutObj))
                        {
                            currData.currTextureQuality = (TextureQuality)reslutObj;
                        }
                    }
                    break;
                //case "PixelLightQuality":
                //    {
                //        if (StringHelper.StringToEnum(typeof(PixelLightQuality), result, out reslutObj))
                //        {
                //            currPixelLightQuality = (PixelLightQuality)reslutObj;
                //        }
                //    }
                //    break;
                case "SceneRenderingQuality":
                    {
                        if (StringHelper.StringToEnum(typeof(SceneRenderingQuality), result, out reslutObj))
                        {
                            currData.currSceneRenderingQuality = (SceneRenderingQuality)reslutObj;
                        }
                    }
                    break;
                //case "RealTimeReflectionProbe":
                //    {
                //        if (int.TryParse(result, out iResult))
                //        {
                //            currRealTimeReflectionProbeEnabled = iResult;
                //        }
                //    }
                //    break;
                //case "TextureAsino":
                //    {
                //        if (int.TryParse(result, out iResult))
                //        {
                //            currAsinoEnabled = iResult;
                //        }
                //    }
                //    break;
                //case "AutoAdjustQuality":
                //    {
                //        if (int.TryParse(result, out iResult))
                //        {
                //            AutoAdjustQuality = iResult;
                //        }
                //    }
                //    break;
                case "EffectQuality":
                    {
                        if (StringHelper.StringToEnum(typeof(EffectQuality), result, out reslutObj))
                        {
                            currData.currEffectQuality = (EffectQuality)reslutObj;
                        }
                    }
                    break;
                //case "WaterQuality":
                //    {
                //        if (StringHelper.StringToEnum(typeof(WaterQuality), result, out reslutObj))
                //        {
                //            currWaterQuality = (WaterQuality)reslutObj;
                //        }
                //    }
                //    break;
                case "Resolution":
                    {
                        if (!Application.isEditor)
                        {
                            if (!result.Equals(string.Empty))
                            {
                                currData.currResolution = result;
                            }
                        }
                    }
                    break;
                case "WindowType":
                    {
                        if (!Application.isEditor)
                        {
                            if (StringHelper.StringToEnum(typeof(WindowType), result, out reslutObj))
                            {
                                currData.currWindowType = (WindowType)reslutObj;
                            }
                        }
                    }
                    break;
                case "SSAOEnabled":
                    {
                        if (int.TryParse(result, out iResult))
                        {
                            currData.SSAOEnabled = iResult;
                        }
                    }
                    break;
                case "FXAAEnabled":
                    {
                        if (int.TryParse(result, out iResult))
                        {
                            currData.FXAAEnabled = iResult;
                        }
                    }
                    break;
                case "BloomAndFlaresEnabled":
                    {
                        if (int.TryParse(result, out iResult))
                        {
                            currData.BloomAndFlaresEnabled = iResult;
                        }
                    }
                    break;
                case "AdvanceImageEffectEnabled":
                    {
                        if (int.TryParse(result, out iResult))
                        {
                            currData.AdvanceImageEffectEnabled = iResult;
                        }
                    }
                    break;

                case "gameBrightness":
                    {
                        float fResult;
                        if (float.TryParse(result, out fResult))
                        {
                            currData.currGameBrightness = fResult;
                        }
                    }
                    break;
                    
                default:
                    break;
            }
        }
        return true;
    }

    private static void ExportConfig()
    {
        #region 暂时废弃
        //StreamWriter writer = new StreamWriter(ConfigFileName, false, new System.Text.UTF8Encoding(true));

        //string s = "";
        //s = "ShadowQuality" + "," + currShadowQuality.ToString();
        //writer.WriteLine(s);

        //s = "TextureQuality" + "," + currTextureQuality.ToString();
        //writer.WriteLine(s);

        //s = "PixelLightQuality" + "," + currPixelLightQuality.ToString();
        //writer.WriteLine(s);

        //s = "SceneRenderingQuality" + "," + currSceneRenderingQuality.ToString();
        //writer.WriteLine(s);

        //s = "RealTimeReflectionProbe" + "," + currRealTimeReflectionProbeEnabled;
        //writer.WriteLine(s);

        //s = "TextureAsino" + "," + currAsinoEnabled;
        //writer.WriteLine(s);

        //s = "AutoAdjustQuality" + "," + AutoAdjustQuality;
        //writer.WriteLine(s);

        //s = "ImageQuality" + "," + currImageQuality.ToString();
        //writer.WriteLine(s);

        //s = "EffectQuality" + "," + currEffectQuality.ToString();
        //writer.WriteLine(s);

        //s = "WaterQuality" + "," + currWaterQuality.ToString();
        //writer.WriteLine(s);

        //s = "Resolution" + "," + currResolution;
        //writer.WriteLine(s);

        //s = "WindowType" + "," + currWindowType.ToString();
        //writer.WriteLine(s);

        //s = "SceneRenderingQuality" + "," + currSceneRenderingQuality.ToString();
        //writer.WriteLine(s);

        //writer.Flush();
        //writer.Close();
        //writer = null;
        #endregion

        List<PlayerSettingData> dataList = new List<PlayerSettingData>();
        PlayerSettingData imageQuality = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "ImageQuality", currData.currImageQuality.ToString());
        dataList.Add(imageQuality);
        PlayerSettingData shadowQuality = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "ShadowQuality", currData.currShadowQuality.ToString());
        dataList.Add(shadowQuality);
        PlayerSettingData textureQuality = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "TextureQuality", currData.currTextureQuality.ToString());
        dataList.Add(textureQuality);
        //PlayerSettingData pixelLightQuality = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "PixelLightQuality", currPixelLightQuality.ToString());
        //dataList.Add(pixelLightQuality);
        PlayerSettingData sceneRenderingQuality = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "SceneRenderingQuality", currData.currSceneRenderingQuality.ToString());
        dataList.Add(sceneRenderingQuality);
        //PlayerSettingData realTimeReflectionProbe = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "RealTimeReflectionProbe", currRealTimeReflectionProbeEnabled.ToString());
        //dataList.Add(realTimeReflectionProbe);
        //PlayerSettingData textureAsino = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "TextureAsino", currAsinoEnabled.ToString());
        //dataList.Add(textureAsino);
        //PlayerSettingData autoAdjustQuality = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "AutoAdjustQuality", AutoAdjustQuality.ToString());
        //dataList.Add(autoAdjustQuality);
        PlayerSettingData effectQuality = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "EffectQuality", currData.currEffectQuality.ToString());
        dataList.Add(effectQuality);
        //PlayerSettingData waterQuality = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "WaterQuality", currWaterQuality.ToString());
        //dataList.Add(waterQuality);
        PlayerSettingData resolution = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "Resolution", currData.currResolution.ToString());
        dataList.Add(resolution);
        PlayerSettingData windowType = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "WindowType", currData.currWindowType.ToString());
        dataList.Add(windowType);

        PlayerSettingData sSAOEnabled = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "SSAOEnabled", currData.SSAOEnabled.ToString());
        dataList.Add(sSAOEnabled);
        PlayerSettingData fXAAEnabled = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "FXAAEnabled", currData.FXAAEnabled.ToString());
        dataList.Add(fXAAEnabled);
        PlayerSettingData bloomAndFlaresEnabled = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "BloomAndFlaresEnabled", currData.BloomAndFlaresEnabled.ToString());
        dataList.Add(bloomAndFlaresEnabled);
        PlayerSettingData advanceImageEffectEnabled = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "AdvanceImageEffectEnabled", currData.AdvanceImageEffectEnabled.ToString());
        dataList.Add(advanceImageEffectEnabled);
        PlayerSettingData gameBrightness = new PlayerSettingData(PlayerSettingDataType.ImageSetting, "gameBrightness", currData.currGameBrightness.ToString());
        dataList.Add(gameBrightness);
        
        PlayerSettingLocalConfig.Instance.ModifyLocalData(PlayerSettingDataType.ImageSetting, dataList);
    }
}