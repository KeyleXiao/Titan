// OperationDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GlobalVoiceServer.h"
#include "OperationDlg.h"
#include "VoiceServerSetting.h"


// COperationDlg 对话框

IMPLEMENT_DYNAMIC(COperationDlg, CDialog)

COperationDlg::COperationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COperationDlg::IDD, pParent)
	, m_strMsgTitle(_T(""))
	, m_dwDelays(0)
	, m_strTxtLabel(_T(""))
	, m_strTxt(_T(""))
	, m_nNumber1(0)
	, m_nNumber2(0)
	, m_bUseNumber1(false)
	, m_bUseNumber2(false)
	, m_strNumberTitle1(_T(""))
	, m_strNumberTitle2(_T(""))
	, m_bEnterVerifyCode(false)
	, m_nMinTxtLens(0)
	, m_nMaxTxtLens(MANAGER_TEXTSTR_MAXSIZE)
	, m_nListSelect(0)
	, m_dwVerifyCode(0)
	, m_bUseNumber3(false)
{

}

COperationDlg::~COperationDlg()
{
}

void COperationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_MSG, m_strMsgTitle);
	DDX_Control(pDX, IDC_COMBO_FLAG, m_ctrlFlagList);
	DDX_Text(pDX, IDC_EDIT_DELAYS, m_dwDelays);
	DDX_Text(pDX, IDC_STATIC_LABEL, m_strTxtLabel);
	DDX_Text(pDX, IDC_EDIT_TXT, m_strTxt);
	DDX_Control(pDX, IDC_EDIT_DELAYS, m_editDelays);
	DDX_Control(pDX, IDC_STATIC_DELAYS, m_staticDelays);
	DDX_Control(pDX, IDC_EDIT_TXT, m_editTxt);
	DDX_Text(pDX, IDC_EDIT_BAK1, m_nNumber1);
	DDX_Text(pDX, IDC_EDIT_BAK2, m_nNumber2);
}


BEGIN_MESSAGE_MAP(COperationDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_FLAG, &COperationDlg::OnSelchangeFlagListCombo)
END_MESSAGE_MAP()


// COperationDlg 消息处理程序

void COperationDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	UpdateData(true);

	char szBuf[256] = {0};

	if (m_strTxt.GetLength()<m_nMinTxtLens)
	{
		sprintf_s(szBuf,sizeof(szBuf), "请输入[%s],内容最少%d字符！",m_strTxtName.GetString(),m_nMinTxtLens);
		MessageBox((LPCTSTR)szBuf,_T("提示"),MB_ICONINFORMATION);
		GetDlgItem(IDC_EDIT_TXT)->SetFocus();
		return;
	}
	else if (m_strTxt.GetLength()>=m_nMaxTxtLens)
	{
		sprintf_s(szBuf,sizeof(szBuf), "请输入[%s],内容最多%d字符！",m_strTxtName.GetString(),m_nMaxTxtLens);
		MessageBox((LPCTSTR)szBuf,_T("提示"),MB_ICONINFORMATION);
		GetDlgItem(IDC_EDIT_TXT)->SetFocus();
		return;
	}
	
	if (m_bEnterVerifyCode)
	{
		char szVerifyCode[128] = {0};
		m_editDelays.GetWindowText((LPTSTR)szVerifyCode,sizeof(szVerifyCode));
		string strVerifyCode = szVerifyCode;
		sprintf_s(szVerifyCode,sizeof(szVerifyCode), "%d",m_dwVerifyCode);
		string strTestCode = szVerifyCode;
		//TraceLn("输入:"<<strVerifyCode.data()<<",验证码："<<strTestCode);
		if (strVerifyCode!=strTestCode)
		{
			sprintf_s(szBuf,sizeof(szBuf), "输入的验证码不正确！");
			MessageBox((LPCTSTR)szBuf,_T("提示"),MB_ICONINFORMATION);
			GetDlgItem(IDC_EDIT_DELAYS)->SetFocus();
			return;
		}
	}

	CDialog::OnOK();
}

BOOL COperationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	srand((UINT)time(NULL));

	// TODO:  在此添加额外的初始化
	if (m_strDlgTitle.GetLength()>0)
	{
		SetWindowText(m_strDlgTitle.GetString());
	}
	if (m_bEnterVerifyCode)
	{
		m_editDelays.ShowWindow(SW_SHOW);
		m_staticDelays.ShowWindow(SW_SHOW);

		ulong lRandNum = 0;
		rktRangeRand(lRandNum, 1900,2050);
		//TraceLn("lRandNum = "<<lRandNum);

		// 验证码数字
		m_dwVerifyCode = lRandNum;
		char szBuf[256] = {0};
		sprintf_s(szBuf,sizeof(szBuf), "验证码：%d",m_dwVerifyCode);
		m_staticDelays.SetWindowText((LPCTSTR)szBuf);
	}

	if (m_InfoIcon>0)
	{
		m_hIcon = AfxGetApp()->LoadIcon(m_InfoIcon);
		SetIcon(m_hIcon, TRUE);			// 设置大图标
	}

	// 使用显示备用数字1
	if (m_bUseNumber1)
	{
		GetDlgItem(IDC_EDIT_BAK1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_BAK1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_BAK1)->SetWindowText(m_strNumberTitle1);
	}
	else
	{
		GetDlgItem(IDC_EDIT_BAK1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_BAK1)->ShowWindow(SW_HIDE);
	}
	// 使用显示备用数字2
	if (m_bUseNumber2)
	{
		GetDlgItem(IDC_EDIT_BAK2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_BAK2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_BAK2)->SetWindowText(m_strNumberTitle2);
	}
	else
	{
		GetDlgItem(IDC_EDIT_BAK2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_BAK2)->ShowWindow(SW_HIDE);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void COperationDlg::OnSelchangeFlagListCombo()
{

}

