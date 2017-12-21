using UnityEngine;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using System;

/// <summary>
/// 资源下载接口
/// </summary>
public interface IResourceDown
{
    // 资源下载成功
    void ResourceDownSucc(ResDownResult result);
    // 资源下载失败
    void ResourceDownFail(ResDownResult result);
}

/// <summary>
/// 资源下载标识
/// </summary>
public enum ResDownFlag
{
    None,                   // 无用标识

    PackCsvFile,            // 资源包配置文件
    ResCsvFile,             // 资源物体配置文件
    ResPackage,             // 资源包（".unity3d"格式的文件)
    ResOther,               // 其它资源标识

    Max,                    // 最大标识
}

/// <summary>
/// 资源下载状态
/// </summary>
public enum ResDownState
{
    NoDown,                 // 未下载
    Downing,                // 下载中
    DownSucc,               // 下载成功
    DownFail,               // 下载失败
}

/// <summary>
/// 资源下载结果结构
/// </summary>
public struct ResDownResult
{
    // 请求ID
    public uint uReqId;

    // 文件ID
    // 下载标识非ResDownType.ResPackage，     设置为-1
    // 下载标识为ResDownType.ResPackage，     代表包ID
    public int nFileID;
	public string strFileName;

    // 用户下载数据
    public int nUserData;

    // 资源下载标识
    public ResDownFlag eFlag;

    // 资源下载状态
    public ResDownState eState;

    // 下载进度
    public float fProgress;

    // 错误信息
    // 下载中  ，为null
    // 下载成功，为null
    // 下载失败，非null
    public string sErrorMsg;

    public ResDownResult(uint reqId = 0)
    {
        uReqId = reqId;
        nFileID = 0;
		strFileName = "";
        nUserData = 0;
        eFlag = ResDownFlag.None;
        eState = ResDownState.NoDown;
            
        fProgress = 0f;
        sErrorMsg = null;
    }
}

/// <summary>
/// 资源下载请求信息
/// </summary>
public class ResDownReqInfo
{
    // 资源下载结果
    public ResDownResult m_result;

    // 是否异常下载
    public bool m_unusual = false;

    // 资源下载url
    public string m_url = null;

    // 资源下载名称
    public string m_name = null;

    //// 资源下载WWW对象
    //public WWW m_wwwObj = null;

    // 资源下载接口对象
    public IResourceDown m_iDown = null;

    public ResDownReqInfo(uint reqId, int fileId, ResDownFlag flag, bool unusual, string url, string name, IResourceDown iDown)
    {
        m_result = new ResDownResult();
        m_result.uReqId = reqId;
        m_result.nFileID = fileId;
		m_result.strFileName = name;
        m_result.eFlag = flag;

        m_url = url;
        m_name = name;
        m_iDown = iDown;
        m_unusual = unusual;
    }
}

public class ResDownManager : MonoBehaviour
{
	// 资源下载并发数
	public static int ConcurrenceNumber = 3;
			
	// 资源下载队列<资源下载请求信息对象>
    private Queue<ResDownReqInfo> m_DownQueue = null;

    // 资源异常下载队列<资源下载请求信息对象>
    private Queue<ResDownReqInfo> m_ExceptionQueue = null;

    // 资源请求下载唯一ID
    private static uint m_ReqDownLoadId = 0;

    // 是否订阅Update事件
    private bool m_isSubscribe = false;

    // 是否处于暂停中
    private bool m_isPause = false;

    // 是否正在下载中
	private Hashtable m_DowningFileList=new Hashtable();

	public static ResDownManager Instance = null;

    // 构造函数
	public void Awake()
    {
		Instance = this;

        m_DownQueue = new Queue<ResDownReqInfo>();

        m_ExceptionQueue = new Queue<ResDownReqInfo>();

    }



