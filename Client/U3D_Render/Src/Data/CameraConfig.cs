using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;
using UnityEngine;

namespace ASpeedGame.Data.CameraConfig
{

    //与CameraConfig.csv表里面对应的列
    public enum CamConfig : int
    {
        EMT_INDEX,                  //配置的ID        
        EMT_HEROTYPE,               //英雄类型
        EMT_MAPTYPE,                //地图类型
        EMT_USER_DEFAULTDISTANCE,   //普通模式摄像机默认距离
        EMT_USER_MINDISTANCE,       //普通模式摄像机最小距离
        EMT_USER_MAXDISTANCE,       //普通模式摄像机最大距离
        EMT_USER_DEFAULTANGLE,      //普通模式默认俯角
        EMT_USER_MINANGLE,          //普通模式最小俯角
        EMT_USER_MAXANGLE,          //普通模式最大俯角
        EMT_USER_TARGETOFFSET,      //普通模式目标Y轴偏移值
        EMT_USER_FOV,               //普通模式FOV
        EMT_FLOATING_DEFAULTDISTANCE,   //翅膀模式摄像机默认距离
        EMT_FLOATING_MINDISTANCE,       //翅膀模式摄像机最小距离
        EMT_FLOATING_MAXDISTANCE,       //翅膀模式摄像机最大距离
        EMT_FLOATING_DEFAULTANGLE,      //翅膀模式默认俯角
        EMT_FLOATING_MINANGLE,          //翅膀模式最小俯角
        EMT_FLOATING_MAXANGLE,          //翅膀模式最大俯角
        EMT_FLOATING_TARGETOFFSET,      //翅膀模式目标Y轴偏移值
        EMT_FLOATING_FOV,               //翅膀模式FOV
        EMT_FPS_DEFAULTDISTANCE,   //FPS模式摄像机默认距离
        EMT_FPS_MINDISTANCE,       //FPS模式摄像机最小距离
        EMT_FPS_MAXDISTANCE,       //FPS模式摄像机最大距离
        EMT_FPS_DEFAULTANGLE,      //FPS模式默认俯角
        EMT_FPS_MINANGLE,          //FPS模式最小俯角
        EMT_FPS_MAXANGLE,          //FPS模式最大俯角
        EMT_FPS_TARGETOFFSET,      //FPS模式目标Y轴偏移值
        EMT_FPS_FOV,               //FPS模式FOV
        EMT_SCOPE_DEFAULTDISTANCE,   //狙击镜模式摄像机默认距离
        EMT_SCOPE_MINDISTANCE,       //狙击镜模式摄像机最小距离
        EMT_SCOPE_MAXDISTANCE,       //狙击镜模式摄像机最大距离
        EMT_SCOPE_DEFAULTANGLE,      //狙击镜模式默认俯角
        EMT_SCOPE_MINANGLE,          //狙击镜模式最小俯角
        EMT_SCOPE_MAXANGLE,          //狙击镜模式最大俯角
        EMT_SCOPE_TARGETOFFSET,      //狙击镜模式目标Y轴偏移值
        EMT_SCOPE_FOV,               //狙击镜模式FOV
        COUNT,
    }

    class CameraConfig
    {
        public int MAX_HERO_TYPE = 1000; //英雄类型的上限，用于将两个类型ID合并为一个类型

        public Hashtable CameraConfigTable;
        private static CameraConfig singleton = new CameraConfig();

        public static CameraConfig Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new CameraConfig();
            }
        }

        public void Load(string stPath)
        {
            InitCameraConfigCsv(stPath);
        }

        public void UnLoad()
        {
            if (CameraConfigTable != null)
            {
                CameraConfigTable.Clear();
                CameraConfigTable = null;
            }
            singleton = null;
        }

        private void InitCameraConfigCsv(string stPath)
        {
            ScpReader reader = new ScpReader(stPath,true,2);
            CameraConfigTable = new Hashtable();

            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                int index = reader.GetInt(i, (int)CamConfig.EMT_INDEX, 0);
                int heroID = reader.GetInt(i, (int)CamConfig.EMT_HEROTYPE, 0);
                int mapID = reader.GetInt(i, (int)CamConfig.EMT_MAPTYPE, 0);


                SoldierCamera.CameraParam[] caminfo= new SoldierCamera.CameraParam[4];

                int modeAttributeCount = 8;//每一项有8个参数
                for (int mode = 0; mode < 4; mode++)
                {
                    caminfo[mode] = new SoldierCamera.CameraParam();
                    caminfo[mode].curDistance = reader.GetInt(i, mode*modeAttributeCount+(int)CamConfig.EMT_USER_DEFAULTDISTANCE, 0);
                    Vector2 disRange = new Vector2();
                    disRange.x = reader.GetInt(i, mode * modeAttributeCount + (int)CamConfig.EMT_USER_MINDISTANCE, 0);
                    disRange.y = reader.GetInt(i, mode * modeAttributeCount + (int)CamConfig.EMT_USER_MAXDISTANCE, 0);
                    caminfo[mode].distanceRange = disRange;
                    caminfo[mode].curPitchAngle = reader.GetInt(i, mode * modeAttributeCount + (int)CamConfig.EMT_USER_DEFAULTANGLE, 0);
                    Vector2 angleRange = new Vector2();
                    angleRange.x = reader.GetInt(i, mode * modeAttributeCount + (int)CamConfig.EMT_USER_MINANGLE, 0);
                    angleRange.y = reader.GetInt(i, mode * modeAttributeCount + (int)CamConfig.EMT_USER_MAXANGLE, 0);
                    caminfo[mode].pitchAngleRange = angleRange;
                    caminfo[mode].targetOffset = reader.GetFloat(i, mode * modeAttributeCount + (int)CamConfig.EMT_USER_TARGETOFFSET, 0);
                    caminfo[mode].FOV = reader.GetInt(i, mode * modeAttributeCount + (int)CamConfig.EMT_USER_FOV, 0);
                }
                CameraConfigTable[mapID*MAX_HERO_TYPE+heroID] = caminfo;
            }
        }

        public SoldierCamera.CameraParam[] GetCameraConfigInfo(int sceneType,int heroType)
        {
            EntityUtil.formatMapID(ref sceneType);

            if (CameraConfigTable == null)
            {
                return null;
            }
            if (CameraConfigTable.ContainsKey(sceneType * MAX_HERO_TYPE+heroType)) //是否存在指定场景的指定英雄配置
            {
                return CameraConfigTable[sceneType * MAX_HERO_TYPE + heroType] as SoldierCamera.CameraParam[];
            }
            else if (CameraConfigTable.ContainsKey(heroType)) //是否存在指定英雄的通用场景配置，即sceneType==0
            {
                return CameraConfigTable[heroType] as SoldierCamera.CameraParam[];
            }
            else if (CameraConfigTable.ContainsKey(sceneType * MAX_HERO_TYPE)) //是否存在指定场景的通用英雄配置即heroType==0
            {
                return CameraConfigTable[sceneType * MAX_HERO_TYPE] as SoldierCamera.CameraParam[];
            }
            else
            {
                return CameraConfigTable[0] as SoldierCamera.CameraParam[];
            }
        }
    }
}
