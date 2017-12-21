/*******************************************************************
** 文件名:	DialogPageMapinfo.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-6-21
** 版  本:	1.0
** 描  述:	
            
********************************************************************/

#include "stdafx.h"
#include "SceneServer.h"
#include "DialogPageMapinfo.h"
#include "SceneServerDlg.h"
#include "SchemeDef.h"
#include "SceneServerControl.h"


// CDialogPageMapinfo 对话框

IMPLEMENT_DYNAMIC(CDialogPageMapinfo, CDialog)
    CDialogPageMapinfo * g_pDialogPageMapinfo = NULL;
CDialogPageMapinfo::CDialogPageMapinfo(CWnd* pParent /*=NULL*/)
    : CDialog(CDialogPageMapinfo::IDD, pParent)
{
    // 上次更新tick
    m_dwLastUpdateMapInfo = 0;

    g_pDialogPageMapinfo = this;	
}

CDialogPageMapinfo::~CDialogPageMapinfo()
{
}

void CDialogPageMapinfo::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_MAPINFO, m_listctrlMapInfo);
}


BEGIN_MESSAGE_MAP(CDialogPageMapinfo, CDialog)

    ON_BN_CLICKED(IDC_BUTTON_UPDATEMAPINFO, &CDialogPageMapinfo::OnBnClickedButtonUpdatemapinfo)
END_MESSAGE_MAP()


/** 初始化对话框
@param   
@param   
@return  
*/
bool CDialogPageMapinfo::InitDialog(void)
{
LVCOLUMNA lvc;

#define INSERT_LIST_COLUMN(listctrl, col_index, col_name, col_fmt, col_width) \
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;\
    lvc.iSubItem = (col_index);\
    lvc.pszText = (LPSTR)(col_name);\
    lvc.cx = (col_width);\
    lvc.fmt = (col_fmt); \
    listctrl.InsertColumn((col_index), (LPCTSTR)lvc.pszText, lvc.fmt, lvc.cx, lvc.iSubItem);

INSERT_LIST_COLUMN(m_listctrlMapInfo, COLUMN_INDEX,         L"序号",   LVCFMT_CENTER, 40);
INSERT_LIST_COLUMN(m_listctrlMapInfo, COLUMN_MAP_NAME,      L"地图名", LVCFMT_CENTER, 200);
INSERT_LIST_COLUMN(m_listctrlMapInfo, COLUMN_MAP_ID,        L"静态ID", LVCFMT_CENTER, 80);
INSERT_LIST_COLUMN(m_listctrlMapInfo, COLUMN_SCENE_ID,      L"动态ID", LVCFMT_CENTER, 150);
INSERT_LIST_COLUMN(m_listctrlMapInfo, COLUMN_PLAYER_NUM,    L"人物数", LVCFMT_CENTER, 80);
INSERT_LIST_COLUMN(m_listctrlMapInfo, COLUMN_MONSTER_NUM,   L"怪物数", LVCFMT_CENTER, 80);
INSERT_LIST_COLUMN(m_listctrlMapInfo, COLUMN_ENTITY_NUM,    L"实体总数", LVCFMT_CENTER, 100);

    // 选中整行
    m_listctrlMapInfo.SetExtendedStyle(m_listctrlMapInfo.GetExtendedStyle()|LVS_EX_FULLROWSELECT); 

    // 使其一些按扭变灰
    BEGIN_WINDOW();
    ENABLE_WINDOW(IDC_BUTTON_UPDATEMAPINFO, false);

    return true;
}

