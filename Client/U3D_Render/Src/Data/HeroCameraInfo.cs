using System;
using System.Xml;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using HeTuGame.Data;

namespace ASpeedGame.Data.HeroCameraInfo
{
    /************************************************************************/
    /* 目前摄像机控制的数据写这    
     * 与CameraInfo.txt
     * example:
     * 		<!-- 摄像机相对于主角的最小距离 -->
            <minDistance value = "3.0" />
    /************************************************************************/
    public class CameraInfo
    {
        /// <summary>
        /// 设置不同的HeroCamera
        /// </summary>
        public int HeroCameraID;
        /// <summary>
        /// 摄像机相对于主角的最小距离
        /// 现在策划不需要滚轮，先不显示
        /// </summary>
        public float MinScrollDistance;
        /// <summary>
        /// 摄像机相对于主角的最大距离    
        /// 现在策划不需要滚轮，先不显示
        /// </summary>
        public float MaxScrollDistance;
        /// <summary>
        /// 鼠标Y轴反向
        /// </summary>
        public bool IsInvertPitch;
        /// <summary>
        /// 摄像机绕角色纵向旋转最小值
        /// </summary>
        public int MinPitchAngle ;
        /// <summary>
        /// 摄像机绕角色纵向旋转最大值
        /// </summary>
        public int MaxPitchAngle;
        /// <summary>
        /// 是否使用鼠标滚轮    
        /// </summary>
        public bool IsUseMouseWhell;
        /// <summary>
        /// 处于Hero坐标上方的高度值为实际射线检测点
        /// </summary>
        public float HeroCheckHeight;
        /// <summary>
        /// 旋转平滑时间
        /// </summary>                       
        public float RotateSmoothTime;
        /// <summary>
        /// 目标偏移量
        /// </summary>
        public float TargetOffset_X ;
        public float TargetOffset_Y;
        public float TargetOffset_Z;
        /// <summary>
        /// 位置平滑时间
        /// </summary>
        public float MoveSmoothTime;
        /// <summary>
        /// 处于Camera坐标上方的射线检测高度
        /// 当相机顶部高度范围内有检测到阻挡时，限制相机移动
        /// </summary>
        public float CameraCheckHeight;
		 /// <summary>
		/// 摄像机的FOV
		/// </summary>
        public float CameraFOV;
        /// <summary>
        /// 模拟角色高度，从这高度上发射一条向相机检测的射线
        /// </summary>
        public float HeroHeight;
        /// <summary>
        /// 摄像机碰撞偏移量
        /// </summary>
        public float CamHitOffset;
        /// <summary>
        /// Yaw偏移速度
        /// </summary>
        public float YawSpeed;
        /// <summary>
        /// Pitch俯仰速度
        /// </summary>
        public float PitchSpeed;
        /// <summary>
        /// 计算滚轮位置的变化量
        /// </summary>
        public int ScrollSpeed ;




        public void LoadFromXmlNode(XmlNode xmlNode)
        {
            CameraCheckHeight   = XmlHelper.GetFloatValue(xmlNode.SelectSingleNode("/root/CameraInfo/CameraCheckHeight"), "value", 0.0f);
            RotateSmoothTime    = XmlHelper.GetFloatValue(xmlNode.SelectSingleNode("/root/CameraInfo/RotateSmoothTime"), "value", 0.0f);
            HeroCheckHeight     = XmlHelper.GetFloatValue(xmlNode.SelectSingleNode("/root/CameraInfo/HeroCheckHeight"), "value", 0.0f);
            TargetOffset_X      = XmlHelper.GetFloatValue(xmlNode.SelectSingleNode("/root/CameraInfo/TargetOffset_X"), "value", 0.0f);
            TargetOffset_Y      = XmlHelper.GetFloatValue(xmlNode.SelectSingleNode("/root/CameraInfo/TargetOffset_Y"), "value", 0.0f);
            TargetOffset_Z      = XmlHelper.GetFloatValue(xmlNode.SelectSingleNode("/root/CameraInfo/TargetOffset_Z"), "value", 0.0f);
            MoveSmoothTime      = XmlHelper.GetFloatValue(xmlNode.SelectSingleNode("/root/CameraInfo/MoveSmoothTime"), "value", 0.0f);
            IsUseMouseWhell     = XmlHelper.GetBoolValue(xmlNode.SelectSingleNode("/root/CameraInfo/bUseMouseWhell"), "value", false);
            IsInvertPitch       = XmlHelper.GetBoolValue(xmlNode.SelectSingleNode("/root/CameraInfo/bInvertMouseY"), "value", false);
            CamHitOffset        = XmlHelper.GetFloatValue(xmlNode.SelectSingleNode("/root/CameraInfo/CamHitOffset"), "value", 0.0f);
            MinScrollDistance   = XmlHelper.GetFloatValue(xmlNode.SelectSingleNode("/root/CameraInfo/minDistance"), "value", 0.0f);
            MaxScrollDistance   = XmlHelper.GetFloatValue(xmlNode.SelectSingleNode("/root/CameraInfo/maxDistance"), "value", 0.0f);
            HeroHeight          = XmlHelper.GetFloatValue(xmlNode.SelectSingleNode("/root/CameraInfo/HeroHeight"), "value", 0.0f);
            PitchSpeed          = XmlHelper.GetFloatValue(xmlNode.SelectSingleNode("/root/CameraInfo/PitchSpeed"), "value", 0.0f);
            CameraFOV           = XmlHelper.GetFloatValue(xmlNode.SelectSingleNode("/root/CameraInfo/CameraFOV"), "value", 0.0f);
            YawSpeed            = XmlHelper.GetFloatValue(xmlNode.SelectSingleNode("/root/CameraInfo/YawSpeed"), "value", 0.0f);
            ScrollSpeed         = XmlHelper.GetIntValue(xmlNode.SelectSingleNode("/root/CameraInfo/ZoomScale"), "value", 0);
            MinPitchAngle       = XmlHelper.GetIntValue(xmlNode.SelectSingleNode("/root/CameraInfo/minAngleY"), "value", 0);
            MaxPitchAngle       = XmlHelper.GetIntValue(xmlNode.SelectSingleNode("/root/CameraInfo/maxAngleY"), "value", 0);
            HeroCameraID        = XmlHelper.GetIntValue(xmlNode, "id", 0);

        }
    }

