/*******************************************************************
** 文件名:	DialogPageSetting.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-6-21
** 版  本:	1.0
** 描  述:	
            
********************************************************************/

#include "stdafx.h"
#include "SceneServer.h"
#include "DialogPageSetting.h"
#include "afxdialogex.h"
#include "SceneServerControl.h"

#include "IServerGlobal.h"
#include "IManagerConnectorService.h"
#include "DManagerModule.h"



CDialogPageSetting * g_pDialogPageSetting = NULL;

IMPLEMENT_DYNAMIC(CDialogPageSetting, CDialog)

CDialogPageSetting::CDialogPageSetting(CWnd* pParent /*=NULL*/)
    : CDialog(CDialogPageSetting::IDD, pParent)
    , m_nSceneSvrID(0)
    , m_stringAreaName(_T(""))
    , m_stringWorldID(_T(""))
{
    g_pDialogPageSetting = this;
}

CDialogPageSetting::~CDialogPageSetting()
{

}



void CDialogPageSetting::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_SCENE_SVRID, m_nSceneSvrID);
    DDX_Text(pDX, IDC_EDIT_WORLD_NAME, m_stringAreaName);
    DDX_Text(pDX, IDC_EDIT_SCENE_WORLD_ID, m_stringWorldID);
}

BEGIN_MESSAGE_MAP(CDialogPageSetting, CDialog)

END_MESSAGE_MAP()


/** 初始化对话框
@param   
@param   
@return  
*/
bool CDialogPageSetting::InitDialog(void)
{
    CWnd * pTeampWnd = NULL;
    //ENABLE_WINDOW(IDC_CHECK_LASTSUCCSTOP, false);

    return true;
}


/** 刷新对话框
@param   
@param   
@return  
*/
bool CDialogPageSetting::RefreshDialog(int nZoneSvrID)
{
    // 场景服ID
    m_nSceneSvrID = nZoneSvrID;

    g_SceneServerControl.SetUIControl(UICONTROL_REFRESH_SETTING);

    return true;
}


/** 执行刷新配置信息
@param   
@return  
*/
void CDialogPageSetting::OnRefresh()
{

    IManagerConnectorService * pManagerConnectorService = gServerGlobal->getManagerConnectorService();
    if(pManagerConnectorService)
    {
        SGameWorldConnectorInfo info = pManagerConnectorService->GetGameWorldInfo();

        // 游戏世界ID
        m_stringWorldID.Format(_T("%d"), info.dwWorldID);

        
        // 游戏世界名
        m_stringAreaName = CString(utf82w(info.szWorldName));

        if (info.bIsPublic)
        {
            CButton *pWind = (CButton*)GetDlgItem(IDC_CHK_ISPUBLIC);
            if(pWind)
            {
                pWind->SetCheck(1);
            }
        }
    }
    
    //// 定时存盘间隔
    //m_nTickSaveInterval = pInfo->nTickSaveInterval;

    //// 关闭服务广告时长
    //m_nStopSvrNoteLong = pInfo->nCloseServiceInterval;

    //// 自定义停机公告
    //m_stringStopCustom = pInfo->szCustomStop;

    UpdateData(FALSE);
}