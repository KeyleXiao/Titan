using UnityEngine;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System;



// 资源版本检查状态
public enum ResVerCheckState
{
    None = 0,		    // 无用的

    Init,               // 初始状态
	Start,             //开始状态
    DownPackCsv,       // 下载包配置文件
    DownResCsv,        // 下载资源配置文件
    ResVerCheck,        // 资源版本检查
    DownResPack,        // 下载资源包
    Finished,           // 结束状态

    MAX,                // 最大值
}


/// <summary>
/// 资源下载信息
/// </summary>
public class ResDownInfo
{
    // 资源下载状态
    public ResDownState m_state;

    // 下载异常计数
    public int m_epCount;

    public ResDownInfo(ResDownState state, int count)
    {
        m_state = state;
        m_epCount = count;
    }
}

public class ResVerManager : MonoBehaviour, IResourceDown
{

	public struct PackageInfo
	{
		public string name;
		public int ver;
		public long filelength;
		public long datatime;
	}

    // 资源版本检查当前状态
    private ResVerCheckState m_CurState = ResVerCheckState.None;

    // 是否启动版本检查流程
    private bool m_bStartVerCheck = false;

	// 资源版本检查是否开始
	private bool m_bStartSucc = false;
		
	// New包配置是否下载成功
    private bool m_bPackCsvDownSucc = false;

    // 资源配置是否下载成功
    private bool m_bResCsvDownSucc = false;

    // 资源版本检查是否完成
    private bool m_bResVerCheckIsOver = false;

    // 需要更新下载的资源包列表(包ID)
    private List<string> m_NeedDownPackList = null;

    // 请求更新下载信息字典(请求ID，ResDownInfo对象)
    private Dictionary<uint, ResDownInfo> m_ReqDownInfoDic = null;

    // 检测是否下载完成的时间间隔
    private const float Check_Time = 0.5f;
    // 检测是否下载完成的时间控制器
    private float m_timer = 0f;
	// 未完成计数
	private int m_count=0;

    // 是否订阅Update事件
    private bool m_isSubscribe = false;

//        // Local资源包配置（内部使用）
//        private PackageCsv m_localPackageCsv = null;
//
//        // New资源包配置（内部使用）
//        private PackageCsv m_newPackageCsv = null;

 

    /// <summary>
    /// 资源版本管理流程是否结束
    /// </summary>
	public static bool bResVerMgrIsOver = false;

	private Hashtable oldpackagelist=null;
	private Hashtable newpackagelist=null;
	private int checkcount=0;
	//private int enumator;
	private List<string> newpackagekeys=null;
    private string errormsg = "";

	public static ResVerManager Instance = null;

	string ext=".zen";

    // 构造函数
	public void Awake()
    {
		bool bEncrypt = false;
		ext = bEncrypt ? ".zen" : ".unity3D";

		Instance = this;

        m_NeedDownPackList = new List<string>();
        m_ReqDownInfoDic = new Dictionary<uint, ResDownInfo>();

//            m_newPackageCsv = new PackageCsv();
//            m_localPackageCsv = new PackageCsv();

        bResVerMgrIsOver = false;

		// 设置初始状态
		SetState(ResVerCheckState.Init);
    }


    /// <summary>
    /// 启动版本检查流程
    /// </summary>
    public void StartVerCheck()
    {
        m_bStartVerCheck = true;
    }

    // 版本管理刷新
    public void Update()
    {
        // 驱动流程
        DoTask();
    }

