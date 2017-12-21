// ListInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "VoiceServer.h"
#include "GlobalVoiceServer.h"
#include "VoiceServerDlg.h"
#include "ListInfoDlg.h"
#include "GatewayList.h"


//Table

static _TCHAR *_gszTableColumnLabel[8] =
{
	_T("ID.名称"),_T("游戏世界"),_T("服务器类型"),	_T("第n号"),	_T("IP"),	_T("端口"),	_T("连入时间"),	_T("公共")
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

static _TCHAR *_gszTable0ColumnLabel[4] =
{
	_T("序号"),_T("游戏世界"),_T("服务器个数"),	_T("公共")
};
//	LVCFMT_LEFT,LVCFMT_RIGHT,LVCFMT_CENTER
static int _gnTable0ColumnFmt[4] =
{
	LVCFMT_LEFT,LVCFMT_LEFT, LVCFMT_CENTER, LVCFMT_CENTER
};
static int _gnTable0ColumnWidth[4] =
{
	6,20,6,6
};


static _TCHAR *_gszTable1ColumnLabel[6] =
{
	_T("ID"),_T("网关ID"),_T("最大人数"),_T("IP"),	_T("端口"),	_T("连入时间")
};
//	LVCFMT_LEFT,LVCFMT_RIGHT,LVCFMT_CENTER
static int _gnTable1ColumnFmt[6] =
{
	LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT,LVCFMT_LEFT,LVCFMT_LEFT
};
static int _gnTable1ColumnWidth[6] =
{
	4,6, 8, 11, 4,11
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
	ON_BN_CLICKED(IDC_SERVER_BUTTON2, &CListInfoDlg::OnBnClickedServerButton2)
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

void CListInfoDlg::OnBnClickedServerButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowListInfo(2);
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
	LV_COLUMN lvc;

	// 清除
	ListCtrl.DeleteAllItems();

	while(ListCtrl.DeleteColumn(0)) {}

	if (nListType==0)	//0:游戏世界列表
	{
		for(i = 0; i<4; i++)
		{
			lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvc.iSubItem = i;
			lvc.pszText = _gszTable0ColumnLabel[i];
			lvc.cx = _gnTable0ColumnWidth[i]*12;
			lvc.fmt = _gnTable0ColumnFmt[i];
			ListCtrl.InsertColumn(i,&lvc);
		}
	} 
	else if (nListType==1)	//1:软件服务器列表
	{
		for(i = 0; i<8; i++)
		{
			lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvc.iSubItem = i;
			lvc.pszText = _gszTableColumnLabel[i];
			lvc.cx = _gnTableColumnWidth[i]*12;
			lvc.fmt = _gnTableColumnFmt[i];
			ListCtrl.InsertColumn(i,&lvc);
		}
	}
	else if (nListType==2)	//2:网关服列表
	{
		for(i = 0; i<6; i++)
		{
			lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvc.iSubItem = i;
			lvc.pszText = _gszTable1ColumnLabel[i];
			lvc.cx = _gnTable1ColumnWidth[i]*12;
			lvc.fmt = _gnTable1ColumnFmt[i];
			ListCtrl.InsertColumn(i,&lvc);
		}
	}


}


BOOL CListInfoDlg::OnInitDialog()
{
	CResizingDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CVoiceServerDlg * pServerDlg =(CVoiceServerDlg *) AfxGetApp()->m_pMainWnd;
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
	CVoiceServerDlg * pServerDlg =(CVoiceServerDlg *) AfxGetApp()->m_pMainWnd;

	CListCtrl& ListCtrl = m_InfoList;

	GlobalVoiceServer& mainServer = pServerDlg->m_VoiceServer;	//语音服务器服务

	char szBuf[256]={0};

	// 防止闪
	ListCtrl.SetRedraw(FALSE);

	InitColumns(nListType);

	int nImageID = 0;

	// insert items 

	LVITEMA lvi;

	int nItemIndex = 0;

	int nIndex = 0;

	if (nListType==0)	//0:游戏世界列表
	{
		for(TMAP_VoiceWorldIDInfo::iterator iter = mainServer.m_mapWorldList.begin(); iter!= mainServer.m_mapWorldList.end();++iter)
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

			//nItemIndex = ListCtrl.InsertItem(&lvi);
            nItemIndex = ListCtrl_InsertItem(ListCtrl,&lvi);
			//	_T("序号"),_T("游戏世界ID"),_T("游戏世界名称"),_T("服务器个数"),	_T("公共")
			sprintf_s(szBuf, sizeof(szBuf), "%d",nIndex);
			//ListCtrl.SetItemText(nItemIndex,0,szBuf);
            ListCtrl_SetItemText(ListCtrl, nItemIndex,0,szBuf);
			sprintf_s(szBuf, sizeof(szBuf), "%d:%s",iter->second.dwWorldID,iter->second.strWorldName.c_str());
			//ListCtrl.SetItemText(nItemIndex,1,szBuf);
            ListCtrl_SetItemText(ListCtrl, nItemIndex,1,szBuf);
			sprintf_s(szBuf, sizeof(szBuf), "%d",iter->second.mapList.size());
			//ListCtrl.SetItemText(nItemIndex,2,szBuf);
            ListCtrl_SetItemText(ListCtrl, nItemIndex,2,szBuf);
			sprintf_s(szBuf, sizeof(szBuf), "%s",(mainServer.m_dwPublicWorldID!=iter->second.dwWorldID?"":"公共"));
			//ListCtrl.SetItemText(nItemIndex,3,szBuf);
            ListCtrl_SetItemText(ListCtrl, nItemIndex,3,szBuf);

			ListCtrl.SetItemData(nItemIndex,iter->second.dwWorldID);
		}
	} 
	else if (nListType==1)	//1:软件服务器列表
	{
		for(TMAP_VoiceWorldIDInfo::iterator iter = mainServer.m_mapWorldList.begin(); iter!= mainServer.m_mapWorldList.end();++iter)
		{
			for(TMAP_VoiceID2IDInfo::iterator iter1 = iter->second.mapList.begin(); iter1!= iter->second.mapList.end();++iter1)
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

				//nItemIndex = ListCtrl.InsertItem(&lvi);
                nItemIndex = ListCtrl_InsertItem(ListCtrl,&lvi);
				//	_T("ID"),_T("游戏世界ID"),_T("软件服务器名称"),	_T("服务器类型"),	_T("第n号"),	_T("IP"),	_T("端口"),	_T("连入时间"),	_T("公共")
				//ListCtrl.SetItemText(nItemIndex,0,szBuf);
                ListCtrl_SetItemText(ListCtrl, nItemIndex,0,szBuf);
				sprintf_s(szBuf, sizeof(szBuf), "%d:%s",pServer->m_dwWorldID,pServer->m_szWorldName);
				//ListCtrl.SetItemText(nItemIndex,1,szBuf);
                ListCtrl_SetItemText(ListCtrl, nItemIndex,1,szBuf);
				sprintf_s(szBuf, sizeof(szBuf), "%s",GetServerTypeString(pServer->GetServerType()));
				//ListCtrl.SetItemText(nItemIndex,2,szBuf);
                ListCtrl_SetItemText(ListCtrl, nItemIndex,2,szBuf);
				sprintf_s(szBuf, sizeof(szBuf), "%d",pServer->m_wSubID);
				//ListCtrl.SetItemText(nItemIndex,3,szBuf);
                ListCtrl_SetItemText(ListCtrl, nItemIndex,3,szBuf);
				sprintf_s(szBuf, sizeof(szBuf), "%s",pServer->GetRemoteIP());
				//ListCtrl.SetItemText(nItemIndex,4,szBuf);
                ListCtrl_SetItemText(ListCtrl, nItemIndex,4,szBuf);
				sprintf_s(szBuf, sizeof(szBuf), "%d",pServer->GetRemotePort());
				//ListCtrl.SetItemText(nItemIndex,5,szBuf);
                ListCtrl_SetItemText(ListCtrl, nItemIndex,5,szBuf);
				sprintf_s(szBuf, sizeof(szBuf), "%s",GetTimeString(pServer->m_dwAddTime));
				//ListCtrl.SetItemText(nItemIndex,6,szBuf);
                ListCtrl_SetItemText(ListCtrl, nItemIndex,6,szBuf);
				sprintf_s(szBuf, sizeof(szBuf), "%s",(pServer->m_bIsPublic?"公共":""));
				//ListCtrl.SetItemText(nItemIndex,7,szBuf);
                ListCtrl_SetItemText(ListCtrl, nItemIndex,7,szBuf);

				ListCtrl.SetItemData(nItemIndex,dwServerID);
			}
		}
	}
	else if (nListType==2)	//2:网关服列表
	{
		for(DWORD i=0;i<VOICE_MAX_GATEWAYID;i++)
		{
			GatewayUser * pServer = GatewayList::getInstance().GetUserByGatewayID(i);
			if (NULL==pServer)
			{
				continue;
			}
			nIndex++;
			sprintf_s(szBuf, sizeof(szBuf), "%d",pServer->GetServerID());
			nImageID = TREE_ICON_COMPUTER;

			lvi.pszText = szBuf;
			lvi.mask = LVIF_TEXT | LVIF_IMAGE| LVIF_STATE;
			lvi.iItem = nIndex;
			lvi.iSubItem = 0;
			lvi.iImage = nImageID;
			lvi.stateMask = LVIS_STATEIMAGEMASK;
			lvi.state = INDEXTOSTATEIMAGEMASK(1);

			//nItemIndex = ListCtrl.InsertItem(&lvi);
            nItemIndex = ListCtrl_InsertItem(ListCtrl,&lvi);
			//	_T("ID"),_T("网关ID"),_T("最大人数"),_T("IP"),	_T("端口"),	_T("连入时间")
			//ListCtrl.SetItemText(nItemIndex,0,szBuf);
            ListCtrl_SetItemText(ListCtrl, nItemIndex,0,szBuf);
			sprintf_s(szBuf, sizeof(szBuf), "%d",pServer->GetGatewayID());
			//ListCtrl.SetItemText(nItemIndex,1,szBuf);
            ListCtrl_SetItemText(ListCtrl, nItemIndex,1,szBuf);
			sprintf_s(szBuf, sizeof(szBuf), "%d",pServer->m_dwMaxCounts);
			//ListCtrl.SetItemText(nItemIndex,2,szBuf);
            ListCtrl_SetItemText(ListCtrl, nItemIndex,2,szBuf);
			sprintf_s(szBuf, sizeof(szBuf), "%s",pServer->GetRemoteIP());
			//ListCtrl.SetItemText(nItemIndex,3,szBuf);
            ListCtrl_SetItemText(ListCtrl, nItemIndex,3,szBuf);
			sprintf_s(szBuf, sizeof(szBuf), "%d",pServer->GetRemotePort());
			//ListCtrl.SetItemText(nItemIndex,4,szBuf);
            ListCtrl_SetItemText(ListCtrl, nItemIndex,4,szBuf);
			sprintf_s(szBuf, sizeof(szBuf), "%s",GetTimeString(pServer->m_dwAddTime));
			//ListCtrl.SetItemText(nItemIndex,5,szBuf);
            ListCtrl_SetItemText(ListCtrl, nItemIndex,5,szBuf);

			ListCtrl.SetItemData(nItemIndex,i);
		}
	}


	// 防止闪
	ListCtrl.SetRedraw(TRUE);
	ListCtrl.Invalidate();
	ListCtrl.UpdateWindow();

}