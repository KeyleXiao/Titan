/*******************************************************************
** 文件名:	ServerCapabilityTest.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015-08-17
** 版  本:	1.0
** 描  述:	
** 应  用:  服务器性能检测	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

class GuardConnector;

class CServerCapabilityTest :	public TimerHandler	
{
	// 服务器检测定时器
#define DTIMER_ID_SERVERCHECK	1

	// 数据存盘定时器
#define DTIMER_ID_SAVERESULT	2

	// 数据存盘定时器
#define DTIMER_ID_TESTSTART		3

	// 数据存盘定时器时长
#define DSAVERESULT_INTERVAL	(120*1000)

	// 完成检测的队列最大存放多少个结点
#define DHODENODE_MAX			(5000)

	// 正在等待检测的结点
	typedef map<int, SMsgGuardServerCheck> TSEVERITEM;

public:
	/** 性能测试对象创建
	@param   
	@param   
	@return  
	*/
	virtual bool Create(GuardConnector* pConnector);

	/** 性能测试对象关闭
	@param   
	@param   
	@return  
	*/
	void Close();

	/** 启动服务器性能测试
	@param   
	@param   
	@return  
	*/
	virtual void  StartCapabilityTest(int nCheckInterval);

	/** 停止服务器性能测试
	@param   
	@param   
	@return  
	*/
	virtual void  StopCapabilityTest();

	/** 保存数据
	@param   
	@param   
	@return  
	*/
	void Save(bool bForce);

	/** 是否已启动
	@param   
	@param   
	@return  
	*/
	bool IsStartup();

	/** 收到中心服连接器的检测请求
	@param   
	@param   
	@return  
	*/
	virtual void HandleServerCheckReq(LPVOID pData,DWORD dwDataLen);

	/** 收到其它连接器或是中心服的回应
	@param   
	@param   
	@return  
	*/
	virtual void HandleServerCheckRes(LPVOID pData,DWORD dwDataLen);

	/** 取完成性能测试的结点
	@param   
	@param   
	@return  
	*/
	virtual TCAPABILITYNODE& GetCapabilityNode();

	/** 取最新监控结点数据
	@param   
	@param   
	@return  
	*/
	virtual TMONITORNODE& GetMonitorNode();

	/** 构造
	@param   
	@param   
	@return  
	*/
	CServerCapabilityTest();

	/** 析构
	@param   
	@param   
	@return  
	*/
	~CServerCapabilityTest();
protected:
	/** 定时器
	@param   
	@param   
	@return  
	*/
	virtual void OnTimer( unsigned long dwTimerID );

	/** 发起测试请求到各个服务器
	@param   
	@param   
	@return  
	*/
	virtual void SendCapabilityTestRequest();

	/** 
	@param   
	@param   
	@return  
	*/
	virtual void AddCompleteNode(SMsgGuardServerCheck& data);

private:

	// 服务器检测项
	TSEVERITEM m_serverItem;

	// 序号
	int m_nSerial;

	// 是否检测已启动
	bool m_bStartTest;

	// 连接器
	GuardConnector* m_pConnector;

	// 检测完成的结点
	TCAPABILITYNODE m_completeNode;

	// 最新监控结点数据
	TMONITORNODE m_monitorNode;
};