		// 进入新的状态
    private void OnEnter(ResVerCheckState newState, ResVerCheckState oldState)
    {
        switch (newState)
        {
            case ResVerCheckState.Init:
                {
					try
					{
	                    // 用户目录不存在Package目录，创建Package目录
                        if (!Directory.Exists(ResDef.OuterPackageDirectory))
	                    {
                            Directory.CreateDirectory(ResDef.OuterPackageDirectory);
	                    }
					}
					catch(Exception e)
					{
					
					}
                }
                break;

			case ResVerCheckState.Start:
				{
					m_bStartSucc=true;
				}
				break;
				
			case ResVerCheckState.DownPackCsv:
                {
//						// 判断User下载目录中有没有Local_Package.txt，没有就从Resources目录中拷贝
//						if (!File.Exists(ResDef.UserDownLoadDirectory + ResDef.LocalPackageFileName))
//                      {
//							TextAsset obj = (TextAsset)AssetSystem.Singleton.LoadFromResources(LoadType.CSVFile, "Scp/Package");
//                          ResTools.CreateFile(ResDef.LocalPackageFileName, ResDownFlag.PackCsvFile, obj.bytes);
//                      }

                    // 请求下载New包配置文件
                    uint uReqId = 0;
					ResDownManager.Instance.ReqDownLoadHandler(out uReqId, -1, ResDownFlag.PackCsvFile, ResDef.ResPackageCommonURL + ResDef.NetPackageFileName, ResDef.NewPackageFileName, this);
                    m_ReqDownInfoDic.Add(uReqId, new ResDownInfo(ResDownState.NoDown, 0));

                }
                break;

            case ResVerCheckState.DownResCsv:
                {
					// 请求下载资源配置文件
					uint uReqId = 0;
					ResDownManager.Instance.ReqDownLoadHandler(out uReqId, -1, ResDownFlag.ResCsvFile, ResDef.ResPackageCommonURL + ResDef.ResourceFileName, ResDef.ResourceFileName, this);
                    m_ReqDownInfoDic.Add(uReqId, new ResDownInfo(ResDownState.NoDown, 0));
                }
                break;

            case ResVerCheckState.ResVerCheck:
                {
					oldpackagelist=new Hashtable();
					newpackagelist=new Hashtable();
					newpackagekeys=new List<string>();
					checkcount=0;

                    string NewPath = ResDef.OuterPackageDirectory + ResDef.NewPackageFileName;
                    string OldPath = ResDef.OuterPackageDirectory + ResDef.CurrentPackageFileName;
					if (System.IO.File.Exists(OldPath))
					{
						string pText=System.IO.File.ReadAllText(OldPath);
						ScpReader scpreader=new ScpReader(pText,"oldpackagecfg");
						int n=scpreader.GetRecordCount();
						for(int i=0;i<n;i++)
						{
							string packname=scpreader.GetString(i,1,"");
							int ver=scpreader.GetInt(i,2,0);
							long filelength=scpreader.GetLong(i,3,0);
							long datatime=scpreader.GetLong(i,4,0);
							if (packname.Length>0 && ver>0 && datatime>0)
							{
								PackageInfo packageinfo=new PackageInfo();
								packageinfo.name=packname;
								packageinfo.ver=ver;
								packageinfo.filelength=filelength;
								packageinfo.datatime=datatime;
								oldpackagelist[packname]=packageinfo;
							}
						}
                        Trace.Log("oldpackagelist count=" + oldpackagelist.Count.ToString());
						scpreader.Dispose();
					}
					if (System.IO.File.Exists(NewPath))
					{
						string pText=System.IO.File.ReadAllText(NewPath);
						ScpReader scpreader=new ScpReader(pText,"newpackagecfg");
						int n=scpreader.GetRecordCount();
						for(int i=0;i<n;i++)
						{
							string packname=scpreader.GetString(i,1,"");
							int ver=scpreader.GetInt(i,2,0);
							long filelength=scpreader.GetLong(i,3,0);
							long datatime=scpreader.GetLong(i,4,0);
							if (packname.Length>0 && ver>0 && datatime>0)
							{
								PackageInfo packageinfo=new PackageInfo();
								packageinfo.name=packname;
								packageinfo.ver=ver;
								packageinfo.filelength=filelength;
								packageinfo.datatime=datatime;
								newpackagelist[packname]=packageinfo;
								newpackagekeys.Add(packname);
							}
						}
                        Trace.Log("newpackagelist count=" + newpackagelist.Count.ToString());
                        Trace.Log("newpackagekeys count=" + newpackagekeys.Count.ToString());
						scpreader.Dispose();
					}

//						foreach(DictionaryEntry newkeypair in newpackagelist)
//						{
//							string newpackagename=(string)newkeypair.Key;
//							PackageInfo newpackageinfo=(PackageInfo)newkeypair.Value;
//							if (oldpackagelist.Contains(newpackagename))
//							{
//								PackageInfo oldpackageinfo=(PackageInfo)oldpackagelist[newpackagename];
//								//有更新变化
//								if (newpackageinfo.ver>oldpackageinfo.ver ||
//									newpackageinfo.filelength!=oldpackageinfo.filelength||
//									newpackageinfo.datatime!=oldpackageinfo.datatime)
//								{
//									m_NeedDownPackList.Add(newpackagename);
//								}
//								else//无更新变化
//								{
//									string destpath= ResDef.UserPackageDirectory+newpackagename+ext;
//									//文件不存在，重新下
//									if (System.IO.File.Exists(destpath)==false)
//									{
//										m_NeedDownPackList.Add(newpackagename);
//										continue;
//									}
//
//									//本地文件大小跟配置的不一样，重新下
//									FileInfo fi = new FileInfo(destpath);
//									if (fi.Length!=newpackageinfo.filelength)
//									{
//										m_NeedDownPackList.Add(newpackagename);
//										continue;
//									}
//								}
//							}
//							// 新增包
//					        else
//					        {
//					        	m_NeedDownPackList.Add(newpackagename);
//					        }
//						}

                    // 资源版本检查完成
                    //m_bResVerCheckIsOver = true;
                }
                break;

            case ResVerCheckState.DownResPack:
                {
                    if (m_NeedDownPackList.Count > 0)
                    {
                        for (int i = 0; i < m_NeedDownPackList.Count; ++i)
                        {
                            // 获取对应的New包配置数据
                            //PackageData data = m_newPackageCsv.GetCsvValue(m_NeedDownPackList[i]);
							string packname=m_NeedDownPackList[i]+ext;

                            // 请求下载资源包
                            uint uReqId = 0;
							ResDownManager.Instance.ReqDownLoadHandler(out uReqId, -1, ResDownFlag.ResPackage, ResDef.ResPackageCommonURL + packname, packname, this);
                            m_ReqDownInfoDic.Add(uReqId, new ResDownInfo(ResDownState.NoDown, 0));
                        }

                        // todo开始显示进度条

                    }                       
                }
                break;

            case ResVerCheckState.Finished:
                {
                    // 取消订阅Update事件
                    //SubscribeUpdateEvent(false);

					string NewPath=ResDef.OuterPackageDirectory+ResDef.NewPackageFileName;
                    string OldPath = ResDef.OuterPackageDirectory + ResDef.CurrentPackageFileName;

					//copy new_package.csv as local_package.csv
					if (File.Exists(NewPath))
					{
						File.Copy(NewPath,OldPath,true);
					}


				
					// 资源版本管理流程结束
                    bResVerMgrIsOver = true;

                }
                break;

            default:
                break;
        }

    }