    public class CameraInfoManager
    {
        /// <summary>
        /// 根据ID获取对应的摄像机信息
        /// </summary>
        public static CameraInfo CameraInfoItemList(int id) 
        {
            if (singleton.cameraInfoItemList[id] == null)
            {
                UnityEngine.Debug.LogError("列表没有ID：" + id + " 的的摄像机信息，检测是否没有加载！");
                return null;
            }
            return singleton.cameraInfoItemList[id]; 
        }

        /// <summary>
        /// 摄像机列表
        /// </summary>
        private Dictionary<int, CameraInfo> cameraInfoItemList = new Dictionary<int, CameraInfo>();

        /// <summary>
        /// 摄像机列表管理实例
        /// </summary>
        private static CameraInfoManager singleton = new CameraInfoManager();

        /// <summary>
        /// 摄像机配置信息资源文件
        /// </summary>
        private UnityEngine.TextAsset cameraInfoAsset;

        /// <summary>
        /// 获取摄像机列表管理实例
        /// </summary>
        /// <returns>摄像机列表管理实例</returns>
        public static CameraInfoManager GetSingleton()
        {
            return singleton;
        }

        /// <summary>
        /// 以string方式载入资源
        /// </summary>
        /// <param name="xmlString"></param>
        public void Load(string xmlString)
        {
            XmlDocument xmlDocument = new XmlDocument();
            xmlDocument.LoadXml(xmlString);
            Load(xmlDocument);
        }

        /// <summary>
        /// 以TextReader方式载入资源
        /// </summary>
        /// <param name="textReader"></param>
        public void Load(TextReader textReader)
        {
            XmlDocument xmlDocument = new XmlDocument();
            xmlDocument.Load(textReader);
            Load(xmlDocument);
        }

        /// <summary>
        /// 载入摄像机配置表 并匹配ID 
        /// </summary>
        /// <param name="xmlDocument"></param>
        private void Load(XmlDocument xmlDocument)
        {
            cameraInfoItemList = new Dictionary<int, CameraInfo>();

            XmlNodeList ItemList = xmlDocument.SelectNodes("/root/CameraInfo");
            foreach (XmlNode itemNode in ItemList)
            {
                int id = XmlHelper.GetIntValue(itemNode, "id", 0);
                if (id == 0)
                    continue;

                CameraInfo itemItem = new CameraInfo();
                itemItem.LoadFromXmlNode(itemNode);

                if(!cameraInfoItemList.ContainsKey(id))
                    cameraInfoItemList.Add(id, itemItem);
            }
        }

        /// <summary>
        /// 载入摄像机资源文件
        /// </summary>
        public void LoadCameraInfo()
        {
           if(cameraInfoAsset == null)
               cameraInfoAsset = (UnityEngine.TextAsset)UnityEngine.Resources.Load("Data/CameraInfo");

            if (cameraInfoAsset == null)
            {
                UnityEngine.Debug.LogError("摄像机配置文件读取失败!!");
                return;
            }

            Load(cameraInfoAsset.text);
        }
    }
}