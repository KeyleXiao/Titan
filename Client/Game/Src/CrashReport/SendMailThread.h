#ifndef _SEND_MAIL_THREAD_H_
#define _SEND_MAIL_THREAD_H_

#include <winsock.h>
#include "base64.h"
#include <string>
#include <vector>

#pragma warning(disable:4100)

struct SendMailStruct
{
	std::string strAuthLogin;
	std::string strMailTo;
	std::string strSubject;
	std::string strMessage;
	std::string szAttachmentFileName;
	HWND	hParent;
	bool	hideProgress;
};

extern SendMailStruct *g_pMail;


struct AUTHLOGIN
{
	std::string szUser;
	std::string szPw;
	std::string szMail;
	std::string	szSmtpSvr;
	unsigned short	usPort;
	AUTHLOGIN(std::string szUser,std::string szPw,std::string szMail,std::string szSmtpSvr,unsigned short usPort)
	{
		this->szUser = szUser;
		this->szPw = szPw;
		this->szMail = szMail;
		this->szSmtpSvr = szSmtpSvr;
		this->usPort = usPort;
	}
	void Init(const std::vector<std::string>& infos)
	{
		if (infos.size() == 5)
		{
			szUser = infos[0];
			szPw = infos[1];
			szMail = infos[2];
			szSmtpSvr = infos[3];
			usPort = atoi(infos[4].c_str());
		}
	}
};

extern AUTHLOGIN g_AuthLogin[];
extern int g_iTimes;

#define S_ADDR	(g_AuthLogin[g_iTimes].szSmtpSvr.c_str())
#define S_PORT	(g_AuthLogin[g_iTimes].usPort)
#define S_USER	(g_AuthLogin[g_iTimes].szUser.c_str())
#define S_PW	(g_AuthLogin[g_iTimes].szPw.c_str())
#define S_MAILFROM	(g_AuthLogin[g_iTimes].szMail.c_str())

#define XCRASHREPORT_SEND_TO_NAME			"glacier technicist"
#define XCRASHREPORT_SEND_TO_ADDRESS		"bing.chuan001@163.com"

extern void SendMailThread(void* p);
extern void OUTPUT(const char* lpszFormat, ...);
extern SOCKET Cs;
extern void OUTPUT2(const char *lpszFormat,...);

template< class STATE >
class CESmtpStatusSwitch
{
	STATE * m_pCurrentSate;
public:
	CESmtpStatusSwitch() : m_pCurrentSate( NULL )
	{
	}

	~CESmtpStatusSwitch()
	{
		if( m_pCurrentSate != NULL ) m_pCurrentSate->Stop();
	}

	void GoToState( STATE * pNextState , const char * pContext, int nLen)
	{
		ASSERT( pNextState );
		if( m_pCurrentSate == pNextState ) return;
		if( m_pCurrentSate!= NULL ) m_pCurrentSate->Stop();
		m_pCurrentSate = pNextState;
		m_pCurrentSate->Start( pContext , nLen );
	}

