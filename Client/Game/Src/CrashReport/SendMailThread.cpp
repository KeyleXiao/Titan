#include "stdafx.h"
#include "SendMailThread.h"
#include <winsock.h>
#include <time.h>
#include <sys/timeb.h>
#include "Resource.h"

SOCKET Cs = INVALID_SOCKET;
struct sockaddr_in SA;
SendMailStruct *g_pMail = 0;
HWND	g_hResult = 0;
HWND	g_hDlg = 0;

AUTHLOGIN g_AuthLogin[] = 
{
	//AUTHLOGIN("bccr001@21cn.com", "107628", "bccr001@21cn.com", "smtp.21cn.com", 25)
	AUTHLOGIN("report@szgla.cn", "Report.com", "report@szgla.cn", "mail.szgla.cn", 25)
};

static void split(std::vector<std::string>& strList, const char* source, size_t len, const char sep = ',')
{
	if (!source || *source == 0)
		return;

	const char* s = source;
	const char* e = source;
	const char* end = source + len;
	while (e < end)
	{
		if (*e == sep) // 发现分隔符
		{
			std::string str;
			str.assign(s, e - s);
			strList.push_back(str);
			e++;
			s = e;
		}
		else
			e++;
	}

	// 尾部有
	if (e > s)
	{
		std::string str;
		str.assign(s, e - s);
		strList.push_back(str);
	}
}

int g_iTimes = -1;

void OUTPUT2(const char *lpszFormat,...)
{
	va_list args;
	va_start(args, lpszFormat);
	
	int nBuf;
	char szBuffer[32768] = {0};
	
	nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);

	static TCHAR szText[32768];
	GetWindowText(g_hResult,szText,_countof(szText));
	_tcscat(szText,my_utf82t(szBuffer));
	_tcscat(szText,_T("\r\n"));
	SetWindowText(g_hResult,szText);

	va_end(args);
}

#define _TEST_

void OUTPUT(const char* lpszFormat, ...)
{
#ifndef _TEST_
	return;
#endif
	va_list args;
	va_start(args, lpszFormat);
	
	int nBuf;
	char szBuffer[32768] = {0};
	
	nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);

	//OutputDebugString(szBuffer);