    /// <summary>
    /// 请求下载处理器（正常下载方法）
    /// </summary>
    /// <param name="reqId">请求ID</param>
    /// <param name="fileId">文件ID
    /// 资源下载标识非ResDownType.ResPackage, 设置为-1
    /// 资源下载标识为ResDownType.ResPackage，代表包ID
    /// </param>
    /// <param name="flag">资源下载标识</param>
    /// <param name="url">下载地址</param>
    /// <param name="name">文件名称（带文件格式）</param>
    /// <param name="type">文件类型</param>
    /// <param name="iDown">下载回调接口</param>
    public void ReqDownLoadHandler(out uint reqId, int fileId, ResDownFlag flag, string url, string name, IResourceDown iDown)
    {
        // 输出下载请求唯一ID
        reqId = m_ReqDownLoadId++;

        // 加入下载队列(非异常下载传入false）
        m_DownQueue.Enqueue(new ResDownReqInfo(reqId, fileId, flag, false, url, name, iDown));

        // 订阅Update事件
        //SubscribeUpdateEvent(true);
    }

    /// <summary>
    /// 请求下载处理器（异常下载方法）
    /// 用于下载错误时，重新下载的方法
    /// </summary>
    /// <param name="reqId">请求ID</param>
    /// <param name="fileId">文件ID
    /// 资源下载标识非ResDownType.ResPackage, 设置为-1
    /// 资源下载标识为ResDownType.ResPackage，代表包ID
    /// </param>
    /// <param name="flag">资源下载标识</param>
    /// <param name="url">下载地址</param>
    /// <param name="name">文件名称（带文件格式）</param>
    /// <param name="type">文件类型</param>
    /// <param name="iDown">下载回调接口</param>
    public void ReqDownLoadHandlerException(uint reqId, int fileId, ResDownFlag flag, string url, string name, IResourceDown iDown)
    {
        // 加入异常下载队列（异常下载传入true）
        m_ExceptionQueue.Enqueue(new ResDownReqInfo(reqId, fileId, flag, true, url, name, iDown));

        // 订阅Update事件
        //SubscribeUpdateEvent(true);
    }

    /// <summary>
    /// 判断是否暂停下载中
    /// </summary>
    /// <returns>返回是否暂停下载中</returns>
    public bool isPause()
    {
        return m_isPause;
    }

    /// <summary>
    /// 暂停下载
    /// </summary>
    /// <param name="pause">是否暂停下载</param>
    public void PauseDownLoad(bool pause)
    {
        if (m_isPause == pause) return;

        m_isPause = pause;

//            if (m_isPause)
//            {
//                // 暂停下载，取消订阅Update事件
//                SubscribeUpdateEvent(false);
//            }
//            else
//            {
//                // 继续下载，参与订阅Update事件
//                SubscribeUpdateEvent(true);
//            }
    }

    /// <summary>
    /// 获取当前下载信息
    /// </summary>
    /// <returns>返回当前下载信息</returns>
    public ResDownResult? GetCurDownInfo()
    {
        if (m_DownQueue.Count > 0)
        {
            // 取出当前下载请求信息对象
            ResDownReqInfo info = m_DownQueue.Peek();
            return info.m_result;
        }
            
        return null;
    }