/** 更新场景信息
@param   
@param   
@return  
*/
void CDialogPageMapinfo::UpdateSceneInfoToUI(void)
{
    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    if(pSchemeCenter == NULL)
    {
        return;
    }

    // 取得场景管理器
    ISceneManager * pSceneManager = gServerGlobal->getSceneManager();
    if(pSceneManager == NULL)
    {
        return;
    }

    // 清除
    while(m_listctrlMapInfo.DeleteItem(0));

    CListCtrl& ListCtrl = m_listctrlMapInfo;
    char szBuf[256]={0};
    int nItemIndex = 0;
    int nTotalEntityQty = 0;
    int nTotalPersonQty = 0;
    int nTotalMonsterQty = 0;

    LVITEMA lvi;
    
    // 添加最新数据
    const DWORD ARRAY_SCENE_COUNT = 512;
    SERVICE_PTR  array_scene[ARRAY_SCENE_COUNT];

    int nRow = 0;
    // 获取所有场景同步共享信息列表
    int nSceneCount = pSceneManager->getAllScenes(array_scene, ARRAY_SCENE_COUNT);
    for (int i = 0; i < nSceneCount; ++i)
    {
        SERVICE_PTR ptrSceneSvr = array_scene[i];
        if (ptrSceneSvr == SERVICE_PTR(0) || !ptrSceneSvr->is_started())
        {
            continue;
        }
        IGameSceneService * pScene = (IGameSceneService*)ptrSceneSvr->query_interface();
        if (!pScene)
        {
            continue;
        }
        SceneUpdateShareInfo & node = pScene->getUpdateShareInfo();
        if(node.nMapID == INVALID_MAPID)
        {
            continue;
        }

        lvi.pszText = szBuf;
        lvi.mask = LVIF_TEXT |  LVIF_STATE;
        lvi.iItem = nRow;
        lvi.iSubItem = 0;
        lvi.stateMask = LVIS_STATEIMAGEMASK;
        lvi.state = INDEXTOSTATEIMAGEMASK(1);

        nItemIndex = ListCtrl_InsertItem(ListCtrl,&lvi);

        //	"序号",	"地图名",	"静态地图ID",	"动态地图ID",	"人物数",	"怪物数", "实体数"
        ssprintf_s(szBuf, sizeof(szBuf), "%d", nRow++);
        ListCtrl_SetItemText(ListCtrl, nItemIndex, COLUMN_INDEX, szBuf);

        // 地图名
        SMapSchemeInfo * pMapScheme = pSchemeCenter->getSchemeMapInfo()->GetMapSchemeInfo(node.nMapID);
        ssprintf_s(szBuf, sizeof(szBuf), "%s", pMapScheme ? pMapScheme->szName : a2utf8("未知地图"));
        ListCtrl_SetItemText(ListCtrl, nItemIndex, COLUMN_MAP_NAME, szBuf);

        // 静态地图ID
        ssprintf_s(szBuf, sizeof(szBuf), "%d", node.nMapID);
        ListCtrl_SetItemText(ListCtrl, nItemIndex, COLUMN_MAP_ID, szBuf);

        // 动态地图ID
        ssprintf_s(szBuf, sizeof(szBuf), "%d", node.nSceneID);
        ListCtrl_SetItemText(ListCtrl, nItemIndex, COLUMN_SCENE_ID, szBuf);

        // 人物数
        ssprintf_s(szBuf, sizeof(szBuf), "%d", node.nPlayerQty);
        ListCtrl_SetItemText(ListCtrl, nItemIndex, COLUMN_PLAYER_NUM, szBuf);
        nTotalPersonQty += node.nPlayerQty;

        // 怪物数
        ssprintf_s(szBuf, sizeof(szBuf), "%d", node.nMonsterQty);
        ListCtrl_SetItemText(ListCtrl, nItemIndex, COLUMN_MONSTER_NUM, szBuf);
        nTotalMonsterQty += node.nMonsterQty;

        // 实体数
        ssprintf_s(szBuf, sizeof(szBuf), "%d", node.nPlayerQty+node.nMonsterQty);
        ListCtrl_SetItemText(ListCtrl, nItemIndex, COLUMN_ENTITY_NUM, szBuf);
        nTotalEntityQty += node.nPlayerQty+node.nMonsterQty;
    }


    ///////////////////////////////////////////////////////////////////
    // 打印总数
    lvi.pszText = szBuf;
    lvi.mask = LVIF_TEXT |  LVIF_STATE;
    lvi.iItem = nRow; // pServer->GetServerID();
    lvi.iSubItem = 0;
    lvi.stateMask = LVIS_STATEIMAGEMASK;
    lvi.state = INDEXTOSTATEIMAGEMASK(1);

    nItemIndex = ListCtrl_InsertItem(ListCtrl,&lvi);

    // 序号
    ssprintf_s(szBuf, sizeof(szBuf), "%d", nRow++);
    ListCtrl_SetItemText(ListCtrl, nItemIndex, COLUMN_INDEX, szBuf);

    // 地图名
    ssprintf_s(szBuf, sizeof(szBuf), a2utf8("总数统计"));
    ListCtrl_SetItemText(ListCtrl, nItemIndex, COLUMN_MAP_NAME, szBuf);

    // 静态地图ID
    ssprintf_s(szBuf, sizeof(szBuf), " ");
    ListCtrl_SetItemText(ListCtrl, nItemIndex, COLUMN_MAP_ID, szBuf);

    // 动态地图ID
    ssprintf_s(szBuf, sizeof(szBuf), " ");
    ListCtrl_SetItemText(ListCtrl, nItemIndex, COLUMN_SCENE_ID, szBuf);

    // 人物数
    ssprintf_s(szBuf, sizeof(szBuf), "%d", nTotalPersonQty);
    ListCtrl_SetItemText(ListCtrl, nItemIndex, COLUMN_PLAYER_NUM, szBuf);

    // 怪物数
    ssprintf_s(szBuf, sizeof(szBuf), "%d", nTotalMonsterQty);
    ListCtrl_SetItemText(ListCtrl, nItemIndex, COLUMN_MONSTER_NUM, szBuf);

    // 实体数
    ssprintf_s(szBuf, sizeof(szBuf), "%d", nTotalEntityQty);
    ListCtrl_SetItemText(ListCtrl, nItemIndex, COLUMN_ENTITY_NUM, szBuf);

    // 内存量	
    //m_listctrlMapInfo.SetItemText(nRow, 11, "");

    //UpdateData(FALSE);
}

void CDialogPageMapinfo::OnBnClickedButtonUpdatemapinfo()
{
    // TODO: 在此添加控件通知处理程序代码
    if(GetTickCount() - m_dwLastUpdateMapInfo <= 5000)
    {
        WarningLn("Refresh the server information for 5 seconds!");
        return;
    }
    m_dwLastUpdateMapInfo = GetTickCount();
    g_SceneServerControl.SetUIControl(UICONTROL_UPDATEMAPINFO);
}