/*	static char szText[32768];
	GetWindowText(g_hResult,szText,32768);
	strcat(szText,szBuffer);
	SetWindowText(g_hResult,szText);
*/
	va_end(args);
}
void CloseSM(HWND hWnd)
{
	WSAAsyncSelect(Cs, hWnd, 0, 0);
	OUTPUT("WSAAsyncSelect ->0,0!\n");

	closesocket(Cs);
	OUTPUT("socket close!\n");

	WSACleanup( );
	OUTPUT("WSACleanup OK!\n");
}
BOOL InitSM(HWND hWnd)
{
	CloseSM(hWnd);

	WSADATA ws;
	if(WSAStartup(0x0101,&ws) != 0)
	{
		OUTPUT("WSAStartup Failed! Error Code = %d",WSAGetLastError());
		return FALSE;
	}
	OUTPUT("WSAStartup OK!\n");
	
	Cs = socket(PF_INET, SOCK_STREAM, 0);
	if(Cs == INVALID_SOCKET)
	{
		OUTPUT("socket Failed! Error Code = %d",WSAGetLastError());
		WSACleanup();
		return FALSE;
	}
	OUTPUT("socket OK!\n");

	hostent *hp = gethostbyname(S_ADDR);
	if(!hp)
	{
		OUTPUT("socket Failed! Error Code = %d",WSAGetLastError());
		closesocket(Cs);
		WSACleanup();
		return FALSE;
	}
	unsigned char ip[4];
	memcpy(ip,(char*)hp->h_addr,hp->h_length);
	OUTPUT("gethostbyname %s   IP:%d.%d.%d.%d\n",S_ADDR,ip[0],ip[1],ip[2],ip[3]);
	memcpy((char*)&SA.sin_addr,(char*)hp->h_addr,hp->h_length);
	SA.sin_family = AF_INET;
	SA.sin_port = htons(S_PORT);//服务器端口。

	int err = connect(Cs,(struct sockaddr *)&SA,sizeof(SA));
	if(err)
	{
		OUTPUT("connect Failed! Error Code = %d",WSAGetLastError());
		closesocket(Cs);
		WSACleanup();
		return FALSE;
	}
	OUTPUT("connect OK!\n");

	if(WSAAsyncSelect(Cs,hWnd,WM_USER+1,FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE) == SOCKET_ERROR)
	{
		OUTPUT("WSAAsyncSelect Failed! Error Code = %d",WSAGetLastError());
		closesocket(Cs);
		WSACleanup();
		return FALSE;
	}
	OUTPUT("WSAAsyncSelect OK!\n");

	return TRUE;
}
long _stdcall DlgProc(HWND hW,UINT m,WPARAM wP,LPARAM lP)
{
	switch (m)
	{
	case WM_INITDIALOG:
		{
			g_hDlg = hW;
			SendMailStruct* sms = (SendMailStruct*)lP;
			if (sms->hideProgress) // hide
				MoveWindow(hW,0,0,0,0,TRUE);

			//ShowWindow(hW,SW_HIDE);
			if (!sms->strMailTo.empty())
			{
				CEsmtp::Instance()->setMailTo(sms->strMailTo);
			}

			if (!sms->strAuthLogin.empty())
			{
				std::vector<std::string> infos;
				split(infos, sms->strAuthLogin.c_str(), sms->strAuthLogin.length());
				g_AuthLogin->Init(infos);
			}

			CEsmtp::Instance()->Restart();
			break;
		}
	case WM_COMMAND:
		switch(wP)
		{
		case IDCANCEL:
			//EndDialog(hW,0);
			break;
		}
		break;
	case WM_USER+1:
		switch(LOWORD(lP))
		{
		case FD_CONNECT:
			OUTPUT("FD_CONNECT->Ok!\n");
			break;
		case FD_READ:
			{
				char szRecv[256];
				int iSize = recv(wP,szRecv,sizeof(szRecv),0);//wP=Cs
				szRecv[iSize]=0;
				OUTPUT("FD_READ->:%s\n",szRecv);

				CEsmtp::Instance()->GetCurrentState()->OnRecv(szRecv,iSize);
			}
			break;
		case FD_CLOSE:
			OUTPUT("FD_CLOSE->Disconnected!\n");
			if(!CEsmtp::Instance()->GetSuccFlag())
			{
				CEsmtp::Instance()->Restart();
				break;
			}
			CloseSM(hW);
			EndDialog(hW,0);
			break;
		case FD_WRITE:
			{
				OUTPUT("\r\n\r\nREADY FOR WRITE\r\n\r\n\r\n");
			}
			break;
		}
		break;
	}

	return 0;
}
void SendMailThread(void *p)
{
	g_pMail = (SendMailStruct*)p;
	if(!g_pMail)return;

	DialogBoxParam(GetModuleHandle(0),(LPCTSTR)IDD_SM,g_pMail->hParent,(DLGPROC)DlgProc, (LPARAM)g_pMail);
	return;
}
#define SEND													\
{																\
	if(send(Cs,szSend,strlen(szSend),0)==SOCKET_ERROR)			\
	{															\
		OUTPUT2("Send %s Failed!\n",szSend);					\
		m_pEsmtp->Restart();									\
		return;													\
	}															\
	OUTPUT("Send %s OK!\n",szSend);								\
}
#define ONRECV(pBuf,iSize,cv)	{			\
	if(lSize < 3)							\
	{										\
		OUTPUT("Result Must >= 3\n");		\
		m_pEsmtp->Restart();				\
		return;								\
	}										\
	char szBuf[4];							\
	memcpy(szBuf,pBuf,3);					\
	szBuf[3] = 0;							\
	if(strcmp(szBuf,cv)!=0)					\
	{										\
		OUTPUT2("Error  --->Result is not %s!\n",cv);	\
		m_pEsmtp->Restart();							\
		return;								\
	}										\
	OUTPUT("Result is %s --->OK!\n",cv);	\
}
void IESmtpStatus::Send(char *szSend,int iSize)
{
	if(send(Cs,szSend,iSize,0)==SOCKET_ERROR)
	{
		OUTPUT("Send: %s Failed!\n",szSend);
		m_pEsmtp->Restart();
		return;
	}
	
	OUTPUT("Send: %s OK!\n",szSend);
}
void CConnectingStatus::OnRecv(void *pBuf,long lSize)
{
	ONRECV(pBuf,lSize,"220");

	OUTPUT("Connected!\n");
	m_pEsmtp->Go2ConnectedStatus();
}
void CConnectedStatus::OnRecv(void *pBuf,long lSize)
{
	ONRECV(pBuf,lSize,"250");

	OUTPUT("EHLO OK!\n");
	m_pEsmtp->Go2AuthLoginStatus();
}
void CAuthLoginStatus::OnRecv(void *pBuf,long lSize)
{
	ONRECV(pBuf,lSize,"334");

	OUTPUT("AUTH LOGIN OK!\n");
	m_pEsmtp->Go2AuthLoginUserStatus();
}
void CAuthLoginUserStatus::OnRecv(void *pBuf,long lSize)
{
	ONRECV(pBuf,lSize,"334");

	OUTPUT("USER NAME OK!\n");
	m_pEsmtp->Go2AuthLoginPwStatus();
}
void CAuthLoginPwStatus::OnRecv(void *pBuf,long lSize)
{
	ONRECV(pBuf,lSize,"235");

	OUTPUT("USER PW OK!\n");
	m_pEsmtp->Go2MailFromStatus();
}
void CMailFromStatus::OnRecv(void *pBuf,long lSize)
{
	ONRECV(pBuf,lSize,"250");

	OUTPUT("MAIL FROM OK!\n");
	m_pEsmtp->Go2RcptToStatus();
}
void CRcptToStatus::Start( const char * pContext,int nLen)
{
	char szSend[256];
	sprintf(szSend,"RCPT TO: %s\r\n",m_pEsmtp->GetTargetMailBox());
	//MessageBox(0,0,szSend,0);
	Send(szSend,strlen(szSend));
}
void CRcptToStatus::OnRecv(void *pBuf,long lSize)
{
	ONRECV(pBuf,lSize,"250");

	OUTPUT("RCPT TO OK!\n");
	m_pEsmtp->Go2DataStatus();
}
void CDataStatus::OnRecv(void *pBuf,long lSize)
{
	ONRECV(pBuf,lSize,"354");

	OUTPUT("DATA OK!\n");
	m_pEsmtp->Go2MailContentStatus();
}