	STATE * GetCurrentState()
	{
		ASSERT( m_pCurrentSate );
		return m_pCurrentSate;
	}
};
class CEsmtp;
struct IESmtpStatus
{
	virtual void Create(CEsmtp*) = 0;
	virtual void OnRecv(void *pBuf,long lSize) = 0;
	virtual void Start( const char * pContext,int nLen) = NULL;
	virtual void Stop() = NULL;
	virtual void Send(char *szSend,int iSize);
	CEsmtp	*m_pEsmtp;
};
class CConnectingStatus : public IESmtpStatus
{
public:
	virtual void Create(CEsmtp* pEsmtp)
	{
		m_pEsmtp = pEsmtp;
	}
	virtual void OnRecv(void *pBuf,long lSize);
	virtual void Start( const char * pContext,int nLen)
	{
	}
	virtual void Stop()
	{
	}
public:
	CConnectingStatus()
	{
		m_pEsmtp = 0;
	}
};
class CConnectedStatus : public IESmtpStatus
{
public:
	virtual void Create(CEsmtp* pEsmtp)
	{
		m_pEsmtp = pEsmtp;
	}
	virtual void OnRecv(void *pBuf,long lSize);
	virtual void Start( const char * pContext,int nLen)
	{
		char szSend[256];
		sprintf(szSend,"EHLO %s\r\n","ABCDE");
		Send(szSend,strlen(szSend));
	}
	virtual void Stop()
	{
	}
public:
	CConnectedStatus()
	{
		m_pEsmtp = 0;
	}
};
class CAuthLoginStatus : public IESmtpStatus
{
public:
	virtual void Create(CEsmtp* pEsmtp)
	{
		m_pEsmtp = pEsmtp;
	}
	virtual void OnRecv(void *pBuf,long lSize);
	virtual void Start( const char * pContext,int nLen)
	{
		char szSend[256];
		sprintf(szSend,"AUTH LOGIN\r\n");
		Send(szSend,strlen(szSend));
	}
	virtual void Stop()
	{
	}
public:
	CAuthLoginStatus()
	{
		m_pEsmtp = 0;
	}
};
class CAuthLoginUserStatus : public IESmtpStatus
{
public:
	virtual void Create(CEsmtp* pEsmtp)
	{
		m_pEsmtp = pEsmtp;
	}
	virtual void OnRecv(void *pBuf,long lSize);
	virtual void Start( const char * pContext,int nLen)
	{
		unsigned char szEncode[1024];
		base64encode((unsigned char*)S_USER,strlen((const char*)S_USER),szEncode,sizeof(szEncode));

		char szSend[256];
		sprintf(szSend,"%s\r\n",szEncode);
		Send(szSend,strlen(szSend));
	}
	virtual void Stop()
	{
	}
public:
	CAuthLoginUserStatus()
	{
		m_pEsmtp = 0;
	}
};
class CAuthLoginPwStatus : public IESmtpStatus
{
public:
	virtual void Create(CEsmtp* pEsmtp)
	{
		m_pEsmtp = pEsmtp;
	}
	virtual void OnRecv(void *pBuf,long lSize);
	virtual void Start( const char * pContext,int nLen)
	{
		unsigned char szEncode[1024];
		base64encode((unsigned char*)S_PW,strlen((const char*)S_PW),szEncode,sizeof(szEncode));

		char szSend[256];
		sprintf(szSend,"%s\r\n",szEncode);
		Send(szSend,strlen(szSend));
	}
	virtual void Stop()
	{
	}
public:
	CAuthLoginPwStatus()
	{
		m_pEsmtp = 0;
	}
};
class CMailFromStatus : public IESmtpStatus
{
public:
	virtual void Create(CEsmtp* pEsmtp)
	{
		m_pEsmtp = pEsmtp;
	}
	virtual void OnRecv(void *pBuf,long lSize);
	virtual void Start( const char * pContext,int nLen)
	{
		char szSend[256];
		sprintf(szSend,"MAIL From: %s\r\n",S_MAILFROM);
		Send(szSend,strlen(szSend));
	}
	virtual void Stop()
	{
	}
public:
	CMailFromStatus()
	{
		m_pEsmtp = 0;
	}
};
class CRcptToStatus : public IESmtpStatus
{
public:
	virtual void Create(CEsmtp* pEsmtp)
	{
		m_pEsmtp = pEsmtp;
	}
	virtual void OnRecv(void *pBuf,long lSize);
	virtual void Start( const char * pContext,int nLen);
	virtual void Stop()
	{
	}
public:
	CRcptToStatus()
	{
		m_pEsmtp = 0;
	}
};
class CDataStatus : public IESmtpStatus
{
public:
	virtual void Create(CEsmtp* pEsmtp)
	{
		m_pEsmtp = pEsmtp;
	}
	virtual void OnRecv(void *pBuf,long lSize);
	virtual void Start( const char * pContext,int nLen)
	{
		char szSend[256];
		sprintf(szSend,"DATA\r\n");
		Send(szSend,strlen(szSend));
	}
	virtual void Stop()
	{
	}
public:
	CDataStatus()
	{
		m_pEsmtp = 0;
	}
};
class CMailContentStatus : public IESmtpStatus
{
public:
	virtual void Create(CEsmtp* pEsmtp)
	{
		m_pEsmtp = pEsmtp;
	}
	virtual void OnRecv(void *pBuf,long lSize);
	virtual void Start( const char * pContext,int nLen);
	virtual void Stop()
	{
	}
public:
	CMailContentStatus()
	{
		m_pEsmtp = 0;
	}
};
class CMailEndStatus : public IESmtpStatus
{
public:
	virtual void Create(CEsmtp* pEsmtp)
	{
		m_pEsmtp = pEsmtp;
	}
	virtual void OnRecv(void *pBuf,long lSize);
	virtual void Start( const char * pContext,int nLen)
	{
	//	char szSend[256];
	//	sprintf(szSend,"\r\n.\r\n");	//结束标志
	//	Send(szSend,strlen(szSend));
	}
	virtual void Stop()
	{
	}
public:
	CMailEndStatus()
	{
		m_pEsmtp = 0;
	}
};
class CQuitStatus : public IESmtpStatus
{
public:
	virtual void Create(CEsmtp* pEsmtp)
	{
		m_pEsmtp = pEsmtp;
	}
	virtual void OnRecv(void *pBuf,long lSize);
	virtual void Start( const char * pContext,int nLen)
	{
		char szSend[256];
		sprintf(szSend,"QUIT\r\n",S_MAILFROM);
		Send(szSend,strlen(szSend));
		g_iTimes = -1;
	}
	virtual void Stop()
	{
	}
public:
	CQuitStatus()
	{
		m_pEsmtp = 0;
	}
};
class CEsmtp : public CESmtpStatusSwitch<IESmtpStatus>
{
protected:
	CEsmtp()
	{
		m_ConnectedStatus.Create(this);
		m_ConnectingStatus.Create(this);
		m_AuthLoginStatus.Create(this);
		m_AuthLoginUserStatus.Create(this);
		m_AuthLoginPwStatus.Create(this);
		m_MailFromStatus.Create(this);
		m_RcptToStatus.Create(this);
		m_DataStatus.Create(this);
		m_MailContentStatus.Create(this);
		m_MailEndStatus.Create(this);
		m_QuitStatus.Create(this);

		m_bSuccFlag = FALSE;
		m_strMailTo = XCRASHREPORT_SEND_TO_ADDRESS;
	}
public:
	static CEsmtp* Instance()
	{
		static CEsmtp e;
		return &e;
	}
private:
	CConnectedStatus	m_ConnectedStatus;
	CConnectingStatus	m_ConnectingStatus;
	CAuthLoginStatus	m_AuthLoginStatus;
	CAuthLoginUserStatus	m_AuthLoginUserStatus;
	CAuthLoginPwStatus	m_AuthLoginPwStatus;
	CMailFromStatus		m_MailFromStatus;
	CRcptToStatus		m_RcptToStatus;
	CDataStatus			m_DataStatus;
	CMailContentStatus	m_MailContentStatus;
	CMailEndStatus		m_MailEndStatus;
	CQuitStatus			m_QuitStatus;
public:
	void Go2ConnectedStatus()
	{
		OUTPUT2(_GT("准备错误报告内容..."));
		GoToState(&m_ConnectedStatus,0,0);
	}
	void Go2ConnectingStatus()
	{
		//OUTPUT2("Connecting!");
		GoToState(&m_ConnectingStatus,0,0);
	}
	void Go2AuthLoginStatus()
	{
		//OUTPUT2("开始验证!");
		GoToState(&m_AuthLoginStatus,0,0);
	}
	void Go2AuthLoginUserStatus()
	{
		//OUTPUT2("验证用户名!");
		GoToState(&m_AuthLoginUserStatus,0,0);
	}
	void Go2AuthLoginPwStatus()
	{
		//OUTPUT2("验证密码!");
		GoToState(&m_AuthLoginPwStatus,0,0);
	}
	void Go2MailFromStatus()
	{
		//OUTPUT2("MailFrom!");
		GoToState(&m_MailFromStatus,0,0);
	}
	void Go2RcptToStatus()
	{
		//OUTPUT2("RcptTo!");
		GoToState(&m_RcptToStatus,0,0);
	}
	void Go2DataStatus()
	{
		OUTPUT2(_GT("正在发送数据..."));
		GoToState(&m_DataStatus,0,0);
	}
	void Go2MailContentStatus()
	{
		//OUTPUT2("发送错误报告内容!");
		GoToState(&m_MailContentStatus,0,0);
	}
	void Go2MailEndStatus()
	{
		//OUTPUT2("发送错误报告完毕!开始断开连接...");
		GoToState(&m_MailEndStatus,0,0);
	}
	void Go2QuitStatus()
	{
		//OUTPUT2("连接已断开!");
// _GAMEMARKET_VER_BEGIN_
		//char strOut[128]="";
		//sprintf(strOut,"发送完毕!\r\n\r\n感谢您的支持！");
		//OUTPUT2(_GT(strOut));
		OUTPUT2(_GT("发送完毕!感谢您的支持！"));
// _GAMEMARKET_VER_END_
		//OUTPUT2(_GT("发送完毕!\r\n\r\n感谢您对远征Online的支持!"));
		TraceLn(_GT("邮件发送完毕"));
		GoToState(&m_QuitStatus,0,0);
	}
	void Restart();
	void setMailTo(const std::string& mailto)
	{
		m_strMailTo = mailto;
	}
private:
	BOOL	m_bSuccFlag;
	std::string m_strMailTo;
public:
	void SetSuccFlag(BOOL b)	{	m_bSuccFlag = b;	}
	BOOL GetSuccFlag()			{	return m_bSuccFlag;	}
	LPCSTR GetTargetMailBox()
	{
		return m_strMailTo.c_str(); // XCRASHREPORT_SEND_TO_ADDRESS;
	}
};
#endif