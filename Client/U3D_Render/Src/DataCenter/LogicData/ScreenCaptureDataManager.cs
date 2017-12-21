using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Runtime.InteropServices;

namespace DataCenter
{
    //截图和视频名称定义
    public class ScreenShotAndVedioDefines
    {
        public const string ImageDef = "Image";
        public const string VideoDef = "Video";
    }

    public enum EMScreenRECType : int
    {
        EMSS_Invalid,
        EMSS_Kill,
        EMSS_DoubleKill,
        EMSS_TripleKill,
        EMSS_Quadrakill,
        EMSS_PentaKill,
        EMSS_Max,
    }

    //截图类型
    public enum EMScreenShotType : int
    {
        EMSS_InValid,
        EMSS_Default,
        EMSS_RemoveUI,
        EMSS_TripleKill,
        EMSS_QuadraKill,
        EMSS_PentaKill,
        EMSS_God,
        EMSS_Max,
    }

    //截图数据
    public struct SScreenShotImgInfo
    {
        public int nHeroID;
        public Int64 nDBWarID;
        public EMScreenShotType screenShotType;

        public SScreenShotImgInfo(int heroID, EMScreenShotType _Type)
        {
            nHeroID = heroID;
            screenShotType = _Type;
            nDBWarID = LogicDataCenter.warInfoDataManager.DBWarID;
        }
    }

    public class CaptureImgData
    {
        public FileInfo fileInfo;
        public SScreenShotImgInfo imgInfo;
        public byte[] imgByteData;
    }

    public class CaptureVideoData
    {
        public FileInfo fileInfo;
        public EMScreenRECType fileType;
        public int nHeroID;
        public Int64 nDBWarID;
    }

    public class ScreenCaptureDataManager
    {
        private readonly string imgFolderName = "ScreenShots";
        private readonly string videoFolderName = "GameVideos";

        private Dictionary<string, List<CaptureImgData>> m_ImageFileTable;    //图片文件 
        private Dictionary<string, List<CaptureVideoData>> m_VideoFileTable;  //视频文件
        private List<Int64> m_DBWarIDList;

        private bool bInit;

        public string ImgFolderFullPath
        {
            get
            {
                string path = ViewConfigManager.GetAccountConfigFullPath(imgFolderName);
                //没有目录的话创建一个
                if (!System.IO.Directory.Exists(path))
                {
                    System.IO.Directory.CreateDirectory(path);
                }
                return path;
            }
        }

        public string VideoFolderFullPath
        {
            get
            {
                string path = ViewConfigManager.GetAccountConfigFullPath(videoFolderName);
                //没有目录的话创建一个
                if (!System.IO.Directory.Exists(path))
                {
                    System.IO.Directory.CreateDirectory(path);
                }
                return path;
            }
        }

        /// <summary>
        /// 初始化
        /// </summary>
        public void Init()
        {
            if (bInit)
                return;

            m_ImageFileTable = new Dictionary<string, List<CaptureImgData>>();
            m_VideoFileTable = new Dictionary<string, List<CaptureVideoData>>();
            m_DBWarIDList = new List<Int64>();

            bInit = true;
        }

        /// <summary>
        /// 卸载
        /// </summary>
        public void Clear()
        {
            if (!bInit)
                return;

            ClearData();
            m_ImageFileTable = null;
            m_VideoFileTable = null;
            m_DBWarIDList = null;
        }

        /// <summary>
        /// 载入本账号数据
        /// </summary>
        public void Load()
        {
            if (!bInit)
                Init();

            LoadFolderFiles(ImgFolderFullPath);
            LoadFolderFiles(VideoFolderFullPath);
        }

        /// <summary>
        /// 卸载本账号数据
        /// </summary>
        public void UnLoad()
        {
            if (!bInit)
                return;

            ClearData();
            ScreenREC.GetInstance().WarVideoCount = 0;
            CaptureScreen.GetInstance().WarImgCount = 0;
        }

        /// <summary>
        /// 重新载入数据
        /// </summary>
        public void ReLoad()
        {
            if (!bInit)
                Init();

            ClearData();
            Load();
        }

        //获取匹配图片
        public CaptureImgData GetMatchImg(DateTime dateTime, int shotType) 
        {
            if (!bInit)
                return null;

            string key = dateTime.ToString("yyyy-MM-dd");
            if (m_ImageFileTable.ContainsKey(key))
                return m_ImageFileTable[key].Find(x => x.imgInfo.screenShotType == (EMScreenShotType)shotType && x.fileInfo.CreationTime <= dateTime);
            return null;
        }