    // 切换资源版本检查状态
    private bool SetState(ResVerCheckState newState)
    {
        // 如果设置状态与当前状态一致，跳过
        if (m_CurState == newState) return false;

        // 获取旧的状态
        ResVerCheckState oldState = m_CurState;

        // 当游戏流程退出
        OnExit(oldState, newState);

		// 设置新的状态
		m_CurState = newState;
        errormsg = "";
			
		// 当游戏流程进入
        OnEnter(newState, oldState);

        // 打印状态切换日志
        Trace.Log("ResVerManager.SetState():" + oldState.ToString() + "->" + newState.ToString());

        return true;
    }

    // 驱动版本检查流程
    private void DoTask()
    {
        switch (m_CurState)
        {
            case ResVerCheckState.Init:
                {    
                    if (m_bStartVerCheck)
                    {
                        SetState(ResVerCheckState.DownPackCsv);
                    }
                }
                break;

			case ResVerCheckState.Start:
				{
					if (m_bStartSucc)
					{
						SetState(ResVerCheckState.DownPackCsv);
					}						
				}
				break;
				
			case ResVerCheckState.DownPackCsv:
			{
				if (m_bPackCsvDownSucc)
                    {
                        SetState(ResVerCheckState.DownResCsv);
                    }
                }
                break;

            case ResVerCheckState.DownResCsv:
                {
                    if (m_bResCsvDownSucc)
                    {
                        SetState(ResVerCheckState.ResVerCheck);
                    }
                }
                break;

            case ResVerCheckState.ResVerCheck:
                {
					for(int i=0;i<5;i++) //one frame check 5 package
					{
						if (checkcount>=newpackagekeys.Count || checkcount>=newpackagelist.Count)
						{
							m_bResVerCheckIsOver=true;
							break;
						}
						string newpackagename=newpackagekeys[checkcount];
						PackageInfo newpackageinfo=(PackageInfo)newpackagelist[newpackagename];
						if (oldpackagelist.Contains(newpackagename))
						{
							PackageInfo oldpackageinfo=(PackageInfo)oldpackagelist[newpackagename];
							//有更新变化
							if (newpackageinfo.ver>oldpackageinfo.ver ||
								newpackageinfo.filelength!=oldpackageinfo.filelength||
								newpackageinfo.datatime!=oldpackageinfo.datatime)
							{
								m_NeedDownPackList.Add(newpackagename);
							}
							else//无更新变化
							{
								string destpath= ResDef.OuterPackageDirectory+newpackagename+ext;
								//文件不存在，重新下
								if (System.IO.File.Exists(destpath)==false)
								{
									m_NeedDownPackList.Add(newpackagename);
								}
								else
								{
									//本地文件大小跟配置的不一样，重新下
									FileInfo fi = new FileInfo(destpath);
									if (fi.Length!=newpackageinfo.filelength)
									{
										m_NeedDownPackList.Add(newpackagename);
									}
								}
							}
						}
						// 新增包
					    else
					    {
					        m_NeedDownPackList.Add(newpackagename);
					    }
						checkcount++;
					}

                    if (m_bResVerCheckIsOver)
                    {
                        SetState(ResVerCheckState.DownResPack);
                    }
                }
                break;

            case ResVerCheckState.DownResPack:
                {
                    // 控制Check_Time秒检测，是否全部成功下载
                    m_timer += Time.deltaTime;
                    if (m_timer >= Check_Time)
                    {
                        // 重置时间控制器
                        m_timer = 0;

						m_count = 0;
                        foreach (KeyValuePair<uint, ResDownInfo> pair in m_ReqDownInfoDic)
                        {
                            if (pair.Value.m_state != ResDownState.DownSucc && pair.Value.m_state != ResDownState.DownFail)
                            {
								++m_count;
                            }
                        }

                        // 资源下载完毕
						if (m_count == 0)
                        {
                            SetState(ResVerCheckState.Finished);
                            break;
                        }
                    }

                }
                break;

            case ResVerCheckState.Finished:
                {

                }
                break;

            default:
                break;
        } 
    }