#if 0
#define EPOCH 1970

static time_t libesmtp_mktime (struct tm *tm)
{
	time_t when;
	int day, year;
	static const int days[] =
	{
		0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334,
	};

	year = tm->tm_year + 1900;
	day = days[tm->tm_mon] + tm->tm_mday - 1;

	/* adjust for leap years paying attention to January and February */
	day += (year - (EPOCH - (EPOCH % 4))) / 4;
	day -= (year - (EPOCH - (EPOCH % 100))) / 100;
	day += (year - (EPOCH - (EPOCH % 400))) / 400;
	if (tm->tm_mon < 2 && (year % 4 == 0) && (year % 100 != 0 || year / 400 == 0))
		day -= 1;

	when = ((year - EPOCH) * 365 + day) * 24 + tm->tm_hour;
	when = (when * 60 + tm->tm_min) * 60 + tm->tm_sec;
	return when;
}


static const char *days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", };
static const char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", };

char * rfc2822date (char buf[], size_t buflen, time_t *timedate)
{
	struct tm *tm;
	int dir, minutes;
	time_t gmtoff;

	tm = localtime (timedate);

	gmtoff = libesmtp_mktime (tm);
	gmtoff -= *timedate;
	minutes = gmtoff / 60;

	dir = (minutes > 0) ? '+' : '-';
	if (minutes < 0)
		minutes = -minutes;
	_snprintf (buf, buflen, "%s, %d %s %d %02d:%02d:%02d %c%02d%02d",
		days[tm->tm_wday],
		tm->tm_mday, months[tm->tm_mon], tm->tm_year + 1900,
		tm->tm_hour, tm->tm_min, tm->tm_sec,
		dir, minutes / 60, minutes % 60);
	return buf;
}
#endif