        //获取图片时间排布表
        public List<string> GetImgDateTimeList()
        {
            if (!bInit)
                return null;

            List<string> dateList = new List<string>();
            List<DateTime> tempList = new List<DateTime>();
            foreach (string time in m_ImageFileTable.Keys)
            {
                int index = m_ImageFileTable[time].FindIndex(x => x.imgInfo.screenShotType > EMScreenShotType.EMSS_RemoveUI && x.imgInfo.screenShotType < EMScreenShotType.EMSS_Max);
                if (index > -1)
                    tempList.Add(DateTime.Parse(time));
            }
            tempList.Sort();
            tempList.Reverse();
            foreach (DateTime dateTime in tempList)
            {
                dateList.Add(dateTime.ToString("yyyy-MM-dd"));
            }
            return dateList;
        }

        //获取视频时间排布表
        public List<string> GetVideoDateTimeList()
        {
            if (!bInit)
                return null;

            List<string> dateList = new List<string>();
            List<DateTime> tempList = new List<DateTime>();
            foreach (string time in m_VideoFileTable.Keys)
            {
                tempList.Add(DateTime.Parse(time));
            }
            tempList.Sort();
            tempList.Reverse();
            foreach (DateTime dateTime in tempList)
            {
                dateList.Add(dateTime.ToString("yyyy-MM-dd"));
            }
            return dateList;
        }

        //获取图片文件List
        public List<CaptureImgData> GetImgDataByDateTime(string dateTime)
        {
            if (!bInit)
                return null;

            List<CaptureImgData> imgData;
            if (m_ImageFileTable.TryGetValue(dateTime, out imgData))
            {
                return imgData;
            }

            return null;
        }

        //获取视频文件List
        public List<CaptureVideoData> GetVideoDataByDateTime(string dateTime)
        {
            if (!bInit)
                return null;

            List<CaptureVideoData> videoData;
            if (m_VideoFileTable.TryGetValue(dateTime, out videoData))
            {
                return videoData;
            }

            return null;
        }

        //查询某场游戏是否有精彩截图
        public bool FindScreenCaptureImg(Int64 dbWarID) 
        {
            if (null == m_DBWarIDList)
                return false;

            return m_DBWarIDList.Contains(dbWarID);
        }

        ///// <summary>
        ///// 载入单个文件
        ///// </summary>
        ///// <param name="filePath"></param>
        //public void LoadFile(string filePath)
        //{
        //    FileInfo fileInfo = new FileInfo(filePath);

        //    if (fileInfo != null)
        //    {
        //        if (!CheckFile(fileInfo))
        //            return;

        //        m_FileInfoList.Add(fileInfo);
        //        AddDataByType(fileInfo);
        //    }
        //}

        /// <summary>
        /// 清除数据
        /// </summary>
        public void ClearData()
        {
            List<string> tempImgKeyList = new List<string>();
            foreach (string key in m_ImageFileTable.Keys)
            {
                tempImgKeyList.Add(key);
            }

            foreach (string key in tempImgKeyList)
            {
                m_ImageFileTable[key].Clear();
                m_ImageFileTable[key] = null;
                m_ImageFileTable.Remove(key);
            }

            m_ImageFileTable.Clear();

            tempImgKeyList.Clear();
            tempImgKeyList = null;

            List<string> tempVideoKeyList = new List<string>();
            foreach (string key in m_VideoFileTable.Keys)
            {
                tempVideoKeyList.Add(key);
            }

            foreach (string key in tempVideoKeyList)
            {
                m_VideoFileTable[key].Clear();
                m_VideoFileTable[key] = null;
                m_VideoFileTable.Remove(key);
            }

            m_VideoFileTable.Clear();

            tempVideoKeyList.Clear();
            tempVideoKeyList = null;

            m_DBWarIDList.Clear();
        }