    // 资源下载刷新
    public void Update()
    {
		//暂停
		if (m_isPause) 
		{
			return;
		}

        // 有异常下载任务，优先下载
        if (m_ExceptionQueue.Count > 0)
        {
            // 没有正在下载
			if (m_DowningFileList.Count<=0)
			{
                // 开启协程下载
				for(int i=0;i<ConcurrenceNumber;i++)
				{
					if (m_ExceptionQueue.Count>0)
					{
						ResDownReqInfo info=m_ExceptionQueue.Dequeue();
						m_DowningFileList[info.m_name]=info.m_url;
                    	StartCoroutine(DownLoadHandler(info));
					}
				}
            }
            else
            {
                return;
            }
        }

        // 有正常下载任务，其次下载
        if (m_DownQueue.Count > 0)
        {
            // 没有正在下载
			if (m_DowningFileList.Count<=0)
			{
				for(int i=0;i<ConcurrenceNumber;i++)
				{
					if (m_DownQueue.Count>0)
					{
						// 开启协程下载
						ResDownReqInfo info=m_DownQueue.Dequeue();
						m_DowningFileList[info.m_name]=info.m_url;
		                StartCoroutine(DownLoadHandler(info));
					}
				}
            }
            else
            {
                return;
            }
        }

//            // 下载结束，取消订阅Update事件
//            if (m_ExceptionQueue.Count <= 0 && m_DownQueue.Count <= 0)
//            {
//                SubscribeUpdateEvent(false);
//            }
    }

		
	// 资源下载处理器
//    private IEnumerator DownLoadHandler(ResDownReqInfo info)
//    {
//        // 开始下载
//        info.m_wwwObj = new WWW(info.m_url);

//        // 正在下载，设置下载状态
//        info.m_result.eState = ResDownState.Downing;

//        // 等待下载
//        yield return info.m_wwwObj;

//        // 下载失败
//        if (info.m_wwwObj.error != null)
//        {
//            // 更新下载结果信息
//            info.m_result.eState = ResDownState.DownFail;
//            info.m_result.fProgress = info.m_wwwObj.progress;
//            info.m_result.sErrorMsg = info.m_wwwObj.error;

//            // 通知请求下载对象，下载资源失败
//            info.m_iDown.ResourceDownFail(info.m_result);
//        }
//        // 下载成功
//        else
//        {
//            // 创建文件，保存本地
//            File.WriteAllBytes(ResDef.OuterPackageDirectory + info.m_name, info.m_wwwObj.bytes);
//            //ResTools.CreateFile(info.m_name, info.m_result.eFlag, info.m_wwwObj.bytes);

//            // 更新下载结果信息
//            info.m_result.eState = ResDownState.DownSucc;
//            info.m_result.fProgress = info.m_wwwObj.progress;
//            info.m_result.sErrorMsg = info.m_wwwObj.error;

//            // 通知请求下载对象，下载资源成功
//            info.m_iDown.ResourceDownSucc(info.m_result);
//        }

////            // 异常下载完成
////            if (info.m_unusual)
////            {
////                m_ExceptionQueue.Dequeue();
////            }
////            // 正常下载完成
////            else
////            {
////                m_DownQueue.Dequeue();
////            }

//        // 重置下载中标识
//        m_DowningFileList.Remove (info.m_name);
//    }

    Dictionary<WebClient, ResDownReqInfo> clientinfomap=new Dictionary<WebClient, ResDownReqInfo>();

    private void ProgressChanged(object sender, DownloadProgressChangedEventArgs e)
    {
        WebClient client = (WebClient)sender;
        ResDownReqInfo info = clientinfomap[client];
        info.m_result.fProgress = e.ProgressPercentage/100.0f;
    } 

    void DownloadFileCompleted(object sender, System.ComponentModel.AsyncCompletedEventArgs e)
    {
        WebClient client = (WebClient)sender;
        ResDownReqInfo info = clientinfomap[client];
        clientinfomap.Remove(client);
        if (e.Error == null)
        {
            // 更新下载结果信息
            info.m_result.eState = ResDownState.DownSucc;
            info.m_result.fProgress = 1.01f;
            info.m_result.sErrorMsg = "";

        }
        else // 下载失败
        {
            // 更新下载结果信息
            info.m_result.eState = ResDownState.DownFail;
            info.m_result.fProgress = 1.01f;
            info.m_result.sErrorMsg = e.Error.ToString();

        }
        
    }

    private IEnumerator DownLoadHandler(ResDownReqInfo info)
    {
        // 开始下载
        WebClient client = new WebClient();
        client.DownloadFileCompleted += new System.ComponentModel.AsyncCompletedEventHandler(DownloadFileCompleted);
        client.DownloadProgressChanged += new DownloadProgressChangedEventHandler(ProgressChanged);
        client.DownloadFileAsync(new Uri(info.m_url), ResDef.OuterPackageDirectory + info.m_name);
        clientinfomap[client] = info;

        // 正在下载，设置下载状态
        info.m_result.eState = ResDownState.Downing;

        while (info.m_result.fProgress<=1.0f)
        {
            yield return new WaitForEndOfFrame();
        }

        if (info.m_result.sErrorMsg == "")
        {
            // 通知请求下载对象，下载资源成功
            info.m_iDown.ResourceDownSucc(info.m_result);
        }
        else // 下载失败
        {
            // 通知请求下载对象，下载资源失败
            info.m_iDown.ResourceDownFail(info.m_result);
        }

        // 重置下载中标识
        m_DowningFileList.Remove(info.m_name);

        client.Dispose();
        client = null;

    }
}