    // 退出旧的状态
    private void OnExit(ResVerCheckState oldState, ResVerCheckState newState)
    {
        switch (oldState)
        {
            case ResVerCheckState.Init:
                {
                }
                break;
			case ResVerCheckState.Start:
				{
				}
				break;
			case ResVerCheckState.DownPackCsv:
				{
				}
                break;
            case ResVerCheckState.DownResCsv:
                {
                }
                break;
            case ResVerCheckState.ResVerCheck:
                {
					oldpackagelist=null;
					newpackagelist=null;
					newpackagekeys=null;
					//enumator=null;
                }
                break;
            case ResVerCheckState.DownResPack:
                {
                }
                break;
            case ResVerCheckState.Finished:
                {
                }
                break;
            default:
                break;
        }
    }

		
	// 资源下载成功
	public void ResourceDownSucc(ResDownResult result)
	{
		if (result.eFlag == ResDownFlag.PackCsvFile)
		{
			// 加载New包配置数据，保存在m_newPackageCsv中，供版本检查使用
            string filePath = ResDef.OuterPackageDirectory + ResDef.NewPackageFileName;
				
			// New包配置下载成功，设置对应下载信息
			m_bPackCsvDownSucc = true;
			m_ReqDownInfoDic[result.uReqId].m_epCount = 0;
			m_ReqDownInfoDic[result.uReqId].m_state = result.eState;
		}
		else if (result.eFlag == ResDownFlag.ResCsvFile)
        {
            // 加载资源配置, 保存在mResourceCsv中，供外部使用
            string filePath = ResDef.OuterPackageDirectory + ResDef.ResourceFileName;

            // 资源配置下载成功，设置对应下载信息
            m_bResCsvDownSucc = true;
            m_ReqDownInfoDic[result.uReqId].m_epCount = 0;
            m_ReqDownInfoDic[result.uReqId].m_state = result.eState;
        }
        else if (result.eFlag == ResDownFlag.ResPackage)
        {
//                // 成功下载一个包，获取对应的New包配置数据
//                PackageData data = m_newPackageCsv.GetCsvValue(result.nFileID);
//
//                // 下载的包放在用户目录中，属于外部包
//                data.m_isOut = 1;
//
//                // 设置对应的Local包配置数据,并同步到Local包配置文件中
//				m_localPackageCsv.SetCsvValueData(result.nFileID, data, true, ResDef.CurrentPackageFileName);

			string packname=result.strFileName;

            // 资源包下载成功，设置对应下载信息
            m_ReqDownInfoDic[result.uReqId].m_epCount = 0;
            m_ReqDownInfoDic[result.uReqId].m_state = result.eState;
     
        }  
    }

