// ListInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DataGuardServer.h"
#include "GlobalDataGuardServer.h"
#include "BridgeServerDlg.h"
#include "ListInfoDlg.h"


//Table

static const char *_gszTableColumnLabel[8] =
{
	_GT("ID.名称"),_GT("游戏世界"),_GT("服务器类型"),	_GT("第n号"),	_GT("IP"),	_GT("端口"),	_GT("连入时间"),	_GT("公共")
};
//	LVCFMT_LEFT,LVCFMT_RIGHT,LVCFMT_CENTER
static int _gnTableColumnFmt[8] =
{
	LVCFMT_LEFT, LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_CENTER,LVCFMT_LEFT,LVCFMT_CENTER, LVCFMT_CENTER,LVCFMT_CENTER
};
static int _gnTableColumnWidth[8] =
{
	10, 8, 8, 4, 9,4,11,4
};

static const char *_gszTable0ColumnLabel[4] =
{
	_GT("序号"),_GT("游戏世界"),_GT("服务器个数"),	_GT("公共")
};
//	LVCFMT_LEFT,LVCFMT_RIGHT,LVCFMT_CENTER
static int _gnTable0ColumnFmt[4] =
{
	LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_CENTER, LVCFMT_CENTER
};
static int _gnTable0ColumnWidth[4] =
{
	6,20,6,6
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
	ShowListInfo(1);
}


void CListInfoDlg::OnBnClickedButtonWorld()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowListInfo(0);
}


// 初始化列表头 0:游戏世界列表,1:软件服务器列表
void CListInfoDlg::InitColumns(int nListType)
{

	// insert columns
	CListCtrl& ListCtrl = m_InfoList;

	ListCtrl.ModifyStyle(LVS_TYPEMASK,LVS_REPORT & LVS_TYPEMASK);
	// 选中整行 |LVS_EX_GRIDLINES|LVS_EX_DOUBLEBUFFER
	ListCtrl.SetExtendedStyle(ListCtrl.GetExtendedStyle()|LVS_EX_FULLROWSELECT ); 

	int i=0;
	//LV_COLUMN lvc;
	LVCOLUMNA lvc;

	// 清除
	ListCtrl.DeleteAllItems();

	while(ListCtrl.DeleteColumn(0)) {}

	if (nListType==0)	//0:游戏世界列表
	{
		for(i = 0; i<4; i++)
		{
			lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvc.iSubItem = i;
			lvc.pszText = (LPSTR)_gszTable0ColumnLabel[i];
			lvc.cx = _gnTable0ColumnWidth[i]*12;
			lvc.fmt = _gnTable0ColumnFmt[i];
			ListCtrl_InsertColumn(ListCtrl,i,&lvc);
		}
	} 
	else if (nListType==1)	//1:软件服务器列表
	{
		for(i = 0; i<8; i++)
		{
			lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvc.iSubItem = i;
			lvc.pszText = (LPSTR)_gszTableColumnLabel[i];
			lvc.cx = _gnTableColumnWidth[i]*12;
			lvc.fmt = _gnTableColumnFmt[i];
			ListCtrl_InsertColumn(ListCtrl,i,&lvc);
		}
	}


}