        /// <summary>
        /// 载入单个文件夹下文件
        /// </summary>
        /// <param name="rootPath"></param>
        private void LoadFolderFiles(string rootPath)
        {
            if (rootPath == "")
            {
                Trace.LogWarning("rootPath文件夹路径为空!");
            }

            if (!System.IO.Directory.Exists(rootPath))
                return;

            DirectoryInfo rootInfo = new DirectoryInfo(rootPath);
            if (null == rootInfo)
                return;

            List<FileInfo> m_FileInfoList = new List<FileInfo>();

            //查找
            FileInfo[] fileInfos = rootInfo.GetFiles();

            //添加
            foreach (FileInfo file in fileInfos)
            {
                if (!CheckFile(file))
                    continue;

                m_FileInfoList.Add(file);
            }

            //排序
            m_FileInfoList.Sort((x, y) =>
            {
                if (x.CreationTime == y.CreationTime)
                    return 0;
                else
                    return (x.CreationTime > y.CreationTime) ? -1 : 1;
            });

            for(int i = 0; i < m_FileInfoList.Count; i++)
            {
                if (i > 99)
                    break;

                if(null != m_FileInfoList[i])
                    AddDataByType(m_FileInfoList[i]);
            }

            m_FileInfoList.Clear();
            m_FileInfoList = null;
        }

        //按类型添加文件
        private void AddDataByType(FileInfo fileInfo)
        {
            if (fileInfo.Extension == ".jpg")
            {
                CaptureImgData imgData = new CaptureImgData();
                imgData.fileInfo = fileInfo;

                byte[] allBytes = ImageExtensions.GetAllByteData(fileInfo.FullName);
                imgData.imgByteData = allBytes;

                SScreenShotImgInfo imgInfo = new SScreenShotImgInfo();
                imgInfo = ImageExtensions.GetCustomData<SScreenShotImgInfo>(allBytes);

                if (imgInfo.nHeroID < 0 || imgInfo.nDBWarID < 0)
                    return;

                imgData.imgInfo = imgInfo;

                string createTime = fileInfo.CreationTime.ToString("yyyy-MM-dd");

                if (!m_ImageFileTable.ContainsKey(createTime))
                {
                    m_ImageFileTable.Add(createTime, new List<CaptureImgData>());
                }
                m_ImageFileTable[createTime].Add(imgData);

                if (imgInfo.nDBWarID > 0)
                {
                    if (!m_DBWarIDList.Contains(imgInfo.nDBWarID))
                        m_DBWarIDList.Add(imgInfo.nDBWarID);
                }
            }
            else if (fileInfo.Extension == ".mp4" || fileInfo.Extension == ".avi")
            {
                CaptureVideoData videoData;

                if (!CheckVideo(fileInfo, out videoData))
                    return;

                string createTime = fileInfo.CreationTime.ToString("yyyy-MM-dd");

                if (!m_VideoFileTable.ContainsKey(createTime))
                {
                    m_VideoFileTable.Add(createTime, new List<CaptureVideoData>());
                }
                m_VideoFileTable[createTime].Add(videoData);
            }
        }

        //文件检查
        private bool CheckFile(FileInfo file)
        {
            //后缀不对，踢掉
            if (file.Extension != ".jpg" && file.Extension != ".mp4")
                return false;

            if (file.Extension == ".jpg")
            {
                //类型不对，踢掉
                //加一下简单的判断，防止文件夹内文件被混乱或移动
                if (!file.Name.StartsWith(ScreenShotAndVedioDefines.ImageDef))
                    return false;
            }
            else if (file.Extension == ".mp4")
            {
                //类型不对，踢掉
                if (!file.Name.StartsWith(ScreenShotAndVedioDefines.VideoDef))
                    return false;
            }

            return true;
        }

        //视频文件检查
        private bool CheckVideo(FileInfo fileInfo, out CaptureVideoData videoData)
        {
            videoData = new CaptureVideoData();

            if (fileInfo == null)
                return false;

            videoData.fileInfo = fileInfo;

            string[] infoArgs = fileInfo.Name.Split('_');

            if (infoArgs.Length < 5)
                return false;

            int type;
            if (int.TryParse(infoArgs[1], out type))
            {
                if (type > (int)EMScreenRECType.EMSS_Invalid && type < (int)EMScreenRECType.EMSS_Max)
                    videoData.fileType = (EMScreenRECType)type;
                else
                    return false;
            }
            else
                return false;

            int heroID;
            if (int.TryParse(infoArgs[2], out heroID))
            {
                if (heroID > 0)
                    videoData.nHeroID = heroID;
                else
                    return false;
            }
            else
                return false;

            Int64 warID;
            if (Int64.TryParse(infoArgs[3], out warID))
            {
                if (warID > 0)
                    videoData.nDBWarID = warID;
                else
                    return false;
            }
            else
                return false;

            return true;
        }
    }
}