    // 资源下载失败
    public void ResourceDownFail(ResDownResult result)
    {
        if (result.eFlag == ResDownFlag.PackCsvFile)
        {
            Trace.Error("ResVerManager::ResourceDownFail 资源包配置文件下载失败，error = " + result.sErrorMsg);

            m_ReqDownInfoDic[result.uReqId].m_state = result.eState;

            if (m_ReqDownInfoDic[result.uReqId].m_epCount < 3)
            {
                m_ReqDownInfoDic[result.uReqId].m_epCount++;

                // 包配置文件下载失败，使用异常下载方法重新下载
				ResDownManager.Instance.ReqDownLoadHandlerException(result.uReqId, -1, ResDownFlag.PackCsvFile, ResDef.ResPackageCommonURL + ResDef.NetPackageFileName, ResDef.NewPackageFileName, this);
            }
            else
            {
                // todo 弹出提示框下载异常提示（待修改）
				string msg="Network Error when downloading:"+result.strFileName;
                errormsg = msg;
				//Game.m_singleton.UIManager.UIMsgBox.ShowMessageBox(UI.EMsgBoxType.OnlyOK, msg,OnMsgBoxConfirmClick);
                // todo 取消订阅Update事件
            }
        }
        else if (result.eFlag == ResDownFlag.ResCsvFile)
        {
            Trace.Error("ResVerManager::ResourceDownFail 资源配置文件下载失败，error = " + result.sErrorMsg);

            m_ReqDownInfoDic[result.uReqId].m_state = result.eState;

            if (m_ReqDownInfoDic[result.uReqId].m_epCount < 3)
            {
                m_ReqDownInfoDic[result.uReqId].m_epCount++;

                // 资源配置文件下载失败，使用异常下载方法重新下载
				ResDownManager.Instance.ReqDownLoadHandlerException(result.uReqId, -1, ResDownFlag.ResCsvFile, ResDef.ResPackageCommonURL + ResDef.ResourceFileName, ResDef.ResourceFileName, this);
            }
            else
            {
                // todo 弹出提示框下载异常提示（待修改）
				string msg="Network Error when downloading:"+result.strFileName;
                errormsg = msg;
				//Game.m_singleton.UIManager.UIMsgBox.ShowMessageBox(UI.EMsgBoxType.OnlyOK, msg, OnMsgBoxConfirmClick);
                // todo 取消订阅Update事件
            }

        }
        else if (result.eFlag == ResDownFlag.ResPackage)
        {
            Trace.Error("ResVerManager::ResourceDownFail 资源包下载失败，error = " + result.sErrorMsg + "资源包ID = " + result.nFileID);

            m_ReqDownInfoDic[result.uReqId].m_state = result.eState;

            if (m_ReqDownInfoDic[result.uReqId].m_epCount < 3)
            {
                m_ReqDownInfoDic[result.uReqId].m_epCount++;

				string packname=result.strFileName;

                // 资源包下载失败，使用异常下载方法重新下载
				ResDownManager.Instance.ReqDownLoadHandlerException(result.uReqId, -1, ResDownFlag.ResPackage, ResDef.ResPackageCommonURL + packname, packname, this);
            }
            else
            {
                // todo 弹出提示框下载异常提示(待修改）
				string msg="Network Error when downloading:"+result.strFileName;
                errormsg = msg;
                //Game.m_singleton.UIManager.UIMsgBox.ShowMessageBox(UI.EMsgBoxType.OnlyOK, msg, OnMsgBoxConfirmClick);
                // todo 取消订阅Update事件
            }
        }
    }

    //// 消息框点击按钮事件处理
    //public void OnMsgBoxConfirmClick(UI.EMsgBoxButtonType btnType)
    //{
    //    if(btnType == UI.EMsgBoxButtonType.OK)
    //    {
    //        // todo 做下载异常，网络异常等处理操作（待完善）
    //    }
    //}
        
}