void CMailContentStatus::Start( const char * pContext,int nLen)
{
	// 设为阻塞模式
	WSAAsyncSelect(Cs, g_hDlg, 0, 0);

	unsigned long param = 0;
	// 改为阻塞模式
	if(ioctlsocket(Cs,FIONBIO,&param) == SOCKET_ERROR)
	{
		CloseSM(g_hDlg);
		EndDialog(g_hDlg,0);
		return;
	}


	static char szSend[32768];

	bool hasAttachment = !g_pMail->szAttachmentFileName.empty();

	time_t tTime;
	struct tm* ptm;
	char szTime[100];
	struct timeb tbTime;
	tTime=time(NULL);

/* timeb
time是从UTC时间1970年1月1日午夜(00:00:00)起累计的秒数； 
millitm是一秒内的毫秒数 
dstflag不为0，说明这是夏令时时间 
timezone是UTC时间和本地时间的相差分钟数,大于0表示西方时间，小于0为东方时间
*/

	ptm=localtime(&tTime);
	//strftime( szTime,sizeof(szTime), "%a, %d %b %Y %H:%M:%S %Z", ptm );
	strftime( szTime,sizeof(szTime), "%a, %d %b %Y %H:%M:%S", ptm );
	ftime(&tbTime);
	if(tbTime.dstflag)tbTime.timezone-=60;
	int dir = tbTime.timezone > 0 ? '-' : '+';
	size_t len = strlen(szTime);
	sprintf_s( szTime + len, sizeof(szTime) - len, " %c%2.2d%2.2d",dir, -tbTime.timezone / 60, tbTime.timezone % 60 );


	//rfc2822date(szTime, 100, &tTime);
	//OutputDebugString(szTime);

	char szBoundary[]="----=_NextPart_000_0013_01C4EADA.58FB2280";

	sprintf(szSend,"Date: %s\r\n",szTime);
	sprintf(szSend,"%sFrom: %s<%s>\r\n",szSend,XCRASHREPORT_SEND_TO_NAME, S_MAILFROM);
	sprintf(szSend,"%sTo: %s\r\n",szSend, m_pEsmtp->GetTargetMailBox());
	sprintf(szSend,"%sSubject: %s\r\n",szSend,g_pMail->strSubject.c_str());
	//sprintf(szSend,"%sX-AIMailer: AIMC 2.9.5.1 2001.05.18\r\n",szSend);
	//sprintf(szSend,"%sX-AIMime: MIME/SMIME Lib 2.9 2.9 2001.05.18\r\n",szSend);
	//X-Mailer: <>必须要有<>号，否则有的smtp服务器不行
	sprintf(szSend,"%sMIME-Version: 1.0\r\n",szSend);
	if (hasAttachment)
		sprintf(szSend,"%sContent-Type:multipart/mixed;\r\n",szSend);	//mixed而不是Alternative
	else
		sprintf(szSend,"%sContent-Type:multipart/alternative;\r\n",szSend);	//mixed而不是Alternative
	sprintf(szSend,"%s\tboundary=\"%s\"\r\n",szSend,szBoundary);
	sprintf(szSend,"%sX-Mailer: <gx^_^ mail system>\r\n",szSend);
	SEND;
	//OutputDebugString(szSend);

	sprintf(szSend,"\r\nThis is a multi-part message in MIME format.\r\n\r\n");
	SEND;
	//OutputDebugString(szSend);

	//发错误报告邮件内容
	sprintf(szSend,"--%s\r\n",szBoundary);
	sprintf(szSend,"%sContent-Type:text/html;\r\n",szSend);
	sprintf(szSend,"%s\tcharset=\"gb2312\"\r\n",szSend);
	sprintf(szSend,"%sContent-Transfer-Encoding: 8bit\r\n",szSend);
	sprintf(szSend,"%s\r\n\r\n%s\r\n\r\n\r\n",szSend,g_pMail->strMessage.c_str());	//邮件内容
	SEND;
	//OutputDebugString(szSend);

	if (hasAttachment)
	{
		//发附件头
		char szFileName[256] = {0};
		char *pTemp = (char*)g_pMail->szAttachmentFileName.c_str() + g_pMail->szAttachmentFileName.size();
		while(*pTemp != '\\')pTemp--;
		strcpy(szFileName,pTemp + 1);
		sprintf(szSend,"--%s\r\n",szBoundary);
		sprintf(szSend,"%sContent-Type:text/plain;\r\n",szSend);
		sprintf(szSend,"%s\tname=\"%s\"\r\n",szSend,szFileName);
		sprintf(szSend,"%sContent-Transfer-Encoding: base64\r\n",szSend);
		sprintf(szSend,"%sContent-Disposition: attachment;\r\n",szSend);
		sprintf(szSend,"%s\tfilename=\"%s\"\r\n",szSend,szFileName);
		SEND;
		//OutputDebugString(szSend);
		
		sprintf(szSend,"\r\n");
		SEND;
		//OutputDebugString(szSend);

		OUTPUT("Crash File Name = %s\r\n",g_pMail->szAttachmentFileName.c_str());

		if(1)
		{
			//发附件内容
			HANDLE hFile=CreateFile(my_utf82t(g_pMail->szAttachmentFileName.c_str()),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if(hFile != INVALID_HANDLE_VALUE)
			{
				DWORD dwSizeLow=GetFileSize(hFile,0);
				//base64解码过程中，解码后的大小总是原先的4/3倍，所以尽量让解码大小为3的倍数
				char *pOrigin = new char[dwSizeLow];
				ReadFile(hFile,pOrigin,dwSizeLow,&dwSizeLow,0);
				size_t bufSize = dwSizeLow / 3 * 5;
				char *pEncode = new char[bufSize];
				//在下面的函数里，第二个参数不能是strlen(szTemp)，因为文件里可能有\0
				base64encode((unsigned char*)pOrigin,dwSizeLow,(unsigned char*)pEncode,dwSizeLow / 3 * 5);
				//multi part结束符号
				sprintf_s(pEncode,bufSize,"%s\r\n--%s--\r\n",pEncode,szBoundary);
				//邮件结束符号----放一起发送
				sprintf_s(pEncode,bufSize,"%s\r\n.\r\n",pEncode,szBoundary);
				int iLen = strlen(pEncode);
				int iRet = send(Cs,pEncode,iLen,0);
				delete[] pOrigin;
				delete[] pEncode;

				if(iRet == SOCKET_ERROR)
				{
					OUTPUT("\r\nstrlen(szSend) = %d FAILED!\r\n",iLen);
					m_pEsmtp->Restart();
					return;
				}
				else
				{
					OUTPUT("\r\nstrlen(szSend) = %d OK!\r\n",iLen);
				}
				CloseHandle(hFile);
			}
			else
			{
				OUTPUT(_GT("找不到附件%s"),g_pMail->szAttachmentFileName.c_str());
			}//*/
		}
	}
	else
	{
		//multi part结束符号
		sprintf(szSend,"\r\n--%s--\r\n",szBoundary);
		//邮件结束符号----放一起发送
		sprintf(szSend,"%s\r\n.\r\n",szSend,szBoundary);
		SEND;
		OutputDebugStringA(szSend);
	}
	//Sleep(10000);
	
	//发送结束符
	//sprintf(szSend,"\r\n--%s--\r\n",szBoundary);
	//SEND;
	// 改回非阻塞模式
	WSAAsyncSelect(Cs,g_hDlg,WM_USER+1,FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE);
	
	m_pEsmtp->Go2MailEndStatus();
}
void CMailContentStatus::OnRecv(void *pBuf,long lSize)
{
	OUTPUT("Error --->Should Never Arrive Here!\n");
}
void CMailEndStatus::OnRecv(void *pBuf,long lSize)
{
	ONRECV(pBuf,lSize,"250");

	OUTPUT("MAIL END OK!\n");
	m_pEsmtp->Go2QuitStatus();
}
void CQuitStatus::OnRecv(void *pBuf,long lSize)
{
	ONRECV(pBuf,lSize,"221");

	OUTPUT("QUIT OK!\n");

	TCHAR szText[256];
	GetWindowText(g_hResult,szText,_countof(szText));
	//strcat(szText,"发送完毕\r\n");
	SetWindowText(g_hResult,szText);

	m_pEsmtp->SetSuccFlag(TRUE);
}
void CEsmtp::Restart()
{
	if(++g_iTimes >= (sizeof(g_AuthLogin) / sizeof(g_AuthLogin[0])))
	{
		OUTPUT2(_GT("发送错误报告失败"));
		CloseSM(g_hDlg);
		EndDialog(g_hDlg,0);
		return;
	}
	OUTPUT("\r\nRESTART\r\n");
	if(!InitSM(g_hDlg))
	{
		EndDialog(g_hDlg,0);
		return;
	}
	g_hResult = GetDlgItem(g_hDlg,IDE_DESC);
	SetWindowText(g_hResult,my_utf82t(_GT("准备发送错误报告...\r\n正在连接服务器...请稍等!\r\n")));
	CEsmtp::Instance()->Go2ConnectingStatus();
}
