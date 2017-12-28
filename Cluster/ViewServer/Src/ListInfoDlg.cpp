// ListInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ViewServer.h"
#include "GlobalViewServer.h"
#include "ViewServerDlg.h"
#include "ListInfoDlg.h"


//Table

static _TCHAR *_gszTableColumnLabel[9] =
{
	_T("客户端ID"),_T("网关ID"),_T("IP"),	_T("端口"),	_T("网卡地址"),	_T("帐号"), _T("昵称"),_T("PDBID"),_T("连入时间")
};
//	LVCFMT_LEFT,LVCFMT_RIGHT,LVCFMT_CENTER
static int _gnTableColumnFmt[9] =
{
	LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT,LVCFMT_LEFT,LVCFMT_LEFT, LVCFMT_LEFT,LVCFMT_LEFT,LVCFMT_LEFT
};
static int _gnTableColumnWidth[9] =
{
	8,4, 10, 4, 11, 8,8,8,11
};


// CListInfoDlg 对话框

IMPLEMENT_DYNAMIC(CListInfoDlg, CResizingDialog)

CListInfoDlg::CListInfoDlg(CWnd* pParent /*=NULL*/)
	: CResizingDialog(CListInfoDlg::IDD, pParent)
{
	CDWordArray c_info;
	SetControlInfo(IDC_LIST1,	ANCHORE_LEFT | RESIZE_BOTH);
	SetControlInfo(IDOK,		ANCHORE_RIGHT);
	SetControlInfo(IDCANCEL,	ANCHORE_RIGHT);
	m_bRememberSize = FALSE;
}

CListInfoDlg::~CListInfoDlg()
{
}

void CListInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizingDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_InfoList);
}


BEGIN_MESSAGE_MAP(CListInfoDlg, CResizingDialog)
	ON_BN_CLICKED(IDC_SERVER_BUTTON, &CListInfoDlg::OnBnClickedServerButton)
	ON_BN_CLICKED(IDC_BUTTON_WORLD, &CListInfoDlg::OnBnClickedButtonWorld)
END_MESSAGE_MAP()


// CListInfoDlg 消息处理程序

void CListInfoDlg::OnBnClickedServerButton()
{
	// TODO: 在此添加控件通知处理程序代码
	//ShowListInfo(0);
}


void CListInfoDlg::OnBnClickedButtonWorld()
{
	// TODO: 在此添加控件通知处理程序代码
	//ShowListInfo(0);
}


// 初始化列表头 0:用户列表
void CListInfoDlg::InitColumns(int nListType)
{

	// insert columns
	CListCtrl& ListCtrl = m_InfoList;

	ListCtrl.ModifyStyle(LVS_TYPEMASK,LVS_REPORT & LVS_TYPEMASK);
	// 选中整行 |LVS_EX_GRIDLINES|LVS_EX_DOUBLEBUFFER
	ListCtrl.SetExtendedStyle(ListCtrl.GetExtendedStyle()|LVS_EX_FULLROWSELECT ); 

	int i=0;
	LV_COLUMN lvc;

	// 清除
	ListCtrl.DeleteAllItems();

	while(ListCtrl.DeleteColumn(0)) {}

	for(i = 0; i<9; i++)
	{
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.iSubItem = i;
		lvc.pszText = _gszTableColumnLabel[i];
		lvc.cx = _gnTableColumnWidth[i]*12;
		lvc.fmt = _gnTableColumnFmt[i];
		ListCtrl.InsertColumn(i,&lvc);
	}

}


BOOL CListInfoDlg::OnInitDialog()
{
	CResizingDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CViewServerDlg * pServerDlg =(CViewServerDlg *) AfxGetApp()->m_pMainWnd;
	if (pServerDlg!=NULL)
	{
		m_InfoList.SetImageList(&(pServerDlg->m_TreeImgList),TVSIL_NORMAL);
		m_InfoList.SetImageList(&(pServerDlg->m_TreeImgList),LVSIL_SMALL);
	}

	// 初始化列表头
	InitColumns();

	//ShowListInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
