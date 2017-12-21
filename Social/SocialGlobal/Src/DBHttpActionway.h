/*******************************************************************
** 文件名:	DBHttpActionway.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/14/2017
** 版  本:	1.0
** 描  述:	HTTPSERVICE的Action

********************************************************************/
#pragma once

#include "IActionway.h"
#include "TimerHandler.h"

using namespace DBHttp;




struct DBHttpActionNotity : public IActionway, public TimerHandler
{
    enum
    {
        ONLINE_USERS_READ_MAIL_TIMER = 1,        // 在线用户读取邮件定时器
    };

    // 构造函数
    DBHttpActionNotity();
    // 析构函数
    ~DBHttpActionNotity();

	virtual void OnExectue(DWORD pdbid, SMsgDBHttp_Request& msg, std::string& value,std::vector<std::string>& valueParams ) override;

	virtual void OnGodExectue(SMsgDBHttp_Request& msg, std::string& value,std::vector<std::string>& valueParams) override;
    
    /**
    @purpose          : 定时器触发后回调,你可以在这里编写处理代码
    @param	 dwTimerID: 定时器ID,用于区分是哪个定时器
    @return		      : empty
    */
    virtual void OnTimer(unsigned long dwTimerID) override;

private:
    // 发送数据到场景服
    void sendDataToScene(BYTE byKeyModule, BYTE byKeyAction, LPCSTR pData, size_t nLen);

    // 玩家读取邮件
    void playerReadMail(DWORD dwPDBID);

private:
	// 当前进行到的索引
	DWORD					m_dwIndex;

	// 当前需要通知的读取邮件的玩家列表
	std::vector<DWORD>		m_PlayerList;
};


/*
@brief：系统消息
*/
struct ActionwaySystemMessage : public IActionway
{
    virtual void OnExectue(DWORD pdbid, SMsgDBHttp_Request& msg, std::string& value, std::vector<std::string>& valueParams) override;

    virtual void OnGodExectue(SMsgDBHttp_Request& msg, std::string& value, std::vector<std::string>& valueParams) override;

    // 发送数据到场景服
    void sendDataToHttpSrv(BYTE byKeyModule, BYTE byKeyAction, LPCSTR pData, size_t nLen);
};