BOOL CListInfoDlg::OnInitDialog()
{
	CResizingDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CDataGuardServerDlg * pServerDlg =(CDataGuardServerDlg *) AfxGetApp()->m_pMainWnd;
	if (pServerDlg!=NULL)
	{
		m_InfoList.SetImageList(&(pServerDlg->m_TreeImgList),TVSIL_NORMAL);
		m_InfoList.SetImageList(&(pServerDlg->m_TreeImgList),LVSIL_SMALL);
	}

	// 初始化列表头
	InitColumns();

	ShowListInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// 软件服务器列表  0:游戏世界列表,1:软件服务器列表
void CListInfoDlg::ShowListInfo(int nListType)
{
	CDataGuardServerDlg * pServerDlg =(CDataGuardServerDlg *) AfxGetApp()->m_pMainWnd;

	CListCtrl& ListCtrl = m_InfoList;

	GlobalDataGuardServer& mainServer = pServerDlg->m_BridgeServer;	//数据监护服务器服务

	char szBuf[256]={0};

	// 防止闪
	ListCtrl.SetRedraw(FALSE);

	InitColumns(nListType);

	int nImageID = 0;

	// insert items 

	LVITEMA lvi;	//LV_ITEM lvi;

	int nItemIndex = 0;

	int nIndex = 0;

	if (nListType==0)	//0:游戏世界列表
	{
		for(TMAP_BridgeWorldIDInfo::iterator iter = mainServer.m_mapWorldList.begin(); iter!= mainServer.m_mapWorldList.end();iter++)
		{
			nIndex++;
			sprintf_s(szBuf, sizeof(szBuf), "%d:%s",iter->second.dwWorldID,iter->second.strWorldName.c_str());
			nImageID = TREE_ICON_COMPUTER;

			lvi.pszText = szBuf;
			lvi.mask = LVIF_TEXT | LVIF_IMAGE| LVIF_STATE;
			lvi.iItem = nIndex;
			lvi.iSubItem = 0;
			lvi.iImage = nImageID;
			lvi.stateMask = LVIS_STATEIMAGEMASK;
			lvi.state = INDEXTOSTATEIMAGEMASK(1);

			nItemIndex = ListCtrl_InsertItem(ListCtrl,&lvi);	//nItemIndex = ListCtrl.InsertItem(&lvi);
			//	_T("序号"),_T("游戏世界ID"),_T("游戏世界名称"),_T("服务器个数"),	_T("公共")
			sprintf_s(szBuf, sizeof(szBuf), "%d",nIndex);
			ListCtrl_SetItemText(ListCtrl,nItemIndex,0,szBuf);
			sprintf_s(szBuf, sizeof(szBuf), "%d:%s",iter->second.dwWorldID,iter->second.strWorldName.c_str());
			ListCtrl_SetItemText(ListCtrl,nItemIndex,1,szBuf);
			sprintf_s(szBuf, sizeof(szBuf), "%d",iter->second.mapList.size());
			ListCtrl_SetItemText(ListCtrl,nItemIndex,2,szBuf);
			sprintf_s(szBuf, sizeof(szBuf), "%s",(mainServer.m_dwPublicWorldID!=iter->second.dwWorldID?"":"公共"));
			ListCtrl_SetItemText(ListCtrl,nItemIndex,3,szBuf);

			ListCtrl.SetItemData(nItemIndex,iter->second.dwWorldID);
		}
	} 
	else if (nListType==1)	//1:软件服务器列表
	{
		for(TMAP_BridgeWorldIDInfo::iterator iter = mainServer.m_mapWorldList.begin(); iter!= mainServer.m_mapWorldList.end();iter++)
		{
			for(TMAP_BridgeID2IDInfo::iterator iter1 = iter->second.mapList.begin(); iter1!= iter->second.mapList.end();iter1++)
			{
				DWORD dwServerID = iter1->second;
				ServerUser* pServer = ServerList::getInstance().GetUser(dwServerID);
				if (NULL==pServer)
				{
					continue;
				}
				nIndex++;
				sprintf_s(szBuf, sizeof(szBuf), "%d.%s%d",dwServerID,pServer->m_szName,pServer->m_wSubID);
				nImageID = TREE_ICON_SERVER;

				lvi.pszText = szBuf;
				lvi.mask = LVIF_TEXT | LVIF_IMAGE| LVIF_STATE;
				lvi.iItem = nIndex;
				lvi.iSubItem = 0;
				lvi.iImage = nImageID;
				lvi.stateMask = LVIS_STATEIMAGEMASK;
				lvi.state = INDEXTOSTATEIMAGEMASK(1);

				nItemIndex = ListCtrl_InsertItem(ListCtrl,&lvi);	//nItemIndex = ListCtrl.InsertItem(&lvi);
				//	_T("ID"),_T("游戏世界ID"),_T("软件服务器名称"),	_T("服务器类型"),	_T("第n号"),	_T("IP"),	_T("端口"),	_T("连入时间"),	_T("公共")
				ListCtrl_SetItemText(ListCtrl,nItemIndex,0,szBuf);
				sprintf_s(szBuf, sizeof(szBuf), "%d:%s",pServer->m_dwWorldID,pServer->m_szWorldName);
				ListCtrl_SetItemText(ListCtrl,nItemIndex,1,szBuf);
				sprintf_s(szBuf, sizeof(szBuf), "%s",GetServerTypeString(pServer->GetServerType()));
				ListCtrl_SetItemText(ListCtrl,nItemIndex,2,szBuf);
				sprintf_s(szBuf, sizeof(szBuf), "%d",pServer->m_wSubID);
				ListCtrl_SetItemText(ListCtrl,nItemIndex,3,szBuf);
				sprintf_s(szBuf, sizeof(szBuf), "%s",pServer->GetRemoteIP());
				ListCtrl_SetItemText(ListCtrl,nItemIndex,4,szBuf);
				sprintf_s(szBuf, sizeof(szBuf), "%d",pServer->GetRemotePort());
				ListCtrl_SetItemText(ListCtrl,nItemIndex,5,szBuf);
				sprintf_s(szBuf, sizeof(szBuf), "%s",GetTimeString(pServer->m_dwAddTime));
				ListCtrl_SetItemText(ListCtrl,nItemIndex,6,szBuf);
				sprintf_s(szBuf, sizeof(szBuf), "%s",(pServer->m_bIsPublic?"公共":""));
				ListCtrl_SetItemText(ListCtrl,nItemIndex,7,szBuf);

				ListCtrl.SetItemData(nItemIndex,dwServerID);
			}
		}
	}


	// 防止闪
	ListCtrl.SetRedraw(TRUE);
	ListCtrl.Invalidate();
	ListCtrl.UpdateWindow();

}